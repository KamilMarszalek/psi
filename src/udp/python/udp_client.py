import socket
import sys
from datagram import Datagram
from bytes_generator import get_bytes

HOST = "127.0.0.1"
PORT = 8000


def main(args):
    if len(args) < 3:
        print("default host:", HOST)
        print("default port", PORT)
        port = PORT
        host = HOST
    else:
        host = args[1]
        port = int(args[2])
    print("Will send to ", host, ":", port)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        for i in range(1, 66000, 1):
            try:
                datagram_to_be_sent = Datagram(i, get_bytes(i))
                stream_data = datagram_to_be_sent.to_bytes()
                s.sendto(stream_data, (host, port))
                data = s.recv(datagram_to_be_sent.datagram_length)
                received_datagram = Datagram.from_bytes(data)
                assert received_datagram == datagram_to_be_sent
                print("datagram length", received_datagram.length + 2)
            except OSError as e:
                print(e)
                print(datagram_to_be_sent.datagram_length)
                break


if __name__ == "__main__":
    main(sys.argv)
