/*
  +----------------------------------------------------------------------+
  | Zend Big Integer Support - LibTomMath backend                        |
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

#include "zend.h"
#include "zend_types.h"
#include "zend_bigint.h"
#include "zend_string.h"

/* Here we define custom allocators to be used by LibTomMath
 * emalloc/realloc/free are macros, not functions, so we need wrappers to pass
 * them as our custom allocators
 */
static void* zend_bigint_malloc(size_t size)
{
	return emalloc(size);
}
#define XMALLOC zend_bigint_malloc

static void* zend_bigint_realloc(void *ptr, size_t size)
{
	return erealloc(ptr, size);
}
#define XREALLOC zend_bigint_realloc

static void* zend_bigint_calloc(size_t num, size_t size)
{
	return ecalloc(num, size);
}
#define XCALLOC zend_bigint_calloc

static void zend_bigint_free(void *ptr)
{
	efree(ptr);
}
#define XFREE zend_bigint_free

#include "tommath.h"

/* We'll build libtommath as part of this file */
#include "libtommath/mpi.c"

struct _zend_bigint {
    zend_refcounted   gc;
    mp_int            mp;
};

/*** INTERNAL MACROS ***/

#define int_abs(n) ((n) >= 0 ? (n) : -(n)) 
#define int_sgn(n) ((n) > 0 ? 1 : ((n) < 0 ? -1 : 0))


#define mp_sgn(mp) (USED(mp) ? ((SIGN(mp) == MP_NEG) ? -1 : 1) : 0)

/* used for checking if a number fits within a 32-bit unsigned int */
#define BITMASK_32 0xFFFFFFFFL

/* used for checking if a number fits within a LibTomMath "digit" */
#define BITMASK_DIGIT (((zend_ulong)1 << DIGIT_BIT) - 1) 

#define FITS_DIGIT(long) ((zend_ulong)(int_abs(long) & BITMASK_DIGIT) == (zend_ulong)int_abs(long))

/* Convenience macro to create a temporary mpz_t from a zend_long
 * Used when LibTomMath has no function for an operation taking a long */
#define WITH_TEMP_MP_FROM_ZEND_LONG(long, temp, codeblock) { \
	mp_int temp;								\
	mp_init(&temp);								\
	zend_bigint_long_to_mp_int(long, &temp);	\
	codeblock									\
	mp_clear(&temp);							\
}

/*** INTERNAL FUNCTIONS ***/

static void zend_bigint_long_to_mp_int(zend_long value, mp_int *mp)
{
	zend_ulong ulong_value = (value >= 0) ? value : -value;

	/* mp_set_int only uses 32 bits of an unsigned long
	   so if zend_long is too big, we need to do multiple ops */
#if SIZEOF_ZEND_LONG > 4
	if (ulong_value != (ulong_value & BITMASK_32)) {
#	if defined(mp_import)
		/* FIXME: This code is currently untested! */
		mp_import(mp, 1, 1, sizeof(ulong_value), 0, 0, &ulong_value);
		mp_neg(&mp, &mp);
		return;
#	elif SIZEOF_ZEND_LONG == 8
		mp_int tmp;

		mp_init_set_int(&tmp, ulong_value & BITMASK_32);

		mp_set_int(mp, (ulong_value >> 32) & BITMASK_32);
		mp_mul_2d(mp, 32, mp);
		mp_or(mp, &tmp, mp);

		if (value < 0) {
			mp_neg(mp, mp);
		}

		mp_clear(&tmp);
		return;
#	else
#		error SIZEOF_ZEND_LONG other than 4 or 8 unsupported
#	endif
	}
#endif
	
	mp_set_int(mp, ulong_value);
	if (value < 0) {
		mp_neg(mp, mp);
	}
}

static zend_long zend_bigint_mp_int_to_long(mp_int *mp)
{
#	if SIZEOF_ZEND_LONG == 4
		/* mp_get_int always returns 32 bits */
		return mp_get_int(mp) * ((SIGN(mp) == MP_NEG) ? -1 : 1);
# 	elif SIZEOF_ZEND_LONG == 8
		mp_int tmp;
		zend_ulong ulong_value = 0;
		zend_long value;

		mp_init_copy(&tmp, mp);
		
		ulong_value = mp_get_int(&tmp);
		mp_div_2d(&tmp, 32, &tmp, NULL);
		ulong_value |= mp_get_int(&tmp) << 32;

		mp_clear(&tmp);

		value = ulong_value * ((SIGN(mp) == MP_NEG) ? -1 : 1);
		return value;
#	else
#		error SIZEOF_ZEND_LONG other than 4 or 8 unsupported
#	endif
}

/* All documentation for bigint functions is found in zend_bigint.h */

