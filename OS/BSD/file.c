#include "../file.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/types.h>

// =============================================================================
// File reading and writing/appending
fn String8 fs_read(Arena *arena, String8 filepath) {
  Assert(arena);
  if (filepath.size == 0) {
    return (String8) {0};
  }

  i32 fd = open((char *)filepath.str, O_RDONLY);
  if (fd < 0) {
    (void)close(fd);
    return (String8) {0};
  }

  struct stat file_stat;
  if (stat((char *)filepath.str, &file_stat) < 0) {
    (void)close(fd);
    return (String8) {0};
  }

  String8 res = { .str = (u8 *)Newarr(arena, u8, file_stat.st_size) };
  res.size = read(fd, res.str, file_stat.st_size);

  (void)close(fd);
  return res;
}

fn bool fs_write(String8 filepath, String8 content) {
  if (filepath.size == 0) {
    return false;
  }

  i32 fd = open((char *)filepath.str, O_WRONLY | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0) {
    (void)close(fd);
    return false;
  }

  if (write(fd, content.str, content.size) != content.size) {
    (void)close(fd);
    return false;
  }

  (void)close(fd);
  return true;
}

fn bool fs_write(String8 filepath, StringStream content) {
  StringNode *start = content.first++;
  if (!fs_write(filepath, start->value)) {
    return false;
  }

  for (; start < content.first + content.size; ++start) {
    if (!fs_append(filepath, start->value)) {
      return false;
    }
  }

  return true;
}

fn bool fs_append(String8 filepath, String8 content) {
  if (filepath.size == 0) {
    return false;
  }

  i32 fd = open((char *)filepath.str, O_WRONLY | O_CREAT | O_APPEND,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0) {
    (void)close(fd);
    return false;
  }

  if (write(fd, content.str, content.size) != content.size) {
    (void)close(fd);
    return false;
  }

  (void)close(fd);
  return true;
}

fn bool fs_append(String8 filepath, StringStream content) {
  for (StringNode *start = content.first; start < content.first + content.size;
       ++start) {
    if (!fs_append(filepath, start->value)) {
      return false;
    }
  }

  return true;
}

fn FileProperties fs_getProp(String8 filepath) {
  FileProperties res = {0};
  if (filepath.size == 0) {
    return res;
  }

  struct stat file_stat;
  if (stat((char *)filepath.str, &file_stat) < 0) {
    return res;
  }

  res.ownerID = file_stat.st_uid;
  res.groupID = file_stat.st_gid;
  res.size = (usize)file_stat.st_size;
  res.last_access_time = (u64)file_stat.st_atime;
  res.last_modification_time = (u64)file_stat.st_mtime;
  res.last_status_change_time = (u64)file_stat.st_ctime;

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

fn File fs_open(Arena *arena, void *location) {
  char path[] = "/tmp/base-XXXXXX";
  i32 fd = mkstemp(path);

  String8 pathstr = {
    .str = (u8 *)Newarr(arena, u8, Arrsize(path)),
    .size = Arrsize(path),
  };
  memCopy(pathstr.str, path, Arrsize(path));

  return (File) {
    .descriptor = fd,
    .path = pathstr,
    .prop = fs_getProp(pathstr),
    .content = str8((char *)mmap(location, 0,
				 PROT_READ | PROT_WRITE,
				 MAP_SHARED, fd, 0), 0),
  };
}

fn File fs_open(Arena *arena, String8 filepath, void *location) {
  Assert(arena);
  if (filepath.size == 0) {
    return (File) {0};
  }

  i32 fd = open((char *)filepath.str, O_RDWR | O_CREAT,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0) {
    (void)close(fd);
    return (File) {0};
  }

  FileProperties prop = fs_getProp(filepath);
  return (File) {
    .descriptor = fd,
    .path = filepath,
    .prop = prop,
    .content = str8((char *)mmap(location, prop.size,
				 PROT_READ | PROT_WRITE,
				 MAP_SHARED, fd, 0), prop.size),
  };
}

bool File::write(const String8 &content) {
  return ::write(descriptor, content.str, content.size) != content.size;
}

bool File::write(const StringStream &content) {
  for (StringNode *curr = content.first; curr; curr = curr->next) {
    if (!write(curr->value)) { return false; }
  }

  return true;
}

bool File::close() {
  return ::msync(content.str, prop.size, MS_SYNC) >= 0 &&
	 ::munmap(content.str, prop.size) >= 0 &&
	 ::close(descriptor) >= 0;
}

bool File::hasChanged() {
  FileProperties prop = fs_getProp(path);
  return (prop.last_access_time != prop.last_access_time) ||
	 (prop.last_modification_time != prop.last_modification_time) ||
	 (prop.last_status_change_time != prop.last_status_change_time);
}

bool File::erase() {
  return ::unlink((char *) path.str) >= 0 && close();
}

bool File::rename(String8 to) {
  return ::rename((char *) path.str, (char *) to.str) >= 0;
}

void File::sync(bool force_it) {
  if (!force_it && !hasChanged()) { return; }

  (void)::munmap(content.str, prop.size);
  prop = fs_getProp(path);
  content = str8((char *)::mmap(0, prop.size,
				PROT_READ | PROT_WRITE,
				MAP_SHARED, descriptor, 0), prop.size);
}

inline fn void fs_close(File *file) {
  Assert(file);
  (void)munmap(file->content.str, file->prop.size);
  (void)close(file->descriptor);
}

inline fn bool fs_hasChanged(File *file) {
  FileProperties prop = fs_getProp(file->path);
  return (prop.last_access_time != file->prop.last_access_time) ||
	 (prop.last_modification_time != file->prop.last_modification_time) ||
	 (prop.last_status_change_time != file->prop.last_status_change_time);
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

// =============================================================================
// File iteration
fn FilenameList fs_iterFiles(Arena *arena, String8 dirname) {
  const String8 currdir = Strlit(".");
  const String8 parentdir = Strlit("..");

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

    FilenameNode *node = (FilenameNode *) New(arena, FilenameNode);
    node->value = str;
    DLLPushBack(res.first, res.last, node);
  }

  (void)closedir(dir);
  return res;
}

fn bool fs_rmIter(Arena *temp_arena, String8 dirname) {
  const String8 currdir = Strlit(".");
  const String8 parentdir = Strlit("..");
  usize prev_head = temp_arena->head;

  FilenameList dirstack = {0};
  FilenameList deletable = {0};
  FilenameNode *root = (FilenameNode *) New(temp_arena, FilenameNode);
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
      String8 fullpath = strFormat(temp_arena, "%.*s/%.*s",
                                   Strexpand(current->value), Strexpand(str));

      if (entry->d_type == DT_DIR) {
        FilenameNode *childdir = (FilenameNode *) New(temp_arena, FilenameNode);
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

  temp_arena->head = prev_head;
  return res;
}
