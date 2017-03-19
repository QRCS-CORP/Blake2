#ifndef __BLAKE2BX_H__
#define __BLAKE2BX_H__

#define HAVE_SSE41
#define HAVE_SSSE3

#ifdef _MSC_VER
#include <intrin.h> /* for _mm_set_epi64x */ 
#endif
#include <emmintrin.h>//
#if defined(HAVE_SSSE3)
#include <tmmintrin.h>
#endif
#if defined(HAVE_SSE41)
#include <smmintrin.h>
#endif
#if defined(HAVE_AVX)
#include <immintrin.h>
#endif
#if defined(HAVE_XOP)
#include <x86intrin.h>
#endif
#include "blake2b-round.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

namespace TestBlake2
{
#pragma pack(push, 1)
	typedef struct __blake2bx_state
	{
		uint64_t h[8];
		uint64_t t[2];
		uint64_t f[2];
		uint8_t  buf[2 * 128];
		size_t   buflen;
		uint8_t  last_node;
	} blake2bx_state;
#pragma pack(pop)

	class Blake2bX
	{
	private:
		static constexpr uint64_t blake2b_IV[] =
		{
			0x6A09E667F3BCC908UL, 0xBB67AE8584CAA73BUL, 0x3C6EF372FE94F82BUL, 0xA54FF53A5F1D36F1UL,
			0x510E527FADE682D1UL, 0x9B05688C2B3E6C1FUL, 0x1F83D9ABFB41BD6BUL, 0x5BE0CD19137E2179UL
		};

		static constexpr int BLAKE2B_BLOCKBYTES = 128;
		static constexpr int BLAKE2B_OUTBYTES = 64;
		static constexpr int BLAKE2B_KEYBYTES = 64;
		static constexpr int BLAKE2B_SALTBYTES = 16;
		static constexpr int BLAKE2B_PERSONALBYTES = 16;

		typedef struct __blake2b_param
		{
			uint8_t  digest_length; // 1
			uint8_t  key_length;    // 2
			uint8_t  fanout;        // 3
			uint8_t  depth;         // 4
			uint32_t leaf_length;   // 8
			uint64_t node_offset;   // 16
			uint8_t  node_depth;    // 17
			uint8_t  inner_length;  // 18
			uint8_t  reserved[14];  // 32
			uint8_t  salt[BLAKE2B_SALTBYTES]; // 48
			uint8_t  personal[BLAKE2B_PERSONALBYTES];  // 64
		} blake2b_param;

		static inline uint32_t load32(const void *src)
		{
			uint32_t w;
			memcpy(&w, src, sizeof w);
			return w;
		}

		static inline uint64_t load64(const void *src)
		{
			uint64_t w;
			memcpy(&w, src, sizeof w);
			return w;
		}

		static inline void store32(void *dst, uint32_t w)
		{
			memcpy(dst, &w, sizeof w);
		}

		static inline void store64(void *dst, uint64_t w)
		{
			memcpy(dst, &w, sizeof w);
		}

		static inline uint64_t load48(const void *src)
		{
			const uint8_t *p = (const uint8_t *)src;
			uint64_t w = *p++;
			w |= (uint64_t)(*p++) << 8;
			w |= (uint64_t)(*p++) << 16;
			w |= (uint64_t)(*p++) << 24;
			w |= (uint64_t)(*p++) << 32;
			w |= (uint64_t)(*p++) << 40;
			return w;
		}

		static inline void store48(void *dst, uint64_t w)
		{
			uint8_t *p = (uint8_t *)dst;
			*p++ = (uint8_t)w; w >>= 8;
			*p++ = (uint8_t)w; w >>= 8;
			*p++ = (uint8_t)w; w >>= 8;
			*p++ = (uint8_t)w; w >>= 8;
			*p++ = (uint8_t)w; w >>= 8;
			*p++ = (uint8_t)w;
		}

		static inline uint32_t rotl32(const uint32_t w, const unsigned c)
		{
			return (w << c) | (w >> (32 - c));
		}

