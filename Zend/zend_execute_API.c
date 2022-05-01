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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

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
#include "zend_weakrefs.h"
#include "zend_inheritance.h"
#include "zend_observer.h"
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

ZEND_API void (*zend_execute_ex)(zend_execute_data *execute_data);
ZEND_API void (*zend_execute_internal)(zend_execute_data *execute_data, zval *return_value);
ZEND_API zend_class_entry *(*zend_autoload)(zend_string *name, zend_string *lc_name);

/* true globals */
ZEND_API const zend_fcall_info empty_fcall_info = {0};
ZEND_API const zend_fcall_info_cache empty_fcall_info_cache = { NULL, NULL, NULL, NULL };

#ifdef ZEND_WIN32
ZEND_TLS HANDLE tq_timer = NULL;
#endif

#if 0&&ZEND_DEBUG
static void (*original_sigsegv_handler)(int);
static void zend_handle_sigsegv(void) /* {{{ */
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

static int clean_non_persistent_constant_full(zval *zv) /* {{{ */
{
	zend_constant *c = Z_PTR_P(zv);
	return (ZEND_CONSTANT_FLAGS(c) & CONST_PERSISTENT) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static int clean_non_persistent_function_full(zval *zv) /* {{{ */
{
	zend_function *function = Z_PTR_P(zv);
	return (function->type == ZEND_INTERNAL_FUNCTION) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static int clean_non_persistent_class_full(zval *zv) /* {{{ */
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

	EG(symtable_cache_ptr) = EG(symtable_cache);
	EG(symtable_cache_limit) = EG(symtable_cache) + SYMTABLE_CACHE_SIZE;
	EG(no_extensions) = 0;

	EG(function_table) = CG(function_table);
	EG(class_table) = CG(class_table);

	EG(in_autoload) = NULL;
	EG(error_handling) = EH_NORMAL;
	EG(flags) = EG_FLAGS_INITIAL;

	zend_vm_stack_init();

	zend_hash_init(&EG(symbol_table), 64, NULL, ZVAL_PTR_DTOR, 0);

	zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_activator);

	zend_hash_init(&EG(included_files), 8, NULL, NULL, 0);

	EG(ticks_count) = 0;

	ZVAL_UNDEF(&EG(user_error_handler));
	ZVAL_UNDEF(&EG(user_exception_handler));

	EG(current_execute_data) = NULL;

	zend_stack_init(&EG(user_error_handlers_error_reporting), sizeof(int));
	zend_stack_init(&EG(user_error_handlers), sizeof(zval));
	zend_stack_init(&EG(user_exception_handlers), sizeof(zval));

	zend_objects_store_init(&EG(objects_store), 1024);

	EG(full_tables_cleanup) = 0;
	EG(vm_interrupt) = 0;
	EG(timed_out) = 0;

	EG(exception) = NULL;
	EG(prev_exception) = NULL;

	EG(fake_scope) = NULL;
	EG(trampoline).common.function_name = NULL;

	EG(ht_iterators_count) = sizeof(EG(ht_iterators_slots)) / sizeof(HashTableIterator);
	EG(ht_iterators_used) = 0;
	EG(ht_iterators) = EG(ht_iterators_slots);
	memset(EG(ht_iterators), 0, sizeof(EG(ht_iterators_slots)));

	EG(persistent_constants_count) = EG(zend_constants)->nNumUsed;
	EG(persistent_functions_count) = EG(function_table)->nNumUsed;
	EG(persistent_classes_count)   = EG(class_table)->nNumUsed;

	EG(get_gc_buffer).start = EG(get_gc_buffer).end = EG(get_gc_buffer).cur = NULL;

	zend_weakrefs_init();

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
	i_zval_ptr_dtor(zv);
}
/* }}} */

static ZEND_COLD void zend_throw_or_error(int fetch_type, zend_class_entry *exception_ce, const char *format, ...) /* {{{ */
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
	zend_string *key;
	zval *zv;
#if ZEND_DEBUG
	zend_bool fast_shutdown = 0;
#else
	zend_bool fast_shutdown = is_zend_mm() && !EG(full_tables_cleanup);
