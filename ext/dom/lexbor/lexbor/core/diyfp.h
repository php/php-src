/*
 * Copyright (C) Alexander Borisov
 *
 * Based on nxt_diyfp.h from NGINX NJS project
 *
 * Copyright (C) Dmitry Volyntsev
 * Copyright (C) NGINX, Inc.
 *
 * An internal diy_fp implementation.
 * For details, see Loitsch, Florian. "Printing floating-point numbers quickly
 * and accurately with integers." ACM Sigplan Notices 45.6 (2010): 233-243.
 */

#ifndef LEXBOR_DIYFP_H
#define LEXBOR_DIYFP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"

#include <math.h>


#ifdef __cplusplus
#define lexbor_diyfp(_s, _e)           { .significand = (_s), .exp = (int) (_e) }
#else
#define lexbor_diyfp(_s, _e)           (lexbor_diyfp_t)                        \
                                       { .significand = (_s), .exp = (int) (_e) }
#endif
#define lexbor_uint64_hl(h, l)   (((uint64_t) (h) << 32) + (l))


#define LEXBOR_DBL_SIGNIFICAND_SIZE    52
#define LEXBOR_DBL_EXPONENT_BIAS       (0x3FF + LEXBOR_DBL_SIGNIFICAND_SIZE)
#define LEXBOR_DBL_EXPONENT_MIN        (-LEXBOR_DBL_EXPONENT_BIAS)
#define LEXBOR_DBL_EXPONENT_MAX        (0x7FF - LEXBOR_DBL_EXPONENT_BIAS)
#define LEXBOR_DBL_EXPONENT_DENORMAL   (-LEXBOR_DBL_EXPONENT_BIAS + 1)

#define LEXBOR_DBL_SIGNIFICAND_MASK    lexbor_uint64_hl(0x000FFFFF, 0xFFFFFFFF)
#define LEXBOR_DBL_HIDDEN_BIT          lexbor_uint64_hl(0x00100000, 0x00000000)
#define LEXBOR_DBL_EXPONENT_MASK       lexbor_uint64_hl(0x7FF00000, 0x00000000)

#define LEXBOR_DIYFP_SIGNIFICAND_SIZE  64

#define LEXBOR_SIGNIFICAND_SIZE        53
#define LEXBOR_SIGNIFICAND_SHIFT       (LEXBOR_DIYFP_SIGNIFICAND_SIZE          \
                                       - LEXBOR_DBL_SIGNIFICAND_SIZE)

#define LEXBOR_DECIMAL_EXPONENT_OFF    348
#define LEXBOR_DECIMAL_EXPONENT_MIN    (-348)
#define LEXBOR_DECIMAL_EXPONENT_MAX    340
#define LEXBOR_DECIMAL_EXPONENT_DIST   8


typedef struct {
    uint64_t significand;
    int      exp;
}
lexbor_diyfp_t;


LXB_API lexbor_diyfp_t
lexbor_cached_power_dec(int exp, int *dec_exp);

LXB_API lexbor_diyfp_t
lexbor_cached_power_bin(int exp, int *dec_exp);


/*
 * Inline functions
 */
#if (LEXBOR_HAVE_BUILTIN_CLZLL)
#define nxt_leading_zeros64(x)  (((x) == 0) ? 64 : __builtin_clzll(x))

#else

lxb_inline uint64_t
lexbor_diyfp_leading_zeros64(uint64_t x)
{
    uint64_t  n;

    if (x == 0) {
        return 64;
    }

    n = 0;

    while ((x & 0x8000000000000000) == 0) {
        n++;
        x <<= 1;
    }

    return n;
}

#endif

lxb_inline lexbor_diyfp_t
lexbor_diyfp_from_d2(double d)
{
    int biased_exp;
    uint64_t significand;
    lexbor_diyfp_t r;

    union {
        double   d;
        uint64_t u64;
    }
    u;

    u.d = d;

    biased_exp = (u.u64 & LEXBOR_DBL_EXPONENT_MASK)
                 >> LEXBOR_DBL_SIGNIFICAND_SIZE;
    significand = u.u64 & LEXBOR_DBL_SIGNIFICAND_MASK;

    if (biased_exp != 0) {
        r.significand = significand + LEXBOR_DBL_HIDDEN_BIT;
        r.exp = biased_exp - LEXBOR_DBL_EXPONENT_BIAS;
    }
    else {
        r.significand = significand;
        r.exp = LEXBOR_DBL_EXPONENT_MIN + 1;
    }

    return r;
}

