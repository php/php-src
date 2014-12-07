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

/* Called by zend_startup */
void zend_startup_bigint(void)
{
	mp_set_memory_functions(gmp_emalloc, gmp_erealloc, gmp_efree);
}

/*** INITIALISERS ***/

/* Allocates a bigint and returns pointer, does NOT initialise
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API zend_bigint* zend_bigint_alloc(void) /* {{{ */
{
	return emalloc(sizeof(zend_bigint));
}
/* }}} */

/* Initialises a bigint
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API void zend_bigint_init(zend_bigint *big) /* {{{ */
{
	GC_REFCOUNT(big) = 1;
	GC_TYPE_INFO(big) = IS_BIGINT;
	mpz_init(big->mpz);
}
/* }}} */

/* Convenience function: Allocates and initialises a bigint, returns pointer
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API zend_bigint* zend_bigint_init_alloc(void) /* {{{ */
{
	zend_bigint *return_value;
	return_value = zend_bigint_alloc();
	zend_bigint_init(return_value);
	return return_value;
}
/* }}} */

/* Initialises a bigint from a string with the specified base (in range 2-36)
 * Returns FAILURE on failure (if the string is not entirely numeric), else SUCCESS
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
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

/* Initialises a bigint from a string with the specified base (in range 2-36)
 * Takes a length - due to an extra memory allocation, this function is slower
 * Returns FAILURE on failure, else SUCCESS
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
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

/* Intialises a bigint from a C-string with the specified base (10 or 16)
 * If endptr is not NULL, it it set to point to first character after number
 * If base is zero, it shall be detected from the prefix: 0x/0X for 16, else 10
 * Leading whitespace is ignored, will take as many valid characters as possible
 * Stops at first non-valid character, else null byte
 * If there are no valid characters, the bigint is initialised to zero
 * This behaviour is supposed to match that of strtol but is not exactly the same
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
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

/* Initialises a bigint from a long */
ZEND_API void zend_bigint_init_from_long(zend_bigint *big, zend_long value) /* {{{ */
{
	zend_bigint_init(big);
	mpz_set_si(big->mpz, value);
}
/* }}} */

/* Initialises a bigint from a double
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API void zend_bigint_init_from_double(zend_bigint *big, double value) /* {{{ */
{
	zend_bigint_init(big);
	/* prevents crash */
	if (zend_finite(value) && !zend_isnan(value)) {
		mpz_set_d(big->mpz, value);
	}
}
/* }}} */

/* Initialises a bigint and duplicates a bigint to it (copies value)
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API void zend_bigint_init_dup(zend_bigint *big, const zend_bigint *source) /* {{{ */
{
	zend_bigint_init(big);
	mpz_set(big->mpz, source->mpz);
}
/* }}} */

/* Destroys a bigint (does NOT deallocate) */
ZEND_API void zend_bigint_dtor(zend_bigint *big) /* {{{ */
{
	mpz_clear(big->mpz);
}
/* }}} */

/* Decreases the refcount of a bigint and, if <= 0, destroys and frees it */
ZEND_API void zend_bigint_release(zend_bigint *big) /* {{{ */
{
	if (--GC_REFCOUNT(big) <= 0) {
		zend_bigint_dtor(big);
		efree(big);
	}
}
/* }}} */

/*** INFORMATION ***/

/* Returns true if bigint can fit into an unsigned long without truncation */
ZEND_API zend_bool zend_bigint_can_fit_ulong(const zend_bigint *big) /* {{{ */
{
	return mpz_fits_ulong_p(big->mpz);
}
/* }}} */

/* Returns true if bigint can fit into a long without truncation */
ZEND_API zend_bool zend_bigint_can_fit_long(const zend_bigint *big) /* {{{ */
{
	return mpz_fits_slong_p(big->mpz);
}
/* }}} */

/* Returns sign of bigint (-1 for negative, 0 for zero or 1 for positive) */
ZEND_API int zend_bigint_sign(const zend_bigint *big) /* {{{ */
{
	return mpz_sgn(big->mpz);
}
/* }}} */

