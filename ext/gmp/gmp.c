/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
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

#if HAVE_GMP

#include <gmp.h>

/* Needed for gmp_random() */
#include "ext/standard/php_rand.h"
#include "ext/standard/php_lcg.h"
#define GMP_ABS(x) ((x) >= 0 ? (x) : -(x))

/* True global resources - no need for thread safety here */
static int le_gmp;

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_init, 0, 0, 1)
	ZEND_ARG_INFO(0, number)
	ZEND_ARG_INFO(0, base)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_intval, 0)
	ZEND_ARG_INFO(0, gmpnumber)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_strval, 0, 0, 1)
	ZEND_ARG_INFO(0, gmpnumber)
	ZEND_ARG_INFO(0, base)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_add, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_sub, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_mul, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_div_qr, 0, 0, 2)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
	ZEND_ARG_INFO(0, round)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_div_r, 0, 0, 2)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
	ZEND_ARG_INFO(0, round)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_div_q, 0, 0, 2)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
	ZEND_ARG_INFO(0, round)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_mod, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_divexact, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_neg, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_abs, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_fact, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_pow, 0)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_INFO(0, exp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_powm, 0)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_INFO(0, exp)
	ZEND_ARG_INFO(0, mod)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_sqrt, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_sqrtrem, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_perfect_square, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_prob_prime, 0, 0, 1)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, reps)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_gcd, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_gcdext, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_invert, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_jacobi, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_legendre, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_cmp, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_sign, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_random, 0, 0, 0)
	ZEND_ARG_INFO(0, limiter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_and, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_or, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_com, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_xor, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gmp_setbit, 0, 0, 2)
	ZEND_ARG_INFO(1, a)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, set_clear)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_clrbit, 0)
	ZEND_ARG_INFO(1, a)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_testbit, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_popcount, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_hamdist, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_scan0, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, start)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_scan1, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, start)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gmp_nextprime, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(gmp)
static ZEND_GINIT_FUNCTION(gmp);

/* {{{ gmp_functions[]
 */
