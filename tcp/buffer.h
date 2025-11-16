#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct Buffer {
  uint8_t* data;
  size_t offset;
} buffer_t;

void buffer_write_bytes(buffer_t* buf, const void* src, size_t len);
void buffer_read_bytes(buffer_t* buf, void* dest, size_t len);

#endif
