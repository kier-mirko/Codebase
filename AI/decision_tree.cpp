#include "decision_tree.h"

/* implement log2? */
#include <math.h>

#define TableSize 20

struct Occurrence {
  String8 name;
  usize count;

  Base::HashMap<String8, Occurrence> targets;

  Occurrence(Arena *arena) : count(1), targets(arena, strHash, TableSize) {}
};

inline fn f64 ai_entropy(f64 val) {
  return val ? -val * log2(val) : 0;
}

fn f64 ai_computeEntropy(Base::HashMap<String8, Occurrence> *map, usize row_count) {
  Assert(row_count > 0);

  f64 res = 0;
  for (Base::HashMap<String8, Occurrence>::Slot slot : map->slots) {
    for (Base::HashMap<String8, Occurrence>::Slot *curr = &slot;
         curr; curr = curr->next) {
      res += ai_entropy((f64)curr->value.count / (f64)row_count);
    }
  }

  return res;
}

fn usize ai_maxInformationGain(Base::HashMap<String8, Occurrence> *maps,
			       usize n_features, usize target_idx,
			       usize row_count, f64 threshold) {
  f64 max_gain = 0;
  usize max_gain_idx = 0;
  f64 target_entropy = ai_computeEntropy(&maps[target_idx], row_count);
  printf("\t\tTarget entropy: %.16lf\n\n", target_entropy);

  for (usize feature = 0; feature < n_features; ++feature) {
    if (feature == target_idx) { continue; }

    f64 entropy = 0;
    printf("\t");
    for (Base::HashMap<String8, Occurrence>::Slot slot : maps[feature].slots) {
      Base::HashMap<String8, Occurrence>::Slot *curr = slot.next;
      for (; curr; curr = curr->next) {
	printf("%ld/%ld * entropy(", curr->value.count, row_count);

	for (Base::HashMap<String8, Occurrence>::Slot fslot : curr->value.targets.slots) {
	  for (Base::HashMap<String8, Occurrence>::Slot *currf = fslot.next;
	       currf; currf = currf->next) {
	    if (currf->value.count == 0) { continue; }
	    printf("%ld/%ld ", currf->value.count, curr->value.count);
	    entropy += (f64)curr->value.count/(f64)row_count
		       * ai_entropy((f64)currf->value.count/(f64)curr->value.count);
	  }
	}

	printf("\b)");
      }
      if (curr) {
	printf(" + ");
      }
    }

    f64 gain = target_entropy - entropy;
    printf("\n\t\tentropy: %.16lf\n", entropy);
    printf("\t\tgain: %.16lf\n\n", gain);

    if (max_gain < gain) {
      max_gain = gain;
      max_gain_idx = feature;
    }
  }

  printf("\tMax gain: %.16lf\n", max_gain);
  return max_gain < threshold ? -1 : max_gain_idx;
}

fn DecisionTreeNode *ai_makeDTNode(Arena *arena, CSV config,
				  Base::HashMap<String8, Occurrence> *maps,
				  usize n_features, usize target_idx, f64 threshold) {
  usize data_row_start_at = config.offset;
  usize row_count = 0;

  /* Occurrences counter */
  for (StringStream row = csv_nextRow(arena, &config);
       row.size != 0;
       row = csv_header(arena, &config), ++row_count) {
    usize i = 0;
    String8 *row_entries = (String8 *)Newarr(arena, String8, row.size);
    for (StringNode *r = row.first; r && i < n_features; r = r->next, ++i) {
      row_entries[i] = r->value;
    }

    for (i = 0; i < n_features; ++i) {
      Occurrence *node = maps[i].search(row_entries[i]);
      if (node) {
	node->count++;
      } else if (!maps[i].insert(arena, row_entries[i], Occurrence(arena))) {
	/* TODO: write to tmp file maybe? */
	Assert(false);
      }

      if (i != target_idx) {
	Occurrence *node = maps[i].search(row_entries[i]);
	Occurrence *tnode = node->targets.search(row_entries[target_idx]);
	if (tnode) {
	  tnode->count++;
	} else if (!node->targets.insert(arena, row_entries[target_idx], Occurrence(arena))) {
	  /* TODO: write to tmp file maybe? */
          Assert(false);
        }
      }
    }
  }

  /* Print table */
  for (usize i = 0; i < n_features; ++i) {
    for (Base::HashMap<String8, Occurrence>::Slot slot : maps[i].slots) {
      if (slot.next == 0) { continue; }

      for (Base::HashMap<String8, Occurrence>::Slot *curr = slot.next;
	   curr; curr = curr->next) {
	printf("`%.*s`: %ld\t", Strexpand(curr->key), curr->value.count);

	for (Base::HashMap<String8, Occurrence>::Slot fslot : curr->value.targets.slots) {
	  if (fslot.next == 0) { continue; }

	  for (Base::HashMap<String8, Occurrence>::Slot *currf = fslot.next;
	       currf; currf = currf->next) {
	    printf("`%.*s`: %ld, ", Strexpand(currf->key), currf->value.count);
	  }
	}

	printf("\n");
      }
    }
    printf("\n");
  }

  usize feature2split_by = ai_maxInformationGain(maps, n_features, target_idx,
						 row_count, threshold);
  printf("\tFeature to split by: %ld\n", feature2split_by);
  if (feature2split_by == -1) {
    return 0;
  }

  usize branches = 0;
  for (Base::HashMap<String8, Occurrence>::Slot &slot: maps[feature2split_by].slots) {
    for (Base::HashMap<String8, Occurrence>::Slot *curr = slot.next;
	 curr; curr = curr->next) {
      branches += 1;
    }
  }

  printf("\tThe dataset will be split into %ld branches\n\n", branches);

  /* Create `branches` tmp files */
  isize *fds = (isize *)Newarr(arena, isize, branches);
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
    String8 *row_entries = (String8 *)Newarr(arena, String8, row.size);
    for (StringNode *r = row.first; r && i < n_features; r = r->next, ++i) {
      row_entries[i] = r->value;
    }

    i = (feature2split_by == 0 ? 1 : 0);
    /* TODO: what if there's a collision? */
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

  return 0;
}

fn DecisionTreeNode *ai_buildDecisionTree(Arena *arena, CSV config,
                                         String8 *header, usize n_features,
                                         usize target_feature, f64 threshold) {
  Assert(target_feature <= n_features && target_feature > 0 &&
         n_features > 1);

  /* Read the header line. */
  if (!header) {
    header = (String8 *)Newarr(arena, String8, n_features);
    StringStream h = csv_header(arena, &config);
    usize i = 0;
    for (StringNode *col = h.first; col && i < n_features;
         col = col->next, ++i) {
      header[i] = col->value;
    }
  }

  using HashMap = Base::HashMap<String8, Occurrence>;
  auto maps = (Base::HashMap<String8, Occurrence> *) Newarr(arena, HashMap,
							    n_features);
  for (usize i = 0; i < n_features; ++i) {
    maps[i] = Base::HashMap<String8, Occurrence>(arena, strHash, TableSize);
  }

  return ai_makeDTNode(arena, config, maps, n_features, --target_feature, threshold);
}
