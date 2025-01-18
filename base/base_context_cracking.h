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
.
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

#endif
