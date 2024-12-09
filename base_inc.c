#include "arena.c"
#include "memory.c"
#include "clock.c"
#include "string.c"

#if OS_LINUX
  #include "OS/Linux/file.c"
  #include "OS/Linux/dynlib.c"
  #include "OS/Linux/thread.c"

  #if GFX_X11
    #include "OS/Linux/X11/window.c"
  #elif GFX_WAYLAND
    #include "OS/Linux/Wayland/window.c"
  #endif
#elif OS_BSD
  #include "OS/BSD/file.c"
  #include "OS/BSD/dynlib.c"
  #include "OS/BSD/thread.c"

  #if GFX_X11
    #include "OS/BSD/X11/window.c"
  #elif GFX_WAYLAND
    #include "OS/BSD/Wayland/window.c"
  #endif
#elif OS_WINDOWS
  #include "OS/Windows/file.c"
  #include "OS/Windows/dynlib.c"
  #include "OS/Windows/thread.c"

  #include "OS/Windows/D3D/window.c"
#endif

#include "serializer/csv.c"

#if CPP
  #include "AI/decision_tree.cpp"
#endif
