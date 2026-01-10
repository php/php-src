/*
 * Copyright (C) Alexander Borisov
 *
 * Based on nxt_strtod.c from NGINX NJS project
 *
 * An internal strtod() implementation based upon V8 src/strtod.cc
 * without bignum support.
 *
 * Copyright 2012 the V8 project authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license
 * that can be found in the LICENSE file.
 */

#include <stdint.h>
#include <math.h>

#include "lexbor/core/diyfp.h"
#include "lexbor/core/strtod.h"


/*
 * Max double: 1.7976931348623157 x 10^308
 * Min non-zero double: 4.9406564584124654 x 10^-324
 * Any x >= 10^309 is interpreted as +infinity.
 * Any x <= 10^-324 is interpreted as 0.
 * Note that 2.5e-324 (despite being smaller than the min double)
 * will be read as non-zero (equal to the min non-zero double).
 */

#define LEXBOR_DECIMAL_POWER_MAX 309
#define LEXBOR_DECIMAL_POWER_MIN (-324)

#define LEXBOR_UINT64_MAX lexbor_uint64_hl(0xFFFFFFFF, 0xFFFFFFFF)
#define LEXBOR_UINT64_DECIMAL_DIGITS_MAX 19

#define LEXBOR_DENOM_LOG   3
#define LEXBOR_DENOM       (1 << LEXBOR_DENOM_LOG)


static lexbor_diyfp_t
lexbor_strtod_diyfp_read(const lxb_char_t *start, size_t length,
                         int *remaining);

static double
lexbor_strtod_diyfp_strtod(const lxb_char_t *start, size_t length, int exp);


/*
 * Reads digits from the buffer and converts them to a uint64.
 * Reads in as many digits as fit into a uint64.
 * When the string starts with "1844674407370955161" no further digit is read.
 * Since 2^64 = 18446744073709551616 it would still be possible read another
 * digit if it was less or equal than 6, but this would complicate the code.
 */
lxb_inline uint64_t
lexbor_strtod_read_uint64(const lxb_char_t *start, size_t length,
                          size_t *ndigits)
{
    lxb_char_t d;
    uint64_t value;
    const lxb_char_t *p, *e;

    value = 0;

    p = start;
    e = p + length;

    while (p < e && value <= (UINT64_MAX / 10 - 1)) {
        d = *p++ - '0';
        value = 10 * value + d;
    }

    *ndigits = p - start;

    return value;
}

/*
 * Reads a nxt_diyfp_t from the buffer.
 * The returned nxt_diyfp_t is not necessarily normalized.
 * If remaining is zero then the returned nxt_diyfp_t is accurate.
 * Otherwise it has been rounded and has error of at most 1/2 ulp.
 */
static lexbor_diyfp_t
lexbor_strtod_diyfp_read(const lxb_char_t *start, size_t length, int *remaining)
{
    size_t read;
    uint64_t significand;

    significand = lexbor_strtod_read_uint64(start, length, &read);

    /* Round the significand. */

    if (length != read) {
        if (start[read] >= '5') {
            significand++;
        }
    }

    *remaining = (int) (length - read);

    return lexbor_diyfp(significand, 0);
}

/*
 * Returns 10^exp as an exact nxt_diyfp_t.
 * The given exp must be in the range [1; NXT_DECIMAL_EXPONENT_DIST[.
 */
lxb_inline lexbor_diyfp_t
lexbor_strtod_adjust_pow10(int exp)
{
    switch (exp) {
    case 1:
        return lexbor_diyfp(lexbor_uint64_hl(0xa0000000, 00000000), -60);
    case 2:
        return lexbor_diyfp(lexbor_uint64_hl(0xc8000000, 00000000), -57);
    case 3:
        return lexbor_diyfp(lexbor_uint64_hl(0xfa000000, 00000000), -54);
    case 4:
        return lexbor_diyfp(lexbor_uint64_hl(0x9c400000, 00000000), -50);
    case 5:
        return lexbor_diyfp(lexbor_uint64_hl(0xc3500000, 00000000), -47);
    case 6:
        return lexbor_diyfp(lexbor_uint64_hl(0xf4240000, 00000000), -44);
    case 7:
        return lexbor_diyfp(lexbor_uint64_hl(0x98968000, 00000000), -40);
    default:
        return lexbor_diyfp(0, 0);
    }
}

/*
 * Returns the significand size for a given order of magnitude.
 * If v = f*2^e with 2^p-1 <= f <= 2^p then p+e is v's order of magnitude.
 * This function returns the number of significant binary digits v will have
 * once its encoded into a double. In almost all cases this is equal to
 * NXT_SIGNIFICAND_SIZE. The only exception are denormals. They start with
 * leading zeroes and their effective significand-size is hence smaller.
 */
lxb_inline int
lexbor_strtod_diyfp_sgnd_size(int order)
{
    if (order >= (LEXBOR_DBL_EXPONENT_DENORMAL + LEXBOR_SIGNIFICAND_SIZE)) {
        return LEXBOR_SIGNIFICAND_SIZE;
    }

    if (order <= LEXBOR_DBL_EXPONENT_DENORMAL) {
        return 0;
    }

    return order - LEXBOR_DBL_EXPONENT_DENORMAL;
}

