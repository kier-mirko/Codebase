fn String8 fs_read(Arena *arena, String8 filepath) {
  String8 result = {0};
  Scratch scratch = ScratchBegin(&arena, 1);
  String16 path = UTF16From8(scratch.arena, &filepath);
  SECURITY_ATTRIBUTES security_attributes = {sizeof(SECURITY_ATTRIBUTES), 0, 0};
  HANDLE handle = CreateFileW((WCHAR*)path.str, GENERIC_READ, 0, 
                              &security_attributes, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if(handle != INVALID_HANDLE_VALUE) {
    LARGE_INTEGER file_size;
    if(GetFileSizeEx(handle, &file_size)) {
      void *buffer = New(arena, u8, file_size.QuadPart);
      DWORD size = (DWORD)file_size.QuadPart;
      DWORD bytes_read = 0;
      if(ReadFile(handle, buffer, size, &bytes_read, 0) && size == bytes_read) {
        result.str = buffer;
        result.size = size;
      }
    }
  }
  CloseHandle(handle);
  ScratchEnd(scratch);
  return result;
}

fn bool fs_write(String8 filepath, String8 content) {
  bool result = false;
  Scratch scratch = ScratchBegin(0, 0);
  String16 path = UTF16From8(scratch.arena, &filepath);
  SECURITY_ATTRIBUTES security_attributes = {sizeof(SECURITY_ATTRIBUTES), 0, 0};
  HANDLE handle = CreateFileW((WCHAR*)path.str, GENERIC_WRITE, 0, 
                              &security_attributes, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  if(handle != INVALID_HANDLE_VALUE) {
    DWORD bytes_written = 0;
    if(WriteFile(handle, content.str, content.size, &bytes_written, 0) 
       && content.size == bytes_written) {
      result = true;
    }
  }
  CloseHandle(handle);
  return result;
}

fn bool fs_writeStream(String8 filepath, StringStream content) {
  Scratch scratch = ScratchBegin(0, 0);
  String8 str = str8FromStream(scratch.arena, &content);
  bool result = fs_write(filepath, str);
  return result;
}

fn bool fs_append(String8 filepath, String8 content) {
  bool result = false;
  Scratch scratch = ScratchBegin(0, 0);
  String16 path = UTF16From8(scratch.arena, &filepath);
  SECURITY_ATTRIBUTES security_attributes = {sizeof(SECURITY_ATTRIBUTES), 0, 0};
  HANDLE handle = CreateFileW((WCHAR*)path.str, FILE_APPEND_DATA, 0, 
                              &security_attributes, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  if(handle != INVALID_HANDLE_VALUE) {
    DWORD bytes_written = 0;
    if(WriteFile(handle, content.str, content.size, &bytes_written, 0) 
       && content.size == bytes_written) {
      result = true;
    }
  }
  
  CloseHandle(handle);
  return result;
}

fn bool fs_appendStream(String8 filepath, StringStream content) {
  Scratch scratch = ScratchBegin(0, 0);
  String8 str = str8FromStream(scratch.arena, &content);
  bool result = fs_append(filepath, str);
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
