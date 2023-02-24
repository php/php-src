/*
	Copyright (c) 2021 Tyson Andre

	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.

	- Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

	- Neither the name of the 'teds' nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
	THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "php.h"
#include "php_ini.h"

#include "Zend/zend_strictmap.h"

#define ZEND_STRICTHASH_HASH_NULL 8310
#define ZEND_STRICTHASH_HASH_FALSE 8311
#define ZEND_STRICTHASH_HASH_TRUE 8312
#define ZEND_STRICTHASH_HASH_EMPTY_ARRAY 8313
/*
 * See https://en.wikipedia.org/wiki/NaN
 * For nan, the 12 most significant bits are:
 * - 1 sign bit (0 or 1)
 * - 11 sign bits
 * (and at least one of the significand bits must be non-zero)
 *
 * Here, 0xff is the most significant byte with the sign and part of the exponent,
 * and 0xf8 is the second most significant byte with part of the exponent and significand.
 *
 * Return an arbitrary choice of 0xff f_, with bytes in the reverse order.
 */
#define ZEND_STRICTHASH_HASH_NAN 0xf8ff

#define ZEND_STRICTHASH_HASH_OFFSET_DOUBLE 8315
#define ZEND_STRICTHASH_HASH_OFFSET_OBJECT 31415926
#define ZEND_STRICTHASH_HASH_OFFSET_RESOURCE 27182818

typedef struct _array_rec_prot_node {
	const zend_array *ht;
	const struct _array_rec_prot_node *prev;
} array_rec_prot_node;

static zend_long zend_stricthash_array(HashTable *const ht, const array_rec_prot_node *const node);
static uint64_t zend_convert_double_to_uint64_t(double value);
static zend_always_inline zend_long zend_stricthash_inner(zval *value, array_rec_prot_node *node);

static zend_always_inline uint64_t zend_inline_hash_of_uint64(uint64_t orig) {
	/* Copied from code written for igbinary. Works best when data that frequently
	 * differs is in the least significant bits of data. */
	uint64_t data = orig * 0x5e2d58d8b3bce8d9;
	return ZEND_BSWAP_64(data);
}

zend_long zend_stricthash_hash(zval *value) {
	uint64_t raw_data = zend_stricthash_inner(value, NULL);
	return zend_inline_hash_of_uint64(raw_data);
}

static zend_always_inline zend_long zend_stricthash_inner(zval *value, array_rec_prot_node *node) {
again:
	switch (Z_TYPE_P(value)) {
		case IS_NULL:
			return ZEND_STRICTHASH_HASH_NULL;
		case IS_FALSE:
			return ZEND_STRICTHASH_HASH_FALSE;
		case IS_TRUE:
			return ZEND_STRICTHASH_HASH_TRUE;
		case IS_LONG:
			return Z_LVAL_P(value);
		case IS_DOUBLE:
			return zend_convert_double_to_uint64_t(Z_DVAL_P(value)) + ZEND_STRICTHASH_HASH_OFFSET_DOUBLE;
		case IS_STRING:
			return ZSTR_HASH(Z_STR_P(value));
		case IS_ARRAY:
			return zend_stricthash_array(Z_ARR_P(value), node);
		case IS_OBJECT:
			return Z_OBJ_HANDLE_P(value) + ZEND_STRICTHASH_HASH_OFFSET_OBJECT;
		case IS_RESOURCE:
			return Z_RES_HANDLE_P(value) + ZEND_STRICTHASH_HASH_OFFSET_RESOURCE;
		case IS_REFERENCE:
			value = Z_REFVAL_P(value);
			goto again;
		case IS_INDIRECT:
			value = Z_INDIRECT_P(value);
			goto again;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

inline static uint64_t zend_convert_double_to_uint64_t(double value) {
	if (value == 0) {
		/* Signed positive and negative 0 have different bits. However, $signedZero === $signedNegativeZero in php and many other languages. */
		return 0;
	}
	if (UNEXPECTED(isnan(value))) {
		return ZEND_STRICTHASH_HASH_NAN;
	}
	uint8_t *data = (uint8_t *)&value;
#ifndef WORDS_BIGENDIAN
	return
		(((uint64_t)data[0]) << 56) |
		(((uint64_t)data[1]) << 48) |
		(((uint64_t)data[2]) << 40) |
		(((uint64_t)data[3]) << 32) |
		(((uint64_t)data[4]) << 24) |
		(((uint64_t)data[5]) << 16) |
		(((uint64_t)data[6]) << 8) |
		(((uint64_t)data[7]));
#else
	return
		(((uint64_t)data[7]) << 56) |
		(((uint64_t)data[6]) << 48) |
		(((uint64_t)data[5]) << 40) |
		(((uint64_t)data[4]) << 32) |
		(((uint64_t)data[3]) << 24) |
		(((uint64_t)data[2]) << 16) |
		(((uint64_t)data[1]) << 8) |
		(((uint64_t)data[0]));
#endif
}

static zend_long zend_stricthash_array(HashTable *const ht, const array_rec_prot_node *const node) {
	if (zend_hash_num_elements(ht) == 0) {
		return ZEND_STRICTHASH_HASH_EMPTY_ARRAY;
	}

	uint64_t result = 1;
	bool protected_recursion = false;

	array_rec_prot_node new_node;
	array_rec_prot_node *new_node_ptr = NULL;
	if (!(GC_FLAGS(ht) & GC_IMMUTABLE)) {
		new_node.prev = node;
		new_node.ht = ht;
		if (UNEXPECTED(GC_IS_RECURSIVE(ht))) {
			for (const array_rec_prot_node *tmp = node; tmp != NULL; tmp = tmp->prev) {
				if (tmp->ht == ht) {
					zend_error_noreturn(E_ERROR, "Nesting level too deep - recursive dependency?");
				}
			}
		} else {
			protected_recursion = true;
			GC_PROTECT_RECURSION(ht);
		}
		new_node_ptr = &new_node;
	}

	zend_long num_key;
	zend_string *str_key;
	zval *field_value;
	ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, str_key, field_value) {
		/* str_key is in a hash table, meaning that the hash was already computed. */
		result += str_key ? ZSTR_H(str_key) : (zend_ulong) num_key;
		zend_long field_hash = zend_stricthash_inner(field_value, new_node_ptr);
		result += (field_hash + (result << 7));
		result = zend_inline_hash_of_uint64(result);
	} ZEND_HASH_FOREACH_END();

	if (protected_recursion) {
		GC_UNPROTECT_RECURSION(ht);
	}
	return result;
}