lxb_inline double
lexbor_diyfp_2d(lexbor_diyfp_t v)
{
    int exp;
    uint64_t significand, biased_exp;

    union {
        double   d;
        uint64_t u64;
    }
    u;

    exp = v.exp;
    significand = v.significand;

    while (significand > LEXBOR_DBL_HIDDEN_BIT + LEXBOR_DBL_SIGNIFICAND_MASK) {
        significand >>= 1;
        exp++;
    }

    if (exp >= LEXBOR_DBL_EXPONENT_MAX) {
        return INFINITY;
    }

    if (exp < LEXBOR_DBL_EXPONENT_DENORMAL) {
        return 0.0;
    }

    while (exp > LEXBOR_DBL_EXPONENT_DENORMAL
           && (significand & LEXBOR_DBL_HIDDEN_BIT) == 0)
    {
        significand <<= 1;
        exp--;
    }

    if (exp == LEXBOR_DBL_EXPONENT_DENORMAL
        && (significand & LEXBOR_DBL_HIDDEN_BIT) == 0)
    {
        biased_exp = 0;

    } else {
        biased_exp = (uint64_t) (exp + LEXBOR_DBL_EXPONENT_BIAS);
    }

    u.u64 = (significand & LEXBOR_DBL_SIGNIFICAND_MASK)
            | (biased_exp << LEXBOR_DBL_SIGNIFICAND_SIZE);

    return u.d;
}

lxb_inline lexbor_diyfp_t
lexbor_diyfp_shift_left(lexbor_diyfp_t v, unsigned shift)
{
    return lexbor_diyfp(v.significand << shift, v.exp - shift);
}

lxb_inline lexbor_diyfp_t
lexbor_diyfp_shift_right(lexbor_diyfp_t v, unsigned shift)
{
    return lexbor_diyfp(v.significand >> shift, v.exp + shift);
}

lxb_inline lexbor_diyfp_t
lexbor_diyfp_sub(lexbor_diyfp_t lhs, lexbor_diyfp_t rhs)
{
    return lexbor_diyfp(lhs.significand - rhs.significand, lhs.exp);
}

lxb_inline lexbor_diyfp_t
lexbor_diyfp_mul(lexbor_diyfp_t lhs, lexbor_diyfp_t rhs)
{
#if (LEXBOR_HAVE_UNSIGNED_INT128)

    uint64_t l, h;
    lxb_uint128_t u128;

    u128 = (lxb_uint128_t) (lhs.significand)
           * (lxb_uint128_t) (rhs.significand);

    h = u128 >> 64;
    l = (uint64_t) u128;

    /* rounding. */

    if (l & ((uint64_t) 1 << 63)) {
        h++;
    }

    return lexbor_diyfp(h, lhs.exp + rhs.exp + 64);

#else

    uint64_t a, b, c, d, ac, bc, ad, bd, tmp;

    a = lhs.significand >> 32;
    b = lhs.significand & 0xffffffff;
    c = rhs.significand >> 32;
    d = rhs.significand & 0xffffffff;

    ac = a * c;
    bc = b * c;
    ad = a * d;
    bd = b * d;

    tmp = (bd >> 32) + (ad & 0xffffffff) + (bc & 0xffffffff);

    /* mult_round. */

    tmp += 1U << 31;

    return lexbor_diyfp(ac + (ad >> 32) + (bc >> 32) + (tmp >> 32),
                        lhs.exp + rhs.exp + 64);
#endif
}

lxb_inline lexbor_diyfp_t
lexbor_diyfp_normalize(lexbor_diyfp_t v)
{
    return lexbor_diyfp_shift_left(v,
                        (unsigned) lexbor_diyfp_leading_zeros64(v.significand));
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DIYFP_H */
