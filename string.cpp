#ifndef BASE_STRING
#define BASE_STRING

#include "base.cpp"

#define strlit(STR) (string8_t{.str = (u8 *)(STR), sizeof(STR) - 1})
#define strexpand(STR8) (int)((STR8).size), ((STR8).str)

// `size` and `cstr` are to be considered immutable
struct string8_t {
  u8 *str;
  size_t size;

  constexpr char operator[](size_t idx) const {
    return idx > this->size ? 0 : this->str[idx];
  }

  constexpr bool operator==(string8_t &other) const {
    if (this->size != other.size) {
      return false;
    }

    for (size_t i = 0; i < this->size; ++i) {
      if (this->str[i] != other.str[i]) {
        return false;
      }
    }

    return true;
  }

  constexpr bool operator==(const char *cstr) const {
    if (this->size == 0 && !cstr) {
      return true;
    } else if (!cstr || this->size == 0) {
      return false;
    }

    size_t i = 0;
    for (; i < this->size; ++i) {
      if (this->str[i] != cstr[i]) {
        return false;
      }
    }

    if (cstr[i]) {
      return false;
    } else {
      return true;
    }
  }
};

constexpr string8_t prefix(const string8_t *s, size_t end) {
  return {.str = s->str, .size = clamp_top(s->size, end)};
}

constexpr string8_t postfix(const string8_t *s, size_t start) {
  return {.str = s->str + start, .size = s->size};
}

constexpr string8_t substr(const string8_t *s, size_t end) {
  return {.str = s->str, .size = clamp_top(s->size, end)};
}

constexpr string8_t substr(const string8_t *s, size_t start, size_t end) {
  return {.str = s->str + start, .size = clamp_top(end, s->size) - start};
}

constexpr string8_t split(const string8_t *s, char ch) {
  size_t newsize = 0;

  for (newsize = 0; newsize < s->size && s->str[newsize] != ch; ++newsize)
    ;
  return {.str = s->str, .size = newsize};
}

#endif
