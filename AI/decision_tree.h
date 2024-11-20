#ifndef BASE_AI_DECISION_TREE
#define BASE_AI_DECISION_TREE

#include "../base.h"
#include "../string.h"

typedef struct {
  /* The label is:
   * - a boolean condition when the current node isn't a leaf
   * - the predicted target feature label */
  String8 label;

  struct DecisionTreeNode *true_branch;
  struct DecisionTreeNode *false_branch;
} DecisionTreeNode;

fn usize ai_oneHotEncoder(String8 *domain, usize size, String8 needle);

inline fn f64 ai_computeEntropy(f64 value);
       fn f64 ai_entropy(usize max_domain_size, usize n_features, usize n_rows,
			 usize feature_idx, usize feature_size, usize *feature_table);
       fn usize ai_maxInformationGain(usize max_domain_size, usize n_features,
				      usize n_rows, usize target_feature_idx,
				      usize *domain_sizes, usize *feature_table);

/* If `is_first_row_header` is T, the value of `header` is discarded
 * and will be read from the CSV file. */
fn DecisionTreeNode ai_buildDecisionTree(Arena *arena, CSV config, String8 *header,
					 bool is_first_row_header, usize n_features,
					 usize target_feature_idx, String8 **domains,
					 usize *domain_sizes);

#endif
