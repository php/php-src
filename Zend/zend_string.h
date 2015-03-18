/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id: $ */

#ifndef ZEND_STRING_H
#define ZEND_STRING_H

#include "zend.h"

BEGIN_EXTERN_C()

ZEND_API extern zend_string *(*zend_new_interned_string)(zend_string *str);
ZEND_API extern void (*zend_interned_strings_snapshot)(void);
ZEND_API extern void (*zend_interned_strings_restore)(void);

ZEND_API zend_ulong zend_hash_func(const char *str, size_t len);
void zend_interned_strings_init(void);
void zend_interned_strings_dtor(void);

END_EXTERN_C()

#define IS_INTERNED(s)					(GC_FLAGS(s) & IS_STR_INTERNED)

#define STR_EMPTY_ALLOC()				CG(empty_string)

#define _STR_HEADER_SIZE XtOffsetOf(zend_string, val)

#define STR_ALLOCA_ALLOC(str, _len, use_heap) do { \
	(str) = (zend_string *)do_alloca(ZEND_MM_ALIGNED_SIZE(_STR_HEADER_SIZE + (_len) + 1), (use_heap)); \
	GC_REFCOUNT(str) = 1; \
	(str)->h = 0; \
	(str)->len = (_len); \
} while (0)
#define STR_ALLOCA_INIT(str, s, len, use_heap) do { \
	STR_ALLOCA_ALLOC(str, len, use_heap); \
	memcpy((str)->val, (s), (len)); \
	(str)->val[(len)] = '\0'; \
} while (0)

#define STR_ALLOCA_FREE(str, use_heap) free_alloca(str, use_heap)

static zend_always_inline zend_ulong zend_string_hash_val(zend_string *s)
{
	if (!s->h) {
		s->h = zend_hash_func(s->val, s->len);
	}
	return s->h;
}

static zend_always_inline void zend_string_forget_hash_val(zend_string *s)
{
	s->h = 0;
}

static zend_always_inline uint32_t zend_string_refcount(zend_string *s)
{
	if (!IS_INTERNED(s)) {
		return GC_REFCOUNT(s);
	}
	return 1;
}

static zend_always_inline uint32_t zend_string_addref(zend_string *s)
{
	if (!IS_INTERNED(s)) {
		return ++GC_REFCOUNT(s);
	}
	return 1;
}

static zend_always_inline uint32_t zend_string_delref(zend_string *s)
{
	if (!IS_INTERNED(s)) {
		return --GC_REFCOUNT(s);
	}
	return 1;
}

static zend_always_inline zend_string *zend_string_alloc(size_t len, int persistent)
{
	zend_string *ret = (zend_string *)pemalloc(ZEND_MM_ALIGNED_SIZE(_STR_HEADER_SIZE + len + 1), persistent);

	GC_REFCOUNT(ret) = 1;
#if 1
	/* optimized single assignment */
	GC_TYPE_INFO(ret) = IS_STRING | ((persistent ? IS_STR_PERSISTENT : 0) << 8);
#else
	GC_TYPE(ret) = IS_STRING;
	GC_FLAGS(ret) = (persistent ? IS_STR_PERSISTENT : 0);
	GC_INFO(ret) = 0;
#endif
	ret->h = 0;
	ret->len = len;
	return ret;
}

static zend_always_inline zend_string *zend_string_safe_alloc(size_t n, size_t m, size_t l, int persistent)
{
	zend_string *ret = (zend_string *)safe_pemalloc(n, m, ZEND_MM_ALIGNED_SIZE(_STR_HEADER_SIZE + l + 1), persistent);

	GC_REFCOUNT(ret) = 1;
#if 1
	/* optimized single assignment */
	GC_TYPE_INFO(ret) = IS_STRING | ((persistent ? IS_STR_PERSISTENT : 0) << 8);
#else
	GC_TYPE(ret) = IS_STRING;
	GC_FLAGS(ret) = (persistent ? IS_STR_PERSISTENT : 0);
	GC_INFO(ret) = 0;
#endif
	ret->h = 0;
	ret->len = (n * m) + l;
	return ret;
}

static zend_always_inline zend_string *zend_string_init(const char *str, size_t len, int persistent)
{
	zend_string *ret = zend_string_alloc(len, persistent);

	memcpy(ret->val, str, len);
	ret->val[len] = '\0';
	return ret;
}

static zend_always_inline zend_string *zend_string_copy(zend_string *s)
{
	if (!IS_INTERNED(s)) {
		GC_REFCOUNT(s)++;
	}
	return s;
}

static zend_always_inline zend_string *zend_string_dup(zend_string *s, int persistent)
{
	if (IS_INTERNED(s)) {
		return s;
	} else {
		return zend_string_init(s->val, s->len, persistent);
	}
}

static zend_always_inline zend_string *zend_string_realloc(zend_string *s, size_t len, int persistent)
{
	zend_string *ret;

	if (IS_INTERNED(s)) {
		ret = zend_string_alloc(len, persistent);
		memcpy(ret->val, s->val, (len > s->len ? s->len : len) + 1);
	} else if (EXPECTED(GC_REFCOUNT(s) == 1)) {
		ret = (zend_string *)perealloc(s, ZEND_MM_ALIGNED_SIZE(_STR_HEADER_SIZE + len + 1), persistent);
		ret->len = len;
		zend_string_forget_hash_val(ret);
	} else {
		ret = zend_string_alloc(len, persistent);
		memcpy(ret->val, s->val, (len > s->len ? s->len : len) + 1);
		GC_REFCOUNT(s)--;
	}
	return ret;
}

