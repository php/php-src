/****************************************************************
 *
 * The author of this software is David M. Gay.
 *
 * Copyright (c) 1991 by AT&T.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR AT&T MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 ***************************************************************/

/* Please send bug reports to
   David M. Gay
   AT&T Bell Laboratories, Room 2C-463
   600 Mountain Avenue
   Murray Hill, NJ 07974-2070
   U.S.A.
   dmg@research.att.com or research!dmg
   */

/* strtod for IEEE-, VAX-, and IBM-arithmetic machines.
 *
 * This strtod returns a nearest machine number to the input decimal
 * string (or sets errno to ERANGE).  With IEEE arithmetic, ties are
 * broken by the IEEE round-even rule.  Otherwise ties are broken by
 * biased rounding (add half and chop).
 *
 * Inspired loosely by William D. Clinger's paper "How to Read Floating
 * Point Numbers Accurately" [Proc. ACM SIGPLAN '90, pp. 92-101].
 *
 * Modifications:
 *
 *	1. We only require IEEE, IBM, or VAX double-precision
 *		arithmetic (not IEEE double-extended).
 *	2. We get by with floating-point arithmetic in a case that
 *		Clinger missed -- when we're computing d * 10^n
 *		for a small integer d and the integer n is not too
 *		much larger than 22 (the maximum integer k for which
 *		we can represent 10^k exactly), we may be able to
 *		compute (d*10^k) * 10^(e-k) with just one roundoff.
 *	3. Rather than a bit-at-a-time adjustment of the binary
 *		result in the hard case, we use floating-point
 *		arithmetic to determine the adjustment to within
 *		one bit; only in really hard cases do we need to
 *		compute a second residual.
 *	4. Because of 3., we don't need a large table of powers of 10
 *		for ten-to-e (just some small tables, e.g. of 10^k
 *		for 0 <= k <= 22).
 */

/*
 * #define IEEE_LITTLE_ENDIAN for IEEE-arithmetic machines where the least
 *	significant byte has the lowest address.
 * #define IEEE_BIG_ENDIAN for IEEE-arithmetic machines where the most
 *	significant byte has the lowest address.
 * #define Long int on machines with 32-bit ints and 64-bit longs.
 * #define Sudden_Underflow for IEEE-format machines without gradual
 *	underflow (i.e., that flush to zero on underflow).
 * #define IBM for IBM mainframe-style floating-point arithmetic.
 * #define VAX for VAX-style floating-point arithmetic.
 * #define Unsigned_Shifts if >> does treats its left operand as unsigned.
 * #define No_leftright to omit left-right logic in fast floating-point
 *	computation of dtoa.
 * #define Check_FLT_ROUNDS if FLT_ROUNDS can assume the values 2 or 3.
 * #define RND_PRODQUOT to use rnd_prod and rnd_quot (assembly routines
 *	that use extended-precision instructions to compute rounded
 *	products and quotients) with IBM.
 * #define ROUND_BIASED for IEEE-format with biased rounding.
 * #define Inaccurate_Divide for IEEE-format with correctly rounded
 *	products but inaccurate quotients, e.g., for Intel i860.
 * #define Just_16 to store 16 bits per 32-bit Long when doing high-precision
 *	integer arithmetic.  Whether this speeds things up or slows things
 *	down depends on the machine and the number being converted.
 * #define KR_headers for old-style C function headers.
 * #define Bad_float_h if your system lacks a float.h or if it does not
 *	define some or all of DBL_DIG, DBL_MAX_10_EXP, DBL_MAX_EXP,
 *	FLT_RADIX, FLT_ROUNDS, and DBL_MAX.
 * #define MALLOC your_malloc, where your_malloc(n) acts like malloc(n)
 *	if memory is available and otherwise does something you deem
 *	appropriate.  If MALLOC is undefined, malloc will be invoked
 *	directly -- and assumed always to succeed.
 */

/* $Id$ */

#include <zend_operators.h>
#include <zend_strtod.h>

#ifdef ZTS
#include <TSRM.h>
#endif

#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#ifndef HAVE_INT32_T
# if SIZEOF_INT == 4
typedef int int32_t;
# elif SIZEOF_LONG == 4
typedef long int int32_t;
# endif
#endif

#ifndef HAVE_UINT32_T
# if SIZEOF_INT == 4
typedef unsigned int uint32_t;
# elif SIZEOF_LONG == 4
typedef unsigned long int uint32_t;
# endif
#endif

#if (defined(__APPLE__) || defined(__APPLE_CC__)) && (defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__))
# if defined(__LITTLE_ENDIAN__)
#  undef WORDS_BIGENDIAN
# else
#  if defined(__BIG_ENDIAN__)
#   define WORDS_BIGENDIAN
#  endif
# endif
#endif

#ifdef WORDS_BIGENDIAN
#define IEEE_BIG_ENDIAN
#else
#define IEEE_LITTLE_ENDIAN
#endif

#if defined(__arm__) && !defined(__VFP_FP__)
/*
 *  * Although the CPU is little endian the FP has different
 *   * byte and word endianness. The byte order is still little endian
 *    * but the word order is big endian.
 *     */
#define IEEE_BIG_ENDIAN
#undef IEEE_LITTLE_ENDIAN
#endif

#ifdef __vax__
#define VAX
#undef IEEE_LITTLE_ENDIAN
#endif

#if defined(_MSC_VER)
#define int32_t __int32
#define uint32_t unsigned __int32
#define IEEE_LITTLE_ENDIAN
#endif

#define Long    int32_t
#define ULong   uint32_t

#ifdef __cplusplus
#include "malloc.h"
#include "memory.h"
#else
#ifndef KR_headers
#include "stdlib.h"
#include "string.h"
#include "locale.h"
#else
#include "malloc.h"
#include "memory.h"
#endif
#endif

#ifdef MALLOC
#ifdef KR_headers
extern char *MALLOC();
#else
extern void *MALLOC(size_t);
#endif
#else
#define MALLOC malloc
#endif

#include "ctype.h"
#include "errno.h"

#ifdef Bad_float_h
#ifdef IEEE_BIG_ENDIAN
#define IEEE_ARITHMETIC
#endif
#ifdef IEEE_LITTLE_ENDIAN
#define IEEE_ARITHMETIC
#endif

#ifdef IEEE_ARITHMETIC
#define DBL_DIG 15
#define DBL_MAX_10_EXP 308
#define DBL_MAX_EXP 1024
#define FLT_RADIX 2
#define FLT_ROUNDS 1
#define DBL_MAX 1.7976931348623157e+308
#endif

#ifdef IBM
#define DBL_DIG 16
#define DBL_MAX_10_EXP 75
#define DBL_MAX_EXP 63
#define FLT_RADIX 16
#define FLT_ROUNDS 0
#define DBL_MAX 7.2370055773322621e+75
#endif

#ifdef VAX
#define DBL_DIG 16
#define DBL_MAX_10_EXP 38
#define DBL_MAX_EXP 127
#define FLT_RADIX 2
#define FLT_ROUNDS 1
#define DBL_MAX 1.7014118346046923e+38
#endif


#ifndef LONG_MAX
#define LONG_MAX 2147483647
#endif
#else
#include "float.h"
#endif
#ifndef __MATH_H__
#include "math.h"
#endif

BEGIN_EXTERN_C()

#ifndef CONST
#ifdef KR_headers
#define CONST /* blank */
#else
#define CONST const
#endif
#endif

#ifdef Unsigned_Shifts
#define Sign_Extend(a,b) if (b < 0) a |= 0xffff0000;
#else
#define Sign_Extend(a,b) /*no-op*/
#endif

#if defined(IEEE_LITTLE_ENDIAN) + defined(IEEE_BIG_ENDIAN) + defined(VAX) + \
		    defined(IBM) != 1
#error "Exactly one of IEEE_LITTLE_ENDIAN IEEE_BIG_ENDIAN, VAX, or IBM should be defined."
#endif

	typedef union {
		    double d;
			    ULong ul[2];
	} _double;
#define value(x) ((x).d)
#ifdef IEEE_LITTLE_ENDIAN
#define word0(x) ((x).ul[1])
#define word1(x) ((x).ul[0])
#else
#define word0(x) ((x).ul[0])
#define word1(x) ((x).ul[1])
#endif

/* The following definition of Storeinc is appropriate for MIPS processors.
 * An alternative that might be better on some machines is
 * #define Storeinc(a,b,c) (*a++ = b << 16 | c & 0xffff)
 */
#if defined(IEEE_LITTLE_ENDIAN) + defined(VAX) + defined(__arm__)
#define Storeinc(a,b,c) (((unsigned short *)a)[1] = (unsigned short)b, \
		((unsigned short *)a)[0] = (unsigned short)c, a++)
#else
#define Storeinc(a,b,c) (((unsigned short *)a)[0] = (unsigned short)b, \
		((unsigned short *)a)[1] = (unsigned short)c, a++)
#endif

/* #define P DBL_MANT_DIG */
/* Ten_pmax = floor(P*log(2)/log(5)) */
/* Bletch = (highest power of 2 < DBL_MAX_10_EXP) / 16 */
/* Quick_max = floor((P-1)*log(FLT_RADIX)/log(10) - 1) */
/* Int_max = floor(P*log(FLT_RADIX)/log(10) - 1) */

