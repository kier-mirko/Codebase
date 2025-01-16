#ifndef BASE_DYNARRAY
#define BASE_DYNARRAY

#include "array.hpp"

template <typename T>
struct DynArray {
  USZ size;
  
  ArrayListNode<T> *first = 0;
  ArrayListNode<T> *last = 0;
  
  DynArray(Arena *arena, USZ initial_capacity = 8)
    : size(initial_capacity) {
    Assert(initial_capacity > 0);
    first = last = (ArrayListNode<T> *) make(arena, ArrayListNode<T>);
    first->block = Array<T>(arena, initial_capacity);
  }
  
  T& operator[](USZ i) {
    Assert(i < size);
    
    for (ArrayListNode<T> *it = first; it; it = it->next) {
      if (i < it->block.size) {
        return it->block[i];
      } else {
        i -= it->block.size;
      }
    }
    
    Assert(false);
    return first->block[0];
  }
  
  struct Iterator {
    ArrayListNode<T>* current;
    USZ idx;
    
    Iterator(ArrayListNode<T> *ptr, USZ idx) : current(ptr), idx(idx) {}
    
    T& operator*() { return current->block[idx]; }
    
    Iterator& operator++() {
      ++idx;
      if (idx >= current->block.size) {
        current = current->next;
        idx = 0;
      }
      
      return *this;
    }
    
    B32 operator==(const Iterator& other) {
      return current == other.current && idx == other.idx;
    }
    
    B32 operator!=(const Iterator& other) { return !(*this == other); }
  };
  
  Iterator begin() { return Iterator(first, 0); }
  Iterator end() { return Iterator(0, 0); }
};

template <typename T>
fn B32 dynarrayExpand(Arena *arena, DynArray<T> *arr,
                      USZ expansion_size);


#endif
