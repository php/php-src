/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2002 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifdef ZTS

#include "zend_ts_hash.h"

/* ts management functions */
static void begin_read(TsHashTable *ht)
{
	tsrm_mutex_lock(ht->mx_reader);
	if ((++(ht->reader)) == 1) {
		tsrm_mutex_lock(ht->mx_writer);
	}
	tsrm_mutex_unlock(ht->mx_reader);
}

static void end_read(TsHashTable *ht)
{
	tsrm_mutex_lock(ht->mx_reader);
	if ((--(ht->reader)) == 0) {
		tsrm_mutex_unlock(ht->mx_writer);
	}
	tsrm_mutex_unlock(ht->mx_reader);
}

static void begin_write(TsHashTable *ht)
{
	tsrm_mutex_lock(ht->mx_writer);
}

static void end_write(TsHashTable *ht)
{
	tsrm_mutex_unlock(ht->mx_writer);
}

/* delegates */
ZEND_API int zend_ts_hash_init(TsHashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent)
{
	ht->mx_reader = tsrm_mutex_alloc();
	ht->mx_writer = tsrm_mutex_alloc();
	ht->reader = 0;
	return zend_hash_init(&(ht->hash), nSize, pHashFunction, pDestructor, persistent);
}

ZEND_API int zend_ts_hash_init_ex(TsHashTable *ht, uint nSize, hash_func_t pHashFunction, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection)
{
	ht->mx_reader = tsrm_mutex_alloc();
	ht->mx_writer = tsrm_mutex_alloc();
	ht->reader = 0;
	return zend_hash_init_ex(&(ht->hash), nSize, pHashFunction, pDestructor, persistent, bApplyProtection);
}

ZEND_API void zend_ts_hash_destroy(TsHashTable *ht)
{
	tsrm_mutex_free(ht->mx_reader);
	tsrm_mutex_free(ht->mx_reader);
	zend_hash_destroy(&(ht->hash));
}

ZEND_API void zend_ts_hash_clean(TsHashTable *ht)
{
	ht->reader = 0;
	zend_hash_clean(&(ht->hash));
}

ZEND_API int zend_ts_hash_add_or_update(TsHashTable *ht, char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_add_or_update(&(ht->hash), arKey, nKeyLength, pData, nDataSize, pDest, flag);
	end_write(ht);

	return retval;
}

ZEND_API int zend_ts_hash_quick_add_or_update(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, void *pData, uint nDataSize, void **pDest, int flag)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_quick_add_or_update(&(ht->hash), arKey, nKeyLength, h, pData, nDataSize, pDest, flag);
	end_write(ht);

	return retval;
}

ZEND_API int zend_ts_hash_index_update_or_next_insert(TsHashTable *ht, ulong h, void *pData, uint nDataSize, void **pDest, int flag)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_index_update_or_next_insert(&(ht->hash), h, pData, nDataSize, pDest, flag);
	end_write(ht);

	return retval;
}

ZEND_API int zend_ts_hash_add_empty_element(TsHashTable *ht, char *arKey, uint nKeyLength)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_add_empty_element(&(ht->hash), arKey, nKeyLength);
	end_write(ht);

	return retval;
}

ZEND_API void zend_ts_hash_graceful_destroy(TsHashTable *ht)
{
	tsrm_mutex_free(ht->mx_reader);
	tsrm_mutex_free(ht->mx_reader);
	zend_hash_graceful_destroy(&(ht->hash));
}

ZEND_API void zend_ts_hash_apply(TsHashTable *ht, apply_func_t apply_func TSRMLS_DC)
{
	begin_write(ht);
	zend_hash_apply(&(ht->hash), apply_func TSRMLS_CC);
	end_write(ht);
}

ZEND_API void zend_ts_hash_apply_with_argument(TsHashTable *ht, apply_func_arg_t apply_func, void *argument TSRMLS_DC)
{
	begin_write(ht);
	zend_hash_apply_with_argument(&(ht->hash), apply_func, argument TSRMLS_CC);
	end_write(ht);
}

