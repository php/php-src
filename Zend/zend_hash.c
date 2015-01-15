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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"

#if ZEND_DEBUG
/*
#define HASH_MASK_CONSISTENCY	0x60
*/
#define HT_OK					0x00
#define HT_IS_DESTROYING		0x20
#define HT_DESTROYED			0x40
#define HT_CLEANING				0x60

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
		if ((ht)->u.flags >= (3 << 8)) {												\
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

static void zend_hash_do_resize(HashTable *ht);

#define CHECK_INIT(ht, packed) do { \
	if (UNEXPECTED(!((ht)->u.flags & HASH_FLAG_INITIALIZED))) { \
		if (packed) { \
			(ht)->u.flags |= HASH_FLAG_INITIALIZED | HASH_FLAG_PACKED; \
			(ht)->arData = (Bucket *) safe_pemalloc((ht)->nTableSize, sizeof(Bucket), 0, (ht)->u.flags & HASH_FLAG_PERSISTENT); \
		} else { \
			(ht)->u.flags |= HASH_FLAG_INITIALIZED; \
			(ht)->nTableMask = (ht)->nTableSize - 1; \
			(ht)->arData = (Bucket *) safe_pemalloc((ht)->nTableSize, sizeof(Bucket) + sizeof(uint32_t), 0, (ht)->u.flags & HASH_FLAG_PERSISTENT); \
			(ht)->arHash = (uint32_t*)((ht)->arData + (ht)->nTableSize); \
			memset((ht)->arHash, INVALID_IDX, (ht)->nTableSize * sizeof(uint32_t)); \
		} \
	} \
} while (0)

static const uint32_t uninitialized_bucket = {INVALID_IDX};

ZEND_API void _zend_hash_init(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, zend_bool persistent ZEND_FILE_LINE_DC)
{
#if defined(ZEND_WIN32)
	unsigned long index;
#endif
	/* Use big enough power of 2 */
	/* size should be between 8 and 0x80000000 */
	nSize = (nSize <= 8 ? 8 : (nSize >= 0x80000000 ? 0x80000000 : nSize));

#if defined(ZEND_WIN32)
	if (BitScanReverse(&index, nSize - 1)) {
		ht->nTableSize = 0x2 << ((31 - index) ^ 0x1f);
	} else {
		/* nSize is ensured to be in the valid range, fall back to it
		   rather than using an undefined bis scan result. */
		ht->nTableSize = nSize;
	}
#elif defined(__GNUC__)
	ht->nTableSize =  0x2 << (__builtin_clz(nSize - 1) ^ 0x1f);
#else
	nSize -= 1;
	nSize |= (nSize >> 1);
	nSize |= (nSize >> 2);
	nSize |= (nSize >> 4);
	nSize |= (nSize >> 8);
	nSize |= (nSize >> 16);
	ht->nTableSize = nSize + 1;
#endif

	ht->nTableMask = 0;
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->arData = NULL;
	ht->arHash = (uint32_t*)&uninitialized_bucket;
	ht->pDestructor = pDestructor;
	ht->nInternalPointer = INVALID_IDX;
	ht->u.flags = (persistent ? HASH_FLAG_PERSISTENT : 0) | HASH_FLAG_APPLY_PROTECTION;
}

static void zend_hash_packed_grow(HashTable *ht)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->arData = (Bucket *) safe_perealloc(ht->arData, (ht->nTableSize << 1), sizeof(Bucket), 0, ht->u.flags & HASH_FLAG_PERSISTENT);
	ht->nTableSize = (ht->nTableSize << 1);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void zend_hash_real_init(HashTable *ht, zend_bool packed)
{
	IS_CONSISTENT(ht);

	CHECK_INIT(ht, packed);
}

ZEND_API void zend_hash_packed_to_hash(HashTable *ht)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->u.flags &= ~HASH_FLAG_PACKED;
	ht->nTableMask = ht->nTableSize - 1;
	ht->arData = (Bucket *) safe_perealloc(ht->arData, ht->nTableSize, sizeof(Bucket) + sizeof(uint32_t), 0, ht->u.flags & HASH_FLAG_PERSISTENT);
	ht->arHash = (uint32_t*)(ht->arData + ht->nTableSize);
	zend_hash_rehash(ht);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void zend_hash_to_packed(HashTable *ht)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->u.flags |= HASH_FLAG_PACKED;
	ht->nTableMask = 0;
	ht->arData = (Bucket *) perealloc(ht->arData, ht->nTableSize * sizeof(Bucket), ht->u.flags & HASH_FLAG_PERSISTENT);
	ht->arHash = (uint32_t*)&uninitialized_bucket;
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void _zend_hash_init_ex(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC)
{
	_zend_hash_init(ht, nSize, pDestructor, persistent ZEND_FILE_LINE_CC);
	if (!bApplyProtection) {
		ht->u.flags &= ~HASH_FLAG_APPLY_PROTECTION;
	}
}


