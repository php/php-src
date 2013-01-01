/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Harald Radi <harald.radi@nme.at>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_TS_HASH_H
#define ZEND_TS_HASH_H

#include "zend.h"

typedef struct _zend_ts_hashtable {
	HashTable hash;
	zend_uint reader;
#ifdef ZTS
	MUTEX_T mx_reader;
	MUTEX_T mx_writer;
#endif
} TsHashTable;

BEGIN_EXTERN_C()

#define TS_HASH(table) (&(table->hash))

/* startup/shutdown */
ZEND_API int _zend_ts_hash_init(TsHashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent ZEND_FILE_LINE_DC);
ZEND_API int _zend_ts_hash_init_ex(TsHashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC);
ZEND_API void zend_ts_hash_destroy(TsHashTable *ht);
ZEND_API void zend_ts_hash_clean(TsHashTable *ht);

#define zend_ts_hash_init(ht, nSize, pHashFunction, pDestructor, persistent)	\
	_zend_ts_hash_init(ht, nSize, pHashFunction, pDestructor, persistent ZEND_FILE_LINE_CC)
#define zend_ts_hash_init_ex(ht, nSize, pHashFunction, pDestructor, persistent, bApplyProtection)	\
	_zend_ts_hash_init_ex(ht, nSize, pHashFunction, pDestructor, persistent, bApplyProtection ZEND_FILE_LINE_CC)


/* additions/updates/changes */
ZEND_API int _zend_ts_hash_add_or_update(TsHashTable *ht, char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC);
#define zend_ts_hash_update(ht, arKey, nKeyLength, pData, nDataSize, pDest) \
		_zend_ts_hash_add_or_update(ht, arKey, nKeyLength, pData, nDataSize, pDest, HASH_UPDATE ZEND_FILE_LINE_CC)
#define zend_ts_hash_add(ht, arKey, nKeyLength, pData, nDataSize, pDest) \
		_zend_ts_hash_add_or_update(ht, arKey, nKeyLength, pData, nDataSize, pDest, HASH_ADD ZEND_FILE_LINE_CC)

ZEND_API int _zend_ts_hash_quick_add_or_update(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC);
#define zend_ts_hash_quick_update(ht, arKey, nKeyLength, h, pData, nDataSize, pDest) \
		_zend_ts_hash_quick_add_or_update(ht, arKey, nKeyLength, h, pData, nDataSize, pDest, HASH_UPDATE ZEND_FILE_LINE_CC)
#define zend_ts_hash_quick_add(ht, arKey, nKeyLength, h, pData, nDataSize, pDest) \
		_zend_ts_hash_quick_add_or_update(ht, arKey, nKeyLength, h, pData, nDataSize, pDest, HASH_ADD ZEND_FILE_LINE_CC)

ZEND_API int _zend_ts_hash_index_update_or_next_insert(TsHashTable *ht, ulong h, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC);
#define zend_ts_hash_index_update(ht, h, pData, nDataSize, pDest) \
		_zend_ts_hash_index_update_or_next_insert(ht, h, pData, nDataSize, pDest, HASH_UPDATE ZEND_FILE_LINE_CC)
#define zend_ts_hash_next_index_insert(ht, pData, nDataSize, pDest) \
		_zend_ts_hash_index_update_or_next_insert(ht, 0, pData, nDataSize, pDest, HASH_NEXT_INSERT ZEND_FILE_LINE_CC)

ZEND_API int zend_ts_hash_add_empty_element(TsHashTable *ht, char *arKey, uint nKeyLength);

ZEND_API void zend_ts_hash_graceful_destroy(TsHashTable *ht);
ZEND_API void zend_ts_hash_apply(TsHashTable *ht, apply_func_t apply_func TSRMLS_DC);
ZEND_API void zend_ts_hash_apply_with_argument(TsHashTable *ht, apply_func_arg_t apply_func, void * TSRMLS_DC);
ZEND_API void zend_ts_hash_apply_with_arguments(TsHashTable *ht TSRMLS_DC, apply_func_args_t apply_func, int, ...);

ZEND_API void zend_ts_hash_reverse_apply(TsHashTable *ht, apply_func_t apply_func TSRMLS_DC);


/* Deletes */
ZEND_API int zend_ts_hash_del_key_or_index(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, int flag);
#define zend_ts_hash_del(ht, arKey, nKeyLength) \
		zend_ts_hash_del_key_or_index(ht, arKey, nKeyLength, 0, HASH_DEL_KEY)
#define zend_ts_hash_index_del(ht, h) \
		zend_ts_hash_del_key_or_index(ht, NULL, 0, h, HASH_DEL_INDEX)

ZEND_API ulong zend_ts_get_hash_value(TsHashTable *ht, char *arKey, uint nKeyLength);

/* Data retreival */
ZEND_API int zend_ts_hash_find(TsHashTable *ht, char *arKey, uint nKeyLength, void **pData);
ZEND_API int zend_ts_hash_quick_find(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, void **pData);
ZEND_API int zend_ts_hash_index_find(TsHashTable *ht, ulong h, void **pData);

/* Misc */
ZEND_API int zend_ts_hash_exists(TsHashTable *ht, char *arKey, uint nKeyLength);
ZEND_API int zend_ts_hash_index_exists(TsHashTable *ht, ulong h);

/* Copying, merging and sorting */
ZEND_API void zend_ts_hash_copy(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size);
ZEND_API void zend_ts_hash_copy_to_hash(HashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size);
ZEND_API void zend_ts_hash_merge(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size, int overwrite);
ZEND_API void zend_ts_hash_merge_ex(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, uint size, merge_checker_func_t pMergeSource, void *pParam);
ZEND_API int zend_ts_hash_sort(TsHashTable *ht, sort_func_t sort_func, compare_func_t compare_func, int renumber TSRMLS_DC);
ZEND_API int zend_ts_hash_compare(TsHashTable *ht1, TsHashTable *ht2, compare_func_t compar, zend_bool ordered TSRMLS_DC);
ZEND_API int zend_ts_hash_minmax(TsHashTable *ht, compare_func_t compar, int flag, void **pData TSRMLS_DC);

ZEND_API int zend_ts_hash_num_elements(TsHashTable *ht);

ZEND_API int zend_ts_hash_rehash(TsHashTable *ht);

ZEND_API ulong zend_ts_hash_func(char *arKey, uint nKeyLength);

#if ZEND_DEBUG
/* debug */
void zend_ts_hash_display_pListTail(TsHashTable *ht);
void zend_ts_hash_display(TsHashTable *ht);
#endif

END_EXTERN_C()

#define ZEND_TS_INIT_SYMTABLE(ht)								\
	ZEND_TS_INIT_SYMTABLE_EX(ht, 2, 0)

#define ZEND_TS_INIT_SYMTABLE_EX(ht, n, persistent)			\
	zend_ts_hash_init(ht, n, NULL, ZVAL_PTR_DTOR, persistent)

#endif							/* ZEND_HASH_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