const zend_function_entry gmp_functions[] = {
	ZEND_FE(gmp_init,	arginfo_gmp_init)
	ZEND_FE(gmp_intval,	arginfo_gmp_intval)
	ZEND_FE(gmp_strval,	arginfo_gmp_strval)
	ZEND_FE(gmp_add,	    arginfo_gmp_add)
	ZEND_FE(gmp_sub,	    arginfo_gmp_sub)
	ZEND_FE(gmp_mul,	    arginfo_gmp_mul)
	ZEND_FE(gmp_div_qr,	arginfo_gmp_div_qr)
	ZEND_FE(gmp_div_q,	arginfo_gmp_div_q)
	ZEND_FE(gmp_div_r,	arginfo_gmp_div_r)
	ZEND_FALIAS(gmp_div,	gmp_div_q, arginfo_gmp_div_q)
	ZEND_FE(gmp_mod,	arginfo_gmp_mod)
	ZEND_FE(gmp_divexact,	arginfo_gmp_divexact)
	ZEND_FE(gmp_neg,	arginfo_gmp_neg)
	ZEND_FE(gmp_abs,	arginfo_gmp_abs)
	ZEND_FE(gmp_fact,	arginfo_gmp_fact)
	ZEND_FE(gmp_sqrt,	arginfo_gmp_sqrt)
	ZEND_FE(gmp_sqrtrem,	arginfo_gmp_sqrtrem)
	ZEND_FE(gmp_pow,	arginfo_gmp_pow)
	ZEND_FE(gmp_powm,	arginfo_gmp_powm)
	ZEND_FE(gmp_perfect_square,	arginfo_gmp_perfect_square)
	ZEND_FE(gmp_prob_prime,	arginfo_gmp_prob_prime)
	ZEND_FE(gmp_gcd,	arginfo_gmp_gcd)
	ZEND_FE(gmp_gcdext,	arginfo_gmp_gcdext)
	ZEND_FE(gmp_invert,	arginfo_gmp_invert)
	ZEND_FE(gmp_jacobi,	arginfo_gmp_jacobi)
	ZEND_FE(gmp_legendre,	arginfo_gmp_legendre)
	ZEND_FE(gmp_cmp,	arginfo_gmp_cmp)
	ZEND_FE(gmp_sign,	arginfo_gmp_sign)
	ZEND_FE(gmp_random,	arginfo_gmp_random)
	ZEND_FE(gmp_and,	arginfo_gmp_and)
	ZEND_FE(gmp_or,	arginfo_gmp_or)
	ZEND_FE(gmp_com,	arginfo_gmp_com)
	ZEND_FE(gmp_xor,	arginfo_gmp_xor)
	ZEND_FE(gmp_setbit,	arginfo_gmp_setbit)
	ZEND_FE(gmp_clrbit,	arginfo_gmp_clrbit)
	ZEND_FE(gmp_scan0,  arginfo_gmp_scan0)
	ZEND_FE(gmp_scan1,  arginfo_gmp_scan1)
	ZEND_FE(gmp_testbit,arginfo_gmp_testbit)
	ZEND_FE(gmp_popcount, arginfo_gmp_popcount)
	ZEND_FE(gmp_hamdist, arginfo_gmp_hamdist)
	ZEND_FE(gmp_nextprime, arginfo_gmp_nextprime)
	{NULL, NULL, NULL}	/* Must be the last line in gmp_functions[] */
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
ZEND_GET_MODULE(gmp)
#endif

static void _php_gmpnum_free(zend_rsrc_list_entry *rsrc TSRMLS_DC);

#define GMP_RESOURCE_NAME "GMP integer"

#define GMP_ROUND_ZERO      0
#define GMP_ROUND_PLUSINF   1
#define GMP_ROUND_MINUSINF  2

/* {{{ gmp_emalloc
 */
static void *gmp_emalloc(size_t size)
{
	return emalloc(size);
}
/* }}} */

/* {{{ gmp_erealloc
 */
static void *gmp_erealloc(void *ptr, size_t old_size, size_t new_size)
{
	return erealloc(ptr, new_size);
}
/* }}} */

/* {{{ gmp_efree
 */
static void gmp_efree(void *ptr, size_t size)
{
	efree(ptr);
}
/* }}} */

/* {{{ ZEND_GINIT_FUNCTION
 */
static ZEND_GINIT_FUNCTION(gmp)
{
	gmp_globals->rand_initialized = 0;
}
/* }}} */

/* {{{ ZEND_MINIT_FUNCTION
 */
ZEND_MODULE_STARTUP_D(gmp)
{
	le_gmp = zend_register_list_destructors_ex(_php_gmpnum_free, NULL, GMP_RESOURCE_NAME, module_number);
	REGISTER_LONG_CONSTANT("GMP_ROUND_ZERO", GMP_ROUND_ZERO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_ROUND_PLUSINF", GMP_ROUND_PLUSINF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_ROUND_MINUSINF", GMP_ROUND_MINUSINF, CONST_CS | CONST_PERSISTENT);
#ifdef mpir_version
	REGISTER_STRING_CONSTANT("GMP_MPIR_VERSION", (char *)mpir_version, CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_STRING_CONSTANT("GMP_VERSION", (char *)gmp_version, CONST_CS | CONST_PERSISTENT);

	mp_set_memory_functions(gmp_emalloc, gmp_erealloc, gmp_efree);

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

/* Fetch zval to be GMP number.
   Initially, zval can be also number or string */
#define FETCH_GMP_ZVAL(gmpnumber, zval, tmp_resource)								\
if (Z_TYPE_PP(zval) == IS_RESOURCE) {												\
	ZEND_FETCH_RESOURCE(gmpnumber, mpz_t *, zval, -1, GMP_RESOURCE_NAME, le_gmp);	\
	tmp_resource = 0;																\
} else {																			\
	if (convert_to_gmp(&gmpnumber, zval, 0 TSRMLS_CC) == FAILURE) {					\
		RETURN_FALSE;																\
	}																				\
	tmp_resource = ZEND_REGISTER_RESOURCE(NULL, gmpnumber, le_gmp);					\
}

#define FREE_GMP_TEMP(tmp_resource)			\
	if(tmp_resource) {						\
		zend_list_delete(tmp_resource);		\
	}


/* create a new initialized GMP number */
#define INIT_GMP_NUM(gmpnumber) { gmpnumber=emalloc(sizeof(mpz_t)); mpz_init(*gmpnumber); }
#define FREE_GMP_NUM(gmpnumber) { mpz_clear(*gmpnumber); efree(gmpnumber); }

/* {{{ convert_to_gmp
 * Convert zval to be gmp number */
static int convert_to_gmp(mpz_t * *gmpnumber, zval **val, int base TSRMLS_DC) 
{
	int ret = 0;
	int skip_lead = 0;

	*gmpnumber = emalloc(sizeof(mpz_t));

	switch (Z_TYPE_PP(val)) {
	case IS_LONG:
	case IS_BOOL:
	case IS_CONSTANT:
		{
			convert_to_long_ex(val);
			mpz_init_set_si(**gmpnumber, Z_LVAL_PP(val));
		}
		break;
	case IS_STRING:
		{
			char *numstr = Z_STRVAL_PP(val);

			if (Z_STRLEN_PP(val) > 2) {
				if (numstr[0] == '0') {
					if (numstr[1] == 'x' || numstr[1] == 'X') {
						base = 16;
						skip_lead = 1;
					} else if (base != 16 && (numstr[1] == 'b' || numstr[1] == 'B')) {
						base = 2;
						skip_lead = 1;
					}
				}
			}
			ret = mpz_init_set_str(**gmpnumber, (skip_lead ? &numstr[2] : numstr), base);
		}
		break;
	default:
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Unable to convert variable to GMP - wrong type");
		efree(*gmpnumber);
		return FAILURE;
	}

	if (ret) {
		FREE_GMP_NUM(*gmpnumber);
		return FAILURE;
	}
	
	return SUCCESS;
}
/* }}} */

/* {{{ typedefs
 */
typedef void (*gmp_unary_op_t)(mpz_ptr, mpz_srcptr);
typedef int (*gmp_unary_opl_t)(mpz_srcptr);

typedef void (*gmp_unary_ui_op_t)(mpz_ptr, unsigned long);

typedef void (*gmp_binary_op_t)(mpz_ptr, mpz_srcptr, mpz_srcptr);
typedef int (*gmp_binary_opl_t)(mpz_srcptr, mpz_srcptr);

typedef unsigned long (*gmp_binary_ui_op_t)(mpz_ptr, mpz_srcptr, unsigned long);
typedef void (*gmp_binary_op2_t)(mpz_ptr, mpz_ptr, mpz_srcptr, mpz_srcptr);
typedef unsigned long (*gmp_binary_ui_op2_t)(mpz_ptr, mpz_ptr, mpz_srcptr, unsigned long);
/* }}} */

#define gmp_zval_binary_ui_op(r, a, b, o, u) gmp_zval_binary_ui_op_ex(r, a, b, o, u, 0, 0 TSRMLS_CC)
#define gmp_zval_binary_ui_op2(r, a, b, o, u) gmp_zval_binary_ui_op2_ex(r, a, b, o, u, 0, 0 TSRMLS_CC)

#define gmp_binary_ui_op(op, uop) _gmp_binary_ui_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op, uop)
#define gmp_binary_op(op)         _gmp_binary_ui_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op, NULL)
#define gmp_binary_opl(op) _gmp_binary_opl(INTERNAL_FUNCTION_PARAM_PASSTHRU, op)

/* Unary operations */
#define gmp_unary_op(op)         _gmp_unary_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op)
#define gmp_unary_opl(op)         _gmp_unary_opl(INTERNAL_FUNCTION_PARAM_PASSTHRU, op)
#define gmp_unary_ui_op(op)      _gmp_unary_ui_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op)

/* {{{ gmp_zval_binary_ui_op_ex
   Execute GMP binary operation.
   May return GMP resource or long if operation allows this
*/
static inline void gmp_zval_binary_ui_op_ex(zval *return_value, zval **a_arg, zval **b_arg, gmp_binary_op_t gmp_op, gmp_binary_ui_op_t gmp_ui_op, int allow_ui_return, int check_b_zero TSRMLS_DC) 
{
	mpz_t *gmpnum_a, *gmpnum_b, *gmpnum_result;
	unsigned long long_result = 0;
	int use_ui = 0;
	int arga_tmp = 0, argb_tmp = 0;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, arga_tmp);
	
	if (gmp_ui_op && Z_TYPE_PP(b_arg) == IS_LONG && Z_LVAL_PP(b_arg) >= 0) {
		use_ui = 1;
	} else {
		FETCH_GMP_ZVAL(gmpnum_b, b_arg, argb_tmp);
	}

	if(check_b_zero) {
		int b_is_zero = 0;
		if(use_ui) {
			b_is_zero = (Z_LVAL_PP(b_arg) == 0);
		} else {
			b_is_zero = !mpz_cmp_ui(*gmpnum_b, 0);
		}

		if(b_is_zero) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Zero operand not allowed");
			FREE_GMP_TEMP(arga_tmp);
			FREE_GMP_TEMP(argb_tmp);
			RETURN_FALSE;
		}
	}

	INIT_GMP_NUM(gmpnum_result);

	if (use_ui && gmp_ui_op) {
		if (allow_ui_return) {
			long_result = gmp_ui_op(*gmpnum_result, *gmpnum_a, (unsigned long)Z_LVAL_PP(b_arg));
			if (mpz_sgn(*gmpnum_a) == -1) {
				long_result = -long_result;
			}
		} else {
			gmp_ui_op(*gmpnum_result, *gmpnum_a, (unsigned long)Z_LVAL_PP(b_arg));
		}
	} else {
		gmp_op(*gmpnum_result, *gmpnum_a, *gmpnum_b);
	}

	FREE_GMP_TEMP(arga_tmp);
	FREE_GMP_TEMP(argb_tmp);

	if (use_ui && allow_ui_return) {
		FREE_GMP_NUM(gmpnum_result);
		RETURN_LONG((long)long_result);
	} else {
		ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);
	}
}
/* }}} */

