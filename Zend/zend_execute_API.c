/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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
   |          Dmitry Stogov <dmitry@zend.com>                             |
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
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

ZEND_API void (*zend_execute_ex)(zend_execute_data *execute_data);
ZEND_API void (*zend_execute_internal)(zend_execute_data *execute_data, zval *return_value);

/* true globals */
ZEND_API const zend_fcall_info empty_fcall_info = { 0, {{0}, {{0}}, {0}}, NULL, NULL, NULL, 0, 0 };
ZEND_API const zend_fcall_info_cache empty_fcall_info_cache = { 0, NULL, NULL, NULL, NULL };

#ifdef ZEND_WIN32
ZEND_TLS HANDLE tq_timer = NULL;
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

		fprintf(stderr, "SIGSEGV caught on opcode %d on opline %d of %s() at %s:%d\n\n",
				active_opline->opcode,
				active_opline-EG(active_op_array)->opcodes,
				get_active_function_name(),
				zend_get_executed_filename(),
				zend_get_executed_lineno());
/* See http://support.microsoft.com/kb/190351 */
#ifdef ZEND_WIN32
		fflush(stderr);
#endif
	}
	if (original_sigsegv_handler!=zend_handle_sigsegv) {
		original_sigsegv_handler(dummy);
	}
}
/* }}} */
#endif

static void zend_extension_activator(zend_extension *extension) /* {{{ */
{
	if (extension->activate) {
		extension->activate();
	}
}
/* }}} */

static void zend_extension_deactivator(zend_extension *extension) /* {{{ */
{
	if (extension->deactivate) {
		extension->deactivate();
	}
}
/* }}} */

