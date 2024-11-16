#include "csv.h"

fn StringStream csv_header(Arena *arena, CSV *config) {
  usize line_ends = strFindFirst(config->file->content, '\n');
  String8 header_line = strPrefix(config->file->content, line_ends);
  StringStream res = {0};

  config->offset += line_ends + 1;

  while (header_line.size) {
    usize i = strFindFirst(header_line, config->delimiter);
    stringstreamAppend(arena, &res, strPrefix(header_line, i));
    header_line = strPostfix(header_line, i + 1);
  }

  return res;
}

fn StringStream csv_nextRow(Arena *arena, CSV *config) {
  String8 content = strPostfix(config->file->content, config->offset);
  usize line_ends = strFindFirst(content, '\n');
  String8 header_line = strPrefix(content, line_ends);
  StringStream res = {0};

  config->offset += line_ends + 1;

  while (header_line.size) {
    usize i = strFindFirst(header_line, config->delimiter);
    stringstreamAppend(arena, &res, strPrefix(header_line, i));
    header_line = strPostfix(header_line, i + 1);
  }

  return res;
}
