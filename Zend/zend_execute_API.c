/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_vm.h"
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

ZEND_API void (*zend_execute)(zend_op_array *op_array TSRMLS_DC);
ZEND_API void (*zend_execute_internal)(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC);

/* true globals */
ZEND_API zend_fcall_info_cache empty_fcall_info_cache = { 0, NULL, NULL, NULL };

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
static void zend_handle_sigsegv(int dummy)
{
	fflush(stdout);
	fflush(stderr);
	if (original_sigsegv_handler==zend_handle_sigsegv) {
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
	}
	if (original_sigsegv_handler!=zend_handle_sigsegv) {
		original_sigsegv_handler(dummy);
	}
}
#endif


static void zend_extension_activator(zend_extension *extension TSRMLS_DC)
{
	if (extension->activate) {
		extension->activate();
	}
}


static void zend_extension_deactivator(zend_extension *extension TSRMLS_DC)
{
	if (extension->deactivate) {
		extension->deactivate();
	}
}


static int clean_non_persistent_function(zend_function *function TSRMLS_DC)
{
	return (function->type == ZEND_INTERNAL_FUNCTION) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}


static int clean_non_persistent_function_full(zend_function *function TSRMLS_DC)
{
	return (function->type != ZEND_INTERNAL_FUNCTION);
}


static int clean_non_persistent_class(zend_class_entry **ce TSRMLS_DC)
{
	return ((*ce)->type == ZEND_INTERNAL_CLASS) ? ZEND_HASH_APPLY_STOP : ZEND_HASH_APPLY_REMOVE;
}


static int clean_non_persistent_class_full(zend_class_entry **ce TSRMLS_DC)
{
	return ((*ce)->type != ZEND_INTERNAL_CLASS);
}


void init_executor(TSRMLS_D)
{
	INIT_ZVAL(EG(uninitialized_zval));
	/* trick to make uninitialized_zval never be modified, passed by ref, etc.  */
	EG(uninitialized_zval).refcount++;
	INIT_ZVAL(EG(error_zval));
	EG(uninitialized_zval_ptr)=&EG(uninitialized_zval);
	EG(error_zval_ptr)=&EG(error_zval);
	zend_ptr_stack_init(&EG(arg_types_stack));
/* destroys stack frame, therefore makes core dumps worthless */
#if 0&&ZEND_DEBUG
	original_sigsegv_handler = signal(SIGSEGV, zend_handle_sigsegv);
#endif
	EG(return_value_ptr_ptr) = NULL;

	EG(symtable_cache_ptr) = EG(symtable_cache)-1;
	EG(symtable_cache_limit)=EG(symtable_cache)+SYMTABLE_CACHE_SIZE-1;
	EG(no_extensions)=0;

	EG(function_table) = CG(function_table);
	EG(class_table) = CG(class_table);

	EG(in_execution) = 0;
	EG(in_autoload) = NULL;
	EG(autoload_func) = NULL;

	zend_ptr_stack_init(&EG(argument_stack));
	zend_ptr_stack_push(&EG(argument_stack), (void *) NULL);

	zend_hash_init(&EG(symbol_table), 50, NULL, ZVAL_PTR_DTOR, 0);
	{
		zval *globals;

		ALLOC_ZVAL(globals);
		globals->refcount=1;
		globals->is_ref=1;
		Z_TYPE_P(globals) = IS_ARRAY;
		Z_ARRVAL_P(globals) = &EG(symbol_table);
		zend_hash_update(&EG(symbol_table), "GLOBALS", sizeof("GLOBALS"), &globals, sizeof(zval *), NULL);
	}
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

	EG(scope) = NULL;

	EG(This) = NULL;
	
	EG(active_op_array) = NULL;

	EG(active) = 1;
}

static int zval_call_destructor(zval **zv TSRMLS_DC)
{
	if (Z_TYPE_PP(zv) == IS_OBJECT && (*zv)->refcount == 1) {
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}

void shutdown_destructors(TSRMLS_D) {
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

void shutdown_executor(TSRMLS_D)
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
		   so that if handler used some class, crash would not happen */
		if (EG(user_error_handler)) {
			zeh = EG(user_error_handler);
			EG(user_error_handler) = NULL;
			zval_dtor(zeh);
			FREE_ZVAL(zeh);
		}

		if (EG(user_exception_handler)) {
			zeh = EG(user_exception_handler);
			EG(user_exception_handler) = NULL;
			zval_dtor(zeh);
			FREE_ZVAL(zeh);
		}

		zend_stack_destroy(&EG(user_error_handlers_error_reporting));
		zend_stack_init(&EG(user_error_handlers_error_reporting));
		zend_ptr_stack_clean(&EG(user_error_handlers), ZVAL_DESTRUCTOR, 1);
		zend_ptr_stack_clean(&EG(user_exception_handlers), ZVAL_DESTRUCTOR, 1);
	} zend_end_try();

	zend_try {
		/* Cleanup static data for functions and arrays.
		   We need a separate cleanup stage because of the following problem:
		   Suppose we destroy class X, which destroys the class's function table,
		   and in the function table we have function foo() that has static $bar.
		   Now if an object of class X is assigned to $bar, its destructor will be
		   called and will fail since X's function table is in mid-destruction.
		   So we want first of all to clean up all data and then move to tables destruction.
		   Note that only run-time accessed data need to be cleaned up, pre-defined data can
		   not contain objects and thus are not probelmatic */
		if (EG(full_tables_cleanup)) {
			zend_hash_apply(EG(function_table), (apply_func_t) zend_cleanup_function_data_full TSRMLS_CC);
		} else {
			zend_hash_reverse_apply(EG(function_table), (apply_func_t) zend_cleanup_function_data TSRMLS_CC);
		}
		zend_hash_apply(EG(class_table), (apply_func_t) zend_cleanup_class_data TSRMLS_CC);

		zend_ptr_stack_destroy(&EG(argument_stack));

		/* Destroy all op arrays */
		if (EG(full_tables_cleanup)) {
			zend_hash_apply(EG(function_table), (apply_func_t) clean_non_persistent_function_full TSRMLS_CC);
			zend_hash_apply(EG(class_table), (apply_func_t) clean_non_persistent_class_full TSRMLS_CC);
		} else {
			zend_hash_reverse_apply(EG(function_table), (apply_func_t) clean_non_persistent_function TSRMLS_CC);
			zend_hash_reverse_apply(EG(class_table), (apply_func_t) clean_non_persistent_class TSRMLS_CC);
		}

		while (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
			zend_hash_destroy(*EG(symtable_cache_ptr));
			FREE_HASHTABLE(*EG(symtable_cache_ptr));
			EG(symtable_cache_ptr)--;
		}
		zend_objects_store_free_object_storage(&EG(objects_store) TSRMLS_CC);
	} zend_end_try();

	zend_try {
		clean_non_persistent_constants(TSRMLS_C);
	} zend_end_try();

	zend_try {
#if 0&&ZEND_DEBUG
	signal(SIGSEGV, original_sigsegv_handler);
#endif

		zend_hash_destroy(&EG(included_files));

		zend_ptr_stack_destroy(&EG(arg_types_stack));
		zend_stack_destroy(&EG(user_error_handlers_error_reporting));
		zend_ptr_stack_destroy(&EG(user_error_handlers));
		zend_ptr_stack_destroy(&EG(user_exception_handlers));
		zend_objects_store_destroy(&EG(objects_store));
		if (EG(in_autoload)) {
			zend_hash_destroy(EG(in_autoload));
			FREE_HASHTABLE(EG(in_autoload));
		}
	} zend_end_try();
	EG(active) = 0;
}


