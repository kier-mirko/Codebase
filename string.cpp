#ifndef BASE_STRING
#define BASE_STRING

#include "arena.cpp"
#include "base.cpp"
#include "memory.cpp"

// `size` and `cstr` are to be considered immutable

// =============================================================================
// UTF-8 Strings
#define strlit(STR) (::base::string8{.str = (u8 *)(STR), sizeof(STR) - 1})
#define strexpand(STR) (int)((STR).size), (char *)((STR).str)

namespace base {
struct string8 {
  u8 *str;
  size_t size = 0;
  size_t length = size;

  constexpr char operator[](size_t idx) const {
    return idx > this->size ? 0 : this->str[idx];
  }

  constexpr bool operator==(string8 &other) const {
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
} // namespace base

fn constexpr base::string8 prefix(const base::string8 *s, size_t end) {
  return {.str = s->str, .size = clamp_top(s->size, end)};
}

fn constexpr base::string8 postfix(const base::string8 *s, size_t start) {
  return {.str = s->str + start, .size = s->size};
}

fn constexpr base::string8 substr(const base::string8 *s, size_t end) {
  return {.str = s->str, .size = clamp_top(s->size, end)};
}

fn constexpr base::string8 substr(const base::string8 *s, size_t start,
                                  size_t end) {
  return {.str = s->str + start, .size = clamp_top(end, s->size) - start};
}

fn constexpr base::string8 split(const base::string8 *s, char ch) {
  size_t newsize = 0;

  for (newsize = 0; newsize < s->size && s->str[newsize] != ch; ++newsize)
    ;
  return {.str = s->str, .size = newsize};
}

fn bool is_utf8(u8 *bytes, size_t bytes_size) { return true; }

fn base::string8 utf8_decode(base::arena *arena, u8 *bytes, size_t bytes_size) {
  if (!is_utf8(bytes, bytes_size)) {
    return {};
  }

  base::string8 res{.size = bytes_size};
  size_t length = 0;

  for (u8 *start = bytes, *end = bytes + bytes_size; start < end; ++length) {
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

  res.length = length;
  res.str = makearr(arena, u8, bytes_size);
  base::memcpy(arena, res.str, bytes, bytes_size);

  return res;
}

fn u8 *utf8_encode(base::arena *arena, base::string8 *str) {
  if (!str || str->size == 0) {
    return 0;
  }

  return (u8 *)base::memcpy(arena, makearr(arena, u8, str->size), str->str,
                            str->size);
}

// =============================================================================
// UTF-16 Strings
namespace base {
struct string16 {
  u16 *str;
  size_t size = 0;
  size_t length = size;
};
} // namespace base

// No other operations are defined. Use UTF-8 strings instead.

fn bool is_utf16(u16 *words, size_t words_size) {
  for (size_t i = 0; i < words_size; ++i) {
    if (words[i] >= 0xDC00 && words[i] <= 0xDFFF) {
      return false;
    } else if (words[i] >= 0xD800 && words[i] <= 0xDBFF) {
      if (++i >= words_size) {
        return false;
      } else if (words[i] < 0xDC00 || words[i] > 0xDFFF) {
        return false;
      }
    }
  }

  return true;
}

fn base::string16 utf16_decode(base::arena *arena, u16 *words,
                               size_t words_size) {
  if (!is_utf16(words, words_size)) {
    return {};
  }

  base::string16 res{.size = words_size, .length = 0};
  for (u16 *start = words, *end = words + words_size; start < end;
       ++res.length) {
    if (*start <= 0xD7FF || *start >= 0xE000) {
      start += 1;
    } else {
      start += 2;
    }
  }

  res.str = makearr(arena, u16, words_size);
  base::memcpy(arena, res.str, words, words_size);

  return res;
}

fn u16 *utf16_encode(base::arena *arena, base::string16 *str) {
  if (!str || str->size == 0) {
    return 0;
  }

  u16 *res = makearr(arena, u16, str->size);
  base::memcpy(arena, res, str->str, str->size);

  return res;
}

// =============================================================================
// UTF-32 Strings
namespace base {
struct string32 {
  u32 *str;
  size_t size = 0;
  size_t length = size;
};
} // namespace base

// No other operations are defined. Use UTF-8 strings instead.

fn bool is_utf32(u32 *dwords, size_t dwords_size) {
  for (size_t i = 0; i < dwords_size; ++i) {
    if (dwords[i] > 0x10FFFF) {
      return false;
    } else if (dwords[i] >= 0xD800 && dwords[i] <= 0xDFFF) {
      return false;
    }
  }

  return true;
}

fn base::string32 utf32_decode(base::arena *arena, u32 *dwords,
                               size_t dwords_size) {
  if (!is_utf32(dwords, dwords_size)) {
    return {};
  }

  base::string32 res{.str = makearr(arena, u32, dwords_size),
                     .size = dwords_size,
                     .length = dwords_size};
  base::memcpy(arena, res.str, dwords, dwords_size);

  return res;
}

fn u32 *utf32_encode(base::arena *arena, base::string32 *str) {
  if (!str || str->size == 0) {
    return 0;
  }

  u32 *res = makearr(arena, u32, str->size);
  base::memcpy(arena, res, str->str, str->size);

  return res;
}

#endif
