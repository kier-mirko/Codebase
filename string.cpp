#ifndef BASE_STRING
#define BASE_STRING

#include "base.cpp"

#define STR(cstr) (string_t{arrsize(cstr)-1, cstr})

// `size` and `cstr` are to be considered immutable
struct string_t {
  size_t size;
  const char *cstr;

  constexpr char operator[](size_t idx) const {
    return idx > this->size ? 0 : this->cstr[idx];
  }

  constexpr bool operator==(string_t &other) const {
    if (this->size != other.size) {
      return false;
    }

    for (size_t i = 0; i < this->size; ++i) {
      if (this->cstr[i] != other.cstr[i]) {
        return false;
      }
    }

    return true;
  }
};

constexpr string_t prefix(string_t *s, size_t end) {
  return {.size = clamp_top(s->size, end), .cstr = s->cstr};
}

constexpr string_t postfix(string_t *s, size_t start) {
  return {.size = s->size, .cstr = s->cstr + start};
}

constexpr string_t substr(string_t *s, size_t end) {
  return {.size = clamp_top(s->size, end), .cstr = s->cstr};
}

constexpr string_t substr(string_t *s, size_t start, size_t end) {
  return {.size = clamp_top(end, s->size) - start, .cstr = s->cstr + start};
}

constexpr string_t split(string_t *s, char ch) {
  size_t newsize = 0;

  for (newsize = 0; newsize < s->size && s->cstr[newsize] != ch; ++newsize)
    ;
  return {.size = newsize, .cstr = s->cstr};
}

// ?Maybe temporary?
// #include <ostream>
// std::ostream &operator<<(std::ostream &os, string s) {
//   os << s.size << " ";
//   for (size_t i = 0; i < s.size; ++i) {
//     os << s.cstr[i];
//   }

//   return os;
// }
#endif
