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

#ifndef ZEND_OPTIMIZER_INTERNAL_H
#define ZEND_OPTIMIZER_INTERNAL_H

#include "zend_ssa.h"
#include "zend_func_info.h"

#include <stdint.h>

#define ZEND_OP1_LITERAL(opline)		(op_array)->literals[(opline)->op1.constant]
#define ZEND_OP1_JMP_ADDR(opline)		OP_JMP_ADDR(opline, (opline)->op1)
#define ZEND_OP2_LITERAL(opline)		(op_array)->literals[(opline)->op2.constant]
#define ZEND_OP2_JMP_ADDR(opline)		OP_JMP_ADDR(opline, (opline)->op2)

#define VAR_NUM(v) EX_VAR_TO_NUM(v)
#define NUM_VAR(v) EX_NUM_TO_VAR(v)

#define INV_COND(op)       ((op) == ZEND_JMPZ    ? ZEND_JMPNZ    : ZEND_JMPZ)
#define INV_EX_COND(op)    ((op) == ZEND_JMPZ_EX ? ZEND_JMPNZ    : ZEND_JMPZ)
#define INV_COND_EX(op)    ((op) == ZEND_JMPZ    ? ZEND_JMPNZ_EX : ZEND_JMPZ_EX)
#define INV_EX_COND_EX(op) ((op) == ZEND_JMPZ_EX ? ZEND_JMPNZ_EX : ZEND_JMPZ_EX)

#define RESULT_UNUSED(op)	(op->result_type == IS_UNUSED)
#define SAME_VAR(op1, op2)  (op1 ## _type == op2 ## _type && op1.var == op2.var)

typedef struct _zend_optimizer_ctx {
	zend_arena             *arena;
	zend_script            *script;
	HashTable              *constants;
	zend_long               optimization_level;
	zend_long               debug_level;
} zend_optimizer_ctx;

#define LITERAL_LONG(op, val) do { \
		zval _c; \
		ZVAL_LONG(&_c, val); \
		op.constant = zend_optimizer_add_literal(op_array, &_c); \
	} while (0)

#define LITERAL_BOOL(op, val) do { \
		zval _c; \
		ZVAL_BOOL(&_c, val); \
		op.constant = zend_optimizer_add_literal(op_array, &_c); \
	} while (0)

#define literal_dtor(zv) do { \
		zval_ptr_dtor_nogc(zv); \
		ZVAL_NULL(zv); \
	} while (0)

#define COPY_NODE(target, src) do { \
		target ## _type = src ## _type; \
		target = src; \
	} while (0)

static inline bool zend_optimizer_is_loop_var_free(const zend_op *opline) {
	return (opline->opcode == ZEND_FE_FREE && opline->extended_value != ZEND_FREE_ON_RETURN)
		|| (opline->opcode == ZEND_FREE && opline->extended_value == ZEND_FREE_SWITCH);
}

void zend_optimizer_convert_to_free_op1(zend_op_array *op_array, zend_op *opline);
int  zend_optimizer_add_literal(zend_op_array *op_array, const zval *zv);
bool zend_optimizer_get_persistent_constant(zend_string *name, zval *result, int copy);
void zend_optimizer_collect_constant(zend_optimizer_ctx *ctx, zval *name, zval* value);
bool zend_optimizer_get_collected_constant(HashTable *constants, zval *name, zval* value);
zend_result zend_optimizer_eval_binary_op(zval *result, uint8_t opcode, zval *op1, zval *op2);
zend_result zend_optimizer_eval_unary_op(zval *result, uint8_t opcode, zval *op1);
zend_result zend_optimizer_eval_cast(zval *result, uint32_t type, zval *op1);
zend_result zend_optimizer_eval_strlen(zval *result, const zval *op1);
zend_result zend_optimizer_eval_special_func_call(
		zval *result, zend_string *name, zend_string *arg);
bool zend_optimizer_update_op1_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zval          *val);
bool zend_optimizer_update_op2_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zval          *val);
bool zend_optimizer_replace_by_const(zend_op_array *op_array,
                                     zend_op       *opline,
                                     uint8_t     type,
                                     uint32_t       var,
                                     zval          *val);
zend_op *zend_optimizer_get_loop_var_def(const zend_op_array *op_array, zend_op *free_opline);
zend_class_entry *zend_optimizer_get_class_entry(
		const zend_script *script, const zend_op_array *op_array, zend_string *lcname);
zend_class_entry *zend_optimizer_get_class_entry_from_op1(
		const zend_script *script, const zend_op_array *op_array, const zend_op *opline);
const zend_class_constant *zend_fetch_class_const_info(
		const zend_script *script, const zend_op_array *op_array, const zend_op *opline, bool *is_prototype);

void zend_optimizer_pass1(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimizer_pass3(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimize_func_calls(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimize_cfg(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimize_dfa(zend_op_array *op_array, zend_optimizer_ctx *ctx);
zend_result zend_dfa_analyze_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa);
void zend_dfa_optimize_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa, zend_call_info **call_map);
void zend_optimize_temporary_variables(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimizer_nop_removal(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimizer_compact_literals(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimizer_compact_vars(zend_op_array *op_array);
zend_function *zend_optimizer_get_called_func(
		zend_script *script, zend_op_array *op_array, zend_op *opline, bool *is_prototype);
uint32_t zend_optimizer_classify_function(zend_string *name, uint32_t num_args);
void zend_optimizer_migrate_jump(zend_op_array *op_array, zend_op *new_opline, zend_op *opline);
void zend_optimizer_shift_jump(zend_op_array *op_array, zend_op *opline, uint32_t *shiftlist);
int sccp_optimize_op_array(zend_optimizer_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa, zend_call_info **call_map);
int dce_optimize_op_array(zend_op_array *op_array, zend_optimizer_ctx *optimizer_ctx, zend_ssa *ssa, bool reorder_dtor_effects);
zend_result zend_ssa_escape_analysis(const zend_script *script, zend_op_array *op_array, zend_ssa *ssa);

typedef void (*zend_op_array_func_t)(zend_op_array *, void *context);
void zend_foreach_op_array(zend_script *script, zend_op_array_func_t func, void *context);

#endif
