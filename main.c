#include "base/base_inc.h"
#include "os/os_inc.h"

#include "base/base_inc.c"
#include "os/os_inc.c"

int
main(void)
{
  Arena *arena = arena_alloc(GB(10),0);
  
  int *x = make(arena, int, 10);
  
  OS_Handle handle = os_file_open(OS_AccessFlag_Read | OS_AccessFlag_Append, str8_lit("test.txt"));
  String8 content = os_file_read(arena, handle);
  
  String8 y = str8_lit("Hello, World\n");
  os_file_write(handle, y);
  
  for(;;){}
  return 0;
}
