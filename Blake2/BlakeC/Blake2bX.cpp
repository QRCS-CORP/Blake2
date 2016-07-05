#include "Blake2bX.h"


namespace BlakeTest
{
	/* init xors IV with input parameter block */
	int Blake2bX::blake2b_init_paramX(blake2bx_state *S, const blake2b_param *P)
	{
		//blake2b_init0( S );
		const uint8_t * v = (const uint8_t *)(blake2b_IV);
		const uint8_t * p = (const uint8_t *)(P);
		uint8_t * h = (uint8_t *)(S->h);
		/* IV XOR ParamBlock */
		memset(S, 0, sizeof(blake2bx_state));

		for (int i = 0; i < BLAKE2B_OUTBYTES; ++i)
			h[i] = v[i] ^ p[i];

		return 0;
	}

	/* Some sort of default parameter block initialization, for sequential blake2b */
	int Blake2bX::blake2b_initX(blake2bx_state *S, const uint8_t outlen)
	{
		if ((!outlen) || (outlen > BLAKE2B_OUTBYTES)) return -1;

		const blake2b_param P =
		{
			outlen,
			0,
			1,
			1,
			0,
			0,
			0,
			0,
			{ 0 },
			{ 0 },
			{ 0 }
		};
		return blake2b_init_paramX(S, &P);
	}

	int Blake2bX::blake2b_init_keyX(blake2bx_state *S, const uint8_t outlen, const void *key, const uint8_t keylen)
	{
		if ((!outlen) || (outlen > BLAKE2B_OUTBYTES)) return -1;

		if ((!keylen) || keylen > BLAKE2B_KEYBYTES) return -1;

		const blake2b_param P =
		{
			outlen,
			keylen,
			1,
			1,
			0,
			0,
			0,
			0,
			{ 0 },
			{ 0 },
			{ 0 }
		};

		if (blake2b_init_paramX(S, &P) < 0)
			return 0;

		{
			uint8_t block[BLAKE2B_BLOCKBYTES];
			memset(block, 0, BLAKE2B_BLOCKBYTES);
			memcpy(block, key, keylen);
			blake2b_updateX(S, block, BLAKE2B_BLOCKBYTES);
			secure_zero_memory(block, BLAKE2B_BLOCKBYTES); /* Burn the key from stack */
		}
		return 0;
	}

	int Blake2bX::blake2b_updateX(blake2bx_state *S, const uint8_t *in, uint64_t inlen)
	{
		while (inlen > 0)
		{
			size_t left = S->buflen;
			size_t fill = 2 * BLAKE2B_BLOCKBYTES - left;

			if (inlen > fill)
			{
				memcpy(S->buf + left, in, fill); // Fill buffer
				S->buflen += fill;
				blake2b_increment_counter(S, BLAKE2B_BLOCKBYTES);
				blake2b_compress(S, S->buf); // Compress
				memcpy(S->buf, S->buf + BLAKE2B_BLOCKBYTES, BLAKE2B_BLOCKBYTES); // Shift buffer left
				S->buflen -= BLAKE2B_BLOCKBYTES;
				in += fill;
				inlen -= fill;
			}
			else // inlen <= fill
			{
				memcpy(S->buf + left, in, inlen);
				S->buflen += inlen; // Be lazy, do not compress
				in += inlen;
				inlen -= inlen;
			}
		}

		return 0;
	}

	int Blake2bX::blake2b_finalX(blake2bx_state *S, uint8_t *out, uint8_t outlen)
	{
		if (outlen > BLAKE2B_OUTBYTES)
			return -1;

		if (blake2b_is_lastblock(S))
			return -1;

		if (S->buflen > BLAKE2B_BLOCKBYTES)
		{
			blake2b_increment_counter(S, BLAKE2B_BLOCKBYTES);
			blake2b_compress(S, S->buf);
			S->buflen -= BLAKE2B_BLOCKBYTES;
			memcpy(S->buf, S->buf + BLAKE2B_BLOCKBYTES, S->buflen);
		}

		blake2b_increment_counter(S, S->buflen);
		blake2b_set_lastblock(S);
		memset(S->buf + S->buflen, 0, 2 * BLAKE2B_BLOCKBYTES - S->buflen); /* Padding */
		blake2b_compress(S, S->buf);
		memcpy(out, &S->h[0], outlen);
		return 0;
	}
}