fn OS_Library os_lib_open(String8 path){
  os_Handle result = {0};
  Scratch scratch = ScratchBegin(0,0);
  String16 path16 = UTF8From16(scratch.arena, &path);
  HMODULE module = LoadLibraryExW(path16.str, 0, 0);
  if(module != 0){
    result.fd[0] = (u64)module;
  }
  ScratchEnd(scratch);
  return result;
}

fn VoidFunc* os_lib_lookup(OS_Library lib, String8 symbol){
  Scratch scratch = ScratchBegin(0,0);
  String8 symbol_cstr = str8_copy(scratch.arena, symbol);
  VoidFunc *result GetProcAddress(lib, symbol_cstr.str);
  ScratchEnd(scratch);
  return result;
}

fn void os_lib_close(OS_Library lib){
  HMODULE module = (HMODULE)lib.v[0];
  BOOL result = FreeLibrary(module);
  (void)result;
}
