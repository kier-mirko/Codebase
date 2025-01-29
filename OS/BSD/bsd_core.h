#ifndef OS_BSD_CORE_H
#define OS_BSD_CORE_H

#ifndef MEMFILES_ALLOWED
#  define MEMFILES_ALLOWED 234414
#endif

typedef struct {
  ThreadFunc *func;
  void *args;
} bsd_thdData;

typedef struct {
  Arena *arena;
  OS_SystemInfo info;
  String8 filemap[MEMFILES_ALLOWED];
} BSD_State;

#endif
