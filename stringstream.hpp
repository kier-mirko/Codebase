#pragma once

#include "base.h"
#include "string.hpp"

namespace Base {
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
} // namespace Base
