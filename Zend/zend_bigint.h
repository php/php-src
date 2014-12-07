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

#ifndef ZEND_BIGINT_H
#define ZEND_BIGINT_H

#include <gmp.h>

#include "zend.h"
#include "zend_types.h"

/* All documentation for bigint functions is found in zend_bigint.c */

/*** INTERNAL FUNCTIONS ***/

void zend_startup_bigint(void);

/*** INITIALISERS ***/

ZEND_API zend_bigint* zend_bigint_alloc(void);
ZEND_API void zend_bigint_init(zend_bigint *big);
ZEND_API zend_bigint* zend_bigint_init_alloc(void);
ZEND_API int zend_bigint_init_from_string(zend_bigint *big, const char *str, int base);
ZEND_API int zend_bigint_init_from_string_length(zend_bigint *big, const char *str, size_t length, int base);
ZEND_API void zend_bigint_init_strtol(zend_bigint *big, const char *str, char** endptr, int base);
ZEND_API void zend_bigint_init_from_long(zend_bigint *big, zend_long value);
ZEND_API void zend_bigint_init_from_double(zend_bigint *big, double value);
ZEND_API void zend_bigint_init_dup(zend_bigint *big, const zend_bigint *source);
ZEND_API void zend_bigint_dtor(zend_bigint *big);
ZEND_API void zend_bigint_release(zend_bigint *big);

/*** INFORMATION ***/

ZEND_API zend_bool zend_bigint_can_fit_ulong(const zend_bigint *big);
ZEND_API zend_bool zend_bigint_can_fit_long(const zend_bigint *big);
ZEND_API int zend_bigint_sign(const zend_bigint *big);
ZEND_API zend_bool zend_bigint_divisible(const zend_bigint *num, const zend_bigint *divisor);
ZEND_API zend_bool zend_bigint_divisible_long(const zend_bigint *num, zend_long divisor);
ZEND_API zend_bool zend_bigint_long_divisible(zend_long num, const zend_bigint *divisor);

/*** CONVERTORS ***/

ZEND_API zend_long zend_bigint_to_long(const zend_bigint *big);
ZEND_API zend_long zend_bigint_to_long_saturate(const zend_bigint *big);
ZEND_API zend_long zend_bigint_to_long_ex(const zend_bigint *big, zend_bool *overflow);
ZEND_API zend_ulong zend_bigint_to_ulong(const zend_bigint *big);
ZEND_API zend_bool zend_bigint_to_bool(const zend_bigint *big);
ZEND_API double zend_bigint_to_double(const zend_bigint *big);
ZEND_API char* zend_bigint_to_string(const zend_bigint *big);
ZEND_API zend_string* zend_bigint_to_zend_string(const zend_bigint *big, int persistent);
ZEND_API char* zend_bigint_to_string_base(const zend_bigint *big, int base);

/*** OPERATIONS **/

ZEND_API void zend_bigint_add(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_add_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);
ZEND_API void zend_bigint_long_add_long(zend_bigint *out, zend_long op1, zend_long op2);
ZEND_API void zend_bigint_subtract(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_subtract_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);
ZEND_API void zend_long_subtract_long(zend_bigint *out, zend_long op1, zend_long op2);
ZEND_API void zend_bigint_long_subtract(zend_bigint *out, zend_long op1, const zend_bigint *op2);
ZEND_API void zend_bigint_long_subtract_long(zend_bigint *out, zend_long op1, zend_long op2);
ZEND_API void zend_bigint_multiply(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_multiply_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);
ZEND_API void zend_bigint_long_multiply_long(zend_bigint *out, zend_long op1, zend_long op2);
ZEND_API void zend_bigint_pow_ulong(zend_bigint *out, const zend_bigint *base, zend_ulong power);
ZEND_API void zend_bigint_long_pow_ulong(zend_bigint *out, zend_long base, zend_ulong power);
ZEND_API void zend_bigint_divide(zend_bigint *out, const zend_bigint *big, const zend_bigint *divisor);
ZEND_API double zend_bigint_divide_as_double(const zend_bigint *num, const zend_bigint *divisor);
ZEND_API void zend_bigint_divide_long(zend_bigint *out, const zend_bigint *big, zend_long divisor);
ZEND_API double zend_bigint_divide_long_as_double(const zend_bigint *num, zend_long divisor);
ZEND_API void zend_bigint_long_divide(zend_bigint *out, zend_long big, const zend_bigint *divisor);
ZEND_API double zend_bigint_long_divide_as_double(zend_long num, const zend_bigint *divisor);
ZEND_API void zend_bigint_modulus(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor);
ZEND_API void zend_bigint_modulus_long(zend_bigint *out, const zend_bigint *num, zend_long divisor);
ZEND_API void zend_bigint_long_modulus(zend_bigint *out, zend_long num, const zend_bigint *divisor);
ZEND_API void zend_bigint_ones_complement(zend_bigint *out, const zend_bigint *op);
ZEND_API void zend_bigint_or(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_or_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);
ZEND_API void zend_bigint_and(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_and_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);
ZEND_API void zend_bigint_xor(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_xor_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);
ZEND_API void zend_bigint_shift_left_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift);
ZEND_API void zend_bigint_long_shift_left_ulong(zend_bigint *out, zend_long num, zend_ulong shift);
ZEND_API void zend_bigint_shift_right_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift);
ZEND_API int zend_bigint_cmp(const zend_bigint *op1, const zend_bigint *op2);
ZEND_API int zend_bigint_cmp_long(const zend_bigint *op1, zend_long op2);
ZEND_API int zend_bigint_cmp_double(const zend_bigint *op1, double op2);
ZEND_API void zend_bigint_abs(zend_bigint *out, const zend_bigint *big);

#endif
