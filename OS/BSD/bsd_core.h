#ifndef OS_BSD_CORE_H
#define OS_BSD_CORE_H

#include <sys/sysinfo.h>

typedef struct {
  ThreadFunc *func;
  void *args;
} bsd_thdData;

#ifndef MEMFILES_ALLOWED
#  define MEMFILES_ALLOWED 234414
#endif

#endif
