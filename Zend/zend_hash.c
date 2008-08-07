/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2008 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_operators.h"
#include "zend_globals.h"

#include <unicode/utypes.h>
#include <unicode/uchar.h>

#define CONNECT_TO_BUCKET_DLLIST(element, list_head)		\
	(element)->pNext = (list_head);							\
	(element)->pLast = NULL;								\
	if ((element)->pNext) {									\
		(element)->pNext->pLast = (element);				\
	}

#define CONNECT_TO_GLOBAL_DLLIST(element, ht)				\
	(element)->pListLast = (ht)->pListTail;					\
	(ht)->pListTail = (element);							\
	(element)->pListNext = NULL;							\
	if ((element)->pListLast != NULL) {						\
		(element)->pListLast->pListNext = (element);		\
	}														\
	if (!(ht)->pListHead) {									\
		(ht)->pListHead = (element);						\
	}														\
	if ((ht)->pInternalPointer == NULL) {					\
		(ht)->pInternalPointer = (element);					\
	}

#define ZEND_HASH_CVT_ERROR() zend_error(E_WARNING, "Could not convert String to Unicode")

#define UNICODE_KEY(ht, type, arKey, nKeyLength, tmp) \
	if (ht->unicode && type == IS_STRING) { \
		UErrorCode status = U_ZERO_ERROR; \
		UChar *u = NULL; \
		int u_len; \
		TSRMLS_FETCH(); \
		zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &u, &u_len, arKey.s, nKeyLength-1, &status); \
		if (U_FAILURE(status)) { \
			ZEND_HASH_CVT_ERROR(); \
		} else { \
			type = IS_UNICODE; \
			tmp = arKey.u = u; \
			nKeyLength = u_len + 1; \
		} \
	}


#if ZEND_DEBUG
#define HT_OK				0
#define HT_IS_DESTROYING	1
#define HT_DESTROYED		2
#define HT_CLEANING			3

static void _zend_is_inconsistent(HashTable *ht, char *file, int line) /* {{{ */
{
	if (ht->inconsistent==HT_OK) {
		return;
	}
	switch (ht->inconsistent) {
		case HT_IS_DESTROYING:
			zend_output_debug_string(1, "%s(%d) : ht=%p is being destroyed", file, line, ht);
			break;
		case HT_DESTROYED:
			zend_output_debug_string(1, "%s(%d) : ht=%p is already destroyed", file, line, ht);
			break;
		case HT_CLEANING:
			zend_output_debug_string(1, "%s(%d) : ht=%p is being cleaned", file, line, ht);
			break;
	}
	zend_bailout();
}
/* }}} */

#define IS_CONSISTENT(a) _zend_is_inconsistent(a, __FILE__, __LINE__);
#define SET_INCONSISTENT(n) ht->inconsistent = n;
#else
#define IS_CONSISTENT(a)
#define SET_INCONSISTENT(n)
#endif

#define HASH_PROTECT_RECURSION(ht)														\
	if ((ht)->bApplyProtection) {														\
		if ((ht)->nApplyCount++ >= 3) {													\
			zend_error(E_ERROR, "Nesting level too deep - recursive dependency?");		\
		}																				\
	}


#define HASH_UNPROTECT_RECURSION(ht)													\
	if ((ht)->bApplyProtection) {														\
		(ht)->nApplyCount--;															\
	}


#define ZEND_HASH_IF_FULL_DO_RESIZE(ht)				\
	if ((ht)->nNumOfElements > (ht)->nTableSize) {	\
		zend_hash_do_resize(ht);					\
	}

static int zend_hash_do_resize(HashTable *ht);

ZEND_API ulong zend_u_hash_func(zend_uchar type, zstr arKey, uint nKeyLength) /* {{{ */
{
	return zend_u_inline_hash_func(type, arKey, nKeyLength);
}
/* }}} */

ZEND_API ulong zend_hash_func(const char *arKey, uint nKeyLength) /* {{{ */
{
	return zend_u_hash_func(IS_STRING, ZSTR(arKey), nKeyLength);
}
/* }}} */

#define UPDATE_DATA(ht, p, pData, nDataSize)											\
	if (nDataSize == sizeof(void*)) {													\
		if ((p)->pData != &(p)->pDataPtr) {												\
			pefree_rel(p->pData, ht->persistent);										\
		}																				\
		memcpy(&(p)->pDataPtr, pData, sizeof(void *));									\
		(p)->pData = &(p)->pDataPtr;													\
	} else {																			\
		if ((p)->pData == &(p)->pDataPtr) {												\
			(p)->pData = (void *) pemalloc_rel(nDataSize, (ht)->persistent);			\
		} else {																		\
			(p)->pData = (void *) perealloc_rel((p)->pData, nDataSize, (ht)->persistent);	\
		}																				\
		memcpy((p)->pData, pData, nDataSize);											\
	}

#define INIT_DATA(ht, p, pData, nDataSize);								\
	if (nDataSize == sizeof(void*)) {									\
		memcpy(&(p)->pDataPtr, pData, sizeof(void *));					\
		(p)->pData = &(p)->pDataPtr;									\
	} else {															\
		(p)->pData = (void *) pemalloc_rel(nDataSize, (ht)->persistent);	\
		if (!(p)->pData) {												\
			pefree_rel(p, (ht)->persistent);								\
			return FAILURE;												\
		}																\
		memcpy((p)->pData, pData, nDataSize);							\
	}
/* }}} */

ZEND_API int _zend_u_hash_init(HashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent, zend_bool unicode ZEND_FILE_LINE_DC) /* {{{ */
{
	uint i = 3;
	Bucket **tmp;

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

	ht->nTableMask = ht->nTableSize - 1;
	ht->pDestructor = pDestructor;
	ht->arBuckets = NULL;
	ht->pListHead = NULL;
	ht->pListTail = NULL;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->pInternalPointer = NULL;
	ht->persistent = persistent;
	ht->unicode = unicode;
	ht->nApplyCount = 0;
	ht->bApplyProtection = 1;

	/* Uses ecalloc() so that Bucket* == NULL */
	if (persistent) {
		tmp = (Bucket **) calloc(ht->nTableSize, sizeof(Bucket *));
		if (!tmp) {
			return FAILURE;
		}
		ht->arBuckets = tmp;
	} else {
		tmp = (Bucket **) ecalloc_rel(ht->nTableSize, sizeof(Bucket *));
		if (tmp) {
			ht->arBuckets = tmp;
		}
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int _zend_hash_init(HashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent ZEND_FILE_LINE_DC) /* {{{ */
{
	return _zend_u_hash_init(ht, nSize, pHashFunction, pDestructor, persistent, 0 ZEND_FILE_LINE_CC);
}
/* }}} */

ZEND_API int _zend_hash_init_ex(HashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC) /* {{{ */
{
	int retval = _zend_hash_init(ht, nSize, pHashFunction, pDestructor, persistent ZEND_FILE_LINE_CC);

	ht->bApplyProtection = bApplyProtection;
	return retval;
}
/* }}} */

ZEND_API int _zend_u_hash_init_ex(HashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent, zend_bool unicode, zend_bool bApplyProtection ZEND_FILE_LINE_DC) /* {{{ */
{
	int retval = _zend_u_hash_init(ht, nSize, pHashFunction, pDestructor, persistent, unicode ZEND_FILE_LINE_CC);

	ht->bApplyProtection = bApplyProtection;
	return retval;
}
/* }}} */

ZEND_API void zend_hash_set_apply_protection(HashTable *ht, zend_bool bApplyProtection) /* {{{ */
{
	ht->bApplyProtection = bApplyProtection;
}
/* }}} */

ZEND_API int _zend_u_hash_add_or_update(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC) /* {{{ */
{
	ulong h;
	uint nIndex;
	Bucket *p;
	void *tmp = NULL;
	uint realKeyLength;

	IS_CONSISTENT(ht);

	if (nKeyLength <= 0) {
#if ZEND_DEBUG
		ZEND_PUTS("zend_hash_update: Can't put in empty key\n");
#endif
		return FAILURE;
	}

	UNICODE_KEY(ht, type, arKey, nKeyLength, tmp);
	realKeyLength = USTR_BYTES(type, nKeyLength);

	h = zend_u_inline_hash_func(type, arKey, nKeyLength);
	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) &&
		    (p->key.type == type) &&
		    (p->nKeyLength == nKeyLength) &&
		    !memcmp(p->key.arKey.s, arKey.s, realKeyLength)) {
			if (flag & HASH_ADD) {
				if (tmp) efree(tmp);
				return FAILURE;
			}
			HANDLE_BLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
			if (p->pData == pData) {
				ZEND_PUTS("Fatal error in zend_hash_update: p->pData == pData\n");
				HANDLE_UNBLOCK_INTERRUPTIONS();
				if (tmp) efree(tmp);
				return FAILURE;
			}
#endif
			if (ht->pDestructor) {
				ht->pDestructor(p->pData);
			}
			UPDATE_DATA(ht, p, pData, nDataSize);
			if (pDest) {
				*pDest = p->pData;
			}
			HANDLE_UNBLOCK_INTERRUPTIONS();
			if (tmp) efree(tmp);
			return SUCCESS;
		}
		p = p->pNext;
	}

	p = (Bucket *) pemalloc(sizeof(Bucket)-sizeof(p->key.arKey)+realKeyLength, ht->persistent);
	if (!p) {
		if (tmp) efree(tmp);
		return FAILURE;
	}
	p->key.type = type;
	memcpy(p->key.arKey.s, arKey.s, realKeyLength);
	p->nKeyLength = nKeyLength;
	INIT_DATA(ht, p, pData, nDataSize);
	p->h = h;
	CONNECT_TO_BUCKET_DLLIST(p, ht->arBuckets[nIndex]);
	if (pDest) {
		*pDest = p->pData;
	}

	HANDLE_BLOCK_INTERRUPTIONS();
	CONNECT_TO_GLOBAL_DLLIST(p, ht);
	ht->arBuckets[nIndex] = p;
	HANDLE_UNBLOCK_INTERRUPTIONS();

	ht->nNumOfElements++;
	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */
	if (tmp) efree(tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int _zend_hash_add_or_update(HashTable *ht, const char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC) /* {{{ */
{
	return _zend_u_hash_add_or_update(ht, IS_STRING, ZSTR(arKey), nKeyLength, pData, nDataSize, pDest, flag ZEND_FILE_LINE_CC);
}
/* }}} */

ZEND_API int _zend_ascii_hash_add_or_update(HashTable *ht, const char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;

		key.u = zend_ascii_to_unicode(arKey, nKeyLength ZEND_FILE_LINE_CC);
		ret = _zend_u_hash_add_or_update(ht, IS_UNICODE, key, nKeyLength, pData, nDataSize, pDest, flag ZEND_FILE_LINE_CC);
		efree(key.u);
		return ret;
	} else {
		return _zend_u_hash_add_or_update(ht, IS_STRING, ZSTR(arKey), nKeyLength, pData, nDataSize, pDest, flag ZEND_FILE_LINE_CC);
	}
}
/* }}} */

