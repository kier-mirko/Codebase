#ifndef BASE_OS_BSD_THREAD
#define BASE_OS_BSD_THREAD

#include "../../base.h"

#include <pthread.h>
#include <stdio.h>

typedef struct pthread Thread;

inline fn Thread *threadSpawn(void *(*thread_main)(void *));
       fn Thread *threadSpawnArgs(void *(*thread_main)(void *), void *arg_data);

inline fn void threadJoin(Thread *tcb);
       fn void threadJoinReturn(Thread *tcb, void **save_return_value_in);

#endif
