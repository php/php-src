/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <stdio.h>
#include <signal.h>

#include "zend.h"
#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_API.h"
#include "zend_ptr_stack.h"
#include "zend_constants.h"
#include "zend_extensions.h"
#include "zend_exceptions.h"
#include "zend_closures.h"
#include "zend_generators.h"
#include "zend_vm.h"
#include "zend_float.h"
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

ZEND_API void (*zend_execute_ex)(zend_execute_data *execute_data TSRMLS_DC);
ZEND_API void (*zend_execute_internal)(zend_execute_data *execute_data_ptr, zend_fcall_info *fci, int return_value_used TSRMLS_DC);

/* true globals */
ZEND_API const zend_fcall_info empty_fcall_info = { 0, NULL, NULL, NULL, NULL, 0, NULL, NULL, 0 };
ZEND_API const zend_fcall_info_cache empty_fcall_info_cache = { 0, NULL, NULL, NULL, NULL };

#ifdef ZEND_WIN32
#include <process.h>
static WNDCLASS wc;
static HWND timeout_window;
static HANDLE timeout_thread_event;
static HANDLE timeout_thread_handle;
static DWORD timeout_thread_id;
static int timeout_thread_initialized=0;
#endif

#if 0&&ZEND_DEBUG
static void (*original_sigsegv_handler)(int);
static void zend_handle_sigsegv(int dummy) /* {{{ */
{
	fflush(stdout);
	fflush(stderr);
	if (original_sigsegv_handler == zend_handle_sigsegv) {
		signal(SIGSEGV, original_sigsegv_handler);
	} else {
		signal(SIGSEGV, SIG_DFL);
	}
	{
		TSRMLS_FETCH();

		fprintf(stderr, "SIGSEGV caught on opcode %d on opline %d of %s() at %s:%d\n\n",
				active_opline->opcode,
				active_opline-EG(active_op_array)->opcodes,
				get_active_function_name(TSRMLS_C),
				zend_get_executed_filename(TSRMLS_C),
				zend_get_executed_lineno(TSRMLS_C));
/* See http://support.microsoft.com/kb/190351 */
#ifdef PHP_WIN32
		fflush(stderr);
#endif
	}
	if (original_sigsegv_handler!=zend_handle_sigsegv) {
		original_sigsegv_handler(dummy);
	}
}
/* }}} */
#endif

static void zend_extension_activator(zend_extension *extension TSRMLS_DC) /* {{{ */
{
	if (extension->activate) {
		extension->activate();
	}
}
/* }}} */

static void zend_extension_deactivator(zend_extension *extension TSRMLS_DC) /* {{{ */
{
	if (extension->deactivate) {
		extension->deactivate();
	}
}
/* }}} */

