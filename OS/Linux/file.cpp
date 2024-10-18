#ifndef BASE_OS_FILE
#define BASE_OS_FILE

#include "../../arena.cpp"
#include "../../base.cpp"
#include "../../string.cpp"
#include "../../stringstream.cpp"

#include "../file_properties.cpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Base::OS::FS {
// =============================================================================
// File reading and writing/appending
fn String8 *read(Arena *arena, String8 filepath) {
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

  String8 *res = make(arena, String8);
  res->str = makearr(arena, u8, file_stat.st_size);
  res->size = ::read(fd, res->str, file_stat.st_size);

  (void)::close(fd);
  return res;
}

fn bool write(String8 filepath, String8 *content, bool shouldAppend = false) {
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

fn bool write(String8 filepath, StringStream *content,
              bool shouldAppend = false) {
  for (StringNode *start = content->first;
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

// =============================================================================
// Memory mapping files for easier and faster handling
// TODO: implement `file_open` and `file_close` for memory mapping a file

// =============================================================================
// Getting file properties
fn FileProperties getprop(String8 filepath) {
  if (filepath.size == 0) {
    return {0};
  }

  i32 fd = ::open((char *)filepath.str, O_RDONLY);
  if (fd < 0) {
    (void)::close(fd);
    return {0};
  }

  struct stat file_stat;
  if (::stat((char *)filepath.str, &file_stat) < 0) {
    (void)::close(fd);
    return {0};
  }

  AccessFlag mode = AccessFlag::Unknown;
  if (file_stat.st_mode & S_IRUSR) {
    mode = AccessFlag::Read;
  }
  if (file_stat.st_mode & S_IWUSR) {
    mode = (AccessFlag)(mode | AccessFlag::Write);
  }
  if (file_stat.st_mode & S_IXUSR) {
    mode = (AccessFlag)(mode | AccessFlag::Execute);
  }

  return {
      .ownerID = file_stat.st_uid,
      .groupID = file_stat.st_gid,
      .permissions = mode,
      .byte_size = (size_t)file_stat.st_size,

      .last_access_time = (u64)file_stat.st_atime,
      .last_modification_time = (u64)file_stat.st_mtime,
      .last_status_change_time = (u64)file_stat.st_ctime,
  };
}

// =============================================================================
// Misc operation on the filesystem
fn bool remove(String8 filepath) {
  // TODO: Implement FS remove `filepath`
  return true;
}

fn bool rename(String8 filepath, String8 to) {
  // TODO: Implement FS rename `filepath` to `to`
  return true;
}

fn bool mkdir(String8 path) {
  // TODO: Implement FS make directory `path`
  return true;
}

fn bool rmdir(String8 path) {
  // TODO: Implement FS remove directory `path`
  return true;
}

// TODO: Implement something to iterate over all the files in a given directory
// (ignoring the `.` and `..` file)
} // namespace Base::OS::FS

#endif
