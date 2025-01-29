#if OS_LINUX
#  include "Linux/linux_core.c"
#  include "Linux/file.c"
#  include "Linux/X11/window.c"
#elif OS_BSD
#  include "BSD/bsd_core.c"
#  include "BSD/file.c"
#  include "BSD/X11/window.c"
#elif OS_WINDOWS
#  include "Win32/os_core_win32.c"
#endif