static int clean_non_persistent_function(zend_function *function TSRMLS_DC) /* {{{ */
{
	return (function->type == ZEND_INTERNAL_FUNCTION) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static int clean_non_persistent_function_full(zend_function *function TSRMLS_DC) /* {{{ */
{
	return (function->type == ZEND_INTERNAL_FUNCTION) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static int clean_non_persistent_class(zend_class_entry **ce TSRMLS_DC) /* {{{ */
{
	return ((*ce)->type == ZEND_INTERNAL_CLASS) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static int clean_non_persistent_class_full(zend_class_entry **ce TSRMLS_DC) /* {{{ */
{
	return ((*ce)->type == ZEND_INTERNAL_CLASS) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

void init_executor(TSRMLS_D) /* {{{ */
{
	zend_init_fpu(TSRMLS_C);

	INIT_ZVAL(EG(uninitialized_zval));
	/* trick to make uninitialized_zval never be modified, passed by ref, etc. */
	Z_ADDREF(EG(uninitialized_zval));
	INIT_ZVAL(EG(error_zval));
	EG(uninitialized_zval_ptr)=&EG(uninitialized_zval);
	EG(error_zval_ptr)=&EG(error_zval);
/* destroys stack frame, therefore makes core dumps worthless */
#if 0&&ZEND_DEBUG
	original_sigsegv_handler = signal(SIGSEGV, zend_handle_sigsegv);
#endif
	EG(return_value_ptr_ptr) = NULL;

	EG(symtable_cache_ptr) = EG(symtable_cache) - 1;
	EG(symtable_cache_limit) = EG(symtable_cache) + SYMTABLE_CACHE_SIZE - 1;
	EG(no_extensions) = 0;

	EG(function_table) = CG(function_table);
	EG(class_table) = CG(class_table);

	EG(in_execution) = 0;
	EG(in_autoload) = NULL;
	EG(autoload_func) = NULL;
	EG(error_handling) = EH_NORMAL;

	zend_vm_stack_init(TSRMLS_C);
	zend_vm_stack_push((void *) NULL TSRMLS_CC);

	zend_hash_init(&EG(symbol_table), 50, NULL, ZVAL_PTR_DTOR, 0);
	EG(active_symbol_table) = &EG(symbol_table);

	zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_activator TSRMLS_CC);
	EG(opline_ptr) = NULL;

	zend_hash_init(&EG(included_files), 5, NULL, NULL, 0);

	EG(ticks_count) = 0;

	EG(user_error_handler) = NULL;

	EG(current_execute_data) = NULL;

	zend_stack_init(&EG(user_error_handlers_error_reporting));
	zend_ptr_stack_init(&EG(user_error_handlers));
	zend_ptr_stack_init(&EG(user_exception_handlers));

	zend_objects_store_init(&EG(objects_store), 1024);

	EG(full_tables_cleanup) = 0;
#ifdef ZEND_WIN32
	EG(timed_out) = 0;
#endif

	EG(exception) = NULL;
	EG(prev_exception) = NULL;

	EG(scope) = NULL;
	EG(called_scope) = NULL;

	EG(This) = NULL;

	EG(active_op_array) = NULL;

	EG(active) = 1;
	EG(start_op) = NULL;
}
/* }}} */

static int zval_call_destructor(zval **zv TSRMLS_DC) /* {{{ */
{
	if (Z_TYPE_PP(zv) == IS_OBJECT && Z_REFCOUNT_PP(zv) == 1) {
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}
/* }}} */

void shutdown_destructors(TSRMLS_D) /* {{{ */
{
	zend_try {
		int symbols;
		do {
			symbols = zend_hash_num_elements(&EG(symbol_table));
			zend_hash_reverse_apply(&EG(symbol_table), (apply_func_t) zval_call_destructor TSRMLS_CC);
		} while (symbols != zend_hash_num_elements(&EG(symbol_table)));
		zend_objects_store_call_destructors(&EG(objects_store) TSRMLS_CC);
	} zend_catch {
		/* if we couldn't destruct cleanly, mark all objects as destructed anyway */
		zend_objects_store_mark_destructed(&EG(objects_store) TSRMLS_CC);
	} zend_end_try();
}
/* }}} */

void shutdown_executor(TSRMLS_D) /* {{{ */
{
	zend_try {

/* Removed because this can not be safely done, e.g. in this situation:
   Object 1 creates object 2
   Object 3 holds reference to object 2.
   Now when 1 and 2 are destroyed, 3 can still access 2 in its destructor, with
   very problematic results */
/* 		zend_objects_store_call_destructors(&EG(objects_store) TSRMLS_CC); */

/* Moved after symbol table cleaners, because  some of the cleaners can call
   destructors, which would use EG(symtable_cache_ptr) and thus leave leaks */
/*		while (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
			zend_hash_destroy(*EG(symtable_cache_ptr));
			efree(*EG(symtable_cache_ptr));
			EG(symtable_cache_ptr)--;
		}
*/
		zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_deactivator TSRMLS_CC);
		zend_hash_graceful_reverse_destroy(&EG(symbol_table));
	} zend_end_try();

	zend_try {
		zval *zeh;
		/* remove error handlers before destroying classes and functions,
		 * so that if handler used some class, crash would not happen */
		if (EG(user_error_handler)) {
			zeh = EG(user_error_handler);
			EG(user_error_handler) = NULL;
			zval_ptr_dtor(&zeh);
		}

		if (EG(user_exception_handler)) {
			zeh = EG(user_exception_handler);
			EG(user_exception_handler) = NULL;
			zval_ptr_dtor(&zeh);
		}

		zend_stack_destroy(&EG(user_error_handlers_error_reporting));
		zend_stack_init(&EG(user_error_handlers_error_reporting));
		zend_ptr_stack_clean(&EG(user_error_handlers), ZVAL_DESTRUCTOR, 1);
		zend_ptr_stack_clean(&EG(user_exception_handlers), ZVAL_DESTRUCTOR, 1);
	} zend_end_try();

	zend_try {
		/* Cleanup static data for functions and arrays.
		 * We need a separate cleanup stage because of the following problem:
		 * Suppose we destroy class X, which destroys the class's function table,
		 * and in the function table we have function foo() that has static $bar.
		 * Now if an object of class X is assigned to $bar, its destructor will be
		 * called and will fail since X's function table is in mid-destruction.
		 * So we want first of all to clean up all data and then move to tables destruction.
		 * Note that only run-time accessed data need to be cleaned up, pre-defined data can
		 * not contain objects and thus are not probelmatic */
		if (EG(full_tables_cleanup)) {
			zend_hash_apply(EG(function_table), (apply_func_t) zend_cleanup_function_data_full TSRMLS_CC);
			zend_hash_apply(EG(class_table), (apply_func_t) zend_cleanup_class_data TSRMLS_CC);
		} else {
			zend_hash_reverse_apply(EG(function_table), (apply_func_t) zend_cleanup_function_data TSRMLS_CC);
			zend_hash_reverse_apply(EG(class_table), (apply_func_t) zend_cleanup_user_class_data TSRMLS_CC);
			zend_cleanup_internal_classes(TSRMLS_C);
		}
	} zend_end_try();

	zend_try {
		zend_objects_store_free_object_storage(&EG(objects_store) TSRMLS_CC);

		zend_vm_stack_destroy(TSRMLS_C);

		/* Destroy all op arrays */
		if (EG(full_tables_cleanup)) {
			zend_hash_reverse_apply(EG(function_table), (apply_func_t) clean_non_persistent_function_full TSRMLS_CC);
			zend_hash_reverse_apply(EG(class_table), (apply_func_t) clean_non_persistent_class_full TSRMLS_CC);
		} else {
			zend_hash_reverse_apply(EG(function_table), (apply_func_t) clean_non_persistent_function TSRMLS_CC);
			zend_hash_reverse_apply(EG(class_table), (apply_func_t) clean_non_persistent_class TSRMLS_CC);
		}

		while (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
			zend_hash_destroy(*EG(symtable_cache_ptr));
			FREE_HASHTABLE(*EG(symtable_cache_ptr));
			EG(symtable_cache_ptr)--;
		}
	} zend_end_try();

	zend_try {
		clean_non_persistent_constants(TSRMLS_C);
	} zend_end_try();

	zend_try {
#if 0&&ZEND_DEBUG
	signal(SIGSEGV, original_sigsegv_handler);
#endif

		zend_hash_destroy(&EG(included_files));

		zend_stack_destroy(&EG(user_error_handlers_error_reporting));
		zend_ptr_stack_destroy(&EG(user_error_handlers));
		zend_ptr_stack_destroy(&EG(user_exception_handlers));
		zend_objects_store_destroy(&EG(objects_store));
		if (EG(in_autoload)) {
			zend_hash_destroy(EG(in_autoload));
			FREE_HASHTABLE(EG(in_autoload));
		}
	} zend_end_try();

	zend_shutdown_fpu(TSRMLS_C);

	EG(active) = 0;
}
/* }}} */

/* return class name and "::" or "". */
ZEND_API const char *get_active_class_name(const char **space TSRMLS_DC) /* {{{ */
{
	if (!zend_is_executing(TSRMLS_C)) {
		if (space) {
			*space = "";
		}
		return "";
	}
	switch (EG(current_execute_data)->function_state.function->type) {
		case ZEND_USER_FUNCTION:
		case ZEND_INTERNAL_FUNCTION:
		{
			zend_class_entry *ce = EG(current_execute_data)->function_state.function->common.scope;

			if (space) {
				*space = ce ? "::" : "";
			}
			return ce ? ce->name : "";
		}
		default:
			if (space) {
				*space = "";
			}
			return "";
	}
}
/* }}} */

ZEND_API const char *get_active_function_name(TSRMLS_D) /* {{{ */
{
	if (!zend_is_executing(TSRMLS_C)) {
		return NULL;
	}
	switch (EG(current_execute_data)->function_state.function->type) {
		case ZEND_USER_FUNCTION: {
				const char *function_name = ((zend_op_array *) EG(current_execute_data)->function_state.function)->function_name;

				if (function_name) {
					return function_name;
				} else {
					return "main";
				}
			}
			break;
		case ZEND_INTERNAL_FUNCTION:
			return ((zend_internal_function *) EG(current_execute_data)->function_state.function)->function_name;
			break;
		default:
			return NULL;
	}
}
/* }}} */

ZEND_API const char *zend_get_executed_filename(TSRMLS_D) /* {{{ */
{
	if (EG(active_op_array)) {
		return EG(active_op_array)->filename;
	} else {
		return "[no active file]";
	}
}
/* }}} */

ZEND_API uint zend_get_executed_lineno(TSRMLS_D) /* {{{ */
{
	if(EG(exception) && EG(opline_ptr) && active_opline->opcode == ZEND_HANDLE_EXCEPTION &&
		active_opline->lineno == 0 && EG(opline_before_exception)) {
		return EG(opline_before_exception)->lineno;
	}
	if (EG(opline_ptr)) {
		return active_opline->lineno;
	} else {
		return 0;
	}
}
/* }}} */

ZEND_API zend_bool zend_is_executing(TSRMLS_D) /* {{{ */
{
	return EG(in_execution);
}
/* }}} */

ZEND_API void _zval_ptr_dtor(zval **zval_ptr ZEND_FILE_LINE_DC) /* {{{ */
{
	TSRMLS_FETCH();
	i_zval_ptr_dtor(*zval_ptr ZEND_FILE_LINE_RELAY_CC TSRMLS_CC);
}
/* }}} */

ZEND_API void _zval_internal_ptr_dtor(zval **zval_ptr ZEND_FILE_LINE_DC) /* {{{ */
{
#if DEBUG_ZEND>=2
	printf("Reducing refcount for %x (%x): %d->%d\n", *zval_ptr, zval_ptr, Z_REFCOUNT_PP(zval_ptr), Z_REFCOUNT_PP(zval_ptr) - 1);
#endif
	Z_DELREF_PP(zval_ptr);
	if (Z_REFCOUNT_PP(zval_ptr) == 0) {
		zval_internal_dtor(*zval_ptr);
		free(*zval_ptr);
	} else if (Z_REFCOUNT_PP(zval_ptr) == 1) {
		Z_UNSET_ISREF_PP(zval_ptr);
	}
}
/* }}} */

ZEND_API int zend_is_true(zval *op TSRMLS_DC) /* {{{ */
{
	return i_zend_is_true(op TSRMLS_CC);
}
/* }}} */

#include "../TSRM/tsrm_strtok_r.h"

#define IS_VISITED_CONSTANT			IS_CONSTANT_INDEX
#define IS_CONSTANT_VISITED(p)		(Z_TYPE_P(p) & IS_VISITED_CONSTANT)
#define Z_REAL_TYPE_P(p)			(Z_TYPE_P(p) & ~IS_VISITED_CONSTANT)
#define MARK_CONSTANT_VISITED(p)	Z_TYPE_P(p) |= IS_VISITED_CONSTANT

static void zval_deep_copy(zval **p)
{
	zval *value;

	ALLOC_ZVAL(value);
	*value = **p;
	Z_TYPE_P(value) &= ~IS_CONSTANT_INDEX;
	zval_copy_ctor(value);
	Z_TYPE_P(value) = Z_TYPE_PP(p);
	INIT_PZVAL(value);
	*p = value;
}

ZEND_API int zval_update_constant_ex(zval **pp, void *arg, zend_class_entry *scope TSRMLS_DC) /* {{{ */
{
	zval *p = *pp;
	zend_bool inline_change = (zend_bool) (zend_uintptr_t) arg;
	zval const_value;
	char *colon;

	if (IS_CONSTANT_VISITED(p)) {
		zend_error(E_ERROR, "Cannot declare self-referencing constant '%s'", Z_STRVAL_P(p));
	} else if ((Z_TYPE_P(p) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT) {
		int refcount;
		zend_uchar is_ref;

		SEPARATE_ZVAL_IF_NOT_REF(pp);
		p = *pp;

		MARK_CONSTANT_VISITED(p);

		refcount = Z_REFCOUNT_P(p);
		is_ref = Z_ISREF_P(p);

		if (!zend_get_constant_ex(p->value.str.val, p->value.str.len, &const_value, scope, Z_REAL_TYPE_P(p) TSRMLS_CC)) {
			char *actual = Z_STRVAL_P(p);

			if ((colon = (char*)zend_memrchr(Z_STRVAL_P(p), ':', Z_STRLEN_P(p)))) {
				zend_error(E_ERROR, "Undefined class constant '%s'", Z_STRVAL_P(p));
				Z_STRLEN_P(p) -= ((colon - Z_STRVAL_P(p)) + 1);
				if (inline_change) {
					colon = estrndup(colon, Z_STRLEN_P(p));
					str_efree(Z_STRVAL_P(p));
					Z_STRVAL_P(p) = colon;
				} else {
					Z_STRVAL_P(p) = colon + 1;
				}
			} else {
				char *save = actual, *slash;
				int actual_len = Z_STRLEN_P(p);
				if ((Z_TYPE_P(p) & IS_CONSTANT_UNQUALIFIED) && (slash = (char *)zend_memrchr(actual, '\\', actual_len))) {
					actual = slash + 1;
					actual_len -= (actual - Z_STRVAL_P(p));
					if (inline_change) {
						actual = estrndup(actual, actual_len);
						Z_STRVAL_P(p) = actual;
						Z_STRLEN_P(p) = actual_len;
					}
				}
				if (actual[0] == '\\') {
					if (inline_change) {
						memmove(Z_STRVAL_P(p), Z_STRVAL_P(p)+1, Z_STRLEN_P(p));
						--Z_STRLEN_P(p);
					} else {
						++actual;
					}
					--actual_len;
				}
				if ((Z_TYPE_P(p) & IS_CONSTANT_UNQUALIFIED) == 0) {
					int fix_save = 0;
					if (save[0] == '\\') {
						save++;
						fix_save = 1;
					}
					zend_error(E_ERROR, "Undefined constant '%s'", save);
					if (fix_save) {
						save--;
					}
					if (inline_change) {
						str_efree(save);
					}
					save = NULL;
				}
				if (inline_change && save && save != actual) {
					str_efree(save);
				}
				zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",  actual,  actual);
				p->type = IS_STRING;
				if (!inline_change) {
					Z_STRVAL_P(p) = actual;
					Z_STRLEN_P(p) = actual_len;
					zval_copy_ctor(p);
				}
			}
		} else {
			if (inline_change) {
				str_efree(Z_STRVAL_P(p));
			}
			*p = const_value;
		}

		Z_SET_REFCOUNT_P(p, refcount);
		Z_SET_ISREF_TO_P(p, is_ref);
	} else if (Z_TYPE_P(p) == IS_CONSTANT_ARRAY) {
		zval **element, *new_val;
		char *str_index;
		uint str_index_len;
		ulong num_index;
		int ret;

		SEPARATE_ZVAL_IF_NOT_REF(pp);
		p = *pp;
		Z_TYPE_P(p) = IS_ARRAY;

		if (!inline_change) {
			zval *tmp;
			HashTable *tmp_ht = NULL;

			ALLOC_HASHTABLE(tmp_ht);
			zend_hash_init(tmp_ht, zend_hash_num_elements(Z_ARRVAL_P(p)), NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_copy(tmp_ht, Z_ARRVAL_P(p), (copy_ctor_func_t) zval_deep_copy, (void *) &tmp, sizeof(zval *));
			Z_ARRVAL_P(p) = tmp_ht;
		}

		/* First go over the array and see if there are any constant indices */
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(p));
		while (zend_hash_get_current_data(Z_ARRVAL_P(p), (void **) &element) == SUCCESS) {
			if (!(Z_TYPE_PP(element) & IS_CONSTANT_INDEX)) {
				zend_hash_move_forward(Z_ARRVAL_P(p));
				continue;
			}
			Z_TYPE_PP(element) &= ~IS_CONSTANT_INDEX;
			if (zend_hash_get_current_key_ex(Z_ARRVAL_P(p), &str_index, &str_index_len, &num_index, 0, NULL) != HASH_KEY_IS_STRING) {
				zend_hash_move_forward(Z_ARRVAL_P(p));
				continue;
			}
			if (str_index[str_index_len - 2] == IS_CONSTANT_AST) {
				zend_ast_evaluate(&const_value, *(zend_ast **)str_index, scope TSRMLS_CC);
				zend_ast_destroy(*(zend_ast **)str_index);
			} else if (!zend_get_constant_ex(str_index, str_index_len - 3, &const_value, scope, str_index[str_index_len - 2] TSRMLS_CC)) {
				char *actual;
				const char *save = str_index;
				if ((colon = (char*)zend_memrchr(str_index, ':', str_index_len - 3))) {
					zend_error(E_ERROR, "Undefined class constant '%s'", str_index);
					str_index_len -= ((colon - str_index) + 1);
					str_index = colon;
				} else {
					if (str_index[str_index_len - 2] & IS_CONSTANT_UNQUALIFIED) {
						if ((actual = (char *)zend_memrchr(str_index, '\\', str_index_len - 3))) {
							actual++;
							str_index_len -= (actual - str_index);
							str_index = actual;
						}
					}
					if (str_index[0] == '\\') {
						++str_index;
						--str_index_len;
					}
					if (save[0] == '\\') {
						++save;
					}
					if ((str_index[str_index_len - 2] & IS_CONSTANT_UNQUALIFIED) == 0) {
						zend_error(E_ERROR, "Undefined constant '%s'", save);
					}
					zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",	str_index, str_index);
				}
				ZVAL_STRINGL(&const_value, str_index, str_index_len-3, 1);
			}

			if (Z_REFCOUNT_PP(element) > 1) {
				ALLOC_ZVAL(new_val);
				*new_val = **element;
				zval_copy_ctor(new_val);
				Z_SET_REFCOUNT_P(new_val, 1);
				Z_UNSET_ISREF_P(new_val);

				/* preserve this bit for inheritance */
				Z_TYPE_PP(element) |= IS_CONSTANT_INDEX;
				zval_ptr_dtor(element);
				*element = new_val;
			}

			switch (Z_TYPE(const_value)) {
				case IS_STRING:
					ret = zend_symtable_update_current_key(Z_ARRVAL_P(p), Z_STRVAL(const_value), Z_STRLEN(const_value) + 1, HASH_UPDATE_KEY_IF_BEFORE);
					break;
				case IS_BOOL:
				case IS_LONG:
					ret = zend_hash_update_current_key_ex(Z_ARRVAL_P(p), HASH_KEY_IS_LONG, NULL, 0, Z_LVAL(const_value), HASH_UPDATE_KEY_IF_BEFORE, NULL);
					break;
				case IS_DOUBLE:
					ret = zend_hash_update_current_key_ex(Z_ARRVAL_P(p), HASH_KEY_IS_LONG, NULL, 0, zend_dval_to_lval(Z_DVAL(const_value)), HASH_UPDATE_KEY_IF_BEFORE, NULL);
					break;
				case IS_NULL:
					ret = zend_hash_update_current_key_ex(Z_ARRVAL_P(p), HASH_KEY_IS_STRING, "", 1, 0, HASH_UPDATE_KEY_IF_BEFORE, NULL);
					break;
				default:
					ret = SUCCESS;
					break;
			}
			if (ret == SUCCESS) {
				zend_hash_move_forward(Z_ARRVAL_P(p));
			}
			zval_dtor(&const_value);
		}
		zend_hash_apply_with_argument(Z_ARRVAL_P(p), (apply_func_arg_t) zval_update_constant_inline_change, (void *) scope TSRMLS_CC);
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(p));
	} else if (Z_TYPE_P(p) == IS_CONSTANT_AST) {
		SEPARATE_ZVAL_IF_NOT_REF(pp);
		p = *pp;

		zend_ast_evaluate(&const_value, Z_AST_P(p), scope TSRMLS_CC);
		if (inline_change) {
			zend_ast_destroy(Z_AST_P(p));
		}
		ZVAL_COPY_VALUE(p, &const_value);
	}
	return 0;
}
/* }}} */

ZEND_API int zval_update_constant_inline_change(zval **pp, void *scope TSRMLS_DC) /* {{{ */
{
	return zval_update_constant_ex(pp, (void*)1, scope TSRMLS_CC);
}
/* }}} */

ZEND_API int zval_update_constant_no_inline_change(zval **pp, void *scope TSRMLS_DC) /* {{{ */
{
	return zval_update_constant_ex(pp, (void*)0, scope TSRMLS_CC);
}
/* }}} */

ZEND_API int zval_update_constant(zval **pp, void *arg TSRMLS_DC) /* {{{ */
{
	return zval_update_constant_ex(pp, arg, NULL TSRMLS_CC);
}
/* }}} */

int call_user_function(HashTable *function_table, zval **object_pp, zval *function_name, zval *retval_ptr, zend_uint param_count, zval *params[] TSRMLS_DC) /* {{{ */
{
	zval ***params_array;
	zend_uint i;
	int ex_retval;
	zval *local_retval_ptr = NULL;

	if (param_count) {
		params_array = (zval ***) emalloc(sizeof(zval **)*param_count);
		for (i=0; i<param_count; i++) {
			params_array[i] = &params[i];
		}
	} else {
		params_array = NULL;
	}
	ex_retval = call_user_function_ex(function_table, object_pp, function_name, &local_retval_ptr, param_count, params_array, 1, NULL TSRMLS_CC);
	if (local_retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*retval_ptr, local_retval_ptr);
	} else {
		INIT_ZVAL(*retval_ptr);
	}
	if (params_array) {
		efree(params_array);
	}
	return ex_retval;
}
/* }}} */

int call_user_function_ex(HashTable *function_table, zval **object_pp, zval *function_name, zval **retval_ptr_ptr, zend_uint param_count, zval **params[], int no_separation, HashTable *symbol_table TSRMLS_DC) /* {{{ */
{
	zend_fcall_info fci;

	fci.size = sizeof(fci);
	fci.function_table = function_table;
	fci.object_ptr = object_pp ? *object_pp : NULL;
	fci.function_name = function_name;
	fci.retval_ptr_ptr = retval_ptr_ptr;
	fci.param_count = param_count;
	fci.params = params;
	fci.no_separation = (zend_bool) no_separation;
	fci.symbol_table = symbol_table;

	return zend_call_function(&fci, NULL TSRMLS_CC);
}
/* }}} */

int zend_call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache TSRMLS_DC) /* {{{ */
{
	zend_uint i;
	zval **original_return_value;
	HashTable *calling_symbol_table;
	zend_op_array *original_op_array;
	zend_op **original_opline_ptr;
	zend_class_entry *current_scope;
	zend_class_entry *current_called_scope;
	zend_class_entry *calling_scope = NULL;
	zend_class_entry *called_scope = NULL;
	zval *current_this;
	zend_execute_data execute_data;
	zend_fcall_info_cache fci_cache_local;

	*fci->retval_ptr_ptr = NULL;

	if (!EG(active)) {
		return FAILURE; /* executor is already inactive */
	}

	if (EG(exception)) {
		return FAILURE; /* we would result in an instable executor otherwise */
	}

	switch (fci->size) {
		case sizeof(zend_fcall_info):
			break; /* nothing to do currently */
		default:
			zend_error(E_ERROR, "Corrupted fcall_info provided to zend_call_function()");
			break;
	}

	/* Initialize execute_data */
	if (EG(current_execute_data)) {
		execute_data = *EG(current_execute_data);
		EX(op_array) = NULL;
		EX(opline) = NULL;
		EX(object) = NULL;
	} else {
		/* This only happens when we're called outside any execute()'s
		 * It shouldn't be strictly necessary to NULL execute_data out,
		 * but it may make bugs easier to spot
		 */
		memset(&execute_data, 0, sizeof(zend_execute_data));
	}

	if (!fci_cache || !fci_cache->initialized) {
		char *callable_name;
		char *error = NULL;

		if (!fci_cache) {
			fci_cache = &fci_cache_local;
		}

		if (!zend_is_callable_ex(fci->function_name, fci->object_ptr, IS_CALLABLE_CHECK_SILENT, &callable_name, NULL, fci_cache, &error TSRMLS_CC)) {
			if (error) {
				zend_error(E_WARNING, "Invalid callback %s, %s", callable_name, error);
				efree(error);
			}
			if (callable_name) {
				efree(callable_name);
			}
			return FAILURE;
		} else if (error) {
			/* Capitalize the first latter of the error message */
			if (error[0] >= 'a' && error[0] <= 'z') {
				error[0] += ('A' - 'a');
			}
			zend_error(E_STRICT, "%s", error);
			efree(error);
		}
		efree(callable_name);
	}

	EX(function_state).function = fci_cache->function_handler;
	calling_scope = fci_cache->calling_scope;
	called_scope = fci_cache->called_scope;
	fci->object_ptr = fci_cache->object_ptr;
	EX(object) = fci->object_ptr;
	if (fci->object_ptr && Z_TYPE_P(fci->object_ptr) == IS_OBJECT &&
	    (!EG(objects_store).object_buckets || !EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(fci->object_ptr)].valid)) {
		return FAILURE;
	}

	if (EX(function_state).function->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_DEPRECATED)) {
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_ABSTRACT) {
			zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name);
		}
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_DEPRECATED) {
 			zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
				EX(function_state).function->common.scope ? EX(function_state).function->common.scope->name : "",
				EX(function_state).function->common.scope ? "::" : "",
				EX(function_state).function->common.function_name);
		}
	}

	ZEND_VM_STACK_GROW_IF_NEEDED(fci->param_count + 1);

	for (i=0; i<fci->param_count; i++) {
		zval *param;

		if (ARG_SHOULD_BE_SENT_BY_REF(EX(function_state).function, i + 1)) {
			if (!PZVAL_IS_REF(*fci->params[i]) && Z_REFCOUNT_PP(fci->params[i]) > 1) {
				zval *new_zval;

				if (fci->no_separation &&
				    !ARG_MAY_BE_SENT_BY_REF(EX(function_state).function, i + 1)) {
					if (i || UNEXPECTED(ZEND_VM_STACK_ELEMETS(EG(argument_stack)) == (EG(argument_stack)->top))) {
						/* hack to clean up the stack */
						zend_vm_stack_push((void *) (zend_uintptr_t)i TSRMLS_CC);
						zend_vm_stack_clear_multiple(0 TSRMLS_CC);
					}

					zend_error(E_WARNING, "Parameter %d to %s%s%s() expected to be a reference, value given",
						i+1,
						EX(function_state).function->common.scope ? EX(function_state).function->common.scope->name : "",
						EX(function_state).function->common.scope ? "::" : "",
						EX(function_state).function->common.function_name);
					return FAILURE;
				}

				ALLOC_ZVAL(new_zval);
				*new_zval = **fci->params[i];
				zval_copy_ctor(new_zval);
				Z_SET_REFCOUNT_P(new_zval, 1);
				Z_DELREF_PP(fci->params[i]);
				*fci->params[i] = new_zval;
			}
			Z_ADDREF_PP(fci->params[i]);
			Z_SET_ISREF_PP(fci->params[i]);
			param = *fci->params[i];
		} else if (PZVAL_IS_REF(*fci->params[i]) &&
		           /* don't separate references for __call */
		           (EX(function_state).function->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) == 0 ) {
			ALLOC_ZVAL(param);
			*param = **(fci->params[i]);
			INIT_PZVAL(param);
			zval_copy_ctor(param);
		} else if (*fci->params[i] != &EG(uninitialized_zval)) {
			Z_ADDREF_PP(fci->params[i]);
			param = *fci->params[i];
		} else {
			ALLOC_ZVAL(param);
			*param = **(fci->params[i]);
			INIT_PZVAL(param);
		}
		zend_vm_stack_push(param TSRMLS_CC);
	}

	EX(function_state).arguments = zend_vm_stack_top(TSRMLS_C);
	zend_vm_stack_push((void*)(zend_uintptr_t)fci->param_count TSRMLS_CC);

	current_scope = EG(scope);
	EG(scope) = calling_scope;

	current_this = EG(This);

	current_called_scope = EG(called_scope);
	if (called_scope) {
		EG(called_scope) = called_scope;
	} else if (EX(function_state).function->type != ZEND_INTERNAL_FUNCTION) {
		EG(called_scope) = NULL;
	}

	if (fci->object_ptr) {
		if ((EX(function_state).function->common.fn_flags & ZEND_ACC_STATIC)) {
			EG(This) = NULL;
		} else {
			EG(This) = fci->object_ptr;

			if (!PZVAL_IS_REF(EG(This))) {
				Z_ADDREF_P(EG(This)); /* For $this pointer */
			} else {
				zval *this_ptr;

				ALLOC_ZVAL(this_ptr);
				*this_ptr = *EG(This);
				INIT_PZVAL(this_ptr);
				zval_copy_ctor(this_ptr);
				EG(This) = this_ptr;
			}
		}
	} else {
		EG(This) = NULL;
	}

	EX(prev_execute_data) = EG(current_execute_data);
	EG(current_execute_data) = &execute_data;

	if (EX(function_state).function->type == ZEND_USER_FUNCTION) {
		calling_symbol_table = EG(active_symbol_table);
		EG(scope) = EX(function_state).function->common.scope;
		if (fci->symbol_table) {
			EG(active_symbol_table) = fci->symbol_table;
		} else {
			EG(active_symbol_table) = NULL;
		}

		original_return_value = EG(return_value_ptr_ptr);
		original_op_array = EG(active_op_array);
		EG(return_value_ptr_ptr) = fci->retval_ptr_ptr;
		EG(active_op_array) = (zend_op_array *) EX(function_state).function;
		original_opline_ptr = EG(opline_ptr);

		if (EG(active_op_array)->fn_flags & ZEND_ACC_GENERATOR) {
			*fci->retval_ptr_ptr = zend_generator_create_zval(EG(active_op_array) TSRMLS_CC);
		} else {
			zend_execute(EG(active_op_array) TSRMLS_CC);
		}

		if (!fci->symbol_table && EG(active_symbol_table)) {
			zend_clean_and_cache_symbol_table(EG(active_symbol_table) TSRMLS_CC);
		}
		EG(active_symbol_table) = calling_symbol_table;
		EG(active_op_array) = original_op_array;
		EG(return_value_ptr_ptr)=original_return_value;
		EG(opline_ptr) = original_opline_ptr;
	} else if (EX(function_state).function->type == ZEND_INTERNAL_FUNCTION) {
		int call_via_handler = (EX(function_state).function->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) != 0;
		ALLOC_INIT_ZVAL(*fci->retval_ptr_ptr);
		if (EX(function_state).function->common.scope) {
			EG(scope) = EX(function_state).function->common.scope;
		}
		if (EXPECTED(zend_execute_internal == NULL)) {
			/* saves one function call if zend_execute_internal is not used */
			EX(function_state).function->internal_function.handler(fci->param_count, *fci->retval_ptr_ptr, fci->retval_ptr_ptr, fci->object_ptr, 1 TSRMLS_CC);
		} else {
			zend_execute_internal(&execute_data, fci, 1 TSRMLS_CC);
		}
		/*  We shouldn't fix bad extensions here,
			because it can break proper ones (Bug #34045)
		if (!EX(function_state).function->common.return_reference)
		{
			INIT_PZVAL(*fci->retval_ptr_ptr);
		}*/
		if (EG(exception) && fci->retval_ptr_ptr) {
			zval_ptr_dtor(fci->retval_ptr_ptr);
			*fci->retval_ptr_ptr = NULL;
		}

		if (call_via_handler) {
			/* We must re-initialize function again */
			fci_cache->initialized = 0;
		}
	} else { /* ZEND_OVERLOADED_FUNCTION */
		ALLOC_INIT_ZVAL(*fci->retval_ptr_ptr);

		/* Not sure what should be done here if it's a static method */
		if (fci->object_ptr) {
			Z_OBJ_HT_P(fci->object_ptr)->call_method(EX(function_state).function->common.function_name, fci->param_count, *fci->retval_ptr_ptr, fci->retval_ptr_ptr, fci->object_ptr, 1 TSRMLS_CC);
		} else {
			zend_error_noreturn(E_ERROR, "Cannot call overloaded function for non-object");
		}

		if (EX(function_state).function->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			efree((char*)EX(function_state).function->common.function_name);
		}
		efree(EX(function_state).function);

		if (EG(exception) && fci->retval_ptr_ptr) {
			zval_ptr_dtor(fci->retval_ptr_ptr);
			*fci->retval_ptr_ptr = NULL;
		}
	}
	zend_vm_stack_clear_multiple(0 TSRMLS_CC);

	if (EG(This)) {
		zval_ptr_dtor(&EG(This));
	}
	EG(called_scope) = current_called_scope;
	EG(scope) = current_scope;
	EG(This) = current_this;
	EG(current_execute_data) = EX(prev_execute_data);

	if (EG(exception)) {
		zend_throw_exception_internal(NULL TSRMLS_CC);
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_lookup_class_ex(const char *name, int name_length, const zend_literal *key, int use_autoload, zend_class_entry ***ce TSRMLS_DC) /* {{{ */
{
	zval **args[1];
	zval autoload_function;
	zval *class_name_ptr;
	zval *retval_ptr = NULL;
	int retval, lc_length;
	char *lc_name;
	char *lc_free;
	zend_fcall_info fcall_info;
	zend_fcall_info_cache fcall_cache;
	char dummy = 1;
	ulong hash;
	ALLOCA_FLAG(use_heap)

	if (key) {
		lc_name = Z_STRVAL(key->constant);
		lc_length = Z_STRLEN(key->constant) + 1;
		hash = key->hash_value;
	} else {
		if (name == NULL || !name_length) {
			return FAILURE;
		}

		lc_free = lc_name = do_alloca(name_length + 1, use_heap);
		zend_str_tolower_copy(lc_name, name, name_length);
		lc_length = name_length + 1;

		if (lc_name[0] == '\\') {
			lc_name += 1;
			lc_length -= 1;
		}

		hash = zend_inline_hash_func(lc_name, lc_length);
	}

	if (zend_hash_quick_find(EG(class_table), lc_name, lc_length, hash, (void **) ce) == SUCCESS) {
		if (!key) {
			free_alloca(lc_free, use_heap);
		}
		return SUCCESS;
	}

	/* The compiler is not-reentrant. Make sure we __autoload() only during run-time
	 * (doesn't impact functionality of __autoload()
	*/
	if (!use_autoload || zend_is_compiling(TSRMLS_C)) {
		if (!key) {
			free_alloca(lc_free, use_heap);
		}
		return FAILURE;
	}

	/* Verify class name before passing it to __autoload() */
	if (strspn(name, "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\177\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377\\") != name_length) {
		if (!key) {
			free_alloca(lc_free, use_heap);
		}
		return FAILURE;
	}
	
	if (EG(in_autoload) == NULL) {
		ALLOC_HASHTABLE(EG(in_autoload));
		zend_hash_init(EG(in_autoload), 0, NULL, NULL, 0);
	}

	if (zend_hash_quick_add(EG(in_autoload), lc_name, lc_length, hash, (void**)&dummy, sizeof(char), NULL) == FAILURE) {
		if (!key) {
			free_alloca(lc_free, use_heap);
		}
		return FAILURE;
	}

	ZVAL_STRINGL(&autoload_function, ZEND_AUTOLOAD_FUNC_NAME, sizeof(ZEND_AUTOLOAD_FUNC_NAME) - 1, 0);

	ALLOC_ZVAL(class_name_ptr);
	INIT_PZVAL(class_name_ptr);
	if (name[0] == '\\') {
		ZVAL_STRINGL(class_name_ptr, name+1, name_length-1, 1);
	} else {
		ZVAL_STRINGL(class_name_ptr, name, name_length, 1);
	}

	args[0] = &class_name_ptr;

	fcall_info.size = sizeof(fcall_info);
	fcall_info.function_table = EG(function_table);
	fcall_info.function_name = &autoload_function;
	fcall_info.symbol_table = NULL;
	fcall_info.retval_ptr_ptr = &retval_ptr;
	fcall_info.param_count = 1;
	fcall_info.params = args;
	fcall_info.object_ptr = NULL;
	fcall_info.no_separation = 1;

	fcall_cache.initialized = EG(autoload_func) ? 1 : 0;
	fcall_cache.function_handler = EG(autoload_func);
	fcall_cache.calling_scope = NULL;
	fcall_cache.called_scope = NULL;
	fcall_cache.object_ptr = NULL;

	zend_exception_save(TSRMLS_C);
	retval = zend_call_function(&fcall_info, &fcall_cache TSRMLS_CC);
	zend_exception_restore(TSRMLS_C);

	EG(autoload_func) = fcall_cache.function_handler;

	zval_ptr_dtor(&class_name_ptr);

	zend_hash_quick_del(EG(in_autoload), lc_name, lc_length, hash);

	if (retval_ptr) {
		zval_ptr_dtor(&retval_ptr);
	}

	if (retval == SUCCESS) {
		retval = zend_hash_quick_find(EG(class_table), lc_name, lc_length, hash, (void **) ce);
	}
	if (!key) {
		free_alloca(lc_free, use_heap);
	}
	return retval;
}
/* }}} */

ZEND_API int zend_lookup_class(const char *name, int name_length, zend_class_entry ***ce TSRMLS_DC) /* {{{ */
{
	return zend_lookup_class_ex(name, name_length, NULL, 1, ce TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_eval_stringl(char *str, int str_len, zval *retval_ptr, char *string_name TSRMLS_DC) /* {{{ */
{
	zval pv;
	zend_op_array *new_op_array;
	zend_op_array *original_active_op_array = EG(active_op_array);
	zend_uint original_compiler_options;
	int retval;

	if (retval_ptr) {
		Z_STRLEN(pv) = str_len + sizeof("return ;") - 1;
		Z_STRVAL(pv) = emalloc(Z_STRLEN(pv) + 1);
		memcpy(Z_STRVAL(pv), "return ", sizeof("return ") - 1);
		memcpy(Z_STRVAL(pv) + sizeof("return ") - 1, str, str_len);
		Z_STRVAL(pv)[Z_STRLEN(pv) - 1] = ';';
		Z_STRVAL(pv)[Z_STRLEN(pv)] = '\0';
	} else {
		Z_STRLEN(pv) = str_len;
		Z_STRVAL(pv) = str;
	}
	Z_TYPE(pv) = IS_STRING;

	/*printf("Evaluating '%s'\n", pv.value.str.val);*/

	original_compiler_options = CG(compiler_options);
	CG(compiler_options) = ZEND_COMPILE_DEFAULT_FOR_EVAL;
	new_op_array = zend_compile_string(&pv, string_name TSRMLS_CC);
	CG(compiler_options) = original_compiler_options;

	if (new_op_array) {
		zval *local_retval_ptr=NULL;
		zval **original_return_value_ptr_ptr = EG(return_value_ptr_ptr);
		zend_op **original_opline_ptr = EG(opline_ptr);
		int orig_interactive = CG(interactive);

		EG(return_value_ptr_ptr) = &local_retval_ptr;
		EG(active_op_array) = new_op_array;
		EG(no_extensions)=1;
		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}
		CG(interactive) = 0;

		zend_try {
			zend_execute(new_op_array TSRMLS_CC);
		} zend_catch {
			destroy_op_array(new_op_array TSRMLS_CC);
			efree(new_op_array);
			zend_bailout();
		} zend_end_try();

		CG(interactive) = orig_interactive;
		if (local_retval_ptr) {
			if (retval_ptr) {
				COPY_PZVAL_TO_ZVAL(*retval_ptr, local_retval_ptr);
			} else {
				zval_ptr_dtor(&local_retval_ptr);
			}
		} else {
			if (retval_ptr) {
				INIT_ZVAL(*retval_ptr);
			}
		}

		EG(no_extensions)=0;
		EG(opline_ptr) = original_opline_ptr;
		EG(active_op_array) = original_active_op_array;
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		EG(return_value_ptr_ptr) = original_return_value_ptr_ptr;
		retval = SUCCESS;
	} else {
		retval = FAILURE;
	}
	if (retval_ptr) {
		zval_dtor(&pv);
	}
	return retval;
}
/* }}} */

ZEND_API int zend_eval_string(char *str, zval *retval_ptr, char *string_name TSRMLS_DC) /* {{{ */
{
	return zend_eval_stringl(str, strlen(str), retval_ptr, string_name TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_eval_stringl_ex(char *str, int str_len, zval *retval_ptr, char *string_name, int handle_exceptions TSRMLS_DC) /* {{{ */
{
	int result;

	result = zend_eval_stringl(str, str_len, retval_ptr, string_name TSRMLS_CC);
	if (handle_exceptions && EG(exception)) {
		zend_exception_error(EG(exception), E_ERROR TSRMLS_CC);
		result = FAILURE;
	}
	return result;
}
/* }}} */

ZEND_API int zend_eval_string_ex(char *str, zval *retval_ptr, char *string_name, int handle_exceptions TSRMLS_DC) /* {{{ */
{
	return zend_eval_stringl_ex(str, strlen(str), retval_ptr, string_name, handle_exceptions TSRMLS_CC);
}
/* }}} */

void execute_new_code(TSRMLS_D) /* {{{ */
{
	zend_op *opline, *end;
	zend_op *ret_opline;
	int orig_interactive;

	if (!(CG(active_op_array)->fn_flags & ZEND_ACC_INTERACTIVE)
		|| CG(context).backpatch_count>0
		|| CG(active_op_array)->function_name
		|| CG(active_op_array)->type!=ZEND_USER_FUNCTION) {
		return;
	}

	ret_opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	ret_opline->opcode = ZEND_RETURN;
	ret_opline->op1_type = IS_CONST;
	ret_opline->op1.constant = zend_add_literal(CG(active_op_array), &EG(uninitialized_zval) TSRMLS_CC);
	SET_UNUSED(ret_opline->op2);

	if (!EG(start_op)) {
		EG(start_op) = CG(active_op_array)->opcodes;
	}

	opline=EG(start_op);
	end=CG(active_op_array)->opcodes+CG(active_op_array)->last;

	while (opline<end) {
		if (opline->op1_type == IS_CONST) {
			opline->op1.zv = &CG(active_op_array)->literals[opline->op1.constant].constant;
		}
		if (opline->op2_type == IS_CONST) {
			opline->op2.zv = &CG(active_op_array)->literals[opline->op2.constant].constant;
		}
		switch (opline->opcode) {
			case ZEND_GOTO:
				if (Z_TYPE_P(opline->op2.zv) != IS_LONG) {
					zend_resolve_goto_label(CG(active_op_array), opline, 1 TSRMLS_CC);
				}
				/* break omitted intentionally */
			case ZEND_JMP:
				opline->op1.jmp_addr = &CG(active_op_array)->opcodes[opline->op1.opline_num];
				break;
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_JMP_SET:
			case ZEND_JMP_SET_VAR:
				opline->op2.jmp_addr = &CG(active_op_array)->opcodes[opline->op2.opline_num];
				break;
		}
		ZEND_VM_SET_OPCODE_HANDLER(opline);
		opline++;
	}

	zend_release_labels(1 TSRMLS_CC);

	EG(return_value_ptr_ptr) = NULL;
	EG(active_op_array) = CG(active_op_array);
	orig_interactive = CG(interactive);
	CG(interactive) = 0;
	zend_execute(CG(active_op_array) TSRMLS_CC);
	CG(interactive) = orig_interactive;

	if (EG(exception)) {
		zend_exception_error(EG(exception), E_ERROR TSRMLS_CC);
	}

	CG(active_op_array)->last -= 1;	/* get rid of that ZEND_RETURN */
	EG(start_op) = CG(active_op_array)->opcodes+CG(active_op_array)->last;
}
/* }}} */

ZEND_API void zend_timeout(int dummy) /* {{{ */
{
	TSRMLS_FETCH();

	if (zend_on_timeout) {
#ifdef ZEND_SIGNALS
		/*
		   We got here because we got a timeout signal, so we are in a signal handler
		   at this point. However, we want to be able to timeout any user-supplied
		   shutdown functions, so pretend we are not in a signal handler while we are
		   calling these
		*/
		SIGG(running) = 0;
#endif
		zend_on_timeout(EG(timeout_seconds) TSRMLS_CC);
	}

	zend_error(E_ERROR, "Maximum execution time of %d second%s exceeded", EG(timeout_seconds), EG(timeout_seconds) == 1 ? "" : "s");
}
/* }}} */

#ifdef ZEND_WIN32
static LRESULT CALLBACK zend_timeout_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) /* {{{ */
{
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_REGISTER_ZEND_TIMEOUT:
			/* wParam is the thread id pointer, lParam is the timeout amount in seconds */
			if (lParam == 0) {
				KillTimer(timeout_window, wParam);
			} else {
#ifdef ZTS
				void ***tsrm_ls;
#endif
				SetTimer(timeout_window, wParam, lParam*1000, NULL);
#ifdef ZTS
				tsrm_ls = ts_resource_ex(0, &wParam);
				if (!tsrm_ls) {
					/* shouldn't normally happen */
					break;
				}
#endif
				EG(timed_out) = 0;
			}
			break;
		case WM_UNREGISTER_ZEND_TIMEOUT:
			/* wParam is the thread id pointer */
			KillTimer(timeout_window, wParam);
			break;
		case WM_TIMER: {
#ifdef ZTS
				void ***tsrm_ls;

				tsrm_ls = ts_resource_ex(0, &wParam);
				if (!tsrm_ls) {
					/* Thread died before receiving its timeout? */
					break;
				}
#endif
				KillTimer(timeout_window, wParam);
				EG(timed_out) = 1;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
/* }}} */

static unsigned __stdcall timeout_thread_proc(void *pArgs) /* {{{ */
{
	MSG message;

	wc.style=0;
	wc.lpfnWndProc = zend_timeout_WndProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=NULL;
	wc.hIcon=NULL;
	wc.hCursor=NULL;
	wc.hbrBackground=(HBRUSH)(COLOR_BACKGROUND + 5);
	wc.lpszMenuName=NULL;
	wc.lpszClassName = "Zend Timeout Window";
	if (!RegisterClass(&wc)) {
		return -1;
	}
	timeout_window = CreateWindow(wc.lpszClassName, wc.lpszClassName, 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);
	SetEvent(timeout_thread_event);
	while (GetMessage(&message, NULL, 0, 0)) {
		SendMessage(timeout_window, message.message, message.wParam, message.lParam);
		if (message.message == WM_QUIT) {
			break;
		}
	}
	DestroyWindow(timeout_window);
	UnregisterClass(wc.lpszClassName, NULL);
	SetEvent(timeout_thread_handle);
	return 0;
}
/* }}} */

void zend_init_timeout_thread(void) /* {{{ */
{
	timeout_thread_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	timeout_thread_handle = CreateEvent(NULL, FALSE, FALSE, NULL);
	_beginthreadex(NULL, 0, timeout_thread_proc, NULL, 0, &timeout_thread_id);
	WaitForSingleObject(timeout_thread_event, INFINITE);
}
/* }}} */

void zend_shutdown_timeout_thread(void) /* {{{ */
{
	if (!timeout_thread_initialized) {
		return;
	}
	PostThreadMessage(timeout_thread_id, WM_QUIT, 0, 0);

	/* Wait for thread termination */
	WaitForSingleObject(timeout_thread_handle, 5000);
	CloseHandle(timeout_thread_handle);
	timeout_thread_initialized = 0;
}
/* }}} */

#endif

/* This one doesn't exists on QNX */
#ifndef SIGPROF
#define SIGPROF 27
#endif

void zend_set_timeout(long seconds, int reset_signals) /* {{{ */
{
	TSRMLS_FETCH();

	EG(timeout_seconds) = seconds;

#ifdef ZEND_WIN32
	if(!seconds) {
		return;
	}
	if (timeout_thread_initialized == 0 && InterlockedIncrement(&timeout_thread_initialized) == 1) {
		/* We start up this process-wide thread here and not in zend_startup(), because if Zend
		 * is initialized inside a DllMain(), you're not supposed to start threads from it.
		 */
		zend_init_timeout_thread();
	}
	PostThreadMessage(timeout_thread_id, WM_REGISTER_ZEND_TIMEOUT, (WPARAM) GetCurrentThreadId(), (LPARAM) seconds);
#else
#	ifdef HAVE_SETITIMER
	{
		struct itimerval t_r;		/* timeout requested */
		int signo;

		if(seconds) {
			t_r.it_value.tv_sec = seconds;
			t_r.it_value.tv_usec = t_r.it_interval.tv_sec = t_r.it_interval.tv_usec = 0;

#	ifdef __CYGWIN__
			setitimer(ITIMER_REAL, &t_r, NULL);
		}
		signo = SIGALRM;
#	else
			setitimer(ITIMER_PROF, &t_r, NULL);
		}
		signo = SIGPROF;
#	endif

		if (reset_signals) {
#	ifdef ZEND_SIGNALS
			zend_signal(signo, zend_timeout TSRMLS_CC);
#	else
			sigset_t sigset;

			signal(signo, zend_timeout);
			sigemptyset(&sigset);
			sigaddset(&sigset, signo);
			sigprocmask(SIG_UNBLOCK, &sigset, NULL);
#	endif
		}
	}
#	endif /* HAVE_SETITIMER */
#endif
}
/* }}} */

void zend_unset_timeout(TSRMLS_D) /* {{{ */
{
#ifdef ZEND_WIN32
	if(timeout_thread_initialized) {
		PostThreadMessage(timeout_thread_id, WM_UNREGISTER_ZEND_TIMEOUT, (WPARAM) GetCurrentThreadId(), (LPARAM) 0);
	}
#else
#	ifdef HAVE_SETITIMER
	if (EG(timeout_seconds)) {
		struct itimerval no_timeout;

		no_timeout.it_value.tv_sec = no_timeout.it_value.tv_usec = no_timeout.it_interval.tv_sec = no_timeout.it_interval.tv_usec = 0;

#ifdef __CYGWIN__
		setitimer(ITIMER_REAL, &no_timeout, NULL);
#else
		setitimer(ITIMER_PROF, &no_timeout, NULL);
#endif
	}
#	endif
#endif
}
/* }}} */

zend_class_entry *zend_fetch_class(const char *class_name, uint class_name_len, int fetch_type TSRMLS_DC) /* {{{ */
{
	zend_class_entry **pce;
	int use_autoload = (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) == 0;
	int silent       = (fetch_type & ZEND_FETCH_CLASS_SILENT) != 0;

	fetch_type &= ZEND_FETCH_CLASS_MASK;

check_fetch_type:
	switch (fetch_type) {
		case ZEND_FETCH_CLASS_SELF:
			if (!EG(scope)) {
				zend_error(E_ERROR, "Cannot access self:: when no class scope is active");
			}
			return EG(scope);
		case ZEND_FETCH_CLASS_PARENT:
			if (!EG(scope)) {
				zend_error(E_ERROR, "Cannot access parent:: when no class scope is active");
			}
			if (!EG(scope)->parent) {
				zend_error(E_ERROR, "Cannot access parent:: when current class scope has no parent");
			}
			return EG(scope)->parent;
		case ZEND_FETCH_CLASS_STATIC:
			if (!EG(called_scope)) {
				zend_error(E_ERROR, "Cannot access static:: when no class scope is active");
			}
			return EG(called_scope);
		case ZEND_FETCH_CLASS_AUTO: {
				fetch_type = zend_get_class_fetch_type(class_name, class_name_len);
				if (fetch_type!=ZEND_FETCH_CLASS_DEFAULT) {
					goto check_fetch_type;
				}
			}
			break;
	}

	if (zend_lookup_class_ex(class_name, class_name_len, NULL, use_autoload, &pce TSRMLS_CC) == FAILURE) {
		if (use_autoload) {
			if (!silent && !EG(exception)) {
				if (fetch_type == ZEND_FETCH_CLASS_INTERFACE) {
					zend_error(E_ERROR, "Interface '%s' not found", class_name);
				} else if (fetch_type == ZEND_FETCH_CLASS_TRAIT) {
                	zend_error(E_ERROR, "Trait '%s' not found", class_name);
                } else {
					zend_error(E_ERROR, "Class '%s' not found", class_name);
				}
			}
		}
		return NULL;
	}
	return *pce;
}
/* }}} */

zend_class_entry *zend_fetch_class_by_name(const char *class_name, uint class_name_len, const zend_literal *key, int fetch_type TSRMLS_DC) /* {{{ */
{
	zend_class_entry **pce;
	int use_autoload = (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) == 0;

	if (zend_lookup_class_ex(class_name, class_name_len, key, use_autoload, &pce TSRMLS_CC) == FAILURE) {
		if (use_autoload) {
			if ((fetch_type & ZEND_FETCH_CLASS_SILENT) == 0 && !EG(exception)) {
				if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_INTERFACE) {
					zend_error(E_ERROR, "Interface '%s' not found", class_name);
				} else if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_TRAIT) {
					zend_error(E_ERROR, "Trait '%s' not found", class_name);
				} else {
					zend_error(E_ERROR, "Class '%s' not found", class_name);
				}
			}
		}
		return NULL;
	}
	return *pce;
}
/* }}} */

#define MAX_ABSTRACT_INFO_CNT 3
#define MAX_ABSTRACT_INFO_FMT "%s%s%s%s"
#define DISPLAY_ABSTRACT_FN(idx) \
	ai.afn[idx] ? ZEND_FN_SCOPE_NAME(ai.afn[idx]) : "", \
	ai.afn[idx] ? "::" : "", \
	ai.afn[idx] ? ai.afn[idx]->common.function_name : "", \
	ai.afn[idx] && ai.afn[idx + 1] ? ", " : (ai.afn[idx] && ai.cnt > MAX_ABSTRACT_INFO_CNT ? ", ..." : "")

typedef struct _zend_abstract_info {
	zend_function *afn[MAX_ABSTRACT_INFO_CNT + 1];
	int cnt;
	int ctor;
} zend_abstract_info;

static int zend_verify_abstract_class_function(zend_function *fn, zend_abstract_info *ai TSRMLS_DC) /* {{{ */
{
	if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
		if (ai->cnt < MAX_ABSTRACT_INFO_CNT) {
			ai->afn[ai->cnt] = fn;
		}
		if (fn->common.fn_flags & ZEND_ACC_CTOR) {
			if (!ai->ctor) {
				ai->cnt++;
				ai->ctor = 1;
			} else {
				ai->afn[ai->cnt] = NULL;
			}
		} else {
			ai->cnt++;
		}
	}
	return 0;
}
/* }}} */

void zend_verify_abstract_class(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	zend_abstract_info ai;

	if ((ce->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS) && !(ce->ce_flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		memset(&ai, 0, sizeof(ai));

		zend_hash_apply_with_argument(&ce->function_table, (apply_func_arg_t) zend_verify_abstract_class_function, &ai TSRMLS_CC);

		if (ai.cnt) {
			zend_error(E_ERROR, "Class %s contains %d abstract method%s and must therefore be declared abstract or implement the remaining methods (" MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT ")",
				ce->name, ai.cnt,
				ai.cnt > 1 ? "s" : "",
				DISPLAY_ABSTRACT_FN(0),
				DISPLAY_ABSTRACT_FN(1),
				DISPLAY_ABSTRACT_FN(2)
				);
		}
	}
}
/* }}} */

ZEND_API void zend_reset_all_cv(HashTable *symbol_table TSRMLS_DC) /* {{{ */
{
	zend_execute_data *ex;
	int i;

	for (ex = EG(current_execute_data); ex; ex = ex->prev_execute_data) {
		if (ex->op_array && ex->symbol_table == symbol_table) {
			for (i = 0; i < ex->op_array->last_var; i++) {
				*EX_CV_NUM(ex, i) = NULL;
			}
		}
	}
}
/* }}} */

ZEND_API void zend_delete_variable(zend_execute_data *ex, HashTable *ht, const char *name, int name_len, ulong hash_value TSRMLS_DC) /* {{{ */
{
	if (zend_hash_quick_del(ht, name, name_len, hash_value) == SUCCESS) {
		name_len--;
		while (ex && ex->symbol_table == ht) {
			int i;

			if (ex->op_array) {
				for (i = 0; i < ex->op_array->last_var; i++) {
					if (ex->op_array->vars[i].hash_value == hash_value &&
						ex->op_array->vars[i].name_len == name_len &&
						!memcmp(ex->op_array->vars[i].name, name, name_len)) {
						*EX_CV_NUM(ex, i) = NULL;
						break;
					}
				}
			}
			ex = ex->prev_execute_data;
		}
	}
}
/* }}} */

ZEND_API int zend_delete_global_variable_ex(const char *name, int name_len, ulong hash_value TSRMLS_DC) /* {{{ */
{
	zend_execute_data *ex;

	if (zend_hash_quick_exists(&EG(symbol_table), name, name_len + 1, hash_value)) {
		for (ex = EG(current_execute_data); ex; ex = ex->prev_execute_data) {
			if (ex->op_array && ex->symbol_table == &EG(symbol_table)) {
				int i;
				for (i = 0; i < ex->op_array->last_var; i++) {
					if (ex->op_array->vars[i].hash_value == hash_value &&
						ex->op_array->vars[i].name_len == name_len &&
						!memcmp(ex->op_array->vars[i].name, name, name_len)
					) {
						*EX_CV_NUM(ex, i) = NULL;
						break;
					}
				}
			}
		}
		return zend_hash_quick_del(&EG(symbol_table), name, name_len + 1, hash_value);
	}
	return FAILURE;
}
/* }}} */

ZEND_API int zend_delete_global_variable(const char *name, int name_len TSRMLS_DC) /* {{{ */
{
	return zend_delete_global_variable_ex(name, name_len, zend_inline_hash_func(name, name_len + 1) TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_rebuild_symbol_table(TSRMLS_D) /* {{{ */
{
	zend_uint i;
	zend_execute_data *ex;

	if (!EG(active_symbol_table)) {

		/* Search for last called user function */
		ex = EG(current_execute_data);
		while (ex && !ex->op_array) {
			ex = ex->prev_execute_data;
		}
		if (ex && ex->symbol_table) {
			EG(active_symbol_table) = ex->symbol_table;
			return;
		}

		if (ex && ex->op_array) {
			if (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
				/*printf("Cache hit!  Reusing %x\n", symtable_cache[symtable_cache_ptr]);*/
				EG(active_symbol_table) = *(EG(symtable_cache_ptr)--);
			} else {
				ALLOC_HASHTABLE(EG(active_symbol_table));
				zend_hash_init(EG(active_symbol_table), ex->op_array->last_var, NULL, ZVAL_PTR_DTOR, 0);
				/*printf("Cache miss!  Initialized %x\n", EG(active_symbol_table));*/
			}
			ex->symbol_table = EG(active_symbol_table);

			if (ex->op_array->this_var != -1 &&
			    !*EX_CV_NUM(ex, ex->op_array->this_var) &&
			    EG(This)) {
			    *EX_CV_NUM(ex, ex->op_array->this_var) = (zval**)EX_CV_NUM(ex, ex->op_array->last_var + ex->op_array->this_var);
				**EX_CV_NUM(ex, ex->op_array->this_var) = EG(This);
 			}
			for (i = 0; i < ex->op_array->last_var; i++) {
				if (*EX_CV_NUM(ex, i)) {
					zend_hash_quick_update(EG(active_symbol_table),
						ex->op_array->vars[i].name,
						ex->op_array->vars[i].name_len + 1,
						ex->op_array->vars[i].hash_value,
						(void**)*EX_CV_NUM(ex, i),
						sizeof(zval*),
						(void**)EX_CV_NUM(ex, i));
				}
			}
		}
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
