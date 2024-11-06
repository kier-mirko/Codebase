#include <X11/Xlib.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include "string.h"

typedef struct {
  String8 viewport_name;
  size_t width;
  size_t height;

  Display *xdisplay;
  u32 xscreen_id;
  u32 xroot_id;
  u32 xwindow_id;

  union {
    u32 xatoms[13];

    struct {
      u32 xatom_close;

      u32 xatom_dndAware;
      u32 xatom_dndTypeList;
      u32 xatom_dndSelection;
      u32 xatom_dndEnter;
      u32 xatom_dndPosition;
      u32 xatom_dndDrop;
      u32 xatom_dndStatus;
      u32 xatom_dndLeave;
      u32 xatom_dndFinished;
      u32 xatom_dndActionCopy;
      u32 xatom_dndUriList;
      u32 xatom_dndPlainText;
    };
  };

  GLXContext glx_context;
} Viewport;

fn Viewport viewport_create(String8 viewport_name,
			    size_t initial_width, size_t initial_height) {
  Viewport viewport = {.xdisplay = XOpenDisplay(0)};
  if (!viewport.xdisplay) {
    return viewport;
  }

  viewport.xscreen_id = XDefaultScreen(viewport.xdisplay);
  viewport.xroot_id = XDefaultRootWindow(viewport.xdisplay);

  /* OpenGL stuff */
  GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  XVisualInfo *vi = glXChooseVisual(viewport.xdisplay, 0, att);
  if(vi == 0) {
    printf("\n\tno appropriate visual found\n\n");
    exit(0);
  }

  Colormap cmap = XCreateColormap(viewport.xdisplay, viewport.xroot_id,
				  vi->visual, AllocNone);
  XSetWindowAttributes swa = {.colormap = cmap,
                              .event_mask = ExposureMask |
					    KeyPressMask |
					    ButtonPressMask | PointerMotionMask};
  /* ============================================================================= */

  viewport.xwindow_id = XCreateWindow(viewport.xdisplay, viewport.xroot_id,
				      0, 0,
				      initial_width, initial_height,
				      0, vi->depth,
				      InputOutput, vi->visual,
				      CWColormap | CWEventMask,
				      &swa);

  /* Set the window name */
  XStoreName(viewport.xdisplay, viewport.xwindow_id, viewport_name.str);

  /* Display the window */
  XMapWindow(viewport.xdisplay, viewport.xwindow_id);

  viewport.xatom_close = XInternAtom(viewport.xdisplay, "WM_DELETE_WINDOW", False);

  viewport.xatom_dndAware = XInternAtom(viewport.xdisplay, "XdndAware", False);
  viewport.xatom_dndTypeList = XInternAtom(viewport.xdisplay, "XdndTypeList", False);
  viewport.xatom_dndSelection = XInternAtom(viewport.xdisplay, "XdndSelection", False);
  viewport.xatom_dndEnter = XInternAtom(viewport.xdisplay, "XdndEnter", False);
  viewport.xatom_dndPosition = XInternAtom(viewport.xdisplay, "XdndPosition", False);
  viewport.xatom_dndDrop = XInternAtom(viewport.xdisplay, "XdndDrop", False);
  viewport.xatom_dndStatus = XInternAtom(viewport.xdisplay, "XdndStatus", False);
  viewport.xatom_dndLeave = XInternAtom(viewport.xdisplay, "XdndLeave", False);
  viewport.xatom_dndFinished = XInternAtom(viewport.xdisplay, "XdndFinished", False);
  viewport.xatom_dndActionCopy = XInternAtom(viewport.xdisplay, "XdndActionCopy", False);
  viewport.xatom_dndUriList = XInternAtom(viewport.xdisplay, "text/uri-list", False);
  viewport.xatom_dndPlainText = XInternAtom(viewport.xdisplay, "text/plain", False);

  u8 xdnd_version = 5;
  XChangeProperty(viewport.xdisplay, viewport.xwindow_id,
		  viewport.xatom_dndAware, 4, 32,
		  PropModeReplace, &xdnd_version, 1);

  XSetWMProtocols(viewport.xdisplay, viewport.xwindow_id,
		  (Atom *)&viewport.xatom_close, 1);

  /* More OpenGL stuff */
  viewport.glx_context = glXCreateContext(viewport.xdisplay, vi, NULL, GL_TRUE);
  glXMakeCurrent(viewport.xdisplay, viewport.xwindow_id, viewport.glx_context);

  glEnable(GL_DEPTH_TEST);
  /* ============================================================================= */

  return viewport;
}

/* TODO: This is temporary */
void viewport_echoKbdEvent(Viewport *viewport, void (*on_expose)()) {
  XEvent event = {0};
  XWindowAttributes gwa = {0};

  if (XPending(viewport->xdisplay)) {
    XNextEvent(viewport->xdisplay, &event);

    switch (event.type) {
    case Expose: {
      XGetWindowAttributes(viewport->xdisplay, viewport->xwindow_id, &gwa);
      glViewport(0, 0, gwa.width, gwa.height);
      on_expose();
      glXSwapBuffers(viewport->xdisplay, viewport->xwindow_id);
    } break;
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
      if (event.xclient.data.l[0] == viewport->xatom_close) {
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
	 event.xclient.data.l[0] == viewport->xatom_close;
}

inline fn void viewport_close(Viewport *viewport) {
  glXMakeCurrent(viewport->xdisplay, None, NULL);
  glXDestroyContext(viewport->xdisplay, viewport->glx_context);
  XDestroyWindow(viewport->xdisplay, viewport->xwindow_id);
  (void)XCloseDisplay(viewport->xdisplay);
}
