/*
  +----------------------------------------------------------------------+
  | Zend Big Integer Support - GMP backend                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 2014 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrea Faulds <ajf@ajf.me>                                  |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <gmp.h>

#include "zend.h"
#include "zend_types.h"
#include "zend_bigint.h"
#include "zend_string.h"

struct _zend_bigint {
    zend_refcounted   gc;
    mpz_t             mpz;
};

/*** INTERNAL MACROS ***/

/* Convenience macro to create a temporary mpz_t from a zend_long
 * Used when gmp has no built-in function for an operation taking a long/zend_long */
#define WITH_TEMP_MPZ_FROM_ZEND_LONG(long, temp, codeblock) { \
	mpz_t temp;				\
	mpz_init(temp);			\
	mpz_set_si(temp, long); \
	codeblock				\
	mpz_clear(temp);		\
}

/* Convenience macro to create a temporary mpq_t from an mpz_t
 * We use this to get higher accuracy in floating-point division */
#define WITH_TEMP_MPQ_FROM_MPZ(mpz, temp, codeblock) { \
	mpq_t temp;				\
	mpq_init(temp);			\
	mpq_set_z(temp, mpz);	\
	codeblock				\
	mpq_clear(temp);		\
}

/* Convenience macro to create a temporary mpq_t from a zend_long
 * We use this to get higher accuracy in floating-point division */
#define WITH_TEMP_MPQ_FROM_ZEND_LONG(long, temp, codeblock) { \
	mpq_t temp;				\
	mpq_init(temp);			\
	mpq_set_si(temp, long, 1); \
	codeblock				\
	mpq_clear(temp);		\
}

/* Throws error if gmp "limbs" exceed maximum permissible
 * We use this to catch GMP's "overflow" scenarios before GMP does
 * We need to do this because GMP will print an error and abort() otherwise 
 * We don't actually calculate the actual number of limbs, just maximum that
 * could be produced */
#define assert_limbs_within_limits(_new_limbs) { 					\
	size_t new_limbs = (_new_limbs);								\
	/* This logic is based on logic for gmp overflow error in gmp's source
	 * See /mpz/realloc.c there */ 									\
	if (sizeof(size_t) == sizeof (int))	{							\
		if (UNEXPECTED(new_limbs > ULONG_MAX / GMP_NUMB_BITS)) {	\
			zend_error(E_ERROR, "Result of integer operation would be too large to represent"); \
			return;													\
		}															\
	} else {														\
		if (UNEXPECTED(new_limbs > (size_t)INT_MAX)) {				\
			zend_error(E_ERROR, "Result of integer operation would be too large to represent"); \
			return;													\
		}															\
	}																\
}

/* Sign functions */
#define int_sgn(n) ((n) > 0 ? 1 : ((n) < 0 ? -1 : 0))
#define big_sgn(n) (mpz_sgn((n)->mpz))

/*** INTERNAL FUNCTIONS ***/

/* Gets number of gmp "limbs" a long would occupy were it a bigint
 * i.e. ceil((ceil(log2(abs(lval)) + 1)) / GMP_NUMB_BITS)
 * Ignores sign (as gmp does)
 * TODO: Optimise to use bsr (x86 asm)/clz (ARM asm) */
static zend_always_inline size_t long_limbs(zend_long lval) {
	int used;
	frexp((double)lval, &used);
	return (size_t)(used / GMP_NUMB_BITS + 1);
}

/* Gets number of gmp "limbs" pow(2, a ulong) would occupy were it a bigint
 * i.e. ceil((ceil(log2(pow(2, ulval)) + 1)) / GMP_NUMB_BITS) */
static zend_always_inline size_t long_limbs_2exp(zend_ulong ulval) {
	return ulval / GMP_NUMB_BITS + 1;
}

/* Gets number of gmp "limbs" used in a bigint
 * Ignores sign */
static zend_always_inline size_t bigint_limbs(const zend_bigint *big) {
	return mpz_size(big->mpz);
}

