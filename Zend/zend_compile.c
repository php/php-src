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

#include "zend.h"
#include "zend_compile.h"
#include "zend_llist.h"
#include "zend_API.h"
#include "zend_variables.h"
#include "zend_operators.h"


ZEND_API zend_op_array *(*zend_compile_files)(int mark_as_ref CLS_DC, int file_count, ...);


#ifndef ZTS
ZEND_API zend_compiler_globals compiler_globals;
ZEND_API zend_executor_globals executor_globals;
#endif


#define SET_UNUSED(op)  (op).op_type = IS_UNUSED

static int is_not_internal_function(zend_function *function)
{
	return(function->type != ZEND_INTERNAL_FUNCTION);
}


static int is_not_internal_class(zend_class_entry *ce)
{
	return(ce->type != ZEND_INTERNAL_CLASS);
}


static void free_filename(void *p)
{
	efree(*((char **) p));
}


void init_compiler(CLS_D ELS_DC)
{
	zend_stack_init(&CG(bp_stack));
	zend_stack_init(&CG(function_call_stack));
	zend_stack_init(&CG(switch_cond_stack));
	zend_stack_init(&CG(object_stack));
	CG(active_class_entry) = NULL;
	zend_llist_init(&CG(list_llist), sizeof(list_llist_element), NULL, 0);
	zend_llist_init(&CG(dimension_llist), sizeof(int), NULL, 0);
	zend_llist_init(&CG(filenames_list), sizeof(char *), free_filename, 0);
	CG(short_tags) = ZEND_UV(short_tags);
	CG(asp_tags) = ZEND_UV(asp_tags);
	CG(handle_op_arrays) = 1;
	zend_hash_apply(&module_registry, (int (*)(void *)) module_registry_request_startup);
	init_resource_list(ELS_C);
	CG(unclean_shutdown) = 0;
}


void shutdown_compiler(CLS_D)
{
	zend_stack_destroy(&CG(bp_stack));
	zend_stack_destroy(&CG(function_call_stack));
	zend_stack_destroy(&CG(switch_cond_stack));
	zend_stack_destroy(&CG(object_stack));
	zend_llist_destroy(&CG(filenames_list));
	zend_hash_apply(CG(function_table), (int (*)(void *)) is_not_internal_function);
	zend_hash_apply(CG(class_table), (int (*)(void *)) is_not_internal_class);
	zend_hash_apply(&module_registry, (int (*)(void *)) module_registry_cleanup);
}


ZEND_API void zend_set_compiled_filename(char *new_compiled_filename)
{
	char *p = estrdup(new_compiled_filename);
	CLS_FETCH();

	zend_llist_add_element(&CG(filenames_list), &p);
	CG(compiled_filename) = p;
}


ZEND_API void zend_restore_compiled_filename(char *original_compiled_filename)
{
	CLS_FETCH();

	CG(compiled_filename) = original_compiled_filename;
}

ZEND_API char *zend_get_compiled_filename()
{
	CLS_FETCH();

	return CG(compiled_filename);
}


static int get_temporary_variable(zend_op_array *op_array)
{
	return (op_array->T)++;
}


void do_binary_op(int op, znode *result, znode *op1, znode *op2 CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = op;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *op1;
	opline->op2 = *op2;
	*result = opline->result;
}


void do_unary_op(int op, znode *result, znode *op1 CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = op;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *op1;
	*result = opline->result;
	SET_UNUSED(opline->op2);
}


void do_binary_assign_op(int op, znode *result, znode *op1, znode *op2 CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = op;
	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *op1;
	opline->op2 = *op2;
	*result = opline->result;
}



void do_fetch_globals(znode *varname CLS_DC)
{
	if (!CG(active_op_array)->initialized_globals
		&& varname->op_type == IS_CONST
		&& varname->u.constant.type == IS_STRING
		&& varname->u.constant.value.str.len == (sizeof("GLOBALS")-1)
		&& !memcmp(varname->u.constant.value.str.val, "GLOBALS", sizeof("GLOBALS")-1)) {
		zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

		opline->opcode = ZEND_INIT_GLOBALS;
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);
		CG(active_op_array)->initialized_globals = 1;
	}
}


void fetch_simple_variable(znode *result, znode *varname, int bp CLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	zend_llist *fetch_list_ptr;

	opline->opcode = ZEND_FETCH_W;		/* the default mode must be Write, since fetch_simple_variable() is used to define function arguments */
	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *varname;
	*result = opline->result;
	SET_UNUSED(opline->op2);
	opline->op2.u.constant.value.lval = ZEND_FETCH_LOCAL;

	if (bp) {
		zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);
		zend_llist_add_element(fetch_list_ptr, &next_op_number);
	}
}


void fetch_array_begin(znode *result, znode *varname, znode *first_dim CLS_DC)
{
	fetch_simple_variable(result, varname, 1 CLS_CC);

	fetch_array_dim(result, result, first_dim CLS_CC);
}


void fetch_array_dim(znode *result, znode *parent, znode *dim CLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	zend_llist *fetch_list_ptr;

	opline->opcode = ZEND_FETCH_DIM_W;	/* the backpatching routine assumes W */
	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *parent;
	opline->op2 = *dim;
	*result = opline->result;

	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);
	zend_llist_add_element(fetch_list_ptr, &next_op_number);
}


void do_print(znode *result, znode *arg CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->opcode = ZEND_PRINT;
	opline->op1 = *arg;
	SET_UNUSED(opline->op2);
	*result = opline->result;
}


void do_echo(znode *arg CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_ECHO;
	opline->op1 = *arg;
	SET_UNUSED(opline->op2);
}


void do_assign(znode *result, znode *variable, znode *value CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_ASSIGN;
	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *variable;
	opline->op2 = *value;
	*result = opline->result;
}


void do_assign_ref(znode *result, znode *lvar, znode *rvar CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_ASSIGN_REF;
	if (result) {
		opline->result.op_type = IS_VAR;
		opline->result.u.EA.type = 0;
		opline->result.u.var = get_temporary_variable(CG(active_op_array));
		*result = opline->result;
	} else {
		SET_UNUSED(opline->result);
	}
	opline->op1 = *lvar;
	opline->op2 = *rvar;
}


