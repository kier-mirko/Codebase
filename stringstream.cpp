#ifndef BASE_STRING_LIST
#define BASE_STRING_LIST

#include "arena.cpp"
#include "string.cpp"

namespace base {
struct stringnode {
  stringnode *next;
  string8 value;
};

struct stringstream {
  stringnode *first;
  stringnode *last;
  size_t size;
};
} // namespace base

fn base::string8 stringstream_str(base::arena *arena, base::stringstream *stream) {
  size_t final_len = 0, offset = 0;

  for (base::stringnode *curr = stream->first; curr; curr = curr->next) {
    final_len += curr->value.size;
  }

  u8 *final_chars = makearr(arena, u8, final_len);
  for (base::stringnode *curr = stream->first; curr; curr = curr->next) {
    for (size_t i = 0; i < curr->value.size; ++i) {
      *(final_chars + (offset++)) = curr->value[i];
    }
  }

  return base::string8{.str = final_chars, .size = final_len};
}

fn void stringstream_append(base::arena *arena, base::stringstream *strlist,
                            base::string8 &other) {
  ++strlist->size;

  if (!strlist->last) {
    strlist->first = strlist->last = make(arena, base::stringnode);
    strlist->last->value = other;
  } else [[likely]] {
    strlist->last = strlist->last->next = make(arena, base::stringnode);
    strlist->last->value = other;
  }
}

#endif
