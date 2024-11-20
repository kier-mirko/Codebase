#include "decision_tree.h"

fn usize ai_oneHotEncoder(String8 *domain, usize size, String8 needle) {
  for (usize i = 0; i < size; ++i) {
    if (strEq(domain[i], needle)) {
      return i;
    }
  }

  return USIZE_MAX;
}

inline fn f64 ai_computeEntropy(f64 value) {
  return value ? -value * log2(value) : 0;
}

fn f64 ai_entropy(usize max_domain_size, usize n_features, usize n_rows,
                  usize feature_idx, usize feature_size, usize *feature_table) {
  f64 entropy = 0;

  for (usize depth = 0; depth < max_domain_size; ++depth) {
    for (usize row = 0; row < feature_size; ++row) {
      usize index = row * (n_features * max_domain_size) +
                    feature_idx * max_domain_size + depth;
      entropy += ai_computeEntropy((f64)feature_table[index] / n_rows);
    }
  }

  return entropy;
}

fn usize ai_maxInformationGain(usize max_domain_size, usize n_features,
                               usize n_rows, usize target_feature_idx,
                               usize *domain_sizes, usize *feature_table) {
  f64 target_entropy =
      ai_entropy(max_domain_size, n_features, n_rows, target_feature_idx,
                 domain_sizes[target_feature_idx], feature_table);
  printf("\tentropy(D) = %.16lf\n", target_entropy);

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

  printf("\tres: %ld\n", res);
  return res;
}

fn DecisionTreeNode ai_buildDecisionTree(Arena *arena, CSV config, String8 *header,
					 bool is_first_row_header, usize n_features,
					 usize target_feature_idx, String8 **domains,
					 usize *domain_sizes) {
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

  usize row_count = 0;

  /* Create a 3D matrix to track the occurances of each category for each feature.
   * The rows will be the occurances of a category of the target feature.
   * The columns will be the features.
   * The depth will be the occurances of a category of a feature. */
  usize *feature = Newarr(arena, usize,
			  domain_sizes[--target_feature_idx]
			  * max_domain_size * n_features);

  Arena *row_arena = arenaBuild(MB(5), 0);
  for (StringStream row = csv_nextRow(row_arena, &config); row.first;
       row = csv_nextRow(row_arena, &config), ++row_count) {

    /* Transform the CSV row from StringStream to a String8 array to speed up the lookup. */
    String8 *row_entries = Newarr(row_arena, String8, row.size);
    usize i = 0;
    for (StringNode *r = row.first; r && i < n_features; r = r->next, ++i) {
      row_entries[i] = r->value;
    }

    // TODO: i'm assuming that every feature is categorical but this may not be the case.
    usize row = ai_oneHotEncoder(domains[target_feature_idx],
                                 domain_sizes[target_feature_idx],
                                 row_entries[target_feature_idx]);
    /* The current row doesn't have a value in the target feature. */
    if (row == USIZE_MAX) {
      continue;
    }

    for (usize curr_feature = 0; curr_feature < n_features; ++curr_feature) {
      // TODO: same thing as above.
      usize depth = ai_oneHotEncoder(domains[curr_feature],
				     domain_sizes[curr_feature],
				     row_entries[curr_feature]);
      if (depth == USIZE_MAX) {
        continue;
      }

      feature[row * n_features * max_domain_size +
              curr_feature * max_domain_size + depth] += 1;
    }

    if (row_arena->head - row_arena->base_addr > MB(4)) {
      arenaReset(arena);
    }
  }

  usize max_info_gain = ai_maxInformationGain(max_domain_size, n_features,
					      row_count, target_feature_idx,
					      domain_sizes, feature);
  printf("\tBest feature to split by is `%.*s`\n\n",
         Strexpand(header[max_info_gain]));

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

  return (DecisionTreeNode) {0};
}
