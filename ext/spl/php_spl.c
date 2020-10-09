/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_main.h"
#include "ext/standard/info.h"
#include "php_spl.h"
#include "php_spl_arginfo.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_array.h"
#include "spl_directory.h"
#include "spl_iterators.h"
#include "spl_exceptions.h"
#include "spl_observer.h"
#include "spl_dllist.h"
#include "spl_fixedarray.h"
#include "spl_heap.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "ext/standard/php_mt_rand.h"
#include "main/snprintf.h"

#ifdef COMPILE_DL_SPL
ZEND_GET_MODULE(spl)
#endif

ZEND_DECLARE_MODULE_GLOBALS(spl)

#define SPL_DEFAULT_FILE_EXTENSIONS ".inc,.php"

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(spl)
{
	spl_globals->autoload_extensions = NULL;
	spl_globals->autoload_functions = NULL;
}
/* }}} */

static zend_class_entry * spl_find_ce_by_name(zend_string *name, zend_bool autoload)
{
	zend_class_entry *ce;

	if (!autoload) {
		zend_string *lc_name = zend_string_tolower(name);

		ce = zend_hash_find_ptr(EG(class_table), lc_name);
		zend_string_release(lc_name);
	} else {
 		ce = zend_lookup_class(name);
 	}
 	if (ce == NULL) {
		php_error_docref(NULL, E_WARNING, "Class %s does not exist%s", ZSTR_VAL(name), autoload ? " and could not be loaded" : "");
		return NULL;
	}

	return ce;
}

/* {{{ Return an array containing the names of all parent classes */
PHP_FUNCTION(class_parents)
{
	zval *obj;
	zend_class_entry *parent_class, *ce;
	zend_bool autoload = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &obj, &autoload) == FAILURE) {
		RETURN_THROWS();
	}

	if (Z_TYPE_P(obj) != IS_OBJECT && Z_TYPE_P(obj) != IS_STRING) {
		zend_argument_type_error(1, "must be of type object|string, %s given", zend_zval_type_name(obj));
		RETURN_THROWS();
	}

	if (Z_TYPE_P(obj) == IS_STRING) {
		if (NULL == (ce = spl_find_ce_by_name(Z_STR_P(obj), autoload))) {
			RETURN_FALSE;
		}
	} else {
		ce = Z_OBJCE_P(obj);
	}

	array_init(return_value);
	parent_class = ce->parent;
	while (parent_class) {
		spl_add_class_name(return_value, parent_class, 0, 0);
		parent_class = parent_class->parent;
	}
}
/* }}} */

/* {{{ Return all classes and interfaces implemented by SPL */
PHP_FUNCTION(class_implements)
{
	zval *obj;
	zend_bool autoload = 1;
	zend_class_entry *ce;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &obj, &autoload) == FAILURE) {
		RETURN_THROWS();
	}
	if (Z_TYPE_P(obj) != IS_OBJECT && Z_TYPE_P(obj) != IS_STRING) {
		zend_argument_type_error(1, "must be of type object|string, %s given", zend_zval_type_name(obj));
		RETURN_THROWS();
	}

	if (Z_TYPE_P(obj) == IS_STRING) {
		if (NULL == (ce = spl_find_ce_by_name(Z_STR_P(obj), autoload))) {
			RETURN_FALSE;
		}
	} else {
		ce = Z_OBJCE_P(obj);
	}

	array_init(return_value);
	spl_add_interfaces(return_value, ce, 1, ZEND_ACC_INTERFACE);
}
/* }}} */

/* {{{ Return all traits used by a class. */
PHP_FUNCTION(class_uses)
{
	zval *obj;
	zend_bool autoload = 1;
	zend_class_entry *ce;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &obj, &autoload) == FAILURE) {
		RETURN_THROWS();
	}
	if (Z_TYPE_P(obj) != IS_OBJECT && Z_TYPE_P(obj) != IS_STRING) {
		zend_argument_type_error(1, "must be of type object|string, %s given", zend_zval_type_name(obj));
		RETURN_THROWS();
	}

	if (Z_TYPE_P(obj) == IS_STRING) {
		if (NULL == (ce = spl_find_ce_by_name(Z_STR_P(obj), autoload))) {
			RETURN_FALSE;
		}
	} else {
		ce = Z_OBJCE_P(obj);
	}

	array_init(return_value);
	spl_add_traits(return_value, ce, 1, ZEND_ACC_TRAIT);
}
/* }}} */

