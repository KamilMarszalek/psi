#define _GNU_SOURCE
#include "binary_tree.h"
#include "buffer.h"

#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SERVER_IP "127.0.0.1"
#define N_NODES 2500

int main(int argc, char* argv[]) {
  char* host = SERVER_IP;
  int port = 0;
  if (argc < 3) {
    fprintf(stderr, "address not specified");
  } else {
    host = argv[1];
    port = atoi(argv[2]);
  }

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    perror("opening socket stream");
    exit(EXIT_FAILURE);
  }
  printf("TCP client socket created.\n");

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  int r = inet_pton(AF_INET, host, &server_addr.sin_addr);
  if (r < 0) {
    perror("converting address to binary number");
    exit(EXIT_FAILURE);
  }
  if (r == 0) {
    printf("Resolving address via DNS: %s\n", host);
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, NULL, &hints, &res) != 0) {
      fprintf(stderr, "resolving address: unknown error");
      exit(EXIT_FAILURE);
    };
    struct sockaddr_in* addr = (struct sockaddr_in*) res->ai_addr;
    server_addr.sin_addr = addr->sin_addr;
    freeaddrinfo(res);
  }

  if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
    perror("connecting to stream socket");
    exit(EXIT_FAILURE);
  };
  printf("Connected to server socket.\n\n");

  srand(time(0));
  node_t* root = tree_create_random(0, N_NODES);
  printf("Created random binary tree composed of %d nodes.\n", N_NODES);
  printf("First %d levels (level order):\n", 3);
  tree_print_level_order(root, 3);

  uint32_t tree_size = tree_calc_serialized_size(root);
  uint32_t tree_size_n = htonl(tree_size);
  if (send(sock, &tree_size_n, sizeof(tree_size_n), 0) < 0) {
    perror("sending tree size");
    exit(EXIT_FAILURE);
  }
  printf("\nTree size (%u bytes) sent.\n", tree_size);

  uint8_t* buf = malloc(tree_size);
  buffer_t serialized_tree = {.data = buf, .offset = 0};
  tree_serialize_preorder(root, &serialized_tree);
  size_t sent = 0;
  while (sent < tree_size) {
    ssize_t n = send(sock, serialized_tree.data + sent, tree_size - sent, 0);
    if (n == 0) {
      break;
    }
    if (n < 0) {
      perror("sending serialized tree");
      exit(EXIT_FAILURE);
    }
    sent += n;
  }
  printf("Serialized tree sent. Closing connection.\n");

  tree_free(root);
  free(buf);
  close(sock);
  return 0;
}
