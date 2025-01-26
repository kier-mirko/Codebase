#include <dlfcn.h>

fn OS_Library os_lib_open(String8 path){
  OS_Library result = {0};
  Scratch scratch = ScratchBegin(0, 0);
  char *path_cstr = (char*)str8_copy(scratch.arena, path).str;
  void *handle = dlopen(path_cstrs, RTLD_LAZY);
  if(handle){
    result.v[0] = (u64)handle;
  }
  ScratchEnd(scratch);
  return result;
}

fn VoidFunc *os_lib_lookup(OS_Library lib, String8 symbol){
  Scratch scratch = ScratchBegin(0, 0);
  void *handle = (void*)lib.v[0];
  char *symbol_cstr = (char*)str8_copy(scratch.arena, symbol).str;
  VoidFunc *result = (VoidFunc*)dlsym(handle, symbol_cstr);
  ScratchEnd(scratch);
  return result;
}

fn void os_lib_close(OS_Library lib){
  void *handle = (void*)lib.v[0];
  int result = dlclose(handle);
  (void)result;
}
