/*
 * Copyright (C) Alexander Borisov
 *
 * Based on nxt_dtoa.c from NGINX NJS project
 *
 * Copyright (C) Dmitry Volyntsev
 * Copyright (C) NGINX, Inc.
 *
 * Grisu2 algorithm implementation for printing floating-point numbers based
 * upon the work of Milo Yip and Doug Currie.
 *
 * For algorithm information, see Loitsch, Florian. "Printing
 * floating-point numbers quickly and accurately with integers." ACM Sigplan
 * Notices 45.6 (2010): 233-243.
 *
 * Copyright (C) 2015 Doug Currie
 * based on dtoa_milo.h
 * Copyright (C) 2014 Milo Yip
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "lexbor/core/str.h"
#include "lexbor/core/diyfp.h"
#include "lexbor/core/dtoa.h"

#include <math.h>
#include <string.h>


lxb_inline void
lexbor_grisu2_round(lxb_char_t *start, size_t len, uint64_t delta, uint64_t rest,
                    uint64_t ten_kappa, uint64_t wp_w)
{
    while (rest < wp_w && delta - rest >= ten_kappa
           && (rest + ten_kappa < wp_w ||  /* closer */
               wp_w - rest > rest + ten_kappa - wp_w))
    {
        start[len - 1]--;
        rest += ten_kappa;
    }
}

lxb_inline int
lexbor_dec_count(uint32_t n)
{
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;

    return 10;
}

lxb_inline size_t
lexbor_grisu2_gen(lexbor_diyfp_t W, lexbor_diyfp_t Mp, uint64_t delta,
                  lxb_char_t *begin, lxb_char_t *end, int *dec_exp)
{
    int kappa;
    lxb_char_t c, *p;
    uint32_t p1, d;
    uint64_t p2, tmp;
    lexbor_diyfp_t one, wp_w;

    static const uint64_t pow10[] = {
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000
    };

    wp_w = lexbor_diyfp_sub(Mp, W);

    one = lexbor_diyfp((uint64_t) 1 << -Mp.exp, Mp.exp);
    p1 = (uint32_t) (Mp.significand >> -one.exp);
    p2 = Mp.significand & (one.significand - 1);

    p = begin;

    /* GCC 4.2 complains about uninitialized d. */
    d = 0;

    kappa = lexbor_dec_count(p1);

    while (kappa > 0) {
        switch (kappa) {
            case 10: d = p1 / 1000000000; p1 %= 1000000000; break;
            case  9: d = p1 /  100000000; p1 %=  100000000; break;
            case  8: d = p1 /   10000000; p1 %=   10000000; break;
            case  7: d = p1 /    1000000; p1 %=    1000000; break;
            case  6: d = p1 /     100000; p1 %=     100000; break;
            case  5: d = p1 /      10000; p1 %=      10000; break;
            case  4: d = p1 /       1000; p1 %=       1000; break;
            case  3: d = p1 /        100; p1 %=        100; break;
            case  2: d = p1 /         10; p1 %=         10; break;
            case  1: d = p1;              p1 =           0; break;
            default:
                /* Never go here. */
                return 0;
        }

        if (d != 0 || p != begin) {
            *p = '0' + d;

            p += 1;
            if (p == end) {
                return p - begin;
            }
        }

        kappa--;

        tmp = ((uint64_t) p1 << -one.exp) + p2;

        if (tmp <= delta) {
            *dec_exp += kappa;
            lexbor_grisu2_round(begin, p - begin, delta, tmp,
                                pow10[kappa] << -one.exp, wp_w.significand);
            return p - begin;
        }
    }

    /* kappa = 0. */

    for ( ;; ) {
        p2 *= 10;
        delta *= 10;
        c = (char) (p2 >> -one.exp);

        if (c != 0 || p != begin) {
            *p = '0' + c;

            p += 1;
            if (p == end) {
                return p - begin;
            }
        }

        p2 &= one.significand - 1;
        kappa--;

        if (p2 < delta) {
            *dec_exp += kappa;
            tmp = (-kappa < 10) ? pow10[-kappa] : 0;
            lexbor_grisu2_round(begin, p - begin, delta, p2, one.significand,
                                wp_w.significand * tmp);
            break;
        }
    }

    return p - begin;
}

lxb_inline lexbor_diyfp_t
lexbor_diyfp_normalize_boundary(lexbor_diyfp_t v)
{
    while ((v.significand & (LEXBOR_DBL_HIDDEN_BIT << 1)) == 0) {
            v.significand <<= 1;
            v.exp--;
    }

    return lexbor_diyfp_shift_left(v, LEXBOR_SIGNIFICAND_SHIFT - 2);
}

