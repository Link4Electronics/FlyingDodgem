#ifndef FLYINGDODGEM_TYPES_H
#define FLYINGDODGEM_TYPES_H

/* Fixed-width integer types that work on both the MSVC 5.0 (matching) toolchain
 * and modern 64-bit compilers (SDL3 native build). MSVC 5.0 predates C99
 * <stdint.h>, so we spell out the __intN/MSVC types explicitly. */

#ifdef _MSC_VER
typedef __int8    int8_t;
typedef __int16   int16_t;
typedef __int32   int32_t;
typedef __int64   int64_t;
typedef unsigned __int8    uint8_t;
typedef unsigned __int16   uint16_t;
typedef unsigned __int32   uint32_t;
typedef unsigned __int64   uint64_t;
#else
#include <stdint.h>
#endif

#ifdef _MSC_VER
typedef __int32 intptr_t;
typedef unsigned __int32 uintptr_t;
typedef unsigned __int32 size_t;
#else
#include <stddef.h>
#endif

#ifndef NULL
#define NULL 0
#endif

typedef int8_t   s8;
typedef uint8_t  u8;
typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;

typedef int32_t  bool32;
typedef uint32_t bool8;

/* MSVC 5.0 predates C99, so it has no <stdbool.h>. Provide bool/true/false. */
#ifdef _MSC_VER
#ifndef __cplusplus
#ifndef bool
#define bool int
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#endif
#endif

/* Calling conventions. MSVC 5.0 provides these as built-in keywords; other
 * compilers (e.g. the GCC/Clang SDL3 native build) do not, so define them as
 * empty there. */
#ifndef _MSC_VER
#ifndef __cdecl
#define __cdecl
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef __fastcall
#define __fastcall
#endif
#endif

#endif
