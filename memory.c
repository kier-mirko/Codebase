#ifndef BASE_MEMORY
#define BASE_MEMORY

#include "base.h"
#include "arena.h"

void *memCopy(void *dest, void *src, usize size) {
  if (!dest || !src) {
    return 0;
  } else if (size == 0) {
    return dest;
  }

  u8 *dest_byte = dest, *src_byte = src;
  for (usize i = 0; i < size; ++i) {
    if (!(dest_byte[i] = src_byte[i])) {
      return 0;
    }
  }

  return dest;
}

void memZero(void *dest, usize size) {
  u8 *dest_bytes = dest;
  for (usize i = 0; i < size; ++i) {
    *(dest_bytes + i) = 0;
  }
}

#endif
