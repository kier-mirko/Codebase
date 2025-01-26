fn os_Handle fs_open(String8 filepath, os_AccessFlags flags) {
  os_Handle result = {0};
  Scratch scratch = ScratchBegin(0, 0);
  String16 path = UTF16From8(scratch.arena, &filepath);
  SECURITY_ATTRIBUTES security_attributes = {sizeof(SECURITY_ATTRIBUTES), 0, 0};
  DWORD access_flags = 0;
  DWORD share_mode = 0;
  DWORD creation_disposition = OPEN_EXISTING;
  
  if(flags & os_acfRead) { access_flags |= GENERIC_READ;}
  if(flags & os_acfWrite) { access_flags |= GENERIC_WRITE; creation_disposition = CREATE_ALWAYS; }
  if(flags & os_acfExecute) { access_flags |= GENERIC_EXECUTE; }
  if(flags & os_acfAppend) { creation_disposition = OPEN_ALWAYS; access_flags |= FILE_APPEND_DATA; }
  if(flags & os_acfShareRead) { share_mode |= FILE_SHARE_READ; }
  if(flags & os_acfShareWrite) { share_mode |= FILE_SHARE_WRITE; }
  
  HANDLE file = CreateFileW((WCHAR*)path.str, access_flags, share_mode, 
                            &security_attributes, creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
  if(file != INVALID_HANDLE_VALUE) {
    result.fd[0] = (u64)file;
  }
  
  ScratchEnd(scratch);
  return result;
}

fn String8 fs_read(Arena *arena, os_Handle file) {
  String8 result = {0};
  
  if(file.fd[0] == 0) { return result; }
  
  LARGE_INTEGER file_size = {0};
  HANDLE handle = (HANDLE)file.fd[0];
  
  if(GetFileSizeEx(handle, &file_size)) {
    DWORD bytes_read = 0;
    DWORD size = (DWORD)file_size.QuadPart;
    void *buffer = New(arena, u8, size);
    OVERLAPPED overlapped = {0};
    if(ReadFile(handle, buffer, size, &bytes_read, &overlapped)
       && size == bytes_read) {
      result.str = (u8*)buffer;
      result.size = size;
    }
  }
  
  return result;
}

fn bool fs_write(os_Handle file, String8 content) {
  bool result = false;
  
  if(file.fd[0] == 0) { return result; }
  
  DWORD bytes_written = 0;
  if(WriteFile((HANDLE)file.fd[0], content.str, (DWORD)content.size, &bytes_written, 0)
     && content.size == bytes_written) {
    result = true;
  }
  return result;
}

fn fs_Properties fs_getProp(os_Handle file) {
  // TODO(km): need to fill owner,group, and permission flags
  fs_Properties properties = {0};
  
  return properties;
}

fn File fs_fileOpen(Arena *arena, String8 filepath) {
  File result = {0};
  Scratch scratch = ScratchBegin(&arena, 1);
  String16 path = UTF16From8(scratch.arena, &filepath);
  
  SECURITY_ATTRIBUTES security_attributes = {sizeof(SECURITY_ATTRIBUTES), 0, 0};
  DWORD access_flags = GENERIC_READ | GENERIC_WRITE;
  DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
  HANDLE file = CreateFileW((WCHAR*)path.str, access_flags, share_mode, 
                            &security_attributes, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  
  if(file != INVALID_HANDLE_VALUE) {
    LARGE_INTEGER file_size;
    if(GetFileSizeEx(file, &file_size)) {
      HANDLE file_mapped = CreateFileMappingW(file, 0, PAGE_READWRITE, 0, 0, 0);
      if(file_mapped != INVALID_HANDLE_VALUE) {
        result.handle.fd[0] = (u64)file_mapped;
        result.path = filepath;
        result.prop = fs_getProp(result.handle);
        result.content = (u8*)MapViewOfFileEx(file_mapped, FILE_MAP_ALL_ACCESS, 0, 0, 0, 0);
      }
    }
  }
  return result;
}
