/*
   +----------------------------------------------------------------------+
   | Zend OPcache JIT                                                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef _ZEND_BITSET_H_
#define _ZEND_BITSET_H_

typedef zend_ulong *zend_bitset;

#define ZEND_BITSET_ELM_SIZE sizeof(zend_ulong)

#if SIZEOF_ZEND_LONG == 4
# define ZEND_BITSET_ELM_NUM(n)		((n) >> 5)
# define ZEND_BITSET_BIT_NUM(n)		((zend_ulong)(n) & Z_UL(0x1f))
#elif SIZEOF_ZEND_LONG == 8
# define ZEND_BITSET_ELM_NUM(n)		((n) >> 6)
# define ZEND_BITSET_BIT_NUM(n)		((zend_ulong)(n) & Z_UL(0x3f))
#else
# define ZEND_BITSET_ELM_NUM(n)		((n) / (sizeof(zend_long) * 8))
# define ZEND_BITSET_BIT_NUM(n)		((n) % (sizeof(zend_long) * 8))
#endif

#define ZEND_BITSET_ALLOCA(n, use_heap) \
	(zend_bitset)do_alloca((n) * ZEND_BITSET_ELM_SIZE, use_heap)

/* Number of trailing zero bits (0x01 -> 0; 0x40 -> 6; 0x00 -> LEN) */
static zend_always_inline int zend_ulong_ntz(zend_ulong num)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_ctzl)) \
	&& SIZEOF_ZEND_LONG == SIZEOF_LONG && defined(PHP_HAVE_BUILTIN_CTZL)
	return __builtin_ctzl(num);
#elif (defined(__GNUC__) || __has_builtin(__builtin_ctzll)) && defined(PHP_HAVE_BUILTIN_CTZLL)
	return __builtin_ctzll(num);
#elif defined(_WIN32)
	unsigned long index;

#if defined(_WIN64)
	if (!BitScanForward64(&index, num)) {
#else
	if (!BitScanForward(&index, num)) {
#endif
		/* undefined behavior */
		return SIZEOF_ZEND_LONG * 8;
	}

	return (int) index;
#else
	int n;

	if (num == Z_UL(0)) return SIZEOF_ZEND_LONG * 8;

	n = 1;
#if SIZEOF_ZEND_LONG == 8
	if ((num & 0xffffffff) == 0) {n += 32; num = num >> Z_UL(32);}
#endif
	if ((num & 0x0000ffff) == 0) {n += 16; num = num >> 16;}
	if ((num & 0x000000ff) == 0) {n +=  8; num = num >>  8;}
	if ((num & 0x0000000f) == 0) {n +=  4; num = num >>  4;}
	if ((num & 0x00000003) == 0) {n +=  2; num = num >>  2;}
	return n - (num & 1);
#endif
}

/* Returns the number of zend_ulong words needed to store a bitset that is N
   bits long.  */
static inline uint32_t zend_bitset_len(uint32_t n)
{
	return (n + ((sizeof(zend_long) * 8) - 1)) / (sizeof(zend_long) * 8);
}

static inline zend_bool zend_bitset_in(zend_bitset set, uint32_t n)
{
	return ZEND_BIT_TEST(set, n);
}

static inline void zend_bitset_incl(zend_bitset set, uint32_t n)
{
	set[ZEND_BITSET_ELM_NUM(n)] |= Z_UL(1) << ZEND_BITSET_BIT_NUM(n);
}

static inline void zend_bitset_excl(zend_bitset set, uint32_t n)
{
	set[ZEND_BITSET_ELM_NUM(n)] &= ~(Z_UL(1) << ZEND_BITSET_BIT_NUM(n));
}

static inline void zend_bitset_clear(zend_bitset set, uint32_t len)
{
	memset(set, 0, len * ZEND_BITSET_ELM_SIZE);
}

