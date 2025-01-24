#include <dlfcn.h>

fn OS_Library os_lib_open(String8 path){
  OS_Library result = {0};
  Scratch scratch = ScratchBegin(0, 0);
  String8 path_cstr = str8_copy(scratch.arena, path);
  void *handle = dlopen(path_cstr.str, RTLD_LAZY);
  if(handle){
    result.v[0] = (u64)handle;
  }
  ScratchEnd(scratch);
  return result;
}

fn VoidFunc *os_lib_lookup(OS_Library lib, String8 symbol){
  Scratch scratch = ScratchBegin(0, 0);
  void *handle = (void*)lib.v[0];
  String8 symbol_cstr = str8_copy(scratch.arena, symbol);
  VoidFunc *result = (VoidFunc*)dlsym(handle, symbol_cstr.str);
  ScratchEnd(scratch);
  return result;
}

fn void os_lib_close(OS_Library lib){
  void *handle = (void*)lib.v[0];
  int result = dlclose(handle);
  (void)result;
}
