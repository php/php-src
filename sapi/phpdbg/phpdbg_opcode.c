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

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static inline uint32_t phpdbg_decode_literal(zend_op_array *ops, zval *literal) /* {{{ */
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

static inline char *phpdbg_decode_op(zend_op_array *ops, znode_op *op, uint32_t type, HashTable *vars) /* {{{ */
{
	char *decode = NULL;

	switch (type &~ EXT_TYPE_UNUSED) {
		case IS_CV:
			asprintf(&decode, "$%s", ops->vars[EX_VAR_TO_NUM(op->var)]->val);
		break;

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

		case IS_CONST:
			asprintf(&decode, "C%u", phpdbg_decode_literal(ops, RT_CONSTANT(ops, *op)));
		break;

		case IS_UNUSED:
			asprintf(&decode, "<unused>");
		break;
	}
	return decode;
} /* }}} */

char *phpdbg_decode_opline(zend_op_array *ops, zend_op *op, HashTable *vars) /*{{{ */
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
			asprintf(&decode[1], "J%ld", OP_JMP_ADDR(op, op->op1) - ops->opcodes);
		goto format;

	case ZEND_JMPZNZ:
			decode[1] = phpdbg_decode_op(ops, &op->op1, op->op1_type, vars);
			asprintf(&decode[2], "J%u or J%" PRIu32, op->op2.opline_num, op->extended_value);
		goto result;

	case ZEND_JMPZ:
	case ZEND_JMPNZ:
	case ZEND_JMPZ_EX:
	case ZEND_JMPNZ_EX:

#ifdef ZEND_JMP_SET
	case ZEND_JMP_SET:
#endif
		decode[1] = phpdbg_decode_op(ops, &op->op1, op->op1_type, vars);
		asprintf(&decode[2], "J%ld", OP_JMP_ADDR(op, op->op2) - ops->opcodes);
	goto result;

	case ZEND_RECV_INIT:
		goto result;

		default: {
			decode[1] = phpdbg_decode_op(ops, &op->op1, op->op1_type, vars);
			decode[2] = phpdbg_decode_op(ops, &op->op2, op->op2_type, vars);
result:
			decode[3] = phpdbg_decode_op(ops, &op->result, op->result_type, vars);
format:
			asprintf(&decode[0],
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
			   execute_data->func->op_array.filename ? execute_data->func->op_array.filename->val : "unknown");
		}

		if (!ignore_flags && PHPDBG_G(oplog)) {
			phpdbg_log_ex(fileno(PHPDBG_G(oplog)), "L%-5u %16p %-30s %s %s",
				opline->lineno,
				opline,
				phpdbg_decode_opcode(opline->opcode),
				decode,
				execute_data->func->op_array.filename ? execute_data->func->op_array.filename->val : "unknown");
		}

		if (decode) {
			free(decode);
		}
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
