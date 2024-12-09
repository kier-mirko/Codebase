#ifndef BASE_HASHMAP
#define BASE_HASHMAP

#include "base.h"
#include "dynarray.h"

namespace Base {
  template <typename T, typename U>
  struct Slot {
    T key;
    U value;
    usize size = 0;

    Slot *next = 0;
    Slot *first = 0;
    Slot *last = 0;
  };

  template <typename T, typename U>
  struct HashMap {
    usize size;
    usize (*hasher)(T);
    DynArray<Slot<T, U>> slots;

    HashMap(Arena *arena, usize (*hasher)(T), usize size = 128) :
      size(size), hasher(hasher), slots(arena, size) {}

    Slot<T, U>* operator[](usize i) {
      return &slots[i];
    }
  };

  template <typename T, typename U>
  fn bool hashInsert(Arena *arena, HashMap<T, U> *map,
		     const T &key, const U &value) {
    usize idx = map->hasher(key) % map->slots.size;
    Slot<T, U> &s = map->slots[idx];

    if (s.first) {
      using Slot = Slot<T, U>;
      auto new_slot = (Slot *) New(arena, Slot);
      new_slot->key = key;
      new_slot->value = value;

      s.first->size += 1;
      QueuePush(s.first, s.last, new_slot);
    } else {
      s.key = key;
      s.value = value;
      s.first = &s;
      s.last = &s;
      s.size = 1;
    }

    map->size += 1;
    return true;
  }

  template <typename T, typename U>
  fn U* hashSearch(HashMap<T, U> *map, const T &key) {
    usize idx = map->hasher(key) % map->slots.size;
    Slot<T, U> &s = map->slots[idx];

    for (Slot<T, U> *curr = s.first; curr; curr = curr->next) {
      if (key == curr->key) {
	return &curr->value;
      }
    }

    return 0;
  }

}

#endif
