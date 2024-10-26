#pragma once

#include "base.h"

#if OS_LINUX || OS_BSD
#include <sys/mman.h>
#elif OS_WINDOWS
#include <windows.h>
#endif

#define Make(arenaptr, type) (type *)arenaMake(arenaptr, sizeof(type))
#define Makearr(arenaptr, type, count) (type *)arenaMake(arenaptr, (count) * sizeof(type))

typedef struct {
  void *base_addr;
  void *head;
  size_t total_size;
} Arena;

       fn Arena *arenaBuild(size_t size, void *base_addr);
inline fn void arenaPop(Arena *arena, size_t bytes);
inline fn bool arenaFree(Arena *arena);
       fn void *arenaMake(Arena *arena, size_t size);