static inline void do_begin_loop(CLS_D)
{
	zend_brk_cont_element *brk_cont_element;
	int parent;

	parent = CG(active_op_array)->current_brk_cont;
	CG(active_op_array)->current_brk_cont = CG(active_op_array)->last_brk_cont;
	brk_cont_element = get_next_brk_cont_element(CG(active_op_array));
	brk_cont_element->parent = parent;
}


static inline void do_end_loop(int cont_addr CLS_DC)
{
	CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].cont = cont_addr;
	CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].brk = get_next_op_number(CG(active_op_array));
	CG(active_op_array)->current_brk_cont = CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].parent;
}


void do_while_cond(znode *expr, znode *close_bracket_token CLS_DC)
{
	int while_cond_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_JMPZ;
	opline->op1 = *expr;
	close_bracket_token->u.opline_num = while_cond_op_number;
	SET_UNUSED(opline->op2);

	do_begin_loop(CLS_C);
	INC_BPC(CG(active_op_array));
}


void do_while_end(znode *while_token, znode *close_bracket_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	/* add unconditional jump */
	opline->opcode = ZEND_JMP;
	opline->op1.u.opline_num = while_token->u.opline_num;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	
	/* update while's conditional jmp */
	CG(active_op_array)->opcodes[close_bracket_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));

	do_end_loop(while_token->u.opline_num CLS_CC);

	DEC_BPC(CG(active_op_array));
}