ZEND_API int _zend_rt_hash_add_or_update(HashTable *ht, const char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;
		UErrorCode status = U_ZERO_ERROR;
		int u_len;

		zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &key.u, &u_len, arKey, nKeyLength-1, &status);
		if (U_FAILURE(status)) {
			ZEND_HASH_CVT_ERROR();
			goto string_key;
		}

		ret = _zend_u_hash_add_or_update(ht, IS_UNICODE, key, u_len+1, pData, nDataSize, pDest, flag ZEND_FILE_LINE_CC);
		efree(key.u);
		return ret;
	} else {
string_key:
		return _zend_u_hash_add_or_update(ht, IS_STRING, ZSTR(arKey), nKeyLength, pData, nDataSize, pDest, flag ZEND_FILE_LINE_CC);
	}
}
/* }}} */

ZEND_API int _zend_utf8_hash_add_or_update(HashTable *ht, const char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;
		UErrorCode status = U_ZERO_ERROR;
		int u_len;

		zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(utf8_conv)), &key.u, &u_len, arKey, nKeyLength-1, &status);
		if (U_FAILURE(status)) {
			ZEND_HASH_CVT_ERROR();
			goto string_key;
		}

		ret = _zend_u_hash_add_or_update(ht, IS_UNICODE, key, u_len+1, pData, nDataSize, pDest, flag ZEND_FILE_LINE_CC);
		efree(key.u);
		return ret;
	} else {
string_key:
		return _zend_u_hash_add_or_update(ht, IS_STRING, ZSTR(arKey), nKeyLength, pData, nDataSize, pDest, flag ZEND_FILE_LINE_CC);
	}
}
/* }}} */

ZEND_API int _zend_u_hash_quick_add_or_update(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength, ulong h, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC) /* {{{ */
{
	uint nIndex;
	Bucket *p;
	void *tmp = NULL;
	uint realKeyLength;

	IS_CONSISTENT(ht);

	if (nKeyLength == 0) {
		return zend_hash_index_update(ht, h, pData, nDataSize, pDest);
	}

	if (ht->unicode && type == IS_STRING) {
		UNICODE_KEY(ht, type, arKey, nKeyLength, tmp);
		h = zend_u_inline_hash_func(IS_UNICODE, arKey, nKeyLength);
	}
	realKeyLength = USTR_BYTES(type, nKeyLength);

	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) &&
		    (p->key.type == type) &&
		    (p->nKeyLength == nKeyLength) &&
		    !memcmp(p->key.arKey.s, arKey.s, realKeyLength)) {
			if (flag & HASH_ADD) {
				if (tmp) efree(tmp);
				return FAILURE;
			}
			HANDLE_BLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
			if (p->pData == pData) {
				ZEND_PUTS("Fatal error in zend_hash_update: p->pData == pData\n");
				HANDLE_UNBLOCK_INTERRUPTIONS();
				if (tmp) efree(tmp);
				return FAILURE;
			}
#endif
			if (ht->pDestructor) {
				ht->pDestructor(p->pData);
			}
			UPDATE_DATA(ht, p, pData, nDataSize);
			if (pDest) {
				*pDest = p->pData;
			}
			HANDLE_UNBLOCK_INTERRUPTIONS();
			if (tmp) efree(tmp);
			return SUCCESS;
		}
		p = p->pNext;
	}

	p = (Bucket *) pemalloc(sizeof(Bucket)-sizeof(p->key.arKey)+realKeyLength, ht->persistent);
	if (!p) {
		if (tmp) efree(tmp);
		return FAILURE;
	}

	p->key.type = type;
	memcpy(p->key.arKey.s, arKey.s, realKeyLength);
	p->nKeyLength = nKeyLength;
	INIT_DATA(ht, p, pData, nDataSize);
	p->h = h;

	CONNECT_TO_BUCKET_DLLIST(p, ht->arBuckets[nIndex]);

	if (pDest) {
		*pDest = p->pData;
	}

	HANDLE_BLOCK_INTERRUPTIONS();
	ht->arBuckets[nIndex] = p;
	CONNECT_TO_GLOBAL_DLLIST(p, ht);
	HANDLE_UNBLOCK_INTERRUPTIONS();

	ht->nNumOfElements++;
	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */
	if (tmp) efree(tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int _zend_hash_quick_add_or_update(HashTable *ht, const char *arKey, uint nKeyLength, ulong h, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC) /* {{{ */
{
	return _zend_u_hash_quick_add_or_update(ht, IS_STRING, ZSTR(arKey), nKeyLength, h, pData, nDataSize, pDest, flag ZEND_FILE_LINE_CC);
}
/* }}} */

ZEND_API int zend_u_hash_add_empty_element(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength) /* {{{ */
{
	void *dummy = (void *) 1;

	return zend_u_hash_add(ht, type, arKey, nKeyLength, &dummy, sizeof(void *), NULL);
}
/* }}} */

ZEND_API int zend_hash_add_empty_element(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	void *dummy = (void *) 1;

	return zend_u_hash_add(ht, IS_STRING, ZSTR(arKey), nKeyLength, &dummy, sizeof(void *), NULL);
}
/* }}} */

ZEND_API int _zend_hash_index_update_or_next_insert(HashTable *ht, ulong h, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC) /* {{{ */
{
	uint nIndex;
	Bucket *p;

	IS_CONSISTENT(ht);

	if (flag & HASH_NEXT_INSERT) {
		h = ht->nNextFreeElement;
	}
	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->nKeyLength == 0) && (p->h == h)) {
			if (flag & HASH_NEXT_INSERT || flag & HASH_ADD) {
				return FAILURE;
			}
			HANDLE_BLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
			if (p->pData == pData) {
				ZEND_PUTS("Fatal error in zend_hash_index_update: p->pData == pData\n");
				HANDLE_UNBLOCK_INTERRUPTIONS();
				return FAILURE;
			}
#endif
			if (ht->pDestructor) {
				ht->pDestructor(p->pData);
			}
			UPDATE_DATA(ht, p, pData, nDataSize);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			if ((long)h >= (long)ht->nNextFreeElement) {
				ht->nNextFreeElement = h + 1;
			}
			if (pDest) {
				*pDest = p->pData;
			}
			return SUCCESS;
		}
		p = p->pNext;
	}
	p = (Bucket *) pemalloc_rel(sizeof(Bucket) - sizeof(p->key.arKey), ht->persistent);
	if (!p) {
		return FAILURE;
	}
	p->nKeyLength = 0; /* Numeric indices are marked by making the nKeyLength == 0 */
	p->h = h;
	INIT_DATA(ht, p, pData, nDataSize);
	if (pDest) {
		*pDest = p->pData;
	}

	p->key.type = IS_LONG;

	CONNECT_TO_BUCKET_DLLIST(p, ht->arBuckets[nIndex]);

	HANDLE_BLOCK_INTERRUPTIONS();
	ht->arBuckets[nIndex] = p;
	CONNECT_TO_GLOBAL_DLLIST(p, ht);
	HANDLE_UNBLOCK_INTERRUPTIONS();

	if ((long)h >= (long)ht->nNextFreeElement) {
		ht->nNextFreeElement = h + 1;
	}
	ht->nNumOfElements++;
	ZEND_HASH_IF_FULL_DO_RESIZE(ht);
	return SUCCESS;
}
/* }}} */

