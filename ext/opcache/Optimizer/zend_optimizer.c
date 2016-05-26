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
#include "zend_cfg.h"
#include "zend_func_info.h"
#include "zend_call_graph.h"
#include "zend_inference.h"
#include "zend_dump.h"

#ifndef HAVE_DFA_PASS
# define HAVE_DFA_PASS 1
#endif

static void zend_optimizer_zval_dtor_wrapper(zval *zvalue)
{
	zval_dtor(zvalue);
}

void zend_optimizer_collect_constant(zend_optimizer_ctx *ctx, zval *name, zval* value)
{
	zval val;

	if (!ctx->constants) {
		ctx->constants = zend_arena_alloc(&ctx->arena, sizeof(HashTable));
		zend_hash_init(ctx->constants, 16, NULL, zend_optimizer_zval_dtor_wrapper, 0);
	}
	ZVAL_DUP(&val, value);
	zend_hash_add(ctx->constants, Z_STR_P(name), &val);
}

int zend_optimizer_get_collected_constant(HashTable *constants, zval *name, zval* value)
{
	zval *val;

	if ((val = zend_hash_find(constants, Z_STR_P(name))) != NULL) {
		ZVAL_DUP(value, val);
		return 1;
	}
	return 0;
}

int zend_optimizer_lookup_cv(zend_op_array *op_array, zend_string* name)
{
	int i = 0;
	zend_ulong hash_value = zend_string_hash_val(name);

	while (i < op_array->last_var) {
		if (op_array->vars[i] == name ||
		    (ZSTR_H(op_array->vars[i]) == hash_value &&
		     ZSTR_LEN(op_array->vars[i]) == ZSTR_LEN(name) &&
		     memcmp(ZSTR_VAL(op_array->vars[i]), ZSTR_VAL(name), ZSTR_LEN(name)) == 0)) {
			return (int)(zend_intptr_t)ZEND_CALL_VAR_NUM(NULL, i);
		}
		i++;
	}
	i = op_array->last_var;
	op_array->last_var++;
	op_array->vars = erealloc(op_array->vars, op_array->last_var * sizeof(zend_string*));
	op_array->vars[i] = zend_string_dup(name, 0);

	/* all IS_TMP_VAR and IS_VAR variable numbers have to be adjusted */
	{
		zend_op *opline = op_array->opcodes;
		zend_op *end = opline + op_array->last;
		while (opline < end) {
			if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
				opline->op1.var += sizeof(zval);
			}
			if (opline->op2_type & (IS_TMP_VAR|IS_VAR)) {
				opline->op2.var += sizeof(zval);
			}
			if (opline->result_type & (IS_TMP_VAR|IS_VAR)) {
				opline->result.var += sizeof(zval);
			}
			opline++;
		}
	}

	return (int)(zend_intptr_t)ZEND_CALL_VAR_NUM(NULL, i);
}

int zend_optimizer_add_literal(zend_op_array *op_array, zval *zv)
{
	int i = op_array->last_literal;
	op_array->last_literal++;
	op_array->literals = (zval*)erealloc(op_array->literals, op_array->last_literal * sizeof(zval));
	ZVAL_COPY_VALUE(&op_array->literals[i], zv);
	Z_CACHE_SLOT(op_array->literals[i]) = -1;
	return i;
}

static inline int zend_optimizer_add_literal_string(zend_op_array *op_array, zend_string *str) {
	zval zv;
	ZVAL_STR(&zv, str);
	zend_string_hash_val(str);
	return zend_optimizer_add_literal(op_array, &zv);
}

int zend_optimizer_is_disabled_func(const char *name, size_t len) {
	zend_function *fbc = (zend_function *)zend_hash_str_find_ptr(EG(function_table), name, len);

	return (fbc && fbc->type == ZEND_INTERNAL_FUNCTION &&
			fbc->internal_function.handler == ZEND_FN(display_disabled_function));
}

static inline void drop_leading_backslash(zval *val) {
	if (Z_STRVAL_P(val)[0] == '\\') {
		zend_string *str = zend_string_init(Z_STRVAL_P(val) + 1, Z_STRLEN_P(val) - 1, 0);
		zval_dtor(val);
		ZVAL_STR(val, str);
	}
}

