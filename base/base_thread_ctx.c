thread_static TCTX tctx_thread_local;

fn Arena *
tctx_get_scratch(Arena **conflicts, U64 count)
{
  if(tctx_thread_local.arenas[0] == 0)
  {
    for(U64 i = 0; i < Arrsize(tctx_thread_local.arenas); ++i)
    {
      tctx_thread_local.arenas[i] = arena_alloc(MB(64),0);
    }
  }
  
  Arena *result = 0;
  Arena **arena_ptr = tctx_thread_local.arenas;
  for (U64 i = 0; i < Arrsize(tctx_thread_local.arenas); ++i, arena_ptr += 1){
    B32 has_conflict = 0;
    for (U64 j = 0; j < count; ++j){
      if (*arena_ptr == conflicts[j]){
        has_conflict = 1;
        break;
      }
    }
    if (!has_conflict){
      result = *arena_ptr;
      break;
    }
  }
  
  return result;
}