static int zend_hash_do_resize(HashTable *ht) /* {{{ */
{
	Bucket **t;

	IS_CONSISTENT(ht);

	if ((ht->nTableSize << 1) > 0) {	/* Let's double the table size */
		t = (Bucket **) perealloc_recoverable(ht->arBuckets, (ht->nTableSize << 1) * sizeof(Bucket *), ht->persistent);
		if (t) {
			HANDLE_BLOCK_INTERRUPTIONS();
			ht->arBuckets = t;
			ht->nTableSize = (ht->nTableSize << 1);
			ht->nTableMask = ht->nTableSize - 1;
			zend_hash_rehash(ht);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			return SUCCESS;
		}
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_hash_rehash(HashTable *ht) /* {{{ */
{
	Bucket *p;
	uint nIndex;

	IS_CONSISTENT(ht);

	memset(ht->arBuckets, 0, ht->nTableSize * sizeof(Bucket *));
	p = ht->pListHead;
	while (p != NULL) {
		nIndex = p->h & ht->nTableMask;
		CONNECT_TO_BUCKET_DLLIST(p, ht->arBuckets[nIndex]);
		ht->arBuckets[nIndex] = p;
		p = p->pListNext;
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_u_hash_del_key_or_index(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength, ulong h, int flag) /* {{{ */
{
	uint nIndex;
	Bucket *p;
	void *tmp = NULL;

	IS_CONSISTENT(ht);

	if (flag == HASH_DEL_KEY) {
		UNICODE_KEY(ht, type, arKey, nKeyLength, tmp);
		h = zend_u_inline_hash_func(type, arKey, nKeyLength);
	}
	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h)
			&& (p->nKeyLength == nKeyLength)
		    && ((p->nKeyLength == 0) /* Numeric index (short circuits the memcmp()) */
				|| ((p->key.type == type)
		      		&& !memcmp(p->key.arKey.s, arKey.s, USTR_BYTES(type, nKeyLength))))) {
			HANDLE_BLOCK_INTERRUPTIONS();
			if (p == ht->arBuckets[nIndex]) {
				ht->arBuckets[nIndex] = p->pNext;
			} else {
				p->pLast->pNext = p->pNext;
			}
			if (p->pNext) {
				p->pNext->pLast = p->pLast;
			}
			if (p->pListLast != NULL) {
				p->pListLast->pListNext = p->pListNext;
			} else {
				/* Deleting the head of the list */
				ht->pListHead = p->pListNext;
			}
			if (p->pListNext != NULL) {
				p->pListNext->pListLast = p->pListLast;
			} else {
				ht->pListTail = p->pListLast;
			}
			if (ht->pInternalPointer == p) {
				ht->pInternalPointer = p->pListNext;
			}
			if (ht->pDestructor) {
				ht->pDestructor(p->pData);
			}
			if (p->pData != &p->pDataPtr) {
				pefree(p->pData, ht->persistent);
			}
			pefree(p, ht->persistent);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			ht->nNumOfElements--;
			if (tmp) efree(tmp);
			return SUCCESS;
		}
		p = p->pNext;
	}
	if (tmp) efree(tmp);
	return FAILURE;
}
/* }}} */

ZEND_API int zend_hash_del_key_or_index(HashTable *ht, const char *arKey, uint nKeyLength, ulong h, int flag) /* {{{ */
{
	return zend_u_hash_del_key_or_index(ht, IS_STRING, ZSTR(arKey), nKeyLength, h, flag);
}
/* }}} */

ZEND_API int zend_ascii_hash_del(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;

		key.u = zend_ascii_to_unicode(arKey, nKeyLength ZEND_FILE_LINE_CC);
		ret = zend_u_hash_del_key_or_index(ht, IS_UNICODE, key, nKeyLength, 0, HASH_DEL_KEY);
		efree(key.u);
		return ret;
	} else {
		return zend_u_hash_del_key_or_index(ht, IS_STRING, ZSTR(arKey), nKeyLength, 0, HASH_DEL_KEY);
	}
}
/* }}} */

ZEND_API int zend_rt_hash_del(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;
		UErrorCode status = U_ZERO_ERROR;
		int u_len;

		zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &key.u, &u_len, arKey, nKeyLength-1, &status);
		if (U_FAILURE(status)) {
			ZEND_HASH_CVT_ERROR();
			goto string_key;
		}
		ret = zend_u_hash_del_key_or_index(ht, IS_UNICODE, key, u_len+1, 0, HASH_DEL_KEY);
		efree(key.u);
		return ret;
	} else {
string_key:
		return zend_u_hash_del_key_or_index(ht, IS_STRING, ZSTR(arKey), nKeyLength, 0, HASH_DEL_KEY);
	}
}
/* }}} */

ZEND_API int zend_utf8_hash_del(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;
		UErrorCode status = U_ZERO_ERROR;
		int u_len;

		zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(utf8_conv)), &key.u, &u_len, arKey, nKeyLength-1, &status);
		if (U_FAILURE(status)) {
			ZEND_HASH_CVT_ERROR();
			goto string_key;
		}
		ret = zend_u_hash_del_key_or_index(ht, IS_UNICODE, key, u_len+1, 0, HASH_DEL_KEY);
		efree(key.u);
		return ret;
	} else {
string_key:
		return zend_u_hash_del_key_or_index(ht, IS_STRING, ZSTR(arKey), nKeyLength, 0, HASH_DEL_KEY);
	}
}
/* }}} */

ZEND_API void zend_hash_destroy(HashTable *ht) /* {{{ */
{
	Bucket *p, *q;

	IS_CONSISTENT(ht);

	SET_INCONSISTENT(HT_IS_DESTROYING);

	p = ht->pListHead;
	while (p != NULL) {
		q = p;
		p = p->pListNext;
		if (ht->pDestructor) {
			ht->pDestructor(q->pData);
		}
		if (q->pData != &q->pDataPtr) {
			pefree(q->pData, ht->persistent);
		}
		pefree(q, ht->persistent);
	}
	pefree(ht->arBuckets, ht->persistent);

	SET_INCONSISTENT(HT_DESTROYED);
}
/* }}} */

ZEND_API void zend_hash_clean(HashTable *ht) /* {{{ */
{
	Bucket *p, *q;

	IS_CONSISTENT(ht);

	SET_INCONSISTENT(HT_CLEANING);

	p = ht->pListHead;
	while (p != NULL) {
		q = p;
		p = p->pListNext;
		if (ht->pDestructor) {
			ht->pDestructor(q->pData);
		}
		if (q->pData != &q->pDataPtr) {
			pefree(q->pData, ht->persistent);
		}
		pefree(q, ht->persistent);
	}
	memset(ht->arBuckets, 0, ht->nTableSize*sizeof(Bucket *));
	ht->pListHead = NULL;
	ht->pListTail = NULL;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->pInternalPointer = NULL;

	SET_INCONSISTENT(HT_OK);
}
/* }}} */

/* This function is used by the various apply() functions.
 * It deletes the passed bucket, and returns the address of the
 * next bucket.  The hash *may* be altered during that time, the
 * returned value will still be valid.
 */
