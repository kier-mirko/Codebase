#ifndef BASE_OS_LINUX_OPENGL
#define BASE_OS_LINUX_OPENGL

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

// =============================================================================
// Shader functions
typedef U32 (*GL_CREATE_SHADER) (I32);
global GL_CREATE_SHADER glCreateShader;
typedef void (*GL_SHADER_SOURCE) (U32, I32, U8 **, void *);
global GL_SHADER_SOURCE glShaderSource;
typedef void (*GL_COMPILE_SHADER) (U32);
global GL_COMPILE_SHADER glCompileShader;
typedef void (*GL_GET_SHADERIV) (U32, I32, I32 *);
global GL_GET_SHADERIV glGetShaderiv;
typedef void (*GL_GET_SHADER_INFOLOG) (U32, U32, void *, char *);
global GL_GET_SHADER_INFOLOG glGetShaderInfoLog;
typedef void (*GL_ATTACH_SHADER) (U32, U32);
global GL_ATTACH_SHADER glAttachShader;
typedef void (*GL_DELETE_SHADER) (U32);
global GL_DELETE_SHADER glDeleteShader;

// =============================================================================
// Program functions
typedef U32 (*GL_CREATE_PROGRAM) ();
global GL_CREATE_PROGRAM glCreateProgram;
typedef void (*GL_GET_PROGRAMIV) (U32, I32, I32 *);
global GL_GET_PROGRAMIV glGetProgramiv;
typedef void (*GL_GET_PROGRAM_INFOLOG) (U32, U32, void *, char *);
global GL_GET_PROGRAM_INFOLOG glGetProgramInfoLog;
typedef void (*GL_LINK_PROGRAM) (U32);
global GL_LINK_PROGRAM glLinkProgram;
typedef void (*GL_USE_PROGRAM) (U32);
global GL_USE_PROGRAM glUseProgram;

// =============================================================================
// Vertex Array Object (VAO) functions
typedef void (*GL_GEN_VERTEX_ARRAYS) (U32, U32*);
global GL_GEN_VERTEX_ARRAYS glGenVertexArrays;
typedef void (*GL_BIND_VERTEX_ARRAY) (U32);
global GL_BIND_VERTEX_ARRAY glBindVertexArray;
typedef void (*GL_VERTEX_ATTRIB_POINTER) (I32, U32, I32, B32, USZ, void*);
global GL_VERTEX_ATTRIB_POINTER glVertexAttribPointer;
typedef void (*GL_ENABLE_VERTEX_ATTRIB_ARRAY) (I32);
global GL_ENABLE_VERTEX_ATTRIB_ARRAY glEnableVertexAttribArray;

// =============================================================================
// Vertex Buffer Object (VBO) functions
typedef void (*GL_GEN_BUFFERS) (U32, U32*);
global GL_GEN_BUFFERS glGenBuffers;
typedef void (*GL_BIND_BUFFER) (I32, U32);
global GL_BIND_BUFFER glBindBuffer;
typedef void (*GL_BUFFER_DATA) (I32, USZ, f32*, I32);
global GL_BUFFER_DATA glBufferData;

fn void opengl_init() {
  glCreateShader = (GL_CREATE_SHADER) glXGetProcAddress((U8 *)"glCreateShader");
  glShaderSource = (GL_SHADER_SOURCE) glXGetProcAddress((U8 *)"glShaderSource");
  glCompileShader = (GL_COMPILE_SHADER) glXGetProcAddress((U8 *)"glCompileShader");
  glGetShaderiv = (GL_GET_SHADERIV) glXGetProcAddress((U8 *)"glGetShaderiv");
  glGetShaderInfoLog = (GL_GET_SHADER_INFOLOG) glXGetProcAddress((U8 *)"glGetShaderInfoLog");
  glAttachShader = (GL_ATTACH_SHADER) glXGetProcAddress((U8 *)"glAttachShader");
  glDeleteShader = (GL_DELETE_SHADER) glXGetProcAddress((U8 *)"glDeleteShader");
  
  glCreateProgram = (GL_CREATE_PROGRAM) glXGetProcAddress((U8 *)"glCreateProgram");
  glGetProgramiv = (GL_GET_PROGRAMIV) glXGetProcAddress((U8 *)"glGetProgramiv");
  glGetProgramInfoLog = (GL_GET_PROGRAM_INFOLOG) glXGetProcAddress((U8 *)"glGetProgramInfoLog");
  glLinkProgram = (GL_LINK_PROGRAM) glXGetProcAddress((U8 *)"glLinkProgram");
  glUseProgram = (GL_USE_PROGRAM) glXGetProcAddress((U8 *)"glUseProgram");
  
  glGenVertexArrays = (GL_GEN_VERTEX_ARRAYS) glXGetProcAddress((U8 *)"glGenVertexArrays");
  glBindVertexArray = (GL_BIND_VERTEX_ARRAY) glXGetProcAddress((U8 *)"glBindVertexArray");
  glVertexAttribPointer = (GL_VERTEX_ATTRIB_POINTER) glXGetProcAddress((U8 *)"glVertexAttribPointer");
  glEnableVertexAttribArray = (GL_ENABLE_VERTEX_ATTRIB_ARRAY)
    glXGetProcAddress((U8 *)"glEnableVertexAttribArray");
  
  glGenBuffers = (GL_GEN_BUFFERS) glXGetProcAddress((U8 *)"glGenBuffers");
  glBindBuffer = (GL_BIND_BUFFER) glXGetProcAddress((U8 *)"glBindBuffer");
  glBufferData = (GL_BUFFER_DATA) glXGetProcAddress((U8 *)"glBufferData");
}

#endif
