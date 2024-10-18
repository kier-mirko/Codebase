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
} // namespace Base

fn Base::String8 stringstream_str(Base::Arena *arena,
                                  Base::StringStream *stream) {
  size_t final_len = 0, offset = 0;

  for (Base::StringNode *curr = stream->first; curr; curr = curr->next) {
    final_len += curr->value.size;
  }

  u8 *final_chars = makearr(arena, u8, final_len);
  for (Base::StringNode *curr = stream->first; curr; curr = curr->next) {
    for (size_t i = 0; i < curr->value.size; ++i) {
      *(final_chars + (offset++)) = curr->value[i];
    }
  }

  return Base::String8{.str = final_chars, .size = final_len};
}

fn void stringstream_append(Base::Arena *arena, Base::StringStream *strlist,
                            Base::String8 other) {
  Assert(arena);
  Assert(strlist);
  ++strlist->size;

  if (!strlist->last) {
    strlist->first = strlist->last = make(arena, Base::StringNode);
    strlist->last->value = other;
  } else [[likely]] {
    strlist->last->next = make(arena, Base::StringNode);
    strlist->last = strlist->last->next;
    strlist->last->value = other;
  }
}

#endif
