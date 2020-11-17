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
	zval_ptr_dtor_nogc(zvalue);
}

void zend_optimizer_collect_constant(zend_optimizer_ctx *ctx, zval *name, zval* value)
{
	zval val;

	if (!ctx->constants) {
		ctx->constants = zend_arena_alloc(&ctx->arena, sizeof(HashTable));
		zend_hash_init(ctx->constants, 16, NULL, zend_optimizer_zval_dtor_wrapper, 0);
	}
	ZVAL_COPY(&val, value);
	zend_hash_add(ctx->constants, Z_STR_P(name), &val);
}

int zend_optimizer_eval_binary_op(zval *result, zend_uchar opcode, zval *op1, zval *op2) /* {{{ */
{
	binary_op_type binary_op = get_binary_op(opcode);
	int er, ret;

	if (zend_binary_op_produces_numeric_string_error(opcode, op1, op2)) {
		/* produces numeric string E_NOTICE/E_WARNING */
		return FAILURE;
	}

	switch (opcode) {
		case ZEND_ADD:
			if ((Z_TYPE_P(op1) == IS_ARRAY
			  || Z_TYPE_P(op2) == IS_ARRAY)
			 && Z_TYPE_P(op1) != Z_TYPE_P(op2)) {
				/* produces "Unsupported operand types" exception */
				return FAILURE;
			}
			break;
		case ZEND_DIV:
		case ZEND_MOD:
			if (zval_get_long(op2) == 0) {
				/* division by 0 */
				return FAILURE;
			}
			/* break missing intentionally */
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_POW:
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
			if (Z_TYPE_P(op1) == IS_ARRAY
			 || Z_TYPE_P(op2) == IS_ARRAY) {
				/* produces "Unsupported operand types" exception */
				return FAILURE;
			}
			break;
		case ZEND_SL:
		case ZEND_SR:
			if (zval_get_long(op2) < 0) {
				/* shift by negative number */
				return FAILURE;
			}
			break;
	}

	er = EG(error_reporting);
	EG(error_reporting) = 0;
	ret = binary_op(result, op1, op2);
	EG(error_reporting) = er;

	return ret;
}
/* }}} */

int zend_optimizer_eval_unary_op(zval *result, zend_uchar opcode, zval *op1) /* {{{ */
{
	unary_op_type unary_op = get_unary_op(opcode);

	if (unary_op) {
		if (opcode == ZEND_BW_NOT
		 && Z_TYPE_P(op1) != IS_LONG
		 && Z_TYPE_P(op1) != IS_DOUBLE
		 && Z_TYPE_P(op1) != IS_STRING) {
			/* produces "Unsupported operand types" exception */
			return FAILURE;
		}
		return unary_op(result, op1);
	} else { /* ZEND_BOOL */
		ZVAL_BOOL(result, zend_is_true(op1));
		return SUCCESS;
	}
}
/* }}} */

int zend_optimizer_eval_cast(zval *result, uint32_t type, zval *op1) /* {{{ */
{
	switch (type) {
		case IS_NULL:
			ZVAL_NULL(result);
			return SUCCESS;
		case _IS_BOOL:
			ZVAL_BOOL(result, zval_is_true(op1));
			return SUCCESS;
		case IS_LONG:
			ZVAL_LONG(result, zval_get_long(op1));
			return SUCCESS;
		case IS_DOUBLE:
			ZVAL_DOUBLE(result, zval_get_double(op1));
			return SUCCESS;
		case IS_STRING:
			/* Conversion from double to string takes into account run-time
			   'precision' setting and cannot be evaluated at compile-time */
			if (Z_TYPE_P(op1) != IS_ARRAY && Z_TYPE_P(op1) != IS_DOUBLE) {
				ZVAL_STR(result, zval_get_string(op1));
				return SUCCESS;
			}
			break;
		case IS_ARRAY:
			ZVAL_COPY(result, op1);
			convert_to_array(result);
			return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

int zend_optimizer_eval_strlen(zval *result, zval *op1) /* {{{ */
{
	if (Z_TYPE_P(op1) != IS_STRING) {
		return FAILURE;
	}
	ZVAL_LONG(result, Z_STRLEN_P(op1));
	return SUCCESS;
}
/* }}} */

int zend_optimizer_get_collected_constant(HashTable *constants, zval *name, zval* value)
{
	zval *val;

	if ((val = zend_hash_find(constants, Z_STR_P(name))) != NULL) {
		ZVAL_COPY(value, val);
		return 1;
	}
	return 0;
}

int zend_optimizer_add_literal(zend_op_array *op_array, zval *zv)
{
	int i = op_array->last_literal;
	op_array->last_literal++;
	op_array->literals = (zval*)erealloc(op_array->literals, op_array->last_literal * sizeof(zval));
	ZVAL_COPY_VALUE(&op_array->literals[i], zv);
	Z_EXTRA(op_array->literals[i]) = 0;
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
		zval_ptr_dtor_nogc(val);
		ZVAL_STR(val, str);
	}
}

static inline uint32_t alloc_cache_slots(zend_op_array *op_array, uint32_t num) {
	uint32_t ret = op_array->cache_size;
	op_array->cache_size += num * sizeof(void *);
	return ret;
}

#define REQUIRES_STRING(val) do { \
	if (Z_TYPE_P(val) != IS_STRING) { \
		return 0; \
	} \
} while (0)

#define TO_STRING_NOWARN(val) do { \
	if (Z_TYPE_P(val) >= IS_ARRAY) { \
		return 0; \
	} \
	convert_to_string(val); \
} while (0)

