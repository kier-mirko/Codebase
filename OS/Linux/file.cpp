#ifndef BASE_OS_FILE
#define BASE_OS_FILE

#include "../../arena.cpp"
#include "../../base.cpp"
#include "../../string.cpp"
#include "../../stringstream.cpp"

#include "../file_properties.cpp"

#include <fcntl.h>
#include <stdio.h>
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

inline fn bool write(String8 filepath, String8 content, bool shouldAppend = false) {
  return write(filepath, &content, shouldAppend);
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

fn FileProperties getprop(String8 filepath) {
  if (filepath.size == 0) {
    return {0};
  }

  struct stat file_stat;
  if (::stat((char *)filepath.str, &file_stat) < 0) {
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
// Memory mapping files for easier and faster handling
struct File {
  String8 path;
  i32 descriptor;
  FileProperties prop;
  u8 *content;
};

fn File *open(Arena *arena, String8 filepath, void *location = 0) {
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
inline fn bool remove(String8 filepath) {
  Assert(filepath.size != 0);
  return ::unlink((char *)filepath.str) >= 0;
}

inline fn bool rename(String8 filepath, String8 to) {
  Assert(filepath.size != 0 && to.size != 0);
  return ::rename((char *)filepath.str, (char *)to.str) >= 0;
}

inline fn bool remove(File *f) {
  Assert(f && f->path.size != 0);
  return ::unlink((char *)f->path.str) >= 0;
}

inline fn bool rename(File *f, String8 to) {
  Assert(f && f->path.size != 0 && to.size != 0);
  return ::rename((char *)f->path.str, (char *)to.str) >= 0;
}

fn bool mkdir(String8 path) {
  Assert(path.size != 0);
  return ::mkdir((char *)path.str, S_IRWXU | (S_IRGRP | S_IXGRP) | (S_IROTH | S_IXOTH)) >= 0;
}

fn bool rmdir(String8 path) {
  Assert(path.size != 0);
  return ::rmdir((char *)path.str) >= 0;
}

// TODO: Implement something to iterate over all the files in a given directory
// (ignoring the `.` and `..` file)

// TODO: Use said file iteration to delete all the files in a directory
// recursively (/not actually recursively/)
} // namespace Base::OS::FS

#endif
