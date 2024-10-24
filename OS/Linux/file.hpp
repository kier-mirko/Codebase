#pragma once

#include "../../base.h"
#include "../../list.h"

#include "../../arena.hpp"
#include "../../string.hpp"
#include "../../stringstream.hpp"

#include "../file_properties.hpp"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/types.h>

namespace OS::FS {
// =============================================================================
// File reading and writing/appending
fn Base::String8 *read(Base::Arena *arena, Base::String8 filepath);

       fn bool write(Base::String8 filepath, Base::String8 *content,
                     bool shouldAppend = false);
inline fn bool write(Base::String8 filepath, Base::String8 content,
                     bool shouldAppend = false);
       fn bool write(Base::String8 filepath, Base::StringStream *content,
                     bool shouldAppend = false);

fn Base::FileProperties getprop(Base::String8 filepath);

// =============================================================================
// Memory mapping files for easier and faster handling
struct File {
  Base::String8 path;
  i32 descriptor;
  Base::FileProperties prop;
  u8 *content;
};

fn File *open(Base::Arena *arena, Base::String8 filepath, void *location = 0);
inline fn void close(File *file);

// =============================================================================
// Misc operation on the filesystem

inline fn bool remove(Base::String8 filepath);
inline fn bool remove(File *f);

inline fn bool rename(Base::String8 filepath, Base::String8 to);
inline fn bool rename(File *f, Base::String8 to);

fn bool mkdir(Base::String8 path);

fn bool rmdir(Base::String8 path);

// =============================================================================
// File iteration
struct FilenameNode {
  Base::String8 value;
  FilenameNode *next;
  FilenameNode *prev;
};

struct FilenameList {
  FilenameNode *first;
  FilenameNode *last;
};

fn FilenameList iterFiles(Base::Arena *arena, Base::String8 dirname);

fn bool rmIter(Base::Arena *temp_arena, Base::String8 dirname);
}
