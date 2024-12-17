#include "string.h"
#include "window.h"
#include "../../../image.h"

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
    printf("\tglXChooseVisual\n");
    return (Viewport){0};
  }

  Colormap cmap;
  if (!(cmap = XCreateColormap(viewport.xdisplay, viewport.xroot,
			       vi->visual, AllocNone))) {
    printf("\tXCreateColormap\n");
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
    printf("\tXMapWindow\n");
    return (Viewport){0};
  }

  viewport.xatom_close = XInternAtom(viewport.xdisplay, "WM_DELETE_WINDOW", False);

  viewport.xatom_dndTypeList = XInternAtom(viewport.xdisplay, "XdndTypeList", False);
  viewport.xatom_dndSelection = XInternAtom(viewport.xdisplay, "XdndSelection", False);

  viewport.xatom_dndEnter = XInternAtom(viewport.xdisplay, "XdndEnter", False);
  viewport.xatom_dndPosition = XInternAtom(viewport.xdisplay, "XdndPosition", False);
  viewport.xatom_dndStatus = XInternAtom(viewport.xdisplay, "XdndStatus", False);
  viewport.xatom_dndLeave = XInternAtom(viewport.xdisplay, "XdndLeave", False);
  viewport.xatom_dndDrop = XInternAtom(viewport.xdisplay, "XdndDrop", False);
  viewport.xatom_dndFinished = XInternAtom(viewport.xdisplay, "XdndFinished", False);

  viewport.xatom_dndActionCopy = XInternAtom(viewport.xdisplay, "XdndActionCopy", False);

  viewport.xatom_dndUriList = XInternAtom(viewport.xdisplay, "text/uri-list", False);
  viewport.xatom_dndPlainText = XInternAtom(viewport.xdisplay, "text/plain", False);

  viewport.xatom_dndAware = XInternAtom(viewport.xdisplay, "XdndAware", False);

  if (!XChangeProperty(viewport.xdisplay, viewport.xwindow,
		       viewport.xatom_dndAware, 4, 32,
		       PropModeReplace, &xdnd_version, 1)) {
    printf("\tXChangeProperty\n");
    return (Viewport){0};
  }

  if (!XSetWMProtocols(viewport.xdisplay, viewport.xwindow,
		       (Atom *)&viewport.xatom_close, 1)) {
    printf("\tXSetWMProtocols\n");
    return (Viewport){0};
  }

  /* More OpenGL stuff */
  viewport.glx_context = glXCreateContext(viewport.xdisplay, vi, NULL, GL_TRUE);
  if (!glXMakeCurrent(viewport.xdisplay, viewport.xwindow, viewport.glx_context)) {
    printf("\tglXMakeCurrent\n");
    return (Viewport){0};
  }

  glEnable(GL_DEPTH_TEST);
  /* ============================================================================= */

  return viewport;
}

inline fn void viewport_close(Viewport *viewport) {
  Assert(viewport && viewport->name.str);

  (void)glXMakeCurrent(viewport->xdisplay, None, NULL);
  (void)glXDestroyContext(viewport->xdisplay, viewport->glx_context);
  (void)XDestroyWindow(viewport->xdisplay, viewport->xwindow);
  (void)XCloseDisplay(viewport->xdisplay);
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

fn bool viewport_setWindowIcon(Arena *arena, Viewport *viewport, String8 path) {
  usize head = arena->head;
  i32 width, height, componentXpixel;
  u8 *imgdata = loadImg(path, &width, &height, &componentXpixel);
  if (!imgdata) { return false; }

  i32 size = 2 + width * height;
  u64 *data = (u64 *) Newarr(arena, u64, size);
  if (!data) { return false; }
  data[0] = width;
  data[1] = height;

  for (usize i = 0; i < size - 2; ++i) {
    data[i + 2] = (imgdata[i * 4 + 3] << 24) |
		  (imgdata[i * 4 + 0] << 16) |
		  (imgdata[i * 4 + 1] << 8) |
		  (imgdata[i * 4 + 2]);
  }

  Atom net_wm_icon = XInternAtom(viewport->xdisplay, "_NET_WM_ICON", False);
  Atom cardinal = XInternAtom(viewport->xdisplay, "CARDINAL", False);
  XChangeProperty(viewport->xdisplay, viewport->xwindow, net_wm_icon, cardinal,
		  32, PropModeReplace, (u8 *)data, size);

  arena->head = head;
  destroyImg(imgdata);
  return true;
}

fn void viewport_setWindowTitle(Viewport *viewport, String8 title) {
  XStoreName(viewport->xdisplay, viewport->xwindow, (char *)title.str);
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
      printf("Motion\n");
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

      if (event.xclient.message_type == viewport->xatom_close) {
	printf("dndClose\n");
      } else if (event.xclient.message_type == viewport->xatom_dndAware) {
	printf("dndAware\n");
      } else if (event.xclient.message_type == viewport->xatom_dndTypeList) {
	printf("dndTypeList\n");
      } else if (event.xclient.message_type == viewport->xatom_dndSelection) {
	printf("dndSelection\n");
      } else if (event.xclient.message_type == viewport->xatom_dndEnter) {
	printf("dndEnter\n");
      } else if (event.xclient.message_type == viewport->xatom_dndPosition) {
	printf("dndPosition\n");
      } else if (event.xclient.message_type == viewport->xatom_dndDrop) {
	printf("dndDrop\n");
      } else if (event.xclient.message_type == viewport->xatom_dndStatus) {
	printf("dndStatus\n");
      } else if (event.xclient.message_type == viewport->xatom_dndLeave) {
	printf("dndLeave\n");
      } else if (event.xclient.message_type == viewport->xatom_dndFinished) {
	printf("dndFinished\n");
      } else if (event.xclient.message_type == viewport->xatom_dndActionCopy) {
	printf("dndActionCopy\n");
      } else if (event.xclient.message_type == viewport->xatom_dndUriList) {
	printf("dndUriList\n");
      } else if (event.xclient.message_type == viewport->xatom_dndPlainText) {
	printf("dndPlainText\n");
      }
    } break;
    }
  }

  return res;
}

inline fn void viewport_swapBuffers(Viewport *viewport) {
  glXSwapBuffers(viewport->xdisplay, viewport->xwindow);
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
