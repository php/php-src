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
   | Authors: Harald Radi <harald.radi@nme.at>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend_ts_hash.h"

/* ts management functions */
static void begin_read(TsHashTable *ht)
{
#ifdef ZTS
	tsrm_mutex_lock(ht->mx_reader);
	if ((++(ht->reader)) == 1) {
		tsrm_mutex_lock(ht->mx_writer);
	}
	tsrm_mutex_unlock(ht->mx_reader);
#endif
}

static void end_read(TsHashTable *ht)
{
#ifdef ZTS
	tsrm_mutex_lock(ht->mx_reader);
	if ((--(ht->reader)) == 0) {
		tsrm_mutex_unlock(ht->mx_writer);
	}
	tsrm_mutex_unlock(ht->mx_reader);
#endif
}

static void begin_write(TsHashTable *ht)
{
#ifdef ZTS
	tsrm_mutex_lock(ht->mx_writer);
#endif
}

static void end_write(TsHashTable *ht)
{
#ifdef ZTS
	tsrm_mutex_unlock(ht->mx_writer);
#endif
}

/* delegates */
ZEND_API int zend_ts_hash_init(TsHashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent)
{
#ifdef ZTS
	ht->mx_reader = tsrm_mutex_alloc();
	ht->mx_writer = tsrm_mutex_alloc();
	ht->reader = 0;
#endif
	return zend_hash_init(TS_HASH(ht), nSize, pHashFunction, pDestructor, persistent);
}

ZEND_API int zend_ts_hash_init_ex(TsHashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection)
{
#ifdef ZTS
	ht->mx_reader = tsrm_mutex_alloc();
	ht->mx_writer = tsrm_mutex_alloc();
	ht->reader = 0;
#endif
	return zend_hash_init_ex(TS_HASH(ht), nSize, pHashFunction, pDestructor, persistent, bApplyProtection);
}

ZEND_API void zend_ts_hash_destroy(TsHashTable *ht)
{
#ifdef ZTS
	tsrm_mutex_free(ht->mx_reader);
	tsrm_mutex_free(ht->mx_writer);
#endif
	zend_hash_destroy(TS_HASH(ht));
}

ZEND_API void zend_ts_hash_clean(TsHashTable *ht)
{
	ht->reader = 0;
	zend_hash_clean(TS_HASH(ht));
}

ZEND_API int zend_ts_hash_add_or_update(TsHashTable *ht, char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_add_or_update(TS_HASH(ht), arKey, nKeyLength, pData, nDataSize, pDest, flag);
	end_write(ht);

	return retval;
}

ZEND_API int zend_ts_hash_quick_add_or_update(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, void *pData, uint nDataSize, void **pDest, int flag)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_quick_add_or_update(TS_HASH(ht), arKey, nKeyLength, h, pData, nDataSize, pDest, flag);
	end_write(ht);

	return retval;
}

ZEND_API int zend_ts_hash_index_update_or_next_insert(TsHashTable *ht, ulong h, void *pData, uint nDataSize, void **pDest, int flag)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_index_update_or_next_insert(TS_HASH(ht), h, pData, nDataSize, pDest, flag);
	end_write(ht);

	return retval;
}

ZEND_API int zend_ts_hash_add_empty_element(TsHashTable *ht, char *arKey, uint nKeyLength)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_add_empty_element(TS_HASH(ht), arKey, nKeyLength);
	end_write(ht);

	return retval;
}

ZEND_API void zend_ts_hash_graceful_destroy(TsHashTable *ht)
{
#ifdef ZTS
	tsrm_mutex_free(ht->mx_reader);
	tsrm_mutex_free(ht->mx_reader);
#endif
	zend_hash_graceful_destroy(TS_HASH(ht));
}

ZEND_API void zend_ts_hash_apply(TsHashTable *ht, apply_func_t apply_func TSRMLS_DC)
{
	begin_write(ht);
	zend_hash_apply(TS_HASH(ht), apply_func TSRMLS_CC);
	end_write(ht);
}

ZEND_API void zend_ts_hash_apply_with_argument(TsHashTable *ht, apply_func_arg_t apply_func, void *argument TSRMLS_DC)
{
	begin_write(ht);
	zend_hash_apply_with_argument(TS_HASH(ht), apply_func, argument TSRMLS_CC);
	end_write(ht);
}

