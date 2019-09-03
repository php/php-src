/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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

static zend_function *spl_autoload_fn = NULL;
static zend_function *spl_autoload_call_fn = NULL;

/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(spl)
{
	spl_globals->autoload_extensions     = NULL;
	spl_globals->autoload_functions      = NULL;
	spl_globals->autoload_running        = 0;
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

/* {{{ proto array class_parents(object instance [, bool autoload = true])
 Return an array containing the names of all parent classes */
PHP_FUNCTION(class_parents)
{
	zval *obj;
	zend_class_entry *parent_class, *ce;
	zend_bool autoload = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &obj, &autoload) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(obj) != IS_OBJECT && Z_TYPE_P(obj) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "object or string expected");
		RETURN_FALSE;
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

/* {{{ proto array class_implements(mixed what [, bool autoload ])
 Return all classes and interfaces implemented by SPL */
PHP_FUNCTION(class_implements)
{
	zval *obj;
	zend_bool autoload = 1;
	zend_class_entry *ce;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &obj, &autoload) == FAILURE) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(obj) != IS_OBJECT && Z_TYPE_P(obj) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "object or string expected");
		RETURN_FALSE;
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

/* {{{ proto array class_uses(mixed what [, bool autoload ])
 Return all traits used by a class. */
PHP_FUNCTION(class_uses)
{
	zval *obj;
	zend_bool autoload = 1;
	zend_class_entry *ce;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &obj, &autoload) == FAILURE) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(obj) != IS_OBJECT && Z_TYPE_P(obj) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "object or string expected");
		RETURN_FALSE;
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

/* {{{ proto array spl_classes()
 Return an array containing the names of all clsses and interfaces defined in SPL */
PHP_FUNCTION(spl_classes)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
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

/* {{{ proto void spl_autoload(string class_name [, string file_extensions])
 Default implementation for __autoload() */
PHP_FUNCTION(spl_autoload)
{
	int pos_len, pos1_len;
	char *pos, *pos1;
	zend_string *class_name, *lc_name, *file_exts = SPL_G(autoload_extensions);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &class_name, &file_exts) == FAILURE) {
		RETURN_FALSE;
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

/* {{{ proto string spl_autoload_extensions([string file_extensions])
 Register and return default file extensions for spl_autoload */
PHP_FUNCTION(spl_autoload_extensions)
{
	zend_string *file_exts = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &file_exts) == FAILURE) {
		return;
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
	zval obj;
	zval closure;
	zend_class_entry *ce;
} autoload_func_info;

