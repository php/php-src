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
   | Author: Tyson Andre <tandre@php.net>                                 |
   +----------------------------------------------------------------------+
*/
#ifndef COLLECTIONS_UTIL_H
#define COLLECTIONS_UTIL_H

#include "Zend/zend.h"

#define COLLECTIONS_MAX_ZVAL_COLLECTION_SIZE HT_MAX_SIZE
static zend_always_inline uint32_t collections_next_pow2_capacity_uint32(uint32_t nSize, uint32_t min) {
	if (nSize < min) {
		return min;
	}
	/* Note that for values such as 63 or 31 of the form ((2^n) - 1),
	 * subtracting and xor are the same things for numbers in the range of 0 to the max. */
#ifdef ZEND_WIN32
	unsigned long index;
	if (BitScanReverse(&index, nSize - 1)) {
		return 0x2u << ((31 - index) ^ 0x1f);
	}
	/* nSize is ensured to be in the valid range, fall back to it
	 * rather than using an undefined bit scan result. */
	return nSize;
#elif (defined(__GNUC__) || __has_builtin(__builtin_clz))  && defined(PHP_HAVE_BUILTIN_CLZ)
	return 0x2u << (__builtin_clz(nSize - 1) ^ 0x1f);
#endif
	nSize -= 1;
	nSize |= (nSize >> 1);
	nSize |= (nSize >> 2);
	nSize |= (nSize >> 4);
	nSize |= (nSize >> 8);
	nSize |= (nSize >> 16);
	return nSize + 1;
}

static zend_always_inline size_t collections_next_pow2_capacity(size_t nSize, size_t min) {
#if SIZEOF_SIZE_T <= 4
	return collections_next_pow2_capacity_uint32(nSize, min);
#else
	if (nSize < min) {
		return min;
	}
	/* Note that for values such as 63 or 31 of the form ((2^n) - 1),
	 * subtracting and xor are the same things for numbers in the range of 0 to the max. */
#ifdef ZEND_WIN32
	unsigned long index;
	if (BitScanReverse64(&index, nSize - 1)) {
		return 0x2u << ((63 - index) ^ 0x3f);
	}
	/* nSize is ensured to be in the valid range, fall back to it
	 * rather than using an undefined bit scan result. */
	return nSize;
#elif (defined(__GNUC__) || __has_builtin(__builtin_clz))  && defined(PHP_HAVE_BUILTIN_CLZ)
#if SIZEOF_SIZE_T > SIZEOF_INT
	return 0x2u << (__builtin_clzl(nSize - 1) ^ (sizeof(long) * 8 - 1));
#else
	return 0x2u << (__builtin_clz(nSize - 1) ^ 0x1f);
#endif
#else
	nSize -= 1;
	nSize |= (nSize >> 1);
	nSize |= (nSize >> 2);
	nSize |= (nSize >> 4);
	nSize |= (nSize >> 8);
	nSize |= (nSize >> 16);
	nSize |= (nSize >> 32);
	return nSize + 1;
#endif
#endif
}

/**
 * Returns absence of zvals or hash table to garbage collect.
 * (e.g. for collections that are immutable or made of scalars instead of zvals)
 */
HashTable* collections_noop_get_gc(zend_object *obj, zval **table, int *n);
/**
 * Returns the immutable empty array in a get_properties handler.
 * This is useful to keep memory low when a datastructure is guaranteed to be free of cycles (e.g. only scalars, or empty)
 */
HashTable* collections_noop_empty_array_get_properties_for(zend_object *obj, zend_prop_purpose purpose);

HashTable *collections_internaliterator_get_gc(zend_object_iterator *iter, zval **table, int *n);

#endif
