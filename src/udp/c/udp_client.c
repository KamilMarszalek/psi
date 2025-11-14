#define _GNU_SOURCE
#include "bytes_generator.h"
#include "datagram.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE ((1 << 16) - 1)
#define MSG_LEN 512
#define TIMEOUT_SEC 0       // seconds
#define TIMEOUT_USEC 750000 // microseconds

void parse_args(int argc, char **argv, char **host, int *port) {
  if (argc < 3) {
    *host = SERVER_IP;
    *port = 0;
  } else {
    *host = argv[1];
    *port = atoi(argv[2]);
  }
}

int create_udp_socket(void) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  return sockfd;
}

void resolve_host(const char *host, int port, struct sockaddr_in *server) {
  memset(server, 0, sizeof(*server));
  server->sin_family = AF_INET;
  server->sin_port = htons(port);

  int r = inet_pton(AF_INET, host, &server->sin_addr);

  if (r == 1) {
    printf("Using IPv4 address: %s\n", host);
    return;
  }

  if (r == 0) {
    printf("Resolving hostname via DNS: %s\n", host);

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int err = getaddrinfo(host, NULL, &hints, &res);
    if (err != 0) {
      fprintf(stderr, "DNS lookup failed for %s: %s\n", host,
              gai_strerror(err));
      exit(EXIT_FAILURE);
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    server->sin_addr = addr->sin_addr;
    freeaddrinfo(res);
    return;
  }

  perror("inet_pton");
  exit(EXIT_FAILURE);
}

void send_and_receive(int sockfd, struct sockaddr_in *server, int msg_len,
                      unsigned char seq_bit) {
  char recv_buffer[BUFFER_SIZE];

  char *message = generate_bytes(msg_len);
  if (!message) {
    perror("message generation failed");
    exit(EXIT_FAILURE);
  }

  Datagram *d = create_datagram(seq_bit, msg_len, message);

  size_t packet_size;
  char *packet = to_bytes(d, &packet_size);

  if (sendto(sockfd, packet, packet_size, 0, (const struct sockaddr *)server,
             sizeof(*server)) < 0) {
    fprintf(stderr, "failed to send datagram (length=%d)\n", d->length + 2);
    exit(EXIT_FAILURE);
  }
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

int main(int argc, char *argv[]) {
  char *host;
  int port;

  parse_args(argc, argv, &host, &port);

  int sockfd = create_udp_socket();

  struct sockaddr_in server;
  resolve_host(host, port, &server);

  struct timeval tv;
  tv.tv_sec = TIMEOUT_SEC;
  tv.tv_usec = TIMEOUT_USEC;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  unsigned char seq_bit = 0;
  int num_packets = 10;

  for (int i = 0; i < num_packets; ++i) {
    printf("Sending packet %d/%d with seq_bit=%u\n", i + 1, num_packets,
           seq_bit);

    send_and_receive(sockfd, &server, MSG_LEN, seq_bit);

    seq_bit = 1 - seq_bit;
  }

  close(sockfd);
  return 0;
}