static void autoload_func_info_dtor(zval *element)
{
	autoload_func_info *alfi = (autoload_func_info*)Z_PTR_P(element);
	if (!Z_ISUNDEF(alfi->obj)) {
		zval_ptr_dtor(&alfi->obj);
	}
	if (alfi->func_ptr &&
		UNEXPECTED(alfi->func_ptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
		zend_string_release_ex(alfi->func_ptr->common.function_name, 0);
		zend_free_trampoline(alfi->func_ptr);
	}
	if (!Z_ISUNDEF(alfi->closure)) {
		zval_ptr_dtor(&alfi->closure);
	}
	efree(alfi);
}

/* {{{ proto void spl_autoload_call(string class_name)
 Try all registered autoload function to load the requested class */
PHP_FUNCTION(spl_autoload_call)
{
	zval *class_name, retval;
	zend_string *lc_name, *func_name;
	autoload_func_info *alfi;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &class_name) == FAILURE || Z_TYPE_P(class_name) != IS_STRING) {
		return;
	}

	if (SPL_G(autoload_functions)) {
		HashPosition pos;
		zend_ulong num_idx;
		zend_function *func;
		zend_fcall_info fci;
		zend_fcall_info_cache fcic;
		zend_class_entry *called_scope = zend_get_called_scope(execute_data);
		int l_autoload_running = SPL_G(autoload_running);

		SPL_G(autoload_running) = 1;
		lc_name = zend_string_tolower(Z_STR_P(class_name));

		fci.size = sizeof(fci);
		fci.retval = &retval;
		fci.param_count = 1;
		fci.params = class_name;
		fci.no_separation = 1;

		ZVAL_UNDEF(&fci.function_name); /* Unused */

		zend_hash_internal_pointer_reset_ex(SPL_G(autoload_functions), &pos);
		while (zend_hash_get_current_key_ex(SPL_G(autoload_functions), &func_name, &num_idx, &pos) == HASH_KEY_IS_STRING) {
			alfi = zend_hash_get_current_data_ptr_ex(SPL_G(autoload_functions), &pos);
			func = alfi->func_ptr;
			if (UNEXPECTED(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
				func = emalloc(sizeof(zend_op_array));
				memcpy(func, alfi->func_ptr, sizeof(zend_op_array));
				zend_string_addref(func->op_array.function_name);
			}
			ZVAL_UNDEF(&retval);
			fcic.function_handler = func;
			if (Z_ISUNDEF(alfi->obj)) {
				fci.object = NULL;
				fcic.object = NULL;
				if (alfi->ce &&
				    (!called_scope ||
				     !instanceof_function(called_scope, alfi->ce))) {
					fcic.called_scope = alfi->ce;
				} else {
					fcic.called_scope = called_scope;
				}
			} else {
				fci.object = Z_OBJ(alfi->obj);
				fcic.object = Z_OBJ(alfi->obj);
				fcic.called_scope = Z_OBJCE(alfi->obj);
			}

			zend_call_function(&fci, &fcic);
			zval_ptr_dtor(&retval);

			if (EG(exception)) {
				break;
			}

			if (pos + 1 == SPL_G(autoload_functions)->nNumUsed ||
			    zend_hash_exists(EG(class_table), lc_name)) {
				break;
			}
			zend_hash_move_forward_ex(SPL_G(autoload_functions), &pos);
		}
		zend_string_release_ex(lc_name, 0);
		SPL_G(autoload_running) = l_autoload_running;
	} else {
		/* do not use or overwrite &EG(autoload_func) here */
		zend_fcall_info fcall_info;
		zend_fcall_info_cache fcall_cache;

		ZVAL_UNDEF(&retval);

		fcall_info.size = sizeof(fcall_info);
		ZVAL_UNDEF(&fcall_info.function_name);
		fcall_info.retval = &retval;
		fcall_info.param_count = 1;
		fcall_info.params = class_name;
		fcall_info.object = NULL;
		fcall_info.no_separation = 1;

		fcall_cache.function_handler = spl_autoload_fn;
		fcall_cache.called_scope = NULL;
		fcall_cache.object = NULL;

		zend_call_function(&fcall_info, &fcall_cache);
		zval_ptr_dtor(&retval);
	}
} /* }}} */

#define HT_MOVE_TAIL_TO_HEAD(ht)						        \
	do {												        \
		Bucket tmp = (ht)->arData[(ht)->nNumUsed-1];				\
		memmove((ht)->arData + 1, (ht)->arData,					\
			sizeof(Bucket) * ((ht)->nNumUsed - 1));				\
		(ht)->arData[0] = tmp;									\
		zend_hash_rehash(ht);						        	\
	} while (0)

/* {{{ proto bool spl_autoload_register([mixed autoload_function [, bool throw [, bool prepend]]])
 Register given function as __autoload() implementation */
