#include "Common.h"

// *** these definitions are only for the test version..
// use a config file and compiler switches to derive these values
#if defined(__SSE4_1__) || defined(__SSE4_2__) || defined(__AVX__) || defined(__AVX2__) || defined(__XOP__)
#	define HAS_SSE4
#	define HAS_SSE3
#elif defined(__SSSE3__)
#	define HAS_SSE3
#elif defined(_MSC_VER) 
#	if defined(_M_AMD64) || defined(_M_X64) || _M_IX86_FP == 2
#		define HAS_SSE2
#	endif
#	if _MSC_VER >= 1500 && _MSC_FULL_VER >= 150030729
#		define HAS_SSE4
#		define HAS_SSE3
#	elif _MSC_VER > 1200 || defined(_mm_free)
#		define HAS_SSE3
#	endif
#endif

#if defined(HAS_SSE4) || defined(HAS_SSE3)
#define HAS_INTRINSICS
#endif

#define IS_LITTLE_ENDIAN (((union { unsigned x; unsigned char c; }){1}).c)

#define CPP_EXCEPTIONS

#define LOADBLAKE3
// *** end temp definitions