/* emalloc/realloc/free are macros, not functions, so we need wrappers to pass
   as our custom allocators */
static void* gmp_emalloc(size_t size)
{
	return emalloc(size);
}
static void* gmp_erealloc(void *ptr, size_t old_size, size_t new_size)
{
	return erealloc(ptr, new_size);
}
static void gmp_efree(void *ptr, size_t size)
{
	efree(ptr);
}

/* All documentation for bigint functions is found in zend_bigint.h */

void zend_startup_bigint(void)
{
	mp_set_memory_functions(gmp_emalloc, gmp_erealloc, gmp_efree);
}

/*** INITIALISERS ***/

ZEND_API zend_bigint* zend_bigint_alloc(void) /* {{{ */
{
	return emalloc(sizeof(zend_bigint));
}
/* }}} */

ZEND_API void zend_bigint_init(zend_bigint *big) /* {{{ */
{
	GC_REFCOUNT(big) = 1;
	GC_TYPE_INFO(big) = IS_BIGINT;
	mpz_init(big->mpz);
}
/* }}} */

ZEND_API zend_bigint* zend_bigint_init_alloc(void) /* {{{ */
{
	zend_bigint *return_value;
	return_value = zend_bigint_alloc();
	zend_bigint_init(return_value);
	return return_value;
}
/* }}} */

