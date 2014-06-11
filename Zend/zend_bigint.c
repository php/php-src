/*
  +----------------------------------------------------------------------+
  | Zend Big Integer Support                                             |
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
#include <limits.h>
#include <gmp.h>

#include "zend.h"
#include "zend_types.h"
#include "zend_bigint.h"
#include "zend_string.h"

/*** INTERNAL MACROS ***/

#define WITH_TEMP_MPZ_FROM_LONG(long, temp, codeblock) { \
	mpz_t temp;				\
	mpz_init(temp);			\
	mpz_set_si(temp, long); \
	codeblock				\
	mpz_clear(temp);		\
}

/*** INTERNAL FUNCTIONS ***/

/* emalloc/realloc/free are macros, not functions, so we make them functions */
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

/* Initialises a bigint
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API void zend_bigint_init(zend_bigint *big)
{
	GC_REFCOUNT(big) = 1;
	GC_TYPE_INFO(big) = IS_BIGINT;
	mpz_init(big->mpz);
}

/* Convenience function: Allocates non-persistently and initialises a bigint
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API zend_bigint* zend_bigint_init_alloc(void)
{
	zend_bigint *return_value;
	return_value = emalloc(sizeof(zend_bigint));
	zend_bigint_init(return_value);
	return return_value;
}

/* Initialises a bigint from a string with the specified base (in range 2-36)
 * Returns FAILURE on failure (if the string is not entirely numeric), else SUCCESS
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API int zend_bigint_init_from_string(zend_bigint *big, const char *str, int base)
{
	zend_bigint_init(big);
	if (mpz_set_str(big->mpz, str, base) < 0) {
		mpz_clear(big->mpz);
		return FAILURE;
	}
	return SUCCESS;
}

/* Intialises a bigint from a string with the specified base (in range 2-36)
 * If base is zero, it shall be detected from the prefix (0 for octal, 0x/X for hex, 0b/B for binary)
 * Leading whitespace is ignored, will take as many valid characters as possible
 * Stops at first non-valid character, or end of string
 * This behaviour is supposed to mostly match that of strtol but is not exactly the same
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API void zend_bigint_init_from_string_tolerant(zend_bigint *big, const char *str, int base)
{
	zend_bigint_init(big);
	if (mpz_set_str(big->mpz, str, base) < 0) {
		char *temp_str = estrdup(str);
		size_t len = strlen(temp_str);
		/* truncate string until valid */
		/* FIXME: Do this more performantly */
		do {
			temp_str[--len] = '\0';
			if (!len) {
				break;
			}
		}
		while (mpz_set_str(big->mpz, temp_str, base) < 0);
		efree(temp_str);
	}
}

/* Initialises a bigint from a long */
ZEND_API void zend_bigint_init_from_long(zend_bigint *big, long value)
{
	zend_bigint_init(big);
	mpz_set_si(big->mpz, value);
}

/* Initialises a bigint from a double
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API void zend_bigint_init_from_double(zend_bigint *big, double value)
{
	zend_bigint_init(big);
	mpz_set_d(big->mpz, value);
}

/* Initialises a bigint and duplicates a bigint to it (copies value)
 * HERE BE DRAGONS: Memory allocated internally by gmp is non-persistent */
ZEND_API void zend_bigint_init_dup(zend_bigint *big, const zend_bigint *source)
{
	zend_bigint_init(big);
	mpz_set(big->mpz, source->mpz);
}

/* Destroys a bigint */
ZEND_API void zend_bigint_dtor(zend_bigint *big)
{
	mpz_clear(big->mpz);
}

/*** INFORMATION ***/

/* Returns true if bigint can fit into an unsigned long without truncation */
ZEND_API zend_bool zend_bigint_can_fit_ulong(const zend_bigint *big)
{
	return mpz_fits_ulong_p(big->mpz);
}

/* Returns sign of bigint (-1 for negative, 0 for zero or 1 for positive) */
ZEND_API int zend_bigint_sign(const zend_bigint *big)
{
	return mpz_sgn(big->mpz);
}

