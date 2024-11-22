#include "decision_tree.h"

#define NULL_INFOGAIN USIZE_MAX
#define THRESHOLD_NOT_REACHED ISIZE_MAX

#define DISABLE_LOG() \
  i32 _STDOUT = dup(1);\
  close(1)

#define ENABLE_LOG() \
  dup2(_STDOUT, 1)

fn usize ai_indexFromCategory(String8 *domain, usize size,
			      String8 needle) {
  for (usize i = 0; i < size; ++i) {
    if (strEq(domain[i], needle)) {
      return i;
    }
  }

  return USIZE_MAX;
}

inline fn f64 ai_computeEntropy(f64 value) {
  return value > 0 ? -value * log2(value) : 0;
}

fn f64 ai_entropy(usize max_domain_size, usize n_features,
		  usize n_rows, usize feature_idx,
		  usize feature_size, usize *feature_table) {
  f64 entropy = 0;

  printf("\t\tfeature_idx: %ld\n", feature_idx);
  printf("entropy(");
  for (usize row = 0; row < feature_size; ++row) {
    usize count = 0;
    for (usize depth = 0; depth < max_domain_size; ++depth) {
      usize index = row * (n_features * max_domain_size) +
                    feature_idx * max_domain_size +
		    depth;
      count += feature_table[index];
    }

    printf("%ld/%ld, ", count, n_rows);
    entropy += ai_computeEntropy((f64)count / n_rows);
  }
  printf(")\n");

  return entropy;
}

fn usize ai_maxInformationGain(f64 threshold,
			       usize max_domain_size,
			       usize n_features, usize n_rows,
			       usize target_feature_idx,
			       usize *domain_sizes,
			       usize *feature_table) {
  f64 target_entropy =
      ai_entropy(max_domain_size, n_features, n_rows,
		 target_feature_idx,
                 domain_sizes[target_feature_idx], feature_table);
  printf("\tentropy(D) = %.16lf\n", target_entropy);

  if (target_entropy == 0) {
    return NULL_INFOGAIN;
  }

  usize res = 0;
  f64 max_gain = 0;
  for (usize i = 0; i < n_features; ++i) {
    if (i == target_feature_idx) {
      continue;
    }

    f64 entropy = 0;
    for (usize depth = 0; depth < max_domain_size; ++depth) {
      usize occurance = 0;
      for (usize row = 0; row < domain_sizes[target_feature_idx]; ++row) {
        occurance += feature_table[row * (n_features * max_domain_size) +
                                   i * max_domain_size + depth];
      }

      if (occurance == 0) {
        break;
      }

      printf("occurance = %ld, rows = %ld\n", occurance, n_rows);

      f64 category_entropy = 0;
      for (usize row = 0; row < domain_sizes[target_feature_idx]; ++row) {
        f64 tmp = (f64)occurance / n_rows *
                  ai_computeEntropy(
                      feature_table[row * (n_features * max_domain_size) +
                                    i * max_domain_size + depth] /
                      (f64)occurance);
        category_entropy += tmp;
        entropy += tmp;

        printf("feature_table[%ld][%ld][%ld] = %ld\n", row, i, depth,
               feature_table[row * (n_features * max_domain_size) +
                             i * max_domain_size + depth]);
      }
      printf("category entropy: %.16lf\n\n", category_entropy);
    }

    f64 gain = target_entropy - entropy;
    printf("entropy: %.16lf\n", entropy);
    printf("gain   : %.16lf\n", gain);
    if (gain > max_gain) {
      max_gain = gain;
      res = i;
    }
    printf("===================\n\n");
  }

  if (max_gain <= 0) {
    printf("\tgain = 0\n");
    return NULL_INFOGAIN;
  } else if (max_gain <= threshold) {
    printf("\tgain <= %lf\n", threshold);
    return THRESHOLD_NOT_REACHED;
  }

  printf("\tres: %ld\n", res);
  return res;
}

