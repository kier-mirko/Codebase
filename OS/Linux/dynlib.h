#pragma once

#include "../../string.h"

#include <dlfcn.h>

inline fn void *dynlib_open(String8 path);
inline fn void *dynlib_lookup(void *handle, String8 symbol);
inline fn void dynlib_close(void *handle);
