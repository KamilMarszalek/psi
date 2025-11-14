#include "datagram.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE ((1 << 16) - 1)

int parse_port(int argc, char **argv) {
  if (argc < 2) {
    printf("No port provided, ephemeral port will be used\n");
    return 0;
  }
  return atoi(argv[1]);
}

int create_udp_socket(void) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  return sockfd;
}

void bind_udp_socket(int sockfd, int port) {
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
}

int get_bound_port(int sockfd) {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);

  if (getsockname(sockfd, (struct sockaddr *)&addr, &len) < 0) {
    perror("getsockname failed");
    exit(EXIT_FAILURE);
  }

  return ntohs(addr.sin_port);
}

void handle_echo(int sockfd) {
  char buffer[BUFFER_SIZE];
  struct sockaddr_in client;
  socklen_t client_len = sizeof(client);
  ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                       (struct sockaddr *)&client, &client_len);

  if (n < 0) {
    perror("recvfrom failed");
    return;
  }

  Datagram *d = from_bytes(buffer, n);
  if (!d) {
    fprintf(stderr, "Failed to parse datagram\n");
    return;
  }

  printf("Received datagram (%d bytes): \"%.*s\"\n", d->length, d->length,
         d->content);
  size_t out_size;
  char *reply = to_bytes(d, &out_size);
  sendto(sockfd, reply, out_size, 0, (struct sockaddr *)&client, client_len);
  free(reply);
  free_datagram(d);
}

int main(int argc, char *argv[]) {
  int port = parse_port(argc, argv);
  int sockfd = create_udp_socket();

  bind_udp_socket(sockfd, port);

  int real_port = get_bound_port(sockfd);
  printf("UDP server listening on port %d...\n", real_port);

  while (1) {
    handle_echo(sockfd);
  }

  close(sockfd);
  return 0;
}
