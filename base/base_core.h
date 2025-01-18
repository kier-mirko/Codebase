#ifndef BASE_CORE_H
#define BASE_CORE_H

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

#if OS_LINUX || OS_BSD
#include <sys/mman.h>
#elif OS_WINDOWS
#include <windows.h>
#endif

#define DefaultAlignment (2 * sizeof(void*))

#if COMPILER_GCC
#define alignof(T) __alignof__(T)
#elif COMPILER_CLANG
#define alignof(T) _Alignof(T)
#elif COMPILER_CL
#define alignof(T) __alignof(T)
#else
#error alignof is implemented for this compiler
#endif

#define _stmt(S)  do{ S }while(0)

#ifndef _assert_break
#if OS_WINDOWS
#define _assert_break() __debugbreak()
#else
#define _assert_break() __builtin_trap()
#endif
#endif

#define AssertAlways(c) _stmt(if (!(c)) { _assert_break(); })
#ifdef ENABLE_ASSERT
#define Assert(c) AssertAlways(c)
#else
#define Assert(c)
#endif

#if COMPILER_GCC || COMPILER_CLANG
#define DLLExport export_c __attribute__((visibility("default")))
#elif COMPILER_CL
#define DLLExport export_c __declspec(dllexport)
#endif

#if COMPILER_CL
# define thread_static __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
# define thread_static __thread
#endif


//-km: helper macro

#define Stringify_(S) (#S)
#define Stringify(S) Stringify_(S)

#define Glue_(A, B) (A##B)
#define Glue(A, B) Glue_(A,B)

#define Arrsize(ARR) (sizeof((ARR)) / sizeof(*(ARR)))
#define Max(a, b) ((a) >= (b) ? (a) : (b))
#define Min(a, b) ((a) <= (b) ? (a) : (b))
#define ClampTop(a, b) Min((a), (b))
#define ClampBot(a, b) Max((a), (b))
#define Swap(a, b, _tmp) ((_tmp) = (a), (a) = (b), (b) = (_tmp))
#define Abs(a) ((a) >= 0 ? (a) : (-(a)))

#define DeferLoop(...) for(U8 __i_ = 1; __i_; --__i_, __VA_ARGS__)

#define Not(A) (~(A))
#define And(A, B) ((A) & (B))
#define Or(A, B) ((A) | (B))
#define Nand(A, B) (~((A) && (B)))
#define Nor(A, B) (~((A) || (B)))
#define Xor(A, B) ((A) ^ (B))
#define Xnor(A, B) (~(A) ^ (B))
#define GetBit(NUM, I) ((NUM & (1 << I)) >> I)
#define SetBit(NUM, I, BIT) (NUM | (1 << (BIT - 1)))

#define KiB(BYTES) ((BYTES)*1024)
#define MB(BYTES) (KiB((BYTES)) * 1024)
#define GB(BYTES) (MB((BYTES)) * 1024UL)
#define TB(BYTES) (GB((BYTES)) * 1024UL)

#define global static
#define local static
#define fn static

#define CExport extern "C"
#define CExportBegin extern "C" {
#define CExportEnd }

//-km: constant and basic types

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


typedef float F32;
typedef double F64;
typedef long double F128;

typedef signed char         I8;
typedef short              I16;
typedef int                I32;
typedef signed long        I64;

typedef unsigned char       U8;
typedef unsigned short     U16;
typedef unsigned int       U32;
typedef unsigned long long U64;

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
: ((Last) ? ((Last)->Next = (Nodeptr), (Last) = (Nodeptr))                  \
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
