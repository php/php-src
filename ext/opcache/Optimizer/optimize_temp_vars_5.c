/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
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
#include "zend_bitset.h"

#define GET_AVAILABLE_T()					\
	for (i = 0; i < T; i++) {				\
		if (!zend_bitset_in(taken_T, i)) {	\
			break;							\
		}									\
	}										\
	zend_bitset_incl(taken_T, i);			\
	if (i > max) {							\
		max = i;							\
	}

void optimize_temporary_variables(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	int T = op_array->T;
	int offset = op_array->last_var;
	uint32_t bitset_len;
	zend_bitset taken_T;	/* T index in use */
	zend_op **start_of_T;	/* opline where T is first used */
	zend_bitset valid_T;	/* Is the map_T valid */
	int *map_T;				/* Map's the T to its new index */
	zend_op *opline, *end;
	int currT;
	int i;
	int max = -1;
	int var_to_free = -1;
	void *checkpoint = zend_arena_checkpoint(ctx->arena);

	bitset_len = zend_bitset_len(T);
	taken_T = (zend_bitset) zend_arena_alloc(&ctx->arena, bitset_len * ZEND_BITSET_ELM_SIZE);
	start_of_T = (zend_op **) zend_arena_alloc(&ctx->arena, T * sizeof(zend_op *));
	valid_T = (zend_bitset) zend_arena_alloc(&ctx->arena, bitset_len * ZEND_BITSET_ELM_SIZE);
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

	zend_bitset_clear(valid_T, bitset_len);
	zend_bitset_clear(taken_T, bitset_len);

    end = op_array->opcodes;
    opline = &op_array->opcodes[op_array->last - 1];

    while (opline >= end) {
		if ((ZEND_OP1_TYPE(opline) & (IS_VAR | IS_TMP_VAR))) {
			currT = VAR_NUM(ZEND_OP1(opline).var) - offset;
			if (opline->opcode == ZEND_ROPE_END) {
				int num = (((opline->extended_value + 1) * sizeof(zend_string*)) + (sizeof(zval) - 1)) / sizeof(zval);
				int var;

				var = max;
				while (var >= 0 && !zend_bitset_in(taken_T, var)) {
					var--;
				}
				max = MAX(max, var + num);
				var = var + 1;
				map_T[currT] = var;
				zend_bitset_incl(valid_T, currT);
				zend_bitset_incl(taken_T, var);
				ZEND_OP1(opline).var = NUM_VAR(var + offset);
				while (num > 1) {
					num--;
					zend_bitset_incl(taken_T, var + num);
				}
			} else {
				if (!zend_bitset_in(valid_T, currT)) {
					int use_new_var = 0;

					/* Code in "finally" blocks may modify temorary variables.
					 * We allocate new temporaries for values that need to
					 * relive FAST_CALLs.
					 */
					if ((op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) &&
					    (opline->opcode == ZEND_RETURN ||
					     opline->opcode == ZEND_RETURN_BY_REF ||
					     opline->opcode == ZEND_FREE ||
					     opline->opcode == ZEND_FE_FREE)) {
						zend_op *curr = opline;

						while (--curr >= end) {
							if (curr->opcode == ZEND_FAST_CALL) {
								use_new_var = 1;
								break;
							} else if (curr->opcode != ZEND_FREE &&
							           curr->opcode != ZEND_FE_FREE &&
							           curr->opcode != ZEND_VERIFY_RETURN_TYPE &&
							           curr->opcode != ZEND_DISCARD_EXCEPTION) {
								break;
							}
						}
					}
					if (use_new_var) {
						i = ++max;
						zend_bitset_incl(taken_T, i);
					} else {
						GET_AVAILABLE_T();
					}
					map_T[currT] = i;
					zend_bitset_incl(valid_T, currT);
				}
				ZEND_OP1(opline).var = NUM_VAR(map_T[currT] + offset);
			}
		}

		/* Skip OP_DATA */
		if (opline->opcode == ZEND_OP_DATA &&
		    (opline-1)->opcode == ZEND_ASSIGN_DIM) {
		    opline--;
		    continue;
		}

		if ((ZEND_OP2_TYPE(opline) & (IS_VAR | IS_TMP_VAR))) {
			currT = VAR_NUM(ZEND_OP2(opline).var) - offset;
			if (!zend_bitset_in(valid_T, currT)) {
				GET_AVAILABLE_T();
				map_T[currT] = i;
				zend_bitset_incl(valid_T, currT);
			}
			ZEND_OP2(opline).var = NUM_VAR(map_T[currT] + offset);
		}

		if (opline->opcode == ZEND_DECLARE_INHERITED_CLASS ||
		    opline->opcode == ZEND_DECLARE_ANON_INHERITED_CLASS ||
            opline->opcode == ZEND_DECLARE_INHERITED_CLASS_DELAYED) {
			currT = VAR_NUM(opline->extended_value) - offset;
			if (!zend_bitset_in(valid_T, currT)) {
				GET_AVAILABLE_T();
				map_T[currT] = i;
				zend_bitset_incl(valid_T, currT);
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
			zend_bitset_incl(valid_T, currT);
			zend_bitset_excl(taken_T, i);
			ZEND_OP2(opline + 1).var = NUM_VAR(i + offset);
			var_to_free = i;
		}

		if (ZEND_RESULT_TYPE(opline) & (IS_VAR | IS_TMP_VAR)) {
			currT = VAR_NUM(ZEND_RESULT(opline).var) - offset;
			if (zend_bitset_in(valid_T, currT)) {
				if (start_of_T[currT] == opline) {
					/* ZEND_FAST_CALL can not share temporary var with others
					 * since the fast_var could also be set by ZEND_HANDLE_EXCEPTION
					 * which could be ahead of it */
					if (opline->opcode != ZEND_FAST_CALL) {
						zend_bitset_excl(taken_T, map_T[currT]);
					}
				}
				ZEND_RESULT(opline).var = NUM_VAR(map_T[currT] + offset);
				if (opline->opcode == ZEND_ROPE_INIT) {
					if (start_of_T[currT] == opline) {
						uint32_t num = ((opline->extended_value * sizeof(zend_string*)) + (sizeof(zval) - 1)) / sizeof(zval);
						while (num > 1) {
							num--;
							zend_bitset_excl(taken_T, map_T[currT]+num);
						}
					}
				}
			} else { /* Au still needs to be assigned a T which is a bit dumb. Should consider changing Zend */
				GET_AVAILABLE_T();

				if (RESULT_UNUSED(opline)) {
					zend_bitset_excl(taken_T, i);
				} else {
					/* Code which gets here is using a wrongly built opcode such as RECV() */
					map_T[currT] = i;
					zend_bitset_incl(valid_T, currT);
				}
				ZEND_RESULT(opline).var = NUM_VAR(i + offset);
			}
		}

		if (var_to_free >= 0) {
			zend_bitset_excl(taken_T, var_to_free);
			var_to_free = -1;
		}

		opline--;
	}

	zend_arena_release(&ctx->arena, checkpoint);
	op_array->T = max + 1;
}
