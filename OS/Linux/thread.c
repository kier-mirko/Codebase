#ifndef BASE_OS_THREAD
#define BASE_OS_THREAD

#include "thread.h"

inline fn ThreadID threadSpawn(void *(*thread_main)(void *)) {
  return threadSpawnArgs(thread_main, 0);
}

fn ThreadID threadSpawnArgs(void *(*thread_main)(void *), void *arg_data) {
  Assert(thread_main);

  ThreadID thread_id;
  i32 res = pthread_create(&thread_id, 0, thread_main, arg_data);

  return thread_id;
}

inline fn void threadJoin(ThreadID id) {
  threadJoinReturn(id, 0);
}

fn void threadJoinReturn(ThreadID id, void **save_return_value_in) {
  (void)pthread_join(id, save_return_value_in);
}

#endif