ZEND_API int zend_bigint_init_from_string(zend_bigint *big, const char *str, int base) /* {{{ */
{
	zend_bigint_init(big);
	if (mpz_set_str(big->mpz, str, base) < 0) {
		mpz_clear(big->mpz);
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_bigint_init_from_string_length(zend_bigint *big, const char *str, size_t length, int base) /* {{{ */
{
	char *temp_str = estrndup(str, length);
	zend_bigint_init(big);
	if (mpz_set_str(big->mpz, temp_str, base) < 0) {
		mpz_clear(big->mpz);
		efree(temp_str);
		return FAILURE;
	}
	efree(temp_str);
	return SUCCESS;
}
/* }}} */

ZEND_API void zend_bigint_init_strtol(zend_bigint *big, const char *str, char** endptr, int base) /* {{{ */
{
	size_t len = 0;

	/* Skip leading whitespace */
	while (isspace(*str)) {
		str++;
	}

	/* A single sign is valid */
	if (str[0] == '+' || str[0] == '-') {
		len += 1;
	}

	/* detect hex prefix */
	if (base == 0) {
		if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
			base = 16;
			str += 2;
		} else {
			base = 10;
		}
	}

	if (base == 10) {
		while (isdigit(str[len])) {
			len++;
		}
	} else if (base == 16) {
		while (isxdigit(str[len])) {
			len++;
		}
	}

	zend_bigint_init(big);
	if (len) {
		char *temp_str = estrndup(str, len);
		/* we ignore the return value since if it fails it'll just be zero anyway */
		mpz_set_str(big->mpz, temp_str, base);
		efree(temp_str);
	}

	if (endptr) {
		*endptr = (char*)(str + len);
	}
}
/* }}} */

ZEND_API void zend_bigint_init_from_long(zend_bigint *big, zend_long value) /* {{{ */
{
	zend_bigint_init(big);
	mpz_set_si(big->mpz, value);
}
/* }}} */

ZEND_API void zend_bigint_init_from_double(zend_bigint *big, double value) /* {{{ */
{
	zend_bigint_init(big);
	/* prevents crash */
	if (zend_finite(value) && !zend_isnan(value)) {
		mpz_set_d(big->mpz, value);
	}
}
/* }}} */

ZEND_API void zend_bigint_init_dup(zend_bigint *big, const zend_bigint *source) /* {{{ */
{
	zend_bigint_init(big);
	mpz_set(big->mpz, source->mpz);
}
/* }}} */

ZEND_API void zend_bigint_dtor(zend_bigint *big) /* {{{ */
{
	mpz_clear(big->mpz);
}
/* }}} */

ZEND_API void zend_bigint_release(zend_bigint *big) /* {{{ */
{
	if (--GC_REFCOUNT(big) <= 0) {
		zend_bigint_dtor(big);
		efree(big);
	}
}
/* }}} */

/*** INFORMATION ***/

ZEND_API zend_bool zend_bigint_can_fit_ulong(const zend_bigint *big) /* {{{ */
{
	return mpz_fits_ulong_p(big->mpz);
}
/* }}} */

ZEND_API zend_bool zend_bigint_can_fit_long(const zend_bigint *big) /* {{{ */
{
	return mpz_fits_slong_p(big->mpz);
}
/* }}} */

ZEND_API int zend_bigint_sign(const zend_bigint *big) /* {{{ */
{
	return mpz_sgn(big->mpz);
}
/* }}} */

ZEND_API zend_bool zend_bigint_divisible(const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	return mpz_divisible_p(num->mpz, divisor->mpz) ? 1 : 0;
}
/* }}} */

ZEND_API zend_bool zend_bigint_divisible_long(const zend_bigint *num, zend_long divisor) /* {{{ */
{
	zend_bool return_value;
	WITH_TEMP_MPZ_FROM_ZEND_LONG(divisor, divisor_mpz, {
		return_value = mpz_divisible_p(num->mpz, divisor_mpz) ? 1 : 0;
	})
	return return_value;
}
/* }}} */

ZEND_API zend_bool zend_bigint_long_divisible(zend_long num, const zend_bigint *divisor) /* {{{ */
{
	zend_bool return_value;
	WITH_TEMP_MPZ_FROM_ZEND_LONG(num, num_mpz, {
		return_value = mpz_divisible_p(num_mpz, divisor->mpz) ? 1 : 0;
	})
	return return_value;
}
/* }}} */

/*** CONVERTORS ***/

ZEND_API zend_long zend_bigint_to_long(const zend_bigint *big) /* {{{ */
{
	mpz_t bmod;
	zend_long result;
	mpz_init(bmod);
	
	mpz_tdiv_r_2exp(bmod, big->mpz, SIZEOF_ZEND_LONG * 8);
	
	if (mpz_sgn(bmod) < 0) {
		/* we're going to make this number positive */
		mpz_t two_pow_bits;
		mpz_init(two_pow_bits);
		
		mpz_ui_pow_ui(two_pow_bits, 2, SIZEOF_ZEND_LONG * 8);
		mpz_add(bmod, bmod, two_pow_bits);
		
		mpz_clear(two_pow_bits);
	}
	
	/* ulong -> long conversion is deliberate */
	result = (zend_long)mpz_get_ui(bmod);
	mpz_clear(bmod);
	
	return result;
}
/* }}} */

ZEND_API zend_long zend_bigint_to_long_saturate(const zend_bigint *big) /* {{{ */
{
	if (mpz_fits_slong_p(big->mpz)) {
		return mpz_get_si(big->mpz);
	} else {
		if (mpz_sgn(big->mpz) == 1) {
			return ZEND_LONG_MAX;
		} else {
			return ZEND_LONG_MIN;
		}
	}
}
/* }}} */

ZEND_API zend_long zend_bigint_to_long_ex(const zend_bigint *big, zend_bool *overflow) /* {{{ */
{
	*overflow = mpz_fits_slong_p(big->mpz);
	return mpz_get_si(big->mpz);
}
/* }}} */

ZEND_API zend_ulong zend_bigint_to_ulong(const zend_bigint *big) /* {{{ */
{
	if (mpz_fits_ulong_p(big->mpz)) {
		return mpz_get_ui(big->mpz);
	} else {
		if (mpz_sgn(big->mpz) == 1) {
			return ZEND_ULONG_MAX;
		} else {
			return 0;
		}
	}
}
/* }}} */

ZEND_API zend_bool zend_bigint_to_bool(const zend_bigint *big) /* {{{ */
{
	return mpz_sgn(big->mpz) ? 1 : 0;
}
/* }}} */

ZEND_API double zend_bigint_to_double(const zend_bigint *big) /* {{{ */
{
	return mpz_get_d(big->mpz);
}
/* }}} */

ZEND_API char* zend_bigint_to_string(const zend_bigint *big) /* {{{ */
{
	return mpz_get_str(NULL, 10, big->mpz);
}
/* }}} */

ZEND_API zend_string* zend_bigint_to_zend_string(const zend_bigint *big, int persistent) /* {{{ */
{
	char *temp_string = zend_bigint_to_string(big);
	zend_string *return_value = zend_string_init(temp_string, strlen(temp_string), persistent);
	efree(temp_string);
	return return_value;
}
/* }}} */

ZEND_API char* zend_bigint_to_string_base(const zend_bigint *big, int base) /* {{{ */
{
	return mpz_get_str(NULL, base, big->mpz);
}
/* }}} */

/*** OPERATIONS **/

ZEND_API void zend_bigint_add(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	assert_limbs_within_limits((big_sgn(op1) * big_sgn(op2) > 0)
		? bigint_limbs(op1) + bigint_limbs(op2) + 1
		: 0 ); /* if it's a subtraction, the result is guaranteed to be smaller */
	mpz_add(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

ZEND_API void zend_bigint_add_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	assert_limbs_within_limits((big_sgn(op1) * int_sgn(op2) > 0)
		? bigint_limbs(op1) + long_limbs(op2) + 1
		: 0 ); /* if it's a subtraction, the result is guaranteed to be smaller */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op2, op2_mpz, {
		mpz_add(out->mpz, op1->mpz, op2_mpz);
	})
}
/* }}} */

ZEND_API void zend_bigint_long_add_long(zend_bigint *out, zend_long op1, zend_long op2) /* {{{ */
{
	assert_limbs_within_limits((int_sgn(op1) * int_sgn(op2) > 0)
		? long_limbs(op1) + long_limbs(op2) + 1
		: 0 ); /* if it's a subtraction, the result is guaranteed to be smaller */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op1, op1_mpz, WITH_TEMP_MPZ_FROM_ZEND_LONG(op2, op2_mpz, {
		mpz_add(out->mpz, op1_mpz, op2_mpz);		
	}))
}
/* }}} */

