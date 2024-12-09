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

  struct DecisionTreeNode *first;
  struct DecisionTreeNode *last;

  struct DecisionTreeNode *next;
  struct DecisionTreeNode *prev;
} DecisionTreeNode;

#endif
