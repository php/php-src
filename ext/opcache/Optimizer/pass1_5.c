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

/* pass 1
 * - substitute persistent constants (true, false, null, etc)
 * - perform compile-time evaluation of constant binary and unary operations
 * - optimize series of ADD_STRING and/or ADD_CHAR
 * - convert CAST(IS_BOOL,x) into BOOL(x)
 * - pre-evaluate constant function calls
 */

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"

#define ZEND_IS_CONSTANT_TYPE(t)	((t) == IS_CONSTANT)

void zend_optimizer_pass1(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	int i = 0;
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	zend_bool collect_constants = (ZEND_OPTIMIZER_PASS_15 & OPTIMIZATION_LEVEL)?
		(op_array == &ctx->script->main_op_array) : 0;

	while (opline < end) {
		switch (opline->opcode) {
		case ZEND_ADD:
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_DIV:
		case ZEND_MOD:
		case ZEND_POW:
		case ZEND_SL:
		case ZEND_SR:
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
		case ZEND_IS_EQUAL:
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_SMALLER:
		case ZEND_IS_SMALLER_OR_EQUAL:
		case ZEND_IS_IDENTICAL:
		case ZEND_IS_NOT_IDENTICAL:
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
		case ZEND_BOOL_XOR:
		case ZEND_SPACESHIP:
			if (ZEND_OP1_TYPE(opline) == IS_CONST &&
				ZEND_OP2_TYPE(opline) == IS_CONST) {
				/* binary operation with constant operands */
				binary_op_type binary_op = get_binary_op(opline->opcode);
				uint32_t tv = ZEND_RESULT(opline).var;		/* temporary variable */
				zval result;
				int er;

				if ((opline->opcode == ZEND_DIV || opline->opcode == ZEND_MOD) &&
					zval_get_long(&ZEND_OP2_LITERAL(opline)) == 0) {
					/* div by 0 */
					break;
				} else if ((opline->opcode == ZEND_SL || opline->opcode == ZEND_SR) &&
					zval_get_long(&ZEND_OP2_LITERAL(opline)) < 0) {
					/* shift by negative number */
					break;
				}
				er = EG(error_reporting);
				EG(error_reporting) = 0;
				/* evaluate constant expression */
				if (binary_op(&result, &ZEND_OP1_LITERAL(opline), &ZEND_OP2_LITERAL(opline)) != SUCCESS) {
					EG(error_reporting) = er;
					break;
				}
				EG(error_reporting) = er;

				if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_TMP_VAR, tv, &result)) {
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					literal_dtor(&ZEND_OP2_LITERAL(opline));
					MAKE_NOP(opline);
				}
			}
			break;

		case ZEND_CAST:
			if (ZEND_OP1_TYPE(opline) == IS_CONST &&
				opline->extended_value != IS_ARRAY &&
				opline->extended_value != IS_OBJECT &&
				opline->extended_value != IS_RESOURCE) {
				/* cast of constant operand */
				zend_uchar type = opline->result_type;
				uint32_t tv = ZEND_RESULT(opline).var;		/* temporary variable */
				zval res;
				res = ZEND_OP1_LITERAL(opline);
				zval_copy_ctor(&res);
				switch (opline->extended_value) {
					case IS_NULL:
						convert_to_null(&res);
						break;
					case _IS_BOOL:
						convert_to_boolean(&res);
						break;
					case IS_LONG:
						convert_to_long(&res);
						break;
					case IS_DOUBLE:
						convert_to_double(&res);
						break;
					case IS_STRING:
						convert_to_string(&res);
						break;
				}

				if (zend_optimizer_replace_by_const(op_array, opline + 1, type, tv, &res)) {
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					MAKE_NOP(opline);
				}
			} else if (opline->extended_value == _IS_BOOL) {
				/* T = CAST(X, IS_BOOL) => T = BOOL(X) */
				opline->opcode = ZEND_BOOL;
				opline->extended_value = 0;
			}
			break;

		case ZEND_BW_NOT:
		case ZEND_BOOL_NOT:
			if (ZEND_OP1_TYPE(opline) == IS_CONST) {
				/* unary operation on constant operand */
				unary_op_type unary_op = get_unary_op(opline->opcode);
				zval result;
				uint32_t tv = ZEND_RESULT(opline).var;		/* temporary variable */
				int er;

				er = EG(error_reporting);
				EG(error_reporting) = 0;
				if (unary_op(&result, &ZEND_OP1_LITERAL(opline)) != SUCCESS) {
					EG(error_reporting) = er;
					break;
				}
				EG(error_reporting) = er;

				if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_TMP_VAR, tv, &result)) {
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					MAKE_NOP(opline);
				}
			}
			break;

