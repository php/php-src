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
#include "php.h"
#include "zend_observer.h"

#ifndef ZEND_OPTIMIZER_MAX_REGISTERED_PASSES
# define ZEND_OPTIMIZER_MAX_REGISTERED_PASSES 32
#endif

struct {
	zend_optimizer_pass_t pass[ZEND_OPTIMIZER_MAX_REGISTERED_PASSES];
	int last;
} zend_optimizer_registered_passes = {{NULL}, 0};

void zend_optimizer_collect_constant(zend_optimizer_ctx *ctx, zval *name, zval* value)
{
	if (!ctx->constants) {
		ctx->constants = zend_arena_alloc(&ctx->arena, sizeof(HashTable));
		zend_hash_init(ctx->constants, 16, NULL, zval_ptr_dtor_nogc, 0);
	}

	if (zend_hash_add(ctx->constants, Z_STR_P(name), value)) {
		Z_TRY_ADDREF_P(value);
	}
}

zend_result zend_optimizer_eval_binary_op(zval *result, uint8_t opcode, zval *op1, zval *op2) /* {{{ */
{
	if (zend_binary_op_produces_error(opcode, op1, op2)) {
		return FAILURE;
	}

	binary_op_type binary_op = get_binary_op(opcode);
	return binary_op(result, op1, op2);
}
/* }}} */

zend_result zend_optimizer_eval_unary_op(zval *result, uint8_t opcode, zval *op1) /* {{{ */
{
	unary_op_type unary_op = get_unary_op(opcode);

	if (unary_op) {
		if (zend_unary_op_produces_error(opcode, op1)) {
			return FAILURE;
		}
		return unary_op(result, op1);
	} else { /* ZEND_BOOL */
		ZVAL_BOOL(result, zend_is_true(op1));
		return SUCCESS;
	}
}
/* }}} */

zend_result zend_optimizer_eval_cast(zval *result, uint32_t type, zval *op1) /* {{{ */
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

zend_result zend_optimizer_eval_strlen(zval *result, const zval *op1) /* {{{ */
{
	if (Z_TYPE_P(op1) != IS_STRING) {
		return FAILURE;
	}
	ZVAL_LONG(result, Z_STRLEN_P(op1));
	return SUCCESS;
}
/* }}} */

zend_result zend_optimizer_eval_special_func_call(
		zval *result, zend_string *name, zend_string *arg) {
	if (zend_string_equals_literal(name, "function_exists") ||
			zend_string_equals_literal(name, "is_callable")) {
		zend_string *lc_name = zend_string_tolower(arg);
		zend_internal_function *func = zend_hash_find_ptr(EG(function_table), lc_name);
		zend_string_release_ex(lc_name, 0);

		if (func && func->type == ZEND_INTERNAL_FUNCTION
				&& func->module->type == MODULE_PERSISTENT
#ifdef ZEND_WIN32
				&& func->module->handle == NULL
#endif
		) {
			ZVAL_TRUE(result);
			return SUCCESS;
		}
		return FAILURE;
	}
	if (zend_string_equals_literal(name, "extension_loaded")) {
		zend_string *lc_name = zend_string_tolower(arg);
		zend_module_entry *m = zend_hash_find_ptr(&module_registry, lc_name);
		zend_string_release_ex(lc_name, 0);

		if (!m) {
			if (PG(enable_dl)) {
				return FAILURE;
			}
			ZVAL_FALSE(result);
			return SUCCESS;
		}

		if (m->type == MODULE_PERSISTENT
#ifdef ZEND_WIN32
			&& m->handle == NULL
#endif
		) {
			ZVAL_TRUE(result);
			return SUCCESS;
		}
		return FAILURE;
	}
	if (zend_string_equals_literal(name, "constant")) {
		return zend_optimizer_get_persistent_constant(arg, result, 1) ? SUCCESS : FAILURE;
	}
	if (zend_string_equals_literal(name, "dirname")) {
		if (!IS_ABSOLUTE_PATH(ZSTR_VAL(arg), ZSTR_LEN(arg))) {
			return FAILURE;
		}

		zend_string *dirname = zend_string_init(ZSTR_VAL(arg), ZSTR_LEN(arg), 0);
		ZSTR_LEN(dirname) = zend_dirname(ZSTR_VAL(dirname), ZSTR_LEN(dirname));
		if (IS_ABSOLUTE_PATH(ZSTR_VAL(dirname), ZSTR_LEN(dirname))) {
			ZVAL_STR(result, dirname);
			return SUCCESS;
		}
		zend_string_release_ex(dirname, 0);
		return FAILURE;
	}
	if (zend_string_equals_literal(name, "ini_get")) {
		zend_ini_entry *ini_entry = zend_hash_find_ptr(EG(ini_directives), arg);
		if (!ini_entry) {
			if (PG(enable_dl)) {
				return FAILURE;
			}
			ZVAL_FALSE(result);
		} else if (ini_entry->modifiable != ZEND_INI_SYSTEM) {
			return FAILURE;
		} else if (ini_entry->value) {
			ZVAL_STR_COPY(result, ini_entry->value);
		} else {
			ZVAL_EMPTY_STRING(result);
		}
		return SUCCESS;
	}
	return FAILURE;
}