int zend_optimizer_update_op1_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zval          *val)
{
	switch (opline->opcode) {
		case ZEND_OP_DATA:
			switch ((opline-1)->opcode) {
				case ZEND_ASSIGN_OBJ_REF:
				case ZEND_ASSIGN_STATIC_PROP_REF:
					return 0;
			}
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			break;
		case ZEND_FREE:
		case ZEND_CHECK_VAR:
			MAKE_NOP(opline);
			zval_ptr_dtor_nogc(val);
			return 1;
		case ZEND_SEND_VAR_EX:
		case ZEND_SEND_FUNC_ARG:
		case ZEND_FETCH_DIM_W:
		case ZEND_FETCH_DIM_RW:
		case ZEND_FETCH_DIM_FUNC_ARG:
		case ZEND_FETCH_DIM_UNSET:
		case ZEND_FETCH_LIST_W:
		case ZEND_ASSIGN_DIM:
		case ZEND_RETURN_BY_REF:
		case ZEND_INSTANCEOF:
		case ZEND_MAKE_REF:
			return 0;
		case ZEND_CATCH:
			REQUIRES_STRING(val);
			drop_leading_backslash(val);
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			opline->extended_value = alloc_cache_slots(op_array, 1) | (opline->extended_value & ZEND_LAST_CATCH);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			break;
		case ZEND_DEFINED:
			REQUIRES_STRING(val);
			drop_leading_backslash(val);
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			opline->extended_value = alloc_cache_slots(op_array, 1);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			break;
		case ZEND_NEW:
			REQUIRES_STRING(val);
			drop_leading_backslash(val);
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			opline->op2.num = alloc_cache_slots(op_array, 1);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			break;
		case ZEND_INIT_STATIC_METHOD_CALL:
			REQUIRES_STRING(val);
			drop_leading_backslash(val);
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			if (opline->op2_type != IS_CONST) {
				opline->result.num = alloc_cache_slots(op_array, 1);
			}
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			break;
		case ZEND_FETCH_CLASS_CONSTANT:
			REQUIRES_STRING(val);
			drop_leading_backslash(val);
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			if (opline->op2_type != IS_CONST) {
				opline->extended_value = alloc_cache_slots(op_array, 1);
			}
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			break;
		case ZEND_ASSIGN_OP:
		case ZEND_ASSIGN_DIM_OP:
		case ZEND_ASSIGN_OBJ_OP:
			break;
		case ZEND_ASSIGN_STATIC_PROP_OP:
		case ZEND_ASSIGN_STATIC_PROP:
		case ZEND_ASSIGN_STATIC_PROP_REF:
		case ZEND_FETCH_STATIC_PROP_R:
		case ZEND_FETCH_STATIC_PROP_W:
		case ZEND_FETCH_STATIC_PROP_RW:
		case ZEND_FETCH_STATIC_PROP_IS:
		case ZEND_FETCH_STATIC_PROP_UNSET:
		case ZEND_FETCH_STATIC_PROP_FUNC_ARG:
		case ZEND_UNSET_STATIC_PROP:
		case ZEND_ISSET_ISEMPTY_STATIC_PROP:
		case ZEND_PRE_INC_STATIC_PROP:
		case ZEND_PRE_DEC_STATIC_PROP:
		case ZEND_POST_INC_STATIC_PROP:
		case ZEND_POST_DEC_STATIC_PROP:
			TO_STRING_NOWARN(val);
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			if (opline->op2_type == IS_CONST && (opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*) == op_array->cache_size) {
				op_array->cache_size += sizeof(void *);
			} else {
				opline->extended_value = alloc_cache_slots(op_array, 3) | (opline->extended_value & ZEND_FETCH_OBJ_FLAGS);
			}
			break;
		case ZEND_SEND_VAR:
			opline->opcode = ZEND_SEND_VAL;
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			break;
		case ZEND_SEPARATE:
		case ZEND_SEND_VAR_NO_REF:
		case ZEND_SEND_VAR_NO_REF_EX:
			return 0;
		case ZEND_VERIFY_RETURN_TYPE:
			/* This would require a non-local change.
			 * zend_optimizer_replace_by_const() supports this. */
			return 0;
		case ZEND_CASE:
		case ZEND_FETCH_LIST_R:
		case ZEND_COPY_TMP:
			return 0;
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
		case ZEND_FETCH_R:
		case ZEND_FETCH_W:
		case ZEND_FETCH_RW:
		case ZEND_FETCH_IS:
		case ZEND_FETCH_UNSET:
		case ZEND_FETCH_FUNC_ARG:
			TO_STRING_NOWARN(val);
			if (opline->opcode == ZEND_CONCAT && opline->op2_type == IS_CONST) {
				opline->opcode = ZEND_FAST_CONCAT;
			}
			/* break missing intentionally */
		default:
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			break;
	}

	opline->op1_type = IS_CONST;
	if (Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_STRING) {
		zend_string_hash_val(Z_STR(ZEND_OP1_LITERAL(opline)));
	}
	return 1;
}