ZEND_API void zend_ts_hash_apply_with_arguments(TsHashTable *ht, apply_func_args_t apply_func, int num_args, ...)
{
	va_list args;

	va_start(args, num_args);
	begin_write(ht);
	zend_hash_apply_with_arguments(&(ht->hash), apply_func, num_args, args);
	end_write(ht);
	va_end(args);
}

ZEND_API void zend_ts_hash_reverse_apply(TsHashTable *ht, apply_func_t apply_func TSRMLS_DC)
{
	begin_write(ht);
	zend_hash_reverse_apply(&(ht->hash), apply_func TSRMLS_CC);
	end_write(ht);
}

ZEND_API int zend_ts_hash_del_key_or_index(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, int flag)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_del_key_or_index(&(ht->hash), arKey, nKeyLength, h, flag);
	end_write(ht);

	return retval;
}

ZEND_API ulong zend_ts_get_hash_value(TsHashTable *ht, char *arKey, uint nKeyLength)
{
	ulong retval;

	begin_read(ht);
	retval = zend_get_hash_value(&(ht->hash), arKey, nKeyLength);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_find(TsHashTable *ht, char *arKey, uint nKeyLength, void **pData)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_find(&(ht->hash), arKey, nKeyLength, pData);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_quick_find(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, void **pData)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_quick_find(&(ht->hash), arKey, nKeyLength, h, pData);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_index_find(TsHashTable *ht, ulong h, void **pData)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_index_find(&(ht->hash), h, pData);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_exists(TsHashTable *ht, char *arKey, uint nKeyLength)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_exists(&(ht->hash), arKey, nKeyLength);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_index_exists(TsHashTable *ht, ulong h)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_index_exists(&(ht->hash), h);
	end_read(ht);

	return retval;
}

ZEND_API void zend_ts_hash_copy(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size)
{
	begin_read(source);
	begin_write(target);
	zend_hash_copy(&(target->hash), &(source->hash), pCopyConstructor, tmp, size);
	end_write(target);
	end_read(source);
}

ZEND_API void zend_ts_hash_merge(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size, int overwrite)
{
	begin_read(source);
	begin_write(target);
	zend_hash_merge(&(target->hash), &(source->hash), pCopyConstructor, tmp, size, overwrite);
	end_write(target);
	end_read(source);
}

ZEND_API void zend_ts_hash_merge_ex(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, uint size, zend_bool (*pReplaceOrig)(void *orig, void *p_new))
{
	begin_read(source);
	begin_write(target);
	zend_hash_merge_ex(&(target->hash), &(source->hash), pCopyConstructor, size, pReplaceOrig);
	end_write(target);
	end_read(source);
}

ZEND_API int zend_ts_hash_sort(TsHashTable *ht, sort_func_t sort_func, compare_func_t compare_func, int renumber TSRMLS_DC)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_sort(&(ht->hash), sort_func, compare_func, renumber TSRMLS_CC);
	end_write(ht);

	return retval;
}

ZEND_API int zend_ts_hash_compare(TsHashTable *ht1, TsHashTable *ht2, compare_func_t compar, zend_bool ordered TSRMLS_DC)
{
	int retval;

	begin_read(ht1);
	begin_read(ht2);
	retval = zend_hash_compare(&(ht1->hash), &(ht2->hash), compar, ordered TSRMLS_CC);
	end_read(ht2);
	end_read(ht1);

	return retval;
}

ZEND_API int zend_ts_hash_minmax(TsHashTable *ht, compare_func_t compar, int flag, void **pData TSRMLS_DC)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_minmax(&(ht->hash), compar, flag, pData TSRMLS_CC);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_num_elements(TsHashTable *ht)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_num_elements(&(ht->hash));
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_rehash(TsHashTable *ht)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_rehash(&(ht->hash));
	end_write(ht);

	return retval;
}

#if ZEND_DEBUG
void zend_ts_hash_display_pListTail(TsHashTable *ht)
{
	begin_read(ht);
	zend_hash_display_pListTail(&(ht->hash));
	end_read(ht);
}

void zend_ts_hash_display(TsHashTable *ht)
{
	begin_read(ht);
	zend_hash_display(&(ht->hash));
	end_read(ht);
}
#endif

#endif /* ZTS */
