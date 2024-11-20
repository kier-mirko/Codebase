#pragma once

#include "base.h"

#if OS_LINUX || OS_BSD
#include <sys/mman.h>
#elif OS_WINDOWS
#include <windows.h>
#endif

#define New(arenaptr, type) (type *)arenaPush(arenaptr, sizeof(type), alignof(type))
#define Newarr(arenaptr, type, count) (type *)arenaPush(arenaptr, (count) * sizeof(type), alignof(type))

typedef struct {
  void *base_addr;
  void *head;
  usize total_size;
} Arena;

       fn Arena *arenaBuild(usize size, void *base_addr);
inline fn void arenaPop(Arena *arena, usize bytes);
inline fn void arenaReset(Arena *arena);
inline fn bool arenaFree(Arena *arena);
       fn void *arenaPush(Arena *arena, usize size, usize align);