#endif

	zend_try {
		zend_llist_destroy(&CG(open_files));
	} zend_end_try();

	EG(flags) |= EG_FLAGS_IN_RESOURCE_SHUTDOWN;
	zend_try {
		zend_close_rsrc_list(&EG(regular_list));
	} zend_end_try();

	/* No PHP callback functions should be called after this point. */
	EG(active) = 0;

	if (!fast_shutdown) {
		zend_hash_graceful_reverse_destroy(&EG(symbol_table));

		/* Release static properties and static variables prior to the final GC run,
		 * as they may hold GC roots. */
		ZEND_HASH_REVERSE_FOREACH_VAL(EG(function_table), zv) {
			zend_op_array *op_array = Z_PTR_P(zv);
			if (op_array->type == ZEND_INTERNAL_FUNCTION) {
				break;
			}
			if (op_array->static_variables) {
				HashTable *ht = ZEND_MAP_PTR_GET(op_array->static_variables_ptr);
				if (ht) {
					zend_array_release(ht);
					ZEND_MAP_PTR_SET(op_array->static_variables_ptr, NULL);
				}
			}
		} ZEND_HASH_FOREACH_END();
		ZEND_HASH_REVERSE_FOREACH_VAL(EG(class_table), zv) {
			zend_class_entry *ce = Z_PTR_P(zv);
			if (ce->default_static_members_count) {
				zend_cleanup_internal_class_data(ce);
			}
			if (ce->ce_flags & ZEND_HAS_STATIC_IN_METHODS) {
				zend_op_array *op_array;
				ZEND_HASH_FOREACH_PTR(&ce->function_table, op_array) {
					if (op_array->type == ZEND_USER_FUNCTION) {
						if (op_array->static_variables) {
							HashTable *ht = ZEND_MAP_PTR_GET(op_array->static_variables_ptr);
							if (ht) {
								zend_array_release(ht);
								ZEND_MAP_PTR_SET(op_array->static_variables_ptr, NULL);
							}
						}
					}
				} ZEND_HASH_FOREACH_END();
			}
		} ZEND_HASH_FOREACH_END();

		/* Also release error and exception handlers, which may hold objects. */
		if (Z_TYPE(EG(user_error_handler)) != IS_UNDEF) {
			zval_ptr_dtor(&EG(user_error_handler));
			ZVAL_UNDEF(&EG(user_error_handler));
		}

		if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
			zval_ptr_dtor(&EG(user_exception_handler));
			ZVAL_UNDEF(&EG(user_exception_handler));
		}

		zend_stack_clean(&EG(user_error_handlers_error_reporting), NULL, 1);
		zend_stack_clean(&EG(user_error_handlers), (void (*)(void *))ZVAL_PTR_DTOR, 1);
		zend_stack_clean(&EG(user_exception_handlers), (void (*)(void *))ZVAL_PTR_DTOR, 1);

#if ZEND_DEBUG
		if (!CG(unclean_shutdown)) {
			gc_collect_cycles();
		}
#endif
	}

	zend_objects_store_free_object_storage(&EG(objects_store), fast_shutdown);

	zend_weakrefs_shutdown();

	zend_try {
		zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_deactivator);
	} zend_end_try();

	if (fast_shutdown) {
		/* Fast Request Shutdown
		 * =====================
		 * Zend Memory Manager frees memory by its own. We don't have to free
		 * each allocated block separately.
		 */
		zend_hash_discard(EG(zend_constants), EG(persistent_constants_count));
		zend_hash_discard(EG(function_table), EG(persistent_functions_count));
		zend_hash_discard(EG(class_table), EG(persistent_classes_count));
		zend_cleanup_internal_classes();
	} else {
		zend_vm_stack_destroy();

		if (EG(full_tables_cleanup)) {
			zend_hash_reverse_apply(EG(zend_constants), clean_non_persistent_constant_full);
			zend_hash_reverse_apply(EG(function_table), clean_non_persistent_function_full);
			zend_hash_reverse_apply(EG(class_table), clean_non_persistent_class_full);
		} else {
			ZEND_HASH_REVERSE_FOREACH_STR_KEY_VAL(EG(zend_constants), key, zv) {
				zend_constant *c = Z_PTR_P(zv);
				if (_idx == EG(persistent_constants_count)) {
					break;
				}
				zval_ptr_dtor_nogc(&c->value);
				if (c->name) {
					zend_string_release_ex(c->name, 0);
				}
				efree(c);
				zend_string_release_ex(key, 0);
			} ZEND_HASH_FOREACH_END_DEL();

			ZEND_HASH_REVERSE_FOREACH_STR_KEY_VAL(EG(function_table), key, zv) {
				zend_function *func = Z_PTR_P(zv);
				if (_idx == EG(persistent_functions_count)) {
					break;
				}
				destroy_op_array(&func->op_array);
				zend_string_release_ex(key, 0);
			} ZEND_HASH_FOREACH_END_DEL();

			ZEND_HASH_REVERSE_FOREACH_STR_KEY_VAL(EG(class_table), key, zv) {
				if (_idx == EG(persistent_classes_count)) {
					break;
				}
				destroy_zend_class(zv);
				zend_string_release_ex(key, 0);
			} ZEND_HASH_FOREACH_END_DEL();
		}

		while (EG(symtable_cache_ptr) > EG(symtable_cache)) {
			EG(symtable_cache_ptr)--;
			zend_hash_destroy(*EG(symtable_cache_ptr));
			FREE_HASHTABLE(*EG(symtable_cache_ptr));
		}

		zend_hash_destroy(&EG(included_files));

		zend_stack_destroy(&EG(user_error_handlers_error_reporting));
		zend_stack_destroy(&EG(user_error_handlers));
		zend_stack_destroy(&EG(user_exception_handlers));
		zend_objects_store_destroy(&EG(objects_store));
		if (EG(in_autoload)) {
			zend_hash_destroy(EG(in_autoload));
			FREE_HASHTABLE(EG(in_autoload));
		}

		if (EG(ht_iterators) != EG(ht_iterators_slots)) {
			efree(EG(ht_iterators));
		}
	}

