#ifndef BASE_STRING
#define BASE_STRING

#include "arena.cpp"
#include "base.cpp"

// `size` and `cstr` are to be considered immutable

#define DEFINE_STRFN(KIND)                                                     \
  fn constexpr string##KIND##_t prefix(const string##KIND##_t *s,              \
                                       size_t end) {                           \
    return {.str = s->str, .size = clamp_top(s->size, end)};                   \
  }                                                                            \
                                                                               \
  fn constexpr string##KIND##_t postfix(const string##KIND##_t *s,             \
                                        size_t start) {                        \
    return {.str = s->str + start, .size = s->size};                           \
  }                                                                            \
                                                                               \
  fn constexpr string##KIND##_t substr(const string##KIND##_t *s,              \
                                       size_t end) {                           \
    return {.str = s->str, .size = clamp_top(s->size, end)};                   \
  }                                                                            \
                                                                               \
  fn constexpr string##KIND##_t substr(const string##KIND##_t *s,              \
                                       size_t start, size_t end) {             \
    return {.str = s->str + start, .size = clamp_top(end, s->size) - start};   \
  }                                                                            \
                                                                               \
  fn constexpr string##KIND##_t split(const string##KIND##_t *s, char ch) {    \
    size_t newsize = 0;                                                        \
                                                                               \
    for (newsize = 0; newsize < s->size && s->str[newsize] != ch; ++newsize)   \
      ;                                                                        \
    return {.str = s->str, .size = newsize};                                   \
  }

#define DEFINE_STROP(KIND)                                                     \
  constexpr char operator[](size_t idx) const {                                \
    return idx > this->size ? 0 : this->str[idx];                              \
  }                                                                            \
                                                                               \
  constexpr bool operator==(string##KIND##_t &other) const {                   \
    if (this->size != other.size) {                                            \
      return false;                                                            \
    }                                                                          \
                                                                               \
    for (size_t i = 0; i < this->size; ++i) {                                  \
      if (this->str[i] != other.str[i]) {                                      \
        return false;                                                          \
      }                                                                        \
    }                                                                          \
                                                                               \
    return true;                                                               \
  }                                                                            \
                                                                               \
  constexpr bool operator==(const char *cstr) const {                          \
    if (this->size == 0 && !cstr) {                                            \
      return true;                                                             \
    } else if (!cstr || this->size == 0) {                                     \
      return false;                                                            \
    }                                                                          \
                                                                               \
    size_t i = 0;                                                              \
    for (; i < this->size; ++i) {                                              \
      if (this->str[i] != cstr[i]) {                                           \
        return false;                                                          \
      }                                                                        \
    }                                                                          \
                                                                               \
    if (cstr[i]) {                                                             \
      return false;                                                            \
    } else {                                                                   \
      return true;                                                             \
    }                                                                          \
  }

#define strexpand(STR) (int)((STR).size), (char *)((STR).str)

// =============================================================================
// UTF-8 Strings
#define str8lit(STR) (string8_t{.str = (u8 *)(STR), sizeof(STR) - 1})

struct string8_t {
  u8 *str;
  size_t size;
  size_t length = size;

  DEFINE_STROP(8)
};

DEFINE_STRFN(8)

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

// =============================================================================
// UTF-16 Strings
#define str16lit(STR) (string16_t{.str = (u16 *)(STR), sizeof(STR) - 1})

struct string16_t {
  u16 *str;
  size_t size;
  size_t length = size;

  DEFINE_STROP(16)
};

DEFINE_STRFN(16)

fn string16_t utf16_decode(u16 *bytes, size_t bytes_size) {
  return {};
}

fn u16 *utf16_encode(base::Arena *arena, string16_t *str) {
  return {};
}

#define str16lit(STR) (string16_t{.str = (u16 *)(STR), sizeof(STR) - 1})

// =============================================================================
// UTF-32 Strings
struct string32_t {
  u32 *str;
  size_t size;
  size_t length = size;

  DEFINE_STROP(32)
};

DEFINE_STRFN(32)

fn string32_t utf32_decode(u32 *bytes, size_t bytes_size) {
  return {};
}

fn u32 *utf32_encode(base::Arena *arena, string32_t *str) {
  return {};
}

#undef DEFINE_STRFN
#undef DEFINE_STROP

#endif