#if defined(IEEE_LITTLE_ENDIAN) + defined(IEEE_BIG_ENDIAN)
#define Exp_shift  20
#define Exp_shift1 20
#define Exp_msk1    0x100000
#define Exp_msk11   0x100000
#define Exp_mask  0x7ff00000
#define P 53
#define Bias 1023
#define IEEE_Arith
#define Emin (-1022)
#define Exp_1  0x3ff00000
#define Exp_11 0x3ff00000
#define Ebits 11
#define Frac_mask  0xfffff
#define Frac_mask1 0xfffff
#define Ten_pmax 22
#define Bletch 0x10
#define Bndry_mask  0xfffff
#define Bndry_mask1 0xfffff
#define LSB 1
#define Sign_bit 0x80000000
#define Log2P 1
#define Tiny0 0
#define Tiny1 1
#define Quick_max 14
#define Int_max 14
#define Infinite(x) (word0(x) == 0x7ff00000) /* sufficient test for here */
#else
#undef  Sudden_Underflow
#define Sudden_Underflow
#ifdef IBM
#define Exp_shift  24
#define Exp_shift1 24
#define Exp_msk1   0x1000000
#define Exp_msk11  0x1000000
#define Exp_mask  0x7f000000
#define P 14
#define Bias 65
#define Exp_1  0x41000000
#define Exp_11 0x41000000
#define Ebits 8 /* exponent has 7 bits, but 8 is the right value in b2d */
#define Frac_mask  0xffffff
#define Frac_mask1 0xffffff
#define Bletch 4
#define Ten_pmax 22
#define Bndry_mask  0xefffff
#define Bndry_mask1 0xffffff
#define LSB 1
#define Sign_bit 0x80000000
#define Log2P 4
#define Tiny0 0x100000
#define Tiny1 0
#define Quick_max 14
#define Int_max 15
#else /* VAX */
#define Exp_shift  23
#define Exp_shift1 7
#define Exp_msk1    0x80
#define Exp_msk11   0x800000
#define Exp_mask  0x7f80
#define P 56
#define Bias 129
#define Exp_1  0x40800000
#define Exp_11 0x4080
#define Ebits 8
#define Frac_mask  0x7fffff
#define Frac_mask1 0xffff007f
#define Ten_pmax 24
#define Bletch 2
#define Bndry_mask  0xffff007f
#define Bndry_mask1 0xffff007f
#define LSB 0x10000
#define Sign_bit 0x8000
#define Log2P 1
#define Tiny0 0x80
#define Tiny1 0
#define Quick_max 15
#define Int_max 15
#endif
#endif

#ifndef IEEE_Arith
#define ROUND_BIASED
#endif

#ifdef RND_PRODQUOT
#define rounded_product(a,b) a = rnd_prod(a, b)
#define rounded_quotient(a,b) a = rnd_quot(a, b)
#ifdef KR_headers
extern double rnd_prod(), rnd_quot();
#else
extern double rnd_prod(double, double), rnd_quot(double, double);
#endif
#else
#define rounded_product(a,b) a *= b
#define rounded_quotient(a,b) a /= b
#endif

#define Big0 (Frac_mask1 | Exp_msk1*(DBL_MAX_EXP+Bias-1))
#define Big1 0xffffffff

#ifndef Just_16
/* When Pack_32 is not defined, we store 16 bits per 32-bit Long.
 *  * This makes some inner loops simpler and sometimes saves work
 *   * during multiplications, but it often seems to make things slightly
 *    * slower.  Hence the default is now to store 32 bits per Long.
 *     */
#ifndef Pack_32
#define Pack_32
#endif
#endif

#define Kmax 15

struct Bigint {
	struct Bigint *next;
	int k, maxwds, sign, wds;
	ULong x[1];
};

typedef struct Bigint Bigint;

/* static variables, multithreading fun! */
static Bigint *freelist[Kmax+1];
static Bigint *p5s;

static void destroy_freelist(void);

#ifdef ZTS

static MUTEX_T dtoa_mutex;
static MUTEX_T pow5mult_mutex;

#define _THREAD_PRIVATE_MUTEX_LOCK(x) tsrm_mutex_lock(x);
#define _THREAD_PRIVATE_MUTEX_UNLOCK(x) tsrm_mutex_unlock(x);

#else

#define _THREAD_PRIVATE_MUTEX_LOCK(x)
#define _THREAD_PRIVATE_MUTEX_UNLOCK(x)

#endif /* ZTS */

#ifdef DEBUG
static void Bug(const char *message) {
	fprintf(stderr, "%s\n", message);
}
#endif

ZEND_API int zend_startup_strtod(void) /* {{{ */
{
#ifdef ZTS
	dtoa_mutex = tsrm_mutex_alloc();
	pow5mult_mutex = tsrm_mutex_alloc();
#endif
	return 1;
}
/* }}} */
ZEND_API int zend_shutdown_strtod(void) /* {{{ */
{
	destroy_freelist();
#ifdef ZTS
	tsrm_mutex_free(dtoa_mutex);
	dtoa_mutex = NULL;

	tsrm_mutex_free(pow5mult_mutex);
	pow5mult_mutex = NULL;
#endif
	return 1;
}
/* }}} */

static Bigint * Balloc(int k)
{
	int x;
	Bigint *rv;

	if (k > Kmax) {
		zend_error(E_ERROR, "Balloc() allocation exceeds list boundary");
	}

	_THREAD_PRIVATE_MUTEX_LOCK(dtoa_mutex);
	if ((rv = freelist[k])) {
		freelist[k] = rv->next;
	} else {
		x = 1 << k;
		rv = (Bigint *)MALLOC(sizeof(Bigint) + (x-1)*sizeof(Long));
		if (!rv) {
			_THREAD_PRIVATE_MUTEX_UNLOCK(dtoa_mutex);
			zend_error(E_ERROR, "Balloc() failed to allocate memory");
		}
		rv->k = k;
		rv->maxwds = x;
	}
	_THREAD_PRIVATE_MUTEX_UNLOCK(dtoa_mutex);
	rv->sign = rv->wds = 0;
	return rv;
}

static void Bfree(Bigint *v)
{
	if (v) {
		_THREAD_PRIVATE_MUTEX_LOCK(dtoa_mutex);
		v->next = freelist[v->k];
		freelist[v->k] = v;
		_THREAD_PRIVATE_MUTEX_UNLOCK(dtoa_mutex);
	}
}

#define Bcopy(x,y) memcpy((char *)&x->sign, (char *)&y->sign, \
		y->wds*sizeof(Long) + 2*sizeof(int))

/* return value is only used as a simple string, so mis-aligned parts
 * inside the Bigint are not at risk on strict align architectures
 */
static char * rv_alloc(int i) {
	int j, k, *r;

	j = sizeof(ULong);
	for(k = 0;
			sizeof(Bigint) - sizeof(ULong) - sizeof(int) + j <= i;
			j <<= 1) {
		k++;
	}
	r = (int*)Balloc(k);
	*r = k;
	return (char *)(r+1);
}


static char * nrv_alloc(char *s, char **rve, int n)
{
	char *rv, *t;

	t = rv = rv_alloc(n);
	while((*t = *s++) !=0) {
		t++;
	}
	if (rve) {
		*rve = t;
	}
	return rv;
}

static Bigint * multadd(Bigint *b, int m, int a) /* multiply by m and add a */
{
	int i, wds;
	ULong *x, y;
#ifdef Pack_32
	ULong xi, z;
#endif
	Bigint *b1;

	wds = b->wds;
	x = b->x;
	i = 0;
	do {
#ifdef Pack_32
		xi = *x;
		y = (xi & 0xffff) * m + a;
		z = (xi >> 16) * m + (y >> 16);
		a = (int)(z >> 16);
		*x++ = (z << 16) + (y & 0xffff);
#else
		y = *x * m + a;
		a = (int)(y >> 16);
		*x++ = y & 0xffff;
#endif
	}
	while(++i < wds);
	if (a) {
		if (wds >= b->maxwds) {
			b1 = Balloc(b->k+1);
			Bcopy(b1, b);
			Bfree(b);
			b = b1;
		}
		b->x[wds++] = a;
		b->wds = wds;
	}
	return b;
}

static int hi0bits(ULong x)
{
	int k = 0;

	if (!(x & 0xffff0000)) {
		k = 16;
		x <<= 16;
	}
	if (!(x & 0xff000000)) {
		k += 8;
		x <<= 8;
	}
	if (!(x & 0xf0000000)) {
		k += 4;
		x <<= 4;
	}
	if (!(x & 0xc0000000)) {
		k += 2;
		x <<= 2;
	}
	if (!(x & 0x80000000)) {
		k++;
		if (!(x & 0x40000000)) {
			return 32;
		}
	}
	return k;
}

static int lo0bits(ULong *y)
{
	int k;
	ULong x = *y;

	if (x & 7) {
		if (x & 1) {
			return 0;
		}
		if (x & 2) {
			*y = x >> 1;
			return 1;
		}
		*y = x >> 2;
		return 2;
	}
	k = 0;
	if (!(x & 0xffff)) {
		k = 16;
		x >>= 16;
	}
	if (!(x & 0xff)) {
		k += 8;
		x >>= 8;
	}
	if (!(x & 0xf)) {
		k += 4;
		x >>= 4;
	}
	if (!(x & 0x3)) {
		k += 2;
		x >>= 2;
	}
	if (!(x & 1)) {
		k++;
		x >>= 1;
		if (!(x & 1)) {
			return 32;
		}
	}
	*y = x;
	return k;
}

