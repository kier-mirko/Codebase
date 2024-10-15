#ifndef BASE_STRING
#define BASE_STRING

#include "arena.cpp"
#include "base.cpp"

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
#define strlit(STR)                                                            \
  (::Base::String8{.str = (u8 *)(STR), .size = sizeof(STR) - 1})
#define strexpand(STR) (int)((STR).size), (char *)((STR).str)

// `size` and `cstr` are to be considered immutable
namespace Base {
struct String8 {
  u8 *str;
  size_t size = 0;
  size_t length = size;

  constexpr char operator[](size_t idx) const {
    return idx > this->size ? 0 : this->str[idx];
  }

  constexpr bool operator==(String8 &other) const {
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
} // namespace Base

fn constexpr Base::String8 prefix(const Base::String8 *s, size_t end) {
  return {.str = s->str, .size = clamp_top(s->size, end)};
}

fn constexpr Base::String8 postfix(const Base::String8 *s, size_t start) {
  return {.str = s->str + start, .size = s->size};
}

fn constexpr Base::String8 substr(const Base::String8 *s, size_t end) {
  return {.str = s->str, .size = clamp_top(s->size, end)};
}

fn constexpr Base::String8 substr(const Base::String8 *s, size_t start,
                                  size_t end) {
  return {.str = s->str + start, .size = clamp_top(end, s->size) - start};
}

fn constexpr Base::String8 split(const Base::String8 *s, char ch) {
  size_t newsize = 0;

  for (newsize = 0; newsize < s->size && s->str[newsize] != ch; ++newsize)
    ;
  return {.str = s->str, .size = newsize};
}

fn Base::Codepoint decode_utf8(u8 *glyph_start) {
  Base::Codepoint res = {0};

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
    assert(false);
  }

  return res;
}

fn u8 encode_utf8(u8 *res, Base::Codepoint cp) {
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
    assert(false);
    return -1;
  }
}

// =============================================================================
// UTF-16 Strings
// `size` and `cstr` are to be considered immutable
namespace Base {
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
} // namespace Base

// No other operations are defined. Use UTF-8 strings instead.

fn Base::Codepoint decode_utf16(u16 *glyph_start) {
  Base::Codepoint res = {0};

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
    assert(false);
  }

  return res;
}

fn u8 encode_utf16(u16 *res, Base::Codepoint cp) {
  if (cp.codepoint <= 0xD7FF ||
      (cp.codepoint >= 0xE000 && cp.codepoint <= 0xFFFF)) {
    res[0] = cp.codepoint;
    return 1;
  } else if (cp.codepoint >= 0x10000 && cp.codepoint <= 0x10FFFF) {
    res[0] = ((cp.codepoint - 0x10000) >> 10) + 0xD800;
    res[1] = ((cp.codepoint - 0x10000) & 0x3FF) + 0xDC00;
    return 2;
  } else {
    assert(false);
    return -1;
  }
}

// =============================================================================
// UTF-32 Strings
// `size` and `cstr` are to be considered immutable
namespace Base {
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
} // namespace Base

// No other operations are defined. Use UTF-8 strings instead.

inline fn Base::Codepoint decode_utf32(u32 *glyph_start) {
  return {.codepoint = *glyph_start, .size = 1};
}

inline fn u8 encode_utf32(u32 *res, Base::Codepoint cp) {
  res[0] = cp.codepoint;
  return 1;
}

// =============================================================================
// UTF-32 from UTF-8/16
fn Base::String8 utf8_from_16(Base::Arena *arena, Base::String16 *in) {
  size_t res_size = 0, approx_size = in->size * 4;
  u8 *res = makearr(arena, u8, approx_size), *res_offset = res;

  Base::Codepoint codepoint = {0};
  for (u16 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decode_utf16(start);

    u8 utf8_codepoint_size = encode_utf8(res_offset, codepoint);
    res_size += utf8_codepoint_size;
    res_offset += utf8_codepoint_size;
  }

  arena_pop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

fn Base::String8 utf8_from_32(Base::Arena *arena, Base::String32 *in) {
  size_t res_size = 0, approx_size = in->size * 4;
  u8 *res = makearr(arena, u8, approx_size), *res_offset = res;

  Base::Codepoint codepoint = {0};
  for (u32 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decode_utf32(start);

    u8 utf8_codepoint_size = encode_utf8(res_offset, codepoint);
    res_size += utf8_codepoint_size;
    res_offset += utf8_codepoint_size;
  }

  arena_pop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

// =============================================================================
// UTF-16 from UTF-16/32
fn Base::String16 utf16_from_8(Base::Arena *arena, Base::String8 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u16 *res = makearr(arena, u16, approx_size), *res_offset = res;

  Base::Codepoint codepoint = {0};
  for (u8 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decode_utf8(start);

    u8 utf16_codepoint_size = encode_utf16(res_offset, codepoint);
    res_size += utf16_codepoint_size;
    res_offset += utf16_codepoint_size;
  }

  arena_pop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

fn Base::String16 utf16_from_32(Base::Arena *arena, Base::String32 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u16 *res = makearr(arena, u16, approx_size), *res_offset = res;

  Base::Codepoint codepoint = {0};
  for (u32 *start = in->str, *end = in->str + in->size; start < end;
       start += codepoint.size) {
    codepoint = decode_utf32(start);

    u8 utf16_codepoint_size = encode_utf16(res_offset, codepoint);
    res_size += utf16_codepoint_size;
    res_offset += utf16_codepoint_size;
  }

  arena_pop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

// =============================================================================
// UTF-32 from UTF-8/16
fn Base::String32 utf32_from_8(Base::Arena *arena, Base::String8 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u32 *res = makearr(arena, u32, approx_size), *res_offset = res;

  Base::Codepoint cp = {0};
  for (u8 *start = in->str, *end = in->str + in->size; start < end;
       start += cp.size, ++res_size) {
    cp = decode_utf8(start);
    *res_offset++ = cp.codepoint;
  }

  arena_pop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

fn Base::String32 utf32_from_16(Base::Arena *arena, Base::String16 *in) {
  size_t res_size = 0, approx_size = in->size * 2;
  u32 *res = makearr(arena, u32, approx_size), *res_offset = res;

  Base::Codepoint cp = {0};
  for (u16 *start = in->str, *end = in->str + in->size; start < end;
       start += cp.size, ++res_size) {
    cp = decode_utf16(start);
    *res_offset++ = cp.codepoint;
  }

  arena_pop(arena, (approx_size - res_size));
  return {.str = res, .size = res_size, .length = in->length};
}

#endif
