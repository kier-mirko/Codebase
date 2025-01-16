#ifndef BASE_CONTEXT_CRACKING_H
#define BASE_CONTEXT_CRACKING_H

#if defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#define COMPILER_GCC 1
#elif defined(__clang__) && !defined(_MSC_VER)
#define COMPILER_CLANG 1
#elif defined(_MSC_VER)
#define COMPILER_CL 1
#if defined(_M_IX86)
#define ARCH_X86 1
#elif defined(_M_AMD64)
#define ARCH_X64 1
#elif defined(_M_ARM)
#define ARCH_ARM 1
#elif defined(_M_ARM64)
#define ARCH_ARM64 1
#else
#error "Unsupported platform"
#endif
#else
#error "Unsupported compiler"
#endif

#if defined(__gnu_linux__)
#define OS_LINUX 1
#elif defined(__unix__)
#define OS_BSD 1
#elif defined(_WIN32)
#define OS_WINDOWS 1
#elif defined(__APPLE__)
#define OS_MAC 1
#else
#error "Unsupported OS"
#endif

#if defined(__cplusplus)
#define CPP 1
#else
#define CPP 0
#endif

# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_AMD64)
#  define ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define ARCH_X86 1
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# elif defined(__arm__)
#  define ARCH_ARM32 1
# else
#  error "Unsopported platform"
# endif

#if !defined(COMPILER_GCC)
#define COMPILER_GCC 0
#endif
#if !defined(COMPILER_CLANG)
#define COMPILER_CLANG 0
#endif
#if !defined(COMPILER_CL)
#define COMPILER_CL 0
#endif

#if !defined(OS_LINUX)
#define OS_LINUX 0
#endif
#if !defined(OS_BSD)
#define OS_BSD 0
#endif
#if !defined(OS_MAC)
#define OS_MAC 0
#endif
#if !defined(OS_WINDOWS)
#define OS_WINDOWS 0
#endif

#if !defined(ARCH_X86)
#define ARCH_X86 0
#endif
#if !defined(ARCH_X64)
#define ARCH_X64 0
#endif
#if !defined(ARCH_ARM)
#define ARCH_ARM 0
#endif
#if !defined(ARCH_ARM64)
#define ARCH_ARM64 0
#endif

#if defined(DEBUG)
#undef DEBUG
#define DEBUG 1
#define NDEBUG 0
#else
#define DEBUG 0
#define NDEBUG 1
#endif

#if COMPILER_GCC
#define alignof(TYPE) __alignof__(TYPE)
#elif COMPILER_CLANG
#define alignof(TYPE) _Alignof(TYPE)
#elif COMPILER_CL
#define alignof(TYPE) __alignof(TYPE)
#else
#define alignof(TYPE) 1
#endif

#define _stmt(S)                                                               \
do {                                                                         \
S                                                                          \
} while (0)

#ifndef _assert_break
#if OS_WINDOWS
#define _assert_break() __debugbreak()
#else
#define _assert_break() __builtin_trap()
#endif
#endif

#define AssertAlways(COND) _stmt(if (!(COND)) { _assert_break(); })
#ifdef ENABLE_ASSERT
#define Assert(COND) AssertAlways(COND)
#else
#define Assert(COND) (void)(COND)
#endif

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

#if COMPILER_GCC || COMPILER_CLANG
#define DLLExport export_c __attribute__((visibility("default")))
#elif COMPILER_CL
#define DLLExport export_c __declspec(dllexport)
#endif

#endif
