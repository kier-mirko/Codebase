#ifndef BASE_OS_FILE
#define BASE_OS_FILE

#include "../base.h"
#include "../arena.h"
#include "../string.h"

#include "../DataStructure/list.h"

#include "file_properties.h"

typedef u32 OS_AccessFlags;
enum {
  OS_AccessFlag_Read       = (1<<0),
  OS_AccessFlag_Write      = (1<<1),
  OS_AccessFlag_Execute    = (1<<2),
  OS_AccessFlag_Append     = (1<<3),
  OS_AccessFlag_ShareRead  = (1<<4),
  OS_AccessFlag_ShareWrite = (1<<5),
};

typedef struct OS_Handle OS_Handle;
struct OS_Handle {
  u64 fd[1];
};

fn OS_Handle os_handleZero(void);
fn bool os_handleEq(OS_Handle a, OS_Handle b);

// =============================================================================
// File reading and writing/appending
fn OS_Handle fs_open(String8 filepath, OS_AccessFlags flags);
fn String8 fs_read(Arena *arena, OS_Handle file);

fn bool fs_write(OS_Handle file, String8 content);
fn bool fs_writeStream(OS_Handle file, StringStream content);

fn FileProperties fs_getProp(String8 filepath);

// =============================================================================
// Memory mapping files for easier and faster handling

typedef struct {
  OS_Handle handle;
  String8 path;
  FileProperties prop;
  String8 content;
} File;

fn File fs_fileOpen(Arena *arena, String8 filepath);
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
