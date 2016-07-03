/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id: $ */

#include "zend.h"
#include "zend_globals.h"

#ifndef ZEND_DEBUG_INTERNED_STRINGS
# define ZEND_DEBUG_INTERNED_STRINGS 0
#endif

#if ZEND_DEBUG_INTERNED_STRINGS
# include <sys/mman.h>
#endif

ZEND_API const char *(*zend_new_interned_string)(const char *str, int len, int free_src TSRMLS_DC);
ZEND_API void (*zend_interned_strings_snapshot)(TSRMLS_D);
ZEND_API void (*zend_interned_strings_restore)(TSRMLS_D);

static const char *zend_new_interned_string_int(const char *str, int len, int free_src TSRMLS_DC);
static void zend_interned_strings_snapshot_int(TSRMLS_D);
static void zend_interned_strings_restore_int(TSRMLS_D);

void zend_interned_strings_init(TSRMLS_D)
{
#ifndef ZTS
	size_t size = 1024 * 1024;

#if ZEND_DEBUG_INTERNED_STRINGS
	CG(interned_strings_start) = valloc(size);
#else
	CG(interned_strings_start) = malloc(size);
#endif

	CG(interned_strings_top) = CG(interned_strings_start);
	CG(interned_strings_snapshot_top) = CG(interned_strings_start);
	CG(interned_strings_end) = CG(interned_strings_start) + size;

	zend_hash_init(&CG(interned_strings), 0, NULL, NULL, 1);
	
	CG(interned_strings).nTableMask = CG(interned_strings).nTableSize - 1;
	CG(interned_strings).arBuckets = (Bucket **) pecalloc(CG(interned_strings).nTableSize, sizeof(Bucket *), CG(interned_strings).persistent);

#if ZEND_DEBUG_INTERNED_STRINGS
	mprotect(CG(interned_strings_start), CG(interned_strings_end) - CG(interned_strings_start), PROT_READ);
#endif

    /* interned empty string */
	CG(interned_empty_string) = zend_new_interned_string_int("", sizeof(""), 0 TSRMLS_CC);
#endif

	zend_new_interned_string = zend_new_interned_string_int;
	zend_interned_strings_snapshot = zend_interned_strings_snapshot_int;
	zend_interned_strings_restore = zend_interned_strings_restore_int;
}

void zend_interned_strings_dtor(TSRMLS_D)
{
#ifndef ZTS
#if ZEND_DEBUG_INTERNED_STRINGS
	mprotect(CG(interned_strings_start), CG(interned_strings_end) - CG(interned_strings_start), PROT_WRITE | PROT_READ);
#endif
	free(CG(interned_strings).arBuckets);
	free(CG(interned_strings_start));
#endif
}

static const char *zend_new_interned_string_int(const char *arKey, int nKeyLength, int free_src TSRMLS_DC)
{
#ifndef ZTS
	ulong h;
	uint nIndex;
	Bucket *p;

	if (IS_INTERNED(arKey)) {
		return arKey;
	}

	h = zend_inline_hash_func(arKey, nKeyLength);
	nIndex = h & CG(interned_strings).nTableMask;
	p = CG(interned_strings).arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				if (free_src) {
					efree((void *)arKey);
				}
				return p->arKey;
			}
		}
		p = p->pNext;
	}
	
	if (CG(interned_strings_top) + ZEND_MM_ALIGNED_SIZE(sizeof(Bucket) + nKeyLength) >=
	    CG(interned_strings_end)) {
	    /* no memory */
		return arKey;
	}

	p = (Bucket *) CG(interned_strings_top);
	CG(interned_strings_top) += ZEND_MM_ALIGNED_SIZE(sizeof(Bucket) + nKeyLength);

#if ZEND_DEBUG_INTERNED_STRINGS
	mprotect(CG(interned_strings_start), CG(interned_strings_end) - CG(interned_strings_start), PROT_READ | PROT_WRITE);
#endif
	
	p->arKey = (char*)(p+1);
	memcpy((char*)p->arKey, arKey, nKeyLength);
	if (free_src) {
		efree((void *)arKey);
	}
	p->nKeyLength = nKeyLength;
	p->h = h;
	p->pData = &p->pDataPtr;
	p->pDataPtr = p;
	
	p->pNext = CG(interned_strings).arBuckets[nIndex];
	p->pLast = NULL;
	if (p->pNext) {
		p->pNext->pLast = p;
	}

	HANDLE_BLOCK_INTERRUPTIONS();
	
	p->pListLast = CG(interned_strings).pListTail;
	CG(interned_strings).pListTail = p;
	p->pListNext = NULL;
	if (p->pListLast != NULL) {
		p->pListLast->pListNext = p;
	}
	if (!CG(interned_strings).pListHead) {
		CG(interned_strings).pListHead = p;
	}

	CG(interned_strings).arBuckets[nIndex] = p;

	HANDLE_UNBLOCK_INTERRUPTIONS();

	CG(interned_strings).nNumOfElements++;

	if (CG(interned_strings).nNumOfElements > CG(interned_strings).nTableSize) {
		if ((CG(interned_strings).nTableSize << 1) > 0) {	/* Let's double the table size */
			Bucket **t = (Bucket **) perealloc_recoverable(CG(interned_strings).arBuckets, (CG(interned_strings).nTableSize << 1) * sizeof(Bucket *), CG(interned_strings).persistent);

			if (t) {
				HANDLE_BLOCK_INTERRUPTIONS();
				CG(interned_strings).arBuckets = t;
				CG(interned_strings).nTableSize = (CG(interned_strings).nTableSize << 1);
				CG(interned_strings).nTableMask = CG(interned_strings).nTableSize - 1;
				zend_hash_rehash(&CG(interned_strings));
				HANDLE_UNBLOCK_INTERRUPTIONS();
			}
		}
	}

#if ZEND_DEBUG_INTERNED_STRINGS
	mprotect(CG(interned_strings_start), CG(interned_strings_end) - CG(interned_strings_start), PROT_READ);
#endif

	return p->arKey;
#else
	return arKey;
#endif
}

static void zend_interned_strings_snapshot_int(TSRMLS_D)
{
	CG(interned_strings_snapshot_top) = CG(interned_strings_top);
}

static void zend_interned_strings_restore_int(TSRMLS_D)
{
#ifndef ZTS
	Bucket *p;
	int i;
#endif

	CG(interned_strings_top) = CG(interned_strings_snapshot_top);

#ifndef ZTS
#if ZEND_DEBUG_INTERNED_STRINGS
	mprotect(CG(interned_strings_start), CG(interned_strings_end) - CG(interned_strings_start), PROT_WRITE | PROT_READ);
#endif

	for (i = 0; i < CG(interned_strings).nTableSize; i++) {
		p = CG(interned_strings).arBuckets[i];
		while (p && p->arKey > CG(interned_strings_top)) {
			CG(interned_strings).nNumOfElements--;
			if (p->pListLast != NULL) {
				p->pListLast->pListNext = p->pListNext;
			} else { 
				CG(interned_strings).pListHead = p->pListNext;
			}
			if (p->pListNext != NULL) {
				p->pListNext->pListLast = p->pListLast;
			} else {
				CG(interned_strings).pListTail = p->pListLast;
			}
			p = p->pNext;
		}
		if (p) {
			p->pLast = NULL;
		}
		CG(interned_strings).arBuckets[i] = p;
	}

#if ZEND_DEBUG_INTERNED_STRINGS
	mprotect(CG(interned_strings_start), CG(interned_strings_end) - CG(interned_strings_start), PROT_READ);
#endif
#endif
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