static int clean_non_persistent_function(zval *zv) /* {{{ */
{
	zend_function *function = Z_PTR_P(zv);
	return (function->type == ZEND_INTERNAL_FUNCTION) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

ZEND_API int clean_non_persistent_function_full(zval *zv) /* {{{ */
{
	zend_function *function = Z_PTR_P(zv);
	return (function->type == ZEND_INTERNAL_FUNCTION) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static int clean_non_persistent_class(zval *zv) /* {{{ */
{
	zend_class_entry *ce = Z_PTR_P(zv);
	return (ce->type == ZEND_INTERNAL_CLASS) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

ZEND_API int clean_non_persistent_class_full(zval *zv) /* {{{ */
{
	zend_class_entry *ce = Z_PTR_P(zv);
	return (ce->type == ZEND_INTERNAL_CLASS) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

void init_executor(void) /* {{{ */
{
	zend_init_fpu();

	ZVAL_NULL(&EG(uninitialized_zval));
	ZVAL_ERROR(&EG(error_zval));
/* destroys stack frame, therefore makes core dumps worthless */
#if 0&&ZEND_DEBUG
	original_sigsegv_handler = signal(SIGSEGV, zend_handle_sigsegv);
#endif

	EG(symtable_cache_ptr) = EG(symtable_cache) - 1;
	EG(symtable_cache_limit) = EG(symtable_cache) + SYMTABLE_CACHE_SIZE - 1;
	EG(no_extensions) = 0;

	EG(function_table) = CG(function_table);
	EG(class_table) = CG(class_table);

	EG(in_autoload) = NULL;
	EG(autoload_func) = NULL;
	EG(error_handling) = EH_NORMAL;

	zend_vm_stack_init();

	zend_hash_init(&EG(symbol_table), 64, NULL, ZVAL_PTR_DTOR, 0);
	EG(valid_symbol_table) = 1;

	zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_activator);

	zend_hash_init(&EG(included_files), 8, NULL, NULL, 0);

	EG(ticks_count) = 0;

	ZVAL_UNDEF(&EG(user_error_handler));

	EG(current_execute_data) = NULL;

	zend_stack_init(&EG(user_error_handlers_error_reporting), sizeof(int));
	zend_stack_init(&EG(user_error_handlers), sizeof(zval));
	zend_stack_init(&EG(user_exception_handlers), sizeof(zval));

	zend_objects_store_init(&EG(objects_store), 1024);

	EG(full_tables_cleanup) = 0;
	EG(timed_out) = 0;

	EG(exception) = NULL;
	EG(prev_exception) = NULL;

	EG(fake_scope) = NULL;

	EG(ht_iterators_count) = sizeof(EG(ht_iterators_slots)) / sizeof(HashTableIterator);
	EG(ht_iterators_used) = 0;
	EG(ht_iterators) = EG(ht_iterators_slots);
	memset(EG(ht_iterators), 0, sizeof(EG(ht_iterators_slots)));

	EG(active) = 1;
}
/* }}} */

static int zval_call_destructor(zval *zv) /* {{{ */
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
	if (Z_TYPE_P(zv) == IS_INDIRECT) {
		zv = Z_INDIRECT_P(zv);
	}
	i_zval_ptr_dtor(zv ZEND_FILE_LINE_CC);
}
/* }}} */

static void zend_throw_or_error(int fetch_type, zend_class_entry *exception_ce, const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);

	if (fetch_type & ZEND_FETCH_CLASS_EXCEPTION) {
		zend_throw_error(exception_ce, "%s", message);
	} else {
		zend_error(E_ERROR, "%s", message);
	}

	efree(message);
	va_end(va);
}
/* }}} */

void shutdown_destructors(void) /* {{{ */
{
	if (CG(unclean_shutdown)) {
		EG(symbol_table).pDestructor = zend_unclean_zval_ptr_dtor;
	}
	zend_try {
		uint32_t symbols;
		do {
			symbols = zend_hash_num_elements(&EG(symbol_table));
			zend_hash_reverse_apply(&EG(symbol_table), (apply_func_t) zval_call_destructor);
		} while (symbols != zend_hash_num_elements(&EG(symbol_table)));
		zend_objects_store_call_destructors(&EG(objects_store));
	} zend_catch {
		/* if we couldn't destruct cleanly, mark all objects as destructed anyway */
		zend_objects_store_mark_destructed(&EG(objects_store));
	} zend_end_try();
}
/* }}} */

void shutdown_executor(void) /* {{{ */
{
	zend_function *func;
	zend_class_entry *ce;

	zend_try {

/* Removed because this can not be safely done, e.g. in this situation:
   Object 1 creates object 2
   Object 3 holds reference to object 2.
   Now when 1 and 2 are destroyed, 3 can still access 2 in its destructor, with
   very problematic results */
/* 		zend_objects_store_call_destructors(&EG(objects_store)); */

/* Moved after symbol table cleaners, because  some of the cleaners can call
   destructors, which would use EG(symtable_cache_ptr) and thus leave leaks */
/*		while (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
			zend_hash_destroy(*EG(symtable_cache_ptr));
			efree(*EG(symtable_cache_ptr));
			EG(symtable_cache_ptr)--;
		}
*/
		zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_deactivator);

		if (CG(unclean_shutdown)) {
			EG(symbol_table).pDestructor = zend_unclean_zval_ptr_dtor;
		}
		zend_hash_graceful_reverse_destroy(&EG(symbol_table));
	} zend_end_try();
	EG(valid_symbol_table) = 0;

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

		zend_stack_clean(&EG(user_error_handlers_error_reporting), NULL, 1);
		zend_stack_clean(&EG(user_error_handlers), (void (*)(void *))ZVAL_PTR_DTOR, 1);
		zend_stack_clean(&EG(user_exception_handlers), (void (*)(void *))ZVAL_PTR_DTOR, 1);
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
			ZEND_HASH_FOREACH_PTR(EG(function_table), func) {
				if (func->type == ZEND_USER_FUNCTION) {
					zend_cleanup_op_array_data((zend_op_array *) func);
				}
			} ZEND_HASH_FOREACH_END();
			ZEND_HASH_REVERSE_FOREACH_PTR(EG(class_table), ce) {
				if (ce->type == ZEND_USER_CLASS) {
					zend_cleanup_user_class_data(ce);
				} else {
					zend_cleanup_internal_class_data(ce);
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_REVERSE_FOREACH_PTR(EG(function_table), func) {
				if (func->type != ZEND_USER_FUNCTION) {
					break;
				}
				zend_cleanup_op_array_data((zend_op_array *) func);
			} ZEND_HASH_FOREACH_END();
			ZEND_HASH_REVERSE_FOREACH_PTR(EG(class_table), ce) {
				if (ce->type != ZEND_USER_CLASS) {
					break;
				}
				zend_cleanup_user_class_data(ce);
			} ZEND_HASH_FOREACH_END();
			zend_cleanup_internal_classes();
		}
	} zend_end_try();

	zend_try {
		zend_llist_destroy(&CG(open_files));
	} zend_end_try();

	zend_try {
		zend_close_rsrc_list(&EG(regular_list));
	} zend_end_try();

#if ZEND_DEBUG
	if (GC_G(gc_enabled) && !CG(unclean_shutdown)) {
		gc_collect_cycles();
	}
#endif

	zend_try {
		zend_objects_store_free_object_storage(&EG(objects_store));

		zend_vm_stack_destroy();

		/* Destroy all op arrays */
		if (EG(full_tables_cleanup)) {
			zend_hash_reverse_apply(EG(function_table), clean_non_persistent_function_full);
			zend_hash_reverse_apply(EG(class_table), clean_non_persistent_class_full);
		} else {
			zend_hash_reverse_apply(EG(function_table), clean_non_persistent_function);
			zend_hash_reverse_apply(EG(class_table), clean_non_persistent_class);
		}

		while (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
			zend_hash_destroy(*EG(symtable_cache_ptr));
			FREE_HASHTABLE(*EG(symtable_cache_ptr));
			EG(symtable_cache_ptr)--;
		}
	} zend_end_try();

	zend_try {
		clean_non_persistent_constants();
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

	zend_shutdown_fpu();

#if ZEND_DEBUG
	if (EG(ht_iterators_used) && !CG(unclean_shutdown)) {
		zend_error(E_WARNING, "Leaked %" PRIu32 " hashtable iterators", EG(ht_iterators_used));
	}
#endif

	EG(ht_iterators_used) = 0;
	if (EG(ht_iterators) != EG(ht_iterators_slots)) {
		efree(EG(ht_iterators));
	}

	EG(active) = 0;
}
/* }}} */

/* return class name and "::" or "". */
ZEND_API const char *get_active_class_name(const char **space) /* {{{ */
{
	zend_function *func;

	if (!zend_is_executing()) {
		if (space) {
			*space = "";
		}
		return "";
	}

	func = EG(current_execute_data)->func;
	switch (func->type) {
		case ZEND_USER_FUNCTION:
		case ZEND_INTERNAL_FUNCTION:
		{
			zend_class_entry *ce = func->common.scope;

			if (space) {
				*space = ce ? "::" : "";
			}
			return ce ? ZSTR_VAL(ce->name) : "";
		}
		default:
			if (space) {
				*space = "";
			}
			return "";
	}
}
/* }}} */

ZEND_API const char *get_active_function_name(void) /* {{{ */
{
	zend_function *func;

	if (!zend_is_executing()) {
		return NULL;
	}
	func = EG(current_execute_data)->func;
	switch (func->type) {
		case ZEND_USER_FUNCTION: {
				zend_string *function_name = func->common.function_name;

				if (function_name) {
					return ZSTR_VAL(function_name);
				} else {
					return "main";
				}
			}
			break;
		case ZEND_INTERNAL_FUNCTION:
			return ZSTR_VAL(func->common.function_name);
			break;
		default:
			return NULL;
	}
}
/* }}} */

ZEND_API const char *zend_get_executed_filename(void) /* {{{ */
{
	zend_execute_data *ex = EG(current_execute_data);

	while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->type))) {
		ex = ex->prev_execute_data;
	}
	if (ex) {
		return ZSTR_VAL(ex->func->op_array.filename);
	} else {
		return "[no active file]";
	}
}
/* }}} */

ZEND_API zend_string *zend_get_executed_filename_ex(void) /* {{{ */
{
	zend_execute_data *ex = EG(current_execute_data);

	while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->type))) {
		ex = ex->prev_execute_data;
	}
	if (ex) {
		return ex->func->op_array.filename;
	} else {
		return NULL;
	}
}
/* }}} */

ZEND_API uint zend_get_executed_lineno(void) /* {{{ */
{
	zend_execute_data *ex = EG(current_execute_data);

	while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->type))) {
		ex = ex->prev_execute_data;
	}
	if (ex) {
		if (EG(exception) && ex->opline->opcode == ZEND_HANDLE_EXCEPTION &&
		    ex->opline->lineno == 0 && EG(opline_before_exception)) {
			return EG(opline_before_exception)->lineno;
		}
		return ex->opline->lineno;
	} else {
		return 0;
	}
}
/* }}} */

