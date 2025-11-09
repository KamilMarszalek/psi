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
        for i in range(65000, 66000, 1):
            try:
                datagram_to_be_sent = Datagram(i, get_bytes(i))
                stream_data = datagram_to_be_sent.to_bytes()
                s.sendto(stream_data, (HOST, port))
                data = s.recv(datagram_to_be_sent.datagram_length)
                received_datagram = Datagram.from_bytes(data)
                assert received_datagram == datagram_to_be_sent
            except OSError as e:
                print(e)
                print(datagram_to_be_sent.datagram_length)
                break


if __name__ == "__main__":
    main(sys.argv)
