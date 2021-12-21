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

#ifndef ZEND_STRICTMAP_H
#define ZEND_STRICTMAP_H

#include "Zend/zend_types.h"

#define ZEND_STRICTMAP_FOREACH(_stricthashmap) do { \
	const zend_strictmap *const __stricthashmap = (_stricthashmap); \
	zend_strictmap_entry *_p = __stricthashmap->data + __stricthashmap->first_used_index; \
	zend_strictmap_entry *const _end = __stricthashmap->data + __stricthashmap->used_indices; \
	ZEND_ASSERT(__stricthashmap->first_used_index <= __stricthashmap->used_indices); \
	for (; _p != _end; _p++) { \
		zval *_key = &_p->key; \
		if (Z_TYPE_P(_key) == IS_UNDEF) { continue; }

#define ZEND_STRICTMAP_FOREACH_KEY_VAL(stricthashmap, k, v) ZEND_STRICTMAP_FOREACH(stricthashmap) \
		k = _key; \
		v = &_p->value;

#define ZEND_STRICTMAP_FOREACH_KEY(stricthashmap, k) ZEND_STRICTMAP_FOREACH(stricthashmap) \
		k = _key;

#define ZEND_STRICTMAP_FOREACH_BUCKET(stricthashmap, b) ZEND_STRICTMAP_FOREACH(stricthashmap) \
		b = _p;

#define ZEND_STRICTMAP_FOREACH_END() \
	} \
} while (0)

typedef struct _zend_strictmap_entry {
	zval key;
	zval value;
} zend_strictmap_entry;

/* See Zend/zend_types.h for the zend_array layout this is based on. */
typedef struct _zend_strictmap {
	zend_strictmap_entry *data;
	uint32_t count; /* Number of elements.  */
	uint32_t capacity; /* Power of 2 size. */
	uint32_t used_indices; /* Number of buckets used, including gaps left by remove. */
	uint32_t hash_mask; /* -capacity or ZEND_STRICTMAP_MIN_MASK, e.g. 0xfffffff0 for an array of size 8 with 16 buckets. */
	uint32_t first_used_index; /* The offset of the first bucket used. */
	uint32_t free_index_chain; /* Chain of freed indicies that can be reused. */
} zend_strictmap;

void zend_strictmap_dtor(zend_strictmap *array);
void zend_strictmap_init(zend_strictmap *array);
bool zend_strictmap_insert(zend_strictmap *array, zval *key, zval *value);
zend_strictmap_entry *zend_strictmap_find_value(const zend_strictmap *array, zval *value);
// Unused and untested
//bool zend_strictmap_remove_key(zend_strictmap *array, zval *key);

#endif	/* ZEND_STRICTMAP_H */
