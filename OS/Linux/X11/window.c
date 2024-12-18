#include "string.h"
#include "window.h"
#include "../../../image.h"
#include "../../../vector.h"
#include "../../../math.h"

Viewport Viewport::opengl(String8 name, usize initial_width,
			usize initial_height) {
  Viewport viewport = {.xdisplay = XOpenDisplay(0)};
  if (!viewport.xdisplay) {
    return viewport;
  }

  viewport.name = name;
  viewport.width = initial_width;
  viewport.height = initial_height;

  viewport.xscreen = XDefaultScreen(viewport.xdisplay);
  viewport.xroot = XDefaultRootWindow(viewport.xdisplay);

  /* ========================================================================= */
  /* OpenGL setup */
  i32 majorGLX, minorGLX = 0;
  glXQueryVersion(viewport.xdisplay, &majorGLX, &minorGLX);
  if (majorGLX <= 1 && minorGLX < 2) {
    printf("GLX 1.2 or greater is required.\n");
    XCloseDisplay(viewport.xdisplay);
    return (Viewport) {0};
  } else {
    printf("\e[33mGLX client version:\e[0m %s\n",
	   glXGetClientString(viewport.xdisplay, GLX_VERSION));
    printf("\e[33mGLX client vendor:\e[0m %s\n",
	   glXGetClientString(viewport.xdisplay, GLX_VENDOR));
    printf("\e[33mGLX client extensions:\e[0m %s\n",
	   glXGetClientString(viewport.xdisplay, GLX_EXTENSIONS));

    printf("\e[33mGLX server version:\e[0m %s\n",
	   glXQueryServerString(viewport.xdisplay, viewport.xscreen,
				GLX_VERSION));
    printf("\e[33mGLX server vendor:\e[0m %s\n",
	   glXQueryServerString(viewport.xdisplay, viewport.xscreen,
				GLX_VENDOR));
    printf("\e[33mGLX server extensions:\e[0m %s\n\n",
	   glXQueryServerString(viewport.xdisplay, viewport.xscreen,
				GLX_EXTENSIONS));
  }

  local i32 glx_attribs[] = {
    GLX_X_RENDERABLE    , True,
    GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
    GLX_RENDER_TYPE     , GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
    GLX_RED_SIZE        , 8,
    GLX_GREEN_SIZE      , 8,
    GLX_BLUE_SIZE       , 8,
    GLX_ALPHA_SIZE      , 8,
    GLX_DEPTH_SIZE      , 24,
    GLX_STENCIL_SIZE    , 8,
    GLX_DOUBLEBUFFER    , True,
    None
  };

  i32 fbcount;
  GLXFBConfig* fbc = glXChooseFBConfig(viewport.xdisplay, viewport.xscreen,
				       glx_attribs, &fbcount);
  if (fbc == 0) {
    printf("Failed to retrieve framebuffer.\n");
    XCloseDisplay(viewport.xdisplay);
    return (Viewport) {0};
  }
  printf("Found %d matching framebuffers.\n", fbcount );

  // Pick the FB config/visual with the most samples per pixel
  printf("Getting best XVisualInfo\n");
  i32 best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
  for (int i = 0; i < fbcount; ++i) {
    XVisualInfo *vi = glXGetVisualFromFBConfig(viewport.xdisplay, fbc[i]);
    if (vi != 0) {
      i32 samp_buf, samples;
      glXGetFBConfigAttrib(viewport.xdisplay, fbc[i], GLX_SAMPLE_BUFFERS,
			   &samp_buf);
      glXGetFBConfigAttrib(viewport.xdisplay, fbc[i], GLX_SAMPLES, &samples);

      if (best_fbc < 0 || (samp_buf && samples > best_num_samp)) {
	best_fbc = i;
	best_num_samp = samples;
      }
      if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp) {
	worst_fbc = i;
	worst_num_samp = samples;
      }
    }

    XFree(vi);
  }

  printf("Best visual info index: %d\n", best_fbc);
  GLXFBConfig bestFbc = fbc[best_fbc];
  XFree(fbc);

  XVisualInfo* vi = glXGetVisualFromFBConfig(viewport.xdisplay, bestFbc);
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
  /* ========================================================================= */

  viewport.xwindow = XCreateWindow(viewport.xdisplay, viewport.xroot,
				      0, 0,
				      initial_width, initial_height,
				      0, vi->depth,
				      InputOutput, vi->visual,
				      CWBackPixel | CWColormap | CWBorderPixel |
				      CWEventMask,
				      &swa);

  /* Set the window name */
  XStoreName(viewport.xdisplay, viewport.xwindow, (char *)name.str);

  /* Display the window */
  if (!XMapWindow(viewport.xdisplay, viewport.xwindow)) {
    printf("\tXMapWindow\n");
    return (Viewport){0};
  }

  viewport.xatom_close = XInternAtom(viewport.xdisplay, "WM_DELETE_WINDOW", 0);

  viewport.xatom_state = XInternAtom(viewport.xdisplay, "_NET_WM_STATE", 0);
  viewport.xatom_state_fullscreen = XInternAtom(viewport.xdisplay, "_NET_WM_STATE_FULLSCREEN", 0);


  viewport.xatom_dndTypeList = XInternAtom(viewport.xdisplay, "XdndTypeList", 0);
  viewport.xatom_dndSelection = XInternAtom(viewport.xdisplay,
					    "XdndSelection", 0);

  viewport.xatom_dndEnter = XInternAtom(viewport.xdisplay, "XdndEnter", 0);
  viewport.xatom_dndPosition = XInternAtom(viewport.xdisplay, "XdndPosition", 0);
  viewport.xatom_dndStatus = XInternAtom(viewport.xdisplay, "XdndStatus", 0);
  viewport.xatom_dndLeave = XInternAtom(viewport.xdisplay, "XdndLeave", 0);
  viewport.xatom_dndDrop = XInternAtom(viewport.xdisplay, "XdndDrop", 0);
  viewport.xatom_dndFinished = XInternAtom(viewport.xdisplay, "XdndFinished", 0);

  viewport.xatom_dndActionCopy = XInternAtom(viewport.xdisplay,
					     "XdndActionCopy", 0);

  viewport.xatom_dndUriList = XInternAtom(viewport.xdisplay, "text/uri-list", 0);
  viewport.xatom_dndPlainText = XInternAtom(viewport.xdisplay, "text/plain", 0);

  viewport.xatom_dndAware = XInternAtom(viewport.xdisplay, "XdndAware", 0);

  local u8 xdnd_version = 5;
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

  /* ========================================================================= */
  /* OpenGL context init */

  // Create GLX OpenGL context
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
    glXGetProcAddressARB((const u8 *) "glXCreateContextAttribsARB");

  String8 glx_exts = strFromCstr((char *)
    glXQueryExtensionsString(viewport.xdisplay, viewport.xscreen));
  printf("\e[33mLate extensions:\e[0m %.*s\n\n", Strexpand(glx_exts));
  if (glXCreateContextAttribsARB == 0) {
    printf("glXCreateContextAttribsARB() not found.\n");
  }

  i32 context_attribs[] = {
    GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
    GLX_CONTEXT_MINOR_VERSION_ARB, 3,
    GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    None
  };

  if (!opengl_isExtensionSupported(glx_exts, Strlit("GLX_ARB_create_context"))) {
    viewport.opengl_context = glXCreateNewContext(viewport.xdisplay, bestFbc,
					       GLX_RGBA_TYPE, 0, True);
  } else {
    viewport.opengl_context = glXCreateContextAttribsARB(viewport.xdisplay,
							 bestFbc, 0, true,
							 context_attribs);
  }
  XSync(viewport.xdisplay, 0);

  // Verifying that context is a direct context
  if (!glXIsDirect(viewport.xdisplay, viewport.opengl_context)) {
    printf("Indirect GLX rendering context obtained\n");
  } else {
    printf("Direct GLX rendering context obtained\n");
  }

  glXMakeCurrent(viewport.xdisplay, viewport.xwindow, viewport.opengl_context);

  printf("\e[33mGL Vendor:\e[0m %s\n", glGetString(GL_VENDOR));
  printf("\e[33mGL Renderer:\e[0m %s\n", glGetString(GL_RENDERER));
  printf("\e[33mGL Version:\e[0m %s\n", glGetString(GL_VERSION));
  printf("\e[33mGL Shading Language:\e[0m %s\n\n",
	 glGetString(GL_SHADING_LANGUAGE_VERSION));

  glEnable(GL_DEPTH_TEST);
  /* ========================================================================= */

  return viewport;
}

