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
#include "zend_stack.h"
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
ZEND_API void (*zend_execute_internal)(zend_execute_data *execute_data_ptr, zend_fcall_info *fci TSRMLS_DC);

/* true globals */
ZEND_API const zend_fcall_info empty_fcall_info = { 0, NULL, {{0},0}, NULL, NULL, 0, NULL, NULL, 0 };
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

static int clean_non_persistent_function(zval *zv TSRMLS_DC) /* {{{ */
{
	zend_function *function = Z_PTR_P(zv);
	return (function->type == ZEND_INTERNAL_FUNCTION) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static int clean_non_persistent_function_full(zval *zv TSRMLS_DC) /* {{{ */
{
	zend_function *function = Z_PTR_P(zv);
	return (function->type == ZEND_INTERNAL_FUNCTION) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static int clean_non_persistent_class(zval *zv TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = Z_PTR_P(zv);
	return (ce->type == ZEND_INTERNAL_CLASS) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static int clean_non_persistent_class_full(zval *zv TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = Z_PTR_P(zv);
	return (ce->type == ZEND_INTERNAL_CLASS) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

void init_executor(TSRMLS_D) /* {{{ */
{
	zval tmp;
	zend_init_fpu(TSRMLS_C);

	ZVAL_NULL(&EG(uninitialized_zval));
	/* trick to make uninitialized_zval never be modified, passed by ref, etc. */
	ZVAL_NULL(&EG(error_zval));
/* destroys stack frame, therefore makes core dumps worthless */
#if 0&&ZEND_DEBUG
	original_sigsegv_handler = signal(SIGSEGV, zend_handle_sigsegv);
#endif

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
	ZVAL_LONG(&tmp, 0);
	zend_vm_stack_push(&tmp TSRMLS_CC);

	zend_hash_init(&EG(symbol_table).ht, 50, NULL, ZVAL_PTR_DTOR, 0);
	EG(active_symbol_table) = &EG(symbol_table);

	zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_activator TSRMLS_CC);
	EG(opline_ptr) = NULL;

	zend_hash_init(&EG(included_files), 5, NULL, NULL, 0);

	EG(ticks_count) = 0;

	ZVAL_UNDEF(&EG(user_error_handler));

	EG(current_execute_data) = NULL;

	zend_stack_init(&EG(user_error_handlers_error_reporting));
	zend_stack_init(&EG(user_error_handlers));
	zend_stack_init(&EG(user_exception_handlers));

	zend_objects_store_init(&EG(objects_store), 1024);

	EG(full_tables_cleanup) = 0;
#ifdef ZEND_WIN32
	EG(timed_out) = 0;
#endif

	EG(exception) = NULL;
	EG(prev_exception) = NULL;

	EG(scope) = NULL;
	EG(called_scope) = NULL;

	ZVAL_OBJ(&EG(This), NULL);

	EG(active_op_array) = NULL;

	EG(active) = 1;
	EG(start_op) = NULL;
}
/* }}} */

static int zval_call_destructor(zval *zv TSRMLS_DC) /* {{{ */
{
	if (Z_TYPE_P(zv) == IS_INDIRECT) {
		zv = Z_INDIRECT_P(zv);
	}
	if (Z_TYPE_P(zv) == IS_OBJECT && Z_REFCOUNT_P(zv) == 1) {
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}
/* }}} */

static void zend_unclean_zval_ptr_dtor(zval *zv) /* {{{ */
{
	TSRMLS_FETCH();

	if (Z_TYPE_P(zv) == IS_INDIRECT) {
		zv = Z_INDIRECT_P(zv);
	}
	i_zval_ptr_dtor(zv ZEND_FILE_LINE_CC TSRMLS_CC);
}
/* }}} */

void shutdown_destructors(TSRMLS_D) /* {{{ */
{
	if (CG(unclean_shutdown)) {
		EG(symbol_table).ht.pDestructor = zend_unclean_zval_ptr_dtor;
	}
	zend_try {
		int symbols;
		do {
			symbols = zend_hash_num_elements(&EG(symbol_table).ht);
			zend_hash_reverse_apply(&EG(symbol_table).ht, (apply_func_t) zval_call_destructor TSRMLS_CC);
		} while (symbols != zend_hash_num_elements(&EG(symbol_table).ht));
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

		if (CG(unclean_shutdown)) {
			EG(symbol_table).ht.pDestructor = zend_unclean_zval_ptr_dtor;
		}
		zend_hash_graceful_reverse_destroy(&EG(symbol_table).ht);
	} zend_end_try();

	zend_try {
		zval *zeh;
		/* remove error handlers before destroying classes and functions,
		 * so that if handler used some class, crash would not happen */
		if (Z_TYPE(EG(user_error_handler)) != IS_UNDEF) {
			zeh = &EG(user_error_handler);
			zval_ptr_dtor(zeh);
			ZVAL_UNDEF(&EG(user_error_handler));
		}

		if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
			zeh = &EG(user_exception_handler);
			zval_ptr_dtor(zeh);
			ZVAL_UNDEF(&EG(user_exception_handler));
		}

		zend_stack_destroy(&EG(user_error_handlers_error_reporting));
		zend_stack_init(&EG(user_error_handlers_error_reporting));
		zend_stack_clean(&EG(user_error_handlers), (void (*)(void *))ZVAL_DESTRUCTOR, 1);
		zend_stack_clean(&EG(user_exception_handlers), (void (*)(void *))ZVAL_DESTRUCTOR, 1);
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
			zend_hash_apply(EG(function_table), zend_cleanup_function_data_full TSRMLS_CC);
			zend_hash_apply(EG(class_table), zend_cleanup_class_data TSRMLS_CC);
		} else {
			zend_hash_reverse_apply(EG(function_table), zend_cleanup_function_data TSRMLS_CC);
			zend_hash_reverse_apply(EG(class_table), zend_cleanup_user_class_data TSRMLS_CC);
			zend_cleanup_internal_classes(TSRMLS_C);
		}
	} zend_end_try();

	zend_try {
		zend_llist_destroy(&CG(open_files));
	} zend_end_try();

	zend_try {
		zend_close_rsrc_list(&EG(regular_list) TSRMLS_CC);
	} zend_end_try();

	zend_try {
		zend_objects_store_free_object_storage(&EG(objects_store) TSRMLS_CC);

		zend_vm_stack_destroy(TSRMLS_C);

		/* Destroy all op arrays */
		if (EG(full_tables_cleanup)) {
			zend_hash_reverse_apply(EG(function_table), clean_non_persistent_function_full TSRMLS_CC);
			zend_hash_reverse_apply(EG(class_table), clean_non_persistent_class_full TSRMLS_CC);
		} else {
			zend_hash_reverse_apply(EG(function_table), clean_non_persistent_function TSRMLS_CC);
			zend_hash_reverse_apply(EG(class_table), clean_non_persistent_class TSRMLS_CC);
		}

		while (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
			zend_hash_destroy(&(*EG(symtable_cache_ptr))->ht);
			efree(*EG(symtable_cache_ptr));
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
		zend_stack_destroy(&EG(user_error_handlers));
		zend_stack_destroy(&EG(user_exception_handlers));
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
			return ce ? ce->name->val : "";
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
				zend_string *function_name = ((zend_op_array *) EG(current_execute_data)->function_state.function)->function_name;

				if (function_name) {
					return function_name->val;
				} else {
					return "main";
				}
			}
			break;
		case ZEND_INTERNAL_FUNCTION:
			return ((zend_internal_function *) EG(current_execute_data)->function_state.function)->function_name->val;
			break;
		default:
			return NULL;
	}
}
/* }}} */

ZEND_API const char *zend_get_executed_filename(TSRMLS_D) /* {{{ */
{
	if (EG(active_op_array)) {
		return EG(active_op_array)->filename->val;
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

ZEND_API void _zval_ptr_dtor(zval *zval_ptr ZEND_FILE_LINE_DC) /* {{{ */
{
	TSRMLS_FETCH();
	i_zval_ptr_dtor(zval_ptr ZEND_FILE_LINE_RELAY_CC TSRMLS_CC);
}
/* }}} */

ZEND_API void _zval_internal_ptr_dtor(zval *zval_ptr ZEND_FILE_LINE_DC) /* {{{ */
{
	if (Z_REFCOUNTED_P(zval_ptr)) {
		Z_DELREF_P(zval_ptr);
		if (Z_REFCOUNT_P(zval_ptr) == 0) {
			_zval_internal_dtor_for_ptr(zval_ptr ZEND_FILE_LINE_CC);
		}
	}
}
/* }}} */

ZEND_API int zend_is_true(zval *op TSRMLS_DC) /* {{{ */
{
	return i_zend_is_true(op TSRMLS_CC);
}
/* }}} */

#include "../TSRM/tsrm_strtok_r.h"

#define IS_VISITED_CONSTANT			0x080
#define IS_CONSTANT_VISITED(p)		(Z_TYPE_P(p) & IS_VISITED_CONSTANT)
#define Z_REAL_TYPE_P(p)			(Z_TYPE_P(p) & ~IS_VISITED_CONSTANT)
#define MARK_CONSTANT_VISITED(p)	Z_TYPE_INFO_P(p) |= IS_VISITED_CONSTANT

ZEND_API int zval_update_constant_ex(zval *p, void *arg, zend_class_entry *scope TSRMLS_DC) /* {{{ */
{
	zend_bool inline_change = (zend_bool) (zend_uintptr_t) arg;
	zval const_value;
	char *colon;

	if (IS_CONSTANT_VISITED(p)) {
		zend_error(E_ERROR, "Cannot declare self-referencing constant '%s'", Z_STRVAL_P(p));
	} else if (Z_TYPE_P(p) == IS_CONSTANT) {
		int refcount;

		SEPARATE_ZVAL_IF_NOT_REF(p);
		MARK_CONSTANT_VISITED(p);
		refcount =  Z_REFCOUNTED_P(p) ? Z_REFCOUNT_P(p) : 1;
		if (!zend_get_constant_ex(Z_STRVAL_P(p), Z_STRLEN_P(p), &const_value, scope, Z_CONST_FLAGS_P(p) TSRMLS_CC)) {
			char *actual = Z_STRVAL_P(p);

			if ((colon = (char*)zend_memrchr(Z_STRVAL_P(p), ':', Z_STRLEN_P(p)))) {
				int len;

				zend_error(E_ERROR, "Undefined class constant '%s'", Z_STRVAL_P(p));
				len = Z_STRLEN_P(p) - ((colon - Z_STRVAL_P(p)) + 1);
				if (inline_change) {
					zend_string *tmp = STR_INIT(colon + 1, len, 0);
					STR_RELEASE(Z_STR_P(p));
					Z_STR_P(p) = tmp;
				} else {
					Z_STR_P(p) = STR_INIT(colon + 1, len, 0);
				}
				Z_TYPE_FLAGS_P(p) = IS_TYPE_REFCOUNTED | IS_TYPE_COPYABLE;
			} else {
				zend_string *save = Z_STR_P(p);
				char *slash;
				int actual_len = Z_STRLEN_P(p);
				if ((Z_CONST_FLAGS_P(p) & IS_CONSTANT_UNQUALIFIED) && (slash = (char *)zend_memrchr(actual, '\\', actual_len))) {
					actual = slash + 1;
					actual_len -= (actual - Z_STRVAL_P(p));
					if (inline_change) {
						zend_string *s = STR_INIT(actual, actual_len, 0);
						Z_STR_P(p) = s;
						Z_TYPE_FLAGS_P(p) = IS_TYPE_REFCOUNTED | IS_TYPE_COPYABLE;
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
				if ((Z_CONST_FLAGS_P(p) & IS_CONSTANT_UNQUALIFIED) == 0) {
					if (save->val[0] == '\\') {
						zend_error(E_ERROR, "Undefined constant '%s'", save->val + 1);
					} else {
						zend_error(E_ERROR, "Undefined constant '%s'", save->val);
					}
					if (inline_change) {
						STR_RELEASE(save);
					}
					save = NULL;
				}
				zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",  actual,  actual);
				if (!inline_change) {
					ZVAL_STRINGL(p, actual, actual_len);
				} else {
					Z_TYPE_INFO_P(p) = IS_INTERNED(Z_STR_P(p)) ?
						IS_INTERNED_STRING_EX : IS_STRING_EX;
					if (save && save->val != actual) {
						STR_RELEASE(save);
					}
				}
			}
		} else {
			if (inline_change) {
				STR_RELEASE(Z_STR_P(p));
			}
			ZVAL_COPY_VALUE(p, &const_value);
		}

		if (Z_REFCOUNTED_P(p)) Z_SET_REFCOUNT_P(p, refcount);
	} else if (Z_TYPE_P(p) == IS_CONSTANT_ARRAY) {
		zval *element, new_val;
		zend_string *str_index;
		ulong num_index;
		int ret;

		SEPARATE_ZVAL_IF_NOT_REF(p);

		Z_TYPE_INFO_P(p) = IS_ARRAY_EX;
		if (!inline_change) {
			HashTable *ht = Z_ARRVAL_P(p);
			ZVAL_NEW_ARR(p);
			zend_hash_init(Z_ARRVAL_P(p), zend_hash_num_elements(ht), NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_copy(Z_ARRVAL_P(p), ht, ZVAL_COPY_CTOR);
		}

		/* First go over the array and see if there are any constant indices */
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(p));
		while ((element = zend_hash_get_current_data(Z_ARRVAL_P(p))) != NULL) {
			if (zend_hash_get_current_key(Z_ARRVAL_P(p), &str_index, &num_index, 0) != HASH_KEY_IS_STRING) {
				zend_hash_move_forward(Z_ARRVAL_P(p));
				continue;
			}
			if (!(GC_FLAGS(str_index) & (IS_STR_CONSTANT | IS_STR_AST))) {
				zend_hash_move_forward(Z_ARRVAL_P(p));
				continue;
			}

			if (GC_FLAGS(str_index) & IS_STR_AST) {
				zend_ast_ref *ast = *(zend_ast_ref **)str_index->val;

				zend_ast_evaluate(&const_value, ast->ast, scope TSRMLS_CC);
				zend_ast_destroy(ast->ast);
				efree(ast);
			} else if (!zend_get_constant_ex(str_index->val, str_index->len, &const_value, scope, GC_FLAGS(str_index) & ~(IS_STR_PERSISTENT | IS_STR_INTERNED |IS_STR_PERMANENT) TSRMLS_CC)) {
				char *actual, *str;
				const char *save = str_index->val;
				int len;

				str = str_index->val;
				len = str_index->len;
				if ((colon = (char*)zend_memrchr(str, ':', len))) {
					zend_error(E_ERROR, "Undefined class constant '%s'", str);
					len -= ((colon - str) + 1);
					str = colon;
				} else {
					if (GC_FLAGS(str_index) & IS_STR_CONSTANT_UNQUALIFIED) {
						if ((actual = (char *)zend_memrchr(str, '\\', len))) {
							actual++;
							len -= (actual - str);
							str = actual;
						}
					}
					if (str[0] == '\\') {
						++str;
						--len;
					}
					if (save[0] == '\\') {
						++save;
					}
					if (!(GC_FLAGS(str_index) & IS_STR_CONSTANT_UNQUALIFIED)) {
						zend_error(E_ERROR, "Undefined constant '%s'", save);
					}
					zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",	str, str);
				}
				if (str == str_index->val && len == str_index->len) {
					ZVAL_STR(&const_value, STR_COPY(str_index));
				} else {
					ZVAL_STRINGL(&const_value, str, len);
				}
			}

			if (Z_REFCOUNTED_P(element) && Z_REFCOUNT_P(element) > 1) {
				ZVAL_DUP(&new_val, element);
				zval_ptr_dtor(element);
				ZVAL_COPY_VALUE(element, &new_val);
			}

			switch (Z_TYPE(const_value)) {
				case IS_STRING:
					ret = zend_symtable_update_current_key_ex(Z_ARRVAL_P(p), Z_STR(const_value), HASH_UPDATE_KEY_IF_BEFORE);
					break;
				case IS_BOOL:
				case IS_LONG:
					ret = zend_hash_update_current_key_ex(Z_ARRVAL_P(p), HASH_KEY_IS_LONG, NULL, Z_LVAL(const_value), HASH_UPDATE_KEY_IF_BEFORE);
					break;
				case IS_DOUBLE:
					ret = zend_hash_update_current_key_ex(Z_ARRVAL_P(p), HASH_KEY_IS_LONG, NULL, zend_dval_to_lval(Z_DVAL(const_value)), HASH_UPDATE_KEY_IF_BEFORE);
					break;
				case IS_NULL:
					ret = zend_hash_update_current_key_ex(Z_ARRVAL_P(p), HASH_KEY_IS_STRING, STR_EMPTY_ALLOC(), 0, HASH_UPDATE_KEY_IF_BEFORE);
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
		SEPARATE_ZVAL_IF_NOT_REF(p);

		zend_ast_evaluate(&const_value, Z_ASTVAL_P(p), scope TSRMLS_CC);
		if (inline_change) {
			zend_ast_destroy(Z_ASTVAL_P(p));
			efree(Z_AST_P(p));
		}
		ZVAL_COPY_VALUE(p, &const_value);
	}
	return 0;
}
/* }}} */

ZEND_API int zval_update_constant_inline_change(zval *pp, void *scope TSRMLS_DC) /* {{{ */
{
	return zval_update_constant_ex(pp, (void*)1, scope TSRMLS_CC);
}
/* }}} */

ZEND_API int zval_update_constant_no_inline_change(zval *pp, void *scope TSRMLS_DC) /* {{{ */
{
	return zval_update_constant_ex(pp, (void*)0, scope TSRMLS_CC);
}
/* }}} */

ZEND_API int zval_update_constant(zval *pp, void *arg TSRMLS_DC) /* {{{ */
{
	return zval_update_constant_ex(pp, arg, NULL TSRMLS_CC);
}
/* }}} */

int call_user_function(HashTable *function_table, zval *object, zval *function_name, zval *retval_ptr, zend_uint param_count, zval params[] TSRMLS_DC) /* {{{ */
{
	zval *params_array;
	zend_uint i;
	int ex_retval;

	if (param_count) {
		params_array = (zval *) emalloc(sizeof(zval) * param_count);
		for (i=0; i<param_count; i++) {
			ZVAL_COPY_VALUE(&params_array[i], &params[i]);
		}
	} else {
		params_array = NULL;
	}
	ZVAL_UNDEF(retval_ptr);
	ex_retval = call_user_function_ex(function_table, object, function_name, retval_ptr, param_count, params_array, 1, NULL TSRMLS_CC);
	if (params_array) {
		efree(params_array);
	}
	return ex_retval;
}
/* }}} */

int call_user_function_ex(HashTable *function_table, zval *object, zval *function_name, zval *retval_ptr, zend_uint param_count, zval params[], int no_separation, zend_array *symbol_table TSRMLS_DC) /* {{{ */
{
	zend_fcall_info fci;

	fci.size = sizeof(fci);
	fci.function_table = function_table;
	fci.object = object ? Z_OBJ_P(object) : NULL;
	ZVAL_COPY_VALUE(&fci.function_name, function_name);
	fci.retval = retval_ptr;
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
	zend_array *calling_symbol_table;
	zend_op_array *original_op_array;
	zend_op **original_opline_ptr;
	zend_class_entry *current_scope;
	zend_class_entry *current_called_scope;
	zend_class_entry *calling_scope = NULL;
	zend_class_entry *called_scope = NULL;
	zend_object *current_this;
	zend_execute_data execute_data;
	zend_fcall_info_cache fci_cache_local;
	zval tmp;

	ZVAL_UNDEF(fci->retval);

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
		zend_string *callable_name;
		char *error = NULL;

		if (!fci_cache) {
			fci_cache = &fci_cache_local;
		}

		if (!zend_is_callable_ex(&fci->function_name, fci->object, IS_CALLABLE_CHECK_SILENT, &callable_name, fci_cache, &error TSRMLS_CC)) {
			if (error) {
				zend_error(E_WARNING, "Invalid callback %s, %s", callable_name->val, error);
				efree(error);
			}
			if (callable_name) {
				STR_RELEASE(callable_name);
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
		STR_RELEASE(callable_name);
	}

	EX(function_state).function = fci_cache->function_handler;
	calling_scope = fci_cache->calling_scope;
	called_scope = fci_cache->called_scope;
	EX(object) = fci->object = fci_cache->object;
	if (fci->object &&
	    (!EG(objects_store).object_buckets ||
	     !IS_OBJ_VALID(EG(objects_store).object_buckets[fci->object->handle]))) {
		return FAILURE;
	}

	if (EX(function_state).function->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_DEPRECATED)) {
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_ABSTRACT) {
			zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EX(function_state).function->common.scope->name->val, EX(function_state).function->common.function_name->val);
		}
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_DEPRECATED) {
 			zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
				EX(function_state).function->common.scope ? EX(function_state).function->common.scope->name->val : "",
				EX(function_state).function->common.scope ? "::" : "",
				EX(function_state).function->common.function_name->val);
		}
	}

	ZEND_VM_STACK_GROW_IF_NEEDED(fci->param_count + 1);

	for (i=0; i<fci->param_count; i++) {
		zval *param;

		if (ARG_SHOULD_BE_SENT_BY_REF(EX(function_state).function, i + 1)) {
			if (!Z_REFCOUNTED(fci->params[i]) ||
			    (!Z_ISREF(fci->params[i]) && Z_REFCOUNT(fci->params[i]) > 1)) {

				if (fci->no_separation &&
					!ARG_MAY_BE_SENT_BY_REF(EX(function_state).function, i + 1)) {
					if (i || UNEXPECTED(ZEND_VM_STACK_ELEMETS(EG(argument_stack)) == (EG(argument_stack)->top))) {
						/* hack to clean up the stack */
						ZVAL_LONG(&tmp, i);
						zend_vm_stack_push(&tmp TSRMLS_CC);
						zend_vm_stack_clear_multiple(0 TSRMLS_CC);
					}

					zend_error(E_WARNING, "Parameter %d to %s%s%s() expected to be a reference, value given",
						i+1,
						EX(function_state).function->common.scope ? EX(function_state).function->common.scope->name->val : "",
						EX(function_state).function->common.scope ? "::" : "",
						EX(function_state).function->common.function_name->val);
					return FAILURE;
				}

				if (Z_REFCOUNTED(fci->params[i])) {
					Z_DELREF(fci->params[i]);
				}
				ZVAL_DUP(&tmp, &fci->params[i]);
				ZVAL_NEW_REF(&fci->params[i], &tmp);
				Z_ADDREF(fci->params[i]);
			} else if (!Z_ISREF(fci->params[i])) {
				ZVAL_NEW_REF(&fci->params[i], &fci->params[i]);
				Z_ADDREF(fci->params[i]);
			} else if (Z_REFCOUNTED(fci->params[i])) {
				Z_ADDREF(fci->params[i]);
			}
			param = &fci->params[i];
		} else if (Z_ISREF(fci->params[i]) &&
		           /* don't separate references for __call */
		           (EX(function_state).function->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) == 0 ) {
			param = &tmp;
			ZVAL_DUP(param, Z_REFVAL(fci->params[i]));
		} else {
			param = &tmp;
			ZVAL_COPY(param, &fci->params[i]);
		}
		zend_vm_stack_push(param TSRMLS_CC);
	}

	EX(function_state).arguments = zend_vm_stack_top(TSRMLS_C);
	ZVAL_LONG(&tmp, fci->param_count);
	zend_vm_stack_push(&tmp TSRMLS_CC);

	current_scope = EG(scope);
	EG(scope) = calling_scope;

	current_this = Z_OBJ(EG(This));

	current_called_scope = EG(called_scope);
	if (called_scope) {
		EG(called_scope) = called_scope;
	} else if (EX(function_state).function->type != ZEND_INTERNAL_FUNCTION) {
		EG(called_scope) = NULL;
	}

	if (!fci->object ||
	    (EX(function_state).function->common.fn_flags & ZEND_ACC_STATIC)) {
		Z_OBJ(EG(This)) = NULL;
	} else {
		Z_OBJ(EG(This)) = fci->object;
		Z_ADDREF(EG(This));
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

		original_op_array = EG(active_op_array);
		EG(active_op_array) = (zend_op_array *) EX(function_state).function;
		original_opline_ptr = EG(opline_ptr);

		if (EG(active_op_array)->fn_flags & ZEND_ACC_GENERATOR) {
			zend_generator_create_zval(EG(active_op_array), fci->retval TSRMLS_CC);
		} else {
			zend_execute(EG(active_op_array), fci->retval TSRMLS_CC);
		}

		if (!fci->symbol_table && EG(active_symbol_table)) {
			zend_clean_and_cache_symbol_table(EG(active_symbol_table) TSRMLS_CC);
		}
		EG(active_symbol_table) = calling_symbol_table;
		EG(active_op_array) = original_op_array;
		EG(opline_ptr) = original_opline_ptr;
	} else if (EX(function_state).function->type == ZEND_INTERNAL_FUNCTION) {
		int call_via_handler = (EX(function_state).function->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) != 0;
		ZVAL_NULL(fci->retval);
		if (EX(function_state).function->common.scope) {
			EG(scope) = EX(function_state).function->common.scope;
		}
		if (EXPECTED(zend_execute_internal == NULL)) {
			/* saves one function call if zend_execute_internal is not used */
			EX(function_state).function->internal_function.handler(fci->param_count, fci->retval TSRMLS_CC);
		} else {
			zend_execute_internal(&execute_data, fci TSRMLS_CC);
		}
		/*  We shouldn't fix bad extensions here,
			because it can break proper ones (Bug #34045)
		if (!EX(function_state).function->common.return_reference)
		{
			INIT_PZVAL(*fci->retval_ptr_ptr);
		}*/
		if (EG(exception) && fci->retval) {
			zval_ptr_dtor(fci->retval);
			ZVAL_UNDEF(fci->retval);
		}

		if (call_via_handler) {
			/* We must re-initialize function again */
			fci_cache->initialized = 0;
		}
	} else { /* ZEND_OVERLOADED_FUNCTION */
		ZVAL_NULL(fci->retval);

		/* Not sure what should be done here if it's a static method */
		if (fci->object) {
			fci->object->handlers->call_method(EX(function_state).function->common.function_name, fci->object, fci->param_count, fci->retval TSRMLS_CC);
		} else {
			zend_error_noreturn(E_ERROR, "Cannot call overloaded function for non-object");
		}

		if (EX(function_state).function->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			efree((char*)EX(function_state).function->common.function_name);
		}
		efree(EX(function_state).function);

		if (EG(exception) && fci->retval) {
			zval_ptr_dtor(fci->retval);
			ZVAL_UNDEF(fci->retval);
		}
	}
	zend_vm_stack_clear_multiple(0 TSRMLS_CC);

	if (Z_OBJ(EG(This))) {
		zval_ptr_dtor(&EG(This));
	}
	EG(called_scope) = current_called_scope;
	EG(scope) = current_scope;
	Z_OBJ(EG(This)) = current_this;
	EG(current_execute_data) = EX(prev_execute_data);

	if (EG(exception)) {
		zend_throw_exception_internal(NULL TSRMLS_CC);
	}
	return SUCCESS;
}
/* }}} */

ZEND_API zend_class_entry *zend_lookup_class_ex(zend_string *name, const zval *key, int use_autoload TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = NULL;
	zval args[1];
	zval local_retval;
	int retval;
	zend_string *lc_name;
	zend_fcall_info fcall_info;
	zend_fcall_info_cache fcall_cache;

	if (key) {
		lc_name = Z_STR_P(key);
	} else {
		if (name == NULL || !name->len) {
			return NULL;
		}

		if (name->val[0] == '\\') {
			lc_name = STR_ALLOC(name->len - 1, 0);
			zend_str_tolower_copy(lc_name->val, name->val + 1, name->len - 1);
		} else {
			lc_name = STR_ALLOC(name->len, 0);
			zend_str_tolower_copy(lc_name->val, name->val, name->len);
		}
	}

	ce = zend_hash_find_ptr(EG(class_table), lc_name);
	if (ce) {
		if (!key) {
			STR_FREE(lc_name);
		}
		return ce;
	}

	/* The compiler is not-reentrant. Make sure we __autoload() only during run-time
	 * (doesn't impact functionality of __autoload()
	*/
	if (!use_autoload || zend_is_compiling(TSRMLS_C)) {
		if (!key) {
			STR_FREE(lc_name);
		}
		return NULL;
	}

	/* Verify class name before passing it to __autoload() */
	if (strspn(name->val, "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\177\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377\\") != name->len) {
		if (!key) {
			STR_FREE(lc_name);
		}
		return NULL;
	}
	
	if (EG(in_autoload) == NULL) {
		ALLOC_HASHTABLE(EG(in_autoload));
		zend_hash_init(EG(in_autoload), 0, NULL, NULL, 0);
	}

	if (zend_hash_add_empty_element(EG(in_autoload), lc_name) == NULL) {
		if (!key) {
			STR_FREE(lc_name);
		}
		return NULL;
	}

	ZVAL_UNDEF(&local_retval);

	if (name->val[0] == '\\') {
		ZVAL_STRINGL(&args[0], name->val + 1, name->len - 1);
	} else {
		ZVAL_STR(&args[0], STR_COPY(name));
	}

	fcall_info.size = sizeof(fcall_info);
	fcall_info.function_table = EG(function_table);
	ZVAL_STRINGL(&fcall_info.function_name, ZEND_AUTOLOAD_FUNC_NAME, sizeof(ZEND_AUTOLOAD_FUNC_NAME) - 1);
	fcall_info.symbol_table = NULL;
	fcall_info.retval = &local_retval;
	fcall_info.param_count = 1;
	fcall_info.params = args;
	fcall_info.object = NULL;
	fcall_info.no_separation = 1;

	fcall_cache.initialized = EG(autoload_func) ? 1 : 0;
	fcall_cache.function_handler = EG(autoload_func);
	fcall_cache.calling_scope = NULL;
	fcall_cache.called_scope = NULL;
	fcall_cache.object = NULL;

	zend_exception_save(TSRMLS_C);
	retval = zend_call_function(&fcall_info, &fcall_cache TSRMLS_CC);
	zend_exception_restore(TSRMLS_C);

	EG(autoload_func) = fcall_cache.function_handler;

	zval_ptr_dtor(&args[0]);
	zval_dtor(&fcall_info.function_name);

	zend_hash_del(EG(in_autoload), lc_name);

	zval_ptr_dtor(&local_retval);

	if (retval == SUCCESS) {
		ce = zend_hash_find_ptr(EG(class_table), lc_name);
	}
	if (!key) {
		STR_FREE(lc_name);
	}
	return ce;
}
/* }}} */

ZEND_API zend_class_entry *zend_lookup_class(zend_string *name TSRMLS_DC) /* {{{ */
{
	return zend_lookup_class_ex(name, NULL, 1 TSRMLS_CC);
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
		ZVAL_NEW_STR(&pv, STR_ALLOC(str_len + sizeof("return ;")-1, 1));
		memcpy(Z_STRVAL(pv), "return ", sizeof("return ") - 1);
		memcpy(Z_STRVAL(pv) + sizeof("return ") - 1, str, str_len);
		Z_STRVAL(pv)[Z_STRLEN(pv) - 1] = ';';
		Z_STRVAL(pv)[Z_STRLEN(pv)] = '\0';
	} else {
		ZVAL_STRINGL(&pv, str, str_len);
	}

	/*printf("Evaluating '%s'\n", pv.value.str.val);*/

	original_compiler_options = CG(compiler_options);
	CG(compiler_options) = ZEND_COMPILE_DEFAULT_FOR_EVAL;
	new_op_array = zend_compile_string(&pv, string_name TSRMLS_CC);
	CG(compiler_options) = original_compiler_options;

	if (new_op_array) {
		zval local_retval;
		zend_op **original_opline_ptr = EG(opline_ptr);
		int orig_interactive = CG(interactive);

		EG(active_op_array) = new_op_array;
		EG(no_extensions)=1;
		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}
		CG(interactive) = 0;

		zend_try {
			ZVAL_UNDEF(&local_retval);
			zend_execute(new_op_array, &local_retval TSRMLS_CC);
		} zend_catch {
			destroy_op_array(new_op_array TSRMLS_CC);
			efree(new_op_array);
			zend_bailout();
		} zend_end_try();

		CG(interactive) = orig_interactive;
		if (Z_TYPE(local_retval) != IS_UNDEF) {
			if (retval_ptr) {
				ZVAL_COPY_VALUE(retval_ptr, &local_retval);
			} else {
				zval_ptr_dtor(&local_retval);
			}
		} else {
			if (retval_ptr) {
				ZVAL_NULL(retval_ptr);
			}
		}

		EG(no_extensions)=0;
		EG(opline_ptr) = original_opline_ptr;
		EG(active_op_array) = original_active_op_array;
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		retval = SUCCESS;
	} else {
		retval = FAILURE;
	}
	zval_dtor(&pv);
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
			opline->op1.zv = &CG(active_op_array)->literals[opline->op1.constant];
		}
		if (opline->op2_type == IS_CONST) {
			opline->op2.zv = &CG(active_op_array)->literals[opline->op2.constant];
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

	EG(active_op_array) = CG(active_op_array);
	orig_interactive = CG(interactive);
	CG(interactive) = 0;
	zend_execute(CG(active_op_array), NULL TSRMLS_CC);
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

zend_class_entry *zend_fetch_class(zend_string *class_name, int fetch_type TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce;
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
				fetch_type = zend_get_class_fetch_type(class_name->val, class_name->len);
				if (fetch_type!=ZEND_FETCH_CLASS_DEFAULT) {
					goto check_fetch_type;
				}
			}
			break;
	}

	if ((ce = zend_lookup_class_ex(class_name, NULL, use_autoload TSRMLS_CC)) == NULL) {
		if (use_autoload) {
			if (!silent && !EG(exception)) {
				if (fetch_type == ZEND_FETCH_CLASS_INTERFACE) {
					zend_error(E_ERROR, "Interface '%s' not found", class_name->val);
				} else if (fetch_type == ZEND_FETCH_CLASS_TRAIT) {
                	zend_error(E_ERROR, "Trait '%s' not found", class_name->val);
                } else {
					zend_error(E_ERROR, "Class '%s' not found", class_name->val);
				}
			}
		}
		return NULL;
	}
	return ce;
}
/* }}} */

zend_class_entry *zend_fetch_class_by_name(zend_string *class_name, const zval *key, int fetch_type TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce;
	int use_autoload = (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) == 0;

	if ((ce = zend_lookup_class_ex(class_name, key, use_autoload TSRMLS_CC)) == NULL) {
		if (use_autoload) {
			if ((fetch_type & ZEND_FETCH_CLASS_SILENT) == 0 && !EG(exception)) {
				if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_INTERFACE) {
					zend_error(E_ERROR, "Interface '%s' not found", class_name->val);
				} else if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_TRAIT) {
					zend_error(E_ERROR, "Trait '%s' not found", class_name->val);
				} else {
					zend_error(E_ERROR, "Class '%s' not found", class_name->val);
				}
			}
		}
		return NULL;
	}
	return ce;
}
/* }}} */