ZEND_API zend_class_entry *zend_get_executed_scope(void) /* {{{ */
{
	zend_execute_data *ex = EG(current_execute_data);

	while (1) {
		if (!ex) {
			return NULL;
		} else if (ex->func && (ZEND_USER_CODE(ex->func->type) || ex->func->common.scope)) {
			return ex->func->common.scope;
		}
		ex = ex->prev_execute_data;
	}
}
/* }}} */

ZEND_API zend_bool zend_is_executing(void) /* {{{ */
{
	return EG(current_execute_data) != 0;
}
/* }}} */

ZEND_API void _zval_ptr_dtor(zval *zval_ptr ZEND_FILE_LINE_DC) /* {{{ */
{
	i_zval_ptr_dtor(zval_ptr ZEND_FILE_LINE_RELAY_CC);
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

ZEND_API int zval_update_constant_ex(zval *p, zend_class_entry *scope) /* {{{ */
{
	zval *const_value;
	char *colon;
	zend_bool inline_change;

	if (Z_TYPE_P(p) == IS_CONSTANT) {
		if (IS_CONSTANT_VISITED(p)) {
			zend_throw_error(NULL, "Cannot declare self-referencing constant '%s'", Z_STRVAL_P(p));
			return FAILURE;
		}
		inline_change = (Z_TYPE_FLAGS_P(p) & IS_TYPE_IMMUTABLE) == 0;
		SEPARATE_ZVAL_NOREF(p);
		MARK_CONSTANT_VISITED(p);
		if (Z_CONST_FLAGS_P(p) & IS_CONSTANT_CLASS) {
			ZEND_ASSERT(EG(current_execute_data));
			if (inline_change) {
				zend_string_release(Z_STR_P(p));
			}
			if (scope && scope->name) {
				ZVAL_STR_COPY(p, scope->name);
			} else {
				ZVAL_EMPTY_STRING(p);
			}
		} else if (UNEXPECTED((const_value = zend_get_constant_ex(Z_STR_P(p), scope, Z_CONST_FLAGS_P(p))) == NULL)) {
			char *actual = Z_STRVAL_P(p);

			if (UNEXPECTED(EG(exception))) {
				RESET_CONSTANT_VISITED(p);
				return FAILURE;
			} else if ((colon = (char*)zend_memrchr(Z_STRVAL_P(p), ':', Z_STRLEN_P(p)))) {
				zend_throw_error(NULL, "Undefined class constant '%s'", Z_STRVAL_P(p));
				RESET_CONSTANT_VISITED(p);
				return FAILURE;
			} else {
				zend_string *save = Z_STR_P(p);
				char *slash;
				size_t actual_len = Z_STRLEN_P(p);
				if ((Z_CONST_FLAGS_P(p) & IS_CONSTANT_UNQUALIFIED) && (slash = (char *)zend_memrchr(actual, '\\', actual_len))) {
					actual = slash + 1;
					actual_len -= (actual - Z_STRVAL_P(p));
					if (inline_change) {
						zend_string *s = zend_string_init(actual, actual_len, 0);
						Z_STR_P(p) = s;
						Z_TYPE_FLAGS_P(p) = IS_TYPE_REFCOUNTED | IS_TYPE_COPYABLE;
					}
				}
				if ((Z_CONST_FLAGS_P(p) & IS_CONSTANT_UNQUALIFIED) == 0) {
					zend_throw_error(NULL, "Undefined constant '%s'", ZSTR_VAL(save));
					if (inline_change) {
						zend_string_release(save);
					}
					RESET_CONSTANT_VISITED(p);
					return FAILURE;
				} else {
					zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",  actual,  actual);
					if (!inline_change) {
						ZVAL_STRINGL(p, actual, actual_len);
					} else {
						Z_TYPE_INFO_P(p) = Z_REFCOUNTED_P(p) ?
							IS_STRING_EX : IS_INTERNED_STRING_EX;
						if (save && ZSTR_VAL(save) != actual) {
							zend_string_release(save);
						}
					}
				}
			}
		} else {
			if (inline_change) {
				zend_string_release(Z_STR_P(p));
			}
			ZVAL_COPY_VALUE(p, const_value);
			zval_opt_copy_ctor(p);
		}
	} else if (Z_TYPE_P(p) == IS_CONSTANT_AST) {
		zval tmp;

		inline_change = (Z_TYPE_FLAGS_P(p) & IS_TYPE_IMMUTABLE) == 0;
		if (UNEXPECTED(zend_ast_evaluate(&tmp, Z_ASTVAL_P(p), scope) != SUCCESS)) {
			return FAILURE;
		}
		if (inline_change) {
			zval_ptr_dtor(p);
		}
		ZVAL_COPY_VALUE(p, &tmp);
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int zval_update_constant(zval *pp) /* {{{ */
{
	return zval_update_constant_ex(pp, EG(current_execute_data) ? zend_get_executed_scope() : CG(active_class_entry));
}
/* }}} */

int _call_user_function_ex(zval *object, zval *function_name, zval *retval_ptr, uint32_t param_count, zval params[], int no_separation) /* {{{ */
{
	zend_fcall_info fci;

	fci.size = sizeof(fci);
	fci.object = object ? Z_OBJ_P(object) : NULL;
	ZVAL_COPY_VALUE(&fci.function_name, function_name);
	fci.retval = retval_ptr;
	fci.param_count = param_count;
	fci.params = params;
	fci.no_separation = (zend_bool) no_separation;

	return zend_call_function(&fci, NULL);
}
/* }}} */

int zend_call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache) /* {{{ */
{
	uint32_t i;
	zend_execute_data *call, dummy_execute_data;
	zend_fcall_info_cache fci_cache_local;
	zend_function *func;

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
			zend_error_noreturn(E_CORE_ERROR, "Corrupted fcall_info provided to zend_call_function()");
			break;
	}

	/* Initialize execute_data */
	if (!EG(current_execute_data)) {
		/* This only happens when we're called outside any execute()'s
		 * It shouldn't be strictly necessary to NULL execute_data out,
		 * but it may make bugs easier to spot
		 */
		memset(&dummy_execute_data, 0, sizeof(zend_execute_data));
		EG(current_execute_data) = &dummy_execute_data;
	} else if (EG(current_execute_data)->func &&
	           ZEND_USER_CODE(EG(current_execute_data)->func->common.type) &&
	           EG(current_execute_data)->opline->opcode != ZEND_DO_FCALL &&
	           EG(current_execute_data)->opline->opcode != ZEND_DO_ICALL &&
	           EG(current_execute_data)->opline->opcode != ZEND_DO_UCALL &&
	           EG(current_execute_data)->opline->opcode != ZEND_DO_FCALL_BY_NAME) {
		/* Insert fake frame in case of include or magic calls */
		dummy_execute_data = *EG(current_execute_data);
		dummy_execute_data.prev_execute_data = EG(current_execute_data);
		dummy_execute_data.call = NULL;
		dummy_execute_data.opline = NULL;
		dummy_execute_data.func = NULL;
		EG(current_execute_data) = &dummy_execute_data;
	}

	if (!fci_cache || !fci_cache->initialized) {
		zend_string *callable_name;
		char *error = NULL;

		if (!fci_cache) {
			fci_cache = &fci_cache_local;
		}

		if (!zend_is_callable_ex(&fci->function_name, fci->object, IS_CALLABLE_CHECK_SILENT, &callable_name, fci_cache, &error)) {
			if (error) {
				zend_error(E_WARNING, "Invalid callback %s, %s", ZSTR_VAL(callable_name), error);
				efree(error);
			}
			if (callable_name) {
				zend_string_release(callable_name);
			}
			if (EG(current_execute_data) == &dummy_execute_data) {
				EG(current_execute_data) = dummy_execute_data.prev_execute_data;
			}
			return FAILURE;
		} else if (error) {
			/* Capitalize the first latter of the error message */
			if (error[0] >= 'a' && error[0] <= 'z') {
				error[0] += ('A' - 'a');
			}
			zend_error(E_DEPRECATED, "%s", error);
			efree(error);
		}
		zend_string_release(callable_name);
	}

	func = fci_cache->function_handler;
	fci->object = (func->common.fn_flags & ZEND_ACC_STATIC) ?
		NULL : fci_cache->object;

	call = zend_vm_stack_push_call_frame(ZEND_CALL_TOP_FUNCTION | ZEND_CALL_DYNAMIC,
		func, fci->param_count, fci_cache->called_scope, fci->object);
	if (fci->object &&
	    (!EG(objects_store).object_buckets ||
	     !IS_OBJ_VALID(EG(objects_store).object_buckets[fci->object->handle]))) {
		if (EG(current_execute_data) == &dummy_execute_data) {
			EG(current_execute_data) = dummy_execute_data.prev_execute_data;
		}
		return FAILURE;
	}

	if (func->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_DEPRECATED)) {
		if (func->common.fn_flags & ZEND_ACC_ABSTRACT) {
			zend_throw_error(NULL, "Cannot call abstract method %s::%s()", ZSTR_VAL(func->common.scope->name), ZSTR_VAL(func->common.function_name));
			if (EG(current_execute_data) == &dummy_execute_data) {
				EG(current_execute_data) = dummy_execute_data.prev_execute_data;
			}
			return FAILURE;
		}
		if (func->common.fn_flags & ZEND_ACC_DEPRECATED) {
 			zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
				func->common.scope ? ZSTR_VAL(func->common.scope->name) : "",
				func->common.scope ? "::" : "",
				ZSTR_VAL(func->common.function_name));
		}
	}

	for (i=0; i<fci->param_count; i++) {
		zval *param;
		zval *arg = &fci->params[i];

		if (ARG_SHOULD_BE_SENT_BY_REF(func, i + 1)) {
			if (UNEXPECTED(!Z_ISREF_P(arg))) {
				if (fci->no_separation &&
					!ARG_MAY_BE_SENT_BY_REF(func, i + 1)) {
					if (i) {
						/* hack to clean up the stack */
						ZEND_CALL_NUM_ARGS(call) = i;
						zend_vm_stack_free_args(call);
					}
					zend_vm_stack_free_call_frame(call);

					zend_error(E_WARNING, "Parameter %d to %s%s%s() expected to be a reference, value given",
						i+1,
						func->common.scope ? ZSTR_VAL(func->common.scope->name) : "",
						func->common.scope ? "::" : "",
						ZSTR_VAL(func->common.function_name));
					if (EG(current_execute_data) == &dummy_execute_data) {
						EG(current_execute_data) = dummy_execute_data.prev_execute_data;
					}
					return FAILURE;
				}

				ZVAL_NEW_REF(arg, arg);
			}
			Z_ADDREF_P(arg);
		} else {
			if (Z_ISREF_P(arg) &&
			    !(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
				/* don't separate references for __call */
				arg = Z_REFVAL_P(arg);
			}
			if (Z_OPT_REFCOUNTED_P(arg)) {
				Z_ADDREF_P(arg);
			}
		}
		param = ZEND_CALL_ARG(call, i+1);
		ZVAL_COPY_VALUE(param, arg);
	}

	if (UNEXPECTED(func->op_array.fn_flags & ZEND_ACC_CLOSURE)) {
		ZEND_ASSERT(GC_TYPE((zend_object*)func->op_array.prototype) == IS_OBJECT);
		GC_REFCOUNT((zend_object*)func->op_array.prototype)++;
		ZEND_ADD_CALL_FLAG(call, ZEND_CALL_CLOSURE);
	}

	if (func->type == ZEND_USER_FUNCTION) {
		int call_via_handler = (func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) != 0;
		zend_init_execute_data(call, &func->op_array, fci->retval);
		zend_execute_ex(call);
		if (call_via_handler) {
			/* We must re-initialize function again */
			fci_cache->initialized = 0;
		}
	} else if (func->type == ZEND_INTERNAL_FUNCTION) {
		int call_via_handler = (func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) != 0;
		ZVAL_NULL(fci->retval);
		call->prev_execute_data = EG(current_execute_data);
		call->return_value = NULL; /* this is not a constructor call */
		EG(current_execute_data) = call;
		if (EXPECTED(zend_execute_internal == NULL)) {
			/* saves one function call if zend_execute_internal is not used */
			func->internal_function.handler(call, fci->retval);
		} else {
			zend_execute_internal(call, fci->retval);
		}
		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_args(call);

		/*  We shouldn't fix bad extensions here,
			because it can break proper ones (Bug #34045)
		if (!EX(function_state).function->common.return_reference)
		{
			INIT_PZVAL(f->retval);
		}*/
		if (EG(exception)) {
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
			call->prev_execute_data = EG(current_execute_data);
			EG(current_execute_data) = call;
			fci->object->handlers->call_method(func->common.function_name, fci->object, call, fci->retval);
			EG(current_execute_data) = call->prev_execute_data;
		} else {
			zend_throw_error(NULL, "Cannot call overloaded function for non-object");
		}

		zend_vm_stack_free_args(call);

		if (func->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			zend_string_release(func->common.function_name);
		}
		efree(func);

		if (EG(exception)) {
			zval_ptr_dtor(fci->retval);
			ZVAL_UNDEF(fci->retval);
		}
	}

	zend_vm_stack_free_call_frame(call);

	if (EG(current_execute_data) == &dummy_execute_data) {
		EG(current_execute_data) = dummy_execute_data.prev_execute_data;
	}

	if (EG(exception)) {
		zend_throw_exception_internal(NULL);
	}
	return SUCCESS;
}
/* }}} */