#if ZEND_DEBUG
	if (EG(ht_iterators_used) && !CG(unclean_shutdown)) {
		zend_error(E_WARNING, "Leaked %" PRIu32 " hashtable iterators", EG(ht_iterators_used));
	}
#endif

	/* Check whether anyone is hogging the trampoline. */
	ZEND_ASSERT(EG(trampoline).common.function_name == NULL || CG(unclean_shutdown));

	EG(ht_iterators_used) = 0;

	zend_shutdown_fpu();
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

ZEND_API zend_string *get_active_function_or_method_name(void) /* {{{ */
{
	ZEND_ASSERT(zend_is_executing());

	return get_function_or_method_name(EG(current_execute_data)->func);
}
/* }}} */

ZEND_API zend_string *get_function_or_method_name(const zend_function *func) /* {{{ */
{
	if (func->common.scope && func->common.function_name) {
		return zend_create_member_string(func->common.scope->name, func->common.function_name);
	}

	return func->common.function_name ? zend_string_copy(func->common.function_name) : zend_string_init("main", sizeof("main") - 1, 0);
}
/* }}} */

ZEND_API const char *get_active_function_arg_name(uint32_t arg_num) /* {{{ */
{
	zend_function *func;

	if (!zend_is_executing()) {
		return NULL;
	}

	func = EG(current_execute_data)->func;

	return get_function_arg_name(func, arg_num);
}
/* }}} */

