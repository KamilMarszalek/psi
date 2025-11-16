#include "binary_tree.h"
#include "buffer.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {
  struct sockaddr_in server;

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    perror("opening socket stream");
    exit(EXIT_FAILURE);
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_port = htons(18000);

  if (connect(sock, (struct sockaddr*) &server, sizeof(server)) == -1) {
    perror("connecting to stream socket");
    exit(EXIT_FAILURE);
  };

  srand(time(0));
  node_t* root = tree_create_random_inorder(0, 2500);
  uint32_t tree_size = tree_calc_serialized_size(root);
  uint32_t tree_size_n = htonl(tree_size);
  if (send(sock, &tree_size_n, sizeof(tree_size_n), 0) < 0) {
    perror("sending tree size");
    tree_free(root);
    exit(EXIT_FAILURE);
  }

  uint8_t* buf = malloc(tree_size);
  buffer_t serialized_tree = {.data = buf, .offset = 0};
  tree_serialize_preorder(root, &serialized_tree);
  size_t sent = 0;
  while (sent < tree_size) {
    ssize_t n = send(sock, serialized_tree.data + sent, tree_size - sent, 0);
    if (n == 0) { break; }
    if (n < 0) {
      perror("sending serialized tree");
      exit(EXIT_FAILURE);
    }
    sent += n;
  }

  tree_free(root);
  free(buf);
  close(sock);
  return 0;
}