ZEND_API zend_class_entry *zend_lookup_class_ex(zend_string *name, const zval *key, int use_autoload) /* {{{ */
{
	zend_class_entry *ce = NULL;
	zval args[1];
	zval local_retval;
	zend_string *lc_name;
	zend_fcall_info fcall_info;
	zend_fcall_info_cache fcall_cache;

	if (key) {
		lc_name = Z_STR_P(key);
	} else {
		if (name == NULL || !ZSTR_LEN(name)) {
			return NULL;
		}

		if (ZSTR_VAL(name)[0] == '\\') {
			lc_name = zend_string_alloc(ZSTR_LEN(name) - 1, 0);
			zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 1);
		} else {
			lc_name = zend_string_tolower(name);
		}
	}

	ce = zend_hash_find_ptr(EG(class_table), lc_name);
	if (ce) {
		if (!key) {
			zend_string_release(lc_name);
		}
		return ce;
	}

	/* The compiler is not-reentrant. Make sure we __autoload() only during run-time
	 * (doesn't impact functionality of __autoload()
	*/
	if (!use_autoload || zend_is_compiling()) {
		if (!key) {
			zend_string_release(lc_name);
		}
		return NULL;
	}

	if (!EG(autoload_func)) {
		zend_function *func = zend_hash_find_ptr(EG(function_table), CG(known_strings)[ZEND_STR_MAGIC_AUTOLOAD]);
		if (func) {
			EG(autoload_func) = func;
		} else {
			if (!key) {
				zend_string_release(lc_name);
			}
			return NULL;
		}

	}

	/* Verify class name before passing it to __autoload() */
	if (strspn(ZSTR_VAL(name), "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377\\") != ZSTR_LEN(name)) {
		if (!key) {
			zend_string_release(lc_name);
		}
		return NULL;
	}

	if (EG(in_autoload) == NULL) {
		ALLOC_HASHTABLE(EG(in_autoload));
		zend_hash_init(EG(in_autoload), 8, NULL, NULL, 0);
	}

	if (zend_hash_add_empty_element(EG(in_autoload), lc_name) == NULL) {
		if (!key) {
			zend_string_release(lc_name);
		}
		return NULL;
	}

	ZVAL_UNDEF(&local_retval);

	if (ZSTR_VAL(name)[0] == '\\') {
		ZVAL_STRINGL(&args[0], ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 1);
	} else {
		ZVAL_STR_COPY(&args[0], name);
	}

	fcall_info.size = sizeof(fcall_info);
	ZVAL_STR_COPY(&fcall_info.function_name, EG(autoload_func)->common.function_name);
	fcall_info.retval = &local_retval;
	fcall_info.param_count = 1;
	fcall_info.params = args;
	fcall_info.object = NULL;
	fcall_info.no_separation = 1;

	fcall_cache.initialized = 1;
	fcall_cache.function_handler = EG(autoload_func);
	fcall_cache.calling_scope = NULL;
	fcall_cache.called_scope = NULL;
	fcall_cache.object = NULL;

	zend_exception_save();
	if ((zend_call_function(&fcall_info, &fcall_cache) == SUCCESS) && !EG(exception)) {
		ce = zend_hash_find_ptr(EG(class_table), lc_name);
	}
	zend_exception_restore();

	zval_ptr_dtor(&args[0]);
	zval_dtor(&fcall_info.function_name);

	zend_hash_del(EG(in_autoload), lc_name);

	zval_ptr_dtor(&local_retval);

	if (!key) {
		zend_string_release(lc_name);
	}
	return ce;
}
/* }}} */

