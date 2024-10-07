#ifndef CSTD_ARENA
#define CSTD_ARENA

#include "base.cpp"

#if OS_LINUX || OS_BSD
#include <sys/mman.h>
#elif OS_WINDOWS
#include <windows.h>
#endif

namespace cstd {
struct Arena {
  void *base_addr;
  void *head;
  size_t total_size;
};

template <typename T, typename... A>
T *make(Arena *arena, size_t count = 1, A... args) {
  assert(arena != 0);

  T *res = (T *)arena->head;
  size_t objsize = sizeof(T);
  size_t align = alignof(T);
  size_t pad = (size_t)arena->head & (align - 1);

  for (size_t i = 0; i < count; ++i) {
    new ((void *)&res[i]) T(args...);
  }

  arena->head = (void *)((u8 *)arena->head + count * objsize + pad);
  return res;
}

Arena *make_arena(size_t size, void *base_addr) {
#if OS_LINUX || OS_BSD
  void *fail_state = MAP_FAILED;
  void *mem = mmap(base_addr, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#elif OS_WINDOWS
  void *fail_state = 0;
  void *mem =
      VirtualAlloc(base_addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#endif

  if (mem == fail_state) {
    return 0;
  } else {
    Arena *arena = (Arena *)mem;
    arena->base_addr = (void *)((u8 *)arena + sizeof(Arena));
    arena->head = arena->base_addr;
    arena->total_size = size;

    return arena;
  }
}

inline Arena *make_arena(size_t size) { return make_arena(size, 0); }

inline bool arena_free(Arena *arena) {
#if OS_LINUX || OS_BSD
  return munmap(arena->base_addr, arena->total_size);
#elif OS_WINDOWS
  return VirtualFree(arena->base_addr, 0, MEM_RELEASE);
#else
  return false;
#endif
}
} // namespace cstd

#endif
