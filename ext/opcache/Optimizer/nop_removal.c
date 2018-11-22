/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* pass 10:
 * - remove NOPs
 */

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"

void zend_optimizer_nop_removal(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_op *end, *opline;
	uint32_t new_count, i, shift;
	int j;
	uint32_t *shiftlist;
	ALLOCA_FLAG(use_heap);

	shiftlist = (uint32_t *)do_alloca(sizeof(uint32_t) * op_array->last, use_heap);
	i = new_count = shift = 0;
	end = op_array->opcodes + op_array->last;
	for (opline = op_array->opcodes; opline < end; opline++) {

		/* Kill JMP-over-NOP-s */
		if (opline->opcode == ZEND_JMP && ZEND_OP1_JMP_ADDR(opline) > op_array->opcodes + i) {
			/* check if there are only NOPs under the branch */
			zend_op *target = ZEND_OP1_JMP_ADDR(opline) - 1;

			while (target->opcode == ZEND_NOP) {
				target--;
			}
			if (target == opline) {
				/* only NOPs */
				opline->opcode = ZEND_NOP;
			}
		}

		shiftlist[i++] = shift;
		if (opline->opcode == ZEND_NOP) {
			shift++;
		} else {
			if (shift) {
				zend_op *new_opline = op_array->opcodes + new_count;

				*new_opline = *opline;
				zend_optimizer_migrate_jump(op_array, new_opline, opline);
			}
			new_count++;
		}
	}

	if (shift) {
		op_array->last = new_count;
		end = op_array->opcodes + op_array->last;

		/* update JMPs */
		for (opline = op_array->opcodes; opline<end; opline++) {
			zend_optimizer_shift_jump(op_array, opline, shiftlist);
		}

		/* update brk/cont array */
		for (j = 0; j < op_array->last_live_range; j++) {
			op_array->live_range[j].start -= shiftlist[op_array->live_range[j].start];
			op_array->live_range[j].end   -= shiftlist[op_array->live_range[j].end];
		}

		/* update try/catch array */
		for (j = 0; j < op_array->last_try_catch; j++) {
			op_array->try_catch_array[j].try_op -= shiftlist[op_array->try_catch_array[j].try_op];
			op_array->try_catch_array[j].catch_op -= shiftlist[op_array->try_catch_array[j].catch_op];
			if (op_array->try_catch_array[j].finally_op) {
				op_array->try_catch_array[j].finally_op -= shiftlist[op_array->try_catch_array[j].finally_op];
				op_array->try_catch_array[j].finally_end -= shiftlist[op_array->try_catch_array[j].finally_end];
			}
		}

		/* update early binding list */
		if (op_array->fn_flags & ZEND_ACC_EARLY_BINDING) {
			uint32_t *opline_num = &ctx->script->first_early_binding_opline;

			ZEND_ASSERT(op_array == &ctx->script->main_op_array);
			do {
				*opline_num -= shiftlist[*opline_num];
				opline_num = &op_array->opcodes[*opline_num].result.opline_num;
			} while (*opline_num != (uint32_t)-1);
		}
	}
	free_alloca(shiftlist, use_heap);
}
