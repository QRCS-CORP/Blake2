#include "Common.h"

// *** these definitions are only for the test version..
// use a config file and compiler switches to derive these values
#if defined(_MSC_VER) 
#	if _MSC_VER >= 1500 && _MSC_FULL_VER >= 150030729
#		define HAS_SSE4
#		define HAS_SSE3
#	elif _MSC_VER > 1200 || defined(_mm_free)
#		define HAS_SSE3
#	endif
#elif defined(__SSE4_1__) || defined(__SSE4_2__) || defined(__AVX__) || defined(__XOP__)
#	define HAS_SSE4
#	define HAS_SSE3
#elif defined(__SSSE3__)
#	define HAS_SSE3
#endif

#if defined(HAS_SSE4) || defined(HAS_SSE3)
#define HAS_INTRINSICS
#endif

#define IS_LITTLE_ENDIAN (((union { unsigned x; unsigned char c; }){1}).c)
// *** end temp definitions