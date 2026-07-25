#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define VKOM_PLATFORM_FAMILY_NT
#define VKOM_PLATFORM_OS_WINDOWS
#elif defined(__APPLE__)
#define VKOM_PLATFORM_FAMILY_APPLE
#include <TargetConditionals.h>

#if TARGET_OS_IPHONE == 1
#define VKOM_PLATFORM_FAMILY_UNIX
#define VKOM_PLATFORM_FAMILY_DARWIN
#define VKOM_PLATFORM_OS_IOS
#elif TARGET_OS_MAC == 1
#define VKOM_PLATFORM_FAMILY_UNIX
#define VKOM_PLATFORM_FAMILY_DARWIN
#define VKOM_PLATFORM_OS_MACOS
#endif
#elif defined(__linux__)
#define VKOM_PLATFORM_FAMILY_UNIX
#define VKOM_PLATFORM_OS_LINUX
#elif defined(__FreeBSD__)
#define VKOM_PLATFORM_FAMILY_UNIX
#define VKOM_PLATFORM_FAMILY_BSD
#define VKOM_PLATFORM_OS_FREEBSD
#endif

#ifdef _MSC_VER
#define VKOM_COMPILER_MSVC
#elif defined(__MINGW32__)
#define VKOM_COMPILER_MINGW
#elif defined(__clang__)
#define VKOM_COMPILER_CLANG
#elif defined(__GNUC__)
#define VKOM_COMPILER_GCC
#else
#define VKOM_COMPILER_UNKNOWN
#endif

#ifdef VKOM_COMPILER_MSVC
#define VKOM_FORCE_EXPORT __declspec(dllexport)
#elif VKOM_COMPILER_MINGW
#define VKOM_FORCE_EXPORT __attribute__((dllexport))
#elif VKOM_COMPILER_CLANG
#define VKOM_FORCE_EXPORT __attribute__((__visibility__("default")))
#elif VKOM_COMPILER_GCC
#define VKOM_FORCE_EXPORT __attribute__((__visibility__("default")))
#endif

#ifdef VKOM_COMPILING_DYNAMIC_LIBRARY
#define VKOM_VISIBLE VKOM_FORCE_EXPORT
#else
#define VKOM_VISIBLE 
#endif