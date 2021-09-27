/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* pass 3: (Jump optimization)
 * - optimize series of JMPs
 */

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"

/* we use "jmp_hitlist" to avoid infinity loops during jmp optimization */
static zend_always_inline int in_hitlist(zend_op *target, zend_op **jmp_hitlist, int jmp_hitlist_count)
{
	int i;

	for (i = 0; i < jmp_hitlist_count; i++) {
		if (jmp_hitlist[i] == target) {
			return 1;
		}
	}
	return 0;
}

#define CHECK_LOOP(target) \
	if (EXPECTED(!in_hitlist(target, jmp_hitlist, jmp_hitlist_count))) { \
		jmp_hitlist[jmp_hitlist_count++] = target;	\
	} else { \
		break; \
	}

void zend_optimizer_pass3(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_op *opline;
	zend_op *end;
	zend_op *target;
	zend_op **jmp_hitlist;
	int jmp_hitlist_count;
	ALLOCA_FLAG(use_heap);

	jmp_hitlist = (zend_op**)do_alloca(sizeof(zend_op*)*op_array->last, use_heap);
	opline = op_array->opcodes;
	end =  opline + op_array->last;

	while (opline < end) {

		switch (opline->opcode) {
			case ZEND_JMP:
				jmp_hitlist_count = 0;

				target = ZEND_OP1_JMP_ADDR(opline);
				while (1) {
					if (target->opcode == ZEND_JMP) {
						/* convert JMP L1 ... L1: JMP L2 to JMP L2 .. L1: JMP L2 */
						target = ZEND_OP1_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if (target->opcode == ZEND_NOP) {
						target = target + 1;
					} else {
						break;
					}
					ZEND_SET_OP_JMP_ADDR(opline, opline->op1, target);
				}

				if (target == opline + 1) {
					/* convert L: JMP L+1 to NOP */
					MAKE_NOP(opline);
				} else if (target->opcode == ZEND_JMPZNZ) {
					/* JMP L, L: JMPZNZ L1,L2 -> JMPZNZ L1,L2 */
					*opline = *target;
					if (opline->op1_type == IS_CONST) {
						zval zv;
						ZVAL_COPY(&zv, &ZEND_OP1_LITERAL(opline));
						opline->op1.constant = zend_optimizer_add_literal(op_array, &zv);
					}
					/* Jump addresses may be encoded as offsets, recompute them. */
					ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP2_JMP_ADDR(target));
					opline->extended_value = ZEND_OPLINE_TO_OFFSET(opline,
						ZEND_OFFSET_TO_OPLINE(target, target->extended_value));
					goto optimize_jmpznz;
				} else if ((target->opcode == ZEND_RETURN ||
				            target->opcode == ZEND_RETURN_BY_REF ||
				            target->opcode == ZEND_GENERATOR_RETURN ||
				            target->opcode == ZEND_EXIT) &&
				           !(op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK)) {
					/* JMP L, L: RETURN to immediate RETURN */
					*opline = *target;
					if (opline->op1_type == IS_CONST) {
						zval zv;
						ZVAL_COPY(&zv, &ZEND_OP1_LITERAL(opline));
						opline->op1.constant = zend_optimizer_add_literal(op_array, &zv);
					}
				} else if (opline > op_array->opcodes &&
				           ((opline-1)->opcode == ZEND_JMPZ ||
				            (opline-1)->opcode == ZEND_JMPNZ)) {
				    if (ZEND_OP2_JMP_ADDR(opline-1) == target) {
						/* JMPZ(X,L1), JMP(L1) -> NOP, JMP(L1) */
						if ((opline-1)->op1_type == IS_CV) {
							(opline-1)->opcode = ZEND_CHECK_VAR;
							(opline-1)->op2.num = 0;
						} else if ((opline-1)->op1_type & (IS_TMP_VAR|IS_VAR)) {
							(opline-1)->opcode = ZEND_FREE;
							(opline-1)->op2.num = 0;
						} else {
							MAKE_NOP(opline-1);
						}
				    } else {
						/* JMPZ(X,L1), JMP(L2) -> JMPZNZ(X,L1,L2) */
						if ((opline-1)->opcode == ZEND_JMPZ) {
							(opline-1)->extended_value = ZEND_OPLINE_TO_OFFSET((opline-1), target);
						} else {
							(opline-1)->extended_value = ZEND_OPLINE_TO_OFFSET((opline-1), ZEND_OP2_JMP_ADDR(opline-1));
							ZEND_SET_OP_JMP_ADDR((opline-1), (opline-1)->op2, target);
						}
						(opline-1)->opcode = ZEND_JMPZNZ;
				    }
				}
				break;

			case ZEND_JMP_SET:
			case ZEND_COALESCE:
				jmp_hitlist_count = 0;

				target = ZEND_OP2_JMP_ADDR(opline);
				while (1) {
					if (target->opcode == ZEND_JMP) {
						target = ZEND_OP1_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if (target->opcode == ZEND_NOP) {
						target = target + 1;
					} else {
						break;
					}
					ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
				}
				break;

			case ZEND_JMPZ:
			case ZEND_JMPNZ:
				jmp_hitlist_count = 0;

				target = ZEND_OP2_JMP_ADDR(opline);
				while (1) {
					if (target->opcode == ZEND_JMP) {
						/* plain JMP */
						/* JMPZ(X,L1), L1: JMP(L2) => JMPZ(X,L2), L1: JMP(L2) */
						target = ZEND_OP1_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if (target->opcode == opline->opcode &&
					           SAME_VAR(opline->op1, target->op1)) {
						/* same opcode and same var as this opcode */
						/* JMPZ(X,L1), L1: JMPZ(X,L2) => JMPZ(X,L2), L1: JMPZ(X,L2) */
						target = ZEND_OP2_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if (target->opcode == INV_COND(opline->opcode) &&
					           SAME_VAR(opline->op1, target->op1)) {
						/* convert JMPZ(X,L1), L1: JMPNZ(X,L2) to
						   JMPZ(X,L1+1) */
						target = target + 1;
					} else if (target->opcode == ZEND_JMPZNZ &&
					           SAME_VAR(opline->op1, target->op1)) {
						target = (opline->opcode == ZEND_JMPZ) ?
							ZEND_OP2_JMP_ADDR(target) :
							ZEND_OFFSET_TO_OPLINE(target, target->extended_value);
						CHECK_LOOP(target);
					} else if (target->opcode == ZEND_NOP) {
						target = target + 1;
					} else {
						break;
					}
					ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
				}

				/* convert L: JMPZ L+1 to NOP */
				if (target == opline + 1) {
					if (opline->op1_type == IS_CV) {
						opline->opcode = ZEND_CHECK_VAR;
						opline->op2.num = 0;
					} else if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
						opline->opcode = ZEND_FREE;
						opline->op2.num = 0;
					} else {
						MAKE_NOP(opline);
					}
				}
				break;

			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
				jmp_hitlist_count = 0;

				target = ZEND_OP2_JMP_ADDR(opline);
				while (1) {
					if (target->opcode == ZEND_JMP) {
						/* plain JMP */
						/* JMPZ_EX(X,L1), L1: JMP(L2) => JMPZ_EX(X,L2), L1: JMP(L2) */
						target = ZEND_OP1_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if (target->opcode == opline->opcode-3 &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1))) {
						/* convert T=JMPZ_EX(X,L1), L1: JMPZ(T,L2) to
						   JMPZ_EX(X,L2) */
						target = ZEND_OP2_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if (target->opcode == opline->opcode &&
					           target->result.var == opline->result.var &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1))) {
						/* convert T=JMPZ_EX(X,L1), L1: T=JMPZ_EX(T,L2) to
						   JMPZ_EX(X,L2) */
						target = ZEND_OP2_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if (target->opcode == ZEND_JMPZNZ &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1))) {
						/* Check for JMPZNZ with same cond variable */
						target = (opline->opcode == ZEND_JMPZ_EX) ?
							ZEND_OP2_JMP_ADDR(target) :
							ZEND_OFFSET_TO_OPLINE(target, target->extended_value);
						CHECK_LOOP(target);
					} else if (target->opcode == INV_EX_COND(opline->opcode) &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1))) {
					   /* convert T=JMPZ_EX(X,L1), L1: JMPNZ(T,L2) to
						  JMPZ_EX(X,L1+1) */
						target = target + 1;
					} else if (target->opcode == INV_EX_COND_EX(opline->opcode) &&
					           target->result.var == opline->result.var &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1))) {
					   /* convert T=JMPZ_EX(X,L1), L1: T=JMPNZ_EX(T,L2) to
						  JMPZ_EX(X,L1+1) */
						target = target + 1;
					} else if (target->opcode == ZEND_BOOL &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1))) {
						/* convert Y = JMPZ_EX(X,L1), L1: Z = BOOL(Y) to
						   Z = JMPZ_EX(X,L1+1) */

						/* NOTE: This optimization pattern is not safe, but works, */
						/*       because result of JMPZ_EX instruction             */
						/*       is not used on the following path and             */
						/*       should be used once on the branch path.           */
						/*                                                         */
						/*       The pattern works well only if jums processed in  */
						/*       direct order, otherwise it breaks JMPZ_EX         */
						/*       sequences too early.                              */
						opline->result.var = target->result.var;
						target = target + 1;
						CHECK_LOOP(target);
					} else if (target->opcode == ZEND_NOP) {
						target = target + 1;
					} else {
						break;
					}
					ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
				}

				/* convert L: T = JMPZ_EX X,L+1 to T = BOOL(X) */
				if (target == opline + 1) {
					opline->opcode = ZEND_BOOL;
					opline->op2.num = 0;
				}
				break;

			case ZEND_JMPZNZ:
