/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stanislav Malyshev <stas@php.net>                            |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_gmp.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "zend_smart_str_public.h"
#include "zend_exceptions.h"

#if HAVE_GMP

#include <gmp.h>

/* Needed for gmp_random() */
#include "ext/standard/php_rand.h"
#include "ext/standard/php_lcg.h"
#define GMP_ABS(x) ((x) >= 0 ? (x) : -(x))

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_init, 0, 0, 1)
	ZEND_ARG_INFO(0, number)
	ZEND_ARG_INFO(0, base)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_import, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, word_size)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_export, 0, 0, 1)
	ZEND_ARG_INFO(0, gmpnumber)
	ZEND_ARG_INFO(0, word_size)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_intval, 0, 0, 1)
	ZEND_ARG_INFO(0, gmpnumber)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_strval, 0, 0, 1)
	ZEND_ARG_INFO(0, gmpnumber)
	ZEND_ARG_INFO(0, base)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_unary, 0, 0, 1)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_binary, 0, 0, 2)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_div, 0, 0, 2)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
	ZEND_ARG_INFO(0, round)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_pow, 0, 0, 2)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_INFO(0, exp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_powm, 0, 0, 3)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_INFO(0, exp)
	ZEND_ARG_INFO(0, mod)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_root, 0, 0, 2)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, nth)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_prob_prime, 0, 0, 1)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, reps)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_random, 0, 0, 0)
	ZEND_ARG_INFO(0, limiter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_random_seed, 0, 0, 1)
	ZEND_ARG_INFO(0, seed)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_random_bits, 0, 0, 1)
	ZEND_ARG_INFO(0, bits)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_random_range, 0, 0, 2)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_setbit, 0, 0, 2)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, set_clear)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_bit, 0, 0, 2)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_scan, 0, 0, 2)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, start)
ZEND_END_ARG_INFO()

/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(gmp)
static ZEND_GINIT_FUNCTION(gmp);

/* {{{ gmp_functions[]
 */
const zend_function_entry gmp_functions[] = {
	ZEND_FE(gmp_init,		arginfo_gmp_init)
	ZEND_FE(gmp_import,		arginfo_gmp_import)
	ZEND_FE(gmp_export,		arginfo_gmp_export)
	ZEND_FE(gmp_intval,		arginfo_gmp_intval)
	ZEND_FE(gmp_strval,		arginfo_gmp_strval)
	ZEND_FE(gmp_add,		arginfo_gmp_binary)
	ZEND_FE(gmp_sub,		arginfo_gmp_binary)
	ZEND_FE(gmp_mul,		arginfo_gmp_binary)
	ZEND_FE(gmp_div_qr,		arginfo_gmp_div)
	ZEND_FE(gmp_div_q,		arginfo_gmp_div)
	ZEND_FE(gmp_div_r,		arginfo_gmp_div)
	ZEND_FALIAS(gmp_div, gmp_div_q, arginfo_gmp_div)
	ZEND_FE(gmp_mod,		arginfo_gmp_binary)
	ZEND_FE(gmp_divexact,	arginfo_gmp_binary)
	ZEND_FE(gmp_neg,		arginfo_gmp_unary)
	ZEND_FE(gmp_abs,		arginfo_gmp_unary)
	ZEND_FE(gmp_fact,		arginfo_gmp_unary)
	ZEND_FE(gmp_sqrt,		arginfo_gmp_unary)
	ZEND_FE(gmp_sqrtrem,	arginfo_gmp_unary)
	ZEND_FE(gmp_root,		arginfo_gmp_root)
	ZEND_FE(gmp_rootrem,	arginfo_gmp_root)
	ZEND_FE(gmp_pow,		arginfo_gmp_pow)
	ZEND_FE(gmp_powm,		arginfo_gmp_powm)
	ZEND_FE(gmp_perfect_square,	arginfo_gmp_unary)
	ZEND_FE(gmp_prob_prime, arginfo_gmp_prob_prime)
	ZEND_FE(gmp_gcd,		arginfo_gmp_binary)
	ZEND_FE(gmp_gcdext,		arginfo_gmp_binary)
	ZEND_FE(gmp_invert,		arginfo_gmp_binary)
	ZEND_FE(gmp_jacobi,		arginfo_gmp_binary)
	ZEND_FE(gmp_legendre,	arginfo_gmp_binary)
	ZEND_FE(gmp_cmp,		arginfo_gmp_binary)
	ZEND_FE(gmp_sign,		arginfo_gmp_unary)
	ZEND_FE(gmp_random,		arginfo_gmp_random)
	ZEND_FE(gmp_random_seed,	arginfo_gmp_random_seed)
	ZEND_FE(gmp_random_bits,  arginfo_gmp_random_bits)
	ZEND_FE(gmp_random_range, arginfo_gmp_random_range)
	ZEND_FE(gmp_and,		arginfo_gmp_binary)
	ZEND_FE(gmp_or,			arginfo_gmp_binary)
	ZEND_FE(gmp_com,		arginfo_gmp_unary)
	ZEND_FE(gmp_xor,		arginfo_gmp_binary)
	ZEND_FE(gmp_setbit,		arginfo_gmp_setbit)
	ZEND_FE(gmp_clrbit,		arginfo_gmp_bit)
	ZEND_FE(gmp_testbit,	arginfo_gmp_bit)
	ZEND_FE(gmp_scan0,  	arginfo_gmp_scan)
	ZEND_FE(gmp_scan1,  	arginfo_gmp_scan)
	ZEND_FE(gmp_popcount,	arginfo_gmp_unary)
	ZEND_FE(gmp_hamdist,	arginfo_gmp_binary)
	ZEND_FE(gmp_nextprime,	arginfo_gmp_unary)
	PHP_FE_END
};
/* }}} */

/* {{{ gmp_module_entry
 */
zend_module_entry gmp_module_entry = {
	STANDARD_MODULE_HEADER,
	"gmp",
	gmp_functions,
	ZEND_MODULE_STARTUP_N(gmp),
	NULL,
	NULL,
	ZEND_MODULE_DEACTIVATE_N(gmp),
	ZEND_MODULE_INFO_N(gmp),
	NO_VERSION_YET,
	ZEND_MODULE_GLOBALS(gmp),
	ZEND_GINIT(gmp),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_GMP
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE;
#endif
ZEND_GET_MODULE(gmp)
#endif

zend_class_entry *gmp_ce;
static zend_object_handlers gmp_object_handlers;

typedef struct _gmp_object {
	mpz_t num;
	zend_object std;
} gmp_object;

typedef struct _gmp_temp {
	mpz_t num;
	zend_bool is_used;
} gmp_temp_t;

#define GMP_ROUND_ZERO      0
#define GMP_ROUND_PLUSINF   1
#define GMP_ROUND_MINUSINF  2

#define GMP_MSW_FIRST     (1 << 0)
#define GMP_LSW_FIRST     (1 << 1)
#define GMP_LITTLE_ENDIAN (1 << 2)
#define GMP_BIG_ENDIAN    (1 << 3)
#define GMP_NATIVE_ENDIAN (1 << 4)

#define GMP_MAX_BASE 62

#define IS_GMP(zval) \
	(Z_TYPE_P(zval) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zval), gmp_ce))

#define GET_GMP_OBJECT_FROM_OBJ(obj) \
	((gmp_object *) ((char *) (obj) - XtOffsetOf(gmp_object, std)))
#define GET_GMP_OBJECT_FROM_ZVAL(zv) \
	GET_GMP_OBJECT_FROM_OBJ(Z_OBJ_P(zv))

#define GET_GMP_FROM_ZVAL(zval) \
	GET_GMP_OBJECT_FROM_OBJ(Z_OBJ_P(zval))->num

