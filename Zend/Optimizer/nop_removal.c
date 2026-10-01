/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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

#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"

void zend_optimizer_nop_removal(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_op *opline;
	uint32_t new_count, i, shift;
	uint32_t *shiftlist;
	ALLOCA_FLAG(use_heap);

	shiftlist = (uint32_t *)do_alloca(sizeof(uint32_t) * op_array->last, use_heap);
	i = new_count = shift = 0;
	const zend_op *end = op_array->opcodes + op_array->last;
	for (opline = op_array->opcodes; opline < end; opline++) {

		/* Kill JMP-over-NOP-s */
		if (opline->opcode == ZEND_JMP && ZEND_OP1_JMP_ADDR(opline) > op_array->opcodes + i) {
			/* check if there are only NOPs under the branch */
			const zend_op *target = ZEND_OP1_JMP_ADDR(opline) - 1;

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

		/* update try/catch array */
		for (uint32_t j = 0; j < op_array->last_try_catch; j++) {
			op_array->try_catch_array[j].try_op -= shiftlist[op_array->try_catch_array[j].try_op];
			op_array->try_catch_array[j].catch_op -= shiftlist[op_array->try_catch_array[j].catch_op];
			if (op_array->try_catch_array[j].finally_op) {
				op_array->try_catch_array[j].finally_op -= shiftlist[op_array->try_catch_array[j].finally_op];
				op_array->try_catch_array[j].finally_end -= shiftlist[op_array->try_catch_array[j].finally_end];
			}
		}
	}
	free_alloca(shiftlist, use_heap);
}
