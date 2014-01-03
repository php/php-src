/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 The PHP Group                                |
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

#define OPTIMIZATION_LEVEL \
	ZCG(accel_directives).optimization_level

#if ZEND_EXTENSION_API_NO >= PHP_5_5_X_API_NO
static int zend_optimizer_lookup_cv(zend_op_array *op_array, char* name, int name_len)
{
	int i = 0;
	ulong hash_value = zend_inline_hash_func(name, name_len+1);

	while (i < op_array->last_var) {
		if (op_array->vars[i].name == name ||
		    (op_array->vars[i].hash_value == hash_value &&
		     op_array->vars[i].name_len == name_len &&
		     memcmp(op_array->vars[i].name, name, name_len) == 0)) {
			return i;
		}
		i++;
	}
	i = op_array->last_var;
	op_array->last_var++;
	op_array->vars = erealloc(op_array->vars, op_array->last_var * sizeof(zend_compiled_variable));
	if (IS_INTERNED(name)) {
		op_array->vars[i].name = name;
	} else {
		op_array->vars[i].name = estrndup(name, name_len);
	}
	op_array->vars[i].name_len = name_len;
	op_array->vars[i].hash_value = hash_value;
	return i;
}
#endif

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

static void update_op1_const(zend_op_array *op_array,
                             zend_op       *opline,
                             zval          *val TSRMLS_DC)
{
	if (opline->opcode == ZEND_FREE) {
		MAKE_NOP(opline);
		zval_dtor(val);
	} else {
		ZEND_OP1_TYPE(opline) = IS_CONST;
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		if (Z_TYPE_P(val) == IS_STRING) {
			switch (opline->opcode) {
				case ZEND_INIT_STATIC_METHOD_CALL:
				case ZEND_CATCH:
				case ZEND_FETCH_CONSTANT:
					opline->op1.constant = zend_optimizer_add_literal(op_array, val TSRMLS_CC);
					Z_HASH_P(&ZEND_OP1_LITERAL(opline)) = zend_hash_func(Z_STRVAL(ZEND_OP1_LITERAL(opline)), Z_STRLEN(ZEND_OP1_LITERAL(opline)) + 1);
					op_array->literals[opline->op1.constant].cache_slot = op_array->last_cache_slot++;
					zend_str_tolower(Z_STRVAL_P(val), Z_STRLEN_P(val));
					zend_optimizer_add_literal(op_array, val TSRMLS_CC);
					op_array->literals[opline->op1.constant+1].hash_value = zend_hash_func(Z_STRVAL(op_array->literals[opline->op1.constant+1].constant), Z_STRLEN(op_array->literals[opline->op1.constant+1].constant) + 1);
					break;
				case ZEND_DO_FCALL:
					zend_str_tolower(Z_STRVAL_P(val), Z_STRLEN_P(val));
					opline->op1.constant = zend_optimizer_add_literal(op_array, val TSRMLS_CC);
					Z_HASH_P(&ZEND_OP1_LITERAL(opline)) = zend_hash_func(Z_STRVAL(ZEND_OP1_LITERAL(opline)), Z_STRLEN(ZEND_OP1_LITERAL(opline)) + 1);
					op_array->literals[opline->op1.constant].cache_slot = op_array->last_cache_slot++;
					break;
				default:
					opline->op1.constant = zend_optimizer_add_literal(op_array, val TSRMLS_CC);
					Z_HASH_P(&ZEND_OP1_LITERAL(opline)) = zend_hash_func(Z_STRVAL(ZEND_OP1_LITERAL(opline)), Z_STRLEN(ZEND_OP1_LITERAL(opline)) + 1);
					break;
			}
		} else {
			opline->op1.constant = zend_optimizer_add_literal(op_array, val TSRMLS_CC);
		}
#else
		ZEND_OP1_LITERAL(opline) = *val;
#endif
	}
}

