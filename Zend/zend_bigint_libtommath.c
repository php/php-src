/*
  +----------------------------------------------------------------------+
  | Zend Big Integer Support - LibTomMath backend                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2015 The PHP Group                                     |
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

/* This file is one of the two implementations of zend_bigint.h
 * See zend_bigint.c
 */

#include "zend.h"

#ifdef ZEND_HAVE_LIBTOMMATH

#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>

#include "zend_types.h"
#include "zend_bigint.h"
#include "zend_string.h"

/* Here we define custom allocators to be used by LibTomMath
 * emalloc/realloc/free are macros, not functions, so we need wrappers to pass
 * them as our custom allocators
 */
static void* zend_bigint_custom_malloc(size_t size)
{
	return emalloc(size);
}
#define XMALLOC zend_bigint_custom_malloc

static void* zend_bigint_custom_realloc(void *ptr, size_t size)
{
	return erealloc(ptr, size);
}
#define XREALLOC zend_bigint_custom_realloc

static void* zend_bigint_custom_calloc(size_t num, size_t size)
{
	return ecalloc(num, size);
}
#define XCALLOC zend_bigint_custom_calloc

static void zend_bigint_custom_free(void *ptr)
{
	efree(ptr);
}
#define XFREE zend_bigint_custom_free

#include "tommath.h"

/* We'll build libtommath as part of this file */
#include "libtommath/mpi.c"

struct _zend_bigint {
	zend_refcounted   gc;
	mp_int			mp;
};

/*** INTERNAL MACROS ***/

#define int_abs(n) ((n) >= 0 ? (n) : -(n)) 
#define int_sgn(n) ((n) > 0 ? 1 : ((n) < 0 ? -1 : 0))

#define double_sgn(n) ((n) > 0 ? 1 : ((n) < 0 ? -1 : 0))

#define mp_sgn(mp) (USED(mp) ? ((SIGN(mp) == MP_NEG) ? -1 : 1) : 0)

/* used for checking if a number fits within a 32-bit unsigned int */
#define BITMASK_32 0xFFFFFFFFL

/* used for checking if a number fits within a LibTomMath "digit" */
#define BITMASK_DIGIT (((zend_ulong)1 << DIGIT_BIT) - 1) 

#define FITS_DIGIT(long) ((zend_ulong)(int_abs(long) & BITMASK_DIGIT) == (zend_ulong)int_abs(long))

#define CHECK_ERROR(expr) do { 									\
	int error_code;												\
	if ((error_code = (expr)) != MP_OKAY) {						\
		zend_error(E_ERROR, mp_error_to_string(error_code));	\
	}															\
} while (0)

/* Convenience macro to create a temporary mpz_t from a zend_long
 * Used when LibTomMath has no function for an operation taking a long */
#define WITH_TEMP_MP_FROM_ZEND_LONG(long, temp, codeblock) { \
	mp_int temp;								\
	CHECK_ERROR(mp_init(&temp));				\
	zend_bigint_long_to_mp_int(long, &temp);	\
	codeblock									\
	mp_clear(&temp);							\
}

/*** INTERNAL FUNCTIONS ***/

static void zend_bigint_ulong_to_mp_int(zend_ulong value, mp_int *mp)
{
	/* mp_set_int only uses 32 bits of an unsigned long
	   so if zend_long is too big, we need to do multiple ops */
#if SIZEOF_ZEND_LONG > 4
	if (value != (value & BITMASK_32)) {
#	if defined(mp_import)
		/* FIXME: This code is currently untested! */
		mp_import(mp, 1, 1, sizeof(value), 0, 0, &value);
		return;
#	elif SIZEOF_ZEND_LONG == 8
		mp_int tmp;

		mp_init_set_int(&tmp, value & BITMASK_32);

		mp_set_int(mp, (value >> 32) & BITMASK_32);

		mp_mul_2d(mp, 32, mp);
		mp_or(mp, &tmp, mp);

		mp_clear(&tmp);
		return;
#	else
#		error SIZEOF_ZEND_LONG other than 4 or 8 unsupported
#	endif
	}
#endif

	mp_set_int(mp, value);
}

static void zend_bigint_long_to_mp_int(zend_long value, mp_int *mp)
{
	zend_ulong ulong_value = (value >= 0) ? value : -value;

	zend_bigint_ulong_to_mp_int(ulong_value, mp);

	if (value < 0) {
		mp_neg(mp, mp);
	}
}

