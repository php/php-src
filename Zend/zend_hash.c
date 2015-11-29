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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"

#define HT_DEBUG 0
#if HT_DEBUG
# define HT_ASSERT(c) ZEND_ASSERT(c)
#else
# define HT_ASSERT(c)
#endif

#if ZEND_DEBUG
/*
#define HASH_MASK_CONSISTENCY	0xc0
*/
#define HT_OK					0x00
#define HT_IS_DESTROYING		0x40
#define HT_DESTROYED			0x80
#define HT_CLEANING				0xc0

static void _zend_is_inconsistent(const HashTable *ht, const char *file, int line)
{
	if ((ht->u.flags & HASH_MASK_CONSISTENCY) == HT_OK) {
		return;
	}
	switch ((ht->u.flags & HASH_MASK_CONSISTENCY)) {
		case HT_IS_DESTROYING:
			zend_output_debug_string(1, "%s(%d) : ht=%p is being destroyed", file, line, ht);
			break;
		case HT_DESTROYED:
			zend_output_debug_string(1, "%s(%d) : ht=%p is already destroyed", file, line, ht);
			break;
		case HT_CLEANING:
			zend_output_debug_string(1, "%s(%d) : ht=%p is being cleaned", file, line, ht);
			break;
		default:
			zend_output_debug_string(1, "%s(%d) : ht=%p is inconsistent", file, line, ht);
			break;
	}
	zend_bailout();
}
#define IS_CONSISTENT(a) _zend_is_inconsistent(a, __FILE__, __LINE__);
#define SET_INCONSISTENT(n) do { \
		(ht)->u.flags |= n; \
	} while (0)
#else
#define IS_CONSISTENT(a)
#define SET_INCONSISTENT(n)
#endif

#define HASH_PROTECT_RECURSION(ht)														\
	if ((ht)->u.flags & HASH_FLAG_APPLY_PROTECTION) {									\
		if (((ht)->u.flags & ZEND_HASH_APPLY_COUNT_MASK) >= (3 << 8)) {												\
			zend_error_noreturn(E_ERROR, "Nesting level too deep - recursive dependency?");\
		}																				\
		ZEND_HASH_INC_APPLY_COUNT(ht);													\
	}

#define HASH_UNPROTECT_RECURSION(ht)													\
	if ((ht)->u.flags & HASH_FLAG_APPLY_PROTECTION) {									\
		ZEND_HASH_DEC_APPLY_COUNT(ht);													\
	}

#define ZEND_HASH_IF_FULL_DO_RESIZE(ht)				\
	if ((ht)->nNumUsed >= (ht)->nTableSize) {		\
		zend_hash_do_resize(ht);					\
	}

static void ZEND_FASTCALL zend_hash_do_resize(HashTable *ht);

static uint32_t zend_always_inline zend_hash_check_size(uint32_t nSize)
{
#if defined(ZEND_WIN32)
	unsigned long index;
#endif

	/* Use big enough power of 2 */
	/* size should be between HT_MIN_SIZE and HT_MAX_SIZE */
	if (nSize < HT_MIN_SIZE) {
		nSize = HT_MIN_SIZE;
	} else if (UNEXPECTED(nSize >= HT_MAX_SIZE)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nSize, sizeof(Bucket), sizeof(Bucket));
	}

#if defined(ZEND_WIN32)
	if (BitScanReverse(&index, nSize - 1)) {
		return 0x2 << ((31 - index) ^ 0x1f);
	} else {
		/* nSize is ensured to be in the valid range, fall back to it
		   rather than using an undefined bis scan result. */
		return nSize;
	}
#elif defined(__GNUC__) || __has_builtin(__builtin_clz)
	return 0x2 << (__builtin_clz(nSize - 1) ^ 0x1f);
#else
	nSize -= 1;
	nSize |= (nSize >> 1);
	nSize |= (nSize >> 2);
	nSize |= (nSize >> 4);
	nSize |= (nSize >> 8);
	nSize |= (nSize >> 16);
	return nSize + 1;
#endif
}

static void zend_always_inline zend_hash_real_init_ex(HashTable *ht, int packed)
{
	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	ZEND_ASSERT(!((ht)->u.flags & HASH_FLAG_INITIALIZED));
	if (packed) {
		HT_SET_DATA_ADDR(ht, pemalloc(HT_SIZE(ht), (ht)->u.flags & HASH_FLAG_PERSISTENT));
		(ht)->u.flags |= HASH_FLAG_INITIALIZED | HASH_FLAG_PACKED;
		HT_HASH_RESET_PACKED(ht);
	} else {
		(ht)->nTableMask = -(ht)->nTableSize;
		HT_SET_DATA_ADDR(ht, pemalloc(HT_SIZE(ht), (ht)->u.flags & HASH_FLAG_PERSISTENT));
		(ht)->u.flags |= HASH_FLAG_INITIALIZED;
		if (EXPECTED(ht->nTableMask == -8)) {
			Bucket *arData = ht->arData;

			HT_HASH_EX(arData, -8) = -1;
			HT_HASH_EX(arData, -7) = -1;
			HT_HASH_EX(arData, -6) = -1;
			HT_HASH_EX(arData, -5) = -1;
			HT_HASH_EX(arData, -4) = -1;
			HT_HASH_EX(arData, -3) = -1;
			HT_HASH_EX(arData, -2) = -1;
			HT_HASH_EX(arData, -1) = -1;
		} else {
			HT_HASH_RESET(ht);
		}
	}
}

static void zend_always_inline zend_hash_check_init(HashTable *ht, int packed)
{
	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	if (UNEXPECTED(!((ht)->u.flags & HASH_FLAG_INITIALIZED))) {
		zend_hash_real_init_ex(ht, packed);
	}
}

#define CHECK_INIT(ht, packed) \
	zend_hash_check_init(ht, packed)

static const uint32_t uninitialized_bucket[-HT_MIN_MASK] =
	{HT_INVALID_IDX, HT_INVALID_IDX};

ZEND_API void ZEND_FASTCALL _zend_hash_init(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, zend_bool persistent ZEND_FILE_LINE_DC)
{
	GC_REFCOUNT(ht) = 1;
	GC_TYPE_INFO(ht) = IS_ARRAY;
	ht->u.flags = (persistent ? HASH_FLAG_PERSISTENT : 0) | HASH_FLAG_APPLY_PROTECTION | HASH_FLAG_STATIC_KEYS;
	ht->nTableSize = zend_hash_check_size(nSize);
	ht->nTableMask = HT_MIN_MASK;
	HT_SET_DATA_ADDR(ht, &uninitialized_bucket);
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nInternalPointer = HT_INVALID_IDX;
	ht->nNextFreeElement = 0;
	ht->pDestructor = pDestructor;
}

static void ZEND_FASTCALL zend_hash_packed_grow(HashTable *ht)
{
	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	if (ht->nTableSize >= HT_MAX_SIZE) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", ht->nTableSize * 2, sizeof(Bucket), sizeof(Bucket));
	}
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->nTableSize += ht->nTableSize;
	HT_SET_DATA_ADDR(ht, perealloc2(HT_GET_DATA_ADDR(ht), HT_SIZE(ht), HT_USED_SIZE(ht), ht->u.flags & HASH_FLAG_PERSISTENT));
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void ZEND_FASTCALL zend_hash_real_init(HashTable *ht, zend_bool packed)
{
	IS_CONSISTENT(ht);

	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	zend_hash_real_init_ex(ht, packed);
}