		static inline uint64_t rotl64(const uint64_t w, const unsigned c)
		{
			return (w << c) | (w >> (64 - c));
		}

		static inline uint32_t rotr32(const uint32_t w, const unsigned c)
		{
			return (w >> c) | (w << (32 - c));
		}

		static inline uint64_t rotr64(const uint64_t w, const unsigned c)
		{
			return (w >> c) | (w << (64 - c));
		}

		/* prevents compiler optimizing out memset() */
		static inline void secure_zero_memory(void *v, size_t n)
		{
			static void *(*const volatile memset_v)(void *, int, size_t) = &memset;
			memset_v(v, 0, n);
		}
		/* Some helper functions, not necessarily useful */
		static inline int blake2b_set_lastnode(blake2bx_state *S)
		{
			S->f[1] = -1;
			return 0;
		}

		static inline int blake2b_clear_lastnode(blake2bx_state *S)
		{
			S->f[1] = 0;
			return 0;
		}

		static inline int blake2b_is_lastblock(const blake2bx_state *S)
		{
			return S->f[0] != 0;
		}

		static inline int blake2b_set_lastblock(blake2bx_state *S)
		{
			if (S->last_node) blake2b_set_lastnode(S);

			S->f[0] = -1;
			return 0;
		}

		static inline int blake2b_clear_lastblock(blake2bx_state *S)
		{
			if (S->last_node) blake2b_clear_lastnode(S);

			S->f[0] = 0;
			return 0;
		}


		static inline int blake2b_increment_counter(blake2bx_state *S, const uint64_t inc)
		{
			S->t[0] += inc;
			S->t[1] += (S->t[0] < inc);
			return 0;
		}


		// Parameter-related functions
		static inline int blake2b_param_set_digest_length(blake2b_param *P, const uint8_t digest_length)
		{
			P->digest_length = digest_length;
			return 0;
		}

		static inline int blake2b_param_set_fanout(blake2b_param *P, const uint8_t fanout)
		{
			P->fanout = fanout;
			return 0;
		}

		static inline int blake2b_param_set_max_depth(blake2b_param *P, const uint8_t depth)
		{
			P->depth = depth;
			return 0;
		}

		static inline int blake2b_param_set_leaf_length(blake2b_param *P, const uint32_t leaf_length)
		{
			P->leaf_length = leaf_length;
			return 0;
		}

		static inline int blake2b_param_set_node_offset(blake2b_param *P, const uint64_t node_offset)
		{
			P->node_offset = node_offset;
			return 0;
		}

		static inline int blake2b_param_set_node_depth(blake2b_param *P, const uint8_t node_depth)
		{
			P->node_depth = node_depth;
			return 0;
		}

		static inline int blake2b_param_set_inner_length(blake2b_param *P, const uint8_t inner_length)
		{
			P->inner_length = inner_length;
			return 0;
		}

		static inline int blake2b_param_set_salt(blake2b_param *P, const uint8_t salt[BLAKE2B_SALTBYTES])
		{
			memcpy(P->salt, salt, BLAKE2B_SALTBYTES);
			return 0;
		}

		static inline int blake2b_param_set_personal(blake2b_param *P, const uint8_t personal[BLAKE2B_PERSONALBYTES])
		{
			memcpy(P->personal, personal, BLAKE2B_PERSONALBYTES);
			return 0;
		}

		static inline int blake2b_init0(blake2bx_state *S)
		{
			memset(S, 0, sizeof(blake2bx_state));

			for (int i = 0; i < 8; ++i) S->h[i] = blake2b_IV[i];

			return 0;
		}