optimize_jmpznz:
				jmp_hitlist_count = 0;
				target = ZEND_OP2_JMP_ADDR(opline);
				while (1) {
					if (target->opcode == ZEND_JMP) {
						/* JMPZNZ(X,L1,L2), L1: JMP(L3) => JMPZNZ(X,L3,L2), L1: JMP(L3) */
						target = ZEND_OP1_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if ((target->opcode == ZEND_JMPZ || target->opcode == ZEND_JMPZNZ) &&
					           SAME_VAR(target->op1, opline->op1)) {
						/* JMPZNZ(X, L1, L2), L1: JMPZ(X, L3) -> JMPZNZ(X, L3, L2) */
						target = ZEND_OP2_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if (target->opcode == ZEND_JMPNZ &&
					           SAME_VAR(target->op1, opline->op1)) {
						/* JMPZNZ(X, L1, L2), L1: X = JMPNZ(X, L3) -> JMPZNZ(X, L1+1, L2) */
						target = target + 1;
					} else if (target->opcode == ZEND_NOP) {
						target = target + 1;
					} else {
						break;
					}
					ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
				}

				jmp_hitlist_count = 0;
				target = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
				while (1) {
					if (target->opcode == ZEND_JMP) {
						/* JMPZNZ(X,L1,L2), L2: JMP(L3) => JMPZNZ(X,L1,L3), L2: JMP(L3) */
						target = ZEND_OP1_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if (target->opcode == ZEND_JMPNZ &&
					           SAME_VAR(target->op1, opline->op1)) {
						/* JMPZNZ(X, L1, L2), L1: X = JMPNZ(X, L3) -> JMPZNZ(X, L1+1, L2) */
						target = ZEND_OP2_JMP_ADDR(target);
						CHECK_LOOP(target);
					} else if (target->opcode == ZEND_JMPZ &&
					           SAME_VAR(target->op1, opline->op1)) {
						/* JMPZNZ(X, L1, L2), L1: JMPZ(X, L3) -> JMPZNZ(X, L3, L2) */
						target = target + 1;
					} else if (target->opcode == ZEND_JMPZNZ &&
					           SAME_VAR(target->op1, opline->op1)) {
						/* JMPZNZ(X, L1, L2), L1: JMPZ(X, L3) -> JMPZNZ(X, L3, L2) */
						target = ZEND_OFFSET_TO_OPLINE(target, target->extended_value);
						CHECK_LOOP(target);
					} else if (target->opcode == ZEND_NOP) {
						target = target + 1;
					} else {
						break;
					}
					opline->extended_value = ZEND_OPLINE_TO_OFFSET(opline, target);
				}

				if (ZEND_OP2_JMP_ADDR(opline) == target &&
				    !(opline->op1_type & (IS_VAR|IS_TMP_VAR))) {
					/* JMPZNZ(?,L,L) -> JMP(L) */
					opline->opcode = ZEND_JMP;
					ZEND_SET_OP_JMP_ADDR(opline, opline->op1, target);
					SET_UNUSED(opline->op1);
					SET_UNUSED(opline->op2);
					opline->extended_value = 0;
				}
				/* Don't convert JMPZNZ back to JMPZ/JMPNZ, because the
				   following JMP is not removed yet. */
				break;
		}
		opline++;
	}
	free_alloca(jmp_hitlist, use_heap);
}