PHP_FUNCTION(spl_autoload_register)
{
	zend_string *func_name;
	char *error = NULL;
	zend_string *lc_name;
	zval *zcallable = NULL;
	zend_bool do_throw = 1;
	zend_bool prepend  = 0;
	zend_function *spl_func_ptr;
	autoload_func_info alfi;
	zend_object *obj_ptr;
	zend_fcall_info_cache fcc;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "|zbb", &zcallable, &do_throw, &prepend) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		if (!zend_is_callable_ex(zcallable, NULL, IS_CALLABLE_STRICT, &func_name, &fcc, &error)) {
			alfi.ce = fcc.calling_scope;
			alfi.func_ptr = fcc.function_handler;
			obj_ptr = fcc.object;
			if (Z_TYPE_P(zcallable) == IS_ARRAY) {
				if (!obj_ptr && alfi.func_ptr && !(alfi.func_ptr->common.fn_flags & ZEND_ACC_STATIC)) {
					if (do_throw) {
						zend_throw_exception_ex(spl_ce_LogicException, 0, "Passed array specifies a non static method but no object (%s)", error);
					}
					if (error) {
						efree(error);
					}
					zend_string_release_ex(func_name, 0);
					RETURN_FALSE;
				} else if (do_throw) {
					zend_throw_exception_ex(spl_ce_LogicException, 0, "Passed array does not specify %s %smethod (%s)", alfi.func_ptr ? "a callable" : "an existing", !obj_ptr ? "static " : "", error);
				}
				if (error) {
					efree(error);
				}
				zend_string_release_ex(func_name, 0);
				RETURN_FALSE;
			} else if (Z_TYPE_P(zcallable) == IS_STRING) {
				if (do_throw) {
					zend_throw_exception_ex(spl_ce_LogicException, 0, "Function '%s' not %s (%s)", ZSTR_VAL(func_name), alfi.func_ptr ? "callable" : "found", error);
				}
				if (error) {
					efree(error);
				}
				zend_string_release_ex(func_name, 0);
				RETURN_FALSE;
			} else {
				if (do_throw) {
					zend_throw_exception_ex(spl_ce_LogicException, 0, "Illegal value passed (%s)", error);
				}
				if (error) {
					efree(error);
				}
				zend_string_release_ex(func_name, 0);
				RETURN_FALSE;
			}
		} else if (fcc.function_handler->type == ZEND_INTERNAL_FUNCTION &&
				   fcc.function_handler->internal_function.handler == zif_spl_autoload_call) {
			if (do_throw) {
				zend_throw_exception_ex(spl_ce_LogicException, 0, "Function spl_autoload_call() cannot be registered");
			}
			if (error) {
				efree(error);
			}
			zend_string_release_ex(func_name, 0);
			RETURN_FALSE;
		}
		alfi.ce = fcc.calling_scope;
		alfi.func_ptr = fcc.function_handler;
		obj_ptr = fcc.object;
		if (error) {
			efree(error);
		}

		if (Z_TYPE_P(zcallable) == IS_OBJECT) {
			ZVAL_COPY(&alfi.closure, zcallable);

			lc_name = zend_string_alloc(ZSTR_LEN(func_name) + sizeof(uint32_t), 0);
			zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(func_name), ZSTR_LEN(func_name));
			memcpy(ZSTR_VAL(lc_name) + ZSTR_LEN(func_name), &Z_OBJ_HANDLE_P(zcallable), sizeof(uint32_t));
			ZSTR_VAL(lc_name)[ZSTR_LEN(lc_name)] = '\0';
		} else {
			ZVAL_UNDEF(&alfi.closure);
			/* Skip leading \ */
			if (ZSTR_VAL(func_name)[0] == '\\') {
				lc_name = zend_string_alloc(ZSTR_LEN(func_name) - 1, 0);
				zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(func_name) + 1, ZSTR_LEN(func_name) - 1);
			} else {
				lc_name = zend_string_tolower(func_name);
			}
		}
		zend_string_release_ex(func_name, 0);

		if (SPL_G(autoload_functions) && zend_hash_exists(SPL_G(autoload_functions), lc_name)) {
			if (!Z_ISUNDEF(alfi.closure)) {
				Z_DELREF_P(&alfi.closure);
			}
			goto skip;
		}

		if (obj_ptr && !(alfi.func_ptr->common.fn_flags & ZEND_ACC_STATIC)) {
			/* add object id to the hash to ensure uniqueness, for more reference look at bug #40091 */
			lc_name = zend_string_extend(lc_name, ZSTR_LEN(lc_name) + sizeof(uint32_t), 0);
			memcpy(ZSTR_VAL(lc_name) + ZSTR_LEN(lc_name) - sizeof(uint32_t), &obj_ptr->handle, sizeof(uint32_t));
			ZSTR_VAL(lc_name)[ZSTR_LEN(lc_name)] = '\0';
			ZVAL_OBJ(&alfi.obj, obj_ptr);
			Z_ADDREF(alfi.obj);
		} else {
			ZVAL_UNDEF(&alfi.obj);
		}

		if (!SPL_G(autoload_functions)) {
			ALLOC_HASHTABLE(SPL_G(autoload_functions));
			zend_hash_init(SPL_G(autoload_functions), 1, NULL, autoload_func_info_dtor, 0);
		}

		spl_func_ptr = spl_autoload_fn;

		if (EG(autoload_func) == spl_func_ptr) { /* registered already, so we insert that first */
			autoload_func_info spl_alfi;

			spl_alfi.func_ptr = spl_func_ptr;
			ZVAL_UNDEF(&spl_alfi.obj);
			ZVAL_UNDEF(&spl_alfi.closure);
			spl_alfi.ce = NULL;
			zend_hash_add_mem(SPL_G(autoload_functions), spl_autoload_fn->common.function_name,
					&spl_alfi, sizeof(autoload_func_info));
			if (prepend && SPL_G(autoload_functions)->nNumOfElements > 1) {
				/* Move the newly created element to the head of the hashtable */
				HT_MOVE_TAIL_TO_HEAD(SPL_G(autoload_functions));
			}
		}

		if (UNEXPECTED(alfi.func_ptr == &EG(trampoline))) {
			zend_function *copy = emalloc(sizeof(zend_op_array));

			memcpy(copy, alfi.func_ptr, sizeof(zend_op_array));
			alfi.func_ptr->common.function_name = NULL;
			alfi.func_ptr = copy;
		}
		if (zend_hash_add_mem(SPL_G(autoload_functions), lc_name, &alfi, sizeof(autoload_func_info)) == NULL) {
			if (obj_ptr && !(alfi.func_ptr->common.fn_flags & ZEND_ACC_STATIC)) {
				Z_DELREF(alfi.obj);
			}
			if (!Z_ISUNDEF(alfi.closure)) {
				Z_DELREF(alfi.closure);
			}
			if (UNEXPECTED(alfi.func_ptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
				zend_string_release_ex(alfi.func_ptr->common.function_name, 0);
				zend_free_trampoline(alfi.func_ptr);
			}
		}
		if (prepend && SPL_G(autoload_functions)->nNumOfElements > 1) {
			/* Move the newly created element to the head of the hashtable */
			HT_MOVE_TAIL_TO_HEAD(SPL_G(autoload_functions));
		}
skip:
		zend_string_release_ex(lc_name, 0);
	}

	if (SPL_G(autoload_functions)) {
		EG(autoload_func) = spl_autoload_call_fn;
	} else {
		EG(autoload_func) = spl_autoload_fn;
	}

	RETURN_TRUE;
} /* }}} */