bool zend_optimizer_get_collected_constant(HashTable *constants, zval *name, zval* value)
{
	zval *val;

	if ((val = zend_hash_find(constants, Z_STR_P(name))) != NULL) {
		ZVAL_COPY(value, val);
		return 1;
	}
	return 0;
}

void zend_optimizer_convert_to_free_op1(zend_op_array *op_array, zend_op *opline)
{
	if (opline->op1_type == IS_CV) {
		opline->opcode = ZEND_CHECK_VAR;
		SET_UNUSED(opline->op2);
		SET_UNUSED(opline->result);
		opline->extended_value = 0;
	} else if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
		opline->opcode = ZEND_FREE;
		SET_UNUSED(opline->op2);
		SET_UNUSED(opline->result);
		opline->extended_value = 0;
	} else {
		ZEND_ASSERT(opline->op1_type == IS_CONST);
		literal_dtor(&ZEND_OP1_LITERAL(opline));
		MAKE_NOP(opline);
	}
}

int zend_optimizer_add_literal(zend_op_array *op_array, const zval *zv)
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

bool zend_optimizer_update_op1_const(zend_op_array *op_array,
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
		case ZEND_SEPARATE:
		case ZEND_SEND_VAR_NO_REF:
		case ZEND_SEND_VAR_NO_REF_EX:
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
		case ZEND_CASE:
			opline->opcode = ZEND_IS_EQUAL;
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			break;
		case ZEND_CASE_STRICT:
			opline->opcode = ZEND_IS_IDENTICAL;
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			break;
		case ZEND_VERIFY_RETURN_TYPE:
			/* This would require a non-local change.
			 * zend_optimizer_replace_by_const() supports this. */
			return 0;
		case ZEND_COPY_TMP:
		case ZEND_FETCH_CLASS_NAME:
			return 0;
		case ZEND_ECHO:
		{
			zval zv;
			if (Z_TYPE_P(val) != IS_STRING && zend_optimizer_eval_cast(&zv, IS_STRING, val) == SUCCESS) {
				zval_ptr_dtor_nogc(val);
				val = &zv;
			}
			opline->op1.constant = zend_optimizer_add_literal(op_array, val);
			if (Z_TYPE_P(val) == IS_STRING && Z_STRLEN_P(val) == 0) {
				MAKE_NOP(opline);
				return 1;
			}
			/* TODO: In a subsequent pass, *after* this step and compacting nops, combine consecutive ZEND_ECHOs using the block information from ssa->cfg */
			/* (e.g. for ext/opcache/tests/opt/sccp_010.phpt) */
			break;
		}
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
		case ZEND_FETCH_R:
		case ZEND_FETCH_W:
		case ZEND_FETCH_RW:
		case ZEND_FETCH_IS:
		case ZEND_FETCH_UNSET:
		case ZEND_FETCH_FUNC_ARG:
		case ZEND_ISSET_ISEMPTY_VAR:
		case ZEND_UNSET_VAR:
			TO_STRING_NOWARN(val);
			if (opline->opcode == ZEND_CONCAT && opline->op2_type == IS_CONST) {
				opline->opcode = ZEND_FAST_CONCAT;
			}
			ZEND_FALLTHROUGH;
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

bool zend_optimizer_update_op2_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zval          *val)
{
	zval tmp;

	switch (opline->opcode) {
		case ZEND_ASSIGN_REF:
		case ZEND_FAST_CALL:
			return 0;
		case ZEND_FETCH_CLASS:
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
		case ZEND_ISSET_ISEMPTY_STATIC_PROP:
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
			ZEND_ASSERT(opline->op2_type == IS_CONST && Z_TYPE_P(CRT_CONSTANT(opline->op2)) == IS_STRING);
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
			TO_STRING_NOWARN(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			opline->extended_value = alloc_cache_slots(op_array, 3);
			break;
		case ZEND_ASSIGN_OBJ_OP:
			TO_STRING_NOWARN(val);
			opline->op2.constant = zend_optimizer_add_literal(op_array, val);
			ZEND_ASSERT((opline + 1)->opcode == ZEND_OP_DATA);
			(opline + 1)->extended_value = alloc_cache_slots(op_array, 3);
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
			ZEND_FALLTHROUGH;
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

bool zend_optimizer_replace_by_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    uint8_t        type,
                                    uint32_t       var,
                                    zval          *val)
{
	zend_op *end = op_array->opcodes + op_array->last;

	while (opline < end) {
		if (opline->op1_type == type &&
			opline->op1.var == var) {
			switch (opline->opcode) {
				/* In most cases IS_TMP_VAR operand may be used only once.
				 * The operands are usually destroyed by the opcode handler.
				 * However, there are some exception which keep the operand alive. In that case
				 * we want to try to replace all uses of the temporary.
				 */
				case ZEND_FETCH_LIST_R:
				case ZEND_CASE:
				case ZEND_CASE_STRICT:
				case ZEND_SWITCH_LONG:
				case ZEND_SWITCH_STRING:
				case ZEND_MATCH:
				case ZEND_JMP_NULL: {
					zend_op *end = op_array->opcodes + op_array->last;
					while (opline < end) {
						if (opline->op1_type == type && opline->op1.var == var) {
							/* If this opcode doesn't keep the operand alive, we're done. Check
							 * this early, because op replacement may modify the opline. */
							bool is_last = opline->opcode != ZEND_FETCH_LIST_R
								&& opline->opcode != ZEND_CASE
								&& opline->opcode != ZEND_CASE_STRICT
								&& opline->opcode != ZEND_SWITCH_LONG
								&& opline->opcode != ZEND_SWITCH_STRING
								&& opline->opcode != ZEND_MATCH
								&& opline->opcode != ZEND_JMP_NULL
								&& (opline->opcode != ZEND_FREE
									|| opline->extended_value != ZEND_FREE_ON_RETURN);

							Z_TRY_ADDREF_P(val);
							if (!zend_optimizer_update_op1_const(op_array, opline, val)) {
								zval_ptr_dtor(val);
								return 0;
							}
							if (is_last) {
								break;
							}
						}
						opline++;
					}
					zval_ptr_dtor_nogc(val);
					return 1;
				}
				case ZEND_VERIFY_RETURN_TYPE: {
					zend_arg_info *ret_info = op_array->arg_info - 1;
					if (!ZEND_TYPE_CONTAINS_CODE(ret_info->type, Z_TYPE_P(val))
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
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
		case ZEND_JMP_NULL:
		case ZEND_BIND_INIT_STATIC_OR_JMP:
		case ZEND_JMP_FRAMELESS:
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
		case ZEND_MATCH:
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
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
		case ZEND_JMP_NULL:
		case ZEND_BIND_INIT_STATIC_OR_JMP:
		case ZEND_JMP_FRAMELESS:
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
		case ZEND_MATCH:
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

zend_class_entry *zend_optimizer_get_class_entry(
		const zend_script *script, const zend_op_array *op_array, zend_string *lcname) {
	zend_class_entry *ce = script ? zend_hash_find_ptr(&script->class_table, lcname) : NULL;
	if (ce) {
		return ce;
	}

	ce = zend_hash_find_ptr(CG(class_table), lcname);
	if (ce
	 && (ce->type == ZEND_INTERNAL_CLASS
	  || (op_array && ce->info.user.filename == op_array->filename))) {
		return ce;
	}

	if (op_array && op_array->scope && zend_string_equals_ci(op_array->scope->name, lcname)) {
		return op_array->scope;
	}

	return NULL;
}

zend_class_entry *zend_optimizer_get_class_entry_from_op1(
		const zend_script *script, const zend_op_array *op_array, const zend_op *opline) {
	if (opline->op1_type == IS_CONST) {
		zval *op1 = CRT_CONSTANT(opline->op1);
		if (Z_TYPE_P(op1) == IS_STRING) {
			return zend_optimizer_get_class_entry(script, op_array, Z_STR_P(op1 + 1));
		}
	} else if (opline->op1_type == IS_UNUSED && op_array->scope
			&& !(op_array->scope->ce_flags & ZEND_ACC_TRAIT)
			&& ((opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_SELF
				|| ((opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_STATIC
					&& (op_array->scope->ce_flags & ZEND_ACC_FINAL)))) {
		return op_array->scope;
	}
	return NULL;
}

zend_function *zend_optimizer_get_called_func(
		zend_script *script, zend_op_array *op_array, zend_op *opline, bool *is_prototype)
{
	*is_prototype = 0;
	switch (opline->opcode) {
		case ZEND_INIT_FCALL:
		{
			zval *func_zv = CRT_CONSTANT(opline->op2);
			if (Z_TYPE_P(func_zv) == IS_PTR) {
				return Z_PTR_P(func_zv);
			}
			ZEND_ASSERT(Z_TYPE_P(func_zv) == IS_STRING);
			zend_string *function_name = Z_STR_P(func_zv);
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
			if (opline->op2_type == IS_CONST && Z_TYPE_P(CRT_CONSTANT(opline->op2)) == IS_STRING) {
				zval *function_name = CRT_CONSTANT(opline->op2) + 1;
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
			if (opline->op2_type == IS_CONST && Z_TYPE_P(CRT_CONSTANT(opline->op2)) == IS_STRING) {
				zend_class_entry *ce = zend_optimizer_get_class_entry_from_op1(
					script, op_array, opline);
				if (ce) {
					zend_string *func_name = Z_STR_P(CRT_CONSTANT(opline->op2) + 1);
					zend_function *fbc = zend_hash_find_ptr(&ce->function_table, func_name);
					if (fbc) {
						bool is_public = (fbc->common.fn_flags & ZEND_ACC_PUBLIC) != 0;
						bool same_scope = fbc->common.scope == op_array->scope;
						if (is_public || same_scope) {
							return fbc;
						}
					}
				}
			}
			break;
		case ZEND_INIT_METHOD_CALL:
			if (opline->op1_type == IS_UNUSED
					&& opline->op2_type == IS_CONST && Z_TYPE_P(CRT_CONSTANT(opline->op2)) == IS_STRING
					&& op_array->scope
					&& !(op_array->fn_flags & ZEND_ACC_TRAIT_CLONE)
					&& !(op_array->scope->ce_flags & ZEND_ACC_TRAIT)) {
				zend_string *method_name = Z_STR_P(CRT_CONSTANT(opline->op2) + 1);
				zend_function *fbc = zend_hash_find_ptr(
					&op_array->scope->function_table, method_name);
				if (fbc) {
					bool is_private = (fbc->common.fn_flags & ZEND_ACC_PRIVATE) != 0;
					if (is_private) {
						/* Only use private method if in the same scope. We can't even use it
						 * as a prototype, as it may be overridden with changed signature. */
						bool same_scope = fbc->common.scope == op_array->scope;
						return same_scope ? fbc : NULL;
					}
					/* Prototype methods are potentially overridden. fbc still contains useful type information.
					 * Some optimizations may not be applied, like inlining or inferring the send-mode of superfluous args.
					 * A method cannot be overridden if the class or method is final. */
					if ((fbc->common.fn_flags & ZEND_ACC_FINAL) == 0 &&
						(fbc->common.scope->ce_flags & ZEND_ACC_FINAL) == 0) {
						*is_prototype = true;
					}
					return fbc;
				}
			}
			break;
		case ZEND_NEW:
		{
			zend_class_entry *ce = zend_optimizer_get_class_entry_from_op1(
				script, op_array, opline);
			if (ce && ce->type == ZEND_USER_CLASS) {
				return ce->constructor;
			}
			break;
		}
	}
	return NULL;
}

uint32_t zend_optimizer_classify_function(zend_string *name, uint32_t num_args) {
	if (zend_string_equals_literal(name, "extract")) {
		return ZEND_FUNC_INDIRECT_VAR_ACCESS;
	} else if (zend_string_equals_literal(name, "compact")) {
		return ZEND_FUNC_INDIRECT_VAR_ACCESS;
	} else if (zend_string_equals_literal(name, "get_defined_vars")) {
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

	/* pass 1 (Simple local optimizations)
	 * - persistent constant substitution (true, false, null, etc)
	 * - constant casting (ADD expects numbers, CONCAT strings, etc)
	 * - constant expression evaluation
	 * - optimize constant conditional JMPs
	 * - pre-evaluate constant function calls
	 * - eliminate FETCH $GLOBALS followed by FETCH_DIM/UNSET_DIM/ISSET_ISEMPTY_DIM
	 */
	if (ZEND_OPTIMIZER_PASS_1 & ctx->optimization_level) {
		zend_optimizer_pass1(op_array, ctx);
		if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_1) {
			zend_dump_op_array(op_array, 0, "after pass 1", NULL);
		}
	}

	/* pass 3: (Jump optimization)
	 * - optimize series of JMPs
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

	/* pass 9:
	 * - Optimize temp variables usage
	 */
	if ((ZEND_OPTIMIZER_PASS_9 & ctx->optimization_level) &&
	    !(ZEND_OPTIMIZER_PASS_7 & ctx->optimization_level)) {
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

	ZEND_ASSERT((op_array->fn_flags & ZEND_ACC_DONE_PASS_TWO) != 0);

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		if (opline->op1_type == IS_CONST) {
			ZEND_PASS_TWO_UNDO_CONSTANT(op_array, opline, opline->op1);
		}
		if (opline->op2_type == IS_CONST) {
			ZEND_PASS_TWO_UNDO_CONSTANT(op_array, opline, opline->op2);
		}
		/* reset smart branch flags IS_SMART_BRANCH_JMP[N]Z */
		opline->result_type &= (IS_TMP_VAR|IS_VAR|IS_CV|IS_CONST);
		opline++;
	}
#if !ZEND_USE_ABS_CONST_ADDR
	if (op_array->literals) {
		zval *literals = emalloc(sizeof(zval) * op_array->last_literal);
		memcpy(literals, op_array->literals, sizeof(zval) * op_array->last_literal);
		op_array->literals = literals;
	}
#endif

	op_array->T -= ZEND_OBSERVER_ENABLED;

	op_array->fn_flags &= ~ZEND_ACC_DONE_PASS_TWO;
}

static void zend_redo_pass_two(zend_op_array *op_array)
{
	zend_op *opline, *end;
#if ZEND_USE_ABS_JMP_ADDR && !ZEND_USE_ABS_CONST_ADDR
	zend_op *old_opcodes = op_array->opcodes;
#endif

	ZEND_ASSERT((op_array->fn_flags & ZEND_ACC_DONE_PASS_TWO) == 0);

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

	op_array->T += ZEND_OBSERVER_ENABLED; // reserve last temporary for observers if enabled

	opline = op_array->opcodes;
	end = opline + op_array->last;
	while (opline < end) {
		if (opline->op1_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, opline->op1);
		}
		if (opline->op2_type == IS_CONST) {
			ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, opline->op2);
		}
		/* fix jumps to point to new array */
		switch (opline->opcode) {
#if ZEND_USE_ABS_JMP_ADDR && !ZEND_USE_ABS_CONST_ADDR
			case ZEND_JMP:
			case ZEND_FAST_CALL:
				opline->op1.jmp_addr = &op_array->opcodes[opline->op1.jmp_addr - old_opcodes];
				break;
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_JMP_SET:
			case ZEND_COALESCE:
			case ZEND_FE_RESET_R:
			case ZEND_FE_RESET_RW:
			case ZEND_ASSERT_CHECK:
			case ZEND_JMP_NULL:
			case ZEND_BIND_INIT_STATIC_OR_JMP:
			case ZEND_JMP_FRAMELESS:
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
			case ZEND_MATCH:
				/* relative extended_value don't have to be changed */
				break;
#endif
			case ZEND_IS_IDENTICAL:
			case ZEND_IS_NOT_IDENTICAL:
			case ZEND_IS_EQUAL:
			case ZEND_IS_NOT_EQUAL:
			case ZEND_IS_SMALLER:
			case ZEND_IS_SMALLER_OR_EQUAL:
			case ZEND_CASE:
			case ZEND_CASE_STRICT:
			case ZEND_ISSET_ISEMPTY_CV:
			case ZEND_ISSET_ISEMPTY_VAR:
			case ZEND_ISSET_ISEMPTY_DIM_OBJ:
			case ZEND_ISSET_ISEMPTY_PROP_OBJ:
			case ZEND_ISSET_ISEMPTY_STATIC_PROP:
			case ZEND_INSTANCEOF:
			case ZEND_TYPE_CHECK:
			case ZEND_DEFINED:
			case ZEND_IN_ARRAY:
			case ZEND_ARRAY_KEY_EXISTS:
				if (opline->result_type & IS_TMP_VAR) {
					/* reinitialize result_type of smart branch instructions */
					if (opline + 1 < end) {
						if ((opline+1)->opcode == ZEND_JMPZ
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							opline->result_type = IS_SMART_BRANCH_JMPZ | IS_TMP_VAR;
						} else if ((opline+1)->opcode == ZEND_JMPNZ
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							opline->result_type = IS_SMART_BRANCH_JMPNZ | IS_TMP_VAR;
						}
					}
				}
				break;
		}
		/* INIT_FCALL with IS_PTR *must* use the PTR specialization, because the default spec doesn't
		 * handle IS_PTR. */
		if (opline->opcode == ZEND_INIT_FCALL && Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) == IS_PTR) {
			zend_vm_set_opcode_handler_ex(opline, IS_UNUSED, IS_CONST, IS_UNUSED);
		} else {
			ZEND_VM_SET_OPCODE_HANDLER(opline);
		}
		opline++;
	}

	op_array->fn_flags |= ZEND_ACC_DONE_PASS_TWO;
}

static void zend_redo_pass_two_ex(zend_op_array *op_array, zend_ssa *ssa)
{
	zend_op *opline, *end;
#if ZEND_USE_ABS_JMP_ADDR && !ZEND_USE_ABS_CONST_ADDR
	zend_op *old_opcodes = op_array->opcodes;
#endif

	ZEND_ASSERT((op_array->fn_flags & ZEND_ACC_DONE_PASS_TWO) == 0);

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
		zend_ssa_op *ssa_op = &ssa->ops[opline - op_array->opcodes];
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

		/* fix jumps to point to new array */
		switch (opline->opcode) {
#if ZEND_USE_ABS_JMP_ADDR && !ZEND_USE_ABS_CONST_ADDR
			case ZEND_JMP:
			case ZEND_FAST_CALL:
				opline->op1.jmp_addr = &op_array->opcodes[opline->op1.jmp_addr - old_opcodes];
				break;
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_JMP_SET:
			case ZEND_COALESCE:
			case ZEND_FE_RESET_R:
			case ZEND_FE_RESET_RW:
			case ZEND_ASSERT_CHECK:
			case ZEND_JMP_NULL:
			case ZEND_BIND_INIT_STATIC_OR_JMP:
			case ZEND_JMP_FRAMELESS:
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
			case ZEND_MATCH:
				/* relative extended_value don't have to be changed */
				break;
#endif
			case ZEND_IS_IDENTICAL:
			case ZEND_IS_NOT_IDENTICAL:
			case ZEND_IS_EQUAL:
			case ZEND_IS_NOT_EQUAL:
			case ZEND_IS_SMALLER:
			case ZEND_IS_SMALLER_OR_EQUAL:
			case ZEND_CASE:
			case ZEND_CASE_STRICT:
			case ZEND_ISSET_ISEMPTY_CV:
			case ZEND_ISSET_ISEMPTY_VAR:
			case ZEND_ISSET_ISEMPTY_DIM_OBJ:
			case ZEND_ISSET_ISEMPTY_PROP_OBJ:
			case ZEND_ISSET_ISEMPTY_STATIC_PROP:
			case ZEND_INSTANCEOF:
			case ZEND_TYPE_CHECK:
			case ZEND_DEFINED:
			case ZEND_IN_ARRAY:
			case ZEND_ARRAY_KEY_EXISTS:
				if (opline->result_type & IS_TMP_VAR) {
					/* reinitialize result_type of smart branch instructions */
					if (opline + 1 < end) {
						if ((opline+1)->opcode == ZEND_JMPZ
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							opline->result_type = IS_SMART_BRANCH_JMPZ | IS_TMP_VAR;
						} else if ((opline+1)->opcode == ZEND_JMPNZ
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							opline->result_type = IS_SMART_BRANCH_JMPNZ | IS_TMP_VAR;
						}
					}
				}
				break;
		}
#ifdef ZEND_VERIFY_TYPE_INFERENCE
		if (ssa_op->op1_use >= 0) {
			opline->op1_use_type = ssa->var_info[ssa_op->op1_use].type;
		}
		if (ssa_op->op2_use >= 0) {
			opline->op2_use_type = ssa->var_info[ssa_op->op2_use].type;
		}
		if (ssa_op->result_use >= 0) {
			opline->result_use_type = ssa->var_info[ssa_op->result_use].type;
		}
		if (ssa_op->op1_def >= 0) {
			opline->op1_def_type = ssa->var_info[ssa_op->op1_def].type;
		}
		if (ssa_op->op2_def >= 0) {
			opline->op2_def_type = ssa->var_info[ssa_op->op2_def].type;
		}
		if (ssa_op->result_def >= 0) {
			opline->result_def_type = ssa->var_info[ssa_op->result_def].type;
		}
#endif
		zend_vm_set_opcode_handler_ex(opline, op1_info, op2_info, res_info);
		opline++;
	}

	op_array->fn_flags |= ZEND_ACC_DONE_PASS_TWO;
}

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
			zval *func_zv = RT_CONSTANT(opline, opline->op2);
			if (Z_TYPE_P(func_zv) == IS_STRING) {
				func = zend_hash_find_ptr(&ctx->script->function_table, Z_STR_P(func_zv));
			} else {
				ZEND_ASSERT(Z_TYPE_P(func_zv) == IS_PTR);
				func = Z_PTR_P(func_zv);
			}
			if (func) {
				opline->op1.num = zend_vm_calc_used_stack(opline->extended_value, func);
			}
		}
		opline++;
	}
}

static void zend_adjust_fcall_stack_size_graph(zend_op_array *op_array)
{
	zend_func_info *func_info = ZEND_FUNC_INFO(op_array);

	if (func_info) {
		zend_call_info *call_info =func_info->callee_info;

		while (call_info) {
			zend_op *opline = call_info->caller_init_opline;

			if (opline && call_info->callee_func && opline->opcode == ZEND_INIT_FCALL) {
				ZEND_ASSERT(!call_info->is_prototype);
				opline->op1.num = zend_vm_calc_used_stack(opline->extended_value, call_info->callee_func);
			}
			call_info = call_info->next_callee;
		}
	}
}

static bool needs_live_range(zend_op_array *op_array, zend_op *def_opline) {
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

static void zend_foreach_op_array_helper(
		zend_op_array *op_array, zend_op_array_func_t func, void *context) {
	func(op_array, context);
	for (uint32_t i = 0; i < op_array->num_dynamic_func_defs; i++) {
		zend_foreach_op_array_helper(op_array->dynamic_func_defs[i], func, context);
	}
}

void zend_foreach_op_array(zend_script *script, zend_op_array_func_t func, void *context)
{
	zval *zv;
	zend_op_array *op_array;

	zend_foreach_op_array_helper(&script->main_op_array, func, context);

	ZEND_HASH_MAP_FOREACH_PTR(&script->function_table, op_array) {
		zend_foreach_op_array_helper(op_array, func, context);
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_VAL(&script->class_table, zv) {
		if (Z_TYPE_P(zv) == IS_ALIAS_PTR) {
			continue;
		}
		zend_class_entry *ce = Z_CE_P(zv);
		ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, op_array) {
			if (op_array->scope == ce
					&& op_array->type == ZEND_USER_FUNCTION
					&& !(op_array->fn_flags & ZEND_ACC_ABSTRACT)
					&& !(op_array->fn_flags & ZEND_ACC_TRAIT_CLONE)) {
				zend_foreach_op_array_helper(op_array, func, context);
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();
}

static void step_optimize_op_array(zend_op_array *op_array, void *context) {
	zend_optimize_op_array(op_array, (zend_optimizer_ctx *) context);
}

static void step_adjust_fcall_stack_size(zend_op_array *op_array, void *context) {
	zend_adjust_fcall_stack_size(op_array, (zend_optimizer_ctx *) context);
}

static void step_dump_after_optimizer(zend_op_array *op_array, void *context) {
	zend_dump_op_array(op_array, ZEND_DUMP_LIVE_RANGES, "after optimizer", NULL);
}

static void zend_optimizer_call_registered_passes(zend_script *script, void *ctx) {
	for (int i = 0; i < zend_optimizer_registered_passes.last; i++) {
		if (!zend_optimizer_registered_passes.pass[i]) {
			continue;
		}

		zend_optimizer_registered_passes.pass[i](script, ctx);
	}
}

ZEND_API void zend_optimize_script(zend_script *script, zend_long optimization_level, zend_long debug_level)
{
	zend_op_array *op_array;
	zend_string *name;
	zend_optimizer_ctx ctx;
	zval *zv;

	ctx.arena = zend_arena_create(64 * 1024);
	ctx.script = script;
	ctx.constants = NULL;
	ctx.optimization_level = optimization_level;
	ctx.debug_level = debug_level;

	if ((ZEND_OPTIMIZER_PASS_6 & optimization_level) &&
	    (ZEND_OPTIMIZER_PASS_7 & optimization_level)) {
		/* Optimize using call-graph */
		zend_call_graph call_graph;
		zend_build_call_graph(&ctx.arena, script, &call_graph);

		int i;
		zend_func_info *func_info;

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			zend_revert_pass_two(call_graph.op_arrays[i]);
			zend_optimize(call_graph.op_arrays[i], &ctx);
		}

	    zend_analyze_call_graph(&ctx.arena, script, &call_graph);

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

		if (ZEND_OPTIMIZER_PASS_9 & optimization_level) {
			for (i = 0; i < call_graph.op_arrays_count; i++) {
				zend_optimize_temporary_variables(call_graph.op_arrays[i], &ctx);
				if (debug_level & ZEND_DUMP_AFTER_PASS_9) {
					zend_dump_op_array(call_graph.op_arrays[i], 0, "after pass 9", NULL);
				}
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

		if (ZEND_OBSERVER_ENABLED) {
			for (i = 0; i < call_graph.op_arrays_count; i++) {
				++call_graph.op_arrays[i]->T; // ensure accurate temporary count for stack size precalculation
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
				op_array->T -= ZEND_OBSERVER_ENABLED; // redo_pass_two will re-increment it

				zend_redo_pass_two(op_array);
				if (op_array->live_range) {
					zend_recalc_live_ranges(op_array, NULL);
				}
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			ZEND_SET_FUNC_INFO(call_graph.op_arrays[i], NULL);
		}
	} else {
		zend_foreach_op_array(script, step_optimize_op_array, &ctx);

		if (ZEND_OPTIMIZER_PASS_12 & optimization_level) {
			zend_foreach_op_array(script, step_adjust_fcall_stack_size, &ctx);
		}
	}

	ZEND_HASH_MAP_FOREACH_VAL(&script->class_table, zv) {
		if (Z_TYPE_P(zv) == IS_ALIAS_PTR) {
			continue;
		}
		zend_class_entry *ce = Z_CE_P(zv);
		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->function_table, name, op_array) {
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

	zend_optimizer_call_registered_passes(script, &ctx);

	if ((debug_level & ZEND_DUMP_AFTER_OPTIMIZER) &&
			(ZEND_OPTIMIZER_PASS_7 & optimization_level)) {
		zend_foreach_op_array(script, step_dump_after_optimizer, NULL);
	}

	if (ctx.constants) {
		zend_hash_destroy(ctx.constants);
	}
	zend_arena_destroy(ctx.arena);
}

ZEND_API int zend_optimizer_register_pass(zend_optimizer_pass_t pass)
{
	if (!pass) {
		return -1;
	}

	if (zend_optimizer_registered_passes.last == ZEND_OPTIMIZER_MAX_REGISTERED_PASSES) {
		return -1;
	}

	zend_optimizer_registered_passes.pass[
		zend_optimizer_registered_passes.last++] = pass;

	return zend_optimizer_registered_passes.last;
}

ZEND_API void zend_optimizer_unregister_pass(int idx)
{
	zend_optimizer_registered_passes.pass[idx-1] = NULL;
}

zend_result zend_optimizer_startup(void)
{
	return zend_func_info_startup();
}

zend_result zend_optimizer_shutdown(void)
{
	return zend_func_info_shutdown();
}
