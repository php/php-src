/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "phpdbg.h"
#include "zend_vm_opcodes.h"
#include "zend_compile.h"
#include "phpdbg_opcode.h"
#include "phpdbg_utils.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static inline zend_uint phpdbg_decode_literal(zend_op_array *ops, zend_literal *literal TSRMLS_DC) /* {{{ */
{
	int iter = 0;

	while (iter < ops->last_literal) {
		if (literal == &ops->literals[iter]) {
			return iter;
		}
		iter++;
	}

	return 0;
} /* }}} */

static inline char *phpdbg_decode_op(zend_op_array *ops, znode_op *op, zend_uint type, HashTable *vars TSRMLS_DC) /* {{{ */
{
	char *decode = NULL;

	switch (type &~ EXT_TYPE_UNUSED) {
		case IS_CV:
			asprintf(&decode, "$%s", ops->vars[op->var].name);
		break;

		case IS_VAR:
		case IS_TMP_VAR: {
			zend_ulong id = 0, *pid = NULL;
			if (vars != NULL) {
				if (zend_hash_index_find(vars, (zend_ulong) ops->vars - op->var, (void**) &pid) != SUCCESS) {
					id = zend_hash_num_elements(vars);
					zend_hash_index_update(
						vars, (zend_ulong) ops->vars - op->var,
						(void**) &id,
						sizeof(zend_ulong), NULL);
				} else id = *pid;
			}
			asprintf(&decode, "@%lu", id);
		} break;

		case IS_CONST:
			asprintf(&decode, "C%u", phpdbg_decode_literal(ops, op->literal TSRMLS_CC));
		break;

		case IS_UNUSED:
			asprintf(&decode, "<unused>");
		break;
	}
	return decode;
} /* }}} */

char *phpdbg_decode_opline(zend_op_array *ops, zend_op *op, HashTable *vars TSRMLS_DC) /*{{{ */
{
	char *decode[4] = {NULL, NULL, NULL, NULL};

	switch (op->opcode) {
	case ZEND_JMP:
#ifdef ZEND_GOTO
	case ZEND_GOTO:
#endif
#ifdef ZEND_FAST_CALL
	case ZEND_FAST_CALL:
#endif
			asprintf(&decode[1], "J%ld", op->op1.jmp_addr - ops->opcodes);
		goto format;

	case ZEND_JMPZNZ:
			decode[1] = phpdbg_decode_op(ops, &op->op1, op->op1_type, vars TSRMLS_CC);
			asprintf(
				&decode[2], "J%u or J%lu", op->op2.opline_num, op->extended_value);
		goto result;

	case ZEND_JMPZ:
	case ZEND_JMPNZ:
	case ZEND_JMPZ_EX:
	case ZEND_JMPNZ_EX:

#ifdef ZEND_JMP_SET
	case ZEND_JMP_SET:
#endif
#ifdef ZEND_JMP_SET_VAR
	case ZEND_JMP_SET_VAR:
#endif
		decode[1] = phpdbg_decode_op(ops, &op->op1, op->op1_type, vars TSRMLS_CC);
		asprintf(
			&decode[2], "J%ld", op->op2.jmp_addr - ops->opcodes);
	goto result;

	case ZEND_RECV_INIT:
		goto result;

		default: {
			decode[1] = phpdbg_decode_op(ops, &op->op1, op->op1_type, vars TSRMLS_CC);
			decode[2] = phpdbg_decode_op(ops, &op->op2, op->op2_type, vars TSRMLS_CC);
result:
			decode[3] = phpdbg_decode_op(ops, &op->result, op->result_type, vars TSRMLS_CC);
format:
			asprintf(
				&decode[0],
				"%-20s %-20s %-20s",
				decode[1] ? decode[1] : "",
				decode[2] ? decode[2] : "",
				decode[3] ? decode[3] : "");
		}
	}

	if (decode[1])
		free(decode[1]);
	if (decode[2])
		free(decode[2]);
	if (decode[3])
		free(decode[3]);

	return decode[0];
} /* }}} */