static zend_ulong zend_bigint_mp_int_to_ulong(mp_int *mp)
{
#	if SIZEOF_ZEND_LONG == 4
		/* mp_get_int always returns 32 bits */
		return mp_get_int(mp);
# 	elif SIZEOF_ZEND_LONG == 8
		mp_int tmp;
		zend_ulong value = 0;

		CHECK_ERROR(mp_init_copy(&tmp, mp));

		value = mp_get_int(&tmp);
		mp_div_2d(&tmp, 32, &tmp, NULL);
		value |= mp_get_int(&tmp) << 32;

		mp_clear(&tmp);

		return value;
#	else
#		error SIZEOF_ZEND_LONG other than 4 or 8 unsupported
#	endif
}

static zend_long zend_bigint_mp_int_to_long(mp_int *mp)
{
	return zend_bigint_mp_int_to_ulong(mp) * ((SIGN(mp) == MP_NEG) ? -1 : 1);
}

/* All documentation for bigint functions is found in zend_bigint.h */

mp_int ZEND_LONG_MAX_mp, ZEND_LONG_MIN_mp;
mp_int ZEND_ULONG_MAX_mp;
mp_int two_pow_bits_mp;

void zend_startup_bigint(void)
{
	CHECK_ERROR(mp_init_multi(
		&ZEND_LONG_MAX_mp, &ZEND_LONG_MIN_mp,
		&ZEND_ULONG_MAX_mp,
		&two_pow_bits_mp,
		NULL
	));
	zend_bigint_long_to_mp_int(ZEND_LONG_MAX, &ZEND_LONG_MAX_mp);
	zend_bigint_long_to_mp_int(ZEND_LONG_MIN, &ZEND_LONG_MAX_mp);
	zend_bigint_ulong_to_mp_int(ZEND_ULONG_MAX, &ZEND_ULONG_MAX_mp);

	/* either 2^32 or 2^64 */
	mp_set_int(&two_pow_bits_mp, 1);
	mp_mul_2d(&two_pow_bits_mp, SIZEOF_ZEND_LONG * 8, &two_pow_bits_mp);
}

/*** INITIALISERS ***/

ZEND_API zend_bigint* zend_bigint_init(void) /* {{{ */
{
	zend_bigint *return_value = emalloc(sizeof(zend_bigint));
	GC_REFCOUNT(return_value) = 1;
	GC_TYPE_INFO(return_value) = IS_BIGINT;
	CHECK_ERROR(mp_init(&return_value->mp));
	return return_value;
}
/* }}} */

ZEND_API zend_bigint* zend_bigint_init_from_string(const char *str, int base) /* {{{ */
{
	zend_bigint *big = zend_bigint_init();

	if (mp_read_radix(&big->mp, str, base) < 0) {
		zend_bigint_release(big);
		return NULL;
	}
	return big;
}
/* }}} */

ZEND_API zend_bigint* zend_bigint_init_from_string_length(const char *str, size_t length, int base) /* {{{ */
{
	zend_bigint *big = zend_bigint_init();
	char *temp_str = estrndup(str, length);

	if (mp_read_radix(&big->mp, temp_str, base) < 0) {
		zend_bigint_release(big);
		efree(temp_str);
		return NULL;
	}
	efree(temp_str);
	return big;
}
/* }}} */

ZEND_API zend_bigint* zend_bigint_init_strtol(const char *str, char** endptr, int base) /* {{{ */
{
	zend_bigint *big = zend_bigint_init();
	size_t len = 0;

	/* Skip leading whitespace */
	while (isspace(*str)) {
		str++;
	}

	/* A single sign is valid */
	if (str[0] == '+' || str[0] == '-') {
		len += 1;
	}

	/* detect prefix */
	if (base == 0) {
		if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
			base = 16;
			str += 2;
		} else if (str[0] == '0') {
			base = 8;
			str++;
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
	} else if (base == 8) {
		while (isdigit(str[len]) && str[len] != '8' && str[len] != '9') {
			len++;
		}
	}

	if (len) {
		char *temp_str = estrndup(str, len);
		/* we ignore the return value since if it fails it'll just be zero anyway */
		mp_read_radix(&big->mp, temp_str, base);
		efree(temp_str);
	}

	if (endptr) {
		*endptr = (char*)(str + len);
	}

	return big;
}
/* }}} */

