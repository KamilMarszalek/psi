#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <unistd.h>

#define BUF_SIZE 1 << 10// 100 KiB
#define PORT 18000

int main(int argc, char* argv[]) {
  char buf[BUF_SIZE];
  int serverSock, clientSocket;
  struct sockaddr_in servAddr;
  size_t servAddrSize = sizeof(servAddr);
  int rval;

  if ((serverSock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("opening socket");
    exit(EXIT_FAILURE);
  }

  memset(&servAddr, 0, servAddrSize);
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(PORT);

  if (bind(serverSock, (struct sockaddr*) &servAddr, servAddrSize) == -1) {
    perror("binding stream socket");
    exit(EXIT_FAILURE);
  }

  printf("Socket port %d\n", ntohs(servAddr.sin_port));

  if (listen(serverSock, 1) < 0) {
    perror("starting to listen");
    exit(EXIT_FAILURE);
  }

  printf("Listening on port %d\n", ntohs(servAddr.sin_port));

  if ((clientSocket = accept(serverSock, (struct sockaddr*) &servAddr, (socklen_t*) &servAddrSize)) == -1) {
    perror("accepting connection");
    exit(EXIT_FAILURE);
  }
  do {
    memset(buf, 0, sizeof(buf));
    if ((rval = read(clientSocket, buf, sizeof(buf))) == -1) {
      perror("reading stream message");
      exit(EXIT_FAILURE);
    }
    if (rval == 0) {
      printf("Ending connection\n");
      break;
    } else {
      printf("-->%s\n", buf);
    }
  } while (rval > 0);
  return 0;
}