static Bucket *zend_hash_apply_deleter(HashTable *ht, Bucket *p) /* {{{ */
{
	Bucket *retval;

	HANDLE_BLOCK_INTERRUPTIONS();
	if (p->pLast) {
		p->pLast->pNext = p->pNext;
	} else {
		uint nIndex;

		nIndex = p->h & ht->nTableMask;
		ht->arBuckets[nIndex] = p->pNext;
	}
	if (p->pNext) {
		p->pNext->pLast = p->pLast;
	} else {
		/* Nothing to do as this list doesn't have a tail */
	}

	if (p->pListLast != NULL) {
		p->pListLast->pListNext = p->pListNext;
	} else {
		/* Deleting the head of the list */
		ht->pListHead = p->pListNext;
	}
	if (p->pListNext != NULL) {
		p->pListNext->pListLast = p->pListLast;
	} else {
		ht->pListTail = p->pListLast;
	}
	if (ht->pInternalPointer == p) {
		ht->pInternalPointer = p->pListNext;
	}
	ht->nNumOfElements--;
	HANDLE_UNBLOCK_INTERRUPTIONS();

	if (ht->pDestructor) {
		ht->pDestructor(p->pData);
	}
	if (p->pData != &p->pDataPtr) {
		pefree(p->pData, ht->persistent);
	}
	retval = p->pListNext;
	pefree(p, ht->persistent);

	return retval;
}
/* }}} */

ZEND_API void zend_hash_graceful_destroy(HashTable *ht) /* {{{ */
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = ht->pListHead;
	while (p != NULL) {
		p = zend_hash_apply_deleter(ht, p);
	}
	pefree(ht->arBuckets, ht->persistent);

	SET_INCONSISTENT(HT_DESTROYED);
}
/* }}} */

ZEND_API void zend_hash_graceful_reverse_destroy(HashTable *ht) /* {{{ */
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = ht->pListTail;
	while (p != NULL) {
		zend_hash_apply_deleter(ht, p);
		p = ht->pListTail;
	}

	pefree(ht->arBuckets, ht->persistent);

	SET_INCONSISTENT(HT_DESTROYED);
}
/* }}} */

/* This is used to recurse elements and selectively delete certain entries 
 * from a hashtable. apply_func() receives the data and decides if the entry 
 * should be deleted or recursion should be stopped. The following three 
 * return codes are possible:
 * ZEND_HASH_APPLY_KEEP   - continue
 * ZEND_HASH_APPLY_STOP   - stop iteration
 * ZEND_HASH_APPLY_REMOVE - delete the element, combineable with the former
 */

ZEND_API void zend_hash_apply(HashTable *ht, apply_func_t apply_func TSRMLS_DC) /* {{{ */
{
	Bucket *p;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	p = ht->pListHead;
	while (p != NULL) {
		int result = apply_func(p->pData TSRMLS_CC);
		
		if (result & ZEND_HASH_APPLY_REMOVE) {
			p = zend_hash_apply_deleter(ht, p);
		} else {
			p = p->pListNext;
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}
/* }}} */

ZEND_API void zend_hash_apply_with_argument(HashTable *ht, apply_func_arg_t apply_func, void *argument TSRMLS_DC) /* {{{ */
{
	Bucket *p;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	p = ht->pListHead;
	while (p != NULL) {
		int result = apply_func(p->pData, argument TSRMLS_CC);
		
		if (result & ZEND_HASH_APPLY_REMOVE) {
			p = zend_hash_apply_deleter(ht, p);
		} else {
			p = p->pListNext;
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}
/* }}} */

ZEND_API void zend_hash_apply_with_arguments(HashTable *ht TSRMLS_DC, apply_func_args_t apply_func, int num_args, ...) /* {{{ */
{
	Bucket *p;
	va_list args;
	zend_hash_key hash_key;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);

	p = ht->pListHead;
	while (p != NULL) {
		int result;
		va_start(args, num_args);
		hash_key.nKeyLength = p->nKeyLength;
		hash_key.h = p->h;
		hash_key.type = p->key.type;
		hash_key.arKey.s = p->key.arKey.s;
		result = apply_func(p->pData TSRMLS_CC, num_args, args, &hash_key);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			p = zend_hash_apply_deleter(ht, p);
		} else {
			p = p->pListNext;
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
		va_end(args);
	}

	HASH_UNPROTECT_RECURSION(ht);
}
/* }}} */

ZEND_API void zend_hash_reverse_apply(HashTable *ht, apply_func_t apply_func TSRMLS_DC) /* {{{ */
{
	Bucket *p, *q;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	p = ht->pListTail;
	while (p != NULL) {
		int result = apply_func(p->pData TSRMLS_CC);

		q = p;
		p = p->pListLast;
		if (result & ZEND_HASH_APPLY_REMOVE) {
			if (q->nKeyLength==0) {
				zend_hash_index_del(ht, q->h);
			} else {
				zend_u_hash_del(ht, q->key.type, ZSTR(q->key.arKey.s), q->nKeyLength);
			}
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}
/* }}} */

ZEND_API void zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size) /* {{{ */
{
	Bucket *p;
	void *new_entry;
	zend_bool setTargetPointer;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

	setTargetPointer = !target->pInternalPointer;
	p = source->pListHead;
	while (p) {
		if (setTargetPointer && source->pInternalPointer == p) {
			target->pInternalPointer = NULL;
		}
		if (p->nKeyLength == 0) {
 			zend_hash_index_update(target, p->h, p->pData, size, &new_entry);
		} else {
			zend_u_hash_quick_update(target, p->key.type, ZSTR(p->key.arKey.s), p->nKeyLength, p->h, p->pData, size, &new_entry);
		}
		if (pCopyConstructor) {
			pCopyConstructor(new_entry);
		}
		p = p->pListNext;
	}
	if (!target->pInternalPointer) {
		target->pInternalPointer = target->pListHead;
	}
}
/* }}} */

ZEND_API void _zend_hash_merge(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size, int overwrite ZEND_FILE_LINE_DC) /* {{{ */
{
	Bucket *p;
	void *t;
	int mode = (overwrite?HASH_UPDATE:HASH_ADD);

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

	p = source->pListHead;
	while (p) {
		if (p->nKeyLength==0) {
			if ((mode==HASH_UPDATE || !zend_hash_index_exists(target, p->h)) && zend_hash_index_update(target, p->h, p->pData, size, &t)==SUCCESS && pCopyConstructor) {
				pCopyConstructor(t);
			}
		} else {
			if (_zend_u_hash_quick_add_or_update(target, p->key.type, ZSTR(p->key.arKey.s), p->nKeyLength, p->h, p->pData, size, &t, mode ZEND_FILE_LINE_RELAY_CC)==SUCCESS && pCopyConstructor) {
				pCopyConstructor(t);
			}
		}
		p = p->pListNext;
	}
	target->pInternalPointer = target->pListHead;
}
/* }}} */

static zend_bool zend_hash_replace_checker_wrapper(HashTable *target, void *source_data, Bucket *p, void *pParam, merge_checker_func_t merge_checker_func) /* {{{ */
{
	zend_hash_key hash_key;

	hash_key.nKeyLength = p->nKeyLength;
	hash_key.h = p->h;
	hash_key.type = p->key.type;
	hash_key.arKey.s = p->key.arKey.s;
	return merge_checker_func(target, source_data, &hash_key, pParam);
}
/* }}} */

ZEND_API void zend_hash_merge_ex(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, uint size, merge_checker_func_t pMergeSource, void *pParam) /* {{{ */
{
	Bucket *p;
	void *t;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

	p = source->pListHead;
	while (p) {
		if (zend_hash_replace_checker_wrapper(target, p->pData, p, pParam, pMergeSource)) {
			if (zend_u_hash_quick_update(target, p->key.type, ZSTR(p->key.arKey.s), p->nKeyLength, p->h, p->pData, size, &t)==SUCCESS && pCopyConstructor) {
				pCopyConstructor(t);
			}
		}
		p = p->pListNext;
	}
	target->pInternalPointer = target->pListHead;
}
/* }}} */

ZEND_API ulong zend_u_get_hash_value(zend_uchar type, zstr arKey, uint nKeyLength) /* {{{ */
{
	return zend_u_inline_hash_func(type, arKey, nKeyLength);
}
/* }}} */

ZEND_API ulong zend_get_hash_value(const char *arKey, uint nKeyLength) /* {{{ */
{
	return zend_u_get_hash_value(IS_STRING, ZSTR(arKey), nKeyLength);
}
/* }}} */

/* Returns SUCCESS if found and FAILURE if not. The pointer to the
 * data is returned in pData. The reason is that there's no reason
 * someone using the hash table might not want to have NULL data
 */
ZEND_API int zend_u_hash_find(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength, void **pData) /* {{{ */
{
	ulong h;
	uint nIndex;
	Bucket *p;
	void *tmp = NULL;
	uint realKeyLength;

	IS_CONSISTENT(ht);

	UNICODE_KEY(ht, type, arKey, nKeyLength, tmp);
	realKeyLength = USTR_BYTES(type, nKeyLength);

	h = zend_u_inline_hash_func(type, arKey, nKeyLength);
	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) &&
		    (p->key.type == type) &&
		    (p->nKeyLength == nKeyLength) &&
		    !memcmp(p->key.arKey.s, arKey.s, realKeyLength)) {
			*pData = p->pData;
			if (tmp) efree(tmp);
			return SUCCESS;
		}
		p = p->pNext;
	}
	if (tmp) efree(tmp);
	return FAILURE;
}
/* }}} */

ZEND_API int zend_hash_find(HashTable *ht, const char *arKey, uint nKeyLength, void **pData) /* {{{ */
{
	return zend_u_hash_find(ht, IS_STRING, ZSTR(arKey), nKeyLength, pData);
}
/* }}} */

ZEND_API int zend_ascii_hash_find(HashTable *ht, const char *arKey, uint nKeyLength, void **pData) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;

		key.u = zend_ascii_to_unicode(arKey, nKeyLength ZEND_FILE_LINE_CC);
		ret = zend_u_hash_find(ht, IS_UNICODE, key, nKeyLength, pData);
		efree(key.u);
		return ret;
	} else {
		return zend_u_hash_find(ht, IS_STRING, ZSTR(arKey), nKeyLength, pData);
	}
}
/* }}} */

