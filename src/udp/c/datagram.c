#include "datagram.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Datagram *create_datagram(unsigned char seq_bit, unsigned short packet_num,
                          unsigned short length, const char *content) {
  Datagram *d = malloc(sizeof(Datagram));
  if (!d) {
    fprintf(stderr, "Failed to allocate Datagram\n");
    return NULL;
  }

  d->header.seq_bit = seq_bit & 1;
  d->header.packet_num = packet_num;
  d->length = length;
  d->content = malloc(length);
  if (!d->content) {
    fprintf(stderr, "Failed to allocate Datagram content\n");
    free(d);
    return NULL;
  }

  memcpy(d->content, content, length);
  return d;
}

void free_datagram(Datagram *d) {
  if (!d) {
    return;
  }
  free(d->content);
  d->content = NULL;
  free(d);
}

char *to_bytes(const Datagram *d, size_t *out_size) {
  if (!d) {
    return NULL;
  }

  *out_size = sizeof(unsigned char) + 2 * sizeof(unsigned short) + +d->length;
  char *buffer = malloc(*out_size);

  if (!buffer) {
    return NULL;
  }

  size_t offset = 0;

  buffer[offset] = d->header.seq_bit;
  offset += sizeof(unsigned char);

  unsigned short net_pkt = htons(d->header.packet_num);
  memcpy(buffer + offset, &net_pkt, sizeof(net_pkt));
  offset += sizeof(net_pkt);

  unsigned short net_length = htons(d->length);
  memcpy(buffer + offset, &net_length, sizeof(net_length));
  offset += sizeof(net_length);

  memcpy(buffer + offset, d->content, d->length);

  return buffer;
}

Datagram *from_bytes(const char *bytes, size_t size) {
  if (size < sizeof(unsigned short)) {
    fprintf(stderr, "Buffer too small for datagram\n");
    return NULL;
  }

  size_t offset = 0;

  unsigned char seq_bit = bytes[offset];
  offset += sizeof(unsigned char);

  unsigned short net_pkt;
  memcpy(&net_pkt, bytes + offset, sizeof(unsigned short));
  unsigned short packet_num = ntohs(net_pkt);
  offset += sizeof(net_pkt);

  unsigned short net_length;
  memcpy(&net_length, bytes + offset, sizeof(unsigned short));
  unsigned short length = ntohs(net_length);
  offset += sizeof(net_length);

  if (size < offset + length) {
    fprintf(stderr, "Incomplete datagram\n");
    return NULL;
  }

  const char *content = bytes + offset;

  return create_datagram(seq_bit, packet_num, length, content);
}