int zend_optimizer_update_op2_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zval          *val)
{
	zval tmp;

	switch (opline->opcode) {
		case ZEND_ASSIGN_REF:
		case ZEND_FAST_CALL:
			return 0;
		case ZEND_FETCH_CLASS:
			if ((opline + 1)->opcode == ZEND_INSTANCEOF &&
				(opline + 1)->op2.var == opline->result.var) {
				return 0;
			}
			/* break missing intentionally */
		case ZEND_INSTANCEOF:
			REQUIRES_STRING(val);
			drop_leading_backslash(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			opline->extended_value = alloc_cache_slots(op_array, 1);
			break;
		case ZEND_INIT_FCALL_BY_NAME:
			REQUIRES_STRING(val);
			drop_leading_backslash(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			opline->result.num = alloc_cache_slots(op_array, 1);
			break;
		case ZEND_ASSIGN_STATIC_PROP:
		case ZEND_ASSIGN_STATIC_PROP_REF:
		case ZEND_FETCH_STATIC_PROP_R:
		case ZEND_FETCH_STATIC_PROP_W:
		case ZEND_FETCH_STATIC_PROP_RW:
		case ZEND_FETCH_STATIC_PROP_IS:
		case ZEND_FETCH_STATIC_PROP_UNSET:
		case ZEND_FETCH_STATIC_PROP_FUNC_ARG:
		case ZEND_UNSET_STATIC_PROP:
		case ZEND_PRE_INC_STATIC_PROP:
		case ZEND_PRE_DEC_STATIC_PROP:
		case ZEND_POST_INC_STATIC_PROP:
		case ZEND_POST_DEC_STATIC_PROP:
		case ZEND_ASSIGN_STATIC_PROP_OP:
			REQUIRES_STRING(val);
			drop_leading_backslash(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			if (opline->op1_type != IS_CONST) {
				opline->extended_value = alloc_cache_slots(op_array, 1) | (opline->extended_value & (ZEND_RETURNS_FUNCTION|ZEND_ISEMPTY|ZEND_FETCH_OBJ_FLAGS));
			}
			break;
		case ZEND_INIT_FCALL:
			REQUIRES_STRING(val);
			if (Z_REFCOUNT_P(val) == 1) {
				zend_str_tolower(Z_STRVAL_P(val), Z_STRLEN_P(val));
			} else {
				ZVAL_STR(&tmp, zend_string_tolower(Z_STR_P(val)));
				zval_ptr_dtor_nogc(val);
				val = &tmp;
			}
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			opline->result.num = alloc_cache_slots(op_array, 1);
			break;
		case ZEND_INIT_DYNAMIC_CALL:
			if (Z_TYPE_P(val) == IS_STRING) {
				if (zend_memrchr(Z_STRVAL_P(val), ':', Z_STRLEN_P(val))) {
					return 0;
				}

				if (zend_optimizer_classify_function(Z_STR_P(val), opline->extended_value)) {
					/* Dynamic call to various special functions must stay dynamic,
					 * otherwise would drop a warning */
					return 0;
				}

				opline->opcode = ZEND_INIT_FCALL_BY_NAME;
				drop_leading_backslash(val);
				opline->op2.constant = zend_optimizer_add_literal(op_array, val);
				zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
				opline->result.num = alloc_cache_slots(op_array, 1);
			} else {
				opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			}
			break;
		case ZEND_INIT_METHOD_CALL:
			REQUIRES_STRING(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			opline->result.num = alloc_cache_slots(op_array, 2);
			break;
		case ZEND_INIT_STATIC_METHOD_CALL:
			REQUIRES_STRING(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			zend_optimizer_add_literal_string(op_array, zend_string_tolower(Z_STR_P(val)));
			if (opline->op1_type != IS_CONST) {
				opline->result.num = alloc_cache_slots(op_array, 2);
			}
			break;
		case ZEND_ASSIGN_OBJ:
		case ZEND_ASSIGN_OBJ_REF:
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
		case ZEND_ASSIGN_OBJ_OP:
			TO_STRING_NOWARN(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			opline->extended_value = alloc_cache_slots(op_array, 3);
			break;
		case ZEND_ISSET_ISEMPTY_PROP_OBJ:
			TO_STRING_NOWARN(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			opline->extended_value = alloc_cache_slots(op_array, 3) | (opline->extended_value & ZEND_ISEMPTY);
			break;
		case ZEND_ASSIGN_DIM_OP:
		case ZEND_ISSET_ISEMPTY_DIM_OBJ:
		case ZEND_ASSIGN_DIM:
		case ZEND_UNSET_DIM:
		case ZEND_FETCH_DIM_R:
		case ZEND_FETCH_DIM_W:
		case ZEND_FETCH_DIM_RW:
		case ZEND_FETCH_DIM_IS:
		case ZEND_FETCH_DIM_FUNC_ARG:
		case ZEND_FETCH_DIM_UNSET:
		case ZEND_FETCH_LIST_R:
		case ZEND_FETCH_LIST_W:
			if (Z_TYPE_P(val) == IS_STRING) {
				zend_ulong index;

				if (ZEND_HANDLE_NUMERIC(Z_STR_P(val), index)) {
					ZVAL_LONG(&tmp, index);
					opline->op2.constant = zend_optimizer_add_literal(op_array, &tmp);
					zend_string_hash_val(Z_STR_P(val));
					zend_optimizer_add_literal(op_array, val);
					Z_EXTRA(op_array->literals[opline->op2.constant]) = ZEND_EXTRA_VALUE;
					break;
				}
			}
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			break;
		case ZEND_ADD_ARRAY_ELEMENT:
		case ZEND_INIT_ARRAY:
			if (Z_TYPE_P(val) == IS_STRING) {
				zend_ulong index;
				if (ZEND_HANDLE_NUMERIC(Z_STR_P(val), index)) {
					zval_ptr_dtor_nogc(val);
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
			if (opline->opcode == ZEND_CONCAT && opline->op1_type == IS_CONST) {
				opline->opcode = ZEND_FAST_CONCAT;
			}
			/* break missing intentionally */
		default:
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			break;
	}

	opline->op2_type = IS_CONST;
	if (Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING) {
		zend_string_hash_val(Z_STR(ZEND_OP2_LITERAL(opline)));
	}
	return 1;
}

int zend_optimizer_replace_by_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zend_uchar     type,
                                    uint32_t       var,
                                    zval          *val)
{
	zend_op *end = op_array->opcodes + op_array->last;

	while (opline < end) {
		if (opline->op1_type == type &&
			opline->op1.var == var) {
			switch (opline->opcode) {
				case ZEND_FETCH_DIM_W:
				case ZEND_FETCH_DIM_RW:
				case ZEND_FETCH_DIM_FUNC_ARG:
				case ZEND_FETCH_DIM_UNSET:
				case ZEND_FETCH_LIST_W:
				case ZEND_ASSIGN_DIM:
				case ZEND_SEPARATE:
				case ZEND_RETURN_BY_REF:
					return 0;
				case ZEND_SEND_VAR:
					opline->extended_value = 0;
					opline->opcode = ZEND_SEND_VAL;
					break;
				case ZEND_SEND_VAR_EX:
				case ZEND_SEND_FUNC_ARG:
					opline->extended_value = 0;
					opline->opcode = ZEND_SEND_VAL_EX;
					break;
				case ZEND_SEND_VAR_NO_REF:
					return 0;
				case ZEND_SEND_VAR_NO_REF_EX:
					opline->opcode = ZEND_SEND_VAL;
					break;
				case ZEND_SEND_USER:
					opline->opcode = ZEND_SEND_VAL_EX;
					break;
				/* In most cases IS_TMP_VAR operand may be used only once.
				 * The operands are usually destroyed by the opcode handler.
				 * ZEND_CASE and ZEND_FETCH_LIST_R are exceptions, they keeps operand
				 * unchanged, and allows its reuse. these instructions
				 * usually terminated by ZEND_FREE that finally kills the value.
				 */
				case ZEND_FETCH_LIST_R: {
					zend_op *m = opline;

					do {
						if (m->opcode == ZEND_FETCH_LIST_R &&
							m->op1_type == type &&
							m->op1.var == var) {
							zval v;
							ZVAL_COPY(&v, val);
							if (Z_TYPE(v) == IS_STRING) {
								zend_string_hash_val(Z_STR(v));
							}
							m->op1.constant = zend_optimizer_add_literal(op_array, &v);
							m->op1_type = IS_CONST;
						}
						m++;
					} while (m->opcode != ZEND_FREE || m->op1_type != type || m->op1.var != var);

					ZEND_ASSERT(m->opcode == ZEND_FREE && m->op1_type == type && m->op1.var == var);
					MAKE_NOP(m);
					zval_ptr_dtor_nogc(val);
					return 1;
				}
				case ZEND_SWITCH_LONG:
				case ZEND_SWITCH_STRING:
				case ZEND_CASE: {
					zend_op *end = op_array->opcodes + op_array->last;
					while (opline < end) {
						if (opline->op1_type == type && opline->op1.var == var) {
							if (opline->opcode == ZEND_CASE
									|| opline->opcode == ZEND_SWITCH_LONG
									|| opline->opcode == ZEND_SWITCH_STRING) {
								zval v;

								if (opline->opcode == ZEND_CASE) {
									opline->opcode = ZEND_IS_EQUAL;
								}
								ZVAL_COPY(&v, val);
								if (Z_TYPE(v) == IS_STRING) {
									zend_string_hash_val(Z_STR(v));
								}
								opline->op1.constant = zend_optimizer_add_literal(op_array, &v);
								opline->op1_type = IS_CONST;
							} else if (opline->opcode == ZEND_FREE) {
								if (opline->extended_value == ZEND_FREE_SWITCH) {
									/* We found the end of the switch. */
									MAKE_NOP(opline);
									break;
								}

								ZEND_ASSERT(opline->extended_value == ZEND_FREE_ON_RETURN);
								MAKE_NOP(opline);
							} else {
								ZEND_ASSERT(0);
							}
						}
						opline++;
					}
					zval_ptr_dtor_nogc(val);
					return 1;
				}
				case ZEND_VERIFY_RETURN_TYPE: {
					zend_arg_info *ret_info = op_array->arg_info - 1;
					if (ZEND_TYPE_IS_CLASS(ret_info->type)
						|| ZEND_TYPE_CODE(ret_info->type) == IS_CALLABLE
						|| !ZEND_SAME_FAKE_TYPE(ZEND_TYPE_CODE(ret_info->type), Z_TYPE_P(val))
						|| (op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
						return 0;
					}
					MAKE_NOP(opline);

					/* zend_handle_loops_and_finally may inserts other oplines */
					do {
						++opline;
					} while (opline->opcode != ZEND_RETURN && opline->opcode != ZEND_RETURN_BY_REF);
					ZEND_ASSERT(opline->op1.var == var);

					break;
				  }
				default:
					break;
			}
			return zend_optimizer_update_op1_const(op_array, opline, val);
		}

		if (opline->op2_type == type &&
			opline->op2.var == var) {
			return zend_optimizer_update_op2_const(op_array, opline, val);
		}
		opline++;
	}

	return 1;
}

/* Update jump offsets after a jump was migrated to another opline */
void zend_optimizer_migrate_jump(zend_op_array *op_array, zend_op *new_opline, zend_op *opline) {
	switch (new_opline->opcode) {
		case ZEND_JMP:
		case ZEND_FAST_CALL:
			ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op1, ZEND_OP1_JMP_ADDR(opline));
			break;
		case ZEND_JMPZNZ:
			new_opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, new_opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
			/* break missing intentionally */
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
			ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op2, ZEND_OP2_JMP_ADDR(opline));
			break;
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
			new_opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, new_opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
			break;
		case ZEND_CATCH:
			if (!(opline->extended_value & ZEND_LAST_CATCH)) {
				ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op2, ZEND_OP2_JMP_ADDR(opline));
			}
			break;
		case ZEND_SWITCH_LONG:
		case ZEND_SWITCH_STRING:
		{
			HashTable *jumptable = Z_ARRVAL(ZEND_OP2_LITERAL(opline));
			zval *zv;
			ZEND_HASH_FOREACH_VAL(jumptable, zv) {
				Z_LVAL_P(zv) = ZEND_OPLINE_NUM_TO_OFFSET(op_array, new_opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, Z_LVAL_P(zv)));
			} ZEND_HASH_FOREACH_END();
			new_opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, new_opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
			break;
		}
	}
}

/* Shift jump offsets based on shiftlist */
void zend_optimizer_shift_jump(zend_op_array *op_array, zend_op *opline, uint32_t *shiftlist) {
	switch (opline->opcode) {
		case ZEND_JMP:
		case ZEND_FAST_CALL:
			ZEND_SET_OP_JMP_ADDR(opline, opline->op1, ZEND_OP1_JMP_ADDR(opline) - shiftlist[ZEND_OP1_JMP_ADDR(opline) - op_array->opcodes]);
			break;
		case ZEND_JMPZNZ:
			opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value) - shiftlist[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)]);
			/* break missing intentionally */
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
			ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP2_JMP_ADDR(opline) - shiftlist[ZEND_OP2_JMP_ADDR(opline) - op_array->opcodes]);
			break;
		case ZEND_CATCH:
			if (!(opline->extended_value & ZEND_LAST_CATCH)) {
				ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP2_JMP_ADDR(opline) - shiftlist[ZEND_OP2_JMP_ADDR(opline) - op_array->opcodes]);
			}
			break;
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
			opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value) - shiftlist[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)]);
			break;
		case ZEND_SWITCH_LONG:
		case ZEND_SWITCH_STRING:
		{
			HashTable *jumptable = Z_ARRVAL(ZEND_OP2_LITERAL(opline));
			zval *zv;
			ZEND_HASH_FOREACH_VAL(jumptable, zv) {
				Z_LVAL_P(zv) = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, Z_LVAL_P(zv)) - shiftlist[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, Z_LVAL_P(zv))]);
			} ZEND_HASH_FOREACH_END();
			opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value) - shiftlist[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)]);
			break;
		}
	}
}

