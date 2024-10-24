#pragma once

#include "base.h"
#include "arena.hpp"
#include "stringstream.hpp"

#include <stdarg.h>
#include <stdio.h>

#define Strlit(STR)                                                            \
  (::Base::String8{.str = (u8 *)(STR), .size = sizeof(STR) - 1})
#define StrlitInit(STR)                                                        \
  { (u8 *)(STR), sizeof(STR) - 1, }
#define Strexpand(STR) (int)((STR).size), (char *)((STR).str)

namespace Base {
struct StringStream;

// =============================================================================
// Unicode codepoint
struct Codepoint {
  u32 codepoint;
  u8 size;
};

fn Codepoint decodeUTF8(u8 *glyph_start);
fn Codepoint decodeUTF16(u16 *glyph_start);
inline fn Codepoint decodeUTF32(u32 *glyph_start);
fn u8 encodeUTF8(u8 *res, Codepoint cp);
fn u8 encodeUTF16(u16 *res, Codepoint cp);
inline fn u8 encodeUTF32(u32 *res, Codepoint cp);
// =============================================================================

// =============================================================================
// UTF-8 string
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

fn String8 str8(char *chars);
fn String8 str8(char *chars, size_t len);

fn size_t strlen(char *chars);

fn String8 formatStr(Arena *arena, const char *fmt, ...);
fn String8 formatStr(Arena *arena, const char *fmt, va_list args);

constexpr fn String8 prefix(const String8 s, size_t end);
constexpr fn String8 postfix(const String8 s, size_t start);
constexpr fn String8 substr(const String8 s, size_t end);
constexpr fn String8 substr(const String8 s, size_t start, size_t end);
constexpr fn String8 longestCommonSubstring(Arena *arena, String8 s1,
                                            String8 s2);

fn StringStream split(Arena *arena, String8 s, char ch);
// =============================================================================

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

// =============================================================================
// UTF string conversion

fn String8 UTF8From16(Arena *arena, String16 *in);
fn String8 UTF8From32(Arena *arena, String32 *in);

fn String16 UTF16From8(Arena *arena, String8 *in);
fn String16 UTF16From32(Arena *arena, String32 *in);

fn String32 UTF32From8(Arena *arena, String8 *in);
fn String32 UTF32From16(Arena *arena, String16 *in);

// =============================================================================
} // namespace Base
