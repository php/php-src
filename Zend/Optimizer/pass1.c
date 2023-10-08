/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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

/* pass 1 (Simple local optimizations)
 * - persistent constant substitution (true, false, null, etc)
 * - constant casting (ADD expects numbers, CONCAT strings, etc)
 * - constant expression evaluation
 * - optimize constant conditional JMPs
 * - pre-evaluate constant function calls
 */

#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"

static void replace_by_const_or_qm_assign(zend_op_array *op_array, zend_op *opline, zval *result) {
	if (opline->op1_type == IS_CONST) {
		literal_dtor(&ZEND_OP1_LITERAL(opline));
	}
	if (opline->op2_type == IS_CONST) {
		literal_dtor(&ZEND_OP2_LITERAL(opline));
	}
	if (zend_optimizer_replace_by_const(op_array, opline + 1, opline->result_type, opline->result.var, result)) {
		MAKE_NOP(opline);
	} else {
		opline->opcode = ZEND_QM_ASSIGN;
		opline->extended_value = 0;
		SET_UNUSED(opline->op2);
		zend_optimizer_update_op1_const(op_array, opline, result);
	}
}

void zend_optimizer_pass1(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	bool collect_constants = (ZEND_OPTIMIZER_PASS_15 & ctx->optimization_level)?
		(op_array == &ctx->script->main_op_array) : 0;
	zval result;

	while (opline < end) {
		switch (opline->opcode) {
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
			if (opline->op1_type == IS_CONST && Z_TYPE(ZEND_OP1_LITERAL(opline)) != IS_STRING) {
				convert_to_string(&ZEND_OP1_LITERAL(opline));
			}
			if (opline->op2_type == IS_CONST && Z_TYPE(ZEND_OP2_LITERAL(opline)) != IS_STRING) {
				convert_to_string(&ZEND_OP2_LITERAL(opline));
			}
			ZEND_FALLTHROUGH;
		case ZEND_ADD:
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_DIV:
		case ZEND_POW:
		case ZEND_MOD:
		case ZEND_SL:
		case ZEND_SR:
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
		case ZEND_IS_EQUAL:
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_SMALLER:
		case ZEND_IS_SMALLER_OR_EQUAL:
		case ZEND_IS_IDENTICAL:
		case ZEND_IS_NOT_IDENTICAL:
		case ZEND_BOOL_XOR:
		case ZEND_SPACESHIP:
		case ZEND_CASE:
		case ZEND_CASE_STRICT:
			if (opline->op1_type == IS_CONST && opline->op2_type == IS_CONST &&
					zend_optimizer_eval_binary_op(&result, opline->opcode, &ZEND_OP1_LITERAL(opline), &ZEND_OP2_LITERAL(opline)) == SUCCESS) {
				replace_by_const_or_qm_assign(op_array, opline, &result);
			}
			break;

		case ZEND_ASSIGN_OP:
			if (opline->extended_value == ZEND_CONCAT && opline->op2_type == IS_CONST
					&& Z_TYPE(ZEND_OP2_LITERAL(opline)) != IS_STRING) {
				convert_to_string(&ZEND_OP2_LITERAL(opline));
			}
			break;

		case ZEND_CAST:
			if (opline->op1_type == IS_CONST &&
					zend_optimizer_eval_cast(&result, opline->extended_value, &ZEND_OP1_LITERAL(opline)) == SUCCESS) {
				replace_by_const_or_qm_assign(op_array, opline, &result);
			}
			break;

		case ZEND_BW_NOT:
		case ZEND_BOOL_NOT:
			if (opline->op1_type == IS_CONST &&
					zend_optimizer_eval_unary_op(&result, opline->opcode, &ZEND_OP1_LITERAL(opline)) == SUCCESS) {
				replace_by_const_or_qm_assign(op_array, opline, &result);
			}
			break;

		case ZEND_FETCH_CONSTANT:
			if (opline->op2_type == IS_CONST &&
				Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING &&
				zend_string_equals_literal(Z_STR(ZEND_OP2_LITERAL(opline)), "__COMPILER_HALT_OFFSET__")) {
				/* substitute __COMPILER_HALT_OFFSET__ constant */
				zend_execute_data *orig_execute_data = EG(current_execute_data);
				zend_execute_data fake_execute_data;
				zval *offset;

				memset(&fake_execute_data, 0, sizeof(zend_execute_data));
				fake_execute_data.func = (zend_function*)op_array;
				EG(current_execute_data) = &fake_execute_data;
				if ((offset = zend_get_constant_str("__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__") - 1)) != NULL) {

					literal_dtor(&ZEND_OP2_LITERAL(opline));
					replace_by_const_or_qm_assign(op_array, opline, offset);
				}
				EG(current_execute_data) = orig_execute_data;
				break;
			}

			if (opline->op2_type == IS_CONST &&
				Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING) {
				/* substitute persistent constants */
				if (!zend_optimizer_get_persistent_constant(Z_STR(ZEND_OP2_LITERAL(opline)), &result, 1)) {
					if (!ctx->constants || !zend_optimizer_get_collected_constant(ctx->constants, &ZEND_OP2_LITERAL(opline), &result)) {
						break;
					}
				}
				if (Z_TYPE(result) == IS_CONSTANT_AST) {
					break;
				}
				replace_by_const_or_qm_assign(op_array, opline, &result);
			}
			break;

		case ZEND_FETCH_CLASS_CONSTANT:
			if (opline->op2_type == IS_CONST &&
				Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING) {

				zend_class_entry *ce = zend_optimizer_get_class_entry_from_op1(
					ctx->script, op_array, opline);
				if (ce) {
					zend_class_constant *cc = zend_hash_find_ptr(
						&ce->constants_table, Z_STR(ZEND_OP2_LITERAL(opline)));
					if (cc && !(ZEND_CLASS_CONST_FLAGS(cc) & ZEND_ACC_DEPRECATED) && (ZEND_CLASS_CONST_FLAGS(cc) & ZEND_ACC_PPP_MASK) == ZEND_ACC_PUBLIC && !(ce->ce_flags & ZEND_ACC_TRAIT)) {
						zval *c = &cc->value;
						if (Z_TYPE_P(c) == IS_CONSTANT_AST) {
							zend_ast *ast = Z_ASTVAL_P(c);
							if (ast->kind != ZEND_AST_CONSTANT
							 || !zend_optimizer_get_persistent_constant(zend_ast_get_constant_name(ast), &result, 1)
							 || Z_TYPE(result) == IS_CONSTANT_AST) {
								break;
							}
						} else {
							ZVAL_COPY_OR_DUP(&result, c);
						}

						replace_by_const_or_qm_assign(op_array, opline, &result);
					}
				}
			}
			break;

		case ZEND_DO_ICALL: {
			zend_op *send1_opline = opline - 1;
			zend_op *send2_opline = NULL;
			zend_op *init_opline = NULL;

			while (send1_opline->opcode == ZEND_NOP) {
				send1_opline--;
			}
			if (send1_opline->opcode != ZEND_SEND_VAL ||
			    send1_opline->op1_type != IS_CONST) {
				/* don't collect constants after unknown function call */
				collect_constants = 0;
				break;
			}
			if (send1_opline->op2.num == 2) {
				send2_opline = send1_opline;
				send1_opline--;
				while (send1_opline->opcode == ZEND_NOP) {
					send1_opline--;
				}
				if (send1_opline->opcode != ZEND_SEND_VAL ||
				    send1_opline->op1_type != IS_CONST) {
					/* don't collect constants after unknown function call */
					collect_constants = 0;
					break;
				}
			}
			init_opline = send1_opline - 1;
			while (init_opline->opcode == ZEND_NOP) {
				init_opline--;
			}
			if (init_opline->opcode != ZEND_INIT_FCALL ||
			    init_opline->op2_type != IS_CONST ||
			    Z_TYPE(ZEND_OP2_LITERAL(init_opline)) != IS_STRING) {
				/* don't collect constants after unknown function call */
				collect_constants = 0;
				break;
			}

			/* define("name", scalar); */
			if (zend_string_equals_literal_ci(Z_STR(ZEND_OP2_LITERAL(init_opline)), "define")) {

				if (Z_TYPE(ZEND_OP1_LITERAL(send1_opline)) == IS_STRING && send2_opline) {

					if (collect_constants) {
						zend_optimizer_collect_constant(ctx, &ZEND_OP1_LITERAL(send1_opline), &ZEND_OP1_LITERAL(send2_opline));
					}

					if (RESULT_UNUSED(opline) &&
					    !zend_memnstr(Z_STRVAL(ZEND_OP1_LITERAL(send1_opline)), "::", sizeof("::") - 1, Z_STRVAL(ZEND_OP1_LITERAL(send1_opline)) + Z_STRLEN(ZEND_OP1_LITERAL(send1_opline)))) {

						opline->opcode = ZEND_DECLARE_CONST;
						opline->op1_type = IS_CONST;
						opline->op2_type = IS_CONST;
						opline->result_type = IS_UNUSED;
						opline->op1.constant = send1_opline->op1.constant;
						opline->op2.constant = send2_opline->op1.constant;
						opline->result.num = 0;

						literal_dtor(&ZEND_OP2_LITERAL(init_opline));
						MAKE_NOP(init_opline);
						MAKE_NOP(send1_opline);
						MAKE_NOP(send2_opline);
					}
					break;
				}
			}

			if (!send2_opline && Z_TYPE(ZEND_OP1_LITERAL(send1_opline)) == IS_STRING &&
					zend_optimizer_eval_special_func_call(&result, Z_STR(ZEND_OP2_LITERAL(init_opline)), Z_STR(ZEND_OP1_LITERAL(send1_opline))) == SUCCESS) {
				literal_dtor(&ZEND_OP2_LITERAL(init_opline));
				MAKE_NOP(init_opline);
				literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
				MAKE_NOP(send1_opline);
				replace_by_const_or_qm_assign(op_array, opline, &result);
				break;
			}

			/* don't collect constants after any other function call */
			collect_constants = 0;
			break;
		}
		case ZEND_STRLEN:
			if (opline->op1_type == IS_CONST &&
					zend_optimizer_eval_strlen(&result, &ZEND_OP1_LITERAL(opline)) == SUCCESS) {
				replace_by_const_or_qm_assign(op_array, opline, &result);
			}
			break;
		case ZEND_DEFINED:
			if (!zend_optimizer_get_persistent_constant(Z_STR(ZEND_OP1_LITERAL(opline)), &result, 0)) {
				break;
			}
			ZVAL_TRUE(&result);
			literal_dtor(&ZEND_OP1_LITERAL(opline));
			replace_by_const_or_qm_assign(op_array, opline, &result);
			break;
		case ZEND_DECLARE_CONST:
			if (collect_constants &&
			    Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_STRING &&
			    Z_TYPE(ZEND_OP2_LITERAL(opline)) != IS_CONSTANT_AST) {
				zend_optimizer_collect_constant(ctx, &ZEND_OP1_LITERAL(opline), &ZEND_OP2_LITERAL(opline));
			}
			break;

		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
			/* convert Ti = JMPZ_EX(C, L) => Ti = QM_ASSIGN(C)
			   in case we know it wouldn't jump */
			if (opline->op1_type == IS_CONST) {
				if (zend_is_true(&ZEND_OP1_LITERAL(opline))) {
					if (opline->opcode == ZEND_JMPZ_EX) {
						opline->opcode = ZEND_QM_ASSIGN;
						zval_ptr_dtor_nogc(&ZEND_OP1_LITERAL(opline));
						ZVAL_TRUE(&ZEND_OP1_LITERAL(opline));
						opline->op2.num = 0;
						break;
					}
				} else {
					if (opline->opcode == ZEND_JMPNZ_EX) {
						opline->opcode = ZEND_QM_ASSIGN;
						zval_ptr_dtor_nogc(&ZEND_OP1_LITERAL(opline));
						ZVAL_FALSE(&ZEND_OP1_LITERAL(opline));
						opline->op2.num = 0;
						break;
					}
				}
			}
			collect_constants = 0;
			break;

		case ZEND_JMPZ:
		case ZEND_JMPNZ:
			if (opline->op1_type == IS_CONST) {
				int should_jmp = zend_is_true(&ZEND_OP1_LITERAL(opline));

				if (opline->opcode == ZEND_JMPZ) {
					should_jmp = !should_jmp;
				}
				literal_dtor(&ZEND_OP1_LITERAL(opline));
				opline->op1_type = IS_UNUSED;
				if (should_jmp) {
					opline->opcode = ZEND_JMP;
					COPY_NODE(opline->op1, opline->op2);
					opline->op2.num = 0;
				} else {
					MAKE_NOP(opline);
					break;
				}
			}
			collect_constants = 0;
			break;

		case ZEND_RETURN:
		case ZEND_RETURN_BY_REF:
		case ZEND_GENERATOR_RETURN:
		case ZEND_EXIT:
		case ZEND_THROW:
		case ZEND_MATCH_ERROR:
		case ZEND_CATCH:
		case ZEND_FAST_CALL:
		case ZEND_FAST_RET:
		case ZEND_JMP:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
		case ZEND_JMP_NULL:
		case ZEND_VERIFY_NEVER_TYPE:
		case ZEND_BIND_INIT_STATIC_OR_JMP:
			collect_constants = 0;
			break;
		}
		opline++;
	}
}
