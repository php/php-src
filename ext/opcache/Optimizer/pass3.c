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

/* compares opcodes with allowing oc1 be _EX of oc2 */
#define SAME_OPCODE_EX(oc1, oc2) ((oc1 == oc2) || (oc1 == ZEND_JMPZ_EX && oc2 == ZEND_JMPZ) || (oc1 == ZEND_JMPNZ_EX && oc2 == ZEND_JMPNZ))

/* we use "jmp_hitlist" to avoid infinity loops during jmp optimization */
#define CHECK_JMP(target, label) 			\
	for (i=0; i<jmp_hitlist_count; i++) {	\
		if (jmp_hitlist[i] == ZEND_OP1(&op_array->opcodes[target]).opline_num) {		\
			goto label;						\
		}									\
	}										\
	jmp_hitlist[jmp_hitlist_count++] = ZEND_OP1(&op_array->opcodes[target]).opline_num;

#define CHECK_JMP2(target, label) 			\
	for (i=0; i<jmp_hitlist_count; i++) {	\
		if (jmp_hitlist[i] == ZEND_OP2(&op_array->opcodes[target]).opline_num) {		\
			goto label;						\
		}									\
	}										\
	jmp_hitlist[jmp_hitlist_count++] = ZEND_OP2(&op_array->opcodes[target]).opline_num;