ZEND_API zend_bigint* zend_bigint_init_from_long(zend_long value) /* {{{ */
{
	zend_bigint *big = zend_bigint_init();
	zend_bigint_long_to_mp_int(value, &big->mp);
	return big;
}
/* }}} */

ZEND_API zend_bigint* zend_bigint_init_from_double(double value) /* {{{ */
{
	zend_bigint *big = zend_bigint_init();
	/* prevents crash and ensures zed_dval_to_lval conformity */
	if (zend_finite(value) && !zend_isnan(value)) {
		/* FIXME: Handle larger doubles */
		zend_bigint_long_to_mp_int(zend_dval_to_lval(value), &big->mp);
	}
	return big;
}
/* }}} */

ZEND_API zend_bigint* zend_bigint_dup(const zend_bigint *source) /* {{{ */
{
	zend_bigint *big = zend_bigint_init();
	CHECK_ERROR(mp_copy((mp_int *)&source->mp, &big->mp));
	return big;
}
/* }}} */

ZEND_API void zend_bigint_release(zend_bigint *big) /* {{{ */
{
	if (--GC_REFCOUNT(big) <= 0) {
		mp_clear(&big->mp);
		efree(big);
	}
}

ZEND_API void zend_bigint_free(zend_bigint *big) /* {{{ */
{
	mp_clear(&big->mp);
	efree(big);
}
/* }}} */

/*** INFORMATION ***/

ZEND_API zend_bool zend_bigint_can_fit_ulong(const zend_bigint *big) /* {{{ */
{
#	if SIZEOF_ZEND_LONG == 4
		if (mp_count_bits((mp_int*)&big->mp) < 32) {
			return 1;
		}
#	elif SIZEOF_ZEND_LONG == 8
		if (mp_count_bits((mp_int*)&big->mp) < 64) {
			return 1;
		}
#	else
#		error SIZEOF_ZEND_LONG other than 4 or 8 unsupported
#	endif
	return (mp_cmp((mp_int*)&big->mp, &ZEND_ULONG_MAX_mp) <= 0 && mp_sgn(&big->mp) >= 0);
}
/* }}} */

ZEND_API zend_bool zend_bigint_can_fit_long(const zend_bigint *big) /* {{{ */
{
#	if SIZEOF_ZEND_LONG == 4
		if (mp_count_bits((mp_int*)&big->mp) < 31) {
			return 1;
		}
#	elif SIZEOF_ZEND_LONG == 8
		if (mp_count_bits((mp_int*)&big->mp) < 63) {
			return 1;
		}
#	else
#		error SIZEOF_ZEND_LONG other than 4 or 8 unsupported
#	endif
	return (mp_cmp((mp_int*)&big->mp, &ZEND_LONG_MAX_mp) <= 0 && mp_cmp((mp_int*)&big->mp, &ZEND_LONG_MIN_mp) >= 0);
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

	CHECK_ERROR(mp_init(&remainder));

	CHECK_ERROR(mp_div((mp_int*)&num->mp, (mp_int*)&divisor->mp, NULL, &remainder));
	result = USED(&remainder) ? 0 : 1;

	mp_clear(&remainder);

	return result;
}
/* }}} */

ZEND_API zend_bool zend_bigint_divisible_long(const zend_bigint *num, zend_long divisor) /* {{{ */
{
	/* TODO: Refactor division so we don't have to do it twice for LibTomMath */
	if (FITS_DIGIT(divisor)) {
		mp_digit rem;

		CHECK_ERROR(mp_div_d((mp_int*)&num->mp, int_abs(divisor), NULL, &rem));
		return rem ? 0 : 1;
	} else {
		zend_bool result;
		mp_int remainder;

		CHECK_ERROR(mp_init(&remainder));
		WITH_TEMP_MP_FROM_ZEND_LONG(divisor, divisor_mp, {
			CHECK_ERROR(mp_div((mp_int*)&num->mp, &divisor_mp, NULL, &remainder));
		})	
		result = USED(&remainder) ? 0 : 1;

		mp_clear(&remainder);
		return result;
	}
}
/* }}} */

ZEND_API zend_bool zend_bigint_long_divisible(zend_long num, const zend_bigint *divisor) /* {{{ */
{
	/* TODO: Refactor division so we don't have to do it twice for LibTomMath */
	mp_int remainder;
	zend_bool result;

	CHECK_ERROR(mp_init(&remainder));

	WITH_TEMP_MP_FROM_ZEND_LONG(num, num_mp, {
		CHECK_ERROR(mp_div(&num_mp, (mp_int*)&divisor->mp, NULL, &remainder));
	})
	result = USED(&remainder) ? 0 : 1;

	mp_clear(&remainder);

	return result;
}
/* }}} */