static Bigint * i2b(int i)
{
	Bigint *b;

	b = Balloc(1);
	b->x[0] = i;
	b->wds = 1;
	return b;
}

static Bigint * mult(Bigint *a, Bigint *b)
{
	Bigint *c;
	int k, wa, wb, wc;
	ULong carry, y, z;
	ULong *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
#ifdef Pack_32
	ULong z2;
#endif

	if (a->wds < b->wds) {
		c = a;
		a = b;
		b = c;
	}
	k = a->k;
	wa = a->wds;
	wb = b->wds;
	wc = wa + wb;
	if (wc > a->maxwds) {
		k++;
	}
	c = Balloc(k);
	for(x = c->x, xa = x + wc; x < xa; x++) {
		*x = 0;
	}
	xa = a->x;
	xae = xa + wa;
	xb = b->x;
	xbe = xb + wb;
	xc0 = c->x;
#ifdef Pack_32
	for(; xb < xbe; xb++, xc0++) {
		if ((y = *xb & 0xffff)) {
			x = xa;
			xc = xc0;
			carry = 0;
			do {
				z = (*x & 0xffff) * y + (*xc & 0xffff) + carry;
				carry = z >> 16;
				z2 = (*x++ >> 16) * y + (*xc >> 16) + carry;
				carry = z2 >> 16;
				Storeinc(xc, z2, z);
			}
			while(x < xae);
			*xc = carry;
		}
		if ((y = *xb >> 16)) {
			x = xa;
			xc = xc0;
			carry = 0;
			z2 = *xc;
			do {
				z = (*x & 0xffff) * y + (*xc >> 16) + carry;
				carry = z >> 16;
				Storeinc(xc, z, z2);
				z2 = (*x++ >> 16) * y + (*xc & 0xffff) + carry;
				carry = z2 >> 16;
			}
			while(x < xae);
			*xc = z2;
		}
	}
#else
	for(; xb < xbe; xc0++) {
		if (y = *xb++) {
			x = xa;
			xc = xc0;
			carry = 0;
			do {
				z = *x++ * y + *xc + carry;
				carry = z >> 16;
				*xc++ = z & 0xffff;
			}
			while(x < xae);
			*xc = carry;
		}
	}
#endif
	for(xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc) ;
	c->wds = wc;
	return c;
}

static Bigint * s2b (CONST char *s, int nd0, int nd, ULong y9)
{
	Bigint *b;
	int i, k;
	Long x, y;

	x = (nd + 8) / 9;
	for(k = 0, y = 1; x > y; y <<= 1, k++) ;
#ifdef Pack_32
	b = Balloc(k);
	b->x[0] = y9;
	b->wds = 1;
#else
	b = Balloc(k+1);
	b->x[0] = y9 & 0xffff;
	b->wds = (b->x[1] = y9 >> 16) ? 2 : 1;
#endif

	i = 9;
	if (9 < nd0) {
		s += 9;
		do b = multadd(b, 10, *s++ - '0');
		while(++i < nd0);
		s++;
	} else {
		s += 10;
	}
	for(; i < nd; i++) {
		b = multadd(b, 10, *s++ - '0');
	}
	return b;
}

static Bigint * pow5mult(Bigint *b, int k)
{
	Bigint *b1, *p5, *p51;
	int i;
	static int p05[3] = { 5, 25, 125 };

	_THREAD_PRIVATE_MUTEX_LOCK(pow5mult_mutex);
	if ((i = k & 3)) {
		b = multadd(b, p05[i-1], 0);
	}

	if (!(k >>= 2)) {
		_THREAD_PRIVATE_MUTEX_UNLOCK(pow5mult_mutex);
		return b;
	}
	if (!(p5 = p5s)) {
		/* first time */
		p5 = p5s = i2b(625);
		p5->next = 0;
	}
	for(;;) {
		if (k & 1) {
			b1 = mult(b, p5);
			Bfree(b);
			b = b1;
		}
		if (!(k >>= 1)) {
			break;
		}
		if (!(p51 = p5->next)) {
			if (!(p51 = p5->next)) {
				p51 = p5->next = mult(p5,p5);
				p51->next = 0;
			}
		}
		p5 = p51;
	}
	_THREAD_PRIVATE_MUTEX_UNLOCK(pow5mult_mutex);
	return b;
}


static Bigint *lshift(Bigint *b, int k)
{
	int i, k1, n, n1;
	Bigint *b1;
	ULong *x, *x1, *xe, z;

#ifdef Pack_32
	n = k >> 5;
#else
	n = k >> 4;
#endif
	k1 = b->k;
	n1 = n + b->wds + 1;
	for(i = b->maxwds; n1 > i; i <<= 1) {
		k1++;
	}
	b1 = Balloc(k1);
	x1 = b1->x;
	for(i = 0; i < n; i++) {
		*x1++ = 0;
	}
	x = b->x;
	xe = x + b->wds;
#ifdef Pack_32
	if (k &= 0x1f) {
		k1 = 32 - k;
		z = 0;
		do {
			*x1++ = *x << k | z;
			z = *x++ >> k1;
		}
		while(x < xe);
		if ((*x1 = z)) {
			++n1;
		}
	}
#else
	if (k &= 0xf) {
		k1 = 16 - k;
		z = 0;
		do {
			*x1++ = *x << k  & 0xffff | z;
			z = *x++ >> k1;
		}
		while(x < xe);
		if (*x1 = z) {
			++n1;
		}
	}
#endif
	else do
		*x1++ = *x++;
	while(x < xe);
	b1->wds = n1 - 1;
	Bfree(b);
	return b1;
}

static int cmp(Bigint *a, Bigint *b)
{
	ULong *xa, *xa0, *xb, *xb0;
	int i, j;

	i = a->wds;
	j = b->wds;
#ifdef DEBUG
	if (i > 1 && !a->x[i-1])
		Bug("cmp called with a->x[a->wds-1] == 0");
	if (j > 1 && !b->x[j-1])
		Bug("cmp called with b->x[b->wds-1] == 0");
#endif
	if (i -= j)
		return i;
	xa0 = a->x;
	xa = xa0 + j;
	xb0 = b->x;
	xb = xb0 + j;
	for(;;) {
		if (*--xa != *--xb)
			return *xa < *xb ? -1 : 1;
		if (xa <= xa0)
			break;
	}
	return 0;
}


static Bigint * diff(Bigint *a, Bigint *b)
{
	Bigint *c;
	int i, wa, wb;
	Long borrow, y; /* We need signed shifts here. */
	ULong *xa, *xae, *xb, *xbe, *xc;
#ifdef Pack_32
	Long z;
#endif

	i = cmp(a,b);
	if (!i) {
		c = Balloc(0);
		c->wds = 1;
		c->x[0] = 0;
		return c;
	}
	if (i < 0) {
		c = a;
		a = b;
		b = c;
		i = 1;
	} else {
		i = 0;
	}
	c = Balloc(a->k);
	c->sign = i;
	wa = a->wds;
	xa = a->x;
	xae = xa + wa;
	wb = b->wds;
	xb = b->x;
	xbe = xb + wb;
	xc = c->x;
	borrow = 0;
#ifdef Pack_32
	do {
		y = (*xa & 0xffff) - (*xb & 0xffff) + borrow;
		borrow = y >> 16;
		Sign_Extend(borrow, y);
		z = (*xa++ >> 16) - (*xb++ >> 16) + borrow;
		borrow = z >> 16;
		Sign_Extend(borrow, z);
		Storeinc(xc, z, y);
	} while(xb < xbe);
	while(xa < xae) {
		y = (*xa & 0xffff) + borrow;
		borrow = y >> 16;
		Sign_Extend(borrow, y);
		z = (*xa++ >> 16) + borrow;
		borrow = z >> 16;
		Sign_Extend(borrow, z);
		Storeinc(xc, z, y);
	}
#else
	do {
		y = *xa++ - *xb++ + borrow;
		borrow = y >> 16;
		Sign_Extend(borrow, y);
		*xc++ = y & 0xffff;
	} while(xb < xbe);
	while(xa < xae) {
		y = *xa++ + borrow;
		borrow = y >> 16;
		Sign_Extend(borrow, y);
		*xc++ = y & 0xffff;
	}
#endif
	while(!*--xc) {
		wa--;
	}
	c->wds = wa;
	return c;
}

static double ulp (double _x)
{
	volatile _double x;
	register Long L;
	volatile _double a;

	value(x) = _x;
	L = (word0(x) & Exp_mask) - (P-1)*Exp_msk1;
#ifndef Sudden_Underflow
	if (L > 0) {
#endif
#ifdef IBM
		L |= Exp_msk1 >> 4;
#endif
		word0(a) = L;
		word1(a) = 0;
#ifndef Sudden_Underflow
	}
	else {
		L = -L >> Exp_shift;
		if (L < Exp_shift) {
			word0(a) = 0x80000 >> L;
			word1(a) = 0;
		}
		else {
			word0(a) = 0;
			L -= Exp_shift;
			word1(a) = L >= 31 ? 1 : 1 << (31 - L);
		}
	}
#endif
	return value(a);
}