ZEND_API int zend_rt_hash_find(HashTable *ht, const char *arKey, uint nKeyLength, void **pData) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;
		UErrorCode status = U_ZERO_ERROR;
		int u_len;

		zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &key.u, &u_len, arKey, nKeyLength-1, &status);
		if (U_FAILURE(status)) {
			ZEND_HASH_CVT_ERROR();
			goto string_key;
		}
		ret = zend_u_hash_find(ht, IS_UNICODE, key, u_len+1, pData);
		efree(key.u);
		return ret;
	} else {
string_key:
		return zend_u_hash_find(ht, IS_STRING, ZSTR(arKey), nKeyLength, pData);
	}
}
/* }}} */

ZEND_API int zend_utf8_hash_find(HashTable *ht, const char *arKey, uint nKeyLength, void **pData) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;
		UErrorCode status = U_ZERO_ERROR;
		int u_len;

		zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(utf8_conv)), &key.u, &u_len, arKey, nKeyLength-1, &status);
		if (U_FAILURE(status)) {
			ZEND_HASH_CVT_ERROR();
			goto string_key;
		}
		ret = zend_u_hash_find(ht, IS_UNICODE, key, u_len+1, pData);
		efree(key.u);
		return ret;
	} else {
string_key:
		return zend_u_hash_find(ht, IS_STRING, ZSTR(arKey), nKeyLength, pData);
	}
}
/* }}} */

ZEND_API int zend_u_hash_quick_find(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength, ulong h, void **pData) /* {{{ */
{
	uint nIndex;
	Bucket *p;
	void *tmp = NULL;
	uint realKeyLength;

	if (nKeyLength==0) {
		return zend_hash_index_find(ht, h, pData);
	}

	IS_CONSISTENT(ht);

	if (ht->unicode && type == IS_STRING) {
		UNICODE_KEY(ht, type, arKey, nKeyLength, tmp);
		h = zend_u_inline_hash_func(IS_UNICODE, arKey, nKeyLength);
	}
	realKeyLength = USTR_BYTES(type, nKeyLength);

	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) &&
		    (p->key.type == type) &&
		    (p->nKeyLength == nKeyLength) &&
		    !memcmp(p->key.arKey.s, arKey.s, realKeyLength)) {
			*pData = p->pData;
			if (tmp) efree(tmp);
			return SUCCESS;
		}
		p = p->pNext;
	}
	if (tmp) efree(tmp);
	return FAILURE;
}
/* }}} */

ZEND_API int zend_hash_quick_find(HashTable *ht, const char *arKey, uint nKeyLength, ulong h, void **pData) /* {{{ */
{
	return zend_u_hash_quick_find(ht, IS_STRING, ZSTR(arKey), nKeyLength, h, pData);
}
/* }}} */

ZEND_API int zend_u_hash_exists(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength) /* {{{ */
{
	ulong h;
	uint nIndex;
	Bucket *p;
	void *tmp = NULL;
	uint realKeyLength;

	IS_CONSISTENT(ht);

	UNICODE_KEY(ht, type, arKey, nKeyLength, tmp);
	realKeyLength = USTR_BYTES(type, nKeyLength);

	h = zend_u_inline_hash_func(type, arKey, nKeyLength);
	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) &&
		    (p->key.type == type) &&
		    (p->nKeyLength == nKeyLength) &&
		    !memcmp(p->key.arKey.s, arKey.s, realKeyLength)) {
			if (tmp) efree(tmp);
			return 1;
		}
		p = p->pNext;
	}
	if (tmp) efree(tmp);
	return 0;
}
/* }}} */

ZEND_API int zend_hash_exists(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	return zend_u_hash_exists(ht, IS_STRING, ZSTR(arKey), nKeyLength);
}
/* }}} */

ZEND_API int zend_ascii_hash_exists(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;

		key.u = zend_ascii_to_unicode(arKey, nKeyLength ZEND_FILE_LINE_CC);
		ret = zend_u_hash_exists(ht, IS_UNICODE, key, nKeyLength);
		efree(key.u);
		return ret;
	} else {
		return zend_u_hash_exists(ht, IS_STRING, ZSTR(arKey), nKeyLength);
	}
}
/* }}} */

ZEND_API int zend_rt_hash_exists(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;
		UErrorCode status = U_ZERO_ERROR;
		int u_len;

		zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &key.u, &u_len, arKey, nKeyLength-1, &status);
		if (U_FAILURE(status)) {
			ZEND_HASH_CVT_ERROR();
			goto string_key;
		}
		ret = zend_u_hash_exists(ht, IS_UNICODE, key, u_len+1);
		efree(key.u);
		return ret;
	} else {
string_key:
		return zend_u_hash_exists(ht, IS_STRING, ZSTR(arKey), nKeyLength);
    }
}
/* }}} */

ZEND_API int zend_utf8_hash_exists(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	TSRMLS_FETCH();

	if (UG(unicode)) {
		zstr key;
		int ret;
		UErrorCode status = U_ZERO_ERROR;
		int u_len;

		zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(utf8_conv)), &key.u, &u_len, arKey, nKeyLength-1, &status);
		if (U_FAILURE(status)) {
			ZEND_HASH_CVT_ERROR();
			goto string_key;
		}
		ret = zend_u_hash_exists(ht, IS_UNICODE, key, u_len+1);
		efree(key.u);
		return ret;
	} else {
string_key:
		return zend_u_hash_exists(ht, IS_STRING, ZSTR(arKey), nKeyLength);
    }
}
/* }}} */

ZEND_API int zend_u_hash_quick_exists(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength, ulong h) /* {{{ */
{
	uint nIndex;
	Bucket *p;
	void *tmp = NULL;
	uint realKeyLength;

	if (nKeyLength==0) {
		return zend_hash_index_exists(ht, h);
	}

	IS_CONSISTENT(ht);

	if (ht->unicode && type == IS_STRING) {
		UNICODE_KEY(ht, type, arKey, nKeyLength, tmp);
		h = zend_u_inline_hash_func(type, arKey, nKeyLength);
	}
	realKeyLength = USTR_BYTES(type, nKeyLength);

	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) &&
		    (p->key.type == type) &&
		    (p->nKeyLength == nKeyLength) &&
		    !memcmp(p->key.arKey.s, arKey.s, realKeyLength)) {
		  if (tmp) efree(tmp);
			return 1;
		}
		p = p->pNext;
	}
  if (tmp) efree(tmp);
	return 0;
}
/* }}} */

ZEND_API int zend_hash_quick_exists(HashTable *ht, const char *arKey, uint nKeyLength, ulong h) /* {{{ */
{
	return zend_u_hash_quick_exists(ht, IS_STRING, ZSTR(arKey), nKeyLength, h);
}
/* }}} */

ZEND_API int zend_hash_index_find(HashTable *ht, ulong h, void **pData) /* {{{ */
{
	uint nIndex;
	Bucket *p;

	IS_CONSISTENT(ht);

	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->nKeyLength == 0)) {
			*pData = p->pData;
			return SUCCESS;
		}
		p = p->pNext;
	}
	return FAILURE;
}
/* }}} */

