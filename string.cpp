#ifndef BASE_STRING
#define BASE_STRING

#include "arena.cpp"
#include "base.cpp"

#define strlit(STR) (string8_t{.str = (u8 *)(STR), sizeof(STR) - 1})
#define strexpand(STR8) (int)((STR8).size), ((STR8).str)

// `size` and `cstr` are to be considered immutable
struct string8_t {
  u8 *str;
  size_t size;
  size_t length = size;

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

fn constexpr string8_t prefix(const string8_t *s, size_t end) {
  return {.str = s->str, .size = clamp_top(s->size, end)};
}

fn constexpr string8_t postfix(const string8_t *s, size_t start) {
  return {.str = s->str + start, .size = s->size};
}

fn constexpr string8_t substr(const string8_t *s, size_t end) {
  return {.str = s->str, .size = clamp_top(s->size, end)};
}

fn constexpr string8_t substr(const string8_t *s, size_t start, size_t end) {
  return {.str = s->str + start, .size = clamp_top(end, s->size) - start};
}

fn constexpr string8_t split(const string8_t *s, char ch) {
  size_t newsize = 0;

  for (newsize = 0; newsize < s->size && s->str[newsize] != ch; ++newsize)
    ;
  return {.str = s->str, .size = newsize};
}

fn string8_t utf8_decode(u8 *bytes, size_t bytes_size) {
  size_t len = 0;

  for (u8 *start = bytes, *end = bytes + bytes_size; start < end; ++len) {
    if ((*start & 0x80) == 0) {
      ++start;
    } else if ((*start & 0xE0) == 0xC0) {
      start += 2;
    } else if ((*start & 0xF0) == 0xE0) {
      start += 3;
    } else if ((*start & 0xF8) == 0xF0) {
      start += 4;
    }
  }

  return {.str = bytes, .size = bytes_size, .length = len};
}

fn u8 *utf8_encode(base::Arena *arena, string8_t *str) {
  if (!str || str->size == 0) {
    return 0;
  }

  u8 *res = makearr(arena, u8, str->size);
  for (size_t i = 0; i < str->size; ++i) {
    res[i] = str->str[i];
  }

  return res;
}

#endif
