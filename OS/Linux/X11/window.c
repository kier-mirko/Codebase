#include "string.h"
#include "window.h"

fn Viewport viewport_create(String8 name,
			    usize initial_width, usize initial_height) {
  local GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  local u8 xdnd_version = 5;

  Viewport viewport = {.xdisplay = XOpenDisplay(0)};
  if (!viewport.xdisplay) {
    return viewport;
  }

  viewport.name = name;
  viewport.width = initial_width;
  viewport.height = initial_height;

  viewport.xscreen = XDefaultScreen(viewport.xdisplay);
  viewport.xroot = XDefaultRootWindow(viewport.xdisplay);

  /* OpenGL stuff */
  XVisualInfo *vi = glXChooseVisual(viewport.xdisplay, 0, att);
  if(!vi) {
    return (Viewport){0};
  }

  Colormap cmap;
  if (!(cmap = XCreateColormap(viewport.xdisplay, viewport.xroot,
			       vi->visual, AllocNone))) {
    return (Viewport){0};
  }

  XSetWindowAttributes swa = {.event_mask = ExposureMask |
					     KeyPressMask |
					     ButtonPressMask |
					     PointerMotionMask,
                              .colormap = cmap };
  /* ============================================================================= */

  viewport.xwindow = XCreateWindow(viewport.xdisplay, viewport.xroot,
				      0, 0,
				      initial_width, initial_height,
				      0, vi->depth,
				      InputOutput, vi->visual,
				      CWColormap | CWEventMask,
				      &swa);

  /* Set the window name */
  XStoreName(viewport.xdisplay, viewport.xwindow, (char *)name.str);

  /* Display the window */
  if (!XMapWindow(viewport.xdisplay, viewport.xwindow)) {
    return (Viewport){0};
  }

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

  if (!XChangeProperty(viewport.xdisplay, viewport.xwindow,
		       viewport.xatom_dndAware, 4, 32,
		       PropModeReplace, &xdnd_version, 1)) {
    return (Viewport){0};
  }

  if (!XSetWMProtocols(viewport.xdisplay, viewport.xwindow,
		       (Atom *)&viewport.xatom_close, 1)) {
    return (Viewport){0};
  }

  /* More OpenGL stuff */
  viewport.glx_context = glXCreateContext(viewport.xdisplay, vi, NULL, GL_TRUE);
  if (!glXMakeCurrent(viewport.xdisplay, viewport.xwindow, viewport.glx_context)) {
    return (Viewport){0};
  }

  glEnable(GL_DEPTH_TEST);
  /* ============================================================================= */

  return viewport;
}

inline fn void viewport_swapBuffers(Viewport *viewport) {
  glXSwapBuffers(viewport->xdisplay, viewport->xwindow);
}