/* Returns true if bigint is divisible by a bigint */
ZEND_API zend_bool zend_bigint_divisible(const zend_bigint *num, const zend_bigint *divisor)
{
	return mpz_divisible_p(num->mpz, divisor->mpz) ? 1 : 0;
}

/* Returns true if bigint is divisible by a long */
ZEND_API zend_bool zend_bigint_divisible_long(const zend_bigint *num, long divisor)
{
	zend_bool return_value;
	WITH_TEMP_MPZ_FROM_LONG(divisor, divisor_mpz, {
		return_value = mpz_divisible_p(num->mpz, divisor_mpz) ? 1 : 0;
	})
	return return_value;
}

/* Returns true if long is divisible by a bigint */
ZEND_API zend_bool zend_bigint_long_divisible(long num, const zend_bigint *divisor)
{
	zend_bool return_value;
	WITH_TEMP_MPZ_FROM_LONG(num, num_mpz, {
		return_value = mpz_divisible_p(num_mpz, divisor->mpz) ? 1 : 0;
	})
	return return_value;
}

/*** CONVERTORS ***/

/* Converts to long; this will cap at the max value of a long */
ZEND_API long zend_bigint_to_long(const zend_bigint *big)
{
	if (mpz_fits_slong_p(big->mpz)) {
		return mpz_get_si(big->mpz);
	} else {
		if (mpz_sgn(big->mpz) == 1) {
			return LONG_MAX;
		} else {
			return LONG_MIN;
		}
	}
}

/* Converts to unsigned long; this will cap at the max value of an unsigned long */
ZEND_API unsigned long zend_bigint_to_ulong(const zend_bigint *big)
{
	if (mpz_fits_ulong_p(big->mpz)) {
		return mpz_get_ui(big->mpz);
	} else {
		if (mpz_sgn(big->mpz) == 1) {
			return ULONG_MAX;
		} else {
			return 0;
		}
	}
}

/* Converts to bool */
ZEND_API zend_bool zend_bigint_to_bool(const zend_bigint *big)
{
	return mpz_sgn(big->mpz) ? 1 : 0;
}

/* Converts to double; this will lose precision beyond a certain point */
ZEND_API double zend_bigint_to_double(const zend_bigint *big)
{
	return mpz_get_d(big->mpz);
}

/* Converts to decimal C string
 * HERE BE DRAGONS: String allocated  is non-persistent */
ZEND_API char* zend_bigint_to_string(const zend_bigint *big)
{
	return mpz_get_str(NULL, 10, big->mpz);
}

/* Convenience function: Converts to zend string */
ZEND_API zend_string* zend_bigint_to_zend_string(const zend_bigint *big, int persistent)
{
	char *temp_string = zend_bigint_to_string(big);
	zend_string *return_value = STR_INIT(temp_string, strlen(temp_string), persistent);
	efree(temp_string);
	return return_value;
}

/* Converts to C string of arbitrary base */
ZEND_API char* zend_bigint_to_string_base(const zend_bigint *big, int base)
{
	return mpz_get_str(NULL, base, big->mpz);
}

/*** OPERATIONS **/

/* Adds two bigints and stores result in out */
ZEND_API void zend_bigint_add(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2)
{
	mpz_add(out->mpz, op1->mpz, op2->mpz);
}

/* Adds a bigint and a long and stores result in out */
ZEND_API void zend_bigint_add_long(zend_bigint *out, const zend_bigint *op1, long op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op2, op2_mpz, {
		mpz_add(out->mpz, op1->mpz, op2_mpz);
	})
}

/* Adds a long and a long and stores result in out */
ZEND_API void zend_bigint_long_add_long(zend_bigint *out, long op1, long op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op1, op1_mpz, WITH_TEMP_MPZ_FROM_LONG(op2, op2_mpz, {
		mpz_add(out->mpz, op1_mpz, op2_mpz);		
	}))
}

/* Subtracts two bigints and stores result in out */
ZEND_API void zend_bigint_subtract(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2)
{
	mpz_sub(out->mpz, op1->mpz, op2->mpz);
}

/* Subtracts a bigint and a long and stores result in out */
ZEND_API void zend_bigint_subtract_long(zend_bigint *out, const zend_bigint *op1, long op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op2, op2_mpz, {
		mpz_sub(out->mpz, op1->mpz, op2_mpz);
	})
}

