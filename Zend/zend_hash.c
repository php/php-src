/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
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

#define HANDLE_NUMERIC(key, length, func) {												\
	register char *tmp=key;																\
																						\
	if ((*tmp>='0' && *tmp<='9')) do { /* possibly a numeric index */					\
		char *end=tmp+length-1;															\
		ulong idx;																		\
																						\
		if (*tmp++=='0' && length>2) { /* don't accept numbers with leading zeros */	\
			break;																		\
		}																				\
		while (tmp<end) {																\
			if (!(*tmp>='0' && *tmp<='9')) {											\
				break;																	\
			}																			\
			tmp++;																		\
		}																				\
		if (tmp==end && *tmp=='\0') { /* a numeric index */								\
			idx = strtol(key, NULL, 10);													\
			if (idx!=LONG_MAX) {														\
				return func;															\
			}																			\
		}																				\
	} while (0);																			\
}


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

#if ZEND_DEBUG
#define HT_OK				0
#define HT_IS_DESTROYING	1
#define HT_DESTROYED		2
#define HT_CLEANING			3

static void _zend_is_inconsistent(HashTable *ht, char *file, int line)
{
	if (ht->inconsistent==HT_OK) {
		return;
	}
	switch (ht->inconsistent) {
		case HT_IS_DESTROYING:
			zend_output_debug_string(1, "%s(%d) : ht=0x%08x is being destroyed", file, line, ht);
			break;
		case HT_DESTROYED:
			zend_output_debug_string(1, "%s(%d) : ht=0x%08x is already destroyed", file, line, ht);
			break;
		case HT_CLEANING:
			zend_output_debug_string(1, "%s(%d) : ht=0x%08x is being cleaned", file, line, ht);
			break;
	}
	zend_bailout();
}
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

ZEND_API ulong zend_hash_func(char *arKey, uint nKeyLength)
{
	return zend_inline_hash_func(arKey, nKeyLength);
}


#define UPDATE_DATA(ht, p, pData, nDataSize)								\
	if (nDataSize == sizeof(void*)) {										\
		if (!(p)->pDataPtr) {												\
			pefree((p)->pData, (ht)->persistent);							\
		}																	\
		memcpy(&(p)->pDataPtr, pData, sizeof(void *));						\
		(p)->pData = &(p)->pDataPtr;										\
	} else {																\
		if ((p)->pDataPtr) {												\
			(p)->pData = (void *) pemalloc(nDataSize, (ht)->persistent);	\
			(p)->pDataPtr=NULL;												\
		}																	\
		memcpy((p)->pData, pData, nDataSize);								\
	}

#define INIT_DATA(ht, p, pData, nDataSize);								\
	if (nDataSize == sizeof(void*)) {									\
		memcpy(&(p)->pDataPtr, pData, sizeof(void *));					\
		(p)->pData = &(p)->pDataPtr;									\
	} else {															\
		(p)->pData = (void *) pemalloc(nDataSize, (ht)->persistent);	\
		if (!(p)->pData) {												\
			pefree(p, (ht)->persistent);								\
			return FAILURE;												\
		}																\
		memcpy((p)->pData, pData, nDataSize);							\
		(p)->pDataPtr=NULL;												\
	}


ZEND_API int _zend_hash_init(HashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent ZEND_FILE_LINE_DC)
{
	uint i = 3;

	SET_INCONSISTENT(HT_OK);

	while ((1U << i) < nSize) {
		i++;
	}

	ht->nTableSize = 1 << i;
	ht->nTableMask = ht->nTableSize - 1;
	
	/* Uses ecalloc() so that Bucket* == NULL */
	if (persistent) {
		ht->arBuckets = (Bucket **) calloc(ht->nTableSize, sizeof(Bucket *));
	} else {
		ht->arBuckets = (Bucket **) ecalloc_rel(ht->nTableSize, sizeof(Bucket *));
	}
	
	if (!ht->arBuckets) {
		return FAILURE;
	}

	ht->pDestructor = pDestructor;
	ht->pListHead = NULL;
	ht->pListTail = NULL;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->pInternalPointer = NULL;
	ht->persistent = persistent;
	ht->nApplyCount = 0;
	ht->bApplyProtection = 1;
	return SUCCESS;
}


