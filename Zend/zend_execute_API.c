/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
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
#include "zend_variables.h"
#include "zend_operators.h"
#include "zend_constants.h"
#include "zend_extensions.h"


ZEND_API void (*zend_execute)(zend_op_array *op_array ELS_DC);


static void (*original_sigsegv_handler)(int);
static void zend_handle_sigsegv(int dummy)
{
	fflush(stdout);
	signal(SIGSEGV, original_sigsegv_handler);
/*
	printf("SIGSEGV caught on opcode %d on opline %d of %s() at %s:%d\n\n",
			active_opline->opcode,
			active_opline-EG(active_op_array)->opcodes,
			get_active_function_name(),
			zend_get_executed_filename(),
			zend_get_executed_lineno());
	original_sigsegv_handler(dummy);
*/
}


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
	var_uninit(&EG(uninitialized_zval));
	var_uninit(&EG(error_zval));
	EG(uninitialized_zval).refcount = 1;
	EG(uninitialized_zval).is_ref=0;
	EG(uninitialized_zval_ptr)=&EG(uninitialized_zval);
	EG(error_zval).refcount = 1;
	EG(error_zval).is_ref=0;
	EG(error_zval_ptr)=&EG(error_zval);
	zend_ptr_stack_init(&EG(function_symbol_table_stack));
	zend_ptr_stack_init(&EG(arg_types_stack));
	zend_stack_init(&EG(overloaded_objects_stack));
	original_sigsegv_handler = signal(SIGSEGV, zend_handle_sigsegv);
	EG(return_value) = &EG(global_return_value);
	EG(symtable_cache_ptr) = EG(symtable_cache)-1;
	EG(symtable_cache_limit)=EG(symtable_cache)+SYMTABLE_CACHE_SIZE-1;
	EG(no_extensions)=0;

	EG(function_table) = CG(function_table);
	EG(class_table) = CG(class_table);

	EG(AiCount) = 0;
	zend_ptr_stack_init(&EG(garbage));

	EG(main_op_array) = NULL;
	zend_hash_init(&EG(symbol_table), 50, NULL, PVAL_PTR_DTOR, 0);
	EG(active_symbol_table) = &EG(symbol_table);

	zend_llist_apply(&zend_extensions, (void (*)(void *)) zend_extension_activator);

	/* $GLOBALS array */
	{
		zval *globals = (zval *) emalloc(sizeof(zval));
		
		globals->value.ht = &EG(symbol_table);
		globals->type = IS_ARRAY;
		globals->refcount = 1;
		globals->is_ref = 0;
		zend_hash_update(&EG(symbol_table), "GLOBALS", sizeof("GLOBALS"), &globals, sizeof(zval *), NULL);
	}
}


void shutdown_executor(ELS_D)
{
	zend_ptr_stack_destroy(&EG(function_symbol_table_stack));
	zend_ptr_stack_destroy(&EG(arg_types_stack));
	zend_stack_destroy(&EG(overloaded_objects_stack));
			
	while (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
		zend_hash_destroy(*EG(symtable_cache_ptr));
		efree(*EG(symtable_cache_ptr));
		EG(symtable_cache_ptr)--;
	}
	zend_llist_apply(&zend_extensions, (void (*)(void *)) zend_extension_deactivator);
	zend_ptr_stack_destroy(&EG(garbage));

	zend_hash_destroy(&EG(symbol_table));

	if (EG(main_op_array)) {
		destroy_op_array(EG(main_op_array));
		efree(EG(main_op_array));
	}
	clean_non_persistent_constants();
}


ZEND_API char *get_active_function_name(ELS_D)
{
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
	return active_opline->filename;
}


ZEND_API uint zend_get_executed_lineno(ELS_D)
{
	return active_opline->lineno;
}


ZEND_API inline void safe_free_zval_ptr(zval *p)
{
	ELS_FETCH();

	if (p!=EG(uninitialized_zval_ptr)) {
		efree(p);
	}
}


ZEND_API void zval_ptr_dtor(zval **zval_ptr)
{
#if DEBUG_ZEND>=2
	printf("Reducing refcount for %x (%x):  %d->%d\n", *zval_ptr, zval_ptr, (*zval_ptr)->refcount, (*zval_ptr)->refcount-1);
#endif
	(*zval_ptr)->refcount--;
	if ((*zval_ptr)->refcount==0) {
		zval_dtor(*zval_ptr);
		safe_free_zval_ptr(*zval_ptr);
	}
}


