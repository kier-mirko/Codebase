#ifndef BASE_ARRAY
#define BASE_ARRAY

#include "base.h"

template <typename T, usize N>
struct Buffer {
  T values[N];
  usize size;

  Buffer() : values{0}, size(N) {}

  template<typename... Ts>
  Buffer(Ts... args) : values{args...}, size(N) {}

  T& operator[](usize i) {
    Assert(i < size);
    return values[i];
  }

  struct Iterator {
    T *current;

    explicit Iterator(T *ptr) : current(ptr) {}

    T& operator*() { return *current; }
    T* operator->() { return current; }

    Iterator& operator++() {
      current += 1;
      return *this;
    }

    Iterator& operator--() {
      current -= 1;
      return *this;
    }

    Iterator& operator+=(usize step) {
      current += step;
      return *this;
    }

    Iterator& operator-=(usize step) {
      current -= step;
      return *this;
    }

    bool operator==(const Iterator& other) { return current == other.current; }
    bool operator!=(const Iterator& other) { return current != other.current; }
  };

  Iterator begin() { return Iterator(&values[0]); }
  Iterator end() { return Iterator(&values[N]); }
};

template <typename T>
struct Array {
  T *values;
  usize size;

  Array(Arena *arena, usize size) :
    values((T *) Newarr(arena, T, size)), size(size) {
    Assert(size > 0);
  }

  T& operator[](usize i) {
    Assert(i < size);
    return values[i];
  }

  struct Iterator {
    T *current;

    explicit Iterator(T *ptr) : current(ptr) {}

    T& operator*() { return *current; }
    T* operator->() { return current; }

    Iterator& operator++() {
      current += 1;
      return *this;
    }

    Iterator& operator--() {
      current -= 1;
      return *this;
    }

    Iterator& operator+=(usize step) {
      current += step;
      return *this;
    }

    Iterator& operator-=(usize step) {
      current -= step;
      return *this;
    }

    bool operator==(const Iterator& other) { return current == other.current; }
    bool operator!=(const Iterator& other) { return current != other.current; }
  };

  Iterator begin() { return Iterator(&values[0]); }
  Iterator end() { return Iterator(&values[size]); }
};

template <typename T>
struct ArrayList {
  Array<T> block;

  ArrayList *next = 0;
  ArrayList *prev = 0;

  struct Iterator {
    ArrayList *current;
    usize idx;

    explicit Iterator(ArrayList *ptr, usize idx) :
      current(ptr), idx(idx) {}

    Iterator& operator++() {
      ++idx;
      if (idx >= current->block.size) {
	current = current->next;
	idx = 0;
      }

      return *this;
    }

    Iterator& operator--() {
      if (idx == 0) {
	current = current->prev;
	idx = current->block.size - 1;
      } else {
	--idx;
      }

      return *this;
    }

    T& operator*() { return current->block[idx]; }

    bool operator==(const Iterator& other) {
      return current == other.current && idx == other.idx;
    }
    bool operator!=(const Iterator& other) {
      return !(*this == other);
    }
  };

  Iterator begin() { return Iterator(this, 0); }
  Iterator begin(ArrayList *first) { return Iterator(first, 0); }
  Iterator end(ArrayList *last) { return Iterator(last, last->block.size); }
  Iterator end() {
    ArrayList *last = this;
    while (last->next) {
      last = last->next;
    }

    return Iterator(last, last->block.size);
  }

};

#endif