ZEND_API int zend_hash_index_exists(HashTable *ht, ulong h) /* {{{ */
{
	uint nIndex;
	Bucket *p;

	IS_CONSISTENT(ht);

	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->nKeyLength == 0)) {
			return 1;
		}
		p = p->pNext;
	}
	return 0;
}
/* }}} */

ZEND_API int zend_hash_num_elements(HashTable *ht) /* {{{ */
{
	IS_CONSISTENT(ht);

	return ht->nNumOfElements;
}
/* }}} */

ZEND_API int zend_hash_get_pointer(HashTable *ht, HashPointer *ptr) /* {{{ */
{
	ptr->pos = ht->pInternalPointer;
	if (ht->pInternalPointer) {
		ptr->h = ht->pInternalPointer->h;
		return 1;
	} else {
		ptr->h = 0;
		return 0;
	}
}
/* }}} */

ZEND_API int zend_hash_set_pointer(HashTable *ht, const HashPointer *ptr) /* {{{ */
{
	if (ptr->pos == NULL) {
		ht->pInternalPointer = NULL;
	} else if (ht->pInternalPointer != ptr->pos) {
		Bucket *p;

		IS_CONSISTENT(ht);
		p = ht->arBuckets[ptr->h & ht->nTableMask];
		while (p != NULL) {
			if (p == ptr->pos) {
				ht->pInternalPointer = p;
				return 1;
			}
			p = p->pNext;
		}
		return 0;
	}
	return 1;
}
/* }}} */

ZEND_API void zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos) /* {{{ */
{
	IS_CONSISTENT(ht);

	if (pos)
		*pos = ht->pListHead;
	else
		ht->pInternalPointer = ht->pListHead;
}
/* }}} */

/* This function will be extremely optimized by remembering
 * the end of the list
 */
ZEND_API void zend_hash_internal_pointer_end_ex(HashTable *ht, HashPosition *pos) /* {{{ */
{
	IS_CONSISTENT(ht);

	if (pos)
		*pos = ht->pListTail;
	else
		ht->pInternalPointer = ht->pListTail;
}
/* }}} */

ZEND_API int zend_hash_move_forward_ex(HashTable *ht, HashPosition *pos) /* {{{ */
{
	HashPosition *current = pos ? pos : &ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (*current) {
		*current = (*current)->pListNext;
		return SUCCESS;
	} else
		return FAILURE;
}
/* }}} */

ZEND_API int zend_hash_move_backwards_ex(HashTable *ht, HashPosition *pos) /* {{{ */
{
	HashPosition *current = pos ? pos : &ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (*current) {
		*current = (*current)->pListLast;
		return SUCCESS;
	} else
		return FAILURE;
}
/* }}} */