/*** CONVERTORS ***/

ZEND_API zend_long zend_bigint_to_long(const zend_bigint *big) /* {{{ */
{
	mp_int bmod;
	zend_long result;

	mp_init(&bmod);

	mp_mod_2d((mp_int*)&big->mp, SIZEOF_ZEND_LONG * 8, &bmod);

	if (SIGN(&bmod) == MP_NEG) {
		/* we're going to make this number positive */

		mp_add(&bmod, &two_pow_bits_mp, &bmod);
	}

	/* ulong -> long conversion is deliberate */
	result = (zend_long)zend_bigint_mp_int_to_ulong(&bmod);

	mp_clear(&bmod);
	
	return result;
}

ZEND_API zend_long zend_bigint_to_long_saturate(const zend_bigint *big) /* {{{ */
{
	if (zend_bigint_can_fit_long(big)) {
		return zend_bigint_mp_int_to_long((mp_int*)&big->mp);
	} else {
		if (SIGN(&big->mp) == MP_NEG) {
			return ZEND_LONG_MIN;
		} else {
			return ZEND_LONG_MAX;
		}
	}
}
/* }}} */

ZEND_API zend_long zend_bigint_to_long_ex(const zend_bigint *big, zend_bool *overflow) /* {{{ */
{
	*overflow = !zend_bigint_can_fit_long(big);
	return zend_bigint_mp_int_to_long((mp_int*)&big->mp);
}
/* }}} */

ZEND_API zend_ulong zend_bigint_to_ulong(const zend_bigint *big) /* {{{ */
{
	/* FIXME: reimplement saturation algo and handle larger nos */
	return mp_get_int((mp_int*)&big->mp);
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
	const int PRECISION = 53;
	const int NEED_DIGITS = (PRECISION + 2 * DIGIT_BIT - 2) / DIGIT_BIT;
	const double DIGIT_MULTI = (mp_digit)1 << DIGIT_BIT;

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
	CHECK_ERROR(mp_radix_size((mp_int*)&big->mp, 10, &size));
	str = emalloc(size);
	CHECK_ERROR(mp_toradix((mp_int*)&big->mp, str, 10));
	return str;
}
/* }}} */

ZEND_API zend_string* zend_bigint_to_zend_string(const zend_bigint *big, int persistent) /* {{{ */
{
	int size;
	zend_string *str;
	CHECK_ERROR(mp_radix_size((mp_int*)&big->mp, 10, &size));
	str = zend_string_alloc(size - 1, persistent);
	CHECK_ERROR(mp_toradix((mp_int*)&big->mp, str->val, 10));
	return str;
}
/* }}} */

ZEND_API char* zend_bigint_to_string_base(const zend_bigint *big, int base) /* {{{ */
{
	int size;
	char *str;
	CHECK_ERROR(mp_radix_size((mp_int*)&big->mp, base, &size));
	str = emalloc(size);
	CHECK_ERROR(mp_toradix((mp_int*)&big->mp, str, base));
	return str;
}
/* }}} */

/*** OPERATIONS **/

ZEND_API void zend_bigint_add(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	CHECK_ERROR(mp_add((mp_int*)&op1->mp, (mp_int*)&op2->mp, &out->mp));
}
/* }}} */

ZEND_API void zend_bigint_add_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	if (FITS_DIGIT(op2)) {
		if (int_sgn(op2) >= 1) {
			CHECK_ERROR(mp_add_d((mp_int*)&op1->mp, op2, &out->mp));
		} else {
			CHECK_ERROR(mp_sub_d((mp_int*)&op1->mp, -op2, &out->mp));
		}
	} else {
		WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
			CHECK_ERROR(mp_add((mp_int*)&op1->mp, &op2_mp, &out->mp));
		})
	}
}
/* }}} */

ZEND_API void zend_bigint_long_add_long(zend_bigint *out, zend_long op1, zend_long op2) /* {{{ */
{
	WITH_TEMP_MP_FROM_ZEND_LONG(op1, op1_mp, WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
		CHECK_ERROR(mp_add(&op1_mp, &op2_mp, &out->mp));
	}))
}
/* }}} */

