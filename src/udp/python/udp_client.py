import socket
import sys

from bytes_generator import get_bytes
from datagram import Datagram

HOST = "127.0.0.1"
PORT = 8000
DEFAULT_START = 65000
DEFAULT_STOP = 66000
DEFAULT_STEP = 1


def parse_args(args):
    if len(args) < 3:
        print(f"default host: {HOST}")
        print(f"default port: {PORT}")
        return HOST, PORT

    return args[1], int(args[2])


def create_udp_socket():
    return socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


def send_one_datagram(sock, host, port, length):
    """Send one datagram and return received datagram (or None)."""
    outgoing = Datagram(length, get_bytes(length))
    outgoing_bytes = outgoing.to_bytes()

    sock.sendto(outgoing_bytes, (host, port))
    data, _ = sock.recvfrom(outgoing.datagram_length)

    incoming = Datagram.from_bytes(data)

    if incoming != outgoing:
        raise ValueError("Received datagram mismatch!")

    return incoming


def run_test_datagrams(sock, host, port, start, stop, step):
    for length in range(start, stop, step):
        try:
            send_one_datagram(sock, host, port, length)
            # print(f"OK len={length}")
        except Exception as e:
            print("ERROR at length", length, "->", e)
            break


def main(args):
    host, port = parse_args(args)
    print("Will send to", host, ":", port)

    with create_udp_socket() as sock:
        run_test_datagrams(
            sock,
            host,
            port,
            DEFAULT_START,
            DEFAULT_STOP,
            DEFAULT_STEP,
        )


if __name__ == "__main__":
    main(sys.argv)