#define MAX_ABSTRACT_INFO_CNT 3
#define MAX_ABSTRACT_INFO_FMT "%s%s%s%s"
#define DISPLAY_ABSTRACT_FN(idx) \
	ai.afn[idx] ? ZEND_FN_SCOPE_NAME(ai.afn[idx]) : "", \
	ai.afn[idx] ? "::" : "", \
	ai.afn[idx] ? ai.afn[idx]->common.function_name->val : "", \
	ai.afn[idx] && ai.afn[idx + 1] ? ", " : (ai.afn[idx] && ai.cnt > MAX_ABSTRACT_INFO_CNT ? ", ..." : "")

typedef struct _zend_abstract_info {
	zend_function *afn[MAX_ABSTRACT_INFO_CNT + 1];
	int cnt;
	int ctor;
} zend_abstract_info;

static int zend_verify_abstract_class_function(zval *zv, zend_abstract_info *ai TSRMLS_DC) /* {{{ */
{
	zend_function *fn = Z_PTR_P(zv);

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
				ce->name->val, ai.cnt,
				ai.cnt > 1 ? "s" : "",
				DISPLAY_ABSTRACT_FN(0),
				DISPLAY_ABSTRACT_FN(1),
				DISPLAY_ABSTRACT_FN(2)
				);
		}
	}
}
/* }}} */

