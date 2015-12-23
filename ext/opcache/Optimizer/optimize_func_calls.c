/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   |          Xinchen Hui <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* pass 4
 * - optimize INIT_FCALL_BY_NAME to DO_FCALL
 */

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"

#define ZEND_OP2_IS_CONST_STRING(opline) \
	(ZEND_OP2_TYPE(opline) == IS_CONST && \
	Z_TYPE(op_array->literals[(opline)->op2.constant]) == IS_STRING)

typedef struct _optimizer_call_info {
	zend_function *func;
	zend_op       *opline;
} optimizer_call_info;

void optimize_func_calls(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	int call = 0;
	void *checkpoint;
	optimizer_call_info *call_stack;
	zend_bool had_fcall = 0;

	if (op_array->last < 2) {
		return;
	}

	checkpoint = zend_arena_checkpoint(ctx->arena);
	call_stack = zend_arena_calloc(&ctx->arena, op_array->last / 2, sizeof(optimizer_call_info));
	while (opline < end) {
		switch (opline->opcode) {
			case ZEND_INIT_FCALL_BY_NAME:
			case ZEND_INIT_NS_FCALL_BY_NAME:
				if (ZEND_OP2_IS_CONST_STRING(opline)) {
					zend_function *func;
					zval *function_name = &op_array->literals[opline->op2.constant + 1];
					if ((func = zend_hash_find_ptr(&ctx->script->function_table,
							Z_STR_P(function_name))) != NULL) {
						call_stack[call].func = func;
					}
				}
				/* break missing intentionally */
			case ZEND_NEW:
			case ZEND_INIT_DYNAMIC_CALL:
			case ZEND_INIT_METHOD_CALL:
			case ZEND_INIT_STATIC_METHOD_CALL:
			case ZEND_INIT_FCALL:
			case ZEND_INIT_USER_CALL:
				call_stack[call].opline = opline;
				call++;
				break;
			case ZEND_DO_FCALL:
			case ZEND_DO_ICALL:
			case ZEND_DO_UCALL:
			case ZEND_DO_FCALL_BY_NAME:
			case ZEND_DO_UNPACK_FCALL:
				call--;
				had_fcall = call != 0; /* we must not replace ZEND_SEND_VAL after a DO_FCALL inside nested calls */
				if (call_stack[call].func && call_stack[call].opline) {
					zend_op *fcall = call_stack[call].opline;

					if (fcall->opcode == ZEND_INIT_FCALL_BY_NAME) {
						fcall->opcode = ZEND_INIT_FCALL;
						Z_CACHE_SLOT(op_array->literals[fcall->op2.constant + 1]) = Z_CACHE_SLOT(op_array->literals[fcall->op2.constant]);
						literal_dtor(&ZEND_OP2_LITERAL(fcall));
						fcall->op2.constant = fcall->op2.constant + 1;
						if (opline->opcode != ZEND_DO_UNPACK_FCALL) {
							opline->opcode = zend_get_call_op(ZEND_INIT_FCALL, call_stack[call].func);
						}
					} else if (fcall->opcode == ZEND_INIT_NS_FCALL_BY_NAME) {
						fcall->opcode = ZEND_INIT_FCALL;
						Z_CACHE_SLOT(op_array->literals[fcall->op2.constant + 1]) = Z_CACHE_SLOT(op_array->literals[fcall->op2.constant]);
						literal_dtor(&op_array->literals[fcall->op2.constant]);
						literal_dtor(&op_array->literals[fcall->op2.constant + 2]);
						fcall->op2.constant = fcall->op2.constant + 1;
						if (opline->opcode != ZEND_DO_UNPACK_FCALL) {
							opline->opcode = zend_get_call_op(ZEND_INIT_FCALL, call_stack[call].func);
						}
					} else {
						ZEND_ASSERT(0);
					}
					if (opline->opcode == ZEND_DO_UCALL) {
						opline->op2.num = zend_vm_calc_used_stack(fcall->extended_value, call_stack[call].func);
					}
				}
				call_stack[call].func = NULL;
				call_stack[call].opline = NULL;
				break;
			case ZEND_FETCH_FUNC_ARG:
			case ZEND_FETCH_STATIC_PROP_FUNC_ARG:
			case ZEND_FETCH_OBJ_FUNC_ARG:
			case ZEND_FETCH_DIM_FUNC_ARG:
				if (call_stack[call - 1].func) {
					if (opline->opcode == ZEND_FETCH_FUNC_ARG) {
						opline->op2_type = IS_UNUSED;
					} else {
						/* NOP the ZEND_OP_DATA out */
						MAKE_NOP(opline + 1);
					}
					if (ARG_SHOULD_BE_SENT_BY_REF(call_stack[call - 1].func, (opline->extended_value & ZEND_FETCH_ARG_MASK))) {
						opline->extended_value &= ZEND_FETCH_TYPE_MASK;
						if (opline->opcode != ZEND_FETCH_STATIC_PROP_FUNC_ARG) {
							opline->opcode -= 9;
						} else {
							opline->opcode = ZEND_FETCH_STATIC_PROP_W;
						}
					} else {
						opline->extended_value &= ZEND_FETCH_TYPE_MASK;
						if (opline->opcode != ZEND_FETCH_STATIC_PROP_FUNC_ARG) {
							opline->opcode -= 12;
						} else {
							opline->opcode = ZEND_FETCH_STATIC_PROP_R;
						}
					}
				}
				break;
			case ZEND_SEND_VAL_EX:
				if (!call_stack[call - 1].func) {
					break;
				}
				if (ARG_MUST_BE_SENT_BY_REF(call_stack[call - 1].func, opline->op2.num)) {
					/* We won't convert it into_DO_FCALL to emit error at run-time */
					call_stack[call - 1].opline = NULL;
					break;
				}

				opline->opcode = ZEND_SEND_VAL;
				/* break intentionally missing */
			case ZEND_SEND_VAL:
				if (!had_fcall && opline->op1_type == IS_TMP_VAR) {
					int i, result = -1, src = -1;
					zend_op *orig = opline;
					while (--orig >= op_array->opcodes) {
						if (orig->op1.var == opline->op1.var && orig->op1_type == IS_TMP_VAR) {
							orig->op1.var = opline->result.var;
						}
						if (orig->op2.var == opline->op1.var && orig->op2_type == IS_TMP_VAR) {
							orig->op2.var = opline->result.var;
						}
						if (orig->result.var == opline->op1.var && orig->result_type == IS_TMP_VAR) {
							orig->result.var = opline->result.var;
						}
					}
					for (i = 0; i < op_array->last_live_range; i++) {
						if ((op_array->live_range[i].var & ZEND_LIVE_MASK) == opline->op1.var) {
							src = i;
						}
						if ((op_array->live_range[i].var & ZEND_LIVE_MASK) == opline->result.var) {
							result = i;
						}
					}
					if (!src && !result) {
						/* we're done, the temporary is immediately followed by the fcall executing opcode */
					} else if (src && result) {
						op_array->live_range[result].start = op_array->live_range[src].start;
						zend_optimizer_remove_live_range(op_array, opline->op1.var);
					} else if (src) {
						op_array->live_range[src].end++;
					} else if (result) {
						op_array->live_range[result].start--;
					}
					MAKE_NOP(opline);
				}
				break;
			case ZEND_SEND_VAR_EX:
				if (call_stack[call - 1].func) {
					if (ARG_SHOULD_BE_SENT_BY_REF(call_stack[call - 1].func, opline->op2.num)) {
						opline->opcode = ZEND_SEND_REF;
					} else {
						opline->opcode = ZEND_SEND_VAR;
					}
				}
				break;
			case ZEND_SEND_VAR_NO_REF:
				if (!(opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) && call_stack[call - 1].func) {
					if (ARG_SHOULD_BE_SENT_BY_REF(call_stack[call - 1].func, opline->op2.num)) {
						opline->extended_value |= ZEND_ARG_COMPILE_TIME_BOUND | ZEND_ARG_SEND_BY_REF;
					} else {
						opline->opcode = ZEND_SEND_VAR;
						opline->extended_value = 0;
					}
				}
				break;
#if 0
			case ZEND_SEND_REF:
				if (opline->extended_value != ZEND_ARG_COMPILE_TIME_BOUND && call_stack[call - 1].func) {
					/* We won't handle run-time pass by reference */
					call_stack[call - 1].opline = NULL;
				}
				break;
#endif
			default:
				break;
		}
		opline++;
	}

	zend_arena_release(&ctx->arena, checkpoint);
}