/* {{{ gmp_zval_binary_ui_op2_ex
   Execute GMP binary operation which returns 2 values.
   May return GMP resources or longs if operation allows this.
*/
static inline void gmp_zval_binary_ui_op2_ex(zval *return_value, zval **a_arg, zval **b_arg, gmp_binary_op2_t gmp_op, gmp_binary_ui_op2_t gmp_ui_op, int allow_ui_return, int check_b_zero TSRMLS_DC)
{
	mpz_t *gmpnum_a, *gmpnum_b, *gmpnum_result1, *gmpnum_result2;
	zval r;
	int use_ui = 0;
	unsigned long long_result = 0;
	int arga_tmp = 0, argb_tmp = 0;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, arga_tmp);

	if (gmp_ui_op && Z_TYPE_PP(b_arg) == IS_LONG && Z_LVAL_PP(b_arg) >= 0) {
		/* use _ui function */
		use_ui = 1;
	} else {
		FETCH_GMP_ZVAL(gmpnum_b, b_arg, argb_tmp);
	}

	if(check_b_zero) {
		int b_is_zero = 0;
		if(use_ui) {
			b_is_zero = (Z_LVAL_PP(b_arg) == 0);
		} else {
			b_is_zero = !mpz_cmp_ui(*gmpnum_b, 0);
		}

		if(b_is_zero) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Zero operand not allowed");
			FREE_GMP_TEMP(arga_tmp);
			FREE_GMP_TEMP(argb_tmp);
			RETURN_FALSE;
		}
	}

	INIT_GMP_NUM(gmpnum_result1);
	INIT_GMP_NUM(gmpnum_result2);

	if (use_ui && gmp_ui_op) {
		if (allow_ui_return) {
			long_result = gmp_ui_op(*gmpnum_result1, *gmpnum_result2, *gmpnum_a, (unsigned long)Z_LVAL_PP(b_arg));
		} else {
			gmp_ui_op(*gmpnum_result1, *gmpnum_result2, *gmpnum_a, (unsigned long)Z_LVAL_PP(b_arg));
		}
	} else {
		gmp_op(*gmpnum_result1, *gmpnum_result2, *gmpnum_a, *gmpnum_b);
	}

	FREE_GMP_TEMP(arga_tmp);
	FREE_GMP_TEMP(argb_tmp);

	array_init(return_value);
	ZEND_REGISTER_RESOURCE(&r, gmpnum_result1, le_gmp);
	add_index_resource(return_value, 0, Z_LVAL(r));
	if (use_ui && allow_ui_return) {
		mpz_clear(*gmpnum_result2);
		add_index_long(return_value, 1, long_result);
	} else {
		ZEND_REGISTER_RESOURCE(&r, gmpnum_result2, le_gmp);
		add_index_resource(return_value, 1, Z_LVAL(r));
	}
}
/* }}} */

