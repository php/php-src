/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include "zend.h"

#include <stdio.h>

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#define HANDLE_NUMERIC(key,length,func) { \
	register char *tmp=key; \
\
	if ((*tmp>='0' && *tmp<='9')) do { /* possibly a numeric index */ \
		char *end=tmp+length-1; \
		ulong idx; \
		\
		if (*tmp++=='0' && length>2) { /* don't accept numbers with leading zeros */ \
			break; \
		} \
		while (tmp<end) { \
			if (!(*tmp>='0' && *tmp<='9')) { \
				break; \
			} \
			tmp++; \
		} \
		if (tmp==end && *tmp=='\0') { /* a numeric index */ \
			idx = strtol(key,NULL,10); \
			if (idx!=LONG_MAX) { \
				return func; \
			} \
		} \
	} while(0); \
}

#if ZEND_DEBUG
#define HT_IS_DESTROYING	1
#define HT_DESTROYED		2
#define HT_CLEANING		3
#define HT_OK				0

static void _zend_is_inconsistent(HashTable *ht,char *file, int line)
{	
    switch (ht->inconsistent) {
	case HT_IS_DESTROYING:
        zend_error(E_CORE_ERROR, "ht=%08x is destroying in %s:%d",ht,file,line);
		break;
	case HT_DESTROYED:
        zend_error(E_CORE_ERROR, "ht=%08x is already destroyed in %s:%d",ht,file,line);
		break;
	case HT_CLEANING:
        zend_error(E_CORE_ERROR, "ht=%08x is cleaning %s:%d",ht,file,line);
		break;
    }
}
#define IS_CONSISTENT(a) _zend_is_inconsistent(a,__FILE__,__LINE__);
#define SET_INCONSISTENT(n) ht->inconsistent = n;
#else
#define IS_CONSISTENT(a)
#define SET_INCONSISTENT(n)
#endif

/* Generated on an Octa-ALPHA 300MHz CPU & 2.5GB RAM monster */
static uint PrimeNumbers[] =
{5, 11, 19, 53, 107, 223, 463, 983, 1979, 3907, 7963, 16229, 32531, 65407, 130987, 262237, 524521, 1048793, 2097397, 4194103, 8388857, 16777447, 33554201, 67108961, 134217487, 268435697, 536870683, 1073741621, 2147483399};

static int if_full_do_resize(HashTable *ht);

static uint nNumPrimeNumbers = sizeof(PrimeNumbers) / sizeof(ulong);