ZEND_API zend_class_entry *zend_lookup_class(zend_string *name) /* {{{ */
{
	return zend_lookup_class_ex(name, NULL, 1);
}
/* }}} */

ZEND_API zend_class_entry *zend_get_called_scope(zend_execute_data *ex) /* {{{ */
{
	while (ex) {
		if (Z_TYPE(ex->This) == IS_OBJECT) {
			return Z_OBJCE(ex->This);
		} else if (Z_CE(ex->This)) {
			return Z_CE(ex->This);
		} else if (ex->func) {
			if (ex->func->type != ZEND_INTERNAL_FUNCTION || ex->func->common.scope) {
				return NULL;
			}
		}
		ex = ex->prev_execute_data;
	}
	return NULL;
}
/* }}} */

ZEND_API zend_object *zend_get_this_object(zend_execute_data *ex) /* {{{ */
{
	while (ex) {
		if (Z_TYPE(ex->This) == IS_OBJECT) {
			return Z_OBJ(ex->This);
		} else if (ex->func) {
			if (ex->func->type != ZEND_INTERNAL_FUNCTION || ex->func->common.scope) {
				return NULL;
			}
		}
		ex = ex->prev_execute_data;
	}
	return NULL;
}
/* }}} */

ZEND_API int zend_eval_stringl(char *str, size_t str_len, zval *retval_ptr, char *string_name) /* {{{ */
{
	zval pv;
	zend_op_array *new_op_array;
	uint32_t original_compiler_options;
	int retval;

	if (retval_ptr) {
		ZVAL_NEW_STR(&pv, zend_string_alloc(str_len + sizeof("return ;")-1, 1));
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
	new_op_array = zend_compile_string(&pv, string_name);
	CG(compiler_options) = original_compiler_options;

	if (new_op_array) {
		zval local_retval;

		EG(no_extensions)=1;

		zend_try {
			ZVAL_UNDEF(&local_retval);
			zend_execute(new_op_array, &local_retval);
		} zend_catch {
			destroy_op_array(new_op_array);
			efree_size(new_op_array, sizeof(zend_op_array));
			zend_bailout();
		} zend_end_try();

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
		destroy_op_array(new_op_array);
		efree_size(new_op_array, sizeof(zend_op_array));
		retval = SUCCESS;
	} else {
		retval = FAILURE;
	}
	zval_dtor(&pv);
	return retval;
}
/* }}} */

ZEND_API int zend_eval_string(char *str, zval *retval_ptr, char *string_name) /* {{{ */
{
	return zend_eval_stringl(str, strlen(str), retval_ptr, string_name);
}
/* }}} */

ZEND_API int zend_eval_stringl_ex(char *str, size_t str_len, zval *retval_ptr, char *string_name, int handle_exceptions) /* {{{ */
{
	int result;

	result = zend_eval_stringl(str, str_len, retval_ptr, string_name);
	if (handle_exceptions && EG(exception)) {
		zend_exception_error(EG(exception), E_ERROR);
		result = FAILURE;
	}
	return result;
}
/* }}} */

ZEND_API int zend_eval_string_ex(char *str, zval *retval_ptr, char *string_name, int handle_exceptions) /* {{{ */
{
	return zend_eval_stringl_ex(str, strlen(str), retval_ptr, string_name, handle_exceptions);
}
/* }}} */

static void zend_set_timeout_ex(zend_long seconds, int reset_signals);

ZEND_API ZEND_NORETURN void zend_timeout(int dummy) /* {{{ */
{
#if defined(PHP_WIN32)
# ifndef ZTS
	/* No action is needed if we're timed out because zero seconds are
	   just ignored. Also, the hard timeout needs to be respected. If the
	   timer is not restarted properly, it could hang in the shutdown
	   function. */
	if (EG(hard_timeout) > 0) {
		EG(timed_out) = 0;
		zend_set_timeout_ex(EG(hard_timeout), 1);
		/* XXX Abused, introduce an additional flag if the value needs to be kept. */
		EG(hard_timeout) = 0;
	}
# endif
#else
	EG(timed_out) = 0;
	zend_set_timeout_ex(0, 1);
#endif

	zend_error_noreturn(E_ERROR, "Maximum execution time of %pd second%s exceeded", EG(timeout_seconds), EG(timeout_seconds) == 1 ? "" : "s");
}
/* }}} */

#ifndef ZEND_WIN32
static void zend_timeout_handler(int dummy) /* {{{ */
{
#ifndef ZTS
    if (EG(timed_out)) {
		/* Die on hard timeout */
		const char *error_filename = NULL;
		uint error_lineno = 0;
		char *log_buffer = NULL;

		if (zend_is_compiling()) {
			error_filename = ZSTR_VAL(zend_get_compiled_filename());
			error_lineno = zend_get_compiled_lineno();
		} else if (zend_is_executing()) {
			error_filename = zend_get_executed_filename();
			if (error_filename[0] == '[') { /* [no active file] */
				error_filename = NULL;
				error_lineno = 0;
			} else {
				error_lineno = zend_get_executed_lineno();
			}
		}
		if (!error_filename) {
			error_filename = "Unknown";
		}

		zend_spprintf(&log_buffer, 0, "\nFatal error: Maximum execution time of %pd+%pd seconds exceeded (terminated) in %s on line %d\n", EG(timeout_seconds), EG(hard_timeout), error_filename, error_lineno);
		write(2, log_buffer, strlen(log_buffer));
		_exit(1);
    }
#endif

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
		zend_on_timeout(EG(timeout_seconds));
	}

	EG(timed_out) = 1;

#ifndef ZTS
	if (EG(hard_timeout) > 0) {
		/* Set hard timeout */
		zend_set_timeout_ex(EG(hard_timeout), 1);
	}
#endif
}
/* }}} */
#endif