/* return class name and "::" or "". */
ZEND_API char *get_active_class_name(char **space TSRMLS_DC)
{
	if (!zend_is_executing(TSRMLS_C)) {
		if (space) {
			*space = "";
		}
		return "";
	}
	switch (EG(function_state_ptr)->function->type) {
		case ZEND_USER_FUNCTION:
		case ZEND_INTERNAL_FUNCTION:
		{
			zend_class_entry *ce = EG(function_state_ptr)->function->common.scope;

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


ZEND_API char *get_active_function_name(TSRMLS_D)
{
	if (!zend_is_executing(TSRMLS_C)) {
		return NULL;
	}
	switch (EG(function_state_ptr)->function->type) {
		case ZEND_USER_FUNCTION: {
				char *function_name = ((zend_op_array *) EG(function_state_ptr)->function)->function_name;

				if (function_name) {
					return function_name;
				} else {
					return "main";
				}
			}
			break;
		case ZEND_INTERNAL_FUNCTION:
			return ((zend_internal_function *) EG(function_state_ptr)->function)->function_name;
			break;
		default:
			return NULL;
	}
}


ZEND_API char *zend_get_executed_filename(TSRMLS_D)
{
	if (EG(active_op_array)) {
		return EG(active_op_array)->filename;
	} else {
		return "[no active file]";
	}
}


ZEND_API uint zend_get_executed_lineno(TSRMLS_D)
{
	if (EG(opline_ptr)) {
		return active_opline->lineno;
	} else {
		return 0;
	}
}


ZEND_API zend_bool zend_is_executing(TSRMLS_D)
{
	return EG(in_execution);
}


ZEND_API void _zval_ptr_dtor(zval **zval_ptr ZEND_FILE_LINE_DC)
{
#if DEBUG_ZEND>=2
	printf("Reducing refcount for %x (%x):  %d->%d\n", *zval_ptr, zval_ptr, (*zval_ptr)->refcount, (*zval_ptr)->refcount-1);
#endif
	(*zval_ptr)->refcount--;
	if ((*zval_ptr)->refcount==0) {
		zval_dtor(*zval_ptr);
		safe_free_zval_ptr_rel(*zval_ptr ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC);
	} else if ((*zval_ptr)->refcount == 1) {
		if ((*zval_ptr)->type == IS_OBJECT) {
			TSRMLS_FETCH();

			if (EG(ze1_compatibility_mode)) {
				return;
			}
		}
		(*zval_ptr)->is_ref = 0;
	}
}


ZEND_API void _zval_internal_ptr_dtor(zval **zval_ptr ZEND_FILE_LINE_DC)
{
#if DEBUG_ZEND>=2
	printf("Reducing refcount for %x (%x):  %d->%d\n", *zval_ptr, zval_ptr, (*zval_ptr)->refcount, (*zval_ptr)->refcount-1);
#endif
	(*zval_ptr)->refcount--;
	if ((*zval_ptr)->refcount==0) {
		zval_internal_dtor(*zval_ptr);
		free(*zval_ptr);
	} else if ((*zval_ptr)->refcount == 1) {
		(*zval_ptr)->is_ref = 0;
	}
}


ZEND_API int zend_is_true(zval *op)
{
	return i_zend_is_true(op);
}

#include "../TSRM/tsrm_strtok_r.h"

#define IS_VISITED_CONSTANT       IS_CONSTANT_INDEX
#define IS_CONSTANT_VISITED(p)    (Z_TYPE_P(p) & IS_VISITED_CONSTANT)
#define MARK_CONSTANT_VISITED(p)  Z_TYPE_P(p) |= IS_VISITED_CONSTANT

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

ZEND_API int zval_update_constant_ex(zval **pp, void *arg, zend_class_entry *scope TSRMLS_DC)
{
	zval *p = *pp;
	zend_bool inline_change = (zend_bool) (zend_uintptr_t) arg;
	zval const_value;
	char *colon;

	if (IS_CONSTANT_VISITED(p)) {
		zend_error(E_ERROR, "Cannot declare self-referencing constant '%s'", Z_STRVAL_P(p));
	} else if (Z_TYPE_P(p) == IS_CONSTANT) {
		int refcount;
		zend_uchar is_ref;

		SEPARATE_ZVAL_IF_NOT_REF(pp);
		p = *pp;

		MARK_CONSTANT_VISITED(p);

		refcount = p->refcount;
		is_ref = p->is_ref;

		if (!zend_get_constant_ex(p->value.str.val, p->value.str.len, &const_value, scope TSRMLS_CC)) {
			if ((colon = memchr(Z_STRVAL_P(p), ':', Z_STRLEN_P(p))) && colon[1] == ':') {
				zend_error(E_ERROR, "Undefined class constant '%s'", Z_STRVAL_P(p));
			}
			zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",
					   p->value.str.val,
					   p->value.str.val);
			p->type = IS_STRING;
			if (!inline_change) {
				zval_copy_ctor(p);
			}
		} else {
			if (inline_change) {
				STR_FREE(p->value.str.val);
			}
			*p = const_value;
		}

		p->refcount = refcount;
		p->is_ref = is_ref;
	} else if (Z_TYPE_P(p) == IS_CONSTANT_ARRAY) {
		zval **element, *new_val;
		char *str_index;
		uint str_index_len;
		ulong num_index;

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
		while (zend_hash_get_current_data(Z_ARRVAL_P(p), (void **) &element)==SUCCESS) {
			if (!(Z_TYPE_PP(element) & IS_CONSTANT_INDEX)) {
				zend_hash_move_forward(Z_ARRVAL_P(p));
				continue;
			}
			Z_TYPE_PP(element) &= ~IS_CONSTANT_INDEX;
			if (zend_hash_get_current_key_ex(Z_ARRVAL_P(p), &str_index, &str_index_len, &num_index, 0, NULL)!=HASH_KEY_IS_STRING) {
				zend_hash_move_forward(Z_ARRVAL_P(p));
				continue;
			}
			if (!zend_get_constant_ex(str_index, str_index_len-1, &const_value, scope TSRMLS_CC)) {
				if ((colon = memchr(str_index, ':', str_index_len-1)) && colon[1] == ':') {
					zend_error(E_ERROR, "Undefined class constant '%s'", str_index);
				}
				zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",	str_index, str_index);
				zend_hash_move_forward(Z_ARRVAL_P(p));
				continue;
			}

			if (const_value.type == IS_STRING && const_value.value.str.len == (int)str_index_len-1 &&
			   !strncmp(const_value.value.str.val, str_index, str_index_len)) {
				/* constant value is the same as its name */
				zval_dtor(&const_value);
				zend_hash_move_forward(p->value.ht);
				continue;
			}

			ALLOC_ZVAL(new_val);
			*new_val = **element;
			zval_copy_ctor(new_val);
			new_val->refcount = 1;
			new_val->is_ref = 0;

			/* preserve this bit for inheritance */
			Z_TYPE_PP(element) |= IS_CONSTANT_INDEX;
			zval_ptr_dtor(element);
			*element = new_val;

			switch (Z_TYPE(const_value)) {
				case IS_STRING:
					zend_symtable_update_current_key(Z_ARRVAL_P(p), const_value.value.str.val, const_value.value.str.len+1);
					break;
				case IS_BOOL:
				case IS_LONG:
					zend_hash_update_current_key(Z_ARRVAL_P(p), HASH_KEY_IS_LONG, NULL, 0, Z_LVAL(const_value));
					break;
				case IS_DOUBLE:
					zend_hash_update_current_key(Z_ARRVAL_P(p), HASH_KEY_IS_LONG, NULL, 0, (long)Z_DVAL(const_value));
					break;
				case IS_NULL:
					zend_hash_update_current_key(Z_ARRVAL_P(p), HASH_KEY_IS_STRING, "", 1, 0);
					break;
			}
			zend_hash_move_forward(Z_ARRVAL_P(p));
			zval_dtor(&const_value);
		}
		zend_hash_apply_with_argument(Z_ARRVAL_P(p), (apply_func_arg_t) zval_update_constant, (void *) 1 TSRMLS_CC);
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(p));
	}
	return 0;
}

ZEND_API int zval_update_constant(zval **pp, void *arg TSRMLS_DC)
{
	return zval_update_constant_ex(pp, arg, NULL TSRMLS_CC);
}

int call_user_function(HashTable *function_table, zval **object_pp, zval *function_name, zval *retval_ptr, zend_uint param_count, zval *params[] TSRMLS_DC)
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


int call_user_function_ex(HashTable *function_table, zval **object_pp, zval *function_name, zval **retval_ptr_ptr, zend_uint param_count, zval **params[], int no_separation, HashTable *symbol_table TSRMLS_DC)
{
	zend_fcall_info fci;

	fci.size = sizeof(fci);
	fci.function_table = function_table;
	fci.object_pp = object_pp;
	fci.function_name = function_name;
	fci.retval_ptr_ptr = retval_ptr_ptr;
	fci.param_count = param_count;
	fci.params = params;
	fci.no_separation = (zend_bool) no_separation;
	fci.symbol_table = symbol_table;

	return zend_call_function(&fci, NULL TSRMLS_CC);
}


int zend_call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache TSRMLS_DC)
{
	zend_uint i;
	zval **original_return_value;
	HashTable *calling_symbol_table;
	zend_function_state *original_function_state_ptr;
	zend_op_array *original_op_array;
	zend_op **original_opline_ptr;
	zend_class_entry *current_scope;
	zend_class_entry *calling_scope = NULL;
	zend_class_entry *check_scope_or_static = NULL;
	zval *current_this;
	zend_execute_data execute_data;
	zval *method_name;
	zval *params_array;
	int call_via_handler = 0;
	char *fname, *colon;
	int fname_len;

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
		EX(object) =  NULL;
	} else {
		/* This only happens when we're called outside any execute()'s
		 * It shouldn't be strictly necessary to NULL execute_data out,
		 * but it may make bugs easier to spot
		 */
		memset(&execute_data, 0, sizeof(zend_execute_data));
	}

	if (!fci_cache || !fci_cache->initialized) {
		if (Z_TYPE_P(fci->function_name)==IS_ARRAY) { /* assume array($obj, $name) couple */
			zval **tmp_object_ptr, **tmp_real_function_name;

			if (zend_hash_index_find(Z_ARRVAL_P(fci->function_name), 0, (void **) &tmp_object_ptr)==FAILURE) {
				return FAILURE;
			}
			if (zend_hash_index_find(Z_ARRVAL_P(fci->function_name), 1, (void **) &tmp_real_function_name)==FAILURE) {
				return FAILURE;
			}
			fci->function_name = *tmp_real_function_name;
			SEPARATE_ZVAL_IF_NOT_REF(tmp_object_ptr);
			fci->object_pp = tmp_object_ptr;
			(*fci->object_pp)->is_ref = 1;
		}

		if (fci->object_pp && !*fci->object_pp) {
			fci->object_pp = NULL;
		}

		if (fci->object_pp) {
			if (Z_TYPE_PP(fci->object_pp) == IS_OBJECT
				&& (!EG(objects_store).object_buckets || !EG(objects_store).object_buckets[Z_OBJ_HANDLE_PP(fci->object_pp)].valid)) {
				return FAILURE;
			}
			/* TBI!! new object handlers */
			if (Z_TYPE_PP(fci->object_pp) == IS_OBJECT) {
				if (!IS_ZEND_STD_OBJECT(**fci->object_pp)) {
					zend_error(E_WARNING, "Cannot use call_user_function on objects without a class entry");
					return FAILURE;
				}

				calling_scope = Z_OBJCE_PP(fci->object_pp);
				fci->function_table = &calling_scope->function_table;
				EX(object) =  *fci->object_pp;
			} else if (Z_TYPE_PP(fci->object_pp) == IS_STRING) {
				zend_class_entry **ce;
				int found = FAILURE;

				if (EG(active_op_array) && strcmp(Z_STRVAL_PP(fci->object_pp), "self") == 0) {
					if (!EG(active_op_array)->scope) {
						zend_error(E_ERROR, "Cannot access self:: when no class scope is active");
					}
					ce = &(EG(active_op_array)->scope);
					found = (*ce != NULL?SUCCESS:FAILURE);
					fci->object_pp = EG(This)?&EG(This):NULL;
					EX(object) = EG(This);
				} else if (strcmp(Z_STRVAL_PP(fci->object_pp), "parent") == 0 && EG(active_op_array)) {

					if (!EG(active_op_array)->scope) {
						zend_error(E_ERROR, "Cannot access parent:: when no class scope is active");
					}
					if (!EG(active_op_array)->scope->parent) {
						zend_error(E_ERROR, "Cannot access parent:: when current class scope has no parent");
					}
					ce = &(EG(active_op_array)->scope->parent);
					found = (*ce != NULL?SUCCESS:FAILURE);
					fci->object_pp = EG(This)?&EG(This):NULL;
					EX(object) = EG(This);
				} else {
					zend_class_entry *scope;
					scope = EG(active_op_array) ? EG(active_op_array)->scope : NULL;

					found = zend_lookup_class(Z_STRVAL_PP(fci->object_pp), Z_STRLEN_PP(fci->object_pp), &ce TSRMLS_CC);
					if (found == FAILURE) {
						zend_error(E_ERROR, "Class '%s' not found", Z_STRVAL_PP(fci->object_pp));
					}
					if (scope && EG(This) &&
						instanceof_function(Z_OBJCE_P(EG(This)), scope TSRMLS_CC) &&
						instanceof_function(scope, *ce TSRMLS_CC)) {
						fci->object_pp = &EG(This);
						EX(object) = EG(This);
					} else {
						fci->object_pp = NULL;
					}
				}
				if (found == FAILURE)
					return FAILURE;

				fci->function_table = &(*ce)->function_table;
				calling_scope = *ce;
			} else {
				zend_error(E_NOTICE, "Non-callable array passed to zend_call_function()");
				return FAILURE;
			}

			if (fci->function_table == NULL) {
				return FAILURE;
			}
		}

		if (fci->function_name->type!=IS_STRING) {
			return FAILURE;
		}

		fname = Z_STRVAL_P(fci->function_name);
		fname_len = Z_STRLEN_P(fci->function_name);
		if ((colon = strstr(fname, "::")) != NULL) {
			int clen = colon - fname;
			int mlen = fname_len - clen - 2;
			zend_class_entry **pce, *ce_child = NULL;
			if (zend_lookup_class(fname, clen, &pce TSRMLS_CC) == SUCCESS) {
				ce_child = *pce;
			} else {
				char *lcname = zend_str_tolower_dup(fname, clen);
				/* caution: lcname is not '\0' terminated */
				if (calling_scope) {
					if (clen == sizeof("self") - 1 && memcmp(lcname, "self", sizeof("self") - 1) == 0) {
						ce_child = EG(active_op_array) ? EG(active_op_array)->scope : NULL;
					} else if (clen == sizeof("parent") - 1 && memcmp(lcname, "parent", sizeof("parent") - 1) == 0 && EG(active_op_array)->scope) {
						ce_child = EG(active_op_array) && EG(active_op_array)->scope ? EG(scope)->parent : NULL;
					}
				}
				efree(lcname);
			}
			if (!ce_child) {
				zend_error(E_ERROR, "Cannot call method %s() or method does not exist", fname);
				return FAILURE;
			}
			check_scope_or_static = calling_scope;
			fci->function_table = &ce_child->function_table;
			calling_scope = ce_child;
			fname = fname + clen + 2;
			fname_len = mlen;
		}

		if (fci->object_pp) {
			if (Z_OBJ_HT_PP(fci->object_pp)->get_method == NULL) {
				zend_error(E_ERROR, "Object does not support method calls");
			}
			EX(function_state).function = 
			  Z_OBJ_HT_PP(fci->object_pp)->get_method(fci->object_pp, fname, fname_len TSRMLS_CC);
			if (EX(function_state).function &&
			    (EX(function_state).function->common.fn_flags & ZEND_ACC_PRIVATE) == 0 &&
			    calling_scope != EX(function_state).function->common.scope) {
				char *function_name_lc = zend_str_tolower_dup(fname, fname_len);
				if (zend_hash_find(&calling_scope->function_table, function_name_lc, fname_len+1, (void **) &EX(function_state).function)==FAILURE) {
					efree(function_name_lc);
					zend_error(E_ERROR, "Cannot call method %s::%s() or method does not exist", calling_scope->name, fname);
				}
				efree(function_name_lc);
			}
		} else if (calling_scope) {
			char *function_name_lc = zend_str_tolower_dup(fname, fname_len);

			EX(function_state).function = 
				zend_std_get_static_method(calling_scope, function_name_lc, fname_len TSRMLS_CC);
			efree(function_name_lc);
			if (check_scope_or_static && EX(function_state).function
			&& !(EX(function_state).function->common.fn_flags & ZEND_ACC_STATIC)
			&& !instanceof_function(check_scope_or_static, calling_scope TSRMLS_CC)) {
				zend_error(E_ERROR, "Cannot call method %s() of class %s which is not a derived from %s", fname, calling_scope->name, check_scope_or_static->name);
				return FAILURE;
			}
		} else {
			char *function_name_lc = zend_str_tolower_dup(fname, fname_len);

			if (zend_hash_find(fci->function_table, function_name_lc, fname_len+1, (void **) &EX(function_state).function)==FAILURE) {
			  EX(function_state).function = NULL;
			}
			efree(function_name_lc);
		}

		if (EX(function_state).function == NULL) {			  
			/* try calling __call */
			if (calling_scope && calling_scope->__call) {
				EX(function_state).function = calling_scope->__call;
				/* prepare params */
				ALLOC_INIT_ZVAL(method_name);
				ZVAL_STRINGL(method_name, Z_STRVAL_P(fci->function_name), Z_STRLEN_P(fci->function_name), 0);

				ALLOC_INIT_ZVAL(params_array);
				array_init(params_array);
				call_via_handler = 1;
			} else {
				return FAILURE;
			}
		}
		if (fci_cache &&
		    (EX(function_state).function->type != ZEND_INTERNAL_FUNCTION ||
		     ((zend_internal_function*)EX(function_state).function)->handler != zend_std_call_user_call)) {
			fci_cache->function_handler = EX(function_state).function;
			fci_cache->object_pp = fci->object_pp;
			fci_cache->calling_scope = calling_scope;
			fci_cache->initialized = 1;
		}
	} else {
		EX(function_state).function = fci_cache->function_handler;
		calling_scope = fci_cache->calling_scope;
		fci->object_pp = fci_cache->object_pp;
		EX(object) = fci->object_pp ? *fci->object_pp : NULL;
		if (fci->object_pp && *fci->object_pp && Z_TYPE_PP(fci->object_pp) == IS_OBJECT
			&& (!EG(objects_store).object_buckets || !EG(objects_store).object_buckets[Z_OBJ_HANDLE_PP(fci->object_pp)].valid)) {
			return FAILURE;
		}
	}

	if (EX(function_state).function->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_DEPRECATED)) {
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_ABSTRACT) {
			zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name);
		}
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_DEPRECATED) {
			zend_error(E_STRICT, "Function %s%s%s() is deprecated",
				EX(function_state).function->common.scope ? EX(function_state).function->common.scope->name : "",
				EX(function_state).function->common.scope ? "::" : "",
				EX(function_state).function->common.function_name);
		}
	}

	/* Prevent crash because of stack reallocation */
	if (!call_via_handler &&
	    fci->param_count &&
	    EG(argument_stack).top + fci->param_count > EG(argument_stack).max &&
	    *(void***)fci->params >= EG(argument_stack).elements &&
	    *(void***)fci->params < EG(argument_stack).top_element) {

		/* Manual stack reallocation */
		void **prev_elements = EG(argument_stack).elements;
		void **prev_top_element = EG(argument_stack).top_element;

		ZEND_PTR_STACK_RESIZE_IF_NEEDED((&EG(argument_stack)), fci->param_count);
		for (i=0; i<fci->param_count; i++) {
			if ((void**)fci->params[i] >= prev_elements &&
			    (void**)fci->params[i] < prev_top_element) {
				fci->params[i] = (zval**)((void**)fci->params[i] - prev_elements + EG(argument_stack).elements);
			}
		}
	}

	for (i=0; i<fci->param_count; i++) {
		zval *param;

		if (ARG_SHOULD_BE_SENT_BY_REF(EX(function_state).function, i+1)
			&& !PZVAL_IS_REF(*fci->params[i])) {
			if ((*fci->params[i])->refcount>1) {
				zval *new_zval;

				if (fci->no_separation) {
					if(i) {
						/* hack to clean up the stack */
						zend_ptr_stack_n_push(&EG(argument_stack), 2, (void *) (zend_uintptr_t) i, NULL);
						zend_ptr_stack_clear_multiple(TSRMLS_C);
					}

					if (call_via_handler) {
						zval_ptr_dtor(&method_name);
						zval_ptr_dtor(&params_array);
					}
					return FAILURE;
				}
				ALLOC_ZVAL(new_zval);
				*new_zval = **fci->params[i];
				zval_copy_ctor(new_zval);
				new_zval->refcount = 1;
				(*fci->params[i])->refcount--;
				*fci->params[i] = new_zval;
			}
			(*fci->params[i])->refcount++;
			(*fci->params[i])->is_ref = 1;
			param = *fci->params[i];
		} else if (*fci->params[i] != &EG(uninitialized_zval)) {
			(*fci->params[i])->refcount++;
			param = *fci->params[i];
		} else {
			ALLOC_ZVAL(param);
			*param = **(fci->params[i]);
			INIT_PZVAL(param);
		}
		if (call_via_handler) {
			add_next_index_zval(params_array, param);
		} else {
			zend_ptr_stack_push(&EG(argument_stack), param);
		}
	}

	if (call_via_handler) {
		zend_ptr_stack_push(&EG(argument_stack), method_name);
		zend_ptr_stack_push(&EG(argument_stack), params_array);
		fci->param_count = 2;
	}

	zend_ptr_stack_2_push(&EG(argument_stack), (void *) (zend_uintptr_t) fci->param_count, NULL);

	original_function_state_ptr = EG(function_state_ptr);
	EG(function_state_ptr) = &EX(function_state);

	current_scope = EG(scope);
	EG(scope) = calling_scope;

	current_this = EG(This);

	if (fci->object_pp) {
		if ((EX(function_state).function->common.fn_flags & ZEND_ACC_STATIC)) {
			EG(This) = NULL;
		} else {
			EG(This) = *fci->object_pp;

			if (!PZVAL_IS_REF(EG(This))) {
				EG(This)->refcount++; /* For $this pointer */
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
		if (calling_scope && !(EX(function_state).function->common.fn_flags & ZEND_ACC_STATIC)) {
			int severity;
			if (EX(function_state).function->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
			} else {
				severity = E_ERROR;
			}
			zend_error(severity, "Non-static method %s::%s() cannot be called statically", calling_scope->name, EX(function_state).function->common.function_name);
		}
	}

	EX(prev_execute_data) = EG(current_execute_data);
	EG(current_execute_data) = &execute_data;

	if (EX(function_state).function->type == ZEND_USER_FUNCTION) {
		calling_symbol_table = EG(active_symbol_table);
		EG(scope) = EX(function_state).function->common.scope;
		if (fci->symbol_table) {
			EG(active_symbol_table) = fci->symbol_table;
		} else {
			ALLOC_HASHTABLE(EG(active_symbol_table));
			zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);
		}

		original_return_value = EG(return_value_ptr_ptr);
		original_op_array = EG(active_op_array);
		EG(return_value_ptr_ptr) = fci->retval_ptr_ptr;
		EG(active_op_array) = (zend_op_array *) EX(function_state).function;
		original_opline_ptr = EG(opline_ptr);
		zend_execute(EG(active_op_array) TSRMLS_CC);
		if (!fci->symbol_table) {
			zend_hash_destroy(EG(active_symbol_table));
			FREE_HASHTABLE(EG(active_symbol_table));
		}
		EG(active_symbol_table) = calling_symbol_table;
		EG(active_op_array) = original_op_array;
		EG(return_value_ptr_ptr)=original_return_value;
		EG(opline_ptr) = original_opline_ptr;
	} else {
		ALLOC_INIT_ZVAL(*fci->retval_ptr_ptr);
		if (EX(function_state).function->common.scope) {
			EG(scope) = EX(function_state).function->common.scope;
		}
		((zend_internal_function *) EX(function_state).function)->handler(fci->param_count, *fci->retval_ptr_ptr, EX(function_state).function->common.return_reference?fci->retval_ptr_ptr:NULL, (fci->object_pp?*fci->object_pp:NULL), 1 TSRMLS_CC);
		INIT_PZVAL(*fci->retval_ptr_ptr);
	}
	zend_ptr_stack_clear_multiple(TSRMLS_C);
	if (call_via_handler) {
		zval_ptr_dtor(&method_name);
		zval_ptr_dtor(&params_array);
	}
	EG(function_state_ptr) = original_function_state_ptr;

	if (EG(This)) {
		zval_ptr_dtor(&EG(This));
	}
	EG(scope) = current_scope;
	EG(This) = current_this;
	EG(current_execute_data) = EX(prev_execute_data);

	if (EG(exception)) {
		zend_throw_exception_internal(NULL TSRMLS_CC);
	}
	return SUCCESS;
}


