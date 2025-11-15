#include "binary_tree.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {
  int sock;
  struct sockaddr_in server;

  sock = socket(AF_INET, SOCK_STREAM, 0);
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

  node_t* root = node_new(1, 10, "a");
  root->left = node_new(2, 20, "ab");
  root->right = node_new(3, 30, "abc");

  if (tree_send_preorder(sock, root) == -1) {
    perror("sending nodes over socket");
    exit(EXIT_FAILURE);
  }

  tree_free(root);
  close(sock);

  return 0;
}