void do_for_cond(znode *expr, znode *second_semicolon_token CLS_DC)
{
	int for_cond_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_JMPZNZ;
	opline->result = *expr;  /* the conditional expression */
	second_semicolon_token->u.opline_num = for_cond_op_number;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void do_for_before_statement(znode *cond_start, znode *second_semicolon_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_JMP;
	opline->op1.u.opline_num = cond_start->u.opline_num;
	CG(active_op_array)->opcodes[second_semicolon_token->u.opline_num].op1.u.opline_num = get_next_op_number(CG(active_op_array));
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	do_begin_loop(CLS_C);

	INC_BPC(CG(active_op_array));
}


void do_for_end(znode *second_semicolon_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_JMP;
	opline->op1.u.opline_num = second_semicolon_token->u.opline_num+1;
	CG(active_op_array)->opcodes[second_semicolon_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	do_end_loop(second_semicolon_token->u.opline_num+1 CLS_CC);

	DEC_BPC(CG(active_op_array));
}


void do_pre_incdec(znode *result, znode *op1, int op CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = op;
	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *op1;
	*result = opline->result;
	SET_UNUSED(opline->op2);
}


void do_post_incdec(znode *result, znode *op1, int op CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = op;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *op1;
	*result = opline->result;
	SET_UNUSED(opline->op2);
}


void do_if_cond(znode *cond, znode *closing_bracket_token CLS_DC)
{
	int if_cond_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_JMPZ;
	opline->op1 = *cond;
	closing_bracket_token->u.opline_num = if_cond_op_number;
	SET_UNUSED(opline->op2);
	INC_BPC(CG(active_op_array));
}


void do_if_after_statement(znode *closing_bracket_token, unsigned char initialize CLS_DC)
{
	int if_end_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
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


void do_if_end(CLS_D)
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


void do_begin_variable_parse(CLS_D)
{
	zend_llist fetch_list;

	zend_llist_init(&fetch_list, sizeof(int), NULL, 0);
	zend_stack_push(&CG(bp_stack), (void *) &fetch_list, sizeof(zend_llist));
}


void do_end_variable_parse(int type CLS_DC)
{
	zend_llist *fetch_list_ptr;
	zend_llist_element *le;
	zend_op *opline;

	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);

	le = fetch_list_ptr->head;

	while (le) {
		opline = &CG(active_op_array)->opcodes[*((int *) le->data)];
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
		}
		le = le->next;
	}
	zend_llist_destroy(fetch_list_ptr);
	zend_stack_del_top(&CG(bp_stack));
}


void do_init_string(znode *result CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_INIT_STRING;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	*result = opline->result;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void do_add_char(znode *result, znode *op1, znode *op2 CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_ADD_CHAR;
	opline->op1 = *op1;
	opline->op2 = *op2;
	opline->op2.op_type = IS_CONST;
	opline->result = opline->op1;
	*result = opline->result;
}


void do_add_string(znode *result, znode *op1, znode *op2 CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_ADD_STRING;
	opline->op1 = *op1;
	opline->op2 = *op2;
	opline->op2.op_type = IS_CONST;
	opline->result = opline->op1;
	*result = opline->result;
}


void do_add_variable(znode *result, znode *op1, znode *op2 CLS_DC)
{
	zend_op *opline;

	if (op1->op_type == IS_CONST) {
		opline = get_next_op(CG(active_op_array) CLS_CC);
		opline->opcode = ZEND_INIT_STRING;
		opline->result.op_type = IS_TMP_VAR;
		opline->result.u.var = get_temporary_variable(CG(active_op_array));
		*result = opline->result;
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);

		if (op1->u.constant.value.str.len>0) {
			opline = get_next_op(CG(active_op_array) CLS_CC);
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

	opline = get_next_op(CG(active_op_array) CLS_CC);
	opline->opcode = ZEND_ADD_VAR;
	opline->result = *result;
	opline->op1 = *result;
	opline->op2 = *op2;
	*result = opline->result;
}

	
void do_free(znode *op1 CLS_DC)
{
	if (op1->op_type==IS_TMP_VAR) {
		zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

		opline->opcode = ZEND_FREE;
		opline->op1 = *op1;
		SET_UNUSED(opline->op2);
	} else if (op1->op_type==IS_VAR) {
		zend_op *opline = &CG(active_op_array)->opcodes[CG(active_op_array)->last-1];

		if (opline->result.op_type == op1->op_type
			&& opline->result.u.var == op1->u.var) {
			opline->result.u.EA.type |= EXT_TYPE_UNUSED;
		} else {

			/* This should be an object instanciation
			 * Find JMP_NO_CTOR, mark the preceding ASSIGN and the
			 * proceeding INIT_FCALL_BY_NAME as unused
			 */
			while (opline>CG(active_op_array)->opcodes) {
				if (opline->opcode == ZEND_JMP_NO_CTOR) {
					(opline-1)->result.u.EA.type |= EXT_TYPE_UNUSED;
					(opline+1)->op1.u.EA.type |= EXT_TYPE_UNUSED;
					break;
				}
				opline--;
			}
		}
	}
}


void do_begin_function_declaration(znode *function_token, znode *function_name, int is_method CLS_DC)
{
	zend_op_array op_array;
	char *name = function_name->u.constant.value.str.val;
	int name_len = function_name->u.constant.value.str.len;
	int function_begin_line = function_token->u.opline_num;

	function_token->u.op_array = CG(active_op_array);
	zend_str_tolower(name, name_len);

	init_op_array(&op_array, INITIAL_OP_ARRAY_SIZE);
	op_array.function_name = name;
	op_array.arg_types = NULL;

	if (is_method) {
		zend_hash_update(&CG(active_class_entry)->function_table, name, name_len+1, &op_array, sizeof(zend_op_array), (void **) &CG(active_op_array));
	} else {
		zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

		opline->opcode = ZEND_DECLARE_FUNCTION_OR_CLASS;
		opline->op1.op_type = IS_CONST;
		opline->op1.u.constant.type = IS_LONG;
		opline->op1.u.constant.value.lval = zend_hash_next_free_element(CG(function_table));
		opline->op2.op_type = IS_CONST;
		opline->op2.u.constant.type = IS_STRING;
		opline->op2.u.constant.value.str.val = estrndup(name, name_len);
		opline->op2.u.constant.value.str.len = name_len;
		opline->extended_value = ZEND_DECLARE_FUNCTION;
		zend_hash_next_index_insert(CG(function_table), &op_array, sizeof(zend_op_array), (void **) &CG(active_op_array));
	}

	if (CG(extended_info)) {
		zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

		opline->opcode = ZEND_EXT_NOP;
		opline->lineno = function_begin_line;
		SET_UNUSED(opline->op1);
		SET_UNUSED(opline->op2);
	}
}


void do_end_function_declaration(znode *function_token CLS_DC)
{
	pass_two(CG(active_op_array));
	CG(active_op_array) = function_token->u.op_array;
}


void do_receive_arg(int op, znode *var, znode *offset, znode *initialization, unsigned char pass_type CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = op;
	opline->result = *var;
	opline->op1 = *offset;
	if ((op == ZEND_RECV_INIT) && initialization) {
		opline->op2 = *initialization;
	} else {
		SET_UNUSED(opline->op2);
	}
	if (pass_type==BYREF_FORCE && !CG(active_op_array)->arg_types) {
		int i;

		CG(active_op_array)->arg_types = (unsigned char *) emalloc(sizeof(unsigned char)*(offset->u.constant.value.lval+1));
		for (i=1; i<offset->u.constant.value.lval; i++) {
			CG(active_op_array)->arg_types[i] = BYREF_NONE;
		}
		CG(active_op_array)->arg_types[0]=(unsigned char) offset->u.constant.value.lval;
	}
	if (CG(active_op_array)->arg_types) {
		CG(active_op_array)->arg_types = (unsigned char *) erealloc(CG(active_op_array)->arg_types, sizeof(unsigned char)*(offset->u.constant.value.lval+1));
		CG(active_op_array)->arg_types[offset->u.constant.value.lval] = pass_type;
		CG(active_op_array)->arg_types[0]++;
	}
}


void do_begin_function_call(znode *function_name CLS_DC)
{
	zend_op *opline ;
	zend_function *function;
	
	zend_str_tolower(function_name->u.constant.value.str.val, function_name->u.constant.value.str.len);
	if (zend_hash_find(CG(function_table), function_name->u.constant.value.str.val,function_name->u.constant.value.str.len+1, (void **) &function)==FAILURE) {
		znode tmp = *function_name;

		zval_copy_ctor(&tmp.u.constant);
		do_begin_dynamic_function_call(&tmp CLS_CC);
		return;
	}
	
	opline = get_next_op(CG(active_op_array) CLS_CC);

	switch (function->type) {
		case ZEND_USER_FUNCTION:	{
				zend_op_array *op_array = (zend_op_array *) function;
				
				zend_stack_push(&CG(function_call_stack), (void *) &op_array->arg_types, sizeof(unsigned char *));
			}
			break;
		case ZEND_INTERNAL_FUNCTION: {
				zend_internal_function *internal_function = (zend_internal_function *) function;
				
				zend_stack_push(&CG(function_call_stack), (void *) &internal_function->arg_types, sizeof(unsigned char *));
			}
			break;
	}
	opline->opcode = ZEND_INIT_FCALL;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void do_begin_dynamic_function_call(znode *function_name CLS_DC)
{
	unsigned char *ptr = NULL;
	int last_op_number = get_next_op_number(CG(active_op_array))-1;

	if (last_op_number>=0 && CG(active_op_array)->opcodes[last_op_number].opcode == ZEND_FETCH_OBJ_R) {
		CG(active_op_array)->opcodes[last_op_number].opcode = ZEND_INIT_FCALL_BY_NAME;
	} else {
		zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	
		opline->opcode = ZEND_INIT_FCALL_BY_NAME;
		opline->op2 = *function_name;
		SET_UNUSED(opline->op1);
	}
	zend_stack_push(&CG(function_call_stack), (void *) &ptr, sizeof(unsigned char *));
}


void do_begin_class_member_function_call(znode *class_name, znode *function_name CLS_DC)
{
	unsigned char *ptr = NULL;
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_INIT_FCALL_BY_NAME;
	zend_str_tolower(class_name->u.constant.value.str.val, class_name->u.constant.value.str.len);
	opline->op1 = *class_name;
	opline->op2 = *function_name;
	zval_copy_ctor(&opline->op2.u.constant);
	zend_stack_push(&CG(function_call_stack), (void *) &ptr, sizeof(unsigned char *));
}


void do_end_function_call(znode *function_name, znode *result, znode *argument_list, int is_method CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	ELS_FETCH();
	
	opline->opcode = ZEND_DO_FCALL;
	opline->op1 = *function_name;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->result.op_type = IS_TMP_VAR;
	*result = opline->result;
	SET_UNUSED(opline->op2);
	opline->op2.u.constant.value.lval = is_method;
	zend_stack_del_top(&CG(function_call_stack));
	opline->extended_value = argument_list->u.constant.value.lval;
}


void do_pass_param(znode *param, int op, int offset CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	unsigned char **arg_types_ptr, *arg_types;
	int original_op=op;

	zend_stack_top(&CG(function_call_stack), (void **) &arg_types_ptr);
	arg_types = *arg_types_ptr;

	if (op == ZEND_SEND_VAL) {
		switch (param->op_type) {
			case IS_CONST:	/* constants behave like variables when passed to functions,
							 * as far as reference counting is concerned.  Treat them
							 * as if they were variables here.
							 */
			case IS_VAR:
				op = ZEND_SEND_VAR;
				break;
		}
	}
	if (arg_types && offset<=arg_types[0]
		&& arg_types[offset]==BYREF_FORCE) {
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

	if (original_op==ZEND_SEND_VAR) {
		switch(op) {
			case ZEND_SEND_VAR:
				do_end_variable_parse(BP_VAR_R CLS_CC);
				break;
			case ZEND_SEND_REF:
				do_end_variable_parse(BP_VAR_W CLS_CC);
				break;
		}
	}

	opline->opcode = op;
	opline->op1 = *param;
	opline->op2.u.opline_num = offset;
	SET_UNUSED(opline->op2);
}


void do_return(znode *expr CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_RETURN;
	if (expr) {
		opline->op1 = *expr;
	} else {
		var_uninit(&opline->op1.u.constant);
		opline->op1.op_type = IS_CONST;
		opline->op1.u.constant.refcount=1;
		opline->op1.u.constant.is_ref=0;
	}
	SET_UNUSED(opline->op2);
}


void do_bind_function_or_class(zend_op *opline, HashTable *function_table, HashTable *class_table)
{
	switch (opline->extended_value) {
		case ZEND_DECLARE_FUNCTION: {
				zend_function *function;

				zend_hash_index_find(function_table, opline->op1.u.constant.value.lval, (void **) &function);
				(*function->op_array.refcount)++;
				if (zend_hash_add(function_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, function, sizeof(zend_function), NULL)==FAILURE) {
					zend_error(E_ERROR, "Cannot redeclare %s()", opline->op2.u.constant.value.str.val);
				}
			}
			break;
		case ZEND_DECLARE_CLASS: {
				zend_class_entry *ce;

				zend_hash_index_find(class_table, opline->op1.u.constant.value.lval, (void **) &ce);
				(*ce->refcount)++;
				if (zend_hash_add(class_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, ce, sizeof(zend_class_entry), NULL)==FAILURE) {
					zend_error(E_ERROR, "Cannot redeclare class %s", opline->op2.u.constant.value.str.val);
				}
			}
			break;
	}
}


void do_early_binding(CLS_D)
{
	zend_op *opline = &CG(active_op_array)->opcodes[CG(active_op_array)->last-1];

	do_bind_function_or_class(opline, CG(function_table), CG(class_table));
	switch (opline->extended_value) {
		case ZEND_DECLARE_FUNCTION:
			zend_hash_index_del(CG(function_table), opline->op1.u.constant.value.lval);
			break;
		case ZEND_DECLARE_CLASS:
			zend_hash_index_del(CG(class_table), opline->op1.u.constant.value.lval);
			break;
	}
	zval_dtor(&opline->op2.u.constant);
	opline->opcode = ZEND_NOP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void do_boolean_or_begin(znode *expr1, znode *op_token CLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

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


void do_boolean_or_end(znode *result, znode *expr1, znode *expr2, znode *op_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	*result = *expr1; /* we saved the original result in expr1 */
	opline->opcode = ZEND_BOOL;
	opline->result = *result;
	opline->op1 = *expr2;
	SET_UNUSED(opline->op2);

	CG(active_op_array)->opcodes[op_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));
}


void do_boolean_and_begin(znode *expr1, znode *op_token CLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

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


void do_boolean_and_end(znode *result, znode *expr1, znode *expr2, znode *op_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	*result = *expr1; /* we saved the original result in expr1 */
	opline->opcode = ZEND_BOOL;
	opline->result = *result;
	opline->op1 = *expr2;
	SET_UNUSED(opline->op2);

	CG(active_op_array)->opcodes[op_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));
}


void do_do_while_begin(CLS_D)
{
	do_begin_loop(CLS_C);
	INC_BPC(CG(active_op_array));
}


void do_do_while_end(znode *do_token, znode *expr CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_JMPNZ;
	opline->op1 = *expr;
	opline->op2.u.opline_num = do_token->u.opline_num;

	do_end_loop(do_token->u.opline_num CLS_CC);

	DEC_BPC(CG(active_op_array));
}


void do_brk_cont(int op, znode *expr CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = op;
	opline->op1.u.opline_num = CG(active_op_array)->current_brk_cont;
	SET_UNUSED(opline->op1);
	if (expr) {
		opline->op2 = *expr;
	} else {
		opline->op2.u.constant.type = IS_LONG;
		opline->op2.u.constant.value.lval = 1;
		opline->op2.u.constant.refcount=1;
		opline->op2.u.constant.is_ref=0;
		opline->op2.op_type = IS_CONST;
	}
}


void do_switch_cond(znode *cond CLS_DC)
{
	zend_switch_entry switch_entry;
	zend_op *opline = &CG(active_op_array)->opcodes[CG(active_op_array)->last-1];

	switch_entry.cond = *cond;
	switch_entry.default_case = -1;
	switch_entry.control_var = -1;
	zend_stack_push(&CG(switch_cond_stack), (void *) &switch_entry, sizeof(switch_entry));

	if (opline->result.op_type == IS_VAR) {
		opline->result.u.EA.type |= EXT_TYPE_UNUSED;
	}

	do_begin_loop(CLS_C);

	INC_BPC(CG(active_op_array));
}



void do_switch_end(znode *case_list CLS_DC)
{
	zend_op *opline;
	zend_switch_entry *switch_entry_ptr;
	
	zend_stack_top(&CG(switch_cond_stack), (void **) &switch_entry_ptr);

	if (case_list->u.opline_num != -1) { /* empty switch */
		int next_op_number = get_next_op_number(CG(active_op_array));

		CG(active_op_array)->opcodes[case_list->u.opline_num].op1.u.opline_num = next_op_number;
	}

	/* add code to jmp to default case */
	if (switch_entry_ptr->default_case != -1) {
		opline = get_next_op(CG(active_op_array) CLS_CC);
		opline->opcode = ZEND_JMPZ;
		opline->op1.op_type = IS_TMP_VAR;
		opline->op1.u.var = switch_entry_ptr->control_var;
		opline->op2.u.opline_num = switch_entry_ptr->default_case;
		SET_UNUSED(opline->op2);
	}


	/* remember break/continue loop information */
	CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].cont = CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].brk = get_next_op_number(CG(active_op_array));
	CG(active_op_array)->current_brk_cont = CG(active_op_array)->brk_cont_array[CG(active_op_array)->current_brk_cont].parent;

	/* emit free for the switch condition*/
	do_free(&switch_entry_ptr->cond CLS_CC);
	if (switch_entry_ptr->cond.op_type == IS_CONST) {
		zval_dtor(&switch_entry_ptr->cond.u.constant);
	}

	zend_stack_del_top(&CG(switch_cond_stack));

	DEC_BPC(CG(active_op_array));
}


void do_case_before_statement(znode *case_list, znode *case_token, znode *case_expr CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	int next_op_number;
	zend_switch_entry *switch_entry_ptr;
	znode result;

	zend_stack_top(&CG(switch_cond_stack), (void **) &switch_entry_ptr);

	opline->opcode = ZEND_CASE;
	if (switch_entry_ptr->control_var==-1) {
		switch_entry_ptr->control_var = get_temporary_variable(CG(active_op_array));
	}
	opline->result.u.var = switch_entry_ptr->control_var;
	opline->result.op_type = IS_TMP_VAR;
	opline->op1 = switch_entry_ptr->cond;
	opline->op2 = *case_expr;
	if (opline->op1.op_type == IS_CONST) {
		zval_copy_ctor(&opline->op1.u.constant);
	}
	result = opline->result;
	
	next_op_number = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) CLS_CC);
	opline->opcode = ZEND_JMPZ;
	opline->op1 = result;
	SET_UNUSED(opline->op2);
	case_token->u.opline_num = next_op_number;

	if (case_list->u.opline_num==-1) {
		return;
	}
	next_op_number = get_next_op_number(CG(active_op_array));
	CG(active_op_array)->opcodes[case_list->u.opline_num].op1.u.opline_num = next_op_number;
}


void do_case_after_statement(znode *result, znode *case_token CLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

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



void do_default_before_statement(znode *case_list, znode *default_token CLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	zend_switch_entry *switch_entry_ptr;

	zend_stack_top(&CG(switch_cond_stack), (void **) &switch_entry_ptr);

	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	default_token->u.opline_num = next_op_number;

	next_op_number = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) CLS_CC);
	opline->opcode = ZEND_BOOL;
	if (switch_entry_ptr->control_var==-1) {
		switch_entry_ptr->control_var = get_temporary_variable(CG(active_op_array));
	}
	opline->result.u.var = switch_entry_ptr->control_var;
	opline->result.op_type = IS_TMP_VAR;
	opline->op1.op_type = IS_CONST;
	opline->op1.u.constant.type = IS_LONG;
	opline->op1.u.constant.value.lval = 1;
	opline->op1.u.constant.refcount=1;
	opline->op1.u.constant.is_ref=0;
	SET_UNUSED(opline->op2);
	switch_entry_ptr->default_case = next_op_number;

	next_op_number = get_next_op_number(CG(active_op_array));
	if (case_list->u.opline_num==-1) {
		return;
	}
	CG(active_op_array)->opcodes[case_list->u.opline_num].op1.u.opline_num = next_op_number;
}

static void function_add_ref(zend_function *function)
{
	if (function->type == ZEND_USER_FUNCTION) {
		(*((zend_op_array *) function)->refcount)++;
	}
}


void do_begin_class_declaration(znode *class_name, znode *parent_class_name CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	if (CG(active_class_entry)) {
		zend_error(E_COMPILE_ERROR, "Class declarations may not be nested");
		return;
	}
	CG(class_entry).type = ZEND_USER_CLASS;
	CG(class_entry).name = class_name->u.constant.value.str.val;
	CG(class_entry).name_length = class_name->u.constant.value.str.len;
	CG(class_entry).refcount = (int *) emalloc(sizeof(int));
	*CG(class_entry).refcount = 1;
	
	zend_str_tolower(CG(class_entry).name, CG(class_entry).name_length);

	zend_hash_init(&CG(class_entry).function_table, 10, NULL, (void (*)(void *)) destroy_zend_function, 0);
	zend_hash_init(&CG(class_entry).default_properties, 10, NULL, PVAL_PTR_DTOR, 0);

	/* code for inheritance from parent class */
	if (parent_class_name) {
		zend_class_entry *parent_class;
		zend_function tmp_zend_function;
		zval *tmp;

		if (zend_hash_find(CG(class_table), parent_class_name->u.constant.value.str.val, parent_class_name->u.constant.value.str.len+1, (void **) &parent_class)==FAILURE) {
			zend_error(E_ERROR, "Undefined parent class '%s'", parent_class_name->u.constant.value.str.val);
			return;
		}
		/* copy functions */
		zend_hash_copy(&CG(class_entry).function_table, &parent_class->function_table, (void (*)(void *)) function_add_ref, &tmp_zend_function, sizeof(zend_function));

		/* copy default properties */
		zend_hash_copy(&CG(class_entry).default_properties, &parent_class->default_properties, (void (*)(void *)) zval_add_ref, (void *) &tmp, sizeof(zval *));

		zval_dtor(&parent_class_name->u.constant);
	} else {
		CG(class_entry).parent = NULL;
	}

	CG(class_entry).handle_function_call = NULL;
	CG(class_entry).handle_property_set = NULL;
	CG(class_entry).handle_property_get = NULL;

	opline->opcode = ZEND_DECLARE_FUNCTION_OR_CLASS;
	opline->op1.op_type = IS_CONST;
	opline->op1.u.constant.type = IS_LONG;
	opline->op1.u.constant.value.lval = zend_hash_next_free_element(CG(class_table));
	opline->op2.op_type = IS_CONST;
	opline->op2.u.constant.type = IS_STRING;
	opline->op2.u.constant.value.str.val = estrndup(CG(class_entry).name, CG(class_entry).name_length);
	opline->op2.u.constant.value.str.len = CG(class_entry).name_length;
	opline->extended_value = ZEND_DECLARE_CLASS;

	zend_hash_next_index_insert(CG(class_table), &CG(class_entry), sizeof(zend_class_entry), (void **) &CG(active_class_entry));
}


void do_end_class_declaration(CLS_D)
{
	CG(active_class_entry) = NULL;
}


void do_declare_property(znode *var_name, znode *value CLS_DC)
{
	if (value) {
		zval *property = (zval *) emalloc(sizeof(zval));

		*property = value->u.constant;
		zend_hash_update(&CG(active_class_entry)->default_properties, var_name->u.constant.value.str.val, var_name->u.constant.value.str.len+1, &property, sizeof(zval *), NULL);
	}
	FREE_PNODE(var_name);
}


void do_fetch_property(znode *result, znode *object, znode *property CLS_DC)
{
	int next_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	zend_llist *fetch_list_ptr;

	opline->opcode = ZEND_FETCH_OBJ_W;	/* the backpatching routine assumes W */
	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *object;
	opline->op2 = *property;
	*result = opline->result;

	zend_stack_top(&CG(bp_stack), (void **) &fetch_list_ptr);
	zend_llist_add_element(fetch_list_ptr, &next_op_number);
}


void do_push_object(znode *object CLS_DC)
{
	zend_stack_push(&CG(object_stack), object, sizeof(znode));
}


void do_pop_object(znode *object CLS_DC)
{
	znode *tmp;

	zend_stack_top(&CG(object_stack), (void **) &tmp);
	*object = *tmp;
	zend_stack_del_top(&CG(object_stack));
}


void do_begin_new_object(znode *result, znode *variable, znode *new_token, znode *class_name CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	unsigned char *ptr = NULL;
	
	opline->opcode = ZEND_NEW;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *class_name;
	SET_UNUSED(opline->op2);

	do_assign(result, variable, &opline->result CLS_CC);

	new_token->u.opline_num = get_next_op_number(CG(active_op_array));
	opline = get_next_op(CG(active_op_array) CLS_CC);
	opline->opcode = ZEND_JMP_NO_CTOR;
	opline->op1 = *result;
	SET_UNUSED(opline->op2);

	if (class_name->op_type == IS_CONST) {
		zval_copy_ctor(&class_name->u.constant);
	}
	opline = get_next_op(CG(active_op_array) CLS_CC);
	opline->opcode = ZEND_INIT_FCALL_BY_NAME;
	opline->op1 = *result;
	opline->op2 = *class_name;
	zend_stack_push(&CG(function_call_stack), (void *) &ptr, sizeof(unsigned char *));
}


void do_end_new_object(znode *class_name, znode *new_token, znode *argument_list CLS_DC)
{
	znode ctor_result;

	if (class_name->op_type == IS_CONST) {
		zval_copy_ctor(&class_name->u.constant);
	}
	do_end_function_call(class_name, &ctor_result, argument_list, 1 CLS_CC);
	do_free(&ctor_result CLS_CC);

	CG(active_op_array)->opcodes[new_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));
}


