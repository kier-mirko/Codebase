inline fn Thread os_thdSpawn(void *(*thread_main)(void *)) {
  return os_thdSpawnArgs(thread_main, 0);
}

fn Thread os_thdSpawnArgs(void *(*thread_main)(void *), void *arg_data) {
  Assert(thread_main);

  Thread thread_id;
  pthread_create(&thread_id, 0, thread_main, arg_data);

  return thread_id;
}

inline fn void os_thdJoin(Thread id) {
  os_thdJoinReturn(id, 0);
}

fn void os_thdJoinReturn(Thread id, void **save_return_value_in) {
  (void)pthread_join(id, save_return_value_in);
}
