#include "../file.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/types.h>

// =============================================================================
// File reading and writing/appending
fn OS_Handle fs_open(String8 filepath, OS_AccessFlags flags) {
  OS_Handle result = {0};
  int access_flags = 0;
  
  if((flags & OS_AccessFlag_Read) && (flags & OS_AccessFlag_Write)) {
    access_flags |= O_RDWR;
  } else if(flags & OS_AccessFlag_Read) { 
    access_flags |= O_RDONLY; 
  } else if(flags & OS_AccessFlag_Write) { 
    access_flags |= O_WRONLY | O_CREAT | O_TRUNC; 
  }
  
  if(flags & OS_AccessFlag_Append) { access_flags |= O_APPEND | O_CREAT; }
  
  int fd = open((char*)filepath.str, access_flags, 0644);
  if(fd != -1) {
    result.fd[0] = fd;
  } 
  
  return result;
}

fn String8 fs_read(Arena *arena, OS_Handle file) {
  String8 result = {0};
  
  if(os_handleEq(file, os_handleZero())) { return result; }
  
  int fd = file.fd[0];
  struct stat file_stat;
  if (fstat(fd, &file_stat) == 0) {
    void *buffer = New(arena, u8, file_stat.st_size);
    if(pread(fd, buffer, file_stat.st_size, 0) >= 0) {
      result.str = buffer;
      result.size = file_stat.st_size;
    }
  }
  
  return result;
}

fn bool fs_write(OS_Handle file, String8 content) {
  bool result = false;
  
  if(os_handleEq(file, os_handleZero())) { return result; }
  
  if (write(file.fd[0], content.str, content.size) == (isize)content.size) {
    result = true;
  }
  
  return result;
}

fn FileProperties fs_getProp(OS_Handle file) {
  FileProperties result = {0};
  
  if(os_handleEq(file, os_handleZero())) { return result; }
  
  struct stat file_stat;
  if (fstat((int)file.fd[0], &file_stat) < 0) {
    return result;
  }
  
  result.ownerID = file_stat.st_uid;
  result.groupID = file_stat.st_gid;
  result.size = (usize)file_stat.st_size;
  result.last_access_time = (u64)file_stat.st_atime;
  result.last_modification_time = (u64)file_stat.st_mtime;
  result.last_status_change_time = (u64)file_stat.st_ctime;
  
  result.user = ACF_Unknown;
  if (file_stat.st_mode & S_IRUSR) { result.user |= ACF_Read; }
  if (file_stat.st_mode & S_IWUSR) { result.user |= ACF_Write; }
  if (file_stat.st_mode & S_IXUSR) { result.user |= ACF_Execute; }
  
  result.group = ACF_Unknown;
  if (file_stat.st_mode & S_IRGRP) { result.group |= ACF_Read; }
  if (file_stat.st_mode & S_IWGRP) { result.group |= ACF_Write; }
  if (file_stat.st_mode & S_IXGRP) { result.group |= ACF_Execute; }
  
  result.other = ACF_Unknown;
  if (file_stat.st_mode & S_IROTH) { result.other |= ACF_Read; }
  if (file_stat.st_mode & S_IWOTH) { result.other |= ACF_Write; }
  if (file_stat.st_mode & S_IXOTH) { result.other |= ACF_Execute; }
  
  return result;
}

// =============================================================================
// Memory mapping files for easier and faster handling

fn File fs_openTmp(Arena *arena) {
  char path[] = "/tmp/base-XXXXXX";
  i32 fd = mkstemp(path);
  
  String8 pathstr = {0};
  pathstr.str = New(arena, u8, Arrsize(path));
  pathstr.size = Arrsize(path);
  memCopy(pathstr.str, path, Arrsize(path));
  
  File file = {0};
  file.handle[0] = (u64)fd;
  file.path = pathstr;
  file.prop = fs_getProp(file.handle);
  file..content = str8((u8*)mmap(0, 0PROT_READ | PROT_WRITE,MAP_SHARED, fd, 0), 0),
  return file;
}

fn File fs_fileOpen(Arena *arena, String8 filepath) {
  File result = {0};
  
  i32 fd = open((char *)filepath.str, O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd != -1) {
    result.handle[0] = (u64)fd;
    reuslt.path = filepath;
    FileProperties prop = fs_getProp(result.handle);
    reuslt.prop = prop;
    result.content = str8((char *)mmap(0, prop.size, PROT_READ | PROT_WRITE,MAP_SHARED, fd, 0), prop.size);
  }
  
  return result;
}

fn bool fs_fileWrite(File *file, String8 content) {
  return write(file->handle.fd[0], content.str, content.size) != (isize)content.size;
}

