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
   | Authors: George Peter Banyard <girgias@php.net>                      |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_autoload.h"
#include "zend_hash.h"
#include "zend_types.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_string.h"

ZEND_TLS HashTable *autoloader_class_autoload_functions;
ZEND_TLS HashTable *autoloader_function_autoload_functions;

#define HT_MOVE_TAIL_TO_HEAD(ht) \
	do { \
		Bucket tmp = (ht)->arData[(ht)->nNumUsed-1]; \
		memmove((ht)->arData + 1, (ht)->arData, \
			sizeof(Bucket) * ((ht)->nNumUsed - 1)); \
		(ht)->arData[0] = tmp; \
		if (UNEXPECTED(!((ht)->u.flags & HASH_FLAG_PACKED))) { \
			zend_hash_rehash(ht); \
		} else { \
			zend_autoload_reindex(ht); \
		} \
	} while (0)

static void zend_autoload_reindex(HashTable *ht)
{
	ZEND_ASSERT(ht->u.flags & HASH_FLAG_PACKED);
	for (size_t i = 0; i < ht->nNumUsed; i++) {
		ht->arData[i].h = i;
	}
}

ZEND_API void zend_autoload_callback_zval_destroy(zval *element)
{
	zend_fcall_info_cache *fcc = Z_PTR_P(element);
	zend_fcc_dtor(fcc);
	efree(fcc);
}

static Bucket *autoload_find_registered_function(HashTable *autoloader_table, zend_fcall_info_cache *function_entry)
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
	if (!autoloader_class_autoload_functions) {
		return NULL;
	}

	zval zname;
	ZVAL_STR(&zname, class_name);

	HashTable *class_autoload_functions = autoloader_class_autoload_functions;

	/* Cannot use ZEND_HASH_MAP_FOREACH_PTR here as autoloaders may be
	 * added/removed during autoloading. */
	HashPosition pos;
	zend_hash_internal_pointer_reset_ex(class_autoload_functions, &pos);
	while (1) {
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
		if (zend_hash_exists(EG(class_table), lc_name)) {
			return (zend_class_entry*) zend_hash_find_ptr(EG(class_table), lc_name);
		}

		zend_hash_move_forward_ex(class_autoload_functions, &pos);
	}
	return NULL;
}

ZEND_API zend_function *zend_perform_function_autoload(zend_string *function_name, zend_string *lc_name)
{
	if (!autoloader_function_autoload_functions) {
		return NULL;
	}

	zval zname;
	ZVAL_STR(&zname, function_name);

	HashTable *function_autoload_functions = autoloader_function_autoload_functions;

	/* Cannot use ZEND_HASH_MAP_FOREACH_PTR here as autoloaders may be
	 * added/removed during autoloading. */
	HashPosition pos;
	zend_hash_internal_pointer_reset_ex(function_autoload_functions, &pos);
	while (1) {
		zend_fcall_info_cache *func_info = zend_hash_get_current_data_ptr_ex(function_autoload_functions, &pos);
		if (!func_info) {
			break;
		}
		zend_call_known_fcc(func_info, /* retval */ NULL, /* param_count */ 1, /* params */ &zname, /* named_params */ NULL);

		if (EG(exception)) {
			return NULL;
		}
		if (zend_hash_exists(EG(function_table), lc_name)) {
			return (zend_function*) zend_hash_find_ptr(EG(function_table), lc_name);
		}

		zend_hash_move_forward_ex(function_autoload_functions, &pos);
	}
	return NULL;
}

