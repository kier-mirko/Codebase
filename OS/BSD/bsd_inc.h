#ifndef OS_BSD_INC_H
#define OS_BSD_INC_H

#include "thread.h"

#if GFX_X11 || GFX_WAYLAND
#include "opengl.h"
#endif

#if GFX_X11
#include "X11/window.h"
#elif GFX_WAYLAND
#include "Wayland/window.h"
#endif

#endif