Viewport Viewport::vulkan(String8 name, usize initial_width,
			  usize initial_height) {
  return (Viewport) {0};
}

void Viewport::close() {
  (void)glXMakeCurrent(xdisplay, None, NULL);
  (void)glXDestroyContext(xdisplay, opengl_context);
  (void)XDestroyWindow(xdisplay, xwindow);
  (void)XCloseDisplay(xdisplay);
}

bool Viewport::shouldClose() {
  XEvent event = {0};
  if (XCheckTypedEvent(xdisplay, ClientMessage, &event)) {
    if (event.xclient.data.l[0] == xatom_close) {
      return true;
    } else {
      XPutBackEvent(xdisplay, &event);
    }
  }

  return false;
}

ViewportEvent Viewport::getEvent() {
  XEvent event = {0};
  XWindowAttributes gwa = {0};
  ViewportEvent res = {};

  if (XPending(xdisplay)) {
    XNextEvent(xdisplay, &event);

    switch (event.type) {
    case Expose: {
      res.type = SHOW;

      XGetWindowAttributes(xdisplay, xwindow, &gwa);
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
      res.mouse.y = event.xbutton.y <= height
		    ? height - event.xbutton.y
		    : 0;
    } break;
    case MotionNotify: {
      printf("Motion\n");
      res.type = PTR_MOTION;
      res.motion.x = ClampBot(event.xmotion.x, 0);
      res.motion.y = event.xmotion.y <= height
		     ? height - event.xmotion.y
		     : 0;
    } break;
    case ClientMessage: {
      /* Make sure we aren't consuming the `window close` message */
      if (event.xclient.data.l[0] == xatom_close) {
	XPutBackEvent(xdisplay, &event);
      }

      if (event.xclient.message_type == xatom_close) {
	printf("dndClose\n");
      } else if (event.xclient.message_type == xatom_dndAware) {
	printf("dndAware\n");
      } else if (event.xclient.message_type == xatom_dndTypeList) {
	printf("dndTypeList\n");
      } else if (event.xclient.message_type == xatom_dndSelection) {
	printf("dndSelection\n");
      } else if (event.xclient.message_type == xatom_dndEnter) {
	printf("dndEnter\n");
      } else if (event.xclient.message_type == xatom_dndPosition) {
	printf("dndPosition\n");
      } else if (event.xclient.message_type == xatom_dndDrop) {
	printf("dndDrop\n");
      } else if (event.xclient.message_type == xatom_dndStatus) {
	printf("dndStatus\n");
      } else if (event.xclient.message_type == xatom_dndLeave) {
	printf("dndLeave\n");
      } else if (event.xclient.message_type == xatom_dndFinished) {
	printf("dndFinished\n");
      } else if (event.xclient.message_type == xatom_dndActionCopy) {
	printf("dndActionCopy\n");
      } else if (event.xclient.message_type == xatom_dndUriList) {
	printf("dndUriList\n");
      } else if (event.xclient.message_type == xatom_dndPlainText) {
	printf("dndPlainText\n");
      }
    } break;
    }
  }

  return res;
}

