#include <netdb.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <unistd.h>

#define PORT 18000
#define N_CLIENTS 5

int main(int argc, char* argv[]) {
  struct sockaddr_in server_addr;

  int server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock == -1) {
    perror("opening socket");
    exit(EXIT_FAILURE);
  }
  printf("TCP server socket created.\n");

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  if (bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
    perror("binding stream socket");
    exit(EXIT_FAILURE);
  }
  printf("Bind to the port number %d.\n", PORT);

  if (listen(server_sock, N_CLIENTS) == -1) {
    perror("starting to listen");
    exit(EXIT_FAILURE);
  }
  printf("Listening...\n\n");

  while (1) {
    int client_sock = accept(server_sock, NULL, NULL);
    if (client_sock == -1) {
      perror("accepting connection");
      exit(EXIT_FAILURE);
    }
    printf("Client connected.\n");

    size_t tree_size = 0;
    if (recv(client_sock, &tree_size, sizeof(tree_size), MSG_WAITALL) < 0) {
      perror("receiving tree size");
      exit(EXIT_FAILURE);
    }
    printf("Received tree size: %zu bytes.\n", tree_size);

    uint8_t* buf = malloc(tree_size);
    size_t received = 0;
    while (received < tree_size) {
      ssize_t n = recv(client_sock, buf + received, tree_size - received, 0);
      if (n == 0) { break; }
      if (n < 0) {
        perror("receiving serialized tree");
        free(buf);
        exit(EXIT_FAILURE);
      }
      received += n;
    }
    printf("Received tree ready to deserialize.\n\n");
  }
  return 0;
}
