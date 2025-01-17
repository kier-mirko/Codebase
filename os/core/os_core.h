#ifndef BASE_OS_FILE
#define BASE_OS_FILE

typedef struct OS_Handle OS_Handle;
struct OS_Handle
{
  U64 u64[1];
};

typedef U32 OS_AccessFlags;
enum 
{
  OS_AccessFlag_Read       = (1<<0),
  OS_AccessFlag_Write      = (1<<1),
  OS_AccessFlag_Execute    = (1<<2),
  OS_AccessFlag_Append     = (1<<3),
  OS_AccessFlag_ShareRead  = (1<<4),
  OS_AccessFlag_ShareWrite = (1<<5),
};

typedef struct FileProperties FileProperties;
struct FileProperties
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
fn OS_Handle os_handle_zero(void);
fn B32 os_handle_match(OS_Handle a, OS_Handle b);
fn OS_Handle os_file_open(OS_AccessFlags flags, String8 filepath);
fn String8 os_file_read(Arena *arena, OS_Handle file);

fn B32 os_file_write(OS_Handle file, String8 content);
fn B32 os_file_write_list(OS_Handle file, String8List content);


fn FileProperties os_file_get_properties(OS_Handle file);

fn void os_file_close(OS_Handle file);
// =============================================================================
// Memory mapping files for easier and faster handling
typedef struct File File;
struct File 
{
  I32 descriptor;
  String8 path;
  FileProperties prop;
  String8 content;
};

fn File os_file_open_mapped(Arena *arena, String8 filepath);
fn File os_file_open_mappedTmp(Arena *arena);

fn B32 os_file_map_write(File *file, String8 content);
fn B32 os_file_map_write_list(File *file, String8List content);
fn B32 os_file_map_close(File *file);

fn B32 os_file_map_has_changed(File *file);
fn B32 os_file_map_erase(File *file);
fn B32 os_file_map_rename(File *file, String8 to);

inline void os_file_map_sync(File *file);
fn void os_file_map_force_sync(File *file);

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
