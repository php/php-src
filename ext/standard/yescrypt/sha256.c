/*-
 * Copyright 2005-2016 Colin Percival
 * Copyright 2016-2018 Alexander Peslyak
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

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "insecure_memzero.h"
#include "sysendian.h"

#include "sha256.h"

#ifdef __ICC
/* Miscompile with icc 14.0.0 (at least), so don't use restrict there */
#define restrict
#elif __STDC_VERSION__ >= 199901L
/* Have restrict */
#elif defined(__GNUC__)
#define restrict __restrict
#else
#define restrict
#endif

/* PHP change: Windows compatibility */
#ifdef _MSC_VER
# define PHP_STATIC_RESTRICT
#else
# define PHP_STATIC_RESTRICT static restrict
#endif

/*
 * Encode a length len*2 vector of (uint32_t) into a length len*8 vector of
 * (uint8_t) in big-endian form.
 */
static void
be32enc_vect(uint8_t * dst, const uint32_t * src, size_t len)
{

	/* Encode vector, two words at a time. */
	do {
		be32enc(&dst[0], src[0]);
		be32enc(&dst[4], src[1]);
		src += 2;
		dst += 8;
	} while (--len);
}

/*
 * Decode a big-endian length len*8 vector of (uint8_t) into a length
 * len*2 vector of (uint32_t).
 */
static void
be32dec_vect(uint32_t * dst, const uint8_t * src, size_t len)
{

	/* Decode vector, two words at a time. */
	do {
		dst[0] = be32dec(&src[0]);
		dst[1] = be32dec(&src[4]);
		src += 8;
		dst += 2;
	} while (--len);
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
#define SHR(x, n)	(x >> n)
#define ROTR(x, n)	((x >> n) | (x << (32 - n)))
#define S0(x)		(ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S1(x)		(ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define s0(x)		(ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define s1(x)		(ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

/* SHA256 round function */
#define RND(a, b, c, d, e, f, g, h, k)			\
	h += S1(e) + Ch(e, f, g) + k;			\
	d += h;						\
	h += S0(a) + Maj(a, b, c);

/* Adjusted round function for rotating state */
#define RNDr(S, W, i, ii)			\
	RND(S[(64 - i) % 8], S[(65 - i) % 8],	\
	    S[(66 - i) % 8], S[(67 - i) % 8],	\
	    S[(68 - i) % 8], S[(69 - i) % 8],	\
	    S[(70 - i) % 8], S[(71 - i) % 8],	\
	    W[i + ii] + Krnd[i + ii])

/* Message schedule computation */
#define MSCH(W, ii, i)				\
	W[i + ii + 16] = s1(W[i + ii + 14]) + W[i + ii + 9] + s0(W[i + ii + 1]) + W[i + ii]

/*
 * SHA256 block compression function.  The 256-bit state is transformed via
 * the 512-bit input block to produce a new state.
 */
static void
SHA256_Transform(uint32_t state[PHP_STATIC_RESTRICT 8],
    const uint8_t block[PHP_STATIC_RESTRICT 64],
    uint32_t W[PHP_STATIC_RESTRICT 64], uint32_t S[PHP_STATIC_RESTRICT 8])
{
	int i;

	/* 1. Prepare the first part of the message schedule W. */
	be32dec_vect(W, block, 8);

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
		MSCH(W, 0, i);
		MSCH(W, 1, i);
		MSCH(W, 2, i);
		MSCH(W, 3, i);
		MSCH(W, 4, i);
		MSCH(W, 5, i);
		MSCH(W, 6, i);
		MSCH(W, 7, i);
		MSCH(W, 8, i);
		MSCH(W, 9, i);
		MSCH(W, 10, i);
		MSCH(W, 11, i);
		MSCH(W, 12, i);
		MSCH(W, 13, i);
		MSCH(W, 14, i);
		MSCH(W, 15, i);
	}

	/* 4. Mix local working variables into global state. */
	state[0] += S[0];
	state[1] += S[1];
	state[2] += S[2];
	state[3] += S[3];
	state[4] += S[4];
	state[5] += S[5];
	state[6] += S[6];
	state[7] += S[7];
}

