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

/* pass 1 (Simple local optimizations)
 * - persistent constant substitution (true, false, null, etc)
 * - constant casting (ADD expects numbers, CONCAT strings, etc)
 * - constant expression evaluation
 * - optimize constant conditional JMPs
 * - pre-evaluate constant function calls
 * - eliminate FETCH $GLOBALS followed by FETCH_DIM/UNSET_DIM/ISSET_ISEMPTY_DIM
 */

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"

void zend_optimizer_pass1(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	zend_bool collect_constants = (ZEND_OPTIMIZER_PASS_15 & ctx->optimization_level)?
		(op_array == &ctx->script->main_op_array) : 0;

	while (opline < end) {
		switch (opline->opcode) {
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
			if (opline->op1_type == IS_CONST) {
				if (Z_TYPE(ZEND_OP1_LITERAL(opline)) != IS_STRING) {
					convert_to_string(&ZEND_OP1_LITERAL(opline));
				}
			}
			if (opline->op2_type == IS_CONST) {
				if (Z_TYPE(ZEND_OP2_LITERAL(opline)) != IS_STRING) {
					convert_to_string(&ZEND_OP2_LITERAL(opline));
				}
				if (opline->op1_type == IS_CONST) {
					goto constant_binary_op;
				}
			}
			break;

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
			if (opline->op1_type == IS_CONST &&
				opline->op2_type == IS_CONST) {
				/* binary operation with constant operands */
				zval result;

constant_binary_op:
				if (zend_optimizer_eval_binary_op(&result, opline->opcode, &ZEND_OP1_LITERAL(opline), &ZEND_OP2_LITERAL(opline)) == SUCCESS) {
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					literal_dtor(&ZEND_OP2_LITERAL(opline));
					if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_TMP_VAR, opline->result.var, &result)) {
						MAKE_NOP(opline);
					} else {
						opline->opcode = ZEND_QM_ASSIGN;
						SET_UNUSED(opline->op2);
						zend_optimizer_update_op1_const(op_array, opline, &result);
					}
				}
			}
			break;

		case ZEND_ASSIGN_OP:
			if (opline->op2_type == IS_CONST) {
				if (opline->extended_value == ZEND_ADD
				 || opline->extended_value == ZEND_SUB
				 || opline->extended_value == ZEND_MUL
				 || opline->extended_value == ZEND_DIV
				 || opline->extended_value == ZEND_POW) {
					if (Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING) {
						/* don't optimise if it should produce a runtime numeric string error */
						if (is_numeric_string(Z_STRVAL(ZEND_OP2_LITERAL(opline)), Z_STRLEN(ZEND_OP2_LITERAL(opline)), NULL, NULL, 0)) {
							convert_scalar_to_number(&ZEND_OP2_LITERAL(opline));
						}
					}
				} else if (opline->extended_value == ZEND_MOD
				 || opline->extended_value == ZEND_SL
				 || opline->extended_value == ZEND_SR) {
					if (Z_TYPE(ZEND_OP2_LITERAL(opline)) != IS_LONG) {
						/* don't optimise if it should produce a runtime numeric string error */
						if (!(Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING
							&& !is_numeric_string(Z_STRVAL(ZEND_OP2_LITERAL(opline)), Z_STRLEN(ZEND_OP2_LITERAL(opline)), NULL, NULL, 0))) {
							convert_to_long(&ZEND_OP2_LITERAL(opline));
						}
					}
				} else if (opline->extended_value == ZEND_CONCAT) {
					if (Z_TYPE(ZEND_OP2_LITERAL(opline)) != IS_STRING) {
						convert_to_string(&ZEND_OP2_LITERAL(opline));
					}
				}
			}
			break;

		case ZEND_CAST:
			if (opline->op1_type == IS_CONST) {
				/* cast of constant operand */
				zval result;

				if (zend_optimizer_eval_cast(&result, opline->extended_value, &ZEND_OP1_LITERAL(opline)) == SUCCESS) {
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					if (zend_optimizer_replace_by_const(op_array, opline + 1, opline->result_type, opline->result.var, &result)) {
						MAKE_NOP(opline);
					} else {
						opline->opcode = ZEND_QM_ASSIGN;
						opline->extended_value = 0;
						zend_optimizer_update_op1_const(op_array, opline, &result);
					}
					break;
				}
			}
			break;

		case ZEND_BW_NOT:
		case ZEND_BOOL_NOT:
			if (opline->op1_type == IS_CONST) {
				/* unary operation on constant operand */
				zval result;

				if (zend_optimizer_eval_unary_op(&result, opline->opcode, &ZEND_OP1_LITERAL(opline)) == SUCCESS) {
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_TMP_VAR, opline->result.var, &result)) {
						MAKE_NOP(opline);
					} else {
						opline->opcode = ZEND_QM_ASSIGN;
						zend_optimizer_update_op1_const(op_array, opline, &result);
					}
				}
			}
			break;

		case ZEND_FETCH_CONSTANT:
			if (opline->op2_type == IS_CONST &&
				Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING &&
				Z_STRLEN(ZEND_OP2_LITERAL(opline)) == sizeof("__COMPILER_HALT_OFFSET__") - 1 &&
				memcmp(Z_STRVAL(ZEND_OP2_LITERAL(opline)), "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__") - 1) == 0) {
				/* substitute __COMPILER_HALT_OFFSET__ constant */
				zend_execute_data *orig_execute_data = EG(current_execute_data);
				zend_execute_data fake_execute_data;
				zval *offset;

				memset(&fake_execute_data, 0, sizeof(zend_execute_data));
				fake_execute_data.func = (zend_function*)op_array;
				EG(current_execute_data) = &fake_execute_data;
				if ((offset = zend_get_constant_str("__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__") - 1)) != NULL) {

					literal_dtor(&ZEND_OP2_LITERAL(opline));
					if (zend_optimizer_replace_by_const(op_array, opline, IS_TMP_VAR, opline->result.var, offset)) {
						MAKE_NOP(opline);
					} else {
						opline->opcode = ZEND_QM_ASSIGN;
						opline->extended_value = 0;
						SET_UNUSED(opline->op2);
						zend_optimizer_update_op1_const(op_array, opline, offset);
					}
				}
				EG(current_execute_data) = orig_execute_data;
				break;
			}

			if (opline->op2_type == IS_CONST &&
				Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING) {
				/* substitute persistent constants */
				zval c;

				if (!zend_optimizer_get_persistent_constant(Z_STR(ZEND_OP2_LITERAL(opline)), &c, 1)) {
					if (!ctx->constants || !zend_optimizer_get_collected_constant(ctx->constants, &ZEND_OP2_LITERAL(opline), &c)) {
						break;
					}
				}
				if (Z_TYPE(c) == IS_CONSTANT_AST) {
					break;
				}
				literal_dtor(&ZEND_OP2_LITERAL(opline));
				if (zend_optimizer_replace_by_const(op_array, opline, IS_TMP_VAR, opline->result.var, &c)) {
					MAKE_NOP(opline);
				} else {
					opline->opcode = ZEND_QM_ASSIGN;
					opline->extended_value = 0;
					SET_UNUSED(opline->op2);
					zend_optimizer_update_op1_const(op_array, opline, &c);
				}
			}
			break;

		case ZEND_FETCH_CLASS_CONSTANT:
			if (opline->op2_type == IS_CONST &&
				Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING) {

				zend_class_entry *ce = NULL;

				if (opline->op1_type == IS_CONST &&
			        Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_STRING) {
					/* for A::B */
					if (op_array->scope &&
						!strncasecmp(Z_STRVAL(ZEND_OP1_LITERAL(opline)),
						ZSTR_VAL(op_array->scope->name), Z_STRLEN(ZEND_OP1_LITERAL(opline)) + 1)) {
						ce = op_array->scope;
					} else {
						if ((ce = zend_hash_find_ptr(EG(class_table),
								Z_STR(op_array->literals[opline->op1.constant + 1]))) == NULL ||
								(ce->type == ZEND_INTERNAL_CLASS &&
								 ce->info.internal.module->type != MODULE_PERSISTENT) ||
								(ce->type == ZEND_USER_CLASS &&
								 ce->info.user.filename != op_array->filename)) {
							break;
						}
					}
				} else if (op_array->scope &&
					opline->op1_type == IS_UNUSED &&
					(opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_SELF) {
					/* for self::B */
					ce = op_array->scope;
				} else if (op_array->scope &&
					opline->op1_type == IS_VAR &&
					(opline - 1)->opcode == ZEND_FETCH_CLASS &&
					((opline - 1)->op2_type == IS_UNUSED &&
					((opline - 1)->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_SELF) &&
					(opline - 1)->result.var == opline->op1.var) {
					/* for self::B */
					ce = op_array->scope;
				}

				if (ce) {
					zend_class_constant *cc;
					zval *c, t;

					if ((cc = zend_hash_find_ptr(&ce->constants_table,
							Z_STR(ZEND_OP2_LITERAL(opline)))) != NULL &&
						(Z_ACCESS_FLAGS(cc->value) & ZEND_ACC_PPP_MASK) == ZEND_ACC_PUBLIC) {
						c = &cc->value;
						if (Z_TYPE_P(c) == IS_CONSTANT_AST) {
							zend_ast *ast = Z_ASTVAL_P(c);
							if (ast->kind != ZEND_AST_CONSTANT
							 || !zend_optimizer_get_persistent_constant(zend_ast_get_constant_name(ast), &t, 1)
							 || Z_TYPE(t) == IS_CONSTANT_AST) {
								break;
							}
						} else {
							ZVAL_COPY_OR_DUP(&t, c);
						}

						if (opline->op1_type == IS_CONST) {
							literal_dtor(&ZEND_OP1_LITERAL(opline));
						} else if (opline->op1_type == IS_VAR) {
							MAKE_NOP((opline - 1));
						}
						literal_dtor(&ZEND_OP2_LITERAL(opline));

						if (zend_optimizer_replace_by_const(op_array, opline, IS_TMP_VAR, opline->result.var, &t)) {
							MAKE_NOP(opline);
						} else {
							opline->opcode = ZEND_QM_ASSIGN;
							opline->extended_value = 0;
							SET_UNUSED(opline->op2);
							zend_optimizer_update_op1_const(op_array, opline, &t);
						}
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
				/* don't colllect constants after unknown function call */
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
					/* don't colllect constants after unknown function call */
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
				/* don't colllect constants after unknown function call */
				collect_constants = 0;
				break;
			}

			/* define("name", scalar); */
			if (Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("define")-1 &&
			    zend_binary_strcasecmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)), Z_STRLEN(ZEND_OP2_LITERAL(init_opline)), "define", sizeof("define")-1) == 0) {

				if (Z_TYPE(ZEND_OP1_LITERAL(send1_opline)) == IS_STRING &&
				    send2_opline &&
				    Z_TYPE(ZEND_OP1_LITERAL(send2_opline)) <= IS_STRING) {

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

			/* pre-evaluate constant functions:
			   constant(x)
			   function_exists(x)
			   is_callable(x)
			   extension_loaded(x)
			*/
			if (!send2_opline &&
			    Z_TYPE(ZEND_OP1_LITERAL(send1_opline)) == IS_STRING) {
				if ((Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("function_exists")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"function_exists", sizeof("function_exists")-1)) ||
					(Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("is_callable")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"is_callable", sizeof("is_callable")))) {
					zend_internal_function *func;
					zend_string *lc_name = zend_string_tolower(
							Z_STR(ZEND_OP1_LITERAL(send1_opline)));

					if ((func = zend_hash_find_ptr(EG(function_table), lc_name)) != NULL
						 && func->type == ZEND_INTERNAL_FUNCTION
						 && func->module->type == MODULE_PERSISTENT
#ifdef ZEND_WIN32
						 && func->module->handle == NULL
#endif
						) {
						zval t;
						ZVAL_TRUE(&t);
						literal_dtor(&ZEND_OP2_LITERAL(init_opline));
						MAKE_NOP(init_opline);
						literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
						MAKE_NOP(send1_opline);
						if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_VAR, opline->result.var, &t)) {
							MAKE_NOP(opline);
						} else {
							opline->opcode = ZEND_QM_ASSIGN;
							opline->extended_value = 0;
							SET_UNUSED(opline->op2);
							zend_optimizer_update_op1_const(op_array, opline, &t);
						}
					}
					zend_string_release_ex(lc_name, 0);
					break;
				} else if (Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("extension_loaded")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"extension_loaded", sizeof("extension_loaded")-1)) {
					zval t;
					zend_string *lc_name = zend_string_tolower(
							Z_STR(ZEND_OP1_LITERAL(send1_opline)));
					zend_module_entry *m = zend_hash_find_ptr(&module_registry,
							lc_name);

					zend_string_release_ex(lc_name, 0);
					if (!m) {
						if (PG(enable_dl)) {
							break;
						} else {
							ZVAL_FALSE(&t);
						}
					} else {
						if (m->type == MODULE_PERSISTENT
#ifdef ZEND_WIN32
						 && m->handle == NULL
#endif
						) {
							ZVAL_TRUE(&t);
						} else {
							break;
						}
					}

					literal_dtor(&ZEND_OP2_LITERAL(init_opline));
					MAKE_NOP(init_opline);
					literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
					MAKE_NOP(send1_opline);
					if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_VAR, opline->result.var, &t)) {
						MAKE_NOP(opline);
					} else {
						opline->opcode = ZEND_QM_ASSIGN;
						opline->extended_value = 0;
						SET_UNUSED(opline->op2);
						zend_optimizer_update_op1_const(op_array, opline, &t);
					}
					break;
				} else if (Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("constant")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"constant", sizeof("constant")-1)) {
					zval t;

					if (zend_optimizer_get_persistent_constant(Z_STR(ZEND_OP1_LITERAL(send1_opline)), &t, 1)) {
						literal_dtor(&ZEND_OP2_LITERAL(init_opline));
						MAKE_NOP(init_opline);
						literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
						MAKE_NOP(send1_opline);
						if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_VAR, opline->result.var, &t)) {
							MAKE_NOP(opline);
						} else {
							opline->opcode = ZEND_QM_ASSIGN;
							opline->extended_value = 0;
							SET_UNUSED(opline->op2);
							zend_optimizer_update_op1_const(op_array, opline, &t);
						}
					}
					break;
				/* dirname(IS_CONST/IS_STRING) -> IS_CONST/IS_STRING */
				} else if (Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("dirname")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"dirname", sizeof("dirname") - 1) &&
					IS_ABSOLUTE_PATH(Z_STRVAL(ZEND_OP1_LITERAL(send1_opline)), Z_STRLEN(ZEND_OP1_LITERAL(send1_opline)))) {
					zend_string *dirname = zend_string_init(Z_STRVAL(ZEND_OP1_LITERAL(send1_opline)), Z_STRLEN(ZEND_OP1_LITERAL(send1_opline)), 0);
					ZSTR_LEN(dirname) = zend_dirname(ZSTR_VAL(dirname), ZSTR_LEN(dirname));
					if (IS_ABSOLUTE_PATH(ZSTR_VAL(dirname), ZSTR_LEN(dirname))) {
						zval t;

						ZVAL_STR(&t, dirname);
						literal_dtor(&ZEND_OP2_LITERAL(init_opline));
						MAKE_NOP(init_opline);
						literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
						MAKE_NOP(send1_opline);
						if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_VAR, opline->result.var, &t)) {
							MAKE_NOP(opline);
						} else {
							opline->opcode = ZEND_QM_ASSIGN;
							opline->extended_value = 0;
							SET_UNUSED(opline->op2);
							zend_optimizer_update_op1_const(op_array, opline, &t);
						}
					} else {
						zend_string_release_ex(dirname, 0);
					}
					break;
				}
			}
			/* don't colllect constants after any other function call */
			collect_constants = 0;
			break;
		}
		case ZEND_STRLEN:
			if (opline->op1_type == IS_CONST) {
				zval t;

				if (zend_optimizer_eval_strlen(&t, &ZEND_OP1_LITERAL(opline)) == SUCCESS) {
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_TMP_VAR, opline->result.var, &t)) {
						MAKE_NOP(opline);
					} else {
						opline->opcode = ZEND_QM_ASSIGN;
						zend_optimizer_update_op1_const(op_array, opline, &t);
					}
				}
			}
			break;
		case ZEND_DEFINED:
			{
				zval c;
				if (!zend_optimizer_get_persistent_constant(Z_STR(ZEND_OP1_LITERAL(opline)), &c, 0)) {
					break;
				}
				ZVAL_TRUE(&c);
				literal_dtor(&ZEND_OP1_LITERAL(opline));
				if (zend_optimizer_replace_by_const(op_array, opline, IS_TMP_VAR, opline->result.var, &c)) {
					MAKE_NOP(opline);
				} else {
					opline->opcode = ZEND_QM_ASSIGN;
					zend_optimizer_update_op1_const(op_array, opline, &c);
				}
			}
			break;
		case ZEND_DECLARE_CONST:
			if (collect_constants &&
			    Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_STRING &&
			    Z_TYPE(ZEND_OP2_LITERAL(opline)) <= IS_STRING) {
				zend_optimizer_collect_constant(ctx, &ZEND_OP1_LITERAL(opline), &ZEND_OP2_LITERAL(opline));
			}
			break;