ZEND_API void ZEND_FASTCALL zend_hash_packed_to_hash(HashTable *ht)
{
	void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
	Bucket *old_buckets = ht->arData;

	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->u.flags &= ~HASH_FLAG_PACKED;
	new_data = pemalloc(HT_SIZE_EX(ht->nTableSize, -ht->nTableSize), (ht)->u.flags & HASH_FLAG_PERSISTENT);
	ht->nTableMask = -ht->nTableSize;
	HT_SET_DATA_ADDR(ht, new_data);
	memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
	pefree(old_data, (ht)->u.flags & HASH_FLAG_PERSISTENT);
	zend_hash_rehash(ht);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void ZEND_FASTCALL zend_hash_to_packed(HashTable *ht)
{
	void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
	Bucket *old_buckets = ht->arData;

	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	HANDLE_BLOCK_INTERRUPTIONS();
	new_data = pemalloc(HT_SIZE_EX(ht->nTableSize, HT_MIN_MASK), (ht)->u.flags & HASH_FLAG_PERSISTENT);
	ht->u.flags |= HASH_FLAG_PACKED | HASH_FLAG_STATIC_KEYS;
	ht->nTableMask = HT_MIN_MASK;
	HT_SET_DATA_ADDR(ht, new_data);
	HT_HASH_RESET_PACKED(ht);
	memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
	pefree(old_data, (ht)->u.flags & HASH_FLAG_PERSISTENT);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void ZEND_FASTCALL _zend_hash_init_ex(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC)
{
	_zend_hash_init(ht, nSize, pDestructor, persistent ZEND_FILE_LINE_RELAY_CC);
	if (!bApplyProtection) {
		ht->u.flags &= ~HASH_FLAG_APPLY_PROTECTION;
	}
}

ZEND_API void ZEND_FASTCALL zend_hash_extend(HashTable *ht, uint32_t nSize, zend_bool packed)
{
	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	if (nSize == 0) return;
	if (UNEXPECTED(!((ht)->u.flags & HASH_FLAG_INITIALIZED))) {
		if (nSize > ht->nTableSize) {
			ht->nTableSize = zend_hash_check_size(nSize);
		}
		zend_hash_check_init(ht, packed);
	} else {
		if (packed) {
			ZEND_ASSERT(ht->u.flags & HASH_FLAG_PACKED);
			if (nSize > ht->nTableSize) {
				HANDLE_BLOCK_INTERRUPTIONS();
				ht->nTableSize = zend_hash_check_size(nSize);
				HT_SET_DATA_ADDR(ht, perealloc2(HT_GET_DATA_ADDR(ht), HT_SIZE(ht), HT_USED_SIZE(ht), ht->u.flags & HASH_FLAG_PERSISTENT));
				HANDLE_UNBLOCK_INTERRUPTIONS();
			}
		} else {
			ZEND_ASSERT(!(ht->u.flags & HASH_FLAG_PACKED));
			if (nSize > ht->nTableSize) {
				void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
				Bucket *old_buckets = ht->arData;
				nSize = zend_hash_check_size(nSize);
				HANDLE_BLOCK_INTERRUPTIONS();
				new_data = pemalloc(HT_SIZE_EX(nSize, -nSize), ht->u.flags & HASH_FLAG_PERSISTENT);
				ht->nTableSize = nSize;
				ht->nTableMask = -ht->nTableSize;
				HT_SET_DATA_ADDR(ht, new_data);
				memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
				pefree(old_data, ht->u.flags & HASH_FLAG_PERSISTENT);
				zend_hash_rehash(ht);
				HANDLE_UNBLOCK_INTERRUPTIONS();
			}
		}
	}
}

static uint32_t zend_array_recalc_elements(HashTable *ht)
{
       zval *val;
       uint32_t num = ht->nNumOfElements;

	   ZEND_HASH_FOREACH_VAL(ht, val) {
		   if (Z_TYPE_P(val) == IS_UNDEF) continue;
		   if (Z_TYPE_P(val) == IS_INDIRECT) {
			   if (UNEXPECTED(Z_TYPE_P(Z_INDIRECT_P(val)) == IS_UNDEF)) {
				   num--;
			   }
		   }
       } ZEND_HASH_FOREACH_END();
       return num;
}
/* }}} */

ZEND_API uint32_t zend_array_count(HashTable *ht)
{
	uint32_t num;
	if (UNEXPECTED(ht->u.v.flags & HASH_FLAG_HAS_EMPTY_IND)) {
		num = zend_array_recalc_elements(ht);
		if (UNEXPECTED(ht->nNumOfElements == num)) {
			ht->u.v.flags &= ~HASH_FLAG_HAS_EMPTY_IND;
		}
	} else if (UNEXPECTED(ht == &EG(symbol_table))) {
		num = zend_array_recalc_elements(ht);
	} else {
		num = zend_hash_num_elements(ht);
	}
	return num;
}
/* }}} */

ZEND_API void ZEND_FASTCALL zend_hash_set_apply_protection(HashTable *ht, zend_bool bApplyProtection)
{
	if (bApplyProtection) {
		ht->u.flags |= HASH_FLAG_APPLY_PROTECTION;
	} else {
		ht->u.flags &= ~HASH_FLAG_APPLY_PROTECTION;
	}
}

ZEND_API uint32_t ZEND_FASTCALL zend_hash_iterator_add(HashTable *ht, HashPosition pos)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_count);
	uint32_t idx;

	if (EXPECTED(ht->u.v.nIteratorsCount != 255)) {
		ht->u.v.nIteratorsCount++;
	}
	while (iter != end) {
		if (iter->ht == NULL) {
			iter->ht = ht;
			iter->pos = pos;
			idx = iter - EG(ht_iterators);
			if (idx + 1 > EG(ht_iterators_used)) {
				EG(ht_iterators_used) = idx + 1;
			}
			return idx;
		}
		iter++;
	}
	if (EG(ht_iterators) == EG(ht_iterators_slots)) {
		EG(ht_iterators) = emalloc(sizeof(HashTableIterator) * (EG(ht_iterators_count) + 8));
		memcpy(EG(ht_iterators), EG(ht_iterators_slots), sizeof(HashTableIterator) * EG(ht_iterators_count));
	} else {
		EG(ht_iterators) = erealloc(EG(ht_iterators), sizeof(HashTableIterator) * (EG(ht_iterators_count) + 8));
	}
	iter = EG(ht_iterators) + EG(ht_iterators_count);
	EG(ht_iterators_count) += 8;
	iter->ht = ht;
	iter->pos = pos;
	memset(iter + 1, 0, sizeof(HashTableIterator) * 7);
	idx = iter - EG(ht_iterators);
	EG(ht_iterators_used) = idx + 1;
	return idx;
}

ZEND_API HashPosition ZEND_FASTCALL zend_hash_iterator_pos(uint32_t idx, HashTable *ht)
{
	HashTableIterator *iter = EG(ht_iterators) + idx;

	ZEND_ASSERT(idx != (uint32_t)-1);
	if (iter->pos == HT_INVALID_IDX) {
		return HT_INVALID_IDX;
	} else if (UNEXPECTED(iter->ht != ht)) {
		if (EXPECTED(iter->ht) && EXPECTED(iter->ht->u.v.nIteratorsCount != 255)) {
			iter->ht->u.v.nIteratorsCount--;
		}
		if (EXPECTED(ht->u.v.nIteratorsCount != 255)) {
			ht->u.v.nIteratorsCount++;
		}
		iter->ht = ht;
		iter->pos = ht->nInternalPointer;
	}
	return iter->pos;
}

ZEND_API void ZEND_FASTCALL zend_hash_iterator_del(uint32_t idx)
{
	HashTableIterator *iter = EG(ht_iterators) + idx;

	ZEND_ASSERT(idx != (uint32_t)-1);

	if (EXPECTED(iter->ht) && EXPECTED(iter->ht->u.v.nIteratorsCount != 255)) {
		iter->ht->u.v.nIteratorsCount--;
	}
	iter->ht = NULL;

	if (idx == EG(ht_iterators_used) - 1) {
		while (idx > 0 && EG(ht_iterators)[idx - 1].ht == NULL) {
			idx--;
		}
		EG(ht_iterators_used) = idx;
	}
}

static zend_never_inline void ZEND_FASTCALL _zend_hash_iterators_remove(HashTable *ht)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);
	uint32_t idx;

	while (iter != end) {
		if (iter->ht == ht) {
			iter->ht = NULL;
		}
		iter++;
	}

	idx = EG(ht_iterators_used);
	while (idx > 0 && EG(ht_iterators)[idx - 1].ht == NULL) {
		idx--;
	}
	EG(ht_iterators_used) = idx;
}

static zend_always_inline void zend_hash_iterators_remove(HashTable *ht)
{
	if (UNEXPECTED(ht->u.v.nIteratorsCount)) {
		_zend_hash_iterators_remove(ht);
	}
}

ZEND_API HashPosition ZEND_FASTCALL zend_hash_iterators_lower_pos(HashTable *ht, HashPosition start)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);
	HashPosition res = HT_INVALID_IDX;

	while (iter != end) {
		if (iter->ht == ht) {
			if (iter->pos >= start && iter->pos < res) {
				res = iter->pos;
			}
		}
		iter++;
	}
	return res;
}

ZEND_API void ZEND_FASTCALL _zend_hash_iterators_update(HashTable *ht, HashPosition from, HashPosition to)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);

	while (iter != end) {
		if (iter->ht == ht && iter->pos == from) {
			iter->pos = to;
		}
		iter++;
	}
}

