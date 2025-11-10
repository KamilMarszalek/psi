#include "bytes_generator.h"
#include <stdio.h>
#include <stdlib.h>

char *generate_bytes(unsigned short length) {
  char *bytes = malloc(length);
  if (!bytes) {
    fprintf(stderr, "Failed to allocate bytes\n");
    return NULL;
  }
  for (int i = 0; i < length; ++i) {
    *(bytes + i) = 'A' + (i % 26);
  }
  return bytes;
}