/* The FETCH_GMP_ZVAL_* family of macros is used to fetch a gmp number
 * (mpz_ptr) from a zval. If the zval is not a GMP instance, then we
 * try to convert the value to a temporary gmp number using convert_to_gmp.
 * This temporary number is stored in the temp argument, which is of type
 * gmp_temp_t. This temporary value needs to be freed lateron using the
 * FREE_GMP_TEMP macro.
 *
 * If the conversion to a gmp number fails, the macros return false.
 * The _DEP / _DEP_DEP variants additionally free the temporary values
 * passed in the last / last two arguments.
 *
 * If one zval can sometimes be fetched as a long you have to set the
 * is_used member of the corresponding gmp_temp_t value to 0, otherwise
 * the FREE_GMP_TEMP and *_DEP macros will not work properly.
 *
 * The three FETCH_GMP_ZVAL_* macros below are mostly copy & paste code
 * as I couldn't find a way to combine them.
 */

#define FREE_GMP_TEMP(temp)  \
	if (temp.is_used) {      \
		mpz_clear(temp.num); \
	}

#define FETCH_GMP_ZVAL_DEP_DEP(gmpnumber, zval, temp, dep1, dep2) \
if (IS_GMP(zval)) {                                               \
	gmpnumber = GET_GMP_FROM_ZVAL(zval);                          \
	temp.is_used = 0;                                             \
} else {                                                          \
	mpz_init(temp.num);                                           \
	if (convert_to_gmp(temp.num, zval, 0) == FAILURE) { \
		mpz_clear(temp.num);                                      \
		FREE_GMP_TEMP(dep1);                                      \
		FREE_GMP_TEMP(dep2);                                      \
		RETURN_FALSE;                                             \
	}                                                             \
	temp.is_used = 1;                                             \
	gmpnumber = temp.num;                                         \
}

#define FETCH_GMP_ZVAL_DEP(gmpnumber, zval, temp, dep)            \
if (IS_GMP(zval)) {                                               \
	gmpnumber = GET_GMP_FROM_ZVAL(zval);                          \
	temp.is_used = 0;                                             \
} else {                                                          \
	mpz_init(temp.num);                                           \
	if (convert_to_gmp(temp.num, zval, 0) == FAILURE) { \
		mpz_clear(temp.num);                                      \
		FREE_GMP_TEMP(dep);                                       \
		RETURN_FALSE;                                             \
	}                                                             \
	temp.is_used = 1;                                             \
	gmpnumber = temp.num;                                         \
}

#define FETCH_GMP_ZVAL(gmpnumber, zval, temp)                     \
if (IS_GMP(zval)) {                                               \
	gmpnumber = GET_GMP_FROM_ZVAL(zval);                          \
	temp.is_used = 0;                                             \
} else {                                                          \
	mpz_init(temp.num);                                           \
	if (convert_to_gmp(temp.num, zval, 0) == FAILURE) { \
		mpz_clear(temp.num);                                      \
		RETURN_FALSE;                                             \
	}                                                             \
	temp.is_used = 1;                                             \
	gmpnumber = temp.num;                                         \
}

#define INIT_GMP_RETVAL(gmpnumber) \
	gmp_create(return_value, &gmpnumber)

static void gmp_strval(zval *result, mpz_t gmpnum, int base);
static int convert_to_gmp(mpz_t gmpnumber, zval *val, zend_long base);
static void gmp_cmp(zval *return_value, zval *a_arg, zval *b_arg);

/*
 * The gmp_*_op functions provide an implementation for several common types
 * of GMP functions. The gmp_zval_(unary|binary)_*_op functions have to be manually
 * passed zvals to work on, whereas the gmp_(unary|binary)_*_op macros already
 * include parameter parsing.
 */
typedef void (*gmp_unary_op_t)(mpz_ptr, mpz_srcptr);
typedef int (*gmp_unary_opl_t)(mpz_srcptr);

typedef void (*gmp_unary_ui_op_t)(mpz_ptr, gmp_ulong);

typedef void (*gmp_binary_op_t)(mpz_ptr, mpz_srcptr, mpz_srcptr);
typedef int (*gmp_binary_opl_t)(mpz_srcptr, mpz_srcptr);

typedef void (*gmp_binary_ui_op_t)(mpz_ptr, mpz_srcptr, gmp_ulong);
typedef void (*gmp_binary_op2_t)(mpz_ptr, mpz_ptr, mpz_srcptr, mpz_srcptr);
typedef void (*gmp_binary_ui_op2_t)(mpz_ptr, mpz_ptr, mpz_srcptr, gmp_ulong);

static inline void gmp_zval_binary_ui_op(zval *return_value, zval *a_arg, zval *b_arg, gmp_binary_op_t gmp_op, gmp_binary_ui_op_t gmp_ui_op, int check_b_zero);
static inline void gmp_zval_binary_ui_op2(zval *return_value, zval *a_arg, zval *b_arg, gmp_binary_op2_t gmp_op, gmp_binary_ui_op2_t gmp_ui_op, int check_b_zero);
static inline void gmp_zval_unary_op(zval *return_value, zval *a_arg, gmp_unary_op_t gmp_op);
static inline void gmp_zval_unary_ui_op(zval *return_value, zval *a_arg, gmp_unary_ui_op_t gmp_op);

/* Binary operations */
#define gmp_binary_ui_op(op, uop) _gmp_binary_ui_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op, uop, 0)
#define gmp_binary_op(op)         _gmp_binary_ui_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op, NULL, 0)
#define gmp_binary_opl(op) _gmp_binary_opl(INTERNAL_FUNCTION_PARAM_PASSTHRU, op)
#define gmp_binary_ui_op_no_zero(op, uop) \
	_gmp_binary_ui_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op, uop, 1)

/* Unary operations */
#define gmp_unary_op(op)         _gmp_unary_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op)
#define gmp_unary_opl(op)         _gmp_unary_opl(INTERNAL_FUNCTION_PARAM_PASSTHRU, op)
#define gmp_unary_ui_op(op)      _gmp_unary_ui_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op)

static void gmp_free_object_storage(zend_object *obj) /* {{{ */
{
	gmp_object *intern = GET_GMP_OBJECT_FROM_OBJ(obj);

	mpz_clear(intern->num);
	zend_object_std_dtor(&intern->std);
}
/* }}} */

static inline zend_object *gmp_create_object_ex(zend_class_entry *ce, mpz_ptr *gmpnum_target) /* {{{ */
{
	gmp_object *intern = emalloc(sizeof(gmp_object) + zend_object_properties_size(ce));

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);

	mpz_init(intern->num);
	*gmpnum_target = intern->num;
	intern->std.handlers = &gmp_object_handlers;

	return &intern->std;
}
/* }}} */

static zend_object *gmp_create_object(zend_class_entry *ce) /* {{{ */
{
	mpz_ptr gmpnum_dummy;
	return gmp_create_object_ex(ce, &gmpnum_dummy);
}
/* }}} */

static inline void gmp_create(zval *target, mpz_ptr *gmpnum_target) /* {{{ */
{
	ZVAL_OBJ(target, gmp_create_object_ex(gmp_ce, gmpnum_target));
}
/* }}} */

static int gmp_cast_object(zval *readobj, zval *writeobj, int type) /* {{{ */
{
	mpz_ptr gmpnum;
	switch (type) {
	case IS_STRING:
		gmpnum = GET_GMP_FROM_ZVAL(readobj);
		gmp_strval(writeobj, gmpnum, 10);
		return SUCCESS;
	case IS_LONG:
		gmpnum = GET_GMP_FROM_ZVAL(readobj);
		ZVAL_LONG(writeobj, mpz_get_si(gmpnum));
		return SUCCESS;
	case IS_DOUBLE:
		gmpnum = GET_GMP_FROM_ZVAL(readobj);
		ZVAL_DOUBLE(writeobj, mpz_get_d(gmpnum));
		return SUCCESS;
	default:
		return FAILURE;
	}
}
/* }}} */

static HashTable *gmp_get_debug_info(zval *obj, int *is_temp) /* {{{ */
{
	HashTable *ht, *props = zend_std_get_properties(obj);
	mpz_ptr gmpnum = GET_GMP_FROM_ZVAL(obj);
	zval zv;

	*is_temp = 1;
	ALLOC_HASHTABLE(ht);
	zend_array_dup(ht, props);

	gmp_strval(&zv, gmpnum, 10);
	zend_hash_str_update(ht, "num", sizeof("num")-1, &zv);

	return ht;
}
/* }}} */