static zend_always_inline Bucket *zend_hash_find_bucket(const HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p, *arData;

	h = zend_string_hash_val(key);
	arData = ht->arData;
	nIndex = h | ht->nTableMask;
	idx = HT_HASH_EX(arData, nIndex);
	while (EXPECTED(idx != HT_INVALID_IDX)) {
		p = HT_HASH_TO_BUCKET_EX(arData, idx);
		if (EXPECTED(p->key == key)) { /* check for the same interned string */
			return p;
		} else if (EXPECTED(p->h == h) &&
		     EXPECTED(p->key) &&
		     EXPECTED(ZSTR_LEN(p->key) == ZSTR_LEN(key)) &&
		     EXPECTED(memcmp(ZSTR_VAL(p->key), ZSTR_VAL(key), ZSTR_LEN(key)) == 0)) {
			return p;
		}
		idx = Z_NEXT(p->val);
	}
	return NULL;
}

static zend_always_inline Bucket *zend_hash_str_find_bucket(const HashTable *ht, const char *str, size_t len, zend_ulong h)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p, *arData;

	arData = ht->arData;
	nIndex = h | ht->nTableMask;
	idx = HT_HASH_EX(arData, nIndex);
	while (idx != HT_INVALID_IDX) {
		ZEND_ASSERT(idx < HT_IDX_TO_HASH(ht->nTableSize));
		p = HT_HASH_TO_BUCKET_EX(arData, idx);
		if ((p->h == h)
			 && p->key
			 && (ZSTR_LEN(p->key) == len)
			 && !memcmp(ZSTR_VAL(p->key), str, len)) {
			return p;
		}
		idx = Z_NEXT(p->val);
	}
	return NULL;
}

static zend_always_inline Bucket *zend_hash_index_find_bucket(const HashTable *ht, zend_ulong h)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p, *arData;

	arData = ht->arData;
	nIndex = h | ht->nTableMask;
	idx = HT_HASH_EX(arData, nIndex);
	while (idx != HT_INVALID_IDX) {
		ZEND_ASSERT(idx < HT_IDX_TO_HASH(ht->nTableSize));
		p = HT_HASH_TO_BUCKET_EX(arData, idx);
		if (p->h == h && !p->key) {
			return p;
		}
		idx = Z_NEXT(p->val);
	}
	return NULL;
}

