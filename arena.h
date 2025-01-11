#ifndef BASE_ARENA
#define BASE_ARENA

#include "base.h"

#if OS_LINUX || OS_BSD
#include <sys/mman.h>
#elif OS_WINDOWS
#include <windows.h>
#endif

#define make(...) makex(__VA_ARGS__,make3,make2)(__VA_ARGS__)
#define makex(a,b,c,d,...) d
#define make2(a, t)    (t *)arenaPush(a, sizeof(t), alignof(t))
#define make3(a, t, n) (t *)arenaPush(a, sizeof(t) * (n), alignof(t))

#define New(arenaptr, type) (type*)arenaPush(arenaptr, sizeof(type), alignof(type))
#define Newarr(arenaptr, type, count) (type*)arenaPush(arenaptr, (count) * sizeof(type), alignof(type))

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
