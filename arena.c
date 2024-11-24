#ifndef BASE_ARENA
#define BASE_ARENA

#include "base.h"
#include "arena.h"

fn Arena *arenaBuild(usize size, usize base_addr) {
#if OS_LINUX || OS_BSD
  void *fail_state = MAP_FAILED;
  void *mem = mmap((void *)base_addr, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#elif OS_WINDOWS
  void *fail_state = 0;
  void *mem =
      VirtualAlloc((void *)base_addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#endif

  if (mem == fail_state) {
    return 0;
  } else {
    Arena *arena = mem;
    arena->base_addr = arena + sizeof(Arena);
    arena->head = arena->base_addr;
    arena->total_size = size - sizeof(Arena);

    return arena;
  }
}

inline fn void arenaPop(Arena *arena, usize bytes) {
  Assert(arena);
  arena->head = ClampBot(arena->head - bytes, arena->base_addr);
}

inline fn void arenaReset(Arena *arena) {
  arena->head = arena->base_addr;
}

inline fn bool arenaFree(Arena *arena) {
#if OS_LINUX || OS_BSD
  return munmap(arena->base_addr, arena->total_size + sizeof(Arena));
#elif OS_WINDOWS
  return VirtualFree(arena->base_addr, 0, MEM_RELEASE);
#else
  return false;
#endif
}

fn void *arenaPush(Arena *arena, usize size, usize align) {
  Assert(arena);

  usize padding = (align - ((usize)arena->head % align)) % align;
  if (arena->head + size + padding >=
      arena->base_addr + arena->total_size) {
    return 0;
  }

  void *res = arena->head + padding;
  arena->head = arena->head + padding + size;

  return res;
}

#endif