/* {{{ _gmp_binary_ui_op
 */
static inline void _gmp_binary_ui_op(INTERNAL_FUNCTION_PARAMETERS, gmp_binary_op_t gmp_op, gmp_binary_ui_op_t gmp_ui_op)
{
	zval **a_arg, **b_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &a_arg, &b_arg) == FAILURE){
		return;
	}
	
	gmp_zval_binary_ui_op(return_value, a_arg, b_arg, gmp_op, gmp_ui_op);
}
/* }}} */

/* Unary operations */

/* {{{ gmp_zval_unary_op
 */
static inline void gmp_zval_unary_op(zval *return_value, zval **a_arg, gmp_unary_op_t gmp_op TSRMLS_DC) 
{
	mpz_t *gmpnum_a, *gmpnum_result;
	int temp_a;
	
	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	INIT_GMP_NUM(gmpnum_result);
	gmp_op(*gmpnum_result, *gmpnum_a);

	FREE_GMP_TEMP(temp_a);
	ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);
}
/* }}} */

/* {{{ gmp_zval_unary_ui_op
 */
static inline void gmp_zval_unary_ui_op(zval *return_value, zval **a_arg, gmp_unary_ui_op_t gmp_op)
{
	mpz_t *gmpnum_result;

	convert_to_long_ex(a_arg);

	INIT_GMP_NUM(gmpnum_result);
	gmp_op(*gmpnum_result, Z_LVAL_PP(a_arg));

	ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);
}
/* }}} */

/* {{{ _gmp_unary_ui_op
   Execute GMP unary operation.
*/
static inline void _gmp_unary_ui_op(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_ui_op_t gmp_op)
{
	zval **a_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &a_arg) == FAILURE){
		return;
	}

	gmp_zval_unary_ui_op(return_value, a_arg, gmp_op);
}
/* }}} */

/* {{{ _gmp_unary_op
 */
static inline void _gmp_unary_op(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_op_t gmp_op)
{
	zval **a_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &a_arg) == FAILURE){
		return;
	}
	
	gmp_zval_unary_op(return_value, a_arg, gmp_op TSRMLS_CC);
}
/* }}} */

/* {{{ _gmp_unary_opl
 */
static inline void _gmp_unary_opl(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_opl_t gmp_op)
{
	zval **a_arg;
	mpz_t *gmpnum_a;
	int temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &a_arg) == FAILURE){
		return;
	}
	
	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	RETVAL_LONG(gmp_op(*gmpnum_a));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ _gmp_binary_opl
 */
static inline void _gmp_binary_opl(INTERNAL_FUNCTION_PARAMETERS, gmp_binary_opl_t gmp_op)
{
	zval **a_arg, **b_arg;
	mpz_t *gmpnum_a, *gmpnum_b;
	int temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &a_arg, &b_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	FETCH_GMP_ZVAL(gmpnum_b, b_arg, temp_b);

	RETVAL_LONG(gmp_op(*gmpnum_a, *gmpnum_b));

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ proto resource gmp_init(mixed number [, int base])
   Initializes GMP number */
ZEND_FUNCTION(gmp_init)
{
	zval **number_arg;
	mpz_t * gmpnumber;
	long base=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|l", &number_arg, &base) == FAILURE) {
		return;
	}

#if (__GNU_MP_VERSION >= 4 && __GNU_MP_VERSION_MINOR >= 2)
	if (base && (base < 2 || base > 62)) {
#else
	if (base && (base < 2 || base > 36)) {
#endif
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad base for conversion: %ld (should be between 2 and 36)", base);
		RETURN_FALSE;
	}

	if (convert_to_gmp(&gmpnumber, number_arg, base TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}

	/* Write your own code here to handle argument number. */
	ZEND_REGISTER_RESOURCE(return_value, gmpnumber, le_gmp);
}
/* }}} */

/* {{{ proto int gmp_intval(resource gmpnumber)
   Gets signed long value of GMP number */
ZEND_FUNCTION(gmp_intval)
{
	zval **gmpnumber_arg;
	mpz_t * gmpnum;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &gmpnumber_arg) == FAILURE){
		return;
	}
	
	if (Z_TYPE_PP(gmpnumber_arg) == IS_RESOURCE) {
		ZEND_FETCH_RESOURCE(gmpnum, mpz_t *, gmpnumber_arg, -1, GMP_RESOURCE_NAME, le_gmp);
		RETVAL_LONG(mpz_get_si(*gmpnum));
	} else {
		convert_to_long_ex(gmpnumber_arg);
		RETVAL_LONG(Z_LVAL_PP(gmpnumber_arg));
	}
}
/* }}} */

/* {{{ proto string gmp_strval(resource gmpnumber [, int base])
   Gets string representation of GMP number  */
ZEND_FUNCTION(gmp_strval)
{
	zval **gmpnumber_arg;
	int num_len;
	long base = 10;
	mpz_t * gmpnum;
	char *out_string;
	int temp_a;

	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "Z|l", &gmpnumber_arg, &base ) == FAILURE ) {
		return;
	}

