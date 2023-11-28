import asyncio
import os
import socket


async def tcp_echo_client(message, client_id):
    try:
        reader, writer = await asyncio.open_connection(
            '127.0.0.1', 1234, family=socket.AF_INET, proto=socket.IPPROTO_TCP)

        print(f'Client {client_id}: Send: {message}')
        writer.write(message.encode())

        data = await reader.read(100)
        print(f'Client {client_id}: Received: {data.decode()}')

        writer.close()
        await writer.wait_closed()
    except ConnectionResetError:
        print(f"Client {client_id}: Connection was reset by the server.")
    except Exception as e:
        print(f"Client {client_id}: An error occurred: {e}")


async def client_task(client_id, num_requests):
    pid = os.getpid() + client_id
    messages = [
                   f"register:{pid},1,2",
                   "setTimeSlice:10",
                   "getTimeSlice",
                   f"deregister:{pid}"
               ] * num_requests

    tasks = [tcp_echo_client(message, client_id) for message in messages]
    await asyncio.gather(*tasks)


async def main(num_clients, num_requests_per_client):
    tasks = [client_task(i, num_requests_per_client) for i in range(num_clients)]
    await asyncio.gather(*tasks)


num_clients = 100
num_requests_per_client = 15

asyncio.run(main(num_clients, num_requests_per_client))
