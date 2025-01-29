#include <dlfcn.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>

global LNX_State lnx_state = {0};

fn LNX_Primitive* lnx_primitiveAlloc(LNX_PrimitiveType type) {
  pthread_mutex_lock(&lnx_state.primitive_lock);
  LNX_Primitive *res = lnx_state.primitive_freelist;
  if (res) {
    StackPop(lnx_state.primitive_freelist);
  } else {
    res = New(lnx_state.arena, LNX_Primitive);
  }
  pthread_mutex_unlock(&lnx_state.primitive_lock);

  res->type = type;
  return res;
}

fn void lnx_primitiveFree(LNX_Primitive *ptr) {
  pthread_mutex_lock(&lnx_state.primitive_lock);
  StackPush(lnx_state.primitive_freelist, ptr);
  pthread_mutex_unlock(&lnx_state.primitive_lock);
}

fn void* lnx_threadEntry(void *args) {
  LNX_Primitive *wrap_args = (LNX_Primitive *)args;
  ThreadFunc *func = (ThreadFunc *)wrap_args->thread.func;
  func(wrap_args->thread.args);
  return 0;
}

fn String8 lnx_gethostname() {
  char name[HOST_NAME_MAX];
  (void)gethostname(name, HOST_NAME_MAX);

  String8 namestr = {
    .str = (u8 *)name,
    .size = str8len(name),
  };
  return namestr;
}

fn void lnx_parseMeminfo() {
  OS_Handle meminfo = fs_open(Strlit("/proc/meminfo"), OS_acfRead);
  StringStream lines = strSplit(lnx_state.arena, fs_readVirtual(lnx_state.arena, meminfo, 4096), '\n');
  for (StringNode *curr_line = lines.first; curr_line; curr_line = curr_line->next) {
    StringStream ss = strSplit(lnx_state.arena, curr_line->value, ':');
    for (StringNode *curr = ss.first; curr; curr = curr->next) {
      if (strEq(curr->value, Strlit("MemTotal"))) {
	curr = curr->next;
	lnx_state.info.total_memory = KiB(1) *
				u64FromStr(strSplit(lnx_state.arena, strTrim(curr->value),
						    ' ').first->value);
      } else if (strEq(curr->value, Strlit("Hugepagesize"))) {
	curr = curr->next;
	lnx_state.info.hugepage_size = KiB(1) *
				 u64FromStr(strSplit(lnx_state.arena, strTrim(curr->value),
						     ' ').first->value);
	return;
      }
    }
  }
}

fn OS_SystemInfo *os_getSystemInfo() {
  return &lnx_state.info;
}

