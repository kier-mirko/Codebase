#ifndef OS_LINUX_DYNLIB
#define OS_LINUX_DYNLIB

#include "dynlib.hpp"

namespace OS::DynLib {
inline fn void *open(Base::String8 path) {
  return dlopen((char *)path.str, RTLD_LAZY);
}

inline fn void *lookup(void *handle, Base::String8 symbol) {
  Assert(!dlerror());
  Assert(handle);
  void *res = dlsym(handle, (char *)symbol.str);
  Assert(res);
  Assert(!dlerror());

  return res;
}

inline fn void close(void *handle) {
  Assert(handle);
  i8 res = dlclose(handle);
  Assert(!res);
}
} // namespace OS::DynLib

#endif
