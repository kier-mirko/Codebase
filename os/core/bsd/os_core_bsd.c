// =============================================================================
// File reading and writing/appending
fn String8 os_file_read(Arena *arena, String8 filepath) {
  Assert(arena);
  if (filepath.size == 0) {
    return (String8) {0};
  }
  
  I32 fd = open((char *)filepath.str, O_RDONLY);
  if (fd < 0) {
    (void)close(fd);
    return (String8) {0};
  }
  
  struct stat file_stat;
  if (stat((char *)filepath.str, &file_stat) < 0) {
    (void)close(fd);
    return (String8) {0};
  }
  
  String8 res = { .str = (U8 *)make(arena, U8, file_stat.st_size) };
  res.size = read(fd, res.str, file_stat.st_size);
  
  (void)close(fd);
  return res;
}

fn B32 os_file_write(String8 filepath, String8 content) {
  if (filepath.size == 0) {
    return 0;
  }
  
  I32 fd = open((char *)filepath.str, O_WRONLY | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0) {
    (void)close(fd);
    return 0;
  }
  
  if (write(fd, content.str, content.size) != content.size) {
    (void)close(fd);
    return 0;
  }
  
  (void)close(fd);
  return 1;
}

fn B32 os_file_writeStream(String8 filepath, String8List content) {
  String8Node *start = content.first++;
  if (!os_file_write(filepath, start->string)) {
    return 0;
  }
  
  for (; start < content.first + content.size; ++start) {
    if (!os_file_append(filepath, start->string)) {
      return 0;
    }
  }
  
  return 1;
}

fn B32 os_file_append(String8 filepath, String8 content) {
  if (filepath.size == 0) {
    return 0;
  }
  
  I32 fd = open((char *)filepath.str, O_WRONLY | O_CREAT | O_APPEND,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0) {
    (void)close(fd);
    return 0;
  }
  
  if (write(fd, content.str, content.size) != content.size) {
    (void)close(fd);
    return 0;
  }
  
  (void)close(fd);
  return 1;
}

fn B32 os_file_appendStream(String8 filepath, String8List content) {
  for (String8Node *start = content.first; start < content.first + content.size;
       ++start) {
    if (!os_file_append(filepath, start->string)) {
      return 0;
    }
  }
  
  return 1;
}

fn FileProperties os_file_get_properties(String8 filepath) {
  FileProperties res = {0};
  if (filepath.size == 0) {
    return res;
  }
  
  struct stat file_stat;
  if (stat((char *)filepath.str, &file_stat) < 0) {
    return res;
  }
  
  res.owner_id = file_stat.st_uid;
  res.group_id = file_stat.st_gid;
  res.size = (USZ)file_stat.st_size;
  res.last_access_time = (U64)file_stat.st_atime;
  res.last_modification_time = (U64)file_stat.st_mtime;
  res.last_status_change_time = (U64)file_stat.st_ctime;
  
  res.user = ACF_Unknown;
  if (file_stat.st_mode & S_IRUSR) {
    res.user = ACF_Read;
  }
  if (file_stat.st_mode & S_IWUSR) {
    res.user = (AccessFlag)(res.user | ACF_Write);
  }
  if (file_stat.st_mode & S_IXUSR) {
    res.user = (AccessFlag)(res.user | ACF_Execute);
  }
  
  res.group = ACF_Unknown;
  if (file_stat.st_mode & S_IRGRP) {
    res.group = ACF_Read;
  }
  if (file_stat.st_mode & S_IWGRP) {
    res.group = (AccessFlag)(res.group | ACF_Write);
  }
  if (file_stat.st_mode & S_IXGRP) {
    res.group = (AccessFlag)(res.group | ACF_Execute);
  }
  
  res.other = ACF_Unknown;
  if (file_stat.st_mode & S_IROTH) {
    res.other = ACF_Read;
  }
  if (file_stat.st_mode & S_IWOTH) {
    res.other = (AccessFlag)(res.other | ACF_Write);
  }
  if (file_stat.st_mode & S_IXOTH) {
    res.other = (AccessFlag)(res.other | ACF_Execute);
  }
  
  return res;
}

// =============================================================================
// Memory mapping files for easier and faster handling

fn File os_file_open_mappedTmp(Arena *arena) {
  char path[] = "/tmp/base-XXXXXX";
  I32 fd = mkstemp(path);
  
  String8 pathstr = {
    .str = (U8 *)make(arena, U8, Arrsize(path)),
    .size = Arrsize(path),
  };
  memCopy(pathstr.str, path, Arrsize(path));
  
  return (File) {
    .descriptor = fd,
    .path = pathstr,
    .prop = os_file_get_properties(pathstr),
    .content = str8((char *)mmap(0, 0,
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd, 0), 0),
  };
}

fn File os_file_open_mapped(Arena *arena, String8 filepath) {
  Assert(arena);
  if (filepath.size == 0) {
    return (File) {0};
  }
  
  I32 fd = open((char *)filepath.str, O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0) {
    (void)close(fd);
    return (File) {0};
  }
  
  FileProperties prop = os_file_get_properties(filepath);
  return (File) {
    .descriptor = fd,
    .path = filepath,
    .prop = prop,
    .content = str8((char *)mmap(0, prop.size,
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd, 0), prop.size),
  };
}

fn B32 fs_fileWrite(File *file, String8 content) {
  return write(file->descriptor, content.str, content.size) != content.size;
}

fn B32 fs_fileWriteStream(File *file, String8List content) {
  for (String8Node *curr = content.first; curr; curr = curr->next) {
    if (!fs_fileWrite(file, curr->string)) { return 0; }
  }
  
  return 1;
}