/* Subtracts a long and a long and stores result in out */
ZEND_API void zend_bigint_long_subtract_long(zend_bigint *out, long op1, long op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op1, op1_mpz, WITH_TEMP_MPZ_FROM_LONG(op2, op2_mpz, {
		mpz_sub(out->mpz, op1_mpz, op2_mpz);		
	}))
}

/* Subtracts a long and a bigint and stores result in out */
ZEND_API void zend_bigint_long_subtract(zend_bigint *out, long op1, const zend_bigint *op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op1, op1_mpz, {
		mpz_sub(out->mpz, op1_mpz, op2->mpz);
	})
}

/* Multiplies two bigints and stores result in out */
ZEND_API void zend_bigint_multiply(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2)
{
	mpz_mul(out->mpz, op1->mpz, op2->mpz);
}

/* Multiplies a bigint and a long and stores result in out */
ZEND_API void zend_bigint_multiply_long(zend_bigint *out, const zend_bigint *op1, long op2)
{
	mpz_mul_si(out->mpz, op1->mpz, op2);
}

/* Multiplies a long and a long and stores result in out */
ZEND_API void zend_bigint_long_multiply_long(zend_bigint *out, long op1, long op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op1, op1_mpz, {
		mpz_mul_si(out->mpz, op1_mpz, op2);		
	})
}

/* Raises a bigint base to an unsigned long power and stores result in out */
ZEND_API void zend_bigint_pow_ulong(zend_bigint *out, const zend_bigint *base, unsigned long power)
{
	mpz_pow_ui(out->mpz, base->mpz, power);
}

/* Raises a long base to an unsigned long power and stores result in out */
ZEND_API void zend_bigint_long_pow_ulong(zend_bigint *out, long base, unsigned long power)
{
	WITH_TEMP_MPZ_FROM_LONG(base, base_mpz, {
		mpz_pow_ui(out->mpz, base_mpz, power);
	})
}

/* Divides a bigint by a bigint and stores result in out */
ZEND_API void zend_bigint_divide(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor)
{
	mpz_fdiv_q(out->mpz, num->mpz, divisor->mpz);
}

/* Divides a bigint by a long and stores result in out */
ZEND_API void zend_bigint_divide_long(zend_bigint *out, const zend_bigint *num, long divisor)
{
	WITH_TEMP_MPZ_FROM_LONG(divisor, divisor_mpz, {
		mpz_fdiv_q(out->mpz, num->mpz, divisor_mpz);
	})
}

/* Divides a long by a bigint and stores result in out */
ZEND_API void zend_bigint_long_divide(zend_bigint *out, long num, const zend_bigint *divisor)
{
	WITH_TEMP_MPZ_FROM_LONG(num, num_mpz, {
		mpz_fdiv_q(out->mpz, num_mpz, divisor->mpz);
	})
}

/* Finds the remainder of the division of a bigint by a bigint and stores result in out */
ZEND_API void zend_bigint_modulus(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor)
{
	mpz_tdiv_r(out->mpz, num->mpz, divisor->mpz);
}

/* Finds the remainder of the division of a bigint by a long and stores result in out */
ZEND_API void zend_bigint_modulus_long(zend_bigint *out, const zend_bigint *num, long divisor)
{
	WITH_TEMP_MPZ_FROM_LONG(divisor, divisor_mpz, {
		mpz_tdiv_r(out->mpz, num->mpz, divisor_mpz);
	)}
}

/* Finds the remainder of the division of a long by a bigint and stores result in out */
ZEND_API void zend_bigint_long_modulus(zend_bigint *out, long num, const zend_bigint *divisor)
{
	WITH_TEMP_MPZ_FROM_LONG(num, num_mpz, {
		mpz_tdiv_r(out->mpz, num_mpz, divisor->mpz);
	)}
}

/* Finds the one's complement of a bigint and stores result in out */
ZEND_API void zend_bigint_ones_complement(zend_bigint *out, const zend_bigint *op)
{
	mpz_com(out->mpz, op->mpz);
}

