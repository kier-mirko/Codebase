thread_local TCTX tctx_thread_local;

Arena *tctx_get_scratch(Arena **conflicts, usize count) {
  if(tctx_thread_local.arenas[0] == 0) {
    for(usize i = 0; i < Arrsize(tctx_thread_local.arenas); ++i) {
      tctx_thread_local->arenas[i] = arenaBuild(MB(64),0);
    }
  }
  
  Arena *result = 0;
  for(usize i = 0; i < Arrsize(tctx_thread_local.arenas); ++i) {
    bool has_conflict = false;
    for(usize j = 0; j < count; ++j) {
      if(tctx_thread_local.arenas[i] == conflicts[j]) {
        has_conflict = true;
        break
      }
    }
    if(!has_conflict) {
      result = tctx_thread_local.arenas[i];
      break;
    }
  }
  
  return result
}
