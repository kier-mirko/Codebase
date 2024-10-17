#ifndef BASE_OS_THREAD
#define BASE_OS_THREAD

#include "../../base.cpp"

#include <pthread.h>
#include <stdio.h>

namespace Base::OS {
typedef struct pthread Thread;

fn Thread *thread_spawn(void *(*thread_main)(void *), void *arg_data) {
  Assert(thread_main);

  Thread *thread;
  i32 res = pthread_create(&thread, 0, thread_main, arg_data);

  if (!thread || res != 0) {
    perror("`Base::OS::spawn_thread`");
  }

  return thread;
}

inline fn Thread *thread_spawn(void *(*thread_main)(void *)) {
  return thread_spawn(thread_main, 0);
}

fn void thread_join(Thread *tcb, void **save_return_value_in) {
  if (pthread_join(tcb, save_return_value_in) != 0) {
    perror("`Base::OS::thread_join`");
  }
}

inline fn void thread_join(Thread *tcb) {
  thread_join(tcb, 0);
}
} // namespace Base::OS

#endif
