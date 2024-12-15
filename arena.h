#ifndef BASE_ARENA
#define BASE_ARENA

#include "base.h"

#if OS_LINUX || OS_BSD
#include <sys/mman.h>
#elif OS_WINDOWS
#include <windows.h>
#endif

#define New(arenaptr, type) arenaPush(arenaptr, sizeof(type), alignof(type))
#define Newarr(arenaptr, type, count) arenaPush(arenaptr, (count) * sizeof(type), alignof(type))

typedef struct {
  void *base;
  usize head;
  usize total_size;
} Arena;

inline fn void *forwardAlign(void *ptr, usize align);
inline fn bool isPowerOfTwo(usize value);

       fn Arena *arenaBuild(usize size, usize base_addr);
inline fn void arenaPop(Arena *arena, usize bytes);
inline fn bool arenaFree(Arena *arena);
       fn void *arenaPush(Arena *arena, usize size, usize align);

#endif
