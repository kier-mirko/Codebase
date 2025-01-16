#include "string.h"
#include "time.h"

#include <stdlib.h>

// `size` and `cstr` are to be considered immutable
// =============================================================================
// Unicode codepoint
fn Codepoint utf8_decode(U8 *glyph_start) {
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
    Assert(0);
  }
  
  return res;
}

fn Codepoint utf16_decode(U16 *glyph_start) {
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
    Assert(0);
  }
  
  return res;
}

inline Codepoint utf32_decode(U32 *glyph_start) {
  Codepoint res = {.codepoint = *glyph_start, .size = 1};
  return res;
}

fn U8 utf8_encode(U8 *res, Codepoint cp) {
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
    Assert(0);
    return -1;
  }
}

fn U8 utf16_encode(U16 *res, Codepoint cp) {
  if (cp.codepoint <= 0xD7FF ||
      (cp.codepoint >= 0xE000 && cp.codepoint <= 0xFFFF)) {
    res[0] = cp.codepoint;
    return 1;
  } else if (cp.codepoint >= 0x10000 && cp.codepoint <= 0x10FFFF) {
    res[0] = ((cp.codepoint - 0x10000) >> 10) + 0xD800;
    res[1] = ((cp.codepoint - 0x10000) & 0x3FF) + 0xDC00;
    return 2;
  } else {
    Assert(0);
    return -1;
  }
}

inline U8 utf32_encode(U32 *res, Codepoint cp) {
  res[0] = cp.codepoint;
  return 1;
}

// =============================================================================
// UTF-8 string
fn String8 str8_list_join(Arena *arena, String8List list) {
  
  U64 size = 0;
  
  for (String8Node *n = list.first; n; n = n->next) {
    size += n->string.size;
  }
  
  U8 *str = make(arena, U8, size);
  for (String8Node *n = list.first; n; n = n->next) {
    memcpy(str, n->string.str, n->string.size);
    str += n->string.size;
  }
  return str8(str, size);
}


fn void str8_list_append(Arena *arena, String8List strlist, String8 other) {
  Assert(arena);
  Assert(strlist);
  ++strlist.size;
  
  String8Node *str = make(arena, String8Node);
  str->string = other;
  DLLPushBack(strlist.first, strlist.last, str);
}

inline String8 str8(U8 *chars, USZ len) {
  return (String8) {
    .str = chars,
    .size = len,
  };
}

inline String8 str8_from_cstr(char *chars) {
  return (String8) {
    .str = (U8 *)chars,
    .size = cstr_len(chars),
  };
}

inline String8 str8_from_unix_time(Arena *arena, U64 unix_timestamp) {
  DateTime dt = t_date_time_from_unix(unix_timestamp);
  return str8_from_date_time(arena, dt);
}

inline String8 str8_from_date_time(Arena *arena, DateTime dt) {
  return str8_format(arena, "%02d/%02d/%04d %02d:%02d:%02d.%02d",
                     dt.day, dt.month, dt.year,
                     dt.hour, dt.minute, dt.second, dt.ms);
}

fn B32 str8_match(String8 s1, String8 s2) {
  if (s1.size != s2.size) {
    return 0;
  }
  
  for (USZ i = 0; i < s1.size; ++i) {
    if (s1.str[i] != s2.str[i]) {
      return 0;
    }
  }
  
  return 1;
}

fn B32 str8_match_cstr(String8 s, const char *cstr) {
  if (s.size == 0 && !cstr) {
    return 1;
  } else if (!cstr || s.size == 0) {
    return 0;
  }
  
  USZ i = 0;
  for (; i < s.size; ++i) {
    if (s.str[i] != cstr[i]) {
      return 0;
    }
  }
  
  if (cstr[i]) {
    return 0;
  } else {
    return 1;
  }
}

fn B32 cstr_match(char *s1, char *s2) {
  if (s1 == s2) {
    return 1;
  }
  if (!s1 || !s2) {
    return 0;
  }
  
  char *it1 = s1, *it2 = s2;
  for (; *it1 && *it2; ++it1, ++it2) {
    if (*it1 != *it2) {
      return 0;
    }
  }
  
  return !*it1 && !*it2;
}

fn B32 str8_is_signed_int(String8 s) {
  U8 *curr = s.str;
  if (*curr == '-' || *curr == '+') {
    ++curr;
  }
  
  for (; curr < s.str + s.size; ++curr) {
    if (!char_is_digit(*curr)) {
      return 0;
    }
  }
  
  return 1;
}

fn B32 str8_is_int(String8 s) {
  for (U8 *curr = s.str; curr < s.str + s.size; ++curr) {
    if (!char_is_digit(*curr)) {
      return 0;
    }
  }
  
  return 1;
}

