#ifndef OS_INC_H
#define OS_INC_H

#if OS_LINUX
#include "core/linux/os_core_linux.h"
#elif OS_BSD
#include "core/bsd/os_core_bsd.h"
#elif OS_WINDOWS
#include "core/win32/os_core_win32.h"
#else
#error OS core layer not implemented
#endif

#endif //OS_INC_H
