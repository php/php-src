/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include <stdio.h>
#include <signal.h>

#include "zend.h"
#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_API.h"
#include "zend_ptr_stack.h"
#include "zend_constants.h"
#include "zend_extensions.h"
#include "zend_execute_locks.h"


ZEND_API void (*zend_execute)(zend_op_array *op_array ELS_DC);


#if ZEND_DEBUG
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
		ELS_FETCH();

		fprintf(stderr, "SIGSEGV caught on opcode %d on opline %d of %s() at %s:%d\n\n",
				active_opline->opcode,
				active_opline-EG(active_op_array)->opcodes,
				get_active_function_name(),
				zend_get_executed_filename(ELS_C),
				zend_get_executed_lineno(ELS_C));
	}
	if (original_sigsegv_handler!=zend_handle_sigsegv) {
		original_sigsegv_handler(dummy);
	}
}
#endif


static void zend_extension_activator(zend_extension *extension)
{
	if (extension->activate) {
		extension->activate();
	}
}


static void zend_extension_deactivator(zend_extension *extension)
{
	if (extension->deactivate) {
		extension->deactivate();
	}
}


void init_executor(CLS_D ELS_DC)
{
	INIT_ZVAL(EG(uninitialized_zval));
	INIT_ZVAL(EG(error_zval));
	EG(uninitialized_zval_ptr)=&EG(uninitialized_zval);
	EG(error_zval_ptr)=&EG(error_zval);
	zend_ptr_stack_init(&EG(arg_types_stack));
/* destroys stack frame, therefore makes core dumps worthless */
#if 0&&ZEND_DEBUG
	original_sigsegv_handler = signal(SIGSEGV, zend_handle_sigsegv);
#endif
	EG(return_value_ptr_ptr) = &EG(global_return_value_ptr);
	EG(global_return_value_ptr) = &EG(global_return_value);
	INIT_ZVAL(EG(global_return_value));

	EG(symtable_cache_ptr) = EG(symtable_cache)-1;
	EG(symtable_cache_limit)=EG(symtable_cache)+SYMTABLE_CACHE_SIZE-1;
	EG(no_extensions)=0;

	EG(function_table) = CG(function_table);
	EG(class_table) = CG(class_table);

	EG(in_execution) = 0;

	zend_ptr_stack_init(&EG(argument_stack));

	EG(main_op_array) = NULL;
	zend_hash_init(&EG(symbol_table), 50, NULL, ZVAL_PTR_DTOR, 0);
	EG(active_symbol_table) = &EG(symbol_table);

	zend_llist_apply(&zend_extensions, (void (*)(void *)) zend_extension_activator);
	EG(opline_ptr) = NULL;
	EG(garbage_ptr) = 0;

	zend_hash_init(&EG(included_files), 5, NULL, NULL, 0);

	EG(ticks_count) = 0;
}


void shutdown_executor(ELS_D)
{
	if (EG(global_return_value_ptr) == &EG(global_return_value)) {
		zval_dtor(&EG(global_return_value));
	} else {
		zval_ptr_dtor(EG(return_value_ptr_ptr));
	}
	zend_ptr_stack_destroy(&EG(arg_types_stack));
			
	while (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
		zend_hash_destroy(*EG(symtable_cache_ptr));
		efree(*EG(symtable_cache_ptr));
		EG(symtable_cache_ptr)--;
	}
	zend_llist_apply(&zend_extensions, (void (*)(void *)) zend_extension_deactivator);

	zend_hash_destroy(&EG(symbol_table));

	while (EG(garbage_ptr)--) {
		if (EG(garbage)[EG(garbage_ptr)]->refcount==1) {
			zval_ptr_dtor(&EG(garbage)[EG(garbage_ptr)]);
		}
	}

	zend_destroy_rsrc_list(ELS_C); /* must be destroyed after the main symbol table is destroyed */

	zend_ptr_stack_destroy(&EG(argument_stack));
	if (EG(main_op_array)) {
		destroy_op_array(EG(main_op_array));
		efree(EG(main_op_array));
	}
	clean_non_persistent_constants();
#if ZEND_DEBUG
	signal(SIGSEGV, original_sigsegv_handler);
#endif


	zend_hash_destroy(&EG(included_files));
}