#if 0
		case ZEND_ADD_STRING:
		case ZEND_ADD_CHAR:
			{
				zend_op *next_op = opline + 1;
				int requires_conversion = (opline->opcode == ZEND_ADD_CHAR? 1 : 0);
				size_t final_length = 0;
				zend_string *str;
				char *ptr;
				zend_op *last_op;

				/* There is always a ZEND_RETURN at the end
				if (next_op>=end) {
					break;
				}
				*/
				while (next_op->opcode == ZEND_ADD_STRING || next_op->opcode == ZEND_ADD_CHAR) {
					if (ZEND_RESULT(opline).var != ZEND_RESULT(next_op).var) {
						break;
					}
					if (next_op->opcode == ZEND_ADD_CHAR) {
						final_length += 1;
					} else { /* ZEND_ADD_STRING */
						final_length += Z_STRLEN(ZEND_OP2_LITERAL(next_op));
					}
					next_op++;
				}
				if (final_length == 0) {
					break;
				}
				last_op = next_op;
				final_length += (requires_conversion? 1 : Z_STRLEN(ZEND_OP2_LITERAL(opline)));
				str = zend_string_alloc(final_length, 0);
				str->len = final_length;
				ptr = str->val;
				ptr[final_length] = '\0';
				if (requires_conversion) { /* ZEND_ADD_CHAR */
					char chval = (char)Z_LVAL(ZEND_OP2_LITERAL(opline));

					ZVAL_NEW_STR(&ZEND_OP2_LITERAL(opline), str);
					ptr[0] = chval;
					opline->opcode = ZEND_ADD_STRING;
					ptr++;
				} else { /* ZEND_ADD_STRING */
					memcpy(ptr, Z_STRVAL(ZEND_OP2_LITERAL(opline)), Z_STRLEN(ZEND_OP2_LITERAL(opline)));
					ptr += Z_STRLEN(ZEND_OP2_LITERAL(opline));
					zend_string_release(Z_STR(ZEND_OP2_LITERAL(opline)));
					ZVAL_NEW_STR(&ZEND_OP2_LITERAL(opline), str);
				}
				next_op = opline + 1;
				while (next_op < last_op) {
					if (next_op->opcode == ZEND_ADD_STRING) {
						memcpy(ptr, Z_STRVAL(ZEND_OP2_LITERAL(next_op)), Z_STRLEN(ZEND_OP2_LITERAL(next_op)));
						ptr += Z_STRLEN(ZEND_OP2_LITERAL(next_op));
						literal_dtor(&ZEND_OP2_LITERAL(next_op));
					} else { /* ZEND_ADD_CHAR */
						*ptr = (char)Z_LVAL(ZEND_OP2_LITERAL(next_op));
						ptr++;
					}
					MAKE_NOP(next_op);
					next_op++;
				}
				if (!((ZEND_OPTIMIZER_PASS_5|ZEND_OPTIMIZER_PASS_10) & OPTIMIZATION_LEVEL)) {
					/* NOP removal is disabled => insert JMP over NOPs */
					if (last_op-opline >= 3) { /* If we have more than 2 NOPS then JMP over them */
						(opline + 1)->opcode = ZEND_JMP;
						ZEND_OP1(opline + 1).opline_num = last_op - op_array->opcodes; /* that's OK even for ZE2, since opline_num's are resolved in pass 2 later */
					}
				}
			}
			break;