static inline void alloc_cache_slots_op1(zend_op_array *op_array, zend_op *opline, uint32_t num) {
	Z_CACHE_SLOT(op_array->literals[opline->op1.constant]) = op_array->cache_size;
	op_array->cache_size += num * sizeof(void *);
}
static inline void alloc_cache_slots_op2(zend_op_array *op_array, zend_op *opline, uint32_t num) {
	Z_CACHE_SLOT(op_array->literals[opline->op2.constant]) = op_array->cache_size;
	op_array->cache_size += num * sizeof(void *);
}

#define REQUIRES_STRING(val) do { \
	if (Z_TYPE_P(val) != IS_STRING) { \
		zval_dtor(val); \
		return 0; \
	} \
} while (0)

#define TO_STRING_NOWARN(val) do { \
	if (Z_TYPE_P(val) >= IS_ARRAY) { \
		zval_dtor(val); \
		return 0; \
	} \
	convert_to_string(val); \
} while (0)

int zend_optimizer_update_op1_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zval          *val)
{
	switch (opline->opcode) {
		case ZEND_FREE:
			MAKE_NOP(opline);
			zval_dtor(val);
			return 1;
		case ZEND_INIT_STATIC_METHOD_CALL:
		case ZEND_CATCH:
		case ZEND_FETCH_CONSTANT:
		case ZEND_FETCH_CLASS_CONSTANT:
		case ZEND_DEFINED:
		case ZEND_NEW:
			REQUIRES_STRING(val);
			drop_leading_backslash(val);
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			alloc_cache_slots_op1(op_array, opline, 1);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			break;
		case ZEND_FETCH_STATIC_PROP_R:
		case ZEND_FETCH_STATIC_PROP_W:
		case ZEND_FETCH_STATIC_PROP_RW:
		case ZEND_FETCH_STATIC_PROP_IS:
		case ZEND_FETCH_STATIC_PROP_UNSET:
		case ZEND_FETCH_STATIC_PROP_FUNC_ARG:
			TO_STRING_NOWARN(val);
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			alloc_cache_slots_op1(op_array, opline, 2);
			break;
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
		case ZEND_FETCH_R:
		case ZEND_FETCH_W:
		case ZEND_FETCH_RW:
		case ZEND_FETCH_IS:
		case ZEND_FETCH_UNSET:
		case ZEND_FETCH_FUNC_ARG:
			TO_STRING_NOWARN(val);
			/* break missing intentionally */
		default:
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			break;
	}

	ZEND_OP1_TYPE(opline) = IS_CONST;
	if (Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_STRING) {
		zend_string_hash_val(Z_STR(ZEND_OP1_LITERAL(opline)));
	}
	return 1;
}

