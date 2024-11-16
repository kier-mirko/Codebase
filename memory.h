#pragma once

#include "base.h"
#include "arena.h"

void *memcopy(Arena *arena, void *dest, void *src, usize size);
