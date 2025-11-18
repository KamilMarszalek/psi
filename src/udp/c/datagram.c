#include "datagram.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Datagram *create_datagram(unsigned short length, const char *content) {
  Datagram *d = malloc(sizeof(Datagram));
  if (!d) {
    fprintf(stderr, "Failed to allocate Datagram\n");
    return NULL;
  }

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

  *out_size = sizeof(unsigned short) + d->length;
  char *buffer = malloc(*out_size);
  if (!buffer) {
    return NULL;
  }
  unsigned short net_length = htons(d->length);
  memcpy(buffer, &net_length, sizeof(unsigned short));

  memcpy(buffer + sizeof(unsigned short), d->content, d->length);

  return buffer;
}

Datagram *from_bytes(const char *bytes, size_t size) {
  if (size < sizeof(unsigned short)) {
    fprintf(stderr, "Buffer too small for datagram\n");
    return NULL;
  }

  unsigned short net_length;
  memcpy(&net_length, bytes, sizeof(unsigned short));
  unsigned short length = ntohs(net_length);

  if (size < sizeof(unsigned short) + length) {
    fprintf(stderr, "Incomplete datagram\n");
    return NULL;
  }
  const char *content = bytes + sizeof(unsigned short);
  return create_datagram(length, content);
}

int are_datagrams_equal(const Datagram *d1, const Datagram *d2) {
  if (d1->length != d2->length) {
    return 0;
  }
  return memcmp(d1->content, d2->content, d1->length) == 0;
}