ZEND_API const char *get_function_arg_name(const zend_function *func, uint32_t arg_num) /* {{{ */
{
	if (!func || func->common.num_args < arg_num) {
		return NULL;
	}

	if (func->type == ZEND_USER_FUNCTION || (func->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
		return ZSTR_VAL(func->common.arg_info[arg_num - 1].name);
	} else {
		return ((zend_internal_arg_info*) func->common.arg_info)[arg_num - 1].name;
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

ZEND_API uint32_t zend_get_executed_lineno(void) /* {{{ */
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

ZEND_API zend_result zval_update_constant_ex(zval *p, zend_class_entry *scope) /* {{{ */
{
	if (Z_TYPE_P(p) == IS_CONSTANT_AST) {
		zend_ast *ast = Z_ASTVAL_P(p);

		if (ast->kind == ZEND_AST_CONSTANT) {
			zend_string *name = zend_ast_get_constant_name(ast);
			zval *zv = zend_get_constant_ex(name, scope, ast->attr);
			if (UNEXPECTED(zv == NULL)) {
				return FAILURE;
			}

			zval_ptr_dtor_nogc(p);
			ZVAL_COPY_OR_DUP(p, zv);
		} else {
			zval tmp;

			if (UNEXPECTED(zend_ast_evaluate(&tmp, ast, scope) != SUCCESS)) {
				return FAILURE;
			}
			zval_ptr_dtor_nogc(p);
			ZVAL_COPY_VALUE(p, &tmp);
		}
	}
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result zval_update_constant(zval *pp) /* {{{ */
{
	return zval_update_constant_ex(pp, EG(current_execute_data) ? zend_get_executed_scope() : CG(active_class_entry));
}
/* }}} */

zend_result _call_user_function_impl(zval *object, zval *function_name, zval *retval_ptr, uint32_t param_count, zval params[], HashTable *named_params) /* {{{ */
{
	zend_fcall_info fci;

	fci.size = sizeof(fci);
	fci.object = object ? Z_OBJ_P(object) : NULL;
	ZVAL_COPY_VALUE(&fci.function_name, function_name);
	fci.retval = retval_ptr;
	fci.param_count = param_count;
	fci.params = params;
	fci.named_params = named_params;

	return zend_call_function(&fci, NULL);
}
/* }}} */

zend_result zend_call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache) /* {{{ */
{
	uint32_t i;
	zend_execute_data *call, dummy_execute_data;
	zend_fcall_info_cache fci_cache_local;
	zend_function *func;
	uint32_t call_info;
	void *object_or_called_scope;
	zend_class_entry *orig_fake_scope;

	ZVAL_UNDEF(fci->retval);

	if (!EG(active)) {
		return FAILURE; /* executor is already inactive */
	}

	if (EG(exception)) {
		return FAILURE; /* we would result in an instable executor otherwise */
	}

	ZEND_ASSERT(fci->size == sizeof(zend_fcall_info));

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

	if (!fci_cache || !fci_cache->function_handler) {
		char *error = NULL;

		if (!fci_cache) {
			fci_cache = &fci_cache_local;
		}

		if (!zend_is_callable_ex(&fci->function_name, fci->object, IS_CALLABLE_CHECK_SILENT, NULL, fci_cache, &error)) {
			if (error) {
				zend_string *callable_name
					= zend_get_callable_name_ex(&fci->function_name, fci->object);
				zend_error(E_WARNING, "Invalid callback %s, %s", ZSTR_VAL(callable_name), error);
				efree(error);
				zend_string_release_ex(callable_name, 0);
			}
			if (EG(current_execute_data) == &dummy_execute_data) {
				EG(current_execute_data) = dummy_execute_data.prev_execute_data;
			}
			return FAILURE;
		}

		ZEND_ASSERT(!error);
	}

	func = fci_cache->function_handler;
	if ((func->common.fn_flags & ZEND_ACC_STATIC) || !fci_cache->object) {
		fci->object = NULL;
		object_or_called_scope = fci_cache->called_scope;
		call_info = ZEND_CALL_TOP_FUNCTION | ZEND_CALL_DYNAMIC;
	} else {
		fci->object = fci_cache->object;
		object_or_called_scope = fci->object;
		call_info = ZEND_CALL_TOP_FUNCTION | ZEND_CALL_DYNAMIC | ZEND_CALL_HAS_THIS;
	}

	call = zend_vm_stack_push_call_frame(call_info,
		func, fci->param_count, object_or_called_scope);

	if (UNEXPECTED(func->common.fn_flags & ZEND_ACC_DEPRECATED)) {
		zend_deprecated_function(func);

		if (UNEXPECTED(EG(exception))) {
			zend_vm_stack_free_call_frame(call);
			if (EG(current_execute_data) == &dummy_execute_data) {
				EG(current_execute_data) = dummy_execute_data.prev_execute_data;
				zend_rethrow_exception(EG(current_execute_data));
			}
			return FAILURE;
		}
	}

	for (i=0; i<fci->param_count; i++) {
		zval *param = ZEND_CALL_ARG(call, i+1);
		zval *arg = &fci->params[i];
		zend_bool must_wrap = 0;
		if (UNEXPECTED(Z_ISUNDEF_P(arg))) {
			/* Allow forwarding undef slots. This is only used by Closure::__invoke(). */
			ZVAL_UNDEF(param);
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_MAY_HAVE_UNDEF);
			continue;
		}

		if (ARG_SHOULD_BE_SENT_BY_REF(func, i + 1)) {
			if (UNEXPECTED(!Z_ISREF_P(arg))) {
				if (!ARG_MAY_BE_SENT_BY_REF(func, i + 1)) {
					/* By-value send is not allowed -- emit a warning,
					 * and perform the call with the value wrapped in a reference. */
					zend_param_must_be_ref(func, i + 1);
					must_wrap = 1;
					if (UNEXPECTED(EG(exception))) {
						ZEND_CALL_NUM_ARGS(call) = i;
cleanup_args:
						zend_vm_stack_free_args(call);
						zend_vm_stack_free_call_frame(call);
						if (EG(current_execute_data) == &dummy_execute_data) {
							EG(current_execute_data) = dummy_execute_data.prev_execute_data;
						}
						return FAILURE;
					}
				}
			}
		} else {
			if (Z_ISREF_P(arg) &&
			    !(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
				/* don't separate references for __call */
				arg = Z_REFVAL_P(arg);
			}
		}

		if (EXPECTED(!must_wrap)) {
			ZVAL_COPY(param, arg);
		} else {
			Z_TRY_ADDREF_P(arg);
			ZVAL_NEW_REF(param, arg);
		}
	}

	if (fci->named_params) {
		zend_string *name;
		zval *arg;
		uint32_t arg_num = ZEND_CALL_NUM_ARGS(call) + 1;
		zend_bool have_named_params = 0;
		ZEND_HASH_FOREACH_STR_KEY_VAL(fci->named_params, name, arg) {
			zend_bool must_wrap = 0;
			zval *target;
			if (name) {
				void *cache_slot[2] = {NULL, NULL};
				have_named_params = 1;
				target = zend_handle_named_arg(&call, name, &arg_num, cache_slot);
				if (!target) {
					goto cleanup_args;
				}
			} else {
				if (have_named_params) {
					zend_throw_error(NULL,
						"Cannot use positional argument after named argument");
					goto cleanup_args;
				}

				zend_vm_stack_extend_call_frame(&call, arg_num - 1, 1);
				target = ZEND_CALL_ARG(call, arg_num);
			}

			if (ARG_SHOULD_BE_SENT_BY_REF(func, arg_num)) {
				if (UNEXPECTED(!Z_ISREF_P(arg))) {
					if (!ARG_MAY_BE_SENT_BY_REF(func, arg_num)) {
						/* By-value send is not allowed -- emit a warning,
						 * and perform the call with the value wrapped in a reference. */
						zend_param_must_be_ref(func, arg_num);
						must_wrap = 1;
						if (UNEXPECTED(EG(exception))) {
							goto cleanup_args;
						}
					}
				}
			} else {
				if (Z_ISREF_P(arg) &&
					!(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
					/* don't separate references for __call */
					arg = Z_REFVAL_P(arg);
				}
			}

			if (EXPECTED(!must_wrap)) {
				ZVAL_COPY(target, arg);
			} else {
				Z_TRY_ADDREF_P(arg);
				ZVAL_NEW_REF(target, arg);
			}
			if (!name) {
				ZEND_CALL_NUM_ARGS(call)++;
				arg_num++;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_MAY_HAVE_UNDEF)) {
		if (zend_handle_undef_args(call) == FAILURE) {
			zend_vm_stack_free_args(call);
			zend_vm_stack_free_call_frame(call);
			if (EG(current_execute_data) == &dummy_execute_data) {
				EG(current_execute_data) = dummy_execute_data.prev_execute_data;
			}
			return SUCCESS;
		}
	}

	if (UNEXPECTED(func->op_array.fn_flags & ZEND_ACC_CLOSURE)) {
		uint32_t call_info;

		GC_ADDREF(ZEND_CLOSURE_OBJECT(func));
		call_info = ZEND_CALL_CLOSURE;
		if (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) {
			call_info |= ZEND_CALL_FAKE_CLOSURE;
		}
		ZEND_ADD_CALL_FLAG(call, call_info);
	}

	orig_fake_scope = EG(fake_scope);
	EG(fake_scope) = NULL;
	if (func->type == ZEND_USER_FUNCTION) {
		int call_via_handler = (func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) != 0;
		const zend_op *current_opline_before_exception = EG(opline_before_exception);
		uint32_t orig_jit_trace_num = EG(jit_trace_num);

		zend_init_func_execute_data(call, &func->op_array, fci->retval);
		ZEND_OBSERVER_FCALL_BEGIN(call);
		zend_execute_ex(call);
		EG(jit_trace_num) = orig_jit_trace_num;
		EG(opline_before_exception) = current_opline_before_exception;
		if (call_via_handler) {
			/* We must re-initialize function again */
			fci_cache->function_handler = NULL;
		}
	} else {
		int call_via_handler = (func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) != 0;

		ZEND_ASSERT(func->type == ZEND_INTERNAL_FUNCTION);
		ZVAL_NULL(fci->retval);
		call->prev_execute_data = EG(current_execute_data);
		EG(current_execute_data) = call;
		if (EXPECTED(zend_execute_internal == NULL)) {
			/* saves one function call if zend_execute_internal is not used */
			func->internal_function.handler(call, fci->retval);
		} else {
			zend_execute_internal(call, fci->retval);
		}
		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_args(call);
		if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS)) {
			zend_array_release(call->extra_named_params);
		}

		if (EG(exception)) {
			zval_ptr_dtor(fci->retval);
			ZVAL_UNDEF(fci->retval);
		}

		if (call_via_handler) {
			/* We must re-initialize function again */
			fci_cache->function_handler = NULL;
		}

		/* This flag is regularly checked while running user functions, but not internal
		 * So see whether interrupt flag was set while the function was running... */
		if (EG(vm_interrupt)) {
			EG(vm_interrupt) = 0;
			if (EG(timed_out)) {
				zend_timeout();
			} else if (zend_interrupt_function) {
				zend_interrupt_function(EG(current_execute_data));
			}
		}
	}
	EG(fake_scope) = orig_fake_scope;

	zend_vm_stack_free_call_frame(call);

	if (EG(current_execute_data) == &dummy_execute_data) {
		EG(current_execute_data) = dummy_execute_data.prev_execute_data;
	}

	if (UNEXPECTED(EG(exception))) {
		if (UNEXPECTED(!EG(current_execute_data))) {
			zend_throw_exception_internal(NULL);
		} else if (EG(current_execute_data)->func &&
		           ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) {
			zend_rethrow_exception(EG(current_execute_data));
		}
	}

	return SUCCESS;
}
/* }}} */

ZEND_API void zend_call_known_function(
		zend_function *fn, zend_object *object, zend_class_entry *called_scope, zval *retval_ptr,
		uint32_t param_count, zval *params, HashTable *named_params)
{
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fcic;

	ZEND_ASSERT(fn && "zend_function must be passed!");

	fci.size = sizeof(fci);
	fci.object = object;
	fci.retval = retval_ptr ? retval_ptr : &retval;
	fci.param_count = param_count;
	fci.params = params;
	fci.named_params = named_params;
	ZVAL_UNDEF(&fci.function_name); /* Unused */

	fcic.function_handler = fn;
	fcic.object = object;
	fcic.called_scope = called_scope;

	zend_result result = zend_call_function(&fci, &fcic);
	if (UNEXPECTED(result == FAILURE)) {
		if (!EG(exception)) {
			zend_error_noreturn(E_CORE_ERROR, "Couldn't execute method %s%s%s",
				fn->common.scope ? ZSTR_VAL(fn->common.scope->name) : "",
				fn->common.scope ? "::" : "", ZSTR_VAL(fn->common.function_name));
		}
	}

	if (!retval_ptr) {
		zval_ptr_dtor(&retval);
	}
}

ZEND_API void zend_call_known_instance_method_with_2_params(
		zend_function *fn, zend_object *object, zval *retval_ptr, zval *param1, zval *param2)
{
	zval params[2];
	ZVAL_COPY_VALUE(&params[0], param1);
	ZVAL_COPY_VALUE(&params[1], param2);
	zend_call_known_instance_method(fn, object, retval_ptr, 2, params);
}

/* 0-9 a-z A-Z _ \ 0x80-0xff */
static const uint32_t valid_chars[8] = {
	0x00000000,
	0x03ff0000,
	0x97fffffe,
	0x07fffffe,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
};

ZEND_API zend_bool zend_is_valid_class_name(zend_string *name) {
	for (size_t i = 0; i < ZSTR_LEN(name); i++) {
		unsigned char c = ZSTR_VAL(name)[i];
		if (!ZEND_BIT_TEST(valid_chars, c)) {
			return 0;
		}
	}
	return 1;
}

ZEND_API zend_class_entry *zend_lookup_class_ex(zend_string *name, zend_string *key, uint32_t flags) /* {{{ */
{
	zend_class_entry *ce = NULL;
	zval *zv;
	zend_string *lc_name;
	zend_string *autoload_name;

	if (key) {
		lc_name = key;
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

	zv = zend_hash_find(EG(class_table), lc_name);
	if (zv) {
		if (!key) {
			zend_string_release_ex(lc_name, 0);
		}
		ce = (zend_class_entry*)Z_PTR_P(zv);
		if (UNEXPECTED(!(ce->ce_flags & ZEND_ACC_LINKED))) {
			if ((flags & ZEND_FETCH_CLASS_ALLOW_UNLINKED) ||
				((flags & ZEND_FETCH_CLASS_ALLOW_NEARLY_LINKED) &&
					(ce->ce_flags & ZEND_ACC_NEARLY_LINKED))) {
				ce->ce_flags |= ZEND_ACC_HAS_UNLINKED_USES;
				return ce;
			}
			return NULL;
		}
		return ce;
	}

	/* The compiler is not-reentrant. Make sure we autoload only during run-time. */
	if ((flags & ZEND_FETCH_CLASS_NO_AUTOLOAD) || zend_is_compiling()) {
		if (!key) {
			zend_string_release_ex(lc_name, 0);
		}
		return NULL;
	}

	if (!zend_autoload) {
		if (!key) {
			zend_string_release_ex(lc_name, 0);
		}
		return NULL;
	}

	/* Verify class name before passing it to the autoloader. */
	if (!key && !zend_is_valid_class_name(name)) {
		zend_string_release_ex(lc_name, 0);
		return NULL;
	}

	if (EG(in_autoload) == NULL) {
		ALLOC_HASHTABLE(EG(in_autoload));
		zend_hash_init(EG(in_autoload), 8, NULL, NULL, 0);
	}

	if (zend_hash_add_empty_element(EG(in_autoload), lc_name) == NULL) {
		if (!key) {
			zend_string_release_ex(lc_name, 0);
		}
		return NULL;
	}

	if (ZSTR_VAL(name)[0] == '\\') {
		autoload_name = zend_string_init(ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 1, 0);
	} else {
		autoload_name = zend_string_copy(name);
	}

	zend_exception_save();
	ce = zend_autoload(autoload_name, lc_name);
	zend_exception_restore();

	zend_string_release_ex(autoload_name, 0);
	zend_hash_del(EG(in_autoload), lc_name);

	if (!key) {
		zend_string_release_ex(lc_name, 0);
	}
	return ce;
}
/* }}} */

ZEND_API zend_class_entry *zend_lookup_class(zend_string *name) /* {{{ */
{
	return zend_lookup_class_ex(name, NULL, 0);
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

ZEND_API zend_result zend_eval_stringl(const char *str, size_t str_len, zval *retval_ptr, const char *string_name) /* {{{ */
{
	zend_op_array *new_op_array;
	uint32_t original_compiler_options;
	zend_result retval;
	zend_string *code_str;

	if (retval_ptr) {
		code_str = zend_string_concat3(
			"return ", sizeof("return ")-1, str, str_len, ";", sizeof(";")-1);
	} else {
		code_str = zend_string_init(str, str_len, 0);
	}

	/*printf("Evaluating '%s'\n", pv.value.str.val);*/

	original_compiler_options = CG(compiler_options);
	CG(compiler_options) = ZEND_COMPILE_DEFAULT_FOR_EVAL;
	new_op_array = zend_compile_string(code_str, string_name);
	CG(compiler_options) = original_compiler_options;

	if (new_op_array) {
		zval local_retval;

		EG(no_extensions)=1;

		new_op_array->scope = zend_get_executed_scope();

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
	zend_string_release(code_str);
	return retval;
}
/* }}} */

ZEND_API zend_result zend_eval_string(const char *str, zval *retval_ptr, const char *string_name) /* {{{ */
{
	return zend_eval_stringl(str, strlen(str), retval_ptr, string_name);
}
/* }}} */

ZEND_API zend_result zend_eval_stringl_ex(const char *str, size_t str_len, zval *retval_ptr, const char *string_name, bool handle_exceptions) /* {{{ */
{
	zend_result result;

	result = zend_eval_stringl(str, str_len, retval_ptr, string_name);
	if (handle_exceptions && EG(exception)) {
		result = zend_exception_error(EG(exception), E_ERROR);
	}
	return result;
}
/* }}} */

ZEND_API zend_result zend_eval_string_ex(const char *str, zval *retval_ptr, const char *string_name, bool handle_exceptions) /* {{{ */
{
	return zend_eval_stringl_ex(str, strlen(str), retval_ptr, string_name, handle_exceptions);
}
/* }}} */

static void zend_set_timeout_ex(zend_long seconds, bool reset_signals);

ZEND_API ZEND_NORETURN void ZEND_FASTCALL zend_timeout(void) /* {{{ */
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

	zend_error_noreturn(E_ERROR, "Maximum execution time of " ZEND_LONG_FMT " second%s exceeded", EG(timeout_seconds), EG(timeout_seconds) == 1 ? "" : "s");
}
/* }}} */

#ifndef ZEND_WIN32
static void zend_timeout_handler(int dummy) /* {{{ */
{
#ifndef ZTS
    if (EG(timed_out)) {
		/* Die on hard timeout */
		const char *error_filename = NULL;
		uint32_t error_lineno = 0;
		char log_buffer[2048];
		int output_len = 0;

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

		output_len = snprintf(log_buffer, sizeof(log_buffer), "\nFatal error: Maximum execution time of " ZEND_LONG_FMT "+" ZEND_LONG_FMT " seconds exceeded (terminated) in %s on line %d\n", EG(timeout_seconds), EG(hard_timeout), error_filename, error_lineno);
		if (output_len > 0) {
			zend_quiet_write(2, log_buffer, MIN(output_len, sizeof(log_buffer)));
		}
		_exit(124);
    }
#endif

	if (zend_on_timeout) {
		zend_on_timeout(EG(timeout_seconds));
	}

	EG(timed_out) = 1;
	EG(vm_interrupt) = 1;

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
	zend_executor_globals *eg;

	/* The doc states it'll be always true, however it theoretically
		could be FALSE when the thread was signaled. */
	if (!timed_out) {
		return;
	}

	eg = (zend_executor_globals *)arg;
	eg->timed_out = 1;
	eg->vm_interrupt = 1;
}
#endif

/* This one doesn't exists on QNX */
#ifndef SIGPROF
#define SIGPROF 27
#endif

static void zend_set_timeout_ex(zend_long seconds, bool reset_signals) /* {{{ */
{
#ifdef ZEND_WIN32
	zend_executor_globals *eg;

	if (!seconds) {
		return;
	}

	/* Don't use ChangeTimerQueueTimer() as it will not restart an expired
	 * timer, so we could end up with just an ignored timeout. Instead
	 * delete and recreate. */
	if (NULL != tq_timer) {
		if (!DeleteTimerQueueTimer(NULL, tq_timer, INVALID_HANDLE_VALUE)) {
			tq_timer = NULL;
			zend_error_noreturn(E_ERROR, "Could not delete queued timer");
			return;
		}
		tq_timer = NULL;
	}

	/* XXX passing NULL means the default timer queue provided by the system is used */
	eg = ZEND_MODULE_GLOBALS_BULK(executor);
	if (!CreateTimerQueueTimer(&tq_timer, NULL, (WAITORTIMERCALLBACK)tq_timer_cb, (VOID*)eg, seconds*1000, 0, WT_EXECUTEONLYONCE)) {
		tq_timer = NULL;
		zend_error_noreturn(E_ERROR, "Could not queue new timer");
		return;
	}
#elif defined(HAVE_SETITIMER)
	{
		struct itimerval t_r;		/* timeout requested */
		int signo;

		if(seconds) {
			t_r.it_value.tv_sec = seconds;
			t_r.it_value.tv_usec = t_r.it_interval.tv_sec = t_r.it_interval.tv_usec = 0;

# if defined(__CYGWIN__) || defined(__PASE__)
			setitimer(ITIMER_REAL, &t_r, NULL);
		}
		signo = SIGALRM;
# else
			setitimer(ITIMER_PROF, &t_r, NULL);
		}
		signo = SIGPROF;
# endif

		if (reset_signals) {
# ifdef ZEND_SIGNALS
			zend_signal(signo, zend_timeout_handler);
# else
			sigset_t sigset;
#  ifdef HAVE_SIGACTION
			struct sigaction act;

			act.sa_handler = zend_timeout_handler;
			sigemptyset(&act.sa_mask);
			act.sa_flags = SA_RESETHAND | SA_NODEFER;
			sigaction(signo, &act, NULL);
#  else
			signal(signo, zend_timeout_handler);
#  endif /* HAVE_SIGACTION */
			sigemptyset(&sigset);
			sigaddset(&sigset, signo);
			sigprocmask(SIG_UNBLOCK, &sigset, NULL);
# endif /* ZEND_SIGNALS */
		}
	}
#endif /* HAVE_SETITIMER */
}
/* }}} */

void zend_set_timeout(zend_long seconds, bool reset_signals) /* {{{ */
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
		if (!DeleteTimerQueueTimer(NULL, tq_timer, INVALID_HANDLE_VALUE)) {
			EG(timed_out) = 0;
			tq_timer = NULL;
			zend_error_noreturn(E_ERROR, "Could not delete queued timer");
			return;
		}
		tq_timer = NULL;
	}
#elif defined(HAVE_SETITIMER)
	if (EG(timeout_seconds)) {
		struct itimerval no_timeout;

		no_timeout.it_value.tv_sec = no_timeout.it_value.tv_usec = no_timeout.it_interval.tv_sec = no_timeout.it_interval.tv_usec = 0;

# if defined(__CYGWIN__) || defined(__PASE__)
		setitimer(ITIMER_REAL, &no_timeout, NULL);
# else
		setitimer(ITIMER_PROF, &no_timeout, NULL);
# endif
	}
#endif
	EG(timed_out) = 0;
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
				zend_throw_or_error(fetch_type, NULL, "Cannot access \"self\" when no class scope is active");
			}
			return scope;
		case ZEND_FETCH_CLASS_PARENT:
			scope = zend_get_executed_scope();
			if (UNEXPECTED(!scope)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access \"parent\" when no class scope is active");
				return NULL;
			}
			if (UNEXPECTED(!scope->parent)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access \"parent\" when current class scope has no parent");
			}
			return scope->parent;
		case ZEND_FETCH_CLASS_STATIC:
			ce = zend_get_called_scope(EG(current_execute_data));
			if (UNEXPECTED(!ce)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access \"static\" when no class scope is active");
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
		return zend_lookup_class_ex(class_name, NULL, fetch_type);
	} else if ((ce = zend_lookup_class_ex(class_name, NULL, fetch_type)) == NULL) {
		if (!(fetch_type & ZEND_FETCH_CLASS_SILENT) && !EG(exception)) {
			if (fetch_sub_type == ZEND_FETCH_CLASS_INTERFACE) {
				zend_throw_or_error(fetch_type, NULL, "Interface \"%s\" not found", ZSTR_VAL(class_name));
			} else if (fetch_sub_type == ZEND_FETCH_CLASS_TRAIT) {
				zend_throw_or_error(fetch_type, NULL, "Trait \"%s\" not found", ZSTR_VAL(class_name));
			} else {
				zend_throw_or_error(fetch_type, NULL, "Class \"%s\" not found", ZSTR_VAL(class_name));
			}
		}
		return NULL;
	}
	return ce;
}
/* }}} */

