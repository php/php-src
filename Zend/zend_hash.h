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

#ifndef _HASH_
#define _HASH_

#include <sys/types.h>

#define HASH_KEY_IS_STRING 1
#define HASH_KEY_IS_LONG 2
#define HASH_KEY_NON_EXISTANT 3

#define HASH_UPDATE 		(1<<0)
#define HASH_ADD			(1<<1)
#define HASH_NEXT_INSERT	(1<<2)
#define HASH_ADD_PTR		(1<<3)

#define HASH_DEL_KEY 0
#define HASH_DEL_INDEX 1

struct hashtable;

typedef struct bucket {
	ulong h;						/* Used for numeric indexing */
	uint nKeyLength;
	void *pData;
	void *pDataPtr;
	char bIsPointer;
	struct bucket *pListNext;
	struct bucket *pListLast;
	struct bucket *pNext;
	char arKey[1]; /* Must be last element */
} Bucket;

typedef struct hashtable {
	uint nTableSize;
	uint nHashSizeIndex;
	uint nNumOfElements;
	ulong nNextFreeElement;
	ulong(*pHashFunction) (char *arKey, uint nKeyLength);
	Bucket *pInternalPointer;	/* Used for element traversal */
	Bucket *pListHead;
	Bucket *pListTail;
	Bucket **arBuckets;
	int (*pDestructor) (void *pData);
	unsigned char persistent;
#if ZEND_DEBUG
	int inconsistent;
#endif
} HashTable;

typedef int  (*compare_func_t) (const void *, const void *);
typedef void (*sort_func_t) (void *, size_t, register size_t, compare_func_t);

BEGIN_EXTERN_C()

/* startup/shutdown */
ZEND_API int zend_hash_init(HashTable *ht, uint nSize, ulong(*pHashFunction) (char *arKey, uint nKeyLength), int (*pDestructor) (void *pData), int persistent);
ZEND_API void zend_hash_destroy(HashTable *ht);

ZEND_API void zend_hash_clean(HashTable *ht);

/* additions/updates/changes */
ZEND_API int zend_hash_add_or_update(HashTable *ht, char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest,int flag);
#define zend_hash_update(ht,arKey,nKeyLength,pData,nDataSize,pDest) \
		zend_hash_add_or_update(ht,arKey,nKeyLength,pData,nDataSize,pDest,HASH_UPDATE)
#define zend_hash_add(ht,arKey,nKeyLength,pData,nDataSize,pDest) \
		zend_hash_add_or_update(ht,arKey,nKeyLength,pData,nDataSize,pDest,HASH_ADD)
#define zend_hash_update_ptr(ht,arKey,nKeyLength,pData,nDataSize,pDest) \
		zend_hash_add_or_update(ht,arKey,nKeyLength,pData,0,pDest,(HASH_UPDATE|HASH_ADD_PTR))

ZEND_API int zend_hash_quick_add_or_update(HashTable *ht, char *arKey, uint nKeyLength, ulong h, void *pData, uint nDataSize, void **pDest,int flag);
#define zend_hash_quick_update(ht,arKey,nKeyLength,h,pData,nDataSize,pDest) \
		zend_hash_quick_add_or_update(ht,arKey,nKeyLength,h,pData,nDataSize,pDest,HASH_UPDATE)
#define zend_hash_quick_add(ht,arKey,nKeyLength,h,pData,nDataSize,pDest) \
		zend_hash_quick_add_or_update(ht,arKey,nKeyLength,h,pData,nDataSize,pDest,HASH_ADD)
#define zend_hash_quick_update_ptr(ht,arKey,nKeyLength,h,pData,nDataSize,pDest) \
		zend_hash_quick_add_or_update(ht,arKey,nKeyLength,h,pData,0,pDest,HASH_UPDATE|HASH_ADD_PTR)

ZEND_API int zend_hash_index_update_or_next_insert(HashTable *ht, ulong h, void *pData, uint nDataSize, void **pDest, int flag);
#define zend_hash_index_update(ht,h,pData,nDataSize,pDest) \
		zend_hash_index_update_or_next_insert(ht,h,pData,nDataSize,pDest,HASH_UPDATE)
#define zend_hash_next_index_insert(ht,pData,nDataSize,pDest) \
		zend_hash_index_update_or_next_insert(ht,0,pData,nDataSize,pDest,HASH_NEXT_INSERT)
