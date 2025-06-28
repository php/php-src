/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_main.h"
#include "ext/standard/info.h"
#include "php_spl.h"
#include "php_spl_arginfo.h"
#include "spl_functions.h"
#include "spl_array.h"
#include "spl_directory.h"
#include "spl_iterators.h"
#include "spl_exceptions.h"
#include "spl_observer.h"
#include "spl_dllist.h"
#include "spl_fixedarray.h"
#include "spl_heap.h"
#include "zend_autoload.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"

ZEND_TLS zend_string *spl_autoload_extensions;

#define SPL_DEFAULT_FILE_EXTENSIONS ".inc,.php"

static zend_class_entry * spl_find_ce_by_name(zend_string *name, bool autoload)
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
	bool autoload = true;

	/* We do not use Z_PARAM_OBJ_OR_STR here to be able to exclude int, float, and bool which are bogus class names */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &obj, &autoload) == FAILURE) {
		RETURN_THROWS();
	}

	if (Z_TYPE_P(obj) != IS_OBJECT && Z_TYPE_P(obj) != IS_STRING) {
		zend_argument_type_error(1, "must be of type object|string, %s given", zend_zval_value_name(obj));
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
	bool autoload = true;
	zend_class_entry *ce;

	/* We do not use Z_PARAM_OBJ_OR_STR here to be able to exclude int, float, and bool which are bogus class names */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &obj, &autoload) == FAILURE) {
		RETURN_THROWS();
	}
	if (Z_TYPE_P(obj) != IS_OBJECT && Z_TYPE_P(obj) != IS_STRING) {
		zend_argument_type_error(1, "must be of type object|string, %s given", zend_zval_value_name(obj));
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
	bool autoload = true;
	zend_class_entry *ce;

	/* We do not use Z_PARAM_OBJ_OR_STR here to be able to exclude int, float, and bool which are bogus class names */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &obj, &autoload) == FAILURE) {
		RETURN_THROWS();
	}
	if (Z_TYPE_P(obj) != IS_OBJECT && Z_TYPE_P(obj) != IS_STRING) {
		zend_argument_type_error(1, "must be of type object|string, %s given", zend_zval_value_name(obj));
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

/* {{{ Return an array containing the names of all classes and interfaces defined in SPL */
PHP_FUNCTION(spl_classes)
{
	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	SPL_LIST_CLASSES(return_value, 0, 0, 0)
}
/* }}} */

static bool spl_autoload(zend_string *lc_name, const char *ext, size_t ext_len) /* {{{ */
{
	zend_string *class_file;
	zval dummy;
	zend_file_handle file_handle;
	zval result;

	class_file = zend_string_concat2(ZSTR_VAL(lc_name), ZSTR_LEN(lc_name), ext, ext_len);

#if DEFAULT_SLASH != '\\'
	{
		char *ptr = ZSTR_VAL(class_file);
		const char *end = ptr + ZSTR_LEN(class_file);

		while ((ptr = memchr(ptr, '\\', (end - ptr))) != NULL) {
			*ptr = DEFAULT_SLASH;
		}
	}
#endif

	bool ret = false;
	zend_stream_init_filename_ex(&file_handle, class_file);
	if (php_stream_open_for_zend_ex(&file_handle, USE_PATH|STREAM_OPEN_FOR_INCLUDE) == SUCCESS) {
		zend_string *opened_path;
		if (!file_handle.opened_path) {
			file_handle.opened_path = zend_string_copy(class_file);
		}
		opened_path = zend_string_copy(file_handle.opened_path);
		ZVAL_NULL(&dummy);
		zend_op_array *new_op_array = NULL;
		if (zend_hash_add(&EG(included_files), opened_path, &dummy)) {
			new_op_array = zend_compile_file(&file_handle, ZEND_REQUIRE);
		}
		zend_string_release_ex(opened_path, false);
		if (new_op_array) {
			uint32_t orig_jit_trace_num = EG(jit_trace_num);

			ZVAL_UNDEF(&result);
			zend_execute(new_op_array, &result);
			EG(jit_trace_num) = orig_jit_trace_num;

			destroy_op_array(new_op_array);
			efree(new_op_array);
			zval_ptr_dtor(&result);

			ret = zend_hash_exists(EG(class_table), lc_name);
		}
	}
	zend_destroy_file_handle(&file_handle);
	zend_string_release(class_file);
	return ret;
} /* }}} */

/* {{{ Default autoloader implementation */
PHP_FUNCTION(spl_autoload)
{
	size_t pos_len, pos1_len;
	char *pos, *pos1;
	zend_string *class_name, *lc_name, *file_exts = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S!", &class_name, &file_exts) == FAILURE) {
		RETURN_THROWS();
	}

	if (!file_exts) {
		file_exts = spl_autoload_extensions;
	}

	if (file_exts == NULL) { /* autoload_extensions is not initialized, set to defaults */
		pos = SPL_DEFAULT_FILE_EXTENSIONS;
		pos_len = sizeof(SPL_DEFAULT_FILE_EXTENSIONS) - 1;
	} else {
		pos = ZSTR_VAL(file_exts);
		pos_len = ZSTR_LEN(file_exts);
	}

	lc_name = zend_string_tolower(class_name);
	while (pos && *pos && !EG(exception)) {
		pos1 = strchr(pos, ',');
		if (pos1) {
			pos1_len = (size_t)(pos1 - pos);
		} else {
			pos1_len = pos_len;
		}
		if (spl_autoload(lc_name, pos, pos1_len)) {
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
		if (spl_autoload_extensions) {
			zend_string_release_ex(spl_autoload_extensions, 0);
		}
		spl_autoload_extensions = zend_string_copy(file_exts);
	}

	if (spl_autoload_extensions == NULL) {
		RETURN_STRINGL(SPL_DEFAULT_FILE_EXTENSIONS, sizeof(SPL_DEFAULT_FILE_EXTENSIONS) - 1);
	} else {
		zend_string_addref(spl_autoload_extensions);
		RETURN_STR(spl_autoload_extensions);
	}
} /* }}} */

/* {{{ Try all registered autoload function to load the requested class */
PHP_FUNCTION(spl_autoload_call)
{
	zend_string *class_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &class_name) == FAILURE) {
		RETURN_THROWS();
	}

	zend_string *lc_name = zend_string_tolower(class_name);
	zend_perform_class_autoload(class_name, lc_name);
	zend_string_release(lc_name);
} /* }}} */

