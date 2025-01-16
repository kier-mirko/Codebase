#ifndef BASE_SERIALIZER_CSV
#define BASE_SERIALIZER_CSV

typedef struct CSV CSV;
struct CSV {
  char delimiter;
  File file;
  
  USZ offset;
};

fn String8List csv_header(Arena *arena, CSV *config);
fn String8List csv_next_row(Arena *arena, CSV *config);

#endif