static zend_always_inline zval *_zend_hash_add_or_update_i(HashTable *ht, zend_string *key, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (UNEXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		CHECK_INIT(ht, 0);
		goto add_to_hash;
	} else if (ht->u.flags & HASH_FLAG_PACKED) {
		zend_hash_packed_to_hash(ht);
	} else if ((flag & HASH_ADD_NEW) == 0) {
		p = zend_hash_find_bucket(ht, key);

		if (p) {
			zval *data;

			if (flag & HASH_ADD) {
				return NULL;
			}
			ZEND_ASSERT(&p->val != pData);
			data = &p->val;
			if ((flag & HASH_UPDATE_INDIRECT) && Z_TYPE_P(data) == IS_INDIRECT) {
				data = Z_INDIRECT_P(data);
			}
			HANDLE_BLOCK_INTERRUPTIONS();
			if (ht->pDestructor) {
				ht->pDestructor(data);
			}
			ZVAL_COPY_VALUE(data, pData);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			return data;
		}
	}

	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */

add_to_hash:
	HANDLE_BLOCK_INTERRUPTIONS();
	idx = ht->nNumUsed++;
	ht->nNumOfElements++;
	if (ht->nInternalPointer == HT_INVALID_IDX) {
		ht->nInternalPointer = idx;
	}
	zend_hash_iterators_update(ht, HT_INVALID_IDX, idx);
	p = ht->arData + idx;
	p->key = key;
	if (!ZSTR_IS_INTERNED(key)) {
		zend_string_addref(key);
		ht->u.flags &= ~HASH_FLAG_STATIC_KEYS;
		zend_string_hash_val(key);
	}
	p->h = h = ZSTR_H(key);
	ZVAL_COPY_VALUE(&p->val, pData);
	nIndex = h | ht->nTableMask;
	Z_NEXT(p->val) = HT_HASH(ht, nIndex);
	HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(idx);
	HANDLE_UNBLOCK_INTERRUPTIONS();

	return &p->val;
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_add_or_update(HashTable *ht, zend_string *key, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, flag ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_add(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_update(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_update_ind(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE | HASH_UPDATE_INDIRECT ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_add_new(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD_NEW ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_str_add_or_update(HashTable *ht, const char *str, size_t len, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, flag ZEND_FILE_LINE_RELAY_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_str_update(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE ZEND_FILE_LINE_RELAY_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_str_update_ind(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE | HASH_UPDATE_INDIRECT ZEND_FILE_LINE_RELAY_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_str_add(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD ZEND_FILE_LINE_RELAY_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_str_add_new(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD_NEW ZEND_FILE_LINE_RELAY_CC);
	zend_string_delref(key);
	return ret;
}

ZEND_API zval* ZEND_FASTCALL zend_hash_index_add_empty_element(HashTable *ht, zend_ulong h)
{
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_index_add(ht, h, &dummy);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_add_empty_element(HashTable *ht, zend_string *key)
{
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_add(ht, key, &dummy);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_str_add_empty_element(HashTable *ht, const char *str, size_t len)
{
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_str_add(ht, str, len, &dummy);
}

static zend_always_inline zval *_zend_hash_index_add_or_update_i(HashTable *ht, zend_ulong h, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (UNEXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		CHECK_INIT(ht, h < ht->nTableSize);
		if (h < ht->nTableSize) {
			p = ht->arData + h;
			goto add_to_packed;
		}
		goto add_to_hash;
	} else if (ht->u.flags & HASH_FLAG_PACKED) {
		if (h < ht->nNumUsed) {
			p = ht->arData + h;
			if (Z_TYPE(p->val) != IS_UNDEF) {
				if (flag & HASH_ADD) {
					return NULL;
				}
				if (ht->pDestructor) {
					ht->pDestructor(&p->val);
				}
				ZVAL_COPY_VALUE(&p->val, pData);
				if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
					ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
				}
				return &p->val;
			} else { /* we have to keep the order :( */
				goto convert_to_hash;
			}
		} else if (EXPECTED(h < ht->nTableSize)) {
			p = ht->arData + h;
		} else if ((h >> 1) < ht->nTableSize &&
		           (ht->nTableSize >> 1) < ht->nNumOfElements) {
			zend_hash_packed_grow(ht);
			p = ht->arData + h;
		} else {
			goto convert_to_hash;
		}

add_to_packed:
		HANDLE_BLOCK_INTERRUPTIONS();
		/* incremental initialization of empty Buckets */
		if ((flag & (HASH_ADD_NEW|HASH_ADD_NEXT)) == (HASH_ADD_NEW|HASH_ADD_NEXT)) {
			ht->nNumUsed = h + 1;
		} else if (h >= ht->nNumUsed) {
			if (h > ht->nNumUsed) {
				Bucket *q = ht->arData + ht->nNumUsed;
				while (q != p) {
					ZVAL_UNDEF(&q->val);
					q++;
				}
			}
			ht->nNumUsed = h + 1;
		}
		ht->nNumOfElements++;
		if (ht->nInternalPointer == HT_INVALID_IDX) {
			ht->nInternalPointer = h;
		}
		zend_hash_iterators_update(ht, HT_INVALID_IDX, h);
		if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
			ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
		}
		p->h = h;
		p->key = NULL;
		ZVAL_COPY_VALUE(&p->val, pData);

		HANDLE_UNBLOCK_INTERRUPTIONS();

		return &p->val;

convert_to_hash:
		zend_hash_packed_to_hash(ht);
	} else if ((flag & HASH_ADD_NEW) == 0) {
		p = zend_hash_index_find_bucket(ht, h);
		if (p) {
			if (flag & HASH_ADD) {
				return NULL;
			}
			ZEND_ASSERT(&p->val != pData);
			HANDLE_BLOCK_INTERRUPTIONS();
			if (ht->pDestructor) {
				ht->pDestructor(&p->val);
			}
			ZVAL_COPY_VALUE(&p->val, pData);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
				ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
			}
			return &p->val;
		}
	}

	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */

add_to_hash:
	HANDLE_BLOCK_INTERRUPTIONS();
	idx = ht->nNumUsed++;
	ht->nNumOfElements++;
	if (ht->nInternalPointer == HT_INVALID_IDX) {
		ht->nInternalPointer = idx;
	}
	zend_hash_iterators_update(ht, HT_INVALID_IDX, idx);
	if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
		ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
	}
	p = ht->arData + idx;
	p->h = h;
	p->key = NULL;
	nIndex = h | ht->nTableMask;
	ZVAL_COPY_VALUE(&p->val, pData);
	Z_NEXT(p->val) = HT_HASH(ht, nIndex);
	HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(idx);
	HANDLE_UNBLOCK_INTERRUPTIONS();

	return &p->val;
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_index_add_or_update(HashTable *ht, zend_ulong h, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, flag ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_index_add(HashTable *ht, zend_ulong h, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_ADD ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_index_add_new(HashTable *ht, zend_ulong h, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_ADD | HASH_ADD_NEW ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_index_update(HashTable *ht, zend_ulong h, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_UPDATE ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_next_index_insert(HashTable *ht, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, ht->nNextFreeElement, pData, HASH_ADD | HASH_ADD_NEXT ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_next_index_insert_new(HashTable *ht, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, ht->nNextFreeElement, pData, HASH_ADD | HASH_ADD_NEW | HASH_ADD_NEXT ZEND_FILE_LINE_RELAY_CC);
}

static void ZEND_FASTCALL zend_hash_do_resize(HashTable *ht)
{

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (ht->nNumUsed > ht->nNumOfElements + (ht->nNumOfElements >> 5)) { /* additional term is there to amortize the cost of compaction */
		HANDLE_BLOCK_INTERRUPTIONS();
		zend_hash_rehash(ht);
		HANDLE_UNBLOCK_INTERRUPTIONS();
	} else if (ht->nTableSize < HT_MAX_SIZE) {	/* Let's double the table size */
		void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
		uint32_t nSize = ht->nTableSize + ht->nTableSize;
		Bucket *old_buckets = ht->arData;

		HANDLE_BLOCK_INTERRUPTIONS();
		new_data = pemalloc(HT_SIZE_EX(nSize, -nSize), ht->u.flags & HASH_FLAG_PERSISTENT);
		ht->nTableSize = nSize;
		ht->nTableMask = -ht->nTableSize;
		HT_SET_DATA_ADDR(ht, new_data);
		memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
		pefree(old_data, ht->u.flags & HASH_FLAG_PERSISTENT);
		zend_hash_rehash(ht);
		HANDLE_UNBLOCK_INTERRUPTIONS();
	} else {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", ht->nTableSize * 2, sizeof(Bucket) + sizeof(uint32_t), sizeof(Bucket));
	}
}

ZEND_API int ZEND_FASTCALL zend_hash_rehash(HashTable *ht)
{
	Bucket *p;
	uint32_t nIndex, i;

	IS_CONSISTENT(ht);

	if (UNEXPECTED(ht->nNumOfElements == 0)) {
		if (ht->u.flags & HASH_FLAG_INITIALIZED) {
			ht->nNumUsed = 0;
			HT_HASH_RESET(ht);
		}
		return SUCCESS;
	}

	HT_HASH_RESET(ht);
	i = 0;
	p = ht->arData;
	if (ht->nNumUsed == ht->nNumOfElements) {
		do {
			nIndex = p->h | ht->nTableMask;
			Z_NEXT(p->val) = HT_HASH(ht, nIndex);
			HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(i);
			p++;
		} while (++i < ht->nNumUsed);
	} else {
		do {
			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) {
				uint32_t j = i;
				Bucket *q = p;

				if (EXPECTED(ht->u.v.nIteratorsCount == 0)) {
					while (++i < ht->nNumUsed) {
						p++;
						if (EXPECTED(Z_TYPE_INFO(p->val) != IS_UNDEF)) {
							ZVAL_COPY_VALUE(&q->val, &p->val);
							q->h = p->h;
							nIndex = q->h | ht->nTableMask;
							q->key = p->key;
							Z_NEXT(q->val) = HT_HASH(ht, nIndex);
							HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(j);
							if (UNEXPECTED(ht->nInternalPointer == i)) {
								ht->nInternalPointer = j;
							}
							q++;
							j++;
						}
					}
				} else {
					uint32_t iter_pos = zend_hash_iterators_lower_pos(ht, 0);

					while (++i < ht->nNumUsed) {
						p++;
						if (EXPECTED(Z_TYPE_INFO(p->val) != IS_UNDEF)) {
							ZVAL_COPY_VALUE(&q->val, &p->val);
							q->h = p->h;
							nIndex = q->h | ht->nTableMask;
							q->key = p->key;
							Z_NEXT(q->val) = HT_HASH(ht, nIndex);
							HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(j);
							if (UNEXPECTED(ht->nInternalPointer == i)) {
								ht->nInternalPointer = j;
							}
							if (UNEXPECTED(i == iter_pos)) {
								zend_hash_iterators_update(ht, i, j);
								iter_pos = zend_hash_iterators_lower_pos(ht, iter_pos + 1);
							}
							q++;
							j++;
						}
					}
				}
				ht->nNumUsed = j;
				break;
			}
			nIndex = p->h | ht->nTableMask;
			Z_NEXT(p->val) = HT_HASH(ht, nIndex);
			HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(i);
			p++;
		} while (++i < ht->nNumUsed);
	}
	return SUCCESS;
}

static zend_always_inline void _zend_hash_del_el_ex(HashTable *ht, uint32_t idx, Bucket *p, Bucket *prev)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	if (!(ht->u.flags & HASH_FLAG_PACKED)) {
		if (prev) {
			Z_NEXT(prev->val) = Z_NEXT(p->val);
		} else {
			HT_HASH(ht, p->h | ht->nTableMask) = Z_NEXT(p->val);
		}
	}
	if (HT_IDX_TO_HASH(ht->nNumUsed - 1) == idx) {
		do {
			ht->nNumUsed--;
		} while (ht->nNumUsed > 0 && (UNEXPECTED(Z_TYPE(ht->arData[ht->nNumUsed-1].val) == IS_UNDEF)));
	}
	ht->nNumOfElements--;
	if (HT_IDX_TO_HASH(ht->nInternalPointer) == idx || UNEXPECTED(ht->u.v.nIteratorsCount)) {
		uint32_t new_idx;

		new_idx = idx = HT_HASH_TO_IDX(idx);
		while (1) {
			new_idx++;
			if (new_idx >= ht->nNumUsed) {
				new_idx = HT_INVALID_IDX;
				break;
			} else if (Z_TYPE(ht->arData[new_idx].val) != IS_UNDEF) {
				break;
			}
		}
		if (ht->nInternalPointer == idx) {
			ht->nInternalPointer = new_idx;
		}
		zend_hash_iterators_update(ht, idx, new_idx);
	}
	if (p->key) {
		zend_string_release(p->key);
	}
	if (ht->pDestructor) {
		zval tmp;
		ZVAL_COPY_VALUE(&tmp, &p->val);
		ZVAL_UNDEF(&p->val);
		ht->pDestructor(&tmp);
	} else {
		ZVAL_UNDEF(&p->val);
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

static zend_always_inline void _zend_hash_del_el(HashTable *ht, uint32_t idx, Bucket *p)
{
	Bucket *prev = NULL;

	if (!(ht->u.flags & HASH_FLAG_PACKED)) {
		uint32_t nIndex = p->h | ht->nTableMask;
		uint32_t i = HT_HASH(ht, nIndex);

		if (i != idx) {
			prev = HT_HASH_TO_BUCKET(ht, i);
			while (Z_NEXT(prev->val) != idx) {
				i = Z_NEXT(prev->val);
				prev = HT_HASH_TO_BUCKET(ht, i);
			}
	 	}
	}

	_zend_hash_del_el_ex(ht, idx, p, prev);
}

ZEND_API void ZEND_FASTCALL zend_hash_del_bucket(HashTable *ht, Bucket *p)
{
	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	_zend_hash_del_el(ht, HT_IDX_TO_HASH(p - ht->arData), p);
}

ZEND_API int ZEND_FASTCALL zend_hash_del(HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	h = zend_string_hash_val(key);
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->key == key) ||
			(p->h == h &&
		     p->key &&
		     ZSTR_LEN(p->key) == ZSTR_LEN(key) &&
		     memcmp(ZSTR_VAL(p->key), ZSTR_VAL(key), ZSTR_LEN(key)) == 0)) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int ZEND_FASTCALL zend_hash_del_ind(HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	h = zend_string_hash_val(key);
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->key == key) ||
			(p->h == h &&
		     p->key &&
		     ZSTR_LEN(p->key) == ZSTR_LEN(key) &&
		     memcmp(ZSTR_VAL(p->key), ZSTR_VAL(key), ZSTR_LEN(key)) == 0)) {
			if (Z_TYPE(p->val) == IS_INDIRECT) {
				zval *data = Z_INDIRECT(p->val);

				if (UNEXPECTED(Z_TYPE_P(data) == IS_UNDEF)) {
					return FAILURE;
				} else {
					if (ht->pDestructor) {
						zval tmp;
						ZVAL_COPY_VALUE(&tmp, data);
						ZVAL_UNDEF(data);
						ht->pDestructor(&tmp);
					} else {
						ZVAL_UNDEF(data);
					}
					ht->u.v.flags |= HASH_FLAG_HAS_EMPTY_IND;
				}
			} else {
				_zend_hash_del_el_ex(ht, idx, p, prev);
			}
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int ZEND_FASTCALL zend_hash_str_del_ind(HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	h = zend_inline_hash_func(str, len);
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->h == h)
			 && p->key
			 && (ZSTR_LEN(p->key) == len)
			 && !memcmp(ZSTR_VAL(p->key), str, len)) {
			if (Z_TYPE(p->val) == IS_INDIRECT) {
				zval *data = Z_INDIRECT(p->val);

				if (UNEXPECTED(Z_TYPE_P(data) == IS_UNDEF)) {
					return FAILURE;
				} else {
					if (ht->pDestructor) {
						ht->pDestructor(data);
					}
					ZVAL_UNDEF(data);
					ht->u.v.flags |= HASH_FLAG_HAS_EMPTY_IND;
				}
			} else {
				_zend_hash_del_el_ex(ht, idx, p, prev);
			}
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int ZEND_FASTCALL zend_hash_str_del(HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	h = zend_inline_hash_func(str, len);
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->h == h)
			 && p->key
			 && (ZSTR_LEN(p->key) == len)
			 && !memcmp(ZSTR_VAL(p->key), str, len)) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int ZEND_FASTCALL zend_hash_index_del(HashTable *ht, zend_ulong h)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (h < ht->nNumUsed) {
			p = ht->arData + h;
			if (Z_TYPE(p->val) != IS_UNDEF) {
				_zend_hash_del_el_ex(ht, HT_IDX_TO_HASH(h), p, NULL);
				return SUCCESS;
			}
		}
		return FAILURE;
	}
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->h == h) && (p->key == NULL)) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API void ZEND_FASTCALL zend_hash_destroy(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) <= 1);

	if (ht->nNumUsed) {
		p = ht->arData;
		end = p + ht->nNumUsed;
		if (ht->pDestructor) {
			SET_INCONSISTENT(HT_IS_DESTROYING);

			if (ht->u.flags & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS)) {
				if (ht->nNumUsed == ht->nNumOfElements) {
					do {
						ht->pDestructor(&p->val);
					} while (++p != end);
				} else {
					do {
						if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
							ht->pDestructor(&p->val);
						}
					} while (++p != end);
				}
			} else if (ht->nNumUsed == ht->nNumOfElements) {
				do {
					ht->pDestructor(&p->val);
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				} while (++p != end);
			} else {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						ht->pDestructor(&p->val);
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					}
				} while (++p != end);
			}

			SET_INCONSISTENT(HT_DESTROYED);
		} else {
			if (!(ht->u.flags & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS))) {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					}
				} while (++p != end);
			}
		}
		zend_hash_iterators_remove(ht);
	} else if (EXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		return;
	}
	pefree(HT_GET_DATA_ADDR(ht), ht->u.flags & HASH_FLAG_PERSISTENT);
}

