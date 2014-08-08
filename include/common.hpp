/*/////////////////////////////////////////////////////////////////////////////
/// @summary Abstract away compiler differences, define some useful macros, and
/// include some definitions required everywhere.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GW_COMMON_HPP
#define GW_COMMON_HPP

/*///////////////
//   Defines   //
///////////////*/
#define BACKEND_PLATFORM_UNKNOWN           0
#define BACKEND_PLATFORM_IOS               1
#define BACKEND_PLATFORM_ANDROID           2
#define BACKEND_PLATFORM_WIN32             3
#define BACKEND_PLATFORM_WINRT             4
#define BACKEND_PLATFORM_WP8               5
#define BACKEND_PLATFORM_OSX               6
#define BACKEND_PLATFORM_LINUX             7

#define BACKEND_COMPILER_UNKNOWN           0
#define BACKEND_COMPILER_GNUC              1
#define BACKEND_COMPILER_MSVC              2

/// @summary The current target platform.
#define BACKEND_TARGET_PLATFORM            BACKEND_PLATFORM_UNKNOWN

/// @summary The current target compiler. Note that BACKEND_COMPILER_GNUC
/// is also specified when building with clang.
#define BACKEND_TARGET_COMPILER            BACKEND_COMPILER_UNKNOWN

/// @summary Compiler detection - Microsoft Visual C++.
#ifdef _MSC_VER
    #undef  BACKEND_TARGET_COMPILER
    #define BACKEND_TARGET_COMPILER        BACKEND_COMPILER_MSVC
#endif

/// @summary Compiler detection - GNU C/C++, CLANG and Intel C/C++.
#ifdef __GNUC__
    #undef  BACKEND_TARGET_COMPILER
    #define BACKEND_TARGET_COMPILER        BACKEND_COMPILER_GNUC
#endif

/// @summary Platform detection - Apple OSX and iOS.
#ifdef __APPLE__
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #undef  BACKEND_TARGET_PLATFORM
        #define BACKEND_TARGET_PLATFORM    BACKEND_PLATFORM_IOS
        #define BACKEND_CALL_C
    #else
        #undef  BACKEND_TARGET_PLATFORM
        #define BACKEND_TARGET_PLATFORM    BACKEND_PLATFORM_OSX
        #define BACKEND_CALL_C
    #endif
#endif

/// @summary Platform detection - Android.
#ifdef ANDROID
    #undef  BACKEND_TARGET_PLATFORM
    #define BACKEND_TARGET_PLATFORM        BACKEND_PLATFORM_ANDROID
    #define BACKEND_CALL_C
#endif

/// @summary Platform detection - Linux.
#ifdef __linux__
    #undef  BACKEND_TARGET_PLATFORM
    #define BACKEND_TARGET_PLATFORM        BACKEND_PLATFORM_LINUX
    #define BACKEND_CALL_C
#endif

/// @summary Platform detection - Windows desktop, surface and phone.
#if defined(WIN32) || defined(WIN64) || defined(WINDOWS)
    #if defined(WINRT)
        #undef  BACKEND_TARGET_PLATFORM
        #define BACKEND_TARGET_PLATFORM    BACKEND_PLATFORM_WINRT
        #define BACKEND_CALL_C             __cdecl
    #elif defined(WP8)
        #undef  BACKEND_TARGET_PLATFORM
        #define BACKEND_TARGET_PLATFORM    BACKEND_PLATFORM_WP8
        #define BACKEND_CALL_C             __cdecl
    #else
        #define BACKEND_TARGET_PLATFORM    BACKEND_PLATFORM_WIN32
        #define BACKEND_CALL_C             __cdecl
    #endif
#endif

/// @summary Force a compiler error if we can't detect the target compiler.
#if BACKEND_TARGET_COMPILER == BACKEND_COMPILER_UNKNOWN
    #error Unable to detect the target compiler.
#endif

/// @summary Force a compiler error if we can't detect the target platform.
#if BACKEND_TARGET_PLATFORM == BACKEND_PLATFORM_UNKNOWN
    #error Unable to detect the target platform.
#endif

/// @summary Force a compiler error if Visual C++ 2012 or newer is not available.
#if BACKEND_TARGET_COMPILER == BACKEND_COMPILER_MSVC
    #if _MSC_VER < 1700
        #error Visual C++ 2012 or later is required.
    #endif
