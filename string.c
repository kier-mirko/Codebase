#ifndef BASE_STRING
#define BASE_STRING

#include "string.h"

#include <stdlib.h>

// `size` and `cstr` are to be considered immutable
// =============================================================================
// Unicode codepoint
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

inline fn Codepoint decodeUTF32(u32 *glyph_start) {
  Codepoint res = {.codepoint = *glyph_start, .size = 1};
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

inline fn u8 encodeUTF32(u32 *res, Codepoint cp) {
  res[0] = cp.codepoint;
  return 1;
}

// =============================================================================
// UTF-8 string
fn String8 str8FromStream(Arena *arena, StringStream *stream) {
  size_t final_len = 0, offset = 0;

  for (StringNode *curr = stream->first; curr; curr = curr->next) {
    final_len += curr->value.size;
  }

  u8 *final_chars = Makearr(arena, u8, final_len);
  for (StringNode *curr = stream->first; curr; curr = curr->next) {
    for (size_t i = 0; i < curr->value.size; ++i) {
      *(final_chars + (offset++)) = curr->value.str[i];
    }
  }
  String8 res = {.str = final_chars, .size = final_len};
  return res;
}

fn void stringstreamAppend(Arena *arena, StringStream *strlist, String8 other) {
  Assert(arena);
  Assert(strlist);
  ++strlist->size;

  if (!strlist->last) {
    strlist->first = strlist->last = Make(arena, StringNode);
    strlist->last->value = other;
  } else {
    strlist->last->next = Make(arena, StringNode);
    strlist->last = strlist->last->next;
    strlist->last->value = other;
  }
}

fn String8 str8(char *chars, size_t len) {
  String8 res = {
      .str = (u8 *)chars,
      .size = len,
  };
  return res;
}

fn String8 strFromCstr(char *chars) {
  String8 res = {
      .str = (u8 *)chars,
      .size = strlen(chars),
  };
  return res;
}

fn bool strEq(String8 s1, String8 s2) {
  if (s1.size != s2.size) {
    return false;
  }

  for (size_t i = 0; i < s1.size; ++i) {
    if (s1.str[i] != s2.str[i]) {
      return false;
    }
  }

  return true;
}

fn bool strEqCstr(String8 s, const char *cstr) {
  if (s.size == 0 && !cstr) {
    return true;
  } else if (!cstr || s.size == 0) {
    return false;
  }

  size_t i = 0;
  for (; i < s.size; ++i) {
    if (s.str[i] != cstr[i]) {
      return false;
    }
  }

  if (cstr[i]) {
    return false;
  } else {
    return true;
  }
}

fn bool cstrEq(char *s1, char *s2) {
  if (s1 == s2) {
    return true;
  }
  if (!s1 || !s2) {
    return false;
  }

  char *it1 = s1, *it2 = s2;
  for (; *it1 && *it2; ++it1, ++it2) {
    if (*it1 != *it2) {
      return false;
    }
  }

  return !*it1 && !*it2;
}

fn bool strIsSignedInteger(String8 s) {
  u8 *curr = s.str;
  if (*curr == '-' || *curr == '+') {
    ++curr;
  }

  for (; curr < s.str + s.size; ++curr) {
    if (!charIsDigit(*curr)) {
      return false;
    }
  }

  return true;
}

fn bool strIsInteger(String8 s) {
  for (u8 *curr = s.str; curr < s.str + s.size; ++curr) {
    if (!charIsDigit(*curr)) {
      return false;
    }
  }

  return true;
}

fn bool strIsFloating(String8 s) {
  bool decimal_found = false;
  u8 *curr = s.str;
  if (*curr == '-' || *curr == '+') {
    ++curr;
  }

  for (; curr < s.str + s.size; ++curr) {
    if (!charIsDigit(*curr)) {
      if (*curr == '.' && !decimal_found) {
        decimal_found = true;
      } else {
        return false;
      }
    }
  }

  return true;
}

fn i64 i64FromStr(String8 s) {
  i64 res = 0, decimal = 1;

  Assert(strIsSignedInteger(s));
  for (u8 *curr = s.str + s.size - 1; curr > s.str; --curr, decimal *= 10) {
    res += (*curr - '0') * decimal;
  }

  if (s.str[0] == '-') {
    return -res;
  } else if (s.str[0] == '+') {
    return res;
  } else {
    return res + (s.str[0] - '0') * decimal;
  }
}

fn u64 u64FromStr(String8 s) {
  i64 res = 0, decimal = 1;

  Assert(strIsInteger(s));
  for (u8 *curr = s.str + s.size - 1; curr >= s.str; --curr, decimal *= 10) {
    res += (*curr - '0') * decimal;
  }

  return res;
}

// TODO: maybe implement `strtod`?
fn f64 f64FromStr(String8 s) {
  u8 *end = s.str + s.size;
  return strtod((char *)s.str, (char **)&end);
}

fn String8 stringifyI64(Arena *arena, i64 n) {
  i64 sign = n;
  if (n < 0) {
    n = -n;
  }

  size_t i = 0, approx = 30;
  u8 *str = Makearr(arena, u8, approx);
  for (; n > 0; ++i, n /= 10) {
    str[i] = n % 10 + '0';
  }

  if (sign < 0) {
    str[i++] = '-';
  }

  for (size_t j = 0, k = i - 1; j < k; ++j, --k) {
    u8 tmp = str[k];
    str[k] = str[j];
    str[j] = tmp;
  }

  arenaPop(arena, approx - i);
  String8 res = {
      .str = str,
      .size = i,
  };
  return res;
}

fn String8 stringifyU64(Arena *arena, u64 n) {
  size_t i = 0, approx = 30;
  u8 *str = Makearr(arena, u8, approx);
  for (; n > 0; ++i, n /= 10) {
    str[i] = n % 10 + '0';
  }

  for (size_t j = 0, k = i - 1; j < k; ++j, --k) {
    u8 tmp = str[k];
    str[k] = str[j];
    str[j] = tmp;
  }

  arenaPop(arena, approx - i);
  String8 res = {
      .str = str,
      .size = i,
  };
  return res;
}

fn String8 stringifyF64(Arena *arena, f64 n) {
  size_t approx = 100, size = 0;
  u8 *str = Makearr(arena, u8, approx);

  // TODO: maybe implement `sprintf`?
  size = sprintf((char *)str, "%f", n);
  arenaPop(arena, approx - size);
  String8 res = {
      .str = str,
      .size = size,
  };
  return res;
}

fn size_t strlen(char *chars) {
  char *start = chars;
  for (; *start; ++start)
    ;

  return start - chars;
}

fn String8 strFormat(Arena *arena, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  String8 res = strFormatVa(arena, fmt, args);
  va_end(args);

  return res;
}

fn String8 strFormatVa(Arena *arena, const char *fmt, va_list args) {
  va_list args2;
  va_copy(args2, args);
  u32 needed_bytes = vsnprintf(0, 0, fmt, args2) + 1;

  String8 res = {0};
  res.str = Makearr(arena, u8, needed_bytes);
  res.size = vsnprintf((char *)res.str, needed_bytes, fmt, args);
  res.str[res.size] = 0;

  va_end(args2);
  return res;
}

fn String8 strPrefix(String8 s, size_t end) {
  String8 res = {.str = s.str, .size = ClampTop(s.size, end)};
  return res;
}

fn String8 strPostfix(String8 s, size_t start) {
  String8 res = {.str = s.str + start, .size = s.size};
  return res;
}

fn String8 substr(String8 s, size_t end) {
  String8 res = {.str = s.str, .size = ClampTop(s.size, end)};
  return res;
}

fn String8 strRange(String8 s, size_t start, size_t end) {
  String8 res = {.str = s.str + start, .size = ClampTop(end, s.size) - start};
  return res;
}

fn bool strEndsWith(String8 s, char ch) { return s.str[s.size - 1] == ch; }

fn String8 longestCommonSubstring(Arena *arena, String8 s1, String8 s2) {
  String8 res = {0};
  if (s1.size == 0 || s2.size == 0) {
    return res;
  }

  size_t(*memo)[s2.size + 1] =
      arenaMake(arena, sizeof(size_t[s1.size + 1][s2.size + 1]));

  for (i32 i = s1.size - 1; i >= 0; --i) {
    for (i32 j = s2.size - 1; j >= 0; --j) {
      if (s1.str[i] == s2.str[j]) {
        memo[i][j] = 1 + memo[i + 1][j + 1];
      } else {
        memo[i][j] = Max(memo[i + 1][j], memo[i][j + 1]);
      }
    }
  }

  res.size = memo[0][0];
  res.str = Makearr(arena, u8, memo[0][0]);
  for (i32 i = 0, j = 0, last = 0; i < s1.size && j < s2.size;) {
    if (memo[i][j] == memo[i + 1][j]) {
      ++i;
    } else if (memo[i][j] == memo[i][j + 1]) {
      ++j;
    } else if (memo[i][j] - 1 == memo[i + 1][j + 1]) {
      res.str[last++] = s1.str[i];
      ++i;
      ++j;
    }
  }

  return res;
}

fn String8 upperFromStr(Arena *arena, String8 s) {
  String8 res = {.str = Makearr(arena, u8, s.size), .size = s.size};

  for (size_t i = 0; i < s.size; ++i) {
    res.str[i] = charToUpper(s.str[i]);
  }

  return res;
}

fn String8 lowerFromStr(Arena *arena, String8 s) {
  String8 res = {.str = Makearr(arena, u8, s.size), .size = s.size};

  for (size_t i = 0; i < s.size; ++i) {
    res.str[i] = charToLower(s.str[i]);
  }

  return res;
}

fn String8 capitalizeFromStr(Arena *arena, String8 s) {
  String8 res = {.str = Makearr(arena, u8, s.size), .size = s.size};

  res.str[0] = charToUpper(s.str[0]);
  for (size_t i = 1; i < s.size; ++i) {
    if (charIsSpace(s.str[i])) {
      res.str[i] = s.str[i];
      ++i;
      res.str[i] = charToUpper(s.str[i]);
    } else {
      res.str[i] = charToLower(s.str[i]);
    }
  }

  return res;
}

fn StringStream strSplit(Arena *arena, String8 s, char ch) {
  StringStream res = {0};

  size_t prev = 0;
  for (size_t i = 0; i < s.size; ++i) {
    if (s.str[i] == ch) {
      stringstreamAppend(arena, &res, strRange(s, prev, i));
      prev = i + 1;
    }
  }

  stringstreamAppend(arena, &res, strRange(s, prev, s.size));
  return res;
}

fn bool strContains(String8 s, char ch) {
  for (u8 *curr = s.str; curr < s.str + s.size + 1; ++curr) {
    if (*curr == ch) {
      return true;
    }
  }

  return false;
}

fn bool charIsSpace(u8 ch) { return ch == ' '; }
fn bool charIsSlash(u8 ch) { return ch == '/'; }
fn bool charIsUpper(u8 ch) { return ch >= 'A' && ch <= 'Z'; }
fn bool charIsLower(u8 ch) { return ch >= 'a' && ch <= 'z'; }
fn bool charIsDigit(u8 ch) { return ch >= '0' && ch <= '9'; }
fn bool charIsAlpha(u8 ch) {
  return (ch >= 'A' && ch <= 'Z') || ch >= 'a' && ch <= 'z';
}

fn bool charIsAlphanumeric(u8 ch) {
  return charIsDigit(ch) || charIsAlpha(ch);
}

fn u8 charToUpper(u8 ch) { return charIsLower(ch) ? ch - 32 : ch; }
fn u8 charToLower(u8 ch) { return charIsUpper(ch) ? ch + 32 : ch; }
fn u8 getCorrectPathSeparator() {
#if OS_WINDOWS
  return '\\';
#else
  return '/';
#endif
}

// =============================================================================
// Other UTF strings
bool str16Eq(String16 s1, String16 s2) {
  if (s1.size != s2.size) {
    return false;
  }

  for (size_t i = 0; i < s1.size; ++i) {
    if (s1.str[i] != s2.str[i]) {
      return false;
    }
  }

  return true;
}

fn bool str32Eq(String32 s1, String32 s2) {
  if (s1.size != s2.size) {
    return false;
  }

  for (size_t i = 0; i < s1.size; ++i) {
    if (s1.str[i] != s2.str[i]) {
      return false;
    }
  }

  return true;
}

// =============================================================================
// UTF string conversion
fn String8 UTF8From16(Arena *arena, String16 *in) {
  size_t res_size = 0, approx_size = in->size * 4;
  u8 *bytes = Makearr(arena, u8, approx_size), *res_offset = bytes;

  Codepoint codepoint = {0};
  for (u16 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decodeUTF16(start);

    u8 utf8_codepoint_size = encodeUTF8(res_offset, codepoint);
    res_size += utf8_codepoint_size;
    res_offset += utf8_codepoint_size;
  }

  arenaPop(arena, (approx_size - res_size));
  String8 res = {.str = bytes, .size = res_size};
  return res;
}

fn String8 UTF8From32(Arena *arena, String32 *in) {
  size_t res_size = 0, approx_size = in->size * 4;
  u8 *bytes = Makearr(arena, u8, approx_size), *res_offset = bytes;

  Codepoint codepoint = {0};
  for (u32 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decodeUTF32(start);

    u8 utf8_codepoint_size = encodeUTF8(res_offset, codepoint);
    res_size += utf8_codepoint_size;
    res_offset += utf8_codepoint_size;
  }

  arenaPop(arena, (approx_size - res_size));
  String8 res = {.str = bytes, .size = res_size};
  return res;
}

fn String16 UTF16From8(Arena *arena, String8 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u16 *words = Makearr(arena, u16, approx_size), *res_offset = words;

  Codepoint codepoint = {0};
  for (u8 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decodeUTF8(start);

    u8 utf16_codepoint_size = encodeUTF16(res_offset, codepoint);
    res_size += utf16_codepoint_size;
    res_offset += utf16_codepoint_size;
  }

  arenaPop(arena, (approx_size - res_size));
  String16 res = {.str = words, .size = res_size};
  return res;
}

fn String16 UTF16From32(Arena *arena, String32 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u16 *words = Makearr(arena, u16, approx_size), *res_offset = words;

  Codepoint codepoint = {0};
  for (u32 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decodeUTF32(start);

    u8 utf16_codepoint_size = encodeUTF16(res_offset, codepoint);
    res_size += utf16_codepoint_size;
    res_offset += utf16_codepoint_size;
  }

  arenaPop(arena, (approx_size - res_size));
  String16 res = {.str = words, .size = res_size};
  return res;
}

fn String32 UTF32From8(Arena *arena, String8 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u32 *dwords = Makearr(arena, u32, approx_size), *res_offset = dwords;

  Codepoint cp = {0};
  for (u8 *start = in->str, *end = in->str + in->size; start < end;
       start += cp.size, ++res_size) {
    cp = decodeUTF8(start);
    *res_offset++ = cp.codepoint;
  }

  arenaPop(arena, (approx_size - res_size));
  String32 res = {.str = dwords, .size = res_size};
  return res;
}

fn String32 UTF32From16(Arena *arena, String16 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u32 *dwords = Makearr(arena, u32, approx_size), *res_offset = dwords;

  Codepoint cp = {0};
  for (u16 *start = in->str, *end = in->str + in->size; start < end;
       start += cp.size, ++res_size) {
    cp = decodeUTF16(start);
    *res_offset++ = cp.codepoint;
  }

  arenaPop(arena, (approx_size - res_size));
  String32 res = {.str = dwords, .size = res_size};
  return res;
}

#endif
