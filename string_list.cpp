#ifndef CSTD_STRING_LIST
#define CSTD_STRING_LIST

#include "arena.cpp"
#include "string.cpp"

namespace cstd {
struct stringnode {
  stringnode *next;
  string value;

  stringnode *append(Arena *arena, string &other) {
    if (!this->next) [[likely]] {
      this->next = make<stringnode>(arena);
      this->next->value = other;
      return this->next;
    }

    return this->next->append(arena, other);
  }
};

struct stringlist {
  stringnode *first;
  stringnode *last;
  size_t size;

  string build(Arena *arena) {
    size_t final_len = 0, offset = 0;

    for (stringnode *curr = this->first; curr; curr = curr->next) {
      final_len += curr->value.size;
    }

    char *final_chars = make<char>(arena, final_len);
    for (stringnode *curr = this->first; curr; curr = curr->next) {
      for (size_t i = 0; i < curr->value.size; ++i) {
        *(final_chars + (offset++)) = curr->value[i];
      }
    }

    return string(final_len, final_chars);
  }

  void append(Arena *arena, string &other) {
    ++size;

    if (!this->last) {
      this->first = make<stringnode>(arena);
      this->last = this->first;
      this->last->value = other;
    } else [[likely]] {
      this->last = this->last->append(arena, other);
    }
  }
};
} // namespace cstd

// ?Maybe temporary?
std::ostream &operator<<(std::ostream &os, cstd::stringlist *s) {
  for (cstd::stringnode *curr = s->first; curr; curr = curr->next) {
    os << curr->value;
  }

  return os;
}
#endif
