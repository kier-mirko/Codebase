#include "decision_tree.h"

/* implement log2? */
#include <math.h>

#define TableSize 20

struct OccSlot {
  struct HashNode *first;
  struct HashNode *last;

  usize size;
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

  map[idx].size += 1;
  DLLPushBack(map[idx].first, map[idx].last, new_node);

  return true;
}

inline fn f64 ai_entropy(f64 val) {
  return val ? -val * log2(val) : 0;
}

fn f64 ai_computeEntropy(struct OccSlot map[TableSize], usize row_count) {
  Assert(row_count > 0);

  f64 res = 0;
  for (usize i = 0; i < TableSize; ++i) {
    for (struct HashNode *curr = map[i].first; curr; curr = curr->next) {
      res += ai_entropy((f64)curr->count / (f64)row_count);
    }
  }

  return res;
}

fn usize ai_maxInformationGain(struct OccSlot(*maps)[TableSize],
			       usize n_features, usize target_idx,
			       usize row_count) {
  f64 max_gain = 0;
  usize max_gain_idx = 0;
  f64 target_entropy = ai_computeEntropy(maps[target_idx], row_count);
  printf("\t\tTarget entropy: %.16lf\n\n", target_entropy);

  for (usize feature = 0; feature < n_features; ++feature) {
    if (feature == target_idx) { continue; }

    f64 entropy = 0;
    printf("\t");
    for (usize slot = 0; slot < TableSize; ++slot) {
      for (struct HashNode *curr = maps[feature][slot].first; curr;
           curr = curr->next) {

	printf("%ld/%ld * entropy(", curr->count, row_count);
	for (usize target_category = 0; target_category < TableSize;
	     ++target_category) {
	  for (struct HashNode *currf = curr->targets[target_category].first;
	       currf; currf = currf->next) {
	    printf("%ld/%ld ", currf->count, curr->count);
	    entropy += (f64)curr->count/(f64)row_count
		       * ai_entropy((f64)currf->count/(f64)curr->count);
	  }
	}

	printf("\b)");
      }
      if (maps[feature][slot].first)
	printf(" + ");
    }

    f64 gain = target_entropy - entropy;
    printf("\b\b\n\t\tentropy: %.16lf\n", entropy);
    printf("\t\tgain: %.16lf\n\n", gain);

    if (max_gain < gain) {
      max_gain = gain;
      max_gain_idx = feature;
    }
  }

  printf("\tMax gain: %.16lf\n", max_gain);
  return max_gain_idx;
}

fn DecisionTreeNode ai_makeDTNode(Arena *arena, CSV config,
				  struct OccSlot(*maps)[TableSize],
				  usize n_features, usize target_idx) {
  usize data_row_start_at = config.offset;
  usize row_count = 0;

  /* Occurrences counter */
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
	struct HashNode *tnode = occmapSearch(node->targets,
					      row_entries[target_idx]);
	if (tnode) {
	  tnode->count++;
	} else if (!occmapInsert(arena, node->targets,
				 row_entries[target_idx])) {
	  /* TODO: write to tmp file maybe? */
          Assert(false);
        }
      }
    }
  }

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

  usize feature2split_by = ai_maxInformationGain(maps, n_features, target_idx,
						 row_count);
  printf("\tFeature to split by: %ld\n", feature2split_by);

  usize branches = 0;
  for (usize j = 0; j < TableSize; ++j) {
    branches += maps[feature2split_by][j].size;
  }

  printf("\tThe dataset will be split into %ld branches\n\n", branches);

  /* Create `branches` tmp files */
  isize *fds = Newarr(arena, isize, branches);
  for (usize i = 0; i < branches; ++i) {
    fds[i] = fs_makeTmpFd();
  }

  /* Iterator over the entire CSV file and write into the corresponding tmp */
  /*   file the CSV row. */
  config.offset = data_row_start_at;
  for (StringStream row = csv_nextRow(arena, &config);
       row.size != 0;
       row = csv_header(arena, &config), ++row_count) {
    usize i = 0;
    String8 *row_entries = Newarr(arena, String8, row.size);
    for (StringNode *r = row.first; r && i < n_features; r = r->next, ++i) {
      row_entries[i] = r->value;
    }

    i = (feature2split_by == 0 ? 1 : 0);
    usize file = strHash(row_entries[feature2split_by]) % branches;
    fs_fappend(fds[file], row_entries[i++]);
    for (; i < row.size; ++i) {
      if (i == feature2split_by) { continue; }
      fs_fappend(fds[file], Strlit(","));
      fs_fappend(fds[file], row_entries[i]);
    }
    fs_fappend(fds[file], Strlit("\n"));
  }

  /* Call recursively to create the decision tree child nodes. */

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
