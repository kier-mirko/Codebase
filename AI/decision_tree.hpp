#ifndef AI_DECISION_TREE
#define AI_DECISION_TREE

#include <math.h>

struct DecisionTreeNode {
  /* The predicted target feature label. */
  String8 label;
  u64 should_split_by;

  DecisionTreeNode *first;
  DecisionTreeNode *last;

  DecisionTreeNode *next;
  DecisionTreeNode *prev;

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
};

struct Occurrence {
  String8 name;
  usize count;

  HashMap<String8, Occurrence> targets;

  Occurrence(Arena *arena) : count(0), targets(arena, strHash) {}
};

inline fn f32 ai_entropy(f32 val);
       fn f32 ai_computeEntropy(HashMap<String8, Occurrence> *map, u32 row_count);
       fn u32 ai_maxInformationGain(HashMap<String8, Occurrence> *maps,
				    u32 n_features, u32 target_idx,
				    u32 row_count, f32 threshold);

fn DecisionTreeNode *ai_makeDTNode(Arena *arena, Arena *map_arena, CSV config,
                                   StringStream header,
                                   HashMap<String8, Occurrence> *maps,
                                   u32 n_features, u32 target_idx,
                                   f32 threshold);
fn DecisionTreeNode *ai_buildDecisionTree(Arena *arena, Arena *map_arena,
                                          CSV config, StringStream header,
                                          u32 n_features,
                                          u32 target_feature, f32 threshold);

#endif