ZEND_API ulong hashpjw(char *arKey, uint nKeyLength)
{
	ulong h = 0, g;
	char *arEnd=arKey+nKeyLength;

	while (arKey < arEnd) {
		h = (h << 4) + *arKey++;
		if ((g = (h & 0xF0000000))) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h;
}


ZEND_API int zend_hash_init(HashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, int persistent)
{
	uint i;

	SET_INCONSISTENT(HT_OK);
	
	for (i = 0; i < nNumPrimeNumbers; i++) {
		if (nSize <= PrimeNumbers[i]) {
			nSize = PrimeNumbers[i];
			ht->nHashSizeIndex = i;
			break;
		}
	}
	if (i == nNumPrimeNumbers) {	/* This shouldn't really happen unless the ask for a ridiculous size */
		nSize = PrimeNumbers[i - 1];
		ht->nHashSizeIndex = i - 1;
	}
	
	/* Uses ecalloc() so that Bucket* == NULL */
	ht->arBuckets = (Bucket **) pecalloc(nSize, sizeof(Bucket *), persistent);
	
	if (!ht->arBuckets) {
		return FAILURE;
	}
	if (pHashFunction == NULL) {
		ht->pHashFunction = hashpjw;
	} else {
		ht->pHashFunction = pHashFunction;
	}
	ht->pDestructor = pDestructor;
	ht->nTableSize = nSize;
	ht->pListHead = NULL;
	ht->pListTail = NULL;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->pInternalPointer = NULL;
	ht->persistent = persistent;
	return SUCCESS;
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

	HANDLE_NUMERIC(arKey,nKeyLength,zend_hash_index_update_or_next_insert(ht,idx,pData,nDataSize,pDest,flag));
	
	h = ht->pHashFunction(arKey, nKeyLength);
	nIndex = h % ht->nTableSize;
	
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
				if (flag & HASH_ADD_PTR) {
					if (!p->pDataPtr) {
						efree(p->pData);
					}
					p->pDataPtr = pData;
					p->pData = &p->pDataPtr;
				} else {
					if (p->pDataPtr) {
						p->pData = (void *) emalloc(nDataSize);
						p->pDataPtr=NULL;
					}
					memcpy(p->pData, pData, nDataSize);
				}
				if (pDest) {
					*pDest = p->pData;
				}
				HANDLE_UNBLOCK_INTERRUPTIONS();
				return SUCCESS;
			}
		}
		p = p->pNext;
	}
	
	p = (Bucket *) pemalloc(sizeof(Bucket)-1+nKeyLength,ht->persistent);
	if (!p) {
		return FAILURE;
	}
	memcpy(p->arKey, arKey, nKeyLength);
	p->nKeyLength = nKeyLength;
	if (flag & HASH_ADD_PTR) {
		p->pDataPtr = pData;
		p->pData = &p->pDataPtr;
	} else {
		p->pData = (void *) pemalloc(nDataSize,ht->persistent);
		if (!p->pData) {
			pefree(p,ht->persistent);
			pefree(p->arKey,ht->persistent);
			return FAILURE;
		}
		memcpy(p->pData, pData, nDataSize);
		p->pDataPtr=NULL;
	}
	p->h = h;
	p->pNext = ht->arBuckets[nIndex];
	if (pDest) {
		*pDest = p->pData;
	}

	HANDLE_BLOCK_INTERRUPTIONS();
	if (ht->pInternalPointer == NULL) {
		ht->pInternalPointer = p;
	}
	ht->arBuckets[nIndex] = p;

	/* Setup the double linked list */
	p->pListLast = ht->pListTail;
	ht->pListTail = p;
	p->pListNext = NULL;
	if (p->pListLast != NULL) {
		p->pListLast->pListNext = p;
	}
	if (!ht->pListHead) {
		ht->pListHead = p;
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
	ht->nNumOfElements++;
	if_full_do_resize(ht);		/* If the Hash table is full, resize it */
	return SUCCESS;
}

ZEND_API int zend_hash_quick_add_or_update(HashTable *ht, char *arKey, uint nKeyLength, ulong h, void *pData, uint nDataSize, void **pDest, int flag)
{
	uint nIndex;
	Bucket *p;

	IS_CONSISTENT(ht);

	if (nKeyLength <= 0) {
#if ZEND_DEBUG
		ZEND_PUTS("zend_hash_update: Can't put in empty key\n");
#endif
		return FAILURE;
	}

	nIndex = h % ht->nTableSize;
	
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
				if (flag & HASH_ADD_PTR) {
					if (!p->pDataPtr) {
						efree(p->pData);
					}
					p->pDataPtr = pData;
					p->pData = &p->pDataPtr;
				} else {
					if (p->pDataPtr) {
						p->pData = (void *) emalloc(nDataSize);
						p->pDataPtr=NULL;
					}
					memcpy(p->pData, pData, nDataSize);
				}
				if (pDest) {
					*pDest = p->pData;
				}
				HANDLE_UNBLOCK_INTERRUPTIONS();
				return SUCCESS;
			}
		}
		p = p->pNext;
	}
	
	p = (Bucket *) pemalloc(sizeof(Bucket)-1+nKeyLength,ht->persistent);
	if (!p) {
		return FAILURE;
	}

	memcpy(p->arKey, arKey, nKeyLength);
	p->nKeyLength = nKeyLength;
	if (flag & HASH_ADD_PTR) {
		p->pDataPtr = pData;
		p->pData = &p->pDataPtr;
	} else {
		p->pData = (void *) pemalloc(nDataSize,ht->persistent);
		if (!p->pData) {
			pefree(p,ht->persistent);
			pefree(p->arKey,ht->persistent);
			return FAILURE;
		}
		
		memcpy(p->pData, pData, nDataSize);
		p->pDataPtr=NULL;
	}
	p->h = h;
	p->pNext = ht->arBuckets[nIndex];
	if (pDest) {
		*pDest = p->pData;
	}

	HANDLE_BLOCK_INTERRUPTIONS();
	if (ht->pInternalPointer == NULL) {
		ht->pInternalPointer = p;
	}
	ht->arBuckets[nIndex] = p;

	/* Setup the double linked list */
	p->pListLast = ht->pListTail;
	ht->pListTail = p;
	p->pListNext = NULL;
	if (p->pListLast != NULL) {
		p->pListLast->pListNext = p;
	}
	if (!ht->pListHead) {
		ht->pListHead = p;
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
	ht->nNumOfElements++;
	if_full_do_resize(ht);		/* If the Hash table is full, resize it */
	return SUCCESS;
}


