#ifndef OS_CORE_WIN32_H
#define OS_CORE_WIN32_H

typedef u64 OS_W32_PrimitiveType;
enum
{
  OS_W32_Primitive_Nil,
  OS_W32_Primitive_Thread,
  OS_W32_Primitive_RWLock,
  OS_W32_Primitive_Mutex,
};

typedef struct OS_W32_Primitive OS_W32_Primitive;
struct OS_W32_Primitive
{
  OS_W32_PrimitiveType *next;
  OS_W32_PrimitiveType kind;
  union
  {
    struct
    {
      HANDLE handle;
      DWORD tid;
      ThreadFunc *func;
      void *arg;
    } thread;
    
    CRITICAL_SECTION mutex;
    SRWLOCK rw_mutex;
  }
};

typedef struct OS_W32_State OS_W32_State;
struct OS_W32_State
{
  Arena *arena;
  OS_W32_Thread *free_list;
  u64 pos;
};

typedef struct OS_W32_FileIter OS_W32_FileIter;
struct OS_W32_FileIter
{
  HANDLE handle;
  WIN32_FIND_DATAW file_data;
  bool done;
};

StaticAssert(sizeof(OS_W32_FileIter) <= sizeof(OS_FileIter), file_iter_size_check);

global OS_W32_State w32_state;
global OS_SystemInfo w32_info;


fn OS_W32_Primitive* os_w32_primitive_alloc(OS_W32_PrimitiveType kind);
fn void os_w32_primitive_release(OS_W32_Primitive *primitive);
fn DWORD os_w32_thread_entry_point(void *ptr);


#endif //OS_CORE_WIN32_H
