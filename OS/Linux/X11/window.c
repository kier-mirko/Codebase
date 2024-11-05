#include <X11/Xlib.h>

#include "string.h"

/* TODO: this definition shouldn't be in here and i'm not sure about the name */
typedef struct {
  String8 viewport_name;
  size_t width;
  size_t height;

  Display *xdisplay;
  u32 xscreen_id;
  u32 xroot_id;
  u32 xwindow_id;
  u32 xatom_delete_id;
} Viewport;

fn Viewport viewport_create(String8 viewport_name,
			 size_t initial_width, size_t initial_height) {
  Viewport viewport = {.xdisplay = XOpenDisplay(0)};
  if (!viewport.xdisplay) {
    return viewport;
  }

  viewport.xscreen_id = XDefaultScreen(viewport.xdisplay);
  viewport.xroot_id = XDefaultRootWindow(viewport.xdisplay);
  viewport.xwindow_id = XCreateSimpleWindow(viewport.xdisplay, viewport.xroot_id, 0, 0,
					   initial_width, initial_height, 1, 0,
					   BlackPixel(viewport.xdisplay,
						      viewport.xscreen_id));

  XStoreName(viewport.xdisplay, viewport.xwindow_id, viewport_name.str);

  XMapWindow(viewport.xdisplay, viewport.xwindow_id);
  XSelectInput(viewport.xdisplay, viewport.xwindow_id,
	       ExposureMask |
	       KeyPressMask |
	       ButtonPressMask | PointerMotionMask);

  viewport.xatom_delete_id = XInternAtom(viewport.xdisplay, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(viewport.xdisplay, viewport.xwindow_id,
		  (Atom *)&viewport.xatom_delete_id, 1);

  return viewport;
}

/* TODO: This is temporary */
void viewport_echoKbdEvent(Viewport *viewport) {
  XEvent event = {0};

  if (XPending(viewport->xdisplay)) {
    XNextEvent(viewport->xdisplay, &event);

    switch (event.type) {
    case KeyPress: {
      u32 keycode = event.xkey.keycode;
      printf("Key press: %d\n", keycode);
    } break;
    case ButtonPress: {
      u32 keycode = event.xbutton.button;
      printf("Button press: %d\n", keycode);
    } break;
    case MotionNotify: {
      i32 x = event.xmotion.x;
      i32 y = event.xmotion.y;
      printf("Pointer motion: (%d, %d)\n", x, y);
    } break;
    case ClientMessage: {
      /* Make sure we aren't consuming the `window close` message */
      if (event.xclient.data.l[0] == viewport->xatom_delete_id) {
	XPutBackEvent(viewport->xdisplay, &event);
      }

      printf("Client message\n");
    } break;
    }
  }
}

inline fn bool viewport_shouldClose(Viewport *viewport) {
  XEvent event = {0};
  return XCheckTypedEvent(viewport->xdisplay, ClientMessage, &event) &&
	 event.xclient.data.l[0] == viewport->xatom_delete_id;
}

inline fn void viewport_close(Viewport *viewport) {
  (void)XCloseDisplay(viewport->xdisplay);
}
