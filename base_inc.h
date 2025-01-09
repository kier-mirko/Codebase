#ifndef BASE_INC_H
#define BASE_INC_H

#include "base.h"
#include "memory.h"
#include "arena.h"
#include "string.h"
#include "image.h"

#include "time/chrono.h"
#include "time/clock.h"

#include "math/calculus.h"
#if CPP
  #include "math/vector.hpp"
  #include "math/matrix.hpp"
  #include "math/complex.hpp"
  #include "math/simplex.hpp"
#endif

#include "DataStructure/list.h"
#if CPP
  #include "DataStructure/array.hpp"
  #include "DataStructure/dynarray.hpp"
  #include "DataStructure/hashmap.hpp"
#endif

#include "OS/dynlib.h"
#include "OS/file_properties.h"
#include "OS/file.h"

#if OS_LINUX
  #include "OS/Linux/thread.h"
  #include "OS/Linux/net.h"

  #if GFX_X11 || GFX_WAYLAND
  #include "OS/Linux/opengl.h"
  #endif

  #if GFX_X11
    #include "OS/Linux/X11/window.h"
  #elif GFX_WAYLAND
    #include "OS/Linux/Wayland/window.h"
  #endif
#elif OS_BSD
  #include "OS/BSD/thread.h"

  #if GFX_X11 || GFX_WAYLAND
  #include "OS/BSD/opengl.h"
  #endif

  #if GFX_X11
    #include "OS/BSD/X11/window.h"
  #elif GFX_WAYLAND
    #include "OS/BSD/Wayland/window.h"
  #endif
#elif OS_WINDOWS
  #include "OS/Windows/thread.h"
  #include "OS/Windows/D3D/window.h"
#endif

#include "serializer/csv.h"

#if CPP
  #include "AI/decision_tree.h"
#endif

#endif
