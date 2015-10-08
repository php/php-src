#ifndef ZEND_HASH_FUNC_H
#define ZEND_HASH_FUNC_H

/* <MIT License>
 Copyright (c) 2013  Marek Majkowski <marek@popcount.org>
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 </MIT License>
 Original location:
    https://github.com/majek/csiphash/
 Solution inspired by code from:
    Samuel Neves (supercop/crypto_auth/siphash24/little)
    djb (supercop/crypto_auth/siphash24/little2)
    Jean-Philippe Aumasson (https://131002.net/siphash/siphash24.c)
*/

#include "zend_types.h"

#if WORDS_BIGENDIAN
# if defined(__APPLE__)
#  include <libkern/OSByteOrder.h>
#  define _le64toh(x) OSSwapLittleToHostInt64(x)
# else
/* See: http://sourceforge.net/p/predef/wiki/Endianness/ */
#  if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#   include <sys/endian.h>
#  else
#   include <endian.h>
#  endif
# endif
# define _le64toh(x) le64toh(x)
#else
# define _le64toh(x) ((uint64_t)(x))
#endif


#define ROTATE(x, b) (uint64_t)( ((x) << (b)) | ( (x) >> (64 - (b))) )

#define HALF_ROUND(a,b,c,d,s,t)			\
	a += b; c += d;				\
	b = ROTATE(b, s) ^ a;			\
	d = ROTATE(d, t) ^ c;			\
	a = ROTATE(a, 32);

#define DOUBLE_ROUND(v0,v1,v2,v3)		\
	HALF_ROUND(v0,v1,v2,v3,13,16);		\
	HALF_ROUND(v2,v1,v0,v3,17,21);		\
	HALF_ROUND(v0,v1,v2,v3,13,16);		\
	HALF_ROUND(v2,v1,v0,v3,17,21);

extern unsigned char zend_siphash_key[16];

static zend_always_inline uint64_t siphash24_bytes(
		const unsigned char *src, size_t src_sz, const unsigned char *key) {
	const uint64_t *_key = (uint64_t *) key;
	uint64_t k0 = _le64toh(_key[0]);
	uint64_t k1 = _le64toh(_key[1]);
	uint64_t b = (uint64_t)src_sz << 56;
	const uint64_t *in = (uint64_t*)src;

	uint64_t v0 = k0 ^ 0x736f6d6570736575ULL;
	uint64_t v1 = k1 ^ 0x646f72616e646f6dULL;
	uint64_t v2 = k0 ^ 0x6c7967656e657261ULL;
	uint64_t v3 = k1 ^ 0x7465646279746573ULL;

	uint64_t t;
	uint8_t *pt, *m;

	while (src_sz >= 8) {
		uint64_t mi = _le64toh(*in);
		in += 1; src_sz -= 8;
		v3 ^= mi;
		DOUBLE_ROUND(v0,v1,v2,v3);
		v0 ^= mi;
	}

	t = 0;
	pt = (uint8_t *)&t;
	m = (uint8_t *)in;
	switch (src_sz) {
		case 7: pt[6] = m[6];
		case 6: pt[5] = m[5];
		case 5: pt[4] = m[4];
		case 4: *((uint32_t*)&pt[0]) = *((uint32_t*)&m[0]); break;
		case 3: pt[2] = m[2];
		case 2: pt[1] = m[1];
		case 1: pt[0] = m[0];
	}
	b |= _le64toh(t);

	v3 ^= b;
	DOUBLE_ROUND(v0,v1,v2,v3);
	v0 ^= b; v2 ^= 0xff;
	DOUBLE_ROUND(v0,v1,v2,v3);
	DOUBLE_ROUND(v0,v1,v2,v3);
	return (v0 ^ v1) ^ (v2 ^ v3);
}

static zend_always_inline uint64_t siphash24_u64(uint64_t in, const unsigned char *key) {
	const uint64_t *_key = (uint64_t *) key;
	uint64_t k0 = _le64toh(_key[0]);
	uint64_t k1 = _le64toh(_key[1]);
	uint64_t b = (uint64_t)8 << 56;

	uint64_t v0 = k0 ^ 0x736f6d6570736575ULL;
	uint64_t v1 = k1 ^ 0x646f72616e646f6dULL;
	uint64_t v2 = k0 ^ 0x6c7967656e657261ULL;
	uint64_t v3 = k1 ^ 0x7465646279746573ULL;

	/* TODO: Need _le64toh here? */
	uint64_t mi = _le64toh(in);
	v3 ^= mi;
	DOUBLE_ROUND(v0,v1,v2,v3);
	v0 ^= mi;
	v3 ^= b;
	DOUBLE_ROUND(v0,v1,v2,v3);
	v0 ^= b;
	v2 ^= 0xff;
	DOUBLE_ROUND(v0,v1,v2,v3);
	DOUBLE_ROUND(v0,v1,v2,v3);
	return (v0 ^ v1) ^ (v2 ^ v3);
}

static zend_always_inline zend_ulong zend_inline_hash_func(const char *str, size_t len)
{
	uint64_t hash = siphash24_bytes((unsigned char *) str, len, zend_siphash_key);
	return hash | HT_IS_STR_BIT;
}

static zend_always_inline zend_ulong zend_hash_integer(zend_ulong h) {
	uint64_t hash = siphash24_u64(h, zend_siphash_key);
	return hash & ~HT_IS_STR_BIT;
}


#endif

