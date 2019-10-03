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
#define CHECK_JMP(target, label) 			\
	for (i=0; i<jmp_hitlist_count; i++) {	\
		if (jmp_hitlist[i] == ZEND_OP1_JMP_ADDR(target)) {		\
			goto label;						\
		}									\
	}										\
	jmp_hitlist[jmp_hitlist_count++] = ZEND_OP1_JMP_ADDR(target);

#define CHECK_JMP2(target, label) 			\
	for (i=0; i<jmp_hitlist_count; i++) {	\
		if (jmp_hitlist[i] == ZEND_OP2_JMP_ADDR(target)) {		\
			goto label;						\
		}									\
	}										\
	jmp_hitlist[jmp_hitlist_count++] = ZEND_OP2_JMP_ADDR(target);

void zend_optimizer_pass3(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_op *opline;
	zend_op *end;
	zend_op *target;
	zend_op **jmp_hitlist;
	int jmp_hitlist_count;
	int i;
	ALLOCA_FLAG(use_heap);

	if (op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) {
		return;
	}

	jmp_hitlist = (zend_op**)do_alloca(sizeof(zend_op*)*op_array->last, use_heap);
	opline = op_array->opcodes;
	end =  opline + op_array->last;

	while (opline < end) {

		switch (opline->opcode) {
			case ZEND_JMP:
				jmp_hitlist_count = 0;

				/* convert JMP L1 ... L1: JMP L2 to JMP L2 .. L1: JMP L2 */
				target = ZEND_OP1_JMP_ADDR(opline);
				while (target->opcode == ZEND_JMP) {
					CHECK_JMP(target, done_jmp_optimization);
					target = ZEND_OP1_JMP_ADDR(target);
					ZEND_SET_OP_JMP_ADDR(opline, opline->op1, target);
				}

				/* convert L: JMP L+1 to NOP */
				if (target == opline + 1) {
					MAKE_NOP(opline);
					break;
				}

				if (opline > op_array->opcodes &&
				    ((opline-1)->opcode == ZEND_JMPZ ||
				     (opline-1)->opcode == ZEND_JMPNZ)) {
				    if (ZEND_OP2_JMP_ADDR(opline-1) == target) {
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
				while (target->opcode == ZEND_JMP) {
					CHECK_JMP(target, done_jmp_optimization);
					target = ZEND_OP1_JMP_ADDR(target);
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
						CHECK_JMP(target, done_jmp_optimization);
						target = ZEND_OP1_JMP_ADDR(target);
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else if (target->opcode == opline->opcode &&
					           SAME_VAR(opline->op1, target->op1)) {
						/* same opcode and same var as this opcode */
						/* JMPZ(X,L1), L1: JMPZ(X,L2) => JMPZ(X,L2), L1: JMPZ(X,L2) */
						CHECK_JMP2(target, done_jmp_optimization);
						target = ZEND_OP2_JMP_ADDR(target);
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else if (target->opcode == INV_COND(opline->opcode) &&
					           SAME_VAR(opline->op1, target->op1)) {
						/* convert JMPZ(X,L1), L1: JMPNZ(X,L2) to
						   JMPZ(X,L1+1) */
						target = target + 1;
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else if (target->opcode == ZEND_JMPZNZ &&
					           SAME_VAR(opline->op1, target->op1)) {
						if (opline->opcode == ZEND_JMPZ) {
							CHECK_JMP2(target, done_jmp_optimization);
							target = ZEND_OP2_JMP_ADDR(target);
						} else {
							/* JMPNZ_EX */
							target = ZEND_OFFSET_TO_OPLINE(target, target->extended_value);
						}
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else {
						break;
					}
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
						CHECK_JMP(target, done_jmp_optimization);
						target = ZEND_OP1_JMP_ADDR(target);
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else if (target->opcode == opline->opcode-3 &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1))) {
						/* convert T=JMPZ_EX(X,L1), L1: JMPZ(T,L2) to
						   JMPZ_EX(X,L2) */
						CHECK_JMP2(target, done_jmp_optimization);
						target = ZEND_OP2_JMP_ADDR(target);
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else if (target->opcode == opline->opcode &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1)) &&
					           SAME_VAR(target->result, opline->result)) {
						/* convert T=JMPZ_EX(X,L1), L1: T=JMPZ_EX(T,L2) to
						   JMPZ_EX(X,L2) */
						CHECK_JMP2(target, done_jmp_optimization);
						target = ZEND_OP2_JMP_ADDR(target);
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else if (target->opcode == ZEND_JMPZNZ &&
							   SAME_VAR(target->op1, opline->result)) {
						/* Check for JMPZNZ with same cond variable */
						if (opline->opcode == ZEND_JMPZ_EX) {
							CHECK_JMP2(target, done_jmp_optimization);
							target = ZEND_OP2_JMP_ADDR(target);
						} else {
							/* JMPNZ_EX */
							target = ZEND_OFFSET_TO_OPLINE(target, target->extended_value);
						}
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else if (target->opcode == INV_EX_COND(opline->opcode) &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1))) {
					   /* convert T=JMPZ_EX(X,L1), L1: JMPNZ(T,L2) to
						  JMPZ_EX(X,L1+1) */
						target = target + 1;
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else if (target->opcode == INV_EX_COND_EX(opline->opcode) &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1)) &&
					           SAME_VAR(target->op1, opline->result) &&
					           SAME_VAR(target->result, opline->result)) {
					   /* convert T=JMPZ_EX(X,L1), L1: T=JMPNZ_EX(T,L2) to
						  JMPZ_EX(X,L1+1) */
						target = target + 1;
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else if (target->opcode == ZEND_BOOL &&
					           (SAME_VAR(target->op1, opline->result) ||
					            SAME_VAR(target->op1, opline->op1))) {
						/* convert Y = JMPZ_EX(X,L1), L1: Z = BOOL(Y) to
						   Z = JMPZ_EX(X,L1+1) */
						opline->result.var = target->result.var;
						target = target + 1;
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
					} else {
						break;
					}
				} /* while */

				/* convert L: T = JMPZ_EX X,L+1 to T = BOOL(X) */
				if (target == opline + 1) {
					opline->opcode = ZEND_BOOL;
					opline->op2.num = 0;
					break;
				}

				break;

			case ZEND_JMPZNZ:
				jmp_hitlist_count = 0;

				/* JMPZNZ(X,L1,L2), L1: JMP(L3) => JMPZNZ(X,L3,L2), L1: JMP(L3) */
				target = ZEND_OP2_JMP_ADDR(opline);
				while (target->opcode == ZEND_JMP) {
					CHECK_JMP(target, continue_jmpznz_optimization);
					target = ZEND_OP1_JMP_ADDR(target);
					ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target);
				}
continue_jmpznz_optimization:
				/* JMPZNZ(X,L1,L2), L2: JMP(L3) => JMPZNZ(X,L1,L3), L2: JMP(L3) */
				target = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
				while (target->opcode == ZEND_JMP) {
					CHECK_JMP(target, done_jmp_optimization);
					target = ZEND_OP1_JMP_ADDR(target);
					opline->extended_value = ZEND_OPLINE_TO_OFFSET(opline, target);
				}
				break;
		}
done_jmp_optimization:
		opline++;
	}
	free_alloca(jmp_hitlist, use_heap);
}
