#ifndef OS_LINUX_CORE_H
#define OS_LINUX_CORE_H

typedef struct {
  Arena *arena;
  OS_SystemInfo info;
} LNX_State;

typedef struct {
  ThreadFunc *func;
  void *args;
} lnx_thdData;

fn void* lnx_thdEntry(void *args);

fn String8 lnx_getHostname();

#endif