static zend_object *gmp_clone_obj(zval *obj) /* {{{ */
{
	gmp_object *old_object = GET_GMP_OBJECT_FROM_ZVAL(obj);
	gmp_object *new_object = GET_GMP_OBJECT_FROM_OBJ(gmp_create_object(Z_OBJCE_P(obj)));

	zend_objects_clone_members( &new_object->std, &old_object->std);

	mpz_set(new_object->num, old_object->num);

	return &new_object->std;
}
/* }}} */

static void shift_operator_helper(gmp_binary_ui_op_t op, zval *return_value, zval *op1, zval *op2) {
	zend_long shift = zval_get_long(op2);

	if (shift < 0) {
		php_error_docref(NULL, E_WARNING, "Shift cannot be negative");
		RETVAL_FALSE;
	} else {
		mpz_ptr gmpnum_op, gmpnum_result;
		gmp_temp_t temp;

		FETCH_GMP_ZVAL(gmpnum_op, op1, temp);
		INIT_GMP_RETVAL(gmpnum_result);
		op(gmpnum_result, gmpnum_op, (gmp_ulong) shift);
		FREE_GMP_TEMP(temp);
	}
}

#define DO_BINARY_UI_OP_EX(op, uop, check_b_zero)       \
	gmp_zval_binary_ui_op(                              \
		result, op1, op2, op, (gmp_binary_ui_op_t) uop, \
		check_b_zero                          \
	);                                                  \
	return SUCCESS;

#define DO_BINARY_UI_OP(op) DO_BINARY_UI_OP_EX(op, op ## _ui, 0)
#define DO_BINARY_OP(op) DO_BINARY_UI_OP_EX(op, NULL, 0)

#define DO_UNARY_OP(op) \
	gmp_zval_unary_op(result, op1, op); \
	return SUCCESS;

static int gmp_do_operation_ex(zend_uchar opcode, zval *result, zval *op1, zval *op2) /* {{{ */
{
	switch (opcode) {
	case ZEND_ADD:
		DO_BINARY_UI_OP(mpz_add);
	case ZEND_SUB:
		DO_BINARY_UI_OP(mpz_sub);
	case ZEND_MUL:
		DO_BINARY_UI_OP(mpz_mul);
	case ZEND_POW:
		shift_operator_helper(mpz_pow_ui, result, op1, op2);
		return SUCCESS;
	case ZEND_DIV:
		DO_BINARY_UI_OP_EX(mpz_tdiv_q, mpz_tdiv_q_ui, 1);
	case ZEND_MOD:
		DO_BINARY_UI_OP_EX(mpz_mod, mpz_mod_ui, 1);
	case ZEND_SL:
		shift_operator_helper(mpz_mul_2exp, result, op1, op2);
		return SUCCESS;
	case ZEND_SR:
		shift_operator_helper(mpz_fdiv_q_2exp, result, op1, op2);
		return SUCCESS;
	case ZEND_BW_OR:
		DO_BINARY_OP(mpz_ior);
	case ZEND_BW_AND:
		DO_BINARY_OP(mpz_and);
	case ZEND_BW_XOR:
		DO_BINARY_OP(mpz_xor);
	case ZEND_BW_NOT:
		DO_UNARY_OP(mpz_com);

	default:
		return FAILURE;
	}
}
/* }}} */

static int gmp_do_operation(zend_uchar opcode, zval *result, zval *op1, zval *op2) /* {{{ */
{
	zval op1_copy;
	int retval;

	if (result == op1) {
		ZVAL_COPY_VALUE(&op1_copy, op1);
		op1 = &op1_copy;
	}

	retval = gmp_do_operation_ex(opcode, result, op1, op2);

	if (retval == SUCCESS && op1 == &op1_copy) {
		zval_dtor(op1);
	}

	return retval;
}
/* }}} */

static int gmp_compare(zval *result, zval *op1, zval *op2) /* {{{ */
{
	gmp_cmp(result, op1, op2);
	if (Z_TYPE_P(result) == IS_FALSE) {
		ZVAL_LONG(result, 1);
	}
	return SUCCESS;
}
/* }}} */

static int gmp_serialize(zval *object, unsigned char **buffer, size_t *buf_len, zend_serialize_data *data) /* {{{ */
{
	mpz_ptr gmpnum = GET_GMP_FROM_ZVAL(object);
	smart_str buf = {0};
	zval zv;
	php_serialize_data_t serialize_data = (php_serialize_data_t) data;
	zend_array tmp_arr;

	PHP_VAR_SERIALIZE_INIT(serialize_data);

	gmp_strval(&zv, gmpnum, 10);
	php_var_serialize(&buf, &zv, &serialize_data);
	zval_dtor(&zv);

	ZVAL_ARR(&zv, &tmp_arr);
	tmp_arr.ht = *zend_std_get_properties(object);
	php_var_serialize(&buf, &zv, &serialize_data);

	PHP_VAR_SERIALIZE_DESTROY(serialize_data);
	*buffer = (unsigned char *) estrndup(buf.s->val, buf.s->len);
	*buf_len = buf.s->len;
	zend_string_release(buf.s);

	return SUCCESS;
}
/* }}} */

static int gmp_unserialize(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data) /* {{{ */
{
	mpz_ptr gmpnum;
	const unsigned char *p, *max;
	zval zv;
	int retval = FAILURE;
	php_unserialize_data_t unserialize_data = (php_unserialize_data_t) data;

	ZVAL_UNDEF(&zv);
	PHP_VAR_UNSERIALIZE_INIT(unserialize_data);
	gmp_create(object, &gmpnum);

	p = buf;
	max = buf + buf_len;

	if (!php_var_unserialize(&zv, &p, max, &unserialize_data)
		|| Z_TYPE(zv) != IS_STRING
		|| convert_to_gmp(gmpnum, &zv, 10) == FAILURE
	) {
		zend_throw_exception(NULL, "Could not unserialize number", 0);
		goto exit;
	}
	zval_dtor(&zv);
	ZVAL_UNDEF(&zv);

	if (!php_var_unserialize(&zv, &p, max, &unserialize_data)
		|| Z_TYPE(zv) != IS_ARRAY
	) {
		zend_throw_exception(NULL, "Could not unserialize properties", 0);
		goto exit;
	}

	if (zend_hash_num_elements(Z_ARRVAL(zv)) != 0) {
		zend_hash_copy(
			zend_std_get_properties(object), Z_ARRVAL(zv),
			(copy_ctor_func_t) zval_add_ref
		);
	}

	retval = SUCCESS;
exit:
	zval_dtor(&zv);
	PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
	return retval;
}
/* }}} */

/* {{{ ZEND_GINIT_FUNCTION
 */
static ZEND_GINIT_FUNCTION(gmp)
{
#if defined(COMPILE_DL_GMP) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE;
#endif
	gmp_globals->rand_initialized = 0;
}
/* }}} */

/* {{{ ZEND_MINIT_FUNCTION
 */
