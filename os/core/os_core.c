fn OS_Handle
os_handle_zero(void)
{
  OS_Handle handle = {0};
  return handle;
}

fn B32
os_handle_match(OS_Handle a, OS_Handle b)
{
  return a.u64[0] == b.u64[0];
}