ZEND_API int _zend_hash_init_ex(HashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC)
{
	int retval = _zend_hash_init(ht, nSize, pHashFunction, pDestructor, persistent ZEND_FILE_LINE_CC);

	ht->bApplyProtection = bApplyProtection;
	return retval;
}


ZEND_API void zend_hash_set_apply_protection(HashTable *ht, zend_bool bApplyProtection)
{
	ht->bApplyProtection = bApplyProtection;
}



ZEND_API int zend_hash_add_or_update(HashTable *ht, char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag)
{
	ulong h;
	uint nIndex;
	Bucket *p;

	IS_CONSISTENT(ht);

	if (nKeyLength <= 0) {
#if ZEND_DEBUG
		ZEND_PUTS("zend_hash_update: Can't put in empty key\n");
#endif
		return FAILURE;
	}

	HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_update_or_next_insert(ht, idx, pData, nDataSize, pDest, flag));
	
	h = zend_inline_hash_func(arKey, nKeyLength);
	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				if (flag & HASH_ADD) {
					return FAILURE;
				}
				HANDLE_BLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
				if (p->pData == pData) {
					ZEND_PUTS("Fatal error in zend_hash_update: p->pData == pData\n");
					HANDLE_UNBLOCK_INTERRUPTIONS();
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
				return SUCCESS;
			}
		}
		p = p->pNext;
	}
	
	p = (Bucket *) pemalloc(sizeof(Bucket)-1+nKeyLength, ht->persistent);
	if (!p) {
		return FAILURE;
	}
	memcpy(p->arKey, arKey, nKeyLength);
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
	return SUCCESS;
}

ZEND_API int zend_hash_quick_add_or_update(HashTable *ht, char *arKey, uint nKeyLength, ulong h, void *pData, uint nDataSize, void **pDest, int flag)
{
	uint nIndex;
	Bucket *p;

	IS_CONSISTENT(ht);

	if (nKeyLength == 0) {
		return zend_hash_index_update(ht, h, pData, nDataSize, pDest);
	}

	nIndex = h & ht->nTableMask;
	
	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				if (flag & HASH_ADD) {
					return FAILURE;
				}
				HANDLE_BLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
				if (p->pData == pData) {
					ZEND_PUTS("Fatal error in zend_hash_update: p->pData == pData\n");
					HANDLE_UNBLOCK_INTERRUPTIONS();
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
				return SUCCESS;
			}
		}
		p = p->pNext;
	}
	
	p = (Bucket *) pemalloc(sizeof(Bucket)-1+nKeyLength, ht->persistent);
	if (!p) {
		return FAILURE;
	}

	memcpy(p->arKey, arKey, nKeyLength);
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
	return SUCCESS;
}


ZEND_API int zend_hash_add_empty_element(HashTable *ht, char *arKey, uint nKeyLength)
{
	void *dummy = (void *) 1;

	return zend_hash_add(ht, arKey, nKeyLength, &dummy, sizeof(void *), NULL);
}


ZEND_API int zend_hash_index_update_or_next_insert(HashTable *ht, ulong h, void *pData, uint nDataSize, void **pDest, int flag)
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
	p = (Bucket *) pemalloc(sizeof(Bucket)-1, ht->persistent);
	if (!p) {
		return FAILURE;
	}
	p->nKeyLength = 0;			/*  Numeric indices are marked by making the nKeyLength == 0 */
	p->h = h;
	INIT_DATA(ht, p, pData, nDataSize);
	if (pDest) {
		*pDest = p->pData;
	}

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


static int zend_hash_do_resize(HashTable *ht)
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

ZEND_API int zend_hash_rehash(HashTable *ht)
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