#endif

/// @summary Indicate a C-callable API function.
#ifndef BACKEND_C_API
#define BACKEND_C_API(return_type)         extern return_type BACKEND_CALL_C
#endif

/// @summary Indicate the start of a multi-line macro.
#ifndef BACKEND_MLMACRO_BEGIN
#define BACKEND_MLMACRO_BEGIN              do {
#endif

/// @summary Indicate the end of a multi-line macro.
#ifndef BACKEND_MLMACRO_END
    #ifdef _MSC_VER
        #define BACKEND_MLMACRO_END                                           \
            __pragma(warning(push))                                           \
            __pragma(warning(disable:4127))                                   \
            } while (0)                                                       \
            __pragma(warning(pop))
    #else
        #define BACKEND_MLMACRO_END                                           \
            } while (0)
    #endif /* defined(_MSC_VER) */
#endif /* !defined(BACKEND_MLMACRO_END) */

/// @summary Internal macro used to suppress warnings about unused variables.
#ifndef BACKEND_UNUSED_X
    #ifdef _MSC_VER
        #define BACKEND_UNUSED_X(x)        (x)
    #else
        #define BACKEND_UNUSED_X(x)        (void)sizeof((x))
    #endif
#endif /* !defined(BACKEND_UNUSED_X) */

/// @summary Used to suppress warnings about unused arguments.
#ifndef UNUSED_ARG
#define UNUSED_ARG(x)                                                         \
    BACKEND_MLMACRO_BEGIN                                                     \
    BACKEND_UNUSED_X(x);                                                      \
    BACKEND_MLMACRO_END
#endif /* !defined(UNUSED_ARG) */

/// @summary Used to suppress warnings about unused local variables.
#ifndef UNUSED_LOCAL
#define UNUSED_LOCAL(x)                                                       \
    BACKEND_MLMACRO_BEGIN                                                     \
    BACKEND_UNUSED_X(x);                                                      \
    BACKEND_MLMACRO_END
#endif /* !defined(UNUSED_LOCAL) */

/// @summary Wrap compiler differences used to control structure alignment.
#ifndef BACKEND_ALIGNMENT_DEFINED
    #ifdef _MSC_VER
        #define BACKEND_ALIGN_BEGIN(_al)  __declspec(align(_al))
        #define BACKEND_ALIGN_END(_al)
        #define BACKEND_ALIGN_OF(_type)   __alignof(_type)
        #define BACKEND_ALIGNMENT_DEFINED
    #endif /* defined(_MSC_VER) */
    #ifdef __GNUC__
        #define BACKEND_ALIGN_BEGIN(_al)
        #define BACKEND_ALIGN_END(_al)    __attribute__((aligned(_al)))
        #define BACKEND_ALIGN_OF(_type)   __alignof__(_type)
        #define BACKEND_ALIGNMENT_DEFINED
    #endif /* defined(__GNUC__) */
#endif /* !defined(BACKEND_ALIGNMENT_DEFINED) */

/// @summary Wrap compiler differences used to force inlining of a function.
#ifndef BACKEND_FORCE_INLINE
    #ifdef _MSC_VER
        #define BACKEND_FORCE_INLINE       __forceinline
        #define force_inline               __forceinline
    #endif /* defined(_MSC_VER) */
    #ifdef __GNUC__
        #define BACKEND_FORCE_INLINE       __attribute__((always_inline))
        #define force_inline               __attribute__((always_inline))
    #endif /* defined(__GNUC__) */
#endif /* !defined(BACKEND_FORCE_INLINE) */

/// @summary Wrap compiler differences used to specify lack of aliasing.
#ifndef BACKEND_RESTRICT
    #ifdef _MSC_VER
        #define BACKEND_RESTRICT           __restrict
        #define restrict                   __restrict
    #endif /* defined(_MSC_VER) */
    #ifdef __GNUC__
        #define BACKEND_RESTRICT           __restrict
        #define restrict                   __restrict
    #endif /* defined(__GNUC__) */
#endif /*!defined(BACKEND_RESTRICT) */

/*////////////////
//   Includes   //
////////////////*/
#include <stddef.h>
#include <stdint.h>

#endif /* !defined(GW_COMMON_HPP) */