void do_fetch_constant(znode *result, znode *constant_name, int mode CLS_DC)
{
	switch (mode) {
		case ZEND_CT:
			*result = *constant_name;
			result->u.constant.type = IS_CONSTANT;
			break;
		case ZEND_RT: {
				zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	
				opline->opcode = ZEND_FETCH_CONSTANT;
				opline->result.op_type = IS_TMP_VAR;
				opline->result.u.var = get_temporary_variable(CG(active_op_array));
				opline->op1 = *constant_name;
				*result = opline->result;
				SET_UNUSED(opline->op2);
			}
			break;
	}
}


void do_shell_exec(znode *result, znode *cmd CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_INIT_FCALL;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);

	opline = get_next_op(CG(active_op_array) CLS_CC);
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
	SET_UNUSED(opline->op2);

	opline = get_next_op(CG(active_op_array) CLS_CC);
	opline->opcode = ZEND_DO_FCALL;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->result.op_type = IS_TMP_VAR;
	opline->op1.u.constant.value.str.val = estrndup("shell_exec",sizeof("shell_exec")-1);
	opline->op1.u.constant.value.str.len = sizeof("shell_exec")-1;
	opline->op1.u.constant.refcount = 1;
	opline->op1.u.constant.is_ref = 0;
	opline->op1.u.constant.type = IS_STRING;
	opline->op1.op_type = IS_CONST;
	SET_UNUSED(opline->op2);
	*result = opline->result;
}



