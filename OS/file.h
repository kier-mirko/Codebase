#pragma once

#include "../base.h"
#include "../list.h"

#include "../arena.h"
#include "../string.h"

#include "file_properties.h"

// =============================================================================
// File reading and writing/appending
fn String8 fs_read(Arena *arena, String8 filepath);
fn String8 fs_fread(Arena *arena, isize fd);

fn isize fs_write(String8 filepath, String8 content);
fn isize fs_write(String8 filepath, StringStream content);

fn isize fs_append(String8 filepath, String8 content);
fn isize fs_append(String8 filepath, StringStream content);

fn FileProperties fs_getProp(String8 filepath);

// =============================================================================
// Memory mapping files for easier and faster handling
typedef struct {
  i32 descriptor;
  String8 path;
  FileProperties prop;
  String8 content;

  bool write(const String8 &content);
  bool write(const StringStream &content);
  bool close();

  bool hasChanged();
  bool erase();
  bool rename(String8 to);

  void sync();
} File;

fn File fs_open(Arena *arena, void *location = 0);
fn File fs_open(Arena *arena, String8 filepath, void *location = 0);

// =============================================================================
// Misc operation on the filesystem
inline fn bool fs_delete(String8 filepath);
inline fn bool fs_rename(String8 filepath, String8 to);

fn bool fs_mkdir(String8 path);
fn bool fs_rmdir(String8 path);

// =============================================================================
// File iteration
typedef struct FilenameNode {
  String8 value;
  struct FilenameNode *next;
  struct FilenameNode *prev;
} FilenameNode;

typedef struct {
  FilenameNode *first;
  FilenameNode *last;
} FilenameList;

fn FilenameList fs_iterFiles(Arena *arena, String8 dirname);
fn bool fs_rmIter(Arena *temp_arena, String8 dirname);
