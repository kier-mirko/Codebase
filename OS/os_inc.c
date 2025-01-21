#if OS_LINUX
  #include "Linux/dynlib.c"
  #include "Linux/file.c"
  #include "Linux/thread.c"

  #include "Linux/X11/window.c"
#elif OS_BSD
  #include "BSD/dynlib.c"
  #include "BSD/file.c"
  #include "BSD/thread.c"

  #include "BSD/X11/window.c"
#elif OS_WINDOWS
  #include "Windows/win32_file.c"
#endif
