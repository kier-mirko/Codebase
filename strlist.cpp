#ifndef BASE_STRING_LIST
#define BASE_STRING_LIST

#include "arena.cpp"
#include "string.cpp"

namespace base {
struct string8node {
  string8node *next;
  string8_t value;
};

struct string8list {
  string8node *first;
  string8node *last;
  size_t size;
};
} // namespace base

string8_t into_str8(base::Arena *arena, base::string8list *strlist) {
  size_t final_len = 0, offset = 0;

  for (base::string8node *curr = strlist->first; curr; curr = curr->next) {
    final_len += curr->value.size;
  }

  u8 *final_chars = makearr(arena, u8, final_len);
  for (base::string8node *curr = strlist->first; curr; curr = curr->next) {
    for (size_t i = 0; i < curr->value.size; ++i) {
      *(final_chars + (offset++)) = curr->value[i];
    }
  }

  return string8_t{.str = final_chars, .size = final_len};
}

void str8list_append(base::Arena *arena, base::string8list *strlist,
                     string8_t &other) {
  ++strlist->size;

  if (!strlist->last) {
    strlist->first = strlist->last = make(arena, base::string8node);
    strlist->last->value = other;
  } else [[likely]] {
    strlist->last = strlist->last->next = make(arena, base::string8node);
    strlist->last->value = other;
  }
}

#endif