zend_class_entry *zend_fetch_class_by_name(zend_string *class_name, zend_string *key, int fetch_type) /* {{{ */
{
	zend_class_entry *ce;

	if (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) {
		return zend_lookup_class_ex(class_name, key, fetch_type);
	} else if ((ce = zend_lookup_class_ex(class_name, key, fetch_type)) == NULL) {
		if (fetch_type & ZEND_FETCH_CLASS_SILENT) {
			return NULL;
		}
		if (EG(exception)) {
			if (!(fetch_type & ZEND_FETCH_CLASS_EXCEPTION)) {
				zend_string *exception_str;
				zval exception_zv;
				ZVAL_OBJ(&exception_zv, EG(exception));
				Z_ADDREF(exception_zv);
				zend_clear_exception();
				exception_str = zval_get_string(&exception_zv);
				zend_error_noreturn(E_ERROR,
					"During class fetch: Uncaught %s", ZSTR_VAL(exception_str));
			}
			return NULL;
		}
		if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_INTERFACE) {
			zend_throw_or_error(fetch_type, NULL, "Interface \"%s\" not found", ZSTR_VAL(class_name));
		} else if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_TRAIT) {
			zend_throw_or_error(fetch_type, NULL, "Trait \"%s\" not found", ZSTR_VAL(class_name));
		} else {
			zend_throw_or_error(fetch_type, NULL, "Class \"%s\" not found", ZSTR_VAL(class_name));
		}
		return NULL;
	}
	return ce;
}
/* }}} */

