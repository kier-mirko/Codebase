#pragma once

#include "base.h"
#include "arena.hpp"

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

struct StringNode {
  StringNode *next;
  String8 value;
};

struct StringStream {
  StringNode *first;
  StringNode *last;
  size_t size;
};

fn String8 str8FromStream(Arena *arena, StringStream *stream);
fn void stringstreamAppend(Arena *arena, StringStream *strlist, String8 other);

fn String8 str8(char *chars);
fn String8 str8(char *chars, size_t len);

fn bool strIsSignedInteger(String8 s);
fn bool strIsInteger(String8 s);
fn bool strIsFloating(String8 s);
fn i64 i64FromStr(String8 s);
fn u64 u64FromStr(String8 s);
fn f64 f64FromStr(String8 s);

fn String8 stringifyI64(Arena *arena, i64 n);
fn String8 stringifyU64(Arena *arena, u64 n);
fn String8 stringifyF64(Arena *arena, f64 n);

fn size_t strlen(char *chars);

fn String8 strfmt(Arena *arena, const char *fmt, ...);
fn String8 strfmt(Arena *arena, const char *fmt, va_list args);

constexpr fn String8 strPrefix(String8 s, size_t end);
constexpr fn String8 strPostfix(String8 s, size_t start);
constexpr fn String8 substr(String8 s, size_t end);
constexpr fn String8 substr(String8 s, size_t start, size_t end);
constexpr fn bool strEndsWith(String8 s, char ch);
constexpr fn String8 longestCommonSubstring(Arena *arena, String8 s1,
                                            String8 s2);

fn String8 upperFromStr(Arena *arena, String8 s);
fn String8 lowerFromStr(Arena *arena, String8 s);
fn String8 capitalizeFromStr(Arena *arena, String8 s);

fn StringStream strSplit(Arena *arena, String8 s, char ch);
fn bool strContains(String8 s, char ch);

constexpr fn bool charIsSpace(u8 ch);
constexpr fn bool charIsSlash(u8 ch);
constexpr fn bool charIsUpper(u8 ch);
constexpr fn bool charIsLower(u8 ch);
constexpr fn bool charIsDigit(u8 ch);
constexpr fn bool charIsAlpha(u8 ch);
constexpr fn bool charIsAlphanumeric(u8 ch);

constexpr fn u8 charToUpper(u8 ch);
constexpr fn u8 charToLower(u8 ch);
constexpr fn u8 getCorrectPathSeparator();
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