ZEND_MINIT_FUNCTION(gmp)
{
	zend_class_entry tmp_ce;
	INIT_CLASS_ENTRY(tmp_ce, "GMP", NULL);
	gmp_ce = zend_register_internal_class(&tmp_ce);
	gmp_ce->create_object = gmp_create_object;
	gmp_ce->serialize = gmp_serialize;
	gmp_ce->unserialize = gmp_unserialize;

	memcpy(&gmp_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	gmp_object_handlers.offset = XtOffsetOf(gmp_object, std);
	gmp_object_handlers.free_obj = gmp_free_object_storage;
	gmp_object_handlers.cast_object = gmp_cast_object;
	gmp_object_handlers.get_debug_info = gmp_get_debug_info;
	gmp_object_handlers.clone_obj = gmp_clone_obj;
	gmp_object_handlers.do_operation = gmp_do_operation;
	gmp_object_handlers.compare = gmp_compare;

	REGISTER_LONG_CONSTANT("GMP_ROUND_ZERO", GMP_ROUND_ZERO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_ROUND_PLUSINF", GMP_ROUND_PLUSINF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_ROUND_MINUSINF", GMP_ROUND_MINUSINF, CONST_CS | CONST_PERSISTENT);
#ifdef mpir_version
	REGISTER_STRING_CONSTANT("GMP_MPIR_VERSION", (char *)mpir_version, CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_STRING_CONSTANT("GMP_VERSION", (char *)gmp_version, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("GMP_MSW_FIRST", GMP_MSW_FIRST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_LSW_FIRST", GMP_LSW_FIRST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_LITTLE_ENDIAN", GMP_LITTLE_ENDIAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_BIG_ENDIAN", GMP_BIG_ENDIAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_NATIVE_ENDIAN", GMP_NATIVE_ENDIAN, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_RSHUTDOWN_FUNCTION
 */
ZEND_MODULE_DEACTIVATE_D(gmp)
{
	if (GMPG(rand_initialized)) {
		gmp_randclear(GMPG(rand_state));
		GMPG(rand_initialized) = 0;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_MINFO_FUNCTION
 */
ZEND_MODULE_INFO_D(gmp)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "gmp support", "enabled");
#ifdef mpir_version
	php_info_print_table_row(2, "MPIR version", mpir_version);
#else
	php_info_print_table_row(2, "GMP version", gmp_version);
#endif
	php_info_print_table_end();
}
/* }}} */


/* {{{ convert_to_gmp
 * Convert zval to be gmp number */
static int convert_to_gmp(mpz_t gmpnumber, zval *val, zend_long base)
{
	switch (Z_TYPE_P(val)) {
	case IS_LONG:
	case IS_FALSE:
	case IS_TRUE: {
		mpz_set_si(gmpnumber, zval_get_long(val));
		return SUCCESS;
	}
	case IS_STRING: {
		char *numstr = Z_STRVAL_P(val);
		zend_bool skip_lead = 0;
		int ret;

		if (Z_STRLEN_P(val) > 2 && numstr[0] == '0') {
			if ((base == 0 || base == 16) && (numstr[1] == 'x' || numstr[1] == 'X')) {
				base = 16;
				skip_lead = 1;
			} else if ((base == 0 || base == 2) && (numstr[1] == 'b' || numstr[1] == 'B')) {
				base = 2;
				skip_lead = 1;
			}
		}

		ret = mpz_set_str(gmpnumber, (skip_lead ? &numstr[2] : numstr), (int) base);
		if (-1 == ret) {
			php_error_docref(NULL, E_WARNING,
				"Unable to convert variable to GMP - string is not an integer");
			return FAILURE;
		}

		return SUCCESS;
	}
	default:
		php_error_docref(NULL, E_WARNING,
			"Unable to convert variable to GMP - wrong type");
		return FAILURE;
	}
}
/* }}} */

static void gmp_strval(zval *result, mpz_t gmpnum, int base) /* {{{ */
{
	size_t num_len;
	zend_string *str;

	num_len = mpz_sizeinbase(gmpnum, abs(base));
	if (mpz_sgn(gmpnum) < 0) {
		num_len++;
	}

	str = zend_string_alloc(num_len, 0);
	mpz_get_str(str->val, base, gmpnum);

	/*
	 * From GMP documentation for mpz_sizeinbase():
	 * The returned value will be exact or 1 too big.  If base is a power of
	 * 2, the returned value will always be exact.
	 *
	 * So let's check to see if we already have a \0 byte...
	 */

	if (str->val[str->len - 1] == '\0') {
		str->len--;
	} else {
		str->val[str->len] = '\0';
	}

	ZVAL_NEW_STR(result, str);
}
/* }}} */

static void gmp_cmp(zval *return_value, zval *a_arg, zval *b_arg) /* {{{ */
{
	mpz_ptr gmpnum_a, gmpnum_b;
	gmp_temp_t temp_a, temp_b;
	zend_bool use_si = 0;
	zend_long res;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	if (Z_TYPE_P(b_arg) == IS_LONG) {
		use_si = 1;
		temp_b.is_used = 0;
	} else {
		FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a);
	}

	if (use_si) {
		res = mpz_cmp_si(gmpnum_a, Z_LVAL_P(b_arg));
	} else {
		res = mpz_cmp(gmpnum_a, gmpnum_b);
	}

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);

	RETURN_LONG(res);
}
/* }}} */

/* {{{ gmp_zval_binary_ui_op
   Execute GMP binary operation.
*/
static inline void gmp_zval_binary_ui_op(zval *return_value, zval *a_arg, zval *b_arg, gmp_binary_op_t gmp_op, gmp_binary_ui_op_t gmp_ui_op, int check_b_zero)
{
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result;
	int use_ui = 0;
	gmp_temp_t temp_a, temp_b;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	if (gmp_ui_op && Z_TYPE_P(b_arg) == IS_LONG && Z_LVAL_P(b_arg) >= 0) {
		use_ui = 1;
		temp_b.is_used = 0;
	} else {
		FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a);
	}

	if (check_b_zero) {
		int b_is_zero = 0;
		if (use_ui) {
			b_is_zero = (Z_LVAL_P(b_arg) == 0);
		} else {
			b_is_zero = !mpz_cmp_ui(gmpnum_b, 0);
		}

		if (b_is_zero) {
			php_error_docref(NULL, E_WARNING, "Zero operand not allowed");
			FREE_GMP_TEMP(temp_a);
			FREE_GMP_TEMP(temp_b);
			RETURN_FALSE;
		}
	}

	INIT_GMP_RETVAL(gmpnum_result);

	if (use_ui) {
		gmp_ui_op(gmpnum_result, gmpnum_a, (gmp_ulong) Z_LVAL_P(b_arg));
	} else {
		gmp_op(gmpnum_result, gmpnum_a, gmpnum_b);
	}

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ gmp_zval_binary_ui_op2
   Execute GMP binary operation which returns 2 values.
*/
static inline void gmp_zval_binary_ui_op2(zval *return_value, zval *a_arg, zval *b_arg, gmp_binary_op2_t gmp_op, gmp_binary_ui_op2_t gmp_ui_op, int check_b_zero)
{
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result1, gmpnum_result2;
	int use_ui = 0;
	gmp_temp_t temp_a, temp_b;
	zval result1, result2;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	if (gmp_ui_op && Z_TYPE_P(b_arg) == IS_LONG && Z_LVAL_P(b_arg) >= 0) {
		/* use _ui function */
		use_ui = 1;
		temp_b.is_used = 0;
	} else {
		FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a);
	}

	if (check_b_zero) {
		int b_is_zero = 0;
		if (use_ui) {
			b_is_zero = (Z_LVAL_P(b_arg) == 0);
		} else {
			b_is_zero = !mpz_cmp_ui(gmpnum_b, 0);
		}

		if (b_is_zero) {
			php_error_docref(NULL, E_WARNING, "Zero operand not allowed");
			FREE_GMP_TEMP(temp_a);
			FREE_GMP_TEMP(temp_b);
			RETURN_FALSE;
		}
	}

	gmp_create(&result1, &gmpnum_result1);
	gmp_create(&result2, &gmpnum_result2);

	array_init(return_value);
	add_next_index_zval(return_value, &result1);
	add_next_index_zval(return_value, &result2);

	if (use_ui) {
		gmp_ui_op(gmpnum_result1, gmpnum_result2, gmpnum_a, (gmp_ulong) Z_LVAL_P(b_arg));
	} else {
		gmp_op(gmpnum_result1, gmpnum_result2, gmpnum_a, gmpnum_b);
	}

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ _gmp_binary_ui_op
 */
static inline void _gmp_binary_ui_op(INTERNAL_FUNCTION_PARAMETERS, gmp_binary_op_t gmp_op, gmp_binary_ui_op_t gmp_ui_op, int check_b_zero)
{
	zval *a_arg, *b_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		return;
	}

	gmp_zval_binary_ui_op(return_value, a_arg, b_arg, gmp_op, gmp_ui_op, check_b_zero);
}
/* }}} */

/* Unary operations */

/* {{{ gmp_zval_unary_op
 */
static inline void gmp_zval_unary_op(zval *return_value, zval *a_arg, gmp_unary_op_t gmp_op)
{
	mpz_ptr gmpnum_a, gmpnum_result;
	gmp_temp_t temp_a;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	INIT_GMP_RETVAL(gmpnum_result);
	gmp_op(gmpnum_result, gmpnum_a);

	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ gmp_zval_unary_ui_op
 */
static inline void gmp_zval_unary_ui_op(zval *return_value, zval *a_arg, gmp_unary_ui_op_t gmp_op)
{
	mpz_ptr gmpnum_result;

	INIT_GMP_RETVAL(gmpnum_result);
	gmp_op(gmpnum_result, zval_get_long(a_arg));
}
/* }}} */

/* {{{ _gmp_unary_ui_op
   Execute GMP unary operation.
*/
static inline void _gmp_unary_ui_op(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_ui_op_t gmp_op)
{
	zval *a_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		return;
	}

	gmp_zval_unary_ui_op(return_value, a_arg, gmp_op);
}
/* }}} */

/* {{{ _gmp_unary_op
 */
static inline void _gmp_unary_op(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_op_t gmp_op)
{
	zval *a_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		return;
	}

	gmp_zval_unary_op(return_value, a_arg, gmp_op);
}
/* }}} */

/* {{{ _gmp_unary_opl
 */
static inline void _gmp_unary_opl(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_opl_t gmp_op)
{
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	RETVAL_LONG(gmp_op(gmpnum_a));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ _gmp_binary_opl
 */
static inline void _gmp_binary_opl(INTERNAL_FUNCTION_PARAMETERS, gmp_binary_opl_t gmp_op)
{
	zval *a_arg, *b_arg;
	mpz_ptr gmpnum_a, gmpnum_b;
	gmp_temp_t temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a);

	RETVAL_LONG(gmp_op(gmpnum_a, gmpnum_b));

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ proto GMP gmp_init(mixed number [, int base])
   Initializes GMP number */
ZEND_FUNCTION(gmp_init)
{
	zval *number_arg;
	mpz_ptr gmpnumber;
	zend_long base = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|l", &number_arg, &base) == FAILURE) {
		return;
	}

	if (base && (base < 2 || base > GMP_MAX_BASE)) {
		php_error_docref(NULL, E_WARNING, "Bad base for conversion: %pd (should be between 2 and %d)", base, GMP_MAX_BASE);
		RETURN_FALSE;
	}

	INIT_GMP_RETVAL(gmpnumber);
	if (convert_to_gmp(gmpnumber, number_arg, base) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

int gmp_import_export_validate(zend_long size, zend_long options, int *order, int *endian)
{
	if (size < 1) {
		php_error_docref(NULL, E_WARNING,
			"Word size must be positive, %pd given", size);
		return FAILURE;
	}

	switch (options & (GMP_LSW_FIRST | GMP_MSW_FIRST)) {
		case GMP_LSW_FIRST:
			*order = -1;
			break;
		case GMP_MSW_FIRST:
		case 0: /* default */
			*order = 1;
			break;
		default:
			php_error_docref(NULL, E_WARNING,
				"Invalid options: Conflicting word orders");
			return FAILURE;
	}

	switch (options & (GMP_LITTLE_ENDIAN | GMP_BIG_ENDIAN | GMP_NATIVE_ENDIAN)) {
		case GMP_LITTLE_ENDIAN:
			*endian = -1;
			break;
		case GMP_BIG_ENDIAN:
			*endian = 1;
			break;
		case GMP_NATIVE_ENDIAN:
		case 0: /* default */
			*endian = 0;
			break;
		default:
			php_error_docref(NULL, E_WARNING,
				"Invalid options: Conflicting word endianness");
			return FAILURE;
	}

	return SUCCESS;
}

/* {{{ proto GMP gmp_import(string data [, int word_size = 1, int options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN])
   Imports a GMP number from a binary string */
ZEND_FUNCTION(gmp_import)
{
	char *data;
	size_t data_len;
	zend_long size = 1;
	zend_long options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN;
	int order, endian;
	mpz_ptr gmpnumber;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|ll", &data, &data_len, &size, &options) == FAILURE) {
		return;
	}

	if (gmp_import_export_validate(size, options, &order, &endian) == FAILURE) {
		RETURN_FALSE;
	}

	if ((data_len % size) != 0) {
		php_error_docref(NULL, E_WARNING,
			"Input length must be a multiple of word size");
		RETURN_FALSE;
	}

	INIT_GMP_RETVAL(gmpnumber);

	mpz_import(gmpnumber, data_len / size, order, size, endian, 0, data);
}
/* }}} */

/* {{{ proto string gmp_export(GMP gmpnumber [, int word_size = 1, int options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN])
   Exports a GMP number to a binary string */
ZEND_FUNCTION(gmp_export)
{
	zval *gmpnumber_arg;
	zend_long size = 1;
	zend_long options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN;
	int order, endian;
	mpz_ptr gmpnumber;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|ll", &gmpnumber_arg, &size, &options) == FAILURE) {
		return;
	}

	if (gmp_import_export_validate(size, options, &order, &endian) == FAILURE) {
		RETURN_FALSE;
	}

	FETCH_GMP_ZVAL(gmpnumber, gmpnumber_arg, temp_a);

	if (mpz_sgn(gmpnumber) == 0) {
		RETURN_EMPTY_STRING();
	} else {
		size_t bits_per_word = size * 8;
		size_t count = (mpz_sizeinbase(gmpnumber, 2) + bits_per_word - 1) / bits_per_word;
		size_t out_len = count * size;

		zend_string *out_string = zend_string_alloc(out_len, 0);
		mpz_export(out_string->val, NULL, order, size, endian, 0, gmpnumber);
		out_string->val[out_len] = '\0';

		RETURN_STR(out_string);
	}

	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto int gmp_intval(mixed gmpnumber)
   Gets signed long value of GMP number */
ZEND_FUNCTION(gmp_intval)
{
	zval *gmpnumber_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &gmpnumber_arg) == FAILURE){
		return;
	}

	if (IS_GMP(gmpnumber_arg)) {
		RETVAL_LONG(mpz_get_si(GET_GMP_FROM_ZVAL(gmpnumber_arg)));
	} else {
		RETVAL_LONG(zval_get_long(gmpnumber_arg));
	}
}
/* }}} */