ZEND_API inline int i_zend_is_true(zval *op)
{
	int result;

	switch (op->type) {
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


ZEND_API void zval_update_constant(zval *p)
{
	if (p->type == IS_CONSTANT) {
		zval c;

		if (!zend_get_constant(p->value.str.val, p->value.str.len, &c)) {
			zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",
						p->value.str.val,
						p->value.str.val);
			p->type = IS_STRING;
		} else {
			STR_FREE(p->value.str.val);
			*p = c;
		}
		p->refcount = 1;
		p->is_ref = 0;
	}
}

int call_user_function(HashTable *function_table, zval *object, zval *function_name, zval *retval, int param_count, zval *params[])
{
	int i;
	zval *original_return_value;
	HashTable *calling_symbol_table;
	zend_function_state function_state;
	zend_function_state *original_function_state_ptr;
	zend_op_array *original_op_array;
	zend_op **original_opline_ptr;
	zval *this=NULL;
	CLS_FETCH();
	ELS_FETCH();

	if (object) {
		if (object->type != IS_OBJECT) {
			return FAILURE;
		}
		function_table = &object->value.obj.ce->function_table;
		/* unimplemented */
	}
	original_function_state_ptr = EG(function_state_ptr);
	zend_str_tolower(function_name->value.str.val, function_name->value.str.len);
	if (zend_hash_find(function_table, function_name->value.str.val, function_name->value.str.len+1, (void **) &function_state.function)==FAILURE) {
		zend_error(E_ERROR, "Unknown function:  %s()\n", function_name->value.str.val);
		return FAILURE;
	}

	zend_ptr_stack_push(&EG(function_symbol_table_stack), function_state.function_symbol_table);
	function_state.function_symbol_table = (HashTable *) emalloc(sizeof(HashTable));
	zend_hash_init(function_state.function_symbol_table, 0, NULL, PVAL_PTR_DTOR, 0);

	for (i=0; i<param_count; i++) {
		zval *param;

		param = (zval *) emalloc(sizeof(zval));
		*param = *(params[i]);
		param->refcount=1;
		param->is_ref=0;
		zval_copy_ctor(param);
		zend_hash_next_index_insert_ptr(function_state.function_symbol_table, param, sizeof(zval *), NULL);
	}

	if (object) {
		zval *dummy = (zval *) emalloc(sizeof(zval)), **this_ptr;

		var_uninit(dummy);
		dummy->refcount=1;
		dummy->is_ref=0;
		zend_hash_update_ptr(function_state.function_symbol_table, "this", sizeof("this"), dummy, sizeof(zval *), (void **) &this_ptr);
		zend_assign_to_variable_reference(NULL, this_ptr, &object, NULL ELS_CC);
	}

	calling_symbol_table = EG(active_symbol_table);
	EG(active_symbol_table) = function_state.function_symbol_table;
	var_uninit(retval);
	if (function_state.function->type == ZEND_USER_FUNCTION) {
		original_return_value = EG(return_value);
		original_op_array = EG(active_op_array);
		EG(return_value) = retval;
		EG(active_op_array) = (zend_op_array *) function_state.function;
		original_opline_ptr = EG(opline_ptr);	
		zend_execute(EG(active_op_array) ELS_CC);
		EG(active_op_array) = original_op_array;
		EG(return_value)=original_return_value;
		EG(opline_ptr) = original_opline_ptr;
	} else {
		((zend_internal_function *) function_state.function)->handler(EG(active_symbol_table), retval, &EG(regular_list), &EG(persistent_list));
	}
	zend_hash_destroy(EG(active_symbol_table));
	efree(EG(active_symbol_table));
	EG(active_symbol_table) = calling_symbol_table;
	EG(function_state_ptr) = original_function_state_ptr;
	function_state.function_symbol_table = zend_ptr_stack_pop(&EG(function_symbol_table_stack));

	return SUCCESS;
}


ZEND_API void zend_eval_string(char *str, zval *retval CLS_DC ELS_DC)
{
	zval pv;
	zend_op_array *new_op_array;
	zend_op_array *original_active_op_array = EG(active_op_array);
	zend_function_state *original_function_state_ptr = EG(function_state_ptr);
	int original_handle_op_arrays;

	if (retval) {
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
		zval dummy_retval;
		zval *original_return_value = EG(return_value);
		zend_op **original_opline_ptr = EG(opline_ptr);
		
		EG(return_value) = (retval?retval:&dummy_retval);
		var_reset(EG(return_value));
		EG(active_op_array) = new_op_array;
		EG(no_extensions)=1;
		zend_execute(new_op_array ELS_CC);
		EG(no_extensions)=0;
		EG(opline_ptr) = original_opline_ptr;
		EG(active_op_array) = original_active_op_array;
		EG(function_state_ptr) = original_function_state_ptr;
		destroy_op_array(new_op_array);
		efree(new_op_array);
		EG(return_value) = original_return_value;
		if (!retval) {
			zval_dtor(&dummy_retval);
		}
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
	} else if (variable_ptr!=value_ptr) {
		variable_ptr->refcount--;
		if (variable_ptr->refcount==0) {
			zendi_zval_dtor(*variable_ptr);
			efree(variable_ptr);
		}

		if (!value_ptr->is_ref) {
			/* break it away */
			value_ptr->refcount--;
			if (value_ptr->refcount>0) {
				*value_ptr_ptr = (zval *) emalloc(sizeof(zval));
				**value_ptr_ptr = *value_ptr;
				value_ptr = *value_ptr_ptr;
				zendi_zval_copy_ctor(*value_ptr);
			}
			value_ptr->refcount=1;
			value_ptr->is_ref=1;
		}

		*variable_ptr_ptr = value_ptr;
		value_ptr->refcount++;
	} else {
		/* nothing to do */
	}

	if (result && (result->op_type != IS_UNUSED)) {
		Ts[result->u.var].var = variable_ptr_ptr;
	}
}


#if SUPPORT_INTERACTIVE
void execute_new_code(CLS_D)
{
	ELS_FETCH();

	if (!EG(interactive)
		|| CG(active_op_array)->backpatch_count>0) {
		
		return;
	}
	CG(active_op_array)->start_op_number = CG(active_op_array)->last_executed_op_number;
	CG(active_op_array)->end_op_number = CG(active_op_array)->last;
	EG(active_op_array) = CG(active_op_array);
	zend_execute(CG(active_op_array) ELS_CC);
	CG(active_op_array)->start_op_number = CG(active_op_array)->last_executed_op_number;
}
#endif
