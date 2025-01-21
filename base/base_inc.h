#ifndef BASE_INC_H
#define BASE_INC_H

#include "base.h"
#include "list.h"
#include "memory.h"
#include "arena.h"
#include "thread_ctx.h"
#include "chrono.h"
#include "clock.h"
#include "string.h"

#if CPP
#  include "array.hpp"
#  include "dynarray.hpp"
#  include "hashmap.hpp"
#endif

// TODO(lb): find a place for this
#include "../image.h"

#endif
