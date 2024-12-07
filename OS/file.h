#pragma once

#include "../base.h"
#include "../list.h"

#include "../arena.h"
#include "../string.h"

#include "file_properties.h"

// =============================================================================
// File reading and writing/appending
fn String8 fs_read(Arena *arena, String8 filepath);

fn bool fs_write(String8 filepath, String8 content);
fn bool fs_writeStream(String8 filepath, StringStream content);

fn bool fs_append(String8 filepath, String8 content);
fn bool fs_appendStream(String8 filepath, StringStream content);

fn FileProperties fs_getProp(String8 filepath);

// =============================================================================
// Temporary files
fn String8 fs_makeTmpFile(Arena *arena);
fn String8 fs_writeTmpFile(Arena* arena, String8 content);

// =============================================================================
// Memory mapping files for easier and faster handling
typedef struct {
  i32 descriptor;
  String8 path;
  FileProperties prop;
  String8 content;
} File;

       fn File *fs_open(Arena *arena, String8 filepath, void *location);
inline fn void fs_sync(File *file, usize size);
inline fn void fs_close(File *file);

inline fn bool fs_hasChanged(File *file);

// =============================================================================
// Misc operation on the filesystem
inline fn bool fs_delete(String8 filepath);
inline fn bool fs_deleteFile(File *f);

inline fn bool fs_rename(String8 filepath, String8 to);
inline fn bool fs_renameFile(File *f, String8 to);

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