void do_init_array(znode *result, znode *expr, znode *offset CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

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
}


void do_add_array_element(znode *result, znode *expr, znode *offset CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_ADD_ARRAY_ELEMENT;
	opline->result = *result;
	opline->op1 = *expr;
	if (offset) {
		opline->op2 = *offset;
	} else {
		SET_UNUSED(opline->op2);
	}
}



void do_add_static_array_element(znode *result, znode *expr, znode *offset)
{
	zval *element = (zval *) emalloc(sizeof(zval));

	*element = expr->u.constant;
	if (offset) {
		switch (offset->u.constant.type) {
			case IS_STRING:
				zend_hash_update(result->u.constant.value.ht, offset->u.constant.value.str.val, offset->u.constant.value.str.len+1, &element, sizeof(zval *), NULL);
				break;
			case IS_LONG:
				zend_hash_index_update(result->u.constant.value.ht, offset->u.constant.value.lval, &element, sizeof(zval *), NULL);
				break;
		}
	} else {
		zend_hash_next_index_insert(result->u.constant.value.ht, &element, sizeof(zval *), NULL);
	}
}


void do_add_list_element(znode *element CLS_DC)
{
	list_llist_element lle;

	if(element) {
		lle.var = *element;
		zend_llist_copy(&lle.dimensions, &CG(dimension_llist));
		zend_llist_add_element(&CG(list_llist), &lle);
	}
	(*((int *)CG(dimension_llist).tail->data))++;
}


