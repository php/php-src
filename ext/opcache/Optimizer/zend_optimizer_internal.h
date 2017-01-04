/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 The PHP Group                                |
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

#include "ZendAccelerator.h"

#define VAR_NUM(v) EX_VAR_TO_NUM(v)
#define NUM_VAR(v) ((uint32_t)(zend_uintptr_t)ZEND_CALL_VAR_NUM(0, v))

#define INV_COND(op)       ((op) == ZEND_JMPZ    ? ZEND_JMPNZ    : ZEND_JMPZ)
#define INV_EX_COND(op)    ((op) == ZEND_JMPZ_EX ? ZEND_JMPNZ    : ZEND_JMPZ)
#define INV_COND_EX(op)    ((op) == ZEND_JMPZ    ? ZEND_JMPNZ_EX : ZEND_JMPZ_EX)
#define INV_EX_COND_EX(op) ((op) == ZEND_JMPZ_EX ? ZEND_JMPNZ_EX : ZEND_JMPZ_EX)

#undef MAKE_NOP

#define MAKE_NOP(opline) do { \
	(opline)->op1.num = 0; \
	(opline)->op2.num = 0; \
	(opline)->result.num = 0; \
	(opline)->opcode = ZEND_NOP; \
	(opline)->op1_type =  IS_UNUSED; \
	(opline)->op2_type = IS_UNUSED; \
	(opline)->result_type = IS_UNUSED; \
	 zend_vm_set_opcode_handler(opline); \
} while (0)

#define RESULT_USED(op)	    (((op->result_type & IS_VAR) && !(op->result_type & EXT_TYPE_UNUSED)) || op->result_type == IS_TMP_VAR)
#define RESULT_UNUSED(op)	((op->result_type & EXT_TYPE_UNUSED) != 0)
#define SAME_VAR(op1, op2)  ((((op1 ## _type & IS_VAR) && (op2 ## _type & IS_VAR)) || (op1 ## _type == IS_TMP_VAR && op2 ## _type == IS_TMP_VAR)) && op1.var == op2.var)

typedef struct _zend_optimizer_ctx {
	zend_arena             *arena;
	zend_persistent_script *script;
	HashTable              *constants;
} zend_optimizer_ctx;

typedef struct _zend_code_block zend_code_block;
typedef struct _zend_block_source zend_block_source;

struct _zend_code_block {
	int                 access;
	zend_op            *start_opline;
	int                 start_opline_no;
	int                 len;
	zend_code_block    *op1_to;
	zend_code_block    *op2_to;
	zend_code_block    *ext_to;
	zend_code_block    *follow_to;
	zend_code_block    *next;
	zend_block_source  *sources;
	zend_bool           protected; /* don't merge this block with others */
};

typedef struct _zend_cfg {
	zend_code_block    *blocks;
	zend_code_block   **try;
	zend_code_block   **catch;
	zend_code_block   **loop_start;
	zend_code_block   **loop_cont;
	zend_code_block   **loop_brk;
	zend_op           **Tsource;
	char               *same_t;
} zend_cfg;

struct _zend_block_source {
	zend_code_block    *from;
	zend_block_source  *next;
};

#define OPTIMIZATION_LEVEL \
	ZCG(accel_directives).optimization_level

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

void zend_optimizer_pass1(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimizer_pass2(zend_op_array *op_array);
void zend_optimizer_pass3(zend_op_array *op_array);
void optimize_func_calls(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void optimize_cfg(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void optimize_temporary_variables(zend_op_array *op_array, zend_optimizer_ctx *ctx);
void zend_optimizer_nop_removal(zend_op_array *op_array);
void zend_optimizer_compact_literals(zend_op_array *op_array, zend_optimizer_ctx *ctx);
int zend_optimizer_is_disabled_func(const char *name, size_t len);

#endif
