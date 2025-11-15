#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct Buffer {
  uint8_t* data;
  size_t length;
} buffer_t;

void buffer_write_bytes(buffer_t* buf, const void* src, size_t len);

#endif
