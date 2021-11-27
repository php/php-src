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

#define HT_MOVE_TAIL_TO_HEAD(ht) \
	do { \
		Bucket tmp = (ht)->arData[(ht)->nNumUsed-1]; \
		memmove((ht)->arData + 1, (ht)->arData, \
			sizeof(Bucket) * ((ht)->nNumUsed - 1)); \
		(ht)->arData[0] = tmp; \
		if (!((ht)->u.flags & HASH_FLAG_PACKED)) { \
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

static zend_always_inline bool zend_autoload_callback_equals(const zend_autoload_func* func, const zend_autoload_func* current)
{
	return func->fcc.function_handler == current->fcc.function_handler
		&& func->fcc.object == current->fcc.object
		&& func->fcc.calling_scope == current->fcc.calling_scope
	;
}

static zend_autoload_func *autoload_func_from_fci(zend_fcall_info *fci, zend_fcall_info_cache *fcc) {
	zend_autoload_func *entry = emalloc(sizeof(zend_autoload_func));
	memcpy(&entry->fci, fci, sizeof(zend_fcall_info));
	memcpy(&entry->fcc, fcc, sizeof(zend_fcall_info_cache));
	Z_TRY_ADDREF(entry->fci.function_name);
	return entry;
}

static void zend_autoload_callback_destroy(zend_autoload_func *entry)
{
	zend_release_fcall_info_cache(&entry->fcc);
	zend_fcall_info_args_clear(&entry->fci, true);
	zval_ptr_dtor(&entry->fci.function_name);
	efree(entry);
}

ZEND_API void zend_autoload_callback_zval_destroy(zval *element)
{
	zend_autoload_callback_destroy(Z_PTR_P(element));
}

static Bucket *autoload_find_registered_function(HashTable *autoloader_table, zend_autoload_func *function_entry)
{
	zend_autoload_func *current_function_entry;
	ZEND_HASH_MAP_FOREACH_PTR(autoloader_table, current_function_entry) {
		if (zend_autoload_callback_equals(current_function_entry, function_entry)) {
			return _p;
		}
	} ZEND_HASH_FOREACH_END();
	return NULL;
}

ZEND_API zend_class_entry *zend_perform_class_autoload(zend_string *class_name, zend_string *lc_name)
{
	zval zname;

	ZEND_ASSERT(&EG(autoloaders).class_autoload_functions);

	ZVAL_STR(&zname, class_name);

	HashTable *class_autoload_functions = &EG(autoloaders).class_autoload_functions;

	/* Cannot use ZEND_HASH_MAP_FOREACH_PTR here as autoloaders may be
	 * added/removed during autoloading. */
	HashPosition pos;
	zend_hash_internal_pointer_reset_ex(class_autoload_functions, &pos);
	while (1) {
		zend_autoload_func *func_info = zend_hash_get_current_data_ptr_ex(class_autoload_functions, &pos);
		if (!func_info) {
			break;
		}
		zval retval;

		func_info->fci.retval = &retval;
		zend_fcall_info_argn(&func_info->fci, 1, &zname);
		zend_call_function(&func_info->fci, &func_info->fcc);

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
	zval zname;

	ZEND_ASSERT(&EG(autoloaders).function_autoload_functions);

	ZVAL_STR(&zname, function_name);

	HashTable *function_autoload_functions = &EG(autoloaders).function_autoload_functions;

	/* Cannot use ZEND_HASH_MAP_FOREACH_PTR here as autoloaders may be
	 * added/removed during autoloading. */
	HashPosition pos;
	zend_hash_internal_pointer_reset_ex(function_autoload_functions, &pos);
	while (1) {
		zend_autoload_func *func_info = zend_hash_get_current_data_ptr_ex(function_autoload_functions, &pos);
		if (!func_info) {
			break;
		}
		zval retval;

		func_info->fci.retval = &retval;
		zend_fcall_info_argn(&func_info->fci, 1, &zname);
		zend_call_function(&func_info->fci, &func_info->fcc);

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
	zend_autoload_func *autoload_function_entry;

	ZEND_ASSERT(&EG(autoloaders).class_autoload_functions);

	if (!fcc->function_handler) {
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

	autoload_function_entry = autoload_func_from_fci(fci, fcc);

	/* If function is already registered, don't do anything */
	if (autoload_find_registered_function(&EG(autoloaders).class_autoload_functions, autoload_function_entry)) {
		zend_autoload_callback_destroy(autoload_function_entry);
		return;
	}

	zend_hash_next_index_insert_ptr(&EG(autoloaders).class_autoload_functions, autoload_function_entry);
	if (prepend && zend_hash_num_elements(&EG(autoloaders).class_autoload_functions) > 1) {
		/* Move the newly created element to the head of the hashtable */
		HT_MOVE_TAIL_TO_HEAD(&EG(autoloaders).class_autoload_functions);
	}
}

// TODO USERLAND FUNCTIONS, maybe namespace them?
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
	zend_autoload_func *autoload_function_entry;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fci, fcc)
	ZEND_PARSE_PARAMETERS_END();

	ZEND_ASSERT(&EG(autoloaders).class_autoload_functions);

	/* Why the fuck does this flush the autoloading table? This makes close to no sense.
	 * This is forward-ported from SPL */
	if (fcc.function_handler->type == ZEND_INTERNAL_FUNCTION &&
		fcc.function_handler->internal_function.handler == zif_autoload_call_class) {
		// Don't destroy the hash table, as we might be iterating over it right now.
		zend_hash_clean(&EG(autoloaders).class_autoload_functions);
		RETURN_TRUE;
	}

	autoload_function_entry = autoload_func_from_fci(&fci, &fcc);

	Bucket *p = autoload_find_registered_function(&EG(autoloaders).class_autoload_functions, autoload_function_entry);
	zend_autoload_callback_destroy(autoload_function_entry);

	if (p) {
		zend_hash_del_bucket(&EG(autoloaders).class_autoload_functions, p);
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
	zend_autoload_func *func_info;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	ZEND_HASH_FOREACH_PTR(&EG(autoloaders).class_autoload_functions, func_info) {
		if (Z_TYPE(func_info->fci.function_name) == IS_OBJECT) {
			add_next_index_zval(return_value, &func_info->fci.function_name);
		} else if (func_info->fcc.function_handler->common.scope) {
			zval tmp;

			array_init(&tmp);
			if (func_info->fcc.object) {
				add_next_index_object(&tmp, func_info->fcc.object);
			} else {
				add_next_index_str(&tmp, zend_string_copy(func_info->fcc.calling_scope->name));
			}
			add_next_index_str(&tmp, zend_string_copy(func_info->fcc.function_handler->common.function_name));
			add_next_index_zval(return_value, &tmp);
		} else {
			add_next_index_str(return_value, zend_string_copy(func_info->fcc.function_handler->common.function_name));
		}
	} ZEND_HASH_FOREACH_END();
}

/* Register given function as a function autoloader */
ZEND_FUNCTION(autoload_register_function)
{
	bool prepend = false;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_autoload_func *autoload_function_entry;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_FUNC(fci, fcc)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(prepend)
	ZEND_PARSE_PARAMETERS_END();

	ZEND_ASSERT(&EG(autoloaders).function_autoload_functions);

	if (!fcc.function_handler) {
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

	autoload_function_entry = autoload_func_from_fci(&fci, &fcc);

	/* If function is already registered, don't do anything */
	if (autoload_find_registered_function(&EG(autoloaders).function_autoload_functions, autoload_function_entry)) {
		zend_autoload_callback_destroy(autoload_function_entry);
		return;
	}

	zend_hash_next_index_insert_ptr(&EG(autoloaders).function_autoload_functions, autoload_function_entry);
	if (prepend && zend_hash_num_elements(&EG(autoloaders).function_autoload_functions) > 1) {
		/* Move the newly created element to the head of the hashtable */
		HT_MOVE_TAIL_TO_HEAD(&EG(autoloaders).function_autoload_functions);
	}
}

ZEND_FUNCTION(autoload_unregister_function)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_autoload_func *autoload_function_entry;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fci, fcc)
	ZEND_PARSE_PARAMETERS_END();

	ZEND_ASSERT(&EG(autoloaders).function_autoload_functions);

	autoload_function_entry = autoload_func_from_fci(&fci, &fcc);

	Bucket *p = autoload_find_registered_function(&EG(autoloaders).function_autoload_functions, autoload_function_entry);
	zend_autoload_callback_destroy(autoload_function_entry);

	if (p) {
		zend_hash_del_bucket(&EG(autoloaders).function_autoload_functions, p);
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
	zend_autoload_func *func_info;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	ZEND_HASH_FOREACH_PTR(&EG(autoloaders).function_autoload_functions, func_info) {
		if (Z_TYPE(func_info->fci.function_name) == IS_OBJECT) {
			add_next_index_zval(return_value, &func_info->fci.function_name);
		} else if (func_info->fcc.function_handler->common.scope) {
			zval tmp;

			array_init(&tmp);
			if (func_info->fcc.object) {
				add_next_index_object(&tmp, func_info->fcc.object);
			} else {
				add_next_index_str(&tmp, zend_string_copy(func_info->fcc.calling_scope->name));
			}
			add_next_index_str(&tmp, zend_string_copy(func_info->fcc.function_handler->common.function_name));
			add_next_index_zval(return_value, &tmp);
		} else {
			add_next_index_str(return_value, zend_string_copy(func_info->fcc.function_handler->common.function_name));
		}
	} ZEND_HASH_FOREACH_END();
}

void zend_autoload_shutdown(void)
{
	zend_hash_destroy(&EG(autoloaders.class_autoload_functions));
	zend_hash_destroy(&EG(autoloaders.function_autoload_functions));
}
