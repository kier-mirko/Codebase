inline B32 
is_power_of_two(USZ value) 
{
  return !(value & (value - 1));
}

inline void *
forward_align(void *ptr, USZ align) 
{
  Assert(is_power_of_two(align));
  
  USZ mod = (USZ)ptr & (align - 1);
  return (mod ? ptr = (U8 *)ptr + align - mod
          : ptr);
}

fn Arena *
arena_alloc(USZ size, USZ base) 
{
#if OS_LINUX || OS_BSD
  void *fail_state = MAP_FAILED;
  void *mem = mmap((void *)base, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#elif OS_WINDOWS
  void *fail_state = 0;
  void *mem = VirtualAlloc((void *)base, size,
                           MEM_COMMIT | MEM_RESERVE,
                           PAGE_READWRITE);
#endif
  
  if (mem == fail_state) 
  {
    return 0;
  } 
  else 
  {
    Arena *arena = (Arena *)mem;
    arena->base = arena + sizeof(Arena);
    arena->total_size = size - sizeof(Arena);
    arena->head = 0;
    
    return arena;
  }
}

inline void 
arena_pop(Arena *arena, USZ bytes) 
{
  arena->head = ClampBot((ISZ)arena->head - (ISZ)bytes, 0);
}

inline B32 
arena_release(Arena *arena) 
{
#if OS_LINUX || OS_BSD
  return munmap(arena->base, arena->total_size + sizeof(Arena));
#elif OS_WINDOWS
  return VirtualFree(arena->base, 0, MEM_RELEASE);
#else
  return 0;
#endif
}

fn void *
arena_push(Arena *arena, USZ size, USZ align) 
{
  if (!align) {align = DefaultAlignment;}
  void *aligned_head = forward_align((U8 *)arena->base + arena->head, align);
  USZ offset = (U8 *)aligned_head - (U8 *)arena->base;
  
  if ((U8 *)aligned_head + size > (U8 *)arena->base + arena->total_size) {
    return 0;
  }
  
  void *res = aligned_head;
  arena->head = offset + size;
  
  memset(res, 0, size);
  return res;
}

fn Temp
temp_begin(Arena *arena)
{
  Temp result = {arena, arena->head};
  return result;
}

fn void
temp_end(Temp temp)
{
  temp.arena->head = temp.pos;
}