/* This function should be made binary safe  */
ZEND_API int zend_hash_get_current_key_ex(HashTable *ht, zstr *str_index, uint *str_length, ulong *num_index, zend_bool duplicate, HashPosition *pos) /* {{{ */
{
	Bucket *p;

	p = pos ? (*pos) : ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (p) {
		if (p->nKeyLength) {
			if (p->key.type == IS_STRING) {
				if (duplicate) {
					str_index->s = estrndup(p->key.arKey.s, p->nKeyLength-1);
				} else {
					str_index->s = p->key.arKey.s;
				}
				if (str_length) {
					*str_length = p->nKeyLength;
				}
				return HASH_KEY_IS_STRING;
			} else if (p->key.type == IS_UNICODE) {
				if (duplicate) {
					str_index->u = eustrndup(p->key.arKey.u, p->nKeyLength-1);
				} else {
					str_index->u = p->key.arKey.u;
				}
				if (str_length) {
					*str_length = p->nKeyLength;
				}
				return HASH_KEY_IS_UNICODE;
			}
		} else {
			*num_index = p->h;
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTANT;
}
/* }}} */

ZEND_API int zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos) /* {{{ */
{
	Bucket *p;

	p = pos ? (*pos) : ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (p) {
		if (p->nKeyLength) {
			if (p->key.type == IS_UNICODE) {
				return HASH_KEY_IS_UNICODE;
			} else {
				return HASH_KEY_IS_STRING;
			}
		} else {
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTANT;
}
/* }}} */

ZEND_API int zend_hash_get_current_data_ex(HashTable *ht, void **pData, HashPosition *pos) /* {{{ */
{
	Bucket *p;

	p = pos ? (*pos) : ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (p) {
		*pData = p->pData;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

/* This function changes key of currevt element without changing elements'
 * order. If element with target key already exists, it will be deleted first.
 */
ZEND_API int zend_hash_update_current_key_ex(HashTable *ht, int key_type, zstr str_index, uint str_length, ulong num_index, int mode, HashPosition *pos) /* {{{ */
{
	Bucket *p;
	uint real_length;

	p = pos ? (*pos) : ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (p) {
		if (key_type == HASH_KEY_IS_LONG) {
			real_length = str_length = 0;
			if (!p->nKeyLength && p->h == num_index) {
				return SUCCESS;
			}

			if (mode != HASH_UPDATE_KEY_ANYWAY) {
				Bucket *q = ht->arBuckets[num_index & ht->nTableMask];
				int found = 0;

				while (q != NULL) {
					if (q == p) {
						found = 1;
					} else if (!q->nKeyLength && q->h == num_index) {
					    if (found) {
					    	if (mode & HASH_UPDATE_KEY_IF_BEFORE) {
					    		break;
					    	} else {
					    		if (p->nKeyLength) {
									zend_u_hash_del(ht, p->key.type, ZSTR(p->key.arKey.s), p->nKeyLength);
								} else {
									zend_hash_index_del(ht, p->h);
								}
					    		return FAILURE;
					    	}
					    } else {
					    	if (mode & HASH_UPDATE_KEY_IF_AFTER) {
					    		break;
					    	} else {
					    		if (p->nKeyLength) {
									zend_u_hash_del(ht, p->key.type, ZSTR(p->key.arKey.s), p->nKeyLength);
								} else {
									zend_hash_index_del(ht, p->h);
								}
					    		return FAILURE;
					    	}
						}
					}
					q = q->pNext;
				}
			}

			zend_hash_index_del(ht, num_index);
		} else if (key_type == HASH_KEY_IS_STRING ||
		           key_type == HASH_KEY_IS_UNICODE) {
			real_length = key_type == HASH_KEY_IS_STRING ? str_length : str_length * sizeof(UChar);
			if (p->nKeyLength == str_length &&
			    p->key.type == key_type &&
			    memcmp(p->key.arKey.s, str_index.v, real_length) == 0) {
				return SUCCESS;
			}

			if (mode != HASH_UPDATE_KEY_ANYWAY) {
				ulong h = zend_u_inline_hash_func(key_type, str_index, str_length);
				Bucket *q = ht->arBuckets[h & ht->nTableMask];
				int found = 0;

				while (q != NULL) {
					if (q == p) {
						found = 1;
					} else if (q->h == h && q->nKeyLength == str_length && 
					           q->key.type == key_type &&
					           memcmp(q->key.arKey.s, str_index.v, real_length) == 0) {
					    if (found) {
					    	if (mode & HASH_UPDATE_KEY_IF_BEFORE) {
					    		break;
					    	} else {
					    		if (p->nKeyLength) {
									zend_u_hash_del(ht, p->key.type, ZSTR(p->key.arKey.s), p->nKeyLength);
								} else {
									zend_hash_index_del(ht, p->h);
								}
					    		return FAILURE;
					    	}
					    } else {
					    	if (mode & HASH_UPDATE_KEY_IF_AFTER) {
					    		break;
					    	} else {
					    		if (p->nKeyLength) {
									zend_u_hash_del(ht, p->key.type, ZSTR(p->key.arKey.s), p->nKeyLength);
								} else {
									zend_hash_index_del(ht, p->h);
								}
					    		return FAILURE;
					    	}
						}
					}
					q = q->pNext;
				}
			}

			zend_u_hash_del(ht, key_type, str_index, str_length);
		} else {
			return FAILURE;
		}

		HANDLE_BLOCK_INTERRUPTIONS();

		if (p->pNext) {
			p->pNext->pLast = p->pLast;
		}
		if (p->pLast) {
			p->pLast->pNext = p->pNext;
		} else{
			ht->arBuckets[p->h & ht->nTableMask] = p->pNext;
		}

		if (p->nKeyLength != str_length) {
			Bucket *q = (Bucket *) pemalloc(sizeof(Bucket) - 1 + real_length, ht->persistent);

			q->nKeyLength = str_length;
			if (p->pData == &p->pDataPtr) {
				q->pData = &q->pDataPtr;
			} else {
				q->pData = p->pData;
			}
			q->pDataPtr = p->pDataPtr;
			q->pListNext = p->pListNext;
			q->pListLast = p->pListLast;
			if (q->pListNext) {
				p->pListNext->pListLast = q;
			} else {
				ht->pListTail = q;
			}
			if (q->pListLast) {
				p->pListLast->pListNext = q;
			} else {
				ht->pListHead = q;
			}
			if (ht->pInternalPointer == p) {
				ht->pInternalPointer = q;
			}
			if (pos) {
				*pos = q;
			}
			pefree(p, ht->persistent);
			p = q;
		}

		if (key_type == HASH_KEY_IS_LONG) {
			p->h = num_index;
		} else if (key_type == HASH_KEY_IS_UNICODE) {
			memcpy(p->key.arKey.u, str_index.u, real_length);
			p->key.type = IS_UNICODE;
			p->h = zend_u_inline_hash_func(IS_UNICODE, str_index, str_length);
		} else {
			memcpy(p->key.arKey.s, str_index.s, real_length);
			p->key.type = IS_STRING;
			p->h = zend_u_inline_hash_func(p->key.type, str_index, str_length);
		}

		CONNECT_TO_BUCKET_DLLIST(p, ht->arBuckets[p->h & ht->nTableMask]);
		ht->arBuckets[p->h & ht->nTableMask] = p;
		HANDLE_UNBLOCK_INTERRUPTIONS();

		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

ZEND_API int zend_hash_sort(HashTable *ht, sort_func_t sort_func, compare_func_t compar, int renumber TSRMLS_DC) /* {{{ */
{
	Bucket **arTmp;
	Bucket *p;
	int i, j;

	IS_CONSISTENT(ht);

	if (!(ht->nNumOfElements>1) && !(renumber && ht->nNumOfElements>0)) { /* Doesn't require sorting */
		return SUCCESS;
	}
	arTmp = (Bucket **) pemalloc(ht->nNumOfElements * sizeof(Bucket *), ht->persistent);
	if (!arTmp) {
		return FAILURE;
	}
	p = ht->pListHead;
	i = 0;
	while (p) {
		arTmp[i] = p;
		p = p->pListNext;
		i++;
	}

	(*sort_func)((void *) arTmp, i, sizeof(Bucket *), compar TSRMLS_CC);

	HANDLE_BLOCK_INTERRUPTIONS();
	ht->pListHead = arTmp[0];
	ht->pListTail = NULL;
	ht->pInternalPointer = ht->pListHead;

	arTmp[0]->pListLast = NULL;
	if (i > 1) {
		arTmp[0]->pListNext = arTmp[1];
		for (j = 1; j < i-1; j++) {
			arTmp[j]->pListLast = arTmp[j-1];
			arTmp[j]->pListNext = arTmp[j+1];
		}
		arTmp[j]->pListLast = arTmp[j-1];
		arTmp[j]->pListNext = NULL;
	} else {
		arTmp[0]->pListNext = NULL;
	}
	ht->pListTail = arTmp[i-1];

	pefree(arTmp, ht->persistent);
	HANDLE_UNBLOCK_INTERRUPTIONS();

	if (renumber) {
		p = ht->pListHead;
		i=0;
		while (p != NULL) {
			p->nKeyLength = 0;
			p->h = i++;
			p = p->pListNext;
		}
		ht->nNextFreeElement = i;
		zend_hash_rehash(ht);
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_func_t compar, zend_bool ordered TSRMLS_DC) /* {{{ */
{
	Bucket *p1, *p2 = NULL;
	int result;
	void *pData2;

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

	p1 = ht1->pListHead;
	if (ordered) {
		p2 = ht2->pListHead;
	}

	while (p1) {
		if (ordered && !p2) {
			HASH_UNPROTECT_RECURSION(ht1);
			HASH_UNPROTECT_RECURSION(ht2);
			return 1; /* That's not supposed to happen */
		}
		if (ordered) {
			if (p1->nKeyLength==0 && p2->nKeyLength==0) { /* numeric indices */
				result = p1->h - p2->h;
				if (result!=0) {
					HASH_UNPROTECT_RECURSION(ht1);
					HASH_UNPROTECT_RECURSION(ht2);
					return result;
				}
			} else { /* string indices */
			  result = p1->key.type - p2->key.type;
				if (result==0) {
					result = p1->nKeyLength - p2->nKeyLength;
				}
				if (result==0) {
					result = memcmp(p1->key.arKey.s, p2->key.arKey.s, USTR_BYTES(p1->key.type, p1->nKeyLength));
				}
				if (result!=0) {
					HASH_UNPROTECT_RECURSION(ht1);
					HASH_UNPROTECT_RECURSION(ht2);
					return result;
				}
			}
			pData2 = p2->pData;
		} else {
			if (p1->nKeyLength==0) { /* numeric index */
				if (zend_hash_index_find(ht2, p1->h, &pData2)==FAILURE) {
					HASH_UNPROTECT_RECURSION(ht1);
					HASH_UNPROTECT_RECURSION(ht2);
					return 1;
				}
			} else { /* string, binary or unicode index */
				if (zend_u_hash_find(ht2, p1->key.type, ZSTR(p1->key.arKey.s), p1->nKeyLength, &pData2)==FAILURE) {
					HASH_UNPROTECT_RECURSION(ht1);
					HASH_UNPROTECT_RECURSION(ht2);
					return 1;
				}
			}
		}
		result = compar(p1->pData, pData2 TSRMLS_CC);
		if (result!=0) {
			HASH_UNPROTECT_RECURSION(ht1);
			HASH_UNPROTECT_RECURSION(ht2);
			return result;
		}
		p1 = p1->pListNext;
		if (ordered) {
			p2 = p2->pListNext;
		}
	}

	HASH_UNPROTECT_RECURSION(ht1);
	HASH_UNPROTECT_RECURSION(ht2);
	return 0;
}
/* }}} */

ZEND_API int zend_hash_minmax(HashTable *ht, compare_func_t compar, int flag, void **pData TSRMLS_DC) /* {{{ */
{
	Bucket *p, *res;

	IS_CONSISTENT(ht);

	if (ht->nNumOfElements == 0 ) {
		*pData=NULL;
		return FAILURE;
	}

	res = p = ht->pListHead;
	while ((p = p->pListNext)) {
		if (flag) {
			if (compar(&res, &p TSRMLS_CC) < 0) { /* max */
				res = p;
			}
		} else {
			if (compar(&res, &p TSRMLS_CC) > 0) { /* min */
				res = p;
			}
		}
	}
	*pData = res->pData;
	return SUCCESS;
}
/* }}} */

ZEND_API ulong zend_hash_next_free_element(HashTable *ht) /* {{{ */
{
	IS_CONSISTENT(ht);

	return ht->nNextFreeElement;

}
/* }}} */

ZEND_API int zend_u_symtable_update(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest) /* {{{ */
{
	if (type == IS_STRING) {
		ZEND_HANDLE_NUMERIC(arKey.s, nKeyLength, zend_hash_index_update(ht, idx, pData, nDataSize, pDest));
	} else if (type == IS_UNICODE) {
		ZEND_HANDLE_U_NUMERIC(arKey.u, nKeyLength, zend_hash_index_update(ht, idx, pData, nDataSize, pDest));
	}
	return zend_u_hash_update(ht, type, arKey, nKeyLength, pData, nDataSize, pDest);
}
/* }}} */

ZEND_API int zend_u_symtable_del(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength) /* {{{ */
{
	if (type == IS_STRING) {
		ZEND_HANDLE_NUMERIC(arKey.s, nKeyLength, zend_hash_index_del(ht, idx));
	} else if (type == IS_UNICODE) {
		ZEND_HANDLE_U_NUMERIC(arKey.u, nKeyLength, zend_hash_index_del(ht, idx));
	}
	return zend_u_hash_del(ht, type, arKey, nKeyLength);
}
/* }}} */

ZEND_API int zend_u_symtable_find(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength, void **pData) /* {{{ */
{
	if (type == IS_STRING) {
		ZEND_HANDLE_NUMERIC(arKey.s, nKeyLength, zend_hash_index_find(ht, idx, pData));
	} else if (type == IS_UNICODE) {
		ZEND_HANDLE_U_NUMERIC(arKey.u, nKeyLength, zend_hash_index_find(ht, idx, pData));
	}
	return zend_u_hash_find(ht, type, arKey, nKeyLength, pData);
}
/* }}} */

ZEND_API int zend_u_symtable_exists(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength) /* {{{ */
{
	if (type == IS_STRING) {
		ZEND_HANDLE_NUMERIC(arKey.s, nKeyLength, zend_hash_index_exists(ht, idx));
	} else if (type == IS_UNICODE) {
		ZEND_HANDLE_U_NUMERIC(arKey.u, nKeyLength, zend_hash_index_exists(ht, idx));
	}
	return zend_u_hash_exists(ht, type, arKey, nKeyLength);
}
/* }}} */

ZEND_API int zend_u_symtable_update_current_key(HashTable *ht, zend_uchar type, zstr arKey, uint nKeyLength, int mode) /* {{{ */
{
	zend_uchar key_type;

	if (type == IS_STRING) {
		key_type = HASH_KEY_IS_STRING;
		ZEND_HANDLE_NUMERIC(arKey.s, nKeyLength, zend_hash_update_current_key_ex(ht, HASH_KEY_IS_LONG, NULL_ZSTR, 0, idx, mode, NULL));
	} else {
		key_type = HASH_KEY_IS_UNICODE;
		ZEND_HANDLE_U_NUMERIC(arKey.u, nKeyLength, zend_hash_update_current_key_ex(ht, HASH_KEY_IS_LONG, NULL_ZSTR, 0, idx, mode, NULL));
	}
	return zend_hash_update_current_key_ex(ht, key_type, arKey, nKeyLength, 0, mode, NULL);
}
/* }}} */

ZEND_API int zend_symtable_update(HashTable *ht, const char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_update(ht, idx, pData, nDataSize, pDest));
	return zend_hash_update(ht, arKey, nKeyLength, pData, nDataSize, pDest);
}
/* }}} */

ZEND_API int zend_symtable_del(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_del(ht, idx))
	return zend_hash_del(ht, arKey, nKeyLength);
}
/* }}} */

ZEND_API int zend_symtable_find(HashTable *ht, const char *arKey, uint nKeyLength, void **pData) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_find(ht, idx, pData));
	return zend_hash_find(ht, arKey, nKeyLength, pData);
}
/* }}} */

ZEND_API int zend_symtable_exists(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_exists(ht, idx));
	return zend_hash_exists(ht, arKey, nKeyLength);
}
/* }}} */

ZEND_API int zend_symtable_update_current_key(HashTable *ht, const char *arKey, uint nKeyLength, int mode) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_update_current_key_ex(ht, HASH_KEY_IS_LONG, NULL_ZSTR, 0, idx, mode, NULL));
	return zend_hash_update_current_key_ex(ht, HASH_KEY_IS_STRING, ZSTR(arKey), nKeyLength, 0, mode, NULL);
}
/* }}} */

ZEND_API int zend_ascii_symtable_update(HashTable *ht, const char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_update(ht, idx, pData, nDataSize, pDest));
	return zend_ascii_hash_update(ht, arKey, nKeyLength, pData, nDataSize, pDest);
}
/* }}} */