ZEND_API char *get_active_function_name()
{
	ELS_FETCH();

	switch(EG(function_state_ptr)->function->type) {
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


ZEND_API char *zend_get_executed_filename(ELS_D)
{
	if (EG(opline_ptr)) {
		return active_opline->filename;
	} else {
		return "[no active file]";
	}
}


ZEND_API uint zend_get_executed_lineno(ELS_D)
{
	if (EG(opline_ptr)) {
		return active_opline->lineno;
	} else {
		return 0;
	}
}


ZEND_API zend_bool zend_is_executing()
{
	ELS_FETCH();

	return EG(in_execution);
}


ZEND_API inline void safe_free_zval_ptr(zval *p)
{
	ELS_FETCH();

	if (p!=EG(uninitialized_zval_ptr)) {
		FREE_ZVAL(p);
	}
}


ZEND_API void _zval_ptr_dtor(zval **zval_ptr ZEND_FILE_LINE_DC)
{
#if DEBUG_ZEND>=2
	printf("Reducing refcount for %x (%x):  %d->%d\n", *zval_ptr, zval_ptr, (*zval_ptr)->refcount, (*zval_ptr)->refcount-1);
#endif
	(*zval_ptr)->refcount--;
	if ((*zval_ptr)->refcount==0) {
		zval_dtor(*zval_ptr);
		safe_free_zval_ptr(*zval_ptr);
	} else if (((*zval_ptr)->refcount == 1) && ((*zval_ptr)->type != IS_OBJECT)) {
		(*zval_ptr)->is_ref = 0;
	}
}
	

ZEND_API inline int i_zend_is_true(zval *op)
{
	int result;

	switch (op->type) {
		case IS_NULL:
			result = 0;
			break;
		case IS_LONG:
		case IS_BOOL:
		case IS_RESOURCE:
			result = (op->value.lval?1:0);
			break;
		case IS_DOUBLE:
			result = (op->value.dval ? 1 : 0);
			break;
		case IS_STRING:
			if (op->value.str.len == 0
				|| (op->value.str.len==1 && op->value.str.val[0]=='0')) {
				result = 0;
			} else {
				result = 1;
			}
			break;
		case IS_ARRAY:
			result = (zend_hash_num_elements(op->value.ht)?1:0);
			break;
		case IS_OBJECT:
			result = (zend_hash_num_elements(op->value.obj.properties)?1:0);
			break;
		default:
			result = 0;
			break;
	}
	return result;
}


ZEND_API int zend_is_true(zval *op)
{
	return i_zend_is_true(op);
}


ZEND_API int zval_update_constant(zval **pp)
{
	zval *p = *pp;

	if (p->type == IS_CONSTANT) {
		zval c;
		int refcount = p->refcount;

		if (!zend_get_constant(p->value.str.val, p->value.str.len, &c)) {
			zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",
						p->value.str.val,
						p->value.str.val);
			p->type = IS_STRING;
		} else {
			STR_FREE(p->value.str.val);
			*p = c;
		}
		INIT_PZVAL(p);
		p->refcount = refcount;
	} else if (p->type == IS_ARRAY) {
		zend_hash_apply(p->value.ht, (int (*)(void *)) zval_update_constant);
	}
	return 0;
}


int call_user_function(HashTable *function_table, zval *object, zval *function_name, zval *retval_ptr, int param_count, zval *params[])
{
	zval ***params_array = (zval ***) emalloc(sizeof(zval **)*param_count);
	int i;
	int ex_retval;
	zval *local_retval_ptr;

	for (i=0; i<param_count; i++) {
		params_array[i] = &params[i];
	}
	ex_retval = call_user_function_ex(function_table, object, function_name, &local_retval_ptr, param_count, params_array, 1);
	if (local_retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*retval_ptr, local_retval_ptr);
	} else {
		INIT_ZVAL(*retval_ptr);
	}
	efree(params_array);
	return ex_retval;
}