ZEND_API void zend_bigint_subtract(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	assert_limbs_within_limits((big_sgn(op1) * big_sgn(op2) < 0)
		? bigint_limbs(op1) + bigint_limbs(op2) + 1
		: 0 ); /* if it's a subtraction, the result is guaranteed to be smaller */
	mpz_sub(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

ZEND_API void zend_bigint_subtract_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	assert_limbs_within_limits((big_sgn(op1) * int_sgn(op2) < 0)
		? bigint_limbs(op1) + long_limbs(op2) + 1
		: 0 ); /* if it's a subtraction, the result is guaranteed to be smaller */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op2, op2_mpz, {
		mpz_sub(out->mpz, op1->mpz, op2_mpz);
	})
}
/* }}} */

ZEND_API void zend_bigint_long_subtract_long(zend_bigint *out, zend_long op1, zend_long op2) /* {{{ */
{
	assert_limbs_within_limits((int_sgn(op1) * int_sgn(op2) < 0)
		? long_limbs(op1) + long_limbs(op2) + 1
		: 0 ); /* if it's a subtraction, the result is guaranteed to be smaller */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op1, op1_mpz, WITH_TEMP_MPZ_FROM_ZEND_LONG(op2, op2_mpz, {
		mpz_sub(out->mpz, op1_mpz, op2_mpz);
	}))
}
/* }}} */