ZEND_API int zend_hash_del_key_or_index(HashTable *ht, char *arKey, uint nKeyLength, ulong h, int flag)
{
	uint nIndex;
	Bucket *p;

	IS_CONSISTENT(ht);

	if (flag == HASH_DEL_KEY) {
		HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_del_key_or_index(ht, arKey, nKeyLength, idx, HASH_DEL_INDEX));
		h = zend_inline_hash_func(arKey, nKeyLength);
	}
	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && ((p->nKeyLength == 0) || /* Numeric index */
			((p->nKeyLength == nKeyLength) && (!memcmp(p->arKey, arKey, nKeyLength))))) {
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
			if (!p->pDataPtr) {
				pefree(p->pData, ht->persistent);
			}
			pefree(p, ht->persistent);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			ht->nNumOfElements--;
			return SUCCESS;
		}
		p = p->pNext;
	}
	return FAILURE;
}


ZEND_API void zend_hash_destroy(HashTable *ht)
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
		if (!q->pDataPtr && q->pData) {
			pefree(q->pData, ht->persistent);
		}
		pefree(q, ht->persistent);
	}
	pefree(ht->arBuckets, ht->persistent);

	SET_INCONSISTENT(HT_DESTROYED);
}


ZEND_API void zend_hash_clean(HashTable *ht)
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
		if (!q->pDataPtr && q->pData) {
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

/* This function is used by the various apply() functions.
 * It deletes the passed bucket, and returns the address of the
 * next bucket.  The hash *may* be altered during that time, the
 * returned value will still be valid.
 */
static Bucket *zend_hash_apply_deleter(HashTable *ht, Bucket *p)
{
	Bucket *retval;

	HANDLE_BLOCK_INTERRUPTIONS();

	if (ht->pDestructor) {
		ht->pDestructor(p->pData);
	}
	if (!p->pDataPtr) {
		pefree(p->pData, ht->persistent);
	}
	retval = p->pListNext;

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
	pefree(p, ht->persistent);
	HANDLE_UNBLOCK_INTERRUPTIONS();
	ht->nNumOfElements--;

	return retval;
}


ZEND_API void zend_hash_graceful_destroy(HashTable *ht)
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

ZEND_API void zend_hash_graceful_reverse_destroy(HashTable *ht)
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

/* This is used to selectively delete certain entries from a hashtable.
 * destruct() receives the data and decides if the entry should be deleted 
 * or not
 */


ZEND_API void zend_hash_apply(HashTable *ht, apply_func_t apply_func TSRMLS_DC)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	p = ht->pListHead;
	while (p != NULL) {
		if (apply_func(p->pData TSRMLS_CC)) {
			p = zend_hash_apply_deleter(ht, p);
		} else {
			p = p->pListNext;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_apply_with_argument(HashTable *ht, apply_func_arg_t apply_func, void *argument TSRMLS_DC)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);
	p = ht->pListHead;
	while (p != NULL) {
		if (apply_func(p->pData, argument TSRMLS_CC)) {
			p = zend_hash_apply_deleter(ht, p);
		} else {
			p = p->pListNext;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_apply_with_arguments(HashTable *ht, apply_func_args_t destruct, int num_args, ...)
{
	Bucket *p;
	va_list args;
	zend_hash_key hash_key;

	IS_CONSISTENT(ht);

	HASH_PROTECT_RECURSION(ht);

	p = ht->pListHead;
	while (p != NULL) {
		va_start(args, num_args);
		hash_key.arKey = p->arKey;
		hash_key.nKeyLength = p->nKeyLength;
		hash_key.h = p->h;
		if (destruct(p->pData, num_args, args, &hash_key)) {
			p = zend_hash_apply_deleter(ht, p);
		} else {
			p = p->pListNext;
		}
		va_end(args);
	}

	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_reverse_apply(HashTable *ht, apply_func_t apply_func TSRMLS_DC)
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
			if (q->nKeyLength>0) {
				zend_hash_del(ht, q->arKey, q->nKeyLength);
			} else {
				zend_hash_index_del(ht, q->h);
			}
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}


ZEND_API void zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size)
{
	Bucket *p;
	void *new_entry;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

	p = source->pListHead;
	while (p) {
		if (p->nKeyLength) {
			zend_hash_update(target, p->arKey, p->nKeyLength, p->pData, size, &new_entry);
		} else {
			zend_hash_index_update(target, p->h, p->pData, size, &new_entry);
		}
        if (pCopyConstructor) {
            pCopyConstructor(new_entry);
        }
		p = p->pListNext;
	}
	target->pInternalPointer = target->pListHead;
}


ZEND_API void zend_hash_merge(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size, int overwrite)
{
	Bucket *p;
	void *t;
	int mode = (overwrite?HASH_UPDATE:HASH_ADD);

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

    p = source->pListHead;
	while (p) {
		if (p->nKeyLength>0) {
			if (zend_hash_add_or_update(target, p->arKey, p->nKeyLength, p->pData, size, &t, mode)==SUCCESS && pCopyConstructor) {
				pCopyConstructor(t);
			}
		} else {
			if ((mode==HASH_UPDATE || !zend_hash_index_exists(target, p->h)) && zend_hash_index_update(target, p->h, p->pData, size, &t)==SUCCESS && pCopyConstructor) {
				pCopyConstructor(t);
			}
		}
		p = p->pListNext;
	}
	target->pInternalPointer = target->pListHead;
}


static int zend_hash_replace_checker_wrapper(HashTable *target, void *source_data, Bucket *p, void *pParam, merge_checker_func_t merge_checker_func)
{
	zend_hash_key hash_key;

	hash_key.arKey = p->arKey;
	hash_key.nKeyLength = p->nKeyLength;
	hash_key.h = p->h;
	return merge_checker_func(target, source_data, &hash_key, pParam);
}


ZEND_API void zend_hash_merge_ex(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, uint size, merge_checker_func_t pMergeSource, void *pParam)
{
	Bucket *p;
	void *t;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

    p = source->pListHead;
	while (p) {
		if (zend_hash_replace_checker_wrapper(target, p->pData, p, pParam, pMergeSource)) {
			if (zend_hash_quick_update(target, p->arKey, p->nKeyLength, p->h, p->pData, size, &t)==SUCCESS && pCopyConstructor) {
				pCopyConstructor(t);
			}
		}
		p = p->pListNext;
	}
	target->pInternalPointer = target->pListHead;
}


ZEND_API ulong zend_get_hash_value(char *arKey, uint nKeyLength)
{
	return zend_inline_hash_func(arKey, nKeyLength);
}


/* Returns SUCCESS if found and FAILURE if not. The pointer to the
 * data is returned in pData. The reason is that there's no reason
 * someone using the hash table might not want to have NULL data
 */
ZEND_API int zend_hash_find(HashTable *ht, char *arKey, uint nKeyLength, void **pData)
{
	ulong h;
	uint nIndex;
	Bucket *p;

	IS_CONSISTENT(ht);

	HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_find(ht, idx, pData));

	h = zend_inline_hash_func(arKey, nKeyLength);
	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				*pData = p->pData;
				return SUCCESS;
			}
		}
		p = p->pNext;
	}
	return FAILURE;
}


