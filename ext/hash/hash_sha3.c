/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sara Golemon <pollita@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "php_hash.h"
#include "php_hash_sha3.h"

#ifdef HAVE_SLOW_HASH3
// ================= slow algo ==============================================

#if (defined(__APPLE__) || defined(__APPLE_CC__)) && \
    (defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__))
# if defined(__LITTLE_ENDIAN__)
#  undef WORDS_BIGENDIAN
# else
#  if defined(__BIG_ENDIAN__)
#   define WORDS_BIGENDIAN
#  endif
# endif
#endif

static inline uint64_t rol64(uint64_t v, unsigned char b) {
	return (v << b) | (v >> (64 - b));
}
static inline unsigned char idx(unsigned char x, unsigned char y) {
	return x + (5 * y);
}

#ifdef WORDS_BIGENDIAN
static inline uint64_t load64(const unsigned char* x) {
	signed char i;
	uint64_t ret = 0;
	for (i = 7; i >= 0; --i) {
		ret <<= 8;
		ret |= x[i];
	}
	return ret;
}
static inline void store64(unsigned char* x, uint64_t val) {
	char i;
	for (i = 0; i < 8; ++i) {
		x[i] = val & 0xFF;
		val >>= 8;
	}
}
static inline void xor64(unsigned char* x, uint64_t val) {
	char i;
	for (i = 0; i < 8; ++i) {
		x[i] ^= val & 0xFF;
		val >>= 8;
	}
}
# define readLane(x, y)     load64(ctx->state+sizeof(uint64_t)*idx(x, y))
# define writeLane(x, y, v) store64(ctx->state+sizeof(uint64_t)*idx(x, y), v)
# define XORLane(x, y, v)   xor64(ctx->state+sizeof(uint64_t)*idx(x, y), v)
#else
# define readLane(x, y)     (((uint64_t*)ctx->state)[idx(x,y)])
# define writeLane(x, y, v) (((uint64_t*)ctx->state)[idx(x,y)] = v)
# define XORLane(x, y, v)   (((uint64_t*)ctx->state)[idx(x,y)] ^= v)
#endif

static inline char LFSR86540(unsigned char* pLFSR)
{
	unsigned char LFSR = *pLFSR;
	char result = LFSR & 0x01;
	if (LFSR & 0x80) {
		// Primitive polynomial over GF(2): x^8+x^6+x^5+x^4+1
		LFSR = (LFSR << 1) ^ 0x71;
	} else {
		LFSR <<= 1;
	}
	*pLFSR = LFSR;
	return result;
}

static void permute(PHP_SHA3_CTX* ctx) {
	unsigned char LFSRstate = 0x01;
	unsigned char round;

	for (round = 0; round < 24; ++round) {
		{ // Theta step (see [Keccak Reference, Section 2.3.2])
			uint64_t C[5], D;
			unsigned char x, y;
			for (x = 0; x < 5; ++x) {
				C[x] = readLane(x, 0) ^ readLane(x, 1) ^
				readLane(x, 2) ^ readLane(x, 3) ^ readLane(x, 4);
			}
			for (x = 0; x < 5; ++x) {
				D = C[(x+4)%5] ^ rol64(C[(x+1)%5], 1);
				for (y = 0; y < 5; ++y) {
					XORLane(x, y, D);
				}
			}
		}

		{ // p and Pi steps (see [Keccak Reference, Sections 2.3.3 and 2.3.4])
			unsigned char x = 1, y = 0, t;
			uint64_t current = readLane(x, y);
			for (t = 0; t < 24; ++t) {
				unsigned char r = ((t + 1) * (t + 2) / 2) % 64;
				unsigned char Y = (2*x + 3*y) % 5;
				uint64_t temp;
				x = y;
				y = Y;
				temp = readLane(x, y);
				writeLane(x, y, rol64(current, r));
				current = temp;
			}
		}

		{ // X step (see [Keccak Reference, Section 2.3.1])
			unsigned char x, y;
			for (y = 0; y < 5; ++y) {
				uint64_t temp[5];
				for (x = 0; x < 5; ++x) {
					temp[x] = readLane(x, y);
				}
				for (x = 0; x < 5; ++x) {
					writeLane(x, y, temp[x] ^((~temp[(x+1)%5]) & temp[(x+2)%5]));
				}
			}
		}

		{ // i step (see [Keccak Reference, Section 2.3.5])
			unsigned char j;
			for (j = 0; j < 7; ++j) {
				if (LFSR86540(&LFSRstate)) {
					uint64_t bitPos = (1<<j) - 1;
					XORLane(0, 0, (uint64_t)1 << bitPos);
				}
			}
		}
	}
}

// ==========================================================================

static void PHP_SHA3_Init(PHP_SHA3_CTX* ctx,
                          int bits) {
	memset(ctx, 0, sizeof(PHP_SHA3_CTX));
}

