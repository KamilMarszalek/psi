#include "binary_tree.h"
#include "buffer.h"

#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char* argv[]) {
  int port = 0;
  if (argc >= 2) {
    port = atoi(argv[1]);
  }

  int server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock == -1) {
    perror("opening socket");
    exit(EXIT_FAILURE);
  }
  printf("TCP server socket created.\n");

  struct sockaddr_in server_addr;
  socklen_t length = sizeof(server_addr);
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  if (bind(server_sock, (struct sockaddr*) &server_addr, length) == -1) {
    perror("binding stream socket");
    exit(EXIT_FAILURE);
  }

  if (getsockname(server_sock, (struct sockaddr*) &server_addr, &length) == -1) {
    perror("getting socket name");
    exit(EXIT_FAILURE);
  }
  printf("Bind to the port number %d.\n", ntohs(server_addr.sin_port));

  if (listen(server_sock, 1) == -1) {
    perror("starting to listen");
    exit(EXIT_FAILURE);
  }
  printf("Listening...\n");

  while (1) {
    int client_sock = accept(server_sock, NULL, NULL);
    if (client_sock == -1) {
      perror("accepting connection");
      exit(EXIT_FAILURE);
    }
    printf("\nClient connected.\n");

    uint32_t tree_size_n = 0;
    if (recv(client_sock, &tree_size_n, sizeof(tree_size_n), MSG_WAITALL) < 0) {
      perror("receiving tree size");
      exit(EXIT_FAILURE);
    }
    uint32_t tree_size = ntohl(tree_size_n);
    printf("Received tree size: %u bytes.\n", tree_size);

    uint8_t* buf = malloc(tree_size);
    size_t received = 0;
    while (received < tree_size_n) {
      ssize_t n = recv(client_sock, buf + received, tree_size_n - received, 0);
      if (n == 0) {
        break;
      }
      if (n < 0) {
        perror("receiving serialized tree");
        exit(EXIT_FAILURE);
      }
      received += n;
    }
    printf("Received tree serialized tree.\n");

    buffer_t serialized_tree = {.data = buf, .offset = 0};
    node_t* root = tree_deserialize_preorder(&serialized_tree);
    printf("Successfully deserialized tree.\n");
    printf("First %d levels (level order):\n", 3);
    tree_print_level_order(root, 3);
  }
}
