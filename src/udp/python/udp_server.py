import socket
import sys

from datagram import Datagram

HOST = "0.0.0.0"
BUFSIZE = 1048576


def parse_port(args):
    if len(args) < 2:
        print("no port passed, ephemeral port will be used")
        return 0
    return int(args[1])


def create_udp_socket(port):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind((HOST, port))
    real_port = s.getsockname()[1]
    print(f"Server listening on {HOST}:{real_port}")
    return s


def echo_loop(sock: socket.socket):
    while True:
        data, addr = sock.recvfrom(BUFSIZE)
        if not data:
            continue

        d = Datagram.from_bytes(data)
        sock.sendto(d.to_bytes(), addr)


def main(args):
    port = parse_port(args)
    with create_udp_socket(port) as s:
        echo_loop(s)


if __name__ == "__main__":
    main(sys.argv)