/* {{{ proto bool spl_autoload_unregister(mixed autoload_function)
 Unregister given function as __autoload() implementation */
PHP_FUNCTION(spl_autoload_unregister)
{
	zend_string *func_name = NULL;
	char *error = NULL;
	zend_string *lc_name;
	zval *zcallable;
	int success = FAILURE;
	zend_function *spl_func_ptr;
	zend_object *obj_ptr;
	zend_fcall_info_cache fcc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zcallable) == FAILURE) {
		return;
	}

	if (!zend_is_callable_ex(zcallable, NULL, IS_CALLABLE_CHECK_SYNTAX_ONLY, &func_name, &fcc, &error)) {
		zend_throw_exception_ex(spl_ce_LogicException, 0, "Unable to unregister invalid function (%s)", error);
		if (error) {
			efree(error);
		}
		if (func_name) {
			zend_string_release_ex(func_name, 0);
		}
		RETURN_FALSE;
	}
	obj_ptr = fcc.object;
	if (error) {
		efree(error);
	}

	if (Z_TYPE_P(zcallable) == IS_OBJECT) {
		lc_name = zend_string_alloc(ZSTR_LEN(func_name) + sizeof(uint32_t), 0);
		zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(func_name), ZSTR_LEN(func_name));
		memcpy(ZSTR_VAL(lc_name) + ZSTR_LEN(func_name), &Z_OBJ_HANDLE_P(zcallable), sizeof(uint32_t));
		ZSTR_VAL(lc_name)[ZSTR_LEN(lc_name)] = '\0';
	} else {
		/* Skip leading \ */
		if (ZSTR_VAL(func_name)[0] == '\\') {
			lc_name = zend_string_alloc(ZSTR_LEN(func_name) - 1, 0);
			zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(func_name) + 1, ZSTR_LEN(func_name) - 1);
		} else {
			lc_name = zend_string_tolower(func_name);
		}
	}
	zend_string_release_ex(func_name, 0);

	if (SPL_G(autoload_functions)) {
		if (zend_string_equals(lc_name, spl_autoload_call_fn->common.function_name)) {
			/* remove all */
			if (!SPL_G(autoload_running)) {
				zend_hash_destroy(SPL_G(autoload_functions));
				FREE_HASHTABLE(SPL_G(autoload_functions));
				SPL_G(autoload_functions) = NULL;
				EG(autoload_func) = NULL;
			} else {
				zend_hash_clean(SPL_G(autoload_functions));
			}
			success = SUCCESS;
		} else {
			/* remove specific */
			success = zend_hash_del(SPL_G(autoload_functions), lc_name);
			if (success != SUCCESS && obj_ptr) {
				lc_name = zend_string_extend(lc_name, ZSTR_LEN(lc_name) + sizeof(uint32_t), 0);
				memcpy(ZSTR_VAL(lc_name) + ZSTR_LEN(lc_name) - sizeof(uint32_t), &obj_ptr->handle, sizeof(uint32_t));
				ZSTR_VAL(lc_name)[ZSTR_LEN(lc_name)] = '\0';
				success = zend_hash_del(SPL_G(autoload_functions), lc_name);
			}
		}
	} else if (zend_string_equals(lc_name, spl_autoload_fn->common.function_name)) {
		/* register single spl_autoload() */
		spl_func_ptr = spl_autoload_fn;

		if (EG(autoload_func) == spl_func_ptr) {
			success = SUCCESS;
			EG(autoload_func) = NULL;
		}
	}

	zend_string_release_ex(lc_name, 0);
	RETURN_BOOL(success == SUCCESS);
} /* }}} */

