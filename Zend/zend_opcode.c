/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2002 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include <stdio.h>

#include "zend.h"
#include "zend_alloc.h"
#include "zend_compile.h"
#include "zend_extensions.h"
#include "zend_API.h"


static void zend_extension_op_array_ctor_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC)
{
	if (extension->op_array_ctor) {
		extension->op_array_ctor(op_array);
	}
}


static void zend_extension_op_array_dtor_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC)
{
	if (extension->op_array_dtor) {
		extension->op_array_dtor(op_array);
	}
}


static void op_array_alloc_ops(zend_op_array *op_array)
{
	op_array->opcodes = erealloc(op_array->opcodes, (op_array->size)*sizeof(zend_op));
}



void init_op_array(zend_op_array *op_array, zend_uchar type, int initial_ops_size TSRMLS_DC)
{
	op_array->type = type;

	op_array->backpatch_count = 0;
	if (CG(interactive)) {
		/* We must avoid a realloc() on the op_array in interactive mode, since pointers to constants
		 * will become invalid
		 */
		initial_ops_size = 8192;
	}

	op_array->refcount = (zend_uint *) emalloc(sizeof(zend_uint));
	*op_array->refcount = 1;
	op_array->size = initial_ops_size;
	op_array->last = 0;
	op_array->opcodes = NULL;
	op_array_alloc_ops(op_array);

	op_array->T = 0;

	op_array->function_name = NULL;
	op_array->filename = zend_get_compiled_filename(TSRMLS_C);

	op_array->arg_types = NULL;

	op_array->scope = NULL;

	op_array->brk_cont_array = NULL;
	op_array->last_brk_cont = 0;
	op_array->current_brk_cont = -1;

	op_array->static_variables = NULL;

	op_array->return_reference = 0;
	op_array->done_pass_two = 0;

	op_array->uses_this = 0;

	op_array->start_op = NULL;

	zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_ctor_handler, op_array TSRMLS_CC);
}


ZEND_API void destroy_zend_function(zend_function *function)
{
	switch (function->type) {
		case ZEND_USER_FUNCTION:
			destroy_op_array((zend_op_array *) function);
			break;
		case ZEND_INTERNAL_FUNCTION:
			/* do nothing */
			break;
	}
}


ZEND_API void destroy_zend_class(zend_class_entry **pce)
{
	zend_class_entry *ce = *pce;
	
	if (--ce->refcount > 0) {
		return;
	}
	switch (ce->type) {
		case ZEND_USER_CLASS:
			zend_hash_destroy(&ce->default_properties);
			zend_hash_destroy(&ce->private_properties);
			zend_hash_destroy(&ce->protected_properties);
			zend_hash_destroy(ce->static_members);
			efree(ce->name);
			zend_hash_destroy(&ce->function_table);
			FREE_HASHTABLE(ce->static_members);
			zend_hash_destroy(&ce->constants_table);
			zend_hash_destroy(&ce->class_table);
			efree(ce);
			break;
		case ZEND_INTERNAL_CLASS:
			zend_hash_destroy(&ce->default_properties);
			zend_hash_destroy(&ce->private_properties);
			zend_hash_destroy(&ce->protected_properties);
			zend_hash_destroy(ce->static_members);
			free(ce->name);
			zend_hash_destroy(&ce->function_table);
			free(ce->static_members);
			zend_hash_destroy(&ce->constants_table);
			zend_hash_destroy(&ce->class_table);
			free(ce);
			break;
	}
}


void zend_class_add_ref(zend_class_entry **ce)
{
	(*ce)->refcount++;
}


ZEND_API void destroy_op_array(zend_op_array *op_array)
{
	zend_op *opline = op_array->opcodes;
	zend_op *end = op_array->opcodes+op_array->last;
	TSRMLS_FETCH();

	if (op_array->static_variables) {
		zend_hash_destroy(op_array->static_variables);
		FREE_HASHTABLE(op_array->static_variables);
	}

	if (--(*op_array->refcount)>0) {
		return;
	}

	efree(op_array->refcount);

	while (opline<end) {
		if (opline->op1.op_type==IS_CONST) {
#if DEBUG_ZEND>2
			printf("Reducing refcount for %x 1=>0 (destroying)\n", &opline->op1.u.constant);
#endif
			zval_dtor(&opline->op1.u.constant);
		}
		if (opline->op2.op_type==IS_CONST) {
#if DEBUG_ZEND>2
			printf("Reducing refcount for %x 1=>0 (destroying)\n", &opline->op2.u.constant);
#endif
			zval_dtor(&opline->op2.u.constant);
		}
		opline++;
	}
	efree(op_array->opcodes);
	if (op_array->function_name) {
		efree(op_array->function_name);
	}
	if (op_array->arg_types) {
		efree(op_array->arg_types);
	}
	if (op_array->brk_cont_array) {
		efree(op_array->brk_cont_array);
	}
	if (op_array->done_pass_two) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_dtor_handler, op_array TSRMLS_CC);
	}
}


void init_op(zend_op *op TSRMLS_DC)
{
	memset(&op->result, 0, sizeof(znode));
	op->lineno = CG(zend_lineno);
	SET_UNUSED(op->result);
	op->extended_value = 0;
	memset(&op->op1, 0, sizeof(znode));
	memset(&op->op2, 0, sizeof(znode));
}

