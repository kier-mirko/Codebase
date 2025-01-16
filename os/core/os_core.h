#ifndef BASE_OS_FILE
#define BASE_OS_FILE

typedef U32 OS_AccessFlags;
enum 
{
  ACF_Unknown = 0,
  ACF_Execute = 1 << 0,
  ACF_Read = 1 << 2,
  ACF_Write = 1 << 1,
};

typedef struct FileProperties FileProperties;
struct 
{
  U32 owner_id;
  U32 group_id;
  USZ size;
  
  U64 last_access_time;
  U64 last_modification_time;
  U64 last_status_change_time;
  
  union 
  {
    OS_AccessFlags permissions[3];
    
    struct 
    {
      OS_AccessFlags user;
      OS_AccessFlags group;
      OS_AccessFlags other;
    };
  };
};

// =============================================================================
// File reading and writing/appending
fn String8 os_file_read(Arena *arena, String8 filepath);

fn B32 os_file_write(String8 filepath, String8 content);
fn B32 os_file_write_list(String8 filepath, String8List content);

fn B32 os_file_append(String8 filepath, String8 content);
fn B32 os_file_append_list(String8 filepath, String8List content);

fn FileProperties os_file_get_pro(String8 filepath);

// =============================================================================
// Memory mapping files for easier and faster handling
typedef struct {
  I32 descriptor;
  String8 path;
  FileProperties prop;
  String8 content;
} File;

fn File fs_open(Arena *arena, String8 filepath);
fn File fs_openTmp(Arena *arena);

fn B32 fs_fileWrite(File *file, String8 content);
fn B32 fs_fileWriteStream(File *file, String8List content);
fn B32 fs_fileClose(File *file);

fn B32 fs_fileHasChanged(File *file);
fn B32 fs_fileErase(File *file);
fn B32 fs_fileRename(File *file, String8 to);

inline void fs_fileSync(File *file);
fn void fs_fileForceSync(File *file);

// =============================================================================
// Misc operation on the filesystem
inline B32 fs_delete(String8 filepath);
inline B32 fs_rename(String8 filepath, String8 to);

fn B32 os_make_dir(String8 path);
fn B32 os_remove_dir(String8 path);

// =============================================================================
// File iteration

fn String8List os_file_iter(Arena *arena, String8 dirname);
fn B32 os_file_remove_iter(Arena *temp_arena, String8 dirname);

// =============================================================================
//- Dynamic libraries

inline void *dynlib_open(String8 path);
inline void *dynlib_lookup(void *handle, String8 symbol);
inline void dynlib_close(void *handle);

#endif