#endif

		case ZEND_FETCH_CONSTANT:
			if (ZEND_OP1_TYPE(opline) == IS_UNUSED &&
				ZEND_OP2_TYPE(opline) == IS_CONST &&
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
					uint32_t tv = ZEND_RESULT(opline).var;

					if (zend_optimizer_replace_by_const(op_array, opline, IS_TMP_VAR, tv, offset)) {
						literal_dtor(&ZEND_OP2_LITERAL(opline));
						MAKE_NOP(opline);
					}
				}
				EG(current_execute_data) = orig_execute_data;
				break;
			}

			if (ZEND_OP1_TYPE(opline) == IS_UNUSED &&
				ZEND_OP2_TYPE(opline) == IS_CONST &&
				Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING) {
				/* substitute persistent constants */
				uint32_t tv = ZEND_RESULT(opline).var;
				zval c;

				if (!zend_optimizer_get_persistent_constant(Z_STR(ZEND_OP2_LITERAL(opline)), &c, 1)) {
					if (!ctx->constants || !zend_optimizer_get_collected_constant(ctx->constants, &ZEND_OP2_LITERAL(opline), &c)) {
						break;
					}
				}
				if (Z_TYPE(c) == IS_CONSTANT_AST) {
					break;
				}
				if (zend_optimizer_replace_by_const(op_array, opline, IS_TMP_VAR, tv, &c)) {
					literal_dtor(&ZEND_OP2_LITERAL(opline));
					MAKE_NOP(opline);
				}
			}

			/* class constant */
			if (ZEND_OP1_TYPE(opline) != IS_UNUSED &&
			    ZEND_OP2_TYPE(opline) == IS_CONST &&
				Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING) {

				zend_class_entry *ce = NULL;

				if (ZEND_OP1_TYPE(opline) == IS_CONST &&
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
								 ZEND_CE_FILENAME(ce) != op_array->filename)) {
							break;
						}
					}
				} else if (op_array->scope &&
					ZEND_OP1_TYPE(opline) == IS_VAR &&
					(opline - 1)->opcode == ZEND_FETCH_CLASS &&
					(ZEND_OP1_TYPE(opline - 1) == IS_UNUSED &&
					((opline - 1)->extended_value & ~ZEND_FETCH_CLASS_NO_AUTOLOAD) == ZEND_FETCH_CLASS_SELF) &&
					ZEND_RESULT((opline - 1)).var == ZEND_OP1(opline).var) {
					/* for self::B */
					ce = op_array->scope;
				}

				if (ce) {
					uint32_t tv = ZEND_RESULT(opline).var;
					zval *c, t;

					if ((c = zend_hash_find(&ce->constants_table,
							Z_STR(ZEND_OP2_LITERAL(opline)))) != NULL) {
						ZVAL_DEREF(c);
						if (Z_TYPE_P(c) == IS_CONSTANT_AST) {
							break;
						}
						if (ZEND_IS_CONSTANT_TYPE(Z_TYPE_P(c))) {
							if (!zend_optimizer_get_persistent_constant(Z_STR_P(c), &t, 1) ||
							    ZEND_IS_CONSTANT_TYPE(Z_TYPE(t))) {
								break;
							}
						} else {
							ZVAL_COPY_VALUE(&t, c);
							zval_copy_ctor(&t);
						}

						if (ZEND_OP1_TYPE(opline) == IS_CONST) {
							literal_dtor(&ZEND_OP1_LITERAL(opline));
						} else {
							MAKE_NOP((opline - 1));
						}
						if (zend_optimizer_replace_by_const(op_array, opline, IS_TMP_VAR, tv, &t)) {
							literal_dtor(&ZEND_OP2_LITERAL(opline));
							MAKE_NOP(opline);
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
			    ZEND_OP1_TYPE(send1_opline) != IS_CONST) {
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
				    ZEND_OP1_TYPE(send1_opline) != IS_CONST) {
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
			    ZEND_OP2_TYPE(init_opline) != IS_CONST ||
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
			   defined(x)
			   constant(x)
			   function_exists(x)
			   is_callable(x)
			   extension_loaded(x)
			*/
			if (!send2_opline &&
			    Z_TYPE(ZEND_OP1_LITERAL(send1_opline)) == IS_STRING) {
				if ((Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("function_exists")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"function_exists", sizeof("function_exists")-1) &&
					!zend_optimizer_is_disabled_func("function_exists", sizeof("function_exists") - 1)) ||
					(Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("is_callable")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"is_callable", sizeof("is_callable")) &&
					!zend_optimizer_is_disabled_func("is_callable", sizeof("is_callable") - 1))) {
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
						if (Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("is_callable") - 1 ||
								func->handler != ZEND_FN(display_disabled_function)) {
							ZVAL_TRUE(&t);
						} else {
							ZVAL_FALSE(&t);
						}
						if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_VAR, ZEND_RESULT(opline).var, &t)) {
							literal_dtor(&ZEND_OP2_LITERAL(init_opline));
							MAKE_NOP(init_opline);
							literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
							MAKE_NOP(send1_opline);
							MAKE_NOP(opline);
							zend_string_release(lc_name);
							break;
						}
					}
					zend_string_release(lc_name);
				} else if (Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("extension_loaded")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"extension_loaded", sizeof("extension_loaded")-1) &&
					!zend_optimizer_is_disabled_func("extension_loaded", sizeof("extension_loaded") - 1)) {
					zval t;
					zend_string *lc_name = zend_string_tolower(
							Z_STR(ZEND_OP1_LITERAL(send1_opline)));
					zend_module_entry *m = zend_hash_find_ptr(&module_registry,
							lc_name);

					zend_string_release(lc_name);
					if (!m) {
						if (!PG(enable_dl)) {
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

					if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_VAR, ZEND_RESULT(opline).var, &t)) {
						literal_dtor(&ZEND_OP2_LITERAL(init_opline));
						MAKE_NOP(init_opline);
						literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
						MAKE_NOP(send1_opline);
						MAKE_NOP(opline);
						break;
					}
				} else if (Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("defined")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"defined", sizeof("defined")-1) &&
					!zend_optimizer_is_disabled_func("defined", sizeof("defined") - 1)) {
					zval t;

					if (zend_optimizer_get_persistent_constant(Z_STR(ZEND_OP1_LITERAL(send1_opline)), &t, 0)) {

						ZVAL_TRUE(&t);
						if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_VAR, ZEND_RESULT(opline).var, &t)) {
							literal_dtor(&ZEND_OP2_LITERAL(init_opline));
							MAKE_NOP(init_opline);
							literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
							MAKE_NOP(send1_opline);
							MAKE_NOP(opline);
							break;
						}
					}
				} else if (Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("constant")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"constant", sizeof("constant")-1) &&
					!zend_optimizer_is_disabled_func("constant", sizeof("constant") - 1)) {
					zval t;

					if (zend_optimizer_get_persistent_constant(Z_STR(ZEND_OP1_LITERAL(send1_opline)), &t, 1)) {
						if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_VAR, ZEND_RESULT(opline).var, &t)) {
							literal_dtor(&ZEND_OP2_LITERAL(init_opline));
							MAKE_NOP(init_opline);
							literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
							MAKE_NOP(send1_opline);
							MAKE_NOP(opline);
							break;
						}
					}
				} else if ((CG(compiler_options) & ZEND_COMPILE_NO_BUILTIN_STRLEN) == 0 &&
					Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("strlen") - 1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)), "strlen", sizeof("strlen") - 1) &&
					!zend_optimizer_is_disabled_func("strlen", sizeof("strlen") - 1)) {
					zval t;

					ZVAL_LONG(&t, Z_STRLEN(ZEND_OP1_LITERAL(send1_opline)));
					if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_VAR, ZEND_RESULT(opline).var, &t)) {
						literal_dtor(&ZEND_OP2_LITERAL(init_opline));
						MAKE_NOP(init_opline);
						literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
						MAKE_NOP(send1_opline);
						MAKE_NOP(opline);
						break;
					}
				/* dirname(IS_CONST/IS_STRING) -> IS_CONST/IS_STRING */
				} else if (Z_STRLEN(ZEND_OP2_LITERAL(init_opline)) == sizeof("dirname")-1 &&
					!memcmp(Z_STRVAL(ZEND_OP2_LITERAL(init_opline)),
						"dirname", sizeof("dirname") - 1) &&
					!zend_optimizer_is_disabled_func("dirname", sizeof("dirname") - 1) &&
					IS_ABSOLUTE_PATH(Z_STRVAL(ZEND_OP1_LITERAL(send1_opline)), Z_STRLEN(ZEND_OP1_LITERAL(send1_opline)))) {
					zend_string *dirname = zend_string_init(Z_STRVAL(ZEND_OP1_LITERAL(send1_opline)), Z_STRLEN(ZEND_OP1_LITERAL(send1_opline)), 0);
					ZSTR_LEN(dirname) = zend_dirname(ZSTR_VAL(dirname), ZSTR_LEN(dirname));
					if (IS_ABSOLUTE_PATH(ZSTR_VAL(dirname), ZSTR_LEN(dirname))) {
						zval t;

						ZVAL_STR(&t, dirname);
						if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_VAR, ZEND_RESULT(opline).var, &t)) {
							literal_dtor(&ZEND_OP2_LITERAL(init_opline));
							MAKE_NOP(init_opline);
							literal_dtor(&ZEND_OP1_LITERAL(send1_opline));
							MAKE_NOP(send1_opline);
							MAKE_NOP(opline);
							break;
						}
					} else {
						zend_string_release(dirname);
					}
				}
			}
			/* don't colllect constants after any other function call */
			collect_constants = 0;
			break;
		}
		case ZEND_STRLEN:
			if (ZEND_OP1_TYPE(opline) == IS_CONST &&
			    Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_STRING) {
				zval t;

				ZVAL_LONG(&t, Z_STRLEN(ZEND_OP1_LITERAL(opline)));
				if (zend_optimizer_replace_by_const(op_array, opline + 1, IS_TMP_VAR, ZEND_RESULT(opline).var, &t)) {
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					MAKE_NOP(opline);
				}
			}
			break;
		case ZEND_DEFINED:
			{
				zval c;
				uint32_t tv = ZEND_RESULT(opline).var;
				if (!zend_optimizer_get_persistent_constant(Z_STR(ZEND_OP1_LITERAL(opline)), &c, 0)) {
					break;
				}
				ZVAL_TRUE(&c);
				if (zend_optimizer_replace_by_const(op_array, opline, IS_TMP_VAR, tv, &c)) {
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					MAKE_NOP(opline);
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

		case ZEND_RETURN:
		case ZEND_RETURN_BY_REF:
		case ZEND_GENERATOR_RETURN:
		case ZEND_EXIT:
		case ZEND_THROW:
		case ZEND_CATCH:
		case ZEND_FAST_CALL:
		case ZEND_FAST_RET:
		case ZEND_JMP:
		case ZEND_JMPZNZ:
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
		case ZEND_NEW:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
			collect_constants = 0;
			break;
		case ZEND_FETCH_R:
		case ZEND_FETCH_W:
		case ZEND_FETCH_RW:
		case ZEND_FETCH_FUNC_ARG:
		case ZEND_FETCH_IS:
		case ZEND_FETCH_UNSET:
			if (opline != op_array->opcodes &&
			    (opline-1)->opcode == ZEND_BEGIN_SILENCE &&
			    (opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_LOCAL &&
				opline->op1_type == IS_CONST &&
			    opline->op2_type == IS_UNUSED &&
			    Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_STRING &&
			    (Z_STRLEN(ZEND_OP1_LITERAL(opline)) != sizeof("this")-1 ||
			     memcmp(Z_STRVAL(ZEND_OP1_LITERAL(opline)), "this", sizeof("this") - 1) != 0)) {

			    int var = opline->result.var;
			    int level = 0;
				zend_op *op = opline + 1;
				zend_op *use = NULL;

				while (op < end) {
					if (op->opcode == ZEND_BEGIN_SILENCE) {
						level++;
					} else if (op->opcode == ZEND_END_SILENCE) {
						if (level == 0) {
							break;
						} else {
							level--;
						}
					}
					if (op->op1_type == IS_VAR && op->op1.var == var) {
						if (use) {
							/* used more than once */
							use = NULL;
							break;
						}
						use = op;
					} else if (op->op2_type == IS_VAR && op->op2.var == var) {
						if (use) {
							/* used more than once */
							use = NULL;
							break;
						}
						use = op;
					}
					op++;
				}
				if (use) {
					if (use->op1_type == IS_VAR && use->op1.var == var) {
						use->op1_type = IS_CV;
						use->op1.var = zend_optimizer_lookup_cv(op_array,
							Z_STR(ZEND_OP1_LITERAL(opline)));
						MAKE_NOP(opline);
					} else if (use->op2_type == IS_VAR && use->op2.var == var) {
						use->op2_type = IS_CV;
						use->op2.var = zend_optimizer_lookup_cv(op_array,
							Z_STR(ZEND_OP1_LITERAL(opline)));
						MAKE_NOP(opline);
					}
				}
			}
			break;
		}
		opline++;
		i++;
	}
}
