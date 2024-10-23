#ifndef BASE_STRING_LIST
#define BASE_STRING_LIST

#include "arena.cpp"
#include "string.cpp"

namespace Base {
struct StringNode {
  StringNode *next;
  String8 value;
};

struct StringStream {
  StringNode *first;
  StringNode *last;
  size_t size;
};

fn String8 str8FromStream(Arena *arena, StringStream *stream) {
  size_t final_len = 0, offset = 0;

  for (StringNode *curr = stream->first; curr; curr = curr->next) {
    final_len += curr->value.size;
  }

  u8 *final_chars = Makearr(arena, u8, final_len);
  for (StringNode *curr = stream->first; curr; curr = curr->next) {
    for (size_t i = 0; i < curr->value.size; ++i) {
      *(final_chars + (offset++)) = curr->value[i];
    }
  }

  return String8{.str = final_chars, .size = final_len};
}

fn void stringstreamAppend(Arena *arena, StringStream *strlist,
                            String8 other) {
  Assert(arena);
  Assert(strlist);
  ++strlist->size;

  if (!strlist->last) {
    strlist->first = strlist->last = Make(arena, StringNode);
    strlist->last->value = other;
  } else [[likely]] {
    strlist->last->next = Make(arena, StringNode);
    strlist->last = strlist->last->next;
    strlist->last->value = other;
  }
}
} // namespace Base

#endif
