#ifndef BASE_SERIALIZER_CSV
#define BASE_SERIALIZER_CSV

typedef struct {
  char delimiter;
  File file;
  
  isize offset;
} CSV;

fn StringStream csv_header(Arena *arena, CSV *config);
fn StringStream csv_nextRow(Arena *arena, CSV *config);

#endif
