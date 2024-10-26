#ifndef BASE_ARENA
#define BASE_ARENA

#include "base.h"
#include "arena.h"

fn Arena *arenaBuild(size_t size, void *base_addr) {
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
    Arena *arena = mem;
    arena->base_addr = arena + sizeof(Arena);
    arena->head = arena->base_addr;
    arena->total_size = size;

    return arena;
  }
}

inline fn void arenaPop(Arena *arena, size_t bytes) {
  Assert(arena);
  arena->head = ClampBot(arena->head - bytes, arena->base_addr);
}

inline fn bool arenaFree(Arena *arena) {
#if OS_LINUX || OS_BSD
  return munmap(arena->base_addr, arena->total_size);
#elif OS_WINDOWS
  return VirtualFree(arena->base_addr, 0, MEM_RELEASE);
#else
  return false;
#endif
}

fn void *arenaMake(Arena *arena, size_t size) {
  Assert(arena);

  if (arena->head + size >=
      arena->base_addr + arena->total_size - sizeof(Arena)) {
    return 0;
  }

  void *res = arena->head;
  arena->head = arena->head + size;
  return res;
}

#endif