/* {{{ proto false|array spl_autoload_functions()
 Return all registered __autoload() functionns */
PHP_FUNCTION(spl_autoload_functions)
{
	zend_function *fptr;
	autoload_func_info *alfi;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!EG(autoload_func)) {
		if ((fptr = zend_hash_find_ptr(EG(function_table), ZSTR_KNOWN(ZEND_STR_MAGIC_AUTOLOAD)))) {
			zval tmp;

			array_init(return_value);
			ZVAL_STR_COPY(&tmp, ZSTR_KNOWN(ZEND_STR_MAGIC_AUTOLOAD));
			zend_hash_next_index_insert_new(Z_ARR_P(return_value), &tmp);
			return;
		}
		RETURN_FALSE;
	}

	fptr = spl_autoload_call_fn;

	if (EG(autoload_func) == fptr) {
		zend_string *key;
		array_init(return_value);
		ZEND_HASH_FOREACH_STR_KEY_PTR(SPL_G(autoload_functions), key, alfi) {
			if (!Z_ISUNDEF(alfi->closure)) {
				Z_ADDREF(alfi->closure);
				add_next_index_zval(return_value, &alfi->closure);
			} else if (alfi->func_ptr->common.scope) {
				zval tmp;

				array_init(&tmp);
				if (!Z_ISUNDEF(alfi->obj)) {
					Z_ADDREF(alfi->obj);
					add_next_index_zval(&tmp, &alfi->obj);
				} else {
					add_next_index_str(&tmp, zend_string_copy(alfi->ce->name));
				}
				add_next_index_str(&tmp, zend_string_copy(alfi->func_ptr->common.function_name));
				add_next_index_zval(return_value, &tmp);
			} else {
				if (strncmp(ZSTR_VAL(alfi->func_ptr->common.function_name), "__lambda_func", sizeof("__lambda_func") - 1)) {
					add_next_index_str(return_value, zend_string_copy(alfi->func_ptr->common.function_name));
				} else {
					add_next_index_str(return_value, zend_string_copy(key));
				}
			}
		} ZEND_HASH_FOREACH_END();
		return;
	}

	array_init(return_value);
	add_next_index_str(return_value, zend_string_copy(EG(autoload_func)->common.function_name));
} /* }}} */

