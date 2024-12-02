#include "decision_tree.h"

#define TableSize 20

struct OccSlot {
  struct HashNode *first;
  struct HashNode *last;
};

struct HashNode {
  String8 name;
  usize count;

  struct HashNode *next;
  struct HashNode *prev;

  struct OccSlot targets[TableSize];
};

fn struct HashNode *occmapSearch(struct OccSlot map[TableSize],
				 String8 category) {
  usize idx = strHash(category) % TableSize;
  for (struct HashNode *curr = map[idx].first; curr; curr = curr->next) {
    if (strEq(curr->name, category)) {
      return curr;
    }
  }

  return 0;
}

fn bool occmapInsert(Arena *arena, struct OccSlot map[TableSize],
		     String8 category) {
  usize idx = strHash(category) % TableSize;

  struct HashNode *new_node = New(arena, struct HashNode);
  if (!new_node) {
    printf("\tArena out of space!\n");
    return false;
  }

  new_node->name = category;
  new_node->count = 1;
  DLLPushBack(map[idx].first, map[idx].last, new_node);

  return true;
}

fn DecisionTreeNode ai_makeDTNode(Arena *arena, CSV config,
				  struct OccSlot(*maps)[TableSize],
				  usize n_features, usize target_idx) {
  usize row_count = 0;

  for (StringStream row = csv_nextRow(arena, &config);
       row.size != 0;
       row = csv_header(arena, &config), ++row_count) {
    usize i = 0;
    String8 *row_entries = Newarr(arena, String8, row.size);
    for (StringNode *r = row.first; r && i < n_features; r = r->next, ++i) {
      row_entries[i] = r->value;
    }

    for (i = 0; i < n_features; ++i) {
      struct HashNode *node = occmapSearch(maps[i], row_entries[i]);
      if (node) {
	node->count++;
      } else if (!occmapInsert(arena, maps[i], row_entries[i])) {
	/* TODO: write to tmp file maybe? */
	Assert(false);
      }

      if (i != target_idx) {
	struct HashNode *node = occmapSearch(maps[i], row_entries[i]);
	struct HashNode *tnode = occmapSearch(node->targets, row_entries[target_idx]);
	if (tnode) {
	  tnode->count++;
	} else if (!occmapInsert(arena, node->targets, row_entries[target_idx])) {
	  /* TODO: write to tmp file maybe? */
          Assert(false);
        }
      }
    }
  }
  /* TODO: write to tmp file here too? */

  /* Print table */
  for (usize i = 0; i < n_features; ++i) {
    for (usize j = 0; j < TableSize; ++j) {
      for (struct HashNode *curr = maps[i][j].first; curr; curr = curr->next) {
	printf("`%.*s`: %ld\t", Strexpand(curr->name), curr->count);

	for (usize k = 0; k < TableSize; ++k) {
	  for (struct HashNode *currf = curr->targets[k].first;
	       currf; currf = currf->next) {
	    printf("`%.*s`:%ld ", Strexpand(currf->name), currf->count);
	  }
	}

	printf("\n");
      }
    }
    printf("\n");
  }

  return (DecisionTreeNode) {0};
}

fn DecisionTreeNode ai_buildDecisionTree(Arena *arena, CSV config,
                                         String8 *header, usize n_features,
                                         usize target_feature, f64 treshold) {
  Assert(target_feature <= n_features && target_feature > 0 &&
         n_features > 1);

  /* Read the header line. */
  if (!header) {
    header = Newarr(arena, String8, n_features);
    StringStream h = csv_header(arena, &config);
    usize i = 0;
    for (StringNode *col = h.first; col && i < n_features;
         col = col->next, ++i) {
      header[i] = col->value;
    }
  }

  struct OccSlot (*slots)[TableSize] = Newarr(arena, struct OccSlot[TableSize],
					      n_features);
  return ai_makeDTNode(arena, config, slots, n_features, --target_feature);
}
