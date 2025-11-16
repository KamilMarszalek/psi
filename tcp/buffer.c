
#include "buffer.h"
#include <string.h>


void buffer_write_bytes(buffer_t* buf, const void* src, size_t len) {
  memcpy(buf->data + buf->offset, src, len);
  buf->offset += len;
}

void buffer_read_bytes(buffer_t* buf, void* dest, size_t len) {
  memcpy(dest, buf->data + buf->offset, len);
  buf->offset += len;
}
