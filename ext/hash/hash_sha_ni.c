/*-
 * Copyright 2018 Tarsnap Backup Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "php_hash.h"
#include "php_hash_sha.h"

#if defined(PHP_HASH_INTRIN_SHA_NATIVE) || defined(PHP_HASH_INTRIN_SHA_RESOLVER)

# include <immintrin.h>

# if defined(PHP_HASH_INTRIN_SHA_RESOLVER) && defined(HAVE_FUNC_ATTRIBUTE_TARGET)
static __m128i be32dec_128(const uint8_t * src)  __attribute__((target("ssse3")));
void SHA256_Transform_shani(uint32_t state[PHP_STATIC_RESTRICT 8], const uint8_t block[PHP_STATIC_RESTRICT 64])  __attribute__((target("ssse3,sha")));
# endif

/* Original implementation from libcperciva follows.
 *
 * Modified to use `PHP_STATIC_RESTRICT` for MSVC compatibility.
 */

/**
 * This code uses intrinsics from the following feature sets:
 * SHANI: _mm_sha256msg1_epu32, _mm_sha256msg2_epu32, _mm_sha256rnds2_epu32
 * SSSE3: _mm_shuffle_epi8, _mm_alignr_epi8
 * SSE2: Everything else
 *
 * The SSSE3 intrinsics could be avoided at a slight cost by using a few SSE2
 * instructions in their place; we have not done this since to our knowledge
 * there are presently no CPUs which support the SHANI instruction set but do
 * not support SSSE3.
 */

/* Load 32-bit big-endian words. */
static __m128i
be32dec_128(const uint8_t * src)
{
	const __m128i SHUF = _mm_set_epi8(12, 13, 14, 15, 8, 9, 10, 11,
	    4, 5, 6, 7, 0, 1, 2, 3);
	__m128i x;

	/* Load four 32-bit words. */
	x = _mm_loadu_si128((const __m128i *)src);

	/* Reverse the order of the bytes in each word. */
	return (_mm_shuffle_epi8(x, SHUF));
}

/* Convert an unsigned 32-bit immediate into a signed value. */
#define I32(a) ((UINT32_C(a) >= UINT32_C(0x80000000)) ?			\
    -(int32_t)(UINT32_C(0xffffffff) - UINT32_C(a)) - 1 : (int32_t)INT32_C(a))

/* Load four unsigned 32-bit immediates into a vector register. */
#define IMM4(a, b, c, d) _mm_set_epi32(I32(a), I32(b), I32(c), I32(d))

/* Run four rounds of SHA256. */
#define RND4(S, W, K0, K1, K2, K3) do {						\
	__m128i M;								\
										\
	/* Add the next four words of message schedule and round constants. */	\
	M = _mm_add_epi32(W, IMM4(K3, K2, K1, K0));				\
										\
	/* Perform two rounds of SHA256, using the low two words in M. */	\
	S[1] = _mm_sha256rnds2_epu32(S[1], S[0], M);				\
										\
	/* Shift the two words of M down and perform the next two rounds. */	\
	M = _mm_srli_si128(M, 8);						\
	S[0] = _mm_sha256rnds2_epu32(S[0], S[1], M);				\
} while (0)

/* Compute the ith set of four words of message schedule. */
#define MSG4(W, i) do {								\
	W[(i + 0) % 4] = _mm_sha256msg1_epu32(W[(i + 0) % 4], W[(i + 1) % 4]);	\
	W[(i + 0) % 4] = _mm_add_epi32(W[(i + 0) % 4],				\
	    _mm_alignr_epi8(W[(i + 3) % 4], W[(i + 2) % 4], 4));		\
	W[(i + 0) % 4] = _mm_sha256msg2_epu32(W[(i + 0) % 4], W[(i + 3) % 4]);	\
} while (0)

/* Perform 4 rounds of SHA256 and generate more message schedule if needed. */
#define RNDMSG(S, W, i, K0, K1, K2, K3) do {			\
	RND4(S, W[i % 4], K0, K1, K2, K3);			\
	if (i < 12)						\
		MSG4(W, i + 4);					\
} while (0)

/**
 * SHA256_Transform_shani(state, block):
 * Compute the SHA256 block compression function, transforming ${state} using
 * the data in ${block}.  This implementation uses x86 SHANI and SSSE3
 * instructions, and should only be used if CPUSUPPORT_X86_SHANI and _SSSE3
 * are defined and cpusupport_x86_shani() and _ssse3() return nonzero.
 */
void
SHA256_Transform_shani(uint32_t state[PHP_STATIC_RESTRICT 8],
    const uint8_t block[PHP_STATIC_RESTRICT 64])
{
	__m128i S3210, S7654;
	__m128i S0123, S4567;
	__m128i S0145, S2367;
	__m128i W[4];
	__m128i S[2];

	/* Load state. */
	S3210 = _mm_loadu_si128((const __m128i *)&state[0]);
	S7654 = _mm_loadu_si128((const __m128i *)&state[4]);

	/* Shuffle the 8 32-bit values into the order we need them. */
	S0123 = _mm_shuffle_epi32(S3210, 0x1B);
	S4567 = _mm_shuffle_epi32(S7654, 0x1B);
	S0145 = _mm_unpackhi_epi64(S4567, S0123);
	S2367 = _mm_unpacklo_epi64(S4567, S0123);

	/* Load input block; this is the start of the message schedule. */
	W[0] = be32dec_128(&block[0]);
	W[1] = be32dec_128(&block[16]);
	W[2] = be32dec_128(&block[32]);
	W[3] = be32dec_128(&block[48]);

	/* Initialize working variables. */
	S[0] = S0145;
	S[1] = S2367;

	/* Perform 64 rounds, 4 at a time. */
	RNDMSG(S, W, 0, 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5);
	RNDMSG(S, W, 1, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5);
	RNDMSG(S, W, 2, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3);
	RNDMSG(S, W, 3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174);
	RNDMSG(S, W, 4, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc);
	RNDMSG(S, W, 5, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da);
	RNDMSG(S, W, 6, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7);
	RNDMSG(S, W, 7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967);
	RNDMSG(S, W, 8, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13);
	RNDMSG(S, W, 9, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85);
	RNDMSG(S, W, 10, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3);
	RNDMSG(S, W, 11, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070);
	RNDMSG(S, W, 12, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5);
	RNDMSG(S, W, 13, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3);
	RNDMSG(S, W, 14, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208);
	RNDMSG(S, W, 15, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2);

	/* Mix local working variables into global state. */
	S0145 = _mm_add_epi32(S0145, S[0]);
	S2367 = _mm_add_epi32(S2367, S[1]);

	/* Shuffle state back to the original word order and store. */
	S0123 = _mm_unpackhi_epi64(S2367, S0145);
	S4567 = _mm_unpacklo_epi64(S2367, S0145);
	S3210 = _mm_shuffle_epi32(S0123, 0x1B);
	S7654 = _mm_shuffle_epi32(S4567, 0x1B);
	_mm_storeu_si128((__m128i *)&state[0], S3210);
	_mm_storeu_si128((__m128i *)&state[4], S7654);
}

#endif
