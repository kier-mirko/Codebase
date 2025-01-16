#ifndef BASE_ARENA
#define BASE_ARENA

#define make(...) makex(__VA_ARGS__,make3,make2)(__VA_ARGS__)
#define makex(a,b,c,d,...) d
#define make2(arenaptr, type) (type*)arena_push(arenaptr, sizeof(type), alignof(type))
#define make3(arenaptr, type, count) (type*)arena_push(arenaptr, (count) * sizeof(type), alignof(type))

typedef struct Arena Arena;
struct Arena {
  void *base;
  USZ head;
  USZ total_size;
};

typedef struct Temp Temp;
struct Temp
{
  Arena *arena;
  U64 pos;
};

inline void *forward_align(void *ptr, USZ align);
inline B32 is_power_of_two(USZ value);

fn Arena *arena_alloc(USZ size, USZ base_addr);
inline void arena_pop(Arena *arena, USZ bytes);
inline B32 arena_release(Arena *arena);
fn void *arena_push(Arena *arena, USZ size, USZ align);

fn Temp temp_begin(Arena *arena);
fn void temp_end(Temp temp);

#endif
