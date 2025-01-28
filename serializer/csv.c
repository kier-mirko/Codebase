#include "csv.h"

inline fn StringStream csv_header(Arena *arena, CSV *config) {
  return csv_nextRow(arena, config);
}

fn StringStream csv_nextRow(Arena *arena, CSV *config) {
  StringStream res = {0};
  String8 file_content = str8(config->file.content, config->file.prop.size);
  if (file_content.size <= config->offset) {
    return res;
  }

  String8 content = strPostfix(file_content, config->offset);
  usize line_ends = strFindFirst(content, '\n');
  String8 row = strPrefix(content, line_ends);

  config->offset += line_ends + 1;

  return strSplit(arena, row, config->delimiter);
}