/* {{{ proto string gmp_strval(mixed gmpnumber [, int base])
   Gets string representation of GMP number  */
ZEND_FUNCTION(gmp_strval)
{
	zval *gmpnumber_arg;
	zend_long base = 10;
	mpz_ptr gmpnum;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|l", &gmpnumber_arg, &base) == FAILURE) {
		return;
	}

	/* Although the maximum base in general in GMP is 62, mpz_get_str()
	 * is explicitly limited to -36 when dealing with negative bases. */
	if ((base < 2 && base > -2) || base > GMP_MAX_BASE || base < -36) {
		php_error_docref(NULL, E_WARNING, "Bad base for conversion: %pd (should be between 2 and %d or -2 and -36)", base, GMP_MAX_BASE);
		RETURN_FALSE;
	}

	FETCH_GMP_ZVAL(gmpnum, gmpnumber_arg, temp_a);

	gmp_strval(return_value, gmpnum, (int)base);

	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto GMP gmp_add(mixed a, mixed b)
   Add a and b */
ZEND_FUNCTION(gmp_add)
{
	gmp_binary_ui_op(mpz_add, mpz_add_ui);
}
/* }}} */

/* {{{ proto GMP gmp_sub(mixed a, mixed b)
   Subtract b from a */
ZEND_FUNCTION(gmp_sub)
{
	gmp_binary_ui_op(mpz_sub, mpz_sub_ui);
}
/* }}} */

/* {{{ proto GMP gmp_mul(mixed a, mixed b)
   Multiply a and b */
