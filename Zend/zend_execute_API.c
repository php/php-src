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
#include "zend_fibers.h"
#include "zend_weakrefs.h"
#include "zend_inheritance.h"
#include "zend_observer.h"
#include "zend_call_stack.h"
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef ZEND_MAX_EXECUTION_TIMERS
#include <sys/syscall.h>
#endif

ZEND_API void (*zend_execute_ex)(zend_execute_data *execute_data);
ZEND_API void (*zend_execute_internal)(zend_execute_data *execute_data, zval *return_value);
ZEND_API zend_class_entry *(*zend_autoload)(zend_string *name, zend_string *lc_name);

/* true globals */
ZEND_API const zend_fcall_info empty_fcall_info = {0};
ZEND_API const zend_fcall_info_cache empty_fcall_info_cache = {0};

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
	ZEND_ATOMIC_BOOL_INIT(&EG(vm_interrupt), false);
	ZEND_ATOMIC_BOOL_INIT(&EG(timed_out), false);

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

	EG(record_errors) = false;
	EG(num_errors) = 0;
	EG(errors) = NULL;

	EG(filename_override) = NULL;
	EG(lineno_override) = -1;

	zend_max_execution_timer_init();
	zend_fiber_init();
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
		zend_error_noreturn(E_ERROR, "%s", message);
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