lxb_inline void
lexbor_diyfp_normalize_boundaries(lexbor_diyfp_t v, lexbor_diyfp_t* minus,
                                  lexbor_diyfp_t* plus)
{
    lexbor_diyfp_t pl, mi;

    pl = lexbor_diyfp_normalize_boundary(lexbor_diyfp((v.significand << 1) + 1,
                                                      v.exp - 1));

    if (v.significand == LEXBOR_DBL_HIDDEN_BIT) {
        mi = lexbor_diyfp((v.significand << 2) - 1, v.exp - 2);

    } else {
        mi = lexbor_diyfp((v.significand << 1) - 1, v.exp - 1);
    }

    mi.significand <<= mi.exp - pl.exp;
    mi.exp = pl.exp;

    *plus = pl;
    *minus = mi;
}

lxb_inline size_t
lexbor_grisu2(double value, lxb_char_t *begin, lxb_char_t *end, int *dec_exp)
{
    lexbor_diyfp_t v, w_m, w_p, c_mk, W, Wp, Wm;

    v = lexbor_diyfp_from_d2(value);

    lexbor_diyfp_normalize_boundaries(v, &w_m, &w_p);

    c_mk = lexbor_cached_power_bin(w_p.exp, dec_exp);
    W = lexbor_diyfp_mul(lexbor_diyfp_normalize(v), c_mk);

    Wp = lexbor_diyfp_mul(w_p, c_mk);
    Wm = lexbor_diyfp_mul(w_m, c_mk);

    Wm.significand++;
    Wp.significand--;

   return lexbor_grisu2_gen(W, Wp, Wp.significand - Wm.significand, begin, end,
                            dec_exp);
}

lxb_inline size_t
lexbor_write_exponent(int exp, lxb_char_t *begin, lxb_char_t *end)
{
    char *p;
    size_t len;
    uint32_t u32;
    char buf[4];

    /* -324 <= exp <= 308. */

    if ((begin + (sizeof(buf) - 1) + 1) >= end) {
        return 0;
    }

    if (exp < 0) {
        *begin = '-';
        begin += 1;

        exp = -exp;
    }
    else {
        *begin++ = '+';
    }

    u32 = exp;
    p = buf + (sizeof(buf) - 1);

    do {
        *--p = u32 % 10 + '0';
        u32 /= 10;
    }
    while (u32 != 0);

    len = buf + (sizeof(buf) - 1) - p;

    memcpy(begin, p, len);

    return len + 1;
}

lxb_inline size_t
lexbor_prettify(lxb_char_t *begin, lxb_char_t *end, size_t len, int dec_exp)
{
    int kk, offset, length;
    size_t size;

    /* 10^(kk-1) <= v < 10^kk */

    length = (int) len;
    kk = length + dec_exp;

    if (length <= kk && kk <= 21) {
        /* 1234e7 -> 12340000000 */

        if (kk - length > 0) {
            if ((&begin[length] + (kk - length)) < end) {
                memset(&begin[length], '0', kk - length);
            }
            else {
                memset(&begin[length], '0', (end - &begin[length]));
            }
        }

        return kk;
    }
    else if (0 < kk && kk <= 21) {
        /* 1234e-2 -> 12.34 */

        if ((&begin[kk + 1] + (length - kk)) >= end) {
            return length;
        }

        memmove(&begin[kk + 1], &begin[kk], length - kk);
        begin[kk] = '.';

        return (length + 1);
    }
    else if (-6 < kk && kk <= 0) {
        /* 1234e-6 -> 0.001234 */

        offset = 2 - kk;
        if ((&begin[offset] + length) >= end
            || (begin + 2) >= end)
        {
            return length;
        }

        memmove(&begin[offset], begin, length);
        begin[0] = '0';
        begin[1] = '.';

        if (offset - 2 > 0) {
            if ((&begin[2] + (offset - 2)) >= end) {
                return length;
            }

            memset(&begin[2], '0', offset - 2);
        }

        return (length + offset);
    }
    else if (length == 1) {
        /* 1e30 */

        if ((begin + 1) >= end) {
            return length;
        }

        begin[1] = 'e';

        size =  lexbor_write_exponent(kk - 1, &begin[2], end);

        return (size + 2);
    }

    /* 1234e30 -> 1.234e33 */

    if ((&begin[2] + (length - 1)) >= end) {
        return length;
    }

    memmove(&begin[2], &begin[1], length - 1);
    begin[1] = '.';
    begin[length + 1] = 'e';

    size = lexbor_write_exponent(kk - 1, &begin[length + 2], end);

    return (size + length + 2);
}

size_t
lexbor_dtoa(double value, lxb_char_t *begin, size_t len)
{
    int dec_exp, minus;
    size_t length;
    lxb_char_t *end = begin + len;

    if (len == 0) {
        return 0;
    }

    /* Not handling NaN and inf. */

    minus = 0;

    if (value == 0) {
        *begin = '0';

        return 1;
    }

    if (signbit(value)) {
        *begin = '-';

        begin += 1;
        if (begin == end) {
            return 1;
        }

        value = -value;
        minus = 1;
    }

    length = lexbor_grisu2(value, begin, end, &dec_exp);
    length = lexbor_prettify(begin, end, length, dec_exp);

    return (minus + length);
}
