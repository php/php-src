/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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
	size_t i;
	for (i = 0; i < 8; ++i) {
		x[i] = val & 0xFF;
		val >>= 8;
	}
}
static inline void xor64(unsigned char* x, uint64_t val) {
	size_t i;
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
                            size_t count,
                            size_t block_size) {
	while (count > 0) {
		size_t len = block_size - ctx->pos;

		if (len > count) {
			len = count;
		}

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
                           size_t block_size,
                           size_t digest_size) {
	size_t len = digest_size;

	// Pad state to finalize
	ctx->state[ctx->pos++] ^= 0x06;
	ctx->state[block_size-1] ^= 0x80;
	permute(ctx);

	// Square output for digest
	for(;;) {
		int bs = (len < block_size) ? len : block_size;
		digest = zend_mempcpy(digest, ctx->state, bs);
		len -= bs;
		if (!len) break;
		permute(ctx);
	}

	// Zero out context
	ZEND_SECURE_ZERO(ctx, sizeof(PHP_SHA3_CTX));
}

static int php_sha3_unserialize(php_hashcontext_object *hash,
				zend_long magic,
				const zval *zv,
				size_t block_size)
{
	PHP_SHA3_CTX *ctx = (PHP_SHA3_CTX *) hash->context;
	int r = FAILURE;
	if (magic == PHP_HASH_SERIALIZE_MAGIC_SPEC
		&& (r = php_hash_unserialize_spec(hash, zv, PHP_SHA3_SPEC)) == SUCCESS
		&& ctx->pos < block_size) {
		return SUCCESS;
	} else {
		return r != SUCCESS ? r : -2000;
	}
}

// ==========================================================================

#define DECLARE_SHA3_OPS(bits) \
void PHP_SHA3##bits##Init(PHP_SHA3_##bits##_CTX* ctx, ZEND_ATTRIBUTE_UNUSED HashTable *args) { \
	PHP_SHA3_Init(ctx, bits); \
} \
void PHP_SHA3##bits##Update(PHP_SHA3_##bits##_CTX* ctx, \
                            const unsigned char* input, \
                            size_t inputLen) { \
	PHP_SHA3_Update(ctx, input, inputLen, \
                    (1600 - (2 * bits)) >> 3); \
} \
void PHP_SHA3##bits##Final(unsigned char* digest, \
                           PHP_SHA3_##bits##_CTX* ctx) { \
	PHP_SHA3_Final(digest, ctx, \
                   (1600 - (2 * bits)) >> 3, \
                   bits >> 3); \
} \
static int php_sha3##bits##_unserialize(php_hashcontext_object *hash, \
					zend_long magic, \
					const zval *zv) { \
	return php_sha3_unserialize(hash, magic, zv, (1600 - (2 * bits)) >> 3); \
} \
const php_hash_ops php_hash_sha3_##bits##_ops = { \
	"sha3-" #bits, \
	(php_hash_init_func_t) PHP_SHA3##bits##Init, \
	(php_hash_update_func_t) PHP_SHA3##bits##Update, \
	(php_hash_final_func_t) PHP_SHA3##bits##Final, \
	php_hash_copy, \
	php_hash_serialize, \
	php_sha3##bits##_unserialize, \
	PHP_SHA3_SPEC, \
	bits >> 3, \
	(1600 - (2 * bits)) >> 3, \
	sizeof(PHP_SHA3_##bits##_CTX), \
	1 \
}

#else

// ================= fast algo ==============================================

#define SUCCESS SHA3_SUCCESS /* Avoid conflict between KeccacHash.h and zend_types.h */
#include "KeccakHash.h"

/* KECCAK SERIALIZATION

   Keccak_HashInstance consists of:
	KeccakWidth1600_SpongeInstance {
		unsigned char state[200];
		unsigned int rate;         -- fixed for digest size
		unsigned int byteIOIndex;  -- in range [0, rate/8)
		int squeezing;             -- 0 normally, 1 only during finalize
	} sponge;
	unsigned int fixedOutputLength;    -- fixed for digest size
	unsigned char delimitedSuffix;     -- fixed for digest size

   NB If the external sha3/ library is updated, the serialization code
   may need to be updated.

   The simpler SHA3 code's serialization states are not interchangeable with
   Keccak. Furthermore, the Keccak sponge state is sensitive to architecture
   -- 32-bit and 64-bit implementations produce different states. It does not
   appear that the state is sensitive to endianness. */

#if Keccak_HashInstance_ImplType == 64
/* corresponds to sha3/generic64lc */
# define PHP_HASH_SERIALIZE_MAGIC_KECCAK 100
#elif Keccak_HashInstance_ImplType == 32
/* corresponds to sha3/generic32lc */
# define PHP_HASH_SERIALIZE_MAGIC_KECCAK 101
#else
# error "Unknown Keccak_HashInstance_ImplType"
#endif
#define PHP_KECCAK_SPEC "b200IiIIB"

static int php_keccak_serialize(const php_hashcontext_object *hash, zend_long *magic, zval *zv)
{
	*magic = PHP_HASH_SERIALIZE_MAGIC_KECCAK;
	return php_hash_serialize_spec(hash, zv, PHP_KECCAK_SPEC);
}

static int php_keccak_unserialize(php_hashcontext_object *hash, zend_long magic, const zval *zv)
{
	Keccak_HashInstance *ctx = (Keccak_HashInstance *) hash->context;
	int r = FAILURE;
	if (magic == PHP_HASH_SERIALIZE_MAGIC_KECCAK
		&& (r = php_hash_unserialize_spec(hash, zv, PHP_KECCAK_SPEC)) == SUCCESS
		&& ctx->sponge.byteIOIndex < ctx->sponge.rate / 8) {
		return SUCCESS;
	} else {
		return r != SUCCESS ? r : -2000;
	}
}

// ==========================================================================

#define DECLARE_SHA3_OPS(bits) \
void PHP_SHA3##bits##Init(PHP_SHA3_##bits##_CTX* ctx, ZEND_ATTRIBUTE_UNUSED HashTable *args) { \
	ZEND_ASSERT(sizeof(Keccak_HashInstance) <= sizeof(PHP_SHA3_##bits##_CTX)); \
	Keccak_HashInitialize_SHA3_##bits((Keccak_HashInstance *)ctx); \
} \
void PHP_SHA3##bits##Update(PHP_SHA3_##bits##_CTX* ctx, \
                            const unsigned char* input, \
                            size_t inputLen) { \
	Keccak_HashUpdate((Keccak_HashInstance *)ctx, input, inputLen * 8); \
} \
void PHP_SHA3##bits##Final(unsigned char* digest, \
                           PHP_SHA3_##bits##_CTX* ctx) { \
	Keccak_HashFinal((Keccak_HashInstance *)ctx, digest); \
} \
const php_hash_ops php_hash_sha3_##bits##_ops = { \
	"sha3-" #bits, \
	(php_hash_init_func_t) PHP_SHA3##bits##Init, \
	(php_hash_update_func_t) PHP_SHA3##bits##Update, \
	(php_hash_final_func_t) PHP_SHA3##bits##Final, \
	php_hash_copy, \
	php_keccak_serialize, \
	php_keccak_unserialize, \
	PHP_KECCAK_SPEC, \
	bits >> 3, \
	(1600 - (2 * bits)) >> 3, \
	sizeof(PHP_SHA3_CTX), \
	1 \
}

#endif
// ================= both algo ==============================================

DECLARE_SHA3_OPS(224);
DECLARE_SHA3_OPS(256);
DECLARE_SHA3_OPS(384);
DECLARE_SHA3_OPS(512);

#undef DECLARE_SHA3_OPS