#if __GNU_MP_VERSION >= 4 && __GNU_MP_VERSION_MINOR >= 2
	if ((base < 2 && base > -2) || base > 62 || base < -36) {
#else
	if (base < 2 || base > 36) {
#endif
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad base for conversion: %ld", base);
		RETURN_FALSE;
	}

	FETCH_GMP_ZVAL(gmpnum, gmpnumber_arg, temp_a);

	num_len = mpz_sizeinbase(*gmpnum, abs(base));
	out_string = emalloc(num_len+2);
	if (mpz_sgn(*gmpnum) < 0) {
		num_len++;
	}
	mpz_get_str(out_string, base, *gmpnum);
	
	FREE_GMP_TEMP(temp_a);
	
	/* 
	From GMP documentation for mpz_sizeinbase():
	The returned value will be exact or 1 too big.  If base is a power of
	2, the returned value will always be exact.

	So let's check to see if we already have a \0 byte...
	*/

	if (out_string[num_len-1] == '\0') {
		num_len--;
	} else {
		out_string[num_len] = '\0';
	}
	RETVAL_STRINGL(out_string, num_len, 0);
}
/* }}} */

/* {{{ proto resource gmp_add(resource a, resource b)
   Add a and b */
ZEND_FUNCTION(gmp_add)
{
	gmp_binary_ui_op(mpz_add, (gmp_binary_ui_op_t)mpz_add_ui);
}
/* }}} */

/* {{{ proto resource gmp_sub(resource a, resource b)
   Subtract b from a */
ZEND_FUNCTION(gmp_sub)
{
	gmp_binary_ui_op(mpz_sub, (gmp_binary_ui_op_t)mpz_sub_ui);
}
/* }}} */

/* {{{ proto resource gmp_mul(resource a, resource b)
   Multiply a and b */
ZEND_FUNCTION(gmp_mul)
{
	gmp_binary_ui_op(mpz_mul, (gmp_binary_ui_op_t)mpz_mul_ui);
}
/* }}} */

/* {{{ proto array gmp_div_qr(resource a, resource b [, int round])
   Divide a by b, returns quotient and reminder */
ZEND_FUNCTION(gmp_div_qr)
{
	zval **a_arg, **b_arg;
	long round = GMP_ROUND_ZERO;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ|l", &a_arg, &b_arg, &round) == FAILURE) {
		return;
	}

	switch (round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op2_ex(return_value, a_arg, b_arg, mpz_tdiv_qr, (gmp_binary_ui_op2_t)mpz_tdiv_qr_ui, 0, 1 TSRMLS_CC);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op2_ex(return_value, a_arg, b_arg, mpz_cdiv_qr, (gmp_binary_ui_op2_t)mpz_cdiv_qr_ui, 0, 1 TSRMLS_CC);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op2_ex(return_value, a_arg, b_arg, mpz_fdiv_qr, (gmp_binary_ui_op2_t)mpz_fdiv_qr_ui, 0, 1 TSRMLS_CC);
		break;
	}
							   
}
/* }}} */

/* {{{ proto resource gmp_div_r(resource a, resource b [, int round])
   Divide a by b, returns reminder only */
ZEND_FUNCTION(gmp_div_r)
{
	zval **a_arg, **b_arg;
	long round = GMP_ROUND_ZERO;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ|l", &a_arg, &b_arg, &round) == FAILURE) {
		return;
	}

	switch (round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_tdiv_r, (gmp_binary_ui_op_t)mpz_tdiv_r_ui, 1, 1 TSRMLS_CC);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_cdiv_r, (gmp_binary_ui_op_t)mpz_cdiv_r_ui, 1, 1 TSRMLS_CC);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_fdiv_r, (gmp_binary_ui_op_t)mpz_fdiv_r_ui, 1, 1 TSRMLS_CC);
		break;
	}
}
/* }}} */

/* {{{ proto resource gmp_div_q(resource a, resource b [, int round])
   Divide a by b, returns quotient only */
ZEND_FUNCTION(gmp_div_q)
{
	zval **a_arg, **b_arg;
	long round = GMP_ROUND_ZERO;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ|l", &a_arg, &b_arg, &round) == FAILURE) {
		return;
	}

	switch (round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_tdiv_q, (gmp_binary_ui_op_t)mpz_tdiv_q_ui, 0, 1 TSRMLS_CC);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_cdiv_q, (gmp_binary_ui_op_t)mpz_cdiv_q_ui, 0, 1 TSRMLS_CC);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_fdiv_q, (gmp_binary_ui_op_t)mpz_fdiv_q_ui, 0, 1 TSRMLS_CC);
		break;
	}
							   
}
/* }}} */

/* {{{ proto resource gmp_mod(resource a, resource b)
   Computes a modulo b */
ZEND_FUNCTION(gmp_mod)
{
	zval **a_arg, **b_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &a_arg, &b_arg) == FAILURE){
		return;
	}

	gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_mod, (gmp_binary_ui_op_t)mpz_mod_ui, 1, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto resource gmp_divexact(resource a, resource b)
   Divide a by b using exact division algorithm */
ZEND_FUNCTION(gmp_divexact)
{
	zval **a_arg, **b_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &a_arg, &b_arg) == FAILURE){
		return;
	}
	
	gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_divexact, NULL, 0, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto resource gmp_neg(resource a)
   Negates a number */
ZEND_FUNCTION(gmp_neg)
{
	gmp_unary_op(mpz_neg);
}
/* }}} */

/* {{{ proto resource gmp_abs(resource a)
   Calculates absolute value */
ZEND_FUNCTION(gmp_abs)
{
	gmp_unary_op(mpz_abs);
}
/* }}} */

/* {{{ proto resource gmp_fact(int a)
   Calculates factorial function */
