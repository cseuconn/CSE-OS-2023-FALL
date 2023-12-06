import asyncio
import os
import socket


async def tcp_echo_client(message):
    try:
        reader, writer = await asyncio.open_connection(
            '127.0.0.1', 1234, family=socket.AF_INET, proto=socket.IPPROTO_TCP)

        print(f'Send: {message}')
        writer.write(message.encode())

        data = await reader.read(100)
        print(f'Received: {data.decode()}')

        print('Close the socket')
        writer.close()
        await writer.wait_closed()
    except ConnectionResetError:
        print("Connection was reset by the server.")
    except Exception as e:
        print(f"An error occurred: {e}")


messages = [
    # f"register:{os.getpid()},1,2",
    "setTimeSlice:10",
    # "getTimeSlice",
    # f"deregister:{os.getpid()}"
]


async def main():
    for message in messages:
        await tcp_echo_client(message)
        await asyncio.sleep(5)


asyncio.run(main())