static double
b2d
#ifdef KR_headers
(a, e) Bigint *a; int *e;
#else
(Bigint *a, int *e)
#endif
{
	ULong *xa, *xa0, w, y, z;
	int k;
	volatile _double d;
#ifdef VAX
	ULong d0, d1;
#else
#define d0 word0(d)
#define d1 word1(d)
#endif

	xa0 = a->x;
	xa = xa0 + a->wds;
	y = *--xa;
#ifdef DEBUG
	if (!y) Bug("zero y in b2d");
#endif
	k = hi0bits(y);
	*e = 32 - k;
#ifdef Pack_32
	if (k < Ebits) {
		d0 = Exp_1 | y >> (Ebits - k);
		w = xa > xa0 ? *--xa : 0;
		d1 = y << ((32-Ebits) + k) | w >> (Ebits - k);
		goto ret_d;
	}
	z = xa > xa0 ? *--xa : 0;
	if (k -= Ebits) {
		d0 = Exp_1 | y << k | z >> (32 - k);
		y = xa > xa0 ? *--xa : 0;
		d1 = z << k | y >> (32 - k);
	}
	else {
		d0 = Exp_1 | y;
		d1 = z;
	}
#else
	if (k < Ebits + 16) {
		z = xa > xa0 ? *--xa : 0;
		d0 = Exp_1 | y << k - Ebits | z >> Ebits + 16 - k;
		w = xa > xa0 ? *--xa : 0;
		y = xa > xa0 ? *--xa : 0;
		d1 = z << k + 16 - Ebits | w << k - Ebits | y >> 16 + Ebits - k;
		goto ret_d;
	}
	z = xa > xa0 ? *--xa : 0;
	w = xa > xa0 ? *--xa : 0;
	k -= Ebits + 16;
	d0 = Exp_1 | y << k + 16 | z << k | w >> 16 - k;
	y = xa > xa0 ? *--xa : 0;
	d1 = w << k + 16 | y << k;
#endif
ret_d:
#ifdef VAX
	word0(d) = d0 >> 16 | d0 << 16;
	word1(d) = d1 >> 16 | d1 << 16;
#else
#undef d0
#undef d1
#endif
	return value(d);
}


static Bigint * d2b(double _d, int *e, int *bits)
{
	Bigint *b;
	int de, i, k;
	ULong *x, y, z;
	volatile _double d;
#ifdef VAX
	ULong d0, d1;
#endif

	value(d) = _d;
#ifdef VAX
	d0 = word0(d) >> 16 | word0(d) << 16;
	d1 = word1(d) >> 16 | word1(d) << 16;
#else
#define d0 word0(d)
#define d1 word1(d)
#endif

#ifdef Pack_32
	b = Balloc(1);
#else
	b = Balloc(2);
#endif
	x = b->x;

	z = d0 & Frac_mask;
	d0 &= 0x7fffffff;   /* clear sign bit, which we ignore */
#ifdef Sudden_Underflow
	de = (int)(d0 >> Exp_shift);
#ifndef IBM
	z |= Exp_msk11;
#endif
#else
	if ((de = (int)(d0 >> Exp_shift)))
		z |= Exp_msk1;
#endif
#ifdef Pack_32
	if ((y = d1)) {
		if ((k = lo0bits(&y))) {
			x[0] = y | (z << (32 - k));
			z >>= k;
		} else {
			x[0] = y;
		}
		i = b->wds = (x[1] = z) ? 2 : 1;
	} else {
#ifdef DEBUG
		if (!z)
			Bug("Zero passed to d2b");
#endif
		k = lo0bits(&z);
		x[0] = z;
		i = b->wds = 1;
		k += 32;
	}
#else
	if (y = d1) {
		if (k = lo0bits(&y)) {
			if (k >= 16) {
				x[0] = y | z << 32 - k & 0xffff;
				x[1] = z >> k - 16 & 0xffff;
				x[2] = z >> k;
				i = 2;
			} else {
				x[0] = y & 0xffff;
				x[1] = y >> 16 | z << 16 - k & 0xffff;
				x[2] = z >> k & 0xffff;
				x[3] = z >> k+16;
				i = 3;
			}
		} else {
			x[0] = y & 0xffff;
			x[1] = y >> 16;
			x[2] = z & 0xffff;
			x[3] = z >> 16;
			i = 3;
		}
	} else {
#ifdef DEBUG
		if (!z)
			Bug("Zero passed to d2b");
#endif
		k = lo0bits(&z);
		if (k >= 16) {
			x[0] = z;
			i = 0;
		} else {
			x[0] = z & 0xffff;
			x[1] = z >> 16;
			i = 1;
		}
		k += 32;
	}
	while(!x[i])
		--i;
	b->wds = i + 1;
#endif
#ifndef Sudden_Underflow
	if (de) {
#endif
#ifdef IBM
		*e = (de - Bias - (P-1) << 2) + k;
		*bits = 4*P + 8 - k - hi0bits(word0(d) & Frac_mask);
#else
		*e = de - Bias - (P-1) + k;
		*bits = P - k;
#endif
#ifndef Sudden_Underflow
	} else {
		*e = de - Bias - (P-1) + 1 + k;
#ifdef Pack_32
		*bits = 32*i - hi0bits(x[i-1]);
#else
		*bits = (i+2)*16 - hi0bits(x[i]);
#endif
	}
#endif
	return b;
}
#undef d0
#undef d1


static double ratio (Bigint *a, Bigint *b)
{
	volatile _double da, db;
	int k, ka, kb;

	value(da) = b2d(a, &ka);
	value(db) = b2d(b, &kb);
#ifdef Pack_32
	k = ka - kb + 32*(a->wds - b->wds);
#else
	k = ka - kb + 16*(a->wds - b->wds);
#endif
#ifdef IBM
	if (k > 0) {
		word0(da) += (k >> 2)*Exp_msk1;
		if (k &= 3) {
			da *= 1 << k;
		}
	} else {
		k = -k;
		word0(db) += (k >> 2)*Exp_msk1;
		if (k &= 3)
			db *= 1 << k;
	}
#else
	if (k > 0) {
		word0(da) += k*Exp_msk1;
	} else {
		k = -k;
		word0(db) += k*Exp_msk1;
	}
#endif
	return value(da) / value(db);
}

static CONST double
tens[] = {
	1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
	1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
	1e20, 1e21, 1e22
#ifdef VAX
		, 1e23, 1e24
#endif
};

#ifdef IEEE_Arith
static CONST double bigtens[] = { 1e16, 1e32, 1e64, 1e128, 1e256 };
static CONST double tinytens[] = { 1e-16, 1e-32, 1e-64, 1e-128, 1e-256 };
#define n_bigtens 5
#else
#ifdef IBM
static CONST double bigtens[] = { 1e16, 1e32, 1e64 };
static CONST double tinytens[] = { 1e-16, 1e-32, 1e-64 };
#define n_bigtens 3
#else
static CONST double bigtens[] = { 1e16, 1e32 };
static CONST double tinytens[] = { 1e-16, 1e-32 };
#define n_bigtens 2
#endif
#endif


static int quorem(Bigint *b, Bigint *S)
{
	int n;
	Long borrow, y;
	ULong carry, q, ys;
	ULong *bx, *bxe, *sx, *sxe;
#ifdef Pack_32
	Long z;
	ULong si, zs;
#endif

	n = S->wds;
#ifdef DEBUG
	/*debug*/ if (b->wds > n)
		/*debug*/   Bug("oversize b in quorem");
#endif
	if (b->wds < n)
		return 0;
	sx = S->x;
	sxe = sx + --n;
	bx = b->x;
	bxe = bx + n;
	q = *bxe / (*sxe + 1);  /* ensure q <= true quotient */
#ifdef DEBUG
	/*debug*/ if (q > 9)
		/*debug*/   Bug("oversized quotient in quorem");
#endif
	if (q) {
		borrow = 0;
		carry = 0;
		do {
#ifdef Pack_32
			si = *sx++;
			ys = (si & 0xffff) * q + carry;
			zs = (si >> 16) * q + (ys >> 16);
			carry = zs >> 16;
			y = (*bx & 0xffff) - (ys & 0xffff) + borrow;
			borrow = y >> 16;
			Sign_Extend(borrow, y);
			z = (*bx >> 16) - (zs & 0xffff) + borrow;
			borrow = z >> 16;
			Sign_Extend(borrow, z);
			Storeinc(bx, z, y);
#else
			ys = *sx++ * q + carry;
			carry = ys >> 16;
			y = *bx - (ys & 0xffff) + borrow;
			borrow = y >> 16;
			Sign_Extend(borrow, y);
			*bx++ = y & 0xffff;
#endif
		}
		while(sx <= sxe);
		if (!*bxe) {
			bx = b->x;
			while(--bxe > bx && !*bxe)
				--n;
			b->wds = n;
		}
	}
	if (cmp(b, S) >= 0) {
		q++;
		borrow = 0;
		carry = 0;
		bx = b->x;
		sx = S->x;
		do {
#ifdef Pack_32
			si = *sx++;
			ys = (si & 0xffff) + carry;
			zs = (si >> 16) + (ys >> 16);
			carry = zs >> 16;
			y = (*bx & 0xffff) - (ys & 0xffff) + borrow;
			borrow = y >> 16;
			Sign_Extend(borrow, y);
			z = (*bx >> 16) - (zs & 0xffff) + borrow;
			borrow = z >> 16;
			Sign_Extend(borrow, z);
			Storeinc(bx, z, y);
#else
			ys = *sx++ + carry;
			carry = ys >> 16;
			y = *bx - (ys & 0xffff) + borrow;
			borrow = y >> 16;
			Sign_Extend(borrow, y);
			*bx++ = y & 0xffff;
#endif
		}
		while(sx <= sxe);
		bx = b->x;
		bxe = bx + n;
		if (!*bxe) {
			while(--bxe > bx && !*bxe)
				--n;
			b->wds = n;
		}
	}
	return q;
}

