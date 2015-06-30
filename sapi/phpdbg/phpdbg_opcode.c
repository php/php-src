/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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
#include "ext/standard/php_string.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static inline char *phpdbg_decode_op(zend_op_array *ops, znode_op *op, uint32_t type, HashTable *vars) /* {{{ */
{
	char *decode = NULL;

	switch (type &~ EXT_TYPE_UNUSED) {
		case IS_CV: {
			zend_string *var = ops->vars[EX_VAR_TO_NUM(op->var)];
			asprintf(&decode, "$%.*s%c", ZSTR_LEN(var) <= 19 ? (int) ZSTR_LEN(var) : 18, ZSTR_VAL(var), ZSTR_LEN(var) <= 19 ? 0 : '+');
		} break;

		case IS_VAR:
		case IS_TMP_VAR: {
			zend_ulong id = 0, *pid = NULL;
			if (vars != NULL) {
				if ((pid = zend_hash_index_find_ptr(vars, (zend_ulong) ops->vars - op->var))) {
					id = *pid;
				} else {
					id = zend_hash_num_elements(vars);
					zend_hash_index_update_mem(vars, (zend_ulong) ops->vars - op->var, &id, sizeof(zend_ulong));
				}
			}
			asprintf(&decode, "@" ZEND_ULONG_FMT, id);
		} break;

		case IS_CONST: {
			zval *literal = RT_CONSTANT(ops, *op);
			decode = phpdbg_short_zval_print(literal, 20);
		} break;
	}
	return decode;
} /* }}} */

char *phpdbg_decode_opline(zend_op_array *ops, zend_op *op, HashTable *vars) /*{{{ */
{
	char *decode[4] = {NULL, NULL, NULL, NULL};

	/* OP1 */
	switch (op->opcode) {
	case ZEND_JMP:
	case ZEND_GOTO:
	case ZEND_FAST_CALL:
		asprintf(&decode[1], "J%ld", OP_JMP_ADDR(op, op->op1) - ops->opcodes);
		break;

	case ZEND_INIT_FCALL:
	case ZEND_RECV:
	case ZEND_RECV_INIT:
	case ZEND_RECV_VARIADIC:
		asprintf(&decode[1], "%" PRIu32, op->op1.num);
		break;

	default:
		decode[1] = phpdbg_decode_op(ops, &op->op1, op->op1_type, vars);
		break;
	}

	/* OP2 */
	switch (op->opcode) {
	/* TODO: ZEND_FAST_CALL, ZEND_FAST_RET op2 */
	case ZEND_JMPZNZ:
		asprintf(&decode[2], "J%u or J%" PRIu32, op->op2.opline_num, op->extended_value);
		break;

	case ZEND_JMPZ:
	case ZEND_JMPNZ:
	case ZEND_JMPZ_EX:
	case ZEND_JMPNZ_EX:
	case ZEND_JMP_SET:
	case ZEND_ASSERT_CHECK:
		asprintf(&decode[2], "J%ld", OP_JMP_ADDR(op, op->op2) - ops->opcodes);
		break;

	case ZEND_SEND_VAL:
	case ZEND_SEND_VAL_EX:
	case ZEND_SEND_VAR:
	case ZEND_SEND_VAR_NO_REF:
	case ZEND_SEND_REF:
	case ZEND_SEND_VAR_EX:
	case ZEND_SEND_USER:
		asprintf(&decode[2], "%" PRIu32, op->op2.num);
		break;

	default:
		decode[2] = phpdbg_decode_op(ops, &op->op2, op->op2_type, vars);
		break;
	}

	/* RESULT */
	switch (op->opcode) {
	case ZEND_CATCH:
		asprintf(&decode[2], "%" PRIu32, op->result.num);
		break;
	default:
		decode[3] = phpdbg_decode_op(ops, &op->result, op->result_type, vars);
		break;
	}

	asprintf(&decode[0],
		"%-20s %-20s %-20s",
		decode[1] ? decode[1] : "",
		decode[2] ? decode[2] : "",
		decode[3] ? decode[3] : "");

	if (decode[1])
		free(decode[1]);
	if (decode[2])
		free(decode[2]);
	if (decode[3])
		free(decode[3]);

	return decode[0];
} /* }}} */

void phpdbg_print_opline_ex(zend_execute_data *execute_data, HashTable *vars, zend_bool ignore_flags) /* {{{ */
{
	/* force out a line while stepping so the user knows what is happening */
	if (ignore_flags ||
		(!(PHPDBG_G(flags) & PHPDBG_IS_QUIET) ||
		(PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ||
		(PHPDBG_G(oplog)))) {

		zend_op *opline = (zend_op *) execute_data->opline;
		char *decode = phpdbg_decode_opline(&execute_data->func->op_array, opline, vars);

		if (ignore_flags || (!(PHPDBG_G(flags) & PHPDBG_IS_QUIET) || (PHPDBG_G(flags) & PHPDBG_IS_STEPPING))) {
			/* output line info */
			phpdbg_notice("opline", "line=\"%u\" opline=\"%p\" opcode=\"%s\" op=\"%s\" file=\"%s\"", "L%-5u %16p %-30s %s %s",
			   opline->lineno,
			   opline,
			   phpdbg_decode_opcode(opline->opcode),
			   decode,
			   execute_data->func->op_array.filename ? ZSTR_VAL(execute_data->func->op_array.filename) : "unknown");
		}

		if (!ignore_flags && PHPDBG_G(oplog)) {
			phpdbg_log_ex(fileno(PHPDBG_G(oplog)), "L%-5u %16p %-30s %s %s",
				opline->lineno,
				opline,
				phpdbg_decode_opcode(opline->opcode),
				decode,
				execute_data->func->op_array.filename ? ZSTR_VAL(execute_data->func->op_array.filename) : "unknown");
		}

		if (decode) {
			free(decode);
		}
	}

	if (PHPDBG_G(oplog_list)) {
		phpdbg_oplog_entry *cur = zend_arena_alloc(&PHPDBG_G(oplog_arena), sizeof(phpdbg_oplog_entry));
		cur->op = (zend_op *) execute_data->opline;
		cur->op_array = &execute_data->func->op_array;
		cur->next = NULL;
		PHPDBG_G(oplog_cur)->next = cur;
		PHPDBG_G(oplog_cur) = cur;
	}
} /* }}} */

void phpdbg_print_opline(zend_execute_data *execute_data, zend_bool ignore_flags) /* {{{ */
{
	phpdbg_print_opline_ex(execute_data, NULL, ignore_flags);
} /* }}} */

const char *phpdbg_decode_opcode(zend_uchar opcode) /* {{{ */
{
	const char *ret = zend_get_opcode_name(opcode);
	return ret?ret:"UNKNOWN";
} /* }}} */
