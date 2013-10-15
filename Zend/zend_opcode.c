/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
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

#include "zend.h"
#include "zend_alloc.h"
#include "zend_compile.h"
#include "zend_extensions.h"
#include "zend_API.h"

#include "zend_vm.h"

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

static void op_array_alloc_ops(zend_op_array *op_array, zend_uint size)
{
	op_array->opcodes = erealloc(op_array->opcodes, size * sizeof(zend_op));
}

void init_op_array(zend_op_array *op_array, zend_uchar type, int initial_ops_size TSRMLS_DC)
{
	op_array->type = type;

	if (CG(interactive)) {
		/* We must avoid a realloc() on the op_array in interactive mode, since pointers to constants
		 * will become invalid
		 */
		initial_ops_size = INITIAL_INTERACTIVE_OP_ARRAY_SIZE;
	}

	op_array->refcount = (zend_uint *) emalloc(sizeof(zend_uint));
	*op_array->refcount = 1;
	op_array->last = 0;
	op_array->opcodes = NULL;
	op_array_alloc_ops(op_array, initial_ops_size);

	op_array->last_var = 0;
	op_array->vars = NULL;

	op_array->T = 0;

	op_array->nested_calls = 0;
	op_array->used_stack = 0;

	op_array->function_name = NULL;
	op_array->filename = zend_get_compiled_filename(TSRMLS_C);
	op_array->doc_comment = NULL;
	op_array->doc_comment_len = 0;

	op_array->arg_info = NULL;
	op_array->num_args = 0;
	op_array->required_num_args = 0;

	op_array->scope = NULL;

	op_array->brk_cont_array = NULL;
	op_array->try_catch_array = NULL;
	op_array->last_brk_cont = 0;

	op_array->static_variables = NULL;
	op_array->last_try_catch = 0;
	op_array->has_finally_block = 0;

	op_array->this_var = -1;

	op_array->fn_flags = CG(interactive)?ZEND_ACC_INTERACTIVE:0;

	op_array->early_binding = -1;

	op_array->last_literal = 0;
	op_array->literals = NULL;

	op_array->run_time_cache = NULL;
	op_array->last_cache_slot = 0;

	memset(op_array->reserved, 0, ZEND_MAX_RESERVED_RESOURCES * sizeof(void*));

	zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_ctor_handler, op_array TSRMLS_CC);
}

ZEND_API void destroy_zend_function(zend_function *function TSRMLS_DC)
{
	switch (function->type) {
		case ZEND_USER_FUNCTION:
			destroy_op_array((zend_op_array *) function TSRMLS_CC);
			break;
		case ZEND_INTERNAL_FUNCTION:
			/* do nothing */
			break;
	}
}

ZEND_API void zend_function_dtor(zend_function *function)
{
	TSRMLS_FETCH();

	destroy_zend_function(function TSRMLS_CC);
}

static void zend_cleanup_op_array_data(zend_op_array *op_array)
{
	if (op_array->static_variables) {
		zend_hash_clean(op_array->static_variables);
	}
}

ZEND_API int zend_cleanup_function_data(zend_function *function TSRMLS_DC)
{
	if (function->type == ZEND_USER_FUNCTION) {
		zend_cleanup_op_array_data((zend_op_array *) function);
		return ZEND_HASH_APPLY_KEEP;
	} else {
		return ZEND_HASH_APPLY_STOP;
	}
}

ZEND_API int zend_cleanup_function_data_full(zend_function *function TSRMLS_DC)
{
	if (function->type == ZEND_USER_FUNCTION) {
		zend_cleanup_op_array_data((zend_op_array *) function);
	}
	return 0;
}

static inline void cleanup_user_class_data(zend_class_entry *ce TSRMLS_DC)
{
	/* Clean all parts that can contain run-time data */
	/* Note that only run-time accessed data need to be cleaned up, pre-defined data can
	   not contain objects and thus are not probelmatic */
	if (ce->ce_flags & ZEND_HAS_STATIC_IN_METHODS) {
		zend_hash_apply(&ce->function_table, (apply_func_t) zend_cleanup_function_data_full TSRMLS_CC);
	}
	if (ce->static_members_table) {
		int i;

		for (i = 0; i < ce->default_static_members_count; i++) {
			if (ce->static_members_table[i]) {
				zval *p = ce->static_members_table[i];
				ce->static_members_table[i] = NULL;
				zval_ptr_dtor(&p);
			}
		}
		ce->static_members_table = NULL;
	}
}

