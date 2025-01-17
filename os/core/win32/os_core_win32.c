fn OS_Handle
os_file_open(OS_AccessFlags flags, String8 filepath)
{
  OS_Handle handle = {0};
  Temp scratch = scratch_begin(0, 0);
  String16 path = str16_from_str8(scratch.arena, filepath);
  DWORD access_flags = 0;
  DWORD share_mode = 0;
  SECURITY_ATTRIBUTES security_attributes = {sizeof(SECURITY_ATTRIBUTES), 0, 0};
  DWORD creation_disposition = OPEN_EXISTING;
  DWORD flags_and_attributes = 0;
  HANDLE template_file = 0;
  if(flags & OS_AccessFlag_Read)  { access_flags |= GENERIC_READ; }
  if(flags & OS_AccessFlag_Write) { access_flags |= GENERIC_WRITE; }
  if(flags & OS_AccessFlag_Execute) { access_flags |= GENERIC_EXECUTE; }
  if(flags & OS_AccessFlag_ShareRead) { share_mode |= FILE_SHARE_READ; }
  if(flags & OS_AccessFlag_ShareWrite) { share_mode |= FILE_SHARE_WRITE; }
  if(flags & OS_AccessFlag_Write) { creation_disposition = CREATE_ALWAYS; }
  if(flags & OS_AccessFlag_Append) { creation_disposition = OPEN_ALWAYS; access_flags |= FILE_APPEND_DATA; }
  
  HANDLE file = CreateFileW((WCHAR*)path.str, access_flags, share_mode, &security_attributes, creation_disposition, flags_and_attributes, template_file);
  if(file != INVALID_HANDLE_VALUE)
  {
    handle.u64[0] = (U64)file;
  }
  scratch_end(scratch);
  return handle;
}

fn String8
os_file_read(Arena *arena, OS_Handle file)
{
  String8 result = {0};
  if(os_handle_match(file, os_handle_zero())) return result;
  
  LARGE_INTEGER file_size = {0};
  HANDLE handle = (HANDLE)file.u64[0];
  if(GetFileSizeEx(handle, &file_size) != 0)
  {
    LONGLONG size = file_size.QuadPart;
    U8 *buffer = make(arena, U8, result.size);
    DWORD bytes_read = 0;
    if(ReadFile(handle, buffer, (DWORD)size, &bytes_read, 0) && size == bytes_read)
    {
      result.str = buffer;
      result.size = (U64)size;
    }
  }
  return result;
}

fn B32
os_file_write(OS_Handle file, String8 content)
{
  if(os_handle_match(file, os_handle_zero())) return 0;
  B32 result = 0;
  DWORD bytes_written = 0;
  if(WriteFile((HANDLE)file.u64[0], content.str, (DWORD)content.size, &bytes_written, 0) 
     && content.size == bytes_written)
  {
    result = 1;
  }
  return result;
}

fn B32
os_file_write_list(OS_Handle file, String8List content)
{
  Temp scratch = scratch_begin(0, 0);
  String8 str = str8_list_join(scratch.arena, content);
  B32 result = os_file_write(file, str);
  return result;
}

fn FileProperties
os_file_get_propertie(OS_Handle file)
{
  (void)0;
}

fn void
os_file_close(OS_Handle file)
{
  if(os_handle_match(file, os_handle_zero())) return;
  HANDLE handle = (HANDLE)file.u64[0];
  BOOL result = CloseHandle(handle);
  (void)result;
}

