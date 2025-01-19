fn OS_Handle fs_open(String8 filepath, OS_AccessFlags flags) {
  OS_Handle result = {0};
  Scratch scratch = ScratchBegin(0, 0);
  String16 path = UTF16From8(scratch.arena, &filepath);
  SECURITY_ATTRIBUTES security_attributes = {sizeof(SECURITY_ATTRIBUTES), 0, 0};
  DWORD access_flags = 0;
  DWORD share_mode = 0;
  DWORD creation_disposition = OPEN_EXISTING;
  
  if(flags & OS_AccessFlag_Read) { access_flags |= GENERIC_READ;}
  if(flags & OS_AccessFlag_Write) { access_flags |= GENERIC_WRITE; creation_disposition = CREATE_ALWAYS; }
  if(flags & OS_AccessFlag_Execute) { access_flags |= GENERIC_EXECUTE; }
  if(flags & OS_AccessFlag_Append) { creation_disposition = OPEN_ALWAYS; access_flags |= FILE_APPEND_DATA; }
  if(flags & OS_AccessFlag_ShareRead) { share_mode |= FILE_SHARE_READ; }
  if(flags & OS_AccessFlag_ShareWrite) { share_mode |= FILE_SHARE_WRITE; }
  
  HANDLE file = CreateFileW((WCHAR*)path.str, access_flags, share_mode, 
                            &security_attributes, creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
  if(file != INVALID_HANDLE_VALUE) {
    result.fd[0] = (u64)file;
  }
  
  ScratchEnd(scratch);
  return result;
}

fn String8 fs_read(Arena *arena, OS_Handle file) {
  String8 result = {0};
  
  if(os_handleEq(file, os_handleZero())) { return result; }
  
  LARGE_INTEGER file_size = {0};
  HANDLE handle = (HANDLE)file.fd[0];
  
  if(GetFileSizeEx(handle, &file_size)) {
    DWORD bytes_read = 0;
    DWORD size = (DWORD)file_size.QuadPart;
    void *buffer = New(arena, u8, size);
    OVERLAPPED overlapped = {0};
    if(ReadFile(handle, buffer, size, &bytes_read, &overlapped)
       && size == bytes_read) {
      result.str = buffer;
      result.size = size;
    }
  }
  
  return result;
}

fn bool fs_write(OS_Handle file, String8 content) {
  bool result = false;
  
  if(os_handleEq(file, os_handleZero())) { return result; }
  
  DWORD bytes_written = 0;
  if(WriteFile((HANDLE)file.fd[0], content.str, content.size, &bytes_written, 0)
     && content.size == bytes_written) {
    result = true;
  }
  return result;
}

fn FileProperties fs_getProp(String8 filepath) {
  // TODO(km): need to fill owner,group, and permission flags
  FileProperties properties = {0};
  
  Scratch scratch = ScratchBegin(0, 0);
  String16 path = UTF16From8(scratch.arena, &filepath);
  
  WIN32_FILE_ATTRIBUTE_DATA file_attributes;
  GetFileAttributesExW((WCHAR*)path.str, GetFileExInfoStandard, &file_attributes);
  
  LARGE_INTEGER last_access_time = {
    .LowPart = file_attributes.ftLastAccessTime.dwLowDateTime,
    .HighPart = file_attributes.ftLastAccessTime.dwHighDateTime,
  };
  properties.last_access_time = last_access_time.QuadPart;
  
  LARGE_INTEGER last_modification_time = {
    .LowPart = file_attributes.ftLastWriteTime.dwLowDateTime,
    .HighPart = file_attributes.ftLastWriteTime.dwHighDateTime,
  };
  properties.last_modification_time = last_modification_time.QuadPart;
  
  LARGE_INTEGER size = { 
    .LowPart = file_attributes.nFileSizeLow, 
    .HighPart = file_attributes.nFileSizeHigh 
  };
  properties.size = size.QuadPart;
  
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
        result.prop = fs_getProp(filepath);
        result.content.str = MapViewOfFileEx(file_mapped, FILE_MAP_ALL_ACCESS, 0, 0, 0, 0);
        result.content.size = file_size.QuadPart;
      }
    }
  }
  return result;
}