static void destroy_freelist(void)
{
	int i;
	Bigint *tmp;

	_THREAD_PRIVATE_MUTEX_LOCK(dtoa_mutex);
	for (i = 0; i <= Kmax; i++) {
		Bigint **listp = &freelist[i];
		while ((tmp = *listp) != NULL) {
			*listp = tmp->next;
			free(tmp);
		}
		freelist[i] = NULL;
	}
	_THREAD_PRIVATE_MUTEX_UNLOCK(dtoa_mutex);

}


ZEND_API void zend_freedtoa(char *s)
{
	Bigint *b = (Bigint *)((int *)s - 1);
	b->maxwds = 1 << (b->k = *(int*)b);
	Bfree(b);
}

/* dtoa for IEEE arithmetic (dmg): convert double to ASCII string.
 *
 * Inspired by "How to Print Floating-Point Numbers Accurately" by
 * Guy L. Steele, Jr. and Jon L. White [Proc. ACM SIGPLAN '90, pp. 92-101].
 *
 * Modifications:
 *  1. Rather than iterating, we use a simple numeric overestimate
 *     to determine k = floor(log10(d)).  We scale relevant
 *     quantities using O(log2(k)) rather than O(k) multiplications.
 *  2. For some modes > 2 (corresponding to ecvt and fcvt), we don't
 *     try to generate digits strictly left to right.  Instead, we
 *     compute with fewer bits and propagate the carry if necessary
 *     when rounding the final digit up.  This is often faster.
 *  3. Under the assumption that input will be rounded nearest,
 *     mode 0 renders 1e23 as 1e23 rather than 9.999999999999999e22.
 *     That is, we allow equality in stopping tests when the
 *     round-nearest rule will give the same floating-point value
 *     as would satisfaction of the stopping test with strict
 *     inequality.
 *  4. We remove common factors of powers of 2 from relevant
 *     quantities.
 *  5. When converting floating-point integers less than 1e16,
 *     we use floating-point arithmetic rather than resorting
 *     to multiple-precision integers.
 *  6. When asked to produce fewer than 15 digits, we first try
 *     to get by with floating-point arithmetic; we resort to
 *     multiple-precision integer arithmetic only if we cannot
 *     guarantee that the floating-point calculation has given
 *     the correctly rounded result.  For k requested digits and
 *     "uniformly" distributed input, the probability is
 *     something like 10^(k-15) that we must resort to the Long
 *     calculation.
 */