void zend_startup_bigint(void)
{
	
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
	mp_init(&big->mp);
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
	if (mp_read_radix(&big->mp, str, base) < 0) {
		mp_clear(&big->mp);
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_bigint_init_from_string_length(zend_bigint *big, const char *str, size_t length, int base) /* {{{ */
{
	char *temp_str = estrndup(str, length);
	zend_bigint_init(big);
	if (mp_read_radix(&big->mp, temp_str, base) < 0) {
		mp_clear(&big->mp);
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
		mp_read_radix(&big->mp, temp_str, base);
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
	zend_bigint_long_to_mp_int(value, &big->mp);
}
/* }}} */

ZEND_API void zend_bigint_init_from_double(zend_bigint *big, double value) /* {{{ */
{
	zend_bigint_init(big);
	/* prevents crash and ensures zed_dval_to_lval conformity */
	if (zend_finite(value) && !zend_isnan(value)) {
		/* FIXME: Handle larger doubles */
		zend_bigint_long_to_mp_int(zend_dval_to_lval(value), &big->mp);
	}
}
/* }}} */

ZEND_API void zend_bigint_init_dup(zend_bigint *big, const zend_bigint *source) /* {{{ */
{
	zend_bigint_init(big);
	mp_copy(&source->mp, &big->mp);
}
/* }}} */

ZEND_API void zend_bigint_dtor(zend_bigint *big) /* {{{ */
{
	mp_clear(&big->mp);
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
	/* FIXME: non-stub */
	return 0;
}
/* }}} */

ZEND_API zend_bool zend_bigint_can_fit_long(const zend_bigint *big) /* {{{ */
{
	/* FIXME: non-stub */
	return 0;
}
/* }}} */

ZEND_API int zend_bigint_sign(const zend_bigint *big) /* {{{ */
{
	
	return mp_sgn(&big->mp);
}
/* }}} */

ZEND_API zend_bool zend_bigint_divisible(const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	/* TODO: Refactor division so we don't have to do it twice for LibTomMath */
	mp_int remainder;
	zend_bool result;

	mp_init(&remainder);

	mp_div(&num->mp, &divisor->mp, NULL, &remainder);
	result = USED(&remainder) ? 0 : 1;

	mp_clear(&remainder);

	return result;
}
/* }}} */

ZEND_API zend_bool zend_bigint_divisible_long(const zend_bigint *num, zend_long divisor) /* {{{ */
{
	/* TODO: Refactor division so we don't have to do it twice for LibTomMath */
	mp_int remainder;
	zend_bool result;

	mp_init(&remainder);

	if (FITS_DIGIT(divisor)) {
		mp_div_d(&num->mp, int_abs(divisor), NULL, &remainder);
	} else {
		WITH_TEMP_MP_FROM_ZEND_LONG(divisor, divisor_mp, {
			mp_div(&num->mp, &divisor_mp, NULL, &remainder);
		})
	}
	result = USED(&remainder) ? 0 : 1;

	mp_clear(&remainder);

	return result;
}
/* }}} */

ZEND_API zend_bool zend_bigint_long_divisible(zend_long num, const zend_bigint *divisor) /* {{{ */
{
	/* TODO: Refactor division so we don't have to do it twice for LibTomMath */
	mp_int remainder;
	zend_bool result;

	mp_init(&remainder);

	WITH_TEMP_MP_FROM_ZEND_LONG(num, num_mp, {
		mp_div(&num_mp, &divisor->mp, NULL, &remainder);
	})
	result = USED(&remainder) ? 0 : 1;

	mp_clear(&remainder);

	return result;
}
/* }}} */

/*** CONVERTORS ***/

ZEND_API zend_long zend_bigint_to_long(const zend_bigint *big) /* {{{ */
{
	/* FIXME: reimplement dval_to_lval algo */
	return zend_bigint_mp_int_to_long(&big->mp);
}

ZEND_API zend_long zend_bigint_to_long_saturate(const zend_bigint *big) /* {{{ */
{
	/* FIXME: reimplement saturation algo and handle larger nos */
	return zend_bigint_mp_int_to_long(&big->mp);
}
/* }}} */

ZEND_API zend_long zend_bigint_to_long_ex(const zend_bigint *big, zend_bool *overflow) /* {{{ */
{
	/* FIXME: reimplement saturation algo and handle larger nos */
	*overflow = 0;
	return zend_bigint_mp_int_to_long(&big->mp);
}
/* }}} */

ZEND_API zend_ulong zend_bigint_to_ulong(const zend_bigint *big) /* {{{ */
{
	/* FIXME: reimplement saturation algo and handle larger nos */
	return mp_get_int(&big->mp);
}

ZEND_API zend_bool zend_bigint_to_bool(const zend_bigint *big) /* {{{ */
{
	return USED(&big->mp) ? 1 : 0;
}
/* }}} */

ZEND_API double zend_bigint_to_double(const zend_bigint *big) /* {{{ */
{
	/* from http://github.com/libtom/libtommath/issues/3#issuecomment-5668076
	   having it depend on library internals is bad, but sadly not yet a function
	   FIXME: TODO: Replace with built-in LibTomMath function once available */
	static const int PRECISION = 53;
	static const int NEED_DIGITS = (PRECISION + 2 * DIGIT_BIT - 2) / DIGIT_BIT;
	static const double DIGIT_MULTI = (mp_digit)1 << DIGIT_BIT;

	mp_int *a = (mp_int*)&big->mp;
	int i, limit;
	double d = 0.0;

	/* this is technically a violation of the parameter being a const
	 * however, it shouldn't change the value, and I'd rather not duplicate
	 */
	mp_clamp(a);
	i = USED(a);
	limit = i <= NEED_DIGITS ? 0 : i - NEED_DIGITS;

	while (i-- > limit) {
		d += DIGIT(a, i);
		d *= DIGIT_MULTI;
	}

	if (SIGN(a) == MP_NEG) {
		d *= -1.0;
	}

	d *= pow(2.0, i * DIGIT_BIT);
	return d;
}
/* }}} */

ZEND_API char* zend_bigint_to_string(const zend_bigint *big) /* {{{ */
{
	int size;
	char *str;
	mp_radix_size(&big->mp, 10, &size);
	str = emalloc(size);
	mp_toradix(&big->mp, str, 10);
	return str;
}
/* }}} */

ZEND_API zend_string* zend_bigint_to_zend_string(const zend_bigint *big, int persistent) /* {{{ */
{
	int size;
	zend_string *str;
	str = zend_string_alloc(size - 1, persistent);
	mp_toradix(&big->mp, str->val, 10);
	return str;
}
/* }}} */

ZEND_API char* zend_bigint_to_string_base(const zend_bigint *big, int base) /* {{{ */
{
	int size;
	char *str;
	mp_radix_size(&big->mp, base, &size);
	str = emalloc(size);
	mp_toradix(&big->mp, str, base);
	return str;
}
/* }}} */

/*** OPERATIONS **/

ZEND_API void zend_bigint_add(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	mp_add(&op1->mp, &op2->mp, &out->mp);
}
/* }}} */

ZEND_API void zend_bigint_add_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	if (FITS_DIGIT(op2)) {
		if (int_sgn(op2) >= 1) {
			mp_add_d(&op1->mp, op2, &out->mp);
		} else {
			mp_sub_d(&op1->mp, -op2, &out->mp);
		}
	} else {
		WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
			mp_add(&op1->mp, &op2_mp, &out->mp);
		})
	}
}
/* }}} */

