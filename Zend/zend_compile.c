/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
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

/* $Id$ */

#include "zend_language_parser.h"
#include "zend.h"
#include "zend_compile.h"
#include "zend_llist.h"
#include "zend_API.h"
#include "zend_fast_cache.h"


ZEND_API zend_op_array *(*zend_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);


#ifndef ZTS
ZEND_API zend_compiler_globals compiler_globals;
ZEND_API zend_executor_globals executor_globals;
#endif

static void build_runtime_defined_function_key(zval *result, char *name, int name_length, zend_op *opline TSRMLS_DC)
{
	char lineno_buf[32];
	uint lineno_len;
	char *filename;

	lineno_len = zend_sprintf(lineno_buf, "%d", opline->lineno);
	if (CG(active_op_array)->filename) {
		filename = CG(active_op_array)->filename;
	} else {
		filename = "-";
	}

	/* NULL, name length, filename length, line number length */
	result->value.str.len = 1+name_length+strlen(filename)+lineno_len;
	result->value.str.val = (char *) emalloc(result->value.str.len+1);
	sprintf(result->value.str.val, "%c%s%s%s", '\0', name, filename, lineno_buf);
	result->type = IS_STRING;
	result->refcount = 1;
}


static void init_compiler_declarables(TSRMLS_D)
{
	CG(declarables).ticks.type = IS_LONG;
	CG(declarables).ticks.value.lval = 0;
}



void zend_init_compiler_data_structures(TSRMLS_D)
{
	zend_stack_init(&CG(bp_stack));
	zend_stack_init(&CG(function_call_stack));
	zend_stack_init(&CG(switch_cond_stack));
	zend_stack_init(&CG(foreach_copy_stack));
	zend_stack_init(&CG(object_stack));
	zend_stack_init(&CG(declare_stack));
	CG(active_class_entry) = NULL;
	CG(active_ce_parent_class_name).value.str.val = NULL;
	zend_llist_init(&CG(list_llist), sizeof(list_llist_element), NULL, 0);
	zend_llist_init(&CG(dimension_llist), sizeof(int), NULL, 0);
	zend_stack_init(&CG(list_stack));
	CG(handle_op_arrays) = 1;
	CG(in_compilation) = 0;
	CG(start_lineno) = 0;
	init_compiler_declarables(TSRMLS_C);
	CG(throw_list) = NULL;
 	zend_register_auto_global("GLOBALS", sizeof("GLOBALS")-1 TSRMLS_CC);
	CG(in_clone_method) = 0;
}


void init_compiler(TSRMLS_D)
{
	zend_init_compiler_data_structures(TSRMLS_C);
	zend_init_rsrc_list(TSRMLS_C);
	zend_hash_init(&CG(filenames_table), 5, NULL, (dtor_func_t) free_estring, 0);
	zend_llist_init(&CG(open_files), sizeof(zend_file_handle), (void (*)(void *)) zend_file_handle_dtor, 0);
	CG(unclean_shutdown) = 0;
}


void shutdown_compiler(TSRMLS_D)
{
	zend_stack_destroy(&CG(bp_stack));
	zend_stack_destroy(&CG(function_call_stack));
	zend_stack_destroy(&CG(switch_cond_stack));
	zend_stack_destroy(&CG(foreach_copy_stack));
	zend_stack_destroy(&CG(object_stack));
	zend_stack_destroy(&CG(declare_stack));
	zend_stack_destroy(&CG(list_stack));
	zend_hash_destroy(&CG(filenames_table));
	zend_llist_destroy(&CG(open_files));
}


ZEND_API char *zend_set_compiled_filename(char *new_compiled_filename TSRMLS_DC)
{
	char **pp, *p;
	int length = strlen(new_compiled_filename);

	if (zend_hash_find(&CG(filenames_table), new_compiled_filename, length+1, (void **) &pp)==SUCCESS) {
		CG(compiled_filename) = *pp;
		return *pp;
	}
	p = estrndup(new_compiled_filename, length);
	zend_hash_update(&CG(filenames_table), new_compiled_filename, length+1, &p, sizeof(char *), (void **) &pp);
	CG(compiled_filename) = p;
	return p;
}


ZEND_API void zend_restore_compiled_filename(char *original_compiled_filename TSRMLS_DC)
{
	CG(compiled_filename) = original_compiled_filename;
}


ZEND_API char *zend_get_compiled_filename(TSRMLS_D)
{
	return CG(compiled_filename);
}


ZEND_API int zend_get_compiled_lineno(TSRMLS_D)
{
	return CG(zend_lineno);
}


ZEND_API zend_bool zend_is_compiling(TSRMLS_D)
{
	return CG(in_compilation);
}


static zend_uint get_temporary_variable(zend_op_array *op_array)
{
	return (op_array->T)++ * sizeof(temp_variable);
}


void zend_do_binary_op(zend_uchar op, znode *result, znode *op1, znode *op2 TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = op;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *op1;
	opline->op2 = *op2;
	*result = opline->result;
}


void zend_do_unary_op(zend_uchar op, znode *result, znode *op1 TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = op;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *op1;
	*result = opline->result;
	SET_UNUSED(opline->op2);
}

#define MAKE_NOP(opline)	{ opline->opcode = ZEND_NOP;  memset(&opline->result,0,sizeof(znode)); memset(&opline->op1,0,sizeof(znode)); memset(&opline->op2,0,sizeof(znode)); opline->result.op_type=opline->op1.op_type=opline->op2.op_type=IS_UNUSED;  }


static void zend_do_op_data(zend_op *data_op, znode *value TSRMLS_DC)
{
	data_op->opcode = ZEND_OP_DATA;
	data_op->op1 = *value;
	SET_UNUSED(data_op->op2);
}

void zend_do_binary_assign_op(zend_uchar op, znode *result, znode *op1, znode *op2 TSRMLS_DC)
{
	int last_op_number = get_next_op_number(CG(active_op_array))-1;
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_op *last_op = &CG(active_op_array)->opcodes[last_op_number];
	
	if (last_op->opcode == ZEND_FETCH_OBJ_RW) {
		switch (op) {
			case ZEND_ASSIGN_ADD:
				last_op->opcode = ZEND_ASSIGN_ADD_OBJ;
				break;
			case ZEND_ASSIGN_SUB:
				last_op->opcode = ZEND_ASSIGN_SUB_OBJ;
				break;
			case ZEND_ASSIGN_MUL:
				last_op->opcode = ZEND_ASSIGN_MUL_OBJ;
				break;
			case ZEND_ASSIGN_DIV:
				last_op->opcode = ZEND_ASSIGN_DIV_OBJ;
				break;
			case ZEND_ASSIGN_MOD:
				last_op->opcode = ZEND_ASSIGN_MOD_OBJ;
				break;
			case ZEND_ASSIGN_SL:
				last_op->opcode = ZEND_ASSIGN_SL_OBJ;
				break;
			case ZEND_ASSIGN_SR:
				last_op->opcode = ZEND_ASSIGN_SR_OBJ;
				break;
			case ZEND_ASSIGN_CONCAT:
				last_op->opcode = ZEND_ASSIGN_CONCAT_OBJ;
				break;
			case ZEND_ASSIGN_BW_OR:
				last_op->opcode = ZEND_ASSIGN_BW_OR_OBJ;
				break;
			case ZEND_ASSIGN_BW_AND:
				last_op->opcode = ZEND_ASSIGN_BW_AND_OBJ;
				break;
			case ZEND_ASSIGN_BW_XOR:
				last_op->opcode = ZEND_ASSIGN_BW_XOR_OBJ;
				break;
			default:
				zend_error(E_COMPILE_ERROR, "Unknown binary op opcode %d", op);
		}

		zend_do_op_data(opline, op2 TSRMLS_CC);
		SET_UNUSED(opline->result);
		*result = last_op->result;
	} else {
		opline->opcode = op;
		opline->op1 = *op1;
		opline->op2 = *op2;
		opline->result.op_type = IS_VAR;
		opline->result.u.EA.type = 0;
		opline->result.u.var = get_temporary_variable(CG(active_op_array));
		*result = opline->result;
	}
}


void fetch_simple_variable_ex(znode *result, znode *varname, int bp, zend_uchar op TSRMLS_DC)
{
	zend_op opline;
	zend_op *opline_ptr;
	zend_llist *fetch_list_ptr;

	if (bp) {
		opline_ptr = &opline;
		init_op(opline_ptr TSRMLS_CC);
	} else {
		opline_ptr = get_next_op(CG(active_op_array) TSRMLS_CC);
	}

	opline_ptr->opcode = op;
	opline_ptr->result.op_type = IS_VAR;
	opline_ptr->result.u.EA.type = 0;
	opline_ptr->result.u.var = get_temporary_variable(CG(active_op_array));
	opline_ptr->op1 = *varname;
	*result = opline_ptr->result;
	SET_UNUSED(opline_ptr->op2);

	opline_ptr->op2.u.EA.type = ZEND_FETCH_LOCAL;
	if (varname->op_type == IS_CONST && varname->u.constant.type == IS_STRING) {
		if (zend_hash_exists(CG(auto_globals), varname->u.constant.value.str.val, varname->u.constant.value.str.len+1)) {
			opline_ptr->op2.u.EA.type = ZEND_FETCH_GLOBAL;
		} else {
/*			if (CG(active_op_array)->static_variables && zend_hash_exists(CG(active_op_array)->static_variables, varname->u.constant.value.str.val, varname->u.constant.value.str.len+1)) {
				opline_ptr->op2.u.EA.type = ZEND_FETCH_STATIC;
			} 
*/		}
	}

	if (bp) {
		zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);
		zend_llist_add_element(fetch_list_ptr, opline_ptr);
	}
}

void fetch_simple_variable(znode *result, znode *varname, int bp TSRMLS_DC)
{
	/* the default mode must be Write, since fetch_simple_variable() is used to define function arguments */
	fetch_simple_variable_ex(result, varname, bp, ZEND_FETCH_W TSRMLS_CC);
}

void zend_do_fetch_static_member(znode *class_znode TSRMLS_DC)
{
	zend_llist *fetch_list_ptr;
	zend_llist_element *le;
	zend_op *opline_ptr;

	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);
	le = fetch_list_ptr->head;

	opline_ptr = (zend_op *)le->data;
	opline_ptr->op2 = *class_znode;
	opline_ptr->op2.u.EA.type = ZEND_FETCH_STATIC_MEMBER;
}

void fetch_array_begin(znode *result, znode *varname, znode *first_dim TSRMLS_DC)
{
	fetch_simple_variable(result, varname, 1 TSRMLS_CC);

	fetch_array_dim(result, result, first_dim TSRMLS_CC);
}


void fetch_array_dim(znode *result, znode *parent, znode *dim TSRMLS_DC)
{
	zend_op opline;
	zend_llist *fetch_list_ptr;

	init_op(&opline TSRMLS_CC);
	opline.opcode = ZEND_FETCH_DIM_W;	/* the backpatching routine assumes W */
	opline.result.op_type = IS_VAR;
	opline.result.u.EA.type = 0;
	opline.result.u.var = get_temporary_variable(CG(active_op_array));
	opline.op1 = *parent;
	opline.op2 = *dim;
	opline.extended_value = ZEND_FETCH_STANDARD;
	*result = opline.result;

	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);
	zend_llist_add_element(fetch_list_ptr, &opline);
}


void fetch_string_offset(znode *result, znode *parent, znode *offset TSRMLS_DC)
{
	fetch_array_dim(result, parent, offset TSRMLS_CC);
}


void zend_do_print(znode *result, znode *arg TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->opcode = ZEND_PRINT;
	opline->op1 = *arg;
	SET_UNUSED(opline->op2);
	*result = opline->result;
}


void zend_do_echo(znode *arg TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_ECHO;
	opline->op1 = *arg;
	SET_UNUSED(opline->op2);
}

void zend_do_abstract_method(TSRMLS_D)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_RAISE_ABSTRACT_ERROR;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void zend_do_assign(znode *result, znode *variable, znode *value TSRMLS_DC)
{
	int last_op_number = get_next_op_number(CG(active_op_array))-1;
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_op *last_op = &CG(active_op_array)->opcodes[last_op_number];

	if (last_op->opcode == ZEND_FETCH_OBJ_W) {
		last_op->opcode = ZEND_ASSIGN_OBJ;

		zend_do_op_data(opline, value TSRMLS_CC);
		SET_UNUSED(opline->result);
		*result = last_op->result;
	} else {
		opline->opcode = ZEND_ASSIGN;
		opline->op1 = *variable;
		opline->op2 = *value;
		opline->result.op_type = IS_VAR;
		opline->result.u.EA.type = 0;
		opline->result.u.var = get_temporary_variable(CG(active_op_array));
		*result = opline->result;
	}
}


void zend_do_assign_ref(znode *result, znode *lvar, znode *rvar TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_ASSIGN_REF;
	if (result) {
		opline->result.op_type = IS_VAR;
		opline->result.u.EA.type = 0;
		opline->result.u.var = get_temporary_variable(CG(active_op_array));
		*result = opline->result;
	} else {
		/* SET_UNUSED(opline->result); */
		opline->result.u.EA.type |= EXT_TYPE_UNUSED;
	}
	opline->op1 = *lvar;
	opline->op2 = *rvar;
}


static inline void do_begin_loop(TSRMLS_D)
{
	zend_brk_cont_element *brk_cont_element;
	int parent;

	parent = CG(active_op_array)->current_brk_cont;
	CG(active_op_array)->current_brk_cont = CG(active_op_array)->last_brk_cont;
	brk_cont_element = get_next_brk_cont_element(CG(active_op_array));
	brk_cont_element->parent = parent;
}


