#pragma once

#include "../../string.hpp"

#include <dlfcn.h>

namespace OS::DynLib {
inline fn void *open(Base::String8 path);
inline fn void *lookup(void *handle, Base::String8 symbol);
inline fn void close(void *handle);
} // namespace OS::DynLib