static void PHP_SHA3_Update(PHP_SHA3_CTX* ctx,
                            const unsigned char* buf,
                            unsigned int count,
                            size_t block_size) {
	while (count > 0) {
		unsigned int len = block_size - ctx->pos;
		if (len > count) len = count;
		count -= len;
		while (len-- > 0) {
			ctx->state[ctx->pos++] ^= *(buf++);
		}
		if (ctx->pos >= block_size) {
			permute(ctx);
			ctx->pos = 0;
		}
	}
}

static void PHP_SHA3_Final(unsigned char* digest,
                           PHP_SHA3_CTX* ctx,
                           int block_size,
                           int digest_size) {
	int len = digest_size;

	// Pad state to finalize
	ctx->state[ctx->pos++] ^= 0x06;
	ctx->state[block_size-1] ^= 0x80;
	permute(ctx);

	// Square output for digest
	for(;;) {
		int bs = (len < block_size) ? len : block_size;
		memcpy(digest, ctx->state, bs);
		digest += bs;
		len -= bs;
		if (!len) break;
		permute(ctx);
	}

	// Zero out context
	memset(ctx, 0, sizeof(PHP_SHA3_CTX));
}

// ==========================================================================

#define DECLARE_SHA3_OPS(bits) \
void PHP_SHA3##bits##Init(PHP_SHA3_##bits##_CTX* ctx) { \
	PHP_SHA3_Init(ctx, bits); \
} \
void PHP_SHA3##bits##Update(PHP_SHA3_##bits##_CTX* ctx, \
                            const unsigned char* input, \
                            unsigned int inputLen) { \
	PHP_SHA3_Update(ctx, input, inputLen, \
                    (1600 - (2 * bits)) >> 3); \
} \
void PHP_SHA3##bits##Final(unsigned char* digest, \
                           PHP_SHA3_##bits##_CTX* ctx) { \
	PHP_SHA3_Final(digest, ctx, \
                   (1600 - (2 * bits)) >> 3, \
                   bits >> 3); \
} \
const php_hash_ops php_hash_sha3_##bits##_ops = { \
	(php_hash_init_func_t) PHP_SHA3##bits##Init, \
	(php_hash_update_func_t) PHP_SHA3##bits##Update, \
	(php_hash_final_func_t) PHP_SHA3##bits##Final, \
	php_hash_copy, \
	bits >> 3, \
	(1600 - (2 * bits)) >> 3, \
	sizeof(PHP_SHA3_##bits##_CTX), \
	1 \
}

#else

// ================= fast algo ==============================================

#define SUCCESS SHA3_SUCCESS /* Avoid conflict between KeccacHash.h and zend_types.h */
#include "KeccakHash.h"


// ==========================================================================

static int hash_sha3_copy(const void *ops, void *orig_context, void *dest_context)
{
	PHP_SHA3_CTX* orig = (PHP_SHA3_CTX*)orig_context;
	PHP_SHA3_CTX* dest = (PHP_SHA3_CTX*)dest_context;
	memcpy(dest->hashinstance, orig->hashinstance, sizeof(Keccak_HashInstance));
	return SUCCESS;
}

#define DECLARE_SHA3_OPS(bits) \
void PHP_SHA3##bits##Init(PHP_SHA3_##bits##_CTX* ctx) { \
	ctx->hashinstance = emalloc(sizeof(Keccak_HashInstance)); \
	Keccak_HashInitialize_SHA3_##bits((Keccak_HashInstance *)ctx->hashinstance); \
} \
void PHP_SHA3##bits##Update(PHP_SHA3_##bits##_CTX* ctx, \
                            const unsigned char* input, \
                            unsigned int inputLen) { \
	Keccak_HashUpdate((Keccak_HashInstance *)ctx->hashinstance, input, inputLen * 8); \
} \
void PHP_SHA3##bits##Final(unsigned char* digest, \
                           PHP_SHA3_##bits##_CTX* ctx) { \
	Keccak_HashFinal((Keccak_HashInstance *)ctx->hashinstance, digest); \
	efree(ctx->hashinstance); \
	ctx->hashinstance = NULL; \
} \
const php_hash_ops php_hash_sha3_##bits##_ops = { \
	(php_hash_init_func_t) PHP_SHA3##bits##Init, \
	(php_hash_update_func_t) PHP_SHA3##bits##Update, \
	(php_hash_final_func_t) PHP_SHA3##bits##Final, \
	hash_sha3_copy, \
	bits >> 3, \
	(1600 - (2 * bits)) >> 3, \
	sizeof(PHP_SHA3_##bits##_CTX), \
	1 \
}

#endif
// ================= both algo ==============================================

DECLARE_SHA3_OPS(224);
DECLARE_SHA3_OPS(256);
DECLARE_SHA3_OPS(384);
DECLARE_SHA3_OPS(512);

#undef DECLARE_SHA3_OPS

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