#define zend_hash_next_index_insert_ptr(ht,pData,nDataSize,pDest) \
		zend_hash_index_update_or_next_insert(ht,0,pData,nDataSize,pDest,HASH_NEXT_INSERT|HASH_ADD_PTR)

ZEND_API int zend_hash_pointer_update(HashTable *ht, char *arKey, uint nKeyLength, void *pData);

typedef struct _zend_hash_key {
	char *arKey;
	uint nKeyLength;
	ulong h;
} zend_hash_key;


#define ZEND_STD_HASH_APPLIER		\
	int (*)(void *element, int num_args, va_list args, zend_hash_key *hash_key)

ZEND_API int zend_hash_pointer_index_update_or_next_insert(HashTable *ht, ulong h, void *pData, int flag);
#define zend_hash_pointer_index_update(ht,h,pData) \
		zend_hash_pointer_index_update_or_next_insert(ht,h,pData,HASH_UPDATE)
#define zend_hash_next_index_pointer_insert(ht,pData) \
        zend_hash_pointer_index_update_or_next_insert(ht,0,pData,HASH_NEXT_INSERT)
ZEND_API void zend_hash_graceful_destroy(HashTable *ht);
ZEND_API void zend_hash_apply(HashTable *ht,int (*destruct)(void *));
ZEND_API void zend_hash_apply_with_argument(HashTable *ht,int (*destruct)(void *, void *), void *);
ZEND_API void zend_hash_apply_with_arguments(HashTable *ht, ZEND_STD_HASH_APPLIER, int, ...);




/* Deletes */
ZEND_API int zend_hash_del_key_or_index(HashTable *ht, char *arKey, uint nKeyLength, ulong h, int flag);
#define zend_hash_del(ht,arKey,nKeyLength) \
		zend_hash_del_key_or_index(ht,arKey,nKeyLength,0,HASH_DEL_KEY)
#define zend_hash_index_del(ht,h) \
		zend_hash_del_key_or_index(ht,NULL,0,h,HASH_DEL_INDEX)

ZEND_API ulong zend_get_hash_value(HashTable *ht, char *arKey, uint nKeyLength);

/* Data retreival */
ZEND_API int zend_hash_find(HashTable *ht, char *arKey, uint nKeyLength, void **pData);
ZEND_API int zend_hash_quick_find(HashTable *ht, char *arKey, uint nKeyLength, ulong h, void **pData);
ZEND_API int zend_hash_index_find(HashTable *ht, ulong h, void **pData);

/* Misc */
ZEND_API int zend_hash_exists(HashTable *ht, char *arKey, uint nKeyLength);
ZEND_API int zend_hash_index_exists(HashTable *ht, ulong h);
ZEND_API int zend_hash_is_pointer(HashTable *ht, char *arKey, uint nKeyLength);
ZEND_API int zend_hash_index_is_pointer(HashTable *ht, ulong h);
ZEND_API ulong zend_hash_next_free_element(HashTable *ht);

/* traversing */
ZEND_API void zend_hash_move_forward(HashTable *ht);
ZEND_API void zend_hash_move_backwards(HashTable *ht);
ZEND_API int zend_hash_get_current_key(HashTable *ht, char **str_index, ulong *num_index);
ZEND_API int zend_hash_get_current_key_type(HashTable *ht);
ZEND_API int zend_hash_get_current_data(HashTable *ht, void **pData);
ZEND_API void zend_hash_internal_pointer_reset(HashTable *ht);
ZEND_API void zend_hash_internal_pointer_end(HashTable *ht);

/* Copying, merging and sorting */
ZEND_API void zend_hash_copy(HashTable *target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size);
ZEND_API void zend_hash_merge(HashTable *target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size, int overwrite);
ZEND_API int zend_hash_sort(HashTable *ht, sort_func_t sort_func, compare_func_t compare_func, int renumber);
ZEND_API int zend_hash_minmax(HashTable *ht, int (*compar) (const void *, const void *), int flag, void **pData);

ZEND_API int zend_hash_num_elements(HashTable *ht);

ZEND_API int zend_hash_rehash(HashTable *ht);

ZEND_API ulong hashpjw(char *arKey, uint nKeyLength);

#if ZEND_DEBUG
/* debug */
void zend_hash_display_pListTail(HashTable *ht);
void zend_hash_display(HashTable *ht);
#endif

END_EXTERN_C()

#endif							/* _HASH_ */