bool Viewport::setIcon(Arena *arena, String8 path) {
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

  Atom net_wm_icon = XInternAtom(xdisplay, "_NET_WM_ICON", 0);
  Atom cardinal = XInternAtom(xdisplay, "CARDINAL", 0);
  XChangeProperty(xdisplay, xwindow, net_wm_icon, cardinal,
		  32, PropModeReplace, (u8 *)data, size);

  arena->head = head;
  destroyImg(imgdata);
  return true;
}

void Viewport::setTitle(String8 title) {
  XStoreName(xdisplay, xwindow, (char *)title.str);
}

void Viewport::toggleFullscreen() {
  xatom_state = XInternAtom(xdisplay, "_NET_WM_STATE", 0);
  xatom_state_fullscreen = XInternAtom(xdisplay, "_NET_WM_STATE_FULLSCREEN", 0);

  if (xatom_state == None || xatom_state_fullscreen == None) {
    return;
  }

  XEvent xev;
  xev.xclient.type = ClientMessage;
  xev.xclient.serial = 0;
  xev.xclient.send_event = True;
  xev.xclient.display = xdisplay;
  xev.xclient.window = xwindow;
  xev.xclient.message_type = xatom_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 2;
  xev.xclient.data.l[1] = xatom_state_fullscreen;
  xev.xclient.data.l[2] = 0;
  xev.xclient.data.l[3] = 1;
  xev.xclient.data.l[4] = 0;

  XSendEvent(xdisplay, xroot, 0, SubstructureNotifyMask | SubstructureRedirectMask, &xev);
  XFlush(xdisplay);
}