ZEND_FUNCTION(gmp_fact)
{
	zval **a_arg;
	mpz_t *gmpnum_tmp;
	int temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &a_arg) == FAILURE){
		return;
	}

	if (Z_TYPE_PP(a_arg) == IS_RESOURCE) {
		FETCH_GMP_ZVAL(gmpnum_tmp, a_arg, temp_a);	/* no need to free this since it's IS_RESOURCE */
		if (mpz_sgn(*gmpnum_tmp) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number has to be greater than or equal to 0");
			RETURN_FALSE;
		}
	} else {
		convert_to_long_ex(a_arg);
		if (Z_LVAL_PP(a_arg) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number has to be greater than or equal to 0");
			RETURN_FALSE;
		}
	}
		
	gmp_zval_unary_ui_op(return_value, a_arg, mpz_fac_ui);
}
/* }}} */

/* {{{ proto resource gmp_pow(resource base, int exp)
   Raise base to power exp */
ZEND_FUNCTION(gmp_pow)
{
	zval **base_arg;
	mpz_t *gmpnum_result, *gmpnum_base;
	int use_ui = 0;
	int temp_base;
	long exp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zl", &base_arg, &exp) == FAILURE) {
		return;
	}

	if (Z_TYPE_PP(base_arg) == IS_LONG && Z_LVAL_PP(base_arg) >= 0) {
		use_ui = 1;
	} else {
		FETCH_GMP_ZVAL(gmpnum_base, base_arg, temp_base);
	}

	if (exp < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Negative exponent not supported");
		RETURN_FALSE;
	}
	
	INIT_GMP_NUM(gmpnum_result);
	if (use_ui) {
		mpz_ui_pow_ui(*gmpnum_result, Z_LVAL_PP(base_arg), exp);
	} else {
		mpz_pow_ui(*gmpnum_result, *gmpnum_base, exp);
		FREE_GMP_TEMP(temp_base);
	}
	ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);
}
/* }}} */

/* {{{ proto resource gmp_powm(resource base, resource exp, resource mod)
   Raise base to power exp and take result modulo mod */
ZEND_FUNCTION(gmp_powm)
{
	zval **base_arg, **exp_arg, **mod_arg;
	mpz_t *gmpnum_base, *gmpnum_exp, *gmpnum_mod, *gmpnum_result;
	int use_ui = 0;
	int temp_base, temp_exp, temp_mod;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZZ", &base_arg, &exp_arg, &mod_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_base, base_arg, temp_base);

	if (Z_TYPE_PP(exp_arg) == IS_LONG && Z_LVAL_PP(exp_arg) >= 0) {
		use_ui = 1;
	} else {
		FETCH_GMP_ZVAL(gmpnum_exp, exp_arg, temp_exp);
		if (mpz_sgn(*gmpnum_exp) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"Second parameter cannot be less than 0");
			RETURN_FALSE;
		}
	}
	FETCH_GMP_ZVAL(gmpnum_mod, mod_arg, temp_mod);

	if (!mpz_cmp_ui(*gmpnum_mod, 0)) {
		FREE_GMP_TEMP(temp_base);
		if (use_ui) {
			FREE_GMP_TEMP(temp_exp);
		}
		FREE_GMP_TEMP(temp_mod);
		RETURN_FALSE;
	}

	INIT_GMP_NUM(gmpnum_result);
	if (use_ui) {
		mpz_powm_ui(*gmpnum_result, *gmpnum_base, (unsigned long)Z_LVAL_PP(exp_arg), *gmpnum_mod);
	} else {
		mpz_powm(*gmpnum_result, *gmpnum_base, *gmpnum_exp, *gmpnum_mod);
		FREE_GMP_TEMP(temp_exp);
	}

	FREE_GMP_TEMP(temp_base);
	FREE_GMP_TEMP(temp_mod);

	ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);

}
/* }}} */

/* {{{ proto resource gmp_sqrt(resource a)
   Takes integer part of square root of a */
ZEND_FUNCTION(gmp_sqrt)
{
	zval **a_arg;
	mpz_t *gmpnum_a, *gmpnum_result;
	int temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &a_arg) == FAILURE){
		return;
	}
	
	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	if (mpz_sgn(*gmpnum_a) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number has to be greater than or equal to 0");
		FREE_GMP_TEMP(temp_a);
		RETURN_FALSE;
	}	
	
	INIT_GMP_NUM(gmpnum_result);
	mpz_sqrt(*gmpnum_result, *gmpnum_a);
	FREE_GMP_TEMP(temp_a);

	ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);
}
/* }}} */

/* {{{ proto array gmp_sqrtrem(resource a)
   Square root with remainder */
ZEND_FUNCTION(gmp_sqrtrem)
{
	zval **a_arg;
	mpz_t *gmpnum_a, *gmpnum_result1, *gmpnum_result2;
	zval r;
	int temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &a_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	
	if (mpz_sgn(*gmpnum_a) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Number has to be greater than or equal to 0");
		RETURN_FALSE;
	}
	
	INIT_GMP_NUM(gmpnum_result1);
	INIT_GMP_NUM(gmpnum_result2);

	mpz_sqrtrem(*gmpnum_result1, *gmpnum_result2, *gmpnum_a);
	FREE_GMP_TEMP(temp_a);

	array_init(return_value);
	ZEND_REGISTER_RESOURCE(&r, gmpnum_result1, le_gmp);
	add_index_resource(return_value, 0, Z_LVAL(r));
	ZEND_REGISTER_RESOURCE(&r, gmpnum_result2, le_gmp);
	add_index_resource(return_value, 1, Z_LVAL(r));
}
/* }}} */

/* {{{ proto bool gmp_perfect_square(resource a)
   Checks if a is an exact square */
