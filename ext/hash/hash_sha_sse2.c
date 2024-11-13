/*-
 * Copyright 2021 Tarsnap Backup Inc.
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

#ifdef __SSE2__
# include <emmintrin.h>

/* Original implementation from libcperciva follows.
 *
 * Modified to use `PHP_STATIC_RESTRICT` for MSVC compatibility.
 */

/**
 * mm_bswap_epi32(a):
 * Byte-swap each 32-bit word.
 */
static inline __m128i
mm_bswap_epi32(__m128i a)
{

	/* Swap bytes in each 16-bit word. */
	a = _mm_or_si128(_mm_slli_epi16(a, 8), _mm_srli_epi16(a, 8));

	/* Swap all 16-bit words. */
	a = _mm_shufflelo_epi16(a, _MM_SHUFFLE(2, 3, 0, 1));
	a = _mm_shufflehi_epi16(a, _MM_SHUFFLE(2, 3, 0, 1));

	return (a);
}

/* SHA256 round constants. */
static const uint32_t Krnd[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/* Elementary functions used by SHA256 */
#define Ch(x, y, z)	((x & (y ^ z)) ^ z)
#define Maj(x, y, z)	((x & (y | z)) | (y & z))
#define ROTR(x, n)	((x >> n) | (x << (32 - n)))
#define S0(x)		(ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S1(x)		(ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))

/* SHA256 round function */
#define RND(a, b, c, d, e, f, g, h, k)			\
	h += S1(e) + Ch(e, f, g) + k;			\
	d += h;						\
	h += S0(a) + Maj(a, b, c)

/* Adjusted round function for rotating state */
#define RNDr(S, W, i, ii)			\
	RND(S[(64 - i) % 8], S[(65 - i) % 8],	\
	    S[(66 - i) % 8], S[(67 - i) % 8],	\
	    S[(68 - i) % 8], S[(69 - i) % 8],	\
	    S[(70 - i) % 8], S[(71 - i) % 8],	\
	    W[i + ii] + Krnd[i + ii])

/* Message schedule computation */
#define SHR32(x, n) (_mm_srli_epi32(x, n))
#define ROTR32(x, n) (_mm_or_si128(SHR32(x, n), _mm_slli_epi32(x, (32-n))))
#define s0_128(x) _mm_xor_si128(_mm_xor_si128(			\
	ROTR32(x, 7), ROTR32(x, 18)), SHR32(x, 3))

static inline __m128i
s1_128_high(__m128i a)
{
	__m128i b;
	__m128i c;

	/* ROTR, loading data as {B, B, A, A}; lanes 1 & 3 will be junk. */
	b = _mm_shuffle_epi32(a, _MM_SHUFFLE(1, 1, 0, 0));
	c = _mm_xor_si128(_mm_srli_epi64(b, 17), _mm_srli_epi64(b, 19));

	/* Shift and XOR with rotated data; lanes 1 & 3 will be junk. */
	c = _mm_xor_si128(c, _mm_srli_epi32(b, 10));

	/* Shuffle good data back and zero unwanted lanes. */
	c = _mm_shuffle_epi32(c, _MM_SHUFFLE(2, 0, 2, 0));
	c = _mm_slli_si128(c, 8);

	return (c);
}

static inline __m128i
s1_128_low(__m128i a)
{
	__m128i b;
	__m128i c;

	/* ROTR, loading data as {B, B, A, A}; lanes 1 & 3 will be junk. */
	b = _mm_shuffle_epi32(a, _MM_SHUFFLE(3, 3, 2, 2));
	c = _mm_xor_si128(_mm_srli_epi64(b, 17), _mm_srli_epi64(b, 19));

	/* Shift and XOR with rotated data; lanes 1 & 3 will be junk. */
	c = _mm_xor_si128(c, _mm_srli_epi32(b, 10));

	/* Shuffle good data back and zero unwanted lanes. */
	c = _mm_shuffle_epi32(c, _MM_SHUFFLE(2, 0, 2, 0));
	c = _mm_srli_si128(c, 8);

	return (c);
}

/**
 * SPAN_ONE_THREE(a, b):
 * Combine the upper three words of ${a} with the lowest word of ${b}.  This
 * could also be thought of returning bits [159:32] of the 256-bit value
 * consisting of (b[127:0] a[127:0]).  In other words, set:
 *     dst[31:0] := a[63:32]
 *     dst[63:32] := a[95:64]
 *     dst[95:64] := a[127:96]
 *     dst[127:96] := b[31:0]
 */
#define SPAN_ONE_THREE(a, b) (_mm_shuffle_epi32(_mm_castps_si128(	\
	_mm_move_ss(_mm_castsi128_ps(a), _mm_castsi128_ps(b))),		\
	_MM_SHUFFLE(0, 3, 2, 1)))

