#include "binary_tree.h"
#include "buffer.h"

#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>

volatile sig_atomic_t is_running = 1;
void handle_signal(int sig) {
  is_running = 0;
}

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

  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  while (is_running) {
    int client_sock = accept(server_sock, NULL, NULL);
    if (client_sock == -1) {
      perror("accepting connection");
      continue;
    }
    printf("\nClient connected.\n");

    uint32_t tree_size_n = 0;
    if (recv(client_sock, &tree_size_n, sizeof(tree_size_n), MSG_WAITALL) < 0) {
      perror("receiving tree size");
      close(client_sock);
      continue;
    }
    uint32_t tree_size = ntohl(tree_size_n);
    printf("Received tree size: %u bytes.\n", tree_size);

    uint8_t* buf = malloc(tree_size);
    size_t total_received = 0;
    int error_occured = 0;
    while (total_received < tree_size_n) {
      ssize_t received = recv(client_sock, buf + total_received, tree_size_n - total_received, 0);
      if (received == 0) {
        break;
      }
      if (received < 0) {
        perror("receiving serialized tree");
        error_occured = 1;
        break;
      }
      total_received += received;
    }
    if (error_occured) {
      free(buf);
      close(client_sock);
      continue;
    }
    printf("Received tree serialized tree.\n");

    buffer_t serialized_tree = {.data = buf, .offset = 0};
    node_t* root = tree_deserialize_preorder(&serialized_tree);
    printf("Successfully deserialized tree.\n\n");
    printf("First %d levels (level order):\n", 3);
    tree_print_level_order(root, 3);
    fflush(stdout);

    tree_free(root);
    free(buf);
    close(client_sock);
  }
  printf("\nShutdown requested - stopping server.\n");
}