ZEND_API void zend_bigint_long_add_long(zend_bigint *out, zend_long op1, zend_long op2) /* {{{ */
{
	WITH_TEMP_MP_FROM_ZEND_LONG(op1, op1_mp, WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
		mp_add(&op1_mp, &op2_mp, &out->mp);
	}))
}
/* }}} */

ZEND_API void zend_bigint_subtract(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	mp_sub(&op1->mp, &op2->mp, &out->mp);
}
/* }}} */

ZEND_API void zend_bigint_subtract_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	if (FITS_DIGIT(op2)) {
		if (int_sgn(op2) >= 1) {
			mp_sub_d(&op1->mp, op2, &out->mp);
		} else {
			mp_add_d(&op1->mp, -op2, &out->mp);
		}
	} else {
		WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
			mp_add(&op1->mp, &op2_mp, &out->mp);
		})
	}
}
/* }}} */

ZEND_API void zend_bigint_long_subtract_long(zend_bigint *out, zend_long op1, zend_long op2) /* {{{ */
{
	/* TODO: Use mp_sub_d where possible, to allocate only one temp mp_int */
	WITH_TEMP_MP_FROM_ZEND_LONG(op1, op1_mp, WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
		mp_sub(&op1_mp, &op2_mp, &out->mp);
	}))
}
/* }}} */

ZEND_API void zend_bigint_long_subtract(zend_bigint *out, zend_long op1, const zend_bigint *op2) /* {{{ */
{
	/* TODO: Rather than having _long_subtract detect negation, maybe make a
	 * _negate function?
	 */
	if (op1 == 0) {
		mp_neg(&op2->mp, &out->mp);
	} else {
		/* b - a is just -(a - b) */
		zend_bigint_subtract_long(out, op2, op1);
		mp_neg(&out->mp, &out->mp);
	}
}
/* }}} */

ZEND_API void zend_bigint_multiply(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	mp_mul(&op1->mp, &op2->mp, &out->mp);
}
/* }}} */

