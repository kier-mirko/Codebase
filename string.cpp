#ifndef BASE_STRING
#define BASE_STRING

#include "arena.cpp"
#include "base.cpp"

#include <stdio.h>
#include <stdarg.h>

// =============================================================================
// Unicode codepoint
namespace Base {
struct Codepoint {
  u32 codepoint;
  u8 size;
};
} // namespace Base

// =============================================================================
// UTF-8 Strings
#define Strlit(STR) (::Base::String8{.str = (u8 *)(STR), .size = sizeof(STR) - 1})
#define StrlitInit(STR) { (u8 *)(STR), sizeof(STR) - 1, }
#define Strexpand(STR) (int)((STR).size), (char *)((STR).str)

// `size` and `cstr` are to be considered immutable
namespace Base {
struct String8 {
  u8 *str;
  size_t size = 0;
  size_t length = size;

  constexpr char operator[](size_t idx) const {
    return idx > this->size ? 0 : this->str[idx];
  }

  constexpr bool operator==(String8 other) const {
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

fn String8 str8(u8 *chars, size_t len) {
  return {
      .str = chars,
      .size = len,
  };
}

fn size_t strlen(u8 *chars) {
  u8 *start = chars;
  for (; *start; ++start)
    ;

  return start - chars;
}

fn String8 str8(u8 *chars) {
  return {
      .str = chars,
      .size = strlen(chars),
  };
}

fn String8 formatStr(Arena *arena, const char *fmt, va_list args) {
  va_list args2;
  va_copy(args2, args);
  u32 needed_bytes = vsnprintf(0, 0, fmt, args2) + 1;

  String8 res{0};
  res.str = Makearr(arena, u8, needed_bytes);
  res.size = vsnprintf((char *)res.str, needed_bytes, fmt, args);
  res.str[res.size] = 0;

  va_end(args2);
  return res;
}

fn String8 formatStr(Arena *arena, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  String8 res = formatStr(arena, fmt, args);
  va_end(args);

  return res;
}

fn constexpr String8 prefix(const String8 *s, size_t end) {
  return {.str = s->str, .size = ClampTop(s->size, end)};
}

fn constexpr String8 postfix(const String8 *s, size_t start) {
  return {.str = s->str + start, .size = s->size};
}

fn constexpr String8 substr(const String8 *s, size_t end) {
  return {.str = s->str, .size = ClampTop(s->size, end)};
}

fn constexpr String8 substr(const String8 *s, size_t start, size_t end) {
  return {.str = s->str + start, .size = ClampTop(end, s->size) - start};
}

fn constexpr String8 split(const String8 *s, char ch) {
  size_t newsize = 0;

  for (newsize = 0; newsize < s->size && s->str[newsize] != ch; ++newsize)
    ;
  return {.str = s->str, .size = newsize};
}

fn constexpr String8 longestCommonSubstring(Arena *arena, String8 s1,
                                            String8 s2) {
  if (s1.size == 0 || s2.size == 0) {
    return {0};
  }

  auto memo = (size_t(*)[s2.size + 1])
      Base::arenaMake(arena, sizeof(size_t[s1.size + 1][s2.size + 1]));

  for (i32 i = s1.size - 1; i >= 0; --i) {
    for (i32 j = s2.size - 1; j >= 0; --j) {
      if (s1[i] == s2[j]) {
        memo[i][j] = 1 + memo[i + 1][j + 1];
      } else {
        memo[i][j] = Max(memo[i + 1][j], memo[i][j + 1]);
      }
    }
  }

  char *res = Makearr(arena, char, memo[0][0]);
  for (i32 i = 0, j = 0, last = 0; i < s1.size && j < s2.size;) {
    if (memo[i][j] == memo[i + 1][j]) {
      ++i;
    } else if (memo[i][j] == memo[i][j + 1]) {
      ++j;
    } else if (memo[i][j] - 1 == memo[i + 1][j + 1]) {
      res[last++] = s1[i];
      ++i;
      ++j;
    }
  }

  return {
      .str = (u8 *)res,
      .size = memo[0][0],
  };
}

fn Codepoint decodeUTF8(u8 *glyph_start) {
  Codepoint res = {0};

  if ((*glyph_start & 0x80) == 0) {
    res.codepoint = *glyph_start;
    res.size = 1;
  } else if ((*glyph_start & 0xE0) == 0xC0) {
    res.codepoint = glyph_start[1] & 0x3F;
    res.codepoint |= (glyph_start[0] & 0x1F) << 6;
    res.size = 2;
  } else if ((*glyph_start & 0xF0) == 0xE0) {
    res.codepoint = glyph_start[2] & 0x3F;
    res.codepoint |= (glyph_start[1] & 0x3F) << 6;
    res.codepoint |= (glyph_start[0] & 0xf) << 12;
    res.size = 3;
  } else if ((*glyph_start & 0xF8) == 0xF0) {
    res.codepoint = glyph_start[3] & 0x3F;
    res.codepoint |= (glyph_start[2] & 0x3F) << 6;
    res.codepoint |= (glyph_start[1] & 0x3F) << 12;
    res.codepoint |= (glyph_start[0] & 0x7) << 18;
    res.size = 4;
  } else {
    Assert(false);
  }

  return res;
}

fn u8 encodeUTF8(u8 *res, Codepoint cp) {
  if (cp.codepoint <= 0x7F) {
    res[0] = cp.codepoint;
    return 1;
  } else if (cp.codepoint <= 0x7FF) {
    res[0] = 0xC0 | (cp.codepoint >> 6);
    res[1] = 0x80 | (cp.codepoint & 0x3F);
    return 2;
  } else if (cp.codepoint <= 0xFFFF) {
    res[0] = 0xE0 | (cp.codepoint >> 12);
    res[1] = 0x80 | ((cp.codepoint >> 6) & 0x3F);
    res[2] = 0x80 | (cp.codepoint & 0x3F);
    return 3;
  } else if (cp.codepoint <= 0x10FFFF) {
    res[0] = 0xF0 | (cp.codepoint >> 18);
    res[1] = 0x80 | ((cp.codepoint >> 12) & 0x3F);
    res[2] = 0x80 | ((cp.codepoint >> 6) & 0x3F);
    res[3] = 0x80 | (cp.codepoint & 0x3F);
    return 4;
  } else {
    Assert(false);
    return -1;
  }
}

// =============================================================================
// UTF-16 Strings
// `size` and `cstr` are to be considered immutable
struct String16 {
  u16 *str;
  size_t size = 0;
  size_t length = size;

  constexpr char operator[](size_t idx) const {
    return idx > this->size ? 0 : this->str[idx];
  }

  constexpr bool operator==(String16 &other) const {
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
};

// No other operations are defined. Use UTF-8 strings instead.

fn Codepoint decodeUTF16(u16 *glyph_start) {
  Codepoint res = {0};

  if (glyph_start[0] <= 0xD7FF ||
      (glyph_start[0] >= 0xE000 && glyph_start[0] <= 0xFFFF)) {
    res.size = 1;
    res.codepoint = *glyph_start;
  } else if ((glyph_start[0] >= 0xD800 && glyph_start[0] <= 0xDBFF) &&
             (glyph_start[1] >= 0xDC00 && glyph_start[1] <= 0xDFFF)) {
    res.size = 2;
    res.codepoint =
        ((glyph_start[0] - 0xD800) << 10) + (glyph_start[1] - 0xDC00) + 0x10000;
  } else {
    Assert(false);
  }

  return res;
}

fn u8 encodeUTF16(u16 *res, Codepoint cp) {
  if (cp.codepoint <= 0xD7FF ||
      (cp.codepoint >= 0xE000 && cp.codepoint <= 0xFFFF)) {
    res[0] = cp.codepoint;
    return 1;
  } else if (cp.codepoint >= 0x10000 && cp.codepoint <= 0x10FFFF) {
    res[0] = ((cp.codepoint - 0x10000) >> 10) + 0xD800;
    res[1] = ((cp.codepoint - 0x10000) & 0x3FF) + 0xDC00;
    return 2;
  } else {
    Assert(false);
    return -1;
  }
}

// =============================================================================
// UTF-32 Strings
// `size` and `cstr` are to be considered immutable
struct String32 {
  u32 *str;
  size_t size = 0;
  size_t length = size;

  constexpr char operator[](size_t idx) const {
    return idx > this->size ? 0 : this->str[idx];
  }

  constexpr bool operator==(String32 &other) const {
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
};

// No other operations are defined. Use UTF-8 strings instead.

inline fn Codepoint decodeUTF32(u32 *glyph_start) {
  return {.codepoint = *glyph_start, .size = 1};
}

inline fn u8 encodeUTF32(u32 *res, Codepoint cp) {
  res[0] = cp.codepoint;
  return 1;
}

// =============================================================================
// UTF-32 from UTF-8/16
fn String8 UTF8From16(Arena *arena, String16 *in) {
  size_t res_size = 0, approx_size = in->size * 4;
  u8 *res = Makearr(arena, u8, approx_size), *res_offset = res;

  Codepoint codepoint = {0};
  for (u16 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decodeUTF16(start);

    u8 utf8_codepoint_size = encodeUTF8(res_offset, codepoint);
    res_size += utf8_codepoint_size;
    res_offset += utf8_codepoint_size;
  }

  arenaPop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

fn String8 UTF8From32(Arena *arena, String32 *in) {
  size_t res_size = 0, approx_size = in->size * 4;
  u8 *res = Makearr(arena, u8, approx_size), *res_offset = res;

  Codepoint codepoint = {0};
  for (u32 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decodeUTF32(start);

    u8 utf8_codepoint_size = encodeUTF8(res_offset, codepoint);
    res_size += utf8_codepoint_size;
    res_offset += utf8_codepoint_size;
  }

  arenaPop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

// =============================================================================
// UTF-16 from UTF-16/32
fn String16 UTF16From8(Arena *arena, String8 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u16 *res = Makearr(arena, u16, approx_size), *res_offset = res;

  Codepoint codepoint = {0};
  for (u8 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decodeUTF8(start);

    u8 utf16_codepoint_size = encodeUTF16(res_offset, codepoint);
    res_size += utf16_codepoint_size;
    res_offset += utf16_codepoint_size;
  }

  arenaPop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

fn String16 UTF16From32(Arena *arena, String32 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u16 *res = Makearr(arena, u16, approx_size), *res_offset = res;

  Codepoint codepoint = {0};
  for (u32 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decodeUTF32(start);

    u8 utf16_codepoint_size = encodeUTF16(res_offset, codepoint);
    res_size += utf16_codepoint_size;
    res_offset += utf16_codepoint_size;
  }

  arenaPop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

// =============================================================================
// UTF-32 from UTF-8/16
fn String32 UTF32From8(Arena *arena, String8 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u32 *res = Makearr(arena, u32, approx_size), *res_offset = res;

  Codepoint cp = {0};
  for (u8 *start = in->str, *end = in->str + in->size; start < end;
       start += cp.size, ++res_size) {
    cp = decodeUTF8(start);
    *res_offset++ = cp.codepoint;
  }

  arenaPop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

fn String32 UTF32From16(Arena *arena, String16 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u32 *res = Makearr(arena, u32, approx_size), *res_offset = res;

  Codepoint cp = {0};
  for (u16 *start = in->str, *end = in->str + in->size; start < end;
       start += cp.size, ++res_size) {
    cp = decodeUTF16(start);
    *res_offset++ = cp.codepoint;
  }

  arenaPop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}
} // namespace Base

#endif
