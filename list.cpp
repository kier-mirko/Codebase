#ifndef BASE_LIST
#define BASE_LIST

// =============================================================================
// Singly Linked List
#define StackPush(Head, Nodeptr)                                               \
  LLPushFrontCustom((Head), (Head), (Nodeptr), next)

#define LLPushFrontCustom(Head, Last, Nodeptr, Next)                           \
  (!(Head) ? (Head) = (Last) = (Nodeptr)                                       \
           : ((Nodeptr)->(Next) = (Head), (Head) = (Nodeptr)))

#define QueuePush(Head, Last, Nodeptr)                                         \
  LLPushBackCustom((Head), (Last), (Nodeptr), next)

#define LLPushBackCustom(Head, Last, Nodeptr, Next)                            \
  (!(Head) ? (Head) = (Last) = (Nodeptr)                                       \
           : ((Last)->(Next) = (Nodeptr), (Last) = (Nodeptr)))

// =============================================================================
// Doubly Linked List
#define DLLPushFront(Head, Last, Nodeptr)                                      \
  DLLPushFrontCustom(Head, Last, Nodeptr, next, prev)

#define DLLPushBack(Head, Last, Nodeptr)                                       \
  DLLPushBackCustom(Head, Last, Nodeptr, next, prev)

#define DLLPushFrontCustom(Head, Last, Nodeptr, Next, Prev)                    \
  (!(Head) ? (Head) = (Last) = (Nodeptr)                                       \
           : ((Nodeptr)->(Next) = (Head), (Head)->Prev = (Nodeptr),            \
              (Head) = (Nodeptr)))

#define DLLPushBackCustom(DLLNodeHead, DLLNodeLast, NodeToInsertptr, Next,     \
                          Prev)                                                \
  (!DLLNodeHead                                                                \
       ? DLLNodeHead = DLLNodeLast = NodeToInsertptr                           \
       : (DLLNodeLast->Next = NodeToInsertptr,                                 \
          NodeToInsertptr->Prev = DLLNodeLast, DLLNodeLast = NodeToInsertptr))

#define DLLDelete(Head, Last, Nodeptr)                                         \
  (!(Head)->next && (Head) == (Nodeptr)                                        \
       ? (Head) = (Last) = 0                                                   \
       : ((Last) == (Nodeptr)                                                  \
              ? ((Last) = (Last)->prev, (Last)->next = 0)                      \
              : ((Head) == (Nodeptr)                                           \
                     ? ((Head) = (Head)->next, (Head)->prev = 0)               \
                     : ((Nodeptr)->prev->next = (Nodeptr)->next,               \
                        (Nodeptr)->next->prev = (Nodeptr)->prev))))

#endif
