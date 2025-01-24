#ifndef BASE_OS_LINUX_THREAD
#define BASE_OS_LINUX_THREAD

#include <stdio.h>

#if OS_LINUX || OS_BSD
#include <pthread.h>
typedef pthread_t Thread;
#endif

typedef void (*thd_fn)(void *);

fn Thread os_thdSpawn(thd_fn thread_main, void *args);
inline fn void os_thdJoin(Thread id, void **return_buff);

#endif