ViewportEvent viewport_getNextEvent(Viewport *viewport) {
  XEvent event = {0};
  XWindowAttributes gwa = {0};
  ViewportEvent res = {};

  if (XPending(viewport->xdisplay)) {
    XNextEvent(viewport->xdisplay, &event);

    switch (event.type) {
    case Expose: {
      res.type = SHOW;

      XGetWindowAttributes(viewport->xdisplay, viewport->xwindow, &gwa);
      glViewport(0, 0, gwa.width, gwa.height);
    } break;
    case KeyPress: {
      res.type = KBD_PRESS;
      res.kbd.modifiers = event.xkey.state;
      res.kbd.key =
	codepointFromKeySym(XLookupKeysym(&event.xkey,
					  Xor(ShiftMod(res.kbd.modifiers),
					      CapsLockMod(res.kbd.modifiers))));
    } break;
    case ButtonPress: {
      res.mouse.modifiers = event.xbutton.state;
      res.mouse.kind = (ViewportMouseBtnType)event.xbutton.button;
      res.mouse.x = event.xbutton.x;
      res.mouse.y = event.xbutton.y <= viewport->height
		    ? viewport->height - event.xbutton.y
		    : 0;
    } break;
    case MotionNotify: {
      res.type = PTR_MOTION;
      res.motion.x = ClampBot(event.xmotion.x, 0);
      res.motion.y = event.xmotion.y <= viewport->height
		     ? viewport->height - event.xmotion.y
		     : 0;
    } break;
    case ClientMessage: {
      /* Make sure we aren't consuming the `window close` message */
      if (event.xclient.data.l[0] == viewport->xatom_close) {
	XPutBackEvent(viewport->xdisplay, &event);
      }

      if (!viewport->xatom_close) {
	printf("close: %d\n", !viewport->xatom_close);
      } else if (!viewport->xatom_dndAware) {
	printf("dndAware: %d\n", !viewport->xatom_dndAware);
      } else if (!viewport->xatom_dndTypeList) {
	printf("dndTypeList: %d\n", viewport->xatom_dndTypeList);
      } else if (!viewport->xatom_dndSelection) {
	printf("dndSelection: %d\n", viewport->xatom_dndSelection);
      } else if (!viewport->xatom_dndEnter) {
	printf("dndEnter: %d\n", viewport->xatom_dndEnter);
      } else if (!viewport->xatom_dndPosition) {
	printf("dndPosition: %d\n", viewport->xatom_dndPosition);
      } else if (!viewport->xatom_dndDrop) {
	printf("dndDrop: %d\n", viewport->xatom_dndDrop);
      } else if (!viewport->xatom_dndStatus) {
	printf("dndStatus: %d\n", viewport->xatom_dndStatus);
      } else if (!viewport->xatom_dndLeave) {
	printf("dndLeave: %d\n", viewport->xatom_dndLeave);
      } else if (!viewport->xatom_dndFinished) {
	printf("dndFinished: %d\n", viewport->xatom_dndFinished);
      } else if (!viewport->xatom_dndActionCopy) {
	printf("dndActionCopy: %d\n", viewport->xatom_dndActionCopy);
      } else if (!viewport->xatom_dndUriList) {
	printf("dndUriList: %d\n", viewport->xatom_dndUriList);
      } else if (!viewport->xatom_dndPlainText) {
	printf("dndPlainText: %d\n", viewport->xatom_dndPlainText);
      }
    } break;
    }
  }

  return res;
}

fn bool viewport_shouldClose(Viewport *viewport) {
  Assert(viewport && viewport->name.str);

  XEvent event = {0};
  if (XCheckTypedEvent(viewport->xdisplay, ClientMessage, &event)) {
    if (event.xclient.data.l[0] == viewport->xatom_close) {
      return true;
    } else {
      XPutBackEvent(viewport->xdisplay, &event);
    }
  }

  return false;
}

inline fn void viewport_close(Viewport *viewport) {
  Assert(viewport && viewport->name.str);

  (void)glXMakeCurrent(viewport->xdisplay, None, NULL);
  (void)glXDestroyContext(viewport->xdisplay, viewport->glx_context);
  (void)XDestroyWindow(viewport->xdisplay, viewport->xwindow);
  (void)XCloseDisplay(viewport->xdisplay);
}

fn Codepoint codepointFromKeySym(KeySym sym) {
  i32 min = 0;
  i32 max = Arrsize(keysymtab);
  i32 mid;

  /* first check for Latin-1 characters (1:1 mapping) */
  if ((sym >= 0x0020 && sym <= 0x007e) ||
      (sym >= 0x00a0 && sym <= 0x00ff))
    return (Codepoint) {
      .codepoint = (u32)sym,
      .size = 1,
    };

  /* also check for directly encoded 24-bit UCS characters */
  if ((sym & 0xff000000) == 0x01000000)
    return (Codepoint) {
      .codepoint = (u32)sym & 0x00ffffff,
      .size = 3,
    };

  /* binary search in table */
  while (max >= min) {
    mid = (min + max) / 2;
    if (keysymtab[mid].keysym < sym)
      min = mid + 1;
    else if (keysymtab[mid].keysym > sym)
      max = mid - 1;
    else {
      return (Codepoint) {
	.codepoint = keysymtab[mid].ucs,
	.size = 4,
      };
    }
  }

  /* no matching Codepoint value found */
  return (Codepoint) {0};
}