static inline void cleanup_internal_class_data(zend_class_entry *ce TSRMLS_DC)
{
	if (CE_STATIC_MEMBERS(ce)) {
		int i;
		
		for (i = 0; i < ce->default_static_members_count; i++) {
			zval_ptr_dtor(&CE_STATIC_MEMBERS(ce)[i]);
		}
		efree(CE_STATIC_MEMBERS(ce));
#ifdef ZTS
		CG(static_members_table)[(zend_intptr_t)(ce->static_members_table)] = NULL;
#else
		ce->static_members_table = NULL;
#endif
	}
}

ZEND_API void zend_cleanup_internal_class_data(zend_class_entry *ce TSRMLS_DC)
{
	cleanup_internal_class_data(ce TSRMLS_CC);
}

ZEND_API int zend_cleanup_user_class_data(zend_class_entry **pce TSRMLS_DC)
{
	if ((*pce)->type == ZEND_USER_CLASS) {
		cleanup_user_class_data(*pce TSRMLS_CC);
		return ZEND_HASH_APPLY_KEEP;
	} else {
		return ZEND_HASH_APPLY_STOP;
	}
}

ZEND_API int zend_cleanup_class_data(zend_class_entry **pce TSRMLS_DC)
{
	if ((*pce)->type == ZEND_USER_CLASS) {
		cleanup_user_class_data(*pce TSRMLS_CC);
	} else {
		cleanup_internal_class_data(*pce TSRMLS_CC);
	}
	return 0;
}

void _destroy_zend_class_traits_info(zend_class_entry *ce)
{
	if (ce->num_traits > 0 && ce->traits) {
		efree(ce->traits);
	}
	
	if (ce->trait_aliases) {
		size_t i = 0;
		while (ce->trait_aliases[i]) {
			if (ce->trait_aliases[i]->trait_method) {
				if (ce->trait_aliases[i]->trait_method->method_name) {
	 				efree((char*)ce->trait_aliases[i]->trait_method->method_name);
				}
				if (ce->trait_aliases[i]->trait_method->class_name) {
	 				efree((char*)ce->trait_aliases[i]->trait_method->class_name);
				}
				efree(ce->trait_aliases[i]->trait_method);
			}
			
			if (ce->trait_aliases[i]->alias) {
				efree((char*)ce->trait_aliases[i]->alias);
			}
			
			efree(ce->trait_aliases[i]);
			i++;
		}
		
		efree(ce->trait_aliases);
	}

	if (ce->trait_precedences) {
		size_t i = 0;
		
		while (ce->trait_precedences[i]) {
			efree((char*)ce->trait_precedences[i]->trait_method->method_name);
			efree((char*)ce->trait_precedences[i]->trait_method->class_name);
			efree(ce->trait_precedences[i]->trait_method);

			if (ce->trait_precedences[i]->exclude_from_classes) {
				efree(ce->trait_precedences[i]->exclude_from_classes);
			}

			efree(ce->trait_precedences[i]);
			i++;
		}
		efree(ce->trait_precedences);
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
			if (ce->default_properties_table) {
				int i;

				for (i = 0; i < ce->default_properties_count; i++) {
					if (ce->default_properties_table[i]) {
						zval_ptr_dtor(&ce->default_properties_table[i]);
				    }
				}
				efree(ce->default_properties_table);
			}
			if (ce->default_static_members_table) {
				int i;

				for (i = 0; i < ce->default_static_members_count; i++) {
					if (ce->default_static_members_table[i]) {
						zval_ptr_dtor(&ce->default_static_members_table[i]);
					}
				}
				efree(ce->default_static_members_table);
			}
			zend_hash_destroy(&ce->properties_info);
			str_efree(ce->name);
			zend_hash_destroy(&ce->function_table);
			zend_hash_destroy(&ce->constants_table);
			if (ce->num_interfaces > 0 && ce->interfaces) {
				efree(ce->interfaces);
			}
			if (ce->info.user.doc_comment) {
				efree((char*)ce->info.user.doc_comment);
			}
			
			_destroy_zend_class_traits_info(ce);
			
			efree(ce);
			break;
		case ZEND_INTERNAL_CLASS:
			if (ce->default_properties_table) {
				int i;

				for (i = 0; i < ce->default_properties_count; i++) {
					if (ce->default_properties_table[i]) {
						zval_internal_ptr_dtor(&ce->default_properties_table[i]);
					}
				}
				free(ce->default_properties_table);
			}
			if (ce->default_static_members_table) {
				int i;

				for (i = 0; i < ce->default_static_members_count; i++) {
					zval_internal_ptr_dtor(&ce->default_static_members_table[i]);
				}
				free(ce->default_static_members_table);
			}
			zend_hash_destroy(&ce->properties_info);
			str_free(ce->name);
			zend_hash_destroy(&ce->function_table);
			zend_hash_destroy(&ce->constants_table);
			if (ce->num_interfaces > 0) {
				free(ce->interfaces);
			}
			free(ce);
			break;
	}
}