void do_new_list_begin(CLS_D)
{
	int current_dimension = 0;
	zend_llist_add_element(&CG(dimension_llist), &current_dimension);
}


void do_new_list_end(CLS_D)
{

	zend_llist_remove_tail(&CG(dimension_llist));
	(*((int *)CG(dimension_llist).tail->data))++;
}


void do_list_init(CLS_D)
{
	zend_llist_init(&CG(list_llist), sizeof(list_llist_element), NULL, 0);
	zend_llist_init(&CG(dimension_llist), sizeof(int), NULL, 0);
	do_new_list_begin(CLS_C);
}


void do_list_end(znode *result, znode *expr CLS_DC)
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
			opline = get_next_op(CG(active_op_array) CLS_CC);
			if (dimension == tmp_dimension_llist->head) { /* first */
				last_container = *expr;
				switch(expr->op_type) {
					case IS_VAR:
						opline->opcode = ZEND_FETCH_DIM_R;
						break;
					case IS_TMP_VAR:
					case IS_CONST: /* fetch_dim_tmp_var will handle this bogus fetch */
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
			opline->op2.u.constant.refcount = 1;
			opline->op2.u.constant.is_ref = 0;
			last_container = opline->result;
			dimension = dimension->next;
		}
		((list_llist_element *) le->data)->value = last_container;
		zend_llist_destroy(&((list_llist_element *) le->data)->dimensions);
		le = le->next;
	}

	le = CG(list_llist).head;
	while (le) {
		do_assign(result, &((list_llist_element *) le->data)->var, &((list_llist_element *) le->data)->value CLS_CC);
		CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result.u.EA.type |= EXT_TYPE_UNUSED;
		le = le->next;
	}
	zend_llist_destroy(&CG(dimension_llist));
	zend_llist_destroy(&CG(list_llist));
	*result = *expr;
}