#define SPL_ADD_CLASS(class_name, z_list, sub, allow, ce_flags) \
	spl_add_classes(spl_ce_ ## class_name, z_list, sub, allow, ce_flags)

#define SPL_LIST_CLASSES(z_list, sub, allow, ce_flags) \
	SPL_ADD_CLASS(AppendIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(ArrayIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(ArrayObject, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(BadFunctionCallException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(BadMethodCallException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(CachingIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(CallbackFilterIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(DirectoryIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(DomainException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(EmptyIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(FilesystemIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(FilterIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(GlobIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(InfiniteIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(InvalidArgumentException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(IteratorIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(LengthException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(LimitIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(LogicException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(MultipleIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(NoRewindIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(OuterIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(OutOfBoundsException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(OutOfRangeException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(OverflowException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(ParentIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RangeException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RecursiveArrayIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RecursiveCachingIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RecursiveCallbackFilterIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RecursiveDirectoryIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RecursiveFilterIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RecursiveIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RecursiveIteratorIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RecursiveRegexIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RecursiveTreeIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RegexIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(RuntimeException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SeekableIterator, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplDoublyLinkedList, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplFileInfo, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplFileObject, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplFixedArray, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplHeap, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplMinHeap, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplMaxHeap, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplObjectStorage, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplObserver, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplPriorityQueue, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplQueue, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplStack, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplSubject, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(SplTempFileObject, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(UnderflowException, z_list, sub, allow, ce_flags); \
	SPL_ADD_CLASS(UnexpectedValueException, z_list, sub, allow, ce_flags); \

/* {{{ Return an array containing the names of all clsses and interfaces defined in SPL */
PHP_FUNCTION(spl_classes)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	SPL_LIST_CLASSES(return_value, 0, 0, 0)
}
/* }}} */

static int spl_autoload(zend_string *class_name, zend_string *lc_name, const char *ext, int ext_len) /* {{{ */
{
	char *class_file;
	int class_file_len;
	zval dummy;
	zend_file_handle file_handle;
	zend_op_array *new_op_array;
	zval result;
	int ret;

	class_file_len = (int)spprintf(&class_file, 0, "%s%.*s", ZSTR_VAL(lc_name), ext_len, ext);

#if DEFAULT_SLASH != '\\'
	{
		char *ptr = class_file;
		char *end = ptr + class_file_len;

		while ((ptr = memchr(ptr, '\\', (end - ptr))) != NULL) {
			*ptr = DEFAULT_SLASH;
		}
	}
#endif

	ret = php_stream_open_for_zend_ex(class_file, &file_handle, USE_PATH|STREAM_OPEN_FOR_INCLUDE);

	if (ret == SUCCESS) {
		zend_string *opened_path;
		if (!file_handle.opened_path) {
			file_handle.opened_path = zend_string_init(class_file, class_file_len, 0);
		}
		opened_path = zend_string_copy(file_handle.opened_path);
		ZVAL_NULL(&dummy);
		if (zend_hash_add(&EG(included_files), opened_path, &dummy)) {
			new_op_array = zend_compile_file(&file_handle, ZEND_REQUIRE);
			zend_destroy_file_handle(&file_handle);
		} else {
			new_op_array = NULL;
			zend_file_handle_dtor(&file_handle);
		}
		zend_string_release_ex(opened_path, 0);
		if (new_op_array) {
			ZVAL_UNDEF(&result);
			zend_execute(new_op_array, &result);

			destroy_op_array(new_op_array);
			efree(new_op_array);
			if (!EG(exception)) {
				zval_ptr_dtor(&result);
			}

			efree(class_file);
			return zend_hash_exists(EG(class_table), lc_name);
		}
	}
	efree(class_file);
	return 0;
} /* }}} */

/* {{{ Default autoloader implementation */
PHP_FUNCTION(spl_autoload)
{
	int pos_len, pos1_len;
	char *pos, *pos1;
	zend_string *class_name, *lc_name, *file_exts = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S!", &class_name, &file_exts) == FAILURE) {
		RETURN_THROWS();
	}

	if (!file_exts) {
		file_exts = SPL_G(autoload_extensions);
	}

	if (file_exts == NULL) { /* autoload_extensions is not initialized, set to defaults */
		pos = SPL_DEFAULT_FILE_EXTENSIONS;
		pos_len = sizeof(SPL_DEFAULT_FILE_EXTENSIONS) - 1;
	} else {
		pos = ZSTR_VAL(file_exts);
		pos_len = (int)ZSTR_LEN(file_exts);
	}

	lc_name = zend_string_tolower(class_name);
	while (pos && *pos && !EG(exception)) {
		pos1 = strchr(pos, ',');
		if (pos1) {
			pos1_len = (int)(pos1 - pos);
		} else {
			pos1_len = pos_len;
		}
		if (spl_autoload(class_name, lc_name, pos, pos1_len)) {
			break; /* loaded */
		}
		pos = pos1 ? pos1 + 1 : NULL;
		pos_len = pos1? pos_len - pos1_len - 1 : 0;
	}
	zend_string_release(lc_name);
} /* }}} */

/* {{{ Register and return default file extensions for spl_autoload */
PHP_FUNCTION(spl_autoload_extensions)
{
	zend_string *file_exts = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &file_exts) == FAILURE) {
		RETURN_THROWS();
	}

	if (file_exts) {
		if (SPL_G(autoload_extensions)) {
			zend_string_release_ex(SPL_G(autoload_extensions), 0);
		}
		SPL_G(autoload_extensions) = zend_string_copy(file_exts);
	}

	if (SPL_G(autoload_extensions) == NULL) {
		RETURN_STRINGL(SPL_DEFAULT_FILE_EXTENSIONS, sizeof(SPL_DEFAULT_FILE_EXTENSIONS) - 1);
	} else {
		zend_string_addref(SPL_G(autoload_extensions));
		RETURN_STR(SPL_G(autoload_extensions));
	}
} /* }}} */

typedef struct {
	zend_function *func_ptr;
	zend_object *obj;
	zend_object *closure;
	zend_class_entry *ce;
} autoload_func_info;

static void autoload_func_info_destroy(autoload_func_info *alfi) {
	if (alfi->obj) {
		zend_object_release(alfi->obj);
	}
	if (alfi->func_ptr &&
		UNEXPECTED(alfi->func_ptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
		zend_string_release_ex(alfi->func_ptr->common.function_name, 0);
		zend_free_trampoline(alfi->func_ptr);
	}
	if (alfi->closure) {
		zend_object_release(alfi->closure);
	}
	efree(alfi);
}

static void autoload_func_info_zval_dtor(zval *element)
{
	autoload_func_info_destroy(Z_PTR_P(element));
}

static autoload_func_info *autoload_func_info_from_fci(
		zend_fcall_info *fci, zend_fcall_info_cache *fcc) {
	autoload_func_info *alfi = emalloc(sizeof(autoload_func_info));
	alfi->ce = fcc->calling_scope;
	alfi->func_ptr = fcc->function_handler;
	alfi->obj = fcc->object;
	if (alfi->obj) {
		GC_ADDREF(alfi->obj);
	}
	if (Z_TYPE(fci->function_name) == IS_OBJECT) {
		alfi->closure = Z_OBJ(fci->function_name);
		GC_ADDREF(alfi->closure);
	} else {
		alfi->closure = NULL;
	}
	return alfi;
}

static zend_bool autoload_func_info_equals(
		const autoload_func_info *alfi1, const autoload_func_info *alfi2) {
	return alfi1->func_ptr == alfi2->func_ptr
		&& alfi1->obj == alfi2->obj
		&& alfi1->ce == alfi2->ce
		&& alfi1->closure == alfi2->closure;
}

static zend_class_entry *spl_perform_autoload(zend_string *class_name, zend_string *lc_name) {
	if (!SPL_G(autoload_functions)) {
		return NULL;
	}

	/* We don't use ZEND_HASH_FOREACH here,
	 * because autoloaders may be added/removed during autoloading. */
	HashPosition pos;
	zend_hash_internal_pointer_reset_ex(SPL_G(autoload_functions), &pos);
	while (1) {
		autoload_func_info *alfi =
			zend_hash_get_current_data_ptr_ex(SPL_G(autoload_functions), &pos);
		if (!alfi) {
			break;
		}

		zend_function *func = alfi->func_ptr;
		if (UNEXPECTED(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
			func = emalloc(sizeof(zend_op_array));
			memcpy(func, alfi->func_ptr, sizeof(zend_op_array));
			zend_string_addref(func->op_array.function_name);
		}

		zval param;
		ZVAL_STR(&param, class_name);
		zend_call_known_function(func, alfi->obj, alfi->ce, NULL, 1, &param, NULL);
		if (EG(exception)) {
			break;
		}

		zend_class_entry *ce = zend_hash_find_ptr(EG(class_table), lc_name);
		if (ce) {
			return ce;
		}

		zend_hash_move_forward_ex(SPL_G(autoload_functions), &pos);
	}
	return NULL;
}

/* {{{ Try all registered autoload function to load the requested class */
PHP_FUNCTION(spl_autoload_call)
{
	zend_string *class_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &class_name) == FAILURE) {
		RETURN_THROWS();
	}

	zend_string *lc_name = zend_string_tolower(class_name);
	spl_perform_autoload(class_name, lc_name);
	zend_string_release(lc_name);
} /* }}} */

#define HT_MOVE_TAIL_TO_HEAD(ht)						        \
	do {												        \
		Bucket tmp = (ht)->arData[(ht)->nNumUsed-1];				\
		memmove((ht)->arData + 1, (ht)->arData,					\
			sizeof(Bucket) * ((ht)->nNumUsed - 1));				\
		(ht)->arData[0] = tmp;									\
		zend_hash_rehash(ht);						        	\
	} while (0)

static Bucket *spl_find_registered_function(autoload_func_info *find_alfi) {
	if (!SPL_G(autoload_functions)) {
		return NULL;
	}

	autoload_func_info *alfi;
	ZEND_HASH_FOREACH_PTR(SPL_G(autoload_functions), alfi) {
		if (autoload_func_info_equals(alfi, find_alfi)) {
			return _p;
		}
	} ZEND_HASH_FOREACH_END();
	return NULL;
}

/* {{{ Register given function as autoloader */
PHP_FUNCTION(spl_autoload_register)
{
	zend_bool do_throw = 1;
	zend_bool prepend  = 0;
	zend_fcall_info fci = {0};
	zend_fcall_info_cache fcc;
	autoload_func_info *alfi;

	ZEND_PARSE_PARAMETERS_START(0, 3)
		Z_PARAM_OPTIONAL
		Z_PARAM_FUNC_OR_NULL(fci, fcc)
		Z_PARAM_BOOL(do_throw)
		Z_PARAM_BOOL(prepend)
	ZEND_PARSE_PARAMETERS_END();

	if (!do_throw) {
		php_error_docref(NULL, E_NOTICE, "Argument #2 ($do_throw) has been ignored, "
			"spl_autoload_register() will always throw");
	}

	if (!SPL_G(autoload_functions)) {
		ALLOC_HASHTABLE(SPL_G(autoload_functions));
		zend_hash_init(SPL_G(autoload_functions), 1, NULL, autoload_func_info_zval_dtor, 0);
		/* Initialize as non-packed hash table for prepend functionality. */
		zend_hash_real_init_mixed(SPL_G(autoload_functions));
	}

	/* If first arg is not null */
	if (ZEND_FCI_INITIALIZED(fci)) {
		if (!fcc.function_handler) {
			/* Call trampoline has been cleared by zpp. Refetch it, because we want to deal
			 * with it outselves. It is important that it is not refetched on every call,
			 * because calls may occur from different scopes. */
			zend_is_callable_ex(&fci.function_name, NULL, 0, NULL, &fcc, NULL);
		}

		if (fcc.function_handler->type == ZEND_INTERNAL_FUNCTION &&
			fcc.function_handler->internal_function.handler == zif_spl_autoload_call) {
			zend_argument_value_error(1, "must not be the spl_autoload_call() function");
			RETURN_THROWS();
		}

		alfi = autoload_func_info_from_fci(&fci, &fcc);
		if (UNEXPECTED(alfi->func_ptr == &EG(trampoline))) {
			zend_function *copy = emalloc(sizeof(zend_op_array));

			memcpy(copy, alfi->func_ptr, sizeof(zend_op_array));
			alfi->func_ptr->common.function_name = NULL;
			alfi->func_ptr = copy;
		}
	} else {
		alfi = emalloc(sizeof(autoload_func_info));
		alfi->func_ptr = zend_hash_str_find_ptr(
			CG(function_table), "spl_autoload", sizeof("spl_autoload") - 1);
		alfi->obj = NULL;
		alfi->ce = NULL;
		alfi->closure = NULL;
	}

	if (spl_find_registered_function(alfi)) {
		autoload_func_info_destroy(alfi);
		RETURN_TRUE;
	}

	zend_hash_next_index_insert_ptr(SPL_G(autoload_functions), alfi);
	if (prepend && SPL_G(autoload_functions)->nNumOfElements > 1) {
		/* Move the newly created element to the head of the hashtable */
		HT_MOVE_TAIL_TO_HEAD(SPL_G(autoload_functions));
	}

	RETURN_TRUE;
} /* }}} */

/* {{{ Unregister given function as autoloader */
PHP_FUNCTION(spl_autoload_unregister)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "f", &fci, &fcc) == FAILURE) {
		RETURN_THROWS();
	}

	if (fcc.function_handler && zend_string_equals_literal(
			fcc.function_handler->common.function_name, "spl_autoload_call")) {
		/* Don't destroy the hash table, as we might be iterating over it right now. */
		zend_hash_clean(SPL_G(autoload_functions));
		RETURN_TRUE;
	}

	autoload_func_info *alfi = autoload_func_info_from_fci(&fci, &fcc);
	Bucket *p = spl_find_registered_function(alfi);
	autoload_func_info_destroy(alfi);
	if (p) {
		zend_hash_del_bucket(SPL_G(autoload_functions), p);
		RETURN_TRUE;
	}

	RETURN_FALSE;
} /* }}} */

/* {{{ Return all registered autoloader functions */
PHP_FUNCTION(spl_autoload_functions)
{
	autoload_func_info *alfi;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);
	if (SPL_G(autoload_functions)) {
		ZEND_HASH_FOREACH_PTR(SPL_G(autoload_functions), alfi) {
			if (alfi->closure) {
				zval obj_zv;
				ZVAL_OBJ_COPY(&obj_zv, alfi->closure);
				add_next_index_zval(return_value, &obj_zv);
			} else if (alfi->func_ptr->common.scope) {
				zval tmp;

				array_init(&tmp);
				if (alfi->obj) {
					zval obj_zv;
					ZVAL_OBJ_COPY(&obj_zv, alfi->obj);
					add_next_index_zval(&tmp, &obj_zv);
				} else {
					add_next_index_str(&tmp, zend_string_copy(alfi->ce->name));
				}
				add_next_index_str(&tmp, zend_string_copy(alfi->func_ptr->common.function_name));
				add_next_index_zval(return_value, &tmp);
			} else {
				add_next_index_str(return_value, zend_string_copy(alfi->func_ptr->common.function_name));
			}
		} ZEND_HASH_FOREACH_END();
	}
} /* }}} */

/* {{{ Return hash id for given object */
PHP_FUNCTION(spl_object_hash)
{
	zval *obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &obj) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_NEW_STR(php_spl_object_hash(obj));
}
/* }}} */

/* {{{ Returns the integer object handle for the given object */
PHP_FUNCTION(spl_object_id)
{
	zval *obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT(obj)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG((zend_long)Z_OBJ_HANDLE_P(obj));
}
/* }}} */

PHPAPI zend_string *php_spl_object_hash(zval *obj) /* {{{*/
{
	intptr_t hash_handle, hash_handlers;

	if (!SPL_G(hash_mask_init)) {
		SPL_G(hash_mask_handle)   = (intptr_t)(php_mt_rand() >> 1);
		SPL_G(hash_mask_handlers) = (intptr_t)(php_mt_rand() >> 1);
		SPL_G(hash_mask_init) = 1;
	}

	hash_handle   = SPL_G(hash_mask_handle)^(intptr_t)Z_OBJ_HANDLE_P(obj);
	hash_handlers = SPL_G(hash_mask_handlers);

	return strpprintf(32, "%016zx%016zx", hash_handle, hash_handlers);
}
/* }}} */

static void spl_build_class_list_string(zval *entry, char **list) /* {{{ */
{
	char *res;

	spprintf(&res, 0, "%s, %s", *list, Z_STRVAL_P(entry));
	efree(*list);
	*list = res;
} /* }}} */

/* {{{ PHP_MINFO(spl) */
PHP_MINFO_FUNCTION(spl)
{
	zval list, *zv;
	char *strg;

	php_info_print_table_start();
	php_info_print_table_header(2, "SPL support",        "enabled");

	array_init(&list);
	SPL_LIST_CLASSES(&list, 0, 1, ZEND_ACC_INTERFACE)
	strg = estrdup("");
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(&list), zv) {
		spl_build_class_list_string(zv, &strg);
	} ZEND_HASH_FOREACH_END();
	zend_array_destroy(Z_ARR(list));
	php_info_print_table_row(2, "Interfaces", strg + 2);
	efree(strg);

	array_init(&list);
	SPL_LIST_CLASSES(&list, 0, -1, ZEND_ACC_INTERFACE)
	strg = estrdup("");
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(&list), zv) {
		spl_build_class_list_string(zv, &strg);
	} ZEND_HASH_FOREACH_END();
	zend_array_destroy(Z_ARR(list));
	php_info_print_table_row(2, "Classes", strg + 2);
	efree(strg);

	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl) */
PHP_MINIT_FUNCTION(spl)
{
	zend_autoload = spl_perform_autoload;

	PHP_MINIT(spl_exceptions)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(spl_iterators)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(spl_array)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(spl_directory)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(spl_dllist)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(spl_heap)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(spl_fixedarray)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(spl_observer)(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

PHP_RINIT_FUNCTION(spl) /* {{{ */
{
	SPL_G(autoload_extensions) = NULL;
	SPL_G(autoload_functions) = NULL;
	SPL_G(hash_mask_init) = 0;
	return SUCCESS;
} /* }}} */

PHP_RSHUTDOWN_FUNCTION(spl) /* {{{ */
{
	if (SPL_G(autoload_extensions)) {
		zend_string_release_ex(SPL_G(autoload_extensions), 0);
		SPL_G(autoload_extensions) = NULL;
	}
	if (SPL_G(autoload_functions)) {
		zend_hash_destroy(SPL_G(autoload_functions));
		FREE_HASHTABLE(SPL_G(autoload_functions));
		SPL_G(autoload_functions) = NULL;
	}
	if (SPL_G(hash_mask_init)) {
		SPL_G(hash_mask_init) = 0;
	}
	return SUCCESS;
} /* }}} */

/* {{{ spl_module_entry */
zend_module_entry spl_module_entry = {
	STANDARD_MODULE_HEADER,
	"SPL",
	ext_functions,
	PHP_MINIT(spl),
	NULL,
	PHP_RINIT(spl),
	PHP_RSHUTDOWN(spl),
	PHP_MINFO(spl),
	PHP_SPL_VERSION,
	PHP_MODULE_GLOBALS(spl),
	PHP_GINIT(spl),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */
