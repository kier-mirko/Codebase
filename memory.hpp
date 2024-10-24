#pragma once

#include "base.h"
#include "arena.hpp"

namespace Base {
void *memcpy(Arena *arena, void *dest, void *src, size_t size);
}
