#include <sys/sysctl.h>
#include <sys/param.h>

#include <unistd.h>
#include <dlfcn.h>

global Arena *bsd_arena = 0;
global OS_SystemInfo bsd_info = {0};
global String8 bsd_filemap[MEMFILES_ALLOWED] = {0};

fn void* bsd_thdEntry(void *args) {
  bsd_thdData *wrap_args = (bsd_thdData *)args;
  ThreadFunc *func = (ThreadFunc *)wrap_args->func;
  func(wrap_args->args);
  return 0;
}

fn OS_Handle os_thdSpawn(ThreadFunc *thread_main, void *args) {
  Assert(thread_main);

  Scratch scr = ScratchBegin(0, 0);
  bsd_thdData *wrap_args = New(scr.arena, bsd_thdData);
  wrap_args->func = thread_main;
  wrap_args->args = args;

  OS_Handle res = {0};
  pthread_t *thread_id = (pthread_t *)&res.h;
  i32 maybeErr = pthread_create(thread_id, 0, bsd_thdEntry, wrap_args);
  ScratchEnd(scr);

  if (maybeErr == 0) {
    return res;
  } else {
    res.h[0] = 0;
    return res;
  }
}

inline fn void os_thdJoin(OS_Handle handle, void **return_buff) {
  (void)pthread_join((pthread_t)handle.h[0], return_buff);
}

fn OS_Handle os_lib_open(String8 path) {
  OS_Handle result = {0};
  Scratch scratch = ScratchBegin(0, 0);
  char *path_cstr = strToCstr(scratch.arena, path);
  void *handle = dlopen(path_cstr, RTLD_LAZY);
  if(handle){
    result.h[0] = (u64)handle;
  }
  ScratchEnd(scratch);
  return result;
}

fn VoidFunc *os_lib_lookup(OS_Handle lib, String8 symbol) {
  Scratch scratch = ScratchBegin(0, 0);
  void *handle = (void*)lib.h[0];
  char *symbol_cstr = strToCstr(scratch.arena, symbol);
  VoidFunc *result = (VoidFunc*)(usize)dlsym(handle, symbol_cstr);
  ScratchEnd(scratch);
  return result;
}

fn i32 os_lib_close(OS_Handle lib) {
  void *handle = (void*)lib.h[0];
  return dlclose(handle);
}

fn OS_SystemInfo *os_getSystemInfo() {
  return &bsd_info;
}

fn String8 bsd_gethostname() {
  char name[MAXHOSTNAMELEN];
  (void)gethostname(name, MAXHOSTNAMELEN);

  String8 namestr = {
    .str = (u8 *)name,
    .size = str8len(name),
  };
  return namestr;
}

fn void* os_reserve(usize base_addr, usize size) {
  void *res = mmap((void *)base_addr, size, PROT_NONE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (res == MAP_FAILED) {
    res = 0;
  }

  return res;
}

fn void* os_reserveHuge(usize base_addr, usize size) {
  void *res = mmap((void *)base_addr, size, PROT_NONE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_ALIGNED_SUPER, -1, 0);
  if (res == MAP_FAILED) {
    res = 0;
  }

  return res;
}

fn void os_release(void *base, usize size) {
  (void)munmap(base, size);
}

fn void os_commit(void *base, usize size) {
  (void)mprotect(base, size, PROT_READ | PROT_WRITE);
}

fn void os_decommit(void *base, usize size) {
  (void)mprotect(base, size, PROT_NONE);
  (void)madvise(base, size, MADV_DONTNEED);
}

i32 main(i32 argc, char **argv) {
  i32 mib[2] = { CTL_HW, HW_NCPU };
  usize len = sizeof(bsd_info.core_count);
  sysctl(mib, 2, &bsd_info.core_count, &len, 0, 0);

  mib[1] = HW_PHYSMEM;
  len = sizeof(bsd_info.total_memory);
  sysctl(mib, 2, &bsd_info.total_memory, &len, 0, 0);

  usize page_sizes[3] = {0};
  getpagesizes(page_sizes, 3);
  bsd_info.page_size = page_sizes[0];
#ifdef USE_SUPERLARGE_PAGES
  bsd_info.hugepage_size = page_sizes[2];
#else
  bsd_info.hugepage_size = page_sizes[1];
#endif

  bsd_info.hostname = bsd_gethostname();
  bsd_arena = ArenaBuild();

  CmdLine cli = {0};
  cli.count = argc - 1;
  cli.exe = strFromCstr(argv[0]);
  cli.args = New(bsd_arena, String8, argc - 1);
  for (isize i = 1; i < argc; ++i) {
    cli.args[i - 1] = strFromCstr(argv[i]);
  }

  start(&cli);
}