ZEND_API int zend_lookup_class_ex(char *name, int name_length, int use_autoload, zend_class_entry ***ce TSRMLS_DC)
{
	zval **args[1];
	zval autoload_function;
	zval *class_name_ptr;
	zval *retval_ptr = NULL;
	int retval;
	char *lc_name;
	zval *exception;
	zend_fcall_info fcall_info;
	zend_fcall_info_cache fcall_cache;
	char dummy = 1;
	ALLOCA_FLAG(use_heap)

	if (name == NULL || !name_length) {
		return FAILURE;
	}
	
	lc_name = do_alloca_with_limit(name_length + 1, use_heap);
	zend_str_tolower_copy(lc_name, name, name_length);

	if (zend_hash_find(EG(class_table), lc_name, name_length+1, (void **) ce) == SUCCESS) {
		free_alloca_with_limit(lc_name, use_heap);
		return SUCCESS;
	}

	/* The compiler is not-reentrant. Make sure we __autoload() only during run-time
	 * (doesn't impact fuctionality of __autoload()
	*/
	if (!use_autoload || zend_is_compiling(TSRMLS_C)) {
		free_alloca_with_limit(lc_name, use_heap);
		return FAILURE;
	}

	if (EG(in_autoload) == NULL) {
		ALLOC_HASHTABLE(EG(in_autoload));
		zend_hash_init(EG(in_autoload), 0, NULL, NULL, 0);	
	}
	
	if (zend_hash_add(EG(in_autoload), lc_name, name_length+1, (void**)&dummy, sizeof(char), NULL) == FAILURE) {
		free_alloca_with_limit(lc_name, use_heap);
		return FAILURE;
	}

	ZVAL_STRINGL(&autoload_function, ZEND_AUTOLOAD_FUNC_NAME, sizeof(ZEND_AUTOLOAD_FUNC_NAME)-1,  0);

	ALLOC_ZVAL(class_name_ptr);
	INIT_PZVAL(class_name_ptr);
	ZVAL_STRINGL(class_name_ptr, name, name_length, 1);
	
	args[0] = &class_name_ptr;

	fcall_info.size = sizeof(fcall_info);
	fcall_info.function_table = EG(function_table);
	fcall_info.function_name = &autoload_function;
	fcall_info.symbol_table = NULL;
	fcall_info.retval_ptr_ptr = &retval_ptr;
	fcall_info.param_count = 1;
	fcall_info.params = args;
	fcall_info.object_pp = NULL;
	fcall_info.no_separation = 1;

	fcall_cache.initialized = EG(autoload_func) ? 1 : 0;
	fcall_cache.function_handler = EG(autoload_func);
	fcall_cache.calling_scope = NULL;
	fcall_cache.object_pp = NULL;

	exception = EG(exception);
	EG(exception) = NULL;
	retval = zend_call_function(&fcall_info, &fcall_cache TSRMLS_CC);
	EG(autoload_func) = fcall_cache.function_handler;

	zval_ptr_dtor(&class_name_ptr);

	zend_hash_del(EG(in_autoload), lc_name, name_length+1);

	if (retval == FAILURE) {
		EG(exception) = exception;
		free_alloca_with_limit(lc_name, use_heap);
		return FAILURE;
	}

	if (EG(exception) && exception) {
		free_alloca_with_limit(lc_name, use_heap);
		zend_error(E_ERROR, "Function %s(%s) threw an exception of type '%s'", ZEND_AUTOLOAD_FUNC_NAME, name, Z_OBJCE_P(EG(exception))->name);
		return FAILURE;
	}
	if (!EG(exception)) {
		EG(exception) = exception;
	}
	if (retval_ptr) {
		zval_ptr_dtor(&retval_ptr);
	}

	retval = zend_hash_find(EG(class_table), lc_name, name_length + 1, (void **) ce);
	free_alloca_with_limit(lc_name, use_heap);
	return retval;
}