bool Viewport::isFullscreen() {
  xatom_state = XInternAtom(xdisplay, "_NET_WM_STATE", 0);
  xatom_state_fullscreen = XInternAtom(xdisplay, "_NET_WM_STATE_FULLSCREEN", 0);

  if (xatom_state == None || xatom_state_fullscreen == None) {
    return false;
  }

  u32 actualType;
  i32 actualFormat;
  u64 nItems, bytesAfter;
  u8 *property = NULL;

  i32 status = XGetWindowProperty(xdisplay, xwindow, xatom_state, 0, (~0L),
                                  False, 4, (u64 *)&actualType, &actualFormat,
                                  &nItems, &bytesAfter, &property);

  if (status != Success || actualType != 4 || actualFormat != 32 || !property) {
    if (property) XFree(property);
    return false;
  }

  u32 *states = (u32 *)property;
  bool isFullscreen = false;
  for (u64 i = 0; i < nItems; i++) {
    if (states[i] == xatom_state_fullscreen) {
      isFullscreen = true;
      break;
    }
  }

  XFree(property);
  return isFullscreen;
}

bool Viewport::isFocused() {
    Window focusedWindow;
    int revertTo;

    XGetInputFocus(xdisplay, &focusedWindow, &revertTo);
    return (focusedWindow == xwindow);
}

inline void Viewport::swapBuffers() {
  glXSwapBuffers(xdisplay, xwindow);
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




/* Graphics stuff that shouldn't be here */

fn bool opengl_isExtensionSupported(String8 ext_list, String8 extension) {
  if (extension.size == 0 || strContains(extension, ' ')) {
    return false;
  }

  for (usize start = 0, terminator; ;
       start = terminator, ext_list = strPostfix(ext_list, terminator)) {
    usize where = strFindFirst(ext_list, extension);
    if (!where) {
      break;
    }

    terminator = where + extension.size;

    if (where == start || ext_list[where - 1] == ' ') {
      if (ext_list[terminator] == ' ' || ext_list[terminator] == '\0') {
	return true;
      }
    }
  }

  return false;
}

fn void viewport_setOrigin(Vector<f32, 2> origin) {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(origin.x(), origin.y(), 0.0f);
}

fn void viewport_setOrigin(Vector<f32, 3> origin) {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(origin.x(), origin.y(), origin.z());
}

fn void viewport_drawCircle(Vector<f32, 2> center, f32 radius) {
  constexpr i32 triangle_amount = 40;

  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(center.x(), center.y());
  for (usize i = 0; i <= triangle_amount; i++) {
    glVertex2f((center.x() + radius) * cos(i * TwoPi / triangle_amount),
               (center.y() - radius) * sin(i * TwoPi / triangle_amount));
  }
  glEnd();
}
