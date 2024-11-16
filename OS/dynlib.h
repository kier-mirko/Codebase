#ifndef BASE_OS_DYNLIB
#define BASE_OS_DYNLIB

#include "../string.h"

inline fn void *dynlib_open(String8 path);
inline fn void *dynlib_lookup(void *handle, String8 symbol);
inline fn void dynlib_close(void *handle);

#endif