int call_user_function_ex(HashTable *function_table, zval *object, zval *function_name, zval **retval_ptr_ptr, int param_count, zval **params[], int no_separation)
{
	int i;
	zval **original_return_value;
	HashTable *calling_symbol_table;
	zend_function_state function_state;
	zend_function_state *original_function_state_ptr;
	zend_op_array *original_op_array;
	zend_op **original_opline_ptr;
	ELS_FETCH();

	*retval_ptr_ptr = NULL;

	if (object) {
		if (object->type != IS_OBJECT) {
			return FAILURE;
		}
		function_table = &object->value.obj.ce->function_table;
	}
	original_function_state_ptr = EG(function_state_ptr);
	zend_str_tolower(function_name->value.str.val, function_name->value.str.len);
	if (zend_hash_find(function_table, function_name->value.str.val, function_name->value.str.len+1, (void **) &function_state.function)==FAILURE) {
		return FAILURE;
	}

	for (i=0; i<param_count; i++) {
		zval *param;

		if (function_state.function->common.arg_types
			&& i<function_state.function->common.arg_types[0]
			&& function_state.function->common.arg_types[i+1]==BYREF_FORCE
			&& !PZVAL_IS_REF(*params[i])) {
			if ((*params[i])->refcount>1) {
				zval *new_zval;

				if (no_separation) {
					return FAILURE;
				}
				ALLOC_ZVAL(new_zval);
				*new_zval = **params[i];
				zval_copy_ctor(new_zval);
				new_zval->refcount = 1;
				(*params[i])->refcount--;
				*params[i] = new_zval;
			}
			(*params[i])->refcount++;
			(*params[i])->is_ref = 1;
			param = *params[i];
		} else if (*params[i] != &EG(uninitialized_zval)) {
			(*params[i])->refcount++;
			param = *params[i];
		} else {
			ALLOC_ZVAL(param);
			*param = **(params[i]);
			INIT_PZVAL(param);
		}
		zend_ptr_stack_push(&EG(argument_stack), param);
	}

	zend_ptr_stack_n_push(&EG(argument_stack), 2, (void *) (long) param_count, NULL);

	if (function_state.function->type == ZEND_USER_FUNCTION) {
		calling_symbol_table = EG(active_symbol_table);
		ALLOC_HASHTABLE(EG(active_symbol_table));
		zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);
		if (object) {
			zval *dummy, **this_ptr;

			ALLOC_ZVAL(dummy);
			INIT_ZVAL(*dummy);
			
			zend_hash_update(EG(active_symbol_table), "this", sizeof("this"), &dummy, sizeof(zval *), (void **) &this_ptr);
			zend_assign_to_variable_reference(NULL, this_ptr, &object, NULL ELS_CC);
		}
		original_return_value = EG(return_value_ptr_ptr);
		original_op_array = EG(active_op_array);
		EG(return_value_ptr_ptr) = retval_ptr_ptr;
		EG(active_op_array) = (zend_op_array *) function_state.function;
		original_opline_ptr = EG(opline_ptr);
		zend_execute(EG(active_op_array) ELS_CC);
		zend_hash_destroy(EG(active_symbol_table));		
		FREE_HASHTABLE(EG(active_symbol_table));
		EG(active_symbol_table) = calling_symbol_table;
		EG(active_op_array) = original_op_array;
		EG(return_value_ptr_ptr)=original_return_value;
		EG(opline_ptr) = original_opline_ptr;
	} else {
		ALLOC_INIT_ZVAL(*retval_ptr_ptr);
		((zend_internal_function *) function_state.function)->handler(param_count, *retval_ptr_ptr, object, 1 ELS_CC);
		INIT_PZVAL(*retval_ptr_ptr);
	}
	zend_ptr_stack_clear_multiple(ELS_C);
	EG(function_state_ptr) = original_function_state_ptr;

	return SUCCESS;
}


ZEND_API void zend_eval_string(char *str, zval *retval_ptr CLS_DC ELS_DC)
{
	zval pv;
	zend_op_array *new_op_array;
	zend_op_array *original_active_op_array = EG(active_op_array);
	zend_function_state *original_function_state_ptr = EG(function_state_ptr);
	int original_handle_op_arrays;

	if (retval_ptr) {
		pv.value.str.len = strlen(str)+sizeof("return  ;")-1;
		pv.value.str.val = emalloc(pv.value.str.len+1);
		strcpy(pv.value.str.val, "return ");
		strcat(pv.value.str.val, str);
		strcat(pv.value.str.val, " ;");
	} else {
		pv.value.str.len = strlen(str);
		pv.value.str.val = estrndup(str, pv.value.str.len);
	}
	pv.type = IS_STRING;

	/*printf("Evaluating '%s'\n", pv.value.str.val);*/

	original_handle_op_arrays = CG(handle_op_arrays);
	CG(handle_op_arrays) = 0;
	new_op_array = compile_string(&pv CLS_CC);
	CG(handle_op_arrays) = original_handle_op_arrays;

	if (new_op_array) {
		zval *local_retval_ptr=NULL;
		zval **original_return_value_ptr_ptr = EG(return_value_ptr_ptr);
		zend_op **original_opline_ptr = EG(opline_ptr);
		
		EG(return_value_ptr_ptr) = &local_retval_ptr;
		EG(active_op_array) = new_op_array;
		EG(no_extensions)=1;

		zend_execute(new_op_array ELS_CC);

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
		destroy_op_array(new_op_array);
		efree(new_op_array);
		EG(return_value_ptr_ptr) = original_return_value_ptr_ptr;
	} else {
		printf("Failed executing:\n%s\n", str);
	}
	zval_dtor(&pv);
}