/* {{{ Register given function as autoloader */
PHP_FUNCTION(spl_autoload_register)
{
	bool do_throw = 1;
	bool prepend  = 0;
	zend_fcall_info fci = {0};
	zend_fcall_info_cache fcc;

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

	/* If first arg is not null */
	if (ZEND_FCI_INITIALIZED(fci)) {
		if (!ZEND_FCC_INITIALIZED(fcc)) {
			/* Call trampoline has been cleared by zpp. Refetch it, because we want to deal
			 * with it ourselves. It is important that it is not refetched on every call,
			 * because calls may occur from different scopes. */
			zend_is_callable_ex(&fci.function_name, NULL, IS_CALLABLE_SUPPRESS_DEPRECATIONS, NULL, &fcc, NULL);
		}

		if (fcc.function_handler->type == ZEND_INTERNAL_FUNCTION &&
			fcc.function_handler->internal_function.handler == zif_spl_autoload_call) {
			zend_argument_value_error(1, "must not be the spl_autoload_call() function");
			RETURN_THROWS();
		}
	} else {
		memset(&fcc, 0, sizeof(fcc));
		fcc.function_handler = zend_hash_str_find_ptr(CG(function_table), ZEND_STRL("spl_autoload"));
	}

	zend_autoload_register_class_loader(&fcc, prepend);

	RETURN_TRUE;
} /* }}} */

/* {{{ Unregister given function as autoloader */
PHP_FUNCTION(spl_autoload_unregister)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC_NO_TRAMPOLINE_FREE(fci, fcc)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_string_equals_literal(fcc.function_handler->common.function_name, "spl_autoload_call")) {
		/* Release trampoline */
		zend_release_fcall_info_cache(&fcc);
		php_error_docref(NULL, E_DEPRECATED,
			"Using spl_autoload_call() as a callback for spl_autoload_unregister() is deprecated,"
			" to remove all registered autoloaders, call spl_autoload_unregister()"
			" for all values returned from spl_autoload_functions()");
		if (UNEXPECTED(EG(exception))) {
			RETURN_THROWS();
		}
		zend_autoload_clean_class_loaders();
		RETURN_TRUE;
	}

	RETVAL_BOOL(zend_autoload_unregister_class_loader(&fcc));
	/* Release trampoline */
	zend_release_fcall_info_cache(&fcc);
} /* }}} */

/* {{{ Return all registered autoloader functions */
PHP_FUNCTION(spl_autoload_functions)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_autoload_fcc_map_to_callable_zval_map(return_value);
} /* }}} */

/* {{{ Return hash id for given object */
PHP_FUNCTION(spl_object_hash)
{
	zend_object *obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ(obj)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_NEW_STR(php_spl_object_hash(obj));
}
/* }}} */

/* {{{ Returns the integer object handle for the given object */
PHP_FUNCTION(spl_object_id)
{
	zend_object *obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ(obj)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG((zend_long)obj->handle);
}
/* }}} */

PHPAPI zend_string *php_spl_object_hash(zend_object *obj) /* {{{*/
{
	return strpprintf(32, "%016zx0000000000000000", (intptr_t)obj->handle);
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
	php_info_print_table_row(2, "SPL support", "enabled");

	array_init(&list);
	SPL_LIST_CLASSES(&list, 0, 1, ZEND_ACC_INTERFACE)
	strg = estrdup("");
	ZEND_HASH_MAP_FOREACH_VAL(Z_ARRVAL_P(&list), zv) {
		spl_build_class_list_string(zv, &strg);
	} ZEND_HASH_FOREACH_END();
	zend_array_destroy(Z_ARR(list));
	php_info_print_table_row(2, "Interfaces", strg + 2);
	efree(strg);

	array_init(&list);
	SPL_LIST_CLASSES(&list, 0, -1, ZEND_ACC_INTERFACE)
	strg = estrdup("");
	ZEND_HASH_MAP_FOREACH_VAL(Z_ARRVAL_P(&list), zv) {
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
	spl_autoload_extensions = NULL;
	return SUCCESS;
} /* }}} */

PHP_RSHUTDOWN_FUNCTION(spl) /* {{{ */
{
	if (spl_autoload_extensions) {
		zend_string_release_ex(spl_autoload_extensions, 0);
		spl_autoload_extensions = NULL;
	}
	return SUCCESS;
} /* }}} */

static const zend_module_dep spl_deps[] = {
	ZEND_MOD_REQUIRED("json")
	ZEND_MOD_END
};

zend_module_entry spl_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	spl_deps,
	"SPL",
	ext_functions,
	PHP_MINIT(spl),
	NULL,
	PHP_RINIT(spl),
	PHP_RSHUTDOWN(spl),
	PHP_MINFO(spl),
	PHP_SPL_VERSION,
	STANDARD_MODULE_PROPERTIES
};