ZEND_API void ZEND_FASTCALL zend_array_destroy(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) <= 1);

	/* break possible cycles */
	GC_REMOVE_FROM_BUFFER(ht);
	GC_TYPE_INFO(ht) = IS_NULL | (GC_WHITE << 16);

	if (ht->nNumUsed) {
		/* In some rare cases destructors of regular arrays may be changed */
		if (UNEXPECTED(ht->pDestructor != ZVAL_PTR_DTOR)) {
			zend_hash_destroy(ht);
			goto free_ht;
		}

		p = ht->arData;
		end = p + ht->nNumUsed;
		SET_INCONSISTENT(HT_IS_DESTROYING);

		if (ht->u.flags & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS)) {
			do {
				i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
			} while (++p != end);
		} else if (ht->nNumUsed == ht->nNumOfElements) {
			do {
				i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
				if (EXPECTED(p->key)) {
					zend_string_release(p->key);
				}
			} while (++p != end);
		} else {
			do {
				if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
					i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				}
			} while (++p != end);
		}
		zend_hash_iterators_remove(ht);
		SET_INCONSISTENT(HT_DESTROYED);
	} else if (EXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		goto free_ht;
	}
	efree(HT_GET_DATA_ADDR(ht));
free_ht:
	FREE_HASHTABLE(ht);
}

ZEND_API void ZEND_FASTCALL zend_hash_clean(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (ht->nNumUsed) {
		p = ht->arData;
		end = p + ht->nNumUsed;
		if (ht->pDestructor) {
			if (ht->u.flags & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS)) {
				if (ht->nNumUsed == ht->nNumOfElements) {
					do {
						ht->pDestructor(&p->val);
					} while (++p != end);
				} else {
					do {
						if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
							ht->pDestructor(&p->val);
						}
					} while (++p != end);
				}
			} else if (ht->nNumUsed == ht->nNumOfElements) {
				do {
					ht->pDestructor(&p->val);
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				} while (++p != end);
			} else {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						ht->pDestructor(&p->val);
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					}
				} while (++p != end);
			}
		} else {
			if (!(ht->u.flags & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS))) {
				if (ht->nNumUsed == ht->nNumOfElements) {
					do {
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					} while (++p != end);
				} else {
					do {
						if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
							if (EXPECTED(p->key)) {
								zend_string_release(p->key);
							}
						}
					} while (++p != end);
				}
			}
		}
		if (!(ht->u.flags & HASH_FLAG_PACKED)) {
			HT_HASH_RESET(ht);
		}
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->nInternalPointer = HT_INVALID_IDX;
}

ZEND_API void ZEND_FASTCALL zend_symtable_clean(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (ht->nNumUsed) {
		p = ht->arData;
		end = p + ht->nNumUsed;
		if (ht->u.flags & HASH_FLAG_STATIC_KEYS) {
			do {
				i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
			} while (++p != end);
		} else if (ht->nNumUsed == ht->nNumOfElements) {
			do {
				i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
				zend_string_release(p->key);
			} while (++p != end);
		} else {
			do {
				if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
					i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
					zend_string_release(p->key);
				}
			} while (++p != end);
		}
		HT_HASH_RESET(ht);
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->nInternalPointer = HT_INVALID_IDX;
}

ZEND_API void ZEND_FASTCALL zend_hash_graceful_destroy(HashTable *ht)
{
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	p = ht->arData;
	for (idx = 0; idx < ht->nNumUsed; idx++, p++) {
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
	}
	if (ht->u.flags & HASH_FLAG_INITIALIZED) {
		pefree(HT_GET_DATA_ADDR(ht), ht->u.flags & HASH_FLAG_PERSISTENT);
	}

	SET_INCONSISTENT(HT_DESTROYED);
}

ZEND_API void ZEND_FASTCALL zend_hash_graceful_reverse_destroy(HashTable *ht)
{
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	idx = ht->nNumUsed;
	p = ht->arData + ht->nNumUsed;
	while (idx > 0) {
		idx--;
		p--;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
	}

	if (ht->u.flags & HASH_FLAG_INITIALIZED) {
		pefree(HT_GET_DATA_ADDR(ht), ht->u.flags & HASH_FLAG_PERSISTENT);
	}

	SET_INCONSISTENT(HT_DESTROYED);
}

/* This is used to recurse elements and selectively delete certain entries
 * from a hashtable. apply_func() receives the data and decides if the entry
 * should be deleted or recursion should be stopped. The following three
 * return codes are possible:
 * ZEND_HASH_APPLY_KEEP   - continue
 * ZEND_HASH_APPLY_STOP   - stop iteration
 * ZEND_HASH_APPLY_REMOVE - delete the element, combineable with the former
 */