/* Free values held by the executor. */
ZEND_API void zend_shutdown_executor_values(bool fast_shutdown)
{
	zend_string *key;
	zval *zv;

	EG(flags) |= EG_FLAGS_IN_RESOURCE_SHUTDOWN;
	zend_try {
		zend_close_rsrc_list(&EG(regular_list));
	} zend_end_try();

	/* No PHP callback functions should be called after this point. */
	EG(active) = 0;

	if (!fast_shutdown) {
		zend_hash_graceful_reverse_destroy(&EG(symbol_table));

		/* Constants may contain objects, destroy them before the object store. */
		if (EG(full_tables_cleanup)) {
			zend_hash_reverse_apply(EG(zend_constants), clean_non_persistent_constant_full);
		} else {
			ZEND_HASH_MAP_REVERSE_FOREACH_STR_KEY_VAL(EG(zend_constants), key, zv) {
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
			} ZEND_HASH_MAP_FOREACH_END_DEL();
		}

		/* Release static properties and static variables prior to the final GC run,
		 * as they may hold GC roots. */
		ZEND_HASH_MAP_REVERSE_FOREACH_VAL(EG(function_table), zv) {
			zend_op_array *op_array = Z_PTR_P(zv);
			if (op_array->type == ZEND_INTERNAL_FUNCTION) {
				break;
			}
			if (ZEND_MAP_PTR(op_array->static_variables_ptr)) {
				HashTable *ht = ZEND_MAP_PTR_GET(op_array->static_variables_ptr);
				if (ht) {
					zend_array_destroy(ht);
					ZEND_MAP_PTR_SET(op_array->static_variables_ptr, NULL);
				}
			}
		} ZEND_HASH_FOREACH_END();
		ZEND_HASH_MAP_REVERSE_FOREACH_VAL(EG(class_table), zv) {
			zend_class_entry *ce = Z_PTR_P(zv);

			if (ce->default_static_members_count) {
				zend_cleanup_internal_class_data(ce);
			}

			if (ZEND_MAP_PTR(ce->mutable_data)) {
				if (ZEND_MAP_PTR_GET_IMM(ce->mutable_data)) {
					zend_cleanup_mutable_class_data(ce);
				}
			} else if (ce->type == ZEND_USER_CLASS && !(ce->ce_flags & ZEND_ACC_IMMUTABLE)) {
				/* Constants may contain objects, destroy the values before the object store. */
				zend_class_constant *c;
				ZEND_HASH_MAP_FOREACH_PTR(&ce->constants_table, c) {
					if (c->ce == ce) {
						zval_ptr_dtor_nogc(&c->value);
						ZVAL_UNDEF(&c->value);
					}
				} ZEND_HASH_FOREACH_END();

				/* properties may contain objects as well */
				if (ce->default_properties_table) {
					zval *p = ce->default_properties_table;
					zval *end = p + ce->default_properties_count;

					while (p != end) {
						i_zval_ptr_dtor(p);
						ZVAL_UNDEF(p);
						p++;
					}
				}
			}

			if (ce->type == ZEND_USER_CLASS && ce->backed_enum_table) {
				ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_IMMUTABLE));
				zend_hash_release(ce->backed_enum_table);
				ce->backed_enum_table = NULL;
			}

			if (ce->ce_flags & ZEND_HAS_STATIC_IN_METHODS) {
				zend_op_array *op_array;
				ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, op_array) {
					if (op_array->type == ZEND_USER_FUNCTION) {
						if (ZEND_MAP_PTR(op_array->static_variables_ptr)) {
							HashTable *ht = ZEND_MAP_PTR_GET(op_array->static_variables_ptr);
							if (ht) {
								zend_array_destroy(ht);
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
	} else {
		zend_hash_discard(EG(zend_constants), EG(persistent_constants_count));
	}

	zend_objects_store_free_object_storage(&EG(objects_store), fast_shutdown);
}

void shutdown_executor(void) /* {{{ */
{
	zend_string *key;
	zval *zv;
#if ZEND_DEBUG
	bool fast_shutdown = 0;
#else
	bool fast_shutdown = is_zend_mm() && !EG(full_tables_cleanup);
#endif

	zend_try {
		zend_stream_shutdown();
	} zend_end_try();

	zend_shutdown_executor_values(fast_shutdown);

	zend_weakrefs_shutdown();
	zend_max_execution_timer_shutdown();
	zend_fiber_shutdown();

	zend_try {
		zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_deactivator);
	} zend_end_try();

	if (fast_shutdown) {
		/* Fast Request Shutdown
		 * =====================
		 * Zend Memory Manager frees memory by its own. We don't have to free
		 * each allocated block separately.
		 */
		zend_hash_discard(EG(function_table), EG(persistent_functions_count));
		zend_hash_discard(EG(class_table), EG(persistent_classes_count));
	} else {
		zend_vm_stack_destroy();

		if (EG(full_tables_cleanup)) {
			zend_hash_reverse_apply(EG(function_table), clean_non_persistent_function_full);
			zend_hash_reverse_apply(EG(class_table), clean_non_persistent_class_full);
		} else {
			ZEND_HASH_MAP_REVERSE_FOREACH_STR_KEY_VAL(EG(function_table), key, zv) {
				zend_function *func = Z_PTR_P(zv);
				if (_idx == EG(persistent_functions_count)) {
					break;
				}
				destroy_op_array(&func->op_array);
				zend_string_release_ex(key, 0);
			} ZEND_HASH_MAP_FOREACH_END_DEL();

			ZEND_HASH_MAP_REVERSE_FOREACH_STR_KEY_VAL(EG(class_table), key, zv) {
				if (_idx == EG(persistent_classes_count)) {
					break;
				}
				destroy_zend_class(zv);
				zend_string_release_ex(key, 0);
			} ZEND_HASH_MAP_FOREACH_END_DEL();
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

	return func->common.function_name ? zend_string_copy(func->common.function_name) : ZSTR_INIT_LITERAL("main", 0);
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
	if (!func || arg_num == 0 || func->common.num_args < arg_num) {
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
	zend_string *filename = zend_get_executed_filename_ex();
	return filename != NULL ? ZSTR_VAL(filename) : "[no active file]";
}
/* }}} */

ZEND_API zend_string *zend_get_executed_filename_ex(void) /* {{{ */
{
	zend_string *filename_override = EG(filename_override);
	if (filename_override != NULL) {
		return filename_override;
	}

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
	zend_long lineno_override = EG(lineno_override);
	if (lineno_override != -1) {
		return lineno_override;
	}

	zend_execute_data *ex = EG(current_execute_data);

	while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->type))) {
		ex = ex->prev_execute_data;
	}
	if (ex) {
		if (!ex->opline) {
			/* Missing SAVE_OPLINE()? Falling back to first line of function */
			return ex->func->op_array.opcodes[0].lineno;
		}
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

ZEND_API bool zend_is_executing(void) /* {{{ */
{
	return EG(current_execute_data) != 0;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL zval_update_constant_with_ctx(zval *p, zend_class_entry *scope, zend_ast_evaluate_ctx *ctx)
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
			bool short_circuited;

			// Increase the refcount during zend_ast_evaluate to avoid releasing the ast too early
			// on nested calls to zval_update_constant_ex which can happen when retriggering ast
			// evaluation during autoloading.
			zend_ast_ref *ast_ref = Z_AST_P(p);
			bool ast_is_refcounted = !(GC_FLAGS(ast_ref) & GC_IMMUTABLE);
			if (ast_is_refcounted) {
				GC_ADDREF(ast_ref);
			}
			zend_result result = zend_ast_evaluate_ex(&tmp, ast, scope, &short_circuited, ctx) != SUCCESS;
			if (ast_is_refcounted && !GC_DELREF(ast_ref)) {
				rc_dtor_func((zend_refcounted *)ast_ref);
			}
			if (UNEXPECTED(result != SUCCESS)) {
				return FAILURE;
			}
			zval_ptr_dtor_nogc(p);
			ZVAL_COPY_VALUE(p, &tmp);
		}
	}
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL zval_update_constant_ex(zval *p, zend_class_entry *scope)
{
	zend_ast_evaluate_ctx ctx = {0};
	return zval_update_constant_with_ctx(p, scope, &ctx);
}

ZEND_API zend_result ZEND_FASTCALL zval_update_constant(zval *pp) /* {{{ */
{
	return zval_update_constant_ex(pp, EG(current_execute_data) ? zend_get_executed_scope() : CG(active_class_entry));
}
/* }}} */

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

ZEND_API bool zend_is_valid_class_name(zend_string *name) {
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
	uint32_t ce_cache = 0;

	if (ZSTR_HAS_CE_CACHE(name) && ZSTR_VALID_CE_CACHE(name)) {
		ce_cache = GC_REFCOUNT(name);
		ce = GET_CE_CACHE(ce_cache);
		if (EXPECTED(ce)) {
			return ce;
		}
	}

	if (key) {
		lc_name = key;
	} else {
		if (!ZSTR_LEN(name)) {
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
				if (!CG(unlinked_uses)) {
					ALLOC_HASHTABLE(CG(unlinked_uses));
					zend_hash_init(CG(unlinked_uses), 0, NULL, NULL, 0);
				}
				zend_hash_index_add_empty_element(CG(unlinked_uses), (zend_long)(uintptr_t)ce);
				return ce;
			}
			return NULL;
		}
		/* Don't populate CE_CACHE for mutable classes during compilation.
		 * The class may be freed while persisting. */
		if (ce_cache &&
				(!CG(in_compilation) || (ce->ce_flags & ZEND_ACC_IMMUTABLE))) {
			SET_CE_CACHE(ce_cache, ce);
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
	if (!key && !ZSTR_HAS_CE_CACHE(name) && !zend_is_valid_class_name(name)) {
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
	if (ce) {
		ZEND_ASSERT(!CG(in_compilation));
		if (ce_cache) {
			SET_CE_CACHE(ce_cache, ce);
		}
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
	new_op_array = zend_compile_string(code_str, string_name, ZEND_COMPILE_POSITION_AFTER_OPEN_TAG);
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
		zend_destroy_static_vars(new_op_array);
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
		zend_atomic_bool_store_ex(&EG(timed_out), false);
		zend_set_timeout_ex(EG(hard_timeout), 1);
		/* XXX Abused, introduce an additional flag if the value needs to be kept. */
		EG(hard_timeout) = 0;
	}
# endif
#else
	zend_atomic_bool_store_ex(&EG(timed_out), false);
	zend_set_timeout_ex(0, 1);
#endif

	zend_error_noreturn(E_ERROR, "Maximum execution time of " ZEND_LONG_FMT " second%s exceeded", EG(timeout_seconds), EG(timeout_seconds) == 1 ? "" : "s");
}
/* }}} */

#ifndef ZEND_WIN32
# ifdef ZEND_MAX_EXECUTION_TIMERS
static void zend_timeout_handler(int dummy, siginfo_t *si, void *uc) /* {{{ */
{
#ifdef ZTS
	if (!tsrm_is_managed_thread()) {
		fprintf(stderr, "zend_timeout_handler() called in a thread not managed by PHP. The expected signal handler will not be called. This is probably a bug.\n");

		return;
	}
#endif

	if (si->si_value.sival_ptr != &EG(max_execution_timer_timer)) {
#ifdef MAX_EXECUTION_TIMERS_DEBUG
		fprintf(stderr, "Executing previous handler (if set) for unexpected signal SIGRTMIN received on thread %d\n", (pid_t) syscall(SYS_gettid));
#endif

		if (EG(oldact).sa_sigaction) {
			EG(oldact).sa_sigaction(dummy, si, uc);

			return;
		}
		if (EG(oldact).sa_handler) EG(oldact).sa_handler(dummy);

		return;
	}
# else
static void zend_timeout_handler(int dummy) /* {{{ */
{
# endif
#ifdef ZTS
	if (!tsrm_is_managed_thread()) {
		fprintf(stderr, "zend_timeout_handler() called in a thread not managed by PHP. The expected signal handler will not be called. This is probably a bug.\n");

		return;
	}
#else
	if (zend_atomic_bool_load_ex(&EG(timed_out))) {
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

	zend_atomic_bool_store_ex(&EG(timed_out), true);
	zend_atomic_bool_store_ex(&EG(vm_interrupt), true);

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
	zend_atomic_bool_store_ex(&eg->timed_out, true);
	zend_atomic_bool_store_ex(&eg->vm_interrupt, true);
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
#elif defined(ZEND_MAX_EXECUTION_TIMERS)
	zend_max_execution_timer_settime(seconds);

	if (reset_signals) {
		sigset_t sigset;
		struct sigaction act;

		act.sa_sigaction = zend_timeout_handler;
		sigemptyset(&act.sa_mask);
		act.sa_flags = SA_ONSTACK | SA_SIGINFO;
		sigaction(SIGRTMIN, &act, NULL);
		sigemptyset(&sigset);
		sigaddset(&sigset, SIGRTMIN);
		sigprocmask(SIG_UNBLOCK, &sigset, NULL);
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
			act.sa_flags = SA_ONSTACK | SA_RESETHAND | SA_NODEFER;
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
	zend_atomic_bool_store_ex(&EG(timed_out), false);
}
/* }}} */

void zend_unset_timeout(void) /* {{{ */
{
#ifdef ZEND_WIN32
	if (NULL != tq_timer) {
		if (!DeleteTimerQueueTimer(NULL, tq_timer, INVALID_HANDLE_VALUE)) {
			zend_atomic_bool_store_ex(&EG(timed_out), false);
			tq_timer = NULL;
			zend_error_noreturn(E_ERROR, "Could not delete queued timer");
			return;
		}
		tq_timer = NULL;
	}
#elif ZEND_MAX_EXECUTION_TIMERS
	zend_max_execution_timer_settime(0);
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
	zend_atomic_bool_store_ex(&EG(timed_out), false);
}
/* }}} */

static ZEND_COLD void report_class_fetch_error(zend_string *class_name, uint32_t fetch_type)
{
	if (fetch_type & ZEND_FETCH_CLASS_SILENT) {
		return;
	}

	if (EG(exception)) {
		if (!(fetch_type & ZEND_FETCH_CLASS_EXCEPTION)) {
			zend_exception_uncaught_error("During class fetch");
		}
		return;
	}

	if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_INTERFACE) {
		zend_throw_or_error(fetch_type, NULL, "Interface \"%s\" not found", ZSTR_VAL(class_name));
	} else if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_TRAIT) {
		zend_throw_or_error(fetch_type, NULL, "Trait \"%s\" not found", ZSTR_VAL(class_name));
	} else {
		zend_throw_or_error(fetch_type, NULL, "Class \"%s\" not found", ZSTR_VAL(class_name));
	}
}

zend_class_entry *zend_fetch_class(zend_string *class_name, uint32_t fetch_type) /* {{{ */
{
	zend_class_entry *ce, *scope;
	uint32_t fetch_sub_type = fetch_type & ZEND_FETCH_CLASS_MASK;

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

	ce = zend_lookup_class_ex(class_name, NULL, fetch_type);
	if (!ce) {
		report_class_fetch_error(class_name, fetch_type);
		return NULL;
	}
	return ce;
}
/* }}} */

zend_class_entry *zend_fetch_class_with_scope(
		zend_string *class_name, uint32_t fetch_type, zend_class_entry *scope)
{
	zend_class_entry *ce;
	switch (fetch_type & ZEND_FETCH_CLASS_MASK) {
		case ZEND_FETCH_CLASS_SELF:
			if (UNEXPECTED(!scope)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access \"self\" when no class scope is active");
			}
			return scope;
		case ZEND_FETCH_CLASS_PARENT:
			if (UNEXPECTED(!scope)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access \"parent\" when no class scope is active");
				return NULL;
			}
			if (UNEXPECTED(!scope->parent)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access \"parent\" when current class scope has no parent");
			}
			return scope->parent;
		case 0:
			break;
		/* Other fetch types are not supported by this function. */
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	ce = zend_lookup_class_ex(class_name, NULL, fetch_type);
	if (!ce) {
		report_class_fetch_error(class_name, fetch_type);
		return NULL;
	}
	return ce;
}

zend_class_entry *zend_fetch_class_by_name(zend_string *class_name, zend_string *key, uint32_t fetch_type) /* {{{ */
{
	zend_class_entry *ce = zend_lookup_class_ex(class_name, key, fetch_type);
	if (!ce) {
		report_class_fetch_error(class_name, fetch_type);
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
			zval *zv = zend_hash_find_known_hash(ht, *str);

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
					    zend_string_equals_cstr(*str, name, len)) {
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
