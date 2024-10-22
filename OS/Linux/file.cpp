#ifndef BASE_OS_FILE
#define BASE_OS_FILE

#include "../../arena.cpp"
#include "../../base.cpp"
#include "../../list.cpp"
#include "../../string.cpp"
#include "../../stringstream.cpp"

#include "../file_properties.cpp"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/types.h>

namespace OS::FS {
// =============================================================================
// File reading and writing/appending
fn Base::String8 *read(Base::Arena *arena, Base::String8 filepath) {
  Assert(arena);
  if (filepath.size == 0) {
    return 0;
  }

  i32 fd = ::open((char *)filepath.str, O_RDONLY);
  if (fd < 0) {
    (void)::close(fd);
    return 0;
  }

  struct stat file_stat;
  if (::stat((char *)filepath.str, &file_stat) < 0) {
    (void)::close(fd);
    return 0;
  }

  Base::String8 *res = make(arena, Base::String8);
  res->str = makearr(arena, u8, file_stat.st_size);
  res->size = ::read(fd, res->str, file_stat.st_size);

  (void)::close(fd);
  return res;
}

fn bool write(Base::String8 filepath, Base::String8 *content,
              bool shouldAppend = false) {
  if (filepath.size == 0) {
    return false;
  }

  i32 fd = ::open((char *)filepath.str,
                  O_WRONLY | O_CREAT | (shouldAppend ? O_APPEND : O_TRUNC),
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0) {
    (void)::close(fd);
    return false;
  }

  if (::write(fd, content->str, content->size) != content->size) {
    (void)::close(fd);
    return false;
  }

  (void)::close(fd);
  return true;
}

inline fn bool write(Base::String8 filepath, Base::String8 content,
                     bool shouldAppend = false) {
  return write(filepath, &content, shouldAppend);
}

fn bool write(Base::String8 filepath, Base::StringStream *content,
              bool shouldAppend = false) {
  for (Base::StringNode *start = content->first;
       start < content->first + content->size; ++start) {
    if (!write(filepath, &start->value, shouldAppend)) {
      return false;
    }

    if (!shouldAppend) {
      shouldAppend = true;
    }
  }

  return true;
}

fn Base::FileProperties getprop(Base::String8 filepath) {
  using namespace Base;
  if (filepath.size == 0) {
    return {0};
  }

  struct stat file_stat;
  if (::stat((char *)filepath.str, &file_stat) < 0) {
    return {0};
  }

  AccessFlag user_permissions = AccessFlag::Unknown;
  if (file_stat.st_mode & S_IRUSR) {
    user_permissions = AccessFlag::Read;
  }
  if (file_stat.st_mode & S_IWUSR) {
    user_permissions = (AccessFlag)(user_permissions | AccessFlag::Write);
  }
  if (file_stat.st_mode & S_IXUSR) {
    user_permissions = (AccessFlag)(user_permissions | AccessFlag::Execute);
  }

  AccessFlag group_permissions = AccessFlag::Unknown;
  if (file_stat.st_mode & S_IRGRP) {
    group_permissions = AccessFlag::Read;
  }
  if (file_stat.st_mode & S_IWGRP) {
    group_permissions = (AccessFlag)(user_permissions | AccessFlag::Write);
  }
  if (file_stat.st_mode & S_IXGRP) {
    group_permissions = (AccessFlag)(user_permissions | AccessFlag::Execute);
  }

  AccessFlag other_permissions = AccessFlag::Unknown;
  if (file_stat.st_mode & S_IROTH) {
    other_permissions = AccessFlag::Read;
  }
  if (file_stat.st_mode & S_IWOTH) {
    other_permissions = (AccessFlag)(user_permissions | AccessFlag::Write);
  }
  if (file_stat.st_mode & S_IXOTH) {
    other_permissions = (AccessFlag)(user_permissions | AccessFlag::Execute);
  }

  return {
      .ownerID = file_stat.st_uid,
      .groupID = file_stat.st_gid,
      .byte_size = (size_t)file_stat.st_size,

      .last_access_time = (u64)file_stat.st_atime,
      .last_modification_time = (u64)file_stat.st_mtime,
      .last_status_change_time = (u64)file_stat.st_ctime,

      .user = user_permissions,
      .group = group_permissions,
      .other = other_permissions,
  };
}

// =============================================================================
// Memory mapping files for easier and faster handling
struct File {
  Base::String8 path;
  i32 descriptor;
  Base::FileProperties prop;
  u8 *content;
};

fn File *open(Base::Arena *arena, Base::String8 filepath, void *location = 0) {
  Assert(arena);
  if (filepath.size == 0) {
    return 0;
  }

  i32 fd = ::open((char *)filepath.str, O_RDONLY);
  if (fd < 0) {
    (void)::close(fd);
    return 0;
  }

  File *memfile = make(arena, File);
  memfile->path = filepath;
  memfile->descriptor = fd;
  memfile->prop = getprop(filepath);
  memfile->content = (u8 *)mmap(location, memfile->prop.byte_size, PROT_READ,
                                MAP_PRIVATE, fd, 0);

  (void)::close(fd);
  return memfile;
}

inline fn void close(File *file) {
  Assert(file);
  (void)::munmap(file->content, file->prop.byte_size);
  (void)::close(file->descriptor);
}

// =============================================================================
// Misc operation on the filesystem
inline fn bool remove(Base::String8 filepath) {
  Assert(filepath.size != 0);
  return ::unlink((char *)filepath.str) >= 0;
}

inline fn bool rename(Base::String8 filepath, Base::String8 to) {
  Assert(filepath.size != 0 && to.size != 0);
  return ::rename((char *)filepath.str, (char *)to.str) >= 0;
}

inline fn bool remove(File *f) {
  Assert(f && f->path.size != 0);
  return ::unlink((char *)f->path.str) >= 0;
}

inline fn bool rename(File *f, Base::String8 to) {
  Assert(f && f->path.size != 0 && to.size != 0);
  return ::rename((char *)f->path.str, (char *)to.str) >= 0;
}

fn bool mkdir(Base::String8 path) {
  Assert(path.size != 0);
  return ::mkdir((char *)path.str,
                 S_IRWXU | (S_IRGRP | S_IXGRP) | (S_IROTH | S_IXOTH)) >= 0;
}

fn bool rmdir(Base::String8 path) {
  Assert(path.size != 0);
  return ::rmdir((char *)path.str) >= 0;
}

struct FilenameNode {
  Base::String8 value;
  FilenameNode *next;
  FilenameNode *prev;
};

struct FilenameList {
  FilenameNode *first;
  FilenameNode *last;
};

fn FilenameList iterFiles(Base::Arena *arena, Base::String8 dirname) {
  using namespace Base;

  FilenameList res{0};

  DIR *dir = opendir((char *)dirname.str);
  if (!dir) {
    return res;
  }

  struct dirent *entry;
  while ((entry = readdir(dir))) {
    String8 str = str8((u8 *)entry->d_name);
    if (str == strlit(".") || str == strlit("..")) {
      continue;
    }

    FilenameNode *node = make(arena, FilenameNode);
    node->value = str;
    DLLPushBack(res.first, res.last, node);
  }

  (void)closedir(dir);
  return res;
}

fn bool rmIter(Base::Arena *arena, Base::String8 dirname) {
  using namespace Base;

  DIR *dir = opendir((char *)dirname.str);
  if (!dir) {
    return false;
  }

  void *prev_head = arena->head;
  struct dirent *entry;
  while ((entry = readdir(dir))) {
    String8 str = str8((u8 *)entry->d_name);
    if (str == strlit(".") || str == strlit("..")) {
      continue;
    } else if (!rmIter(arena, str)) {
      StringStream fullpath = {0};
      stringstreamAppend(arena, &fullpath, dirname);
      stringstreamAppend(arena, &fullpath, strlit("/"));
      stringstreamAppend(arena, &fullpath, str);

      if (!remove(str8FromStream(arena, &fullpath))) {
	arena->head = prev_head;
	return false;
      }
    }
  }

  arena->head = prev_head;
  (void)closedir(dir);
  return rmdir(dirname);
}

} // namespace OS::FS

#endif