ZEND_API void ZEND_FASTCALL zend_hash_apply(HashTable *ht, apply_func_t apply_func)
{
	uint32_t idx;
	Bucket *p;
	int result;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	HASH_PROTECT_RECURSION(ht);
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		result = apply_func(&p->val);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void ZEND_FASTCALL zend_hash_apply_with_argument(HashTable *ht, apply_func_arg_t apply_func, void *argument)
{
    uint32_t idx;
	Bucket *p;
	int result;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	HASH_PROTECT_RECURSION(ht);
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		result = apply_func(&p->val, argument);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void ZEND_FASTCALL zend_hash_apply_with_arguments(HashTable *ht, apply_func_args_t apply_func, int num_args, ...)
{
	uint32_t idx;
	Bucket *p;
	va_list args;
	zend_hash_key hash_key;
	int result;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	HASH_PROTECT_RECURSION(ht);

	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		va_start(args, num_args);
		hash_key.h = p->h;
		hash_key.key = p->key;

		result = apply_func(&p->val, num_args, args, &hash_key);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			va_end(args);
			break;
		}
		va_end(args);
	}

	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void ZEND_FASTCALL zend_hash_reverse_apply(HashTable *ht, apply_func_t apply_func)
{
	uint32_t idx;
	Bucket *p;
	int result;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	HASH_PROTECT_RECURSION(ht);
	idx = ht->nNumUsed;
	while (idx > 0) {
		idx--;
		p = ht->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;

		result = apply_func(&p->val);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void ZEND_FASTCALL zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor)
{
    uint32_t idx;
	Bucket *p;
	zval *new_entry, *data;
	zend_bool setTargetPointer;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);
	HT_ASSERT(GC_REFCOUNT(target) == 1);

	setTargetPointer = (target->nInternalPointer == HT_INVALID_IDX);
	for (idx = 0; idx < source->nNumUsed; idx++) {
		p = source->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;

		if (setTargetPointer && source->nInternalPointer == idx) {
			target->nInternalPointer = HT_INVALID_IDX;
		}
		/* INDIRECT element may point to UNDEF-ined slots */
		data = &p->val;
		if (Z_TYPE_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
			if (UNEXPECTED(Z_TYPE_P(data) == IS_UNDEF)) {
				continue;
			}
		}
		if (p->key) {
			new_entry = zend_hash_update(target, p->key, data);
		} else {
			new_entry = zend_hash_index_update(target, p->h, data);
		}
		if (pCopyConstructor) {
			pCopyConstructor(new_entry);
		}
	}
	if (target->nInternalPointer == HT_INVALID_IDX && target->nNumOfElements > 0) {
		idx = 0;
		while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
			idx++;
		}
		target->nInternalPointer = idx;
	}
}


static zend_always_inline int zend_array_dup_element(HashTable *source, HashTable *target, uint32_t idx, Bucket *p, Bucket *q, int packed, int static_keys, int with_holes)
{
	zval *data = &p->val;

	if (with_holes) {
		if (!packed && Z_TYPE_INFO_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
		}
		if (UNEXPECTED(Z_TYPE_INFO_P(data) == IS_UNDEF)) {
			return 0;
		}
	} else if (!packed) {
		/* INDIRECT element may point to UNDEF-ined slots */
		if (Z_TYPE_INFO_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
			if (UNEXPECTED(Z_TYPE_INFO_P(data) == IS_UNDEF)) {
				return 0;
			}
		}
	}

	do {
		if (Z_OPT_REFCOUNTED_P(data)) {
			if (Z_ISREF_P(data) && Z_REFCOUNT_P(data) == 1 &&
			    (Z_TYPE_P(Z_REFVAL_P(data)) != IS_ARRAY ||
			      Z_ARRVAL_P(Z_REFVAL_P(data)) != source)) {
				data = Z_REFVAL_P(data);
				if (!Z_OPT_REFCOUNTED_P(data)) {
					break;
				}
			}
			Z_ADDREF_P(data);
		}
	} while (0);
	ZVAL_COPY_VALUE(&q->val, data);

	q->h = p->h;
	if (packed) {
		q->key = NULL;
	} else {
		uint32_t nIndex;

		q->key = p->key;
		if (!static_keys && q->key) {
			zend_string_addref(q->key);
		}

		nIndex = q->h | target->nTableMask;
		Z_NEXT(q->val) = HT_HASH(target, nIndex);
		HT_HASH(target, nIndex) = HT_IDX_TO_HASH(idx);
	}
	return 1;
}

static zend_always_inline void zend_array_dup_packed_elements(HashTable *source, HashTable *target, int with_holes)
{
	Bucket *p = source->arData;
	Bucket *q = target->arData;
	Bucket *end = p + source->nNumUsed;

	do {
		if (!zend_array_dup_element(source, target, 0, p, q, 1, 1, with_holes)) {
			if (with_holes) {
				ZVAL_UNDEF(&q->val);
			}
		}
		p++; q++;
	} while (p != end);
}

static zend_always_inline uint32_t zend_array_dup_elements(HashTable *source, HashTable *target, int static_keys, int with_holes)
{
    uint32_t idx = 0;
	Bucket *p = source->arData;
	Bucket *q = target->arData;
	Bucket *end = p + source->nNumUsed;

	do {
		if (!zend_array_dup_element(source, target, idx, p, q, 0, static_keys, with_holes)) {
			uint32_t target_idx = idx;

			idx++; p++;
			while (p != end) {
				if (zend_array_dup_element(source, target, target_idx, p, q, 0, static_keys, with_holes)) {
					if (source->nInternalPointer == idx) {
						target->nInternalPointer = target_idx;
					}
					target_idx++; q++;
				}
				idx++; p++;
			}
			return target_idx;
		}
		idx++; p++; q++;
	} while (p != end);
	return idx;
}

ZEND_API HashTable* ZEND_FASTCALL zend_array_dup(HashTable *source)
{
    uint32_t idx;
	HashTable *target;

	IS_CONSISTENT(source);

	ALLOC_HASHTABLE(target);
	GC_REFCOUNT(target) = 1;
	GC_TYPE_INFO(target) = IS_ARRAY;

	target->nTableSize = source->nTableSize;
	target->pDestructor = source->pDestructor;

	if (source->nNumUsed == 0) {
		target->u.flags = (source->u.flags & ~(HASH_FLAG_INITIALIZED|HASH_FLAG_PACKED|HASH_FLAG_PERSISTENT)) | HASH_FLAG_APPLY_PROTECTION | HASH_FLAG_STATIC_KEYS;
		target->nTableMask = HT_MIN_MASK;
		target->nNumUsed = 0;
		target->nNumOfElements = 0;
		target->nNextFreeElement = 0;
		target->nInternalPointer = HT_INVALID_IDX;
		HT_SET_DATA_ADDR(target, &uninitialized_bucket);
	} else if (GC_FLAGS(source) & IS_ARRAY_IMMUTABLE) {
		target->u.flags = (source->u.flags & ~HASH_FLAG_PERSISTENT) | HASH_FLAG_APPLY_PROTECTION;
		target->nTableMask = source->nTableMask;
		target->nNumUsed = source->nNumUsed;
		target->nNumOfElements = source->nNumOfElements;
		target->nNextFreeElement = source->nNextFreeElement;
		HT_SET_DATA_ADDR(target, emalloc(HT_SIZE(target)));
		target->nInternalPointer = source->nInternalPointer;
		memcpy(HT_GET_DATA_ADDR(target), HT_GET_DATA_ADDR(source), HT_USED_SIZE(source));
		if (target->nNumOfElements > 0 &&
		    target->nInternalPointer == HT_INVALID_IDX) {
			idx = 0;
			while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
				idx++;
			}
			target->nInternalPointer = idx;
		}
	} else if (source->u.flags & HASH_FLAG_PACKED) {
		target->u.flags = (source->u.flags & ~HASH_FLAG_PERSISTENT) | HASH_FLAG_APPLY_PROTECTION;
		target->nTableMask = source->nTableMask;
		target->nNumUsed = source->nNumUsed;
		target->nNumOfElements = source->nNumOfElements;
		target->nNextFreeElement = source->nNextFreeElement;
		HT_SET_DATA_ADDR(target, emalloc(HT_SIZE(target)));
		target->nInternalPointer = source->nInternalPointer;
		HT_HASH_RESET_PACKED(target);

		if (target->nNumUsed == target->nNumOfElements) {
			zend_array_dup_packed_elements(source, target, 0);
		} else {
			zend_array_dup_packed_elements(source, target, 1);
		}
		if (target->nNumOfElements > 0 &&
		    target->nInternalPointer == HT_INVALID_IDX) {
			idx = 0;
			while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
				idx++;
			}
			target->nInternalPointer = idx;
		}
	} else {
		target->u.flags = (source->u.flags & ~HASH_FLAG_PERSISTENT) | HASH_FLAG_APPLY_PROTECTION;
		target->nTableMask = source->nTableMask;
		target->nNextFreeElement = source->nNextFreeElement;
		target->nInternalPointer = HT_INVALID_IDX;
		HT_SET_DATA_ADDR(target, emalloc(HT_SIZE(target)));
		HT_HASH_RESET(target);

		if (target->u.flags & HASH_FLAG_STATIC_KEYS) {
			if (source->nNumUsed == source->nNumOfElements) {
				idx = zend_array_dup_elements(source, target, 1, 0);
			} else {
				idx = zend_array_dup_elements(source, target, 1, 1);
			}
		} else {
			if (source->nNumUsed == source->nNumOfElements) {
				idx = zend_array_dup_elements(source, target, 0, 0);
			} else {
				idx = zend_array_dup_elements(source, target, 0, 1);
			}
		}
		target->nNumUsed = idx;
		target->nNumOfElements = idx;
		if (idx > 0 && target->nInternalPointer == HT_INVALID_IDX) {
			target->nInternalPointer = 0;
		}
	}
	return target;
}


