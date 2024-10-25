#pragma once

#include "../../base.h"

#include <pthread.h>
#include <stdio.h>

namespace Base::OS {
typedef struct pthread Thread;

inline fn Thread *thread_spawn(void *(*thread_main)(void *));
       fn Thread *thread_spawn(void *(*thread_main)(void *), void *arg_data);

inline fn void thread_join(Thread *tcb);
       fn void thread_join(Thread *tcb, void **save_return_value_in);
} // namespace Base::OS
