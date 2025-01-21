#include "csv.h"

inline fn StringStream csv_header(Arena *arena, CSV *config) {
  return csv_nextRow(arena, config);
}

fn StringStream csv_nextRow(Arena *arena, CSV *config) {
  String8 file_content = str8(config->file.content, config->file.prop.size);
  if (file_content.size <= config->offset) {
    return (StringStream) {0};
  }

  String8 content = strPostfix(file_content, config->offset);
  usize line_ends = strFindFirst(content, '\n');
  String8 row = strPrefix(content, line_ends);

  config->offset += line_ends + 1;

  return strSplit(arena, row, config->delimiter);
}

// TODO: write to CSV file
fn void csv_write(String8 path, char delimiter, ...);
