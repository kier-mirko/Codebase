global OS_SystemInfo lnx_info = {0};
global Arena *lnx_arena = 0;

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
  StringStream lines = strSplit(lnx_arena, fs_readVirtual(lnx_arena, meminfo, 4096), '\n');
  for (StringNode *curr_line = lines.first; curr_line; curr_line = curr_line->next) {
    StringStream ss = strSplit(lnx_arena, curr_line->value, ':');
    for (StringNode *curr = ss.first; curr; curr = curr->next) {
      if (strEq(curr->value, Strlit("MemTotal"))) {
	curr = curr->next;
	lnx_info.total_memory = KiB(1) *
				u64FromStr(strSplit(lnx_arena, strTrim(curr->value),
						    ' ').first->value);
      } else if (strEq(curr->value, Strlit("Hugepagesize"))) {
	curr = curr->next;
	lnx_info.hugepage_size = KiB(1) *
				 u64FromStr(strSplit(lnx_arena, strTrim(curr->value),
						     ' ').first->value);
	return;
      }
    }
  }
}

fn OS_SystemInfo *os_getSystemInfo() {
  return &lnx_info;
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

i32 main(i32 argc, char **argv) {
  lnx_info.core_count = get_nprocs();
  lnx_info.page_size = getpagesize();
  lnx_info.hostname = lnx_gethostname();

  lnx_arena = ArenaBuild();
  lnx_parseMeminfo();

  start(0);
}
