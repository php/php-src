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

/* pass 3:
 * - optimize $i = $i+expr to $i+=expr
 * - optimize series of JMPs
 * - change $i++ to ++$i where possible
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
	zend_op *end = op_array->opcodes + op_array->last;
	zend_op **jmp_hitlist;
	int jmp_hitlist_count;
	int i;
	uint32_t opline_num = 0;
	ALLOCA_FLAG(use_heap);

	jmp_hitlist = (zend_op**)do_alloca(sizeof(zend_op*)*op_array->last, use_heap);
	opline = op_array->opcodes;

	while (opline < end) {
		jmp_hitlist_count = 0;

		switch (opline->opcode) {
			case ZEND_ADD:
			case ZEND_SUB:
			case ZEND_MUL:
			case ZEND_DIV:
			case ZEND_MOD:
			case ZEND_POW:
			case ZEND_CONCAT:
			case ZEND_SL:
			case ZEND_SR:
			case ZEND_BW_OR:
			case ZEND_BW_AND:
			case ZEND_BW_XOR:
				{
					zend_op *next_opline = opline + 1;

					while (next_opline < end && next_opline->opcode == ZEND_NOP) {
						++next_opline;
					}

					if (next_opline >= end || next_opline->opcode != ZEND_ASSIGN) {
						break;
					}

					/* change $i=expr+$i to $i=$i+expr so that the following optimization
					 * works on it. Only do this if we are ignoring operator overloading,
					 * as operand order might be significant otherwise. */
					if ((ctx->optimization_level & ZEND_OPTIMIZER_IGNORE_OVERLOADING)
						&& (opline->op2_type & (IS_VAR | IS_CV))
						&& opline->op2.var == next_opline->op1.var &&
						(opline->opcode == ZEND_ADD ||
						 opline->opcode == ZEND_MUL ||
						 opline->opcode == ZEND_BW_OR ||
						 opline->opcode == ZEND_BW_AND ||
						 opline->opcode == ZEND_BW_XOR)) {
						zend_uchar tmp_type = opline->op1_type;
						znode_op tmp = opline->op1;

						if (opline->opcode != ZEND_ADD
								|| (opline->op1_type == IS_CONST
									&& Z_TYPE(ZEND_OP1_LITERAL(opline)) != IS_ARRAY)) {
							/* protection from array add: $a = array + $a is not commutative! */
							COPY_NODE(opline->op1, opline->op2);
							COPY_NODE(opline->op2, tmp);
						}
					}

					if (ZEND_IS_BINARY_ASSIGN_OP_OPCODE(opline->opcode)
					    && (opline->op1_type & (IS_VAR | IS_CV))
						&& opline->op1.var == next_opline->op1.var
						&& opline->op1_type == next_opline->op1_type) {
						opline->extended_value = opline->opcode;
						opline->opcode = ZEND_ASSIGN_OP;
						COPY_NODE(opline->result, next_opline->result);
						MAKE_NOP(next_opline);
						opline++;
						opline_num++;
					}
				}
				break;

			case ZEND_JMP:
				if (op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) {
					break;
				}

				/* convert L: JMP L+1 to NOP */
				if (ZEND_OP1_JMP_ADDR(opline) == opline + 1) {
					MAKE_NOP(opline);
					goto done_jmp_optimization;
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
				if (op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) {
					break;
				}

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
				if (op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) {
					break;
				}

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

					if (op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) {
						break;
					}

					/* convert L: T = JMPZ_EX X,L+1 to T = BOOL(X) */
					/* convert L: T = JMPZ_EX T,L+1 to NOP */
					if (ZEND_OP2_JMP_ADDR(opline) == opline + 1) {
						if (opline->op1.var == opline->result.var) {
							MAKE_NOP(opline);
						} else {
							opline->opcode = ZEND_BOOL;
							SET_UNUSED(opline->op2);
						}
						goto done_jmp_optimization;
					}

					while (ZEND_OP2_JMP_ADDR(opline) < end) {
						zend_op *target = ZEND_OP2_JMP_ADDR(opline);

						if (target->opcode == opline->opcode-3 &&
							SAME_VAR(target->op1, T)) {
						   /* convert T=JMPZ_EX(X,L1), L1: JMPZ(T,L2) to
							  JMPZ_EX(X,L2) */
							CHECK_JMP2(target, continue_jmp_ex_optimization);
							ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP2_JMP_ADDR(target));
						} else if (target->opcode == opline->opcode &&
							SAME_VAR(target->op1, T) &&
							SAME_VAR(target->result, T)) {
						   /* convert T=JMPZ_EX(X,L1), L1: T=JMPZ_EX(T,L2) to
							  JMPZ_EX(X,L2) */
							CHECK_JMP2(target, continue_jmp_ex_optimization);
							ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP2_JMP_ADDR(target));
						} else if (target->opcode == ZEND_JMPZNZ &&
								  SAME_VAR(target->op1, T)) {
							/* Check for JMPZNZ with same cond variable */
							zend_op *new_target;

							CHECK_JMP2(target, continue_jmp_ex_optimization);
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
continue_jmp_ex_optimization:
					break;
#if 0
					/* If Ti = JMPZ_EX(X, L) and Ti is not used, convert to JMPZ(X, L) */
					{
						zend_op *op;
						for(op = opline+1; op<end; op++) {
							if(op->result_type == IS_TMP_VAR &&
							   op->result.var == opline->result.var) {
								break; /* can pass to part 2 */
							}

							if(op->opcode == ZEND_JMP ||
							   op->opcode == ZEND_JMPZ ||
							   op->opcode == ZEND_JMPZ_EX ||
							   op->opcode == ZEND_JMPNZ ||
							   op->opcode == ZEND_JMPNZ_EX ||
							   op->opcode == ZEND_JMPZNZ ||
							   op->opcode == ZEND_CASE ||
							   op->opcode == ZEND_RETURN ||
							   op->opcode == ZEND_RETURN_BY_REF ||
							   op->opcode == ZEND_FAST_RET ||
							   op->opcode == ZEND_FE_FETCH_R ||
							   op->opcode == ZEND_FE_FETCH_RW ||
							   op->opcode == ZEND_EXIT) {
								break;
							}

							if(op->op1_type == IS_TMP_VAR &&
							   op->op1.var == opline->result.var) {
								goto done_jmp_optimization;
							}

							if(op->op2_type == IS_TMP_VAR &&
							   op->op2.var == opline->result.var) {
								goto done_jmp_optimization;
							}
						} /* for */

						for(op = &op_array->opcodes[opline->op2.opline_num]; op<end; op++) {

							if(op->result_type == IS_TMP_VAR &&
							   op->result.var == opline->result.var) {
								break; /* can pass to optimization */
							}

							if(op->opcode == ZEND_JMP ||
							   op->opcode == ZEND_JMPZ ||
							   op->opcode == ZEND_JMPZ_EX ||
							   op->opcode == ZEND_JMPNZ ||
							   op->opcode == ZEND_JMPNZ_EX ||
							   op->opcode == ZEND_JMPZNZ ||
							   op->opcode == ZEND_CASE ||
							   op->opcode == ZEND_RETURN ||
							   op->opcode == ZEND_RETURN_BY_REF ||
							   op->opcode == ZEND_FAST_RET ||
							   op->opcode == ZEND_FE_FETCH_R ||
							   op->opcode == ZEND_FE_FETCH_RW ||
							   op->opcode == ZEND_EXIT) {
								break;
							}

							if(op->op1_type == IS_TMP_VAR &&
							   op->op1.var == opline->result.var) {
								goto done_jmp_optimization;
							}

							if(op->op2_type == IS_TMP_VAR &&
							   op->op2.var == opline->result.var) {
								goto done_jmp_optimization;
							}
						}

						opline->opcode = opline->opcode-3; /* JMP_EX -> JMP */
						SET_UNUSED(opline->result);
						break;
					}
#endif
				}
				break;

			case ZEND_JMPZNZ:
				if (op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) {
					break;
				}

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

			case ZEND_POST_INC_OBJ:
			case ZEND_POST_DEC_OBJ:
			case ZEND_POST_INC:
			case ZEND_POST_DEC: {
					/* POST_INC, FREE => PRE_INC */
					zend_op *next_op = opline + 1;

					if (next_op >= end) {
						break;
					}
					if (next_op->opcode == ZEND_FREE &&
						next_op->op1.var == opline->result.var) {
						MAKE_NOP(next_op);
						opline->opcode -= 2;
						opline->result_type = IS_UNUSED;
					}
				}
				break;
		}
done_jmp_optimization:
		opline++;
		opline_num++;
	}
	free_alloca(jmp_hitlist, use_heap);
}
