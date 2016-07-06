#ifndef _BLAKE2TEST_CONFIG_H
#define _BLAKE2TEST_CONFIG_H

#include "Common.h"

#if defined(__SSE2__)
#	define HAS_SSE2
#endif
#if defined(__SSSE3__)
#	define HAS_SSSE3
#endif
#if defined(__SSE4_1__)
#	define HAS_SSE41
#endif
#if defined(__SSE4_2__)
#	define HAS_SSE42
#endif
#if defined(__AVX__) || defined(_M_AMD64) && _M_AMD64 >= 100
#	define HAS_AVX
#endif
#if defined(__XOP__)
#	define HAS_XOP
#endif

#if defined(HAS_AVX2)
#if !defined(HAS_AVX)
#		define HAS_AVX
#	endif
#endif
#if defined(HAS_XOP)
#if !defined(HAS_AVX)
#		define HAS_AVX
#	endif
#endif
#if defined(HAS_AVX)
#if !defined(HAS_SSE41)
#		define HAS_SSE41
#	endif
#endif
#if defined(HAS_SSE41)
#if !defined(HAS_SSSE3)
#		define HAS_SSSE3
#	endif
#endif
#if defined(HAS_SSSE3)
#	define HAS_SSE2
#endif

#if defined(HAS_SSE41) || defined(HAS_SSE42)
#	define HAS_SSE4
#endif

#if defined(_MSC_VER) && !defined(HAS_SSE4) && !defined(HAS_SSSE3) && !defined(HAS_SSE2) && !defined(HAVE_XOP) && !defined(HAVE_AVX2) && !defined(HAVE_AVX)
#	if defined(_M_AMD64) || defined(_M_X64) || _M_IX86_FP == 2
#		define HAS_SSSE3
#		define HAS_SSE2
#	elif _MSC_VER >= 1500 && _MSC_FULL_VER >= 150030729
#		define HAS_SSSE3
#		if !defined(HAS_SSE2)
#			define HAS_SSE2
#		endif
#	elif _MSC_VER > 1200 || defined(_mm_free)
#		define HAS_SSE3
#		if !defined(HAS_SSE2)
#			define HAS_SSE2
#		endif
#	endif
#endif

#if defined(HAS_SSE42) || defined(HAS_SSE41) || defined(HAS_SSSE3) || defined(HAS_SSE3)
#	define HAS_ADVINTRIN
#endif

#define IS_LITTLE_ENDIAN (((union { unsigned x; unsigned char c; }){1}).c)

#define CPP_EXCEPTIONS

#endif