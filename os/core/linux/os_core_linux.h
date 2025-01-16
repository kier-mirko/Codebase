#ifndef OS_CORE_LINUX_H
#define OS_CORE_LINUX_H

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/types.h>
#include <pthread.h>
#include <dlfcn.h>


typedef pthread_t Thread;

inline Thread os_lnx_thread_spawn(void *(*thread_main)(void *));
fn Thread os_lnx_thread_spawn_args(void *(*thread_main)(void *), void *arg_data);

inline void os_lnx_thread_join(Thread id);
fn void os_lnx_thread_join_return(Thread id, void **save_return_value_in);

#endif //OS_CORE_LINUX_H
