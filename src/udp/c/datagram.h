#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <stddef.h>

typedef struct {
  unsigned short length;
  char *content;
} Datagram;

Datagram *create_datagram(unsigned short length, const char *content);
void free_datagram(Datagram *d);
char *to_bytes(const Datagram *d, size_t *out_size);
Datagram *from_bytes(const char *bytes, size_t size);

#endif