/* Needed for compatibility with spl_register_autoload() */
ZEND_API void zend_register_class_autoloader(zend_fcall_info *fci, zend_fcall_info_cache *fcc, bool prepend)
{
	if (!autoloader_class_autoload_functions) {
		ALLOC_HASHTABLE(autoloader_class_autoload_functions);
		zend_hash_init(autoloader_class_autoload_functions, 1, NULL, zend_autoload_callback_zval_destroy, 0);
		/* Initialize as non-packed hash table for prepend functionality. */
		zend_hash_real_init_mixed(autoloader_class_autoload_functions);
	}

	if (!ZEND_FCC_INITIALIZED(*fcc)) {
		ZEND_ASSERT(fci && ZEND_FCI_INITIALIZED(*fci) && "FCI Must be provided if the callable is a trampoline");
		/* Call trampoline has been cleared by zpp. Refetch it, because we want to deal
		 * with it ourselves. It is important that it is not refetched on every call,
		 * because calls may occur from different scopes. */
		zend_is_callable_ex(&fci->function_name, NULL, 0, NULL, fcc, NULL);
	}

	if (fcc->function_handler->type == ZEND_INTERNAL_FUNCTION &&
		fcc->function_handler->internal_function.handler == zif_autoload_call_class) {
		zend_argument_value_error(1, "must not be the autoload_call_class() function");
		return;
	}

	/* If function is already registered, don't do anything */
	if (autoload_find_registered_function(autoloader_class_autoload_functions, fcc)) {
		/* Release call trampoline */
		zend_release_fcall_info_cache(fcc);
		return;
	}

	zend_fcall_info_cache *entry = emalloc(sizeof(zend_fcall_info_cache));
	zend_fcc_dup(entry, fcc);
	zend_hash_next_index_insert_ptr(autoloader_class_autoload_functions, entry);
	if (prepend && zend_hash_num_elements(autoloader_class_autoload_functions) > 1) {
		/* Move the newly created element to the head of the hashtable */
		HT_MOVE_TAIL_TO_HEAD(autoloader_class_autoload_functions);
	}
}

// TODO USERLAND FUNCTIONS, maybe namespace them?
static void autoload_list(INTERNAL_FUNCTION_PARAMETERS, HashTable *symbol_table)
{

	zend_fcall_info_cache *func_info;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (!symbol_table) {
		RETURN_EMPTY_ARRAY();
	}

	array_init(return_value);

	ZEND_HASH_FOREACH_PTR(symbol_table, func_info) {
		zval tmp;
		zend_get_callable_zval_from_fcc(func_info, &tmp);
		add_next_index_zval(return_value, &tmp);
	} ZEND_HASH_FOREACH_END();
}

/* Register given function as a class autoloader */
ZEND_FUNCTION(autoload_register_class)
{
	bool prepend = false;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_FUNC(fci, fcc)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(prepend)
	ZEND_PARSE_PARAMETERS_END();

	zend_register_class_autoloader(&fci, &fcc, prepend);
}

ZEND_FUNCTION(autoload_unregister_class)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fci, fcc)
	ZEND_PARSE_PARAMETERS_END();

	if (!autoloader_class_autoload_functions) {
		RETURN_FALSE;
	}
	ZEND_ASSERT(autoloader_class_autoload_functions);

	/* Why the fuck does this flush the autoloading table? This makes close to no sense.
	 * This is forward-ported from SPL */
	if (ZEND_FCC_INITIALIZED(fcc) && fcc.function_handler->type == ZEND_INTERNAL_FUNCTION &&
		fcc.function_handler->internal_function.handler == zif_autoload_call_class) {
		zend_error(E_DEPRECATED, "Flushing the class autoloader table by passing autoload_call_class() is deprecated");
		// Don't destroy the hash table, as we might be iterating over it right now.
		zend_hash_clean(autoloader_class_autoload_functions);
		RETURN_TRUE;
	}

	if (!ZEND_FCC_INITIALIZED(fcc)) {
		/* Call trampoline has been cleared by zpp. Refetch it, because we want to deal
		 * with it ourselves. It is important that it is not refetched on every call,
		 * because calls may occur from different scopes. */
		zend_is_callable_ex(&fci.function_name, NULL, 0, NULL, &fcc, NULL);
	}
	Bucket *p = autoload_find_registered_function(autoloader_class_autoload_functions, &fcc);
	/* Release trampoline */
	zend_release_fcall_info_cache(&fcc);

	if (p) {
		zend_hash_del_bucket(autoloader_class_autoload_functions, p);
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

/* Try all registered class autoloader functions to load the requested class */
ZEND_FUNCTION(autoload_call_class)
{
	zend_string *class_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &class_name) == FAILURE) {
		RETURN_THROWS();
	}

	zend_string *lc_name = zend_string_tolower(class_name);
	zend_perform_class_autoload(class_name, lc_name);
	zend_string_release(lc_name);
}
/* Return all registered class autoloader functions */
ZEND_FUNCTION(autoload_list_class)
{
	autoload_list(INTERNAL_FUNCTION_PARAM_PASSTHRU, autoloader_class_autoload_functions);
}