ZEND_API int zend_hash_quick_find(HashTable *ht, char *arKey, uint nKeyLength, ulong h, void **pData)
{
	uint nIndex;
	Bucket *p;

	if (nKeyLength==0) {
		return zend_hash_index_find(ht, h, pData);
	}

	IS_CONSISTENT(ht);

	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				*pData = p->pData;
				return SUCCESS;
			}
		}
		p = p->pNext;
	}
	return FAILURE;
}


ZEND_API int zend_hash_exists(HashTable *ht, char *arKey, uint nKeyLength)
{
	ulong h;
	uint nIndex;
	Bucket *p;

	IS_CONSISTENT(ht);

	HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_exists(ht, idx));

	h = zend_inline_hash_func(arKey, nKeyLength);
	nIndex = h & ht->nTableMask;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				return 1;
			}
		}
		p = p->pNext;
	}
	return 0;
}


ZEND_API int zend_hash_index_find(HashTable *ht, ulong h, void **pData)
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


ZEND_API int zend_hash_index_exists(HashTable *ht, ulong h)
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


ZEND_API int zend_hash_num_elements(HashTable *ht)
{
	IS_CONSISTENT(ht);

	return ht->nNumOfElements;
}


ZEND_API void zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos)
{
	IS_CONSISTENT(ht);

	if (pos)
		*pos = ht->pListHead;
	else
		ht->pInternalPointer = ht->pListHead;
}


