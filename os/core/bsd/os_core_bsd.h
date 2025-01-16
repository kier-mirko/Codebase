#ifndef OS_CORE_BSD_H
#define OS_CORE_BSD_H

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <pthread.h>


typedef pthread_t Thread;

inline Thread os_bsd_thread_spawn(void *(*thread_main)(void *));
fn Thread os_bsd_thread_spawn_args(void *(*thread_main)(void *), void *arg_data);

inline void os_bsd_thread_join(Thread id);
fn void os_bsd_thread_joinReturn(Thread id, void **save_return_value_in);

#endif //OS_CORE_BSD_H
