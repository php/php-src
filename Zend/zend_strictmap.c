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
#include "Zend/zend_stricthash.h"

#define SM_MIN_CAPACITY 8
#define SM_IT_HASH(entry) Z_EXTRA((entry)->key)
#define SM_IT_NEXT(entry) Z_NEXT((entry)->value)
#define SM_SIZE_TO_MASK(capacity) ((uint32_t)(-(capacity)));
#define SM_MEMORY_PER_ENTRY (sizeof(zend_strictmap_entry) + sizeof(uint32_t))
#define SM_INVALID_INDEX ((uint32_t) -1)
#define SM_HASH_EX(data, idx) ((uint32_t*)(data))[(int32_t)(idx)]
#define SM_HASH(ht, idx) SM_HASH_EX((ht)->data, idx)

static size_t zend_strictmap_offset_bytes_for_capacity(uint32_t capacity);
static void zend_strictmap_grow(zend_strictmap *array);
static zend_strictmap_entry *zend_strictmap_find_entry(const zend_strictmap *ht, zval *key, const uint32_t h);
static zend_strictmap_entry *zend_strictmap_alloc_entries(uint32_t capacity);

void zend_strictmap_init(zend_strictmap *array)
{
	array->count = 0;
	array->used_indices = 0;
	array->first_used_index = 0;
	array->data = zend_strictmap_alloc_entries(SM_MIN_CAPACITY);
	array->capacity = SM_MIN_CAPACITY;
	array->hash_mask = SM_SIZE_TO_MASK(SM_MIN_CAPACITY);
	array->free_index_chain = SM_INVALID_INDEX;
}

void zend_strictmap_dtor(zend_strictmap *array)
{
	zend_strictmap_entry *entries = array->data;
	uint32_t capacity = array->capacity;
	uint8_t *data = ((uint8_t *)entries) - zend_strictmap_offset_bytes_for_capacity(capacity);
	efree(data);
}

/* Returns true if a new entry was added to the map, false if updated. Based on _zend_hash_add_or_update_i. */
bool zend_strictmap_insert(zend_strictmap *array, zval *key, zval *value)
{
	ZEND_ASSERT(Z_TYPE_P(key) != IS_UNDEF);
	ZEND_ASSERT(Z_TYPE_P(value) != IS_UNDEF);

	const uint32_t h = zend_stricthash_hash_uint32_t(key);

	zend_strictmap_entry *p = zend_strictmap_find_entry(array, key, h);
	if (p) {
		ZVAL_COPY_VALUE(&p->value, value);
		return false;
	}

	/* If the Hash table is full, resize it */
	if (UNEXPECTED(array->free_index_chain == SM_INVALID_INDEX && array->used_indices >= array->capacity)) {
		zend_strictmap_grow(array);
	}

	zend_strictmap_entry *const data = array->data;
	uint32_t idx;
	if (array->free_index_chain == SM_INVALID_INDEX) {
		idx = array->used_indices;
		array->used_indices++;
	} else {
		idx = array->free_index_chain;
		zend_strictmap_entry *const new_entry = data + idx;
		array->free_index_chain = SM_IT_NEXT(new_entry);
	}
	p = data + idx;
	const uint32_t nIndex = h | array->hash_mask;
	array->count++;
	SM_IT_HASH(p) = h;
	SM_IT_NEXT(p) = SM_HASH_EX(data, nIndex);
	SM_HASH_EX(data, nIndex) = idx;

	ZVAL_COPY_VALUE(&p->key, key);
	ZVAL_COPY_VALUE(&p->value, value);

	return true;
}

