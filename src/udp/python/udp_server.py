import socket
import sys
from datagram import Datagram

HOST = "127.0.0.1"
BUFSIZE = 1048576


def main(args):
    if len(args) < 2:
        print("default port 8000")
        port = 8000
    else:
        port = int(args[1])
    print("Will listen on", HOST, ":", port)

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.bind((HOST, port))
        while True:
            data_address = s.recvfrom(BUFSIZE)
            data = data_address[0]
            address = data_address[1]
            datagram = Datagram.from_bytes(data)
            print("Message from client", str(address))
            print("Received datagram", str(datagram))
            if not data:
                break
            s.sendto(datagram.to_bytes(), address)


if __name__ == "__main__":
    main(sys.argv)
