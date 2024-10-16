#ifndef BASE_OS_THREAD
#define BASE_OS_THREAD

#include "../../base.cpp"

#include <stdio.h>
#include <pthread.h>

#define thread_local __thread

typedef pthread_t thread_id;

namespace Base::OS {
fn thread_id spawn_thread(void *(*thread_main)(void *), void *arg_data) {
  Assert(thread_main);

  thread_id thread_id;
  pthread_create(&thread_id, 0, thread_main, arg_data);

  if (thread_id < 0) {
    perror("`Base::OS::spawn_thread`");
  }

  return thread_id;
}

inline fn thread_id spawn_thread(void *(*thread_main)(void *)) {
  return spawn_thread(thread_main, 0);
}
} // namespace Base::OS

#endif
