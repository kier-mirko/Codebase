#ifndef BASE_OS_FILE
#define BASE_OS_FILE

#include "../base.h"
#include "../arena.h"
#include "../string.h"

#include "../DataStructure/list.h"

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
// Memory mapping files for easier and faster handling
typedef struct {
  i32 descriptor;
  String8 path;
  FileProperties prop;
  String8 content;
} File;

fn File fs_open(Arena *arena, String8 filepath);
fn File fs_openTmp(Arena *arena);

fn bool fs_fileWrite(File *file, String8 content);
fn bool fs_fileWriteStream(File *file, StringStream content);
fn bool fs_fileClose(File *file);

fn bool fs_fileHasChanged(File *file);
fn bool fs_fileErase(File *file);
fn bool fs_fileRename(File *file, String8 to);

inline fn void fs_fileSync(File *file);
       fn void fs_fileForceSync(File *file);

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
fn bool fs_rmIter(String8 dirname);

#endif
