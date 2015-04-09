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

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"

#define GET_AVAILABLE_T()		\
	for (i = 0; i < T; i++) {	\
		if (!taken_T[i]) {		\
			break;				\
		}						\
	}							\
	taken_T[i] = 1;				\
	if (i > max) {				\
		max = i;				\
	}

void optimize_temporary_variables(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	int T = op_array->T;
	int offset = op_array->last_var;
	char *taken_T;			/* T index in use */
	zend_op **start_of_T;	/* opline where T is first used */
	char *valid_T;			/* Is the map_T valid */
	int *map_T;				/* Map's the T to its new index */
	zend_op *opline, *end;
	int currT;
	int i;
	int max = -1;
	int var_to_free = -1;
	void *checkpoint = zend_arena_checkpoint(ctx->arena);

	taken_T = (char *) zend_arena_alloc(&ctx->arena, T);
	start_of_T = (zend_op **) zend_arena_alloc(&ctx->arena, T * sizeof(zend_op *));
	valid_T = (char *) zend_arena_alloc(&ctx->arena, T);
	map_T = (int *) zend_arena_alloc(&ctx->arena, T * sizeof(int));

    end = op_array->opcodes;
    opline = &op_array->opcodes[op_array->last - 1];

    /* Find T definition points */
    while (opline >= end) {
        if (ZEND_RESULT_TYPE(opline) & (IS_VAR | IS_TMP_VAR)) {
			start_of_T[VAR_NUM(ZEND_RESULT(opline).var) - offset] = opline;
		}
		opline--;
	}

	memset(valid_T, 0, T);
	memset(taken_T, 0, T);

    end = op_array->opcodes;
    opline = &op_array->opcodes[op_array->last - 1];

    while (opline >= end) {
		if ((ZEND_OP1_TYPE(opline) & (IS_VAR | IS_TMP_VAR))) {

			currT = VAR_NUM(ZEND_OP1(opline).var) - offset;
			if (!valid_T[currT]) {
				GET_AVAILABLE_T();
				map_T[currT] = i;
				valid_T[currT] = 1;
			}
			ZEND_OP1(opline).var = NUM_VAR(map_T[currT] + offset);
		}

		/* Skip OP_DATA */
		if (opline->opcode == ZEND_OP_DATA &&
		    (opline-1)->opcode == ZEND_ASSIGN_DIM) {
		    opline--;
		    continue;
		}

		if ((ZEND_OP2_TYPE(opline) & (IS_VAR | IS_TMP_VAR))) {
			currT = VAR_NUM(ZEND_OP2(opline).var) - offset;
			if (!valid_T[currT]) {
				GET_AVAILABLE_T();
				map_T[currT] = i;
				valid_T[currT] = 1;
			}
			ZEND_OP2(opline).var = NUM_VAR(map_T[currT] + offset);
		}

		if (opline->opcode == ZEND_DECLARE_INHERITED_CLASS ||
            opline->opcode == ZEND_DECLARE_INHERITED_CLASS_DELAYED) {
			currT = VAR_NUM(opline->extended_value) - offset;
			if (!valid_T[currT]) {
				GET_AVAILABLE_T();
				map_T[currT] = i;
				valid_T[currT] = 1;
			}
			opline->extended_value = NUM_VAR(map_T[currT] + offset);
		}

		/* Allocate OP_DATA->op2 after "operands", but before "result" */
		if (opline->opcode == ZEND_ASSIGN_DIM &&
		    (opline + 1)->opcode == ZEND_OP_DATA &&
		    ZEND_OP2_TYPE(opline + 1) & (IS_VAR | IS_TMP_VAR)) {
			currT = VAR_NUM(ZEND_OP2(opline + 1).var) - offset;
			GET_AVAILABLE_T();
			map_T[currT] = i;
			valid_T[currT] = 1;
			taken_T[i] = 0;
			ZEND_OP2(opline + 1).var = NUM_VAR(i + offset);
			var_to_free = i;
		}

		if (ZEND_RESULT_TYPE(opline) & (IS_VAR | IS_TMP_VAR)) {
			currT = VAR_NUM(ZEND_RESULT(opline).var) - offset;
			if (valid_T[currT]) {
				if (start_of_T[currT] == opline) {
					taken_T[map_T[currT]] = 0;
				}
				ZEND_RESULT(opline).var = NUM_VAR(map_T[currT] + offset);
			} else { /* Au still needs to be assigned a T which is a bit dumb. Should consider changing Zend */
				GET_AVAILABLE_T();

				if (RESULT_UNUSED(opline)) {
					taken_T[i] = 0;
				} else {
					/* Code which gets here is using a wrongly built opcode such as RECV() */
					map_T[currT] = i;
					valid_T[currT] = 1;
				}
				ZEND_RESULT(opline).var = NUM_VAR(i + offset);
			}
		}

		if (var_to_free >= 0) {
			taken_T[var_to_free] = 0;
			var_to_free = -1;
		}

		opline--;
	}

	zend_arena_release(&ctx->arena, checkpoint);
	op_array->T = max + 1;
}
