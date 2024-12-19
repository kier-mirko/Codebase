#ifndef BASE_HASHMAP
#define BASE_HASHMAP

#include "base.h"
#include "dynarray.hpp"

namespace Base {
  template <typename T, typename U>
  struct HashMap {
    usize size;
    usize (*hasher)(T);

    struct Slot {
      T key;
      U value;
      Slot *next;
    };
    DynArray<Slot> slots;

    HashMap(Arena *arena, usize (*hasher)(T), usize size = 16) :
      size(size), hasher(hasher), slots(arena, size) {}

    bool insert(Arena *arena, const T &key, const U &value) {
      usize idx = hasher(key) % slots.size;

      Slot *new_slot = (Slot *) New(arena, Slot);
      if (!new_slot) { return false; }

      new_slot->key = key;
      new_slot->value = value;
      new_slot->next = slots[idx].next;
      slots[idx].next = new_slot;

      size += 1;
      return true;
    }

    U* search(const T &key) {
      usize idx = hasher(key) % slots.size;
      for (Slot *curr = slots[idx].next; curr; curr = curr->next) {
	if (key == curr->key) {
	  return &curr->value;
	}
      }

      return 0;
    }

    bool remove(const T &key) {
      usize idx = hasher(key) % slots.size;
      for (Slot *curr = slots[idx].next, *prev = &slots[idx];
	   curr; curr = curr->next, prev = prev->next) {
	if (key == curr->key) {
	  prev->next = curr->next;
	  return true;
	}
      }

      return false;
    }

    inline U& operator[](const T &key) {
      return *search(key);
    }
  };
}

#endif