fn DecisionTreeNode ai_makeDTreeNode(Arena *arena, f64 threshold,
				     usize max_domain_size, usize n_features,
				     usize n_rows, usize target_feature_idx,
				     String8 *feature_names, usize *domain_sizes,
				     usize *feature_table) {
  usize best_split = ai_maxInformationGain(threshold, max_domain_size, n_features,
					   n_rows, target_feature_idx,
					   domain_sizes, feature_table);
  printf("\n");

  if (best_split == NULL_INFOGAIN) {

  } else if (best_split == THRESHOLD_NOT_REACHED) {

  }

  usize *new_domain_sizes = Newarr(arena, usize, n_features - 1);
  for (usize i = 0, j = 0; i < n_features - 1; ++i) {
    new_domain_sizes[j++] = domain_sizes[i];
  }

  DecisionTreeNode root = {.label = feature_names[best_split]};

  for (usize split = 0; split < best_split; ++split) {
    usize *sub = Newarr(arena, usize,
			domain_sizes[target_feature_idx] *
			(n_features - 1) * max_domain_size);

    for (usize row = 0; row < domain_sizes[target_feature_idx]; ++row) {
      for (usize col = 0, coln = 0; col < n_features; ++col) {
	for (usize depth = 0; depth < max_domain_size; ++depth) {
	  if (col == best_split && depth != split) {continue;}

	  usize table_index = (row * n_features * max_domain_size) +
			      (col * max_domain_size) + depth;
	  usize split_index = (row * (n_features - 1) * max_domain_size) +
                              (coln * max_domain_size) + depth;

	  sub[split_index] = feature_table[table_index];
          printf("splits[%ld] [%ld][%ld][%ld] = %ld\n",
		 split, row, col, depth, sub[split_index]);
	}

	++coln;
      }
      printf("\n");
    }

    /* Printer */
    DISABLE_LOG();
    for (usize depth = 0; depth < max_domain_size; ++depth) {
      printf("Depth %zu:\ncol:\t", depth);

      for (usize j = 0; j < n_features - 1; ++j) {
	printf("%zu ", j);
      }
      printf("\n");

      for (usize row = 0; row < new_domain_sizes[best_split]; ++row) {
	printf("%zu:\t", row);
	for (usize curr_feature = 0; curr_feature < n_features - 1; ++curr_feature) {
          usize index = row * ((n_features - 1) * max_domain_size) +
			curr_feature * max_domain_size + depth;
          printf("%zu ", sub[index]);
	}
	printf("\n");
      }
      printf("\n");
    }
    ENABLE_LOG();

    usize new_row_count = 0;
    for (usize i = 0; i < new_domain_sizes[best_split]; ++i) {
      new_row_count += feature_table[i * n_features * max_domain_size +
				     best_split * max_domain_size +
				     split];
    }

    /* Number of rows in entropy_feature is incorrect */
    ai_maxInformationGain(threshold, max_domain_size, n_features - 1,
			  new_row_count,
			  best_split, new_domain_sizes,
			  sub);

    printf("================================\n\n");
  }

  return root;
}

fn usize ai_countOccurance(CSV *config, String8 **domains, usize *domain_sizes,
			  usize max_domain_size, usize n_features,
			  usize target_feature_idx, usize *feature) {
  usize row_count = 0;
  Arena *row_arena = arenaBuild(MB(5), 0);
  for (StringStream row = csv_nextRow(row_arena, config); row.first;
       row = csv_nextRow(row_arena, config), ++row_count) {

    /* Transform the CSV row from StringStream to a String8 array to speed up the lookup. */
    usize i = 0;
    String8 *row_entries = Newarr(row_arena, String8, row.size);
    for (StringNode *r = row.first; r && i < n_features; r = r->next, ++i) {
      row_entries[i] = r->value;
    }

    // TODO: i'm assuming that every feature is categorical but this may not be the case.
    usize row = ai_indexFromCategory(domains[target_feature_idx],
                                     domain_sizes[target_feature_idx],
                                     row_entries[target_feature_idx]);
    /* The current row doesn't have a value in the target feature. */
    if (row == USIZE_MAX) {
      continue;
    }

    for (usize curr_feature = 0; curr_feature < n_features; ++curr_feature) {
      // TODO: same thing as above.
      usize depth = ai_indexFromCategory(domains[curr_feature],
					 domain_sizes[curr_feature],
					 row_entries[curr_feature]);
      if (depth == USIZE_MAX) {
        continue;
      }

      feature[row * n_features * max_domain_size +
              curr_feature * max_domain_size + depth] += 1;
    }

    if (row_arena->head - row_arena->base_addr > MB(4)) {
      arenaReset(row_arena);
    }
  }

  arenaFree(row_arena);
  return row_count;
}

fn DecisionTreeNode ai_buildDecisionTree(Arena *arena, CSV config, String8 *header,
					 bool is_first_row_header, usize n_features,
					 usize target_feature_idx, f64 treshold,
					 String8 **domains, usize *domain_sizes) {
  Assert(target_feature_idx <= n_features && target_feature_idx > 0 &&
         n_features > 2);

  /* Read the header line. */
  if (is_first_row_header) {
    header = Newarr(arena, String8, n_features);
    StringStream h = csv_header(arena, &config);
    usize i = 0;
    for (StringNode *col = h.first; col && i < n_features;
         col = col->next, ++i) {
      header[i] = col->value;
    }
  }

  /* Find the largest category */
  usize max_domain_size = 1;
  for (usize i = 0; i < n_features; ++i) {
    max_domain_size = Max(max_domain_size, domain_sizes[i]);
  }

  /* Create a 3D matrix to track the occurances of each category for each feature.
   * The rows will be the occurances of a category of the target feature.
   * The columns will be the features.
   * The depth will be the occurances of a category of a feature. */
  usize *feature = Newarr(arena, usize,
			  domain_sizes[--target_feature_idx]
			  * max_domain_size * n_features);

  usize row_count = ai_countOccurance(&config, domains, domain_sizes,
				      max_domain_size, n_features,
				      target_feature_idx, feature);

  /* Final matrix printer */
  for (usize depth = 0; depth < max_domain_size; ++depth) {
    printf("Depth %zu:\ncol:\t", depth);

    for (usize j = 0; j < n_features; ++j) {
      printf("%zu ", j);
    }
    printf("\n");

    for (usize row = 0; row < domain_sizes[target_feature_idx]; ++row) {
      printf("%zu:\t", row);
      for (usize curr_feature = 0; curr_feature < n_features; ++curr_feature) {
        usize index = row * (n_features * max_domain_size) +
                      curr_feature * max_domain_size + depth;
        printf("%zu ", feature[index]);
      }
      printf("\n");
    }
    printf("\n");
  }

  return ai_makeDTreeNode(arena, treshold, max_domain_size, n_features,
			  row_count, target_feature_idx,
			  header, domain_sizes, feature);
}