ZEND_FUNCTION(gmp_perfect_square)
{
	zval **a_arg;
	mpz_t *gmpnum_a;
	int temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &a_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	RETVAL_BOOL((mpz_perfect_square_p(*gmpnum_a)!=0));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto int gmp_prob_prime(resource a[, int reps])
   Checks if a is "probably prime" */
ZEND_FUNCTION(gmp_prob_prime)
{
	zval **gmpnumber_arg;
	mpz_t *gmpnum_a;
	long reps = 10;
	int temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|l", &gmpnumber_arg, &reps) == FAILURE) {
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, gmpnumber_arg, temp_a);

	RETVAL_LONG(mpz_probab_prime_p(*gmpnum_a, reps));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto resource gmp_gcd(resource a, resource b)
   Computes greatest common denominator (gcd) of a and b */
ZEND_FUNCTION(gmp_gcd)
{
	zval **a_arg, **b_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &a_arg, &b_arg) == FAILURE){
		return;
	}

	gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_gcd, (gmp_binary_ui_op_t)mpz_gcd_ui, 0, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto array gmp_gcdext(resource a, resource b)
   Computes G, S, and T, such that AS + BT = G = `gcd' (A, B) */
ZEND_FUNCTION(gmp_gcdext)
{
	zval **a_arg, **b_arg;
	mpz_t *gmpnum_a, *gmpnum_b, *gmpnum_t, *gmpnum_s, *gmpnum_g;
	zval r;
	int temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &a_arg, &b_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	FETCH_GMP_ZVAL(gmpnum_b, b_arg, temp_b);

	INIT_GMP_NUM(gmpnum_g);
	INIT_GMP_NUM(gmpnum_s);
	INIT_GMP_NUM(gmpnum_t);

	mpz_gcdext(*gmpnum_g, *gmpnum_s, *gmpnum_t, *gmpnum_a, *gmpnum_b);
	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);

	array_init(return_value);

	ZEND_REGISTER_RESOURCE(&r, gmpnum_g, le_gmp);
	add_assoc_resource(return_value, "g", Z_LVAL(r));
	ZEND_REGISTER_RESOURCE(&r, gmpnum_s, le_gmp);
	add_assoc_resource(return_value, "s", Z_LVAL(r));
	ZEND_REGISTER_RESOURCE(&r, gmpnum_t, le_gmp);
	add_assoc_resource(return_value, "t", Z_LVAL(r));
}
/* }}} */

/* {{{ proto resource gmp_invert(resource a, resource b)
   Computes the inverse of a modulo b */
ZEND_FUNCTION(gmp_invert)
{
	zval **a_arg, **b_arg;
	mpz_t *gmpnum_a, *gmpnum_b, *gmpnum_result;
	int temp_a, temp_b;
	int res;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &a_arg, &b_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	FETCH_GMP_ZVAL(gmpnum_b, b_arg, temp_b);

	INIT_GMP_NUM(gmpnum_result);
	res=mpz_invert(*gmpnum_result, *gmpnum_a, *gmpnum_b);
	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
	if (res) {
		ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);
	} else {
		FREE_GMP_NUM(gmpnum_result);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int gmp_jacobi(resource a, resource b)
   Computes Jacobi symbol */
ZEND_FUNCTION(gmp_jacobi)
{
	gmp_binary_opl(mpz_jacobi);
}
/* }}} */

/* {{{ proto int gmp_legendre(resource a, resource b)
   Computes Legendre symbol */
ZEND_FUNCTION(gmp_legendre)
{
	gmp_binary_opl(mpz_legendre);
}
/* }}} */

/* {{{ proto int gmp_cmp(resource a, resource b)
   Compares two numbers */
ZEND_FUNCTION(gmp_cmp)
{
	zval **a_arg, **b_arg;
	mpz_t *gmpnum_a, *gmpnum_b;
	int use_si = 0, res;
	int temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &a_arg, &b_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	if (Z_TYPE_PP(b_arg) == IS_LONG) {
		use_si = 1;
	} else {
		FETCH_GMP_ZVAL(gmpnum_b, b_arg, temp_b);
	}

	if (use_si) {
		res = mpz_cmp_si(*gmpnum_a, Z_LVAL_PP(b_arg));
	} else {
		res = mpz_cmp(*gmpnum_a, *gmpnum_b);
	}
	FREE_GMP_TEMP(temp_a);
	
	RETURN_LONG(res);
}
/* }}} */

/* {{{ proto int gmp_sign(resource a)
   Gets the sign of the number */
ZEND_FUNCTION(gmp_sign)
{
	zval **a_arg;
	mpz_t *gmpnum_a;
	int temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &a_arg) == FAILURE){
		return;
	}
	
	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	RETVAL_LONG(mpz_sgn(*gmpnum_a));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto resource gmp_random([int limiter])
   Gets random number */
ZEND_FUNCTION(gmp_random)
{
	long limiter = 20;
	mpz_t *gmpnum_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &limiter) == FAILURE) {
		return;
	}

	INIT_GMP_NUM(gmpnum_result);

	if (!GMPG(rand_initialized)) {
		/* Initialize */
		gmp_randinit_lc_2exp_size(GMPG(rand_state), 32L);

		/* Seed */
		gmp_randseed_ui(GMPG(rand_state), GENERATE_SEED());

		GMPG(rand_initialized) = 1;
	}
#ifdef GMP_LIMB_BITS
	mpz_urandomb(*gmpnum_result, GMPG(rand_state), GMP_ABS (limiter) * GMP_LIMB_BITS);
#else
	mpz_urandomb(*gmpnum_result, GMPG(rand_state), GMP_ABS (limiter) * __GMP_BITS_PER_MP_LIMB);
#endif
	ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);
}
/* }}} */

/* {{{ proto resource gmp_and(resource a, resource b)
   Calculates logical AND of a and b */
ZEND_FUNCTION(gmp_and)
{
	gmp_binary_op(mpz_and);
}
/* }}} */

/* {{{ proto resource gmp_or(resource a, resource b)
   Calculates logical OR of a and b */
ZEND_FUNCTION(gmp_or)
{
	gmp_binary_op(mpz_ior);
}
/* }}} */

/* {{{ proto resource gmp_com(resource a)
   Calculates one's complement of a */
ZEND_FUNCTION(gmp_com)
{
	gmp_unary_op(mpz_com);
}
/* }}} */

/* {{{ proto resource gmp_nextprime(resource a)
   Finds next prime of a */
ZEND_FUNCTION(gmp_nextprime)
{
   gmp_unary_op(mpz_nextprime);
}
/* }}} */

/* {{{ proto resource gmp_xor(resource a, resource b)
   Calculates logical exclusive OR of a and b */
ZEND_FUNCTION(gmp_xor)
{
	/* use formula: a^b = (a|b)&^(a&b) */
	zval **a_arg, **b_arg;
	mpz_t *gmpnum_a, *gmpnum_b, *gmpnum_result, *gmpnum_t;
	int temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &a_arg, &b_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	FETCH_GMP_ZVAL(gmpnum_b, b_arg, temp_b);

	INIT_GMP_NUM(gmpnum_result);
	INIT_GMP_NUM(gmpnum_t);

	mpz_and(*gmpnum_t, *gmpnum_a, *gmpnum_b);
	mpz_com(*gmpnum_t, *gmpnum_t);

	mpz_ior(*gmpnum_result, *gmpnum_a, *gmpnum_b);
	mpz_and(*gmpnum_result, *gmpnum_result, *gmpnum_t);

	FREE_GMP_NUM(gmpnum_t);

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
	ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);
}
/* }}} */

/* {{{ proto void gmp_setbit(resource &a, int index[, bool set_clear])
   Sets or clear bit in a */
ZEND_FUNCTION(gmp_setbit)
{
	zval **a_arg;
	long index;
	zend_bool set = 1;
	mpz_t *gmpnum_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zl|b", &a_arg, &index, &set) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(gmpnum_a, mpz_t *, a_arg, -1, GMP_RESOURCE_NAME, le_gmp);

	if (index < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Index must be greater than or equal to zero");
		return;
	}

	if (set) {
		mpz_setbit(*gmpnum_a, index);
	} else {
		mpz_clrbit(*gmpnum_a, index);
	}
}
/* }}} */

/* {{{ proto void gmp_clrbit(resource &a, int index)
   Clears bit in a */
ZEND_FUNCTION(gmp_clrbit)
{
	zval **a_arg;
	long index;
	mpz_t *gmpnum_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zl", &a_arg, &index) == FAILURE){
		return;
	}

	ZEND_FETCH_RESOURCE(gmpnum_a, mpz_t *, a_arg, -1, GMP_RESOURCE_NAME, le_gmp);

	if (index < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Index must be greater than or equal to zero");
		return;
	}

	mpz_clrbit(*gmpnum_a, index);
}
/* }}} */

/* {{{ proto bool gmp_testbit(resource a, int index)
   Tests if bit is set in a */
ZEND_FUNCTION(gmp_testbit)
{
	zval **a_arg;
	long index;
	mpz_t *gmpnum_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zl", &a_arg, &index) == FAILURE){
		return;
	}

	ZEND_FETCH_RESOURCE(gmpnum_a, mpz_t *, a_arg, -1, GMP_RESOURCE_NAME, le_gmp);

	if (index < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Index must be greater than or equal to zero");
		RETURN_FALSE;
	}

	if (mpz_tstbit(*gmpnum_a, index)) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int gmp_popcount(resource a)
   Calculates the population count of a */
ZEND_FUNCTION(gmp_popcount)
{
	zval **a_arg;
	mpz_t *gmpnum_a;
	int temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &a_arg) == FAILURE){
		return;
	}
	
	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	RETVAL_LONG(mpz_popcount(*gmpnum_a));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto int gmp_hamdist(resource a, resource b)
   Calculates hamming distance between a and b */
ZEND_FUNCTION(gmp_hamdist)
{
	zval **a_arg, **b_arg;
	mpz_t *gmpnum_a, *gmpnum_b;
	int temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &a_arg, &b_arg) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	FETCH_GMP_ZVAL(gmpnum_b, b_arg, temp_b);

	RETVAL_LONG(mpz_hamdist(*gmpnum_a, *gmpnum_b));
	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ proto int gmp_scan0(resource a, int start)
   Finds first zero bit */
ZEND_FUNCTION(gmp_scan0)
{
	zval **a_arg;
	mpz_t *gmpnum_a;
	int temp_a;
	long start;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zl", &a_arg, &start) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);

	if (start < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Starting index must be greater than or equal to zero");
		RETURN_FALSE;
	}

	RETVAL_LONG(mpz_scan0(*gmpnum_a, start));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ proto int gmp_scan1(resource a, int start)
   Finds first non-zero bit */
ZEND_FUNCTION(gmp_scan1)
{
	zval **a_arg;
	mpz_t *gmpnum_a;
	int temp_a;
	long start;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zl", &a_arg, &start) == FAILURE){
		return;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a);
	if (start < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Starting index must be greater than or equal to zero");
		RETURN_FALSE;
	}

	RETVAL_LONG(mpz_scan1(*gmpnum_a, start));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ _php_gmpnum_free
 */
static void _php_gmpnum_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	mpz_t *gmpnum = (mpz_t *)rsrc->ptr;

	FREE_GMP_NUM(gmpnum);
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