/* Finds the bitwise OR of a bigint and a bigint and stores result in out */
ZEND_API void zend_bigint_or(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2)
{
	mpz_ior(out->mpz, op1->mpz, op2->mpz);
}

/* Finds the bitwise OR of a bigint and a long and stores result in out */
ZEND_API void zend_bigint_or_long(zend_bigint *out, const zend_bigint *op1, long op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op2, op2_mpz, {
		mpz_ior(out->mpz, op1->mpz, op2_mpz);
	})
}

/* Finds the bitwise OR of a long and a bigint and stores result in out */
ZEND_API void zend_bigint_long_or(zend_bigint *out, long op1, const zend_bigint *op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op1, op1_mpz, {
		mpz_ior(out->mpz, op1_mpz, op2->mpz);
	})
}

/* Finds the bitwise AND of a bigint and a bigint and stores result in out */
ZEND_API void zend_bigint_and(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2)
{
	mpz_and(out->mpz, op1->mpz, op2->mpz);
}

/* Finds the bitwise AND of a bigint and a long and stores result in out */
ZEND_API void zend_bigint_and_long(zend_bigint *out, const zend_bigint *op1, long op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op2, op2_mpz, {
		mpz_and(out->mpz, op1->mpz, op2_mpz);
	})
}

/* Finds the bitwise AND of a long and a bigint and stores result in out */
ZEND_API void zend_bigint_long_and(zend_bigint *out, long op1, const zend_bigint *op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op1, op1_mpz, {
		mpz_and(out->mpz, op1_mpz, op2->mpz);
	})
}

/* Finds the bitwise XOR of a bigint and a bigint and stores result in out */
ZEND_API void zend_bigint_xor(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2)
{
	mpz_xor(out->mpz, op1->mpz, op2->mpz);
}

/* Finds the bitwise XOR of a bigint and a long and stores result in out */
ZEND_API void zend_bigint_xor_long(zend_bigint *out, const zend_bigint *op1, long op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op2, op2_mpz, {
		mpz_xor(out->mpz, op1->mpz, op2_mpz);
	})
}

/* Finds the bitwise XOR of a long and a bigint and stores result in out */
ZEND_API void zend_bigint_long_xor(zend_bigint *out, long op1, const zend_bigint *op2)
{
	WITH_TEMP_MPZ_FROM_LONG(op1, op1_mpz, {
		mpz_xor(out->mpz, op1_mpz, op2->mpz);
	})
}

/* Shifts a bigint left by an unsigned long and stores result in out */
ZEND_API void zend_bigint_shift_left_ulong(zend_bigint *out, const zend_bigint *num, unsigned long shift)
{
	mpz_mul_2exp(out->mpz, num->mpz, shift);
}

/* Shifts a long left by an unsigned long and stores result in out */
ZEND_API void zend_bigint_long_shift_left_ulong(zend_bigint *out, long num, unsigned long shift)
{
	WITH_TEMP_MPZ_FROM_LONG(num, num_mpz, {
		mpz_mul_2exp(out->mpz, num_mpz, shift);
	})
}

/* Shifts a bigint right by an unsigned long and stores result in out */
ZEND_API void zend_bigint_shift_right_ulong(zend_bigint *out, const zend_bigint *num, unsigned long shift)
{
	mpz_fdiv_q_2exp(out->mpz, num->mpz, shift);
}

/* Compares a bigint and a bigint and returns result (negative if op1 > op2, zero if op1 == op2, positive if op1 < op2) */
ZEND_API int zend_bigint_cmp(const zend_bigint *op1, const zend_bigint *op2)
{
	return mpz_cmp(op1->mpz, op2->mpz);
}

/* Compares a bigint and a long and returns result (negative if op1 > op2, zero if op1 == op2, positive if op1 < op2) */
ZEND_API int zend_bigint_cmp_long(const zend_bigint *op1, long op2)
{
	return mpz_cmp_si(op1->mpz, op2);
}

/* Compares a bigint and a double and returns result (negative if op1 > op2, zero if op1 == op2, positive if op1 < op2) */
ZEND_API int zend_bigint_cmp_double(const zend_bigint *op1, double op2)
{
	return mpz_cmp_d(op1->mpz, op2);
}