void do_fetch_global_or_static_variable(znode *varname, znode *static_assignment, int fetch_type CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
	znode lval;

	if (fetch_type==ZEND_FETCH_STATIC && static_assignment) {
		zval *tmp = (zval *) emalloc(sizeof(zval));

		convert_to_string(&varname->u.constant);
		*tmp = static_assignment->u.constant;
		if (!CG(active_op_array)->static_variables) {
			CG(active_op_array)->static_variables = (HashTable *) emalloc(sizeof(HashTable));
			zend_hash_init(CG(active_op_array)->static_variables, 2, NULL, PVAL_PTR_DTOR, 0);
		}
		zend_hash_update_ptr(CG(active_op_array)->static_variables, varname->u.constant.value.str.val, varname->u.constant.value.str.len+1, tmp, sizeof(zval *), NULL);
	}


	opline->opcode = ZEND_FETCH_W;		/* the default mode must be Write, since fetch_simple_variable() is used to define function arguments */
	opline->result.op_type = IS_VAR;
	opline->result.u.EA.type = 0;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *varname;
	SET_UNUSED(opline->op2);
	opline->op2.u.constant.value.lval = fetch_type;

	if (varname->op_type == IS_CONST) {
		zval_copy_ctor(&varname->u.constant);
	}
	fetch_simple_variable(&lval, varname, 0 CLS_CC); /* Relies on the fact that the default fetch is BP_VAR_W */


	do_assign_ref(NULL, &lval, &opline->result CLS_CC);
	CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result.u.EA.type |= EXT_TYPE_UNUSED;
}


void do_cast(znode *result, znode *expr, int type CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_CAST;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *expr;
	SET_UNUSED(opline->op2);
	opline->op2.u.constant.type = type;
	*result = opline->result;
}


void do_include_or_eval(int type, znode *result, znode *op1 CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_INCLUDE_OR_EVAL;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *op1;
	SET_UNUSED(opline->op2);
	opline->op2.u.constant.value.lval = type;
	*result = opline->result;
}


void do_indirect_references(znode *result, znode *num_references, znode *variable CLS_DC)
{
	int i;

	for (i=1; i<=num_references->u.constant.value.lval; i++) {
		fetch_simple_variable(result, variable, 1 CLS_CC);
		*variable = *result;
	}
}


void do_unset(znode *variable CLS_DC)
{
	zend_op *last_op;

	last_op = &CG(active_op_array)->opcodes[get_next_op_number(CG(active_op_array))-1];

	switch (last_op->opcode) {
		case ZEND_FETCH_R:
			last_op->opcode = ZEND_UNSET_VAR;
			break;
		case ZEND_FETCH_DIM_R:
		case ZEND_FETCH_OBJ_R:
			last_op->opcode = ZEND_UNSET_DIM_OBJ;
			break;

	}
}


void do_isset_or_isempty(int type, znode *result, znode *variable CLS_DC)
{
	zend_op *opline;

	do_end_variable_parse(BP_VAR_IS CLS_CC);
	opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_ISSET_ISEMPTY;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *variable;
	opline->op2.u.constant.value.lval = type;
	SET_UNUSED(opline->op2);
	*result = opline->result;
}


