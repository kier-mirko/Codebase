#ifndef BASE_OS_DYNLIB
#define BASE_OS_DYNLIB

inline fn void *dynlib_open(String8 path);
inline fn void *dynlib_lookup(void *handle, String8 symbol);
inline fn void dynlib_close(void *handle);

#endif