static zend_class_entry *get_class_entry_from_op1(
		zend_script *script, zend_op_array *op_array, zend_op *opline, zend_bool rt_constants) {
	if (opline->op1_type == IS_CONST) {
		zval *op1 = CRT_CONSTANT_EX(op_array, opline, opline->op1, rt_constants);
		if (Z_TYPE_P(op1) == IS_STRING) {
			zend_string *class_name = Z_STR_P(op1 + 1);
			zend_class_entry *ce;
			if (script && (ce = zend_hash_find_ptr(&script->class_table, class_name))) {
				return ce;
			} else if ((ce = zend_hash_find_ptr(EG(class_table), class_name))) {
				if (ce->type == ZEND_INTERNAL_CLASS) {
					return ce;
				} else if (ce->type == ZEND_USER_CLASS &&
						   ce->info.user.filename &&
						   ce->info.user.filename == op_array->filename) {
					return ce;
				}
			}
		}
	} else if (opline->op1_type == IS_UNUSED && op_array->scope
			&& !(op_array->scope->ce_flags & ZEND_ACC_TRAIT)
			&& (opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_SELF) {
		return op_array->scope;
	}
	return NULL;
}

zend_function *zend_optimizer_get_called_func(
		zend_script *script, zend_op_array *op_array, zend_op *opline, zend_bool rt_constants)
{
#define GET_OP(op) CRT_CONSTANT_EX(op_array, opline, opline->op, rt_constants)
	switch (opline->opcode) {
		case ZEND_INIT_FCALL:
		{
			zend_string *function_name = Z_STR_P(GET_OP(op2));
			zend_function *func;
			if (script && (func = zend_hash_find_ptr(&script->function_table, function_name)) != NULL) {
				return func;
			} else if ((func = zend_hash_find_ptr(EG(function_table), function_name)) != NULL) {
				if (func->type == ZEND_INTERNAL_FUNCTION) {
					return func;
				} else if (func->type == ZEND_USER_FUNCTION &&
				           func->op_array.filename &&
				           func->op_array.filename == op_array->filename) {
					return func;
				}
			}
			break;
		}
		case ZEND_INIT_FCALL_BY_NAME:
		case ZEND_INIT_NS_FCALL_BY_NAME:
			if (opline->op2_type == IS_CONST && Z_TYPE_P(GET_OP(op2)) == IS_STRING) {
				zval *function_name = GET_OP(op2) + 1;
				zend_function *func;
				if (script && (func = zend_hash_find_ptr(&script->function_table, Z_STR_P(function_name)))) {
					return func;
				} else if ((func = zend_hash_find_ptr(EG(function_table), Z_STR_P(function_name))) != NULL) {
					if (func->type == ZEND_INTERNAL_FUNCTION) {
						return func;
					} else if (func->type == ZEND_USER_FUNCTION &&
					           func->op_array.filename &&
					           func->op_array.filename == op_array->filename) {
						return func;
					}
				}
			}
			break;
		case ZEND_INIT_STATIC_METHOD_CALL:
			if (opline->op2_type == IS_CONST && Z_TYPE_P(GET_OP(op2)) == IS_STRING) {
				zend_class_entry *ce = get_class_entry_from_op1(
					script, op_array, opline, rt_constants);
				if (ce) {
					zend_string *func_name = Z_STR_P(GET_OP(op2) + 1);
					zend_function *fbc = zend_hash_find_ptr(&ce->function_table, func_name);
					if (fbc) {
						zend_bool is_public = (fbc->common.fn_flags & ZEND_ACC_PUBLIC) != 0;
						zend_bool same_scope = fbc->common.scope == op_array->scope;
						if (is_public|| same_scope) {
							return fbc;
						}
					}
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
		case ZEND_NEW:
		{
			zend_class_entry *ce = get_class_entry_from_op1(
				script, op_array, opline, rt_constants);
			if (ce && ce->type == ZEND_USER_CLASS) {
				return ce->constructor;
			}
			break;
		}
	}
	return NULL;
#undef GET_OP
}

uint32_t zend_optimizer_classify_function(zend_string *name, uint32_t num_args) {
	if (zend_string_equals_literal(name, "extract")) {
		return ZEND_FUNC_INDIRECT_VAR_ACCESS;
	} else if (zend_string_equals_literal(name, "compact")) {
		return ZEND_FUNC_INDIRECT_VAR_ACCESS;
	} else if (zend_string_equals_literal(name, "parse_str") && num_args <= 1) {
		return ZEND_FUNC_INDIRECT_VAR_ACCESS;
	} else if (zend_string_equals_literal(name, "mb_parse_str") && num_args <= 1) {
		return ZEND_FUNC_INDIRECT_VAR_ACCESS;
	} else if (zend_string_equals_literal(name, "get_defined_vars")) {
		return ZEND_FUNC_INDIRECT_VAR_ACCESS;
	} else if (zend_string_equals_literal(name, "assert")) {
		return ZEND_FUNC_INDIRECT_VAR_ACCESS;
	} else if (zend_string_equals_literal(name, "db2_execute")) {
		return ZEND_FUNC_INDIRECT_VAR_ACCESS;
	} else if (zend_string_equals_literal(name, "func_num_args")) {
		return ZEND_FUNC_VARARG;
	} else if (zend_string_equals_literal(name, "func_get_arg")) {
		return ZEND_FUNC_VARARG;
	} else if (zend_string_equals_literal(name, "func_get_args")) {
		return ZEND_FUNC_VARARG;
	} else {
		return 0;
	}
}

zend_op *zend_optimizer_get_loop_var_def(const zend_op_array *op_array, zend_op *free_opline) {
	uint32_t var = free_opline->op1.var;
	ZEND_ASSERT(zend_optimizer_is_loop_var_free(free_opline));

	while (--free_opline >= op_array->opcodes) {
		if ((free_opline->result_type & (IS_TMP_VAR|IS_VAR)) && free_opline->result.var == var) {
			return free_opline;
		}
	}
	return NULL;
}

static void zend_optimize(zend_op_array      *op_array,
                          zend_optimizer_ctx *ctx)
{
	if (op_array->type == ZEND_EVAL_CODE) {
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_BEFORE_OPTIMIZER) {
		zend_dump_op_array(op_array, ZEND_DUMP_LIVE_RANGES, "before optimizer", NULL);
	}

	/* pass 1
	 * - substitute persistent constants (true, false, null, etc)
	 * - perform compile-time evaluation of constant binary and unary operations
	 * - optimize series of ADD_STRING and/or ADD_CHAR
	 * - convert CAST(IS_BOOL,x) into BOOL(x)
         * - pre-evaluate constant function calls
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
		zend_optimizer_pass3(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_3) {
			zend_dump_op_array(op_array, 0, "after pass 3", NULL);
		}
	}

	/* pass 4:
	 * - INIT_FCALL_BY_NAME -> DO_FCALL
	 */
	if (ZEND_OPTIMIZER_PASS_4 & ctx->optimization_level) {
		zend_optimize_func_calls(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_4) {
			zend_dump_op_array(op_array, 0, "after pass 4", NULL);
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
		zend_optimizer_nop_removal(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_10) {
			zend_dump_op_array(op_array, 0, "after pass 10", NULL);
		}
	}

	/* pass 11:
	 * - Compact literals table
	 */
	if ((ZEND_OPTIMIZER_PASS_11 & ctx->optimization_level) &&
	    (!(ZEND_OPTIMIZER_PASS_6 & ctx->optimization_level) ||
	     !(ZEND_OPTIMIZER_PASS_7 & ctx->optimization_level))) {
		zend_optimizer_compact_literals(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_11) {
			zend_dump_op_array(op_array, 0, "after pass 11", NULL);
		}
	}

	if ((ZEND_OPTIMIZER_PASS_13 & ctx->optimization_level) &&
	    (!(ZEND_OPTIMIZER_PASS_6 & ctx->optimization_level) ||
	     !(ZEND_OPTIMIZER_PASS_7 & ctx->optimization_level))) {
		zend_optimizer_compact_vars(op_array);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_13) {
			zend_dump_op_array(op_array, 0, "after pass 13", NULL);
		}
	}

	if (ZEND_OPTIMIZER_PASS_7 & ctx->optimization_level) {
		return;
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
			ZEND_PASS_TWO_UNDO_CONSTANT(op_array, opline, opline->op1);
		}
		if (opline->op2_type == IS_CONST) {
			ZEND_PASS_TWO_UNDO_CONSTANT(op_array, opline, opline->op2);
		}
		opline++;
	}
#if !ZEND_USE_ABS_CONST_ADDR
	if (op_array->literals) {
		zval *literals = emalloc(sizeof(zval) * op_array->last_literal);
		memcpy(literals, op_array->literals, sizeof(zval) * op_array->last_literal);
		op_array->literals = literals;
	}
#endif
}

static void zend_redo_pass_two(zend_op_array *op_array)
{
	zend_op *opline, *end;
#if ZEND_USE_ABS_JMP_ADDR && !ZEND_USE_ABS_CONST_ADDR
	zend_op *old_opcodes = op_array->opcodes;
#endif

#if !ZEND_USE_ABS_CONST_ADDR
	if (op_array->last_literal) {
		op_array->opcodes = (zend_op *) erealloc(op_array->opcodes,
			ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_op) * op_array->last, 16) +
			sizeof(zval) * op_array->last_literal);
		memcpy(((char*)op_array->opcodes) + ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_op) * op_array->last, 16),
			op_array->literals, sizeof(zval) * op_array->last_literal);
		efree(op_array->literals);
		op_array->literals = (zval*)(((char*)op_array->opcodes) + ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_op) * op_array->last, 16));
	} else {
		if (op_array->literals) {
			efree(op_array->literals);
		}
		op_array->literals = NULL;
	}