ZEND_FUNCTION(gmp_mul)
{
	gmp_binary_ui_op(mpz_mul, mpz_mul_ui);
}
/* }}} */

/* {{{ proto array gmp_div_qr(mixed a, mixed b [, int round])
   Divide a by b, returns quotient and reminder */
ZEND_FUNCTION(gmp_div_qr)
{
	zval *a_arg, *b_arg;
	zend_long round = GMP_ROUND_ZERO;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz|l", &a_arg, &b_arg, &round) == FAILURE) {
		return;
	}

	switch (round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op2(return_value, a_arg, b_arg, mpz_tdiv_qr, (gmp_binary_ui_op2_t) mpz_tdiv_qr_ui, 1);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op2(return_value, a_arg, b_arg, mpz_cdiv_qr, (gmp_binary_ui_op2_t) mpz_cdiv_qr_ui, 1);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op2(return_value, a_arg, b_arg, mpz_fdiv_qr, (gmp_binary_ui_op2_t) mpz_fdiv_qr_ui, 1);
		break;
	default:
		php_error_docref(NULL, E_WARNING, "Invalid rounding mode");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto GMP gmp_div_r(mixed a, mixed b [, int round])
   Divide a by b, returns reminder only */
ZEND_FUNCTION(gmp_div_r)
{
	zval *a_arg, *b_arg;
	zend_long round = GMP_ROUND_ZERO;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz|l", &a_arg, &b_arg, &round) == FAILURE) {
		return;
	}

	switch (round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op(return_value, a_arg, b_arg, mpz_tdiv_r, (gmp_binary_ui_op_t) mpz_tdiv_r_ui, 1);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op(return_value, a_arg, b_arg, mpz_cdiv_r, (gmp_binary_ui_op_t) mpz_cdiv_r_ui, 1);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op(return_value, a_arg, b_arg, mpz_fdiv_r, (gmp_binary_ui_op_t) mpz_fdiv_r_ui, 1);
		break;
	default:
		php_error_docref(NULL, E_WARNING, "Invalid rounding mode");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto GMP gmp_div_q(mixed a, mixed b [, int round])
   Divide a by b, returns quotient only */
ZEND_FUNCTION(gmp_div_q)
{
	zval *a_arg, *b_arg;
	zend_long round = GMP_ROUND_ZERO;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz|l", &a_arg, &b_arg, &round) == FAILURE) {
		return;
	}

	switch (round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op(return_value, a_arg, b_arg, mpz_tdiv_q, (gmp_binary_ui_op_t) mpz_tdiv_q_ui, 1);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op(return_value, a_arg, b_arg, mpz_cdiv_q, (gmp_binary_ui_op_t) mpz_cdiv_q_ui, 1);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op(return_value, a_arg, b_arg, mpz_fdiv_q, (gmp_binary_ui_op_t) mpz_fdiv_q_ui, 1);
		break;
	default:
		php_error_docref(NULL, E_WARNING, "Invalid rounding mode");
		RETURN_FALSE;
	}

}
/* }}} */

/* {{{ proto GMP gmp_mod(mixed a, mixed b)
   Computes a modulo b */
ZEND_FUNCTION(gmp_mod)
{
	gmp_binary_ui_op_no_zero(mpz_mod, (gmp_binary_ui_op_t) mpz_mod_ui);
}
/* }}} */

/* {{{ proto GMP gmp_divexact(mixed a, mixed b)
   Divide a by b using exact division algorithm */
ZEND_FUNCTION(gmp_divexact)
{
	gmp_binary_ui_op_no_zero(mpz_divexact, NULL);
}
/* }}} */

/* {{{ proto GMP gmp_neg(mixed a)
   Negates a number */
ZEND_FUNCTION(gmp_neg)
{
	gmp_unary_op(mpz_neg);
}
/* }}} */

/* {{{ proto GMP gmp_abs(mixed a)
   Calculates absolute value */
ZEND_FUNCTION(gmp_abs)
{
	gmp_unary_op(mpz_abs);
}
/* }}} */

/* {{{ proto GMP gmp_fact(int a)
   Calculates factorial function */
ZEND_FUNCTION(gmp_fact)
{
	zval *a_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		return;
	}

	if (IS_GMP(a_arg)) {
		mpz_ptr gmpnum_tmp = GET_GMP_FROM_ZVAL(a_arg);
		if (mpz_sgn(gmpnum_tmp) < 0) {
			php_error_docref(NULL, E_WARNING, "Number has to be greater than or equal to 0");
			RETURN_FALSE;
		}
	} else {
		if (zval_get_long(a_arg) < 0) {
			php_error_docref(NULL, E_WARNING, "Number has to be greater than or equal to 0");
			RETURN_FALSE;
		}
	}

	gmp_zval_unary_ui_op(return_value, a_arg, mpz_fac_ui);
}
/* }}} */

/* {{{ proto GMP gmp_pow(mixed base, int exp)
   Raise base to power exp */
ZEND_FUNCTION(gmp_pow)
{
	zval *base_arg;
	mpz_ptr gmpnum_result, gmpnum_base;
	gmp_temp_t temp_base;
	zend_long exp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &base_arg, &exp) == FAILURE) {
		return;
	}

	if (exp < 0) {
		php_error_docref(NULL, E_WARNING, "Negative exponent not supported");
		RETURN_FALSE;
	}

	INIT_GMP_RETVAL(gmpnum_result);
	if (Z_TYPE_P(base_arg) == IS_LONG && Z_LVAL_P(base_arg) >= 0) {
		mpz_ui_pow_ui(gmpnum_result, Z_LVAL_P(base_arg), exp);
	} else {
		FETCH_GMP_ZVAL(gmpnum_base, base_arg, temp_base);
		mpz_pow_ui(gmpnum_result, gmpnum_base, exp);
		FREE_GMP_TEMP(temp_base);
	}
}
/* }}} */

/* {{{ proto GMP gmp_powm(mixed base, mixed exp, mixed mod)
   Raise base to power exp and take result modulo mod */
ZEND_FUNCTION(gmp_powm)
{
	zval *base_arg, *exp_arg, *mod_arg;
	mpz_ptr gmpnum_base, gmpnum_exp, gmpnum_mod, gmpnum_result;
	int use_ui = 0;
	gmp_temp_t temp_base, temp_exp, temp_mod;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zzz", &base_arg, &exp_arg, &mod_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_base, base_arg, temp_base);

	if (Z_TYPE_P(exp_arg) == IS_LONG && Z_LVAL_P(exp_arg) >= 0) {
		use_ui = 1;
		temp_exp.is_used = 0;
	} else {
		FETCH_GMP_ZVAL_DEP(gmpnum_exp, exp_arg, temp_exp, temp_base);
		if (mpz_sgn(gmpnum_exp) < 0) {
			php_error_docref(NULL, E_WARNING, "Second parameter cannot be less than 0");
			FREE_GMP_TEMP(temp_base);
			FREE_GMP_TEMP(temp_exp);
			RETURN_FALSE;
		}
	}
	FETCH_GMP_ZVAL_DEP_DEP(gmpnum_mod, mod_arg, temp_mod, temp_exp, temp_base);

	if (!mpz_cmp_ui(gmpnum_mod, 0)) {
		php_error_docref(NULL, E_WARNING, "Modulus may not be zero");
		FREE_GMP_TEMP(temp_base);
		FREE_GMP_TEMP(temp_exp);
		FREE_GMP_TEMP(temp_mod);
		RETURN_FALSE;
	}

	INIT_GMP_RETVAL(gmpnum_result);
	if (use_ui) {
		mpz_powm_ui(gmpnum_result, gmpnum_base, (zend_ulong) Z_LVAL_P(exp_arg), gmpnum_mod);
	} else {
		mpz_powm(gmpnum_result, gmpnum_base, gmpnum_exp, gmpnum_mod);
		FREE_GMP_TEMP(temp_exp);
	}

	FREE_GMP_TEMP(temp_base);
	FREE_GMP_TEMP(temp_mod);
}
/* }}} */