static void update_op2_const(zend_op_array *op_array,
                             zend_op       *opline,
                             zval          *val TSRMLS_DC)
{
	ZEND_OP2_TYPE(opline) = IS_CONST;
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	opline->op2.constant = zend_optimizer_add_literal(op_array, val TSRMLS_CC);
	if (Z_TYPE_P(val) == IS_STRING) {
		Z_HASH_P(&ZEND_OP2_LITERAL(opline)) = zend_hash_func(Z_STRVAL(ZEND_OP2_LITERAL(opline)), Z_STRLEN(ZEND_OP2_LITERAL(opline)) + 1);
		switch (opline->opcode) {
			case ZEND_FETCH_R:
			case ZEND_FETCH_W:
			case ZEND_FETCH_RW:
			case ZEND_FETCH_IS:
			case ZEND_FETCH_UNSET:
			case ZEND_FETCH_FUNC_ARG:
			case ZEND_FETCH_CLASS:
			case ZEND_INIT_FCALL_BY_NAME:
			/*case ZEND_INIT_NS_FCALL_BY_NAME:*/
			case ZEND_UNSET_VAR:
			case ZEND_ISSET_ISEMPTY_VAR:
			case ZEND_ADD_INTERFACE:
			case ZEND_ADD_TRAIT:
				op_array->literals[opline->op2.constant].cache_slot = op_array->last_cache_slot++;
				zend_str_tolower(Z_STRVAL_P(val), Z_STRLEN_P(val));
				zend_optimizer_add_literal(op_array, val TSRMLS_CC);
				op_array->literals[opline->op2.constant+1].hash_value = zend_hash_func(Z_STRVAL(op_array->literals[opline->op2.constant+1].constant), Z_STRLEN(op_array->literals[opline->op2.constant+1].constant) + 1);
				break;
			case ZEND_INIT_METHOD_CALL:
			case ZEND_INIT_STATIC_METHOD_CALL:
				zend_str_tolower(Z_STRVAL_P(val), Z_STRLEN_P(val));
				zend_optimizer_add_literal(op_array, val TSRMLS_CC);
				op_array->literals[opline->op2.constant+1].hash_value = zend_hash_func(Z_STRVAL(op_array->literals[opline->op2.constant+1].constant), Z_STRLEN(op_array->literals[opline->op2.constant+1].constant) + 1);
				/* break missing intentionally */						
			/*case ZEND_FETCH_CONSTANT:*/
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
				op_array->literals[opline->op2.constant].cache_slot = op_array->last_cache_slot;
				op_array->last_cache_slot += 2;
				break;
			case ZEND_ASSIGN_ADD:
			case ZEND_ASSIGN_SUB:
			case ZEND_ASSIGN_MUL:
			case ZEND_ASSIGN_DIV:
			case ZEND_ASSIGN_MOD:
			case ZEND_ASSIGN_SL:
			case ZEND_ASSIGN_SR:
			case ZEND_ASSIGN_CONCAT:
			case ZEND_ASSIGN_BW_OR:
			case ZEND_ASSIGN_BW_AND:
			case ZEND_ASSIGN_BW_XOR:
				if (opline->extended_value == ZEND_ASSIGN_OBJ) {
					op_array->literals[opline->op2.constant].cache_slot = op_array->last_cache_slot;
					op_array->last_cache_slot += 2;
				}
				break;
#if ZEND_EXTENSION_API_NO >= PHP_5_4_X_API_NO
			case ZEND_OP_DATA:
				if ((opline-1)->opcode == ZEND_ASSIGN_DIM ||
				    ((opline-1)->extended_value == ZEND_ASSIGN_DIM &&
				     ((opline-1)->opcode == ZEND_ASSIGN_ADD ||
				     (opline-1)->opcode == ZEND_ASSIGN_SUB ||
				     (opline-1)->opcode == ZEND_ASSIGN_MUL ||
				     (opline-1)->opcode == ZEND_ASSIGN_DIV ||
				     (opline-1)->opcode == ZEND_ASSIGN_MOD ||
				     (opline-1)->opcode == ZEND_ASSIGN_SL ||
				     (opline-1)->opcode == ZEND_ASSIGN_SR ||
				     (opline-1)->opcode == ZEND_ASSIGN_CONCAT ||
				     (opline-1)->opcode == ZEND_ASSIGN_BW_OR ||
				     (opline-1)->opcode == ZEND_ASSIGN_BW_AND ||
				     (opline-1)->opcode == ZEND_ASSIGN_BW_XOR))) {
					goto check_numeric;
				}
				break;
			case ZEND_ISSET_ISEMPTY_DIM_OBJ:
			case ZEND_ADD_ARRAY_ELEMENT:
			case ZEND_INIT_ARRAY:
			case ZEND_UNSET_DIM:
			case ZEND_FETCH_DIM_R:
			case ZEND_FETCH_DIM_W:
			case ZEND_FETCH_DIM_RW:
			case ZEND_FETCH_DIM_IS:
			case ZEND_FETCH_DIM_FUNC_ARG:
			case ZEND_FETCH_DIM_UNSET:
			case ZEND_FETCH_DIM_TMP_VAR:
check_numeric:
				{
					ulong index;
					int numeric = 0;

					ZEND_HANDLE_NUMERIC_EX(Z_STRVAL_P(val), Z_STRLEN_P(val)+1, index, numeric = 1);
					if (numeric) {
						zval_dtor(val);
						ZVAL_LONG(val, index);
						op_array->literals[opline->op2.constant].constant = *val;
		        	}
				}
				break;
#endif
			default:
				break;
		}
	}
#else
	ZEND_OP2_LITERAL(opline) = *val;
#endif
}

static void replace_tmp_by_const(zend_op_array *op_array,
                                 zend_op       *opline,
                                 zend_uint      var,
                                 zval          *val
                                 TSRMLS_DC)
{
	zend_op *end = op_array->opcodes + op_array->last;

	while (opline < end) {
		if (ZEND_OP1_TYPE(opline) == IS_TMP_VAR &&
			ZEND_OP1(opline).var == var) {

			update_op1_const(op_array, opline, val TSRMLS_CC);
			/* TMP_VAR my be used only once */
			break;
		}

		if (ZEND_OP2_TYPE(opline) == IS_TMP_VAR &&
			ZEND_OP2(opline).var == var) {

			update_op2_const(op_array, opline, val TSRMLS_CC);
			/* TMP_VAR my be used only once */
			break;
		}
		opline++;
	}
}

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