void phpdbg_print_opline_ex(zend_execute_data *execute_data, HashTable *vars, zend_bool ignore_flags TSRMLS_DC) /* {{{ */
{
	/* force out a line while stepping so the user knows what is happening */
	if (ignore_flags ||
		(!(PHPDBG_G(flags) & PHPDBG_IS_QUIET) ||
		(PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ||
		(PHPDBG_G(oplog)))) {

		zend_op *opline = execute_data->opline;
		char *decode = phpdbg_decode_opline(execute_data->op_array, opline, vars TSRMLS_CC);

		if (ignore_flags || (!(PHPDBG_G(flags) & PHPDBG_IS_QUIET) || (PHPDBG_G(flags) & PHPDBG_IS_STEPPING))) {
			/* output line info */
			phpdbg_notice("L%-5u %16p %-30s %s %s",
			   opline->lineno,
			   opline,
			   phpdbg_decode_opcode(opline->opcode),
			   decode,
			   execute_data->op_array->filename ? execute_data->op_array->filename : "unknown");
		}

		if (!ignore_flags && PHPDBG_G(oplog)) {
			phpdbg_log_ex(PHPDBG_G(oplog), "L%-5u %16p %-30s %s %s",
				opline->lineno,
				opline,
				phpdbg_decode_opcode(opline->opcode),
				decode,
				execute_data->op_array->filename ? execute_data->op_array->filename : "unknown");
		}

		if (decode) {
			free(decode);
		}
	}
} /* }}} */

void phpdbg_print_opline(zend_execute_data *execute_data, zend_bool ignore_flags TSRMLS_DC) /* {{{ */
{
	phpdbg_print_opline_ex(execute_data, NULL, ignore_flags TSRMLS_CC);
} /* }}} */

const char *phpdbg_decode_opcode(zend_uchar opcode) /* {{{ */
{
#if ZEND_EXTENSION_API_NO <= PHP_5_5_API_NO
#define CASE(s) case s: return #s
	switch (opcode) {
		CASE(ZEND_NOP);
		CASE(ZEND_ADD);
		CASE(ZEND_SUB);
		CASE(ZEND_MUL);
		CASE(ZEND_DIV);
		CASE(ZEND_MOD);
		CASE(ZEND_SL);
		CASE(ZEND_SR);
		CASE(ZEND_CONCAT);
		CASE(ZEND_BW_OR);
		CASE(ZEND_BW_AND);
		CASE(ZEND_BW_XOR);
		CASE(ZEND_BW_NOT);
		CASE(ZEND_BOOL_NOT);
		CASE(ZEND_BOOL_XOR);
		CASE(ZEND_IS_IDENTICAL);
		CASE(ZEND_IS_NOT_IDENTICAL);
		CASE(ZEND_IS_EQUAL);
		CASE(ZEND_IS_NOT_EQUAL);
		CASE(ZEND_IS_SMALLER);
		CASE(ZEND_IS_SMALLER_OR_EQUAL);
		CASE(ZEND_CAST);
		CASE(ZEND_QM_ASSIGN);
		CASE(ZEND_ASSIGN_ADD);
		CASE(ZEND_ASSIGN_SUB);
		CASE(ZEND_ASSIGN_MUL);
		CASE(ZEND_ASSIGN_DIV);
		CASE(ZEND_ASSIGN_MOD);
		CASE(ZEND_ASSIGN_SL);
		CASE(ZEND_ASSIGN_SR);
		CASE(ZEND_ASSIGN_CONCAT);
		CASE(ZEND_ASSIGN_BW_OR);
		CASE(ZEND_ASSIGN_BW_AND);
		CASE(ZEND_ASSIGN_BW_XOR);
		CASE(ZEND_PRE_INC);
		CASE(ZEND_PRE_DEC);
		CASE(ZEND_POST_INC);
		CASE(ZEND_POST_DEC);
		CASE(ZEND_ASSIGN);
		CASE(ZEND_ASSIGN_REF);
		CASE(ZEND_ECHO);
		CASE(ZEND_PRINT);
		CASE(ZEND_JMP);
		CASE(ZEND_JMPZ);
		CASE(ZEND_JMPNZ);
		CASE(ZEND_JMPZNZ);
		CASE(ZEND_JMPZ_EX);
		CASE(ZEND_JMPNZ_EX);
		CASE(ZEND_CASE);
		CASE(ZEND_SWITCH_FREE);
		CASE(ZEND_BRK);
		CASE(ZEND_CONT);
		CASE(ZEND_BOOL);
		CASE(ZEND_INIT_STRING);
		CASE(ZEND_ADD_CHAR);
		CASE(ZEND_ADD_STRING);
		CASE(ZEND_ADD_VAR);
		CASE(ZEND_BEGIN_SILENCE);
		CASE(ZEND_END_SILENCE);
		CASE(ZEND_INIT_FCALL_BY_NAME);
		CASE(ZEND_DO_FCALL);
		CASE(ZEND_DO_FCALL_BY_NAME);
		CASE(ZEND_RETURN);
		CASE(ZEND_RECV);
		CASE(ZEND_RECV_INIT);
		CASE(ZEND_SEND_VAL);
		CASE(ZEND_SEND_VAR);
		CASE(ZEND_SEND_REF);
		CASE(ZEND_NEW);
		CASE(ZEND_INIT_NS_FCALL_BY_NAME);
		CASE(ZEND_FREE);
		CASE(ZEND_INIT_ARRAY);
		CASE(ZEND_ADD_ARRAY_ELEMENT);
		CASE(ZEND_INCLUDE_OR_EVAL);
		CASE(ZEND_UNSET_VAR);
		CASE(ZEND_UNSET_DIM);
		CASE(ZEND_UNSET_OBJ);
		CASE(ZEND_FE_RESET);
		CASE(ZEND_FE_FETCH);
		CASE(ZEND_EXIT);
		CASE(ZEND_FETCH_R);
		CASE(ZEND_FETCH_DIM_R);
		CASE(ZEND_FETCH_OBJ_R);
		CASE(ZEND_FETCH_W);
		CASE(ZEND_FETCH_DIM_W);
		CASE(ZEND_FETCH_OBJ_W);
		CASE(ZEND_FETCH_RW);
		CASE(ZEND_FETCH_DIM_RW);
		CASE(ZEND_FETCH_OBJ_RW);
		CASE(ZEND_FETCH_IS);
		CASE(ZEND_FETCH_DIM_IS);
		CASE(ZEND_FETCH_OBJ_IS);
		CASE(ZEND_FETCH_FUNC_ARG);
		CASE(ZEND_FETCH_DIM_FUNC_ARG);
		CASE(ZEND_FETCH_OBJ_FUNC_ARG);
		CASE(ZEND_FETCH_UNSET);
		CASE(ZEND_FETCH_DIM_UNSET);
		CASE(ZEND_FETCH_OBJ_UNSET);
		CASE(ZEND_FETCH_DIM_TMP_VAR);
		CASE(ZEND_FETCH_CONSTANT);
		CASE(ZEND_GOTO);
		CASE(ZEND_EXT_STMT);
		CASE(ZEND_EXT_FCALL_BEGIN);
		CASE(ZEND_EXT_FCALL_END);
		CASE(ZEND_EXT_NOP);
		CASE(ZEND_TICKS);
		CASE(ZEND_SEND_VAR_NO_REF);
		CASE(ZEND_CATCH);
		CASE(ZEND_THROW);
		CASE(ZEND_FETCH_CLASS);
		CASE(ZEND_CLONE);
		CASE(ZEND_RETURN_BY_REF);
		CASE(ZEND_INIT_METHOD_CALL);
		CASE(ZEND_INIT_STATIC_METHOD_CALL);
		CASE(ZEND_ISSET_ISEMPTY_VAR);
		CASE(ZEND_ISSET_ISEMPTY_DIM_OBJ);
		CASE(ZEND_PRE_INC_OBJ);
		CASE(ZEND_PRE_DEC_OBJ);
		CASE(ZEND_POST_INC_OBJ);
		CASE(ZEND_POST_DEC_OBJ);
		CASE(ZEND_ASSIGN_OBJ);
		CASE(ZEND_INSTANCEOF);
		CASE(ZEND_DECLARE_CLASS);
		CASE(ZEND_DECLARE_INHERITED_CLASS);
		CASE(ZEND_DECLARE_FUNCTION);
		CASE(ZEND_RAISE_ABSTRACT_ERROR);
		CASE(ZEND_DECLARE_CONST);
		CASE(ZEND_ADD_INTERFACE);
		CASE(ZEND_DECLARE_INHERITED_CLASS_DELAYED);
		CASE(ZEND_VERIFY_ABSTRACT_CLASS);
		CASE(ZEND_ASSIGN_DIM);
		CASE(ZEND_ISSET_ISEMPTY_PROP_OBJ);
		CASE(ZEND_HANDLE_EXCEPTION);
		CASE(ZEND_USER_OPCODE);
#ifdef ZEND_JMP_SET
		CASE(ZEND_JMP_SET);
#endif
		CASE(ZEND_DECLARE_LAMBDA_FUNCTION);
#ifdef ZEND_ADD_TRAIT
		CASE(ZEND_ADD_TRAIT);
#endif
#ifdef ZEND_BIND_TRAITS
		CASE(ZEND_BIND_TRAITS);
#endif
#ifdef ZEND_SEPARATE
		CASE(ZEND_SEPARATE);
#endif
#ifdef ZEND_QM_ASSIGN_VAR
		CASE(ZEND_QM_ASSIGN_VAR);
#endif
#ifdef ZEND_JMP_SET_VAR
		CASE(ZEND_JMP_SET_VAR);
#endif
#ifdef ZEND_DISCARD_EXCEPTION
		CASE(ZEND_DISCARD_EXCEPTION);
#endif
#ifdef ZEND_YIELD
		CASE(ZEND_YIELD);
#endif
#ifdef ZEND_GENERATOR_RETURN
		CASE(ZEND_GENERATOR_RETURN);
#endif
#ifdef ZEND_FAST_CALL
		CASE(ZEND_FAST_CALL);
#endif
#ifdef ZEND_FAST_RET
		CASE(ZEND_FAST_RET);
#endif
#ifdef ZEND_RECV_VARIADIC
		CASE(ZEND_RECV_VARIADIC);
#endif
		CASE(ZEND_OP_DATA);
		default:
			return "UNKNOWN";
	}
#else
	const char *ret = zend_get_opcode_name(opcode);
	return ret?ret:"UNKNOWN";
#endif
} /* }}} */
