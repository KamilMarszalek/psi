
#include "buffer.h"
#include <stddef.h>
#include <string.h>


void buffer_write_bytes(buffer_t* buf, const void* src, const size_t len) {
  memcpy(buf->data, src, len);
  buf->length += len;
}