void zend_class_add_ref(zend_class_entry **ce)
{
	(*ce)->refcount++;
}

ZEND_API void destroy_op_array(zend_op_array *op_array TSRMLS_DC)
{
	zend_literal *literal = op_array->literals;
	zend_literal *end;
	zend_uint i;

	if (op_array->static_variables) {
		zend_hash_destroy(op_array->static_variables);
		FREE_HASHTABLE(op_array->static_variables);
	}

	if (op_array->run_time_cache) {
		efree(op_array->run_time_cache);
	}

	if (--(*op_array->refcount)>0) {
		return;
	}

	efree(op_array->refcount);

	if (op_array->vars) {
		i = op_array->last_var;
		while (i > 0) {
			i--;
			str_efree(op_array->vars[i].name);
		}
		efree(op_array->vars);
	}

	if (literal) {
	 	end = literal + op_array->last_literal;
	 	while (literal < end) {
			zval_dtor(&literal->constant);
			literal++;
		}
		efree(op_array->literals);
	}
	efree(op_array->opcodes);

	if (op_array->function_name) {
		efree((char*)op_array->function_name);
	}
	if (op_array->doc_comment) {
		efree((char*)op_array->doc_comment);
	}
	if (op_array->brk_cont_array) {
		efree(op_array->brk_cont_array);
	}
	if (op_array->try_catch_array) {
		efree(op_array->try_catch_array);
	}
	if (op_array->fn_flags & ZEND_ACC_DONE_PASS_TWO) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_dtor_handler, op_array TSRMLS_CC);
	}
	if (op_array->arg_info) {
		for (i=0; i<op_array->num_args; i++) {
			str_efree(op_array->arg_info[i].name);
			if (op_array->arg_info[i].class_name) {
				str_efree(op_array->arg_info[i].class_name);
			}
		}
		efree(op_array->arg_info);
	}
}

void init_op(zend_op *op TSRMLS_DC)
{
	memset(op, 0, sizeof(zend_op));
	op->lineno = CG(zend_lineno);
	SET_UNUSED(op->result);
}

