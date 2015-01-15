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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@zend.com>                             |
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

void zend_optimizer_nop_removal(zend_op_array *op_array)
{
	zend_op *end, *opline;
	uint32_t new_count, i, shift;
	int j;
	uint32_t *shiftlist;
	ALLOCA_FLAG(use_heap);

	shiftlist = (uint32_t *)DO_ALLOCA(sizeof(uint32_t) * op_array->last);
	i = new_count = shift = 0;
	end = op_array->opcodes + op_array->last;
	for (opline = op_array->opcodes; opline < end; opline++) {

		/* GOTO target is unresolved yet. We can't optimize. */
		if (opline->opcode == ZEND_GOTO &&
			Z_TYPE(ZEND_OP2_LITERAL(opline)) != IS_LONG) {
			/* TODO: in general we can avoid this restriction */
			FREE_ALLOCA(shiftlist);
			return;
		}

		/* Kill JMP-over-NOP-s */
		if (opline->opcode == ZEND_JMP && ZEND_OP1(opline).opline_num > i) {
			/* check if there are only NOPs under the branch */
			zend_op *target = op_array->opcodes + ZEND_OP1(opline).opline_num - 1;

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
				op_array->opcodes[new_count] = *opline;
			}
			new_count++;
		}
	}

	if (shift) {
		op_array->last = new_count;
		end = op_array->opcodes + op_array->last;

		/* update JMPs */
		for (opline = op_array->opcodes; opline<end; opline++) {
			switch (opline->opcode) {
				case ZEND_JMP:
				case ZEND_GOTO:
				case ZEND_FAST_CALL:
					ZEND_OP1(opline).opline_num -= shiftlist[ZEND_OP1(opline).opline_num];
					break;
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_FE_FETCH:
				case ZEND_FE_RESET:
				case ZEND_NEW:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
					ZEND_OP2(opline).opline_num -= shiftlist[ZEND_OP2(opline).opline_num];
					break;
				case ZEND_JMPZNZ:
					ZEND_OP2(opline).opline_num -= shiftlist[ZEND_OP2(opline).opline_num];
					opline->extended_value -= shiftlist[opline->extended_value];
					break;
				case ZEND_CATCH:
					opline->extended_value -= shiftlist[opline->extended_value];
					break;
			}
		}

		/* update brk/cont array */
		for (j = 0; j < op_array->last_brk_cont; j++) {
			op_array->brk_cont_array[j].brk -= shiftlist[op_array->brk_cont_array[j].brk];
			op_array->brk_cont_array[j].cont -= shiftlist[op_array->brk_cont_array[j].cont];
			op_array->brk_cont_array[j].start -= shiftlist[op_array->brk_cont_array[j].start];
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
		if (op_array->early_binding != (uint32_t)-1) {
			uint32_t *opline_num = &op_array->early_binding;

			do {
				*opline_num -= shiftlist[*opline_num];
				opline_num = &ZEND_RESULT(&op_array->opcodes[*opline_num]).opline_num;
			} while (*opline_num != (uint32_t)-1);
		}
	}
	FREE_ALLOCA(shiftlist);
}