ZEND_API void zend_ts_hash_apply_with_arguments(TsHashTable *ht, apply_func_args_t apply_func, int num_args, ...)
{
	va_list args;

	va_start(args, num_args);
	begin_write(ht);
	zend_hash_apply_with_arguments(TS_HASH(ht), apply_func, num_args, args);
	end_write(ht);
	va_end(args);
}

ZEND_API void zend_ts_hash_reverse_apply(TsHashTable *ht, apply_func_t apply_func TSRMLS_DC)
{
	begin_write(ht);
	zend_hash_reverse_apply(TS_HASH(ht), apply_func TSRMLS_CC);
	end_write(ht);
}

ZEND_API int zend_ts_hash_del_key_or_index(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, int flag)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_del_key_or_index(TS_HASH(ht), arKey, nKeyLength, h, flag);
	end_write(ht);

	return retval;
}

ZEND_API ulong zend_ts_get_hash_value(TsHashTable *ht, char *arKey, uint nKeyLength)
{
	ulong retval;

	begin_read(ht);
	retval = zend_get_hash_value(arKey, nKeyLength);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_find(TsHashTable *ht, char *arKey, uint nKeyLength, void **pData)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_find(TS_HASH(ht), arKey, nKeyLength, pData);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_quick_find(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, void **pData)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_quick_find(TS_HASH(ht), arKey, nKeyLength, h, pData);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_index_find(TsHashTable *ht, ulong h, void **pData)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_index_find(TS_HASH(ht), h, pData);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_exists(TsHashTable *ht, char *arKey, uint nKeyLength)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_exists(TS_HASH(ht), arKey, nKeyLength);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_index_exists(TsHashTable *ht, ulong h)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_index_exists(TS_HASH(ht), h);
	end_read(ht);

	return retval;
}

ZEND_API void zend_ts_hash_copy(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size)
{
	begin_read(source);
	begin_write(target);
	zend_hash_copy(TS_HASH(target), TS_HASH(source), pCopyConstructor, tmp, size);
	end_write(target);
	end_read(source);
}

ZEND_API void zend_ts_hash_merge(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size, int overwrite)
{
	begin_read(source);
	begin_write(target);
	zend_hash_merge(TS_HASH(target), TS_HASH(source), pCopyConstructor, tmp, size, overwrite);
	end_write(target);
	end_read(source);
}

ZEND_API void zend_ts_hash_merge_ex(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, uint size, merge_checker_func_t pMergeSource, void *pParam)
{
	begin_read(source);
	begin_write(target);
	zend_hash_merge_ex(TS_HASH(target), TS_HASH(source), pCopyConstructor, size, pMergeSource, pParam);
	end_write(target);
	end_read(source);
}

ZEND_API int zend_ts_hash_sort(TsHashTable *ht, sort_func_t sort_func, compare_func_t compare_func, int renumber TSRMLS_DC)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_sort(TS_HASH(ht), sort_func, compare_func, renumber TSRMLS_CC);
	end_write(ht);

	return retval;
}

ZEND_API int zend_ts_hash_compare(TsHashTable *ht1, TsHashTable *ht2, compare_func_t compar, zend_bool ordered TSRMLS_DC)
{
	int retval;

	begin_read(ht1);
	begin_read(ht2);
	retval = zend_hash_compare(TS_HASH(ht1), TS_HASH(ht2), compar, ordered TSRMLS_CC);
	end_read(ht2);
	end_read(ht1);

	return retval;
}

ZEND_API int zend_ts_hash_minmax(TsHashTable *ht, compare_func_t compar, int flag, void **pData TSRMLS_DC)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_minmax(TS_HASH(ht), compar, flag, pData TSRMLS_CC);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_num_elements(TsHashTable *ht)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_num_elements(TS_HASH(ht));
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_rehash(TsHashTable *ht)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_rehash(TS_HASH(ht));
	end_write(ht);

	return retval;
}

#if ZEND_DEBUG
void zend_ts_hash_display_pListTail(TsHashTable *ht)
{
	begin_read(ht);
	zend_hash_display_pListTail(TS_HASH(ht));
	end_read(ht);
}

void zend_ts_hash_display(TsHashTable *ht)
{
	begin_read(ht);
	zend_hash_display(TS_HASH(ht));
	end_read(ht);
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
