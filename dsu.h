#ifndef BASE_DSU
#define BASE_DSU

typedef struct UnionFind {
  struct UnionFind *parent;
  u32 rank;

  void *value;
} UnionFind;

UnionFind make_set(void *elem) {
  return (UnionFind) {
    .parent = 0,
    .rank = 1,
    .value = elem,
  };
}

UnionFind *find_set(UnionFind *x) {
  if (!x || !x->parent) { return x; }

  for (; x->parent; x = x->parent);
  return x;
}

UnionFind *merge_set(UnionFind *x, UnionFind *y) {
  UnionFind *root_x = find_set(x);
  UnionFind *root_y = find_set(y);
  UnionFind *new_root = root_x;

  if (root_x == root_y) {
    return new_root;
  }

  if (root_x->rank >= root_y->rank) {
    root_y->parent = root_x;
    root_x->rank += 1;
  } else {
    root_x->parent = root_y;
    root_y->rank += 1;
    new_root = root_y;
  }

  return new_root;
}

#endif
