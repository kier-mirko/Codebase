#ifndef BASE_ARENA
#define BASE_ARENA

#include "base.cpp"

#if OS_LINUX || OS_BSD
#include <sys/mman.h>
#elif OS_WINDOWS
#include <windows.h>
#endif

#define make(arenaptr, type) (type *)arena_raw_make(arenaptr, sizeof(type))
#define makearr(arenaptr, type, count)                                         \
  (type *)arena_raw_make(arenaptr, (count) * sizeof(type))

namespace Base {
struct Arena {
  void *base_addr;
  void *head;
  size_t total_size;
};
} // namespace Base

fn Base::Arena *arena_build(size_t size, void *base_addr = 0) {
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
    Base::Arena *arena = (Base::Arena *)mem;
    arena->base_addr = (void *)((u8 *)arena + sizeof(Base::Arena));
    arena->head = arena->base_addr;
    arena->total_size = size;

    return arena;
  }
}

inline fn void arena_pop(Base::Arena *arena, size_t bytes) {
  Assert(arena);
  arena->head = ClampBot((u8 *)arena->head - bytes, arena->base_addr);
}

inline fn bool arena_free(Base::Arena *arena) {
#if OS_LINUX || OS_BSD
  return munmap(arena->base_addr, arena->total_size);
#elif OS_WINDOWS
  return VirtualFree(arena->base_addr, 0, MEM_RELEASE);
#else
  return false;
#endif
}

fn void *arena_raw_make(Base::Arena *arena, size_t size) {
  Assert(arena);

  if (((u8 *)arena->head + size) >= ((u8 *)arena->base_addr + arena->total_size - sizeof(Base::Arena))) {
    return 0;
  }

  void *res = arena->head;
  arena->head = (void *)((u8 *)arena->head + size);
  return res;
}

#endif