ZEND_API int zend_lookup_class(char *name, int name_length, zend_class_entry ***ce TSRMLS_DC)
{
	return zend_lookup_class_ex(name, name_length, 1, ce TSRMLS_CC);
}

ZEND_API int zend_eval_string(char *str, zval *retval_ptr, char *string_name TSRMLS_DC)
{
	zval pv;
	zend_op_array *new_op_array;
	zend_op_array *original_active_op_array = EG(active_op_array);
	zend_function_state *original_function_state_ptr = EG(function_state_ptr);
	zend_uchar original_handle_op_arrays;
	int retval;

	if (retval_ptr) {
		int l = strlen(str);
		Z_STRLEN(pv) = l+sizeof("return  ;")-1;
		Z_STRVAL(pv) = emalloc(Z_STRLEN(pv) + 1);
		memcpy(Z_STRVAL(pv), "return ", sizeof("return ")-1);
		memcpy(Z_STRVAL(pv) + sizeof("return ")-1, str, l);
		Z_STRVAL(pv)[Z_STRLEN(pv)-2] = ' ';
		Z_STRVAL(pv)[Z_STRLEN(pv)-1] = ';';
		Z_STRVAL(pv)[Z_STRLEN(pv)] = '\0';
	} else {
		pv.value.str.len = strlen(str);
		pv.value.str.val = estrndup(str, pv.value.str.len);
	}
	pv.type = IS_STRING;

	/*printf("Evaluating '%s'\n", pv.value.str.val);*/

	original_handle_op_arrays = CG(handle_op_arrays);
	CG(handle_op_arrays) = 0;
	new_op_array = zend_compile_string(&pv, string_name TSRMLS_CC);
	CG(handle_op_arrays) = original_handle_op_arrays;

	if (new_op_array) {
		zval *local_retval_ptr=NULL;
		zval **original_return_value_ptr_ptr = EG(return_value_ptr_ptr);
		zend_op **original_opline_ptr = EG(opline_ptr);

		EG(return_value_ptr_ptr) = &local_retval_ptr;
		EG(active_op_array) = new_op_array;
		EG(no_extensions)=1;

		zend_execute(new_op_array TSRMLS_CC);

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
		EG(function_state_ptr) = original_function_state_ptr;
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		EG(return_value_ptr_ptr) = original_return_value_ptr_ptr;
		retval = SUCCESS;
	} else {
		retval = FAILURE;
	}
	zval_dtor(&pv);
	return retval;
}


