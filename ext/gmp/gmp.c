/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
/* If you declare any globals in php_gmp.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(gmp)
*/

/* True global resources - no need for thread safety here */
static int le_gmp;

static unsigned char first_of_two_force_ref[] = { 2, BYREF_FORCE, BYREF_NONE };

/* {{{ gmp_functions[]
 */
function_entry gmp_functions[] = {
	ZEND_FE(gmp_init,	NULL)
	ZEND_FE(gmp_intval,	NULL)
	ZEND_FE(gmp_strval,	NULL)
	ZEND_FE(gmp_add,	    NULL)
	ZEND_FE(gmp_sub,	    NULL)
	ZEND_FE(gmp_mul,	    NULL)
	ZEND_FE(gmp_div_qr,	NULL)
	ZEND_FE(gmp_div_q,	NULL)
	ZEND_FE(gmp_div_r,	NULL)
	ZEND_FALIAS(gmp_div,	gmp_div_q, NULL)
	ZEND_FE(gmp_mod,	NULL)
	ZEND_FE(gmp_divexact,	NULL)
	ZEND_FE(gmp_neg,	NULL)
	ZEND_FE(gmp_abs,	NULL)
	ZEND_FE(gmp_fact,	NULL)
	ZEND_FE(gmp_sqrt,	NULL)
	ZEND_FE(gmp_sqrtrem,	NULL)
	ZEND_FE(gmp_pow,	NULL)
	ZEND_FE(gmp_powm,	NULL)
	ZEND_FE(gmp_perfect_square,	NULL)
	ZEND_FE(gmp_prob_prime,	NULL)
	ZEND_FE(gmp_gcd,	NULL)
	ZEND_FE(gmp_gcdext,	NULL)
	ZEND_FE(gmp_invert,	NULL)
	ZEND_FE(gmp_jacobi,	NULL)
	ZEND_FE(gmp_legendre,	NULL)
	ZEND_FE(gmp_cmp,	NULL)
	ZEND_FE(gmp_sign,	NULL)
	ZEND_FE(gmp_random,	NULL)
	ZEND_FE(gmp_and,	NULL)
	ZEND_FE(gmp_or,	NULL)
	ZEND_FE(gmp_com,	NULL)
	ZEND_FE(gmp_xor,	NULL)
	ZEND_FE(gmp_setbit,	first_of_two_force_ref)
	ZEND_FE(gmp_clrbit,	first_of_two_force_ref)
	ZEND_FE(gmp_scan0, NULL)
	ZEND_FE(gmp_scan1, NULL)
	ZEND_FE(gmp_popcount, NULL)
	ZEND_FE(gmp_hamdist, NULL)
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
	ZEND_MODULE_SHUTDOWN_N(gmp),
	NULL,
	NULL,
	ZEND_MODULE_INFO_N(gmp),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
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

/* {{{ ZEND_MINIT_FUNCTION
 */
ZEND_MODULE_STARTUP_D(gmp)
{
	le_gmp = zend_register_list_destructors_ex(_php_gmpnum_free, NULL, GMP_RESOURCE_NAME, module_number);
	REGISTER_LONG_CONSTANT("GMP_ROUND_ZERO", GMP_ROUND_ZERO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_ROUND_PLUSINF", GMP_ROUND_PLUSINF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_ROUND_MINUSINF", GMP_ROUND_MINUSINF, CONST_CS | CONST_PERSISTENT);

	mp_set_memory_functions(gmp_emalloc, gmp_erealloc, gmp_efree);

	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_MSHUTDOWN_FUNCTION
 */
ZEND_MODULE_SHUTDOWN_D(gmp)
{
	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_MINFO_FUNCTION
 */
ZEND_MODULE_INFO_D(gmp)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "gmp support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* Fetch zval to be GMP number.
   Initially, zval can be also number or string */
#define FETCH_GMP_ZVAL(gmpnumber, zval) \
if(Z_TYPE_PP(zval) == IS_RESOURCE) { \
	ZEND_FETCH_RESOURCE(gmpnumber, mpz_t *, zval, -1, GMP_RESOURCE_NAME, le_gmp);\
} else {\
	if(convert_to_gmp(&gmpnumber, zval, 0) == FAILURE) {\
		RETURN_FALSE;\
	}\
	ZEND_REGISTER_RESOURCE(NULL, gmpnumber, le_gmp);\
}

/* create a new initialized GMP number */
#define INIT_GMP_NUM(gmpnumber) { gmpnumber=emalloc(sizeof(mpz_t)); mpz_init(*gmpnumber); }
#define FREE_GMP_NUM(gmpnumber) { mpz_clear(*gmpnumber); efree(gmpnumber); }

/* {{{ convert_to_gmp
 * Convert zval to be gmp number */
static int convert_to_gmp(mpz_t * *gmpnumber, zval **val, int base) 
{
	int ret = 0;
	int skip_lead = 0;

	*gmpnumber = emalloc(sizeof(mpz_t));
	switch(Z_TYPE_PP(val)) {
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
		zend_error(E_WARNING,"Unable to convert variable to GMP - wrong type");
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

#define gmp_zval_binary_ui_op(r, a, b, o, u) gmp_zval_binary_ui_op_ex(r, a, b, o, u, 0 TSRMLS_CC)
#define gmp_zval_binary_ui_op2(r, a, b, o, u) gmp_zval_binary_ui_op2_ex(r, a, b, o, u, 0 TSRMLS_CC)

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
static inline void gmp_zval_binary_ui_op_ex(zval *return_value, zval **a_arg, zval **b_arg, gmp_binary_op_t gmp_op, gmp_binary_ui_op_t gmp_ui_op, int allow_ui_return TSRMLS_DC) 
{
	mpz_t *gmpnum_a, *gmpnum_b, *gmpnum_result;
	unsigned long long_result=0;
	int use_ui=0;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);
	if(gmp_ui_op && Z_TYPE_PP(b_arg) == IS_LONG && Z_LVAL_PP(b_arg) >= 0) {
		use_ui=1;
	} else {
		FETCH_GMP_ZVAL(gmpnum_b, b_arg);
	}

	INIT_GMP_NUM(gmpnum_result);
	if(use_ui && gmp_ui_op) {
		if(allow_ui_return) {
			long_result = gmp_ui_op(*gmpnum_result, *gmpnum_a, (unsigned long)Z_LVAL_PP(b_arg));
		} else {
			gmp_ui_op(*gmpnum_result, *gmpnum_a, (unsigned long)Z_LVAL_PP(b_arg));
		}
	} else {
		gmp_op(*gmpnum_result, *gmpnum_a, *gmpnum_b);
	}

	if(use_ui && allow_ui_return) {
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
static inline void gmp_zval_binary_ui_op2_ex(zval *return_value, zval **a_arg, zval **b_arg, gmp_binary_op2_t gmp_op, gmp_binary_ui_op2_t gmp_ui_op, int allow_ui_return TSRMLS_DC)
{
	mpz_t *gmpnum_a, *gmpnum_b, *gmpnum_result1, *gmpnum_result2;
	zval r;
	int use_ui=0;
	unsigned long long_result;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);
	if(gmp_ui_op && Z_TYPE_PP(b_arg) == IS_LONG && Z_LVAL_PP(b_arg) >= 0) {
		/* use _ui function */
		use_ui=1;
	} else {
		FETCH_GMP_ZVAL(gmpnum_b, b_arg);
	}

	INIT_GMP_NUM(gmpnum_result1);
	INIT_GMP_NUM(gmpnum_result2);

	if(use_ui && gmp_ui_op) {
		if(allow_ui_return) {
			long_result = gmp_ui_op(*gmpnum_result1, *gmpnum_result2, *gmpnum_a, (unsigned long)Z_LVAL_PP(b_arg));
		} else {
			gmp_ui_op(*gmpnum_result1, *gmpnum_result2, *gmpnum_a, (unsigned long)Z_LVAL_PP(b_arg));
		}
	} else {
		gmp_op(*gmpnum_result1, *gmpnum_result2, *gmpnum_a, *gmpnum_b);
	}

	array_init(return_value);
	ZEND_REGISTER_RESOURCE(&r, gmpnum_result1, le_gmp);
	add_index_resource(return_value, 0, Z_LVAL(r));
	if(use_ui && allow_ui_return) {
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
static inline void _gmp_binary_ui_op(INTERNAL_FUNCTION_PARAMETERS, gmp_binary_op_t gmp_op, gmp_binary_ui_op_t gmp_ui_op) {
	zval **a_arg, **b_arg;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &b_arg) == FAILURE){
		WRONG_PARAM_COUNT;
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
	
	FETCH_GMP_ZVAL(gmpnum_a, a_arg);

	INIT_GMP_NUM(gmpnum_result);
	gmp_op(*gmpnum_result, *gmpnum_a);

	ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);
}
/* }}} */

/* {{{ gmp_zval_unary_ui_op
 */
static inline void gmp_zval_unary_ui_op(zval *return_value, zval **a_arg, gmp_unary_ui_op_t gmp_op) {
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
static inline void _gmp_unary_ui_op(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_ui_op_t gmp_op) {
	zval **a_arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &a_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	gmp_zval_unary_ui_op(return_value, a_arg, gmp_op);
}
/* }}} */

/* {{{ _gmp_unary_op
 */
static inline void _gmp_unary_op(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_op_t gmp_op) {
	zval **a_arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &a_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	gmp_zval_unary_op(return_value, a_arg, gmp_op TSRMLS_CC);
}
/* }}} */

/* {{{ _gmp_unary_opl
 */
static inline void _gmp_unary_opl(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_opl_t gmp_op) {
	zval **a_arg;
	mpz_t *gmpnum_a;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &a_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	FETCH_GMP_ZVAL(gmpnum_a, a_arg);

	RETURN_LONG(gmp_op(*gmpnum_a));
}
/* }}} */

/* {{{ _gmp_binary_opl
 */
static inline void _gmp_binary_opl(INTERNAL_FUNCTION_PARAMETERS, gmp_binary_opl_t gmp_op) {
	zval **a_arg, **b_arg;
	mpz_t *gmpnum_a, *gmpnum_b;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &b_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);
	FETCH_GMP_ZVAL(gmpnum_b, a_arg);

	RETURN_LONG(gmp_op(*gmpnum_a, *gmpnum_b));
}
/* }}} */

/* {{{ proto resource gmp_init(mixed number [, int base])
   Initializes GMP number */
ZEND_FUNCTION(gmp_init)
{
	zval **number_arg, **base_arg;
	mpz_t * gmpnumber;
	int argc;
	int base=0;

	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &number_arg, &base_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if (argc==2) {
			convert_to_long_ex(base_arg);
			base = Z_LVAL_PP(base_arg);
		if(base < 2 || base > 36) {
			zend_error(E_WARNING, "Bad base for conversion: %d (should be between 2 and 36)", base);
			RETURN_FALSE;
		}
	}

	if(convert_to_gmp(&gmpnumber, number_arg, base) == FAILURE) {
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

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &gmpnumber_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	if(Z_TYPE_PP(gmpnumber_arg) == IS_RESOURCE) {
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
	zval **gmpnumber_arg, **base_arg;
	int base=10, num_len, argc;
	mpz_t * gmpnum;
	char *out_string;

	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &gmpnumber_arg, &base_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum, gmpnumber_arg);

	switch (argc) {
		case 2:
			convert_to_long_ex(base_arg);
			base = Z_LVAL_PP(base_arg);
			break;
		case 1:
			base = 10;
			break;
	}

	if(base < 2 || base > 36) {
		zend_error(E_WARNING, "Bad base for conversion: %d", base);
		RETURN_FALSE;
	}

	num_len = mpz_sizeinbase(*gmpnum, base);
	out_string = emalloc(num_len+2);
	if(mpz_sgn(*gmpnum) < 0) {
		num_len++;
	}
	mpz_get_str(out_string, base, *gmpnum);

	/* 
	From GMP documentation for mpz_sizeinbase():
	The returned value will be exact or 1 too big.  If base is a power of
	2, the returned value will always be exact.

	So let's check to see if we already have a \0 byte...
	*/

	if (out_string[num_len-1] == '\0')
		num_len--;
	else
		out_string[num_len] = '\0';

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
	zval **a_arg, **b_arg, **round_arg;
	int round=GMP_ROUND_ZERO, argc;

	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 3 || zend_get_parameters_ex(argc, &a_arg, &b_arg, &round_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	switch (argc) {
		case 3:
			convert_to_long_ex(round_arg);
			round = Z_LVAL_PP(round_arg);
			break;
		case 2:
			round = GMP_ROUND_ZERO;
			break;
	}

	switch(round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op2(return_value, a_arg, b_arg, mpz_tdiv_qr, (gmp_binary_ui_op2_t)mpz_tdiv_qr_ui);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op2(return_value, a_arg, b_arg, mpz_cdiv_qr, (gmp_binary_ui_op2_t)mpz_cdiv_qr_ui);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op2(return_value, a_arg, b_arg, mpz_fdiv_qr, (gmp_binary_ui_op2_t)mpz_fdiv_qr_ui);
		break;
	}
							   
}
/* }}} */

/* {{{ proto resource gmp_div_r(resource a, resource b [, int round])
   Divide a by b, returns reminder only */
ZEND_FUNCTION(gmp_div_r)
{
	zval **a_arg, **b_arg, **round_arg;
	int round=GMP_ROUND_ZERO, argc;

	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 3 || zend_get_parameters_ex(argc, &a_arg, &b_arg, &round_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	switch (argc) {
		case 3:
			convert_to_long_ex(round_arg);
			round = Z_LVAL_PP(round_arg);
			break;
		case 2:
			round = GMP_ROUND_ZERO;
			break;
	}

	switch(round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_tdiv_r, (gmp_binary_ui_op_t)mpz_tdiv_r_ui, 1 TSRMLS_CC);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_cdiv_r, (gmp_binary_ui_op_t)mpz_cdiv_r_ui, 1 TSRMLS_CC);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_fdiv_r, (gmp_binary_ui_op_t)mpz_fdiv_r_ui, 1 TSRMLS_CC);
		break;
	}
}
/* }}} */

/* {{{ proto resource gmp_div_q(resource a, resource b [, int round])
   Divide a by b, returns quotient only */
ZEND_FUNCTION(gmp_div_q)
{
	zval **a_arg, **b_arg, **round_arg;
	int round=GMP_ROUND_ZERO, argc;

	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 3 || zend_get_parameters_ex(argc, &a_arg, &b_arg, &round_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	switch (argc) {
		case 3:
			convert_to_long_ex(round_arg);
			round = Z_LVAL_PP(round_arg);
			break;
		case 2:
			round = GMP_ROUND_ZERO;
			break;
	}

	switch(round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op(return_value, a_arg, b_arg, mpz_tdiv_q, (gmp_binary_ui_op_t)mpz_tdiv_q_ui);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op(return_value, a_arg, b_arg, mpz_cdiv_q, (gmp_binary_ui_op_t)mpz_cdiv_q_ui);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op(return_value, a_arg, b_arg, mpz_fdiv_q, (gmp_binary_ui_op_t)mpz_fdiv_q_ui);
		break;
	}
							   
}
/* }}} */

/* {{{ proto resource gmp_mod(resource a, resource b)
   Computes a modulo b */
ZEND_FUNCTION(gmp_mod)
{
	zval **a_arg, **b_arg;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &b_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_mod, (gmp_binary_ui_op_t)mpz_mod_ui, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto resource gmp_divexact(resource a, resource b)
   Divide a by b using exact division algorithm */
ZEND_FUNCTION(gmp_divexact)
{
	gmp_binary_op(mpz_divexact);
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
	gmp_unary_ui_op(mpz_fac_ui);
}
/* }}} */

/* {{{ proto resource gmp_pow(resource base, int exp)
   Raise base to power exp */
ZEND_FUNCTION(gmp_pow)
{
	zval **base_arg, **exp_arg;
	mpz_t *gmpnum_result, *gmpnum_base;
	int use_ui=0;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &base_arg, &exp_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(Z_TYPE_PP(base_arg) == IS_LONG && Z_LVAL_PP(base_arg) >= 0) {
		use_ui=1;
	} else {
		FETCH_GMP_ZVAL(gmpnum_base, base_arg);
	}

	convert_to_long_ex(exp_arg);

	if(Z_LVAL_PP(exp_arg) < 0) {
		zend_error(E_WARNING,"Negative exponent not supported");
		RETURN_FALSE;
	}
	
	INIT_GMP_NUM(gmpnum_result);
	if(use_ui) {
		mpz_ui_pow_ui(*gmpnum_result, Z_LVAL_PP(base_arg), Z_LVAL_PP(exp_arg));
	} else {
		mpz_pow_ui(*gmpnum_result, *gmpnum_base, Z_LVAL_PP(exp_arg));
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
	int use_ui=0;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &base_arg, &exp_arg, &mod_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_base, base_arg);
	if(Z_TYPE_PP(exp_arg) == IS_LONG && Z_LVAL_PP(exp_arg) >= 0) {
		use_ui=1;
	} else {
		FETCH_GMP_ZVAL(gmpnum_exp, exp_arg);
	}
	FETCH_GMP_ZVAL(gmpnum_mod, mod_arg);

	INIT_GMP_NUM(gmpnum_result);
	if(use_ui) {
		mpz_powm_ui(*gmpnum_result, *gmpnum_base, (unsigned long)Z_LVAL_PP(exp_arg), *gmpnum_mod);
	} else {
		mpz_powm(*gmpnum_result, *gmpnum_base, *gmpnum_exp, *gmpnum_mod);
	}

	ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);

}
/* }}} */

/* {{{ proto resource gmp_sqrt(resource a)
   Takes integer part of square root of a */
ZEND_FUNCTION(gmp_sqrt)
{
	gmp_unary_op(mpz_sqrt);
}
/* }}} */

/* {{{ proto array gmp_sqrtrem(resource a)
   Square root with remainder */
ZEND_FUNCTION(gmp_sqrtrem)
{
	zval **a_arg;
	mpz_t *gmpnum_a, *gmpnum_result1, *gmpnum_result2;
	zval r;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &a_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);

	INIT_GMP_NUM(gmpnum_result1);
	INIT_GMP_NUM(gmpnum_result2);

	mpz_sqrtrem(*gmpnum_result1, *gmpnum_result2, *gmpnum_a);

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

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &a_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);

	RETURN_BOOL((mpz_perfect_square_p(*gmpnum_a)!=0));
}
/* }}} */

/* {{{ proto int gmp_prob_prime(resource a[, int reps])
   Checks if a is "probably prime" */
ZEND_FUNCTION(gmp_prob_prime)
{
	zval **gmpnumber_arg, **reps_arg;
	mpz_t *gmpnum_a;
	int argc, reps=10;

	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &gmpnumber_arg, &reps_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_a, gmpnumber_arg);

	switch (argc) {
		case 2:
			convert_to_long_ex(reps_arg);
			reps = Z_LVAL_PP(reps_arg);
			break;
		case 1:
			reps = 10;
			break;
	}

	RETURN_LONG(mpz_probab_prime_p(*gmpnum_a, reps));
}
/* }}} */

/* {{{ proto resource gmp_gcd(resource a, resource b)
   Computes greatest common denominator (gcd) of a and b */
ZEND_FUNCTION(gmp_gcd)
{
	zval **a_arg, **b_arg;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &b_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	gmp_zval_binary_ui_op_ex(return_value, a_arg, b_arg, mpz_gcd, (gmp_binary_ui_op_t)mpz_gcd_ui, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto array gmp_gcdext(resource a, resource b)
   Computes G, S, and T, such that AS + BT = G = `gcd' (A, B) */
ZEND_FUNCTION(gmp_gcdext)
{
	zval **a_arg, **b_arg;
	mpz_t *gmpnum_a, *gmpnum_b, *gmpnum_t, *gmpnum_s, *gmpnum_g;
	zval r;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &b_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);
	FETCH_GMP_ZVAL(gmpnum_b, a_arg);

	INIT_GMP_NUM(gmpnum_g);
	INIT_GMP_NUM(gmpnum_s);
	INIT_GMP_NUM(gmpnum_t);

	mpz_gcdext(*gmpnum_g, *gmpnum_s, *gmpnum_t, *gmpnum_a, *gmpnum_b);

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

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &b_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);
	FETCH_GMP_ZVAL(gmpnum_b, b_arg);

	INIT_GMP_NUM(gmpnum_result);
	if(mpz_invert(*gmpnum_result, *gmpnum_a, *gmpnum_b)) {
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
	int use_si=0, res;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &b_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);

	if(Z_TYPE_PP(b_arg) == IS_LONG) {
		use_si=1;
	} else {
		FETCH_GMP_ZVAL(gmpnum_b, b_arg);
	}

	if(use_si) {
		res = mpz_cmp_si(*gmpnum_a, Z_LVAL_PP(b_arg));
	} else {
		res = mpz_cmp(*gmpnum_a, *gmpnum_b);
	}
	
	RETURN_LONG(res);
}
/* }}} */

/* {{{ proto int gmp_sign(resource a)
   Gets the sign of the number */
ZEND_FUNCTION(gmp_sign)
{
	zval **a_arg;
	mpz_t *gmpnum_a;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &a_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	FETCH_GMP_ZVAL(gmpnum_a, a_arg);

	RETURN_LONG(mpz_sgn(*gmpnum_a));
}
/* }}} */

/* {{{ proto resource gmp_random([int limiter])
   Gets random number */
ZEND_FUNCTION(gmp_random)
{
	zval **limiter_arg;
	int limiter, argc;
	mpz_t *gmpnum_result;

	argc = ZEND_NUM_ARGS();

	if (argc < 0  || argc > 1 || zend_get_parameters_ex(1, &limiter_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(argc) {
		convert_to_long_ex(limiter_arg);
		limiter = Z_LVAL_PP(limiter_arg);
	} else {
		limiter = 20;
	}

	INIT_GMP_NUM(gmpnum_result);
	mpz_random(*gmpnum_result, limiter);

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

/* {{{ proto resource gmp_xor(resource a, resource b)
   Calculates logical exclusive OR of a and b */
ZEND_FUNCTION(gmp_xor)
{
	/* use formula: a^b = (a|b)&^(a&b) */
	zval **a_arg, **b_arg;
	mpz_t *gmpnum_a, *gmpnum_b, *gmpnum_result, *gmpnum_t;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &b_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);
	FETCH_GMP_ZVAL(gmpnum_b, b_arg);

	INIT_GMP_NUM(gmpnum_result);
	INIT_GMP_NUM(gmpnum_t);

	mpz_and(*gmpnum_t, *gmpnum_a, *gmpnum_b);
	mpz_com(*gmpnum_t, *gmpnum_t);

	mpz_ior(*gmpnum_result, *gmpnum_a, *gmpnum_b);
	mpz_and(*gmpnum_result, *gmpnum_result, *gmpnum_t);

	FREE_GMP_NUM(gmpnum_t);

	ZEND_REGISTER_RESOURCE(return_value, gmpnum_result, le_gmp);
}
/* }}} */

/* {{{ proto void gmp_setbit(resource &a, int index[, bool set_clear])
   Sets or clear bit in a */
ZEND_FUNCTION(gmp_setbit)
{
	zval **a_arg, **ind_arg, **set_c_arg;
	int argc, index, set=1;
	mpz_t *gmpnum_a;

	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 3 || zend_get_parameters_ex(argc, &a_arg, &ind_arg, &set_c_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(gmpnum_a, mpz_t *, a_arg, -1, GMP_RESOURCE_NAME, le_gmp);

	convert_to_long_ex(ind_arg);
	index = Z_LVAL_PP(ind_arg);

	switch (argc) {
		case 3:
			convert_to_long_ex(set_c_arg);
			set = Z_LVAL_PP(set_c_arg);
			break;
		case 2:
			set = 1;
			break;
	}

	if(set) {
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
	zval **a_arg, **ind_arg;
	int index;
	mpz_t *gmpnum_a;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &ind_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(gmpnum_a, mpz_t *, a_arg, -1, GMP_RESOURCE_NAME, le_gmp);

	convert_to_long_ex(ind_arg);
	index = Z_LVAL_PP(ind_arg);

	mpz_clrbit(*gmpnum_a, index);
}
/* }}} */

/* {{{ proto int gmp_popcount(resource a)
   Calculates the population count of a */
ZEND_FUNCTION(gmp_popcount)
{
	zval **a_arg;
	mpz_t *gmpnum_a;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &a_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	FETCH_GMP_ZVAL(gmpnum_a, a_arg);

	RETURN_LONG(mpz_popcount(*gmpnum_a));
}
/* }}} */

/* {{{ proto int gmp_hamdist(resource a, resource b)
   Calculates hamming distance between a and b */
ZEND_FUNCTION(gmp_hamdist)
{
	zval **a_arg, **b_arg;
	mpz_t *gmpnum_a, *gmpnum_b;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(1, &a_arg, &b_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	FETCH_GMP_ZVAL(gmpnum_a, a_arg);
	FETCH_GMP_ZVAL(gmpnum_b, b_arg);

	RETURN_LONG(mpz_hamdist(*gmpnum_a, *gmpnum_b));
}
/* }}} */

/* {{{ proto int gmp_scan0(resource a, int start)
   Finds first zero bit */
ZEND_FUNCTION(gmp_scan0)
{
	zval **a_arg, **start_arg;
	mpz_t *gmpnum_a;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &start_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);
	convert_to_long_ex(start_arg);

	RETURN_LONG(mpz_scan0(*gmpnum_a, Z_LVAL_PP(start_arg)));
}
/* }}} */

/* {{{ proto int gmp_scan1(resource a, int start)
   Finds first non-zero bit */
ZEND_FUNCTION(gmp_scan1)
{
	zval **a_arg, **start_arg;
	mpz_t *gmpnum_a;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &a_arg, &start_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg);
	convert_to_long_ex(start_arg);

	RETURN_LONG(mpz_scan1(*gmpnum_a, Z_LVAL_PP(start_arg)));
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
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
