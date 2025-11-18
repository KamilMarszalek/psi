#define _GNU_SOURCE
#include "bytes_generator.h"
#include "datagram.h"
#include <errno.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE ((1 << 16) - 1)
#define MSG_LEN 512
#define TIMEOUT_SEC 1      // seconds
#define TIMEOUT_USEC 500000// microseconds

typedef enum { RETRANSMIT_ENABLED = 1, RETRANSMIT_DISABLED = 0 } RetransmitMode;

int stop_requested = 0;
int successful_packets = 0;
int failed_packets = 0;
int total_packets = 0;

static void on_signal(int signo) {
  (void) signo;
  stop_requested = 1;
}

void parse_args(int argc, char** argv, char** host, int* port) {
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

void resolve_host(const char* host, int port, struct sockaddr_in* server) {
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
      fprintf(stderr, "DNS lookup failed for %s: %s\n", host, gai_strerror(err));
      exit(EXIT_FAILURE);
    }

    struct sockaddr_in* addr = (struct sockaddr_in*) res->ai_addr;
    server->sin_addr = addr->sin_addr;
    freeaddrinfo(res);
    return;
  }

  perror("inet_pton");
  exit(EXIT_FAILURE);
}

void send_and_receive(
        int sockfd, struct sockaddr_in* server, int msg_len, unsigned char seq_bit, unsigned short packet_num,
        RetransmitMode retransmit_mode
) {
  char recv_buffer[BUFFER_SIZE];

  char* message = generate_bytes(msg_len);
  if (!message) {
    perror("message generation failed");
    exit(EXIT_FAILURE);
  }

  Datagram* d = create_datagram(seq_bit, packet_num, msg_len, message);

  size_t packet_size;
  char* packet = to_bytes(d, &packet_size);
  if (!packet) {
    perror("to_bytes failed");
    free_datagram(d);
    exit(EXIT_FAILURE);
  }

  if (sendto(sockfd, packet, packet_size, 0, (const struct sockaddr*) server, sizeof(*server)) < 0) {
    perror("failed to send datagram");
    free(packet);
    free_datagram(d);
    exit(EXIT_FAILURE);
  }

  printf("Sent datagram with seq_bit: %u\n", seq_bit);
  ++total_packets;

  Datagram* resp = NULL;
  ssize_t n;

  while (1) {
    n = recvfrom(sockfd, recv_buffer, BUFFER_SIZE, 0, NULL, NULL);

    if (n < 0) {
      if (errno == EAGAIN) {
        if (retransmit_mode == RETRANSMIT_ENABLED) {
          printf("Timeout, resending datagram with seq_bit: '%u'\n", seq_bit);

          if (sendto(sockfd, packet, packet_size, 0, (const struct sockaddr*) server, sizeof(*server)) < 0) {
            free(packet);
            free_datagram(d);
            exit(EXIT_FAILURE);
          }
          ++total_packets;
          continue;
        } else {
          printf("Timeout, no retransmission (packet lost)\n");
          ++failed_packets;
          break;
        }
      } else {
        perror("recvfrom failed");
        break;
      }
    }

    resp = from_bytes(recv_buffer, n);
    if (!resp) {
      fprintf(stderr, "failed to parse response\n");
      continue;
    }

    if (resp->header.seq_bit == seq_bit) {
      printf("Received correct ACK(%u)\n", resp->header.seq_bit);
      ++successful_packets;
      break;
    } else {
      printf("Wrong ACK (expected %u, got %u)", seq_bit, resp->header.seq_bit);
      free_datagram(resp);
      resp = NULL;

      if (retransmit_mode == RETRANSMIT_ENABLED) {
        printf(", resending\n");
        if (sendto(sockfd, packet, packet_size, 0, (const struct sockaddr*) server, sizeof(*server)) < 0) {
          perror("failed to resend datagram");
          free(packet);
          free_datagram(d);
          exit(EXIT_FAILURE);
        }
        ++total_packets;
      } else {
        printf(", no retransmission\n");
        ++failed_packets;
        break;
      }
    }
  }

  if (resp) {
    printf("Received reply (seq_bit %u, packet_num: %d): \"%.*s\"\n", resp->header.seq_bit, resp->header.packet_num,
           resp->length, resp->content);
    free_datagram(resp);
  }

  free(packet);
  free_datagram(d);
}

int main(int argc, char* argv[]) {
  char* host;
  int port;

  parse_args(argc, argv, &host, &port);

  int sockfd = create_udp_socket();

  signal(SIGINT, on_signal);
  signal(SIGTERM, on_signal);

  struct sockaddr_in server;
  resolve_host(host, port, &server);

  struct timeval tv;
  tv.tv_sec = TIMEOUT_SEC;
  tv.tv_usec = TIMEOUT_USEC;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  unsigned char seq_bit = 0;
  unsigned short packet_count = 30;

  RetransmitMode mode = RETRANSMIT_ENABLED;

  printf("Running in %s mode\n\n", mode == RETRANSMIT_ENABLED ? "RETRANSMIT_ENABLED" : "RETRANSMIT_DISABLED");

  for (unsigned short i = 0; i < packet_count; ++i) {
    if (stop_requested) {
      break;
    }

    printf("\nSending packet %d with seq_bit=%u\n", i + 1, seq_bit);

    send_and_receive(sockfd, &server, MSG_LEN, seq_bit, i + 1, mode);

    seq_bit = 1 - seq_bit;
  }

  printf("\nTotal sent packets: %d\n", total_packets);
  printf("Successfully delivered packets: %d\n", successful_packets);
  printf("Failed packets: %d\n", failed_packets);
  if (mode == RETRANSMIT_ENABLED) {
    printf("Retransmissions: %d\n", total_packets - packet_count);
  }

  close(sockfd);
  return 0;
}