ZEND_API int zend_ascii_symtable_del(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_del(ht, idx))
	return zend_ascii_hash_del(ht, arKey, nKeyLength);
}
/* }}} */

ZEND_API int zend_ascii_symtable_find(HashTable *ht, const char *arKey, uint nKeyLength, void **pData) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_find(ht, idx, pData));
	return zend_ascii_hash_find(ht, arKey, nKeyLength, pData);
}
/* }}} */

ZEND_API int zend_ascii_symtable_exists(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_exists(ht, idx));
	return zend_ascii_hash_exists(ht, arKey, nKeyLength);
}
/* }}} */

ZEND_API int zend_rt_symtable_update(HashTable *ht, const char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_update(ht, idx, pData, nDataSize, pDest));
	return zend_rt_hash_update(ht, arKey, nKeyLength, pData, nDataSize, pDest);
}
/* }}} */

ZEND_API int zend_rt_symtable_del(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_del(ht, idx))
	return zend_rt_hash_del(ht, arKey, nKeyLength);
}
/* }}} */

ZEND_API int zend_rt_symtable_find(HashTable *ht, const char *arKey, uint nKeyLength, void **pData) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_find(ht, idx, pData));
	return zend_rt_hash_find(ht, arKey, nKeyLength, pData);
}
/* }}} */

ZEND_API int zend_rt_symtable_exists(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_exists(ht, idx));
	return zend_rt_hash_exists(ht, arKey, nKeyLength);
}
/* }}} */

ZEND_API int zend_utf8_symtable_update(HashTable *ht, const char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_update(ht, idx, pData, nDataSize, pDest));
	return zend_utf8_hash_update(ht, arKey, nKeyLength, pData, nDataSize, pDest);
}
/* }}} */

ZEND_API int zend_utf8_symtable_del(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_del(ht, idx))
	return zend_utf8_hash_del(ht, arKey, nKeyLength);
}
/* }}} */

ZEND_API int zend_utf8_symtable_find(HashTable *ht, const char *arKey, uint nKeyLength, void **pData) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_find(ht, idx, pData));
	return zend_utf8_hash_find(ht, arKey, nKeyLength, pData);
}
/* }}} */

ZEND_API int zend_utf8_symtable_exists(HashTable *ht, const char *arKey, uint nKeyLength) /* {{{ */
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_exists(ht, idx));
	return zend_utf8_hash_exists(ht, arKey, nKeyLength);
}
/* }}} */

ZEND_API void zend_hash_to_unicode(HashTable *ht, apply_func_t apply_func TSRMLS_DC) /* {{{ */
{
	Bucket **p;
	uint nIndex;

	IS_CONSISTENT(ht);
	if (ht->unicode) {
		return;
	}

	ht->unicode = 1;
	memset(ht->arBuckets, 0, ht->nTableSize * sizeof(Bucket *));
	p = &ht->pListHead;
	while ((*p) != NULL) {
		if ((*p)->key.type == IS_STRING) {
			UErrorCode status = U_ZERO_ERROR;
			UChar *u = NULL;
			int u_len;
			Bucket *q;

			zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &u, &u_len, (*p)->key.arKey.s, (*p)->nKeyLength-1, &status);
			if (U_FAILURE(status)) {
				zend_error(E_ERROR, "Cannot convert HashTable to Unicode");
			}

			q = (Bucket *) pemalloc(sizeof(Bucket)-sizeof(q->key.arKey)+((u_len+1)*2), ht->persistent);
			memcpy(q, *p, sizeof(Bucket)-sizeof(q->key.arKey));
			memcpy(q->key.arKey.u, u, (u_len+1)*2);
			q->key.type = IS_UNICODE;
			q->nKeyLength = u_len+1;
			q->h = zend_u_inline_hash_func(IS_UNICODE, q->key.arKey, q->nKeyLength);
			if ((*p)->pData == &(*p)->pDataPtr) {
				q->pData = &q->pDataPtr;
			}
			efree(u);
			pefree(*p, ht->persistent);
			*p = q;
			if (q->pListNext) {
				q->pListNext->pListLast = q;
			} else {
				ht->pListTail = q;
			}
		}
		nIndex = (*p)->h & ht->nTableMask;
		CONNECT_TO_BUCKET_DLLIST(*p, ht->arBuckets[nIndex]);
		ht->arBuckets[nIndex] = *p;
		if (apply_func) {
			apply_func((*p)->pData TSRMLS_CC);
		}
		p = &(*p)->pListNext;
	}
}
/* }}} */

#if ZEND_DEBUG
void zend_hash_display_pListTail(HashTable *ht) /* {{{ */
{
	Bucket *p;

	p = ht->pListTail;
	while (p != NULL) {
		if (p->key.type == IS_UNICODE) {
			/* FIXME: Unicode support??? */
		} else {
			zend_output_debug_string(0, "pListTail has key %s\n", p->key.arKey.s);
		}
		p = p->pListLast;
	}
}
/* }}} */

void zend_hash_display(HashTable *ht) /* {{{ */
{
	Bucket *p;
	uint i;

	for (i = 0; i < ht->nTableSize; i++) {
		p = ht->arBuckets[i];
		while (p != NULL) {
			if (p->key.type == IS_UNICODE) {
				/* FIXME: Unicode support??? */
			} else {
				zend_output_debug_string(0, "%s <==> 0x%lX\n", p->key.arKey.s, p->h);
			}
			p = p->pNext;
		}
	}

	p = ht->pListTail;
	while (p != NULL) {
		if (p->key.type == IS_UNICODE) {
			/* FIXME: Unicode support??? */
		} else {
			zend_output_debug_string(0, "%s <==> 0x%lX\n", p->key.arKey.s, p->h);
		}
		p = p->pListLast;
	}
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
