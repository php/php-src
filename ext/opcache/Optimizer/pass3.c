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
		jmp_hitlist_count = 0;

		switch (opline->opcode) {
			case ZEND_JMP:
				/* convert L: JMP L+1 to NOP */
				if (ZEND_OP1_JMP_ADDR(opline) == opline + 1) {
					MAKE_NOP(opline);
					break;
				}

				/* convert JMP L1 ... L1: JMP L2 to JMP L2 .. L1: JMP L2 */
				while (ZEND_OP1_JMP_ADDR(opline) < end
						&& ZEND_OP1_JMP_ADDR(opline)->opcode == ZEND_JMP) {
					zend_op *target = ZEND_OP1_JMP_ADDR(opline);
					CHECK_JMP(target, done_jmp_optimization);
					ZEND_SET_OP_JMP_ADDR(opline, opline->op1, ZEND_OP1_JMP_ADDR(target));
				}
				break;

			case ZEND_JMP_SET:
			case ZEND_COALESCE:
				while (ZEND_OP2_JMP_ADDR(opline) < end) {
					zend_op *target = ZEND_OP2_JMP_ADDR(opline);
					if (target->opcode == ZEND_JMP) {
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP1_JMP_ADDR(target));
					} else {
						break;
					}
				}
				break;

			case ZEND_JMPZ:
			case ZEND_JMPNZ:
				while (ZEND_OP2_JMP_ADDR(opline) < end) {
					zend_op *target = ZEND_OP2_JMP_ADDR(opline);

					if (target->opcode == ZEND_JMP) {
						/* plain JMP */
						/* JMPZ(X,L1), L1: JMP(L2) => JMPZ(X,L2), L1: JMP(L2) */
						CHECK_JMP(target, done_jmp_optimization);
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP1_JMP_ADDR(target));
					} else if (target->opcode == opline->opcode &&
					           SAME_VAR(opline->op1, target->op1)) {
						/* same opcode and same var as this opcode */
						/* JMPZ(X,L1), L1: JMPZ(X,L2) => JMPZ(X,L2), L1: JMPZ(X,L2) */
						CHECK_JMP2(target, done_jmp_optimization);
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP2_JMP_ADDR(target));
					} else if (target->opcode == opline->opcode + 3 &&
					           SAME_VAR(opline->op1, target->op1)) {
						/* convert JMPZ(X,L1), L1: T JMPZ_EX(X,L2) to
						   T = JMPZ_EX(X, L2) */
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP2_JMP_ADDR(target));
						opline->opcode += 3;
						COPY_NODE(opline->result, target->result);
						break;
					} else if (target->opcode == INV_COND(opline->opcode) &&
					           SAME_VAR(opline->op1, target->op1)) {
						/* convert JMPZ(X,L1), L1: JMPNZ(X,L2) to
						   JMPZ(X,L1+1) */
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target + 1);
						break;
					} else if (target->opcode == INV_COND_EX(opline->opcode) &&
					           SAME_VAR(opline->op1, target->op1)) {
						/* convert JMPZ(X,L1), L1: T = JMPNZ_EX(X,L2) to
						   T = JMPZ_EX(X,L1+1) */
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target + 1);
						opline->opcode += 3;
						COPY_NODE(opline->result, target->result);
						break;
					} else {
						break;
					}
				}
				break;

			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX: {
					zend_uchar T_type = opline->result_type;
					znode_op T = opline->result;

					/* convert L: T = JMPZ_EX X,L+1 to T = BOOL(X) */
					/* convert L: T = JMPZ_EX T,L+1 to NOP */
					if (ZEND_OP2_JMP_ADDR(opline) == opline + 1) {
						opline->opcode = ZEND_BOOL;
						opline->op2.num = 0;
						break;
					}

					while (ZEND_OP2_JMP_ADDR(opline) < end) {
						zend_op *target = ZEND_OP2_JMP_ADDR(opline);

						if (target->opcode == opline->opcode-3 &&
							SAME_VAR(target->op1, T)) {
						   /* convert T=JMPZ_EX(X,L1), L1: JMPZ(T,L2) to
							  JMPZ_EX(X,L2) */
							CHECK_JMP2(target, done_jmp_optimization);
							ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP2_JMP_ADDR(target));
						} else if (target->opcode == opline->opcode &&
							SAME_VAR(target->op1, T) &&
							SAME_VAR(target->result, T)) {
						   /* convert T=JMPZ_EX(X,L1), L1: T=JMPZ_EX(T,L2) to
							  JMPZ_EX(X,L2) */
							CHECK_JMP2(target, done_jmp_optimization);
							ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP2_JMP_ADDR(target));
						} else if (target->opcode == ZEND_JMPZNZ &&
								  SAME_VAR(target->op1, T)) {
							/* Check for JMPZNZ with same cond variable */
							zend_op *new_target;

							CHECK_JMP2(target, done_jmp_optimization);
							if (opline->opcode == ZEND_JMPZ_EX) {
								new_target = ZEND_OP2_JMP_ADDR(target);
							} else {
								/* JMPNZ_EX */
								new_target = ZEND_OFFSET_TO_OPLINE(target, target->extended_value);
							}
							ZEND_SET_OP_JMP_ADDR(opline, opline->op2, new_target);
						} else if ((target->opcode == INV_EX_COND_EX(opline->opcode) ||
									target->opcode == INV_EX_COND(opline->opcode)) &&
									SAME_VAR(opline->op1, target->op1)) {
						   /* convert JMPZ_EX(X,L1), L1: JMPNZ_EX(X,L2) to
							  JMPZ_EX(X,L1+1) */
							ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target + 1);
							break;
						} else if (target->opcode == INV_EX_COND(opline->opcode) &&
									SAME_VAR(target->op1, T)) {
						   /* convert T=JMPZ_EX(X,L1), L1: JMPNZ(T,L2) to
							  JMPZ_EX(X,L1+1) */
							ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target + 1);
							break;
						} else if (target->opcode == INV_EX_COND_EX(opline->opcode) &&
									SAME_VAR(target->op1, T) &&
									SAME_VAR(target->result, T)) {
						   /* convert T=JMPZ_EX(X,L1), L1: T=JMPNZ_EX(T,L2) to
							  JMPZ_EX(X,L1+1) */
							ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target + 1);
							break;
						} else if (target->opcode == ZEND_BOOL &&
						           SAME_VAR(opline->result, target->op1)) {
							/* convert Y = JMPZ_EX(X,L1), L1: Z = BOOL(Y) to
							   Z = JMPZ_EX(X,L1+1) */
							opline->result.var = target->result.var;
							ZEND_SET_OP_JMP_ADDR(opline, opline->op2, target + 1);
							break;
						} else {
							break;
						}
					} /* while */
				}
				break;

			case ZEND_JMPZNZ:
				/* JMPZNZ(X,L1,L2), L1: JMP(L3) => JMPZNZ(X,L3,L2), L1: JMP(L3) */
				while (ZEND_OP2_JMP_ADDR(opline) < end
						&& ZEND_OP2_JMP_ADDR(opline)->opcode == ZEND_JMP) {
					zend_op *target = ZEND_OP2_JMP_ADDR(opline);
					CHECK_JMP(target, continue_jmpznz_optimization);
					ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP1_JMP_ADDR(target));
				}
continue_jmpznz_optimization:
				/* JMPZNZ(X,L1,L2), L2: JMP(L3) => JMPZNZ(X,L1,L3), L2: JMP(L3) */
				while (ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value) < end
						&& ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value)->opcode == ZEND_JMP) {
					zend_op *target = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
					CHECK_JMP(target, done_jmp_optimization);
					opline->extended_value = ZEND_OPLINE_TO_OFFSET(opline, ZEND_OP1_JMP_ADDR(target));
				}
				break;
		}
done_jmp_optimization:
		opline++;
	}
	free_alloca(jmp_hitlist, use_heap);
}
