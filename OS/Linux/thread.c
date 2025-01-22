typedef struct {
  thd_fn func;
  void *args;
} os_thdData;

fn void* os_thdEntry(void *args) {
  os_thdData *wrap_args = (os_thdData *)args;
  thd_fn func = (thd_fn)wrap_args->func;
  func(wrap_args->args);
  return 0;
}

fn Thread os_thdSpawn(thd_fn thread_main, void *args) {
  Assert(thread_main);

  /* NOTE(lb): i'm pretty sure this isn't the right approach but
     i don't want this function to receive an arena and i don't have a way
     to ask the os to allocate memory that isn't inside an arena right now.
     Another idea would be to have a global arena that is used only for these
     OS functions idk. */
  Scratch scr = ScratchBegin(0, 0);
  os_thdData *wrap_args = New(scr.arena, os_thdData);
  wrap_args->func = thread_main;
  wrap_args->args = args;

  Thread thread_id;
  i32 maybeErr = pthread_create(&thread_id, 0, os_thdEntry, wrap_args);
  ScratchEnd(scr);
  return (maybeErr == 0 ? thread_id : maybeErr);
}

fn void os_thdJoin(Thread id, void **return_buff) {
  (void)pthread_join(id, return_buff);
}