fn bool fs_fileWriteStream(File *file, StringStream content) {
  for (StringNode *curr = content.first; curr; curr = curr->next) {
    if (!fs_fileWrite(file, curr->value)) { return false; }
  }

  return true;
}

fn bool fs_fileClose(File *file) {
  return msync(file->content.str, file->prop.size, MS_SYNC) >= 0 &&
	 munmap(file->content.str, file->prop.size) >= 0 &&
    close(file->handle.fd[0]) >= 0;
}

fn bool fs_fileHasChanged(File *file) {
  FileProperties prop = fs_getProp(file->handle);
  return (file->prop.last_access_time != prop.last_access_time) ||
	 (file->prop.last_modification_time != prop.last_modification_time) ||
	 (file->prop.last_status_change_time != prop.last_status_change_time);
}

fn bool fs_fileErase(File *file) {
  return unlink((char *) file->path.str) >= 0 && fs_fileClose(file);
}

fn bool fs_fileRename(File *file, String8 to) {
  return rename((char *) file->path.str, (char *) to.str) >= 0;
}

inline fn void fs_fileSync(File *file) {
  if (!fs_fileHasChanged(file)) { return; }
  fs_fileForceSync(file);
}

fn void fs_fileForceSync(File *file) {
  (void)munmap(file->content.str, file->prop.size);
  file->prop = fs_getProp(file->handle);
  file->content = str8((char *)mmap(0, file->prop.size,
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED, file->handle.fd[0], 0), file->prop.size);
}

// =============================================================================
// Misc operation on the filesystem
inline fn bool fs_delete(String8 filepath) {
  Assert(filepath.size != 0);
  return unlink((char *)filepath.str) >= 0;
}

inline fn bool fs_rename(String8 filepath, String8 to) {
  Assert(filepath.size != 0 && to.size != 0);
  return rename((char *)filepath.str, (char *)to.str) >= 0;
}

fn bool fs_mkdir(String8 path) {
  Assert(path.size != 0);
  return mkdir((char *)path.str,
               S_IRWXU | (S_IRGRP | S_IXGRP) | (S_IROTH | S_IXOTH)) >= 0;
}

fn bool fs_rmdir(String8 path) {
  Assert(path.size != 0);
  return rmdir((char *)path.str) >= 0;
}

fn FilenameList fs_iterFiles(Arena *arena, String8 dirname) {
  local const String8 currdir = Strlit(".");
  local const String8 parentdir = Strlit("..");

  FilenameList res = {0};
  DIR *dir = opendir((char *)dirname.str);
  if (!dir) {
    return res;
  }

  struct dirent *entry;
  while ((entry = readdir(dir))) {
    String8 str = strFromCstr(entry->d_name);
    if (strEq(str, currdir) || strEq(str, parentdir)) {
      continue;
    }

    FilenameNode *node = New(arena, FilenameNode);
    node->value = str;
    DLLPushBack(res.first, res.last, node);
  }

  (void)closedir(dir);
  return res;
}

fn bool fs_rmIter(String8 dirname) {
  const String8 currdir = Strlit(".");
  const String8 parentdir = Strlit("..");

  Scratch scratch = ScratchBegin(0, 0);
  FilenameList dirstack = {0};
  FilenameList deletable = {0};
  FilenameNode *root = New(scratch.arena, FilenameNode);
  root->value = dirname;
  StackPush(dirstack.first, root);

  while (dirstack.first) {
    FilenameNode *current = dirstack.first;
    StackPop(dirstack.first);

    DIR *dir = opendir((char *)current->value.str);
    Assert(dir);

    struct dirent *entry;
    bool is_empty = true;
    while ((entry = readdir(dir))) {
      String8 str = strFromCstr(entry->d_name);
      if (strEq(str, currdir) || strEq(str, parentdir)) {
        continue;
      }
      
      is_empty = false;
      String8 fullpath = strFormat(scratch.arena, "%.*s/%.*s",
                                   Strexpand(current->value), Strexpand(str));
      
      if (entry->d_type == DT_DIR) {
        FilenameNode *childdir = New(scratch.arena, FilenameNode);
        childdir->value = fullpath;
        StackPush(dirstack.first, childdir);
      } else {
        Assert(fs_delete(fullpath));
      }
    }

    (void)closedir(dir);
    if (is_empty) {
      (void)fs_rmdir(current->value);
    } else {
      StackPush(deletable.first, current);
    }
  }

  bool res = true;
  while (deletable.first && res) {
    res = fs_rmdir(deletable.first->value);
    StackPop(deletable.first);
  }

  ScratchEnd(scratch);
  return res;
}