static inline void do_end_loop(int cont_addr TSRMLS_DC)
{
	CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].cont = cont_addr;
	CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].brk = get_next_op_number(CG(active_op_array));
	CG(active_op_array)->current_brk_cont = CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].parent;
}


void zend_do_while_cond(znode *expr, znode *close_bracket_token TSRMLS_DC)
{
	int while_cond_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPZ;
	opline->op1 = *expr;
	close_bracket_token->u.opline_num = while_cond_op_number;
	SET_UNUSED(opline->op2);

	do_begin_loop(TSRMLS_C);
	INC_BPC(CG(active_op_array));
}


void zend_do_while_end(znode *while_token, znode *close_bracket_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	/* add unconditional jump */
	opline->opcode = ZEND_JMP;
	opline->op1.u.opline_num = while_token->u.opline_num;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	
	/* update while's conditional jmp */
	CG(active_op_array)->opcodes[close_bracket_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));

	do_end_loop(while_token->u.opline_num TSRMLS_CC);

	DEC_BPC(CG(active_op_array));
}


void zend_do_for_cond(znode *expr, znode *second_semicolon_token TSRMLS_DC)
{
	int for_cond_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPZNZ;
	opline->op1 = *expr;  /* the conditional expression */
	second_semicolon_token->u.opline_num = for_cond_op_number;
	SET_UNUSED(opline->op2);
}


void zend_do_for_before_statement(znode *cond_start, znode *second_semicolon_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMP;
	opline->op1.u.opline_num = cond_start->u.opline_num;
	CG(active_op_array)->opcodes[second_semicolon_token->u.opline_num].extended_value = get_next_op_number(CG(active_op_array));
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	do_begin_loop(TSRMLS_C);

	INC_BPC(CG(active_op_array));
}


void zend_do_for_end(znode *second_semicolon_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMP;
	opline->op1.u.opline_num = second_semicolon_token->u.opline_num+1;
	CG(active_op_array)->opcodes[second_semicolon_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	do_end_loop(second_semicolon_token->u.opline_num+1 TSRMLS_CC);

	DEC_BPC(CG(active_op_array));
}


void zend_do_pre_incdec(znode *result, znode *op1, zend_uchar op TSRMLS_DC)
{
	int last_op_number = get_next_op_number(CG(active_op_array))-1;
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_op *last_op = &CG(active_op_array)->opcodes[last_op_number];

	if (last_op->opcode == ZEND_FETCH_OBJ_RW) {
		opline->opcode = (op==ZEND_PRE_INC)?ZEND_PRE_INC_OBJ:ZEND_PRE_DEC_OBJ;
		opline->op1 = last_op->op1;
		opline->op2 = last_op->op2;
		
		MAKE_NOP(last_op);
	} else {
		opline->opcode = op;
		opline->op1 = *op1;
		SET_UNUSED(opline->op2);
	}

	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	*result = opline->result;
}


void zend_do_post_incdec(znode *result, znode *op1, zend_uchar op TSRMLS_DC)
{
	int last_op_number = get_next_op_number(CG(active_op_array))-1;
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_op *last_op = &CG(active_op_array)->opcodes[last_op_number];

	if (last_op->opcode == ZEND_FETCH_OBJ_RW) {
		opline->opcode = (op==ZEND_POST_INC)?ZEND_POST_INC_OBJ:ZEND_POST_DEC_OBJ;
		opline->op1 = last_op->op1;
		opline->op2 = last_op->op2;
		
		MAKE_NOP(last_op);
	} else {
		opline->opcode = op;
		opline->op1 = *op1;
		SET_UNUSED(opline->op2);
	}
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	*result = opline->result;
}


void zend_do_if_cond(znode *cond, znode *closing_bracket_token TSRMLS_DC)
{
	int if_cond_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPZ;
	opline->op1 = *cond;
	closing_bracket_token->u.opline_num = if_cond_op_number;
	SET_UNUSED(opline->op2);
	INC_BPC(CG(active_op_array));
}


void zend_do_if_after_statement(znode *closing_bracket_token, unsigned char initialize TSRMLS_DC)
{
	int if_end_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_llist *jmp_list_ptr;

	opline->opcode = ZEND_JMP;
	/* save for backpatching */
	if (initialize) {
		zend_llist jmp_list;

		zend_llist_init(&jmp_list, sizeof(int), NULL, 0);
		zend_stack_push(&CG(bp_stack), (void *) &jmp_list, sizeof(zend_llist));
	}
	zend_stack_top(&CG(bp_stack), (void **) &jmp_list_ptr);
	zend_llist_add_element(jmp_list_ptr, &if_end_op_number);
	
	CG(active_op_array)->opcodes[closing_bracket_token->u.opline_num].op2.u.opline_num = if_end_op_number+1;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void zend_do_if_end(TSRMLS_D)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_llist *jmp_list_ptr;
	zend_llist_element *le;

	zend_stack_top(&CG(bp_stack), (void **) &jmp_list_ptr);
	for (le=jmp_list_ptr->head; le; le = le->next) {
		CG(active_op_array)->opcodes[*((int *) le->data)].op1.u.opline_num = next_op_number;
	}
	zend_llist_destroy(jmp_list_ptr);
	zend_stack_del_top(&CG(bp_stack));
	DEC_BPC(CG(active_op_array));
}

#if 0
/* variable parsing type (compile-time) */
#define ZEND_PARSED_MEMBER			(1<<0)
#define ZEND_PARSED_METHOD_CALL		(1<<1)
#define ZEND_PARSED_STATIC_MEMBER	(1<<2)
#define ZEND_PARSED_FUNCTION_CALL	(1<<3)
#define ZEND_PARSED_VARIABLE		(1<<4)

#endif

void zend_check_writable_variable(znode *variable)
{
	zend_uint type = variable->u.EA.type;
	
	if (type & ZEND_PARSED_METHOD_CALL) {
		zend_error(E_COMPILE_ERROR, "Can't use method return value in write context");
	}
	if (type == ZEND_PARSED_FUNCTION_CALL) {
		zend_error(E_COMPILE_ERROR, "Can't use function return value in write context");
	}
}

zend_bool zend_is_function_or_method_call(znode *variable)
{
	zend_uint type = variable->u.EA.type;

	return  ((type & ZEND_PARSED_METHOD_CALL) || (type == ZEND_PARSED_FUNCTION_CALL));
}

void zend_do_begin_import(TSRMLS_D)
{
	zend_llist_init(&CG(import_commands), sizeof(zend_op), NULL, 0);
}

void zend_do_import(int type, znode *what TSRMLS_DC)
{
	zend_op opline;

	init_op(&opline TSRMLS_CC);

	switch (type) {
		case T_FUNCTION:
			opline.opcode = ZEND_IMPORT_FUNCTION;
			break;
		case T_CLASS:
			opline.opcode = ZEND_IMPORT_CLASS;
			break;
		case T_CONST:
			opline.opcode = ZEND_IMPORT_CONST;
			break;
	}

	if (what) {
		if (type == T_FUNCTION || type == T_CLASS) {
			zend_str_tolower(what->u.constant.value.str.val, what->u.constant.value.str.len);
		}
		opline.op2 = *what;
	} else {
		SET_UNUSED(opline.op2);
	}
	
	zend_llist_add_element(&CG(import_commands), &opline);
}

void zend_do_end_import(znode *import_from TSRMLS_DC)
{
	zend_llist_element *le;
	zend_op *opline, *opline_ptr;

	
	le = CG(import_commands).head;

	while (le) {
		opline_ptr = (zend_op *)le->data;
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		memcpy(opline, opline_ptr, sizeof(zend_op));
		opline->op1 = *import_from;
		le = le->next;
	}
	zend_llist_destroy(&CG(import_commands));
}



void zend_do_begin_variable_parse(TSRMLS_D)
{
	zend_llist fetch_list;

	zend_llist_init(&fetch_list, sizeof(zend_op), NULL, 0);
	zend_stack_push(&CG(bp_stack), (void *) &fetch_list, sizeof(zend_llist));
}


void zend_do_end_variable_parse(int type, int arg_offset TSRMLS_DC)
{
	zend_llist *fetch_list_ptr;
	zend_llist_element *le;
	zend_op *opline, *opline_ptr=NULL;
	int num_of_created_opcodes = 0;

	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);

	le = fetch_list_ptr->head;

	while (le) {
		opline_ptr = (zend_op *)le->data;
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		memcpy(opline, opline_ptr, sizeof(zend_op));
		switch (type) {
			case BP_VAR_R:
				if (opline->opcode == ZEND_FETCH_DIM_W && opline->op2.op_type == IS_UNUSED) {
					zend_error(E_COMPILE_ERROR, "Cannot use [] for reading");
				}
				opline->opcode -= 3;
				break;
			case BP_VAR_W:
				break;
			case BP_VAR_RW:
				opline->opcode += 3;
				break;
			case BP_VAR_IS:
				opline->opcode += 6; /* 3+3 */
				break;
			case BP_VAR_FUNC_ARG:
				opline->opcode += 9; /* 3+3+3 */
				opline->extended_value = arg_offset;
				break;
			case BP_VAR_UNSET:
				if (opline->opcode == ZEND_FETCH_DIM_W && opline->op2.op_type == IS_UNUSED) {
					zend_error(E_COMPILE_ERROR, "Cannot use [] for unsetting");
				}
				opline->opcode += 12; /* 3+3+3+3 */
				break;
		}
		le = le->next;
		num_of_created_opcodes++;
	}

	if (num_of_created_opcodes == 1) {
		if ((opline_ptr->op1.op_type == IS_CONST) && (opline_ptr->op1.u.constant.type == IS_STRING) &&
		(opline_ptr->op1.u.constant.value.str.len == (sizeof("this")-1)) &&
		!memcmp(opline_ptr->op1.u.constant.value.str.val, "this", sizeof("this"))) {
			CG(active_op_array)->uses_this = 1;
		}
	}

	zend_llist_destroy(fetch_list_ptr);
	zend_stack_del_top(&CG(bp_stack));
}