ZEND_API void zend_bigint_subtract(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	CHECK_ERROR(mp_sub((mp_int*)&op1->mp, (mp_int*)&op2->mp, &out->mp));
}
/* }}} */

ZEND_API void zend_bigint_subtract_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	if (FITS_DIGIT(op2)) {
		if (int_sgn(op2) >= 1) {
			CHECK_ERROR(mp_sub_d((mp_int*)&op1->mp, op2, &out->mp));
		} else {
			CHECK_ERROR(mp_add_d((mp_int*)&op1->mp, -op2, &out->mp));
		}
	} else {
		WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
			CHECK_ERROR(mp_sub((mp_int*)&op1->mp, &op2_mp, &out->mp));
		})
	}
}
/* }}} */

ZEND_API void zend_bigint_long_subtract_long(zend_bigint *out, zend_long op1, zend_long op2) /* {{{ */
{
	/* TODO: Use mp_sub_d where possible, to allocate only one temp mp_int */
	WITH_TEMP_MP_FROM_ZEND_LONG(op1, op1_mp, WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
		CHECK_ERROR(mp_sub(&op1_mp, &op2_mp, &out->mp));
	}))
}
/* }}} */

ZEND_API void zend_bigint_long_subtract(zend_bigint *out, zend_long op1, const zend_bigint *op2) /* {{{ */
{
	/* TODO: Rather than having _long_subtract detect negation, maybe make a
	 * _negate function?
	 */
	if (op1 == 0) {
		CHECK_ERROR(mp_neg((mp_int*)&op2->mp, &out->mp));
	} else {
		/* b - a is just -(a - b) */
		zend_bigint_subtract_long(out, op2, op1);
		CHECK_ERROR(mp_neg(&out->mp, &out->mp));
	}
}
/* }}} */

ZEND_API void zend_bigint_multiply(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	CHECK_ERROR(mp_mul((mp_int*)&op1->mp, (mp_int*)&op2->mp, &out->mp));
}
/* }}} */

ZEND_API void zend_bigint_multiply_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	if (FITS_DIGIT(op2)) {
		CHECK_ERROR(mp_mul_d((mp_int*)&op1->mp, int_abs(op2), &out->mp));
		if (op2 < 0) {
			/* (a * -b) = -ab = -(a * b)
			 * (-a * -b) = ab = -(-a * b)
			 */
			CHECK_ERROR(mp_neg(&out->mp, &out->mp));
		}
	} else {
		WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
			CHECK_ERROR(mp_mul((mp_int*)&op1->mp, &op2_mp, &out->mp));
		})
	}
}
/* }}} */

ZEND_API void zend_bigint_long_multiply_long(zend_bigint *out, zend_long op1, zend_long op2) /* {{{ */
{
	WITH_TEMP_MP_FROM_ZEND_LONG(op1, op1_mp, WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
		CHECK_ERROR(mp_mul(&op1_mp, &op2_mp, &out->mp));
	}))
}
/* }}} */

ZEND_API void zend_bigint_pow_ulong(zend_bigint *out, const zend_bigint *base, zend_ulong power) /* {{{ */
{
	if (!FITS_DIGIT(power)) {
		zend_error_noreturn(E_ERROR, "Exponent too large");
	}

	CHECK_ERROR(mp_expt_d((mp_int*)&base->mp, power, &out->mp));
}
/* }}} */

ZEND_API void zend_bigint_long_pow_ulong(zend_bigint *out, zend_long base, zend_ulong power) /* {{{ */
{
	if (!FITS_DIGIT(power)) {
		zend_error_noreturn(E_ERROR, "Exponent too large");
	}

	WITH_TEMP_MP_FROM_ZEND_LONG(base, base_mp, {
		CHECK_ERROR(mp_expt_d(&base_mp, power, &out->mp));
	})
}
/* }}} */

