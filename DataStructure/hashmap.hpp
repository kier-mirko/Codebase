#ifndef BASE_HASHMAP
#define BASE_HASHMAP

#include "../base.h"

#include "dynarray.hpp"

template <typename T, typename U>
struct HashMap {
  usize size;
  usize (*hasher)(T);

  struct KVNode {
    T key;
    U value;
    KVNode *next;
    KVNode *prev;
  };

  struct Slot {
    KVNode *first;
    KVNode *last;
  };

  DynArray<Slot> slots;

  HashMap(Arena *arena, usize (*hasher)(T), usize size = 16) :
    size(size), hasher(hasher), slots(arena, size) {}

  bool insert(Arena *arena, const T &key, const U &value) {
    usize idx = hasher(key) % slots.size;

    KVNode *existing_node = 0;
    for(KVNode *n = slots[idx].first; n; n = n->next) {
      if (key == n->key) {
        existing_node = n;
        break;
      }
    }
    if(existing_node == 0) {
      existing_node = New(arena, KVNode);
      if(existing_node == 0) return false;
      existing_node->key = key;
      existing_node->value = value;
    }
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

  U* fromKey(Arena *arena, const T &key, const U &default_val = {}) {
    usize idx = hasher(key) % slots.size;
    KVNode *existing_node = 0;
    for(KVNode *n = slots[idx].first; n; n = n->next) {
      if (n->key == key) {
        existing_node = n;
        break;
      }
    }
    if(existing_node == 0) {
      existing_node = New(arena, KVNode);
      existing_node->key = key;
      existing_node->value = default_val;
    }

    return existing_node->value;
  }

  void remove(const T &key) {
    usize idx = hasher(key) % slots.size;

    for(KVNode *n = slots[idx].first; n; n = n->next) {
      if (n->key == key) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
      }
    }
  }

  inline U& operator[](const T &key) {
    return *search(key);
  }
};

#endif
