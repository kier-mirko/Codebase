#pragma once

#include "../../base.h"

#include <stdio.h>
#include <pthread.h>

namespace OS {
typedef pthread_t ThreadID;

inline fn ThreadID threadSpawn(void *(*thread_main)(void *));
       fn ThreadID threadSpawn(void *(*thread_main)(void *), void *arg_data);

inline fn void threadJoin(ThreadID id);
       fn void threadJoin(ThreadID id, void **save_return_value_in);
}