#if 0
		/* see ext/opcache/tests/bug78961.phpt */
//		case ZEND_FETCH_R:
		case ZEND_FETCH_W:
//		case ZEND_FETCH_RW:
		case ZEND_FETCH_IS:
//		case ZEND_FETCH_FUNC_ARG:
		case ZEND_FETCH_UNSET:
			/* convert FETCH $GLOBALS (global), FETCH_DIM $x into FETCH $x (global) */
			if ((opline->extended_value & ZEND_FETCH_GLOBAL) != 0 &&
			    opline->op1_type == IS_CONST &&
			    Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_STRING &&
			    zend_string_equals_literal(Z_STR(ZEND_OP1_LITERAL(opline)), "GLOBALS") &&
			    ((opline + 1)->opcode == opline->opcode + 1 ||
			     ((opline + 1)->opcode == ZEND_UNSET_DIM &&
			      opline->opcode == ZEND_FETCH_UNSET) ||
			     ((opline + 1)->opcode == ZEND_ISSET_ISEMPTY_DIM_OBJ &&
			      opline->opcode == ZEND_FETCH_IS)) &&
			    (opline + 1)->op1_type == opline->result_type &&
			    (opline + 1)->op1.var == opline->result.var &&
			    ((opline + 1)->op2_type != IS_CONST ||
			     Z_TYPE(ZEND_OP2_LITERAL(opline + 1)) < IS_ARRAY)) {

				if ((opline + 1)->opcode == ZEND_UNSET_DIM) {
					(opline + 1)->opcode = ZEND_UNSET_VAR;
					(opline + 1)->extended_value = ZEND_FETCH_GLOBAL;
				} else if ((opline + 1)->opcode == ZEND_ISSET_ISEMPTY_DIM_OBJ) {
					(opline + 1)->opcode = ZEND_ISSET_ISEMPTY_VAR;
					(opline + 1)->extended_value |= ZEND_FETCH_GLOBAL;
				} else {
					(opline + 1)->opcode = opline->opcode;
					(opline + 1)->extended_value = ZEND_FETCH_GLOBAL;
				}
				(opline + 1)->op1_type = (opline + 1)->op2_type;
				(opline + 1)->op1 = (opline + 1)->op2;
				if ((opline + 1)->op1_type == IS_CONST &&
				    Z_TYPE(ZEND_OP1_LITERAL(opline + 1)) != IS_STRING) {

					convert_to_string(&ZEND_OP1_LITERAL(opline + 1));
					zend_string_hash_val(Z_STR(ZEND_OP1_LITERAL(opline + 1)));
				}
				SET_UNUSED((opline + 1)->op2);
				MAKE_NOP(opline);
			}
			break;
#endif

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

		case ZEND_JMPZNZ:
			if (opline->op1_type == IS_CONST) {
				zend_op *target_opline;

				if (zend_is_true(&ZEND_OP1_LITERAL(opline))) {
					target_opline = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value); /* JMPNZ */
				} else {
					target_opline = ZEND_OP2_JMP_ADDR(opline); /* JMPZ */
				}
				literal_dtor(&ZEND_OP1_LITERAL(opline));
				ZEND_SET_OP_JMP_ADDR(opline, opline->op1, target_opline);
				opline->op1_type = IS_UNUSED;
				opline->opcode = ZEND_JMP;
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
			collect_constants = 0;
			break;
		}
		opline++;
	}
}