ZEND_API int zend_delete_global_variable(zend_string *name TSRMLS_DC) /* {{{ */
{
    return zend_hash_del_ind(&EG(symbol_table).ht, name);
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
				EG(active_symbol_table) = emalloc(sizeof(zend_array));
				GC_REFCOUNT(EG(active_symbol_table)) = 0;
				GC_TYPE_INFO(EG(active_symbol_table)) = IS_ARRAY;
				zend_hash_init(&EG(active_symbol_table)->ht, ex->op_array->last_var, NULL, ZVAL_PTR_DTOR, 0);
				/*printf("Cache miss!  Initialized %x\n", EG(active_symbol_table));*/
			}
			ex->symbol_table = EG(active_symbol_table);

			if (ex->op_array->this_var != -1 &&
			    Z_TYPE_P(EX_VAR_2(ex, ex->op_array->this_var)) == IS_UNDEF &&
			    Z_OBJ(EG(This))) {
			    ZVAL_COPY_VALUE(EX_VAR_2(ex, ex->op_array->this_var), &EG(This));
 			}
			for (i = 0; i < ex->op_array->last_var; i++) {
				zval zv;
					
				ZVAL_INDIRECT(&zv, EX_VAR_NUM_2(ex, i));
				zend_hash_update(&EG(active_symbol_table)->ht,
					ex->op_array->vars[i], &zv);
			}
		}
	}
}
/* }}} */