ZEND_API int zend_eval_string_ex(char *str, zval *retval_ptr, char *string_name, int handle_exceptions TSRMLS_DC)
{
	int result;

	result = zend_eval_string(str, retval_ptr, string_name TSRMLS_CC);
	if (handle_exceptions && EG(exception)) {
		zend_exception_error(EG(exception) TSRMLS_CC);
		result = FAILURE;
	}
	return result;
}


void execute_new_code(TSRMLS_D)
{
	zend_op *opline, *end;
	zend_op *ret_opline;
	zval *local_retval=NULL;

	if (!(CG(active_op_array)->fn_flags & ZEND_ACC_INTERACTIVE)
		|| CG(active_op_array)->backpatch_count>0
		|| CG(active_op_array)->function_name
		|| CG(active_op_array)->type!=ZEND_USER_FUNCTION) {
		return;
	}

	ret_opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	ret_opline->opcode = ZEND_RETURN;
	ret_opline->op1.op_type = IS_CONST;
	INIT_ZVAL(ret_opline->op1.u.constant);
	SET_UNUSED(ret_opline->op2);

	zend_do_handle_exception(TSRMLS_C);

	if (!CG(active_op_array)->start_op) {
		CG(active_op_array)->start_op = CG(active_op_array)->opcodes;
	}

	opline=CG(active_op_array)->start_op;
	end=CG(active_op_array)->opcodes+CG(active_op_array)->last;

	while (opline<end) {
		if (opline->op1.op_type==IS_CONST) {
			opline->op1.u.constant.is_ref = 1;
			opline->op1.u.constant.refcount = 2; /* Make sure is_ref won't be reset */
		}
		if (opline->op2.op_type==IS_CONST) {
			opline->op2.u.constant.is_ref = 1;
			opline->op2.u.constant.refcount = 2;
		}
		switch (opline->opcode) {
			case ZEND_JMP:
				opline->op1.u.jmp_addr = &CG(active_op_array)->opcodes[opline->op1.u.opline_num];
				break;
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
				opline->op2.u.jmp_addr = &CG(active_op_array)->opcodes[opline->op2.u.opline_num];
				break;
		}
		ZEND_VM_SET_OPCODE_HANDLER(opline);
		opline++;
	}

	EG(return_value_ptr_ptr) = &local_retval;
	EG(active_op_array) = CG(active_op_array);
	zend_execute(CG(active_op_array) TSRMLS_CC);
	if (local_retval) {
		zval_ptr_dtor(&local_retval);
	}

	if (EG(exception)) {
		zend_exception_error(EG(exception) TSRMLS_CC);
	}

	CG(active_op_array)->last -= 2;	/* get rid of that ZEND_RETURN and ZEND_HANDLE_EXCEPTION */
	CG(active_op_array)->start_op = CG(active_op_array)->opcodes+CG(active_op_array)->last;
}


