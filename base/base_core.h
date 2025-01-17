#ifndef BASE_CORE_H
#define BASE_CORE_H

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#if OS_LINUX || OS_BSD
#include <sys/mman.h>
#elif OS_WINDOWS
#include <windows.h>
#endif

typedef float F32;
typedef double F64;
typedef long double F128;

typedef int8_t I8;
typedef uint8_t U8;
typedef int16_t I16;
typedef uint16_t U16;
typedef int32_t I32;
typedef uint32_t U32;
typedef int64_t I64;
typedef uint64_t U64;

typedef I8 B8;
typedef I32 B32;
typedef I64 B64;

#if defined(ARCH_X64) || defined(ARCH_ARM64)
typedef U64 USZ;
typedef I64 ISZ;
#else
typedef U32 USZ;
typedef I32 ISZ;
#endif

#define DefaultAlignment (2 * sizeof(void*))

#define U8_MAX 0xFF
#define U8_MIN 0
#define U16_MAX 0xFFFF
#define U16_MIN 0
#define U32_MAX 0xFFFFFFFF
#define U32_MIN 0
#define U64_MAX 0xFFFFFFFFFFFFFFFF
#define U64_MIN 0

#define I8_MAX 0x7F
#define I8_MIN (-0x80)
#define I16_MAX 0x7FFF
#define I16_MIN (-0x8000)
#define I32_MAX 0x7FFFFFFF
#define I32_MIN (-0x80000000)
#define I64_MAX 0x7FFFFFFFFFFFFFFF
#define I64_MIN (-0x8000000000000000)

#if defined(ARCH_X64) || defined(ARCH_ARM64)
#define USIZE_MAX U64_MAX
#define USIZE_MIN U64_MIN
#define ISIZE_MAX I64_MAX
#define ISIZE_MIN I64_MIN
#else
#define USIZE_MAX U32_MAX
#define USIZE_MIN U32_MIN
#define ISIZE_MAX I32_MAX
#define ISIZE_MIN I32_MIN
#endif


#if COMPILER_CL
# define thread_static __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
# define thread_static __thread
#endif

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

#define DLLPop(Head, Last) DLLPopBack(Head, Last)
#define DLLPopBack(Head, Last)                                                 \
(!(Last)                                                                     \
? 0                                                                     \
: (!(Last)->prev ? (Head) = (Last) = 0                                  \
: ((Last)->prev->next = 0, (Last) = (Last)->prev)))

#define DLLPopFront(Head, Last)                                                \
(!(Head)                                                                     \
? 0                                                                     \
: (!(Head)->next ? (Head) = (Last) = 0                                  \
: ((Head)->next->prev = 0, (Head) = (Head)->next)))


#define TimedScope							\
for (struct timespec ___start_time = {0}, ___end_time = {0};		\
___start_time.tv_sec == 0 &&					\
clock_gettime(CLOCK_MONOTONIC, &___start_time) == 0;		\
clock_gettime(CLOCK_MONOTONIC, &___end_time),			\
printf("%ldms (%ldns)\n",					\
(U64)(((___end_time.tv_sec - ___start_time.tv_sec) * 1000) + \
((___end_time.tv_nsec - ___start_time.tv_nsec) / 1e6)), \
(U64)((___end_time.tv_sec - ___start_time.tv_sec) * 1e9 + \
(___end_time.tv_nsec - ___start_time.tv_nsec))))	\

#define TimeTrack(expr) _stmt(TimedScope { (void)(expr); })

#endif //BASE_CORE_H