void zend_optimizer_pass3(zend_op_array *op_array)
{
	zend_op *opline;
	zend_op *end = op_array->opcodes + op_array->last;
	uint32_t *jmp_hitlist;
	int jmp_hitlist_count;
	int i;
	uint32_t opline_num = 0;
	ALLOCA_FLAG(use_heap);

	jmp_hitlist = (uint32_t *)DO_ALLOCA(sizeof(uint32_t)*op_array->last);
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

					if ((ZEND_OP2_TYPE(opline) == IS_VAR || ZEND_OP2_TYPE(opline) == IS_CV)
						&& ZEND_OP2(opline).var == ZEND_OP1(next_opline).var &&
						(opline->opcode == ZEND_ADD ||
						 opline->opcode == ZEND_MUL ||
						 opline->opcode == ZEND_BW_OR ||
						 opline->opcode == ZEND_BW_AND ||
						 opline->opcode == ZEND_BW_XOR)) {
						/* change $i=expr+$i to $i=$i+expr so that the next
						* optimization works on it
						*/
						zend_uchar tmp_type = opline->op1_type;
						znode_op tmp = opline->op1;

						if (opline->opcode != ZEND_ADD
								|| (ZEND_OP1_TYPE(opline) == IS_CONST
									&& Z_TYPE(ZEND_OP1_LITERAL(opline)) != IS_ARRAY)) {
							/* protection from array add: $a = array + $a is not commutative! */
							COPY_NODE(opline->op1, opline->op2);
							COPY_NODE(opline->op2, tmp);
						}
					}
					if ((ZEND_OP1_TYPE(opline) == IS_VAR || ZEND_OP1_TYPE(opline) == IS_CV)
						&& ZEND_OP1(opline).var == ZEND_OP1(next_opline).var
						&& ZEND_OP1_TYPE(opline) == ZEND_OP1_TYPE(next_opline)) {
						switch (opline->opcode) {
							case ZEND_ADD:
								opline->opcode = ZEND_ASSIGN_ADD;
								break;
							case ZEND_SUB:
								opline->opcode = ZEND_ASSIGN_SUB;
								break;
							case ZEND_MUL:
								opline->opcode = ZEND_ASSIGN_MUL;
								break;
							case ZEND_DIV:
								opline->opcode = ZEND_ASSIGN_DIV;
								break;
							case ZEND_MOD:
								opline->opcode = ZEND_ASSIGN_MOD;
								break;
							case ZEND_POW:
								opline->opcode = ZEND_ASSIGN_POW;
								break;
							case ZEND_CONCAT:
								opline->opcode = ZEND_ASSIGN_CONCAT;
								break;
							case ZEND_SL:
								opline->opcode = ZEND_ASSIGN_SL;
								break;
							case ZEND_SR:
								opline->opcode = ZEND_ASSIGN_SR;
								break;
							case ZEND_BW_OR:
								opline->opcode = ZEND_ASSIGN_BW_OR;
								break;
							case ZEND_BW_AND:
								opline->opcode = ZEND_ASSIGN_BW_AND;
								break;
							case ZEND_BW_XOR:
								opline->opcode = ZEND_ASSIGN_BW_XOR;
								break;
						}
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
				if (ZEND_OP1(opline).opline_num == opline_num + 1) {
					MAKE_NOP(opline);
					goto done_jmp_optimization;
				}

				/* convert JMP L1 ... L1: JMP L2 to JMP L2 .. L1: JMP L2 */
				while (ZEND_OP1(opline).opline_num < op_array->last
						&& op_array->opcodes[ZEND_OP1(opline).opline_num].opcode == ZEND_JMP) {
					int target = ZEND_OP1(opline).opline_num;
					CHECK_JMP(target, done_jmp_optimization);
					ZEND_OP1(opline).opline_num = ZEND_OP1(&op_array->opcodes[target]).opline_num;
				}
				break;

			case ZEND_JMP_SET:
			case ZEND_COALESCE:
				if (op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) {
					break;
				}

				while (ZEND_OP2(opline).opline_num < op_array->last) {
					int target = ZEND_OP2(opline).opline_num;
					if (op_array->opcodes[target].opcode == ZEND_JMP) {
						ZEND_OP2(opline).opline_num = ZEND_OP1(&op_array->opcodes[target]).opline_num;
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

				while (ZEND_OP2(opline).opline_num < op_array->last) {
					int target = ZEND_OP2(opline).opline_num;

					if (op_array->opcodes[target].opcode == ZEND_JMP) {
						/* plain JMP */
						/* JMPZ(X,L1), L1: JMP(L2) => JMPZ(X,L2), L1: JMP(L2) */
						CHECK_JMP(target, done_jmp_optimization);
						ZEND_OP2(opline).opline_num = ZEND_OP1(&op_array->opcodes[target]).opline_num;
					} else if (op_array->opcodes[target].opcode == opline->opcode &&
					           SAME_VAR(opline->op1, op_array->opcodes[target].op1)) {
						/* same opcode and same var as this opcode */
						/* JMPZ(X,L1), L1: JMPZ(X,L2) => JMPZ(X,L2), L1: JMPZ(X,L2) */
						CHECK_JMP2(target, done_jmp_optimization);
						ZEND_OP2(opline).opline_num = ZEND_OP2(&op_array->opcodes[target]).opline_num;
					} else if (op_array->opcodes[target].opcode == opline->opcode + 3 &&
					           SAME_VAR(opline->op1, op_array->opcodes[target].op1)) {
						/* convert JMPZ(X,L1), L1: T JMPZ_EX(X,L2) to
						   T = JMPZ_EX(X, L2) */
						ZEND_OP2(opline).opline_num = ZEND_OP2(&op_array->opcodes[target]).opline_num;opline->opcode += 3;
						COPY_NODE(opline->result, op_array->opcodes[target].result);
						break;
					} else if (op_array->opcodes[target].opcode == INV_COND(opline->opcode) &&
					           SAME_VAR(opline->op1, op_array->opcodes[target].op1)) {
						/* convert JMPZ(X,L1), L1: JMPNZ(X,L2) to
						   JMPZ(X,L1+1) */
						ZEND_OP2(opline).opline_num = target + 1;
						break;
					} else if (op_array->opcodes[target].opcode == INV_COND_EX(opline->opcode) &&
					           SAME_VAR(opline->op1, op_array->opcodes[target].op1)) {
						/* convert JMPZ(X,L1), L1: T = JMPNZ_EX(X,L2) to
						   T = JMPZ_EX(X,L1+1) */
						ZEND_OP2(opline).opline_num = target + 1;
						opline->opcode += 3;
						COPY_NODE(opline->result, op_array->opcodes[target].result);
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
					if (ZEND_OP2(opline).opline_num == opline_num + 1) {
						if (ZEND_OP1(opline).var == ZEND_RESULT(opline).var) {
							MAKE_NOP(opline);
						} else {
							opline->opcode = ZEND_BOOL;
							SET_UNUSED(opline->op2);
						}
						goto done_jmp_optimization;
					}

					while (ZEND_OP2(opline).opline_num < op_array->last) {
						int target = ZEND_OP2(opline).opline_num;
						if (SAME_OPCODE_EX(opline->opcode, op_array->opcodes[target].opcode) &&
							SAME_VAR(op_array->opcodes[target].op1, T)) {
							/* Check for JMPZ_EX to JMPZ[_EX] with the same condition, either with _EX or not */
							if (op_array->opcodes[target].opcode == opline->opcode) {
								/* change T only if we have _EX opcode there */
								COPY_NODE(T, op_array->opcodes[target].result);
							}
							CHECK_JMP2(target, continue_jmp_ex_optimization);
							ZEND_OP2(opline).opline_num = ZEND_OP2(&op_array->opcodes[target]).opline_num;
						} else if (op_array->opcodes[target].opcode == ZEND_JMPZNZ &&
								  SAME_VAR(op_array->opcodes[target].op1, T)) {
							/* Check for JMPZNZ with same cond variable */
							int new_target;
							CHECK_JMP2(target, continue_jmp_ex_optimization);
							if (opline->opcode == ZEND_JMPZ_EX) {
								new_target = ZEND_OP2(&op_array->opcodes[target]).opline_num;
							} else {
								/* JMPNZ_EX */
								new_target = op_array->opcodes[target].extended_value;
							}
							ZEND_OP2(opline).opline_num = new_target;
						} else if ((op_array->opcodes[target].opcode == INV_EX_COND_EX(opline->opcode) ||
									op_array->opcodes[target].opcode == INV_EX_COND(opline->opcode)) &&
									SAME_VAR(opline->op1, op_array->opcodes[target].op1)) {
						   /* convert JMPZ_EX(X,L1), L1: JMPNZ_EX(X,L2) to
							  JMPZ_EX(X,L1+1) */
						   ZEND_OP2(opline).opline_num = target + 1;
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
							if(ZEND_RESULT_TYPE(op) == IS_TMP_VAR &&
							   ZEND_RESULT(op).var == ZEND_RESULT(opline).var) {
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

							if(ZEND_OP1_TYPE(op) == IS_TMP_VAR &&
							   ZEND_OP1(op).var == ZEND_RESULT(opline).var) {
								goto done_jmp_optimization;
							}

							if(ZEND_OP2_TYPE(op) == IS_TMP_VAR &&
							   ZEND_OP2(op).var == ZEND_RESULT(opline).var) {
								goto done_jmp_optimization;
							}
						} /* for */

						for(op = &op_array->opcodes[ZEND_OP2(opline).opline_num]; op<end; op++) {

							if(ZEND_RESULT_TYPE(op) == IS_TMP_VAR &&
							   ZEND_RESULT(op).var == ZEND_RESULT(opline).var) {
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

							if(ZEND_OP1_TYPE(op) == IS_TMP_VAR &&
							   ZEND_OP1(op).var == ZEND_RESULT(opline).var) {
								goto done_jmp_optimization;
							}

							if(ZEND_OP2_TYPE(op) == IS_TMP_VAR &&
							   ZEND_OP2(op).var == ZEND_RESULT(opline).var) {
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
				while (ZEND_OP2(opline).opline_num < op_array->last
						&& op_array->opcodes[ZEND_OP2(opline).opline_num].opcode == ZEND_JMP) {
					int target = ZEND_OP2(opline).opline_num;
					CHECK_JMP(target, continue_jmpznz_optimization);
					ZEND_OP2(opline).opline_num = ZEND_OP1(&op_array->opcodes[target]).opline_num;
				}
continue_jmpznz_optimization:
				/* JMPZNZ(X,L1,L2), L2: JMP(L3) => JMPZNZ(X,L1,L3), L2: JMP(L3) */
				while (opline->extended_value < op_array->last
						&& op_array->opcodes[opline->extended_value].opcode == ZEND_JMP) {
					int target = opline->extended_value;
					CHECK_JMP(target, done_jmp_optimization);
					opline->extended_value = ZEND_OP1(&op_array->opcodes[target]).opline_num;
				}
				break;

			case ZEND_POST_INC:
			case ZEND_POST_DEC: {
					/* POST_INC, FREE => PRE_INC */
					zend_op *next_op = opline + 1;

					if (next_op >= end) {
						break;
					}
					if (next_op->opcode == ZEND_FREE &&
						ZEND_OP1(next_op).var == ZEND_RESULT(opline).var) {
						MAKE_NOP(next_op);
						switch (opline->opcode) {
							case ZEND_POST_INC:
								opline->opcode = ZEND_PRE_INC;
								break;
							case ZEND_POST_DEC:
								opline->opcode = ZEND_PRE_DEC;
								break;
						}
						ZEND_RESULT_TYPE(opline) = IS_VAR | EXT_TYPE_UNUSED;
					}
				}
				break;
		}
done_jmp_optimization:
		opline++;
		opline_num++;
	}
	FREE_ALLOCA(jmp_hitlist);
}