static zend_always_inline zend_string *zend_string_safe_realloc(zend_string *s, size_t n, size_t m, size_t l, int persistent)
{
	zend_string *ret;

	if (IS_INTERNED(s)) {
		ret = zend_string_safe_alloc(n, m, l, persistent);
		memcpy(ret->val, s->val, ((n * m) + l > (size_t)s->len ? (size_t)s->len : ((n * m) + l)) + 1);
	} else if (GC_REFCOUNT(s) == 1) {
		ret = (zend_string *)safe_perealloc(s, n, m, ZEND_MM_ALIGNED_SIZE(_STR_HEADER_SIZE + l + 1), persistent);
		ret->len = (n * m) + l;
		zend_string_forget_hash_val(ret);
	} else {
		ret = zend_string_safe_alloc(n, m, l, persistent);
		memcpy(ret->val, s->val, ((n * m) + l > (size_t)s->len ? (size_t)s->len : ((n * m) + l)) + 1);
		GC_REFCOUNT(s)--;
	}
	return ret;
}

static zend_always_inline void zend_string_free(zend_string *s)
{
	if (!IS_INTERNED(s)) {
		ZEND_ASSERT(GC_REFCOUNT(s) <= 1);
		pefree(s, GC_FLAGS(s) & IS_STR_PERSISTENT);
	}
}

static zend_always_inline void zend_string_release(zend_string *s)
{
	if (!IS_INTERNED(s)) {
		if (--GC_REFCOUNT(s) == 0) {
			pefree(s, GC_FLAGS(s) & IS_STR_PERSISTENT);
		}
	}
}


static zend_always_inline zend_bool zend_string_equals(zend_string *s1, zend_string *s2)
{
	return s1 == s2 || (s1->len == s2->len && !memcmp(s1->val, s2->val, s1->len));
}

#define zend_string_equals_ci(s1, s2) \
	((s1)->len == (s2)->len && !zend_binary_strcasecmp((s1)->val, (s1)->len, (s2)->val, (s2)->len))

#define zend_string_equals_literal_ci(str, c) \
	((str)->len == sizeof(c) - 1 && !zend_binary_strcasecmp((str)->val, (str)->len, (c), sizeof(c) - 1))

#define zend_string_equals_literal(str, literal) \
	((str)->len == sizeof(literal)-1 && !memcmp((str)->val, literal, sizeof(literal) - 1))

/*
 * DJBX33A (Daniel J. Bernstein, Times 33 with Addition)
 *
 * This is Daniel J. Bernstein's popular `times 33' hash function as
 * posted by him years ago on comp.lang.c. It basically uses a function
 * like ``hash(i) = hash(i-1) * 33 + str[i]''. This is one of the best
 * known hash functions for strings. Because it is both computed very
 * fast and distributes very well.
 *
 * The magic of number 33, i.e. why it works better than many other
 * constants, prime or not, has never been adequately explained by
 * anyone. So I try an explanation: if one experimentally tests all
 * multipliers between 1 and 256 (as RSE did now) one detects that even
 * numbers are not useable at all. The remaining 128 odd numbers
 * (except for the number 1) work more or less all equally well. They
 * all distribute in an acceptable way and this way fill a hash table
 * with an average percent of approx. 86%.
 *
 * If one compares the Chi^2 values of the variants, the number 33 not
 * even has the best value. But the number 33 and a few other equally
 * good numbers like 17, 31, 63, 127 and 129 have nevertheless a great
 * advantage to the remaining numbers in the large set of possible
 * multipliers: their multiply operation can be replaced by a faster
 * operation based on just one shift plus either a single addition
 * or subtraction operation. And because a hash function has to both
 * distribute good _and_ has to be very fast to compute, those few
 * numbers should be preferred and seems to be the reason why Daniel J.
 * Bernstein also preferred it.
 *
 *
 *                  -- Ralf S. Engelschall <rse@engelschall.com>
 */

static zend_always_inline zend_ulong zend_inline_hash_func(const char *str, size_t len)
{
	register zend_ulong hash = Z_UL(5381);

	/* variant with the hash unrolled eight times */
	for (; len >= 8; len -= 8) {
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
	}
	switch (len) {
		case 7: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 6: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 5: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 4: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 3: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 2: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 1: hash = ((hash << 5) + hash) + *str++; break;
		case 0: break;
EMPTY_SWITCH_DEFAULT_CASE()
	}

	return hash;
}

static zend_always_inline void zend_interned_empty_string_init(zend_string **s)
{
	zend_string *str;

	str = zend_string_alloc(sizeof("")-1, 1);
	str->val[0] = '\000';

#ifndef ZTS
	*s = zend_new_interned_string(str);
#else
	zend_string_hash_val(str);
	str->gc.u.v.flags |= IS_STR_INTERNED;
	*s = str;
#endif
}

static zend_always_inline void zend_interned_empty_string_free(zend_string **s)
{
	if (NULL != *s) {
		free(*s);
		*s = NULL;
	}
}

#endif /* ZEND_STRING_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
