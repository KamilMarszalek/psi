#include "bytes_generator.h"
#include "datagram.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8000
#define BUFFER_SIZE (1 << 16) - 1

int main() {
  int sockfd;
  struct sockaddr_in server;
  char recv_buffer[BUFFER_SIZE];

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

  for (int i = 65000; i < 66000; ++i) {
    char *message = generate_bytes(i);
    if (!message) {
      perror("message generation failed");
      exit(EXIT_FAILURE);
    }
    Datagram *d = create_datagram(i, message);
    size_t packet_size;
    char *packet = to_bytes(d, &packet_size);

    if (sendto(sockfd, packet, packet_size, 0, (const struct sockaddr *)&server,
               sizeof(server)) < 0) {
      fprintf(stderr, "failed to send datagram (length=%d)\n", d->length + 2);
      exit(EXIT_FAILURE);
    }
    printf("Sent %zu bytes to server\n", packet_size);

    ssize_t n = recvfrom(sockfd, recv_buffer, BUFFER_SIZE, 0, NULL, NULL);
    if (n > 0) {
      Datagram *resp = from_bytes(recv_buffer, n);
      if (resp) {
        printf("Received reply (%d bytes): \"%.*s\"\n", resp->length,
               resp->length, resp->content);
        free_datagram(resp);
      }
    }
    free(packet);
    free_datagram(d);
  }
  close(sockfd);
  return 0;
}