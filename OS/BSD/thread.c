#include "thread.h"

inline fn Thread *threadSpawn(void *(*thread_main)(void *)) {
  return threadSpawnArgs(thread_main, 0);
}

fn Thread *threadSpawnArgs(void *(*thread_main)(void *), void *arg_data) {
  Assert(thread_main);

  Thread *thread;
  i32 res = pthread_create(&thread, 0, thread_main, arg_data);

  if (!thread || res != 0) {
    perror("`Base::OS::spawn_thread`");
  }

  return thread;
}

inline fn void threadJoin(Thread *tcb) {
  threadJoinReturn(tcb, 0);
}

fn void threadJoinReturn(Thread *tcb, void **save_return_value_in) {
  (void)pthread_join(tcb, save_return_value_in);
}