/* {{{ proto GMP gmp_sqrt(mixed a)
   Takes integer part of square root of a */
ZEND_FUNCTION(gmp_sqrt)
{
	zval *a_arg;
	mpz_ptr gmpnum_a, gmpnum_result;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	if (mpz_sgn(gmpnum_a) < 0) {
		php_error_docref(NULL, E_WARNING, "Number has to be greater than or equal to 0");
		FREE_GMP_TEMP(temp_a);
		RETURN_FALSE;
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_sqrt(gmpnum_result, gmpnum_a);
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto array gmp_sqrtrem(mixed a)
   Square root with remainder */
ZEND_FUNCTION(gmp_sqrtrem)
{
	zval *a_arg;
	mpz_ptr gmpnum_a, gmpnum_result1, gmpnum_result2;
	gmp_temp_t temp_a;
	zval result1, result2;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	if (mpz_sgn(gmpnum_a) < 0) {
		php_error_docref(NULL, E_WARNING, "Number has to be greater than or equal to 0");
		FREE_GMP_TEMP(temp_a);
		RETURN_FALSE;
	}

	gmp_create(&result1, &gmpnum_result1);
	gmp_create(&result2, &gmpnum_result2);

	array_init(return_value);
	add_next_index_zval(return_value, &result1);
	add_next_index_zval(return_value, &result2);

	mpz_sqrtrem(gmpnum_result1, gmpnum_result2, gmpnum_a);
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto GMP gmp_root(mixed a, int nth)
   Takes integer part of nth root */
ZEND_FUNCTION(gmp_root)
{
	zval *a_arg;
	zend_long nth;
	mpz_ptr gmpnum_a, gmpnum_result;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &a_arg, &nth) == FAILURE) {
		return;
	}

	if (nth <= 0) {
		php_error_docref(NULL, E_WARNING, "The root must be positive");
		RETURN_FALSE;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	if (nth % 2 == 0 && mpz_sgn(gmpnum_a) < 0) {
		php_error_docref(NULL, E_WARNING, "Can't take even root of negative number");
		FREE_GMP_TEMP(temp_a);
		RETURN_FALSE;
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_root(gmpnum_result, gmpnum_a, (gmp_ulong) nth);
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto GMP gmp_rootrem(mixed a, int nth)
   Calculates integer part of nth root and remainder */
ZEND_FUNCTION(gmp_rootrem)
{
	zval *a_arg;
	zend_long nth;
	mpz_ptr gmpnum_a, gmpnum_result1, gmpnum_result2;
	gmp_temp_t temp_a;
	zval result1, result2;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &a_arg, &nth) == FAILURE) {
		return;
	}

	if (nth <= 0) {
		php_error_docref(NULL, E_WARNING, "The root must be positive");
		RETURN_FALSE;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	if (nth % 2 == 0 && mpz_sgn(gmpnum_a) < 0) {
		php_error_docref(NULL, E_WARNING, "Can't take even root of negative number");
		FREE_GMP_TEMP(temp_a);
		RETURN_FALSE;
	}

	gmp_create(&result1, &gmpnum_result1);
	gmp_create(&result2, &gmpnum_result2);

	array_init(return_value);
	add_next_index_zval(return_value, &result1);
	add_next_index_zval(return_value, &result2);

	mpz_rootrem(gmpnum_result1, gmpnum_result2, gmpnum_a, (gmp_ulong) nth);

	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto bool gmp_perfect_square(mixed a)
   Checks if a is an exact square */
ZEND_FUNCTION(gmp_perfect_square)
{
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	RETVAL_BOOL((mpz_perfect_square_p(gmpnum_a) != 0));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto int gmp_prob_prime(mixed a[, int reps])
   Checks if a is "probably prime" */
ZEND_FUNCTION(gmp_prob_prime)
{
	zval *gmpnumber_arg;
	mpz_ptr gmpnum_a;
	zend_long reps = 10;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|l", &gmpnumber_arg, &reps) == FAILURE) {
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, gmpnumber_arg, temp_a);

	RETVAL_LONG(mpz_probab_prime_p(gmpnum_a, reps));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto GMP gmp_gcd(mixed a, mixed b)
   Computes greatest common denominator (gcd) of a and b */
ZEND_FUNCTION(gmp_gcd)
{
	gmp_binary_ui_op(mpz_gcd, (gmp_binary_ui_op_t) mpz_gcd_ui);
}
/* }}} */

/* {{{ proto array gmp_gcdext(mixed a, mixed b)
   Computes G, S, and T, such that AS + BT = G = `gcd' (A, B) */
ZEND_FUNCTION(gmp_gcdext)
{
	zval *a_arg, *b_arg;
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_t, gmpnum_s, gmpnum_g;
	gmp_temp_t temp_a, temp_b;
	zval result_g, result_s, result_t;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a);

	gmp_create(&result_g, &gmpnum_g);
	gmp_create(&result_s, &gmpnum_s);
	gmp_create(&result_t, &gmpnum_t);

	array_init(return_value);
	add_assoc_zval(return_value, "g", &result_g);
	add_assoc_zval(return_value, "s", &result_s);
	add_assoc_zval(return_value, "t", &result_t);

	mpz_gcdext(gmpnum_g, gmpnum_s, gmpnum_t, gmpnum_a, gmpnum_b);
	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ proto GMP gmp_invert(mixed a, mixed b)
   Computes the inverse of a modulo b */
ZEND_FUNCTION(gmp_invert)
{
	zval *a_arg, *b_arg;
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result;
	gmp_temp_t temp_a, temp_b;
	int res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a);

	INIT_GMP_RETVAL(gmpnum_result);
	res = mpz_invert(gmpnum_result, gmpnum_a, gmpnum_b);
	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
	if (!res) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int gmp_jacobi(mixed a, mixed b)
   Computes Jacobi symbol */
ZEND_FUNCTION(gmp_jacobi)
{
	gmp_binary_opl(mpz_jacobi);
}
/* }}} */

/* {{{ proto int gmp_legendre(mixed a, mixed b)
   Computes Legendre symbol */
ZEND_FUNCTION(gmp_legendre)
{
	gmp_binary_opl(mpz_legendre);
}
/* }}} */

/* {{{ proto int gmp_cmp(mixed a, mixed b)
   Compares two numbers */
ZEND_FUNCTION(gmp_cmp)
{
	zval *a_arg, *b_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		return;
	}

	gmp_cmp(return_value, a_arg, b_arg);
}
/* }}} */

/* {{{ proto int gmp_sign(mixed a)
   Gets the sign of the number */
ZEND_FUNCTION(gmp_sign)
{
	/* Can't use gmp_unary_opl here, because mpz_sgn is a macro */
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	RETVAL_LONG(mpz_sgn(gmpnum_a));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

static void gmp_init_random(void)
{
	if (!GMPG(rand_initialized)) {
		/* Initialize */
		gmp_randinit_mt(GMPG(rand_state));
		/* Seed */
		gmp_randseed_ui(GMPG(rand_state), GENERATE_SEED());

		GMPG(rand_initialized) = 1;
	}
}

/* {{{ proto GMP gmp_random([int limiter])
   Gets random number */
ZEND_FUNCTION(gmp_random)
{
	zend_long limiter = 20;
	mpz_ptr gmpnum_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &limiter) == FAILURE) {
		return;
	}

	INIT_GMP_RETVAL(gmpnum_result);
	gmp_init_random();

#ifdef GMP_LIMB_BITS
	mpz_urandomb(gmpnum_result, GMPG(rand_state), GMP_ABS (limiter) * GMP_LIMB_BITS);
#else
	mpz_urandomb(gmpnum_result, GMPG(rand_state), GMP_ABS (limiter) * __GMP_BITS_PER_MP_LIMB);
#endif
}
/* }}} */

/* {{{ proto GMP gmp_random_seed(mixed seed)
   Seed the RNG */
ZEND_FUNCTION(gmp_random_seed)
{
	zval *seed;
	mpz_ptr gmpnum_seed;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &seed) == FAILURE) {
		return;
	}

	gmp_init_random();

	if (Z_TYPE_P(seed) == IS_LONG && Z_LVAL_P(seed) >= 0) {
		gmp_randseed_ui(GMPG(rand_state), Z_LVAL_P(seed));
	}
	else {
		FETCH_GMP_ZVAL(gmpnum_seed, seed, temp_a);

		gmp_randseed(GMPG(rand_state), gmpnum_seed);

		FREE_GMP_TEMP(temp_a);
	}
}
/* }}} */

/* {{{ proto GMP gmp_random_bits(int bits)
   Gets a random number in the range 0 to (2 ** n) - 1 */
ZEND_FUNCTION(gmp_random_bits)
{
	zend_long bits;
	mpz_ptr gmpnum_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &bits) == FAILURE) {
		return;
	}

	if (bits <= 0) {
		php_error_docref(NULL, E_WARNING, "The number of bits must be positive");
		RETURN_FALSE;
	}

	INIT_GMP_RETVAL(gmpnum_result);
	gmp_init_random();

	mpz_urandomb(gmpnum_result, GMPG(rand_state), bits);
}
/* }}} */