ZEND_API void zend_hash_set_apply_protection(HashTable *ht, zend_bool bApplyProtection)
{
	if (bApplyProtection) {
		ht->u.flags |= HASH_FLAG_APPLY_PROTECTION;
	} else {
		ht->u.flags &= ~HASH_FLAG_APPLY_PROTECTION;
	}
}

static zend_always_inline Bucket *zend_hash_find_bucket(const HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;

	h = zend_string_hash_val(key);
	nIndex = h & ht->nTableMask;
	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = ht->arData + idx;
		if ((p->key == key) || /* check for the the same interned string */
			(p->h == h &&
			 p->key &&
			 p->key->len == key->len &&
			 memcmp(p->key->val, key->val, key->len) == 0)) {
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
	Bucket *p;

	nIndex = h & ht->nTableMask;
	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		ZEND_ASSERT(idx < ht->nTableSize);
		p = ht->arData + idx;
		if ((p->h == h)
			 && p->key
			 && (p->key->len == len)
			 && !memcmp(p->key->val, str, len)) {
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
	Bucket *p;

	nIndex = h & ht->nTableMask;
	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		ZEND_ASSERT(idx < ht->nTableSize);
		p = ht->arData + idx;
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
	if (ht->nInternalPointer == INVALID_IDX) {
		ht->nInternalPointer = idx;
	}
	p = ht->arData + idx;
	p->h = h = zend_string_hash_val(key);
	p->key = key;
	zend_string_addref(key);
	ZVAL_COPY_VALUE(&p->val, pData);
	nIndex = h & ht->nTableMask;
	Z_NEXT(p->val) = ht->arHash[nIndex];
	ht->arHash[nIndex] = idx;
	HANDLE_UNBLOCK_INTERRUPTIONS();

	return &p->val;
}

ZEND_API zval *_zend_hash_add_or_update(HashTable *ht, zend_string *key, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, flag ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_add(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_update(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_update_ind(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE | HASH_UPDATE_INDIRECT ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_add_new(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD_NEW ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_str_add_or_update(HashTable *ht, const char *str, size_t len, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, flag ZEND_FILE_LINE_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval *_zend_hash_str_update(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE ZEND_FILE_LINE_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval *_zend_hash_str_update_ind(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE | HASH_UPDATE_INDIRECT ZEND_FILE_LINE_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval *_zend_hash_str_add(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD ZEND_FILE_LINE_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval *_zend_hash_str_add_new(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD_NEW ZEND_FILE_LINE_CC);
	zend_string_release(key);
	return ret;
}

ZEND_API zval *zend_hash_index_add_empty_element(HashTable *ht, zend_ulong h)
{

	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_index_add(ht, h, &dummy);
}

ZEND_API zval *zend_hash_add_empty_element(HashTable *ht, zend_string *key)
{

	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_add(ht, key, &dummy);
}

ZEND_API zval *zend_hash_str_add_empty_element(HashTable *ht, const char *str, size_t len)
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
		if (ht->nInternalPointer == INVALID_IDX) {
			ht->nInternalPointer = h;
		}
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
	if (ht->nInternalPointer == INVALID_IDX) {
		ht->nInternalPointer = idx;
	}
	if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
		ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
	}
	p = ht->arData + idx;
	p->h = h;
	p->key = NULL;
	nIndex = h & ht->nTableMask;
	ZVAL_COPY_VALUE(&p->val, pData);
	Z_NEXT(p->val) = ht->arHash[nIndex];
	ht->arHash[nIndex] = idx;
	HANDLE_UNBLOCK_INTERRUPTIONS();

	return &p->val;
}

ZEND_API zval *_zend_hash_index_add_or_update(HashTable *ht, zend_ulong h, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, flag ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_index_add(HashTable *ht, zend_ulong h, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_ADD ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_index_add_new(HashTable *ht, zend_ulong h, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_ADD | HASH_ADD_NEW ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_index_update(HashTable *ht, zend_ulong h, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_UPDATE ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_next_index_insert(HashTable *ht, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, ht->nNextFreeElement, pData, HASH_ADD | HASH_ADD_NEXT ZEND_FILE_LINE_RELAY_CC);
}

ZEND_API zval *_zend_hash_next_index_insert_new(HashTable *ht, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, ht->nNextFreeElement, pData, HASH_ADD | HASH_ADD_NEW | HASH_ADD_NEXT ZEND_FILE_LINE_RELAY_CC);
}

static void zend_hash_do_resize(HashTable *ht)
{

	IS_CONSISTENT(ht);

	if (ht->nNumUsed > ht->nNumOfElements) {
		HANDLE_BLOCK_INTERRUPTIONS();
		zend_hash_rehash(ht);
		HANDLE_UNBLOCK_INTERRUPTIONS();
	} else if ((ht->nTableSize << 1) > 0) {	/* Let's double the table size */
		HANDLE_BLOCK_INTERRUPTIONS();
		ht->arData = (Bucket *) safe_perealloc(ht->arData, (ht->nTableSize << 1), sizeof(Bucket) + sizeof(uint32_t), 0, ht->u.flags & HASH_FLAG_PERSISTENT);
		ht->arHash = (uint32_t*)(ht->arData + (ht->nTableSize << 1));
		ht->nTableSize = (ht->nTableSize << 1);
		ht->nTableMask = ht->nTableSize - 1;
		zend_hash_rehash(ht);
		HANDLE_UNBLOCK_INTERRUPTIONS();
	}
}

ZEND_API int zend_hash_rehash(HashTable *ht)
{
	Bucket *p;
	uint32_t nIndex, i, j;

	IS_CONSISTENT(ht);

	if (UNEXPECTED(ht->nNumOfElements == 0)) {
		if (ht->u.flags & HASH_FLAG_INITIALIZED) {
			memset(ht->arHash, INVALID_IDX, ht->nTableSize * sizeof(uint32_t));
		}
		return SUCCESS;
	}

	memset(ht->arHash, INVALID_IDX, ht->nTableSize * sizeof(uint32_t));
	for (i = 0, j = 0; i < ht->nNumUsed; i++) {
		p = ht->arData + i;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		if (i != j) {
			ht->arData[j] = ht->arData[i];
			if (ht->nInternalPointer == i) {
				ht->nInternalPointer = j;
			}
		}
		nIndex = ht->arData[j].h & ht->nTableMask;
		Z_NEXT(ht->arData[j].val) = ht->arHash[nIndex];
		ht->arHash[nIndex] = j;
		j++;
	}
	ht->nNumUsed = j;
	return SUCCESS;
}

static zend_always_inline void _zend_hash_del_el_ex(HashTable *ht, uint32_t idx, Bucket *p, Bucket *prev)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	if (!(ht->u.flags & HASH_FLAG_PACKED)) {
		if (prev) {
			Z_NEXT(prev->val) = Z_NEXT(p->val);
		} else {
			ht->arHash[p->h & ht->nTableMask] = Z_NEXT(p->val);
		}
	}
	if (ht->nNumUsed - 1 == idx) {
		do {
			ht->nNumUsed--;
		} while (ht->nNumUsed > 0 && (Z_TYPE(ht->arData[ht->nNumUsed-1].val) == IS_UNDEF));
	}
	ht->nNumOfElements--;
	if (ht->nInternalPointer == idx) {
		while (1) {
			idx++;
			if (idx >= ht->nNumUsed) {
				ht->nInternalPointer = INVALID_IDX;
				break;
			} else if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
				ht->nInternalPointer = idx;
				break;
			}
		}
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
		uint32_t nIndex = p->h & ht->nTableMask;
		uint32_t i = ht->arHash[nIndex];

		if (i != idx) {
			prev = ht->arData + i;
			while (Z_NEXT(prev->val) != idx) {
				i = Z_NEXT(prev->val);
				prev = ht->arData + i;
			}
	 	}
	}

	_zend_hash_del_el_ex(ht, idx, p, prev);
}

ZEND_API int zend_hash_del(HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);

	h = zend_string_hash_val(key);
	nIndex = h & ht->nTableMask;

	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = ht->arData + idx;
		if ((p->key == key) ||
			(p->h == h &&
		     p->key &&
		     p->key->len == key->len &&
		     memcmp(p->key->val, key->val, key->len) == 0)) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int zend_hash_del_ind(HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);

	h = zend_string_hash_val(key);
	nIndex = h & ht->nTableMask;

	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = ht->arData + idx;
		if ((p->key == key) ||
			(p->h == h &&
		     p->key &&
		     p->key->len == key->len &&
		     memcmp(p->key->val, key->val, key->len) == 0)) {
			if (Z_TYPE(p->val) == IS_INDIRECT) {
				zval *data = Z_INDIRECT(p->val);

				if (Z_TYPE_P(data) == IS_UNDEF) {
					return FAILURE;
				} else {
					if (ht->pDestructor) {
						ht->pDestructor(data);
					}
					ZVAL_UNDEF(data);
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

ZEND_API int zend_hash_str_del(HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);

	h = zend_inline_hash_func(str, len);
	nIndex = h & ht->nTableMask;

	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = ht->arData + idx;
		if ((p->h == h)
			 && p->key
			 && (p->key->len == len)
			 && !memcmp(p->key->val, str, len)) {
			if (Z_TYPE(p->val) == IS_INDIRECT) {
				zval *data = Z_INDIRECT(p->val);

				if (Z_TYPE_P(data) == IS_UNDEF) {
					return FAILURE;
				} else {
					if (ht->pDestructor) {
						ht->pDestructor(data);
					}
					ZVAL_UNDEF(data);
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

ZEND_API int zend_hash_str_del_ind(HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);

	h = zend_inline_hash_func(str, len);
	nIndex = h & ht->nTableMask;

	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = ht->arData + idx;
		if ((p->h == h)
			 && p->key
			 && (p->key->len == len)
			 && !memcmp(p->key->val, str, len)) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int zend_hash_index_del(HashTable *ht, zend_ulong h)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (h < ht->nNumUsed) {
			p = ht->arData + h;
			if (Z_TYPE(p->val) != IS_UNDEF) {
				_zend_hash_del_el_ex(ht, h, p, NULL);
				return SUCCESS;
			}
		}
		return FAILURE;
	}
	nIndex = h & ht->nTableMask;

	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = ht->arData + idx;
		if ((p->h == h) && (p->key == NULL)) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API void zend_hash_destroy(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);

	if (ht->nNumUsed) {
		p = ht->arData;
		end = p + ht->nNumUsed;
		if (ht->pDestructor) {
			SET_INCONSISTENT(HT_IS_DESTROYING);

			if (ht->u.flags & HASH_FLAG_PACKED) {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						ht->pDestructor(&p->val);
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
			if (!(ht->u.flags & HASH_FLAG_PACKED)) {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					}
				} while (++p != end);
			}
		}
	} else if (EXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		return;
	}
	pefree(ht->arData, ht->u.flags & HASH_FLAG_PERSISTENT);
}

ZEND_API void zend_array_destroy(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);

	if (ht->nNumUsed) {

		/* In some rare cases destructors of regular arrays may be changed */
		if (UNEXPECTED(ht->pDestructor != ZVAL_PTR_DTOR)) {
			zend_hash_destroy(ht);
			return;
		}

		p = ht->arData;
		end = p + ht->nNumUsed;
		SET_INCONSISTENT(HT_IS_DESTROYING);

		if (ht->u.flags & HASH_FLAG_PACKED) {
			do {
				if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
					i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
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

		SET_INCONSISTENT(HT_DESTROYED);
	} else if (EXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		return;
	}
	pefree(ht->arData, ht->u.flags & HASH_FLAG_PERSISTENT);
}

ZEND_API void zend_hash_clean(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);

	if (ht->nNumUsed) {
		p = ht->arData;
		end = p + ht->nNumUsed;
		if (ht->pDestructor) {
			if (ht->u.flags & HASH_FLAG_PACKED) {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						ht->pDestructor(&p->val);
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
			if (!(ht->u.flags & HASH_FLAG_PACKED)) {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					}
				} while (++p != end);
			}
		}
		if (!(ht->u.flags & HASH_FLAG_PACKED)) {
			memset(ht->arHash, INVALID_IDX, ht->nTableSize * sizeof(uint32_t));
		}
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->nInternalPointer = INVALID_IDX;
}

ZEND_API void zend_symtable_clean(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);

	if (ht->nNumUsed) {
		p = ht->arData;
		end = p + ht->nNumUsed;
		do {
			if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
				i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
				if (EXPECTED(p->key)) {
					zend_string_release(p->key);
				}
			}
		} while (++p != end);
		if (!(ht->u.flags & HASH_FLAG_PACKED)) {
			memset(ht->arHash, INVALID_IDX, ht->nTableSize * sizeof(uint32_t));
		}
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->nInternalPointer = INVALID_IDX;
}

ZEND_API void zend_hash_graceful_destroy(HashTable *ht)
{
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);

	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		_zend_hash_del_el(ht, idx, p);
	}
	if (ht->u.flags & HASH_FLAG_INITIALIZED) {
		pefree(ht->arData, ht->u.flags & HASH_FLAG_PERSISTENT);
	}

	SET_INCONSISTENT(HT_DESTROYED);
}

ZEND_API void zend_hash_graceful_reverse_destroy(HashTable *ht)
{
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);

	idx = ht->nNumUsed;
	while (idx > 0) {
		idx--;
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		_zend_hash_del_el(ht, idx, p);
	}

	if (ht->u.flags & HASH_FLAG_INITIALIZED) {
		pefree(ht->arData, ht->u.flags & HASH_FLAG_PERSISTENT);
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

ZEND_API void zend_hash_apply(HashTable *ht, apply_func_t apply_func)
{
	uint32_t idx;
	Bucket *p;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;

		result = apply_func(&p->val);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			_zend_hash_del_el(ht, idx, p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_apply_with_argument(HashTable *ht, apply_func_arg_t apply_func, void *argument)
{
    uint32_t idx;
	Bucket *p;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;

		result = apply_func(&p->val, argument);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			_zend_hash_del_el(ht, idx, p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_apply_with_arguments(HashTable *ht, apply_func_args_t apply_func, int num_args, ...)
{
	uint32_t idx;
	Bucket *p;
	va_list args;
	zend_hash_key hash_key;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);

	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		va_start(args, num_args);
		hash_key.h = p->h;
		hash_key.key = p->key;

		result = apply_func(&p->val, num_args, args, &hash_key);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			_zend_hash_del_el(ht, idx, p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			va_end(args);
			break;
		}
		va_end(args);
	}

	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_reverse_apply(HashTable *ht, apply_func_t apply_func)
{
	uint32_t idx;
	Bucket *p;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	idx = ht->nNumUsed;
	while (idx > 0) {
		idx--;
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;

		result = apply_func(&p->val);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			_zend_hash_del_el(ht, idx, p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor)
{
    uint32_t idx;
	Bucket *p;
	zval *new_entry, *data;
	zend_bool setTargetPointer;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

	setTargetPointer = (target->nInternalPointer == INVALID_IDX);
	for (idx = 0; idx < source->nNumUsed; idx++) {
		p = source->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;

		if (setTargetPointer && source->nInternalPointer == idx) {
			target->nInternalPointer = INVALID_IDX;
		}
		/* INDIRECT element may point to UNDEF-ined slots */
		data = &p->val;
		if (Z_TYPE_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
			if (Z_TYPE_P(data) == IS_UNDEF) {
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
	if (target->nInternalPointer == INVALID_IDX && target->nNumOfElements > 0) {
		idx = 0;
		while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
			idx++;
		}
		target->nInternalPointer = idx;
	}
}


ZEND_API void zend_array_dup(HashTable *target, HashTable *source)
{
    uint32_t idx, target_idx;
	uint32_t nIndex;
	Bucket *p, *q;
	zval *data;

	IS_CONSISTENT(source);

	target->nTableMask = source->nTableMask;
	target->nTableSize = source->nTableSize;
	target->pDestructor = source->pDestructor;
	target->nInternalPointer = INVALID_IDX;
	target->u.flags = (source->u.flags & ~HASH_FLAG_PERSISTENT) | HASH_FLAG_APPLY_PROTECTION;

	target_idx = 0;
	if (target->u.flags & HASH_FLAG_INITIALIZED) {
		if (target->u.flags & HASH_FLAG_PACKED) {
			target->nNumUsed = source->nNumUsed;
			target->nNumOfElements = source->nNumOfElements;
			target->nNextFreeElement = source->nNextFreeElement;
			target->arData = (Bucket *) safe_pemalloc(target->nTableSize, sizeof(Bucket), 0, 0);
			target->arHash = (uint32_t*)&uninitialized_bucket;
			target->nInternalPointer = source->nInternalPointer;

			for (idx = 0; idx < source->nNumUsed; idx++) {
				p = source->arData + idx;
				q = target->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) {
					ZVAL_UNDEF(&q->val);
					continue;
				}
				/* INDIRECT element may point to UNDEF-ined slots */
				data = &p->val;
				if (Z_TYPE_P(data) == IS_INDIRECT) {
					data = Z_INDIRECT_P(data);
					if (Z_TYPE_P(data) == IS_UNDEF) {
						ZVAL_UNDEF(&q->val);
						continue;
					}
				}

				q->h = p->h;
				q->key = NULL;
				if (Z_OPT_REFCOUNTED_P(data)) {
					if (Z_ISREF_P(data) && Z_REFCOUNT_P(data) == 1) {
						ZVAL_COPY(&q->val, Z_REFVAL_P(data));
					} else {
						ZVAL_COPY(&q->val, data);
					}
				} else {
					ZVAL_COPY_VALUE(&q->val, data);
				}
			}
			if (target->nNumOfElements > 0 &&
			    target->nInternalPointer == INVALID_IDX) {
				idx = 0;
				while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
					idx++;
				}
				target->nInternalPointer = idx;
			}
		} else {
			target->nNextFreeElement = source->nNextFreeElement;
			target->arData = (Bucket *) safe_pemalloc(target->nTableSize, sizeof(Bucket) + sizeof(uint32_t), 0, 0);
			target->arHash = (uint32_t*)(target->arData + target->nTableSize);
			memset(target->arHash, INVALID_IDX, target->nTableSize * sizeof(uint32_t));

			for (idx = 0; idx < source->nNumUsed; idx++) {
				p = source->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				/* INDIRECT element may point to UNDEF-ined slots */
				data = &p->val;
				if (Z_TYPE_P(data) == IS_INDIRECT) {
					data = Z_INDIRECT_P(data);
					if (Z_TYPE_P(data) == IS_UNDEF) {
						continue;
					}
				}

				if (source->nInternalPointer == idx) {
					target->nInternalPointer = target_idx;
				}

				q = target->arData + target_idx;
				q->h = p->h;
				q->key = p->key;
				if (q->key) {
					zend_string_addref(q->key);
				}
				nIndex = q->h & target->nTableMask;
				Z_NEXT(q->val) = target->arHash[nIndex];
				target->arHash[nIndex] = target_idx;
				if (Z_OPT_REFCOUNTED_P(data)) {
					if (Z_ISREF_P(data) && Z_REFCOUNT_P(data) == 1) {
						ZVAL_COPY(&q->val, Z_REFVAL_P(data));
					} else {
						ZVAL_COPY(&q->val, data);
					}
				} else {
					ZVAL_COPY_VALUE(&q->val, data);
				}
				target_idx++;
			}
			target->nNumUsed = target_idx;
			target->nNumOfElements = target_idx;
			if (target->nNumOfElements > 0 &&
			    target->nInternalPointer == INVALID_IDX) {
				target->nInternalPointer = 0;
			}
		}
	} else {
		target->nNumUsed = 0;
		target->nNumOfElements = 0;
		target->nNextFreeElement = 0;
		target->arData = NULL;
		target->arHash = (uint32_t*)&uninitialized_bucket;
	}
}


ZEND_API void _zend_hash_merge(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, zend_bool overwrite ZEND_FILE_LINE_DC)
{
    uint32_t idx;
	Bucket *p;
	zval *t;
	uint32_t mode = (overwrite?HASH_UPDATE:HASH_ADD);

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

	for (idx = 0; idx < source->nNumUsed; idx++) {
		p = source->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
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


static zend_bool zend_hash_replace_checker_wrapper(HashTable *target, zval *source_data, Bucket *p, void *pParam, merge_checker_func_t merge_checker_func)
{
	zend_hash_key hash_key;

	hash_key.h = p->h;
	hash_key.key = p->key;
	return merge_checker_func(target, source_data, &hash_key, pParam);
}


ZEND_API void zend_hash_merge_ex(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, merge_checker_func_t pMergeSource, void *pParam)
{
	uint32_t idx;
	Bucket *p;
	zval *t;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

	for (idx = 0; idx < source->nNumUsed; idx++) {
		p = source->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
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


/* Returns SUCCESS if found and FAILURE if not. The pointer to the
 * data is returned in pData. The reason is that there's no reason
 * someone using the hash table might not want to have NULL data
 */
ZEND_API zval *zend_hash_find(const HashTable *ht, zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = zend_hash_find_bucket(ht, key);
	return p ? &p->val : NULL;
}

ZEND_API zval *zend_hash_str_find(const HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	Bucket *p;

	IS_CONSISTENT(ht);

	h = zend_inline_hash_func(str, len);
	p = zend_hash_str_find_bucket(ht, str, len, h);
	return p ? &p->val : NULL;
}

ZEND_API zend_bool zend_hash_exists(const HashTable *ht, zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = zend_hash_find_bucket(ht, key);
	return p ? 1 : 0;
}

ZEND_API zend_bool zend_hash_str_exists(const HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	Bucket *p;

	IS_CONSISTENT(ht);

	h = zend_inline_hash_func(str, len);
	p = zend_hash_str_find_bucket(ht, str, len, h);
	return p ? 1 : 0;
}

ZEND_API zval *zend_hash_index_find(const HashTable *ht, zend_ulong h)
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


ZEND_API zend_bool zend_hash_index_exists(const HashTable *ht, zend_ulong h)
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


ZEND_API void zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos)
{
    uint32_t idx;

	IS_CONSISTENT(ht);
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
			*pos = idx;
			return;
		}
	}
	*pos = INVALID_IDX;
}


/* This function will be extremely optimized by remembering
 * the end of the list
 */
ZEND_API void zend_hash_internal_pointer_end_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx;

	IS_CONSISTENT(ht);

	idx = ht->nNumUsed;
	while (idx > 0) {
		idx--;
		if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
			*pos = idx;
			return;
		}
	}
	*pos = INVALID_IDX;
}


ZEND_API int zend_hash_move_forward_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);

	if (idx != INVALID_IDX) {
		while (1) {
			idx++;
			if (idx >= ht->nNumUsed) {
				*pos = INVALID_IDX;
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

ZEND_API int zend_hash_move_backwards_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);

	if (idx != INVALID_IDX) {
		while (idx > 0) {
			idx--;
			if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
				*pos = idx;
				return SUCCESS;
			}
		}
		*pos = INVALID_IDX;
 		return SUCCESS;
	} else {
 		return FAILURE;
	}
}


/* This function should be made binary safe  */
ZEND_API int zend_hash_get_current_key_ex(const HashTable *ht, zend_string **str_index, zend_ulong *num_index, HashPosition *pos)
{
	uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx != INVALID_IDX) {
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

ZEND_API void zend_hash_get_current_key_zval_ex(const HashTable *ht, zval *key, HashPosition *pos)
{
	uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx == INVALID_IDX) {
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

ZEND_API int zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos)
{
    uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx != INVALID_IDX) {
		p = ht->arData + idx;
		if (p->key) {
			return HASH_KEY_IS_STRING;
		} else {
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTENT;
}


ZEND_API zval *zend_hash_get_current_data_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx != INVALID_IDX) {
		p = ht->arData + idx;
		return &p->val;
	} else {
		return NULL;
	}
}

ZEND_API void zend_hash_bucket_swap(Bucket *p, Bucket *q) {
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

ZEND_API void zend_hash_bucket_renum_swap(Bucket *p, Bucket *q) {
	zval val;

	ZVAL_COPY_VALUE(&val, &p->val);
	ZVAL_COPY_VALUE(&p->val, &q->val);
	ZVAL_COPY_VALUE(&q->val, &val);
}

ZEND_API void zend_hash_bucket_packed_swap(Bucket *p, Bucket *q) {
	zval val;
	zend_ulong h;

	ZVAL_COPY_VALUE(&val, &p->val);
	h = p->h;

	ZVAL_COPY_VALUE(&p->val, &q->val);
	p->h = q->h;

	ZVAL_COPY_VALUE(&q->val, &val);
	q->h = h;
}

ZEND_API int zend_hash_sort_ex(HashTable *ht, sort_func_t sort, compare_func_t compar, zend_bool renumber)
{
	Bucket *p;
	uint32_t i, j;

	IS_CONSISTENT(ht);

	if (!(ht->nNumOfElements>1) && !(renumber && ht->nNumOfElements>0)) { /* Doesn't require sorting */
		return SUCCESS;
	}

	if (ht->nNumUsed == ht->nNumOfElements) {
		i = ht->nNumUsed;
	} else {
		for (j = 0, i = 0; j < ht->nNumUsed; j++) {
			p = ht->arData + j;
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
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
			ht->u.flags |= HASH_FLAG_PACKED;
			ht->nTableMask = 0;
			ht->arData = perealloc(ht->arData, ht->nTableSize * sizeof(Bucket), ht->u.flags & HASH_FLAG_PERSISTENT);
			ht->arHash = (uint32_t*)&uninitialized_bucket;
		} else {
			zend_hash_rehash(ht);
		}
	}

	HANDLE_UNBLOCK_INTERRUPTIONS();

	return SUCCESS;
}


ZEND_API int zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_func_t compar, zend_bool ordered)
{
	uint32_t idx1, idx2;
	Bucket *p1, *p2 = NULL;
	int result;
	zval *pData1, *pData2;

	IS_CONSISTENT(ht1);
	IS_CONSISTENT(ht2);

	HASH_PROTECT_RECURSION(ht1);
	HASH_PROTECT_RECURSION(ht2);

	result = ht1->nNumOfElements - ht2->nNumOfElements;
	if (result!=0) {
		HASH_UNPROTECT_RECURSION(ht1);
		HASH_UNPROTECT_RECURSION(ht2);
		return result;
	}

	for (idx1 = 0, idx2 = 0; idx1 < ht1->nNumUsed; idx1++) {
		p1 = ht1->arData + idx1;
		if (Z_TYPE(p1->val) == IS_UNDEF) continue;

		if (ordered) {
			while (1) {
				p2 = ht2->arData + idx2;
				if (idx2 == ht2->nNumUsed) {
					HASH_UNPROTECT_RECURSION(ht1);
					HASH_UNPROTECT_RECURSION(ht2);
					return 1; /* That's not supposed to happen */
				}
				if (Z_TYPE(p2->val) != IS_UNDEF) break;
				idx2++;
			}
			if (p1->key == NULL && p2->key == NULL) { /* numeric indices */
				result = p1->h - p2->h;
				if (result != 0) {
					HASH_UNPROTECT_RECURSION(ht1);
					HASH_UNPROTECT_RECURSION(ht2);
					return result;
				}
			} else { /* string indices */
				size_t len0 = (p1->key ? p1->key->len : 0);
				size_t len1 = (p2->key ? p2->key->len : 0);
				if (len0 != len1) {
					HASH_UNPROTECT_RECURSION(ht1);
					HASH_UNPROTECT_RECURSION(ht2);
					return len0 > len1 ? 1 : -1;
				}
				result = memcmp(p1->key->val, p2->key->val, p1->key->len);
				if (result != 0) {
					HASH_UNPROTECT_RECURSION(ht1);
					HASH_UNPROTECT_RECURSION(ht2);
					return result;
				}
			}
			pData2 = &p2->val;
		} else {
			if (p1->key == NULL) { /* numeric index */
				pData2 = zend_hash_index_find(ht2, p1->h);
				if (pData2 == NULL) {
					HASH_UNPROTECT_RECURSION(ht1);
					HASH_UNPROTECT_RECURSION(ht2);
					return 1;
				}
			} else { /* string index */
				pData2 = zend_hash_find(ht2, p1->key);
				if (pData2 == NULL) {
					HASH_UNPROTECT_RECURSION(ht1);
					HASH_UNPROTECT_RECURSION(ht2);
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
		}
		if (result != 0) {
			HASH_UNPROTECT_RECURSION(ht1);
			HASH_UNPROTECT_RECURSION(ht2);
			return result;
		}
		if (ordered) {
			idx2++;
		}
	}

	HASH_UNPROTECT_RECURSION(ht1);
	HASH_UNPROTECT_RECURSION(ht2);
	return 0;
}


ZEND_API zval *zend_hash_minmax(const HashTable *ht, compare_func_t compar, uint32_t flag)
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
		if (Z_TYPE(p->val) == IS_UNDEF) continue;

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

ZEND_API int _zend_handle_numeric_str_ex(const char *key, size_t length, zend_ulong *idx)
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