ZEND_API int zend_hash_index_update_or_next_insert(HashTable *ht, ulong h, void *pData, uint nDataSize, void **pDest, int flag)
{
	uint nIndex;
	Bucket *p;

	IS_CONSISTENT(ht);

	if (flag & HASH_NEXT_INSERT) {
		h = ht->nNextFreeElement;
	}
	nIndex = h % ht->nTableSize;

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
			if (flag & HASH_ADD_PTR) {
				if (!p->pDataPtr) {
					efree(p->pData);
				}
				p->pDataPtr = pData;
				p->pData = &p->pDataPtr;
			} else {
				if (p->pDataPtr) {
					p->pData = (void *) emalloc(nDataSize);
					p->pDataPtr=NULL;
				}
				memcpy(p->pData, pData, nDataSize);
			}
			HANDLE_UNBLOCK_INTERRUPTIONS();
			if (h >= ht->nNextFreeElement) {
				ht->nNextFreeElement = h + 1;
			}
			if (pDest) {
				*pDest = p->pData;
			}
			return SUCCESS;
		}
		p = p->pNext;
	}
	p = (Bucket *) pemalloc(sizeof(Bucket)-1,ht->persistent);
	if (!p) {
		return FAILURE;
	}
	p->nKeyLength = 0;			/*  Numeric indices are marked by making the nKeyLength == 0 */
	p->h = h;
	
	if (flag & HASH_ADD_PTR) {
		p->pDataPtr = pData;
		p->pData = &p->pDataPtr;
	} else {
		p->pData = (void *) pemalloc(nDataSize,ht->persistent);
		if (!p->pData) {
			pefree(p,ht->persistent);
			return FAILURE;
		}
		memcpy(p->pData, pData, nDataSize);
		p->pDataPtr=NULL;
	}
	if (pDest) {
		*pDest = p->pData;
	}

	p->pNext = ht->arBuckets[nIndex];
	HANDLE_BLOCK_INTERRUPTIONS();
	if (ht->pInternalPointer == NULL) {
		ht->pInternalPointer = p;
	}
	ht->arBuckets[nIndex] = p;

	/* Setup the double linked list */
	p->pListLast = ht->pListTail;
	ht->pListTail = p;
	p->pListNext = NULL;
	if (p->pListLast != NULL) {
		p->pListLast->pListNext = p;
	}
	if (!ht->pListHead) {
		ht->pListHead = p;
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
	if (h >= ht->nNextFreeElement) {
		ht->nNextFreeElement = h + 1;
	}
	ht->nNumOfElements++;
	if_full_do_resize(ht);
	return SUCCESS;
}


ZEND_API int zend_hash_pointer_update(HashTable *ht, char *arKey, uint nKeyLength, void *pData)
{
	zend_error(E_ERROR, "zend_hash_pointer_*() functions are no longer supported");
	return FAILURE;
}


ZEND_API int zend_hash_pointer_index_update_or_next_insert(HashTable *ht, ulong h, void *pData, int flag)
{
	zend_error(E_ERROR, "zend_hash_pointer_*() functions are no longer supported");
	return FAILURE;
}