ZEND_API void zend_bigint_long_subtract(zend_bigint *out, zend_long op1, const zend_bigint *op2) /* {{{ */
{
	assert_limbs_within_limits((int_sgn(op1) * big_sgn(op2) < 0)
		? long_limbs(op1) + bigint_limbs(op2) + 1
		: 0 ); /* if it's a subtraction, the result is guaranteed to be smaller */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op1, op1_mpz, {
		mpz_sub(out->mpz, op1_mpz, op2->mpz);
	})
}
/* }}} */

ZEND_API void zend_bigint_multiply(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	assert_limbs_within_limits(bigint_limbs(op1) + bigint_limbs(op2));
	mpz_mul(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

ZEND_API void zend_bigint_multiply_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	assert_limbs_within_limits(bigint_limbs(op1) + long_limbs(op2));
	mpz_mul_si(out->mpz, op1->mpz, op2);
}
/* }}} */

ZEND_API void zend_bigint_long_multiply_long(zend_bigint *out, zend_long op1, zend_long op2) /* {{{ */
{
	assert_limbs_within_limits(long_limbs(op1) + long_limbs(op2));
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op1, op1_mpz, {
		mpz_mul_si(out->mpz, op1_mpz, op2);		
	})
}
/* }}} */

ZEND_API void zend_bigint_pow_ulong(zend_bigint *out, const zend_bigint *base, zend_ulong power) /* {{{ */
{
	assert_limbs_within_limits(bigint_limbs(base) * power);
	mpz_pow_ui(out->mpz, base->mpz, power);
}
/* }}} */

ZEND_API void zend_bigint_long_pow_ulong(zend_bigint *out, zend_long base, zend_ulong power) /* {{{ */
{
	assert_limbs_within_limits(long_limbs(base) * power);
	WITH_TEMP_MPZ_FROM_ZEND_LONG(base, base_mpz, {
		mpz_pow_ui(out->mpz, base_mpz, power);
	})
}
/* }}} */

ZEND_API void zend_bigint_divide(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	mpz_fdiv_q(out->mpz, num->mpz, divisor->mpz);
}
/* }}} */

ZEND_API double zend_bigint_divide_as_double(const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	/* We use rational arithmetic for higher accuracy
	   The alternative would be to convert both operands to double, then divide */
	double result;
	WITH_TEMP_MPQ_FROM_MPZ(num->mpz, num_mpq, WITH_TEMP_MPQ_FROM_MPZ(divisor->mpz, divisor_mpq, {
		mpq_div(num_mpq, num_mpq, divisor_mpq);
		result = mpq_get_d(num_mpq);
	}))
	
	return result;
}
/* }}} */

ZEND_API void zend_bigint_divide_long(zend_bigint *out, const zend_bigint *num, zend_long divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(divisor, divisor_mpz, {
		mpz_fdiv_q(out->mpz, num->mpz, divisor_mpz);
	})
}
/* }}} */

ZEND_API double zend_bigint_divide_long_as_double(const zend_bigint *num, zend_long divisor) /* {{{ */
{
	/* We use rational arithmetic for higher accuracy
	   The alternative would be to convert both operands to double, then divide */
	double result;
	WITH_TEMP_MPQ_FROM_MPZ(num->mpz, num_mpq, WITH_TEMP_MPQ_FROM_ZEND_LONG(divisor, divisor_mpq, {
		mpq_div(num_mpq, num_mpq, divisor_mpq);
		result = mpq_get_d(num_mpq);
	}))
	
	return result;
}
/* }}} */

ZEND_API void zend_bigint_long_divide(zend_bigint *out, zend_long num, const zend_bigint *divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(num, num_mpz, {
		mpz_fdiv_q(out->mpz, num_mpz, divisor->mpz);
	})
}
/* }}} */

