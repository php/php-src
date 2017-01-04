/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 Zend Technologies Ltd. (http://www.zend.com) |
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
	uint32_t reader;
#ifdef ZTS
	MUTEX_T mx_reader;
	MUTEX_T mx_writer;
#endif
} TsHashTable;

BEGIN_EXTERN_C()

#define TS_HASH(table) (&(table->hash))

/* startup/shutdown */
ZEND_API void _zend_ts_hash_init(TsHashTable *ht, uint nSize, dtor_func_t pDestructor, zend_bool persistent ZEND_FILE_LINE_DC);
ZEND_API void _zend_ts_hash_init_ex(TsHashTable *ht, uint nSize, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC);
ZEND_API void zend_ts_hash_destroy(TsHashTable *ht);
ZEND_API void zend_ts_hash_clean(TsHashTable *ht);

#define zend_ts_hash_init(ht, nSize, pHashFunction, pDestructor, persistent)	\
	_zend_ts_hash_init(ht, nSize, pDestructor, persistent ZEND_FILE_LINE_CC)
#define zend_ts_hash_init_ex(ht, nSize, pHashFunction, pDestructor, persistent, bApplyProtection)	\
	_zend_ts_hash_init_ex(ht, nSize, pDestructor, persistent, bApplyProtection ZEND_FILE_LINE_CC)


/* additions/updates/changes */
ZEND_API zval *_zend_ts_hash_add_or_update(TsHashTable *ht, zend_string *key, zval *pData, int flag ZEND_FILE_LINE_DC);
#define zend_ts_hash_update(ht, key, pData) \
		_zend_ts_hash_add_or_update(ht, key, pData, HASH_UPDATE ZEND_FILE_LINE_CC)
#define zend_ts_hash_add(ht, key, pData) \
		_zend_ts_hash_add_or_update(ht, key, pData, HASH_ADD ZEND_FILE_LINE_CC)

ZEND_API zval *_zend_ts_hash_index_add_or_update(TsHashTable *ht, zend_ulong h, zval *pData, int flag ZEND_FILE_LINE_DC);
#define zend_ts_hash_index_update(ht, h, pData) \
		_zend_ts_hash_index_add_or_update(ht, h, pData, HASH_UPDATE ZEND_FILE_LINE_CC)
#define zend_ts_hash_next_index_insert(ht, pData) \
		_zend_ts_hash_index_add_or_update(ht, ht->nNextFreeElement, pData, HASH_ADD ZEND_FILE_LINE_CC)

ZEND_API zval* zend_ts_hash_add_empty_element(TsHashTable *ht, zend_string *key);

ZEND_API void zend_ts_hash_graceful_destroy(TsHashTable *ht);
ZEND_API void zend_ts_hash_apply(TsHashTable *ht, apply_func_t apply_func);
ZEND_API void zend_ts_hash_apply_with_argument(TsHashTable *ht, apply_func_arg_t apply_func, void *);
ZEND_API void zend_ts_hash_apply_with_arguments(TsHashTable *ht, apply_func_args_t apply_func, int, ...);

ZEND_API void zend_ts_hash_reverse_apply(TsHashTable *ht, apply_func_t apply_func);


/* Deletes */
ZEND_API int zend_ts_hash_del(TsHashTable *ht, zend_string *key);
ZEND_API int zend_ts_hash_index_del(TsHashTable *ht, zend_ulong h);

/* Data retreival */
ZEND_API zval *zend_ts_hash_find(TsHashTable *ht, zend_string *key);
ZEND_API zval *zend_ts_hash_index_find(TsHashTable *ht, zend_ulong);

/* Misc */
ZEND_API int zend_ts_hash_exists(TsHashTable *ht, zend_string *key);
ZEND_API int zend_ts_hash_index_exists(TsHashTable *ht, zend_ulong h);

/* Copying, merging and sorting */
ZEND_API void zend_ts_hash_copy(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor);
ZEND_API void zend_ts_hash_copy_to_hash(HashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor);
ZEND_API void zend_ts_hash_merge(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, int overwrite);
ZEND_API void zend_ts_hash_merge_ex(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, merge_checker_func_t pMergeSource, void *pParam);
ZEND_API int zend_ts_hash_sort(TsHashTable *ht, sort_func_t sort_func, compare_func_t compare_func, int renumber);
ZEND_API int zend_ts_hash_compare(TsHashTable *ht1, TsHashTable *ht2, compare_func_t compar, zend_bool ordered);
ZEND_API zval *zend_ts_hash_minmax(TsHashTable *ht, compare_func_t compar, int flag);

ZEND_API int zend_ts_hash_num_elements(TsHashTable *ht);

ZEND_API int zend_ts_hash_rehash(TsHashTable *ht);

#if ZEND_DEBUG
/* debug */
void zend_ts_hash_display_pListTail(TsHashTable *ht);
void zend_ts_hash_display(TsHashTable *ht);
#endif

ZEND_API zval *zend_ts_hash_str_find(TsHashTable *ht, const char *key, size_t len);
ZEND_API zval *_zend_ts_hash_str_update(TsHashTable *ht, const char *key, size_t len, zval *pData ZEND_FILE_LINE_DC);
ZEND_API zval *_zend_ts_hash_str_add(TsHashTable *ht, const char *key, size_t len, zval *pData ZEND_FILE_LINE_DC);

#define zend_ts_hash_str_update(ht, key, len, pData) \
		_zend_ts_hash_str_update(ht, key, len, pData ZEND_FILE_LINE_CC)
#define zend_ts_hash_str_add(ht, key, len, pData) \
		_zend_ts_hash_str_add(ht, key, len, pData ZEND_FILE_LINE_CC)

static zend_always_inline void *zend_ts_hash_str_find_ptr(TsHashTable *ht, const char *str, size_t len)
{
	zval *zv;

	zv = zend_ts_hash_str_find(ht, str, len);
	return zv ? Z_PTR_P(zv) : NULL;
}

static zend_always_inline void *zend_ts_hash_str_update_ptr(TsHashTable *ht, const char *str, size_t len, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_ts_hash_str_update(ht, str, len, &tmp);
	return zv ? Z_PTR_P(zv) : NULL;
}

static zend_always_inline void *zend_ts_hash_str_add_ptr(TsHashTable *ht, const char *str, size_t len, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_ts_hash_str_add(ht, str, len, &tmp);
	return zv ? Z_PTR_P(zv) : NULL;
}

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