ZEND_API char * zend_dtoa(double _d, int mode, int ndigits, int *decpt, int *sign, char **rve)
{
 /* Arguments ndigits, decpt, sign are similar to those
    of ecvt and fcvt; trailing zeros are suppressed from
    the returned string.  If not null, *rve is set to point
    to the end of the return value.  If d is +-Infinity or NaN,
    then *decpt is set to 9999.

    mode:
        0 ==> shortest string that yields d when read in
            and rounded to nearest.
        1 ==> like 0, but with Steele & White stopping rule;
            e.g. with IEEE P754 arithmetic , mode 0 gives
            1e23 whereas mode 1 gives 9.999999999999999e22.
        2 ==> max(1,ndigits) significant digits.  This gives a
            return value similar to that of ecvt, except
            that trailing zeros are suppressed.
        3 ==> through ndigits past the decimal point.  This
            gives a return value similar to that from fcvt,
            except that trailing zeros are suppressed, and
            ndigits can be negative.
        4-9 should give the same return values as 2-3, i.e.,
            4 <= mode <= 9 ==> same return as mode
            2 + (mode & 1).  These modes are mainly for
            debugging; often they run slower but sometimes
            faster than modes 2-3.
        4,5,8,9 ==> left-to-right digit generation.
        6-9 ==> don't try fast floating-point estimate
            (if applicable).

        Values of mode other than 0-9 are treated as mode 0.

        Sufficient space is allocated to the return value
        to hold the suppressed trailing zeros.
    */

	int bbits, b2, b5, be, dig, i, ieps, ilim = 0, ilim0, ilim1,
		j, j1, k, k0, k_check, leftright, m2, m5, s2, s5,
		spec_case = 0, try_quick;
	Long L;
#ifndef Sudden_Underflow
	int denorm;
	ULong x;
#endif
	Bigint *b, *b1, *delta, *mlo, *mhi, *S, *tmp;
	double ds;
	char *s, *s0;
	volatile _double d, d2, eps;

	value(d) = _d;

	if (word0(d) & Sign_bit) {
		/* set sign for everything, including 0's and NaNs */
		*sign = 1;
		word0(d) &= ~Sign_bit;  /* clear sign bit */
	}
	else
		*sign = 0;

#if defined(IEEE_Arith) + defined(VAX)
#ifdef IEEE_Arith
	if ((word0(d) & Exp_mask) == Exp_mask)
#else
		if (word0(d)  == 0x8000)
#endif
		{
			/* Infinity or NaN */
			*decpt = 9999;
#ifdef IEEE_Arith
			if (!word1(d) && !(word0(d) & 0xfffff))
				return nrv_alloc("Infinity", rve, 8);
#endif
			return nrv_alloc("NaN", rve, 3);
		}
#endif
#ifdef IBM
	value(d) += 0; /* normalize */
#endif
	if (!value(d)) {
		*decpt = 1;
		return nrv_alloc("0", rve, 1);
	}

	b = d2b(value(d), &be, &bbits);
#ifdef Sudden_Underflow
	i = (int)(word0(d) >> Exp_shift1 & (Exp_mask>>Exp_shift1));
#else
	if ((i = (int)(word0(d) >> Exp_shift1 & (Exp_mask>>Exp_shift1)))) {
#endif
		value(d2) = value(d);
		word0(d2) &= Frac_mask1;
		word0(d2) |= Exp_11;
#ifdef IBM
		if (j = 11 - hi0bits(word0(d2) & Frac_mask))
			value(d2) /= 1 << j;
#endif

		/* log(x)   ~=~ log(1.5) + (x-1.5)/1.5
		 * log10(x)  =  log(x) / log(10)
		 *      ~=~ log(1.5)/log(10) + (x-1.5)/(1.5*log(10))
		 * log10(d) = (i-Bias)*log(2)/log(10) + log10(d2)
		 *
		 * This suggests computing an approximation k to log10(d) by
		 *
		 * k = (i - Bias)*0.301029995663981
		 *  + ( (d2-1.5)*0.289529654602168 + 0.176091259055681 );
		 *
		 * We want k to be too large rather than too small.
		 * The error in the first-order Taylor series approximation
		 * is in our favor, so we just round up the constant enough
		 * to compensate for any error in the multiplication of
		 * (i - Bias) by 0.301029995663981; since |i - Bias| <= 1077,
		 * and 1077 * 0.30103 * 2^-52 ~=~ 7.2e-14,
		 * adding 1e-13 to the constant term more than suffices.
		 * Hence we adjust the constant term to 0.1760912590558.
		 * (We could get a more accurate k by invoking log10,
		 *  but this is probably not worthwhile.)
		 */

		i -= Bias;
#ifdef IBM
		i <<= 2;
		i += j;
#endif
#ifndef Sudden_Underflow
		denorm = 0;
	}
	else {
		/* d is denormalized */

		i = bbits + be + (Bias + (P-1) - 1);
		x = i > 32  ? (word0(d) << (64 - i)) | (word1(d) >> (i - 32))
			: (word1(d) << (32 - i));
		value(d2) = x;
		word0(d2) -= 31*Exp_msk1; /* adjust exponent */
		i -= (Bias + (P-1) - 1) + 1;
		denorm = 1;
	}
#endif
	ds = (value(d2)-1.5)*0.289529654602168 + 0.1760912590558 + i*0.301029995663981;
	k = (int)ds;
	if (ds < 0. && ds != k)
		k--;    /* want k = floor(ds) */
	k_check = 1;
	if (k >= 0 && k <= Ten_pmax) {
		if (value(d) < tens[k])
			k--;
		k_check = 0;
	}
	j = bbits - i - 1;
	if (j >= 0) {
		b2 = 0;
		s2 = j;
	}
	else {
		b2 = -j;
		s2 = 0;
	}
	if (k >= 0) {
		b5 = 0;
		s5 = k;
		s2 += k;
	}
	else {
		b2 -= k;
		b5 = -k;
		s5 = 0;
	}
	if (mode < 0 || mode > 9)
		mode = 0;
	try_quick = 1;
	if (mode > 5) {
		mode -= 4;
		try_quick = 0;
	}
	leftright = 1;
	switch(mode) {
		case 0:
		case 1:
			ilim = ilim1 = -1;
			i = 18;
			ndigits = 0;
			break;
		case 2:
			leftright = 0;
			/* no break */
		case 4:
			if (ndigits <= 0)
				ndigits = 1;
			ilim = ilim1 = i = ndigits;
			break;
		case 3:
			leftright = 0;
			/* no break */
		case 5:
			i = ndigits + k + 1;
			ilim = i;
			ilim1 = i - 1;
			if (i <= 0)
				i = 1;
	}
	s = s0 = rv_alloc(i);

	if (ilim >= 0 && ilim <= Quick_max && try_quick) {

		/* Try to get by with floating-point arithmetic. */

		i = 0;
		value(d2) = value(d);
		k0 = k;
		ilim0 = ilim;
		ieps = 2; /* conservative */
		if (k > 0) {
			ds = tens[k&0xf];
			j = k >> 4;
			if (j & Bletch) {
				/* prevent overflows */
				j &= Bletch - 1;
				value(d) /= bigtens[n_bigtens-1];
				ieps++;
			}
			for(; j; j >>= 1, i++)
				if (j & 1) {
					ieps++;
					ds *= bigtens[i];
				}
			value(d) /= ds;
		}
		else if ((j1 = -k)) {
			value(d) *= tens[j1 & 0xf];
			for(j = j1 >> 4; j; j >>= 1, i++)
				if (j & 1) {
					ieps++;
					value(d) *= bigtens[i];
				}
		}
		if (k_check && value(d) < 1. && ilim > 0) {
			if (ilim1 <= 0)
				goto fast_failed;
			ilim = ilim1;
			k--;
			value(d) *= 10.;
			ieps++;
		}
		value(eps) = ieps*value(d) + 7.;
		word0(eps) -= (P-1)*Exp_msk1;
		if (ilim == 0) {
			S = mhi = 0;
			value(d) -= 5.;
			if (value(d) > value(eps))
				goto one_digit;
			if (value(d) < -value(eps))
				goto no_digits;
			goto fast_failed;
		}
#ifndef No_leftright
		if (leftright) {
			/* Use Steele & White method of only
			 * generating digits needed.
			 */
			value(eps) = 0.5/tens[ilim-1] - value(eps);
			for(i = 0;;) {
				L = value(d);
				value(d) -= L;
				*s++ = '0' + (int)L;
				if (value(d) < value(eps))
					goto ret1;
				if (1. - value(d) < value(eps))
					goto bump_up;
				if (++i >= ilim)
					break;
				value(eps) *= 10.;
				value(d) *= 10.;
			}
		}
		else {
#endif
			/* Generate ilim digits, then fix them up. */
			value(eps) *= tens[ilim-1];
			for(i = 1;; i++, value(d) *= 10.) {
				L = value(d);
				value(d) -= L;
				*s++ = '0' + (int)L;
				if (i == ilim) {
					if (value(d) > 0.5 + value(eps))
						goto bump_up;
					else if (value(d) < 0.5 - value(eps)) {
						while(*--s == '0');
						s++;
						goto ret1;
					}
					break;
				}
			}
#ifndef No_leftright
		}
#endif
fast_failed:
		s = s0;
		value(d) = value(d2);
		k = k0;
		ilim = ilim0;
	}

	/* Do we have a "small" integer? */

	if (be >= 0 && k <= Int_max) {
		/* Yes. */
		ds = tens[k];
		if (ndigits < 0 && ilim <= 0) {
			S = mhi = 0;
			if (ilim < 0 || value(d) <= 5*ds)
				goto no_digits;
			goto one_digit;
		}
		for(i = 1;; i++) {
			L = value(d) / ds;
			value(d) -= L*ds;
#ifdef Check_FLT_ROUNDS
			/* If FLT_ROUNDS == 2, L will usually be high by 1 */
			if (value(d) < 0) {
				L--;
				value(d) += ds;
			}
#endif
			*s++ = '0' + (int)L;
			if (i == ilim) {
				value(d) += value(d);
				if (value(d) > ds || (value(d) == ds && (L & 1))) {
bump_up:
					while(*--s == '9')
						if (s == s0) {
							k++;
							*s = '0';
							break;
						}
					++*s++;
				}
				break;
			}
			if (!(value(d) *= 10.))
				break;
		}
		goto ret1;
	}

	m2 = b2;
	m5 = b5;
	mhi = mlo = 0;
	if (leftright) {
		if (mode < 2) {
			i =
#ifndef Sudden_Underflow
				denorm ? be + (Bias + (P-1) - 1 + 1) :
#endif
#ifdef IBM
				1 + 4*P - 3 - bbits + ((bbits + be - 1) & 3);
#else
			1 + P - bbits;
#endif
		}
		else {
			j = ilim - 1;
			if (m5 >= j)
				m5 -= j;
			else {
				s5 += j -= m5;
				b5 += j;
				m5 = 0;
			}
			if ((i = ilim) < 0) {
				m2 -= i;
				i = 0;
			}
		}
		b2 += i;
		s2 += i;
		mhi = i2b(1);
	}
	if (m2 > 0 && s2 > 0) {
		i = m2 < s2 ? m2 : s2;
		b2 -= i;
		m2 -= i;
		s2 -= i;
	}
	if (b5 > 0) {
		if (leftright) {
			if (m5 > 0) {
				mhi = pow5mult(mhi, m5);
				b1 = mult(mhi, b);
				Bfree(b);
				b = b1;
			}
			if ((j = b5 - m5)) {
				b = pow5mult(b, j);
			}
		} else {
			b = pow5mult(b, b5);
		}
	}
	S = i2b(1);
	if (s5 > 0)
		S = pow5mult(S, s5);
	/* Check for special case that d is a normalized power of 2. */

	if (mode < 2) {
		if (!word1(d) && !(word0(d) & Bndry_mask)
#ifndef Sudden_Underflow
				&& word0(d) & Exp_mask
#endif
		   ) {
			/* The special case */
			b2 += Log2P;
			s2 += Log2P;
			spec_case = 1;
		} else {
			spec_case = 0;
		}
	}

	/* Arrange for convenient computation of quotients:
	 * shift left if necessary so divisor has 4 leading 0 bits.
	 *
	 * Perhaps we should just compute leading 28 bits of S once
	 * and for all and pass them and a shift to quorem, so it
	 * can do shifts and ors to compute the numerator for q.
	 */
#ifdef Pack_32
	if ((i = ((s5 ? 32 - hi0bits(S->x[S->wds-1]) : 1) + s2) & 0x1f))
		i = 32 - i;
#else
	if ((i = ((s5 ? 32 - hi0bits(S->x[S->wds-1]) : 1) + s2) & 0xf))
		i = 16 - i;
#endif
	if (i > 4) {
		i -= 4;
		b2 += i;
		m2 += i;
		s2 += i;
	}
	else if (i < 4) {
		i += 28;
		b2 += i;
		m2 += i;
		s2 += i;
	}
	if (b2 > 0)
		b = lshift(b, b2);
	if (s2 > 0)
		S = lshift(S, s2);
	if (k_check) {
		if (cmp(b,S) < 0) {
			k--;
			b = multadd(b, 10, 0);  /* we botched the k estimate */
			if (leftright)
				mhi = multadd(mhi, 10, 0);
			ilim = ilim1;
		}
	}
	if (ilim <= 0 && mode > 2) {
		if (ilim < 0 || cmp(b,S = multadd(S,5,0)) <= 0) {
			/* no digits, fcvt style */
no_digits:
			k = -1 - ndigits;
			goto ret;
		}
one_digit:
		*s++ = '1';
		k++;
		goto ret;
	}
	if (leftright) {
		if (m2 > 0)
			mhi = lshift(mhi, m2);

		/* Compute mlo -- check for special case
		 * that d is a normalized power of 2.
		 */

		mlo = mhi;
		if (spec_case) {
			mhi = Balloc(mhi->k);
			Bcopy(mhi, mlo);
			mhi = lshift(mhi, Log2P);
		}

		for(i = 1;;i++) {
			dig = quorem(b,S) + '0';
			/* Do we yet have the shortest decimal string
			 * that will round to d?
			 */
			j = cmp(b, mlo);
			delta = diff(S, mhi);
			j1 = delta->sign ? 1 : cmp(b, delta);
			Bfree(delta);
#ifndef ROUND_BIASED
			if (j1 == 0 && !mode && !(word1(d) & 1)) {
				if (dig == '9')
					goto round_9_up;
				if (j > 0)
					dig++;
				*s++ = dig;
				goto ret;
			}
#endif
			if (j < 0 || (j == 0 && !mode
#ifndef ROUND_BIASED
						&& !(word1(d) & 1)
#endif
						)) {
				if (j1 > 0) {
					b = lshift(b, 1);
					j1 = cmp(b, S);
					if ((j1 > 0 || (j1 == 0 && (dig & 1)))
							&& dig++ == '9')
						goto round_9_up;
				}
				*s++ = dig;
				goto ret;
			}
			if (j1 > 0) {
				if (dig == '9') { /* possible if i == 1 */
round_9_up:
					*s++ = '9';
					goto roundoff;
				}
				*s++ = dig + 1;
				goto ret;
			}
			*s++ = dig;
			if (i == ilim)
				break;
			b = multadd(b, 10, 0);
			if (mlo == mhi)
				mlo = mhi = multadd(mhi, 10, 0);
			else {
				mlo = multadd(mlo, 10, 0);
				mhi = multadd(mhi, 10, 0);
			}
		}
	}
	else
		for(i = 1;; i++) {
			*s++ = dig = quorem(b,S) + '0';
			if (i >= ilim)
				break;
			b = multadd(b, 10, 0);
		}

	/* Round off last digit */

	b = lshift(b, 1);
	j = cmp(b, S);
	if (j > 0 || (j == 0 && (dig & 1))) {
roundoff:
		while(*--s == '9')
			if (s == s0) {
				k++;
				*s++ = '1';
				goto ret;
			}
		++*s++;
	}
	else {
		while(*--s == '0');
		s++;
	}
ret:
	Bfree(S);
	if (mhi) {
		if (mlo && mlo != mhi)
			Bfree(mlo);
		Bfree(mhi);
	}
ret1:

	_THREAD_PRIVATE_MUTEX_LOCK(pow5mult_mutex);
	while (p5s) {
		tmp = p5s;
		p5s = p5s->next;
		free(tmp);
	}
	_THREAD_PRIVATE_MUTEX_UNLOCK(pow5mult_mutex);

	Bfree(b);

	if (s == s0) {              /* don't return empty string */
		*s++ = '0';
		k = 0;
	}
	*s = 0;
	*decpt = k + 1;
	if (rve)
		*rve = s;
	return s0;
}