void zend_do_init_string(znode *result TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_INIT_STRING;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	*result = opline->result;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void zend_do_add_char(znode *result, znode *op1, znode *op2 TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_ADD_CHAR;
	opline->op1 = *op1;
	opline->op2 = *op2;
	opline->op2.op_type = IS_CONST;
	opline->result = opline->op1;
	*result = opline->result;
}


void zend_do_add_string(znode *result, znode *op1, znode *op2 TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_ADD_STRING;
	opline->op1 = *op1;
	opline->op2 = *op2;
	opline->op2.op_type = IS_CONST;
	opline->result = opline->op1;
	*result = opline->result;
}


void zend_do_add_variable(znode *result, znode *op1, znode *op2 TSRMLS_DC)
{
	zend_op *opline;

	if (op1->op_type == IS_CONST) {
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_INIT_STRING;
		opline->result.op_type = IS_TMP_VAR;
		opline->result.u.var = get_temporary_variable(CG(active_op_array));
		*result = opline->result;
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);

		if (op1->u.constant.value.str.len>0) {
			opline = get_next_op(CG(active_op_array) TSRMLS_CC);
			opline->opcode = ZEND_ADD_STRING;
			opline->result = *result;
			opline->op1 = *result;
			opline->op2 = *op1;
			opline->result = opline->op1;
		} else {
			zval_dtor(&op1->u.constant);
		}
	} else {
		*result = *op1;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_ADD_VAR;
	opline->result = *result;
	opline->op1 = *result;
	opline->op2 = *op2;
	*result = opline->result;
}

static void zend_lowercase_znode_if_const(znode *z)
{
	if (z->op_type == IS_CONST) {
		zend_str_tolower(z->u.constant.value.str.val, z->u.constant.value.str.len);
	}
}

void zend_do_free(znode *op1 TSRMLS_DC)
{
	if (op1->op_type==IS_TMP_VAR) {
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

		opline->opcode = ZEND_FREE;
		opline->op1 = *op1;
		SET_UNUSED(opline->op2);
	} else if (op1->op_type==IS_VAR) {
		zend_op *opline = &CG(active_op_array)->opcodes[CG(active_op_array)->last-1];

		while (opline->opcode == ZEND_END_SILENCE || opline->opcode == ZEND_EXT_FCALL_END || opline->opcode == ZEND_OP_DATA) {
			opline--;
		}
		if (opline->result.op_type == op1->op_type
			&& opline->result.u.var == op1->u.var) {
			opline->result.u.EA.type |= EXT_TYPE_UNUSED;
		} else {
			while (opline>CG(active_op_array)->opcodes) {
				/* This should be an object instantiation
				 * Find JMP_NO_CTOR, mark the preceding ASSIGN and the
				 * proceeding INIT_FCALL_BY_NAME as unused
				 */
				if (opline->opcode == ZEND_JMP_NO_CTOR) {
					(opline-1)->result.u.EA.type |= EXT_TYPE_UNUSED;
					(opline+1)->op1.u.EA.type |= EXT_TYPE_UNUSED;
					break;
				} else if (opline->opcode == ZEND_FETCH_DIM_R
							&& opline->op1.op_type == IS_VAR
							&& opline->op1.u.var == op1->u.var) {
					/* This should the end of a list() construct
					 * Mark its result as unused
					 */
					opline->extended_value = ZEND_FETCH_STANDARD;
					break;
				} else if (opline->result.op_type==IS_VAR
					&& opline->result.u.var == op1->u.var) {
					break;
				}
				opline--;
			}
		}
	} else if (op1->op_type == IS_CONST) {
		zval_dtor(&op1->u.constant);
	}		
}


int zend_do_verify_access_types(znode *current_access_type, znode *new_modifier)
{
	if ((new_modifier->u.constant.value.lval & ZEND_ACC_PPP_MASK)
		&& ((current_access_type->u.constant.value.lval & ZEND_ACC_PPP_MASK) != (new_modifier->u.constant.value.lval & ZEND_ACC_PPP_MASK))) {
		zend_error(E_COMPILE_ERROR, "Multiple access type modifiers are not allowed");
	}
	return (current_access_type->u.constant.value.lval | new_modifier->u.constant.value.lval);
}


void zend_do_begin_function_declaration(znode *function_token, znode *function_name, int is_method, int return_reference, zend_uint fn_flags  TSRMLS_DC)
{
	zend_op_array op_array;
	char *name = function_name->u.constant.value.str.val;
	int name_len = function_name->u.constant.value.str.len;
	int function_begin_line = function_token->u.opline_num;

	function_token->u.op_array = CG(active_op_array);
	zend_str_tolower(name, name_len);

	init_op_array(&op_array, ZEND_USER_FUNCTION, INITIAL_OP_ARRAY_SIZE TSRMLS_CC);

	op_array.function_name = name;
	op_array.arg_types = NULL;
	op_array.return_reference = return_reference;
	op_array.fn_flags = fn_flags;

	op_array.scope = CG(active_class_entry);

	if (is_method) {
		char *short_class_name = CG(active_class_entry)->name;
		int short_class_name_length = CG(active_class_entry)->name_length;
		zend_uint i;

		for (i=0; i < CG(active_class_entry)->name_length; i++) {
			if (CG(active_class_entry)->name[i] == ':') {
				short_class_name = &CG(active_class_entry)->name[i+1];
				short_class_name_length = strlen(short_class_name);
			}
		}
		if (zend_hash_add(&CG(active_class_entry)->function_table, name, name_len+1, &op_array, sizeof(zend_op_array), (void **) &CG(active_op_array)) == FAILURE) {
			zend_op_array *child_op_array, *parent_op_array;
			if (CG(active_class_entry)->parent
					&& (zend_hash_find(&CG(active_class_entry)->function_table, name, name_len+1, (void **) &child_op_array) == SUCCESS)
					&& (zend_hash_find(&CG(active_class_entry)->parent->function_table, name, name_len+1, (void **) &parent_op_array) == SUCCESS)
					&& (child_op_array == parent_op_array)) {
				zend_hash_update(&CG(active_class_entry)->function_table, name, name_len+1, &op_array, sizeof(zend_op_array), (void **) &CG(active_op_array));
			} else {
				zend_error(E_COMPILE_ERROR, "Cannot redeclare %s()", name);
			}
		}
		if (fn_flags & ZEND_ACC_ABSTRACT) {
			CG(active_class_entry)->ce_flags |= ZEND_ACC_ABSTRACT;
		}

		if ((short_class_name_length == name_len) && (!memcmp(short_class_name, name, name_len))) {
			CG(active_class_entry)->constructor = (zend_function *) CG(active_op_array);
		} else if ((function_name->u.constant.value.str.len == sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1) && (!memcmp(function_name->u.constant.value.str.val, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)))) {
			CG(active_class_entry)->constructor = (zend_function *) CG(active_op_array);
		} else if ((function_name->u.constant.value.str.len == sizeof(ZEND_DESTRUCTOR_FUNC_NAME)-1) && (!memcmp(function_name->u.constant.value.str.val, ZEND_DESTRUCTOR_FUNC_NAME, sizeof(ZEND_DESTRUCTOR_FUNC_NAME)))) {
			CG(active_class_entry)->destructor = (zend_function *) CG(active_op_array);
		} else if ((function_name->u.constant.value.str.len == sizeof(ZEND_CLONE_FUNC_NAME)-1) && (!memcmp(function_name->u.constant.value.str.val, ZEND_CLONE_FUNC_NAME, sizeof(ZEND_CLONE_FUNC_NAME)))) {
			CG(active_class_entry)->clone = (zend_function *) CG(active_op_array);
			CG(in_clone_method) = 1;
		} else if ((function_name->u.constant.value.str.len == sizeof(ZEND_CALL_FUNC_NAME)-1) && (!memcmp(function_name->u.constant.value.str.val, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME)))) {
			CG(active_class_entry)->__call = (zend_function *) CG(active_op_array);
		} else if ((function_name->u.constant.value.str.len == sizeof(ZEND_GET_FUNC_NAME)-1) && (!memcmp(function_name->u.constant.value.str.val, ZEND_GET_FUNC_NAME, sizeof(ZEND_GET_FUNC_NAME)))) {
			CG(active_class_entry)->__get = (zend_function *) CG(active_op_array);
		} else if ((function_name->u.constant.value.str.len == sizeof(ZEND_SET_FUNC_NAME)-1) && (!memcmp(function_name->u.constant.value.str.val, ZEND_SET_FUNC_NAME, sizeof(ZEND_SET_FUNC_NAME)))) {
			CG(active_class_entry)->__set = (zend_function *) CG(active_op_array);
		}
	} else {
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

		opline->opcode = ZEND_DECLARE_FUNCTION;
		opline->op1.op_type = IS_CONST;
		build_runtime_defined_function_key(&opline->op1.u.constant, function_name->u.constant.value.str.val, function_name->u.constant.value.str.len, opline TSRMLS_CC);
		opline->op2.op_type = IS_CONST;
		opline->op2.u.constant.type = IS_STRING;
		opline->op2.u.constant.value.str.val = estrndup(name, name_len);
		opline->op2.u.constant.value.str.len = name_len;
		opline->op2.u.constant.refcount = 1;
		opline->extended_value = ZEND_DECLARE_FUNCTION;
		zend_hash_update(CG(function_table), opline->op1.u.constant.value.str.val, opline->op1.u.constant.value.str.len, &op_array, sizeof(zend_op_array), (void **) &CG(active_op_array));
	}

	if (CG(extended_info)) {
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

		opline->opcode = ZEND_EXT_NOP;
		opline->lineno = function_begin_line;
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);
	}
	
	{
		/* Push a seperator to the switch and foreach stacks */
		zend_switch_entry switch_entry;
	
		switch_entry.cond.op_type = IS_UNUSED;
		switch_entry.default_case = 0;
		switch_entry.control_var = 0;

		zend_stack_push(&CG(switch_cond_stack), (void *) &switch_entry, sizeof(switch_entry));

		zend_stack_push(&CG(foreach_copy_stack), (void *) &switch_entry.cond, sizeof(znode));
	}
	function_token->throw_list = CG(throw_list);
	CG(throw_list) = NULL;	
}


void zend_do_end_function_declaration(znode *function_token TSRMLS_DC)
{
	zend_do_extended_info(TSRMLS_C);
	zend_do_return(NULL, 0 TSRMLS_CC);
	pass_two(CG(active_op_array) TSRMLS_CC);
	CG(active_op_array) = function_token->u.op_array;

	/* Pop the switch and foreach seperators */
	zend_stack_del_top(&CG(switch_cond_stack));
	zend_stack_del_top(&CG(foreach_copy_stack));

	CG(throw_list) = function_token->throw_list;

	CG(in_clone_method) = 0;
}


void zend_do_receive_arg(zend_uchar op, znode *var, znode *offset, znode *initialization, zend_uchar pass_type TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = op;
	opline->result = *var;
	opline->op1 = *offset;
	if ((op == ZEND_RECV_INIT)) {
		opline->op2 = *initialization;
	} else {
		SET_UNUSED(opline->op2);
	}
	if (!CG(active_op_array)->arg_types) {
		if (pass_type==BYREF_FORCE) {
			int i;

			CG(active_op_array)->arg_types = (unsigned char *) emalloc(sizeof(unsigned char)*(offset->u.constant.value.lval+1));
			for (i=1; i<offset->u.constant.value.lval; i++) {
				CG(active_op_array)->arg_types[i] = BYREF_NONE;
			}
			CG(active_op_array)->arg_types[0]=(unsigned char) offset->u.constant.value.lval;
			CG(active_op_array)->arg_types[offset->u.constant.value.lval] = pass_type;
		}
	} else {
		CG(active_op_array)->arg_types = (unsigned char *) erealloc(CG(active_op_array)->arg_types, sizeof(unsigned char)*(offset->u.constant.value.lval+1));
		CG(active_op_array)->arg_types[0]=(unsigned char) offset->u.constant.value.lval;
		CG(active_op_array)->arg_types[offset->u.constant.value.lval] = pass_type;
	}
}


int zend_do_begin_function_call(znode *function_name TSRMLS_DC)
{
	zend_function *function;
	
	zend_str_tolower(function_name->u.constant.value.str.val, function_name->u.constant.value.str.len);
	if (zend_hash_find(CG(function_table), function_name->u.constant.value.str.val, function_name->u.constant.value.str.len+1, (void **) &function)==FAILURE) {
		zend_do_begin_dynamic_function_call(function_name TSRMLS_CC);
		return 1; /* Dynamic */
	}
	
	switch (function->type) {
		case ZEND_USER_FUNCTION:	{
				zend_op_array *op_array = (zend_op_array *) function;
				
				zend_stack_push(&CG(function_call_stack), (void *) &op_array, sizeof(zend_function *));
			}
			break;
		case ZEND_INTERNAL_FUNCTION: {
				zend_internal_function *internal_function = (zend_internal_function *) function;
				
				zend_stack_push(&CG(function_call_stack), (void *) &internal_function, sizeof(zend_function *));
			}
			break;
	}
	zend_do_extended_fcall_begin(TSRMLS_C); 
	return 0;
}



void zend_do_begin_method_call(znode *left_bracket TSRMLS_DC)
{
	zend_op *last_op;
	int last_op_number;
	unsigned char *ptr = NULL;

	zend_do_end_variable_parse(BP_VAR_R, 0 TSRMLS_CC);
	zend_do_begin_variable_parse(TSRMLS_C);
 
	last_op_number = get_next_op_number(CG(active_op_array))-1;
	last_op = &CG(active_op_array)->opcodes[last_op_number];

	if ((last_op->op2.op_type == IS_CONST) && (last_op->op2.u.constant.value.str.len == sizeof(ZEND_CLONE_FUNC_NAME)-1)
		&& !memcmp(last_op->op2.u.constant.value.str.val, ZEND_CLONE_FUNC_NAME, sizeof(ZEND_CLONE_FUNC_NAME))) {
		last_op->opcode = ZEND_CLONE;
		left_bracket->u.constant.value.lval = ZEND_CLONE;
		zend_stack_push(&CG(function_call_stack), (void *) &ptr, sizeof(zend_function *));
		zend_do_extended_fcall_begin(TSRMLS_C); 
		return;
	}

	last_op->opcode = ZEND_INIT_METHOD_CALL;

	left_bracket->u.constant.value.lval = ZEND_INIT_FCALL_BY_NAME;

	zend_stack_push(&CG(function_call_stack), (void *) &ptr, sizeof(zend_function *));
	zend_do_extended_fcall_begin(TSRMLS_C); 
}
 
 
void zend_do_begin_dynamic_function_call(znode *function_name TSRMLS_DC)
{
	unsigned char *ptr = NULL;
	zend_op *opline;

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_INIT_FCALL_BY_NAME;
	opline->op2 = *function_name;
	opline->extended_value = 0;
	SET_UNUSED(opline->op1);

	zend_stack_push(&CG(function_call_stack), (void *) &ptr, sizeof(zend_function *));
	zend_do_extended_fcall_begin(TSRMLS_C); 
}


void do_fetch_class(znode *result, znode *class_entry, znode *class_name TSRMLS_DC)
{
	long fetch_class_op_number;
	zend_op *opline;

	fetch_class_op_number = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_FETCH_CLASS;
	if (class_entry) {
		opline->op1 = *class_entry;
	} else {
		SET_UNUSED(opline->op1);
		CG(catch_begin) = fetch_class_op_number;
	}
	zend_str_tolower(class_name->u.constant.value.str.val, class_name->u.constant.value.str.len);
	if ((class_name->u.constant.value.str.len == (sizeof("self") - 1)) &&
		!memcmp(class_name->u.constant.value.str.val, "self", sizeof("self"))) {
		SET_UNUSED(opline->op2);
		opline->extended_value = ZEND_FETCH_CLASS_SELF;
		zval_dtor(&class_name->u.constant);
	} else if ((class_name->u.constant.value.str.len == (sizeof("parent") - 1)) &&
		!memcmp(class_name->u.constant.value.str.val, "parent", sizeof("parent"))) {
		SET_UNUSED(opline->op2);
		opline->extended_value = ZEND_FETCH_CLASS_PARENT;
		zval_dtor(&class_name->u.constant);
	} else if ((class_name->u.constant.value.str.len == (sizeof("main") - 1)) &&
		!memcmp(class_name->u.constant.value.str.val, "main", sizeof("main"))) {
		SET_UNUSED(opline->op2);
		opline->extended_value = ZEND_FETCH_CLASS_MAIN;
		zval_dtor(&class_name->u.constant);
	} else {
		opline->op2 = *class_name;
	}
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->result.op_type = IS_CONST; /* FIXME: Hack so that INIT_FCALL_BY_NAME still knows this is a class */
	*result = opline->result;
}


