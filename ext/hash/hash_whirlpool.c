/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Michael Wallner <mike@php.net>                              |
  |          Sara Golemon <pollita@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php_hash.h"

/*
 * TODO: simplify Update and Final, those look ridiculously complex
 * Mike, 2005-11-23
 */

#include "php_hash_whirlpool.h"
#include "php_hash_whirlpool_tables.h"

#define DIGESTBYTES 64
#define DIGESTBITS  (8*DIGESTBYTES) /* 512 */

#define WBLOCKBYTES 64
#define WBLOCKBITS  (8*WBLOCKBYTES) /* 512 */

#define LENGTHBYTES 32
#define LENGTHBITS  (8*LENGTHBYTES) /* 256 */

static void WhirlpoolTransform(PHP_WHIRLPOOL_CTX *context)
{
    int i, r;
    php_hash_uint64 K[8];        /* the round key */
    php_hash_uint64 block[8];    /* mu(buffer) */
    php_hash_uint64 state[8];    /* the cipher state */
    php_hash_uint64 L[8];
    unsigned char *buffer = context->buffer.data;

    /*
     * map the buffer to a block:
     */
    for (i = 0; i < 8; i++, buffer += 8) {
        block[i] =
            (((php_hash_uint64)buffer[0]        ) << 56) ^
            (((php_hash_uint64)buffer[1] & 0xffL) << 48) ^
            (((php_hash_uint64)buffer[2] & 0xffL) << 40) ^
            (((php_hash_uint64)buffer[3] & 0xffL) << 32) ^
            (((php_hash_uint64)buffer[4] & 0xffL) << 24) ^
            (((php_hash_uint64)buffer[5] & 0xffL) << 16) ^
            (((php_hash_uint64)buffer[6] & 0xffL) <<  8) ^
            (((php_hash_uint64)buffer[7] & 0xffL)      );
    }
    /*
     * compute and apply K^0 to the cipher state:
     */
    state[0] = block[0] ^ (K[0] = context->state[0]);
    state[1] = block[1] ^ (K[1] = context->state[1]);
    state[2] = block[2] ^ (K[2] = context->state[2]);
    state[3] = block[3] ^ (K[3] = context->state[3]);
    state[4] = block[4] ^ (K[4] = context->state[4]);
    state[5] = block[5] ^ (K[5] = context->state[5]);
    state[6] = block[6] ^ (K[6] = context->state[6]);
    state[7] = block[7] ^ (K[7] = context->state[7]);
    /*
     * iterate over all rounds:
     */
    for (r = 1; r <= R; r++) {
        /*
         * compute K^r from K^{r-1}:
         */
        L[0] =
            C0[(int)(K[0] >> 56)       ] ^
            C1[(int)(K[7] >> 48) & 0xff] ^
            C2[(int)(K[6] >> 40) & 0xff] ^
            C3[(int)(K[5] >> 32) & 0xff] ^
            C4[(int)(K[4] >> 24) & 0xff] ^
            C5[(int)(K[3] >> 16) & 0xff] ^
            C6[(int)(K[2] >>  8) & 0xff] ^
            C7[(int)(K[1]      ) & 0xff] ^
            rc[r];
        L[1] =
            C0[(int)(K[1] >> 56)       ] ^
            C1[(int)(K[0] >> 48) & 0xff] ^
            C2[(int)(K[7] >> 40) & 0xff] ^
            C3[(int)(K[6] >> 32) & 0xff] ^
            C4[(int)(K[5] >> 24) & 0xff] ^
            C5[(int)(K[4] >> 16) & 0xff] ^
            C6[(int)(K[3] >>  8) & 0xff] ^
            C7[(int)(K[2]      ) & 0xff];
        L[2] =
            C0[(int)(K[2] >> 56)       ] ^
            C1[(int)(K[1] >> 48) & 0xff] ^
            C2[(int)(K[0] >> 40) & 0xff] ^
            C3[(int)(K[7] >> 32) & 0xff] ^
            C4[(int)(K[6] >> 24) & 0xff] ^
            C5[(int)(K[5] >> 16) & 0xff] ^
            C6[(int)(K[4] >>  8) & 0xff] ^
            C7[(int)(K[3]      ) & 0xff];
        L[3] =
            C0[(int)(K[3] >> 56)       ] ^
            C1[(int)(K[2] >> 48) & 0xff] ^
            C2[(int)(K[1] >> 40) & 0xff] ^
            C3[(int)(K[0] >> 32) & 0xff] ^
            C4[(int)(K[7] >> 24) & 0xff] ^
            C5[(int)(K[6] >> 16) & 0xff] ^
            C6[(int)(K[5] >>  8) & 0xff] ^
            C7[(int)(K[4]      ) & 0xff];
        L[4] =
            C0[(int)(K[4] >> 56)       ] ^
            C1[(int)(K[3] >> 48) & 0xff] ^
            C2[(int)(K[2] >> 40) & 0xff] ^
            C3[(int)(K[1] >> 32) & 0xff] ^
            C4[(int)(K[0] >> 24) & 0xff] ^
            C5[(int)(K[7] >> 16) & 0xff] ^
            C6[(int)(K[6] >>  8) & 0xff] ^
            C7[(int)(K[5]      ) & 0xff];
        L[5] =
            C0[(int)(K[5] >> 56)       ] ^
            C1[(int)(K[4] >> 48) & 0xff] ^
            C2[(int)(K[3] >> 40) & 0xff] ^
            C3[(int)(K[2] >> 32) & 0xff] ^
            C4[(int)(K[1] >> 24) & 0xff] ^
            C5[(int)(K[0] >> 16) & 0xff] ^
            C6[(int)(K[7] >>  8) & 0xff] ^
            C7[(int)(K[6]      ) & 0xff];
        L[6] =
            C0[(int)(K[6] >> 56)       ] ^
            C1[(int)(K[5] >> 48) & 0xff] ^
            C2[(int)(K[4] >> 40) & 0xff] ^
            C3[(int)(K[3] >> 32) & 0xff] ^
            C4[(int)(K[2] >> 24) & 0xff] ^
            C5[(int)(K[1] >> 16) & 0xff] ^
            C6[(int)(K[0] >>  8) & 0xff] ^
            C7[(int)(K[7]      ) & 0xff];
        L[7] =
            C0[(int)(K[7] >> 56)       ] ^
            C1[(int)(K[6] >> 48) & 0xff] ^
            C2[(int)(K[5] >> 40) & 0xff] ^
            C3[(int)(K[4] >> 32) & 0xff] ^
            C4[(int)(K[3] >> 24) & 0xff] ^
            C5[(int)(K[2] >> 16) & 0xff] ^
            C6[(int)(K[1] >>  8) & 0xff] ^
            C7[(int)(K[0]      ) & 0xff];
        K[0] = L[0];
        K[1] = L[1];
        K[2] = L[2];
        K[3] = L[3];
        K[4] = L[4];
        K[5] = L[5];
        K[6] = L[6];
        K[7] = L[7];
        /*
         * apply the r-th round transformation:
         */
        L[0] =
            C0[(int)(state[0] >> 56)       ] ^
            C1[(int)(state[7] >> 48) & 0xff] ^
            C2[(int)(state[6] >> 40) & 0xff] ^
            C3[(int)(state[5] >> 32) & 0xff] ^
            C4[(int)(state[4] >> 24) & 0xff] ^
            C5[(int)(state[3] >> 16) & 0xff] ^
            C6[(int)(state[2] >>  8) & 0xff] ^
            C7[(int)(state[1]      ) & 0xff] ^
            K[0];
        L[1] =
            C0[(int)(state[1] >> 56)       ] ^
            C1[(int)(state[0] >> 48) & 0xff] ^
            C2[(int)(state[7] >> 40) & 0xff] ^
            C3[(int)(state[6] >> 32) & 0xff] ^
            C4[(int)(state[5] >> 24) & 0xff] ^
            C5[(int)(state[4] >> 16) & 0xff] ^
            C6[(int)(state[3] >>  8) & 0xff] ^
            C7[(int)(state[2]      ) & 0xff] ^
            K[1];
        L[2] =
            C0[(int)(state[2] >> 56)       ] ^
            C1[(int)(state[1] >> 48) & 0xff] ^
            C2[(int)(state[0] >> 40) & 0xff] ^
            C3[(int)(state[7] >> 32) & 0xff] ^
            C4[(int)(state[6] >> 24) & 0xff] ^
            C5[(int)(state[5] >> 16) & 0xff] ^
            C6[(int)(state[4] >>  8) & 0xff] ^
            C7[(int)(state[3]      ) & 0xff] ^
            K[2];
        L[3] =
            C0[(int)(state[3] >> 56)       ] ^
            C1[(int)(state[2] >> 48) & 0xff] ^
            C2[(int)(state[1] >> 40) & 0xff] ^
            C3[(int)(state[0] >> 32) & 0xff] ^
            C4[(int)(state[7] >> 24) & 0xff] ^
            C5[(int)(state[6] >> 16) & 0xff] ^
            C6[(int)(state[5] >>  8) & 0xff] ^
            C7[(int)(state[4]      ) & 0xff] ^
            K[3];
        L[4] =
            C0[(int)(state[4] >> 56)       ] ^
            C1[(int)(state[3] >> 48) & 0xff] ^
            C2[(int)(state[2] >> 40) & 0xff] ^
            C3[(int)(state[1] >> 32) & 0xff] ^
            C4[(int)(state[0] >> 24) & 0xff] ^
            C5[(int)(state[7] >> 16) & 0xff] ^
            C6[(int)(state[6] >>  8) & 0xff] ^
            C7[(int)(state[5]      ) & 0xff] ^
            K[4];
        L[5] =
            C0[(int)(state[5] >> 56)       ] ^
            C1[(int)(state[4] >> 48) & 0xff] ^
            C2[(int)(state[3] >> 40) & 0xff] ^
            C3[(int)(state[2] >> 32) & 0xff] ^
            C4[(int)(state[1] >> 24) & 0xff] ^
            C5[(int)(state[0] >> 16) & 0xff] ^
            C6[(int)(state[7] >>  8) & 0xff] ^
            C7[(int)(state[6]      ) & 0xff] ^
            K[5];
        L[6] =
            C0[(int)(state[6] >> 56)       ] ^
            C1[(int)(state[5] >> 48) & 0xff] ^
            C2[(int)(state[4] >> 40) & 0xff] ^
            C3[(int)(state[3] >> 32) & 0xff] ^
            C4[(int)(state[2] >> 24) & 0xff] ^
            C5[(int)(state[1] >> 16) & 0xff] ^
            C6[(int)(state[0] >>  8) & 0xff] ^
            C7[(int)(state[7]      ) & 0xff] ^
            K[6];
        L[7] =
            C0[(int)(state[7] >> 56)       ] ^
            C1[(int)(state[6] >> 48) & 0xff] ^
            C2[(int)(state[5] >> 40) & 0xff] ^
            C3[(int)(state[4] >> 32) & 0xff] ^
            C4[(int)(state[3] >> 24) & 0xff] ^
            C5[(int)(state[2] >> 16) & 0xff] ^
            C6[(int)(state[1] >>  8) & 0xff] ^
            C7[(int)(state[0]      ) & 0xff] ^
            K[7];
        state[0] = L[0];
        state[1] = L[1];
        state[2] = L[2];
        state[3] = L[3];
        state[4] = L[4];
        state[5] = L[5];
        state[6] = L[6];
        state[7] = L[7];
    }
    /*
     * apply the Miyaguchi-Preneel compression function:
     */
    context->state[0] ^= state[0] ^ block[0];
    context->state[1] ^= state[1] ^ block[1];
    context->state[2] ^= state[2] ^ block[2];
    context->state[3] ^= state[3] ^ block[3];
    context->state[4] ^= state[4] ^ block[4];
    context->state[5] ^= state[5] ^ block[5];
    context->state[6] ^= state[6] ^ block[6];
    context->state[7] ^= state[7] ^ block[7];

	ZEND_SECURE_ZERO(state, sizeof(state));
}