ZEND_API void zend_attach_symbol_table(TSRMLS_D) /* {{{ */
{
	int i;
	zend_execute_data *execute_data = EG(current_execute_data);
	zend_op_array *op_array = execute_data->op_array;
	HashTable *ht = &EG(active_symbol_table)->ht;
	
	/* copy real values from symbol table into CV slots and create
	   INDIRECT references to CV in symbol table  */
	for (i = 0; i < op_array->last_var; i++) {
		zval *zv = zend_hash_find(ht, op_array->vars[i]);

		if (zv) {
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zval *val = Z_INDIRECT_P(zv);
				if (Z_TYPE_P(val) == IS_UNDEF) {
					ZVAL_UNDEF(EX_VAR_NUM(i));
				} else {
					ZVAL_COPY_VALUE(EX_VAR_NUM(i), val);
				}
			} else {
				ZVAL_COPY_VALUE(EX_VAR_NUM(i), zv);
			}
		} else {
			ZVAL_UNDEF(EX_VAR_NUM(i));
			zv = zend_hash_update(ht, op_array->vars[i], EX_VAR_NUM(i));
		}
		ZVAL_INDIRECT(zv, EX_VAR_NUM(i));
	}
}
/* }}} */

ZEND_API void zend_detach_symbol_table(TSRMLS_D) /* {{{ */
{
	int i;
	zend_execute_data *execute_data = EG(current_execute_data);
	zend_op_array *op_array = execute_data->op_array;
	HashTable *ht = &EG(active_symbol_table)->ht;
	
	/* copy real values from CV slots into symbol table */
	for (i = 0; i < op_array->last_var; i++) {
		if (Z_TYPE_P(EX_VAR_NUM(i)) == IS_UNDEF) {
			zend_hash_del(ht, op_array->vars[i]);
		} else {
			zend_hash_update(ht, op_array->vars[i], EX_VAR_NUM(i));
			ZVAL_UNDEF(EX_VAR_NUM(i));
		}
	}
}
/* }}} */