/* {{{ proto string spl_object_hash(object obj)
 Return hash id for given object */
PHP_FUNCTION(spl_object_hash)
{
	zval *obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &obj) == FAILURE) {
		return;
	}

	RETURN_NEW_STR(php_spl_object_hash(obj));
}
/* }}} */

/* {{{ proto int spl_object_id(object obj)
 Returns the integer object handle for the given object */
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

/* {{{ PHP_MINFO(spl)
 */
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

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_iterator_to_array, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_INFO(0, use_keys)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_iterator, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_iterator_apply, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_INFO(0, function)
	ZEND_ARG_ARRAY_INFO(0, args, 1)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_parents, 0, 0, 1)
	ZEND_ARG_INFO(0, instance)
	ZEND_ARG_INFO(0, autoload)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_implements, 0, 0, 1)
	ZEND_ARG_INFO(0, what)
	ZEND_ARG_INFO(0, autoload)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_uses, 0, 0, 1)
	ZEND_ARG_INFO(0, what)
	ZEND_ARG_INFO(0, autoload)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO(arginfo_spl_classes, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_spl_autoload_functions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_spl_autoload, 0, 0, 1)
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_INFO(0, file_extensions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_spl_autoload_extensions, 0, 0, 0)
	ZEND_ARG_INFO(0, file_extensions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_spl_autoload_call, 0, 0, 1)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_spl_autoload_register, 0, 0, 0)
	ZEND_ARG_INFO(0, autoload_function)
	ZEND_ARG_INFO(0, throw)
	ZEND_ARG_INFO(0, prepend)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_spl_autoload_unregister, 0, 0, 1)
	ZEND_ARG_INFO(0, autoload_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_spl_object_hash, 0, 0, 1)
	ZEND_ARG_INFO(0, obj)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_spl_object_id, 0, 0, 1)
	ZEND_ARG_INFO(0, obj)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ spl_functions
 */
static const zend_function_entry spl_functions[] = {
	PHP_FE(spl_classes,             arginfo_spl_classes)
	PHP_FE(spl_autoload,            arginfo_spl_autoload)
	PHP_FE(spl_autoload_extensions, arginfo_spl_autoload_extensions)
	PHP_FE(spl_autoload_register,   arginfo_spl_autoload_register)
	PHP_FE(spl_autoload_unregister, arginfo_spl_autoload_unregister)
	PHP_FE(spl_autoload_functions,  arginfo_spl_autoload_functions)
	PHP_FE(spl_autoload_call,       arginfo_spl_autoload_call)
	PHP_FE(class_parents,           arginfo_class_parents)
	PHP_FE(class_implements,        arginfo_class_implements)
	PHP_FE(class_uses,              arginfo_class_uses)
	PHP_FE(spl_object_hash,         arginfo_spl_object_hash)
	PHP_FE(spl_object_id,           arginfo_spl_object_id)
	PHP_FE(iterator_to_array,       arginfo_iterator_to_array)
	PHP_FE(iterator_count,          arginfo_iterator)
	PHP_FE(iterator_apply,          arginfo_iterator_apply)
	PHP_FE_END
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl)
 */
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

	spl_autoload_fn = zend_hash_str_find_ptr(CG(function_table), "spl_autoload", sizeof("spl_autoload") - 1);
	spl_autoload_call_fn = zend_hash_str_find_ptr(CG(function_table), "spl_autoload_call", sizeof("spl_autoload_call") - 1);
	ZEND_ASSERT(spl_autoload_fn != NULL && spl_autoload_call_fn != NULL);

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

/* {{{ spl_module_entry
 */
zend_module_entry spl_module_entry = {
	STANDARD_MODULE_HEADER,
	"SPL",
	spl_functions,
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
