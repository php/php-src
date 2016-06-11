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

#ifndef ZEND_OPTIMIZER_INTERNAL_H
#define ZEND_OPTIMIZER_INTERNAL_H

#include "zend_ssa.h"

#define ZEND_RESULT_TYPE(opline)		(opline)->result_type
#define ZEND_RESULT(opline)				(opline)->result
#define ZEND_OP1_TYPE(opline)			(opline)->op1_type
#define ZEND_OP1(opline)				(opline)->op1
#define ZEND_OP1_LITERAL(opline)		(op_array)->literals[(opline)->op1.constant]
#define ZEND_OP1_JMP_ADDR(opline)		OP_JMP_ADDR(opline, (opline)->op1)
#define ZEND_OP2_TYPE(opline)			(opline)->op2_type
#define ZEND_OP2(opline)				(opline)->op2
#define ZEND_OP2_LITERAL(opline)		(op_array)->literals[(opline)->op2.constant]
#define ZEND_OP2_JMP_ADDR(opline)		OP_JMP_ADDR(opline, (opline)->op2)

#define VAR_NUM(v) EX_VAR_TO_NUM(v)
#define NUM_VAR(v) ((uint32_t)(zend_uintptr_t)ZEND_CALL_VAR_NUM(0, v))

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
		zval_dtor(zv); \
		ZVAL_NULL(zv); \
	} while (0)

#define COPY_NODE(target, src) do { \
		target ## _type = src ## _type; \
		target = src; \
	} while (0)

int  zend_optimizer_add_literal(zend_op_array *op_array, zval *zv);
int  zend_optimizer_get_persistent_constant(zend_string *name, zval *result, int copy);
void zend_optimizer_collect_constant(zend_optimizer_ctx *ctx, zval *name, zval* value);
int  zend_optimizer_get_collected_constant(HashTable *constants, zval *name, zval* value);
int  zend_optimizer_lookup_cv(zend_op_array *op_array, zend_string* name);
int zend_optimizer_update_op1_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zval          *val);
int zend_optimizer_update_op2_const(zend_op_array *op_array,
                                    zend_op       *opline,
                                    zval          *val);
int  zend_optimizer_replace_by_const(zend_op_array *op_array,
                                     zend_op       *opline,
                                     zend_uchar     type,
                                     uint32_t       var,
                                     zval          *val);

void zend_optimizer_remove_live_range(zend_op_array *op_array, uint32_t var);
void zend_optimizer_pass1(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimizer_pass2(zend_op_array *op_array);
void zend_optimizer_pass3(zend_op_array *op_array);
void zend_optimize_func_calls(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimize_cfg(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimize_dfa(zend_op_array *op_array, zend_optimizer_ctx *ctx);
int  zend_dfa_analyze_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa, uint32_t *flags);
void zend_dfa_optimize_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa);
void zend_optimize_temporary_variables(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimizer_nop_removal(zend_op_array *op_array);
void zend_optimizer_compact_literals(zend_op_array *op_array, zend_optimizer_ctx *ctx);
int zend_optimizer_is_disabled_func(const char *name, size_t len);
zend_function *zend_optimizer_get_called_func(
		zend_script *script, zend_op_array *op_array, zend_op *opline, zend_bool rt_constants);
uint32_t zend_optimizer_classify_function(zend_string *name, uint32_t num_args);

#endif