int zend_optimizer_update_op2_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zval          *val)
{
	switch (opline->opcode) {
		case ZEND_ASSIGN_REF:
			zval_dtor(val);
			return 0;
		case ZEND_FETCH_CLASS:
		case ZEND_INIT_FCALL_BY_NAME:
		/*case ZEND_INIT_NS_FCALL_BY_NAME:*/
		case ZEND_ADD_INTERFACE:
		case ZEND_ADD_TRAIT:
		case ZEND_INSTANCEOF:
		case ZEND_FETCH_STATIC_PROP_R:
		case ZEND_FETCH_STATIC_PROP_W:
		case ZEND_FETCH_STATIC_PROP_RW:
		case ZEND_FETCH_STATIC_PROP_IS:
		case ZEND_FETCH_STATIC_PROP_UNSET:
		case ZEND_FETCH_STATIC_PROP_FUNC_ARG:
		case ZEND_UNSET_STATIC_PROP:
		case ZEND_ISSET_ISEMPTY_STATIC_PROP:
			REQUIRES_STRING(val);
			drop_leading_backslash(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			alloc_cache_slots_op2(op_array, opline, 1);
			break;
		case ZEND_INIT_FCALL:
			REQUIRES_STRING(val);
			zend_str_tolower(Z_STRVAL_P(val), Z_STRLEN_P(val));
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			alloc_cache_slots_op2(op_array, opline, 1);
			break;
		case ZEND_INIT_DYNAMIC_CALL:
			if (Z_TYPE_P(val) == IS_STRING) {
				if (zend_memrchr(Z_STRVAL_P(val), ':', Z_STRLEN_P(val))) {
					zval_dtor(val);
					return 0;
				}

				opline->opcode = ZEND_INIT_FCALL_BY_NAME;
				drop_leading_backslash(val);
				opline->op2.constant = zend_optimizer_add_literal(op_array, val);
				zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
				alloc_cache_slots_op2(op_array, opline, 1);
			} else {
				opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			}
			break;
		case ZEND_INIT_METHOD_CALL:
		case ZEND_INIT_STATIC_METHOD_CALL:
			REQUIRES_STRING(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			alloc_cache_slots_op2(op_array, opline, 2);
			break;
		/*case ZEND_FETCH_CLASS_CONSTANT:*/
		case ZEND_ASSIGN_OBJ:
		case ZEND_FETCH_OBJ_R:
		case ZEND_FETCH_OBJ_W:
		case ZEND_FETCH_OBJ_RW:
		case ZEND_FETCH_OBJ_IS:
		case ZEND_FETCH_OBJ_UNSET:
		case ZEND_FETCH_OBJ_FUNC_ARG:
		case ZEND_UNSET_OBJ:
		case ZEND_PRE_INC_OBJ:
		case ZEND_PRE_DEC_OBJ:
		case ZEND_POST_INC_OBJ:
		case ZEND_POST_DEC_OBJ:
		case ZEND_ISSET_ISEMPTY_PROP_OBJ:
			TO_STRING_NOWARN(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			alloc_cache_slots_op2(op_array, opline, 2);
			break;
		case ZEND_ASSIGN_ADD:
		case ZEND_ASSIGN_SUB:
		case ZEND_ASSIGN_MUL:
		case ZEND_ASSIGN_DIV:
		case ZEND_ASSIGN_POW:
		case ZEND_ASSIGN_MOD:
		case ZEND_ASSIGN_SL:
		case ZEND_ASSIGN_SR:
		case ZEND_ASSIGN_CONCAT:
		case ZEND_ASSIGN_BW_OR:
		case ZEND_ASSIGN_BW_AND:
		case ZEND_ASSIGN_BW_XOR:
			if (opline->extended_value == ZEND_ASSIGN_OBJ) {
				TO_STRING_NOWARN(val);
				opline->op2.constant = zend_optimizer_add_literal(op_array, val);
				alloc_cache_slots_op2(op_array, opline, 2);
			} else {
				opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			}
			break;
		case ZEND_ISSET_ISEMPTY_DIM_OBJ:
		case ZEND_ADD_ARRAY_ELEMENT:
		case ZEND_INIT_ARRAY:
		case ZEND_ASSIGN_DIM:
		case ZEND_UNSET_DIM:
		case ZEND_FETCH_DIM_R:
		case ZEND_FETCH_DIM_W:
		case ZEND_FETCH_DIM_RW:
		case ZEND_FETCH_DIM_IS:
		case ZEND_FETCH_DIM_FUNC_ARG:
		case ZEND_FETCH_DIM_UNSET:
		case ZEND_FETCH_LIST:
			if (Z_TYPE_P(val) == IS_STRING) {
				zend_ulong index;
				if (ZEND_HANDLE_NUMERIC(Z_STR_P(val), index)) {
					zval_dtor(val);
					ZVAL_LONG(val, index);
				}
			}
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			break;
		case ZEND_ROPE_INIT:
		case ZEND_ROPE_ADD:
		case ZEND_ROPE_END:
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
			TO_STRING_NOWARN(val);
			/* break missing intentionally */
		default:
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			break;
	}

	ZEND_OP2_TYPE(opline) = IS_CONST;
	if (Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING) {
		zend_string_hash_val(Z_STR(ZEND_OP2_LITERAL(opline)));
	}
	return 1;
}

void zend_optimizer_remove_live_range(zend_op_array *op_array, uint32_t var)
{
	if (op_array->last_live_range) {
		int i = 0;
		int j = 0;
		uint32_t *map;
		ALLOCA_FLAG(use_heap);

		map = (uint32_t *)do_alloca(sizeof(uint32_t) * op_array->last_live_range, use_heap);

		do {
			if ((op_array->live_range[i].var & ~ZEND_LIVE_MASK) != var) {
				map[i] = j;
				if (i != j) {
					op_array->live_range[j] = op_array->live_range[i];
				}
				j++;
			}
			i++;
		} while (i < op_array->last_live_range);
		if (i != j) {
			if ((op_array->last_live_range = j)) {
				zend_op *opline = op_array->opcodes;
				zend_op *end = opline + op_array->last;

				while (opline != end) {
					if ((opline->opcode == ZEND_FREE || opline->opcode == ZEND_FE_FREE) &&
							opline->extended_value == ZEND_FREE_ON_RETURN) {
						opline->op2.num = map[opline->op2.num];
					}
					opline++;
				}
			} else {
				efree(op_array->live_range);
				op_array->live_range = NULL;
			}
		}
		free_alloca(map, use_heap);
	}
}

int zend_optimizer_replace_by_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zend_uchar     type,
                                    uint32_t       var,
                                    zval          *val)
{
	zend_op *end = op_array->opcodes + op_array->last;

	while (opline < end) {
		if (ZEND_OP1_TYPE(opline) == type &&
			ZEND_OP1(opline).var == var) {
			switch (opline->opcode) {
				case ZEND_FETCH_DIM_W:
				case ZEND_FETCH_DIM_RW:
				case ZEND_FETCH_DIM_FUNC_ARG:
				case ZEND_FETCH_DIM_UNSET:
				case ZEND_ASSIGN_DIM:
				case ZEND_SEPARATE:
				case ZEND_RETURN_BY_REF:
					zval_dtor(val);
					return 0;
				case ZEND_SEND_VAR:
					opline->extended_value = 0;
					opline->opcode = ZEND_SEND_VAL;
					break;
				case ZEND_SEND_VAR_EX:
					opline->extended_value = 0;
					opline->opcode = ZEND_SEND_VAL_EX;
					break;
				case ZEND_SEND_VAR_NO_REF:
					if (opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) {
						if (opline->extended_value & ZEND_ARG_SEND_BY_REF) {
							zval_dtor(val);
							return 0;
						}
						opline->extended_value = 0;
						opline->opcode = ZEND_SEND_VAL_EX;
					} else {
						opline->extended_value = 0;
						opline->opcode = ZEND_SEND_VAL;
					}
					break;
				case ZEND_SEND_USER:
					opline->opcode = ZEND_SEND_VAL_EX;
					break;
				/* In most cases IS_TMP_VAR operand may be used only once.
				 * The operands are usually destroyed by the opcode handler.
				 * ZEND_CASE is an exception, that keeps operand unchanged,
				 * and allows its reuse. The number of ZEND_CASE instructions
				 * usually terminated by ZEND_FREE that finally kills the value.
				 */
				case ZEND_FREE:
				case ZEND_CASE: {
					zend_op *m, *n;
					int brk = op_array->last_live_range;
					zend_bool in_switch = 0;
					while (brk--) {
						if (op_array->live_range[brk].start <= (uint32_t)(opline - op_array->opcodes) &&
						    op_array->live_range[brk].end > (uint32_t)(opline - op_array->opcodes)) {
							in_switch = 1;
							break;
						}
					}

					if (!in_switch) {
						ZEND_ASSERT(opline->opcode == ZEND_FREE);
						MAKE_NOP(opline);
						zval_dtor(val);
						return 1;
					}

					m = opline;
					n = op_array->opcodes + op_array->live_range[brk].end;
					if (n->opcode == ZEND_FREE &&
					    !(n->extended_value & ZEND_FREE_ON_RETURN)) {
						n++;
					} else {
						n = op_array->opcodes + op_array->last;
					}
					while (m < n) {
						if (ZEND_OP1_TYPE(m) == type &&
								ZEND_OP1(m).var == var) {
							if (m->opcode == ZEND_CASE) {
								zval old_val;
								ZVAL_COPY_VALUE(&old_val, val);
								zval_copy_ctor(val);
								zend_optimizer_update_op1_const(op_array, m, val);
								ZVAL_COPY_VALUE(val, &old_val);
							} else if (m->opcode == ZEND_FREE) {
								MAKE_NOP(m);
							} else {
								ZEND_ASSERT(0);
							}
						}
						m++;
					}
					zval_dtor(val);
					zend_optimizer_remove_live_range(op_array, var);
					return 1;
				}
				case ZEND_VERIFY_RETURN_TYPE: {
					zend_arg_info *ret_info = op_array->arg_info - 1;
					ZEND_ASSERT((opline + 1)->opcode == ZEND_RETURN || (opline + 1)->opcode == ZEND_RETURN_BY_REF);
					if (ret_info->class_name
						|| ret_info->type_hint == IS_CALLABLE
						|| !ZEND_SAME_FAKE_TYPE(ret_info->type_hint, Z_TYPE_P(val))
						|| (op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
						zval_dtor(val);
						return 0;
					}
					MAKE_NOP(opline);
					opline++;
					break;
				  }
				default:
					break;
			}
			if (zend_optimizer_update_op1_const(op_array, opline, val)) {
				zend_optimizer_remove_live_range(op_array, var);
				return 1;
			}
			return 0;
		}

		if (ZEND_OP2_TYPE(opline) == type &&
			ZEND_OP2(opline).var == var) {
			if (zend_optimizer_update_op2_const(op_array, opline, val)) {
				zend_optimizer_remove_live_range(op_array, var);
				return 1;
			}
			return 0;
		}
		opline++;
	}

	return 1;
}

zend_function *zend_optimizer_get_called_func(
		zend_script *script, zend_op_array *op_array, zend_op *opline, zend_bool rt_constants)
{
#define GET_OP(op) CRT_CONSTANT_EX(op_array, opline->op, rt_constants)
	switch (opline->opcode) {
		case ZEND_INIT_FCALL:
		{
			zend_string *function_name = Z_STR_P(GET_OP(op2));
			zend_function *func;
			if ((func = zend_hash_find_ptr(&script->function_table, function_name)) != NULL) {
				return func;
			} else if ((func = zend_hash_find_ptr(EG(function_table), function_name)) != NULL) {
				ZEND_ASSERT(func->type == ZEND_INTERNAL_FUNCTION);
				return func;
			}
			break;
		}
		case ZEND_INIT_FCALL_BY_NAME:
		case ZEND_INIT_NS_FCALL_BY_NAME:
			if (opline->op2_type == IS_CONST && Z_TYPE_P(GET_OP(op2)) == IS_STRING) {
				zval *function_name = GET_OP(op2) + 1;
				return zend_hash_find_ptr(&script->function_table, Z_STR_P(function_name));
			}
			break;
		case ZEND_INIT_STATIC_METHOD_CALL:
			if (opline->op2_type == IS_CONST && Z_TYPE_P(GET_OP(op2)) == IS_STRING) {
				zend_class_entry *ce = NULL;
				if (opline->op1_type == IS_CONST && Z_TYPE_P(GET_OP(op1)) == IS_STRING) {
					zend_string *class_name = Z_STR_P(GET_OP(op1) + 1);
					ce = zend_hash_find_ptr(&script->class_table, class_name);
				} else if (opline->op1_type == IS_UNUSED && op_array->scope
						&& !(op_array->scope->ce_flags & ZEND_ACC_TRAIT)
						&& (opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_SELF) {
					ce = op_array->scope;
				}
				if (ce) {
					zend_string *func_name = Z_STR_P(GET_OP(op2) + 1);
					return zend_hash_find_ptr(&ce->function_table, func_name);
				}
			}
			break;
		case ZEND_INIT_METHOD_CALL:
			if (opline->op1_type == IS_UNUSED
					&& opline->op2_type == IS_CONST && Z_TYPE_P(GET_OP(op2)) == IS_STRING
					&& op_array->scope && !(op_array->scope->ce_flags & ZEND_ACC_TRAIT)) {
				zend_string *method_name = Z_STR_P(GET_OP(op2) + 1);
				zend_function *fbc = zend_hash_find_ptr(
					&op_array->scope->function_table, method_name);
				if (fbc) {
					zend_bool is_private = (fbc->common.fn_flags & ZEND_ACC_PRIVATE) != 0;
					zend_bool is_final = (fbc->common.fn_flags & ZEND_ACC_FINAL) != 0;
					zend_bool same_scope = fbc->common.scope == op_array->scope;
					if ((is_private && same_scope)
							|| (is_final && (!is_private || same_scope))) {
						return fbc;
					}
				}
			}
			break;
	}
	return NULL;
#undef GET_OP
}

static void zend_optimize(zend_op_array      *op_array,
                          zend_optimizer_ctx *ctx)
{
	if (op_array->type == ZEND_EVAL_CODE) {
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_BEFORE_OPTIMIZER) {
		zend_dump_op_array(op_array, 0, "before optimizer", NULL);
	}

	/* pass 1
	 * - substitute persistent constants (true, false, null, etc)
	 * - perform compile-time evaluation of constant binary and unary operations
	 * - optimize series of ADD_STRING and/or ADD_CHAR
	 * - convert CAST(IS_BOOL,x) into BOOL(x)
	 */
	if (ZEND_OPTIMIZER_PASS_1 & ctx->optimization_level) {
		zend_optimizer_pass1(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_1) {
			zend_dump_op_array(op_array, 0, "after pass 1", NULL);
		}
	}

	/* pass 2:
	 * - convert non-numeric constants to numeric constants in numeric operators
	 * - optimize constant conditional JMPs
	 * - optimize static BRKs and CONTs
	 * - pre-evaluate constant function calls
	 */
	if (ZEND_OPTIMIZER_PASS_2 & ctx->optimization_level) {
		zend_optimizer_pass2(op_array);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_2) {
			zend_dump_op_array(op_array, 0, "after pass 2", NULL);
		}
	}

	/* pass 3:
	 * - optimize $i = $i+expr to $i+=expr
	 * - optimize series of JMPs
	 * - change $i++ to ++$i where possible
	 */
	if (ZEND_OPTIMIZER_PASS_3 & ctx->optimization_level) {
		zend_optimizer_pass3(op_array);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_3) {
			zend_dump_op_array(op_array, 0, "after pass 1", NULL);
		}
	}

	/* pass 4:
	 * - INIT_FCALL_BY_NAME -> DO_FCALL
	 */
	if (ZEND_OPTIMIZER_PASS_4 & ctx->optimization_level) {
		zend_optimize_func_calls(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_4) {
			zend_dump_op_array(op_array, 0, "after pass 1", NULL);
		}
	}

	/* pass 5:
	 * - CFG optimization
	 */
	if (ZEND_OPTIMIZER_PASS_5 & ctx->optimization_level) {
		zend_optimize_cfg(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_5) {
			zend_dump_op_array(op_array, 0, "after pass 5", NULL);
		}
	}

#if HAVE_DFA_PASS
	/* pass 6:
	 * - DFA optimization
	 */
	if ((ZEND_OPTIMIZER_PASS_6 & ctx->optimization_level) &&
	    !(ZEND_OPTIMIZER_PASS_7 & ctx->optimization_level)) {
		zend_optimize_dfa(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_6) {
			zend_dump_op_array(op_array, 0, "after pass 6", NULL);
		}
	}
#endif

	/* pass 9:
	 * - Optimize temp variables usage
	 */
	if (ZEND_OPTIMIZER_PASS_9 & ctx->optimization_level) {
		zend_optimize_temporary_variables(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_9) {
			zend_dump_op_array(op_array, 0, "after pass 9", NULL);
		}
	}

	/* pass 10:
	 * - remove NOPs
	 */
	if (((ZEND_OPTIMIZER_PASS_10|ZEND_OPTIMIZER_PASS_5) & ctx->optimization_level) == ZEND_OPTIMIZER_PASS_10) {
		zend_optimizer_nop_removal(op_array);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_10) {
			zend_dump_op_array(op_array, 0, "after pass 10", NULL);
		}
	}

	/* pass 11:
	 * - Compact literals table
	 */
	if (ZEND_OPTIMIZER_PASS_11 & ctx->optimization_level) {
		zend_optimizer_compact_literals(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_11) {
			zend_dump_op_array(op_array, 0, "after pass 11", NULL);
		}
	}

	if (ctx->debug_level & ZEND_DUMP_AFTER_OPTIMIZER) {
		zend_dump_op_array(op_array, 0, "after optimizer", NULL);
	}
}

static void zend_revert_pass_two(zend_op_array *op_array)
{
	zend_op *opline, *end;

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		if (opline->op1_type == IS_CONST) {
			ZEND_PASS_TWO_UNDO_CONSTANT(op_array, opline->op1);
		}
		if (opline->op2_type == IS_CONST) {
			ZEND_PASS_TWO_UNDO_CONSTANT(op_array, opline->op2);
		}
		opline++;
	}
}

