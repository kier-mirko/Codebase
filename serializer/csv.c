inline String8List csv_header(Arena *arena, CSV *config) {
  return csv_next_row(arena, config);
}

fn String8List csv_next_row(Arena *arena, CSV *config) {
  if (config->file.content.size <= config->offset) {
    return (String8List) {0};
  }
  
  String8 content = str8_postfix(config->file.content, config->offset);
  USZ line_ends = str8_find_first(content, '\n');
  String8 row = str8_prefix(content, line_ends);
  
  config->offset += line_ends + 1;
  
  return str8_split(arena, row, config->delimiter);
}

// TODO: write to CSV file
fn void csv_write(String8 path, char delimiter, ...);
