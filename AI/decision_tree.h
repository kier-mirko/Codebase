#ifndef BASE_AI_DECISION_TREE
#define BASE_AI_DECISION_TREE

#include "../base.h"
#include "../string.h"

#include <math.h>

typedef struct DecisionTreeNode {
  /* The predicted target feature label. */
  String8 label;
  u64 should_split_by;

  struct DecisionTreeNode *first;
  struct DecisionTreeNode *last;

  struct DecisionTreeNode *next;
  struct DecisionTreeNode *prev;

  void print() {
    if (first) {
      printf("`%.*s` childrens: [ ", Strexpand(label));
      for (DecisionTreeNode *child = first; child; child = child->next) {
	printf("`%.*s`, ", Strexpand(child->label));
      }

      printf("\b\b]\n");
      for (DecisionTreeNode *child = first; child; child = child->next) {
	child->print();
      }
    }
  }
} DecisionTreeNode;

#endif