ZEND_API double zend_strtod (CONST char *s00, CONST char **se)
{
	int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, dsign,
		e, e1, esign, i, j, k, nd, nd0, nf, nz, nz0, sign;
	CONST char *s, *s0, *s1;
	volatile double aadj, aadj1, adj;
	volatile _double rv, rv0;
	Long L;
	ULong y, z;
	Bigint *bb, *bb1, *bd, *bd0, *bs, *delta, *tmp;
	double result;

	CONST char decimal_point = '.';

	sign = nz0 = nz = 0;
	value(rv) = 0.;


	for(s = s00; isspace((unsigned char) *s); s++)
		;

	if (*s == '-') {
		sign = 1;
		s++;
	} else if (*s == '+') {
		s++;
	}

	if (*s == '\0') {
		s = s00;
		goto ret;
	}

	if (*s == '0') {
		nz0 = 1;
		while(*++s == '0') ;
		if (!*s)
			goto ret;
	}
	s0 = s;
	y = z = 0;
	for(nd = nf = 0; (c = *s) >= '0' && c <= '9'; nd++, s++)
		if (nd < 9)
			y = 10*y + c - '0';
		else if (nd < 16)
			z = 10*z + c - '0';
	nd0 = nd;
	if (c == decimal_point) {
		c = *++s;
		if (!nd) {
			for(; c == '0'; c = *++s)
				nz++;
			if (c > '0' && c <= '9') {
				s0 = s;
				nf += nz;
				nz = 0;
				goto have_dig;
			}
			goto dig_done;
		}
		for(; c >= '0' && c <= '9'; c = *++s) {
have_dig:
			nz++;
			if (c -= '0') {
				nf += nz;
				for(i = 1; i < nz; i++)
					if (nd++ < 9)
						y *= 10;
					else if (nd <= DBL_DIG + 1)
						z *= 10;
				if (nd++ < 9)
					y = 10*y + c;
				else if (nd <= DBL_DIG + 1)
					z = 10*z + c;
				nz = 0;
			}
		}
	}
dig_done:
	e = 0;
	if (c == 'e' || c == 'E') {
		if (!nd && !nz && !nz0) {
			s = s00;
			goto ret;
		}
		s00 = s;
		esign = 0;
		switch(c = *++s) {
			case '-':
				esign = 1;
			case '+':
				c = *++s;
		}
		if (c >= '0' && c <= '9') {
			while(c == '0')
				c = *++s;
			if (c > '0' && c <= '9') {
				L = c - '0';
				s1 = s;
				while((c = *++s) >= '0' && c <= '9')
					L = 10*L + c - '0';
				if (s - s1 > 8 || L > 19999)
					/* Avoid confusion from exponents
					 * so large that e might overflow.
					 */
					e = 19999; /* safe for 16 bit ints */
				else
					e = (int)L;
				if (esign)
					e = -e;
			}
			else
				e = 0;
		}
		else
			s = s00;
	}
	if (!nd) {
		if (!nz && !nz0)
			s = s00;
		goto ret;
	}
	e1 = e -= nf;

	/* Now we have nd0 digits, starting at s0, followed by a
	 * decimal point, followed by nd-nd0 digits.  The number we're
	 * after is the integer represented by those digits times
	 * 10**e */

	if (!nd0)
		nd0 = nd;
	k = nd < DBL_DIG + 1 ? nd : DBL_DIG + 1;
	value(rv) = y;
	if (k > 9)
		value(rv) = tens[k - 9] * value(rv) + z;
	bd0 = 0;
	if (nd <= DBL_DIG
#ifndef RND_PRODQUOT
			&& FLT_ROUNDS == 1
#endif
	   ) {
		if (!e)
			goto ret;
		if (e > 0) {
			if (e <= Ten_pmax) {
#ifdef VAX
				goto vax_ovfl_check;
#else
				/* value(rv) = */ rounded_product(value(rv),
						tens[e]);
				goto ret;
#endif
			}
			i = DBL_DIG - nd;
			if (e <= Ten_pmax + i) {
				/* A fancier test would sometimes let us do
				 * this for larger i values.
				 */
				e -= i;
				value(rv) *= tens[i];
#ifdef VAX
				/* VAX exponent range is so narrow we must
				 * worry about overflow here...
				 */
vax_ovfl_check:
				word0(rv) -= P*Exp_msk1;
				/* value(rv) = */ rounded_product(value(rv),
						tens[e]);
				if ((word0(rv) & Exp_mask)
						> Exp_msk1*(DBL_MAX_EXP+Bias-1-P))
					goto ovfl;
				word0(rv) += P*Exp_msk1;
#else
				/* value(rv) = */ rounded_product(value(rv),
						tens[e]);
#endif
				goto ret;
			}
		}
#ifndef Inaccurate_Divide
		else if (e >= -Ten_pmax) {
			/* value(rv) = */ rounded_quotient(value(rv),
					tens[-e]);
			goto ret;
		}
#endif
	}
	e1 += nd - k;

	/* Get starting approximation = rv * 10**e1 */

	if (e1 > 0) {
		if ((i = e1 & 15))
			value(rv) *= tens[i];
		if (e1 &= ~15) {
			if (e1 > DBL_MAX_10_EXP) {
ovfl:
				errno = ERANGE;
#ifndef Bad_float_h
				value(rv) = HUGE_VAL;
#else
				/* Can't trust HUGE_VAL */
#ifdef IEEE_Arith
				word0(rv) = Exp_mask;
				word1(rv) = 0;
#else
				word0(rv) = Big0;
				word1(rv) = Big1;
#endif
#endif
				if (bd0)
					goto retfree;
				goto ret;
			}
			if (e1 >>= 4) {
				for(j = 0; e1 > 1; j++, e1 >>= 1)
					if (e1 & 1)
						value(rv) *= bigtens[j];
				/* The last multiplication could overflow. */
				word0(rv) -= P*Exp_msk1;
				value(rv) *= bigtens[j];
				if ((z = word0(rv) & Exp_mask)
						> Exp_msk1*(DBL_MAX_EXP+Bias-P))
					goto ovfl;
				if (z > Exp_msk1*(DBL_MAX_EXP+Bias-1-P)) {
					/* set to largest number */
					/* (Can't trust DBL_MAX) */
					word0(rv) = Big0;
					word1(rv) = Big1;
				}
				else
					word0(rv) += P*Exp_msk1;
			}

		}
	}
	else if (e1 < 0) {
		e1 = -e1;
		if ((i = e1 & 15))
			value(rv) /= tens[i];
		if (e1 &= ~15) {
			e1 >>= 4;
			if (e1 >= 1 << n_bigtens)
				goto undfl;
			for(j = 0; e1 > 1; j++, e1 >>= 1)
				if (e1 & 1)
					value(rv) *= tinytens[j];
			/* The last multiplication could underflow. */
			value(rv0) = value(rv);
			value(rv) *= tinytens[j];
			if (!value(rv)) {
				value(rv) = 2.*value(rv0);
				value(rv) *= tinytens[j];
				if (!value(rv)) {
undfl:
					value(rv) = 0.;
					errno = ERANGE;
					if (bd0)
						goto retfree;
					goto ret;
				}
				word0(rv) = Tiny0;
				word1(rv) = Tiny1;
				/* The refinement below will clean
				 * this approximation up.
				 */
			}
		}
	}

	/* Now the hard part -- adjusting rv to the correct value.*/

	/* Put digits into bd: true value = bd * 10^e */

	bd0 = s2b(s0, nd0, nd, y);

	for(;;) {
		bd = Balloc(bd0->k);
		Bcopy(bd, bd0);
		bb = d2b(value(rv), &bbe, &bbbits);	/* rv = bb * 2^bbe */
		bs = i2b(1);

		if (e >= 0) {
			bb2 = bb5 = 0;
			bd2 = bd5 = e;
		}
		else {
			bb2 = bb5 = -e;
			bd2 = bd5 = 0;
		}
		if (bbe >= 0)
			bb2 += bbe;
		else
			bd2 -= bbe;
		bs2 = bb2;
#ifdef Sudden_Underflow
#ifdef IBM
		j = 1 + 4*P - 3 - bbbits + ((bbe + bbbits - 1) & 3);
#else
		j = P + 1 - bbbits;
#endif
#else
		i = bbe + bbbits - 1;	/* logb(rv) */
		if (i < Emin)	/* denormal */
			j = bbe + (P-Emin);
		else
			j = P + 1 - bbbits;
#endif
		bb2 += j;
		bd2 += j;
		i = bb2 < bd2 ? bb2 : bd2;
		if (i > bs2)
			i = bs2;
		if (i > 0) {
			bb2 -= i;
			bd2 -= i;
			bs2 -= i;
		}
		if (bb5 > 0) {
			bs = pow5mult(bs, bb5);
			bb1 = mult(bs, bb);
			Bfree(bb);
			bb = bb1;
		}
		if (bb2 > 0)
			bb = lshift(bb, bb2);
		if (bd5 > 0)
			bd = pow5mult(bd, bd5);
		if (bd2 > 0)
			bd = lshift(bd, bd2);
		if (bs2 > 0)
			bs = lshift(bs, bs2);
		delta = diff(bb, bd);
		dsign = delta->sign;
		delta->sign = 0;
		i = cmp(delta, bs);
		if (i < 0) {
			/* Error is less than half an ulp -- check for
			 * special case of mantissa a power of two.
			 */
			if (dsign || word1(rv) || word0(rv) & Bndry_mask)
				break;
			delta = lshift(delta,Log2P);
			if (cmp(delta, bs) > 0)
				goto drop_down;
			break;
		}
		if (i == 0) {
			/* exactly half-way between */
			if (dsign) {
				if ((word0(rv) & Bndry_mask1) == Bndry_mask1
						&&  word1(rv) == 0xffffffff) {
					/*boundary case -- increment exponent*/
					word0(rv) = (word0(rv) & Exp_mask)
						+ Exp_msk1
#ifdef IBM
						| Exp_msk1 >> 4
#endif
						;
					word1(rv) = 0;
					break;
				}
			}
			else if (!(word0(rv) & Bndry_mask) && !word1(rv)) {
drop_down:
				/* boundary case -- decrement exponent */
#ifdef Sudden_Underflow
				L = word0(rv) & Exp_mask;
#ifdef IBM
				if (L <  Exp_msk1)
#else
					if (L <= Exp_msk1)
#endif
						goto undfl;
				L -= Exp_msk1;
#else
				L = (word0(rv) & Exp_mask) - Exp_msk1;
#endif
				word0(rv) = L | Bndry_mask1;
				word1(rv) = 0xffffffff;
#ifdef IBM
				goto cont;
#else
				break;
#endif
			}
#ifndef ROUND_BIASED
			if (!(word1(rv) & LSB))
				break;
#endif
			if (dsign)
				value(rv) += ulp(value(rv));
#ifndef ROUND_BIASED
			else {
				value(rv) -= ulp(value(rv));
#ifndef Sudden_Underflow
				if (!value(rv))
					goto undfl;
#endif
			}
#endif
			break;
		}
		if ((aadj = ratio(delta, bs)) <= 2.) {
			if (dsign)
				aadj = aadj1 = 1.;
			else if (word1(rv) || word0(rv) & Bndry_mask) {
#ifndef Sudden_Underflow
				if (word1(rv) == Tiny1 && !word0(rv))
					goto undfl;
#endif
				aadj = 1.;
				aadj1 = -1.;
			}
			else {
				/* special case -- power of FLT_RADIX to be */
				/* rounded down... */

				if (aadj < 2./FLT_RADIX)
					aadj = 1./FLT_RADIX;
				else
					aadj *= 0.5;
				aadj1 = -aadj;
			}
		}
		else {
			aadj *= 0.5;
			aadj1 = dsign ? aadj : -aadj;
#ifdef Check_FLT_ROUNDS
			switch(FLT_ROUNDS) {
				case 2: /* towards +infinity */
					aadj1 -= 0.5;
					break;
				case 0: /* towards 0 */
				case 3: /* towards -infinity */
					aadj1 += 0.5;
			}
#else
			if (FLT_ROUNDS == 0)
				aadj1 += 0.5;
#endif
		}
		y = word0(rv) & Exp_mask;

		/* Check for overflow */

		if (y == Exp_msk1*(DBL_MAX_EXP+Bias-1)) {
			value(rv0) = value(rv);
			word0(rv) -= P*Exp_msk1;
			adj = aadj1 * ulp(value(rv));
			value(rv) += adj;
			if ((word0(rv) & Exp_mask) >=
					Exp_msk1*(DBL_MAX_EXP+Bias-P)) {
				if (word0(rv0) == Big0 && word1(rv0) == Big1)
					goto ovfl;
				word0(rv) = Big0;
				word1(rv) = Big1;
				goto cont;
			}
			else
				word0(rv) += P*Exp_msk1;
		}
		else {
#ifdef Sudden_Underflow
			if ((word0(rv) & Exp_mask) <= P*Exp_msk1) {
				value(rv0) = value(rv);
				word0(rv) += P*Exp_msk1;
				adj = aadj1 * ulp(value(rv));
				value(rv) += adj;
#ifdef IBM
				if ((word0(rv) & Exp_mask) <  P*Exp_msk1)
#else
					if ((word0(rv) & Exp_mask) <= P*Exp_msk1)
#endif
					{
						if (word0(rv0) == Tiny0
								&& word1(rv0) == Tiny1)
							goto undfl;
						word0(rv) = Tiny0;
						word1(rv) = Tiny1;
						goto cont;
					}
					else
						word0(rv) -= P*Exp_msk1;
			}
			else {
				adj = aadj1 * ulp(value(rv));
				value(rv) += adj;
			}
#else
			/* Compute adj so that the IEEE rounding rules will
			 * correctly round rv + adj in some half-way cases.
			 * If rv * ulp(rv) is denormalized (i.e.,
			 * y <= (P-1)*Exp_msk1), we must adjust aadj to avoid
			 * trouble from bits lost to denormalization;
			 * example: 1.2e-307 .
			 */
			if (y <= (P-1)*Exp_msk1 && aadj >= 1.) {
				aadj1 = (double)(int)(aadj + 0.5);
				if (!dsign)
					aadj1 = -aadj1;
			}
			adj = aadj1 * ulp(value(rv));
			value(rv) += adj;
#endif
		}
		z = word0(rv) & Exp_mask;
		if (y == z) {
			/* Can we stop now? */
			L = aadj;
			aadj -= L;
			/* The tolerances below are conservative. */
			if (dsign || word1(rv) || word0(rv) & Bndry_mask) {
				if (aadj < .4999999 || aadj > .5000001)
					break;
			}
			else if (aadj < .4999999/FLT_RADIX)
				break;
		}
cont:
		Bfree(bb);
		Bfree(bd);
		Bfree(bs);
		Bfree(delta);
	}
retfree:
	Bfree(bb);
	Bfree(bd);
	Bfree(bs);
	Bfree(bd0);
	Bfree(delta);
ret:
	if (se)
		*se = s;
	result = sign ? -value(rv) : value(rv);

	_THREAD_PRIVATE_MUTEX_LOCK(pow5mult_mutex);
	while (p5s) {
		tmp = p5s;
		p5s = p5s->next;
		free(tmp);
	}
	_THREAD_PRIVATE_MUTEX_UNLOCK(pow5mult_mutex);

	return result;
}