#endif

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		if (opline->op1_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, opline->op1);
		}
		if (opline->op2_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, opline->op2);
		}
#if ZEND_USE_ABS_JMP_ADDR && !ZEND_USE_ABS_CONST_ADDR
		if (op_array->fn_flags & ZEND_ACC_DONE_PASS_TWO) {
			/* fix jumps to point to new array */
			switch (opline->opcode) {
				case ZEND_JMP:
				case ZEND_FAST_CALL:
					opline->op1.jmp_addr = &op_array->opcodes[opline->op1.jmp_addr - old_opcodes];
					break;
				case ZEND_JMPZNZ:
					/* relative extended_value don't have to be changed */
					/* break omitted intentionally */
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_ASSERT_CHECK:
					opline->op2.jmp_addr = &op_array->opcodes[opline->op2.jmp_addr - old_opcodes];
					break;
				case ZEND_CATCH:
					if (!(opline->extended_value & ZEND_LAST_CATCH)) {
						opline->op2.jmp_addr = &op_array->opcodes[opline->op2.jmp_addr - old_opcodes];
					}
					break;
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
				case ZEND_SWITCH_LONG:
				case ZEND_SWITCH_STRING:
					/* relative extended_value don't have to be changed */
					break;
			}
		}
#endif
		ZEND_VM_SET_OPCODE_HANDLER(opline);
		opline++;
	}
}