fn B32 fs_fileClose(File *file) {
  return msync(file->content.str, file->prop.size, MS_SYNC) >= 0 &&
    munmap(file->content.str, file->prop.size) >= 0 &&
    close(file->descriptor) >= 0;
}

fn B32 fs_fileHasChanged(File *file) {
  FileProperties prop = os_file_get_properties(file->path);
  return (file->prop.last_access_time != prop.last_access_time) ||
  (file->prop.last_modification_time != prop.last_modification_time) ||
  (file->prop.last_status_change_time != prop.last_status_change_time);
}

fn B32 fs_fileErase(File *file) {
  return unlink((char *) file->path.str) >= 0 && fs_fileClose(file);
}

fn B32 fs_fileRename(File *file, String8 to) {
  return rename((char *) file->path.str, (char *) to.str) >= 0;
}

inline void fs_fileSync(File *file) {
  if (!fs_fileHasChanged(file)) { return; }
  fs_fileForceSync(file);
}

fn void fs_fileForceSync(File *file) {
  (void)munmap(file->content.str, file->prop.size);
  file->prop = os_file_get_properties(file->path);
  file->content = str8((char *)mmap(0, file->prop.size,
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED, file->descriptor, 0),
                       file->prop.size);
}

// =============================================================================
// Misc operation on the filesystem
inline B32 fs_delete(String8 filepath) {
  Assert(filepath.size != 0);
  return unlink((char *)filepath.str) >= 0;
}

inline B32 fs_rename(String8 filepath, String8 to) {
  Assert(filepath.size != 0 && to.size != 0);
  return rename((char *)filepath.str, (char *)to.str) >= 0;
}

fn B32 os_make_dir(String8 path) {
  Assert(path.size != 0);
  return mkdir((char *)path.str,
               S_IRWXU | (S_IRGRP | S_IXGRP) | (S_IROTH | S_IXOTH)) >= 0;
}

fn B32 os_remove_dir(String8 path) {
  Assert(path.size != 0);
  return rmdir((char *)path.str) >= 0;
}

// =============================================================================
// File iteration
fn String8List os_file_iter(Arena *arena, String8 dirname) {
  const String8 currdir = Strlit(".");
  const String8 parentdir = Strlit("..");
  
  String8List res = {0};
  
  DIR *dir = opendir((char *)dirname.str);
  if (!dir) {
    return res;
  }
  
  struct dirent *entry;
  while ((entry = readdir(dir))) {
    String8 str = str8_from_cstr(entry->d_name);
    if (str8_match(str, currdir) || str8_match(str, parentdir)) {
      continue;
    }
    
    String8Node *node = make(arena, String8Node);
    node->string = str;
    DLLPushBack(res.first, res.last, node);
  }
  
  (void)closedir(dir);
  return res;
}

fn B32 os_file_remove_iter(Arena *temp_arena, String8 dirname) {
  const String8 currdir = Strlit(".");
  const String8 parentdir = Strlit("..");
  USZ prev_head = temp_arena->head;
  
  String8List dirstack = {0};
  String8List deletable = {0};
  String8Node *root = make(temp_arena, String8Node);
  root->string = dirname;
  StackPush(dirstack.first, root);
  
  while (dirstack.first) {
    String8Node *current = dirstack.first;
    StackPop(dirstack.first);
    
    DIR *dir = opendir((char *)current->string.str);
    Assert(dir);
    
    struct dirent *entry;
    B32 is_empty = 1;
    while ((entry = readdir(dir))) {
      String8 str = str8_from_cstr(entry->d_name);
      if (str8_match(str, currdir) || str8_match(str, parentdir)) {
        continue;
      }
      
      is_empty = 0;
      String8 fullpath = str8_format(temp_arena, "%.*s/%.*s",
                                     Strexpand(current->string), Strexpand(str));
      
      if (entry->d_type == DT_DIR) {
        String8Node *childdir = make(temp_arena, String8Node);
        childdir->string = fullpath;
        StackPush(dirstack.first, childdir);
      } else {
        Assert(fs_delete(fullpath));
      }
    }
    
    (void)closedir(dir);
    if (is_empty) {
      (void)os_remove_dir(current->string);
    } else {
      StackPush(deletable.first, current);
    }
  }
  
  B32 res = 1;
  while (deletable.first && res) {
    res = os_remove_dir(deletable.first->string);
    StackPop(deletable.first);
  }
  
  temp_arena->head = prev_head;
  return res;
}

inline void *dynlib_open(String8 path) {
  return dlopen((char *)path.str, RTLD_LAZY);
}

inline void *dynlib_lookup(void *handle, String8 symbol) {
  Assert(!dlerror());
  Assert(handle);
  void *res = dlsym(handle, (char *)symbol.str);
  Assert(res);
  Assert(!dlerror());
  
  return res;
}

inline void dynlib_close(void *handle) {
  Assert(handle);
  I8 res = dlclose(handle);
  Assert(!res);
}

inline Thread *os_bsd_thread_spawn(void *(*thread_main)(void *)) {
  return os_bsd_thread_spawnArgs(thread_main, 0);
}

fn Thread *os_bsd_thread_spawn_args(void *(*thread_main)(void *), void *arg_data) {
  Assert(thread_main);
  
  Thread *thread;
  I32 res = pthread_create(&thread, 0, thread_main, arg_data);
  
  if (!thread || res != 0) {
    perror("`Base::OS::spawn_thread`");
  }
  
  return thread;
}

inline void os_bsd_thread_join(Thread *tcb) {
  os_bsd_thread_joinReturn(tcb, 0);
}

fn void os_bsd_thread_join_return(Thread *tcb, void **save_return_value_in) {
  (void)pthread_join(tcb, save_return_value_in);
}