ZEND_API void zend_bigint_divide(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor) /* {{{ */
{
	CHECK_ERROR(mp_div((mp_int*)&num->mp, (mp_int*)&divisor->mp, &out->mp, NULL));
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
		CHECK_ERROR(mp_div_d((mp_int*)&num->mp, int_abs(divisor), &out->mp, NULL));
		if (divisor < 0) {
			CHECK_ERROR(mp_neg(&out->mp, &out->mp));
		}
	} else {
		WITH_TEMP_MP_FROM_ZEND_LONG(divisor, divisor_mp, {
			CHECK_ERROR(mp_div((mp_int*)&num->mp, &divisor_mp, &out->mp, NULL));
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
		CHECK_ERROR(mp_div(&num_mp, (mp_int*)&divisor->mp, &out->mp, NULL));
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
	CHECK_ERROR(mp_div((mp_int*)&num->mp, (mp_int*)&divisor->mp, NULL, &out->mp));
}
/* }}} */

ZEND_API void zend_bigint_modulus_long(zend_bigint *out, const zend_bigint *num, zend_long divisor) /* {{{ */
{
	WITH_TEMP_MP_FROM_ZEND_LONG(divisor, divisor_mp, {
		CHECK_ERROR(mp_div((mp_int*)&num->mp, &divisor_mp, NULL, &out->mp));
	})
}
/* }}} */

ZEND_API void zend_bigint_long_modulus(zend_bigint *out, zend_long num, const zend_bigint *divisor) /* {{{ */
{
	WITH_TEMP_MP_FROM_ZEND_LONG(num, num_mp, {
		CHECK_ERROR(mp_div(&num_mp, (mp_int*)&divisor->mp, NULL, &out->mp));
	})
}
/* }}} */

ZEND_API void zend_bigint_ones_complement(zend_bigint *out, const zend_bigint *op) /* {{{ */
{
	/* A one's complement, aka bitwise NOT, is actually just -a - 1 */
	CHECK_ERROR(mp_neg((mp_int*)&op->mp, &out->mp));
	CHECK_ERROR(mp_sub_d(&out->mp, 1, &out->mp));
}
/* }}} */

static void zend_always_inline _zend_bigint_twos_complement(mp_int *out, mp_int *in, int size) /* {{{ */
{
	mp_int tmp;

	/* two's complement of a number is 2^width - n
	 * so first we need 2^width
	 */
	CHECK_ERROR(mp_init(&tmp));
	CHECK_ERROR(mp_set_int(&tmp, 1));
	CHECK_ERROR(mp_mul_2d(&tmp, size + 1, &tmp));

	/* then, we subtract (or add if our number is already negative) */
	if (SIGN(in) == MP_NEG) {
		CHECK_ERROR(mp_add(&tmp, in, out));
	} else {
		CHECK_ERROR(mp_sub(&tmp, in, out));
	}

	mp_clear(&tmp);
}
/* }}} */

static void zend_always_inline _zend_bigint_bitwise_op(mp_int *out, mp_int *real_op1, mp_int *real_op2, char op) /* {{{ */
{
	zend_bool op1_sign, op2_sign, sign;
	mp_int op1_copy, op2_copy;
	int largest_bit_width;

	op1_sign = (SIGN(real_op1) == MP_NEG);
	op2_sign = (SIGN(real_op2) == MP_NEG);

	/* to emulate two's-complement OR, get twos' complement of negatives */
	largest_bit_width = MAX(mp_count_bits(real_op1), mp_count_bits(real_op2));
	if (op1_sign) {
		CHECK_ERROR(mp_init(&op1_copy));
		_zend_bigint_twos_complement(&op1_copy, real_op1, largest_bit_width);
		real_op1 = &op1_copy;
	}
	if (op2_sign) {
		CHECK_ERROR(mp_init(&op2_copy));
		_zend_bigint_twos_complement(&op2_copy, real_op2, largest_bit_width); 
		real_op2 = &op2_copy;
	}

	switch (op) {
		case '|':
			CHECK_ERROR(mp_or(real_op1, real_op2, out));
			sign = op1_sign | op2_sign;
			break;
		case '^':
			CHECK_ERROR(mp_xor(real_op1, real_op2, out));
			sign = op1_sign ^ op2_sign;
			break;
		case '&':
			CHECK_ERROR(mp_and(real_op1, real_op2, out));
			sign = op1_sign & op2_sign;
			break;
	}

	/* since the sign is negative, we need to undo twos' complement */
	if (sign) {
		_zend_bigint_twos_complement(out, out, largest_bit_width);
		mp_neg(out, out);
	}

	if (op1_sign) {
		mp_clear(&op1_copy);
	}
	if (op2_sign) {
		mp_clear(&op2_copy);
	}
}
/* }}} */

ZEND_API void zend_bigint_or(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	_zend_bigint_bitwise_op(&out->mp, (mp_int*)&op1->mp, (mp_int*)&op2->mp, '|');
}
/* }}} */

ZEND_API void zend_bigint_or_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
		_zend_bigint_bitwise_op(&out->mp, (mp_int*)&op1->mp, &op2_mp, '|');
	})
}
/* }}} */

