#ifndef BASE_OS_LINUX_THREAD
#define BASE_OS_LINUX_THREAD

#include <stdio.h>
#include <pthread.h>

#if OS_LINUX
  typedef pthread_t Thread;
#elif OS_BSD
  typedef struct pthread Thread;
#endif

inline fn Thread os_thdSpawn(void *(*thread_main)(void *));
       fn Thread os_thdSpawnArgs(void *(*thread_main)(void *), void *arg_data);

inline fn void os_thdJoin(Thread id);
       fn void os_thdJoinReturn(Thread id, void **save_return_value_in);

#endif
