#include "core/os_core.c"

#if OS_LINUX
#include "core/linux/os_core_linux.c"
#elif OS_BSD
#include "core/bsd/os_core_bsd.c"
#elif OS_WINDOWS
#include "core/win32/os_core_win32.c"
#else
#error os core layer not implemented
#endif