#ifdef ZEND_WIN32
VOID CALLBACK tq_timer_cb(PVOID arg, BOOLEAN timed_out)
{
	zend_bool *php_timed_out;

	/* The doc states it'll be always true, however it theoretically
		could be FALSE when the thread was signaled. */
	if (!timed_out) {
		return;
	}

	php_timed_out = (zend_bool *)arg;
	*php_timed_out = 1;
}
#endif

/* This one doesn't exists on QNX */
#ifndef SIGPROF
#define SIGPROF 27
#endif

static void zend_set_timeout_ex(zend_long seconds, int reset_signals) /* {{{ */
{

#ifdef ZEND_WIN32
	if(!seconds) {
		return;
	}

        /* Don't use ChangeTimerQueueTimer() as it will not restart an expired
		timer, so we could end up with just an ignored timeout. Instead
		delete and recreate. */
	if (NULL != tq_timer) {
		if (!DeleteTimerQueueTimer(NULL, tq_timer, NULL)) {
			tq_timer = NULL;
			zend_error_noreturn(E_ERROR, "Could not delete queued timer");
			return;
		}
		tq_timer = NULL;
	}

	/* XXX passing NULL means the default timer queue provided by the system is used */
	if (!CreateTimerQueueTimer(&tq_timer, NULL, (WAITORTIMERCALLBACK)tq_timer_cb, (VOID*)&EG(timed_out), seconds*1000, 0, WT_EXECUTEONLYONCE)) {
		tq_timer = NULL;
		zend_error_noreturn(E_ERROR, "Could not queue new timer");
		return;
	}
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
			zend_signal(signo, zend_timeout_handler);
#	else
			sigset_t sigset;
#   ifdef HAVE_SIGACTION
			struct sigaction act;

			act.sa_handler = zend_timeout_handler;
			sigemptyset(&act.sa_mask);
			act.sa_flags = SA_RESETHAND | SA_NODEFER;
			sigaction(signo, &act, NULL);
#   else
			signal(signo, zend_timeout_handler);
#   endif /* HAVE_SIGACTION */
			sigemptyset(&sigset);
			sigaddset(&sigset, signo);
			sigprocmask(SIG_UNBLOCK, &sigset, NULL);
#	endif /* ZEND_SIGNALS */
		}
	}
#	endif /* HAVE_SETITIMER */
#endif
}
/* }}} */

void zend_set_timeout(zend_long seconds, int reset_signals) /* {{{ */
{

	EG(timeout_seconds) = seconds;
	zend_set_timeout_ex(seconds, reset_signals);
	EG(timed_out) = 0;
}
/* }}} */