zend_op *get_next_op(zend_op_array *op_array TSRMLS_DC)
{
	zend_uint next_op_num = op_array->last++;
	zend_op *next_op;

	if (next_op_num >= CG(context).opcodes_size) {
		if (op_array->fn_flags & ZEND_ACC_INTERACTIVE) {
			/* we messed up */
			zend_printf("Ran out of opcode space!\n"
						"You should probably consider writing this huge script into a file!\n");
			zend_bailout();
		}
		CG(context).opcodes_size *= 4;
		op_array_alloc_ops(op_array, CG(context).opcodes_size);
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

static void zend_check_finally_breakout(zend_op_array *op_array, zend_uint op_num, zend_uint dst_num TSRMLS_DC)
{
	zend_uint i;

	for (i = 0; i < op_array->last_try_catch; i++) {
		if (op_array->try_catch_array[i].try_op > op_num) {
			break;
		}
		if ((op_num >= op_array->try_catch_array[i].finally_op 
					&& op_num <= op_array->try_catch_array[i].finally_end)
				&& (dst_num > op_array->try_catch_array[i].finally_end 
					|| dst_num < op_array->try_catch_array[i].finally_op)) {
			CG(in_compilation) = 1;
			CG(active_op_array) = op_array;
			CG(zend_lineno) = op_array->opcodes[op_num].lineno;
			zend_error(E_COMPILE_ERROR, "jump out of a finally block is disallowed");
		}
	} 
}

static void zend_resolve_finally_call(zend_op_array *op_array, zend_uint op_num, zend_uint dst_num TSRMLS_DC)
{
	zend_uint start_op;
	zend_op *opline;
	zend_uint i = op_array->last_try_catch;

	if (dst_num != (zend_uint)-1) {
		zend_check_finally_breakout(op_array, op_num, dst_num TSRMLS_CC);
	}

	/* the backward order is mater */
	while (i > 0) {
		i--;
		if (op_array->try_catch_array[i].finally_op &&
		    op_num >= op_array->try_catch_array[i].try_op &&
		    op_num < op_array->try_catch_array[i].finally_op - 1 &&
		    (dst_num < op_array->try_catch_array[i].try_op ||
		     dst_num > op_array->try_catch_array[i].finally_end)) {
			/* we have a jump out of try block that needs executing finally */

			/* generate a FAST_CALL to finally block */
		    start_op = get_next_op_number(op_array);

			opline = get_next_op(op_array TSRMLS_CC);
			opline->opcode = ZEND_FAST_CALL;
			SET_UNUSED(opline->op1);
			SET_UNUSED(opline->op2);
			opline->op1.opline_num = op_array->try_catch_array[i].finally_op;
			if (op_array->try_catch_array[i].catch_op) {
				opline->extended_value = 1;
				opline->op2.opline_num = op_array->try_catch_array[i].catch_op;
			}

			/* generate a sequence of FAST_CALL to upward finally block */
			while (i > 0) {
				i--;
				if (op_array->try_catch_array[i].finally_op &&
				    op_num >= op_array->try_catch_array[i].try_op &&
				    op_num < op_array->try_catch_array[i].finally_op - 1 &&
				    (dst_num < op_array->try_catch_array[i].try_op ||
				     dst_num > op_array->try_catch_array[i].finally_end)) {
					
					opline = get_next_op(op_array TSRMLS_CC);
					opline->opcode = ZEND_FAST_CALL;
					SET_UNUSED(opline->op1);
					SET_UNUSED(opline->op2);
					opline->op1.opline_num = op_array->try_catch_array[i].finally_op;
				}
			}

			/* Finish the sequence with original opcode */
			opline = get_next_op(op_array TSRMLS_CC);
			*opline = op_array->opcodes[op_num];

			/* Replace original opcode with jump to this sequence */
			opline = op_array->opcodes + op_num;
			opline->opcode = ZEND_JMP;
			SET_UNUSED(opline->op1);
			SET_UNUSED(opline->op2);
			opline->op1.opline_num = start_op;

		    break;
		}
	}	
}

static void zend_resolve_finally_ret(zend_op_array *op_array, zend_uint op_num TSRMLS_DC)
{
	int i;
	zend_uint catch_op_num = 0, finally_op_num = 0;

	for (i = 0; i < op_array->last_try_catch; i++) {
		if (op_array->try_catch_array[i].try_op > op_num) {
			break;
		}
		if (op_num < op_array->try_catch_array[i].finally_op) {
			finally_op_num = op_array->try_catch_array[i].finally_op;
		}
		if (op_num < op_array->try_catch_array[i].catch_op) {
			catch_op_num = op_array->try_catch_array[i].catch_op;
		}
	}

	if (finally_op_num && (!catch_op_num || catch_op_num >= finally_op_num)) {
		/* in case of unhandled exception return to upward finally block */
		op_array->opcodes[op_num].extended_value = ZEND_FAST_RET_TO_FINALLY;
		op_array->opcodes[op_num].op2.opline_num = finally_op_num;
	} else if (catch_op_num) {
		/* in case of unhandled exception return to upward catch block */
		op_array->opcodes[op_num].extended_value = ZEND_FAST_RET_TO_CATCH;
		op_array->opcodes[op_num].op2.opline_num = catch_op_num;
	}
}

static void zend_resolve_finally_calls(zend_op_array *op_array TSRMLS_DC)
{
	zend_uint i;
	zend_op *opline;

	for (i = 0; i < op_array->last; i++) {
		opline = op_array->opcodes + i;
		switch (opline->opcode) {
			case ZEND_RETURN:
			case ZEND_RETURN_BY_REF:
			case ZEND_GENERATOR_RETURN:
				zend_resolve_finally_call(op_array, i, (zend_uint)-1 TSRMLS_CC);
				break;
			case ZEND_BRK:
			case ZEND_CONT:
			{
				int nest_levels, array_offset;
				zend_brk_cont_element *jmp_to;

				nest_levels = Z_LVAL(op_array->literals[opline->op2.constant].constant);
				array_offset = opline->op1.opline_num;
				do {
					jmp_to = &op_array->brk_cont_array[array_offset];
					if (nest_levels > 1) {
						array_offset = jmp_to->parent;
					}
				} while (--nest_levels > 0);
				zend_resolve_finally_call(op_array, i, opline->opcode == ZEND_BRK ? jmp_to->brk : jmp_to->cont TSRMLS_CC);
				break;
			}
			case ZEND_GOTO:
				if (Z_TYPE(op_array->literals[opline->op2.constant].constant) != IS_LONG) {
					zend_uint num = opline->op2.constant;
					opline->op2.zv = &op_array->literals[opline->op2.constant].constant;
					zend_resolve_goto_label(op_array, opline, 1 TSRMLS_CC);
					opline->op2.constant = num;					
				}
				/* break omitted intentionally */
			case ZEND_JMP:
				zend_resolve_finally_call(op_array, i, opline->op1.opline_num TSRMLS_CC);
				break;
			case ZEND_FAST_RET:
				zend_resolve_finally_ret(op_array, i TSRMLS_CC);
				break;
			default:
				break;
		}
	}
}

ZEND_API int pass_two(zend_op_array *op_array TSRMLS_DC)
{
	zend_op *opline, *end;

	if (op_array->type!=ZEND_USER_FUNCTION && op_array->type!=ZEND_EVAL_CODE) {
		return 0;
	}
	if (op_array->has_finally_block) {
		zend_resolve_finally_calls(op_array TSRMLS_CC);
	}
	if (CG(compiler_options) & ZEND_COMPILE_EXTENDED_INFO) {
		zend_update_extended_info(op_array TSRMLS_CC);
	}
	if (CG(compiler_options) & ZEND_COMPILE_HANDLE_OP_ARRAY) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_handler, op_array TSRMLS_CC);
	}

	if (!(op_array->fn_flags & ZEND_ACC_INTERACTIVE) && CG(context).vars_size != op_array->last_var) {
		op_array->vars = (zend_compiled_variable *) erealloc(op_array->vars, sizeof(zend_compiled_variable)*op_array->last_var);
		CG(context).vars_size = op_array->last_var;
	}
	if (!(op_array->fn_flags & ZEND_ACC_INTERACTIVE) && CG(context).opcodes_size != op_array->last) {
		op_array->opcodes = (zend_op *) erealloc(op_array->opcodes, sizeof(zend_op)*op_array->last);
		CG(context).opcodes_size = op_array->last;
	}
	if (!(op_array->fn_flags & ZEND_ACC_INTERACTIVE) && CG(context).literals_size != op_array->last_literal) {
		op_array->literals = (zend_literal*)erealloc(op_array->literals, sizeof(zend_literal) * op_array->last_literal);
		CG(context).literals_size = op_array->last_literal;
	}

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		if (opline->op1_type == IS_CONST) {
			opline->op1.zv = &op_array->literals[opline->op1.constant].constant;
		}
		if (opline->op2_type == IS_CONST) {
			opline->op2.zv = &op_array->literals[opline->op2.constant].constant;
		}
		switch (opline->opcode) {
			case ZEND_GOTO:
				if (Z_TYPE_P(opline->op2.zv) != IS_LONG) {
					zend_resolve_goto_label(op_array, opline, 1 TSRMLS_CC);
				}
				/* break omitted intentionally */
			case ZEND_JMP:
			case ZEND_FAST_CALL:
				opline->op1.jmp_addr = &op_array->opcodes[opline->op1.opline_num];
				break;
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_JMP_SET:
			case ZEND_JMP_SET_VAR:
				opline->op2.jmp_addr = &op_array->opcodes[opline->op2.opline_num];
				break;
			case ZEND_RETURN:
			case ZEND_RETURN_BY_REF:
				if (op_array->fn_flags & ZEND_ACC_GENERATOR) {
					if (opline->op1_type != IS_CONST || Z_TYPE_P(opline->op1.zv) != IS_NULL) {
						CG(zend_lineno) = opline->lineno;
						zend_error(E_COMPILE_ERROR, "Generators cannot return values using \"return\"");
					}

					opline->opcode = ZEND_GENERATOR_RETURN;
				}
				break;
		}
		ZEND_VM_SET_OPCODE_HANDLER(opline);
		opline++;
	}

	op_array->fn_flags |= ZEND_ACC_DONE_PASS_TWO;
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
	switch (opcode) {
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

ZEND_API binary_op_type get_binary_op(int opcode)
{
	switch (opcode) {
		case ZEND_ADD:
		case ZEND_ASSIGN_ADD:
			return (binary_op_type) add_function;
			break;
		case ZEND_SUB:
		case ZEND_ASSIGN_SUB:
			return (binary_op_type) sub_function;
			break;
		case ZEND_MUL:
		case ZEND_ASSIGN_MUL:
			return (binary_op_type) mul_function;
			break;
		case ZEND_DIV:
		case ZEND_ASSIGN_DIV:
			return (binary_op_type) div_function;
			break;
		case ZEND_MOD:
		case ZEND_ASSIGN_MOD:
			return (binary_op_type) mod_function;
			break;
		case ZEND_SL:
		case ZEND_ASSIGN_SL:
			return (binary_op_type) shift_left_function;
			break;
		case ZEND_SR:
		case ZEND_ASSIGN_SR:
			return (binary_op_type) shift_right_function;
			break;
		case ZEND_CONCAT:
		case ZEND_ASSIGN_CONCAT:
			return (binary_op_type) concat_function;
			break;
		case ZEND_IS_IDENTICAL:
			return (binary_op_type) is_identical_function;
			break;
		case ZEND_IS_NOT_IDENTICAL:
			return (binary_op_type) is_not_identical_function;
			break;
		case ZEND_IS_EQUAL:
			return (binary_op_type) is_equal_function;
			break;
		case ZEND_IS_NOT_EQUAL:
			return (binary_op_type) is_not_equal_function;
			break;
		case ZEND_IS_SMALLER:
			return (binary_op_type) is_smaller_function;
			break;
		case ZEND_IS_SMALLER_OR_EQUAL:
			return (binary_op_type) is_smaller_or_equal_function;
			break;
		case ZEND_BW_OR:
		case ZEND_ASSIGN_BW_OR:
			return (binary_op_type) bitwise_or_function;
			break;
		case ZEND_BW_AND:
		case ZEND_ASSIGN_BW_AND:
			return (binary_op_type) bitwise_and_function;
			break;
		case ZEND_BW_XOR:
		case ZEND_ASSIGN_BW_XOR:
			return (binary_op_type) bitwise_xor_function;
			break;
		case ZEND_BOOL_XOR:
			return (binary_op_type) boolean_xor_function;
			break;
		default:
			return (binary_op_type) NULL;
			break;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