static void zend_redo_pass_two(zend_op_array *op_array)
{
	zend_op *opline, *end;

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		if (opline->op1_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline->op1);
		}
		if (opline->op2_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline->op2);
		}
		ZEND_VM_SET_OPCODE_HANDLER(opline);
		opline++;
	}
}

#if HAVE_DFA_PASS
static void zend_redo_pass_two_ex(zend_op_array *op_array, zend_ssa *ssa)
{
	zend_op *opline, *end;

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		zend_vm_set_opcode_handler_ex(opline,
			opline->op1_type == IS_UNUSED ? 0 : (OP1_INFO() & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_KEY_ANY)),
			opline->op2_type == IS_UNUSED ? 0 : (OP2_INFO() & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_KEY_ANY)),
			(opline->opcode == ZEND_PRE_INC ||
			 opline->opcode == ZEND_PRE_DEC ||
			 opline->opcode == ZEND_POST_INC ||
			 opline->opcode == ZEND_POST_DEC) ?
				((ssa->ops[opline - op_array->opcodes].op1_def >= 0) ? (OP1_DEF_INFO() & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_KEY_ANY)) : MAY_BE_ANY) :
				(opline->result_type == IS_UNUSED ? 0 : (RES_INFO() & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_KEY_ANY))));
		if (opline->op1_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline->op1);
		}
		if (opline->op2_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline->op2);
		}
		opline++;
	}
}
#endif

