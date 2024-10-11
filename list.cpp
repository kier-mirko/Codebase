#ifndef BASE_LIST
#define BASE_LIST

// =============================================================================
// Singly Linked List
#define StackPush(Head, Nodeptr)                                               \
  LLPushFrontCustom((Head), (Head), (Nodeptr), next)

#define StackPop(Head) (Head ? (Head = Head->next) : 0)

#define LLPushFrontCustom(Head, Last, Nodeptr, Next)                           \
  (!(Head) ? (Head) = (Last) = (Nodeptr)                                       \
           : ((Nodeptr)->Next = (Head), (Head) = (Nodeptr)))

#define QueuePush(Head, Last, Nodeptr)                                         \
  LLPushBackCustom((Head), (Last), (Nodeptr), next)

#define QueuePop(Head) (Head ? (Head = Head->next) : 0)

#define LLPushBackCustom(Head, Last, Nodeptr, Next)                            \
  (!(Head) ? (Head) = (Last) = (Nodeptr)                                       \
           : ((Last) ? ((Last)->Next = (Nodeptr), (Last) = (Nodeptr))          \
                     : ((Head)->Next = (Last) = (Nodeptr))))

// =============================================================================
// Doubly Linked List
#define DLLPushFront(Head, Last, Nodeptr)                                      \
  DLLPushFrontCustom(Head, Last, Nodeptr, next, prev)

#define DLLPushBack(Head, Last, Nodeptr)                                       \
  DLLPushBackCustom(Head, Last, Nodeptr, next, prev)

#define DLLPushFrontCustom(Head, Last, Nodeptr, Next, Prev)                    \
  (!(Head) ? (Head) = (Last) = (Nodeptr)                                       \
           : ((Nodeptr)->Next = (Head), (Head)->Prev = (Nodeptr),              \
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

// =============================================================================
// Heaps
#define MaxHeapPush(Head, OrderBy, Nodeptr)                                    \
  _stmt(                                                                       \
      if (!(Head)) {                                                           \
        (Head) = (Nodeptr);                                                    \
      } else if ((Nodeptr)->OrderBy > (Head)->OrderBy) {                       \
        (Nodeptr)->next = (Head);                                              \
        (Head)->prev = (Nodeptr);                                              \
        (Head) = (Nodeptr);                                                    \
      } else {                                                                 \
        for (auto curr = (Head); curr; curr = curr->next) {                    \
          if (curr->OrderBy < (Nodeptr)->OrderBy) {                            \
            (Nodeptr)->next = curr;                                            \
            (Nodeptr)->prev = curr->prev;                                      \
            if (curr->prev) {                                                  \
              curr->prev->next = (Nodeptr);                                    \
            }                                                                  \
            curr->prev = (Nodeptr);                                            \
            break;                                                             \
          }                                                                    \
          if (!curr->next) {                                                   \
            curr->next = (Nodeptr);                                            \
            (Nodeptr)->prev = curr;                                            \
            break;                                                             \
          }                                                                    \
        }                                                                      \
      })

#define MinHeapPush(Head, OrderBy, Nodeptr)                                    \
  _stmt(                                                                       \
      if (!(Head)) {                                                           \
        (Head) = (Nodeptr);                                                    \
      } else if ((Nodeptr)->OrderBy < (Head)->OrderBy) {                       \
        (Nodeptr)->next = (Head);                                              \
        (Head)->prev = (Nodeptr);                                              \
        (Head) = (Nodeptr);                                                    \
      } else {                                                                 \
        for (auto curr = (Head); curr; curr = curr->next) {                    \
          if (curr->OrderBy > (Nodeptr)->OrderBy) {                            \
            (Nodeptr)->next = curr;                                            \
            (Nodeptr)->prev = curr->prev;                                      \
            if (curr->prev) {                                                  \
              curr->prev->next = (Nodeptr);                                    \
            }                                                                  \
            curr->prev = (Nodeptr);                                            \
            break;                                                             \
          }                                                                    \
          if (!curr->next) {                                                   \
            curr->next = (Nodeptr);                                            \
            (Nodeptr)->prev = curr;                                            \
            break;                                                             \
          }                                                                    \
        }                                                                      \
      })

#endif