// Unused and untested
// bool zend_strictmap_remove_key(zend_strictmap *array, zval *key)
// {
// 	if (array->count == 0) {
// 		return false;
// 	}
// 	zend_strictmap_entry *const entries = array->data;
// 	const uint32_t h = zend_stricthash_hash_uint32_t(key);
// 	zend_strictmap_entry *entry = zend_strictmap_find_entry(array, key, h);
// 	if (!entry) {
// 		return false;
// 	}
// 	ZEND_ASSERT(SM_IT_HASH(entry) == h);
//
// 	const uint32_t nIndex = h | array->hash_mask;
// 	uint32_t i = SM_HASH(array, nIndex);
//
// 	const uint32_t idx = entry - entries;
// 	if (i == idx) {
// 		SM_HASH(array, nIndex) = SM_IT_NEXT(entry);
// 	} else {
// 		zend_strictmap_entry *prev = &array->data[i];
// 		while (SM_IT_NEXT(prev) != idx) {
// 			i = SM_IT_NEXT(prev);
// 			prev = &array->data[i];
// 		}
// 		SM_IT_NEXT(prev) = SM_IT_NEXT(entry);
// 	}
//
// 	array->count--;
//
// 	SM_IT_NEXT(entry) = array->free_index_chain;
// 	array->free_index_chain = idx;
//
// 	if (array->first_used_index == idx) {
// 		if (array->count == 0) {
// 			ZEND_ASSERT(array->used_indices == 0);
// 			array->first_used_index = 0;
// 		} else {
// 			ZEND_ASSERT(array->first_used_index < array->used_indices);
// 			do {
// 				array->first_used_index++;
// 			} while (array->first_used_index < array->used_indices && (UNEXPECTED(Z_ISUNDEF(array->data[array->first_used_index].key))));
// 			ZEND_ASSERT(array->first_used_index < array->used_indices);
// 		}
// 	}
// 	zval old_key;
// 	zval old_value;
// 	ZVAL_COPY_VALUE(&old_key, &entry->key);
// 	ZVAL_COPY_VALUE(&old_value, &entry->value);
// 	ZEND_ASSERT(array->count <= array->used_indices);
// 	ZEND_ASSERT(array->first_used_index < array->used_indices || (array->first_used_index == array->used_indices && array->count == 0));
// 	ZVAL_UNDEF(&entry->key);
// 	ZVAL_UNDEF(&entry->value);
//
// 	return true;
// }

static size_t zend_strictmap_offset_bytes_for_capacity(uint32_t capacity) {
	return capacity * sizeof(uint32_t);
}

static zend_strictmap_entry *zend_strictmap_alloc_entries(uint32_t capacity) {
	uint8_t *ptr = safe_emalloc(capacity, SM_MEMORY_PER_ENTRY, 0);
	const size_t buckets_byte_count = zend_strictmap_offset_bytes_for_capacity(capacity);
	memset(ptr, SM_INVALID_INDEX, buckets_byte_count);
	return (void *)(ptr + buckets_byte_count);
}

static void zend_strictmap_free_entries(zend_strictmap_entry *old_entries, uint32_t old_capacity) {
	void * old_ptr = ((uint8_t *) old_entries) - zend_strictmap_offset_bytes_for_capacity(old_capacity);
	efree(old_ptr);
}

static void zend_strictmap_grow(zend_strictmap *array)
{
	ZEND_ASSERT(array->used_indices >= array->count);
	if (UNEXPECTED(array->capacity >= HT_MAX_SIZE)) {
		zend_error_noreturn(E_ERROR, "exceeded max valid strictmap capacity");
	}

	const uint32_t new_capacity = array->capacity * 2;
	zend_strictmap_entry *const orig_entries = array->data;
	zend_strictmap_entry *const new_entries = zend_strictmap_alloc_entries(new_capacity);
	zend_strictmap_entry *old_entry;
	zend_strictmap_entry *it = new_entries;
	ZEND_ASSERT(array->count <= array->used_indices);
	uint32_t i = 0;
	const uint32_t new_mask = SM_SIZE_TO_MASK(new_capacity);
	ZEND_STRICTMAP_FOREACH_BUCKET(array, old_entry) {
		const uint32_t h = SM_IT_HASH(old_entry);
		const uint32_t nIndex = h | new_mask;
		ZVAL_COPY_VALUE(&it->key, &old_entry->key);
		SM_IT_NEXT(it) = SM_HASH_EX(new_entries, nIndex);
		ZVAL_COPY_VALUE(&it->value, &old_entry->value);
		SM_IT_HASH(it) = h;
		SM_HASH_EX(new_entries, nIndex) = i;
		it++;
		i++;
	} ZEND_STRICTMAP_FOREACH_END();
	ZEND_ASSERT((size_t)(it - new_entries) == array->count);
	zend_strictmap_free_entries(orig_entries, array->capacity);

	array->data = new_entries;
	array->used_indices = array->count;
	array->capacity = new_capacity;
	array->hash_mask = new_mask;
	array->first_used_index = 0;
	array->free_index_chain = SM_INVALID_INDEX;
}

static zend_strictmap_entry *zend_strictmap_find_entry(const zend_strictmap *ht, zval *key, const uint32_t h)
{
	zend_strictmap_entry *p;

	zend_strictmap_entry *const data = ht->data;
	const uint32_t nIndex = h | ht->hash_mask;
	uint32_t idx = SM_HASH_EX(data, nIndex);
	while (idx != SM_INVALID_INDEX) {
		ZEND_ASSERT(idx < ht->capacity);
		p = data + idx;
		if (SM_IT_HASH(p) == h && zend_is_identical(&p->key, key)) {
			return p;
		}
		idx = SM_IT_NEXT(p);
	}
	return NULL;
}
