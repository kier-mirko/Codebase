#ifndef BASE_HASHMAP
#define BASE_HASHMAP

#include "base.h"
#include "dynarray.h"

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

    HashMap(Arena *arena, usize (*hasher)(T), usize size = 128) :
      size(size), hasher(hasher), slots(arena, size) {}

    bool insert(Arena *arena, const T &key, const U &value) {
      usize idx = hasher(key) % slots.size;
      Slot *curr = &slots[idx];

      // TODO: can i use a tree instead?
      for (; curr->next; curr = curr->next) {
	if (curr->next->key == key) {
	  curr->next->value = value;
	  return true;
	}
      }

      curr = curr->next = (Slot *) New(arena, Slot);
      curr->key = key;
      curr->value = value;

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

    inline U& operator[](const T &key) {
      return *search(key);
    }
  };
}

#endif