PHP_HASH_API void PHP_WHIRLPOOLInit(PHP_WHIRLPOOL_CTX *context)
{
	memset(context, 0, sizeof(*context));
}

PHP_HASH_API void PHP_WHIRLPOOLUpdate(PHP_WHIRLPOOL_CTX *context, const unsigned char *input, size_t len)
{
    php_hash_uint64 sourceBits = len * 8;
    int sourcePos    = 0; /* index of leftmost source unsigned char containing data (1 to 8 bits). */
    int sourceGap    = (8 - ((int)sourceBits & 7)) & 7; /* space on source[sourcePos]. */
    int bufferRem    = context->buffer.bits & 7; /* occupied bits on buffer[bufferPos]. */
    const unsigned char *source = input;
    unsigned char *buffer       = context->buffer.data;
    unsigned char *bitLength    = context->bitlength;
    int bufferBits   = context->buffer.bits;
    int bufferPos    = context->buffer.pos;
    php_hash_uint32 b, carry;
    int i;

    /*
     * tally the length of the added data:
     */
    php_hash_uint64 value = sourceBits;
    for (i = 31, carry = 0; i >= 0 && (carry != 0 || value != L64(0)); i--) {
        carry += bitLength[i] + ((php_hash_uint32)value & 0xff);
        bitLength[i] = (unsigned char)carry;
        carry >>= 8;
        value >>= 8;
    }
    /*
     * process data in chunks of 8 bits (a more efficient approach would be to take whole-word chunks):
     */
    while (sourceBits > 8) {
        /* N.B. at least source[sourcePos] and source[sourcePos+1] contain data. */
        /*
         * take a byte from the source:
         */
        b = ((source[sourcePos] << sourceGap) & 0xff) |
            ((source[sourcePos + 1] & 0xff) >> (8 - sourceGap));
        /*
         * process this byte:
         */
        buffer[bufferPos++] |= (unsigned char)(b >> bufferRem);
        bufferBits += 8 - bufferRem; /* bufferBits = 8*bufferPos; */
        if (bufferBits == DIGESTBITS) {
            /*
             * process data block:
             */
            WhirlpoolTransform(context);
            /*
             * reset buffer:
             */
            bufferBits = bufferPos = 0;
        }
        buffer[bufferPos] = (unsigned char) (b << (8 - bufferRem));
        bufferBits += bufferRem;
        /*
         * proceed to remaining data:
         */
        sourceBits -= 8;
        sourcePos++;
    }
    /* now 0 <= sourceBits <= 8;
     * furthermore, all data (if any is left) is in source[sourcePos].
     */
    if (sourceBits > 0) {
        b = (source[sourcePos] << sourceGap) & 0xff; /* bits are left-justified on b. */
        /*
         * process the remaining bits:
         */
        buffer[bufferPos] |= b >> bufferRem;
    } else {
        b = 0;
    }
    if (bufferRem + sourceBits < 8) {
        /*
         * all remaining data fits on buffer[bufferPos],
         * and there still remains some space.
         */
        bufferBits += (int) sourceBits;
    } else {
        /*
         * buffer[bufferPos] is full:
         */
        bufferPos++;
        bufferBits += 8 - bufferRem; /* bufferBits = 8*bufferPos; */
        sourceBits -= 8 - bufferRem;
        /* now 0 <= sourceBits < 8;
         * furthermore, all data (if any is left) is in source[sourcePos].
         */
        if (bufferBits == DIGESTBITS) {
            /*
             * process data block:
             */
            WhirlpoolTransform(context);
            /*
             * reset buffer:
             */
            bufferBits = bufferPos = 0;
        }
        buffer[bufferPos] = (unsigned char) (b << (8 - bufferRem));
        bufferBits += (int)sourceBits;
    }
    context->buffer.bits   = bufferBits;
    context->buffer.pos    = bufferPos;
}