static const uint8_t PAD[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Add padding and terminating bit-count. */
static void
SHA256_Pad(SHA256_CTX * ctx, uint32_t tmp32[PHP_STATIC_RESTRICT 72])
{
	size_t r;

	/* Figure out how many bytes we have buffered. */
	r = (ctx->count >> 3) & 0x3f;

	/* Pad to 56 mod 64, transforming if we finish a block en route. */
	if (r < 56) {
		/* Pad to 56 mod 64. */
		memcpy(&ctx->buf[r], PAD, 56 - r);
	} else {
		/* Finish the current block and mix. */
		memcpy(&ctx->buf[r], PAD, 64 - r);
		SHA256_Transform(ctx->state, ctx->buf, &tmp32[0], &tmp32[64]);

		/* The start of the final block is all zeroes. */
		memset(&ctx->buf[0], 0, 56);
	}

	/* Add the terminating bit-count. */
	be64enc(&ctx->buf[56], ctx->count);

	/* Mix in the final block. */
	SHA256_Transform(ctx->state, ctx->buf, &tmp32[0], &tmp32[64]);
}

/* Magic initialization constants. */
static const uint32_t initial_state[8] = {
	0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
	0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
};

/**
 * SHA256_Init(ctx):
 * Initialize the SHA256 context ${ctx}.
 */
void
SHA256_Init(SHA256_CTX * ctx)
{

	/* Zero bits processed so far. */
	ctx->count = 0;

	/* Initialize state. */
	memcpy(ctx->state, initial_state, sizeof(initial_state));
}

/**
 * SHA256_Update(ctx, in, len):
 * Input ${len} bytes from ${in} into the SHA256 context ${ctx}.
 */
static void
_SHA256_Update(SHA256_CTX * ctx, const void * in, size_t len,
    uint32_t tmp32[PHP_STATIC_RESTRICT 72])
{
	uint32_t r;
	const uint8_t * src = in;

	/* Return immediately if we have nothing to do. */
	if (len == 0)
		return;

	/* Number of bytes left in the buffer from previous updates. */
	r = (ctx->count >> 3) & 0x3f;

	/* Update number of bits. */
	ctx->count += (uint64_t)(len) << 3;

	/* Handle the case where we don't need to perform any transforms. */
	if (len < 64 - r) {
		memcpy(&ctx->buf[r], src, len);
		return;
	}

	/* Finish the current block. */
	memcpy(&ctx->buf[r], src, 64 - r);
	SHA256_Transform(ctx->state, ctx->buf, &tmp32[0], &tmp32[64]);
	src += 64 - r;
	len -= 64 - r;

	/* Perform complete blocks. */
	while (len >= 64) {
		SHA256_Transform(ctx->state, src, &tmp32[0], &tmp32[64]);
		src += 64;
		len -= 64;
	}

	/* Copy left over data into buffer. */
	memcpy(ctx->buf, src, len);
}

/* Wrapper function for intermediate-values sanitization. */
void
SHA256_Update(SHA256_CTX * ctx, const void * in, size_t len)
{
	uint32_t tmp32[72];

	/* Call the real function. */
	_SHA256_Update(ctx, in, len, tmp32);

	/* Clean the stack. */
	insecure_memzero(tmp32, 288);
}

/**
 * SHA256_Final(digest, ctx):
 * Output the SHA256 hash of the data input to the context ${ctx} into the
 * buffer ${digest}.
 */
static void
_SHA256_Final(uint8_t digest[32], SHA256_CTX * ctx,
    uint32_t tmp32[PHP_STATIC_RESTRICT 72])
{

	/* Add padding. */
	SHA256_Pad(ctx, tmp32);

	/* Write the hash. */
	be32enc_vect(digest, ctx->state, 4);
}

/* Wrapper function for intermediate-values sanitization. */
void
SHA256_Final(uint8_t digest[32], SHA256_CTX * ctx)
{
	uint32_t tmp32[72];

	/* Call the real function. */
	_SHA256_Final(digest, ctx, tmp32);

	/* Clear the context state. */
	insecure_memzero(ctx, sizeof(SHA256_CTX));

	/* Clean the stack. */
	insecure_memzero(tmp32, 288);
}

/**
 * SHA256_Buf(in, len, digest):
 * Compute the SHA256 hash of ${len} bytes from ${in} and write it to ${digest}.
 */
void
SHA256_Buf(const void * in, size_t len, uint8_t digest[32])
{
	SHA256_CTX ctx;
	uint32_t tmp32[72];

	SHA256_Init(&ctx);
	_SHA256_Update(&ctx, in, len, tmp32);
	_SHA256_Final(digest, &ctx, tmp32);

	/* Clean the stack. */
	insecure_memzero(&ctx, sizeof(SHA256_CTX));
	insecure_memzero(tmp32, 288);
}

/**
 * HMAC_SHA256_Init(ctx, K, Klen):
 * Initialize the HMAC-SHA256 context ${ctx} with ${Klen} bytes of key from
 * ${K}.
 */
static void
_HMAC_SHA256_Init(HMAC_SHA256_CTX * ctx, const void * _K, size_t Klen,
    uint32_t tmp32[PHP_STATIC_RESTRICT 72], uint8_t pad[PHP_STATIC_RESTRICT 64],
    uint8_t khash[PHP_STATIC_RESTRICT 32])
{
	const uint8_t * K = _K;
	size_t i;

	/* If Klen > 64, the key is really SHA256(K). */
	if (Klen > 64) {
		SHA256_Init(&ctx->ictx);
		_SHA256_Update(&ctx->ictx, K, Klen, tmp32);
		_SHA256_Final(khash, &ctx->ictx, tmp32);
		K = khash;
		Klen = 32;
	}

	/* Inner SHA256 operation is SHA256(K xor [block of 0x36] || data). */
	SHA256_Init(&ctx->ictx);
	memset(pad, 0x36, 64);
	for (i = 0; i < Klen; i++)
		pad[i] ^= K[i];
	_SHA256_Update(&ctx->ictx, pad, 64, tmp32);

	/* Outer SHA256 operation is SHA256(K xor [block of 0x5c] || hash). */
	SHA256_Init(&ctx->octx);
	memset(pad, 0x5c, 64);
	for (i = 0; i < Klen; i++)
		pad[i] ^= K[i];
	_SHA256_Update(&ctx->octx, pad, 64, tmp32);
}

/* Wrapper function for intermediate-values sanitization. */
void
HMAC_SHA256_Init(HMAC_SHA256_CTX * ctx, const void * _K, size_t Klen)
{
	uint32_t tmp32[72];
	uint8_t pad[64];
	uint8_t khash[32];

	/* Call the real function. */
	_HMAC_SHA256_Init(ctx, _K, Klen, tmp32, pad, khash);

	/* Clean the stack. */
	insecure_memzero(tmp32, 288);
	insecure_memzero(khash, 32);
	insecure_memzero(pad, 64);
}

/**
 * HMAC_SHA256_Update(ctx, in, len):
 * Input ${len} bytes from ${in} into the HMAC-SHA256 context ${ctx}.
 */
static void
_HMAC_SHA256_Update(HMAC_SHA256_CTX * ctx, const void * in, size_t len,
    uint32_t tmp32[PHP_STATIC_RESTRICT 72])
{

	/* Feed data to the inner SHA256 operation. */
	_SHA256_Update(&ctx->ictx, in, len, tmp32);
}

/* Wrapper function for intermediate-values sanitization. */
void
HMAC_SHA256_Update(HMAC_SHA256_CTX * ctx, const void * in, size_t len)
{
	uint32_t tmp32[72];

	/* Call the real function. */
	_HMAC_SHA256_Update(ctx, in, len, tmp32);

	/* Clean the stack. */
	insecure_memzero(tmp32, 288);
}

/**
 * HMAC_SHA256_Final(digest, ctx):
 * Output the HMAC-SHA256 of the data input to the context ${ctx} into the
 * buffer ${digest}.
 */
static void
_HMAC_SHA256_Final(uint8_t digest[32], HMAC_SHA256_CTX * ctx,
    uint32_t tmp32[PHP_STATIC_RESTRICT 72], uint8_t ihash[PHP_STATIC_RESTRICT 32])
{

	/* Finish the inner SHA256 operation. */
	_SHA256_Final(ihash, &ctx->ictx, tmp32);

	/* Feed the inner hash to the outer SHA256 operation. */
	_SHA256_Update(&ctx->octx, ihash, 32, tmp32);

	/* Finish the outer SHA256 operation. */
	_SHA256_Final(digest, &ctx->octx, tmp32);
}

/* Wrapper function for intermediate-values sanitization. */
void
HMAC_SHA256_Final(uint8_t digest[32], HMAC_SHA256_CTX * ctx)
{
	uint32_t tmp32[72];
	uint8_t ihash[32];

	/* Call the real function. */
	_HMAC_SHA256_Final(digest, ctx, tmp32, ihash);

	/* Clean the stack. */
	insecure_memzero(tmp32, 288);
	insecure_memzero(ihash, 32);
}

/**
 * HMAC_SHA256_Buf(K, Klen, in, len, digest):
 * Compute the HMAC-SHA256 of ${len} bytes from ${in} using the key ${K} of
 * length ${Klen}, and write the result to ${digest}.
 */
void
HMAC_SHA256_Buf(const void * K, size_t Klen, const void * in, size_t len,
    uint8_t digest[32])
{
	HMAC_SHA256_CTX ctx;
	uint32_t tmp32[72];
	uint8_t tmp8[96];

	_HMAC_SHA256_Init(&ctx, K, Klen, tmp32, &tmp8[0], &tmp8[64]);
	_HMAC_SHA256_Update(&ctx, in, len, tmp32);
	_HMAC_SHA256_Final(digest, &ctx, tmp32, &tmp8[0]);

	/* Clean the stack. */
	insecure_memzero(&ctx, sizeof(HMAC_SHA256_CTX));
	insecure_memzero(tmp32, 288);
	insecure_memzero(tmp8, 96);
}

/* Add padding and terminating bit-count, but don't invoke Transform yet. */
static int
SHA256_Pad_Almost(SHA256_CTX * ctx, uint8_t len[PHP_STATIC_RESTRICT 8],
    uint32_t tmp32[PHP_STATIC_RESTRICT 72])
{
	uint32_t r;

	r = (ctx->count >> 3) & 0x3f;
	if (r >= 56)
		return -1;

	/*
	 * Convert length to a vector of bytes -- we do this now rather
	 * than later because the length will change after we pad.
	 */
	be64enc(len, ctx->count);

	/* Add 1--56 bytes so that the resulting length is 56 mod 64. */
	_SHA256_Update(ctx, PAD, 56 - r, tmp32);

	/* Add the terminating bit-count. */
	ctx->buf[63] = len[7];
	_SHA256_Update(ctx, len, 7, tmp32);

	return 0;
}

/**
 * PBKDF2_SHA256(passwd, passwdlen, salt, saltlen, c, buf, dkLen):
 * Compute PBKDF2(passwd, salt, c, dkLen) using HMAC-SHA256 as the PRF, and
 * write the output to buf.  The value dkLen must be at most 32 * (2^32 - 1).
 */
void
PBKDF2_SHA256(const uint8_t * passwd, size_t passwdlen, const uint8_t * salt,
    size_t saltlen, uint64_t c, uint8_t * buf, size_t dkLen)
{
	HMAC_SHA256_CTX Phctx, PShctx, hctx;
	uint32_t tmp32[72];
	union {
		uint8_t tmp8[96];
		uint32_t state[8];
	} u;
	size_t i;
	uint8_t ivec[4];
	uint8_t U[32];
	uint8_t T[32];
	uint64_t j;
	int k;
	size_t clen;

	/* Sanity-check. */
	assert(dkLen <= 32 * (size_t)(UINT32_MAX));

	if (c == 1 && (dkLen & 31) == 0 && (saltlen & 63) <= 51) {
		uint32_t oldcount;
		uint8_t * ivecp;

		/* Compute HMAC state after processing P and S. */
		_HMAC_SHA256_Init(&hctx, passwd, passwdlen,
		    tmp32, &u.tmp8[0], &u.tmp8[64]);
		_HMAC_SHA256_Update(&hctx, salt, saltlen, tmp32);

		/* Prepare ictx padding. */
		oldcount = hctx.ictx.count & (0x3f << 3);
		_HMAC_SHA256_Update(&hctx, "\0\0\0", 4, tmp32);
		if ((hctx.ictx.count & (0x3f << 3)) < oldcount ||
		    SHA256_Pad_Almost(&hctx.ictx, u.tmp8, tmp32))
			goto generic; /* Can't happen due to saltlen check */
		ivecp = hctx.ictx.buf + (oldcount >> 3);

		/* Prepare octx padding. */
		hctx.octx.count += 32 << 3;
		SHA256_Pad_Almost(&hctx.octx, u.tmp8, tmp32);

		/* Iterate through the blocks. */
		for (i = 0; i * 32 < dkLen; i++) {
			/* Generate INT(i + 1). */
			be32enc(ivecp, (uint32_t)(i + 1));

			/* Compute U_1 = PRF(P, S || INT(i)). */
			memcpy(u.state, hctx.ictx.state, sizeof(u.state));
			SHA256_Transform(u.state, hctx.ictx.buf,
			    &tmp32[0], &tmp32[64]);
			be32enc_vect(hctx.octx.buf, u.state, 4);
			memcpy(u.state, hctx.octx.state, sizeof(u.state));
			SHA256_Transform(u.state, hctx.octx.buf,
			    &tmp32[0], &tmp32[64]);
			be32enc_vect(&buf[i * 32], u.state, 4);
		}

		goto cleanup;
	}

generic:
	/* Compute HMAC state after processing P. */
	_HMAC_SHA256_Init(&Phctx, passwd, passwdlen,
	    tmp32, &u.tmp8[0], &u.tmp8[64]);

	/* Compute HMAC state after processing P and S. */
	memcpy(&PShctx, &Phctx, sizeof(HMAC_SHA256_CTX));
	_HMAC_SHA256_Update(&PShctx, salt, saltlen, tmp32);

	/* Iterate through the blocks. */
	for (i = 0; i * 32 < dkLen; i++) {
		/* Generate INT(i + 1). */
		be32enc(ivec, (uint32_t)(i + 1));

		/* Compute U_1 = PRF(P, S || INT(i)). */
		memcpy(&hctx, &PShctx, sizeof(HMAC_SHA256_CTX));
		_HMAC_SHA256_Update(&hctx, ivec, 4, tmp32);
		_HMAC_SHA256_Final(T, &hctx, tmp32, u.tmp8);

		if (c > 1) {
			/* T_i = U_1 ... */
			memcpy(U, T, 32);

			for (j = 2; j <= c; j++) {
				/* Compute U_j. */
				memcpy(&hctx, &Phctx, sizeof(HMAC_SHA256_CTX));
				_HMAC_SHA256_Update(&hctx, U, 32, tmp32);
				_HMAC_SHA256_Final(U, &hctx, tmp32, u.tmp8);

				/* ... xor U_j ... */
				for (k = 0; k < 32; k++)
					T[k] ^= U[k];
			}
		}

		/* Copy as many bytes as necessary into buf. */
		clen = dkLen - i * 32;
		if (clen > 32)
			clen = 32;
		memcpy(&buf[i * 32], T, clen);
	}

	/* Clean the stack. */
	insecure_memzero(&Phctx, sizeof(HMAC_SHA256_CTX));
	insecure_memzero(&PShctx, sizeof(HMAC_SHA256_CTX));
	insecure_memzero(U, 32);
	insecure_memzero(T, 32);

cleanup:
	insecure_memzero(&hctx, sizeof(HMAC_SHA256_CTX));
	insecure_memzero(tmp32, 288);
	insecure_memzero(&u, sizeof(u));
}