ZEND_API void zend_timeout(int dummy)
{
	TSRMLS_FETCH();

	if (zend_on_timeout) {
		zend_on_timeout(EG(timeout_seconds) TSRMLS_CC);
	}

	zend_error(E_ERROR, "Maximum execution time of %d second%s exceeded",
			  EG(timeout_seconds), EG(timeout_seconds) == 1 ? "" : "s");
}

#ifdef ZEND_WIN32
static LRESULT CALLBACK zend_timeout_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_REGISTER_ZEND_TIMEOUT:
			/* wParam is the thread id pointer, lParam is the timeout amount in seconds */
			if (lParam==0) {
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



static unsigned __stdcall timeout_thread_proc(void *pArgs)
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


void zend_init_timeout_thread(void)
{
	timeout_thread_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	timeout_thread_handle = CreateEvent(NULL, FALSE, FALSE, NULL);
	_beginthreadex(NULL, 0, timeout_thread_proc, NULL, 0, &timeout_thread_id);
	WaitForSingleObject(timeout_thread_event, INFINITE);
}


void zend_shutdown_timeout_thread(void)
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

#endif

/* This one doesn't exists on QNX */
#ifndef SIGPROF
#define SIGPROF 27
#endif

void zend_set_timeout(long seconds)
{
	TSRMLS_FETCH();

	EG(timeout_seconds) = seconds;
	if(!seconds) {
		return;
	}
#ifdef ZEND_WIN32
	if (timeout_thread_initialized==0 && InterlockedIncrement(&timeout_thread_initialized)==1) {
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
		sigset_t sigset;

		t_r.it_value.tv_sec = seconds;
		t_r.it_value.tv_usec = t_r.it_interval.tv_sec = t_r.it_interval.tv_usec = 0;

#	ifdef __CYGWIN__
		setitimer(ITIMER_REAL, &t_r, NULL);
		signal(SIGALRM, zend_timeout);
		sigemptyset(&sigset);
		sigaddset(&sigset, SIGALRM);
#	else
		setitimer(ITIMER_PROF, &t_r, NULL);
		signal(SIGPROF, zend_timeout);
		sigemptyset(&sigset);
		sigaddset(&sigset, SIGPROF);
#	endif
		sigprocmask(SIG_UNBLOCK, &sigset, NULL);
	}
#	endif
#endif
}


void zend_unset_timeout(TSRMLS_D)
{
#ifdef ZEND_WIN32
	if(timeout_thread_initialized) {
		PostThreadMessage(timeout_thread_id, WM_UNREGISTER_ZEND_TIMEOUT, (WPARAM) GetCurrentThreadId(), (LPARAM) 0);
	}
#else
#	ifdef HAVE_SETITIMER
	{
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


zend_class_entry *zend_fetch_class(char *class_name, uint class_name_len, int fetch_type TSRMLS_DC)
{
	zend_class_entry **pce;
	int use_autoload = (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) == 0;

	fetch_type = fetch_type & ~ZEND_FETCH_CLASS_NO_AUTOLOAD;
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
		case ZEND_FETCH_CLASS_AUTO: {
				fetch_type = zend_get_class_fetch_type(class_name, class_name_len);
				if (fetch_type!=ZEND_FETCH_CLASS_DEFAULT) {
					goto check_fetch_type;
				}
			}
			break;
	}

	if (zend_lookup_class_ex(class_name, class_name_len, use_autoload, &pce TSRMLS_CC)==FAILURE) {
		if (use_autoload) {
			if (fetch_type == ZEND_FETCH_CLASS_INTERFACE) {
				zend_error(E_ERROR, "Interface '%s' not found", class_name);
			} else {
				zend_error(E_ERROR, "Class '%s' not found", class_name);
			}
		}
		return NULL;
	}
	return *pce;
}



#define MAX_ABSTRACT_INFO_CNT 3
#define MAX_ABSTRACT_INFO_FMT "%s%s%s%s"
#define DISPLAY_ABSTRACT_FN(idx) \
	ai.afn[idx] ? ZEND_FN_SCOPE_NAME(ai.afn[idx]) : "", \
	ai.afn[idx] ? "::" : "", \
	ai.afn[idx] ? ai.afn[idx]->common.function_name : "", \
	ai.afn[idx] && ai.afn[idx+1] ? ", " : (ai.afn[idx] && ai.cnt > MAX_ABSTRACT_INFO_CNT ? ", ..." : "")

typedef struct _zend_abstract_info {
	zend_function *afn[MAX_ABSTRACT_INFO_CNT+1];
	int cnt;
} zend_abstract_info;


static int zend_verify_abstract_class_function(zend_function *fn, zend_abstract_info *ai TSRMLS_DC)
{
	if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
		if (ai->cnt < MAX_ABSTRACT_INFO_CNT) {
			ai->afn[ai->cnt] = fn;
		}
		ai->cnt++;
	}
	return 0;
}


void zend_verify_abstract_class(zend_class_entry *ce TSRMLS_DC)
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

ZEND_API void zend_reset_all_cv(HashTable *symbol_table TSRMLS_DC)
{
	zend_execute_data *ex;
	int i;

	for (ex = EG(current_execute_data); ex; ex = ex->prev_execute_data) {
		if (ex->op_array && ex->symbol_table == symbol_table) {
			for (i = 0; i < ex->op_array->last_var; i++) {
				ex->CVs[i] = NULL;
			}
		}
	}
}

ZEND_API int zend_delete_global_variable(char *name, int name_len TSRMLS_DC)
{
	zend_execute_data *ex;
	ulong hash_value = zend_inline_hash_func(name, name_len+1);

	if (zend_hash_quick_exists(&EG(symbol_table), name, name_len+1, hash_value)) {
		for (ex = EG(current_execute_data); ex; ex = ex->prev_execute_data) {
			if (ex->op_array && ex->symbol_table == &EG(symbol_table)) {
				int i;
				for (i = 0; i < ex->op_array->last_var; i++) {
					if (ex->op_array->vars[i].hash_value == hash_value &&
					    ex->op_array->vars[i].name_len == name_len &&
					    !memcmp(ex->op_array->vars[i].name, name, name_len)) {
						ex->CVs[i] = NULL;
						break;
					}
				}
			}
		}
		return zend_hash_del(&EG(symbol_table), name, name_len+1);
	}
	return FAILURE;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