fn B32 str8_is_float(String8 s) {
  B32 decimal_found = 0;
  U8 *curr = s.str;
  if (*curr == '-' || *curr == '+') {
    ++curr;
  }
  
  for (; curr < s.str + s.size; ++curr) {
    if (!char_is_digit(*curr)) {
      if (*curr == '.' && !decimal_found) {
        decimal_found = 1;
      } else {
        return 0;
      }
    }
  }
  
  return 1;
}

inline B32 str8_is_number(String8 s) {
  return str8_is_float(s) || str8_is_int(s);
}

fn I64 i64_from_str8(String8 s) {
  I64 res = 0, decimal = 1;
  
  Assert(str8_is_signed_int(s));
  for (U8 *curr = s.str + s.size - 1; curr > s.str; --curr, decimal *= 10) {
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

fn U64 u64_from_str8(String8 s) {
  I64 res = 0, decimal = 1;
  
  Assert(str8_is_int(s));
  for (U8 *curr = s.str + s.size - 1; curr >= s.str; --curr, decimal *= 10) {
    res += (*curr - '0') * decimal;
  }
  
  return res;
}

// TODO: maybe implement `strtod`?
fn F64 f64_from_str8(String8 s) {
  U8 *end = s.str + s.size;
  return strtod((char *)s.str, (char **)&end);
}

/* Djb2: http://www.cse.yorku.ca/~oz/hash.html */
fn USZ str8_hash(String8 s) {
  USZ hash = 5381;
  for (USZ i = 0; i < s.size; ++i) {
    hash = (hash << 5) + hash + s.str[i];
  }
  
  return hash;
}

fn String8 str8_from_i64(Arena *arena, I64 n) {
  I64 sign = n;
  if (n < 0) {
    n = -n;
  }
  
  USZ i = 0, approx = 30;
  U8 *str = (U8 *)make(arena, U8, approx);
  for (; n > 0; ++i, n /= 10) {
    str[i] = n % 10 + '0';
  }
  
  if (sign < 0) {
    str[i++] = '-';
  }
  
  for (USZ j = 0, k = i - 1; j < k; ++j, --k) {
    U8 tmp = str[k];
    str[k] = str[j];
    str[j] = tmp;
  }
  
  arena_pop(arena, approx - i);
  
  return (String8) {
    .str = str,
    .size = i,
  };
}

fn String8 str8_from_u64(Arena *arena, U64 n) {
  USZ i = 0, approx = 30;
  U8 *str = (U8 *)make(arena, U8, approx);
  for (; n > 0; ++i, n /= 10) {
    str[i] = n % 10 + '0';
  }
  
  for (USZ j = 0, k = i - 1; j < k; ++j, --k) {
    U8 tmp = str[k];
    str[k] = str[j];
    str[j] = tmp;
  }
  
  arena_pop(arena, approx - i);
  
  return (String8) {
    .str = str,
    .size = i,
  };
}

fn String8 str8_from_f64(Arena *arena, F64 n) {
  USZ approx = 100, size = 0;
  U8 *str = (U8 *)make(arena, U8, approx);
  
  // TODO: maybe implement `sprintf`?
  size = sprintf((char *)str, "%f", n);
  arena_pop(arena, approx - size);
  
  return (String8) {
    .str = str,
    .size = size,
  };
}

fn USZ cstr_len(char *chars) {
  char *start = chars;
  for (; *start; ++start)
    ;
  
  return start - chars;
}

fn String8 str8_format(Arena *arena, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  String8 res = str8_format_va(arena, fmt, args);
  va_end(args);
  
  return res;
}

fn String8 str8_format_va(Arena *arena, const char *fmt, va_list args) {
  va_list args2;
  va_copy(args2, args);
  U32 needed_bytes = vsnprintf(0, 0, fmt, args2) + 1;
  
  String8 res;
  res.str = (U8 *)make(arena, U8, needed_bytes);
  res.size = vsnprintf((char *)res.str, needed_bytes, fmt, args);
  res.str[res.size] = 0;
  
  va_end(args2);
  return res;
}

inline String8 str8_prefix(String8 s, USZ end) {
  return (String8) {
    .str = s.str,
    .size = ClampTop(s.size, end)
  };
}

inline String8 str8_postfix(String8 s, USZ start) {
  return (String8) {
    .str = s.str + start,
    .size = (s.size >= start ? s.size - start : 0)
  };
}

inline String8 substr(String8 s, USZ end) {
  return (String8) {
    .str = s.str,
    .size = ClampTop(s.size, end)
  };
}

inline String8 str8_range(String8 s, USZ start, USZ end) {
  return (String8) {
    .str = s.str + start,
    .size = ClampTop(end, s.size) - start
  };
}

inline B32 str8_ends_with(String8 s, char ch) { return s.str[s.size - 1] == ch; }

fn String8 str8_lcs(Arena *arena, String8 s1, String8 s2) {
#define at(m,i,j) m.x[(i)*m.n + (j)]
  typedef struct Matrix Matrix;
  struct Matrix {
    USZ *x;
    USZ m;
    USZ n;
  };
  
  USZ m = s1.size + 1;
  USZ n = s2.size + 1;
  
  Matrix c = {
    .x = make(arena, USZ, m*n),
    .m = m,
    .n = n,
  };
  
  for(USZ i = 1; i < m; ++i) {
    for(USZ j = 1; j < n; ++j) {
      if(s1.str[i-1] == s2.str[j-1]) {
        at(c,i,j) = at(c,i-1,j-1) + 1;
      } else {
        at(c,i,j) = Max(at(c,i-1,j), at(c,i,j-1));
      }
    }
  }
  
  USZ size = at(c,m-1,n-1);
  U8 *str = make(arena, U8, size);
  USZ idx = size - 1;
  for(USZ i = m - 1, j = n - 1; i > 0 && j > 0;) {
    if(at(c,i,j) == at(c,i-1,j)) {
      --i;
    } else if(at(c,i,j) == at(c,i,j-1)) {
      --j;
    } else {
      str[idx] = s1.str[i - 1];
      --idx;
      --i;
      --j;
    }
  }
#undef at
  return str8(str, size);
}

fn String8 str8_upper(Arena *arena, String8 s) {
  String8 res = {.str = (U8 *)make(arena, U8, s.size), .size = s.size};
  
  for (USZ i = 0; i < s.size; ++i) {
    res.str[i] = char_to_upper(s.str[i]);
  }
  
  return res;
}

fn String8 str8_lower(Arena *arena, String8 s) {
  String8 res = {.str = (U8 *)make(arena, U8, s.size), .size = s.size};
  
  for (USZ i = 0; i < s.size; ++i) {
    res.str[i] = char_to_lower(s.str[i]);
  }
  
  return res;
}

fn String8 str8_capitalize(Arena *arena, String8 s) {
  String8 res = {.str = (U8 *)make(arena, U8, s.size), .size = s.size};
  
  res.str[0] = char_to_upper(s.str[0]);
  for (USZ i = 1; i < s.size; ++i) {
    if (char_is_space(s.str[i])) {
      res.str[i] = s.str[i];
      ++i;
      res.str[i] = char_to_upper(s.str[i]);
    } else {
      res.str[i] = char_to_lower(s.str[i]);
    }
  }
  
  return res;
}

fn String8List str8_split(Arena *arena, String8 s, char ch) {
  String8List res = {0};
  
  USZ prev = 0;
  for (USZ i = 0; i < s.size;) {
    if (s.str[i] == ch) {
      if (prev != i) {
        str8_list_append(arena, res, str8_range(s, prev, i));
      }
      
      do {
        prev = ++i;
      } while (s.str[i] == ch);
    } else {
      ++i;
    }
  }
  
  if (prev != s.size) {
    str8_list_append(arena, res, str8_range(s, prev, s.size));
  }
  
  return res;
}

fn USZ str8_find_first(String8 s, char ch) {
  for (U8 *curr = s.str; curr < s.str + s.size + 1; ++curr) {
    if (*curr == ch) {
      return curr - s.str;
    }
  }
  
  return 0;
}

fn USZ str8_find_first_substr(String8 haystack, String8 needle) {
  if (haystack.size < needle.size) {
    return 0;
  }
  
  for (USZ i = 0; i < haystack.size; ++i) {
    if (haystack.str[i] == needle.str[0]) {
      for (USZ j = 0; i < needle.size; ++j) {
        if (haystack.str[i + j] != needle.str[j]) {
          goto outer;
        }
      }
      
      return i;
    }
    
    outer: ;
  }
  
  return 0;
}

fn B32 str8_contains(String8 s, char ch) {
  for (U8 *curr = s.str; curr < s.str + s.size + 1; ++curr) {
    if (*curr == ch) {
      return 1;
    }
  }
  
  return 0;
}

fn B32 char_is_space(U8 ch) { return ch == ' '; }
fn B32 char_is_slash(U8 ch) { return ch == '/'; }
fn B32 char_is_upper(U8 ch) { return ch >= 'A' && ch <= 'Z'; }
fn B32 char_is_lower(U8 ch) { return ch >= 'a' && ch <= 'z'; }
fn B32 char_is_digit(U8 ch) { return ch >= '0' && ch <= '9'; }
fn B32 char_is_alpha(U8 ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

fn B32 char_is_alphanumeric(U8 ch) {
  return char_is_digit(ch) || char_is_alpha(ch);
}

fn U8 char_to_upper(U8 ch) { return char_is_lower(ch) ? ch - 32 : ch; }
fn U8 char_to_lower(U8 ch) { return char_is_upper(ch) ? ch + 32 : ch; }
fn U8 str8_get_separator_from_os() {
#if OS_WINDOWS
  return '\\';
#else
  return '/';
#endif
}

// =============================================================================
// Other UTF strings
B32 str16_match(String16 s1, String16 s2) {
  if (s1.size != s2.size) {
    return 0;
  }
  
  for (USZ i = 0; i < s1.size; ++i) {
    if (s1.str[i] != s2.str[i]) {
      return 0;
    }
  }
  
  return 1;
}

fn B32 str32_match(String32 s1, String32 s2) {
  if (s1.size != s2.size) {
    return 0;
  }
  
  for (USZ i = 0; i < s1.size; ++i) {
    if (s1.str[i] != s2.str[i]) {
      return 0;
    }
  }
  
  return 1;
}

// =============================================================================
// UTF string conversion
fn String8 str8_from_str16(Arena *arena, String16 in) {
  USZ res_size = 0, approx_size = in.size * 4;
  U8 *bytes = (U8 *)make(arena, U8, approx_size), *res_offset = bytes;
  
  Codepoint codepoint = {0};
  for (U16 *start = in.str, *end = in.str + in.size; start < end;
       start += codepoint.size) {
    codepoint = utf16_decode(start);
    
    U8 utf8_codepoint_size = utf8_encode(res_offset, codepoint);
    res_size += utf8_codepoint_size;
    res_offset += utf8_codepoint_size;
  }
  
  arena_pop(arena, (approx_size - res_size));
  return (String8) {.str = bytes, .size = res_size};
}

fn String8 str8_from_str32(Arena *arena, String32 in) {
  USZ res_size = 0, approx_size = in.size * 4;
  U8 *bytes = (U8 *)make(arena, U8, approx_size), *res_offset = bytes;
  
  Codepoint codepoint = {0};
  for (U32 *start = in.str, *end = in.str + in.size; start < end;
       start += codepoint.size) {
    codepoint = utf32_decode(start);
    
    U8 utf8_codepoint_size = utf8_encode(res_offset, codepoint);
    res_size += utf8_codepoint_size;
    res_offset += utf8_codepoint_size;
  }
  
  arena_pop(arena, (approx_size - res_size));
  return (String8) {.str = bytes, .size = res_size};
}

fn String16 str16_from_str8(Arena *arena, String8 in) {
  USZ res_size = 0, approx_size = in.size * 2;
  U16 *words = (U16 *)make(arena, U16, approx_size), *res_offset = words;
  
  Codepoint codepoint = {0};
  for (U8 *start = in.str, *end = in.str + in.size; start < end;
       start += codepoint.size) {
    codepoint = utf8_decode(start);
    
    U8 utf16_codepoint_size = utf16_encode(res_offset, codepoint);
    res_size += utf16_codepoint_size;
    res_offset += utf16_codepoint_size;
  }
  
  arena_pop(arena, (approx_size - res_size));
  String16 res = {.str = words, .size = res_size};
  return res;
}

fn String16 str16_from_str32(Arena *arena, String32 *in) {
  USZ res_size = 0, approx_size = in->size * 2;
  U16 *words = (U16 *)make(arena, U16, approx_size), *res_offset = words;
  
  Codepoint codepoint = {0};
  for (U32 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = utf32_decode(start);
    
    U8 utf16_codepoint_size = utf16_encode(res_offset, codepoint);
    res_size += utf16_codepoint_size;
    res_offset += utf16_codepoint_size;
  }
  
  arena_pop(arena, (approx_size - res_size));
  String16 res = {.str = words, .size = res_size};
  return res;
}

fn String32 str32_from_str8(Arena *arena, String8 in) {
  USZ res_size = 0, approx_size = in.size * 2;
  U32 *dwords = (U32 *)make(arena, U32, approx_size), *res_offset = dwords;
  
  Codepoint cp = {0};
  for (U8 *start = in.str, *end = in.str + in.size; start < end;
       start += cp.size, ++res_size) {
    cp = utf8_decode(start);
    *res_offset++ = cp.codepoint;
  }
  
  arena_pop(arena, (approx_size - res_size));
  String32 res = {.str = dwords, .size = res_size};
  return res;
}

fn String32 str32_from_str16(Arena *arena, String16 *in) {
  USZ res_size = 0, approx_size = in->size * 2;
  U32 *dwords = (U32 *)make(arena, U32, approx_size), *res_offset = dwords;
  
  Codepoint cp = {0};
  for (U16 *start = in->str, *end = in->str + in->size; start < end;
       start += cp.size, ++res_size) {
    cp = utf16_decode(start);
    *res_offset++ = cp.codepoint;
  }
  
  arena_pop(arena, (approx_size - res_size));
  String32 res = {.str = dwords, .size = res_size};
  return res;
}
