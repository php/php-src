/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
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

#include "zend.h"
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
ZEND_API void _zend_ts_hash_init(TsHashTable *ht, uint32_t nSize, dtor_func_t pDestructor, zend_bool persistent)
{
#ifdef ZTS
	ht->mx_reader = tsrm_mutex_alloc();
	ht->mx_writer = tsrm_mutex_alloc();
	ht->reader = 0;
#endif
	_zend_hash_init(TS_HASH(ht), nSize, pDestructor, persistent);
}

ZEND_API void zend_ts_hash_destroy(TsHashTable *ht)
{
	begin_write(ht);
	zend_hash_destroy(TS_HASH(ht));
	end_write(ht);

#ifdef ZTS
	tsrm_mutex_free(ht->mx_reader);
	tsrm_mutex_free(ht->mx_writer);
#endif
}

ZEND_API void zend_ts_hash_clean(TsHashTable *ht)
{
	ht->reader = 0;
	begin_write(ht);
	zend_hash_clean(TS_HASH(ht));
	end_write(ht);
}

ZEND_API zval *zend_ts_hash_add(TsHashTable *ht, zend_string *key, zval *pData)
{
	zval *retval;

	begin_write(ht);
	retval = zend_hash_add(TS_HASH(ht), key, pData);
	end_write(ht);

	return retval;
}

ZEND_API zval *zend_ts_hash_update(TsHashTable *ht, zend_string *key, zval *pData)
{
	zval *retval;

	begin_write(ht);
	retval = zend_hash_update(TS_HASH(ht), key, pData);
	end_write(ht);

	return retval;
}

ZEND_API zval *zend_ts_hash_next_index_insert(TsHashTable *ht, zval *pData)
{
	zval *retval;

	begin_write(ht);
	retval = zend_hash_next_index_insert(TS_HASH(ht), pData);
	end_write(ht);

	return retval;
}

ZEND_API zval *zend_ts_hash_index_update(TsHashTable *ht, zend_ulong h, zval *pData)
{
	zval *retval;

	begin_write(ht);
	retval = zend_hash_index_update(TS_HASH(ht), h, pData);
	end_write(ht);

	return retval;
}

ZEND_API zval *zend_ts_hash_add_empty_element(TsHashTable *ht, zend_string *key)
{
	zval *retval;

	begin_write(ht);
	retval = zend_hash_add_empty_element(TS_HASH(ht), key);
	end_write(ht);

	return retval;
}

ZEND_API void zend_ts_hash_graceful_destroy(TsHashTable *ht)
{
	begin_write(ht);
	zend_hash_graceful_destroy(TS_HASH(ht));
	end_write(ht);

#ifdef ZTS
	tsrm_mutex_free(ht->mx_reader);
	tsrm_mutex_free(ht->mx_writer);
#endif
}

ZEND_API void zend_ts_hash_apply(TsHashTable *ht, apply_func_t apply_func)
{
	begin_write(ht);
	zend_hash_apply(TS_HASH(ht), apply_func);
	end_write(ht);
}

ZEND_API void zend_ts_hash_apply_with_argument(TsHashTable *ht, apply_func_arg_t apply_func, void *argument)
{
	begin_write(ht);
	zend_hash_apply_with_argument(TS_HASH(ht), apply_func, argument);
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

ZEND_API void zend_ts_hash_reverse_apply(TsHashTable *ht, apply_func_t apply_func)
{
	begin_write(ht);
	zend_hash_reverse_apply(TS_HASH(ht), apply_func);
	end_write(ht);
}

ZEND_API int zend_ts_hash_del(TsHashTable *ht, zend_string *key)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_del(TS_HASH(ht), key);
	end_write(ht);

	return retval;
}

ZEND_API int zend_ts_hash_index_del(TsHashTable *ht, zend_ulong h)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_index_del(TS_HASH(ht), h);
	end_write(ht);

	return retval;
}

ZEND_API zval *zend_ts_hash_find(TsHashTable *ht, zend_string *key)
{
	zval *retval;

	begin_read(ht);
	retval = zend_hash_find(TS_HASH(ht), key);
	end_read(ht);

	return retval;
}

ZEND_API zval *zend_ts_hash_index_find(TsHashTable *ht, zend_ulong h)
{
	zval *retval;

	begin_read(ht);
	retval = zend_hash_index_find(TS_HASH(ht), h);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_exists(TsHashTable *ht, zend_string *key)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_exists(TS_HASH(ht), key);
	end_read(ht);

	return retval;
}

ZEND_API int zend_ts_hash_index_exists(TsHashTable *ht, zend_ulong h)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_index_exists(TS_HASH(ht), h);
	end_read(ht);

	return retval;
}

ZEND_API void zend_ts_hash_copy(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor)
{
	begin_read(source);
	begin_write(target);
	zend_hash_copy(TS_HASH(target), TS_HASH(source), pCopyConstructor);
	end_write(target);
	end_read(source);
}

ZEND_API void zend_ts_hash_copy_to_hash(HashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor)
{
	begin_read(source);
	zend_hash_copy(target, TS_HASH(source), pCopyConstructor);
	end_read(source);
}

ZEND_API void zend_ts_hash_merge(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, int overwrite)
{
	begin_read(source);
	begin_write(target);
	zend_hash_merge(TS_HASH(target), TS_HASH(source), pCopyConstructor, overwrite);
	end_write(target);
	end_read(source);
}

ZEND_API void zend_ts_hash_merge_ex(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, merge_checker_func_t pMergeSource, void *pParam)
{
	begin_read(source);
	begin_write(target);
	zend_hash_merge_ex(TS_HASH(target), TS_HASH(source), pCopyConstructor, pMergeSource, pParam);
	end_write(target);
	end_read(source);
}

ZEND_API int zend_ts_hash_sort(TsHashTable *ht, sort_func_t sort_func, compare_func_t compare_func, int renumber)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_sort_ex(TS_HASH(ht), sort_func, compare_func, renumber);
	end_write(ht);

	return retval;
}

ZEND_API int zend_ts_hash_compare(TsHashTable *ht1, TsHashTable *ht2, compare_func_t compar, zend_bool ordered)
{
	int retval;

	begin_read(ht1);
	begin_read(ht2);
	retval = zend_hash_compare(TS_HASH(ht1), TS_HASH(ht2), compar, ordered);
	end_read(ht2);
	end_read(ht1);

	return retval;
}

ZEND_API zval *zend_ts_hash_minmax(TsHashTable *ht, compare_func_t compar, int flag)
{
	zval *retval;

	begin_read(ht);
	retval = zend_hash_minmax(TS_HASH(ht), compar, flag);
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

ZEND_API zval *zend_ts_hash_str_find(TsHashTable *ht, const char *key, size_t len)
{
	zval *retval;

	begin_read(ht);
	retval = zend_hash_str_find(TS_HASH(ht), key, len);
	end_read(ht);

	return retval;
}

ZEND_API zval *zend_ts_hash_str_update(TsHashTable *ht, const char *key, size_t len, zval *pData)
{
	zval *retval;

	begin_write(ht);
	retval = zend_hash_str_update(TS_HASH(ht), key, len, pData);
	end_write(ht);

	return retval;
}

ZEND_API zval *zend_ts_hash_str_add(TsHashTable *ht, const char *key, size_t len, zval *pData)
{
	zval *retval;

	begin_write(ht);
	retval = zend_hash_str_add(TS_HASH(ht), key, len, pData);
	end_write(ht);

	return retval;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
