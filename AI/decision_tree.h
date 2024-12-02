#ifndef BASE_AI_DECISION_TREE
#define BASE_AI_DECISION_TREE

#include "../base.h"
#include "../string.h"

#include <math.h>

typedef struct DecisionTreeNode {
  /* The label is:
   * - a boolean condition when the current node isn't a leaf
   * - the predicted target feature label */
  String8 label;

  struct DecisionTreeNode *branches;
} DecisionTreeNode;

struct FeatureData {
  f64 entropy_threshold;
  usize max_domain_size, n_features, n_rows, target_feature;

  String8 **categories;

  struct {
    String8 *names;
    usize *sizes;
  } domain;
};

fn usize ai_indexFromCategory(String8 *domain, usize size, String8 needle);

inline fn f64 ai_computeEntropy(f64 value);
       fn f64 ai_entropy(struct FeatureData *data, usize *occurrences);
       fn usize ai_maxInformationGain(struct FeatureData *data, usize *feature_table);

fn DecisionTreeNode ai_buildDecisionTree(Arena *arena, CSV config, String8 *header,
					 usize n_features, usize target_feature_idx,
					 f64 treshold);

#endif