ZEND_API void ZEND_FASTCALL _zend_hash_merge(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, zend_bool overwrite ZEND_FILE_LINE_DC)
{
    uint32_t idx;
	Bucket *p;
	zval *t;
	uint32_t mode = (overwrite?HASH_UPDATE:HASH_ADD);

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);
	HT_ASSERT(GC_REFCOUNT(target) == 1);

	for (idx = 0; idx < source->nNumUsed; idx++) {
		p = source->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		if (p->key) {
			t = _zend_hash_add_or_update(target, p->key, &p->val, mode ZEND_FILE_LINE_RELAY_CC);
			if (t && pCopyConstructor) {
				pCopyConstructor(t);
			}
		} else {
			if ((mode==HASH_UPDATE || !zend_hash_index_exists(target, p->h))) {
			 	t = zend_hash_index_update(target, p->h, &p->val);
			 	if (t && pCopyConstructor) {
					pCopyConstructor(t);
				}
			}
		}
	}
	if (target->nNumOfElements > 0) {
		idx = 0;
		while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
			idx++;
		}
		target->nInternalPointer = idx;
	}
}


static zend_bool ZEND_FASTCALL zend_hash_replace_checker_wrapper(HashTable *target, zval *source_data, Bucket *p, void *pParam, merge_checker_func_t merge_checker_func)
{
	zend_hash_key hash_key;

	hash_key.h = p->h;
	hash_key.key = p->key;
	return merge_checker_func(target, source_data, &hash_key, pParam);
}


ZEND_API void ZEND_FASTCALL zend_hash_merge_ex(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, merge_checker_func_t pMergeSource, void *pParam)
{
	uint32_t idx;
	Bucket *p;
	zval *t;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);
	HT_ASSERT(GC_REFCOUNT(target) == 1);

	for (idx = 0; idx < source->nNumUsed; idx++) {
		p = source->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		if (zend_hash_replace_checker_wrapper(target, &p->val, p, pParam, pMergeSource)) {
			t = zend_hash_update(target, p->key, &p->val);
			if (t && pCopyConstructor) {
				pCopyConstructor(t);
			}
		}
	}
	if (target->nNumOfElements > 0) {
		idx = 0;
		while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
			idx++;
		}
		target->nInternalPointer = idx;
	}
}


/* Returns the hash table data if found and NULL if not. */
ZEND_API zval* ZEND_FASTCALL zend_hash_find(const HashTable *ht, zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = zend_hash_find_bucket(ht, key);
	return p ? &p->val : NULL;
}

ZEND_API zval* ZEND_FASTCALL zend_hash_str_find(const HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	Bucket *p;

	IS_CONSISTENT(ht);

	h = zend_inline_hash_func(str, len);
	p = zend_hash_str_find_bucket(ht, str, len, h);
	return p ? &p->val : NULL;
}

ZEND_API zend_bool ZEND_FASTCALL zend_hash_exists(const HashTable *ht, zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = zend_hash_find_bucket(ht, key);
	return p ? 1 : 0;
}

ZEND_API zend_bool ZEND_FASTCALL zend_hash_str_exists(const HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	Bucket *p;

	IS_CONSISTENT(ht);

	h = zend_inline_hash_func(str, len);
	p = zend_hash_str_find_bucket(ht, str, len, h);
	return p ? 1 : 0;
}

ZEND_API zval* ZEND_FASTCALL zend_hash_index_find(const HashTable *ht, zend_ulong h)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (h < ht->nNumUsed) {
			p = ht->arData + h;
			if (Z_TYPE(p->val) != IS_UNDEF) {
				return &p->val;
			}
		}
		return NULL;
	}

	p = zend_hash_index_find_bucket(ht, h);
	return p ? &p->val : NULL;
}


ZEND_API zend_bool ZEND_FASTCALL zend_hash_index_exists(const HashTable *ht, zend_ulong h)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (h < ht->nNumUsed) {
			if (Z_TYPE(ht->arData[h].val) != IS_UNDEF) {
				return 1;
			}
		}
		return 0;
	}

	p = zend_hash_index_find_bucket(ht, h);
	return p ? 1 : 0;
}


ZEND_API void ZEND_FASTCALL zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos)
{
    uint32_t idx;

	IS_CONSISTENT(ht);
	HT_ASSERT(&ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);

	for (idx = 0; idx < ht->nNumUsed; idx++) {
		if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
			*pos = idx;
			return;
		}
	}
	*pos = HT_INVALID_IDX;
}


/* This function will be extremely optimized by remembering
 * the end of the list
 */
ZEND_API void ZEND_FASTCALL zend_hash_internal_pointer_end_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx;

	IS_CONSISTENT(ht);
	HT_ASSERT(&ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);

	idx = ht->nNumUsed;
	while (idx > 0) {
		idx--;
		if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
			*pos = idx;
			return;
		}
	}
	*pos = HT_INVALID_IDX;
}


ZEND_API int ZEND_FASTCALL zend_hash_move_forward_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);
	HT_ASSERT(&ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);

	if (idx != HT_INVALID_IDX) {
		while (1) {
			idx++;
			if (idx >= ht->nNumUsed) {
				*pos = HT_INVALID_IDX;
				return SUCCESS;
			}
			if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
				*pos = idx;
				return SUCCESS;
			}
		}
	} else {
 		return FAILURE;
	}
}

ZEND_API int ZEND_FASTCALL zend_hash_move_backwards_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);
	HT_ASSERT(&ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);

	if (idx != HT_INVALID_IDX) {
		while (idx > 0) {
			idx--;
			if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
				*pos = idx;
				return SUCCESS;
			}
		}
		*pos = HT_INVALID_IDX;
 		return SUCCESS;
	} else {
 		return FAILURE;
	}
}


/* This function should be made binary safe  */
ZEND_API int ZEND_FASTCALL zend_hash_get_current_key_ex(const HashTable *ht, zend_string **str_index, zend_ulong *num_index, HashPosition *pos)
{
	uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx != HT_INVALID_IDX) {
		p = ht->arData + idx;
		if (p->key) {
			*str_index = p->key;
			return HASH_KEY_IS_STRING;
		} else {
			*num_index = p->h;
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTENT;
}

ZEND_API void ZEND_FASTCALL zend_hash_get_current_key_zval_ex(const HashTable *ht, zval *key, HashPosition *pos)
{
	uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx == HT_INVALID_IDX) {
		ZVAL_NULL(key);
	} else {
		p = ht->arData + idx;
		if (p->key) {
			ZVAL_STR_COPY(key, p->key);
		} else {
			ZVAL_LONG(key, p->h);
		}
	}
}

ZEND_API int ZEND_FASTCALL zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos)
{
    uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx != HT_INVALID_IDX) {
		p = ht->arData + idx;
		if (p->key) {
			return HASH_KEY_IS_STRING;
		} else {
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTENT;
}


ZEND_API zval* ZEND_FASTCALL zend_hash_get_current_data_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx != HT_INVALID_IDX) {
		p = ht->arData + idx;
		return &p->val;
	} else {
		return NULL;
	}
}