#if HAVE_DFA_PASS
static void zend_redo_pass_two_ex(zend_op_array *op_array, zend_ssa *ssa)
{
	zend_op *opline, *end;
#if ZEND_USE_ABS_JMP_ADDR && !ZEND_USE_ABS_CONST_ADDR
	zend_op *old_opcodes = op_array->opcodes;
#endif

#if !ZEND_USE_ABS_CONST_ADDR
	if (op_array->last_literal) {
		op_array->opcodes = (zend_op *) erealloc(op_array->opcodes,
			ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_op) * op_array->last, 16) +
			sizeof(zval) * op_array->last_literal);
		memcpy(((char*)op_array->opcodes) + ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_op) * op_array->last, 16),
			op_array->literals, sizeof(zval) * op_array->last_literal);
		efree(op_array->literals);
		op_array->literals = (zval*)(((char*)op_array->opcodes) + ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_op) * op_array->last, 16));
	} else {
		if (op_array->literals) {
			efree(op_array->literals);
		}
		op_array->literals = NULL;
	}
#endif

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		uint32_t op1_info = opline->op1_type == IS_UNUSED ? 0 : (OP1_INFO() & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_KEY_ANY));
		uint32_t op2_info = opline->op1_type == IS_UNUSED ? 0 : (OP2_INFO() & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_KEY_ANY));
		uint32_t res_info =
			(opline->opcode == ZEND_PRE_INC ||
			 opline->opcode == ZEND_PRE_DEC ||
			 opline->opcode == ZEND_POST_INC ||
			 opline->opcode == ZEND_POST_DEC) ?
				((ssa->ops[opline - op_array->opcodes].op1_def >= 0) ? (OP1_DEF_INFO() & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_KEY_ANY)) : MAY_BE_ANY) :
				(opline->result_type == IS_UNUSED ? 0 : (RES_INFO() & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_KEY_ANY)));

		if (opline->op1_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, opline->op1);
		}
		if (opline->op2_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, opline->op2);
		}

		zend_vm_set_opcode_handler_ex(opline, op1_info, op2_info, res_info);