static int if_full_do_resize(HashTable *ht)
{
	Bucket **t;

	IS_CONSISTENT(ht);

	if ((ht->nNumOfElements > ht->nTableSize) && (ht->nHashSizeIndex < nNumPrimeNumbers - 1)) {		/* Let's double the table
																									   size */
		t = (Bucket **) perealloc_recoverable(ht->arBuckets, PrimeNumbers[ht->nHashSizeIndex + 1] * sizeof(Bucket *),ht->persistent);
		if (t) {
			HANDLE_BLOCK_INTERRUPTIONS();
			ht->arBuckets = t;
			ht->nTableSize = PrimeNumbers[ht->nHashSizeIndex + 1];
			ht->nHashSizeIndex++;
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

	memset(ht->arBuckets, 0, PrimeNumbers[ht->nHashSizeIndex] * sizeof(Bucket *));
	p = ht->pListHead;
	while (p != NULL) {
		nIndex = p->h % ht->nTableSize;
		p->pNext = ht->arBuckets[nIndex];
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
		HANDLE_NUMERIC(arKey,nKeyLength,zend_hash_del_key_or_index(ht,arKey,nKeyLength,idx,HASH_DEL_INDEX));
		h = ht->pHashFunction(arKey, nKeyLength);
	}
	nIndex = h % ht->nTableSize;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && ((p->nKeyLength == 0) || /* Numeric index */
			((p->nKeyLength == nKeyLength) && (!memcmp(p->arKey, arKey, nKeyLength))))) {
			HANDLE_BLOCK_INTERRUPTIONS();
			if (ht->arBuckets[nIndex] == p) {
				ht->arBuckets[nIndex] = p->pNext;
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
			if (ht->pDestructor) {
				ht->pDestructor(p->pData);
			}
			if (!p->pDataPtr) {
				pefree(p->pData,ht->persistent);
			}
			if (ht->pInternalPointer == p) {
				ht->pInternalPointer = p->pListNext;
			}
			pefree(p,ht->persistent);
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
			pefree(q->pData,ht->persistent);
		}
		pefree(q,ht->persistent);
	}
	pefree(ht->arBuckets,ht->persistent);

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
			pefree(q->pData,ht->persistent);
		}
		pefree(q,ht->persistent);
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
	uint nIndex;

	HANDLE_BLOCK_INTERRUPTIONS();

	nIndex = p->h % ht->nTableSize;

	if (ht->pDestructor) {
		ht->pDestructor(p->pData);
	}
	if (!p->pDataPtr) {
		pefree(p->pData, ht->persistent);
	}
	retval = p->pListNext;

	if (ht->arBuckets[nIndex] == p) {
		ht->arBuckets[nIndex] = p->pNext;
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
	pefree(p,ht->persistent);
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
	pefree(ht->arBuckets,ht->persistent);

	SET_INCONSISTENT(HT_DESTROYED);
}

/* This is used to selectively delete certain entries from a hashtable.
 * destruct() receives the data and decides if the entry should be deleted 
 * or not
 */


ZEND_API void zend_hash_apply(HashTable *ht,int (*destruct) (void *))
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = ht->pListHead;
	while (p != NULL) {
		if (destruct(p->pData)) {
			p = zend_hash_apply_deleter(ht, p);
		} else {
			p = p->pListNext;
		}
	}
}


ZEND_API void zend_hash_apply_with_argument(HashTable *ht,int (*destruct) (void *, void *), void *argument)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = ht->pListHead;
	while (p != NULL) {
		if (destruct(p->pData, argument)) {
			p = zend_hash_apply_deleter(ht, p);
		} else {
			p = p->pListNext;
		}
	}
}


ZEND_API void zend_hash_apply_with_arguments(HashTable *ht,int (*destruct)(void *, int, va_list, zend_hash_key *), int num_args, ...)
{
	Bucket *p;
	va_list args;
	zend_hash_key hash_key;

	IS_CONSISTENT(ht);

	va_start(args, num_args);

	p = ht->pListHead;
	while (p != NULL) {
		hash_key.arKey = p->arKey;
		hash_key.nKeyLength = p->nKeyLength;
		hash_key.h = p->h;
		if (destruct(p->pData, num_args, args, &hash_key)) {
			p = zend_hash_apply_deleter(ht, p);
		} else {
			p = p->pListNext;
		}
	}

	va_end(args);
}



ZEND_API void zend_hash_copy(HashTable *target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size)
{
	Bucket *p;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

    p = source->pListHead;
	while (p) {
		memcpy(tmp, p->pData, size);
		if (pCopyConstructor) {
			pCopyConstructor(tmp);
		}
		if (p->nKeyLength) {
			zend_hash_update(target, p->arKey, p->nKeyLength, tmp, size, NULL);
		} else {
			zend_hash_index_update(target, p->h, tmp, size, NULL);
		}
		p = p->pListNext;
	}
	target->pInternalPointer = target->pListHead;
}


ZEND_API void zend_hash_merge(HashTable *target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size, int overwrite)
{
	Bucket *p;
	void *t;
	int mode = (overwrite?HASH_UPDATE:HASH_ADD);

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);

    p = source->pListHead;
	while (p) {
		memcpy(tmp, p->pData, size);
		if (p->nKeyLength>0) {
			if (zend_hash_add_or_update(target, p->arKey, p->nKeyLength, tmp, size, &t, mode)==SUCCESS && pCopyConstructor) {
				pCopyConstructor(t);
			}
		} else {
			if ((mode==HASH_UPDATE || !zend_hash_index_exists(target, p->h)) && zend_hash_index_update(target, p->h, tmp, size, &t)==SUCCESS && pCopyConstructor) {
				pCopyConstructor(t);
			}
		}
		p = p->pListNext;
	}
	target->pInternalPointer = target->pListHead;
}


