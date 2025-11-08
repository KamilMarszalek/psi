import socket
import sys
from datagram import Datagram
from bytes_generator import get_bytes

HOST = "127.0.0.1"


def main(args):
    if len(args) < 2:
        print("default port 8000")
        port = 8000
    else:
        port = int(args[1])
    print("Will send to ", HOST, ":", port)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        for i in range(1, 1000000, 10):
            datagram_to_be_sent = Datagram(i, get_bytes(i))
            stream_data = datagram_to_be_sent.to_bytes()
            try:
                s.sendto(stream_data, (HOST, port))
            except OSError:
                print(datagram_to_be_sent)


if __name__ == "__main__":
    main(sys.argv)