void do_foreach_begin(znode *foreach_token, znode *array, znode *open_brackets_token, znode *as_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	/* Preform array reset */
	opline->opcode = ZEND_FE_RESET;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *array;
	SET_UNUSED(opline->op2);
	*open_brackets_token = opline->result;

	/* save the location of the beginning of the loop (array fetching) */
	foreach_token->u.opline_num = get_next_op_number(CG(active_op_array));

	opline = get_next_op(CG(active_op_array) CLS_CC);
	opline->opcode = ZEND_FE_FETCH;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *open_brackets_token;
	SET_UNUSED(opline->op2);
	*as_token = opline->result;
}


void do_foreach_cont(znode *value, znode *key, znode *as_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);
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
	result_value = opline->result;

	if (key->op_type != IS_UNUSED) {
		opline = get_next_op(CG(active_op_array) CLS_CC);
		opline->opcode = ZEND_FETCH_DIM_TMP_VAR;
		opline->result.op_type = IS_VAR;
		opline->result.u.EA.type = 0;
		opline->result.u.opline_num = get_temporary_variable(CG(active_op_array));
		opline->op1 = *as_token;
		opline->op2.op_type = IS_CONST;
		opline->op2.u.constant.type = IS_LONG;
		opline->op2.u.constant.value.lval = 1;
		result_key = opline->result;
	}

	do_assign(&dummy, value, &result_value CLS_CC);
	CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result.u.EA.type |= EXT_TYPE_UNUSED;
	if (key->op_type != IS_UNUSED) {
		do_assign(&dummy, key, &result_key CLS_CC);
		CG(active_op_array)->opcodes[CG(active_op_array)->last-1].result.u.EA.type |= EXT_TYPE_UNUSED;	
	}
	do_free(as_token CLS_CC);

	do_begin_loop(CLS_C);
	INC_BPC(CG(active_op_array));
}


void do_foreach_end(znode *foreach_token, znode *open_brackets_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_JMP;
	opline->op1.u.opline_num = foreach_token->u.opline_num;
	SET_UNUSED(opline->op2);

	CG(active_op_array)->opcodes[foreach_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array));

	do_end_loop(foreach_token->u.opline_num CLS_CC);

	do_free(open_brackets_token CLS_CC);

	DEC_BPC(CG(active_op_array));
}


void do_end_heredoc(CLS_D)
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


void do_exit(znode *result, znode *message CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_EXIT;
	opline->op1 = *message;
	SET_UNUSED(opline->op2);

	result->op_type = IS_CONST;
	result->u.constant.type = IS_BOOL;
	result->u.constant.value.lval = 1;
}


void do_begin_silence(znode *strudel_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_BEGIN_SILENCE;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
	*strudel_token = opline->result;
}


void do_end_silence(znode *strudel_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_END_SILENCE;
	opline->op1 = *strudel_token;
	SET_UNUSED(opline->op2);
}


void do_begin_qm_op(znode *cond, znode *qm_token CLS_DC)
{
	int jmpz_op_number = get_next_op_number(CG(active_op_array));
	zend_op *opline;
	
	opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_JMPZ;
	opline->op1 = *cond;
	SET_UNUSED(opline->op2);
	opline->op2.u.opline_num = jmpz_op_number;
	*qm_token = opline->op2;

	INC_BPC(CG(active_op_array));
}


void do_qm_true(znode *true_value, znode *qm_token, znode *colon_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	CG(active_op_array)->opcodes[qm_token->u.opline_num].op2.u.opline_num = get_next_op_number(CG(active_op_array))+1; /* jmp over the ZEND_JMP */

	opline->opcode = ZEND_QM_ASSIGN;
	opline->result.op_type = IS_TMP_VAR;
	opline->result.u.var = get_temporary_variable(CG(active_op_array));
	opline->op1 = *true_value;
	SET_UNUSED(opline->op2);

	*qm_token = opline->result;
	colon_token->u.opline_num = get_next_op_number(CG(active_op_array));

	opline = get_next_op(CG(active_op_array) CLS_CC);
	opline->opcode = ZEND_JMP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void do_qm_false(znode *result, znode *false_value, znode *qm_token, znode *colon_token CLS_DC)
{
	zend_op *opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_QM_ASSIGN;
	opline->result = *qm_token;
	opline->op1 = *false_value;
	SET_UNUSED(opline->op2);
	
	CG(active_op_array)->opcodes[colon_token->u.opline_num].op1.u.opline_num = get_next_op_number(CG(active_op_array));

	*result = opline->result;

	DEC_BPC(CG(active_op_array));
}

void do_extended_info(CLS_D)
{
	zend_op *opline;
	
	if (!CG(extended_info)) {
		return;
	}
	
	opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_EXT_STMT;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}

void do_extended_fcall_begin(CLS_D)
{
	zend_op *opline;
	
	if (!CG(extended_info)) {
		return;
	}
	
	opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_EXT_FCALL_BEGIN;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


void do_extended_fcall_end(CLS_D)
{
	zend_op *opline;
	
	if (!CG(extended_info)) {
		return;
	}
	
	opline = get_next_op(CG(active_op_array) CLS_CC);

	opline->opcode = ZEND_EXT_FCALL_END;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
}


int zendlex(znode *zendlval CLS_DC)
{
	int retval;

	zendlval->u.constant.type = IS_LONG;
	retval = lex_scan(&zendlval->u.constant CLS_CC);
	switch(retval) {
		case T_COMMENT:
		case T_OPEN_TAG:
		case T_WHITESPACE:
			retval = zendlex(zendlval CLS_CC);
			break;
		case T_CLOSE_TAG:
			retval = ';'; /* implicit ; */
			break;
		case T_OPEN_TAG_WITH_ECHO:
			retval = T_ECHO;
			break;
	}
		
	zendlval->u.constant.refcount = 1;
	zendlval->u.constant.is_ref = 0;
	zendlval->op_type = IS_CONST;
	return retval;
}