ZEND_API ulong zend_get_hash_value(HashTable *ht, char *arKey, uint nKeyLength)
{
	IS_CONSISTENT(ht);

	return ht->pHashFunction(arKey, nKeyLength);
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

	HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_find(ht,idx,pData));

	h = ht->pHashFunction(arKey, nKeyLength);
	nIndex = h % ht->nTableSize;

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

	IS_CONSISTENT(ht);

	nIndex = h % ht->nTableSize;

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

	HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_exists(ht,idx));

	h = ht->pHashFunction(arKey, nKeyLength);
	nIndex = h % ht->nTableSize;

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

	nIndex = h % ht->nTableSize;

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

	nIndex = h % ht->nTableSize;

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


ZEND_API void zend_hash_internal_pointer_reset(HashTable *ht)
{
	IS_CONSISTENT(ht);

	ht->pInternalPointer = ht->pListHead;
}


/* This function will be extremely optimized by remembering 
 * the end of the list
 */
ZEND_API void zend_hash_internal_pointer_end(HashTable *ht)
{
	IS_CONSISTENT(ht);

	ht->pInternalPointer = ht->pListTail;
}


ZEND_API void zend_hash_move_forward(HashTable *ht)
{
	IS_CONSISTENT(ht);

	if (ht->pInternalPointer) {
		ht->pInternalPointer = ht->pInternalPointer->pListNext;
	}
}

ZEND_API void zend_hash_move_backwards(HashTable *ht)
{
	IS_CONSISTENT(ht);

	if (ht->pInternalPointer) {
		ht->pInternalPointer = ht->pInternalPointer->pListLast;
	}
}


/* This function should be made binary safe  */
ZEND_API int zend_hash_get_current_key(HashTable *ht, char **str_index, ulong *num_index)
{
	Bucket *p = ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (p) {
		if (p->nKeyLength) {
			*str_index = (char *) pemalloc(p->nKeyLength,ht->persistent);
			memcpy(*str_index, p->arKey, p->nKeyLength);
			return HASH_KEY_IS_STRING;
		} else {
			*num_index = p->h;
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTANT;
}


ZEND_API int zend_hash_get_current_key_type(HashTable *ht)
{
	Bucket *p = ht->pInternalPointer;

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


ZEND_API int zend_hash_get_current_data(HashTable *ht, void **pData)
{
	Bucket *p = ht->pInternalPointer;

	IS_CONSISTENT(ht);

	if (p) {
		*pData = p->pData;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}


ZEND_API int zend_hash_sort(HashTable *ht, sort_func_t sort_func,
							compare_func_t compar, int renumber)
{
	Bucket **arTmp;
	Bucket *p;
	int i, j;

	IS_CONSISTENT(ht);

	if (ht->nNumOfElements <= 1) {	/* Doesn't require sorting */
		return SUCCESS;
	}
	arTmp = (Bucket **) pemalloc(ht->nNumOfElements * sizeof(Bucket *),ht->persistent);
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

	(*sort_func)((void *) arTmp, i, sizeof(Bucket *), compar);

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
	pefree(arTmp,ht->persistent);
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


ZEND_API int zend_hash_minmax(HashTable *ht, int (*compar) (const void *, const void *), int flag, void **pData)
{
	Bucket *p,*res;

	IS_CONSISTENT(ht);

	if (ht->nNumOfElements == 0 ) {
		*pData=NULL;
		return FAILURE;
	}

	res = p = ht->pListHead;
	while ((p = p->pListNext)) {
		if (flag) {
			if (compar(&res,&p) < 0) { /* max */
				res = p;
			}
		} else {
			if (compar(&res,&p) > 0) { /* min */
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
		zend_printf("pListTail has key %s\n", p->arKey);
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
			zend_printf("%s <==> 0x%X\n", p->arKey, p->h);
			p = p->pNext;
		}
	}

	p = ht->pListTail;
	while (p != NULL) {
		zend_printf("%s <==> 0x%X\n", p->arKey, p->h);
		p = p->pListLast;
	}
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
