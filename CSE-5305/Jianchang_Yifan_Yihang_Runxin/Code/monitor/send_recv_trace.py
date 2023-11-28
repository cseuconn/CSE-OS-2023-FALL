import asyncio
import threading
import socket
from bcc import BPF
import ctypes
import docker
import time

# auth: 6ms
# video: 900ms

# ddl_pid = {
#     428593: 6,
#     431135: 100,
#     431056: 700,
# }

def get_container_entry_pid(container_id):
    client = docker.from_env()
    try:
        container = client.containers.get(container_id)
        return container.attrs['State']['Pid']
    except docker.errors.NotFound:
        print(f"Container {container_id} not found.")
    except Exception as e:
        print(f"An error occurred: {e}")
    return None


def get_all_container_ids():
    client = docker.from_env()
    container_ids = [
        container.id for container in client.containers.list(all=False)]
    return container_ids


# def refresh_container_pids(interval=10):
#     global container_pids
#     while True:
#         try:
#             new_pids = [get_container_entry_pid(
#                 cid) for cid in get_all_container_ids()]
#             with threading.Lock():
#                 container_pids = new_pids
#             time.sleep(interval)
#         except Exception as e:
#             print(f"Error refreshing container PIDs: {e}")


fd_state = {}
# pair of (pid, fd) that have been deregistered, max size = 1000
deregistered_pair = set()


def update_fd_state(pid, fd, event_type):
    if pid not in fd_state:
        fd_state[pid] = {}
    if fd not in fd_state[pid]:
        fd_state[pid][fd] = {'recv_seen': False,
                             'registered': False, 'send_after_recv': False}

    if event_type == 'recv' and not fd_state[pid][fd]['registered']:
        fd_state[pid][fd]['recv_seen'] = True
    elif event_type == 'send' and fd_state[pid][fd]['recv_seen']:
        fd_state[pid][fd]['send_after_recv'] = True


async def send_message(message):
    try:
        reader, writer = await asyncio.open_connection(
            '127.0.0.1', 1234, family=socket.AF_INET, proto=socket.IPPROTO_TCP)
        print(f'Send: {message}')
        writer.write(message.encode())
        data = await reader.read(100)
        print(f'Response: {data.decode()}')
        writer.close()
        await writer.wait_closed()
    except ConnectionResetError:
        print("Connection was reset by the server.")
    except Exception as e:
        print(f"An error occurred: {e}")


def async_run(coroutine):
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    loop.run_until_complete(coroutine)


def register(pid):
    message = f"register:{pid},1,2,{ddl_pid[pid]}"
    threading.Thread(target=async_run, args=(send_message(message),)).start()


def deregister(pid):
    message = f"deregister:{pid}"
    threading.Thread(target=async_run, args=(send_message(message),)).start()


Initialize container PIDs
container_pids = [get_container_entry_pid(
    container_id) for container_id in get_all_container_ids()]
# container_pids = ddl_pid.keys()

# Start a thread to refresh container PIDs
# pid_refresh_thread = threading.Thread(target=refresh_container_pids)
# pid_refresh_thread.daemon = True
# pid_refresh_thread.start()

# Define BPF program
bpf_text = """
#include <uapi/linux/ptrace.h>
#include <net/sock.h>
#include <linux/inet.h>
#include <linux/net.h>
#include <bcc/proto.h> 

struct data_t {
    u32 pid;
    u64 timestamp;
    u32 fd;
    char event_type[10];
};

BPF_PERF_OUTPUT(events);

int kprobe__tcp_sendmsg(struct pt_regs *ctx, struct sock *sk) {
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    struct data_t data = {};
    data.pid = pid;
    data.timestamp = bpf_ktime_get_ns();
    data.fd = sk->sk_socket->file->f_inode->i_ino;
    __builtin_memcpy(&data.event_type, "send", 5);
    events.perf_submit(ctx, &data, sizeof(data));
    return 0;
}

int kprobe__tcp_recvmsg(struct pt_regs *ctx, struct sock *sk) {
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    struct data_t data = {};
    data.pid = pid;
    data.timestamp = bpf_ktime_get_ns();
    data.fd = sk->sk_socket->file->f_inode->i_ino;
    __builtin_memcpy(&data.event_type, "recv", 5);
    events.perf_submit(ctx, &data, sizeof(data));
    return 0;
}
"""

b = BPF(text=bpf_text)


class Data(ctypes.Structure):
    _fields_ = [
        ("pid", ctypes.c_uint),
        ("timestamp", ctypes.c_ulonglong),
        ("fd", ctypes.c_uint),
        ("event_type", ctypes.c_char * 10)
    ]


def callback_fn(cpu, data, size):
    event = ctypes.cast(data, ctypes.POINTER(Data)).contents
    with threading.Lock():
        if event.pid in container_pids:
            fd = event.fd
            event_type = event.event_type.decode('utf-8')

            if event.pid not in fd_state:
                fd_state[event.pid] = {}
            if fd not in fd_state[event.pid]:
                fd_state[event.pid][fd] = {
                    'recv_seen': False, 'registered': False, 'send_after_recv': False}

            # if fd in fd_state.get(event.pid, {}):
                # update_fd_state(event.pid, fd, event_type)
            if event_type == 'recv' and not fd_state[event.pid][fd]['registered']:
                fd_state[event.pid][fd]['recv_seen'] = True
            elif event_type == 'send' and fd_state[event.pid][fd]['recv_seen']:
                fd_state[event.pid][fd]['send_after_recv'] = True

            # Register on the first 'recv' and mark as registered
            if event_type == 'recv' and fd_state[event.pid][fd]['recv_seen'] and not fd_state[event.pid][fd]['registered'] and (event.pid, fd) not in deregistered_pair:
                    register(event.pid)
                    fd_state[event.pid][fd]['registered'] = True

            # Deregister on the first 'send' after 'recv'
            elif event_type == 'send' and fd_state[event.pid][fd]['recv_seen'] and fd_state[event.pid][fd]['send_after_recv']:
                deregister(event.pid)
                del fd_state[event.pid][fd]
                deregistered_pair.add((event.pid, fd))

            # print(f"CPU: {cpu}, PID: {event.pid}, FD: {fd}, Size: {size}, Event: {event_type}, Timestamp: {event.timestamp / 1000000} milliseconds")


b.attach_kprobe(event="tcp_sendmsg", fn_name="kprobe__tcp_sendmsg")
b.attach_kprobe(event="tcp_recvmsg", fn_name="kprobe__tcp_recvmsg")
b["events"].open_perf_buffer(callback_fn)

print(
    f"Monitoring TCP send/recv events for PID {container_pids}. Ctrl-C to end.")
while True:
    try:
        b.perf_buffer_poll()
    except KeyboardInterrupt:
        break