ZEND_API inline void zend_assign_to_variable_reference(znode *result, zval **variable_ptr_ptr, zval **value_ptr_ptr, temp_variable *Ts ELS_DC)
{
	zval *variable_ptr = *variable_ptr_ptr;
	zval *value_ptr;
	

	if (!value_ptr_ptr) {
		zend_error(E_ERROR, "Cannot create references to string offsets nor overloaded objects");
		return;
	}

	value_ptr = *value_ptr_ptr;
	if (variable_ptr == EG(error_zval_ptr) || value_ptr==EG(error_zval_ptr)) {
		variable_ptr_ptr = &EG(uninitialized_zval_ptr);
/*	} else if (variable_ptr==&EG(uninitialized_zval) || variable_ptr!=value_ptr) { */
	} else if (variable_ptr_ptr != value_ptr_ptr) {
		variable_ptr->refcount--;
		if (variable_ptr->refcount==0) {
			zendi_zval_dtor(*variable_ptr);
			FREE_ZVAL(variable_ptr);
		}

		if (!PZVAL_IS_REF(value_ptr)) {
			/* break it away */
			value_ptr->refcount--;
			if (value_ptr->refcount>0) {
				ALLOC_ZVAL(*value_ptr_ptr);
				**value_ptr_ptr = *value_ptr;
				value_ptr = *value_ptr_ptr;
				zendi_zval_copy_ctor(*value_ptr);
			}
			value_ptr->refcount = 1;
			value_ptr->is_ref = 1;
		}

		*variable_ptr_ptr = value_ptr;
		value_ptr->refcount++;
	} else {
		if (variable_ptr->refcount>1) { /* we need to break away */
			SEPARATE_ZVAL(variable_ptr_ptr);
		}
		(*variable_ptr_ptr)->is_ref = 1;
	}

	if (result && (result->op_type != IS_UNUSED)) {
		Ts[result->u.var].var.ptr_ptr = variable_ptr_ptr;
		SELECTIVE_PZVAL_LOCK(*variable_ptr_ptr, result);
		AI_USE_PTR(Ts[result->u.var].var);
	}
}


#if SUPPORT_INTERACTIVE
void execute_new_code(CLS_D)
{
	ELS_FETCH();

	if (!EG(interactive)
		|| CG(active_op_array)->backpatch_count>0
		|| CG(active_op_array)->function_name
		|| CG(active_op_array)->type!=ZEND_USER_FUNCTION) {
		return;
	}
	CG(active_op_array)->start_op_number = CG(active_op_array)->last_executed_op_number;
	CG(active_op_array)->end_op_number = CG(active_op_array)->last;
	EG(active_op_array) = CG(active_op_array);
	zend_execute(CG(active_op_array) ELS_CC);
	CG(active_op_array)->start_op_number = CG(active_op_array)->last_executed_op_number;
}
#endif


/* these are a dedicated, optimized, function, and shouldn't be used for any purpose
 * other than by Zend's executor
 */
ZEND_API inline void zend_ptr_stack_clear_multiple(ELS_D)
{
	void **p = EG(argument_stack).top_element-2;
	int delete_count = (ulong) *p;

	EG(argument_stack).top -= (delete_count+2);
	while (--delete_count>=0) {
		zval_ptr_dtor((zval **) --p);
	}
	EG(argument_stack).top_element = p;
}



ZEND_API int zend_ptr_stack_get_arg(int requested_arg, void **data ELS_DC)
{
	void **p = EG(argument_stack).top_element-2;
	int arg_count = (ulong) *p;

	if (requested_arg>arg_count) {
		return FAILURE;
	}
	*data = (p-arg_count+requested_arg-1);
	return SUCCESS;
}