void zend_unset_timeout(void) /* {{{ */
{
#ifdef ZEND_WIN32
	if (NULL != tq_timer) {
		if (!DeleteTimerQueueTimer(NULL, tq_timer, NULL)) {
			EG(timed_out) = 0;
			tq_timer = NULL;
			zend_error_noreturn(E_ERROR, "Could not delete queued timer");
			return;
		}
		tq_timer = NULL;
	}
	EG(timed_out) = 0;
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
	EG(timed_out) = 0;
#endif
}
/* }}} */

zend_class_entry *zend_fetch_class(zend_string *class_name, int fetch_type) /* {{{ */
{
	zend_class_entry *ce, *scope;
	int fetch_sub_type = fetch_type & ZEND_FETCH_CLASS_MASK;

check_fetch_type:
	switch (fetch_sub_type) {
		case ZEND_FETCH_CLASS_SELF:
			scope = zend_get_executed_scope();
			if (UNEXPECTED(!scope)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access self:: when no class scope is active");
			}
			return scope;
		case ZEND_FETCH_CLASS_PARENT:
			scope = zend_get_executed_scope();
			if (UNEXPECTED(!scope)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access parent:: when no class scope is active");
				return NULL;
			}
			if (UNEXPECTED(!scope->parent)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access parent:: when current class scope has no parent");
			}
			return scope->parent;
		case ZEND_FETCH_CLASS_STATIC:
			ce = zend_get_called_scope(EG(current_execute_data));
			if (UNEXPECTED(!ce)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access static:: when no class scope is active");
				return NULL;
			}
			return ce;
		case ZEND_FETCH_CLASS_AUTO: {
				fetch_sub_type = zend_get_class_fetch_type(class_name);
				if (UNEXPECTED(fetch_sub_type != ZEND_FETCH_CLASS_DEFAULT)) {
					goto check_fetch_type;
				}
			}
			break;
	}

	if (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) {
		return zend_lookup_class_ex(class_name, NULL, 0);
	} else if ((ce = zend_lookup_class_ex(class_name, NULL, 1)) == NULL) {
		if (!(fetch_type & ZEND_FETCH_CLASS_SILENT) && !EG(exception)) {
			if (fetch_sub_type == ZEND_FETCH_CLASS_INTERFACE) {
				zend_throw_or_error(fetch_type, NULL, "Interface '%s' not found", ZSTR_VAL(class_name));
			} else if (fetch_sub_type == ZEND_FETCH_CLASS_TRAIT) {
				zend_throw_or_error(fetch_type, NULL, "Trait '%s' not found", ZSTR_VAL(class_name));
			} else {
				zend_throw_or_error(fetch_type, NULL, "Class '%s' not found", ZSTR_VAL(class_name));
			}
		}
		return NULL;
	}
	return ce;
}
/* }}} */

zend_class_entry *zend_fetch_class_by_name(zend_string *class_name, const zval *key, int fetch_type) /* {{{ */
{
	zend_class_entry *ce;

	if (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) {
		return zend_lookup_class_ex(class_name, key, 0);
	} else if ((ce = zend_lookup_class_ex(class_name, key, 1)) == NULL) {
		if ((fetch_type & ZEND_FETCH_CLASS_SILENT) == 0 && !EG(exception)) {
			if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_INTERFACE) {
				zend_throw_or_error(fetch_type, NULL, "Interface '%s' not found", ZSTR_VAL(class_name));
			} else if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_TRAIT) {
				zend_throw_or_error(fetch_type, NULL, "Trait '%s' not found", ZSTR_VAL(class_name));
			} else {
				zend_throw_or_error(fetch_type, NULL, "Class '%s' not found", ZSTR_VAL(class_name));
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
	ai.afn[idx] ? ZSTR_VAL(ai.afn[idx]->common.function_name) : "", \
	ai.afn[idx] && ai.afn[idx + 1] ? ", " : (ai.afn[idx] && ai.cnt > MAX_ABSTRACT_INFO_CNT ? ", ..." : "")

typedef struct _zend_abstract_info {
	zend_function *afn[MAX_ABSTRACT_INFO_CNT + 1];
	int cnt;
	int ctor;
} zend_abstract_info;

static void zend_verify_abstract_class_function(zend_function *fn, zend_abstract_info *ai) /* {{{ */
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
}
/* }}} */

void zend_verify_abstract_class(zend_class_entry *ce) /* {{{ */
{
	zend_function *func;
	zend_abstract_info ai;

	if ((ce->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS) && !(ce->ce_flags & (ZEND_ACC_TRAIT | ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))) {
		memset(&ai, 0, sizeof(ai));

		ZEND_HASH_FOREACH_PTR(&ce->function_table, func) {
			zend_verify_abstract_class_function(func, &ai);
		} ZEND_HASH_FOREACH_END();

		if (ai.cnt) {
			zend_error_noreturn(E_ERROR, "Class %s contains %d abstract method%s and must therefore be declared abstract or implement the remaining methods (" MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT ")",
				ZSTR_VAL(ce->name), ai.cnt,
				ai.cnt > 1 ? "s" : "",
				DISPLAY_ABSTRACT_FN(0),
				DISPLAY_ABSTRACT_FN(1),
				DISPLAY_ABSTRACT_FN(2)
				);
		}
	}
}
/* }}} */

ZEND_API int zend_delete_global_variable(zend_string *name) /* {{{ */
{
    return zend_hash_del_ind(&EG(symbol_table), name);
}
/* }}} */

ZEND_API zend_array *zend_rebuild_symbol_table(void) /* {{{ */
{
	zend_execute_data *ex;
	zend_array *symbol_table;

	/* Search for last called user function */
	ex = EG(current_execute_data);
	while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->common.type))) {
		ex = ex->prev_execute_data;
	}
	if (!ex) {
		return NULL;
	}
	if (ZEND_CALL_INFO(ex) & ZEND_CALL_HAS_SYMBOL_TABLE) {
		return ex->symbol_table;
	}

	ZEND_ADD_CALL_FLAG(ex, ZEND_CALL_HAS_SYMBOL_TABLE);
	if (EG(symtable_cache_ptr) >= EG(symtable_cache)) {
		/*printf("Cache hit!  Reusing %x\n", symtable_cache[symtable_cache_ptr]);*/
		symbol_table = ex->symbol_table = *(EG(symtable_cache_ptr)--);
		if (!ex->func->op_array.last_var) {
			return symbol_table;
		}
		zend_hash_extend(symbol_table, ex->func->op_array.last_var, 0);
	} else {
		symbol_table = ex->symbol_table = emalloc(sizeof(zend_array));
		zend_hash_init(symbol_table, ex->func->op_array.last_var, NULL, ZVAL_PTR_DTOR, 0);
		if (!ex->func->op_array.last_var) {
			return symbol_table;
		}
		zend_hash_real_init(symbol_table, 0);
		/*printf("Cache miss!  Initialized %x\n", EG(active_symbol_table));*/
	}
	if (EXPECTED(ex->func->op_array.last_var)) {
		zend_string **str = ex->func->op_array.vars;
		zend_string **end = str + ex->func->op_array.last_var;
		zval *var = ZEND_CALL_VAR_NUM(ex, 0);

		do {
			_zend_hash_append_ind(symbol_table, *str, var);
			str++;
			var++;
		} while (str != end);
	}
	return symbol_table;
}
/* }}} */

