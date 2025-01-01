#include "thread.h"

inline fn Thread threadSpawn(void *(*thread_main)(void *)) {
  return threadSpawnArgs(thread_main, 0);
}

fn Thread threadSpawnArgs(void *(*thread_main)(void *), void *arg_data) {
  Assert(thread_main);

  Thread thread_id;
  pthread_create(&thread_id, 0, thread_main, arg_data);

  return thread_id;
}

inline fn void threadJoin(Thread id) {
  threadJoinReturn(id, 0);
}

fn void threadJoinReturn(Thread id, void **save_return_value_in) {
  (void)pthread_join(id, save_return_value_in);
}
