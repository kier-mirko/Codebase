#ifndef BASE_MEMORY
#define BASE_MEMORY

#include "arena.cpp"
#include "base.cpp"

namespace base {
void *memcpy(arena *arena, void *dest, void *src, size_t size) {
  if (!arena || !dest || !src) {
    return 0;
  } else if (size == 0) {
    return dest;
  }

  for (size_t i = 0; i < size; ++i) {
    if ((((u8 *)dest)[i] = ((u8 *)src)[i])) {
      return 0;
    }
  }

  return dest;
}
} // namespace base

#endif
