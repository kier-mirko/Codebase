#include "csv.h"

fn CSVRow csv_header(Arena *arena, File *csv, char delimiter) {
  usize line_ends = strFindFirst(csv->content, '\n');
  String8 header_line = strPrefix(csv->content, line_ends);
  StringStream res = {0};

  while (header_line.size) {
    usize i = strFindFirst(header_line, delimiter);
    stringstreamAppend(arena, &res, strPrefix(header_line, i));
    header_line = strPostfix(header_line, i + 1);
  }

  return (CSVRow) {
    .next_at = line_ends + 1,
    .delimiter = delimiter,
    .value = res,
    .file = csv,
  };
}

fn CSVRow csv_nextRow(Arena *arena, CSVRow prev) {
  String8 content = strPostfix(prev.file->content, prev.next_at);
  usize line_ends = strFindFirst(content, '\n');
  String8 header_line = strPrefix(content, line_ends);
  StringStream res = {0};

  while (header_line.size) {
    usize i = strFindFirst(header_line, prev.delimiter);
    stringstreamAppend(arena, &res, strPrefix(header_line, i));
    header_line = strPostfix(header_line, i + 1);
  }

  return (CSVRow) {
    .next_at = line_ends + prev.next_at + 1,
    .delimiter = prev.delimiter,
    .value = res,
    .file = prev.file,
  };
}
