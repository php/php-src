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

#include "ZendAccelerator.h"

#if ZEND_EXTENSION_API_NO > PHP_5_4_X_API_NO
# define VAR_NUM(v) ((zend_uint)(EX_TMP_VAR_NUM(0, 0) - EX_TMP_VAR(0, v)))
# define NUM_VAR(v) ((zend_uint)(zend_uintptr_t)EX_TMP_VAR_NUM(0, v))
#elif ZEND_EXTENSION_API_NO > PHP_5_2_X_API_NO
# define VAR_NUM(v) ((v)/ZEND_MM_ALIGNED_SIZE(sizeof(temp_variable)))
# define NUM_VAR(v) ((v)*ZEND_MM_ALIGNED_SIZE(sizeof(temp_variable)))
#else
# define VAR_NUM(v) ((v)/(sizeof(temp_variable)))
# define NUM_VAR(v) ((v)*(sizeof(temp_variable)))
#endif

#define INV_COND(op)       ((op) == ZEND_JMPZ    ? ZEND_JMPNZ    : ZEND_JMPZ)
#define INV_EX_COND(op)    ((op) == ZEND_JMPZ_EX ? ZEND_JMPNZ    : ZEND_JMPZ)
#define INV_COND_EX(op)    ((op) == ZEND_JMPZ    ? ZEND_JMPNZ_EX : ZEND_JMPZ_EX)
#define INV_EX_COND_EX(op) ((op) == ZEND_JMPZ_EX ? ZEND_JMPNZ_EX : ZEND_JMPZ_EX)

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
# define MAKE_NOP(opline)	{ opline->opcode = ZEND_NOP;  memset(&opline->result,0,sizeof(opline->result)); memset(&opline->op1,0,sizeof(opline->op1)); memset(&opline->op2,0,sizeof(opline->op2)); opline->result_type=opline->op1_type=opline->op2_type=IS_UNUSED; opline->handler = zend_opcode_handlers[ZEND_NOP]; }
# define RESULT_USED(op)	(((op->result_type & IS_VAR) && !(op->result_type & EXT_TYPE_UNUSED)) || op->result_type == IS_TMP_VAR)
# define RESULT_UNUSED(op)	((op->result_type & EXT_TYPE_UNUSED) != 0)
# define SAME_VAR(op1, op2) ((((op1 ## _type & IS_VAR) && (op2 ## _type & IS_VAR)) || (op1 ## _type == IS_TMP_VAR && op2 ## _type == IS_TMP_VAR)) && op1.var == op2.var)
#else
# define MAKE_NOP(opline)	{ opline->opcode = ZEND_NOP;  memset(&opline->result,0,sizeof(znode)); memset(&opline->op1,0,sizeof(znode)); memset(&opline->op2,0,sizeof(znode)); opline->result.op_type=opline->op1.op_type=opline->op2.op_type=IS_UNUSED; opline->handler = zend_opcode_handlers[ZEND_NOP]; }
# define RESULT_USED(op)	((op->result.op_type == IS_VAR && (op->result.u.EA.type & EXT_TYPE_UNUSED) == 0) || (op->result.op_type == IS_TMP_VAR))
# define RESULT_UNUSED(op)	((op->result.op_type == IS_VAR) && (op->result.u.EA.type == EXT_TYPE_UNUSED))
# define SAME_VAR(op1, op2) 	(((op1.op_type == IS_VAR && op2.op_type == IS_VAR) || (op1.op_type == IS_TMP_VAR && op2.op_type == IS_TMP_VAR)) && op1.u.var == op2.u.var)
#endif

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
} zend_cfg;

struct _zend_block_source {
	zend_code_block    *from;
	zend_block_source  *next;
};

#endif