PHP_HASH_API void PHP_WHIRLPOOLFinal(unsigned char digest[64], PHP_WHIRLPOOL_CTX *context)
{
    int i;
    unsigned char *buffer      = context->buffer.data;
    unsigned char *bitLength   = context->bitlength;
    int bufferBits  = context->buffer.bits;
    int bufferPos   = context->buffer.pos;

    /*
     * append a '1'-bit:
     */
    buffer[bufferPos] |= 0x80U >> (bufferBits & 7);
    bufferPos++; /* all remaining bits on the current unsigned char are set to zero. */
    /*
     * pad with zero bits to complete (N*WBLOCKBITS - LENGTHBITS) bits:
     */
    if (bufferPos > WBLOCKBYTES - LENGTHBYTES) {
        if (bufferPos < WBLOCKBYTES) {
            memset(&buffer[bufferPos], 0, WBLOCKBYTES - bufferPos);
        }
        /*
         * process data block:
         */
        WhirlpoolTransform(context);
        /*
         * reset buffer:
         */
        bufferPos = 0;
    }
    if (bufferPos < WBLOCKBYTES - LENGTHBYTES) {
        memset(&buffer[bufferPos], 0, (WBLOCKBYTES - LENGTHBYTES) - bufferPos);
    }
    bufferPos = WBLOCKBYTES - LENGTHBYTES;
    /*
     * append bit length of hashed data:
     */
    memcpy(&buffer[WBLOCKBYTES - LENGTHBYTES], bitLength, LENGTHBYTES);
    /*
     * process data block:
     */
    WhirlpoolTransform(context);
    /*
     * return the completed message digest:
     */
    for (i = 0; i < DIGESTBYTES/8; i++) {
        digest[0] = (unsigned char)(context->state[i] >> 56);
        digest[1] = (unsigned char)(context->state[i] >> 48);
        digest[2] = (unsigned char)(context->state[i] >> 40);
        digest[3] = (unsigned char)(context->state[i] >> 32);
        digest[4] = (unsigned char)(context->state[i] >> 24);
        digest[5] = (unsigned char)(context->state[i] >> 16);
        digest[6] = (unsigned char)(context->state[i] >>  8);
        digest[7] = (unsigned char)(context->state[i]      );
        digest += 8;
    }

    ZEND_SECURE_ZERO(context, sizeof(*context));
}

const php_hash_ops php_hash_whirlpool_ops = {
	(php_hash_init_func_t) PHP_WHIRLPOOLInit,
	(php_hash_update_func_t) PHP_WHIRLPOOLUpdate,
	(php_hash_final_func_t) PHP_WHIRLPOOLFinal,
	(php_hash_copy_func_t) php_hash_copy,
	64,
	64,
	sizeof(PHP_WHIRLPOOL_CTX)
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