ZEND_API void zend_bigint_and(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	_zend_bigint_bitwise_op(&out->mp, (mp_int*)&op1->mp, (mp_int*)&op2->mp, '&');
}
/* }}} */

ZEND_API void zend_bigint_and_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
		_zend_bigint_bitwise_op(&out->mp, (mp_int*)&op1->mp, &op2_mp, '&');
	})
}
/* }}} */

ZEND_API void zend_bigint_xor(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	_zend_bigint_bitwise_op(&out->mp, (mp_int*)&op1->mp, (mp_int*)&op2->mp, '^');
}
/* }}} */

ZEND_API void zend_bigint_xor_long(zend_bigint *out, const zend_bigint *op1, zend_long op2) /* {{{ */
{
	WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
		_zend_bigint_bitwise_op(&out->mp, (mp_int*)&op1->mp, &op2_mp, '^');
	})
}
/* }}} */

ZEND_API void zend_bigint_shift_left_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift) /* {{{ */
{
	if (shift > INT_MAX) {
		zend_error_noreturn(E_ERROR, "Exponent too large");
	}

	CHECK_ERROR(mp_mul_2d((mp_int*)&num->mp, shift, &out->mp));
}
/* }}} */

ZEND_API void zend_bigint_long_shift_left_ulong(zend_bigint *out, zend_long num, zend_ulong shift) /* {{{ */
{
	if (shift > INT_MAX) {
		zend_error_noreturn(E_ERROR, "Exponent too large");
	}


	WITH_TEMP_MP_FROM_ZEND_LONG(num, num_mp, {
		CHECK_ERROR(mp_mul_2d(&num_mp, shift, &out->mp));
	})
}
/* }}} */

ZEND_API void zend_bigint_shift_right_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift) /* {{{ */
{
	if (shift > INT_MAX) {
		zend_error_noreturn(E_ERROR, "Exponent too large");
	}

	/* LibTomMath's mp_div_2d does a logical right shift
	 * in order to correctly mimick a two's-complement arithmetic right-shift,
	 * we just need to do a one's complement first */
	if (SIGN(&num->mp) == MP_NEG) {
		zend_bigint_ones_complement(out, num);
		CHECK_ERROR(mp_div_2d(&out->mp, shift, &out->mp, NULL));
		zend_bigint_ones_complement(out, out);
	/* No mimickry needed for positive op1 :) */
	} else {
		CHECK_ERROR(mp_div_2d((mp_int*)&num->mp, shift, &out->mp, NULL));
	}
}
/* }}} */

ZEND_API int zend_bigint_cmp(const zend_bigint *op1, const zend_bigint *op2) /* {{{ */
{
	return mp_cmp((mp_int*)&op1->mp, (mp_int*)&op2->mp);
}
/* }}} */

ZEND_API int zend_bigint_cmp_long(const zend_bigint *op1, zend_long op2) /* {{{ */
{
	int provisional_sign =mp_sgn(&op1->mp) - int_sgn(op2);
	if (provisional_sign != 0) {
		return provisional_sign;
	} else if (FITS_DIGIT(op2) && op2 > 0) {
		return mp_cmp_d((mp_int*)&op1->mp, op2);
	} else {
		WITH_TEMP_MP_FROM_ZEND_LONG(op2, op2_mp, {
			provisional_sign = mp_cmp((mp_int*)&op1->mp, &op2_mp);
		})
		return provisional_sign;
	}
}
/* }}} */

ZEND_API int zend_bigint_cmp_double(const zend_bigint *op1, double op2) /* {{{ */
{
	int provisional_sign = mp_sgn(&op1->mp) - double_sgn(op2);
	if (provisional_sign != 0) {
		return provisional_sign;
	} else {
		/* FIXME: Handle larger doubles */
		return zend_bigint_cmp_long(op1, zend_dval_to_lval(op2));
	}
}
/* }}} */

ZEND_API void zend_bigint_abs(zend_bigint *out, const zend_bigint *big) /* {{{ */
{
	CHECK_ERROR(mp_abs((mp_int*)&big->mp, &out->mp));
}

#endif /* ZEND_HAVE_LIBTOMMATH */