#if ZEND_USE_ABS_JMP_ADDR && !ZEND_USE_ABS_CONST_ADDR
		if (op_array->fn_flags & ZEND_ACC_DONE_PASS_TWO) {
			/* fix jumps to point to new array */
			switch (opline->opcode) {
				case ZEND_JMP:
				case ZEND_FAST_CALL:
					opline->op1.jmp_addr = &op_array->opcodes[opline->op1.jmp_addr - old_opcodes];
					break;
				case ZEND_JMPZNZ:
					/* relative extended_value don't have to be changed */
					/* break omitted intentionally */
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_ASSERT_CHECK:
					opline->op2.jmp_addr = &op_array->opcodes[opline->op2.jmp_addr - old_opcodes];
					break;
				case ZEND_CATCH:
					if (!(opline->extended_value & ZEND_LAST_CATCH)) {
						opline->op2.jmp_addr = &op_array->opcodes[opline->op2.jmp_addr - old_opcodes];
					}
					break;
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
				case ZEND_SWITCH_LONG:
				case ZEND_SWITCH_STRING:
					/* relative extended_value don't have to be changed */
					break;
			}
		}
#endif
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

	if (op_array->live_range) {
#if HAVE_DFA_PASS
		if ((ZEND_OPTIMIZER_PASS_6 & ctx->optimization_level) &&
		    (ZEND_OPTIMIZER_PASS_7 & ctx->optimization_level)) {
			return;
		}
#endif
		zend_recalc_live_ranges(op_array, NULL);
	}
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
				Z_STR_P(RT_CONSTANT(opline, opline->op2)));
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

static zend_bool needs_live_range(zend_op_array *op_array, zend_op *def_opline) {
	zend_func_info *func_info = ZEND_FUNC_INFO(op_array);
	zend_ssa_op *ssa_op = &func_info->ssa.ops[def_opline - op_array->opcodes];
	int ssa_var = ssa_op->result_def;
	if (ssa_var < 0) {
		/* Be conservative. */
		return 1;
	}

	/* If the variable is used by a PHI, this may be the assignment of the final branch of a
	 * ternary/etc structure. While this is where the live range starts, the value from the other
	 * branch may also be used. As such, use the type of the PHI node for the following check. */
	if (func_info->ssa.vars[ssa_var].phi_use_chain) {
		ssa_var = func_info->ssa.vars[ssa_var].phi_use_chain->ssa_var;
	}

	uint32_t type = func_info->ssa.var_info[ssa_var].type;
	return (type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) != 0;
}
#endif