ZEND_API void zend_bigint_multiply_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* FIXME: Non-stub */
	if (FITS_DIGIT(op2)) {
		mp_mul_d(&op1->mp, int_abs(op2), &out->mp);
		if (op2 < 0) {
			/* (a * -b) = -ab = -(a * b)
			 * (-a * -b) = ab = -(-a * b)
			 */
			mp_neg(&out->mp, &out->mp);
		}
	} else {
		WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
			mp_mul(&op1->mp, &op2_mp, &out->mp);
		})
	}
}
/* }}} */

ZEND_API void zend_bigint_long_multiply_long(zend_bigint *out, zend_long op1, zend_long op2) /* {{{ */
{
	WITH_TEMP_MP_FROM_ZEND_LONG(op1, op1_mp, WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
		mp_mul(&op1_mp, &op2_mp, &out->mp);
	}))
}
/* }}} */

ZEND_API void zend_bigint_pow_ulong(zend_bigint *out, const zend_bigint *base, zend_ulong power) /* {{{ */
{
	if (!FITS_DIGIT(power)) {
		zend_error_noreturn(E_ERROR, "Exponent too large");
	}

	mp_expt_d(&base->mp, power, &out->mp);
}
/* }}} */

ZEND_API void zend_bigint_long_pow_ulong(zend_bigint *out, zend_long base, zend_ulong power) /* {{{ */
{
	if (!FITS_DIGIT(power)) {
		zend_error_noreturn(E_ERROR, "Exponent too large");
	}

	WITH_TEMP_MP_FROM_ZEND_LONG(base, base_mp, {
		mp_expt_d(&base_mp, power, &out->mp);
	})
}
/* }}} */

ZEND_API void zend_bigint_divide(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	mp_div(&num->mp, &divisor->mp, &out->mp, NULL);
}
/* }}} */

ZEND_API double zend_bigint_divide_as_double(const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	/* FIXME: Higher precision? */
	return zend_bigint_to_double(num) / zend_bigint_to_double(divisor);
}
/* }}} */

ZEND_API void zend_bigint_divide_long(zend_bigint *out, const zend_bigint *num, zend_long divisor) /* {{{ */
{
	if (FITS_DIGIT(divisor)) {
		mp_div_d(&num->mp, int_abs(divisor), &out->mp, NULL);
		if (divisor < 0) {
			mp_neg(&out->mp, &out->mp);
		}
	} else {
		WITH_TEMP_MP_FROM_ZEND_LONG(divisor, divisor_mp, {
			mp_div(&num->mp, &divisor_mp, &out->mp, NULL);
		})
	}
}
/* }}} */

ZEND_API double zend_bigint_divide_long_as_double(const zend_bigint *num, zend_long divisor) /* {{{ */
{
	/* FIXME: Higher precision? */
	return zend_bigint_to_double(num) / (double)divisor;
}
/* }}} */

ZEND_API void zend_bigint_long_divide(zend_bigint *out, zend_long num, const zend_bigint *divisor) /* {{{ */
{
	WITH_TEMP_MP_FROM_ZEND_LONG(num, num_mp, {
		mp_div(&num_mp, &divisor->mp, &out->mp, NULL);
	})
}
/* }}} */

ZEND_API double zend_bigint_long_divide_as_double(zend_long num, const zend_bigint *divisor) /* {{{ */
{
	/* FIXME: Higher precision? */
	return (double)num / zend_bigint_to_double(divisor);
}
/* }}} */

ZEND_API void zend_bigint_modulus(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_modulus_long(zend_bigint *out, const zend_bigint *num, zend_long divisor) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_long_modulus(zend_bigint *out, zend_long num, const zend_bigint *divisor) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_ones_complement(zend_bigint *out, const zend_bigint *op) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_or(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_or_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_and(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_and_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_xor(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_xor_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_shift_left_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_long_shift_left_ulong(zend_bigint *out, zend_long num, zend_ulong shift) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API void zend_bigint_shift_right_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift) /* {{{ */
{
	/* FIXME: Non-stub */
}
/* }}} */

ZEND_API int zend_bigint_cmp(const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	/* FIXME: Non-stub */
	return -1;
}
/* }}} */

ZEND_API int zend_bigint_cmp_long(const zend_bigint *op1, zend_long op2) /* {{{ */
{
	/* FIXME: Non-stub */
	return -1;
}
/* }}} */

ZEND_API int zend_bigint_cmp_double(const zend_bigint *op1, double op2) /* {{{ */
{
	/* FIXME: Non-stub */
	return -1;
}
/* }}} */

ZEND_API void zend_bigint_abs(zend_bigint *out, const zend_bigint *big) /* {{{ */
{
	/* FIXME: Non-stub */
}