void do_fetch_class_name(znode *result, znode *class_name_entry, znode *class_name, zend_bool case_sensitive TSRMLS_DC)
{
	zend_uint length;

	if (!result) {
		result = class_name_entry;
	} else {
		*result = *class_name_entry;
	}
	if (!case_sensitive) {
		zend_str_tolower(class_name->u.constant.value.str.val, class_name->u.constant.value.str.len);
	}

	length = sizeof("::")-1 + result->u.constant.value.str.len + class_name->u.constant.value.str.len;
	result->u.constant.value.str.val = erealloc(result->u.constant.value.str.val, length+1);
	memcpy(&result->u.constant.value.str.val[result->u.constant.value.str.len], "::", sizeof("::")-1);
	memcpy(&result->u.constant.value.str.val[result->u.constant.value.str.len + sizeof("::")-1], class_name->u.constant.value.str.val, class_name->u.constant.value.str.len+1);
	STR_FREE(class_name->u.constant.value.str.val);
	result->u.constant.value.str.len = length;
}

void zend_do_begin_class_member_function_call(znode *class_name, znode *function_name TSRMLS_DC)
{
	unsigned char *ptr = NULL;
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_INIT_STATIC_METHOD_CALL;
	opline->op1 = *class_name;
	zend_lowercase_znode_if_const(function_name);
	opline->op2 = *function_name;

	zend_stack_push(&CG(function_call_stack), (void *) &ptr, sizeof(zend_function *));
}