/* {{{ proto GMP gmp_random_range(mixed min, mixed max)
   Gets a random number in the range min to max */
ZEND_FUNCTION(gmp_random_range)
{
	zval *min_arg, *max_arg;
	mpz_ptr gmpnum_min, gmpnum_max, gmpnum_result;
	gmp_temp_t temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &min_arg, &max_arg) == FAILURE) {
		return;
	}

	gmp_init_random();

	FETCH_GMP_ZVAL(gmpnum_max, max_arg, temp_a);

	if (Z_TYPE_P(min_arg) == IS_LONG && Z_LVAL_P(min_arg) >= 0) {
		if (mpz_cmp_ui(gmpnum_max, Z_LVAL_P(min_arg)) <= 0) {
			FREE_GMP_TEMP(temp_a);
			php_error_docref(NULL, E_WARNING, "The minimum value must be less than the maximum value");
			RETURN_FALSE;
		}

		INIT_GMP_RETVAL(gmpnum_result);

		if (Z_LVAL_P(min_arg)) {
			mpz_sub_ui(gmpnum_max, gmpnum_max, Z_LVAL_P(min_arg));
		}

		mpz_add_ui(gmpnum_max, gmpnum_max, 1);
		mpz_urandomm(gmpnum_result, GMPG(rand_state), gmpnum_max);

		if (Z_LVAL_P(min_arg)) {
			mpz_add_ui(gmpnum_result, gmpnum_result, Z_LVAL_P(min_arg));
		}

		FREE_GMP_TEMP(temp_a);

	}
	else {
		FETCH_GMP_ZVAL_DEP(gmpnum_min, min_arg, temp_b, temp_a);

		if (mpz_cmp(gmpnum_max, gmpnum_min) <= 0) {
			FREE_GMP_TEMP(temp_b);
			FREE_GMP_TEMP(temp_a);
			php_error_docref(NULL, E_WARNING, "The minimum value must be less than the maximum value");
			RETURN_FALSE;
		}

		INIT_GMP_RETVAL(gmpnum_result);

		mpz_sub(gmpnum_max, gmpnum_max, gmpnum_min);
		mpz_add_ui(gmpnum_max, gmpnum_max, 1);
		mpz_urandomm(gmpnum_result, GMPG(rand_state), gmpnum_max);
		mpz_add(gmpnum_result, gmpnum_result, gmpnum_min);

		FREE_GMP_TEMP(temp_b);
		FREE_GMP_TEMP(temp_a);
	}
}
/* }}} */

/* {{{ proto GMP gmp_and(mixed a, mixed b)
   Calculates logical AND of a and b */
ZEND_FUNCTION(gmp_and)
{
	gmp_binary_op(mpz_and);
}
/* }}} */

/* {{{ proto GMP gmp_or(mixed a, mixed b)
   Calculates logical OR of a and b */
ZEND_FUNCTION(gmp_or)
{
	gmp_binary_op(mpz_ior);
}
/* }}} */

/* {{{ proto GMP gmp_com(mixed a)
   Calculates one's complement of a */
ZEND_FUNCTION(gmp_com)
{
	gmp_unary_op(mpz_com);
}
/* }}} */

/* {{{ proto GMP gmp_nextprime(mixed a)
   Finds next prime of a */
ZEND_FUNCTION(gmp_nextprime)
{
   gmp_unary_op(mpz_nextprime);
}
/* }}} */

/* {{{ proto GMP gmp_xor(mixed a, mixed b)
   Calculates logical exclusive OR of a and b */
ZEND_FUNCTION(gmp_xor)
{
	gmp_binary_op(mpz_xor);
}
/* }}} */

/* {{{ proto void gmp_setbit(GMP a, int index[, bool set_clear])
   Sets or clear bit in a */
ZEND_FUNCTION(gmp_setbit)
{
	zval *a_arg;
	zend_long index;
	zend_bool set = 1;
	mpz_ptr gmpnum_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol|b", &a_arg, gmp_ce, &index, &set) == FAILURE) {
		return;
	}

	if (index < 0) {
		php_error_docref(NULL, E_WARNING, "Index must be greater than or equal to zero");
		RETURN_FALSE;
	}

	gmpnum_a = GET_GMP_FROM_ZVAL(a_arg);

	if (set) {
		mpz_setbit(gmpnum_a, index);
	} else {
		mpz_clrbit(gmpnum_a, index);
	}
}
/* }}} */

/* {{{ proto void gmp_clrbit(GMP a, int index)
   Clears bit in a */
ZEND_FUNCTION(gmp_clrbit)
{
	zval *a_arg;
	zend_long index;
	mpz_ptr gmpnum_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &a_arg, gmp_ce, &index) == FAILURE){
		return;
	}

	if (index < 0) {
		php_error_docref(NULL, E_WARNING, "Index must be greater than or equal to zero");
		RETURN_FALSE;
	}

	gmpnum_a = GET_GMP_FROM_ZVAL(a_arg);
	mpz_clrbit(gmpnum_a, index);
}
/* }}} */

/* {{{ proto bool gmp_testbit(mixed a, int index)
   Tests if bit is set in a */
ZEND_FUNCTION(gmp_testbit)
{
	zval *a_arg;
	zend_long index;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &a_arg, &index) == FAILURE){
		return;
	}

	if (index < 0) {
		php_error_docref(NULL, E_WARNING, "Index must be greater than or equal to zero");
		RETURN_FALSE;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	RETVAL_BOOL(mpz_tstbit(gmpnum_a, index));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto int gmp_popcount(mixed a)
   Calculates the population count of a */
ZEND_FUNCTION(gmp_popcount)
{
	gmp_unary_opl((gmp_unary_opl_t) mpz_popcount);
}
/* }}} */

/* {{{ proto int gmp_hamdist(mixed a, mixed b)
   Calculates hamming distance between a and b */
ZEND_FUNCTION(gmp_hamdist)
{
	gmp_binary_opl((gmp_binary_opl_t) mpz_hamdist);
}
/* }}} */

/* {{{ proto int gmp_scan0(mixed a, int start)
   Finds first zero bit */
ZEND_FUNCTION(gmp_scan0)
{
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;
	zend_long start;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &a_arg, &start) == FAILURE){
		return;
	}

	if (start < 0) {
		php_error_docref(NULL, E_WARNING, "Starting index must be greater than or equal to zero");
		RETURN_FALSE;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	RETVAL_LONG(mpz_scan0(gmpnum_a, start));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto int gmp_scan1(mixed a, int start)
   Finds first non-zero bit */
ZEND_FUNCTION(gmp_scan1)
{
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;
	zend_long start;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &a_arg, &start) == FAILURE){
		return;
	}

	if (start < 0) {
		php_error_docref(NULL, E_WARNING, "Starting index must be greater than or equal to zero");
		RETURN_FALSE;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	RETVAL_LONG(mpz_scan1(gmpnum_a, start));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

#endif	/* HAVE_GMP */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