/* Returns true if bigint is divisible by a bigint */
ZEND_API zend_bool zend_bigint_divisible(const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	return mpz_divisible_p(num->mpz, divisor->mpz) ? 1 : 0;
}
/* }}} */

/* Returns true if bigint is divisible by a long */
ZEND_API zend_bool zend_bigint_divisible_long(const zend_bigint *num, zend_long divisor) /* {{{ */
{
	zend_bool return_value;
	WITH_TEMP_MPZ_FROM_ZEND_LONG(divisor, divisor_mpz, {
		return_value = mpz_divisible_p(num->mpz, divisor_mpz) ? 1 : 0;
	})
	return return_value;
}
/* }}} */

/* Returns true if long is divisible by a bigint */
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

/* Converts to long; if it won't fit, wraps around (like zend_dval_to_lval) */
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
	
	result = mpz_get_si(bmod);
	mpz_clear(bmod);
	
	return result;
}
/* }}} */

/* Converts to long; if it won't fit, saturates (caps at ZEND_LONG_MAX/_MIN) */
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

/* Converts to long; if it won't fit, may return garbage
 * If it didn't fit, sets overflow to 1, else to 0 */
ZEND_API zend_long zend_bigint_to_long_ex(const zend_bigint *big, zend_bool *overflow) /* {{{ */
{
	*overflow = mpz_fits_slong_p(big->mpz);
	return mpz_get_si(big->mpz);
}
/* }}} */

/* Converts to unsigned long; this will cap at the max value of an unsigned long */
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

/* Converts to bool */
ZEND_API zend_bool zend_bigint_to_bool(const zend_bigint *big) /* {{{ */
{
	return mpz_sgn(big->mpz) ? 1 : 0;
}
/* }}} */

/* Converts to double; this will lose precision beyond a certain point */
ZEND_API double zend_bigint_to_double(const zend_bigint *big) /* {{{ */
{
	return mpz_get_d(big->mpz);
}
/* }}} */

/* Converts to decimal C string
 * HERE BE DRAGONS: String allocated is non-persistent */
ZEND_API char* zend_bigint_to_string(const zend_bigint *big) /* {{{ */
{
	return mpz_get_str(NULL, 10, big->mpz);
}
/* }}} */

/* Convenience function: Converts to zend string */
ZEND_API zend_string* zend_bigint_to_zend_string(const zend_bigint *big, int persistent) /* {{{ */
{
	char *temp_string = zend_bigint_to_string(big);
	zend_string *return_value = zend_string_init(temp_string, strlen(temp_string), persistent);
	efree(temp_string);
	return return_value;
}
/* }}} */

/* Converts to C string of arbitrary base */
ZEND_API char* zend_bigint_to_string_base(const zend_bigint *big, int base) /* {{{ */
{
	return mpz_get_str(NULL, base, big->mpz);
}
/* }}} */

/*** OPERATIONS **/

/* By the way, in case you're wondering, you can indeed use something as both
 * output and operand. For example, zend_bigint_add_long(foo, foo, 1) is
 * perfectly valid for incrementing foo. This is because gmp supports it, and
 * zend_bigint is (at the time of writing, at least) merely a thin wrapper
 * around gmp. This is not advisable, however, because bigints are reference-
 * counted and should be copy-on-write so far as userland PHP code cares. Do
 * it sparingly, and never to bigints which have been exposed to userland. With
 * great power comes great responsibility.
 */