ZEND_API zend_result zend_delete_global_variable(zend_string *name) /* {{{ */
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
	if (EG(symtable_cache_ptr) > EG(symtable_cache)) {
		symbol_table = ex->symbol_table = *(--EG(symtable_cache_ptr));
		if (!ex->func->op_array.last_var) {
			return symbol_table;
		}
		zend_hash_extend(symbol_table, ex->func->op_array.last_var, 0);
	} else {
		symbol_table = ex->symbol_table = zend_new_array(ex->func->op_array.last_var);
		if (!ex->func->op_array.last_var) {
			return symbol_table;
		}
		zend_hash_real_init_mixed(symbol_table);
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
			zval *zv = zend_hash_find_ex(ht, *str, 1);

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

ZEND_API zend_result zend_set_local_var(zend_string *name, zval *value, bool force) /* {{{ */
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
					    zend_string_equal_content(*str, name)) {
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
					zend_hash_update(symbol_table, name, value);
					return SUCCESS;
				}
			}
		} else {
			zend_hash_update_ind(execute_data->symbol_table, name, value);
			return SUCCESS;
		}
	}
	return FAILURE;
}
/* }}} */

ZEND_API zend_result zend_set_local_var_str(const char *name, size_t len, zval *value, bool force) /* {{{ */
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
					zend_hash_str_update(symbol_table, name, len, value);
					return SUCCESS;
				}
			}
		} else {
			zend_hash_str_update_ind(execute_data->symbol_table, name, len, value);
			return SUCCESS;
		}
	}
	return FAILURE;
}
/* }}} */
