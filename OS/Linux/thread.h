#pragma once

#include "../../base.h"

#include <stdio.h>
#include <pthread.h>

typedef pthread_t Thread;

inline fn Thread threadSpawn(void *(*thread_main)(void *));
       fn Thread threadSpawnArgs(void *(*thread_main)(void *), void *arg_data);

inline fn void threadJoin(Thread id);
       fn void threadJoinReturn(Thread id, void **save_return_value_in);