/* Adds two bigints and stores result in out */
ZEND_API void zend_bigint_add(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	assert_limbs_within_limits((big_sgn(op1) * big_sgn(op2) > 0)
		? bigint_limbs(op1) + bigint_limbs(op2) + 1
		: 0 ); /* if it's a subtraction, the result is guaranteed to be smaller */
	mpz_add(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

/* Adds a bigint and a long and stores result in out */
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

/* Adds a long and a long and stores result in out */
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

/* Subtracts two bigints and stores result in out */
ZEND_API void zend_bigint_subtract(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	assert_limbs_within_limits((big_sgn(op1) * big_sgn(op2) < 0)
		? bigint_limbs(op1) + bigint_limbs(op2) + 1
		: 0 ); /* if it's a subtraction, the result is guaranteed to be smaller */
	mpz_sub(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

/* Subtracts a bigint and a long and stores result in out */
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

/* Subtracts a long and a long and stores result in out */
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

/* Subtracts a long and a bigint and stores result in out */
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

/* Multiplies two bigints and stores result in out */
ZEND_API void zend_bigint_multiply(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	assert_limbs_within_limits(bigint_limbs(op1) + bigint_limbs(op2));
	mpz_mul(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

/* Multiplies a bigint and a long and stores result in out */
ZEND_API void zend_bigint_multiply_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	assert_limbs_within_limits(bigint_limbs(op1) + long_limbs(op2));
	mpz_mul_si(out->mpz, op1->mpz, op2);
}
/* }}} */

/* Multiplies a long and a long and stores result in out */
ZEND_API void zend_bigint_long_multiply_long(zend_bigint *out, zend_long op1, zend_long op2) /* {{{ */
{
	assert_limbs_within_limits(long_limbs(op1) + long_limbs(op2));
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op1, op1_mpz, {
		mpz_mul_si(out->mpz, op1_mpz, op2);		
	})
}
/* }}} */

/* Raises a bigint base to an unsigned long power and stores result in out */
ZEND_API void zend_bigint_pow_ulong(zend_bigint *out, const zend_bigint *base, zend_ulong power) /* {{{ */
{
	assert_limbs_within_limits(bigint_limbs(base) * power);
	mpz_pow_ui(out->mpz, base->mpz, power);
}
/* }}} */

/* Raises a long base to an unsigned long power and stores result in out */
ZEND_API void zend_bigint_long_pow_ulong(zend_bigint *out, zend_long base, zend_ulong power) /* {{{ */
{
	assert_limbs_within_limits(long_limbs(base) * power);
	WITH_TEMP_MPZ_FROM_ZEND_LONG(base, base_mpz, {
		mpz_pow_ui(out->mpz, base_mpz, power);
	})
}
/* }}} */

/* Divides a bigint by a bigint and stores result in out */
ZEND_API void zend_bigint_divide(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	mpz_fdiv_q(out->mpz, num->mpz, divisor->mpz);
}
/* }}} */

/* Divides a bigint by a bigint and returns result as a double */
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

/* Divides a bigint by a long and stores result in out */
ZEND_API void zend_bigint_divide_long(zend_bigint *out, const zend_bigint *num, zend_long divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(divisor, divisor_mpz, {
		mpz_fdiv_q(out->mpz, num->mpz, divisor_mpz);
	})
}
/* }}} */

/* Divides a bigint by a long and returns result as a double */
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

/* Divides a long by a bigint and stores result in out */
ZEND_API void zend_bigint_long_divide(zend_bigint *out, zend_long num, const zend_bigint *divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(num, num_mpz, {
		mpz_fdiv_q(out->mpz, num_mpz, divisor->mpz);
	})
}
/* }}} */

/* Divides a long by a bigint and returns result as a double */
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

/* Finds the remainder of the division of a bigint by a bigint and stores result in out */
ZEND_API void zend_bigint_modulus(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	mpz_tdiv_r(out->mpz, num->mpz, divisor->mpz);
}
/* }}} */

/* Finds the remainder of the division of a bigint by a long and stores result in out */
ZEND_API void zend_bigint_modulus_long(zend_bigint *out, const zend_bigint *num, zend_long divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(divisor, divisor_mpz, {
		mpz_tdiv_r(out->mpz, num->mpz, divisor_mpz);
	})
}
/* }}} */

/* Finds the remainder of the division of a long by a bigint and stores result in out */
ZEND_API void zend_bigint_long_modulus(zend_bigint *out, zend_long num, const zend_bigint *divisor) /* {{{ */
{
	/* no need to assert as division will always return a smaller result */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(num, num_mpz, {
		mpz_tdiv_r(out->mpz, num_mpz, divisor->mpz);
	})
}
/* }}} */

/* Finds the one's complement of a bigint and stores result in out */
ZEND_API void zend_bigint_ones_complement(zend_bigint *out, const zend_bigint *op) /* {{{ */
{
	/* The NOT of a positive value will be a bigger negative value but the reverse
	   is not true. Of course this doesn't apply for some very small values, but
	   they don't need asserting. */
	assert_limbs_within_limits(bigint_limbs(op) + (big_sgn(op) > 0));
	mpz_com(out->mpz, op->mpz);
}
/* }}} */

/* Finds the bitwise OR of a bigint and a bigint and stores result in out */
ZEND_API void zend_bigint_or(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	mpz_ior(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

/* Finds the bitwise OR of a bigint and a long and stores result in out */
ZEND_API void zend_bigint_or_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op2, op2_mpz, {
		mpz_ior(out->mpz, op1->mpz, op2_mpz);
	})
}
/* }}} */

/* Finds the bitwise AND of a bigint and a bigint and stores result in out */
ZEND_API void zend_bigint_and(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	mpz_and(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

/* Finds the bitwise AND of a bigint and a long and stores result in out */
ZEND_API void zend_bigint_and_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op2, op2_mpz, {
		mpz_and(out->mpz, op1->mpz, op2_mpz);
	})
}
/* }}} */

/* Finds the bitwise XOR of a bigint and a bigint and stores result in out */
ZEND_API void zend_bigint_xor(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	mpz_xor(out->mpz, op1->mpz, op2->mpz);
}
/* }}} */

/* Finds the bitwise XOR of a bigint and a long and stores result in out */
ZEND_API void zend_bigint_xor_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* no need to assert; these ops never give a bigger result than operand */
	WITH_TEMP_MPZ_FROM_ZEND_LONG(op2, op2_mpz, {
		mpz_xor(out->mpz, op1->mpz, op2_mpz);
	})
}
/* }}} */

/* Shifts a bigint left by an unsigned long and stores result in out */
ZEND_API void zend_bigint_shift_left_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift) /* {{{ */
{
	assert_limbs_within_limits(bigint_limbs(num) + long_limbs_2exp(shift));
	mpz_mul_2exp(out->mpz, num->mpz, shift);
}
/* }}} */

/* Shifts a long left by an unsigned long and stores result in out */
ZEND_API void zend_bigint_long_shift_left_ulong(zend_bigint *out, zend_long num, zend_ulong shift) /* {{{ */
{
	assert_limbs_within_limits(long_limbs(num) + long_limbs_2exp(shift));
	WITH_TEMP_MPZ_FROM_ZEND_LONG(num, num_mpz, {
		mpz_mul_2exp(out->mpz, num_mpz, shift);
	})
}
/* }}} */

/* Shifts a bigint right by an unsigned long and stores result in out */
ZEND_API void zend_bigint_shift_right_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift) /* {{{ */
{
	/* no need to assert; shift right always gives smaller result */
	mpz_fdiv_q_2exp(out->mpz, num->mpz, shift);
}
/* }}} */

/* Compares a bigint and a bigint and returns result (negative if op1 > op2, zero if op1 == op2, positive if op1 < op2) */
ZEND_API int zend_bigint_cmp(const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	return mpz_cmp(op1->mpz, op2->mpz);
}
/* }}} */

/* Compares a bigint and a long and returns result (negative if op1 > op2, zero if op1 == op2, positive if op1 < op2) */
ZEND_API int zend_bigint_cmp_long(const zend_bigint *op1, zend_long op2) /* {{{ */
{
	return mpz_cmp_si(op1->mpz, op2);
}
/* }}} */

/* Compares a bigint and a double and returns result (negative if op1 > op2, zero if op1 == op2, positive if op1 < op2) */
ZEND_API int zend_bigint_cmp_double(const zend_bigint *op1, double op2) /* {{{ */
{
	return mpz_cmp_d(op1->mpz, op2);
}
/* }}} */

/* Finds the absolute value of a bigint and stores result in out */
ZEND_API void zend_bigint_abs(zend_bigint *out, const zend_bigint *big) /* {{{ */
{
	/* no need to assert; only sign can change */
	mpz_abs(out->mpz, big->mpz);
}
