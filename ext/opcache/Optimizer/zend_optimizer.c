/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 The PHP Group                                |
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

#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"

#define OPTIMIZATION_LEVEL \
	ZCG(accel_directives).optimization_level

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
int zend_optimizer_add_literal(zend_op_array *op_array, const zval *zv TSRMLS_DC)
{
	int i = op_array->last_literal;
	op_array->last_literal++;
	if (i >= CG(context).literals_size) {
		CG(context).literals_size += 16; /* FIXME */
		op_array->literals = (zend_literal*)erealloc(op_array->literals, CG(context).literals_size * sizeof(zend_literal));
	}
	op_array->literals[i].constant = *zv;
	op_array->literals[i].hash_value = 0;
	op_array->literals[i].cache_slot = -1;
	Z_SET_REFCOUNT(op_array->literals[i].constant, 2);
	Z_SET_ISREF(op_array->literals[i].constant);
	return i;
}

# define LITERAL_LONG(op, val) do { \
		zval _c; \
		ZVAL_LONG(&_c, val); \
		op.constant = zend_optimizer_add_literal(op_array, &_c TSRMLS_CC); \
	} while (0)

# define LITERAL_BOOL(op, val) do { \
		zval _c; \
		ZVAL_BOOL(&_c, val); \
		op.constant = zend_optimizer_add_literal(op_array, &_c TSRMLS_CC); \
	} while (0)

# define literal_dtor(zv) do { \
		zval_dtor(zv); \
		Z_TYPE_P(zv) = IS_NULL; \
	} while (0)

#define COPY_NODE(target, src) do { \
		target ## _type = src ## _type; \
		target = src; \
	} while (0)

#else

# define LITERAL_LONG(op, val) ZVAL_LONG(&op.u.constant, val)

# define LITERAL_BOOL(op, val) ZVAL_BOOL(&op.u.constant, val)

# define literal_dtor(zv) zval_dtor(zv)

#define COPY_NODE(target, src) do { \
		target = src; \
	} while (0)

#endif

#include "Optimizer/nop_removal.c"
#include "Optimizer/block_pass.c"
#include "Optimizer/optimize_temp_vars_5.c"

void zend_optimizer(zend_op_array *op_array TSRMLS_DC)
{
	if (op_array->type == ZEND_EVAL_CODE ||
	    (op_array->fn_flags & ZEND_ACC_INTERACTIVE)) {
		return;
	}

	/* pass 1
	 * - substitute persistent constants (true, false, null, etc)
	 * - perform compile-time evaluation of constant binary and unary operations
	 * - optimize series of ADD_STRING and/or ADD_CHAR
	 * - convert CAST(IS_BOOL,x) into BOOL(x)
	 * - convert INTI_FCALL_BY_NAME + DO_FCALL_BY_NAME into DO_FCALL
	 */
#include "Optimizer/pass1_5.c"

	/* pass 2:
	 * - convert non-numeric constants to numeric constants in numeric operators
	 * - optimize constant conditional JMPs
	 * - optimize static BRKs and CONTs
	 */
#include "Optimizer/pass2.c"

	/* pass 3:
	 * - optimize $i = $i+expr to $i+=expr
	 * - optimize series of JMPs
	 * - change $i++ to ++$i where possible
	 */
#include "Optimizer/pass3.c"

	/* pass 5:
	 * - CFG optimization
	 */
#include "Optimizer/pass5.c"

	 /* pass 9:
	 * - Optimize temp variables usage
	 */
#include "Optimizer/pass9.c"

	/* pass 10:
	 * - remove NOPs
	 */
#include "Optimizer/pass10.c"
}