		static inline int blake2b_compress(blake2bx_state *S, const uint8_t block[BLAKE2B_BLOCKBYTES])
		{
			__m128i row1l, row1h;
			__m128i row2l, row2h;
			__m128i row3l, row3h;
			__m128i row4l, row4h;
			__m128i b0, b1;
			__m128i t0, t1;
#if defined(HAVE_SSSE3) && !defined(HAVE_XOP)
			const __m128i r16 = _mm_setr_epi8(2, 3, 4, 5, 6, 7, 0, 1, 10, 11, 12, 13, 14, 15, 8, 9);
			const __m128i r24 = _mm_setr_epi8(3, 4, 5, 6, 7, 0, 1, 2, 11, 12, 13, 14, 15, 8, 9, 10);
#endif
#if defined(HAVE_SSE41)
			const __m128i m0 = LOADU(block + 00);
			const __m128i m1 = LOADU(block + 16);
			const __m128i m2 = LOADU(block + 32);
			const __m128i m3 = LOADU(block + 48);
			const __m128i m4 = LOADU(block + 64);
			const __m128i m5 = LOADU(block + 80);
			const __m128i m6 = LOADU(block + 96);
			const __m128i m7 = LOADU(block + 112);
#else
			const uint64_t  m0 = ((uint64_t *)block)[0];
			const uint64_t  m1 = ((uint64_t *)block)[1];
			const uint64_t  m2 = ((uint64_t *)block)[2];
			const uint64_t  m3 = ((uint64_t *)block)[3];
			const uint64_t  m4 = ((uint64_t *)block)[4];
			const uint64_t  m5 = ((uint64_t *)block)[5];
			const uint64_t  m6 = ((uint64_t *)block)[6];
			const uint64_t  m7 = ((uint64_t *)block)[7];
			const uint64_t  m8 = ((uint64_t *)block)[8];
			const uint64_t  m9 = ((uint64_t *)block)[9];
			const uint64_t m10 = ((uint64_t *)block)[10];
			const uint64_t m11 = ((uint64_t *)block)[11];
			const uint64_t m12 = ((uint64_t *)block)[12];
			const uint64_t m13 = ((uint64_t *)block)[13];
			const uint64_t m14 = ((uint64_t *)block)[14];
			const uint64_t m15 = ((uint64_t *)block)[15];
#endif
			row1l = LOADU(&S->h[0]);
			row1h = LOADU(&S->h[2]);
			row2l = LOADU(&S->h[4]);
			row2h = LOADU(&S->h[6]);
			row3l = LOADU(&blake2b_IV[0]);
			row3h = LOADU(&blake2b_IV[2]);
			row4l = _mm_xor_si128(LOADU(&blake2b_IV[4]), LOADU(&S->t[0]));
			row4h = _mm_xor_si128(LOADU(&blake2b_IV[6]), LOADU(&S->f[0]));
			ROUND(0);
			ROUND(1);
			ROUND(2);
			ROUND(3);
			ROUND(4);
			ROUND(5);
			ROUND(6);
			ROUND(7);
			ROUND(8);
			ROUND(9);
			ROUND(10);
			ROUND(11);
			row1l = _mm_xor_si128(row3l, row1l);
			row1h = _mm_xor_si128(row3h, row1h);
			STOREU(&S->h[0], _mm_xor_si128(LOADU(&S->h[0]), row1l));
			STOREU(&S->h[2], _mm_xor_si128(LOADU(&S->h[2]), row1h));
			row2l = _mm_xor_si128(row4l, row2l);
			row2h = _mm_xor_si128(row4h, row2h);
			STOREU(&S->h[4], _mm_xor_si128(LOADU(&S->h[4]), row2l));
			STOREU(&S->h[6], _mm_xor_si128(LOADU(&S->h[6]), row2h));
			return 0;
		}

		public:

		Blake2bX() {}
		int blake2b_initX(blake2bx_state *S, const uint8_t outlen);
		int blake2b_init_keyX(blake2bx_state *S, const uint8_t outlen, const void *key, const uint8_t keylen);
		int blake2b_init_paramX(blake2bx_state *S, const blake2b_param *P);
		int blake2b_updateX(blake2bx_state *S, const uint8_t *in, uint64_t inlen);
		int blake2b_finalX(blake2bx_state *S, uint8_t *out, uint8_t outlen);
	};
}
#endif