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