/* This function will be extremely optimized by remembering 
 * the end of the list
 */
ZEND_API void zend_hash_internal_pointer_end_ex(HashTable *ht, HashPosition *pos)
{
	IS_CONSISTENT(ht);

	if (pos)
		*pos = ht->pListTail;
	else
		ht->pInternalPointer = ht->pListTail;
}


ZEND_API int zend_hash_move_forward_ex(HashTable *ht, HashPosition *pos)
{
	HashPosition *current = pos ? pos : &ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (*current) {
		*current = (*current)->pListNext;
		return SUCCESS;
	} else
		return FAILURE;
}

ZEND_API int zend_hash_move_backwards_ex(HashTable *ht, HashPosition *pos)
{
	HashPosition *current = pos ? pos : &ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (*current) {
		*current = (*current)->pListLast;
		return SUCCESS;
	} else
		return FAILURE;
}


/* This function should be made binary safe  */
ZEND_API int zend_hash_get_current_key_ex(HashTable *ht, char **str_index, uint *str_length, ulong *num_index, zend_bool duplicate, HashPosition *pos)
{
	Bucket *p;
   
	p = pos ? (*pos) : ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (p) {
		if (p->nKeyLength) {
			if (duplicate) {
				*str_index = estrndup(p->arKey, p->nKeyLength);
			} else {
				*str_index = p->arKey;
			}
			if (str_length) {
				*str_length = p->nKeyLength;
			}
			return HASH_KEY_IS_STRING;
		} else {
			*num_index = p->h;
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTANT;
}


ZEND_API int zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos)
{
	Bucket *p;
   
	p = pos ? (*pos) : ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (p) {
		if (p->nKeyLength) {
			return HASH_KEY_IS_STRING;
		} else {
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTANT;
}


ZEND_API int zend_hash_get_current_data_ex(HashTable *ht, void **pData, HashPosition *pos)
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


ZEND_API int zend_hash_sort(HashTable *ht, sort_func_t sort_func,
							compare_func_t compar, int renumber TSRMLS_DC)
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

	for (j = 0; j < i; j++) {
		if (ht->pListTail) {
			ht->pListTail->pListNext = arTmp[j];
		}
		arTmp[j]->pListLast = ht->pListTail;
		arTmp[j]->pListNext = NULL;
		ht->pListTail = arTmp[j];
	}
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


ZEND_API int zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_func_t compar, zend_bool ordered TSRMLS_DC)
{
	Bucket *p1, *p2;
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
				result = p1->nKeyLength - p2->nKeyLength;
				if (result!=0) {
					HASH_UNPROTECT_RECURSION(ht1); 
					HASH_UNPROTECT_RECURSION(ht2); 
					return result;
				}
				result = memcmp(p1->arKey, p2->arKey, p1->nKeyLength);
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
			} else { /* string index */
				if (zend_hash_find(ht2, p1->arKey, p1->nKeyLength, &pData2)==FAILURE) {
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


ZEND_API int zend_hash_minmax(HashTable *ht, compare_func_t compar, int flag, void **pData TSRMLS_DC)
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

ZEND_API ulong zend_hash_next_free_element(HashTable *ht)
{
	IS_CONSISTENT(ht);

	return ht->nNextFreeElement;

}


#if ZEND_DEBUG
void zend_hash_display_pListTail(HashTable *ht)
{
	Bucket *p;

	p = ht->pListTail;
	while (p != NULL) {
		zend_output_debug_string(0, "pListTail has key %s\n", p->arKey);
		p = p->pListLast;
	}
}

void zend_hash_display(HashTable *ht)
{
	Bucket *p;
	uint i;

	for (i = 0; i < ht->nTableSize; i++) {
		p = ht->arBuckets[i];
		while (p != NULL) {
			zend_output_debug_string(0, "%s <==> 0x%X\n", p->arKey, p->h);
			p = p->pNext;
		}
	}

	p = ht->pListTail;
	while (p != NULL) {
		zend_output_debug_string(0, "%s <==> 0x%X\n", p->arKey, p->h);
		p = p->pListLast;
	}
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
