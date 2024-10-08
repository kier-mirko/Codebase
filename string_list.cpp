#ifndef BASE_STRING_LIST
#define BASE_STRING_LIST

#include "arena.cpp"
#include "string.cpp"

namespace base {
struct stringnode {
  stringnode *next;
  string_t value;

  stringnode *append(Arena *arena, string_t &other) {
    if (!this->next) [[likely]] {
      this->next = make(arena, stringnode);
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

  string_t build(Arena *arena) {
    size_t final_len = 0, offset = 0;

    for (stringnode *curr = this->first; curr; curr = curr->next) {
      final_len += curr->value.size;
    }

    char *final_chars = makearr(arena, char, final_len);
    for (stringnode *curr = this->first; curr; curr = curr->next) {
      for (size_t i = 0; i < curr->value.size; ++i) {
        *(final_chars + (offset++)) = curr->value[i];
      }
    }

    return string_t{.size = final_len, .cstr = final_chars};
  }

  void append(Arena *arena, string_t &other) {
    ++size;

    if (!this->last) {
      this->first = make(arena, stringnode);
      this->last = this->first;
      this->last->value = other;
    } else [[likely]] {
      this->last = this->last->append(arena, other);
    }
  }
};
} // namespace base

// ?Maybe temporary?
// std::ostream &operator<<(std::ostream &os, base::stringlist *s) {
//   for (base::stringnode *curr = s->first; curr; curr = curr->next) {
//     os << curr->value;
//   }

//   return os;
// }
#endif