ZEND_API double zend_hex_strtod(const char *str, const char **endptr)
{
	const char *s = str;
	char c;
	int any = 0;
	double value = 0;

	if (strlen(str) < 2) {
		*endptr = str;
		return 0.0;
	}

	if (*s == '0' && (s[1] == 'x' || s[1] == 'X')) {
		s += 2;
	}

	while ((c = *s++)) {
		if (c >= '0' && c <= '9') {
			c -= '0';
		} else if (c >= 'A' && c <= 'F') {
			c -= 'A' - 10;
		} else if (c >= 'a' && c <= 'f') {
			c -= 'a' - 10;
		} else {
			break;
		}

		any = 1;
		value = value * 16 + c;
	}

	if (endptr != NULL) {
		*endptr = any ? s - 1 : str;
	}

	return value;
}

ZEND_API double zend_oct_strtod(const char *str, const char **endptr)
{
	const char *s = str;
	char c;
	double value = 0;
	int any = 0;

	if (strlen(str) < 1) {
		*endptr = str;
		return 0.0;
	}

	/* skip leading zero */
	s++;

	while ((c = *s++)) {
		if (c < '0' || c > '7') {
			/* break and return the current value if the number is not well-formed
			 * that's what Linux strtol() does
			 */
			break;
		}
		value = value * 8 + c - '0';
		any = 1;
	}

	if (endptr != NULL) {
		*endptr = any ? s - 1 : str;
	}

	return value;
}

ZEND_API double zend_bin_strtod(const char *str, const char **endptr)
{
	const char *s = str;
	char 		c;
	double 		value = 0;
	int 		any = 0;

	if (strlen(str) < 2) {
		*endptr = str;
		return 0.0;
	}

	if ('0' == *s && ('b' == s[1] || 'B' == s[1])) {
		s += 2;
	}

	while ((c = *s++)) {
		/*
		 * Verify the validity of the current character as a base-2 digit.  In
		 * the event that an invalid digit is found, halt the conversion and
		 * return the portion which has been converted thus far.
		 */
		if ('0' == c || '1' == c)
			value = value * 2 + c - '0';
		else
			break;

		any = 1;
	}

	/*
	 * As with many strtoX implementations, should the subject sequence be
	 * empty or not well-formed, no conversion is performed and the original
	 * value of str is stored in *endptr, provided that endptr is not a null
	 * pointer.
	 */
	if (NULL != endptr) {
		*endptr = (char *)(any ? s - 1 : str);
	}

	return value;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
