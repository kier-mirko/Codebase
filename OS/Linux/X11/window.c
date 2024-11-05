#include <X11/Xlib.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

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

  GLXContext glx_context;
} Viewport;

void DrawAQuad() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1., 1., -1., 1., 1., 20.);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

  glBegin(GL_QUADS);
  glColor3f(1., 0., 0.); glVertex3f(-.75, -.75, 0.);
  glColor3f(0., 1., 0.); glVertex3f( .75, -.75, 0.);
  glColor3f(0., 0., 1.); glVertex3f( .75,  .75, 0.);
  glColor3f(1., 1., 0.); glVertex3f(-.75,  .75, 0.);
  glEnd();
}

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

  /* Loop for WM_DELETE_WINDOW client msg for this viewport
   * so that i know how to exit gracefully */
  viewport.xatom_delete_id = XInternAtom(viewport.xdisplay, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(viewport.xdisplay, viewport.xwindow_id,
		  (Atom *)&viewport.xatom_delete_id, 1);

  /* More OpenGL stuff */
  viewport.glx_context = glXCreateContext(viewport.xdisplay, vi, NULL, GL_TRUE);
  glXMakeCurrent(viewport.xdisplay, viewport.xwindow_id, viewport.glx_context);

  glEnable(GL_DEPTH_TEST);
  /* ============================================================================= */

  return viewport;
}

/* TODO: This is temporary */
void viewport_echoKbdEvent(Viewport *viewport) {
  XEvent event = {0};
  XWindowAttributes gwa = {0};

  if (XPending(viewport->xdisplay)) {
    XNextEvent(viewport->xdisplay, &event);

    switch (event.type) {
    case Expose: {
      XGetWindowAttributes(viewport->xdisplay, viewport->xwindow_id, &gwa);
      glViewport(0, 0, gwa.width, gwa.height);
      DrawAQuad();
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
  glXMakeCurrent(viewport->xdisplay, None, NULL);
  glXDestroyContext(viewport->xdisplay, viewport->glx_context);
  XDestroyWindow(viewport->xdisplay, viewport->xwindow_id);
  (void)XCloseDisplay(viewport->xdisplay);
}