/* Register given function as a function autoloader */
ZEND_FUNCTION(autoload_register_function)
{
	bool prepend = false;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_FUNC(fci, fcc)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(prepend)
	ZEND_PARSE_PARAMETERS_END();

	if (!autoloader_function_autoload_functions) {
		ALLOC_HASHTABLE(autoloader_function_autoload_functions);
		zend_hash_init(autoloader_function_autoload_functions, 1, NULL, zend_autoload_callback_zval_destroy, 0);
		/* Initialize as non-packed hash table for prepend functionality. */
		zend_hash_real_init_mixed(autoloader_function_autoload_functions);
	}

	if (!ZEND_FCC_INITIALIZED(fcc)) {
		/* Call trampoline has been cleared by zpp. Refetch it, because we want to deal
		 * with it ourselves. It is important that it is not refetched on every call,
		 * because calls may occur from different scopes. */
		zend_is_callable_ex(&fci.function_name, NULL, 0, NULL, &fcc, NULL);
	}

	if (fcc.function_handler->type == ZEND_INTERNAL_FUNCTION &&
		fcc.function_handler->internal_function.handler == zif_autoload_call_function) {
		zend_argument_value_error(1, "must not be the autoload_call_function() function");
		return;
	}

	/* If function is already registered, don't do anything */
	if (autoload_find_registered_function(autoloader_function_autoload_functions, &fcc)) {
		/* Release call trampoline */
		zend_release_fcall_info_cache(&fcc);
		return;
	}

	zend_fcall_info_cache *entry = emalloc(sizeof(zend_fcall_info_cache));
	zend_fcc_dup(entry, &fcc);
	zend_hash_next_index_insert_ptr(autoloader_function_autoload_functions, entry);
	if (prepend && zend_hash_num_elements(autoloader_function_autoload_functions) > 1) {
		/* Move the newly created element to the head of the hashtable */
		HT_MOVE_TAIL_TO_HEAD(autoloader_function_autoload_functions);
	}
}

ZEND_FUNCTION(autoload_unregister_function)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fci, fcc)
	ZEND_PARSE_PARAMETERS_END();

	if (!autoloader_function_autoload_functions) {
		RETURN_FALSE;
	}

	ZEND_ASSERT(autoloader_function_autoload_functions);

	if (!ZEND_FCC_INITIALIZED(fcc)) {
		/* Call trampoline has been cleared by zpp. Refetch it, because we want to deal
		 * with it ourselves. It is important that it is not refetched on every call,
		 * because calls may occur from different scopes. */
		zend_is_callable_ex(&fci.function_name, NULL, 0, NULL, &fcc, NULL);
	}
	Bucket *p = autoload_find_registered_function(autoloader_function_autoload_functions, &fcc);
	/* Release trampoline */
	zend_release_fcall_info_cache(&fcc);

	if (p) {
		zend_hash_del_bucket(autoloader_function_autoload_functions, p);
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

/* Try all registered function autoloader functions to load the requested function */
ZEND_FUNCTION(autoload_call_function)
{
	zend_string *function_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &function_name) == FAILURE) {
		RETURN_THROWS();
	}

	zend_string *lc_name = zend_string_tolower(function_name);
	zend_perform_function_autoload(function_name, lc_name);
	zend_string_release(lc_name);
}

/* Return all registered function autoloader functions */
ZEND_FUNCTION(autoload_list_function)
{
	autoload_list(INTERNAL_FUNCTION_PARAM_PASSTHRU, autoloader_function_autoload_functions);
}

void zend_autoload_shutdown(void)
{
	if (autoloader_class_autoload_functions) {
		zend_hash_destroy(autoloader_class_autoload_functions);
		FREE_HASHTABLE(autoloader_class_autoload_functions);
		autoloader_class_autoload_functions = NULL;
	}
	if (autoloader_function_autoload_functions) {
		zend_hash_destroy(autoloader_function_autoload_functions);
		FREE_HASHTABLE(autoloader_function_autoload_functions);
		autoloader_function_autoload_functions = NULL;
	}
}