static void zend_optimize_op_array(zend_op_array      *op_array,
                                   zend_optimizer_ctx *ctx)
{
	/* Revert pass_two() */
	zend_revert_pass_two(op_array);

	/* Do actual optimizations */
	zend_optimize(op_array, ctx);

	/* Redo pass_two() */
	zend_redo_pass_two(op_array);
}

static void zend_adjust_fcall_stack_size(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_function *func;
	zend_op *opline, *end;

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		if (opline->opcode == ZEND_INIT_FCALL) {
			func = zend_hash_find_ptr(
				&ctx->script->function_table,
				Z_STR_P(RT_CONSTANT(op_array, opline->op2)));
			if (func) {
				opline->op1.num = zend_vm_calc_used_stack(opline->extended_value, func);
			}
		}
		opline++;
	}
}

#if HAVE_DFA_PASS
static void zend_adjust_fcall_stack_size_graph(zend_op_array *op_array)
{
	zend_func_info *func_info = ZEND_FUNC_INFO(op_array);

	if (func_info) {
		zend_call_info *call_info =func_info->callee_info;

		while (call_info) {
			zend_op *opline = call_info->caller_init_opline;

			if (opline && call_info->callee_func && opline->opcode == ZEND_INIT_FCALL) {
				opline->op1.num = zend_vm_calc_used_stack(opline->extended_value, call_info->callee_func);
			}
			call_info = call_info->next_callee;
		}
	}
}
#endif