int zend_optimize_script(zend_script *script, zend_long optimization_level, zend_long debug_level)
{
	zend_class_entry *ce;
	zend_string *key;
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

	ZEND_HASH_FOREACH_STR_KEY_PTR(&script->class_table, key, ce) {
		if (ce->refcount > 1 && !zend_string_equals_ci(key, ce->name)) {
			continue;
		}
		ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->function_table, name, op_array) {
			if (op_array->scope == ce
			 && op_array->type == ZEND_USER_FUNCTION
			 && !(op_array->fn_flags & ZEND_ACC_TRAIT_CLONE)) {
				zend_optimize_op_array(op_array, &ctx);
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
			func_info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (func_info) {
				func_info->call_map = zend_build_call_map(&ctx.arena, func_info, call_graph.op_arrays[i]);
				if (call_graph.op_arrays[i]->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
					zend_init_func_return_info(call_graph.op_arrays[i], script, &func_info->return_info);
				}
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			func_info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (func_info) {
				if (zend_dfa_analyze_op_array(call_graph.op_arrays[i], &ctx, &func_info->ssa) == SUCCESS) {
					func_info->flags = func_info->ssa.cfg.flags;
				} else {
					ZEND_SET_FUNC_INFO(call_graph.op_arrays[i], NULL);
				}
			}
		}

		//TODO: perform inner-script inference???
		for (i = 0; i < call_graph.op_arrays_count; i++) {
			func_info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (func_info) {
				zend_dfa_optimize_op_array(call_graph.op_arrays[i], &ctx, &func_info->ssa, func_info->call_map);
			}
		}

		if (debug_level & ZEND_DUMP_AFTER_PASS_7) {
			for (i = 0; i < call_graph.op_arrays_count; i++) {
				zend_dump_op_array(call_graph.op_arrays[i], 0, "after pass 7", NULL);
			}
		}

		if (ZEND_OPTIMIZER_PASS_11 & optimization_level) {
			for (i = 0; i < call_graph.op_arrays_count; i++) {
				zend_optimizer_compact_literals(call_graph.op_arrays[i], &ctx);
				if (debug_level & ZEND_DUMP_AFTER_PASS_11) {
					zend_dump_op_array(call_graph.op_arrays[i], 0, "after pass 11", NULL);
				}
			}
		}

		if (ZEND_OPTIMIZER_PASS_13 & optimization_level) {
			for (i = 0; i < call_graph.op_arrays_count; i++) {
				zend_optimizer_compact_vars(call_graph.op_arrays[i]);
				if (debug_level & ZEND_DUMP_AFTER_PASS_13) {
					zend_dump_op_array(call_graph.op_arrays[i], 0, "after pass 13", NULL);
				}
			}
		}

		if (ZEND_OPTIMIZER_PASS_12 & optimization_level) {
			for (i = 0; i < call_graph.op_arrays_count; i++) {
				zend_adjust_fcall_stack_size_graph(call_graph.op_arrays[i]);
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			op_array = call_graph.op_arrays[i];
			func_info = ZEND_FUNC_INFO(op_array);
			if (func_info && func_info->ssa.var_info) {
				zend_redo_pass_two_ex(op_array, &func_info->ssa);
				if (op_array->live_range) {
					zend_recalc_live_ranges(op_array, needs_live_range);
				}
			} else {
				zend_redo_pass_two(op_array);
				if (op_array->live_range) {
					zend_recalc_live_ranges(op_array, NULL);
				}
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			ZEND_SET_FUNC_INFO(call_graph.op_arrays[i], NULL);
		}

		zend_arena_release(&ctx.arena, checkpoint);
	} else
#endif

	if (ZEND_OPTIMIZER_PASS_12 & optimization_level) {
		zend_adjust_fcall_stack_size(&script->main_op_array, &ctx);

		ZEND_HASH_FOREACH_PTR(&script->function_table, op_array) {
			zend_adjust_fcall_stack_size(op_array, &ctx);
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_FOREACH_STR_KEY_PTR(&script->class_table, key, ce) {
			if (ce->refcount > 1 && !zend_string_equals_ci(key, ce->name)) {
				continue;
			}
			ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->function_table, name, op_array) {
				if (op_array->scope == ce
				 && op_array->type == ZEND_USER_FUNCTION
				 && !(op_array->fn_flags & ZEND_ACC_TRAIT_CLONE)) {
					zend_adjust_fcall_stack_size(op_array, &ctx);
				}
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FOREACH_END();
	}

	ZEND_HASH_FOREACH_STR_KEY_PTR(&script->class_table, key, ce) {
		if (ce->refcount > 1 && !zend_string_equals_ci(key, ce->name)) {
			continue;
		}
		ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->function_table, name, op_array) {
			if (op_array->scope != ce && op_array->type == ZEND_USER_FUNCTION) {
				zend_op_array *orig_op_array =
					zend_hash_find_ptr(&op_array->scope->function_table, name);

				ZEND_ASSERT(orig_op_array != NULL);
				if (orig_op_array != op_array) {
					uint32_t fn_flags = op_array->fn_flags;
					zend_function *prototype = op_array->prototype;
					HashTable *ht = op_array->static_variables;

					*op_array = *orig_op_array;
					op_array->fn_flags = fn_flags;
					op_array->prototype = prototype;
					op_array->static_variables = ht;
				}
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();

	if ((debug_level & ZEND_DUMP_AFTER_OPTIMIZER) &&
	    (ZEND_OPTIMIZER_PASS_7 & optimization_level)) {
		zend_dump_op_array(&script->main_op_array,
			ZEND_DUMP_RT_CONSTANTS | ZEND_DUMP_LIVE_RANGES, "after optimizer", NULL);

		ZEND_HASH_FOREACH_PTR(&script->function_table, op_array) {
			zend_dump_op_array(op_array,
				ZEND_DUMP_RT_CONSTANTS | ZEND_DUMP_LIVE_RANGES, "after optimizer", NULL);
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_FOREACH_PTR(&script->class_table, ce) {
			ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->function_table, name, op_array) {
				if (op_array->scope == ce
				 && op_array->type == ZEND_USER_FUNCTION
				 && !(op_array->fn_flags & ZEND_ACC_TRAIT_CLONE)) {
					zend_dump_op_array(op_array,
						ZEND_DUMP_RT_CONSTANTS | ZEND_DUMP_LIVE_RANGES, "after optimizer", NULL);
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
