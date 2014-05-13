/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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

#define CHECK_INIT(ht, packed) do {												\
	if (UNEXPECTED((ht)->nTableMask == 0)) {							\
		if (packed) { \
			(ht)->arData = (Bucket *) safe_pemalloc((ht)->nTableSize, sizeof(Bucket), 0, (ht)->u.flags & HASH_FLAG_PERSISTENT);	\
			(ht)->u.flags |= HASH_FLAG_PACKED; \
		} else { \
			(ht)->arData = (Bucket *) safe_pemalloc((ht)->nTableSize, sizeof(Bucket) + sizeof(zend_uint), 0, (ht)->u.flags & HASH_FLAG_PERSISTENT);	\
			(ht)->arHash = (zend_uint*)((ht)->arData + (ht)->nTableSize);	\
			memset((ht)->arHash, INVALID_IDX, (ht)->nTableSize * sizeof(zend_uint));	\
		} \
		(ht)->nTableMask = (ht)->nTableSize - 1;						\
	}																	\
} while (0)
 
static const zend_uint uninitialized_bucket = {INVALID_IDX};

ZEND_API void _zend_hash_init(HashTable *ht, uint nSize, dtor_func_t pDestructor, zend_bool persistent ZEND_FILE_LINE_DC)
{
	uint i = 3;

	SET_INCONSISTENT(HT_OK);

	if (nSize >= 0x80000000) {
		/* prevent overflow */
		ht->nTableSize = 0x80000000;
	} else {
		while ((1U << i) < nSize) {
			i++;
		}
		ht->nTableSize = 1 << i;
	}

	ht->nTableMask = 0;	/* 0 means that ht->arBuckets is uninitialized */
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->arData = NULL;
	ht->arHash = (zend_uint*)&uninitialized_bucket;
	ht->pDestructor = pDestructor;
	ht->nInternalPointer = INVALID_IDX;
	if (persistent) {
		ht->u.flags = HASH_FLAG_PERSISTENT | HASH_FLAG_APPLY_PROTECTION;
	} else {
		ht->u.flags = HASH_FLAG_APPLY_PROTECTION;
	}
}

static void zend_hash_packed_grow(HashTable *ht)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->arData = (Bucket *) safe_perealloc(ht->arData, (ht->nTableSize << 1), sizeof(Bucket), 0, ht->u.flags & HASH_FLAG_PERSISTENT);
	ht->nTableSize = (ht->nTableSize << 1);
	ht->nTableMask = ht->nTableSize - 1;
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void zend_hash_real_init(HashTable *ht, int packed)
{
	IS_CONSISTENT(ht);

	CHECK_INIT(ht, packed);
}

ZEND_API void zend_hash_packed_to_hash(HashTable *ht)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->u.flags &= ~HASH_FLAG_PACKED;
	ht->arData = (Bucket *) safe_perealloc(ht->arData, ht->nTableSize, sizeof(Bucket) + sizeof(zend_uint), 0, ht->u.flags & HASH_FLAG_PERSISTENT);
	ht->arHash = (zend_uint*)(ht->arData + ht->nTableSize);
	zend_hash_rehash(ht);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void zend_hash_to_packed(HashTable *ht)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->u.flags |= HASH_FLAG_PACKED;
	ht->arData = erealloc(ht->arData, ht->nTableSize * sizeof(Bucket));
	ht->arHash = (zend_uint*)&uninitialized_bucket;
	HANDLE_UNBLOCK_INTERRUPTIONS();
}

ZEND_API void _zend_hash_init_ex(HashTable *ht, uint nSize, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC)
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
	ulong h;
	uint nIndex;
	uint idx;
	Bucket *p;

	h = STR_HASH_VAL(key);
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

static zend_always_inline Bucket *zend_hash_str_find_bucket(const HashTable *ht, const char *str, int len, ulong h)
{
	uint nIndex;
	uint idx;
	Bucket *p;

	nIndex = h & ht->nTableMask;
	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
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

static zend_always_inline Bucket *zend_hash_index_find_bucket(const HashTable *ht, ulong h)
{
	uint nIndex;
	uint idx;
	Bucket *p;

	nIndex = h & ht->nTableMask;
	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = ht->arData + idx;
		if (p->h == h && !p->key) {
			return p;
		}
		idx = Z_NEXT(p->val);
	}
	return NULL;
}