/*
 * Returns either the correct double or the double that is just below
 * the correct double.
 */
static double
lexbor_strtod_diyfp_strtod(const lxb_char_t *start, size_t length, int exp)
{
    int magnitude, prec_digits;
    int remaining, dec_exp, adj_exp, orig_e, shift;
    int64_t error;
    uint64_t prec_bits, half_way;
    lexbor_diyfp_t value, pow, adj_pow, rounded;

    value = lexbor_strtod_diyfp_read(start, length, &remaining);

    exp += remaining;

    /*
     * Since some digits may have been dropped the value is not accurate.
     * If remaining is different than 0 than the error is at most .5 ulp
     * (unit in the last place).
     * Using a common denominator to avoid dealing with fractions.
     */

    error = (remaining == 0 ? 0 : LEXBOR_DENOM / 2);

    orig_e = value.exp;
    value = lexbor_diyfp_normalize(value);
    error <<= orig_e - value.exp;

    if (exp < LEXBOR_DECIMAL_EXPONENT_MIN) {
        return 0.0;
    }

    pow = lexbor_cached_power_dec(exp, &dec_exp);

    if (dec_exp != exp) {
        adj_exp = exp - dec_exp;
        adj_pow = lexbor_strtod_adjust_pow10(exp - dec_exp);
        value = lexbor_diyfp_mul(value, adj_pow);

        if (LEXBOR_UINT64_DECIMAL_DIGITS_MAX - (int) length < adj_exp) {
            /*
             * The adjustment power is exact. There is hence only
             * an error of 0.5.
             */
            error += LEXBOR_DENOM / 2;
        }
    }

    value = lexbor_diyfp_mul(value, pow);

    /*
     * The error introduced by a multiplication of a * b equals
     *  error_a + error_b + error_a * error_b / 2^64 + 0.5
     *  Substituting a with 'value' and b with 'pow':
     *  error_b = 0.5  (all cached powers have an error of less than 0.5 ulp),
     *  error_ab = 0 or 1 / NXT_DENOM > error_a * error_b / 2^64.
     */

    error += LEXBOR_DENOM + (error != 0 ? 1 : 0);

    orig_e = value.exp;
    value = lexbor_diyfp_normalize(value);
    error <<= orig_e - value.exp;

    /*
     * Check whether the double's significand changes when the error is added
     * or subtracted.
     */

    magnitude = LEXBOR_DIYFP_SIGNIFICAND_SIZE + value.exp;
    prec_digits = LEXBOR_DIYFP_SIGNIFICAND_SIZE
        - lexbor_strtod_diyfp_sgnd_size(magnitude);

    if (prec_digits + LEXBOR_DENOM_LOG >= LEXBOR_DIYFP_SIGNIFICAND_SIZE) {
        /*
         * This can only happen for very small denormals. In this case the
         * half-way multiplied by the denominator exceeds the range of uint64.
         * Simply shift everything to the right.
         */
        shift = prec_digits + LEXBOR_DENOM_LOG
            - LEXBOR_DIYFP_SIGNIFICAND_SIZE + 1;

        value = lexbor_diyfp_shift_right(value, shift);

        /*
         * Add 1 for the lost precision of error, and NXT_DENOM
         * for the lost precision of value.significand.
         */
        error = (error >> shift) + 1 + LEXBOR_DENOM;
        prec_digits -= shift;
    }

    prec_bits = value.significand & (((uint64_t) 1 << prec_digits) - 1);
    prec_bits *= LEXBOR_DENOM;

    half_way = (uint64_t) 1 << (prec_digits - 1);
    half_way *= LEXBOR_DENOM;

    rounded = lexbor_diyfp_shift_right(value, prec_digits);

    if (prec_bits >= half_way + error) {
        rounded.significand++;
    }

    return lexbor_diyfp_2d(rounded);
}

double
lexbor_strtod_internal(const lxb_char_t *start, size_t length, int exp)
{
    size_t left, right;
    const lxb_char_t *p, *e, *b;

    /* Trim leading zeroes. */

    p = start;
    e = p + length;

    while (p < e) {
        if (*p != '0') {
            start = p;
            break;
        }

        p++;
    }

    left = e - p;

    /* Trim trailing zeroes. */

    b = start;
    p = b + left - 1;

    while (p > b) {
        if (*p != '0') {
            break;
        }

        p--;
    }

    right = p - b + 1;

    length = right;

    if (length == 0) {
        return 0.0;
    }

    exp += (int) (left - right);

    if (exp + (int) length - 1 >= LEXBOR_DECIMAL_POWER_MAX) {
        return INFINITY;
    }

    if (exp + (int) length <= LEXBOR_DECIMAL_POWER_MIN) {
        return 0.0;
    }

    return lexbor_strtod_diyfp_strtod(start, length, exp);
}

#undef LEXBOR_DECIMAL_POWER_MAX
#undef LEXBOR_DECIMAL_POWER_MIN

#undef LEXBOR_UINT64_MAX
#undef LEXBOR_UINT64_DECIMAL_DIGITS_MAX

#undef LEXBOR_DENOM_LOG
#undef LEXBOR_DENOM