ZEND_API void zend_attach_symbol_table(zend_execute_data *execute_data) /* {{{ */
{
	zend_op_array *op_array = &execute_data->func->op_array;
	HashTable *ht = execute_data->symbol_table;

	/* copy real values from symbol table into CV slots and create
	   INDIRECT references to CV in symbol table  */
	if (EXPECTED(op_array->last_var)) {
		zend_string **str = op_array->vars;
		zend_string **end = str + op_array->last_var;
		zval *var = EX_VAR_NUM(0);

		do {
			zval *zv = zend_hash_find(ht, *str);

			if (zv) {
				if (Z_TYPE_P(zv) == IS_INDIRECT) {
					zval *val = Z_INDIRECT_P(zv);

					ZVAL_COPY_VALUE(var, val);
				} else {
					ZVAL_COPY_VALUE(var, zv);
				}
			} else {
				ZVAL_UNDEF(var);
				zv = zend_hash_add_new(ht, *str, var);
			}
			ZVAL_INDIRECT(zv, var);
			str++;
			var++;
		} while (str != end);
	}
}
/* }}} */

ZEND_API void zend_detach_symbol_table(zend_execute_data *execute_data) /* {{{ */
{
	zend_op_array *op_array = &execute_data->func->op_array;
	HashTable *ht = execute_data->symbol_table;

	/* copy real values from CV slots into symbol table */
	if (EXPECTED(op_array->last_var)) {
		zend_string **str = op_array->vars;
		zend_string **end = str + op_array->last_var;
		zval *var = EX_VAR_NUM(0);

		do {
			if (Z_TYPE_P(var) == IS_UNDEF) {
				zend_hash_del(ht, *str);
			} else {
				zend_hash_update(ht, *str, var);
				ZVAL_UNDEF(var);
			}
			str++;
			var++;
		} while (str != end);
	}
}
/* }}} */

ZEND_API int zend_set_local_var(zend_string *name, zval *value, int force) /* {{{ */
{
	zend_execute_data *execute_data = EG(current_execute_data);

	while (execute_data && (!execute_data->func || !ZEND_USER_CODE(execute_data->func->common.type))) {
		execute_data = execute_data->prev_execute_data;
	}

	if (execute_data) {
		if (!(EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE)) {
			zend_ulong h = zend_string_hash_val(name);
			zend_op_array *op_array = &execute_data->func->op_array;

			if (EXPECTED(op_array->last_var)) {
				zend_string **str = op_array->vars;
				zend_string **end = str + op_array->last_var;

				do {
					if (ZSTR_H(*str) == h &&
					    ZSTR_LEN(*str) == ZSTR_LEN(name) &&
					    memcmp(ZSTR_VAL(*str), ZSTR_VAL(name), ZSTR_LEN(name)) == 0) {
						zval *var = EX_VAR_NUM(str - op_array->vars);
						ZVAL_COPY_VALUE(var, value);
						return SUCCESS;
					}
					str++;
				} while (str != end);
			}
			if (force) {
				zend_array *symbol_table = zend_rebuild_symbol_table();
				if (symbol_table) {
					return zend_hash_update(symbol_table, name, value) ? SUCCESS : FAILURE;;
				}
			}
		} else {
			return (zend_hash_update_ind(execute_data->symbol_table, name, value) != NULL) ? SUCCESS : FAILURE;
		}
	}
	return FAILURE;
}
/* }}} */

ZEND_API int zend_set_local_var_str(const char *name, size_t len, zval *value, int force) /* {{{ */
{
	zend_execute_data *execute_data = EG(current_execute_data);

	while (execute_data && (!execute_data->func || !ZEND_USER_CODE(execute_data->func->common.type))) {
		execute_data = execute_data->prev_execute_data;
	}

	if (execute_data) {
		if (!(EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE)) {
			zend_ulong h = zend_hash_func(name, len);
			zend_op_array *op_array = &execute_data->func->op_array;
			if (EXPECTED(op_array->last_var)) {
				zend_string **str = op_array->vars;
				zend_string **end = str + op_array->last_var;

				do {
					if (ZSTR_H(*str) == h &&
					    ZSTR_LEN(*str) == len &&
					    memcmp(ZSTR_VAL(*str), name, len) == 0) {
						zval *var = EX_VAR_NUM(str - op_array->vars);
						zval_ptr_dtor(var);
						ZVAL_COPY_VALUE(var, value);
						return SUCCESS;
					}
					str++;
				} while (str != end);
			}
			if (force) {
				zend_array *symbol_table = zend_rebuild_symbol_table();
				if (symbol_table) {
					return zend_hash_str_update(symbol_table, name, len, value) ? SUCCESS : FAILURE;;
				}
			}
		} else {
			return (zend_hash_str_update_ind(execute_data->symbol_table, name, len, value) != NULL) ? SUCCESS : FAILURE;
		}
	}
	return FAILURE;
}
/* }}} */

ZEND_API int zend_forbid_dynamic_call(const char *func_name) /* {{{ */
{
	zend_execute_data *ex = EG(current_execute_data);
	ZEND_ASSERT(ex != NULL && ex->func != NULL);

	if (ZEND_CALL_INFO(ex) & ZEND_CALL_DYNAMIC) {
		zend_error(E_WARNING, "Cannot call %s dynamically", func_name);
		return FAILURE;
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