ZEND_API zval *_zend_hash_add_or_update(HashTable *ht, zend_string *key, zval *pData, int flag ZEND_FILE_LINE_DC)
{
	ulong h;
	uint nIndex;
	uint idx;
	Bucket *p;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	IS_CONSISTENT(ht);

	CHECK_INIT(ht, 0);
	if (ht->u.flags & HASH_FLAG_PACKED) {
		zend_hash_packed_to_hash(ht);
	}

	h = STR_HASH_VAL(key);
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
	
	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */

	HANDLE_BLOCK_INTERRUPTIONS();
	idx = ht->nNumUsed++;
	ht->nNumOfElements++;
	if (ht->nInternalPointer == INVALID_IDX) {
		ht->nInternalPointer = idx;
	}
	p = ht->arData + idx; 
	p->h = h;
	p->key = key;
	STR_ADDREF(key);
	ZVAL_COPY_VALUE(&p->val, pData);
	nIndex = h & ht->nTableMask;
	Z_NEXT(p->val) = ht->arHash[nIndex];
	ht->arHash[nIndex] = idx;
	HANDLE_UNBLOCK_INTERRUPTIONS();

	return &p->val;
}

ZEND_API zval *_zend_hash_str_add_or_update(HashTable *ht, const char *str, int len, zval *pData, int flag ZEND_FILE_LINE_DC)
{
	zend_string *key = STR_INIT(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update(ht, key, pData, flag ZEND_FILE_LINE_CC);
	STR_RELEASE(key);
	return ret;
}

ZEND_API zval *zend_hash_index_add_empty_element(HashTable *ht, ulong h)
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

ZEND_API zval *zend_hash_str_add_empty_element(HashTable *ht, const char *str, int len)
{
	
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_str_add(ht, str, len, &dummy);
}

ZEND_API zval *_zend_hash_index_update_or_next_insert(HashTable *ht, ulong h, zval *pData, int flag ZEND_FILE_LINE_DC)
{
	uint nIndex;
	uint idx;
	Bucket *p;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	IS_CONSISTENT(ht);

	if (flag & HASH_NEXT_INSERT) {
		h = ht->nNextFreeElement;
	}
	CHECK_INIT(ht, h >= 0 && h < ht->nTableSize);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (EXPECTED(h >= 0))  {
			if (h < ht->nNumUsed) {
				p = ht->arData + h;
				if (Z_TYPE(p->val) != IS_UNDEF) {
					if (flag & (HASH_NEXT_INSERT | HASH_ADD)) {
						return NULL;
					}
					if (ht->pDestructor) {
						ht->pDestructor(&p->val);
					}
					ZVAL_COPY_VALUE(&p->val, pData);
					if ((long)h >= (long)ht->nNextFreeElement) {
						ht->nNextFreeElement = h < LONG_MAX ? h + 1 : LONG_MAX;
					}
					return &p->val;
				} else { /* we have to keep the order :( */
				    goto convert_to_hash;
				}
			} else if (EXPECTED(h < ht->nTableSize)) {
				p = ht->arData + h;
			} else if (h < ht->nTableSize * 2 &&
			           ht->nTableSize - ht->nNumOfElements < ht->nTableSize / 2) {
				zend_hash_packed_grow(ht);
				p = ht->arData + h;
			} else {
			    goto convert_to_hash;
			}
			HANDLE_BLOCK_INTERRUPTIONS();
			/* incremental initialization of empty Buckets */
			if (h >= ht->nNumUsed) {
				Bucket *q = ht->arData + ht->nNumUsed;
				while (q != p) {
					ZVAL_UNDEF(&q->val);
					q++;
				}
				ht->nNumUsed = h + 1;
			}
			ht->nNumOfElements++;
			if (ht->nInternalPointer == INVALID_IDX) {
				ht->nInternalPointer = h;
			}
			if ((long)h >= (long)ht->nNextFreeElement) {
				ht->nNextFreeElement = h < LONG_MAX ? h + 1 : LONG_MAX;
			}
			p->h = h;
			p->key = NULL;
			ZVAL_COPY_VALUE(&p->val, pData);
			Z_NEXT(p->val) = INVALID_IDX;

			HANDLE_UNBLOCK_INTERRUPTIONS();

			return &p->val;
		} else {
convert_to_hash:
			zend_hash_packed_to_hash(ht);
		}
	}

	p = zend_hash_index_find_bucket(ht, h);
	if (p) {
		if (flag & (HASH_NEXT_INSERT | HASH_ADD)) {
			return NULL;
		}
		ZEND_ASSERT(&p->val != pData);
		HANDLE_BLOCK_INTERRUPTIONS();
		if (ht->pDestructor) {
			ht->pDestructor(&p->val);
		}
		ZVAL_COPY_VALUE(&p->val, pData);
		HANDLE_UNBLOCK_INTERRUPTIONS();
		if ((long)h >= (long)ht->nNextFreeElement) {
			ht->nNextFreeElement = h < LONG_MAX ? h + 1 : LONG_MAX;
		}
		return &p->val;
	}

	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */

	HANDLE_BLOCK_INTERRUPTIONS();
	idx = ht->nNumUsed++;
	ht->nNumOfElements++;
	if (ht->nInternalPointer == INVALID_IDX) {
		ht->nInternalPointer = idx;
	}
	if ((long)h >= (long)ht->nNextFreeElement) {
		ht->nNextFreeElement = h < LONG_MAX ? h + 1 : LONG_MAX;
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


static void zend_hash_do_resize(HashTable *ht)
{
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	IS_CONSISTENT(ht);

	if (ht->nNumUsed < ht->nNumOfElements) {
		HANDLE_BLOCK_INTERRUPTIONS();
		zend_hash_rehash(ht);
		HANDLE_UNBLOCK_INTERRUPTIONS();
	} else if ((ht->nTableSize << 1) > 0) {	/* Let's double the table size */
		HANDLE_BLOCK_INTERRUPTIONS();
		ht->arData = (Bucket *) safe_perealloc(ht->arData, (ht->nTableSize << 1), sizeof(Bucket) + sizeof(zend_uint), 0, ht->u.flags & HASH_FLAG_PERSISTENT);
		ht->arHash = (zend_uint*)(ht->arData + (ht->nTableSize << 1));
		ht->nTableSize = (ht->nTableSize << 1);
		ht->nTableMask = ht->nTableSize - 1;
		zend_hash_rehash(ht);
		HANDLE_UNBLOCK_INTERRUPTIONS();
	}
}

ZEND_API int zend_hash_rehash(HashTable *ht)
{
	Bucket *p;
	uint nIndex, i, j;

	IS_CONSISTENT(ht);
	memset(ht->arHash, INVALID_IDX, ht->nTableSize * sizeof(zend_uint));

	if (UNEXPECTED(ht->nNumOfElements == 0)) {
		return SUCCESS;
	}

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

static zend_always_inline void _zend_hash_del_el_ex(HashTable *ht, uint idx, Bucket *p, Bucket *prev)
{
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
		STR_RELEASE(p->key);
	}
	if (ht->pDestructor) {
		zval tmp;
		ZVAL_COPY_VALUE(&tmp, &p->val);
		ZVAL_UNDEF(&p->val);
		ht->pDestructor(&tmp);
	} else {
		ZVAL_UNDEF(&p->val);
	}
}

static zend_always_inline void _zend_hash_del_el(HashTable *ht, uint idx, Bucket *p)
{
	uint nIndex;
	Bucket *prev = NULL;

	if (!(ht->u.flags & HASH_FLAG_PACKED)) {
		nIndex = p->h & ht->nTableMask;
		idx = ht->arHash[nIndex];
		if (p != ht->arData + idx) {
			prev = ht->arData + idx;
			while (ht->arData + Z_NEXT(prev->val) != p) {
				idx = Z_NEXT(prev->val);
				prev = ht->arData + idx;
			}
			idx = Z_NEXT(prev->val);
	 	}
	}

	_zend_hash_del_el_ex(ht, idx, p, prev);
}

ZEND_API int zend_hash_del(HashTable *ht, zend_string *key)
{
	ulong h;
	uint nIndex;
	uint idx;
	Bucket *p;
	Bucket *prev = NULL;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		return FAILURE;
	}

	h = STR_HASH_VAL(key);
	nIndex = h & ht->nTableMask;

	idx = ht->arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = ht->arData + idx;
		if ((p->key == key) ||
			(p->h == h &&
		     p->key &&
		     p->key->len == key->len &&
		     memcmp(p->key->val, key->val, key->len) == 0)) {
			HANDLE_BLOCK_INTERRUPTIONS();
			_zend_hash_del_el_ex(ht, idx, p, prev);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int zend_hash_del_ind(HashTable *ht, zend_string *key)
{
	ulong h;
	uint nIndex;
	uint idx;
	Bucket *p;
	Bucket *prev = NULL;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		return FAILURE;
	}

	h = STR_HASH_VAL(key);
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
				HANDLE_BLOCK_INTERRUPTIONS();
				_zend_hash_del_el_ex(ht, idx, p, prev);
				HANDLE_UNBLOCK_INTERRUPTIONS();
			}
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int zend_hash_str_del(HashTable *ht, const char *str, int len)
{
	ulong h;
	uint nIndex;
	uint idx;
	Bucket *p;
	Bucket *prev = NULL;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		return FAILURE;
	}

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
				HANDLE_BLOCK_INTERRUPTIONS();
				_zend_hash_del_el_ex(ht, idx, p, prev);
				HANDLE_UNBLOCK_INTERRUPTIONS();
			}
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int zend_hash_str_del_ind(HashTable *ht, const char *str, int len)
{
	ulong h;
	uint nIndex;
	uint idx;
	Bucket *p;
	Bucket *prev = NULL;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

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
			HANDLE_BLOCK_INTERRUPTIONS();
			_zend_hash_del_el_ex(ht, idx, p, prev);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API int zend_hash_index_del(HashTable *ht, ulong h)
{
	uint nIndex;
	uint idx;
	Bucket *p;
	Bucket *prev = NULL;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (h >=0 && h < ht->nNumUsed) {
			p = ht->arData + h;
			if (Z_TYPE(p->val) != IS_UNDEF) {
				HANDLE_BLOCK_INTERRUPTIONS();
				_zend_hash_del_el_ex(ht, h, p, NULL);
				HANDLE_UNBLOCK_INTERRUPTIONS();
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
			HANDLE_BLOCK_INTERRUPTIONS();
			_zend_hash_del_el_ex(ht, idx, p, prev);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API void zend_hash_destroy(HashTable *ht)
{
	uint idx;
	Bucket *p;

	IS_CONSISTENT(ht);

	SET_INCONSISTENT(HT_IS_DESTROYING);

	for (idx = 0 ; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		if (ht->pDestructor) {
			ht->pDestructor(&p->val);
		}
		if (p->key) {
			STR_RELEASE(p->key);
		}
	}
	if (ht->nTableMask) {
		pefree(ht->arData, ht->u.flags & HASH_FLAG_PERSISTENT);
	}

	SET_INCONSISTENT(HT_DESTROYED);
}


ZEND_API void zend_hash_clean(HashTable *ht)
{
	uint idx;
	Bucket *p;

	IS_CONSISTENT(ht);

	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		if (ht->pDestructor) {
			ht->pDestructor(&p->val);
		}
		if (p->key) {
			STR_RELEASE(p->key);
		}
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->nInternalPointer = INVALID_IDX;
	if (ht->nTableMask) {
		if (!(ht->u.flags & HASH_FLAG_PACKED)) {
			memset(ht->arHash, INVALID_IDX, ht->nTableSize * sizeof(zend_uint));	
		}
	}
}

/* This function is used by the various apply() functions.
 * It deletes the passed bucket, and returns the address of the
 * next bucket.  The hash *may* be altered during that time, the
 * returned value will still be valid.
 */
static void zend_hash_apply_deleter(HashTable *ht, uint idx, Bucket *p)
{
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	HANDLE_BLOCK_INTERRUPTIONS();
	_zend_hash_del_el(ht, idx, p);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}


ZEND_API void zend_hash_graceful_destroy(HashTable *ht)
{
	uint idx;
	Bucket *p;

	IS_CONSISTENT(ht);

	for (idx = 0; idx < ht->nNumUsed; idx++) {		
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		zend_hash_apply_deleter(ht, idx, p);
	}
	if (ht->nTableMask) {
		pefree(ht->arData, ht->u.flags & HASH_FLAG_PERSISTENT);
	}

	SET_INCONSISTENT(HT_DESTROYED);
}

ZEND_API void zend_hash_graceful_reverse_destroy(HashTable *ht)
{
	uint idx;
	Bucket *p;

	IS_CONSISTENT(ht);

	idx = ht->nNumUsed;
	while (idx > 0) {
		idx--;
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		zend_hash_apply_deleter(ht, idx, p);
	}

	if (ht->nTableMask) {
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

ZEND_API void zend_hash_apply(HashTable *ht, apply_func_t apply_func TSRMLS_DC)
{
	uint idx;
	Bucket *p;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		
		result = apply_func(&p->val TSRMLS_CC);
		
		if (result & ZEND_HASH_APPLY_REMOVE) {
			zend_hash_apply_deleter(ht, idx, p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_apply_with_argument(HashTable *ht, apply_func_arg_t apply_func, void *argument TSRMLS_DC)
{
    uint idx;
	Bucket *p;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;

		result = apply_func(&p->val, argument TSRMLS_CC);
		
		if (result & ZEND_HASH_APPLY_REMOVE) {
			zend_hash_apply_deleter(ht, idx, p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_apply_with_arguments(HashTable *ht TSRMLS_DC, apply_func_args_t apply_func, int num_args, ...)
{
	uint idx;
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

		result = apply_func(&p->val TSRMLS_CC, num_args, args, &hash_key);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			zend_hash_apply_deleter(ht, idx, p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			va_end(args);
			break;
		}
		va_end(args);
	}

	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_reverse_apply(HashTable *ht, apply_func_t apply_func TSRMLS_DC)
{
	uint idx;
	Bucket *p;
	int result;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	idx = ht->nNumUsed;
	while (idx > 0) {
		idx--;
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		
		result = apply_func(&p->val TSRMLS_CC);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			zend_hash_apply_deleter(ht, idx, p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor)
{
    uint idx;
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


ZEND_API void _zend_hash_merge(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, int overwrite ZEND_FILE_LINE_DC)
{
    uint idx;
	Bucket *p;
	zval *t;
	int mode = (overwrite?HASH_UPDATE:HASH_ADD);

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
	uint idx;
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

	if (ht->u.flags & HASH_FLAG_PACKED) {
		return NULL;
	}

	p = zend_hash_find_bucket(ht, key);
	return p ? &p->val : NULL;
}

ZEND_API zval *zend_hash_str_find(const HashTable *ht, const char *str, int len)
{
	ulong h;
	Bucket *p;

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		return NULL;
	}

	h = zend_inline_hash_func(str, len);
	p = zend_hash_str_find_bucket(ht, str, len, h);
	return p ? &p->val : NULL;
}

ZEND_API int zend_hash_exists(const HashTable *ht, zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		return 0;
	}

	p = zend_hash_find_bucket(ht, key);
	return p ? 1 : 0;
}

ZEND_API int zend_hash_str_exists(const HashTable *ht, const char *str, int len)
{
	ulong h;
	Bucket *p;

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		return 0;
	}

	h = zend_inline_hash_func(str, len);
	p = zend_hash_str_find_bucket(ht, str, len, h);
	return p ? 1 : 0;
}

ZEND_API zval *zend_hash_index_find(const HashTable *ht, ulong h)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (h >= 0 && h < ht->nNumUsed) {
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


ZEND_API int zend_hash_index_exists(const HashTable *ht, ulong h)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (h >= 0 && h < ht->nNumUsed) {
			if (Z_TYPE(ht->arData[h].val) != IS_UNDEF) {
				return 1;
			}
		}
		return 0;
	}

	p = zend_hash_index_find_bucket(ht, h);
	return p ? 1 : 0;
}


ZEND_API int zend_hash_num_elements(const HashTable *ht)
{
	IS_CONSISTENT(ht);

	return ht->nNumOfElements;
}


ZEND_API int zend_hash_get_pointer(const HashTable *ht, HashPointer *ptr)
{
	ptr->pos = ht->nInternalPointer;
	ptr->ht = (HashTable*)ht;
	if (ht->nInternalPointer != INVALID_IDX) {
		ptr->h = ht->arData[ht->nInternalPointer].h;
		return 1;
	} else {
		ptr->h = 0;
		return 0;
	}
}

ZEND_API int zend_hash_set_pointer(HashTable *ht, const HashPointer *ptr)
{
	uint idx;

	if (ptr->pos == INVALID_IDX) {
		ht->nInternalPointer = INVALID_IDX;
	} else if (ptr->ht != ht) {
		IS_CONSISTENT(ht);
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
				ht->nInternalPointer = idx;
				return 0;
			}
		}
		idx = INVALID_IDX;
		return 0;
	} else if (ht->nInternalPointer != ptr->pos) {
		IS_CONSISTENT(ht);
		if (ht->u.flags & HASH_FLAG_PACKED) {
			if (Z_TYPE(ht->arData[ptr->h].val) != IS_UNDEF) {
				ht->nInternalPointer = ptr->h;
				return 1;
			}
		} else {
			idx = ht->arHash[ptr->h & ht->nTableMask];
			while (idx != INVALID_IDX) {
				if (idx == ptr->pos) {
					ht->nInternalPointer = idx;
					return 1;
				}
				idx = Z_NEXT(ht->arData[idx].val);
			}
		}
		return 0;
	}
	return 1;
}

ZEND_API void zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos)
{
    uint idx;
	
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
	uint idx;
	
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
	uint idx = *pos;

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
	uint idx = *pos;

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
ZEND_API int zend_hash_get_current_key_ex(const HashTable *ht, zend_string **str_index, ulong *num_index, zend_bool duplicate, HashPosition *pos)
{
	uint idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx != INVALID_IDX) {
		p = ht->arData + idx;
		if (p->key) {
			if (duplicate) {
				*str_index = STR_COPY(p->key);
			} else {
				*str_index = p->key;
			}
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
	uint idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx == INVALID_IDX) {
		ZVAL_NULL(key);
	} else {
		p = ht->arData + idx;
		if (p->key) {
			ZVAL_STR(key, p->key);
			STR_ADDREF(p->key);
		} else {
			ZVAL_LONG(key, p->h);
		}
	}
}

ZEND_API int zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos)
{
    uint idx = *pos;
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
	uint idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx != INVALID_IDX) {
		p = ht->arData + idx;
		return &p->val;
	} else {
		return NULL;
	}
}

/* This function changes key of current element without changing elements'
 * order. If element with target key already exists, it will be deleted first.
 */
ZEND_API int zend_hash_update_current_key_ex(HashTable *ht, int key_type, zend_string *str_index, ulong num_index, int mode)
{
	uint idx1 = ht->nInternalPointer;
	uint idx2;
	Bucket *p, *q;
	ulong h;
#ifdef ZEND_SIGNALS
	TSRMLS_FETCH();
#endif

	IS_CONSISTENT(ht);
	if (idx1 != INVALID_IDX) {
		p = ht->arData + idx1;
		if (key_type == HASH_KEY_IS_LONG) {
			if (p->h == num_index && p->key == NULL) {
				return SUCCESS;
			}

			idx2 = ht->arHash[num_index & ht->nTableMask];
			while (idx2 != INVALID_IDX) {
				q = ht->arData + idx2;
				if (q->h == num_index && q->key == NULL) {
					break;
				}
				idx2 = Z_NEXT(q->val);
			}
		} else if (key_type == HASH_KEY_IS_STRING) {
			h = STR_HASH_VAL(str_index);
			if (p->key == str_index ||
			    (p->h == h &&
			     p->key &&
			     p->key->len == str_index->len &&
			     memcmp(p->key->val, str_index->val, str_index->len) == 0)) {
				return SUCCESS;
			}

			idx2 = ht->arHash[h & ht->nTableMask];
			while (idx2 != INVALID_IDX) {
				q = ht->arData + idx2;
				if (q->key == str_index ||
				    (q->h == h && q->key && q->key->len == str_index->len &&
				     memcmp(q->key->val, str_index->val, str_index->len) == 0)) {
					break;
				}
				idx2 = Z_NEXT(q->val);
			}
		} else {
			return FAILURE;
		}

		HANDLE_BLOCK_INTERRUPTIONS();

		if (idx2 != INVALID_IDX) {
			/* we have another bucket with the key equal to new one */
			if (mode != HASH_UPDATE_KEY_ANYWAY) {
				int found = (idx1 < idx2) ? HASH_UPDATE_KEY_IF_BEFORE : HASH_UPDATE_KEY_IF_AFTER;

				if (mode & found) {
					/* delete current bucket */
					_zend_hash_del_el(ht, idx1, p);
					HANDLE_UNBLOCK_INTERRUPTIONS();
					return FAILURE;
				}
			}
			/* delete another bucket with the same key */
			_zend_hash_del_el(ht, idx2, q);
		}

		/* remove old key from hash */
		if (ht->arHash[p->h & ht->nTableMask] == idx1) {
			ht->arHash[p->h & ht->nTableMask] = Z_NEXT(p->val);
 		} else {
			uint idx3 = ht->arHash[p->h & ht->nTableMask];
			while (Z_NEXT(ht->arData[idx3].val) != idx1) {
				idx3 = Z_NEXT(ht->arData[idx3].val);
			} 
			Z_NEXT(ht->arData[idx3].val) = Z_NEXT(p->val);
 		}

		/* update key */
 		if (p->key) {
 			STR_RELEASE(p->key);
 		}
		if (key_type == HASH_KEY_IS_LONG) {
			p->h = num_index;
			p->key = NULL;
		} else {
			p->h = h;
			p->key = str_index;
			STR_ADDREF(str_index);
		}

		/* insert new key into hash */
		Z_NEXT(p->val) = ht->arHash[p->h & ht->nTableMask];
		ht->arHash[p->h & ht->nTableMask] = idx1;
		HANDLE_UNBLOCK_INTERRUPTIONS();

		return SUCCESS;
	} else {
		return FAILURE;
	}
}

ZEND_API int zend_hash_sort(HashTable *ht, sort_func_t sort_func,
							compare_func_t compar, int renumber TSRMLS_DC)
{
	Bucket *p;
	int i, j;

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

	(*sort_func)((void *) ht->arData, i, sizeof(Bucket), compar TSRMLS_CC);

	HANDLE_BLOCK_INTERRUPTIONS();
	ht->nNumUsed = i;
	ht->nInternalPointer = 0;

	if (renumber) {
		for (j = 0; j < i; j++) {
			p = ht->arData + j;
			p->h = j;
			if (p->key) {
				STR_RELEASE(p->key);
				p->key = NULL;
			}
		}
	}
	if (renumber) {
		ht->nNextFreeElement = i;
	}
	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (!renumber) {
			zend_hash_packed_to_hash(ht);
		}
	} else {
		if (renumber) {
			ht->u.flags |= HASH_FLAG_PACKED;
			ht->arData = erealloc(ht->arData, ht->nTableSize * sizeof(Bucket));
			ht->arHash = (zend_uint*)&uninitialized_bucket;
		} else {
			zend_hash_rehash(ht);
		}
	}

	HANDLE_UNBLOCK_INTERRUPTIONS();

	return SUCCESS;
}


ZEND_API int zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_func_t compar, zend_bool ordered TSRMLS_DC)
{
	uint idx1, idx2;
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
				result = (p1->key ? p1->key->len : 0) - (p2->key ? p2->key->len : 0);
				if (result != 0) {
					HASH_UNPROTECT_RECURSION(ht1); 
					HASH_UNPROTECT_RECURSION(ht2); 
					return result;
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
			result = compar(pData1, pData2 TSRMLS_CC);
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


ZEND_API zval *zend_hash_minmax(const HashTable *ht, compare_func_t compar, int flag TSRMLS_DC)
{
	uint idx;
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
			if (compar(res, p TSRMLS_CC) < 0) { /* max */
				res = p;
			}
		} else {
			if (compar(res, p TSRMLS_CC) > 0) { /* min */
				res = p;
			}
		}
	}
	return &res->val;
}

ZEND_API ulong zend_hash_next_free_element(const HashTable *ht)
{
	IS_CONSISTENT(ht);

	return ht->nNextFreeElement;

}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