int zend_optimize_script(zend_script *script, zend_long optimization_level, zend_long debug_level)
{
	zend_class_entry *ce;
	zend_op_array *op_array;
	zend_string *name;
	zend_optimizer_ctx ctx;
#if HAVE_DFA_PASS
	zend_call_graph call_graph;
#endif

	ctx.arena = zend_arena_create(64 * 1024);
	ctx.script = script;
	ctx.constants = NULL;
	ctx.optimization_level = optimization_level;
	ctx.debug_level = debug_level;

	zend_optimize_op_array(&script->main_op_array, &ctx);

	ZEND_HASH_FOREACH_PTR(&script->function_table, op_array) {
		zend_optimize_op_array(op_array, &ctx);
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_FOREACH_PTR(&script->class_table, ce) {
		ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->function_table, name, op_array) {
			if (op_array->scope == ce) {
				zend_optimize_op_array(op_array, &ctx);
			} else if (op_array->type == ZEND_USER_FUNCTION) {
				zend_op_array *orig_op_array;
				if ((orig_op_array = zend_hash_find_ptr(&op_array->scope->function_table, name)) != NULL) {
					HashTable *ht = op_array->static_variables;
					*op_array = *orig_op_array;
					op_array->static_variables = ht;
				}
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();

#if HAVE_DFA_PASS
	if ((ZEND_OPTIMIZER_PASS_6 & optimization_level) &&
	    (ZEND_OPTIMIZER_PASS_7 & optimization_level) &&
	    zend_build_call_graph(&ctx.arena, script, ZEND_RT_CONSTANTS, &call_graph) == SUCCESS) {
		/* Optimize using call-graph */
		void *checkpoint = zend_arena_checkpoint(ctx.arena);
		int i;
		zend_func_info *func_info;

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			zend_revert_pass_two(call_graph.op_arrays[i]);
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			if (call_graph.op_arrays[i]->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
				func_info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
				if (func_info) {
					zend_init_func_return_info(call_graph.op_arrays[i], script, &func_info->return_info);
				}
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			func_info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (func_info) {
				zend_dfa_analyze_op_array(call_graph.op_arrays[i], &ctx, &func_info->ssa, &func_info->flags);
			}
		}

		//TODO: perform inner-script inference???
		for (i = 0; i < call_graph.op_arrays_count; i++) {
			func_info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (func_info) {
				zend_dfa_optimize_op_array(call_graph.op_arrays[i], &ctx, &func_info->ssa);
			}
		}

		if (debug_level & ZEND_DUMP_AFTER_PASS_7) {
			for (i = 0; i < call_graph.op_arrays_count; i++) {
				zend_dump_op_array(call_graph.op_arrays[i], 0, "after pass 7", NULL);
			}
		}

		if (ZEND_OPTIMIZER_PASS_12 & optimization_level) {
			for (i = 0; i < call_graph.op_arrays_count; i++) {
				zend_adjust_fcall_stack_size_graph(call_graph.op_arrays[i]);
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			func_info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (func_info && func_info->ssa.var_info) {
				zend_redo_pass_two_ex(call_graph.op_arrays[i], &func_info->ssa);
				ZEND_SET_FUNC_INFO(call_graph.op_arrays[i], NULL);
			} else {
				zend_redo_pass_two(call_graph.op_arrays[i]);
			}
		}

		zend_arena_release(&ctx.arena, checkpoint);
	} else
#endif

	if (ZEND_OPTIMIZER_PASS_12 & optimization_level) {
		zend_adjust_fcall_stack_size(&script->main_op_array, &ctx);

		ZEND_HASH_FOREACH_PTR(&script->function_table, op_array) {
			zend_adjust_fcall_stack_size(op_array, &ctx);
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_FOREACH_PTR(&script->class_table, ce) {
			ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->function_table, name, op_array) {
				if (op_array->scope == ce) {
					zend_adjust_fcall_stack_size(op_array, &ctx);
				} else if (op_array->type == ZEND_USER_FUNCTION) {
					zend_op_array *orig_op_array;
					if ((orig_op_array = zend_hash_find_ptr(&op_array->scope->function_table, name)) != NULL) {
						HashTable *ht = op_array->static_variables;
						*op_array = *orig_op_array;
						op_array->static_variables = ht;
					}
				}
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FOREACH_END();
	}

	if (ctx.constants) {
		zend_hash_destroy(ctx.constants);
	}
	zend_arena_destroy(ctx.arena);

	return 1;
}

int zend_optimizer_startup(void)
{
	return zend_func_info_startup();
}

int zend_optimizer_shutdown(void)
{
	return zend_func_info_shutdown();
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
