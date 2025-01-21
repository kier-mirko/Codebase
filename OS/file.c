fn OS_Handle os_handleZero(void) {
  OS_Handle handle = {0};
  return handle;
}

fn bool os_handleEq(OS_Handle a, OS_Handle b) {
  return a.fd[0] == b.fd[0];
}

fn bool fs_writeStream(OS_Handle file, StringStream content) {
  if(os_handleEq(file, os_handleZero())) { return false; }
  
  bool result = true;
  for(StringNode *node = content.first; node; node = node->next) {
    if(fs_write(file, node->value) == false) {
      result = false;
      break;
    }
  }
  return result;
}