zend_op *get_next_op(zend_op_array *op_array TSRMLS_DC)
{
	zend_uint next_op_num = op_array->last++;
	zend_op *next_op;

	if (next_op_num >= op_array->size) {
		if (CG(interactive)) {
			/* we messed up */
			zend_printf("Ran out of opcode space!\n"
						"You should probably consider writing this huge script into a file!\n");
			zend_bailout();
		}
		op_array->size *= 4;
		op_array_alloc_ops(op_array);
	}
	
	next_op = &(op_array->opcodes[next_op_num]);
	
	init_op(next_op TSRMLS_CC);

	return next_op;
}


int get_next_op_number(zend_op_array *op_array)
{
	return op_array->last;
}




zend_brk_cont_element *get_next_brk_cont_element(zend_op_array *op_array)
{
	op_array->last_brk_cont++;
	op_array->brk_cont_array = erealloc(op_array->brk_cont_array, sizeof(zend_brk_cont_element)*op_array->last_brk_cont);
	return &op_array->brk_cont_array[op_array->last_brk_cont-1];
}


static void zend_update_extended_info(zend_op_array *op_array TSRMLS_DC)
{
	zend_op *opline = op_array->opcodes, *end=opline+op_array->last;

	while (opline<end) {
		if (opline->opcode == ZEND_EXT_STMT) {
			if (opline+1<end) {
				if ((opline+1)->opcode == ZEND_EXT_STMT) {
					opline->opcode = ZEND_NOP;
					opline++;
					continue;
				}
				if (opline+1<end) {
					opline->lineno = (opline+1)->lineno;
				}
			} else {
				opline->opcode = ZEND_NOP;
			}
		}
		opline++;
	}
}



static void zend_extension_op_array_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC)
{
	if (extension->op_array_handler) {
		extension->op_array_handler(op_array);
	}
}


int pass_two(zend_op_array *op_array TSRMLS_DC)
{
	zend_op *opline, *end;

	if (op_array->type!=ZEND_USER_FUNCTION && op_array->type!=ZEND_EVAL_CODE) {
		return 0;
	}
	if (CG(extended_info)) {
		zend_update_extended_info(op_array TSRMLS_CC);
	}
	if (CG(handle_op_arrays)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_handler, op_array TSRMLS_CC);
	}

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		if (opline->op1.op_type == IS_CONST) {
			opline->op1.u.constant.is_ref = 1;
			opline->op1.u.constant.refcount = 2; /* Make sure is_ref won't be reset */
		}
		if (opline->op2.op_type == IS_CONST) {
			opline->op2.u.constant.is_ref = 1;
			opline->op2.u.constant.refcount = 2;
		}
		opline++;
	}
	op_array->opcodes = (zend_op *) erealloc(op_array->opcodes, sizeof(zend_op)*op_array->last);
	op_array->size = op_array->last;
	op_array->done_pass_two = 1;
	return 0;
}


int print_class(zend_class_entry *class_entry TSRMLS_DC)
{
	printf("Class %s:\n", class_entry->name);
	zend_hash_apply(&class_entry->function_table, (apply_func_t) pass_two TSRMLS_CC);
	printf("End of class %s.\n\n", class_entry->name);
	return 0;
}

ZEND_API unary_op_type get_unary_op(int opcode)
{
	switch(opcode) {
		case ZEND_BW_NOT:
			return (unary_op_type) bitwise_not_function;
			break;
		case ZEND_BOOL_NOT:
			return (unary_op_type) boolean_not_function;
			break;
		default:
			return (unary_op_type) NULL;
			break;
	}
}


ZEND_API void *get_binary_op(int opcode)
{
	switch (opcode) {
		case ZEND_ADD:
		case ZEND_ASSIGN_ADD:
			return (void *) add_function;
			break;
		case ZEND_SUB:
		case ZEND_ASSIGN_SUB:
			return (void *) sub_function;
			break;
		case ZEND_MUL:
		case ZEND_ASSIGN_MUL:
			return (void *) mul_function;
			break;
		case ZEND_DIV:
		case ZEND_ASSIGN_DIV:
			return (void *) div_function;
			break;
		case ZEND_MOD:
		case ZEND_ASSIGN_MOD:
			return (void *) mod_function;
			break;
		case ZEND_SL:
		case ZEND_ASSIGN_SL:
			return (void *) shift_left_function;
			break;
		case ZEND_SR:
		case ZEND_ASSIGN_SR:
			return (void *) shift_right_function;
			break;
		case ZEND_CONCAT:
		case ZEND_ASSIGN_CONCAT:
			return (void *) concat_function;
			break;
		case ZEND_IS_IDENTICAL:
			return (void *) is_identical_function;
			break;
		case ZEND_IS_NOT_IDENTICAL:
			return (void *) is_not_identical_function;
			break;
		case ZEND_IS_EQUAL:
			return (void *) is_equal_function;
			break;
		case ZEND_IS_NOT_EQUAL:
			return (void *) is_not_equal_function;
			break;
		case ZEND_IS_SMALLER:
			return (void *) is_smaller_function;
			break;
		case ZEND_IS_SMALLER_OR_EQUAL:
			return (void *) is_smaller_or_equal_function;
			break;
		case ZEND_BW_OR:
		case ZEND_ASSIGN_BW_OR:
			return (void *) bitwise_or_function;
			break;
		case ZEND_BW_AND:
		case ZEND_ASSIGN_BW_AND:
			return (void *) bitwise_and_function;
			break;
		case ZEND_BW_XOR:
		case ZEND_ASSIGN_BW_XOR:
			return (void *) bitwise_xor_function;
			break;
		default:
			return (void *) NULL;
			break;
	}
}
