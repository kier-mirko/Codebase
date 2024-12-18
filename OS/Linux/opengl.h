#ifndef BASE_OS_LINUX_OPENGL
#define BASE_OS_LINUX_OPENGL

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

// =============================================================================
// Shader functions
typedef u32 (*GL_CREATE_SHADER) (i32);
GL_CREATE_SHADER glCreateShader =
  (GL_CREATE_SHADER) glXGetProcAddress((u8 *)"glCreateShader");

typedef void (*GL_SHADER_SOURCE) (u32, i32, u8 **, void *);
GL_SHADER_SOURCE glShaderSource =
  (GL_SHADER_SOURCE) glXGetProcAddress((u8 *)"glShaderSource");

typedef void (*GL_COMPILE_SHADER) (u32);
GL_COMPILE_SHADER glCompileShader =
  (GL_COMPILE_SHADER) glXGetProcAddress((u8 *)"glCompileShader");

typedef void (*GL_GET_SHADERIV) (u32, i32, i32 *);
GL_GET_SHADERIV glGetShaderiv =
  (GL_GET_SHADERIV) glXGetProcAddress((u8 *)"glGetShaderiv");

typedef void (*GL_GET_SHADER_INFOLOG) (u32, u32, void *, char *);
GL_GET_SHADER_INFOLOG glGetShaderInfoLog =
  (GL_GET_SHADER_INFOLOG) glXGetProcAddress((u8 *)"glGetShaderInfoLog");

typedef void (*GL_ATTACH_SHADER) (u32, u32);
GL_ATTACH_SHADER glAttachShader =
  (GL_ATTACH_SHADER) glXGetProcAddress((u8 *)"glAttachShader");

typedef void (*GL_DELETE_SHADER) (u32);
GL_DELETE_SHADER glDeleteShader =
  (GL_DELETE_SHADER) glXGetProcAddress((u8 *)"glDeleteShader");

// =============================================================================
// Program functions
typedef u32 (*GL_CREATE_PROGRAM) ();
GL_CREATE_PROGRAM glCreateProgram =
  (GL_CREATE_PROGRAM) glXGetProcAddress((u8 *)"glCreateProgram");

typedef void (*GL_GET_PROGRAMIV) (u32, i32, i32 *);
GL_GET_PROGRAMIV glGetProgramiv =
  (GL_GET_PROGRAMIV) glXGetProcAddress((u8 *)"glGetProgramiv");

typedef void (*GL_GET_PROGRAM_INFOLOG) (u32, u32, void *, char *);
GL_GET_PROGRAM_INFOLOG glGetProgramInfoLog =
  (GL_GET_PROGRAM_INFOLOG) glXGetProcAddress((u8 *)"glGetProgramInfoLog");

typedef void (*GL_LINK_PROGRAM) (u32);
GL_LINK_PROGRAM glLinkProgram =
  (GL_LINK_PROGRAM) glXGetProcAddress((u8 *)"glLinkProgram");

typedef void (*GL_USE_PROGRAM) (u32);
GL_USE_PROGRAM glUseProgram =
  (GL_USE_PROGRAM) glXGetProcAddress((u8 *)"glUseProgram");

// =============================================================================
// Vertex Array Object (VAO) functions
typedef void (*GL_GEN_VERTEX_ARRAYS) (u32, u32*);
GL_GEN_VERTEX_ARRAYS glGenVertexArrays =
  (GL_GEN_VERTEX_ARRAYS) glXGetProcAddress((u8 *)"glGenVertexArrays");

typedef void (*GL_BIND_VERTEX_ARRAY) (u32);
GL_BIND_VERTEX_ARRAY glBindVertexArray =
  (GL_BIND_VERTEX_ARRAY) glXGetProcAddress((u8 *)"glBindVertexArray");

typedef void (*GL_VERTEX_ATTRIB_POINTER) (i32, u32, i32, bool, usize, void*);
GL_VERTEX_ATTRIB_POINTER glVertexAttribPointer =
  (GL_VERTEX_ATTRIB_POINTER) glXGetProcAddress((u8 *)"glVertexAttribPointer");

typedef void (*GL_ENABLE_VERTEX_ATTRIB_ARRAY) (i32);
GL_ENABLE_VERTEX_ATTRIB_ARRAY glEnableVertexAttribArray =
  (GL_ENABLE_VERTEX_ATTRIB_ARRAY) glXGetProcAddress((u8 *)"glEnableVertexAttribArray");

// =============================================================================
// Vertex Buffer Object (VBO) functions
typedef void (*GL_GEN_BUFFERS) (u32, u32*);
GL_GEN_BUFFERS glGenBuffers =
  (GL_GEN_BUFFERS) glXGetProcAddress((u8 *)"glGenBuffers");

typedef void (*GL_BIND_BUFFER) (i32, u32);
GL_BIND_BUFFER glBindBuffer =
  (GL_BIND_BUFFER) glXGetProcAddress((u8 *)"glBindBuffer");

typedef void (*GL_BUFFER_DATA) (i32, usize, f32*, i32);
GL_BUFFER_DATA glBufferData =
  (GL_BUFFER_DATA) glXGetProcAddress((u8 *)"glBufferData");

#endif
