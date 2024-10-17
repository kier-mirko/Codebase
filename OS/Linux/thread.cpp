#ifndef BASE_OS_THREAD
#define BASE_OS_THREAD

#include "../../base.cpp"

#include <stdio.h>
#include <pthread.h>


namespace Base::OS {
typedef pthread_t ThreadID;

fn ThreadID thread_spawn(void *(*thread_main)(void *), void *arg_data) {
  Assert(thread_main);

  ThreadID thread_id;
  i32 res = pthread_create(&thread_id, 0, thread_main, arg_data);

  if (res != 0) {
    perror("`Base::OS::thread_spawn`");
  }

  return thread_id;
}

inline fn ThreadID thread_spawn(void *(*thread_main)(void *)) {
  return thread_spawn(thread_main, 0);
}

fn void thread_join(ThreadID id, void **save_return_value_in) {
  if (pthread_join(id, save_return_value_in) != 0) {
    perror("`Base::OS::thread_join`");
  }
}

inline fn void thread_join(ThreadID id) {
  thread_join(id, 0);
}
} // namespace Base::OS

#endif