ZEND_API int zend_set_local_var(zend_string *name, zval *value, int force TSRMLS_DC) /* {{{ */
{
	if (!EG(active_symbol_table)) {
		int i;
		zend_execute_data *execute_data = EG(current_execute_data);
		zend_op_array *op_array = execute_data->op_array;
		zend_ulong h = STR_HASH_VAL(name);

		if (op_array) {
			for (i = 0; i < op_array->last_var; i++) {
				if (op_array->vars[i]->h == h &&
				    op_array->vars[i]->len == name->len &&
				    memcmp(op_array->vars[i]->val, name->val, name->len) == 0) {
					ZVAL_COPY_VALUE(EX_VAR_NUM(i), value);
					return SUCCESS;
				}
			}
		}
		if (force) {
			zend_rebuild_symbol_table(TSRMLS_C);
			if (EG(active_symbol_table)) {
				zend_hash_update(&EG(active_symbol_table)->ht, name, value);
			}
		} else {
			return FAILURE;
		}
	} else {
		return (zend_hash_update_ind(&EG(active_symbol_table)->ht, name, value) != NULL) ? SUCCESS : FAILURE;
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_set_local_var_str(const char *name, int len, zval *value, int force TSRMLS_DC) /* {{{ */
{
	if (!EG(active_symbol_table)) {
		int i;
		zend_execute_data *execute_data = EG(current_execute_data);
		zend_op_array *op_array = execute_data->op_array;
		zend_ulong h = zend_hash_func(name, len);

		if (op_array) {
			for (i = 0; i < op_array->last_var; i++) {
				if (op_array->vars[i]->h == h &&
				    op_array->vars[i]->len == len &&
				    memcmp(op_array->vars[i]->val, name, len) == 0) {
					ZVAL_COPY_VALUE(EX_VAR_NUM(i), value);
					return SUCCESS;
				}
			}
		}
		if (force) {
			zend_rebuild_symbol_table(TSRMLS_C);
			if (EG(active_symbol_table)) {
				zend_hash_str_update(&EG(active_symbol_table)->ht, name, len, value);
			}
		} else {
			return FAILURE;
		}
	} else {
		return (zend_hash_str_update_ind(&EG(active_symbol_table)->ht, name, len, value) != NULL) ? SUCCESS : FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