/**
 * MSG4(X0, X1, X2, X3):
 * Calculate the next four values of the message schedule.  If we define
 * ${W[j]} as the first unknown value in the message schedule, then the input
 * arguments are:
 *     X0 = W[j - 16] : W[j - 13]
 *     X1 = W[j - 12] : W[j - 9]
 *     X2 = W[j - 8] : W[j - 5]
 *     X3 = W[j - 4] : W[j - 1]
 * This function therefore calculates:
 *     X4 = W[j + 0] : W[j + 3]
 */
static inline __m128i
MSG4(__m128i X0, __m128i X1, __m128i X2, __m128i X3)
{
	__m128i X4;
	__m128i Xj_minus_seven, Xj_minus_fifteen;

	/* Set up variables which span X values. */
	Xj_minus_seven = SPAN_ONE_THREE(X2, X3);
	Xj_minus_fifteen = SPAN_ONE_THREE(X0, X1);

	/* Begin computing X4. */
	X4 = _mm_add_epi32(X0, Xj_minus_seven);
	X4 = _mm_add_epi32(X4, s0_128(Xj_minus_fifteen));

	/* First half of s1. */
	X4 = _mm_add_epi32(X4, s1_128_low(X3));

	/* Second half of s1; this depends on the above value of X4. */
	X4 = _mm_add_epi32(X4, s1_128_high(X4));

	return (X4);
}

/**
 * SHA256_Transform_sse2(state, block, W, S):
 * Compute the SHA256 block compression function, transforming ${state} using
 * the data in ${block}.  This implementation uses x86 SSE2 instructions, and
 * should only be used if _SSE2 is defined and cpusupport_x86_sse2() returns
 * nonzero.  The arrays W and S may be filled with sensitive data, and should
 * be cleared by the callee.
 */
void
SHA256_Transform_sse2(uint32_t state[PHP_STATIC_RESTRICT 8],
    const uint8_t block[PHP_STATIC_RESTRICT 64], uint32_t W[PHP_STATIC_RESTRICT 64],
    uint32_t S[PHP_STATIC_RESTRICT 8])
{
	__m128i Y[4];
	int i;

	/* 1. Prepare the first part of the message schedule W. */
	Y[0] = mm_bswap_epi32(_mm_loadu_si128((const __m128i *)&block[0]));
	_mm_storeu_si128((__m128i *)&W[0], Y[0]);
	Y[1] = mm_bswap_epi32(_mm_loadu_si128((const __m128i *)&block[16]));
	_mm_storeu_si128((__m128i *)&W[4], Y[1]);
	Y[2] = mm_bswap_epi32(_mm_loadu_si128((const __m128i *)&block[32]));
	_mm_storeu_si128((__m128i *)&W[8], Y[2]);
	Y[3] = mm_bswap_epi32(_mm_loadu_si128((const __m128i *)&block[48]));
	_mm_storeu_si128((__m128i *)&W[12], Y[3]);

	/* 2. Initialize working variables. */
	memcpy(S, state, 32);

	/* 3. Mix. */
	for (i = 0; i < 64; i += 16) {
		RNDr(S, W, 0, i);
		RNDr(S, W, 1, i);
		RNDr(S, W, 2, i);
		RNDr(S, W, 3, i);
		RNDr(S, W, 4, i);
		RNDr(S, W, 5, i);
		RNDr(S, W, 6, i);
		RNDr(S, W, 7, i);
		RNDr(S, W, 8, i);
		RNDr(S, W, 9, i);
		RNDr(S, W, 10, i);
		RNDr(S, W, 11, i);
		RNDr(S, W, 12, i);
		RNDr(S, W, 13, i);
		RNDr(S, W, 14, i);
		RNDr(S, W, 15, i);

		if (i == 48)
			break;
		Y[0] = MSG4(Y[0], Y[1], Y[2], Y[3]);
		_mm_storeu_si128((__m128i *)&W[16 + i + 0], Y[0]);
		Y[1] = MSG4(Y[1], Y[2], Y[3], Y[0]);
		_mm_storeu_si128((__m128i *)&W[16 + i + 4], Y[1]);
		Y[2] = MSG4(Y[2], Y[3], Y[0], Y[1]);
		_mm_storeu_si128((__m128i *)&W[16 + i + 8], Y[2]);
		Y[3] = MSG4(Y[3], Y[0], Y[1], Y[2]);
		_mm_storeu_si128((__m128i *)&W[16 + i + 12], Y[3]);
	}

	/* 4. Mix local working variables into global state. */
	for (i = 0; i < 8; i++)
		state[i] += S[i];
}

#endif