void zend_do_end_function_call(znode *function_name, znode *result, znode *argument_list, int is_method, int is_dynamic_fcall TSRMLS_DC)
{
	zend_op *opline;
	
	if (is_method && function_name && function_name->u.constant.value.lval == ZEND_CLONE) {
		if (argument_list->u.constant.value.lval > 0) {
			zend_error(E_COMPILE_ERROR, "Can't pass arguments to __clone()");
		}
		/* FIXME: throw_list */
		zend_stack_del_top(&CG(function_call_stack));
		*result = CG(active_op_array)->opcodes[get_next_op_number(CG(active_op_array))-1].result;
		return;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	if (!is_method && !is_dynamic_fcall && function_name->op_type==IS_CONST) {
		opline->opcode = ZEND_DO_FCALL;
		opline->op1 = *function_name;
	} else {
		opline->opcode = ZEND_DO_FCALL_BY_NAME;
		SET_UNUSED(opline->op1);
	}
	
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->result.op_type = IS_VAR;
	*result = opline->result;
	SET_UNUSED(opline->op2);

	/* Check how much this is really needed
	  opline->op2.u.constant.value.lval = is_method;
	*/
	if (CG(throw_list) != NULL) {
		long op_number = get_next_op_number(CG(active_op_array))-1;
		zend_llist_add_element(CG(throw_list), &op_number);
	} else {
		opline->op2.u.opline_num = -1;
	}

	zend_stack_del_top(&CG(function_call_stack));
	opline->extended_value = argument_list->u.constant.value.lval;
}


void zend_do_pass_param(znode *param, zend_uchar op, int offset TSRMLS_DC)
{
	zend_op *opline;
	unsigned char *arg_types;
	int original_op=op;
	zend_function **function_ptr_ptr, *function_ptr;
	int send_by_reference;

						
	zend_stack_top(&CG(function_call_stack), (void **) &function_ptr_ptr);
	function_ptr = *function_ptr_ptr;

	if (original_op==ZEND_SEND_REF
		&& !CG(allow_call_time_pass_reference)) {
		zend_error(E_COMPILE_WARNING,
					"Call-time pass-by-reference has been deprecated - argument passed by value;  "
					"If you would like to pass it by reference, modify the declaration of %s().  "
					"If you would like to enable call-time pass-by-reference, you can set "
					"allow_call_time_pass_reference to true in your INI file.  "
					"However, future versions may not support this any longer. ",
					(function_ptr?function_ptr->common.function_name:"[runtime function name]"),
					offset+1);
	}

	if (function_ptr) {
		arg_types = function_ptr->common.arg_types;
	} else {
		arg_types = NULL;
	}

	send_by_reference = ARG_SHOULD_BE_SENT_BY_REF(offset, 1, arg_types)?ZEND_ARG_SEND_BY_REF:0;


	if (op == ZEND_SEND_VAR && zend_is_function_or_method_call(param)) {
		/* Method call */
		op = ZEND_SEND_VAR_NO_REF;
	} else if (op == ZEND_SEND_VAL && param->op_type == IS_VAR) {
		op = ZEND_SEND_VAR_NO_REF;
	}

	if (op!=ZEND_SEND_VAR_NO_REF && send_by_reference == ZEND_ARG_SEND_BY_REF) {
		/* change to passing by reference */
		switch (param->op_type) {
			case IS_VAR:
				op = ZEND_SEND_REF;
				break;
			default:
				zend_error(E_COMPILE_ERROR, "Only variables can be passed by reference");
				break;
		}
	}

	if (original_op == ZEND_SEND_VAR) {
		switch (op) {
			case ZEND_SEND_VAR_NO_REF:
				zend_do_end_variable_parse(BP_VAR_R, 0 TSRMLS_CC);
				break;
			case ZEND_SEND_VAR:
				if (function_ptr) {
					zend_do_end_variable_parse(BP_VAR_R, 0 TSRMLS_CC);
				} else {
					zend_do_end_variable_parse(BP_VAR_FUNC_ARG, offset TSRMLS_CC);
				}
				break;
			case ZEND_SEND_REF:
				zend_do_end_variable_parse(BP_VAR_W, 0 TSRMLS_CC);
				break;
		}
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	if (op == ZEND_SEND_VAR_NO_REF) {
		if (function_ptr) {
			opline->extended_value = ZEND_ARG_COMPILE_TIME_BOUND | send_by_reference;
		} else {
			opline->extended_value = 0;
		}
	} else {
		if (function_ptr) {
			opline->extended_value = ZEND_DO_FCALL;
		} else {
			opline->extended_value = ZEND_DO_FCALL_BY_NAME;
		}
	}
	opline->opcode = op;
	opline->op1 = *param;
	opline->op2.u.opline_num = offset;
	SET_UNUSED(opline->op2);
}


static int generate_free_switch_expr(zend_switch_entry *switch_entry TSRMLS_DC)
{
	zend_op *opline;
	
	if (switch_entry->cond.op_type!=IS_VAR && switch_entry->cond.op_type!=IS_TMP_VAR) {
		return 1;
	}
	
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_SWITCH_FREE;
	opline->op1 = switch_entry->cond;
	SET_UNUSED(opline->op2);
	opline->extended_value = 0;
	return 0;
}

static int generate_free_foreach_copy(znode *foreach_copy TSRMLS_DC)
{
	zend_op *opline;
	
	if (foreach_copy->op_type!=IS_VAR && foreach_copy->op_type!=IS_TMP_VAR) {
		return 1;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_SWITCH_FREE;
	opline->op1 = *foreach_copy;
	SET_UNUSED(opline->op2);
	opline->extended_value = 1;
	return 0;
}

void zend_do_return(znode *expr, int do_end_vparse TSRMLS_DC)
{
	zend_op *opline;
	
	if (do_end_vparse) {
		if (CG(active_op_array)->return_reference && !zend_is_function_or_method_call(expr)) {
			zend_do_end_variable_parse(BP_VAR_W, 0 TSRMLS_CC);
		} else {
			zend_do_end_variable_parse(BP_VAR_R, 0 TSRMLS_CC);
		}
#if 0
	} else if (expr && CG(active_op_array)->return_reference) {
		zend_error(E_COMPILE_ERROR, "Only variables may be returned by reference");
#endif
	}

#ifdef ZTS
	zend_stack_apply_with_argument(&CG(switch_cond_stack), ZEND_STACK_APPLY_TOPDOWN, (int (*)(void *element, void *)) generate_free_switch_expr TSRMLS_CC);
	zend_stack_apply_with_argument(&CG(foreach_copy_stack), ZEND_STACK_APPLY_TOPDOWN, (int (*)(void *element, void *)) generate_free_foreach_copy TSRMLS_CC);
#else
	zend_stack_apply(&CG(switch_cond_stack), ZEND_STACK_APPLY_TOPDOWN, (int (*)(void *element)) generate_free_switch_expr);
	zend_stack_apply(&CG(foreach_copy_stack), ZEND_STACK_APPLY_TOPDOWN, (int (*)(void *element)) generate_free_foreach_copy);
#endif

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_RETURN;
	
	if (expr) {
		opline->op1 = *expr;
	} else {
		opline->op1.op_type = IS_CONST;
		INIT_ZVAL(opline->op1.u.constant);
	}
	SET_UNUSED(opline->op2);
}


void zend_do_try(znode *try_token TSRMLS_DC)
{
	try_token->throw_list = (void *) CG(throw_list);
	CG(throw_list) = (zend_llist *) emalloc(sizeof(zend_llist));
	zend_llist_init(CG(throw_list), sizeof(long), NULL, 0);
	/* Initialize try backpatch list used to backpatch throw, do_fcall */
}

static void throw_list_applier(long *opline_num, long *catch_opline)
{
	zend_op *opline;
	TSRMLS_FETCH(); /* Pass this by argument */

	opline = &CG(active_op_array)->opcodes[*opline_num];

	/* Backpatch the opline of the catch statement */
	switch (opline->opcode) {
		case ZEND_DO_FCALL:
		case ZEND_DO_FCALL_BY_NAME:
		case ZEND_THROW:
			opline->op2.u.opline_num = *catch_opline;
			break;
		default:
			zend_error(E_COMPILE_ERROR, "Bad opcode in throw list");
			break;
	}
}

void zend_do_begin_catch(znode *try_token, znode *catch_class, znode *catch_var, zend_bool first_catch TSRMLS_DC)
{
	long catch_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline;
	
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_CATCH;
	opline->op1 = *catch_class;
	SET_UNUSED(opline->op1); /* FIXME: Define IS_CLASS or something like that */
	opline->op2 = *catch_var;

	if (first_catch) {
		zend_llist_apply_with_argument(CG(throw_list), (llist_apply_with_arg_func_t) throw_list_applier, &CG(catch_begin) TSRMLS_CC);
		zend_llist_destroy(CG(throw_list));
		efree(CG(throw_list));
		CG(throw_list) = (void *) try_token->throw_list;
	}
	try_token->u.opline_num = catch_op_number;
}

void zend_do_end_catch(znode *try_token TSRMLS_DC)
{
	CG(active_op_array)->opcodes[try_token->u.opline_num].extended_value = get_next_op_number(CG(active_op_array));
}

void zend_do_throw(znode *expr TSRMLS_DC)
{
	zend_op *opline;
	long throw_op_number = get_next_op_number(CG(active_op_array));
	
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_THROW;
	opline->op1 = *expr;
	SET_UNUSED(opline->op2);
	
	if (CG(throw_list) != NULL) {
		zend_llist_add_element(CG(throw_list), &throw_op_number);
	} else {
		opline->op2.u.opline_num = -1;
	}
}

ZEND_API void function_add_ref(zend_function *function)
{
	if (function->type == ZEND_USER_FUNCTION) {
		zend_op_array *op_array = &function->op_array;

		(*op_array->refcount)++;
		if (op_array->static_variables) {
			HashTable *static_variables = op_array->static_variables;
			zval *tmp_zval;

			ALLOC_HASHTABLE(op_array->static_variables);
			zend_hash_init(op_array->static_variables, 2, NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_copy(op_array->static_variables, static_variables, (copy_ctor_func_t) zval_add_ref, (void *) &tmp_zval, sizeof(zval *));
		}
	}
}

static void do_inherit_parent_constructor(zend_class_entry *ce)
{
	zend_function *function;

	if (!ce->parent || ce->constructor) {
		return;
	}

	if (!zend_hash_exists(&ce->function_table, ce->name, ce->name_length+1)) {
		if (zend_hash_find(&ce->parent->function_table, ce->parent->name, ce->parent->name_length+1, (void **) &function)==SUCCESS) {
			/* inherit parent's constructor */
			zend_hash_update(&ce->function_table, ce->name, ce->name_length+1, function, sizeof(zend_function), NULL);
			function_add_ref(function);
		}
	}
	if (zend_hash_find(&ce->parent->function_table, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME), (void **) &function)==SUCCESS) {
		/* inherit parent's constructor */
		zend_hash_update(&ce->function_table, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME), function, sizeof(zend_function), NULL);
		function_add_ref(function);
	}
	ce->constructor = ce->parent->constructor;
    if (!ce->__get) {
        ce->__get   = ce->parent->__get;
    }
    if (!ce->__set) {
        ce->__set = ce->parent->__set;
    }
    if (!ce->__call) {
        ce->__call = ce->parent->__call;
    }
}


char *zend_visibility_string(zend_uint fn_flags)
{
	if (fn_flags & ZEND_ACC_PRIVATE) {
		return "private";
	}
	if (fn_flags & ZEND_ACC_PROTECTED) {
		return "protected";
	}
	if (fn_flags & ZEND_ACC_PUBLIC) {
		return "public";
	}
	return "";
}


static void do_inherit_method(zend_function *function)
{
	/* The class entry of the derived function intentionally remains the same
	 * as that of the parent class.  That allows us to know in which context
	 * we're running, and handle private method calls properly.
	 */
	if (function->common.fn_flags & ZEND_ACC_ABSTRACT) {
		function->op_array.scope->ce_flags |= ZEND_ACC_ABSTRACT;
	}
	function_add_ref(function);
}


static zend_bool do_inherit_method_check(HashTable *child_function_table, zend_function *parent, zend_hash_key *hash_key, void *pData)
{
	zend_uint child_flags;
	zend_uint parent_flags = parent->common.fn_flags;
	zend_function *child;

	if (zend_hash_quick_find(child_function_table, hash_key->arKey, hash_key->nKeyLength, hash_key->h, (void **) &child)==FAILURE) {
		return 1; /* method doesn't exist in child, copy from parent */
	}
	child_flags	= child->common.fn_flags;
	/* You cannot change from static to non static and vice versa.
	 */
	if ((child_flags & ZEND_ACC_STATIC) != (parent_flags & ZEND_ACC_STATIC)) {
		if (child->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error(E_COMPILE_ERROR, "Cannot make non static method %s::%s() static in class %s", ZEND_FN_SCOPE_NAME(parent), child->common.function_name, ZEND_FN_SCOPE_NAME(child));
		} else {
			zend_error(E_COMPILE_ERROR, "Cannot make static method %s::%s() non static in class %s", ZEND_FN_SCOPE_NAME(parent), child->common.function_name, ZEND_FN_SCOPE_NAME(child));
		}
	}

	/* Disallow making an inherited method abstract.
	 */
	if ((child_flags & ZEND_ACC_ABSTRACT) && !(parent_flags & ZEND_ACC_ABSTRACT)) {
		zend_error(E_COMPILE_ERROR, "Cannot make non abstract method %s::%s() abstract in class %s", ZEND_FN_SCOPE_NAME(parent), child->common.function_name, ZEND_FN_SCOPE_NAME(child));
	}

	if (parent_flags & ZEND_ACC_CHANGED) {
		child->common.fn_flags |= ZEND_ACC_CHANGED;
	} else {
		/* Prevent derived classes from restricting access that was available in parent classes
		 */
		if ((child_flags & ZEND_ACC_PPP_MASK) > (parent_flags & ZEND_ACC_PPP_MASK)) {
			zend_error(E_COMPILE_ERROR, "Access level to %s::%s() must be %s (as in class %s)%s", ZEND_FN_SCOPE_NAME(child), child->common.function_name, zend_visibility_string(parent_flags), ZEND_FN_SCOPE_NAME(parent), (parent_flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
		} else if (((child_flags & ZEND_ACC_PPP_MASK) < (parent_flags & ZEND_ACC_PPP_MASK)) 
			&& ((parent_flags & ZEND_ACC_PPP_MASK) & ZEND_ACC_PRIVATE)) {
			child->common.fn_flags |= ZEND_ACC_CHANGED;
		}
	}

	return 0;
}


static zend_bool do_inherit_property_access_check(HashTable *target_ht, zend_property_info *parent_info, zend_hash_key *hash_key, zend_class_entry *ce)
{
	zend_property_info *child_info;
	zend_class_entry *parent_ce = ce->parent;

	if (parent_info->flags & ZEND_ACC_PRIVATE) {
		return 0; /* don't copy access information to child */
	}

	if (zend_hash_quick_find(&ce->properties_info, hash_key->arKey, hash_key->nKeyLength, hash_key->h, (void **) &child_info)==SUCCESS) {
		if ((child_info->flags & ZEND_ACC_PPP_MASK) > (parent_info->flags & ZEND_ACC_PPP_MASK)) {
			zend_error(E_COMPILE_ERROR, "Access level to %s::$%s must be %s (as in class %s)%s", ce->name, hash_key->arKey, zend_visibility_string(parent_info->flags), parent_ce->name, (parent_info->flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
		}
		return 0;	/* Don't copy from parent */
	} else {
		return 1;	/* Copy from parent */
	}
}


void zend_do_inheritance(zend_class_entry *ce, zend_class_entry *parent_ce)
{
	ce->parent = parent_ce;

	/* Inherit properties */
	zend_hash_merge(&ce->default_properties, &parent_ce->default_properties, (void (*)(void *)) zval_add_ref, NULL, sizeof(zval *), 0);
	zend_hash_merge_ex(&ce->properties_info, &parent_ce->properties_info, (copy_ctor_func_t) zend_duplicate_property_info, sizeof(zend_property_info), (merge_checker_func_t) do_inherit_property_access_check, ce);

	/* STATIC_MEMBERS_FIXME */
/*	zend_hash_merge(ce->static_members, parent_ce->static_members, (void (*)(void *)) zval_add_ref, (void *) &tmp, sizeof(zval *), 0); */
	zend_hash_merge(&ce->constants_table, &parent_ce->constants_table, (void (*)(void *)) zval_add_ref, NULL, sizeof(zval *), 0);
	zend_hash_merge_ex(&ce->function_table, &parent_ce->function_table, (copy_ctor_func_t) do_inherit_method, sizeof(zend_function), (merge_checker_func_t) do_inherit_method_check, NULL);
	do_inherit_parent_constructor(ce);
}

static void create_class(HashTable *class_table, char *name, int name_length, zend_class_entry **ce)
{
	zend_class_entry *new_class_entry;

	new_class_entry = emalloc(sizeof(zend_class_entry));
	*ce = new_class_entry;
	new_class_entry->type = ZEND_USER_CLASS;
	new_class_entry->name = estrndup(name, name_length);
	new_class_entry->name_length = name_length;
	new_class_entry->refcount = 1;
	new_class_entry->constants_updated = 0;
	new_class_entry->ce_flags = 0;

	zend_str_tolower(new_class_entry->name, new_class_entry->name_length);

	zend_hash_init(&new_class_entry->function_table, 10, NULL, ZEND_FUNCTION_DTOR, 0);
	zend_hash_init(&new_class_entry->class_table, 10, NULL, ZEND_CLASS_DTOR, 0);
	zend_hash_init(&new_class_entry->default_properties, 10, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_init(&new_class_entry->properties_info, 10, NULL, (dtor_func_t) zend_destroy_property_info, 0);
	ALLOC_HASHTABLE(new_class_entry->static_members);
	zend_hash_init(new_class_entry->static_members, 10, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_init(&new_class_entry->constants_table, 10, NULL, ZVAL_PTR_DTOR, 0);

	new_class_entry->constructor = NULL;
	new_class_entry->destructor = NULL;
	new_class_entry->clone = NULL;
	new_class_entry->__get = NULL;
	new_class_entry->__set = NULL;
	new_class_entry->__call = NULL;

	new_class_entry->create_object = NULL;
	new_class_entry->parent = NULL;

	if (zend_hash_update(class_table, new_class_entry->name, name_length+1, &new_class_entry, sizeof(zend_class_entry *), NULL) == FAILURE) {
		zend_error(E_COMPILE_ERROR, "Can't create class. Fatal error, please report!");
	}
}


#include "../TSRM/tsrm_strtok_r.h"

static int create_nested_class(HashTable *class_table, char *path, zend_class_entry *new_ce)
{
	char *cur, *temp;
	char *last;
	zend_class_entry *ce, **pce;


	cur = tsrm_strtok_r(path, ":", &temp);

	if (zend_hash_find(class_table, cur, strlen(cur)+1, (void **)&pce) == FAILURE) {
		create_class(class_table, cur, strlen(cur), &ce);
	} else {
		ce = *pce;
	}

	last = tsrm_strtok_r(NULL, ":", &temp);

	for(;;) {
		cur = tsrm_strtok_r(NULL, ":", &temp);
		if (!cur) {
			break;
		}
		if (zend_hash_find(&ce->class_table, last, strlen(last)+1, (void **)&pce) == FAILURE) {
			create_class(&ce->class_table, last, strlen(last), &ce);
		} else {
			ce = *pce;
		}
		last = cur;
	}
	new_ce->refcount++;
	if (zend_hash_add(&ce->class_table, last, strlen(last)+1, &new_ce, sizeof(zend_class_entry *), NULL) == FAILURE) {
		new_ce->refcount--;
		zend_error(E_COMPILE_ERROR, "Cannot redeclare class %s", last);
		return FAILURE;
	}
	return SUCCESS;
}

ZEND_API int do_bind_function(zend_op *opline, HashTable *function_table, HashTable *class_table, int compile_time)
{
	zend_function *function;

	if (opline->opcode != ZEND_DECLARE_FUNCTION) {
		zend_error(E_ERROR, "Internal compiler error.  Please report!");
	}

	zend_hash_find(function_table, opline->op1.u.constant.value.str.val, opline->op1.u.constant.value.str.len, (void *) &function);
	if (zend_hash_add(function_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, function, sizeof(zend_function), NULL)==FAILURE) {
		int error_level = compile_time ? E_COMPILE_ERROR : E_ERROR;
		zend_function *function;

		if (zend_hash_find(function_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, (void *) &function)==SUCCESS
			&& function->type==ZEND_USER_FUNCTION
			&& ((zend_op_array *) function)->last>0) {
			zend_error(error_level, "Cannot redeclare %s() (previously declared in %s:%d)",
						opline->op2.u.constant.value.str.val,
						((zend_op_array *) function)->filename,
						((zend_op_array *) function)->opcodes[0].lineno);
		} else {
			zend_error(error_level, "Cannot redeclare %s()", opline->op2.u.constant.value.str.val);
		}
		return FAILURE;
	} else {
		(*function->op_array.refcount)++;
		function->op_array.static_variables = NULL; /* NULL out the unbound function */
		return SUCCESS;
	}
}
			

ZEND_API int do_bind_class(zend_op *opline, HashTable *function_table, HashTable *class_table)
{
	zend_class_entry *ce, **pce;

	if (zend_hash_find(class_table, opline->op1.u.constant.value.str.val, opline->op1.u.constant.value.str.len, (void **) &pce)==FAILURE) {
		zend_error(E_ERROR, "Internal Zend error - Missing class information for %s", opline->op1.u.constant.value.str.val);
		return FAILURE;
	} else {
		ce = *pce;
	}
	if (strchr(opline->op2.u.constant.value.str.val, ':')) {
		return create_nested_class(class_table, opline->op2.u.constant.value.str.val, ce);
	}
	ce->refcount++;
	if (zend_hash_add(class_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, &ce, sizeof(zend_class_entry *), NULL)==FAILURE) {
		ce->refcount--;
		zend_error(E_ERROR, "Cannot redeclare class %s", opline->op2.u.constant.value.str.val);
		return FAILURE;
	} else {
		return SUCCESS;
	}
}

ZEND_API int do_bind_inherited_class(zend_op *opline, HashTable *function_table, HashTable *class_table, zend_class_entry *parent_ce)
{
	zend_class_entry *ce, **pce;
	int found_ce;

	found_ce = zend_hash_find(class_table, opline->op1.u.constant.value.str.val, opline->op1.u.constant.value.str.len, (void **) &pce);

	if (found_ce == FAILURE) {
		zend_error(E_ERROR, "Cannot redeclare class %s", (*pce)->name);
		return FAILURE;
	} else {
		ce = *pce;
	}

	zend_do_inheritance(ce, parent_ce);

	if (strchr(opline->op2.u.constant.value.str.val, ':')) {
		return create_nested_class(class_table, opline->op2.u.constant.value.str.val, ce);
	}

	ce->refcount++;

	/* Register the derived class */
	if (zend_hash_add(class_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, pce, sizeof(zend_class_entry *), NULL)==FAILURE) {
		zend_error(E_ERROR, "Cannot redeclare class %s", opline->op2.u.constant.value.str.val);
		ce->refcount--;
		zend_hash_destroy(&ce->function_table);
		zend_hash_destroy(&ce->default_properties);
		zend_hash_destroy(&ce->properties_info);
		zend_hash_destroy(ce->static_members);
		zend_hash_destroy(&ce->constants_table);
		return FAILURE;
	}
	return SUCCESS;
}


void zend_do_early_binding(TSRMLS_D)
{
	zend_op *opline = &CG(active_op_array)->opcodes[CG(active_op_array)->last-1];

	while (opline->opcode == ZEND_TICKS && opline > CG(active_op_array)->opcodes) {
		opline--;
	}

	if (do_bind_function(opline, CG(function_table), CG(class_table), 1) == FAILURE) {
		return;
	}

	zend_hash_del(CG(function_table), opline->op1.u.constant.value.str.val, opline->op1.u.constant.value.str.len);
	zval_dtor(&opline->op1.u.constant);
	zval_dtor(&opline->op2.u.constant);
	opline->opcode = ZEND_NOP;
	memset(&opline->op1, 0, sizeof(znode));
	memset(&opline->op2, 0, sizeof(znode));
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void zend_do_boolean_or_begin(znode *expr1, znode *op_token TSRMLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPNZ_EX;
	if (expr1->op_type == IS_TMP_VAR) {
		opline->result = *expr1;
	} else {
		opline->result.u.var = get_temporary_variable(CG(active_op_array));
		opline->result.op_type = IS_TMP_VAR;
	}
	opline->op1 = *expr1;
	SET_UNUSED(opline->op2);
	
	op_token->u.opline_num = next_op_number;

	*expr1 = opline->result;
}


void zend_do_boolean_or_end(znode *result, znode *expr1, znode *expr2, znode *op_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	*result = *expr1; /* we saved the original result in expr1 */
	opline->opcode = ZEND_BOOL;
	opline->result = *result;
	opline->op1 = *expr2;
	SET_UNUSED(opline->op2);

	CG(active_op_array)->opcodes[op_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));
}


void zend_do_boolean_and_begin(znode *expr1, znode *op_token TSRMLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPZ_EX;
	if (expr1->op_type == IS_TMP_VAR) {
		opline->result = *expr1;
	} else {
		opline->result.u.var = get_temporary_variable(CG(active_op_array));
		opline->result.op_type = IS_TMP_VAR;
	}
	opline->op1 = *expr1;
	SET_UNUSED(opline->op2);
	
	op_token->u.opline_num = next_op_number;

	*expr1 = opline->result;
}


void zend_do_boolean_and_end(znode *result, znode *expr1, znode *expr2, znode *op_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	*result = *expr1; /* we saved the original result in expr1 */
	opline->opcode = ZEND_BOOL;
	opline->result = *result;
	opline->op1 = *expr2;
	SET_UNUSED(opline->op2);

	CG(active_op_array)->opcodes[op_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));
}


void zend_do_do_while_begin(TSRMLS_D)
{
	do_begin_loop(TSRMLS_C);
	INC_BPC(CG(active_op_array));
}


void zend_do_do_while_end(znode *do_token, znode *expr_open_bracket, znode *expr TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPNZ;
	opline->op1 = *expr;
	opline->op2.u.opline_num = do_token->u.opline_num;
	SET_UNUSED(opline->op2);

	do_end_loop(expr_open_bracket->u.opline_num TSRMLS_CC);

	DEC_BPC(CG(active_op_array));
}


void zend_do_brk_cont(zend_uchar op, znode *expr TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = op;
	opline->op1.u.opline_num = CG(active_op_array)->current_brk_cont;
	SET_UNUSED(opline->op1);
	if (expr) {
		opline->op2 = *expr;
	} else {
		opline->op2.u.constant.type = IS_LONG;
		opline->op2.u.constant.value.lval = 1;
		INIT_PZVAL(&opline->op2.u.constant);
		opline->op2.op_type = IS_CONST;
	}
}


void zend_do_switch_cond(znode *cond TSRMLS_DC)
{
	zend_switch_entry switch_entry;
	zend_op *opline;

	/* Initialize the conditional value */
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_BOOL;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->result.op_type = IS_TMP_VAR;
	opline->op1.op_type = IS_CONST;
	opline->op1.u.constant.type = IS_BOOL;
	opline->op1.u.constant.value.lval = 0;
	INIT_PZVAL(&opline->op1.u.constant);
	SET_UNUSED(opline->op2);

	switch_entry.cond = *cond;
	switch_entry.default_case = -1;
	switch_entry.control_var = opline->result.u.var;
	zend_stack_push(&CG(switch_cond_stack), (void *) &switch_entry, sizeof(switch_entry));

	do_begin_loop(TSRMLS_C);

	INC_BPC(CG(active_op_array));
}



void zend_do_switch_end(znode *case_list TSRMLS_DC)
{
	zend_op *opline;
	zend_switch_entry *switch_entry_ptr;
	
	zend_stack_top(&CG(switch_cond_stack), (void **) &switch_entry_ptr);

	if (case_list->op_type != IS_UNUSED) { /* non-empty switch */
		int next_op_number = get_next_op_number(CG(active_op_array));

		CG(active_op_array)->opcodes[case_list->u.opline_num].op1.u.opline_num = next_op_number;
	}

	/* add code to jmp to default case */
	if (switch_entry_ptr->default_case != -1) {
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_JMPZ;
		opline->op1.op_type = IS_TMP_VAR;
		opline->op1.u.var = switch_entry_ptr->control_var;
		opline->op2.u.opline_num = switch_entry_ptr->default_case;
		SET_UNUSED(opline->op2);
	}


	/* remember break/continue loop information */
	CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].cont = CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].brk = get_next_op_number(CG(active_op_array));
	CG(active_op_array)->current_brk_cont = CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].parent;

	if (switch_entry_ptr->cond.op_type==IS_VAR || switch_entry_ptr->cond.op_type==IS_TMP_VAR) {
		/* emit free for the switch condition*/
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_SWITCH_FREE;
		opline->op1 = switch_entry_ptr->cond;
		SET_UNUSED(opline->op2);
	}
	if (switch_entry_ptr->cond.op_type == IS_CONST) {
		zval_dtor(&switch_entry_ptr->cond.u.constant);
	}

	zend_stack_del_top(&CG(switch_cond_stack));

	DEC_BPC(CG(active_op_array));
}


void zend_do_case_before_statement(znode *case_list, znode *case_token, znode *case_expr TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	int next_op_number;
	zend_switch_entry *switch_entry_ptr;
	znode result;

	zend_stack_top(&CG(switch_cond_stack), (void **) &switch_entry_ptr);

	opline->opcode = ZEND_CASE;
	opline->result.u.var = switch_entry_ptr->control_var;
	opline->result.op_type = IS_TMP_VAR;
	opline->op1 = switch_entry_ptr->cond;
	opline->op2 = *case_expr;
	if (opline->op1.op_type == IS_CONST) {
		zval_copy_ctor(&opline->op1.u.constant);
	}
	result = opline->result;
	
	next_op_number = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_JMPZ;
	opline->op1 = result;
	SET_UNUSED(opline->op2);
	case_token->u.opline_num = next_op_number;

	if (case_list->op_type==IS_UNUSED) {
		return;
	}
	next_op_number = get_next_op_number(CG(active_op_array));
	CG(active_op_array)->opcodes[case_list->u.opline_num].op1.u.opline_num = next_op_number;
}


void zend_do_case_after_statement(znode *result, znode *case_token TSRMLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	result->u.opline_num = next_op_number;

	switch (CG(active_op_array)->opcodes[case_token->u.opline_num].opcode) {
		case ZEND_JMP:
			CG(active_op_array)->opcodes[case_token->u.opline_num].op1.u.opline_num = get_next_op_number(CG(active_op_array));
			break;
		case ZEND_JMPZ:
			CG(active_op_array)->opcodes[case_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));
			break;
	}
}



void zend_do_default_before_statement(znode *case_list, znode *default_token TSRMLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	zend_switch_entry *switch_entry_ptr;

	zend_stack_top(&CG(switch_cond_stack), (void **) &switch_entry_ptr);

	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	default_token->u.opline_num = next_op_number;

	next_op_number = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_BOOL;
	opline->result.u.var = switch_entry_ptr->control_var;
	opline->result.op_type = IS_TMP_VAR;
	opline->op1.op_type = IS_CONST;
	opline->op1.u.constant.type = IS_BOOL;
	opline->op1.u.constant.value.lval = 1;
	INIT_PZVAL(&opline->op1.u.constant);
	SET_UNUSED(opline->op2);
	switch_entry_ptr->default_case = next_op_number;

	if (case_list->op_type==IS_UNUSED) {
		return;
	}
	next_op_number = get_next_op_number(CG(active_op_array));
	CG(active_op_array)->opcodes[case_list->u.opline_num].op1.u.opline_num = next_op_number;
}


void zend_do_begin_class_declaration(znode *class_token, znode *class_name, znode *parent_class_name TSRMLS_DC)
{
	zend_op *opline;
	int doing_inheritance = 0;
	zend_class_entry *new_class_entry = emalloc(sizeof(zend_class_entry));

	class_token->u.previously_active_class_entry = CG(active_class_entry);
	new_class_entry->type = ZEND_USER_CLASS;
	if (!(strcmp(class_name->u.constant.value.str.val, "main") && strcmp(class_name->u.constant.value.str.val, "self") &&
			strcmp(class_name->u.constant.value.str.val, "parent"))) {
		zend_error(E_COMPILE_ERROR, "Cannot use '%s' as class name as it is reserved", class_name->u.constant.value.str.val);
	}
	if (CG(active_class_entry)) {
		new_class_entry->name_length = sizeof("::")-1 + class_name->u.constant.value.str.len + CG(active_class_entry)->name_length;
		new_class_entry->name = emalloc(new_class_entry->name_length+1);
		strcpy(new_class_entry->name, CG(active_class_entry)->name);
		strcat(new_class_entry->name, "::");
		strcat(new_class_entry->name, class_name->u.constant.value.str.val);
		STR_FREE(class_name->u.constant.value.str.val);
	} else {
		new_class_entry->name = class_name->u.constant.value.str.val;
		new_class_entry->name_length = class_name->u.constant.value.str.len;
	}
	new_class_entry->refcount = 1;
	new_class_entry->constants_updated = 0;
	new_class_entry->ce_flags = 0;
	
	zend_str_tolower(new_class_entry->name, new_class_entry->name_length);

	zend_hash_init(&new_class_entry->function_table, 10, NULL, ZEND_FUNCTION_DTOR, 0);
	zend_hash_init(&new_class_entry->class_table, 10, NULL, ZEND_CLASS_DTOR, 0);
	zend_hash_init(&new_class_entry->default_properties, 10, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_init(&new_class_entry->properties_info, 10, NULL, (dtor_func_t) zend_destroy_property_info, 0);
	ALLOC_HASHTABLE(new_class_entry->static_members);
	zend_hash_init(new_class_entry->static_members, 10, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_init(&new_class_entry->constants_table, 10, NULL, ZVAL_PTR_DTOR, 0);

	new_class_entry->constructor = NULL;
	new_class_entry->destructor = NULL;
	new_class_entry->clone = NULL;
	new_class_entry->__get = NULL;
	new_class_entry->__set = NULL;
	new_class_entry->__call = NULL;

	new_class_entry->create_object = NULL;
	new_class_entry->parent = NULL;

	if (parent_class_name->op_type != IS_UNUSED) {
		doing_inheritance = 1;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->op1.op_type = IS_CONST;
	build_runtime_defined_function_key(&opline->op1.u.constant, new_class_entry->name, new_class_entry->name_length, opline TSRMLS_CC);
	
	opline->op2.op_type = IS_CONST;
	opline->op2.u.constant.type = IS_STRING;
	opline->op2.u.constant.refcount = 1;

	if (doing_inheritance) {
		opline->extended_value = parent_class_name->u.var;
		opline->opcode = ZEND_DECLARE_INHERITED_CLASS;
	} else {
		opline->opcode = ZEND_DECLARE_CLASS;
	}

	opline->op2.u.constant.value.str.val = estrndup(new_class_entry->name, new_class_entry->name_length);
	opline->op2.u.constant.value.str.len = new_class_entry->name_length;

	zend_hash_update(CG(class_table), opline->op1.u.constant.value.str.val, opline->op1.u.constant.value.str.len, &new_class_entry, sizeof(zend_class_entry *), NULL);
	CG(active_class_entry) = new_class_entry;
}


void zend_do_end_class_declaration(znode *class_token TSRMLS_DC)
{
	do_inherit_parent_constructor(CG(active_class_entry));
	CG(active_class_entry) = class_token->u.previously_active_class_entry;
	if (CG(active_ce_parent_class_name).value.str.val) {
		efree(CG(active_ce_parent_class_name).value.str.val);
		CG(active_ce_parent_class_name).value.str.val = NULL;
	}
}

void mangle_property_name(char **dest, int *dest_length, char *src1, int src1_length, char *src2, int src2_length)
{
	char *prop_name;
	int prop_name_length;
        
	prop_name_length = 1 + src1_length + 1 + src2_length;
	prop_name = emalloc(prop_name_length+1);
	prop_name[0] = '\0';
	memcpy(prop_name + 1, src1, src1_length+1);
	memcpy(prop_name + 1 + src1_length + 1, src2, src2_length+1);

	*dest = prop_name;
	*dest_length = prop_name_length;
}


void zend_do_declare_property(znode *var_name, znode *value TSRMLS_DC)
{
	zval *property;
	zend_property_info property_info;
	HashTable *target_symbol_table;

	ALLOC_ZVAL(property);

	if (value) {
		*property = value->u.constant;
	} else {
		INIT_PZVAL(property);
		property->type = IS_NULL;
	}

	if (CG(access_type) & ZEND_ACC_STATIC) {
		target_symbol_table = CG(active_class_entry)->static_members;
	} else {
		target_symbol_table = &CG(active_class_entry)->default_properties;
	}

	switch (CG(access_type) & ZEND_ACC_PPP_MASK) {
		case ZEND_ACC_PRIVATE: {
				char *priv_name;
				int priv_name_length;

				mangle_property_name(&priv_name, &priv_name_length, CG(active_class_entry)->name, CG(active_class_entry)->name_length, var_name->u.constant.value.str.val, var_name->u.constant.value.str.len);
				zend_hash_update(target_symbol_table, priv_name, priv_name_length+1, &property, sizeof(zval *), NULL);
				property_info.name = priv_name;
				property_info.name_length = priv_name_length;
			}
			break;
		case ZEND_ACC_PROTECTED: {
				char *prot_name;
				int prot_name_length;

				mangle_property_name(&prot_name, &prot_name_length, "*", 1, var_name->u.constant.value.str.val, var_name->u.constant.value.str.len);
				zend_hash_update(target_symbol_table, prot_name, prot_name_length+1, &property, sizeof(zval *), NULL);
				property_info.name = prot_name;
				property_info.name_length = prot_name_length;
			}
			break;
		case ZEND_ACC_PUBLIC:
			zend_hash_update(target_symbol_table, var_name->u.constant.value.str.val, var_name->u.constant.value.str.len+1, &property, sizeof(zval *), NULL);
			property_info.name = var_name->u.constant.value.str.val;
			property_info.name_length = var_name->u.constant.value.str.len;
			break;
	}
	property_info.flags = CG(access_type);
	property_info.h = zend_get_hash_value(property_info.name, property_info.name_length+1);

	zend_hash_update(&CG(active_class_entry)->properties_info, var_name->u.constant.value.str.val, var_name->u.constant.value.str.len+1, &property_info, sizeof(zend_property_info), NULL);
}


void zend_do_declare_class_constant(znode *var_name, znode *value TSRMLS_DC)
{
	zval *property;

	ALLOC_ZVAL(property);

	if (value) {
		*property = value->u.constant;
	} else {
		INIT_PZVAL(property);
		property->type = IS_NULL;
	}

	zend_hash_update(&CG(active_class_entry)->constants_table, var_name->u.constant.value.str.val, var_name->u.constant.value.str.len+1, &property, sizeof(zval *), NULL);

	FREE_PNODE(var_name);
}



void zend_do_fetch_property(znode *result, znode *object, znode *property TSRMLS_DC)
{
	zend_op opline;
	zend_llist *fetch_list_ptr;
	zend_op *opline_ptr=NULL;
        
	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);
        
	if (fetch_list_ptr->count == 1) {
		zend_llist_element *le;

		le = fetch_list_ptr->head;
		opline_ptr = (zend_op *) le->data;

		if ((opline_ptr->op1.op_type == IS_CONST) && (opline_ptr->op1.u.constant.type == IS_STRING) &&
			(opline_ptr->op1.u.constant.value.str.len == (sizeof("this")-1)) &&
			!memcmp(opline_ptr->op1.u.constant.value.str.val, "this", sizeof("this"))) {
			efree(opline_ptr->op1.u.constant.value.str.val);
			SET_UNUSED(opline_ptr->op1); /* this means $this for objects */
			opline_ptr->op2 = *property;
			/* if it was usual fetch, we change it to object fetch */
			switch (opline_ptr->opcode) {
				case ZEND_FETCH_W:
					opline_ptr->opcode = ZEND_FETCH_OBJ_W;
					break;
				case ZEND_FETCH_R:
					opline_ptr->opcode = ZEND_FETCH_OBJ_R;
					break;
				case ZEND_FETCH_RW:
					opline_ptr->opcode = ZEND_FETCH_OBJ_RW;
					break;
				case ZEND_FETCH_IS:
					opline_ptr->opcode = ZEND_FETCH_OBJ_IS;
					break;
				case ZEND_FETCH_UNSET:
					opline_ptr->opcode = ZEND_FETCH_OBJ_UNSET;
					break;
				case ZEND_FETCH_FUNC_ARG:
					opline_ptr->opcode = ZEND_FETCH_OBJ_FUNC_ARG;
					break;
			}
			*result = opline_ptr->result;
			return;
		}
	}

	init_op(&opline TSRMLS_CC);
	opline.opcode = ZEND_FETCH_OBJ_W;	/* the backpatching routine assumes W */
	opline.result.op_type = IS_VAR;
	opline.result.u.EA.type = 0;
	opline.result.u.var = get_temporary_variable(CG(active_op_array));
	opline.op1 = *object;
	opline.op2 = *property;
	*result = opline.result;

	zend_llist_add_element(fetch_list_ptr, &opline);
}


void zend_do_push_object(znode *object TSRMLS_DC)
{
	zend_stack_push(&CG(object_stack), object, sizeof(znode));
}


void zend_do_pop_object(znode *object TSRMLS_DC)
{
	znode *tmp;

	zend_stack_top(&CG(object_stack), (void **) &tmp);
	*object = *tmp;
	zend_stack_del_top(&CG(object_stack));
}


void zend_do_begin_new_object(znode *new_token, znode *class_type TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	unsigned char *ptr = NULL;
	
	opline->opcode = ZEND_NEW;
	opline->result.op_type = IS_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *class_type;
	SET_UNUSED(opline->op2);

	new_token->u.opline_num = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_JMP_NO_CTOR;
	opline->op1 = (opline-1)->result;
	SET_UNUSED(opline->op2);

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_INIT_CTOR_CALL;
	opline->op1 = (opline-2)->result;
	SET_UNUSED(opline->op2);

	zend_stack_push(&CG(function_call_stack), (void *) &ptr, sizeof(unsigned char *));
}


void zend_do_end_new_object(znode *result, znode *new_token, znode *argument_list TSRMLS_DC)
{
	znode ctor_result;

	zend_do_end_function_call(NULL, &ctor_result, argument_list, 1, 0 TSRMLS_CC);
	zend_do_free(&ctor_result TSRMLS_CC);

	CG(active_op_array)->opcodes[new_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));
	*result = CG(active_op_array)->opcodes[new_token->u.opline_num].op1;
}


void zend_do_fetch_constant(znode *result, znode *constant_container, znode *constant_name, int mode TSRMLS_DC)
{
	switch (mode) {
		case ZEND_CT:
			if (constant_container) {
				do_fetch_class_name(NULL, constant_container, constant_name, 1 TSRMLS_CC);
				*result = *constant_container;
			} else {
				*result = *constant_name;
			}
			result->u.constant.type = IS_CONSTANT;
			break;
		case ZEND_RT:
			{
				zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	
				opline->opcode = ZEND_FETCH_CONSTANT;
				opline->result.op_type = IS_TMP_VAR;
				opline->result.u.var = get_temporary_variable(CG(active_op_array));
				if (constant_container) {
					opline->op1 = *constant_container;
				} else {
					SET_UNUSED(opline->op1);
				}
				opline->op2 = *constant_name;
				*result = opline->result;
			}
			break;
	}
}


void zend_do_shell_exec(znode *result, znode *cmd TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	switch (cmd->op_type) {
		case IS_TMP_VAR:
			opline->opcode = ZEND_SEND_VAL;
			break;
		default:
			opline->opcode = ZEND_SEND_VAR;
			break;
	}
	opline->op1 = *cmd;
	opline->op2.u.opline_num = 0;
	opline->extended_value = ZEND_DO_FCALL;
	SET_UNUSED(opline->op2);

	/* FIXME: exception support not added to this op2 */
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_DO_FCALL;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->result.op_type = IS_VAR;
	opline->op1.u.constant.value.str.val = estrndup("shell_exec", sizeof("shell_exec")-1);
	opline->op1.u.constant.value.str.len = sizeof("shell_exec")-1;
	INIT_PZVAL(&opline->op1.u.constant);
	opline->op1.u.constant.type = IS_STRING;
	opline->op1.op_type = IS_CONST;
	opline->extended_value = 1;
	SET_UNUSED(opline->op2);
	*result = opline->result;
}



void zend_do_init_array(znode *result, znode *expr, znode *offset, zend_bool is_ref TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_INIT_ARRAY;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->result.op_type = IS_TMP_VAR;
	*result = opline->result;
	if (expr) {
		opline->op1 = *expr;
		if (offset) {
			opline->op2 = *offset;
		} else {
			SET_UNUSED(opline->op2);
		}
	} else {
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);
	}
	opline->extended_value = is_ref;
}


void zend_do_add_array_element(znode *result, znode *expr, znode *offset, zend_bool is_ref TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_ADD_ARRAY_ELEMENT;
	opline->result = *result;
	opline->op1 = *expr;
	if (offset) {
		opline->op2 = *offset;
	} else {
		SET_UNUSED(opline->op2);
	}
	opline->extended_value = is_ref;
}



void zend_do_add_static_array_element(znode *result, znode *offset, znode *expr)
{
	zval *element;

	ALLOC_ZVAL(element);
	*element = expr->u.constant;
	if (offset) {
		switch (offset->u.constant.type) {
			case IS_CONSTANT:
				/* Ugly hack to denote that this value has a constant index */
				element->type |= IS_CONSTANT_INDEX;
				/* break missing intentionally */
			case IS_STRING:
				zend_hash_update(result->u.constant.value.ht, offset->u.constant.value.str.val, offset->u.constant.value.str.len+1, &element, sizeof(zval *), NULL);
				zval_dtor(&offset->u.constant);
				break;
			case IS_LONG:
				zend_hash_index_update(result->u.constant.value.ht, offset->u.constant.value.lval, &element, sizeof(zval *), NULL);
				break;
		}
	} else {
		zend_hash_next_index_insert(result->u.constant.value.ht, &element, sizeof(zval *), NULL);
	}
}


void zend_do_add_list_element(znode *element TSRMLS_DC)
{
	list_llist_element lle;

	if (element) {
		zend_check_writable_variable(element);

		lle.var = *element;
		zend_llist_copy(&lle.dimensions, &CG(dimension_llist));
		zend_llist_prepend_element(&CG(list_llist), &lle);
	}
	(*((int *)CG(dimension_llist).tail->data))++;
}


void zend_do_new_list_begin(TSRMLS_D)
{
	int current_dimension = 0;
	zend_llist_add_element(&CG(dimension_llist), &current_dimension);
}


void zend_do_new_list_end(TSRMLS_D)
{
	zend_llist_remove_tail(&CG(dimension_llist));
	(*((int *)CG(dimension_llist).tail->data))++;
}


void zend_do_list_init(TSRMLS_D)
{
	zend_stack_push(&CG(list_stack), &CG(list_llist), sizeof(zend_llist));
	zend_stack_push(&CG(list_stack), &CG(dimension_llist), sizeof(zend_llist));
	zend_llist_init(&CG(list_llist), sizeof(list_llist_element), NULL, 0);
	zend_llist_init(&CG(dimension_llist), sizeof(int), NULL, 0);
	zend_do_new_list_begin(TSRMLS_C);
}


void zend_do_list_end(znode *result, znode *expr TSRMLS_DC)
{
	zend_llist_element *le;
	zend_llist_element *dimension;
	zend_op *opline;
	znode last_container;

	le = CG(list_llist).head;
	while (le) {
		zend_llist *tmp_dimension_llist = &((list_llist_element *)le->data)->dimensions;
		dimension = tmp_dimension_llist->head;
		while (dimension) {
			opline = get_next_op(CG(active_op_array) TSRMLS_CC);
			if (dimension == tmp_dimension_llist->head) { /* first */
				last_container = *expr;
				switch (expr->op_type) {
					case IS_VAR:
						opline->opcode = ZEND_FETCH_DIM_R;
						break;
					case IS_TMP_VAR:
						opline->opcode = ZEND_FETCH_DIM_TMP_VAR;
						break;
					case IS_CONST: /* fetch_dim_tmp_var will handle this bogus fetch */
						zval_copy_ctor(&expr->u.constant);
						opline->opcode = ZEND_FETCH_DIM_TMP_VAR;
						break;
				}
			} else {
				opline->opcode = ZEND_FETCH_DIM_R;
			}
			opline->result.op_type = IS_VAR;
			opline->result.u.EA.type = 0;
			opline->result.u.var = get_temporary_variable(CG(active_op_array));
			opline->op1 = last_container;
			opline->op2.op_type = IS_CONST;
			opline->op2.u.constant.type = IS_LONG;
			opline->op2.u.constant.value.lval = *((int *) dimension->data);
			INIT_PZVAL(&opline->op2.u.constant);
			opline->extended_value = ZEND_FETCH_ADD_LOCK;
			last_container = opline->result;
			dimension = dimension->next;
		}
		((list_llist_element *) le->data)->value = last_container;
		zend_llist_destroy(&((list_llist_element *) le->data)->dimensions);
		zend_do_end_variable_parse(BP_VAR_W, 0 TSRMLS_CC);
		zend_do_assign(result, &((list_llist_element *) le->data)->var, &((list_llist_element *) le->data)->value TSRMLS_CC);
		CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result.u.EA.type |= EXT_TYPE_UNUSED;
		le = le->next;
	}
	zend_llist_destroy(&CG(dimension_llist));
	zend_llist_destroy(&CG(list_llist));
	*result = *expr;
	{
		zend_llist *p;

		/* restore previous lists */
		zend_stack_top(&CG(list_stack), (void **) &p);
		CG(dimension_llist) = *p;
		zend_stack_del_top(&CG(list_stack));
		zend_stack_top(&CG(list_stack), (void **) &p);
		CG(list_llist) = *p;
		zend_stack_del_top(&CG(list_stack));
	}
}

void zend_do_fetch_static_variable(znode *varname, znode *static_assignment, int fetch_type TSRMLS_DC)
{
	zval *tmp;
	zend_op *opline;
	znode lval;
	znode result;

	ALLOC_ZVAL(tmp);
	convert_to_string(&varname->u.constant);
	if (static_assignment) {
		*tmp = static_assignment->u.constant;
	} else {
		INIT_ZVAL(*tmp);
	}
	if (!CG(active_op_array)->static_variables) {
		ALLOC_HASHTABLE(CG(active_op_array)->static_variables);
		zend_hash_init(CG(active_op_array)->static_variables, 2, NULL, ZVAL_PTR_DTOR, 0);
	}
	zend_hash_update(CG(active_op_array)->static_variables, varname->u.constant.value.str.val, varname->u.constant.value.str.len+1, &tmp, sizeof(zval *), NULL);

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_FETCH_W;		/* the default mode must be Write, since fetch_simple_variable() is used to define function arguments */
	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *varname;
	SET_UNUSED(opline->op2);
	opline->op2.u.EA.type = fetch_type;
	result = opline->result;

	if (varname->op_type == IS_CONST) {
		zval_copy_ctor(&varname->u.constant);
	}
	fetch_simple_variable(&lval, varname, 0 TSRMLS_CC); /* Relies on the fact that the default fetch is BP_VAR_W */

	zend_do_assign_ref(NULL, &lval, &result TSRMLS_CC);
	CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result.u.EA.type |= EXT_TYPE_UNUSED;

/*	zval_dtor(&varname->u.constant); */
}

void zend_do_fetch_global_variable(znode *varname, znode *static_assignment, int fetch_type TSRMLS_DC)
{
	zend_op *opline;
	znode lval;
	znode result;

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_FETCH_W;		/* the default mode must be Write, since fetch_simple_variable() is used to define function arguments */
	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *varname;
	SET_UNUSED(opline->op2);
	opline->op2.u.EA.type = fetch_type;
	result = opline->result;

	if (varname->op_type == IS_CONST) {
		zval_copy_ctor(&varname->u.constant);
	}
	fetch_simple_variable(&lval, varname, 0 TSRMLS_CC); /* Relies on the fact that the default fetch is BP_VAR_W */

	zend_do_assign_ref(NULL, &lval, &result TSRMLS_CC);
	CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result.u.EA.type |= EXT_TYPE_UNUSED;
}


void zend_do_cast(znode *result, znode *expr, int type TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_CAST;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *expr;
	SET_UNUSED(opline->op2);
	opline->extended_value = type;
	*result = opline->result;
}


void zend_do_include_or_eval(int type, znode *result, znode *op1 TSRMLS_DC)
{
	zend_do_extended_fcall_begin(TSRMLS_C);
	{
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

		opline->opcode = ZEND_INCLUDE_OR_EVAL;
		opline->result.op_type = IS_VAR;
		opline->result.u.var = get_temporary_variable(CG(active_op_array));
		opline->op1 = *op1;
		SET_UNUSED(opline->op2);
		opline->op2.u.constant.value.lval = type;
		*result = opline->result;
		if (type==ZEND_REQUIRE) {
			opline->result.u.EA.type |= EXT_TYPE_UNUSED;
		}
	}
	zend_do_extended_fcall_end(TSRMLS_C);
}


void zend_do_indirect_references(znode *result, znode *num_references, znode *variable TSRMLS_DC)
{
	int i;

	zend_do_end_variable_parse(BP_VAR_R, 0 TSRMLS_CC);
	for (i=1; i<num_references->u.constant.value.lval; i++) {
		fetch_simple_variable_ex(result, variable, 0, ZEND_FETCH_R TSRMLS_CC);
		*variable = *result;
	}
	zend_do_begin_variable_parse(TSRMLS_C);
	fetch_simple_variable(result, variable, 1 TSRMLS_CC);
}


void zend_do_unset(znode *variable TSRMLS_DC)
{
	zend_op *last_op;

	zend_check_writable_variable(variable);

	last_op = &CG(active_op_array)->opcodes[get_next_op_number(CG(active_op_array))-1];

	switch (last_op->opcode) {
		case ZEND_FETCH_UNSET:
			last_op->opcode = ZEND_UNSET_VAR;
			break;
		case ZEND_FETCH_DIM_UNSET:
		case ZEND_FETCH_OBJ_UNSET:
			last_op->opcode = ZEND_UNSET_DIM_OBJ;
			break;

	}
}


void zend_do_isset_or_isempty(int type, znode *result, znode *variable TSRMLS_DC)
{
	zend_op *last_op;

	zend_do_end_variable_parse(BP_VAR_IS, 0 TSRMLS_CC);

	zend_check_writable_variable(variable);
	
	last_op = &CG(active_op_array)->opcodes[get_next_op_number(CG(active_op_array))-1];
	
	switch (last_op->opcode) {
		case ZEND_FETCH_IS:
			last_op->opcode = ZEND_ISSET_ISEMPTY_VAR;
			break;
		case ZEND_FETCH_DIM_IS:
		case ZEND_FETCH_OBJ_IS:
			last_op->opcode = ZEND_ISSET_ISEMPTY_DIM_OBJ;
			break;
	}
	last_op->result.op_type = IS_TMP_VAR;
	last_op->extended_value = type;

	*result = last_op->result;
}


void zend_do_instanceof(znode *result, znode *expr, znode *class_znode, int type TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_INSTANCEOF;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *expr;

	opline->op2 = *class_znode;

	*result = opline->result;
}


void zend_do_foreach_begin(znode *foreach_token, znode *array, znode *open_brackets_token, znode *as_token, int variable TSRMLS_DC)
{
	zend_op *opline;
	zend_bool is_variable;

	if (variable) {
		if (zend_is_function_or_method_call(array)) {
			is_variable = 0;
		} else {
			is_variable = 1;
		}
		zend_do_end_variable_parse(BP_VAR_W, 0 TSRMLS_CC);
	} else {
		is_variable = 0;
	}

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	/* Preform array reset */
	opline->opcode = ZEND_FE_RESET;
	opline->result.op_type = IS_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *array;
	SET_UNUSED(opline->op2);
	opline->extended_value = is_variable;
	*open_brackets_token = opline->result;

	zend_stack_push(&CG(foreach_copy_stack), (void *) &opline->result, sizeof(znode));

	/* save the location of the beginning of the loop (array fetching) */
	foreach_token->u.opline_num = get_next_op_number(CG(active_op_array));

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_FE_FETCH;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *open_brackets_token;
	SET_UNUSED(opline->op2);
	*as_token = opline->result;
}


void zend_do_foreach_cont(znode *value, znode *key, znode *as_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	znode result_value, result_key, dummy;

	if (key->op_type != IS_UNUSED) {
		znode *tmp;

		/* switch between the key and value... */
		tmp = key;
		key = value;
		value = tmp;
	}

	opline->opcode = ZEND_FETCH_DIM_TMP_VAR;
	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.opline_num = get_temporary_variable(CG(active_op_array));
	opline->op1 = *as_token;
	opline->op2.op_type = IS_CONST;
	opline->op2.u.constant.type = IS_LONG;
	opline->op2.u.constant.value.lval = 0;
	opline->extended_value = ZEND_FETCH_STANDARD; /* ignored in fetch_dim_tmp_var, but what the hell. */
	result_value = opline->result;

	if (key->op_type != IS_UNUSED) {
		opline = get_next_op(CG(active_op_array) TSRMLS_CC);
		opline->opcode = ZEND_FETCH_DIM_TMP_VAR;
		opline->result.op_type = IS_VAR;
		opline->result.u.EA.type = 0;
		opline->result.u.opline_num = get_temporary_variable(CG(active_op_array));
		opline->op1 = *as_token;
		opline->op2.op_type = IS_CONST;
		opline->op2.u.constant.type = IS_LONG;
		opline->op2.u.constant.value.lval = 1;
		opline->extended_value = ZEND_FETCH_STANDARD; /* ignored in fetch_dim_tmp_var, but what the hell. */
		result_key = opline->result;
	}

	zend_do_assign(&dummy, value, &result_value TSRMLS_CC);
	CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result.u.EA.type |= EXT_TYPE_UNUSED;
	if (key->op_type != IS_UNUSED) {
		zend_do_assign(&dummy, key, &result_key TSRMLS_CC);
		CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result.u.EA.type |= EXT_TYPE_UNUSED;	
	}
	zend_do_free(as_token TSRMLS_CC);

	do_begin_loop(TSRMLS_C);
	INC_BPC(CG(active_op_array));
}


void zend_do_foreach_end(znode *foreach_token, znode *open_brackets_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMP;
	opline->op1.u.opline_num = foreach_token->u.opline_num;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	CG(active_op_array)->opcodes[foreach_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));

	do_end_loop(foreach_token->u.opline_num TSRMLS_CC);

	generate_free_foreach_copy(open_brackets_token TSRMLS_CC);

	zend_stack_del_top(&CG(foreach_copy_stack));

	DEC_BPC(CG(active_op_array));
}


void zend_do_declare_begin(TSRMLS_D)
{
	zend_stack_push(&CG(declare_stack), &CG(declarables), sizeof(zend_declarables));
}


void zend_do_declare_stmt(znode *var, znode *val TSRMLS_DC)
{
	convert_to_string(&var->u.constant);

	if (!zend_binary_strcasecmp(var->u.constant.value.str.val, var->u.constant.value.str.len, "ticks", sizeof("ticks")-1)) {
		convert_to_long(&val->u.constant);
		CG(declarables).ticks = val->u.constant;
	}
	zval_dtor(&var->u.constant);
}


void zend_do_declare_end(znode *declare_token TSRMLS_DC)
{
	zend_declarables *declarables;

	zend_stack_top(&CG(declare_stack), (void **) &declarables);
	/* We should restore if there was more than (current - start) - (ticks?1:0) opcodes */
	if ((get_next_op_number(CG(active_op_array)) - declare_token->u.opline_num) - ((CG(declarables).ticks.value.lval)?1:0)) {
		CG(declarables) = *declarables;
	}
}


void zend_do_end_heredoc(TSRMLS_D)
{
	int opline_num = get_next_op_number(CG(active_op_array))-1;
	zend_op *opline = &CG(active_op_array)->opcodes[opline_num];

	if (opline->opcode != ZEND_ADD_STRING) {
		return;
	}

	opline->op2.u.constant.value.str.val[(opline->op2.u.constant.value.str.len--)-1] = 0;
	if (opline->op2.u.constant.value.str.len>0) {
		if (opline->op2.u.constant.value.str.val[opline->op2.u.constant.value.str.len-1]=='\r') {
			opline->op2.u.constant.value.str.val[(opline->op2.u.constant.value.str.len--)-1] = 0;
		}
	}	
}


void zend_do_exit(znode *result, znode *message TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_EXIT;
	opline->op1 = *message;
	SET_UNUSED(opline->op2);

	result->op_type = IS_CONST;
	result->u.constant.type = IS_BOOL;
	result->u.constant.value.lval = 1;
}


void zend_do_begin_silence(znode *strudel_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_BEGIN_SILENCE;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	*strudel_token = opline->result;
}


void zend_do_end_silence(znode *strudel_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_END_SILENCE;
	opline->op1 = *strudel_token;
	SET_UNUSED(opline->op2);
}


void zend_do_begin_qm_op(znode *cond, znode *qm_token TSRMLS_DC)
{
	int jmpz_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline;
	
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_JMPZ;
	opline->op1 = *cond;
	SET_UNUSED(opline->op2);
	opline->op2.u.opline_num = jmpz_op_number;
	*qm_token = opline->op2;

	INC_BPC(CG(active_op_array));
}


void zend_do_qm_true(znode *true_value, znode *qm_token, znode *colon_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	CG(active_op_array)->opcodes[qm_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array))+1; /* jmp over the ZEND_JMP */

	opline->opcode = ZEND_QM_ASSIGN;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *true_value;
	SET_UNUSED(opline->op2);

	*qm_token = opline->result;
	colon_token->u.opline_num = get_next_op_number(CG(active_op_array));

	opline = get_next_op(CG(active_op_array) TSRMLS_CC);
	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void zend_do_qm_false(znode *result, znode *false_value, znode *qm_token, znode *colon_token TSRMLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_QM_ASSIGN;
	opline->result = *qm_token;
	opline->op1 = *false_value;
	SET_UNUSED(opline->op2);
	
	CG(active_op_array)->opcodes[colon_token->u.opline_num].op1.u.opline_num = get_next_op_number(CG(active_op_array));

	*result = opline->result;

	DEC_BPC(CG(active_op_array));
}


void zend_do_extended_info(TSRMLS_D)
{
	zend_op *opline;
	
	if (!CG(extended_info)) {
		return;
	}
	
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_EXT_STMT;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void zend_do_extended_fcall_begin(TSRMLS_D)
{
	zend_op *opline;
	
	if (!CG(extended_info)) {
		return;
	}
	
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_EXT_FCALL_BEGIN;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void zend_do_extended_fcall_end(TSRMLS_D)
{
	zend_op *opline;
	
	if (!CG(extended_info)) {
		return;
	}
	
	opline = get_next_op(CG(active_op_array) TSRMLS_CC);

	opline->opcode = ZEND_EXT_FCALL_END;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void zend_do_ticks(TSRMLS_D)
{
	if (CG(declarables).ticks.value.lval) {
		zend_op *opline = get_next_op(CG(active_op_array) TSRMLS_CC);

		opline->opcode = ZEND_TICKS;
		opline->op1.u.constant = CG(declarables).ticks;
		opline->op1.op_type = IS_CONST;
		SET_UNUSED(opline->op2);
	}
}


int zend_register_auto_global(char *name, uint name_len TSRMLS_DC)
{
	return zend_hash_add_empty_element(CG(auto_globals), name, name_len+1);
}


int zendlex(znode *zendlval TSRMLS_DC)
{
	int retval;

	if (CG(increment_lineno)) {
		CG(zend_lineno)++;
		CG(increment_lineno) = 0;
	}

	zendlval->u.constant.type = IS_LONG;
	retval = lex_scan(&zendlval->u.constant TSRMLS_CC);
	switch (retval) {
		case T_COMMENT:
		case T_OPEN_TAG:
		case T_WHITESPACE:
			retval = zendlex(zendlval TSRMLS_CC);
			break;
		case T_CLOSE_TAG:
			if (LANG_SCNG(yy_text)[LANG_SCNG(yy_leng)-1]=='\n'
				|| (LANG_SCNG(yy_text)[LANG_SCNG(yy_leng)-2]=='\r' && LANG_SCNG(yy_text)[LANG_SCNG(yy_leng)-1])) {
				CG(increment_lineno) = 1;
			}
			retval = ';'; /* implicit ; */
			break;
		case T_OPEN_TAG_WITH_ECHO:
			retval = T_ECHO;
			break;
		case T_END_HEREDOC:
			efree(zendlval->u.constant.value.str.val);
			break;
	}
		
	INIT_PZVAL(&zendlval->u.constant);
	zendlval->op_type = IS_CONST;
	return retval;
}


void zend_duplicate_property_info(zend_property_info *property_info)
{
	property_info->name = estrndup(property_info->name, property_info->name_length);
}


void zend_destroy_property_info(zend_property_info *property_info)
{
	efree(property_info->name);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