fn void os_sleep(usize ms) {
  usleep(ms * 1000);
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
  // TODO(lb): MAP_HUGETLB vs MAP_HUGE_2MB/MAP_HUGE_1GB?
  void *res = mmap((void *)base_addr, size, PROT_NONE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
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

fn OS_Handle os_thread_start(ThreadFunc *thread_main, void *args) {
  Assert(thread_main);

  LNX_Primitive *prim = lnx_primitiveAlloc(LNX_Primitive_Thread);
  prim->thread.func = thread_main;
  prim->thread.args = args;

  if (pthread_create(&prim->thread.handle, 0, lnx_threadEntry, prim) != 0) {
    lnx_primitiveFree(prim);
    prim = 0;
  }

  OS_Handle res = {(u64)prim};
  return res;
}

fn void os_thread_kill(OS_Handle thd_handle) {
  LNX_Primitive *prim = (LNX_Primitive *)thd_handle.h[0];
  (void)pthread_kill(prim->thread.handle, 0);
}

fn bool os_thread_wait(OS_Handle thd_handle) {
  LNX_Primitive *prim = (LNX_Primitive *)thd_handle.h[0];
  i32 res = pthread_join(prim->thread.handle, 0);
  lnx_primitiveFree(prim);
  return res;
}

fn OS_ProcHandle os_proc_spawn() {
  LNX_Primitive *prim = lnx_primitiveAlloc(LNX_Primitive_Process);
  prim->proc = fork();

  OS_ProcHandle res = {prim->proc == 0, {(u64)prim}};
  return res;
}

fn void os_proc_kill(OS_ProcHandle proc) {
  Assert(!proc.is_child);
  (void)kill(((LNX_Primitive *)proc.handle.h[0])->proc, SIGKILL);
}

fn void os_proc_join(OS_ProcHandle proc) {
  Assert(!proc.is_child);
  LNX_Primitive *prim = (LNX_Primitive *)proc.handle.h[0];
  i32 child_res;
  (void)waitpid(prim->proc, &child_res, 0);
  lnx_primitiveFree(prim);
}

fn OS_Handle os_mutex_alloc() {
  LNX_Primitive *prim = lnx_primitiveAlloc(LNX_Primitive_Mutex);
  pthread_mutexattr_t attr;
  if (pthread_mutexattr_init(&attr) != 0 ||
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0) {
    lnx_primitiveFree(prim);
    prim = 0;
  } else {
    (void)pthread_mutex_init(&prim->mutex, &attr);
  }

  OS_Handle res = {(u64)prim};
  return res;
}

fn bool os_mutex_lock(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  return pthread_mutex_lock(&prim->mutex) == 0;
}

fn bool os_mutex_trylock(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  return pthread_mutex_trylock(&prim->mutex) == 0;
}

fn bool os_mutex_unlock(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  return pthread_mutex_unlock(&prim->mutex) == 0;
}

fn void os_mutex_free(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  pthread_mutex_destroy(&prim->mutex);
  lnx_primitiveFree(prim);
}

fn OS_Handle os_rwlock_alloc() {
  LNX_Primitive *prim = lnx_primitiveAlloc(LNX_Primitive_Rwlock);
  pthread_rwlock_init(&prim->rwlock, 0);

  OS_Handle res = {(u64)prim};
  return res;
}

fn bool os_rwlock_read_lock(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  return pthread_rwlock_rdlock(&prim->rwlock) == 0;
}

fn bool os_rwlock_read_trylock(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  return pthread_rwlock_tryrdlock(&prim->rwlock) == 0;
}

fn bool os_rwlock_read_unlock(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  return pthread_rwlock_unlock(&prim->rwlock) == 0;
}

fn bool os_rwlock_write_lock(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  return pthread_rwlock_wrlock(&prim->rwlock) == 0;
}

fn bool os_rwlock_write_trylock(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  return pthread_rwlock_trywrlock(&prim->rwlock) == 0;
}

fn bool os_rwlock_write_unlock(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  return pthread_rwlock_unlock(&prim->rwlock) == 0;
}

fn void os_rwlock_free(OS_Handle handle) {
  LNX_Primitive *prim = (LNX_Primitive *)handle.h[0];
  pthread_rwlock_destroy(&prim->rwlock);
  lnx_primitiveFree(prim);
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
  VoidFunc *result = (VoidFunc*)(u64)dlsym(handle, symbol_cstr);
  ScratchEnd(scratch);
  return result;
}

fn i32 os_lib_close(OS_Handle lib) {
  void *handle = (void*)lib.h[0];
  return dlclose(handle);
}

i32 main(i32 argc, char **argv) {
  lnx_state.info.core_count = get_nprocs();
  lnx_state.info.page_size = getpagesize();
  lnx_state.info.hostname = lnx_gethostname();

  lnx_state.arena = ArenaBuild();
  pthread_mutex_init(&lnx_state.primitive_lock, 0);
  lnx_parseMeminfo();

  CmdLine cli = {0};
  cli.count = argc - 1;
  cli.exe = strFromCstr(argv[0]);
  cli.args = New(lnx_state.arena, String8, argc - 1);
  for (isize i = 1; i < argc; ++i) {
    cli.args[i - 1] = strFromCstr(argv[i]);
  }

  start(&cli);
}
