/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

static inline const char *phpdbg_decode_opcode(zend_uchar opcode) /* {{{ */
{
	const char *ret = zend_get_opcode_name(opcode);
	if (ret) {
		return ret + 5; /* Skip ZEND_ prefix */
	}
	return "UNKNOWN";
} /* }}} */

static inline char *phpdbg_decode_op(
		zend_op_array *ops, const zend_op *opline, const znode_op *op, uint32_t type) /* {{{ */
{
	char *decode = NULL;

	switch (type) {
		case IS_CV: {
			zend_string *var = ops->vars[EX_VAR_TO_NUM(op->var)];
			spprintf(&decode, 0, "$%.*s%c",
				ZSTR_LEN(var) <= 19 ? (int) ZSTR_LEN(var) : 18,
				ZSTR_VAL(var), ZSTR_LEN(var) <= 19 ? 0 : '+');
		} break;

		case IS_VAR:
			spprintf(&decode, 0, "@%u", EX_VAR_TO_NUM(op->var) - ops->last_var);
		break;
		case IS_TMP_VAR:
			spprintf(&decode, 0, "~%u", EX_VAR_TO_NUM(op->var) - ops->last_var);
		break;
		case IS_CONST: {
			zval *literal = RT_CONSTANT(opline, *op);
			decode = phpdbg_short_zval_print(literal, 20);
		} break;
	}
	return decode;
} /* }}} */

char *phpdbg_decode_input_op(
		zend_op_array *ops, const zend_op *opline, znode_op op, zend_uchar op_type,
		uint32_t flags) {
	char *result = NULL;
	if (op_type != IS_UNUSED) {
		result = phpdbg_decode_op(ops, opline, &op, op_type);
	} else if (ZEND_VM_OP_JMP_ADDR == (flags & ZEND_VM_OP_MASK)) {
		spprintf(&result, 0, "J%td", OP_JMP_ADDR(opline, op) - ops->opcodes);
	} else if (ZEND_VM_OP_NUM == (flags & ZEND_VM_OP_MASK)) {
		spprintf(&result, 0, "%" PRIu32, op.num);
	} else if (ZEND_VM_OP_TRY_CATCH == (flags & ZEND_VM_OP_MASK)) {
		if (op.num != (uint32_t)-1) {
			spprintf(&result, 0, "try-catch(%" PRIu32 ")", op.num);
		}
	} else if (ZEND_VM_OP_THIS == (flags & ZEND_VM_OP_MASK)) {
		result = estrdup("THIS");
	} else if (ZEND_VM_OP_NEXT == (flags & ZEND_VM_OP_MASK)) {
		result = estrdup("NEXT");
	} else if (ZEND_VM_OP_CLASS_FETCH == (flags & ZEND_VM_OP_MASK)) {
		//zend_dump_class_fetch_type(op.num);
	} else if (ZEND_VM_OP_CONSTRUCTOR == (flags & ZEND_VM_OP_MASK)) {
		result = estrdup("CONSTRUCTOR");
	}
	return result;
}

char *phpdbg_decode_opline(zend_op_array *ops, zend_op *opline) /*{{{ */
{
	const char *opcode_name = phpdbg_decode_opcode(opline->opcode);
	uint32_t flags = zend_get_opcode_flags(opline->opcode);
	char *result, *decode[4] = {NULL, NULL, NULL, NULL};

	/* OpcodeName */
	if (opline->extended_value) {
		spprintf(&decode[0], 0, "%s<%" PRIi32 ">", opcode_name, opline->extended_value);
	}

	/* OP1 */
	decode[1] = phpdbg_decode_input_op(
		ops, opline, opline->op1, opline->op1_type, ZEND_VM_OP1_FLAGS(flags));

	/* OP2 */
	decode[2] = phpdbg_decode_input_op(
		ops, opline, opline->op2, opline->op2_type, ZEND_VM_OP2_FLAGS(flags));

	/* RESULT */
	switch (opline->opcode) {
	case ZEND_CATCH:
		if (opline->extended_value & ZEND_LAST_CATCH) {
			if (decode[2]) {
				efree(decode[2]);
				decode[2] = NULL;
			}
		}
		decode[3] = phpdbg_decode_op(ops, opline, &opline->result, opline->result_type);
		break;
	default:
		decode[3] = phpdbg_decode_op(ops, opline, &opline->result, opline->result_type);
		break;
	}

	spprintf(&result, 0,
		"%-23s %-20s %-20s %-20s",
		decode[0] ? decode[0] : opcode_name,
		decode[1] ? decode[1] : "",
		decode[2] ? decode[2] : "",
		decode[3] ? decode[3] : "");

	if (decode[0])
		efree(decode[0]);
	if (decode[1])
		efree(decode[1]);
	if (decode[2])
		efree(decode[2]);
	if (decode[3])
		efree(decode[3]);

	return result;
} /* }}} */

void phpdbg_print_opline_ex(zend_execute_data *execute_data, zend_bool ignore_flags) /* {{{ */
{
	/* force out a line while stepping so the user knows what is happening */
	if (ignore_flags ||
		(!(PHPDBG_G(flags) & PHPDBG_IS_QUIET) ||
		(PHPDBG_G(flags) & PHPDBG_IS_STEPPING) ||
		(PHPDBG_G(oplog)))) {

		zend_op *opline = (zend_op *) execute_data->opline;
		char *decode = phpdbg_decode_opline(&execute_data->func->op_array, opline);

		if (ignore_flags || (!(PHPDBG_G(flags) & PHPDBG_IS_QUIET) || (PHPDBG_G(flags) & PHPDBG_IS_STEPPING))) {
			/* output line info */
			phpdbg_notice("opline", "line=\"%u\" opline=\"%p\" op=\"%s\" file=\"%s\"", "L%-5u %16p %s %s",
			   opline->lineno,
			   opline,
			   decode,
			   execute_data->func->op_array.filename ? ZSTR_VAL(execute_data->func->op_array.filename) : "unknown");
		}

		if (!ignore_flags && PHPDBG_G(oplog)) {
			phpdbg_log_ex(fileno(PHPDBG_G(oplog)), "L%-5u %16p %s %s\n",
				opline->lineno,
				opline,
				decode,
				execute_data->func->op_array.filename ? ZSTR_VAL(execute_data->func->op_array.filename) : "unknown");
		}

		efree(decode);
	}

	if (PHPDBG_G(oplog_list)) {
		phpdbg_oplog_entry *cur = zend_arena_alloc(&PHPDBG_G(oplog_arena), sizeof(phpdbg_oplog_entry));
		zend_op_array *op_array = &execute_data->func->op_array;
		cur->op = (zend_op *) execute_data->opline;
		cur->opcodes = op_array->opcodes;
		cur->filename = op_array->filename;
		cur->scope = op_array->scope;
		cur->function_name = op_array->function_name;
		cur->next = NULL;
		PHPDBG_G(oplog_cur)->next = cur;
		PHPDBG_G(oplog_cur) = cur;
	}
} /* }}} */

void phpdbg_print_opline(zend_execute_data *execute_data, zend_bool ignore_flags) /* {{{ */
{
	phpdbg_print_opline_ex(execute_data, ignore_flags);
} /* }}} */