ZEND_API double zend_bigint_long_divide_as_double(zend_long num, const zend_bigint *divisor) /* {{{ */
{
	/* We use rational arithmetic for higher accuracy
	   The alternative would be to convert both operands to double, then divide */
	double result;
	WITH_TEMP_MPQ_FROM_ZEND_LONG(num, num_mpq, WITH_TEMP_MPQ_FROM_MPZ(divisor->mpz, divisor_mpq, {
		mpq_div(num_mpq, num_mpq, divisor_mpq);
		result = mpq_get_d(num_mpq);
	}))
	
	return result;
}
/* }}} */

ZEND_API void zend_bigint_modulus(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	mpz_tdiv_r(out->mpz, num->mpz, divisor->mpz);
}
/* }}} */

ZEND_API void zend_bigint_modulus_long(zend_bigint *out, const zend_bigint *num, zend_long divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(divisor, divisor_mpz, {
		mpz_tdiv_r(out->mpz, num->mpz, divisor_mpz);
	})
}
/* }}} */

ZEND_API void zend_bigint_long_modulus(zend_bigint *out, zend_long num, const zend_bigint *divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(num, num_mpz, {
		mpz_tdiv_r(out->mpz, num_mpz, divisor->mpz);
	})
}
/* }}} */

ZEND_API void zend_bigint_ones_complement(zend_bigint *out, const zend_bigint *op) /* {{{ */
{
	/* The NOT of a positive value will be a bigger negative value but the reverse
	   is not true. Of course this doesn't apply for some very small values, but
	   they don't need asserting. */
	assert_limbs_within_limits(bigint_limbs(op) + (big_sgn(op) > 0));
	mpz_com(out->mpz, op->mpz);
}
/* }}} */

ZEND_API void zend_bigint_or(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	mpz_ior(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

ZEND_API void zend_bigint_or_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op2, op2_mpz, {
		mpz_ior(out->mpz, op1->mpz, op2_mpz);
	})
}
/* }}} */

ZEND_API void zend_bigint_and(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	mpz_and(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

ZEND_API void zend_bigint_and_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op2, op2_mpz, {
		mpz_and(out->mpz, op1->mpz, op2_mpz);
	})
}
/* }}} */

ZEND_API void zend_bigint_xor(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	mpz_xor(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

ZEND_API void zend_bigint_xor_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op2, op2_mpz, {
		mpz_xor(out->mpz, op1->mpz, op2_mpz);
	})
}
/* }}} */

ZEND_API void zend_bigint_shift_left_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift) /* {{{ */
{
	assert_limbs_within_limits(bigint_limbs(num) + long_limbs_2exp(shift));
	mpz_mul_2exp(out->mpz, num->mpz, shift);
}
/* }}} */

ZEND_API void zend_bigint_long_shift_left_ulong(zend_bigint *out, zend_long num, zend_ulong shift) /* {{{ */
{
	assert_limbs_within_limits(long_limbs(num) + long_limbs_2exp(shift));
	WITH_TEMP_MPZ_FROM_ZEND_LONG(num, num_mpz, {
		mpz_mul_2exp(out->mpz, num_mpz, shift);
	})
}
/* }}} */

ZEND_API void zend_bigint_shift_right_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift) /* {{{ */
{
	/* no need to assert; shift right always gives smaller result */
	mpz_fdiv_q_2exp(out->mpz, num->mpz, shift);
}
/* }}} */

ZEND_API int zend_bigint_cmp(const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	return mpz_cmp(op1->mpz, op2->mpz);
}
/* }}} */

ZEND_API int zend_bigint_cmp_long(const zend_bigint *op1, zend_long op2) /* {{{ */
{
	return mpz_cmp_si(op1->mpz, op2);
}
/* }}} */

ZEND_API int zend_bigint_cmp_double(const zend_bigint *op1, double op2) /* {{{ */
{
	return mpz_cmp_d(op1->mpz, op2);
}
/* }}} */

ZEND_API void zend_bigint_abs(zend_bigint *out, const zend_bigint *big) /* {{{ */
{
	/* no need to assert; only sign can change */
	mpz_abs(out->mpz, big->mpz);
}
