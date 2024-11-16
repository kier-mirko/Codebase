#ifndef BASE_SERIALIZER_CSV
#define BASE_SERIALIZER_CSV

#include "../base.h"
#include "../arena.h"
#include "../string.h"
#include "../OS/file.h"

typedef struct CSVRow {
  usize next_at;
  char delimiter;
  StringStream value;
  File *file;
} CSVRow;

fn CSVRow csv_header(Arena *arena, File *csv, char delimiter);
fn CSVRow csv_nextRow(Arena *arena, CSVRow prev);

#endif
