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

#endif