static inline int zend_bitset_empty(zend_bitset set, uint32_t len)
{
	uint32_t i;
	for (i = 0; i < len; i++) {
		if (set[i]) {
			return 0;
		}
	}
	return 1;
}

static inline void zend_bitset_fill(zend_bitset set, uint32_t len)
{
	memset(set, 0xff, len * ZEND_BITSET_ELM_SIZE);
}

static inline zend_bool zend_bitset_equal(zend_bitset set1, zend_bitset set2, uint32_t len)
{
    return memcmp(set1, set2, len * ZEND_BITSET_ELM_SIZE) == 0;
}

static inline void zend_bitset_copy(zend_bitset set1, zend_bitset set2, uint32_t len)
{
    memcpy(set1, set2, len * ZEND_BITSET_ELM_SIZE);
}

static inline void zend_bitset_intersection(zend_bitset set1, zend_bitset set2, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
		set1[i] &= set2[i];
	}
}

static inline void zend_bitset_union(zend_bitset set1, zend_bitset set2, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		set1[i] |= set2[i];
	}
}

static inline void zend_bitset_difference(zend_bitset set1, zend_bitset set2, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		set1[i] = set1[i] & ~set2[i];
	}
}

static inline void zend_bitset_union_with_intersection(zend_bitset set1, zend_bitset set2, zend_bitset set3, zend_bitset set4, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		set1[i] = set2[i] | (set3[i] & set4[i]);
	}
}

static inline void zend_bitset_union_with_difference(zend_bitset set1, zend_bitset set2, zend_bitset set3, zend_bitset set4, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		set1[i] = set2[i] | (set3[i] & ~set4[i]);
	}
}

static inline zend_bool zend_bitset_subset(zend_bitset set1, zend_bitset set2, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		if (set1[i] & ~set2[i]) {
			return 0;
		}
	}
	return 1;
}

static inline int zend_bitset_first(zend_bitset set, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		if (set[i]) {
			return ZEND_BITSET_ELM_SIZE * 8 * i + zend_ulong_ntz(set[i]);
		}
	}
	return -1; /* empty set */
}

static inline int zend_bitset_last(zend_bitset set, uint32_t len)
{
	uint32_t i = len;

	while (i > 0) {
		i--;
		if (set[i]) {
			int j = ZEND_BITSET_ELM_SIZE * 8 * i - 1;
			zend_ulong x = set[i];
			while (x != Z_UL(0)) {
				x = x >> Z_UL(1);
				j++;
			}
			return j;
		}
	}
	return -1; /* empty set */
}

#define ZEND_BITSET_FOREACH(set, len, bit) do { \
	zend_bitset _set = (set); \
	uint32_t _i, _len = (len); \
	for (_i = 0; _i < _len; _i++) { \
		zend_ulong _x = _set[_i]; \
		if (_x) { \
			(bit) = ZEND_BITSET_ELM_SIZE * 8 * _i; \
			for (; _x != 0; _x >>= Z_UL(1), (bit)++) { \
				if (!(_x & Z_UL(1))) continue;

#define ZEND_BITSET_REVERSE_FOREACH(set, len, bit) do { \
	zend_bitset _set = (set); \
	uint32_t _i = (len); \
	zend_ulong _test = Z_UL(1) << (ZEND_BITSET_ELM_SIZE * 8 - 1); \
	while (_i-- > 0) { \
		zend_ulong _x = _set[_i]; \
		if (_x) { \
			(bit) = ZEND_BITSET_ELM_SIZE * 8 * (_i + 1) - 1; \
			for (; _x != 0; _x <<= Z_UL(1), (bit)--) { \
				if (!(_x & _test)) continue; \

#define ZEND_BITSET_FOREACH_END() \
			} \
		} \
	} \
} while (0)

static inline int zend_bitset_pop_first(zend_bitset set, uint32_t len) {
	int i = zend_bitset_first(set, len);
	if (i >= 0) {
		zend_bitset_excl(set, i);
	}
	return i;
}

#endif /* _ZEND_BITSET_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
