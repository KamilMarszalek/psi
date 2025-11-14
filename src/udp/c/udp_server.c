#include "datagram.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8000
#define BUFFER_SIZE (1 << 16) - 1

int main(int argc, char *argv[]) {
  int port = 0;
  if (argc < 2) {
    printf("no port provided, efemeric port will be given\n");
  } else {
    port = atoi(argv[1]);
  }
  int sockfd;
  struct sockaddr_in server, client;
  socklen_t client_len = sizeof(client);
  char buffer[BUFFER_SIZE];

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  if (bind(sockfd, (const struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  socklen_t length = sizeof(server);
  if (getsockname(sockfd, (struct sockaddr *)&server, &length) < 0) {
    perror("failed to get socket data");
    exit(EXIT_FAILURE);
  }
  port = server.sin_port;
  printf("UDP server listening on port %d...\n", port);
  while (1) {
    ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                         (struct sockaddr *)&client, &client_len);
    if (n < 0) {
      perror("recvfrom failed");
      continue;
    }

    Datagram *d = from_bytes(buffer, n);
    if (!d) {
      fprintf(stderr, "Failed to parse datagram\n");
      continue;
    }
    printf("Received datagram (%d bytes): \"%.*s\"\n", d->length, d->length,
           d->content);
    size_t out_size;
    char *reply = to_bytes(d, &out_size);
    sendto(sockfd, reply, out_size, 0, (struct sockaddr *)&client, client_len);
    free(reply);
    free_datagram(d);
  }
  close(sockfd);
  return 0;
}
