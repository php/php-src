/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Gina Peter Banyard <girgias@php.net>                        |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_autoload.h"
#include "zend_hash.h"
#include "zend_types.h"
#include "zend_exceptions.h"
#include "zend_string.h"

ZEND_TLS HashTable *zend_class_autoload_functions;

static void zend_autoload_callback_zval_destroy(zval *element)
{
	zend_fcall_info_cache *fcc = Z_PTR_P(element);
	zend_fcc_dtor(fcc);
	efree(fcc);
}

static Bucket *autoload_find_registered_function(const HashTable *autoloader_table, const zend_fcall_info_cache *function_entry)
{
	zend_fcall_info_cache *current_function_entry;
	ZEND_HASH_MAP_FOREACH_PTR(autoloader_table, current_function_entry) {
		if (zend_fcc_equals(current_function_entry, function_entry)) {
			return _p;
		}
	} ZEND_HASH_FOREACH_END();
	return NULL;
}

ZEND_API zend_class_entry *zend_perform_class_autoload(zend_string *class_name, zend_string *lc_name)
{
	if (!zend_class_autoload_functions) {
		return NULL;
	}

	zval zname;
	ZVAL_STR(&zname, class_name);

	const HashTable *class_autoload_functions = zend_class_autoload_functions;

	/* Cannot use ZEND_HASH_MAP_FOREACH_PTR here as autoloaders may be
	 * added/removed during autoloading. */
	HashPosition pos;
	zend_hash_internal_pointer_reset_ex(class_autoload_functions, &pos);
	while (true) {
		zend_fcall_info_cache *func_info = zend_hash_get_current_data_ptr_ex(class_autoload_functions, &pos);
		if (!func_info) {
			break;
		}
		zend_call_known_fcc(func_info, /* retval */ NULL, /* param_count */ 1, /* params */ &zname, /* named_params */ NULL);

		if (EG(exception)) {
			return NULL;
		}
		if (ZSTR_HAS_CE_CACHE(class_name) && ZSTR_GET_CE_CACHE(class_name)) {
			return (zend_class_entry*)ZSTR_GET_CE_CACHE(class_name);
		}

		zend_class_entry *ce = zend_hash_find_ptr(EG(class_table), lc_name);
		if (ce) {
			return ce;
		}

		zend_hash_move_forward_ex(class_autoload_functions, &pos);
	}
	return NULL;
}

/* Needed for compatibility with spl_register_autoload() */
ZEND_API void zend_autoload_register_class_loader(zend_fcall_info_cache *fcc, bool prepend)
{
	ZEND_ASSERT(ZEND_FCC_INITIALIZED(*fcc));

	if (!zend_class_autoload_functions) {
		ALLOC_HASHTABLE(zend_class_autoload_functions);
		zend_hash_init(zend_class_autoload_functions, 1, NULL, zend_autoload_callback_zval_destroy, false);
		/* Initialize as non-packed hash table for prepend functionality. */
		zend_hash_real_init_mixed(zend_class_autoload_functions);
	}

	ZEND_ASSERT(
		fcc->function_handler->type != ZEND_INTERNAL_FUNCTION
		|| !zend_string_equals_literal(fcc->function_handler->common.function_name, "spl_autoload_call")
	);

	/* If function is already registered, don't do anything */
	if (autoload_find_registered_function(zend_class_autoload_functions, fcc)) {
		/* Release potential call trampoline */
		zend_release_fcall_info_cache(fcc);
		return;
	}

	zend_fcc_addref(fcc);
	zend_hash_next_index_insert_mem(zend_class_autoload_functions, fcc, sizeof(zend_fcall_info_cache));
	if (prepend && zend_hash_num_elements(zend_class_autoload_functions) > 1) {
		/* Move the newly created element to the head of the hashtable */
		ZEND_ASSERT(!HT_IS_PACKED(zend_class_autoload_functions));
		Bucket tmp = zend_class_autoload_functions->arData[zend_class_autoload_functions->nNumUsed-1];
		memmove(zend_class_autoload_functions->arData + 1, zend_class_autoload_functions->arData, sizeof(Bucket) * (zend_class_autoload_functions->nNumUsed - 1));
		zend_class_autoload_functions->arData[0] = tmp;
		zend_hash_rehash(zend_class_autoload_functions);
	}
}

ZEND_API bool zend_autoload_unregister_class_loader(const zend_fcall_info_cache *fcc) {
	if (zend_class_autoload_functions) {
		Bucket *p = autoload_find_registered_function(zend_class_autoload_functions, fcc);
		if (p) {
			zend_hash_del_bucket(zend_class_autoload_functions, p);
			return true;
		}
	}
	return false;
}

/* We do not return a HashTable* because zend_empty_array is not collectable,
 * therefore the zval holding this value must do so. Something that ZVAL_EMPTY_ARRAY(); does. */
ZEND_API void zend_autoload_fcc_map_to_callable_zval_map(zval *return_value) {
	if (zend_class_autoload_functions) {
		zend_fcall_info_cache *fcc;

		zend_array *map = zend_new_array(zend_hash_num_elements(zend_class_autoload_functions));
		ZEND_HASH_MAP_FOREACH_PTR(zend_class_autoload_functions, fcc) {
			zval tmp;
			zend_get_callable_zval_from_fcc(fcc, &tmp);
			zend_hash_next_index_insert(map, &tmp);
		} ZEND_HASH_FOREACH_END();
		RETURN_ARR(map);
	}
	RETURN_EMPTY_ARRAY();
}

/* Only for deprecated strange behaviour of spl_autoload_unregister() */
ZEND_API void zend_autoload_clean_class_loaders(void)
{
	if (zend_class_autoload_functions) {
		/* Don't destroy the hash table, as we might be iterating over it right now. */
		zend_hash_clean(zend_class_autoload_functions);
	}
}

void zend_autoload_shutdown(void)
{
	if (zend_class_autoload_functions) {
		zend_hash_destroy(zend_class_autoload_functions);
		FREE_HASHTABLE(zend_class_autoload_functions);
		zend_class_autoload_functions = NULL;
	}
}