ZEND_API void zend_hash_bucket_swap(Bucket *p, Bucket *q)
{
	zval val;
	zend_ulong h;
	zend_string *key;

	ZVAL_COPY_VALUE(&val, &p->val);
	h = p->h;
	key = p->key;

	ZVAL_COPY_VALUE(&p->val, &q->val);
	p->h = q->h;
	p->key = q->key;

	ZVAL_COPY_VALUE(&q->val, &val);
	q->h = h;
	q->key = key;
}

ZEND_API void zend_hash_bucket_renum_swap(Bucket *p, Bucket *q)
{
	zval val;

	ZVAL_COPY_VALUE(&val, &p->val);
	ZVAL_COPY_VALUE(&p->val, &q->val);
	ZVAL_COPY_VALUE(&q->val, &val);
}

ZEND_API void zend_hash_bucket_packed_swap(Bucket *p, Bucket *q)
{
	zval val;
	zend_ulong h;

	ZVAL_COPY_VALUE(&val, &p->val);
	h = p->h;

	ZVAL_COPY_VALUE(&p->val, &q->val);
	p->h = q->h;

	ZVAL_COPY_VALUE(&q->val, &val);
	q->h = h;
}

ZEND_API int ZEND_FASTCALL zend_hash_sort_ex(HashTable *ht, sort_func_t sort, compare_func_t compar, zend_bool renumber)
{
	Bucket *p;
	uint32_t i, j;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (!(ht->nNumOfElements>1) && !(renumber && ht->nNumOfElements>0)) { /* Doesn't require sorting */
		return SUCCESS;
	}

	if (ht->nNumUsed == ht->nNumOfElements) {
		i = ht->nNumUsed;
	} else {
		for (j = 0, i = 0; j < ht->nNumUsed; j++) {
			p = ht->arData + j;
			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
			if (i != j) {
				ht->arData[i] = *p;
			}
			i++;
		}
	}

	sort((void *)ht->arData, i, sizeof(Bucket), compar,
			(swap_func_t)(renumber? zend_hash_bucket_renum_swap :
				((ht->u.flags & HASH_FLAG_PACKED) ? zend_hash_bucket_packed_swap : zend_hash_bucket_swap)));

	HANDLE_BLOCK_INTERRUPTIONS();
	ht->nNumUsed = i;
	ht->nInternalPointer = 0;

	if (renumber) {
		for (j = 0; j < i; j++) {
			p = ht->arData + j;
			p->h = j;
			if (p->key) {
				zend_string_release(p->key);
				p->key = NULL;
			}
		}

		ht->nNextFreeElement = i;
	}
	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (!renumber) {
			zend_hash_packed_to_hash(ht);
		}
	} else {
		if (renumber) {
			void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
			Bucket *old_buckets = ht->arData;

			new_data = pemalloc(HT_SIZE_EX(ht->nTableSize, HT_MIN_MASK), (ht->u.flags & HASH_FLAG_PERSISTENT));
			ht->u.flags |= HASH_FLAG_PACKED | HASH_FLAG_STATIC_KEYS;
			ht->nTableMask = HT_MIN_MASK;
			HT_SET_DATA_ADDR(ht, new_data);
			memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
			pefree(old_data, ht->u.flags & HASH_FLAG_PERSISTENT & HASH_FLAG_PERSISTENT);
			HT_HASH_RESET_PACKED(ht);
		} else {
			zend_hash_rehash(ht);
		}
	}

	HANDLE_UNBLOCK_INTERRUPTIONS();

	return SUCCESS;
}

static zend_always_inline int zend_hash_compare_impl(HashTable *ht1, HashTable *ht2, compare_func_t compar, zend_bool ordered) {
	uint32_t idx1, idx2;

	if (ht1->nNumOfElements != ht2->nNumOfElements) {
		return ht1->nNumOfElements > ht2->nNumOfElements ? 1 : -1;
	}

	for (idx1 = 0, idx2 = 0; idx1 < ht1->nNumUsed; idx1++) {
		Bucket *p1 = ht1->arData + idx1, *p2;
		zval *pData1, *pData2;
		int result;

		if (Z_TYPE(p1->val) == IS_UNDEF) continue;
		if (ordered) {
			while (1) {
				ZEND_ASSERT(idx2 != ht2->nNumUsed);
				p2 = ht2->arData + idx2;
				if (Z_TYPE(p2->val) != IS_UNDEF) break;
				idx2++;
			}
			if (p1->key == NULL && p2->key == NULL) { /* numeric indices */
				if (p1->h != p2->h) {
					return p1->h > p2->h ? 1 : -1;
				}
			} else if (p1->key != NULL && p2->key != NULL) { /* string indices */
				if (ZSTR_LEN(p1->key) != ZSTR_LEN(p2->key)) {
					return ZSTR_LEN(p1->key) > ZSTR_LEN(p2->key) ? 1 : -1;
				}

				result = memcmp(ZSTR_VAL(p1->key), ZSTR_VAL(p2->key), ZSTR_LEN(p1->key));
				if (result != 0) {
					return result;
				}
			} else {
				/* Mixed key types: A string key is considered as larger */
				return p1->key != NULL ? 1 : -1;
			}
			pData2 = &p2->val;
			idx2++;
		} else {
			if (p1->key == NULL) { /* numeric index */
				pData2 = zend_hash_index_find(ht2, p1->h);
				if (pData2 == NULL) {
					return 1;
				}
			} else { /* string index */
				pData2 = zend_hash_find(ht2, p1->key);
				if (pData2 == NULL) {
					return 1;
				}
			}
		}

		pData1 = &p1->val;
		if (Z_TYPE_P(pData1) == IS_INDIRECT) {
			pData1 = Z_INDIRECT_P(pData1);
		}
		if (Z_TYPE_P(pData2) == IS_INDIRECT) {
			pData2 = Z_INDIRECT_P(pData2);
		}

		if (Z_TYPE_P(pData1) == IS_UNDEF) {
			if (Z_TYPE_P(pData2) != IS_UNDEF) {
				return -1;
			}
		} else if (Z_TYPE_P(pData2) == IS_UNDEF) {
			return 1;
		} else {
			result = compar(pData1, pData2);
			if (result != 0) {
				return result;
			}
		}
	}

	return 0;
}

ZEND_API int zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_func_t compar, zend_bool ordered)
{
	int result;
	IS_CONSISTENT(ht1);
	IS_CONSISTENT(ht2);

	HASH_PROTECT_RECURSION(ht1);
	HASH_PROTECT_RECURSION(ht2);
	result = zend_hash_compare_impl(ht1, ht2, compar, ordered);
	HASH_UNPROTECT_RECURSION(ht1);
	HASH_UNPROTECT_RECURSION(ht2);

	return result;
}


ZEND_API zval* ZEND_FASTCALL zend_hash_minmax(const HashTable *ht, compare_func_t compar, uint32_t flag)
{
	uint32_t idx;
	Bucket *p, *res;

	IS_CONSISTENT(ht);

	if (ht->nNumOfElements == 0 ) {
		return NULL;
	}

	idx = 0;
	while (1) {
		if (idx == ht->nNumUsed) {
			return NULL;
		}
		if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) break;
		idx++;
	}
	res = ht->arData + idx;
	for (; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;

		if (flag) {
			if (compar(res, p) < 0) { /* max */
				res = p;
			}
		} else {
			if (compar(res, p) > 0) { /* min */
				res = p;
			}
		}
	}
	return &res->val;
}

ZEND_API int ZEND_FASTCALL _zend_handle_numeric_str_ex(const char *key, size_t length, zend_ulong *idx)
{
	register const char *tmp = key;

	const char *end = key + length;
	ZEND_ASSERT(*end == '\0');

	if (*tmp == '-') {
		tmp++;
	}

	if ((*tmp == '0' && length > 1) /* numbers with leading zeros */
	 || (end - tmp > MAX_LENGTH_OF_LONG - 1) /* number too long */
	 || (SIZEOF_ZEND_LONG == 4 &&
	     end - tmp == MAX_LENGTH_OF_LONG - 1 &&
	     *tmp > '2')) { /* overflow */
		return 0;
	}
	*idx = (*tmp - '0');
	while (1) {
		++tmp;
		if (tmp == end) {
			if (*key == '-') {
				if (*idx-1 > ZEND_LONG_MAX) { /* overflow */
					return 0;
				}
				*idx = 0 - *idx;
			} else if (*idx > ZEND_LONG_MAX) { /* overflow */
				return 0;
			}
			return 1;
		}
		if (*tmp <= '9' && *tmp >= '0') {
			*idx = (*idx * 10) + (*tmp - '0');
		} else {
			return 0;
		}
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
