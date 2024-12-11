#ifndef BASE_DSU
#define BASE_DSU

#include "base.h"
#include "arena.h"
#include "hashmap.h"

namespace Base {
  template <typename T>
  struct UnionFindSet {
    HashMap<T, T> parent;
    HashMap<T, i64> rank;

    UnionFindSet(Arena *arena, usize (*hashfn)(T))
      : parent(arena, hashfn), rank(arena, hashfn) {}

    bool make(Arena *arena, T x) {
      return parent.insert(arena, x, x) && rank.insert(arena, x, 0L);
    }

    T& find(const T &x) {
      T &root = parent[x];
      if (root != x) {
	root = find(root);
      }

      return root;
    }

    void link(const T &x, const T &y) {
      i64 &xrank = rank[x];
      i64 &yrank = rank[y];

      if (xrank > yrank) {
	parent[y] = x;
      } else {
	parent[x] = y;
	if (xrank == yrank) {
	  ++yrank;
	}
      }
    }

    inline void merge(const T &x, const T &y) {
      link(find(x), find(y));
    }

    inline bool connected(const T &x, const T &y) {
      return find(x) == find(y);
    }
  };
}

#endif
