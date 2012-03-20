/**********************************************************************

  time.c -

  $Author$
  created at: Tue Dec 28 14:31:59 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#include "ruby/ruby.h"
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include "ruby/encoding.h"
#include "internal.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <float.h>
#include <math.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "timev.h"

static ID id_divmod, id_mul, id_submicro, id_nano_num, id_nano_den, id_offset;
static ID id_eq, id_ne, id_quo, id_div, id_cmp, id_lshift;

#define NDIV(x,y) (-(-((x)+1)/(y))-1)
#define NMOD(x,y) ((y)-(-((x)+1)%(y))-1)
#define DIV(n,d) ((n)<0 ? NDIV((n),(d)) : (n)/(d))
#define MOD(n,d) ((n)<0 ? NMOD((n),(d)) : (n)%(d))

static int
eq(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y)) {
        return x == y;
    }
    return RTEST(rb_funcall(x, id_eq, 1, y));
}

static int
cmp(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y)) {
        if ((long)x < (long)y)
            return -1;
        if ((long)x > (long)y)
            return 1;
        return 0;
    }
    return rb_cmpint(rb_funcall(x, id_cmp, 1, y), x, y);
}

#define ne(x,y) (!eq((x),(y)))
#define lt(x,y) (cmp((x),(y)) < 0)
#define gt(x,y) (cmp((x),(y)) > 0)
#define le(x,y) (cmp((x),(y)) <= 0)
#define ge(x,y) (cmp((x),(y)) >= 0)

static VALUE
add(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y)) {
        long l = FIX2LONG(x) + FIX2LONG(y);
        if (FIXABLE(l)) return LONG2FIX(l);
        return LONG2NUM(l);
    }
    if (RB_TYPE_P(x, T_BIGNUM)) return rb_big_plus(x, y);
    return rb_funcall(x, '+', 1, y);
}

static VALUE
sub(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y)) {
        long l = FIX2LONG(x) - FIX2LONG(y);
        if (FIXABLE(l)) return LONG2FIX(l);
        return LONG2NUM(l);
    }
    if (RB_TYPE_P(x, T_BIGNUM)) return rb_big_minus(x, y);
    return rb_funcall(x, '-', 1, y);
}

#if !(HAVE_LONG_LONG && SIZEOF_LONG * 2 <= SIZEOF_LONG_LONG)
static int
long_mul(long x, long y, long *z)
{
    unsigned long a, b, c;
    int s;
    if (x == 0 || y == 0) {
	*z = 0;
	return 1;
    }
    if (x < 0) {
	s = -1;
	a = (unsigned long)-x;
    }
    else {
	s = 1;
	a = (unsigned long)x;
    }
    if (y < 0) {
        s = -s;
	b = (unsigned long)-y;
    }
    else {
	b = (unsigned long)y;
    }
    if (a <= ULONG_MAX / b) {
        c = a * b;
	if (s < 0) {
	    if (c <= (unsigned long)LONG_MAX + 1) {
		*z = -(long)c;
		return 1;
	    }
	}
	else {
	    if (c <= (unsigned long)LONG_MAX) {
		*z = (long)c;
		return 1;
	    }
	}
    }
    return 0;
}
#endif

static VALUE
mul(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y)) {
#if HAVE_LONG_LONG && SIZEOF_LONG * 2 <= SIZEOF_LONG_LONG
        LONG_LONG ll = (LONG_LONG)FIX2LONG(x) * FIX2LONG(y);
        if (FIXABLE(ll))
            return LONG2FIX(ll);
        return LL2NUM(ll);
#else
	long z;
	if (long_mul(FIX2LONG(x), FIX2LONG(y), &z))
	    return LONG2NUM(z);
#endif
    }
    if (RB_TYPE_P(x, T_BIGNUM))
        return rb_big_mul(x, y);
    return rb_funcall(x, '*', 1, y);
}

#define div(x,y) (rb_funcall((x), id_div, 1, (y)))

static VALUE
mod(VALUE x, VALUE y)
{
    switch (TYPE(x)) {
      case T_BIGNUM: return rb_big_modulo(x, y);
      default: return rb_funcall(x, '%', 1, y);
    }
}

#define neg(x) (sub(INT2FIX(0), (x)))
#define lshift(x,y) (rb_funcall((x), id_lshift, 1, (y)))

static VALUE
quo(VALUE x, VALUE y)
{
    VALUE ret;
    if (FIXNUM_P(x) && FIXNUM_P(y)) {
        long a, b, c;
        a = FIX2LONG(x);
        b = FIX2LONG(y);
        if (b == 0) rb_num_zerodiv();
        c = a / b;
        if (c * b == a) {
            return LONG2NUM(c);
        }
    }
    ret = rb_funcall(x, id_quo, 1, y);
    if (RB_TYPE_P(ret, T_RATIONAL) &&
        RRATIONAL(ret)->den == INT2FIX(1)) {
        ret = RRATIONAL(ret)->num;
    }
    return ret;
}

#define mulquo(x,y,z) (((y) == (z)) ? (x) : quo(mul((x),(y)),(z)))

static void
divmodv(VALUE n, VALUE d, VALUE *q, VALUE *r)
{
    VALUE tmp, ary;
    tmp = rb_funcall(n, id_divmod, 1, d);
    ary = rb_check_array_type(tmp);
    if (NIL_P(ary)) {
        rb_raise(rb_eTypeError, "unexpected divmod result: into %s",
                 rb_obj_classname(tmp));
    }
    *q = rb_ary_entry(ary, 0);
    *r = rb_ary_entry(ary, 1);
}

#if SIZEOF_LONG == 8
# define INT64toNUM(x) LONG2NUM(x)
# define UINT64toNUM(x) ULONG2NUM(x)
#elif defined(HAVE_LONG_LONG) && SIZEOF_LONG_LONG == 8
# define INT64toNUM(x) LL2NUM(x)
# define UINT64toNUM(x) ULL2NUM(x)
#endif

#if defined(HAVE_UINT64_T) && SIZEOF_LONG*2 <= SIZEOF_UINT64_T
    typedef uint64_t uwideint_t;
    typedef int64_t wideint_t;
    typedef uint64_t WIDEVALUE;
    typedef int64_t SIGNED_WIDEVALUE;
#   define WIDEVALUE_IS_WIDER 1
#   define UWIDEINT_MAX UINT64_MAX
#   define WIDEINT_MAX INT64_MAX
#   define WIDEINT_MIN INT64_MIN
#   define FIXWINT_P(tv) ((tv) & 1)
#   define FIXWVtoINT64(tv) RSHIFT((SIGNED_WIDEVALUE)(tv), 1)
#   define INT64toFIXWV(wi) ((WIDEVALUE)((SIGNED_WIDEVALUE)(wi) << 1 | FIXNUM_FLAG))
#   define FIXWV_MAX (((int64_t)1 << 62) - 1)
#   define FIXWV_MIN (-((int64_t)1 << 62))
#   define FIXWVABLE(wi) (POSFIXWVABLE(wi) && NEGFIXWVABLE(wi))
#   define WINT2FIXWV(i) WIDEVAL_WRAP(INT64toFIXWV(i))
#   define FIXWV2WINT(w) FIXWVtoINT64(WIDEVAL_GET(w))
#else
    typedef unsigned long uwideint_t;
    typedef long wideint_t;
    typedef VALUE WIDEVALUE;
    typedef SIGNED_VALUE SIGNED_WIDEVALUE;
#   define WIDEVALUE_IS_WIDER 0
#   define UWIDEINT_MAX ULONG_MAX
#   define WIDEINT_MAX LONG_MAX
#   define WIDEINT_MIN LONG_MIN
#   define FIXWINT_P(v) FIXNUM_P(v)
#   define FIXWV_MAX FIXNUM_MAX
#   define FIXWV_MIN FIXNUM_MIN
#   define FIXWVABLE(i) FIXABLE(i)
#   define WINT2FIXWV(i) WIDEVAL_WRAP(LONG2FIX(i))
#   define FIXWV2WINT(w) FIX2LONG(WIDEVAL_GET(w))
#endif

#define POSFIXWVABLE(wi) ((wi) < FIXWV_MAX+1)
#define NEGFIXWVABLE(wi) ((wi) >= FIXWV_MIN)
#define FIXWV_P(w) FIXWINT_P(WIDEVAL_GET(w))

/* #define STRUCT_WIDEVAL */
#ifdef STRUCT_WIDEVAL
    /* for type checking */
    typedef struct {
        WIDEVALUE value;
    } wideval_t;
    static inline wideval_t WIDEVAL_WRAP(WIDEVALUE v) { wideval_t w = { v }; return w; }
#   define WIDEVAL_GET(w) ((w).value)
#else
    typedef WIDEVALUE wideval_t;
#   define WIDEVAL_WRAP(v) (v)
#   define WIDEVAL_GET(w) (w)
#endif

#if WIDEVALUE_IS_WIDER
    static inline wideval_t
    wint2wv(wideint_t wi)
    {
        if (FIXWVABLE(wi))
            return WINT2FIXWV(wi);
        else
            return WIDEVAL_WRAP(INT64toNUM(wi));
    }
#   define WINT2WV(wi) wint2wv(wi)
#else
#   define WINT2WV(wi) WIDEVAL_WRAP(LONG2NUM(wi))
#endif

static inline VALUE
w2v(wideval_t w)
{
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(w))
        return INT64toNUM(FIXWV2WINT(w));
    return (VALUE)WIDEVAL_GET(w);
#else
    return WIDEVAL_GET(w);
#endif
}

#if WIDEVALUE_IS_WIDER
static int
bdigit_find_maxbit(BDIGIT d)
{
    int res = 0;
    if (d & ~(BDIGIT)0xffff) {
        d >>= 16;
        res += 16;
    }
    if (d & ~(BDIGIT)0xff) {
        d >>= 8;
        res += 8;
    }
    if (d & ~(BDIGIT)0xf) {
        d >>= 4;
        res += 4;
    }
    if (d & ~(BDIGIT)0x3) {
        d >>= 2;
        res += 2;
    }
    if (d & ~(BDIGIT)0x1) {
        d >>= 1;
        res += 1;
    }
    return res;
}

static VALUE
rb_big_abs_find_maxbit(VALUE big)
{
    BDIGIT *ds = RBIGNUM_DIGITS(big);
    BDIGIT d;
    long len = RBIGNUM_LEN(big);
    VALUE res;
    while (0 < len && ds[len-1] == 0)
        len--;
    if (len == 0)
        return Qnil;
    res = mul(LONG2NUM(len-1), INT2FIX(SIZEOF_BDIGITS * CHAR_BIT));
    d = ds[len-1];
    res = add(res, LONG2FIX(bdigit_find_maxbit(d)));
    return res;
}

static VALUE
rb_big_abs_find_minbit(VALUE big)
{
    BDIGIT *ds = RBIGNUM_DIGITS(big);
    BDIGIT d;
    long len = RBIGNUM_LEN(big);
    long i;
    VALUE res;
    for (i = 0; i < len; i++)
        if (ds[i])
            break;
    if (i == len)
        return Qnil;
    res = mul(LONG2NUM(i), INT2FIX(SIZEOF_BDIGITS * CHAR_BIT));
    d = ds[i];
    res = add(res, LONG2FIX(ffs(d)-1));
    return res;
}

static wideval_t
v2w_bignum(VALUE v)
{
    long len = RBIGNUM_LEN(v);
    BDIGIT *ds;
    wideval_t w;
    VALUE maxbit;
    ds = RBIGNUM_DIGITS(v);
    w = WIDEVAL_WRAP(v);
    maxbit = rb_big_abs_find_maxbit(v);
    if (NIL_P(maxbit))
        return WINT2FIXWV(0);
    if (lt(maxbit, INT2FIX(sizeof(wideint_t) * CHAR_BIT - 2)) ||
        (eq(maxbit, INT2FIX(sizeof(wideint_t) * CHAR_BIT - 2)) &&
         RBIGNUM_NEGATIVE_P(v) &&
         eq(rb_big_abs_find_minbit(v), INT2FIX(sizeof(wideint_t) * CHAR_BIT - 2)))) {
        wideint_t i;
        i = 0;
        while (len)
            i = (i << sizeof(BDIGIT)*CHAR_BIT) | ds[--len];
        if (RBIGNUM_NEGATIVE_P(v)) {
            i = -i;
        }
        w = WINT2FIXWV(i);
    }
    return w;
}
#endif

static inline wideval_t
v2w(VALUE v)
{
#if WIDEVALUE_IS_WIDER
    if (FIXNUM_P(v)) {
        return WIDEVAL_WRAP((WIDEVALUE)(SIGNED_WIDEVALUE)(long)v);
    }
    else if (RB_TYPE_P(v, T_BIGNUM) &&
        RBIGNUM_LEN(v) * sizeof(BDIGIT) <= sizeof(WIDEVALUE)) {
        return v2w_bignum(v);
    }
#endif
    return WIDEVAL_WRAP(v);
}

static int
weq(wideval_t wx, wideval_t wy)
{
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(wx) && FIXWV_P(wy)) {
        return WIDEVAL_GET(wx) == WIDEVAL_GET(wy);
    }
    return RTEST(rb_funcall(w2v(wx), id_eq, 1, w2v(wy)));
#else
    return eq(WIDEVAL_GET(wx), WIDEVAL_GET(wy));
#endif
}

static int
wcmp(wideval_t wx, wideval_t wy)
{
    VALUE x, y;
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(wx) && FIXWV_P(wy)) {
        wideint_t a, b;
        a = FIXWV2WINT(wx);
        b = FIXWV2WINT(wy);
        if (a < b)
            return -1;
        if (a > b)
            return 1;
        return 0;
    }
#endif
    x = w2v(wx);
    y = w2v(wy);
    return rb_cmpint(rb_funcall(x, id_cmp, 1, y), x, y);
}

#define wne(x,y) (!weq((x),(y)))
#define wlt(x,y) (wcmp((x),(y)) < 0)
#define wgt(x,y) (wcmp((x),(y)) > 0)
#define wle(x,y) (wcmp((x),(y)) <= 0)
#define wge(x,y) (wcmp((x),(y)) >= 0)

static wideval_t
wadd(wideval_t wx, wideval_t wy)
{
    VALUE x;
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(wx) && FIXWV_P(wy)) {
        wideint_t r = FIXWV2WINT(wx) + FIXWV2WINT(wy);
        return WINT2WV(r);
    }
    else
#endif
    x = w2v(wx);
    if (RB_TYPE_P(x, T_BIGNUM)) return v2w(rb_big_plus(x, w2v(wy)));
    return v2w(rb_funcall(x, '+', 1, w2v(wy)));
}

static wideval_t
wsub(wideval_t wx, wideval_t wy)
{
    VALUE x;
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(wx) && FIXWV_P(wy)) {
        wideint_t r = FIXWV2WINT(wx) - FIXWV2WINT(wy);
        return WINT2WV(r);
    }
    else
#endif
    x = w2v(wx);
    if (RB_TYPE_P(x, T_BIGNUM)) return v2w(rb_big_minus(x, w2v(wy)));
    return v2w(rb_funcall(x, '-', 1, w2v(wy)));
}

static int
wi_mul(wideint_t x, wideint_t y, wideint_t *z)
{
    uwideint_t a, b, c;
    int s;
    if (x == 0 || y == 0) {
	*z = 0;
	return 1;
    }
    if (x < 0) {
	s = -1;
	a = (uwideint_t)-x;
    }
    else {
	s = 1;
	a = (uwideint_t)x;
    }
    if (y < 0) {
        s = -s;
	b = (uwideint_t)-y;
    }
    else {
	b = (uwideint_t)y;
    }
    if (a <= UWIDEINT_MAX / b) {
        c = a * b;
	if (s < 0) {
	    if (c <= (uwideint_t)WIDEINT_MAX + 1) {
		*z = -(wideint_t)c;
		return 1;
	    }
	}
	else {
	    if (c <= (uwideint_t)WIDEINT_MAX) {
		*z = (wideint_t)c;
		return 1;
	    }
	}
    }
    return 0;
}

static wideval_t
wmul(wideval_t wx, wideval_t wy)
{
    VALUE x, z;
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(wx) && FIXWV_P(wy)) {
	wideint_t z;
	if (wi_mul(FIXWV2WINT(wx), FIXWV2WINT(wy), &z))
	    return WINT2WV(z);
    }
#endif
    x = w2v(wx);
    if (RB_TYPE_P(x, T_BIGNUM)) return v2w(rb_big_mul(x, w2v(wy)));
    z = rb_funcall(x, '*', 1, w2v(wy));
    if (RB_TYPE_P(z, T_RATIONAL) && RRATIONAL(z)->den == INT2FIX(1)) {
        z = RRATIONAL(z)->num;
    }
    return v2w(z);
}

static wideval_t
wquo(wideval_t wx, wideval_t wy)
{
    VALUE x, y, ret;
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(wx) && FIXWV_P(wy)) {
        wideint_t a, b, c;
        a = FIXWV2WINT(wx);
        b = FIXWV2WINT(wy);
        if (b == 0) rb_num_zerodiv();
        c = a / b;
        if (c * b == a) {
            return WINT2WV(c);
        }
    }
#endif
    x = w2v(wx);
    y = w2v(wy);
    ret = rb_funcall(x, id_quo, 1, y);
    if (RB_TYPE_P(ret, T_RATIONAL) &&
        RRATIONAL(ret)->den == INT2FIX(1)) {
        ret = RRATIONAL(ret)->num;
    }
    return v2w(ret);
}

#define wmulquo(x,y,z) ((WIDEVAL_GET(y) == WIDEVAL_GET(z)) ? (x) : wquo(wmul((x),(y)),(z)))
#define wmulquoll(x,y,z) (((y) == (z)) ? (x) : wquo(wmul((x),WINT2WV(y)),WINT2WV(z)))

static void
wdivmod(wideval_t wn, wideval_t wd, wideval_t *wq, wideval_t *wr)
{
    VALUE tmp, ary;
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(wn) && FIXWV_P(wd)) {
        wideint_t n, d, q, r;
        d = FIXWV2WINT(wd);
        if (d == 0) rb_num_zerodiv();
        if (d == 1) {
            *wq = wn;
            *wr = WINT2FIXWV(0);
            return;
        }
        if (d == -1) {
            wideint_t xneg = -FIXWV2WINT(wn);
            *wq = WINT2WV(xneg);
            *wr = WINT2FIXWV(0);
            return;
        }
        n = FIXWV2WINT(wn);
        if (n == 0) {
            *wq = WINT2FIXWV(0);
            *wr = WINT2FIXWV(0);
            return;
        }
        if (d < 0) {
            if (n < 0) {
                q = ((-n) / (-d));
                r = ((-n) % (-d));
                if (r != 0) {
                    q -= 1;
                    r += d;
                }
            }
            else { /* 0 < n */
                q = -(n / (-d));
                r = -(n % (-d));
            }
        }
        else { /* 0 < d */
            if (n < 0) {
                q = -((-n) / d);
                r = -((-n) % d);
                if (r != 0) {
                    q -= 1;
                    r += d;
                }
            }
            else { /* 0 < n */
                q = n / d;
                r = n % d;
            }
        }
        *wq = WINT2FIXWV(q);
        *wr = WINT2FIXWV(r);
        return;
    }
#endif
    tmp = rb_funcall(w2v(wn), id_divmod, 1, w2v(wd));
    ary = rb_check_array_type(tmp);
    if (NIL_P(ary)) {
        rb_raise(rb_eTypeError, "unexpected divmod result: into %s",
                 rb_obj_classname(tmp));
    }
    *wq = v2w(rb_ary_entry(ary, 0));
    *wr = v2w(rb_ary_entry(ary, 1));
}

static void
wmuldivmod(wideval_t wx, wideval_t wy, wideval_t wz, wideval_t *wq, wideval_t *wr)
{
    if (WIDEVAL_GET(wy) == WIDEVAL_GET(wz)) {
        *wq = wx;
        *wr = WINT2FIXWV(0);
        return;
    }
    wdivmod(wmul(wx,wy), wz, wq, wr);
}

static wideval_t
wdiv(wideval_t wx, wideval_t wy)
{
    wideval_t q, r;
    wdivmod(wx, wy, &q, &r);
    return q;
}

static wideval_t
wmod(wideval_t wx, wideval_t wy)
{
    wideval_t q, r;
    wdivmod(wx, wy, &q, &r);
    return r;
}

static VALUE
num_exact(VALUE v)
{
    VALUE tmp;
    int t;

    t = TYPE(v);
    switch (t) {
      case T_FIXNUM:
      case T_BIGNUM:
        return v;

      case T_RATIONAL:
        break;

      case T_STRING:
      case T_NIL:
        goto typeerror;

      default:
        if ((tmp = rb_check_funcall(v, rb_intern("to_r"), 0, NULL)) != Qundef) {
            if (rb_respond_to(v, rb_intern("to_str"))) goto typeerror;
            v = tmp;
            break;
        }
        if (!NIL_P(tmp = rb_check_to_integer(v, "to_int"))) {
            v = tmp;
            break;
        }
        goto typeerror;
    }

    t = TYPE(v);
    switch (t) {
      case T_FIXNUM:
      case T_BIGNUM:
        return v;

      case T_RATIONAL:
        if (RRATIONAL(v)->den == INT2FIX(1))
            v = RRATIONAL(v)->num;
        break;

      default:
      typeerror:
        rb_raise(rb_eTypeError, "can't convert %s into an exact number",
                                NIL_P(v) ? "nil" : rb_obj_classname(v));
    }
    return v;
}

/* time_t */

#ifndef TYPEOF_TIMEVAL_TV_SEC
# define TYPEOF_TIMEVAL_TV_SEC time_t
#endif
#ifndef TYPEOF_TIMEVAL_TV_USEC
# if INT_MAX >= 1000000
# define TYPEOF_TIMEVAL_TV_USEC int
# else
# define TYPEOF_TIMEVAL_TV_USEC long
# endif
#endif

#if SIZEOF_TIME_T == SIZEOF_LONG
typedef unsigned long unsigned_time_t;
#elif SIZEOF_TIME_T == SIZEOF_INT
typedef unsigned int unsigned_time_t;
#elif SIZEOF_TIME_T == SIZEOF_LONG_LONG
typedef unsigned LONG_LONG unsigned_time_t;
#else
# error cannot find integer type which size is same as time_t.
#endif

#define TIMET_MAX (~(time_t)0 <= 0 ? (time_t)((~(unsigned_time_t)0) >> 1) : (time_t)(~(unsigned_time_t)0))
#define TIMET_MIN (~(time_t)0 <= 0 ? (time_t)(((unsigned_time_t)1) << (sizeof(time_t) * CHAR_BIT - 1)) : (time_t)0)

static wideval_t
rb_time_magnify(wideval_t w)
{
    if (FIXWV_P(w)) {
	wideint_t z;
	if (wi_mul(FIXWV2WINT(w), TIME_SCALE, &z))
	    return WINT2WV(z);
    }
    return wmul(w, WINT2FIXWV(TIME_SCALE));
}

static wideval_t
rb_time_unmagnify(wideval_t w)
{
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(w)) {
        wideint_t a, b, c;
        a = FIXWV2WINT(w);
        b = TIME_SCALE;
        c = a / b;
        if (c * b == a) {
            return WINT2FIXWV(c);
        }
    }
#endif
    return wquo(w, WINT2FIXWV(TIME_SCALE));
}

static VALUE
rb_time_unmagnify_to_float(wideval_t w)
{
    VALUE v;
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(w)) {
        wideint_t a, b, c;
        a = FIXWV2WINT(w);
        b = TIME_SCALE;
        c = a / b;
        if (c * b == a) {
            return DBL2NUM((double)c);
        }
        v = DBL2NUM((double)FIXWV2WINT(w));
        return quo(v, DBL2NUM(TIME_SCALE));
    }
#endif
    v = w2v(w);
    return quo(v, DBL2NUM(TIME_SCALE));
}

static void
split_second(wideval_t timew, wideval_t *timew_p, VALUE *subsecx_p)
{
    wideval_t q, r;
    wdivmod(timew, WINT2FIXWV(TIME_SCALE), &q, &r);
    *timew_p = q;
    *subsecx_p = w2v(r);
}

static wideval_t
timet2wv(time_t t)
{
#if WIDEVALUE_IS_WIDER
    if (TIMET_MIN == 0) {
        uwideint_t wi = (uwideint_t)t;
        if (wi <= FIXWV_MAX) {
            return WINT2FIXWV(wi);
        }
    }
    else {
        wideint_t wi = (wideint_t)t;
        if (FIXWV_MIN <= wi && wi <= FIXWV_MAX) {
            return WINT2FIXWV(wi);
        }
    }
#endif
    return v2w(TIMET2NUM(t));
}
#define TIMET2WV(t) timet2wv(t)

static time_t
wv2timet(wideval_t w)
{
#if WIDEVALUE_IS_WIDER
    if (FIXWV_P(w)) {
        wideint_t wi = FIXWV2WINT(w);
        if (TIMET_MIN == 0) {
            if (wi < 0)
                rb_raise(rb_eRangeError, "negative value to convert into `time_t'");
            if (TIMET_MAX < (uwideint_t)wi)
                rb_raise(rb_eRangeError, "too big to convert into `time_t'");
        }
        else {
            if (wi < TIMET_MIN || TIMET_MAX < wi)
                rb_raise(rb_eRangeError, "too big to convert into `time_t'");
        }
        return (time_t)wi;
    }
#endif
    return NUM2TIMET(w2v(w));
}
#define WV2TIMET(t) wv2timet(t)

VALUE rb_cTime;
static VALUE time_utc_offset _((VALUE));

static int obj2int(VALUE obj);
static VALUE obj2vint(VALUE obj);
static int month_arg(VALUE arg);
static void validate_utc_offset(VALUE utc_offset);
static void validate_vtm(struct vtm *vtm);

static VALUE time_gmtime(VALUE);
static VALUE time_localtime(VALUE);
static VALUE time_fixoff(VALUE);

static time_t timegm_noleapsecond(struct tm *tm);
static int tmcmp(struct tm *a, struct tm *b);
static int vtmcmp(struct vtm *a, struct vtm *b);
static const char *find_time_t(struct tm *tptr, int utc_p, time_t *tp);

static struct vtm *localtimew(wideval_t timew, struct vtm *result);

static int leap_year_p(long y);
#define leap_year_v_p(y) leap_year_p(NUM2LONG(mod((y), INT2FIX(400))))

#ifdef HAVE_GMTIME_R
#define rb_gmtime_r(t, tm) gmtime_r((t), (tm))
#define rb_localtime_r(t, tm) localtime_r((t), (tm))
#else
static inline struct tm *
rb_gmtime_r(const time_t *tp, struct tm *result)
{
    struct tm *t = gmtime(tp);
    if (t) *result = *t;
    return t;
}

static inline struct tm *
rb_localtime_r(const time_t *tp, struct tm *result)
{
    struct tm *t = localtime(tp);
    if (t) *result = *t;
    return t;
}
#endif

static struct tm *
rb_localtime_r2(const time_t *t, struct tm *result)
{
#if defined __APPLE__ && defined __LP64__
    if (*t != (time_t)(int)*t) return NULL;
#endif
    result = rb_localtime_r(t, result);
#if defined(HAVE_MKTIME) && defined(LOCALTIME_OVERFLOW_PROBLEM)
    if (result) {
        long gmtoff1 = 0;
        long gmtoff2 = 0;
        struct tm tmp = *result;
        time_t t2;
#  if defined(HAVE_STRUCT_TM_TM_GMTOFF)
        gmtoff1 = result->tm_gmtoff;
#  endif
        t2 = mktime(&tmp);
#  if defined(HAVE_STRUCT_TM_TM_GMTOFF)
        gmtoff2 = tmp.tm_gmtoff;
#  endif
        if (*t + gmtoff1 != t2 + gmtoff2)
            result = NULL;
    }
#endif
    return result;
}
#define LOCALTIME(tm, result) (tzset(),rb_localtime_r2((tm), &(result)))

#if !defined(HAVE_STRUCT_TM_TM_GMTOFF)
static struct tm *
rb_gmtime_r2(const time_t *t, struct tm *result)
{
    result = rb_gmtime_r(t, result);
#if defined(HAVE_TIMEGM) && defined(LOCALTIME_OVERFLOW_PROBLEM)
    if (result) {
	struct tm tmp = *result;
	time_t t2 = timegm(&tmp);
	if (*t != t2)
	    result = NULL;
    }
#endif
    return result;
}
#   define GMTIME(tm, result) rb_gmtime_r2((tm), &(result))
#endif

static const int common_year_yday_offset[] = {
    -1,
    -1 + 31,
    -1 + 31 + 28,
    -1 + 31 + 28 + 31,
    -1 + 31 + 28 + 31 + 30,
    -1 + 31 + 28 + 31 + 30 + 31,
    -1 + 31 + 28 + 31 + 30 + 31 + 30,
    -1 + 31 + 28 + 31 + 30 + 31 + 30 + 31,
    -1 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
    -1 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    -1 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    -1 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30
      /* 1    2    3    4    5    6    7    8    9    10   11 */
};
static const int leap_year_yday_offset[] = {
    -1,
    -1 + 31,
    -1 + 31 + 29,
    -1 + 31 + 29 + 31,
    -1 + 31 + 29 + 31 + 30,
    -1 + 31 + 29 + 31 + 30 + 31,
    -1 + 31 + 29 + 31 + 30 + 31 + 30,
    -1 + 31 + 29 + 31 + 30 + 31 + 30 + 31,
    -1 + 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31,
    -1 + 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    -1 + 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    -1 + 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30
      /* 1    2    3    4    5    6    7    8    9    10   11 */
};

static const int common_year_days_in_month[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
static const int leap_year_days_in_month[] = {
    31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int
calc_tm_yday(long tm_year, int tm_mon, int tm_mday)
{
    int tm_year_mod400 = (int)MOD(tm_year, 400);
    int tm_yday = tm_mday;

    if (leap_year_p(tm_year_mod400 + 1900))
	tm_yday += leap_year_yday_offset[tm_mon];
    else
	tm_yday += common_year_yday_offset[tm_mon];

    return tm_yday;
}

static wideval_t
timegmw_noleapsecond(struct vtm *vtm)
{
    VALUE year1900;
    VALUE q400, r400;
    int year_mod400;
    int yday;
    long days_in400;
    VALUE vdays, ret;
    wideval_t wret;

    year1900 = sub(vtm->year, INT2FIX(1900));

    divmodv(year1900, INT2FIX(400), &q400, &r400);
    year_mod400 = NUM2INT(r400);

    yday = calc_tm_yday(year_mod400, vtm->mon-1, vtm->mday);

    /*
     *  `Seconds Since the Epoch' in SUSv3:
     *  tm_sec + tm_min*60 + tm_hour*3600 + tm_yday*86400 +
     *  (tm_year-70)*31536000 + ((tm_year-69)/4)*86400 -
     *  ((tm_year-1)/100)*86400 + ((tm_year+299)/400)*86400
     */
    ret = LONG2NUM(vtm->sec
                 + vtm->min*60
                 + vtm->hour*3600);
    days_in400 = yday
               - 70*365
               + DIV(year_mod400 - 69, 4)
               - DIV(year_mod400 - 1, 100)
               + (year_mod400 + 299) / 400;
    vdays = LONG2NUM(days_in400);
    vdays = add(vdays, mul(q400, INT2FIX(97)));
    vdays = add(vdays, mul(year1900, INT2FIX(365)));
    wret = wadd(rb_time_magnify(v2w(ret)), wmul(rb_time_magnify(v2w(vdays)), WINT2FIXWV(86400)));
    wret = wadd(wret, v2w(vtm->subsecx));

    return wret;
}

static st_table *zone_table;

static const char *
zone_str(const char *s)
{
    st_data_t k, v;

    if (!zone_table)
        zone_table = st_init_strtable();

    k = (st_data_t)s;
    if (st_lookup(zone_table, k, &v)) {
        return (const char *)v;
    }
    s = strdup(s);
    k = (st_data_t)s;
    st_add_direct(zone_table, k, k);

    return s;
}

static void
gmtimew_noleapsecond(wideval_t timew, struct vtm *vtm)
{
    VALUE v;
    int i, n, x, y;
    const int *yday_offset;
    int wday;
    VALUE timev;
    wideval_t timew2, w, w2;

    vtm->isdst = 0;

    split_second(timew, &timew2, &vtm->subsecx);

    wdivmod(timew2, WINT2FIXWV(86400), &w2, &w);
    timev = w2v(w2);
    v = w2v(w);

    wday = NUM2INT(mod(timev, INT2FIX(7)));
    vtm->wday = (wday + 4) % 7;

    n = NUM2INT(v);
    vtm->sec = n % 60; n = n / 60;
    vtm->min = n % 60; n = n / 60;
    vtm->hour = n;

    /* 97 leap days in the 400 year cycle */
    divmodv(timev, INT2FIX(400*365 + 97), &timev, &v);
    vtm->year = mul(timev, INT2FIX(400));

    /* n is the days in the 400 year cycle.
     * the start of the cycle is 1970-01-01. */

    n = NUM2INT(v);
    y = 1970;

    /* 30 years including 7 leap days (1972, 1976, ... 1996),
     * 31 days in January 2000 and
     * 29 days in February 2000
     * from 1970-01-01 to 2000-02-29 */
    if (30*365+7+31+29-1 <= n) {
        /* 2000-02-29 or after */
        if (n < 31*365+8) {
            /* 2000-02-29 to 2000-12-31 */
            y += 30;
            n -= 30*365+7;
            goto found;
        }
        else {
            /* 2001-01-01 or after */
            n -= 1;
        }
    }

    x = n / (365*100 + 24);
    n = n % (365*100 + 24);
    y += x * 100;
    if (30*365+7+31+29-1 <= n) {
        if (n < 31*365+7) {
            y += 30;
            n -= 30*365+7;
            goto found;
        }
        else
            n += 1;
    }

    x = n / (365*4 + 1);
    n = n % (365*4 + 1);
    y += x * 4;
    if (365*2+31+29-1 <= n) {
        if (n < 365*2+366) {
            y += 2;
            n -= 365*2;
            goto found;
        }
        else
            n -= 1;
    }

    x = n / 365;
    n = n % 365;
    y += x;

  found:
    vtm->yday = n+1;
    vtm->year = add(vtm->year, INT2NUM(y));

    if (leap_year_p(y))
        yday_offset = leap_year_yday_offset;
    else
        yday_offset = common_year_yday_offset;

    for (i = 0; i < 12; i++) {
        if (yday_offset[i] < n) {
            vtm->mon = i+1;
            vtm->mday = n - yday_offset[i];
        }
        else
            break;
    }

    vtm->utc_offset = INT2FIX(0);
    vtm->zone = "UTC";
}

static struct tm *
gmtime_with_leapsecond(const time_t *timep, struct tm *result)
{
#if defined(HAVE_STRUCT_TM_TM_GMTOFF)
    /* 4.4BSD counts leap seconds only with localtime, not with gmtime. */
    struct tm *t;
    int sign;
    int gmtoff_sec, gmtoff_min, gmtoff_hour, gmtoff_day;
    long gmtoff;
    t = LOCALTIME(timep, *result);
    if (t == NULL)
        return NULL;

    /* subtract gmtoff */
    if (t->tm_gmtoff < 0) {
        sign = 1;
        gmtoff = -t->tm_gmtoff;
    }
    else {
        sign = -1;
        gmtoff = t->tm_gmtoff;
    }
    gmtoff_sec = (int)(gmtoff % 60);
    gmtoff = gmtoff / 60;
    gmtoff_min = (int)(gmtoff % 60);
    gmtoff = gmtoff / 60;
    gmtoff_hour = (int)gmtoff;	/* <= 12 */

    gmtoff_sec *= sign;
    gmtoff_min *= sign;
    gmtoff_hour *= sign;

    gmtoff_day = 0;

    if (gmtoff_sec) {
        /* If gmtoff_sec == 0, don't change result->tm_sec.
         * It may be 60 which is a leap second. */
        result->tm_sec += gmtoff_sec;
        if (result->tm_sec < 0) {
            result->tm_sec += 60;
            gmtoff_min -= 1;
        }
        if (60 <= result->tm_sec) {
            result->tm_sec -= 60;
            gmtoff_min += 1;
        }
    }
    if (gmtoff_min) {
        result->tm_min += gmtoff_min;
        if (result->tm_min < 0) {
            result->tm_min += 60;
            gmtoff_hour -= 1;
        }
        if (60 <= result->tm_min) {
            result->tm_min -= 60;
            gmtoff_hour += 1;
        }
    }
    if (gmtoff_hour) {
        result->tm_hour += gmtoff_hour;
        if (result->tm_hour < 0) {
            result->tm_hour += 24;
            gmtoff_day = -1;
        }
        if (24 <= result->tm_hour) {
            result->tm_hour -= 24;
            gmtoff_day = 1;
        }
    }

    if (gmtoff_day) {
        if (gmtoff_day < 0) {
            if (result->tm_yday == 0) {
                result->tm_mday = 31;
                result->tm_mon = 11; /* December */
                result->tm_year--;
                result->tm_yday = leap_year_p(result->tm_year + 1900) ? 365 : 364;
            }
            else if (result->tm_mday == 1) {
                const int *days_in_month = leap_year_p(result->tm_year + 1900) ?
                                           leap_year_days_in_month :
                                           common_year_days_in_month;
                result->tm_mon--;
                result->tm_mday = days_in_month[result->tm_mon];
                result->tm_yday--;
            }
            else {
                result->tm_mday--;
                result->tm_yday--;
            }
            result->tm_wday = (result->tm_wday + 6) % 7;
        }
        else {
            int leap = leap_year_p(result->tm_year + 1900);
            if (result->tm_yday == (leap ? 365 : 364)) {
                result->tm_year++;
                result->tm_mon = 0; /* January */
                result->tm_mday = 1;
                result->tm_yday = 0;
            }
            else if (result->tm_mday == (leap ? leap_year_days_in_month :
                                                common_year_days_in_month)[result->tm_mon]) {
                result->tm_mon++;
                result->tm_mday = 1;
                result->tm_yday++;
            }
            else {
                result->tm_mday++;
                result->tm_yday++;
            }
            result->tm_wday = (result->tm_wday + 1) % 7;
        }
    }
    result->tm_isdst = 0;
    result->tm_gmtoff = 0;
#if defined(HAVE_TM_ZONE)
    result->tm_zone = (char *)"UTC";
#endif
    return result;
#else
    return GMTIME(timep, *result);
#endif
}

static long this_year = 0;
static time_t known_leap_seconds_limit;
static int number_of_leap_seconds_known;

static void
init_leap_second_info()
{
    /*
     * leap seconds are determined by IERS.
     * It is announced 6 months before the leap second.
     * So no one knows leap seconds in the future after the next year.
     */
    if (this_year == 0) {
        time_t now;
        struct tm *tm, result;
        struct vtm vtm;
        wideval_t timew;
        now = time(NULL);
        gmtime(&now);
        tm = gmtime_with_leapsecond(&now, &result);
        if (!tm) return;
        this_year = tm->tm_year;

        if (TIMET_MAX - now < (time_t)(366*86400))
            known_leap_seconds_limit = TIMET_MAX;
        else
            known_leap_seconds_limit = now + (time_t)(366*86400);

        if (!gmtime_with_leapsecond(&known_leap_seconds_limit, &result))
            return;

        vtm.year = LONG2NUM(result.tm_year + 1900);
        vtm.mon = result.tm_mon + 1;
        vtm.mday = result.tm_mday;
        vtm.hour = result.tm_hour;
        vtm.min = result.tm_min;
        vtm.sec = result.tm_sec;
        vtm.subsecx = INT2FIX(0);
        vtm.utc_offset = INT2FIX(0);

        timew = timegmw_noleapsecond(&vtm);

        number_of_leap_seconds_known = NUM2INT(w2v(wsub(TIMET2WV(known_leap_seconds_limit), rb_time_unmagnify(timew))));
    }
}

static wideval_t
timegmw(struct vtm *vtm)
{
    wideval_t timew;
    struct tm tm;
    time_t t;
    const char *errmsg;

    /* The first leap second is 1972-06-30 23:59:60 UTC.
     * No leap seconds before. */
    if (gt(INT2FIX(1972), vtm->year))
        return timegmw_noleapsecond(vtm);

    init_leap_second_info();

    timew = timegmw_noleapsecond(vtm);

    if (wlt(rb_time_magnify(TIMET2WV(known_leap_seconds_limit)), timew)) {
        return wadd(timew, rb_time_magnify(WINT2WV(number_of_leap_seconds_known)));
    }

    tm.tm_year = rb_long2int(NUM2LONG(vtm->year) - 1900);
    tm.tm_mon = vtm->mon - 1;
    tm.tm_mday = vtm->mday;
    tm.tm_hour = vtm->hour;
    tm.tm_min = vtm->min;
    tm.tm_sec = vtm->sec;
    tm.tm_isdst = 0;

    errmsg = find_time_t(&tm, 1, &t);
    if (errmsg)
        rb_raise(rb_eArgError, "%s", errmsg);
    return wadd(rb_time_magnify(TIMET2WV(t)), v2w(vtm->subsecx));
}

static struct vtm *
gmtimew(wideval_t timew, struct vtm *result)
{
    time_t t;
    struct tm tm;
    VALUE subsecx;
    wideval_t timew2;

    if (wlt(timew, WINT2FIXWV(0))) {
        gmtimew_noleapsecond(timew, result);
        return result;
    }

    init_leap_second_info();

    if (wlt(rb_time_magnify(TIMET2WV(known_leap_seconds_limit)), timew)) {
        timew = wsub(timew, rb_time_magnify(WINT2WV(number_of_leap_seconds_known)));
        gmtimew_noleapsecond(timew, result);
        return result;
    }

    split_second(timew, &timew2, &subsecx);

    t = WV2TIMET(timew2);
    if (!gmtime_with_leapsecond(&t, &tm))
        return NULL;

    result->year = LONG2NUM((long)tm.tm_year + 1900);
    result->mon = tm.tm_mon + 1;
    result->mday = tm.tm_mday;
    result->hour = tm.tm_hour;
    result->min = tm.tm_min;
    result->sec = tm.tm_sec;
    result->subsecx = subsecx;
    result->utc_offset = INT2FIX(0);
    result->wday = tm.tm_wday;
    result->yday = tm.tm_yday+1;
    result->isdst = tm.tm_isdst;
    result->zone = "UTC";

    return result;
}

static struct tm *localtime_with_gmtoff_zone(const time_t *t, struct tm *result, long *gmtoff, const char **zone);

/*
 * The idea is come from Perl:
 * http://use.perl.org/articles/08/02/07/197204.shtml
 *
 * compat_common_month_table is generated by following program.
 * This table finds the last month which start the same day of a week.
 * The year 2037 is not used because
 * http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=522949
 *
 *  #!/usr/bin/ruby
 *
 *  require 'date'
 *
 *  h = {}
 *  2036.downto(2010) {|y|
 *    1.upto(12) {|m|
 *      next if m == 2 && y % 4 == 0
 *      d = Date.new(y,m,1)
 *      h[m] ||= {}
 *      h[m][d.wday] ||= y
 *    }
 *  }
 *
 *  1.upto(12) {|m|
 *    print "{"
 *    0.upto(6) {|w|
 *      y = h[m][w]
 *      print " #{y},"
 *    }
 *    puts "},"
 *  }
 *
 */
static int compat_common_month_table[12][7] = {
  /* Sun   Mon   Tue   Wed   Thu   Fri   Sat */
  { 2034, 2035, 2036, 2031, 2032, 2027, 2033 }, /* January */
  { 2026, 2027, 2033, 2034, 2035, 2030, 2031 }, /* February */
  { 2026, 2032, 2033, 2034, 2035, 2030, 2036 }, /* March */
  { 2035, 2030, 2036, 2026, 2032, 2033, 2034 }, /* April */
  { 2033, 2034, 2035, 2030, 2036, 2026, 2032 }, /* May */
  { 2036, 2026, 2032, 2033, 2034, 2035, 2030 }, /* June */
  { 2035, 2030, 2036, 2026, 2032, 2033, 2034 }, /* July */
  { 2032, 2033, 2034, 2035, 2030, 2036, 2026 }, /* August */
  { 2030, 2036, 2026, 2032, 2033, 2034, 2035 }, /* September */
  { 2034, 2035, 2030, 2036, 2026, 2032, 2033 }, /* October */
  { 2026, 2032, 2033, 2034, 2035, 2030, 2036 }, /* November */
  { 2030, 2036, 2026, 2032, 2033, 2034, 2035 }, /* December */
};

/*
 * compat_leap_month_table is generated by following program.
 *
 *  #!/usr/bin/ruby
 *
 *  require 'date'
 *
 *  h = {}
 *  2037.downto(2010) {|y|
 *    1.upto(12) {|m|
 *      next unless m == 2 && y % 4 == 0
 *      d = Date.new(y,m,1)
 *      h[m] ||= {}
 *      h[m][d.wday] ||= y
 *    }
 *  }
 *
 *  2.upto(2) {|m|
 *    0.upto(6) {|w|
 *      y = h[m][w]
 *      print " #{y},"
 *    }
 *    puts
 *  }
 */
static int compat_leap_month_table[7] = {
/* Sun   Mon   Tue   Wed   Thu   Fri   Sat */
  2032, 2016, 2028, 2012, 2024, 2036, 2020, /* February */
};

static int
calc_wday(int year, int month, int day)
{
    int a, y, m;
    int wday;

    a = (14 - month) / 12;
    y = year + 4800 - a;
    m = month + 12 * a - 3;
    wday = day + (153*m+2)/5 + 365*y + y/4 - y/100 + y/400 + 2;
    wday = wday % 7;
    return wday;
}

static VALUE
guess_local_offset(struct vtm *vtm_utc, int *isdst_ret, const char **zone_ret)
{
    struct tm tm;
    long gmtoff;
    const char *zone;
    time_t t;
    struct vtm vtm2;
    VALUE timev;
    int y, wday;

    /* The first DST is at 1916 in German.
     * So we don't need to care DST before that. */
    if (lt(vtm_utc->year, INT2FIX(1916))) {
        VALUE off = INT2FIX(0);
        int isdst = 0;
        zone = "UTC";

# if defined(NEGATIVE_TIME_T)
#  if SIZEOF_TIME_T <= 4
    /* 1901-12-13 20:45:52 UTC : The oldest time in 32-bit signed time_t. */
#   define THE_TIME_OLD_ENOUGH ((time_t)0x80000000)
#  else
    /* Since the Royal Greenwich Observatory was commissioned in 1675,
       no timezone defined using GMT at 1600. */
#   define THE_TIME_OLD_ENOUGH ((time_t)(1600-1970)*366*24*60*60)
#  endif
        if (localtime_with_gmtoff_zone((t = THE_TIME_OLD_ENOUGH, &t), &tm, &gmtoff, &zone)) {
            off = LONG2FIX(gmtoff);
            isdst = tm.tm_isdst;
        }
        else
# endif
        /* 1970-01-01 00:00:00 UTC : The Unix epoch - the oldest time in portable time_t. */
        if (localtime_with_gmtoff_zone((t = 0, &t), &tm, &gmtoff, &zone)) {
            off = LONG2FIX(gmtoff);
            isdst = tm.tm_isdst;
        }

        if (isdst_ret)
            *isdst_ret = isdst;
        if (zone_ret)
            *zone_ret = zone;
        return off;
    }

    /* It is difficult to guess future. */

    vtm2 = *vtm_utc;

    /* guess using a year before 2038. */
    y = NUM2INT(mod(vtm_utc->year, INT2FIX(400)));
    wday = calc_wday(y, vtm_utc->mon, 1);
    if (vtm_utc->mon == 2 && leap_year_p(y))
        vtm2.year = INT2FIX(compat_leap_month_table[wday]);
    else
        vtm2.year = INT2FIX(compat_common_month_table[vtm_utc->mon-1][wday]);

    timev = w2v(rb_time_unmagnify(timegmw(&vtm2)));
    t = NUM2TIMET(timev);
    zone = "UTC";
    if (localtime_with_gmtoff_zone(&t, &tm, &gmtoff, &zone)) {
        if (isdst_ret)
            *isdst_ret = tm.tm_isdst;
        if (zone_ret)
            *zone_ret = zone;
        return LONG2FIX(gmtoff);
    }

    {
        /* Use the current time offset as a last resort. */
        static time_t now = 0;
        static long now_gmtoff = 0;
        static const char *now_zone = "UTC";
        if (now == 0) {
            now = time(NULL);
            localtime_with_gmtoff_zone(&now, &tm, &now_gmtoff, &now_zone);
        }
        if (isdst_ret)
            *isdst_ret = tm.tm_isdst;
        if (zone_ret)
            *zone_ret = now_zone;
        return LONG2FIX(now_gmtoff);
    }
}

static VALUE
small_vtm_sub(struct vtm *vtm1, struct vtm *vtm2)
{
    int off;

    off = vtm1->sec - vtm2->sec;
    off += (vtm1->min - vtm2->min) * 60;
    off += (vtm1->hour - vtm2->hour) * 3600;
    if (ne(vtm1->year, vtm2->year))
        off += lt(vtm1->year, vtm2->year) ? -24*3600 : 24*3600;
    else if (vtm1->mon != vtm2->mon)
        off += vtm1->mon < vtm2->mon ? -24*3600 : 24*3600;
    else if (vtm1->mday != vtm2->mday)
        off += vtm1->mday < vtm2->mday ? -24*3600 : 24*3600;

    return INT2FIX(off);
}

static wideval_t
timelocalw(struct vtm *vtm)
{
    time_t t;
    struct tm tm;
    VALUE v;
    wideval_t timew1, timew2;
    struct vtm vtm1, vtm2;
    int n;

    if (FIXNUM_P(vtm->year)) {
        long l = FIX2LONG(vtm->year) - 1900;
        if (l < INT_MIN || INT_MAX < l)
            goto no_localtime;
        tm.tm_year = (int)l;
    }
    else {
        v = sub(vtm->year, INT2FIX(1900));
        if (lt(v, INT2NUM(INT_MIN)) || lt(INT2NUM(INT_MAX), v))
            goto no_localtime;
        tm.tm_year = NUM2INT(v);
    }

    tm.tm_mon = vtm->mon-1;
    tm.tm_mday = vtm->mday;
    tm.tm_hour = vtm->hour;
    tm.tm_min = vtm->min;
    tm.tm_sec = vtm->sec;
    tm.tm_isdst = vtm->isdst;

    if (find_time_t(&tm, 0, &t))
        goto no_localtime;
    return wadd(rb_time_magnify(TIMET2WV(t)), v2w(vtm->subsecx));

  no_localtime:
    timew1 = timegmw(vtm);

    if (!localtimew(timew1, &vtm1))
        rb_raise(rb_eArgError, "localtimew error");

    n = vtmcmp(vtm, &vtm1);
    if (n == 0) {
        timew1 = wsub(timew1, rb_time_magnify(WINT2FIXWV(12*3600)));
        if (!localtimew(timew1, &vtm1))
            rb_raise(rb_eArgError, "localtimew error");
        n = 1;
    }

    if (n < 0) {
        timew2 = timew1;
        vtm2 = vtm1;
        timew1 = wsub(timew1, rb_time_magnify(WINT2FIXWV(24*3600)));
        if (!localtimew(timew1, &vtm1))
            rb_raise(rb_eArgError, "localtimew error");
    }
    else {
        timew2 = wadd(timew1, rb_time_magnify(WINT2FIXWV(24*3600)));
        if (!localtimew(timew2, &vtm2))
            rb_raise(rb_eArgError, "localtimew error");
    }
    timew1 = wadd(timew1, rb_time_magnify(v2w(small_vtm_sub(vtm, &vtm1))));
    timew2 = wadd(timew2, rb_time_magnify(v2w(small_vtm_sub(vtm, &vtm2))));

    if (weq(timew1, timew2))
        return timew1;

    if (!localtimew(timew1, &vtm1))
        rb_raise(rb_eArgError, "localtimew error");
    if (vtm->hour != vtm1.hour || vtm->min != vtm1.min || vtm->sec != vtm1.sec)
        return timew2;

    if (!localtimew(timew2, &vtm2))
        rb_raise(rb_eArgError, "localtimew error");
    if (vtm->hour != vtm2.hour || vtm->min != vtm2.min || vtm->sec != vtm2.sec)
        return timew1;

    if (vtm->isdst)
        return lt(vtm1.utc_offset, vtm2.utc_offset) ? timew2 : timew1;
    else
        return lt(vtm1.utc_offset, vtm2.utc_offset) ? timew1 : timew2;
}

static struct tm *
localtime_with_gmtoff_zone(const time_t *t, struct tm *result, long *gmtoff, const char **zone)
{
    struct tm tm;

    if (LOCALTIME(t, tm)) {
#if defined(HAVE_STRUCT_TM_TM_GMTOFF)
	*gmtoff = tm.tm_gmtoff;
#else
	struct tm *u, *l;
	long off;
	struct tm tmbuf;
	l = &tm;
	u = GMTIME(t, tmbuf);
	if (!u)
	    return NULL;
	if (l->tm_year != u->tm_year)
	    off = l->tm_year < u->tm_year ? -1 : 1;
	else if (l->tm_mon != u->tm_mon)
	    off = l->tm_mon < u->tm_mon ? -1 : 1;
	else if (l->tm_mday != u->tm_mday)
	    off = l->tm_mday < u->tm_mday ? -1 : 1;
	else
	    off = 0;
	off = off * 24 + l->tm_hour - u->tm_hour;
	off = off * 60 + l->tm_min - u->tm_min;
	off = off * 60 + l->tm_sec - u->tm_sec;
	*gmtoff = off;
#endif

        if (zone) {
#if defined(HAVE_TM_ZONE)
            *zone = zone_str(tm.tm_zone);
#elif defined(HAVE_TZNAME) && defined(HAVE_DAYLIGHT)
            /* this needs tzset or localtime, instead of localtime_r */
            *zone = zone_str(tzname[daylight && tm.tm_isdst]);
#else
            {
                char buf[64];
                strftime(buf, sizeof(buf), "%Z", &tm);
                *zone = zone_str(buf);
            }
#endif
        }

        *result = tm;
	return result;
    }
    return NULL;
}

static int
timew_out_of_timet_range(wideval_t timew)
{
    VALUE timexv;
#if WIDEVALUE_IS_WIDER && SIZEOF_TIME_T < SIZEOF_INT64_T
    if (FIXWV_P(timew)) {
        wideint_t t = FIXWV2WINT(timew);
        if (t < TIME_SCALE * (wideint_t)TIMET_MIN ||
            TIME_SCALE * (1 + (wideint_t)TIMET_MAX) <= t)
            return 1;
        return 0;
    }
#endif
    timexv = w2v(timew);
    if (lt(timexv, mul(INT2FIX(TIME_SCALE), TIMET2NUM(TIMET_MIN))) ||
        le(mul(INT2FIX(TIME_SCALE), add(TIMET2NUM(TIMET_MAX), INT2FIX(1))), timexv))
        return 1;
    return 0;
}

static struct vtm *
localtimew(wideval_t timew, struct vtm *result)
{
    VALUE subsecx, offset;
    const char *zone;
    int isdst;

    if (!timew_out_of_timet_range(timew)) {
        time_t t;
        struct tm tm;
	long gmtoff;
        wideval_t timew2;

        split_second(timew, &timew2, &subsecx);

        t = WV2TIMET(timew2);

        if (localtime_with_gmtoff_zone(&t, &tm, &gmtoff, &zone)) {
            result->year = LONG2NUM((long)tm.tm_year + 1900);
            result->mon = tm.tm_mon + 1;
            result->mday = tm.tm_mday;
            result->hour = tm.tm_hour;
            result->min = tm.tm_min;
            result->sec = tm.tm_sec;
            result->subsecx = subsecx;
            result->wday = tm.tm_wday;
            result->yday = tm.tm_yday+1;
            result->isdst = tm.tm_isdst;
            result->utc_offset = LONG2NUM(gmtoff);
            result->zone = zone;
            return result;
        }
    }

    if (!gmtimew(timew, result))
        return NULL;

    offset = guess_local_offset(result, &isdst, &zone);

    if (!gmtimew(wadd(timew, rb_time_magnify(v2w(offset))), result))
        return NULL;

    result->utc_offset = offset;
    result->isdst = isdst;
    result->zone = zone;

    return result;
}

struct time_object {
    wideval_t timew; /* time_t value * TIME_SCALE.  possibly Rational. */
    struct vtm vtm;
    int gmt; /* 0:utc 1:localtime 2:fixoff */
    int tm_got;
};

#define GetTimeval(obj, tobj) \
    TypedData_Get_Struct((obj), struct time_object, &time_data_type, (tobj))

#define IsTimeval(obj) rb_typeddata_is_kind_of((obj), &time_data_type)

#define TIME_UTC_P(tobj) ((tobj)->gmt == 1)
#define TIME_SET_UTC(tobj) ((tobj)->gmt = 1)

#define TIME_LOCALTIME_P(tobj) ((tobj)->gmt == 0)
#define TIME_SET_LOCALTIME(tobj) ((tobj)->gmt = 0)

#define TIME_FIXOFF_P(tobj) ((tobj)->gmt == 2)
#define TIME_SET_FIXOFF(tobj, off) \
    ((tobj)->gmt = 2, \
     (tobj)->vtm.utc_offset = (off), \
     (tobj)->vtm.zone = NULL)

#define TIME_COPY_GMT(tobj1, tobj2) \
    ((tobj1)->gmt = (tobj2)->gmt, \
     (tobj1)->vtm.utc_offset = (tobj2)->vtm.utc_offset, \
     (tobj1)->vtm.zone = (tobj2)->vtm.zone)

static VALUE time_get_tm(VALUE, struct time_object *);
#define MAKE_TM(time, tobj) \
  do { \
    if ((tobj)->tm_got == 0) { \
	time_get_tm((time), (tobj)); \
    } \
  } while (0)

static void
time_mark(void *ptr)
{
    struct time_object *tobj = ptr;
    if (!tobj) return;
    if (!FIXWV_P(tobj->timew))
        rb_gc_mark(w2v(tobj->timew));
    rb_gc_mark(tobj->vtm.year);
    rb_gc_mark(tobj->vtm.subsecx);
    rb_gc_mark(tobj->vtm.utc_offset);
}

static void
time_free(void *tobj)
{
    if (tobj) xfree(tobj);
}

static size_t
time_memsize(const void *tobj)
{
    return tobj ? sizeof(struct time_object) : 0;
}

static const rb_data_type_t time_data_type = {
    "time",
    {time_mark, time_free, time_memsize,},
};

static VALUE
time_s_alloc(VALUE klass)
{
    VALUE obj;
    struct time_object *tobj;

    obj = TypedData_Make_Struct(klass, struct time_object, &time_data_type, tobj);
    tobj->tm_got=0;
    tobj->timew = WINT2FIXWV(0);

    return obj;
}

static void
time_modify(VALUE time)
{
    rb_check_frozen(time);
    rb_check_trusted(time);
}

static wideval_t
timespec2timew(struct timespec *ts)
{
    wideval_t timew;

    timew = rb_time_magnify(TIMET2WV(ts->tv_sec));
    if (ts->tv_nsec)
        timew = wadd(timew, wmulquoll(WINT2WV(ts->tv_nsec), TIME_SCALE, 1000000000));
    return timew;
}

static struct timespec
timew2timespec(wideval_t timew)
{
    VALUE subsecx;
    struct timespec ts;
    wideval_t timew2;

    if (timew_out_of_timet_range(timew))
        rb_raise(rb_eArgError, "time out of system range");
    split_second(timew, &timew2, &subsecx);
    ts.tv_sec = WV2TIMET(timew2);
    ts.tv_nsec = NUM2LONG(mulquo(subsecx, INT2FIX(1000000000), INT2FIX(TIME_SCALE)));
    return ts;
}

static struct timespec *
timew2timespec_exact(wideval_t timew, struct timespec *ts)
{
    VALUE subsecx;
    wideval_t timew2;
    VALUE nsecv;

    if (timew_out_of_timet_range(timew))
        return NULL;
    split_second(timew, &timew2, &subsecx);
    ts->tv_sec = WV2TIMET(timew2);
    nsecv = mulquo(subsecx, INT2FIX(1000000000), INT2FIX(TIME_SCALE));
    if (!FIXNUM_P(nsecv))
        return NULL;
    ts->tv_nsec = NUM2LONG(nsecv);
    return ts;
}

/*
 *  Document-method: now
 *
 *  Synonym for <code>Time.new</code>. Returns a +Time+ object
 *  initialized to the current system time.
 */

static VALUE
time_init_0(VALUE time)
{
    struct time_object *tobj;
    struct timespec ts;

    time_modify(time);
    GetTimeval(time, tobj);
    tobj->tm_got=0;
    tobj->timew = WINT2FIXWV(0);
#ifdef HAVE_CLOCK_GETTIME
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
	rb_sys_fail("clock_gettime");
    }
#else
    {
        struct timeval tv;
        if (gettimeofday(&tv, 0) < 0) {
            rb_sys_fail("gettimeofday");
        }
        ts.tv_sec = tv.tv_sec;
        ts.tv_nsec = tv.tv_usec * 1000;
    }
#endif
    tobj->timew = timespec2timew(&ts);

    return time;
}

static VALUE
time_set_utc_offset(VALUE time, VALUE off)
{
    struct time_object *tobj;
    off = num_exact(off);

    time_modify(time);
    GetTimeval(time, tobj);

    tobj->tm_got = 0;
    TIME_SET_FIXOFF(tobj, off);

    return time;
}

static void
vtm_add_offset(struct vtm *vtm, VALUE off)
{
    int sign;
    VALUE subsec, v;
    int sec, min, hour;
    int day;

    vtm->utc_offset = sub(vtm->utc_offset, off);

    if (lt(off, INT2FIX(0))) {
        sign = -1;
        off = neg(off);
    }
    else {
        sign = 1;
    }
    divmodv(off, INT2FIX(1), &off, &subsec);
    divmodv(off, INT2FIX(60), &off, &v);
    sec = NUM2INT(v);
    divmodv(off, INT2FIX(60), &off, &v);
    min = NUM2INT(v);
    divmodv(off, INT2FIX(24), &off, &v);
    hour = NUM2INT(v);

    if (sign < 0) {
        subsec = neg(subsec);
        sec = -sec;
        min = -min;
        hour = -hour;
    }

    day = 0;

    if (!rb_equal(subsec, INT2FIX(0))) {
        vtm->subsecx = add(vtm->subsecx, w2v(rb_time_magnify(v2w(subsec))));
        if (lt(vtm->subsecx, INT2FIX(0))) {
            vtm->subsecx = add(vtm->subsecx, INT2FIX(TIME_SCALE));
            sec -= 1;
        }
        if (le(INT2FIX(TIME_SCALE), vtm->subsecx)) {
            vtm->subsecx = sub(vtm->subsecx, INT2FIX(TIME_SCALE));
            sec += 1;
        }
        goto not_zero_sec;
    }
    if (sec) {
      not_zero_sec:
        /* If sec + subsec == 0, don't change vtm->sec.
         * It may be 60 which is a leap second. */
        vtm->sec += sec;
        if (vtm->sec < 0) {
            vtm->sec += 60;
            min -= 1;
        }
        if (60 <= vtm->sec) {
            vtm->sec -= 60;
            min += 1;
        }
    }
    if (min) {
        vtm->min += min;
        if (vtm->min < 0) {
            vtm->min += 60;
            hour -= 1;
        }
        if (60 <= vtm->min) {
            vtm->min -= 60;
            hour += 1;
        }
    }
    if (hour) {
        vtm->hour += hour;
        if (vtm->hour < 0) {
            vtm->hour += 24;
            day = -1;
        }
        if (24 <= vtm->hour) {
            vtm->hour -= 24;
            day = 1;
        }
    }

    if (day) {
        if (day < 0) {
            if (vtm->mon == 1 && vtm->mday == 1) {
                vtm->mday = 31;
                vtm->mon = 12; /* December */
                vtm->year = sub(vtm->year, INT2FIX(1));
                vtm->yday = leap_year_v_p(vtm->year) ? 365 : 364;
            }
            else if (vtm->mday == 1) {
                const int *days_in_month = leap_year_v_p(vtm->year) ?
                                           leap_year_days_in_month :
                                           common_year_days_in_month;
                vtm->mon--;
                vtm->mday = days_in_month[vtm->mon-1];
                vtm->yday--;
            }
            else {
                vtm->mday--;
                vtm->yday--;
            }
            vtm->wday = (vtm->wday + 6) % 7;
        }
        else {
            int leap = leap_year_v_p(vtm->year);
            if (vtm->mon == 12 && vtm->mday == 31) {
                vtm->year = add(vtm->year, INT2FIX(1));
                vtm->mon = 1; /* January */
                vtm->mday = 1;
                vtm->yday = 1;
            }
            else if (vtm->mday == (leap ? leap_year_days_in_month :
                                          common_year_days_in_month)[vtm->mon-1]) {
                vtm->mon++;
                vtm->mday = 1;
                vtm->yday++;
            }
            else {
                vtm->mday++;
                vtm->yday++;
            }
            vtm->wday = (vtm->wday + 1) % 7;
        }
    }
}

static VALUE
utc_offset_arg(VALUE arg)
{
    VALUE tmp;
    if (!NIL_P(tmp = rb_check_string_type(arg))) {
        int n;
        char *s = RSTRING_PTR(tmp);
        if (!rb_enc_str_asciicompat_p(tmp) ||
            RSTRING_LEN(tmp) != 6 ||
            (s[0] != '+' && s[0] != '-') ||
            !ISDIGIT(s[1]) ||
            !ISDIGIT(s[2]) ||
            s[3] != ':' ||
            !ISDIGIT(s[4]) ||
            !ISDIGIT(s[5]))
            rb_raise(rb_eArgError, "\"+HH:MM\" or \"-HH:MM\" expected for utc_offset");
        n = (s[1] * 10 + s[2] - '0' * 11) * 3600;
        n += (s[4] * 10 + s[5] - '0' * 11) * 60;
        if (s[0] == '-')
            n = -n;
        return INT2FIX(n);
    }
    else {
        return num_exact(arg);
    }
}

static VALUE
time_init_1(int argc, VALUE *argv, VALUE time)
{
    struct vtm vtm;
    VALUE v[7];
    struct time_object *tobj;

    vtm.wday = -1;
    vtm.yday = 0;
    vtm.zone = "";

    /*                             year  mon   mday  hour  min   sec   off */
    rb_scan_args(argc, argv, "16", &v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6]);

    vtm.year = obj2vint(v[0]);

    vtm.mon = NIL_P(v[1]) ? 1 : month_arg(v[1]);

    vtm.mday = NIL_P(v[2]) ? 1 : obj2int(v[2]);

    vtm.hour = NIL_P(v[3]) ? 0 : obj2int(v[3]);

    vtm.min  = NIL_P(v[4]) ? 0 : obj2int(v[4]);

    vtm.sec = 0;
    vtm.subsecx = INT2FIX(0);
    if (!NIL_P(v[5])) {
        VALUE sec = num_exact(v[5]);
        VALUE subsec;
        divmodv(sec, INT2FIX(1), &sec, &subsec);
        vtm.sec = NUM2INT(sec);
        vtm.subsecx = w2v(rb_time_magnify(v2w(subsec)));
    }

    vtm.isdst = -1;
    vtm.utc_offset = Qnil;
    if (!NIL_P(v[6])) {
        VALUE arg = v[6];
        if (arg == ID2SYM(rb_intern("dst")))
            vtm.isdst = 1;
        else if (arg == ID2SYM(rb_intern("std")))
            vtm.isdst = 0;
        else
            vtm.utc_offset = utc_offset_arg(arg);
    }

    validate_vtm(&vtm);

    time_modify(time);
    GetTimeval(time, tobj);
    tobj->tm_got=0;
    tobj->timew = WINT2FIXWV(0);

    if (!NIL_P(vtm.utc_offset)) {
        VALUE off = vtm.utc_offset;
        vtm_add_offset(&vtm, neg(off));
        vtm.utc_offset = Qnil;
        tobj->timew = timegmw(&vtm);
        return time_set_utc_offset(time, off);
    }
    else {
        tobj->timew = timelocalw(&vtm);
        return time_localtime(time);
    }
}


/*
 *  call-seq:
 *     Time.new -> time
 *     Time.new(year, month=nil, day=nil, hour=nil, min=nil, sec=nil, utc_offset=nil) -> time
 *
 *  Returns a <code>Time</code> object.
 *
 *  It is initialized to the current system time if no argument.
 *  <b>Note:</b> The object created will be created using the
 *  resolution available on your system clock, and so may include
 *  fractional seconds.
 *
 *  If one or more arguments specified, the time is initialized
 *  to the specified time.
 *  _sec_ may have fraction if it is a rational.
 *
 *  _utc_offset_ is the offset from UTC.
 *  It is a string such as "+09:00" or a number of seconds such as 32400.
 *
 *     a = Time.new      #=> 2007-11-19 07:50:02 -0600
 *     b = Time.new      #=> 2007-11-19 07:50:02 -0600
 *     a == b            #=> false
 *     "%.6f" % a.to_f   #=> "1195480202.282373"
 *     "%.6f" % b.to_f   #=> "1195480202.283415"
 *
 *     Time.new(2008,6,21, 13,30,0, "+09:00") #=> 2008-06-21 13:30:00 +0900
 *
 *     # A trip for RubyConf 2007
 *     t1 = Time.new(2007,11,1,15,25,0, "+09:00") # JST (Narita)
 *     t2 = Time.new(2007,11,1,12, 5,0, "-05:00") # CDT (Minneapolis)
 *     t3 = Time.new(2007,11,1,13,25,0, "-05:00") # CDT (Minneapolis)
 *     t4 = Time.new(2007,11,1,16,53,0, "-04:00") # EDT (Charlotte)
 *     t5 = Time.new(2007,11,5, 9,24,0, "-05:00") # EST (Charlotte)
 *     t6 = Time.new(2007,11,5,11,21,0, "-05:00") # EST (Detroit)
 *     t7 = Time.new(2007,11,5,13,45,0, "-05:00") # EST (Detroit)
 *     t8 = Time.new(2007,11,6,17,10,0, "+09:00") # JST (Narita)
 *     p((t2-t1)/3600.0)                          #=> 10.666666666666666
 *     p((t4-t3)/3600.0)                          #=> 2.466666666666667
 *     p((t6-t5)/3600.0)                          #=> 1.95
 *     p((t8-t7)/3600.0)                          #=> 13.416666666666666
 *
 */

static VALUE
time_init(int argc, VALUE *argv, VALUE time)
{
    if (argc == 0)
        return time_init_0(time);
    else
        return time_init_1(argc, argv, time);
}

static void
time_overflow_p(time_t *secp, long *nsecp)
{
    time_t tmp, sec = *secp;
    long nsec = *nsecp;

    if (nsec >= 1000000000) {	/* nsec positive overflow */
	tmp = sec + nsec / 1000000000;
	nsec %= 1000000000;
	if (sec > 0 && tmp < 0) {
	    rb_raise(rb_eRangeError, "out of Time range");
	}
	sec = tmp;
    }
    if (nsec < 0) {		/* nsec negative overflow */
	tmp = sec + NDIV(nsec,1000000000); /* negative div */
	nsec = NMOD(nsec,1000000000);      /* negative mod */
	if (sec < 0 && tmp > 0) {
	    rb_raise(rb_eRangeError, "out of Time range");
	}
	sec = tmp;
    }
#ifndef NEGATIVE_TIME_T
    if (sec < 0)
	rb_raise(rb_eArgError, "time must be positive");
#endif
    *secp = sec;
    *nsecp = nsec;
}

static wideval_t
nsec2timew(time_t sec, long nsec)
{
    struct timespec ts;
    time_overflow_p(&sec, &nsec);
    ts.tv_sec = sec;
    ts.tv_nsec = nsec;
    return timespec2timew(&ts);
}

static VALUE
time_new_timew(VALUE klass, wideval_t timew)
{
    VALUE time = time_s_alloc(klass);
    struct time_object *tobj;

    GetTimeval(time, tobj);
    tobj->timew = timew;

    return time;
}

VALUE
rb_time_new(time_t sec, long usec)
{
    wideval_t timew;

    if (usec >= 1000000) {
	long sec2 = usec / 1000000;
	if (sec > TIMET_MAX - sec2) {
	    rb_raise(rb_eRangeError, "out of Time range");
	}
	usec -= sec2 * 1000000;
	sec += sec2;
    }
    else if (usec <= 1000000) {
	long sec2 = usec / 1000000;
	if (sec < -TIMET_MAX - sec2) {
	    rb_raise(rb_eRangeError, "out of Time range");
	}
	usec -= sec2 * 1000000;
	sec += sec2;
    }

    timew = nsec2timew(sec, usec * 1000);
    return time_new_timew(rb_cTime, timew);
}

VALUE
rb_time_nano_new(time_t sec, long nsec)
{
    return time_new_timew(rb_cTime, nsec2timew(sec, nsec));
}

VALUE
rb_time_num_new(VALUE timev, VALUE off)
{
    VALUE time = time_new_timew(rb_cTime, rb_time_magnify(v2w(timev)));

    if (!NIL_P(off)) {
        off = utc_offset_arg(off);
        validate_utc_offset(off);
        time_set_utc_offset(time, off);
        return time;
    }

    return time;
}

static struct timespec
time_timespec(VALUE num, int interval)
{
    struct timespec t;
    const char *tstr = interval ? "time interval" : "time";
    VALUE i, f, ary;

#ifndef NEGATIVE_TIME_T
    interval = 1;
#endif

    switch (TYPE(num)) {
      case T_FIXNUM:
	t.tv_sec = NUM2TIMET(num);
	if (interval && t.tv_sec < 0)
	    rb_raise(rb_eArgError, "%s must be positive", tstr);
	t.tv_nsec = 0;
	break;

      case T_FLOAT:
	if (interval && RFLOAT_VALUE(num) < 0.0)
	    rb_raise(rb_eArgError, "%s must be positive", tstr);
	else {
	    double f, d;

	    d = modf(RFLOAT_VALUE(num), &f);
	    if (d >= 0) {
		t.tv_nsec = (int)(d*1e9+0.5);
	    }
	    else if ((t.tv_nsec = (int)(-d*1e9+0.5)) > 0) {
		t.tv_nsec = 1000000000 - t.tv_nsec;
		f -= 1;
	    }
	    t.tv_sec = (time_t)f;
	    if (f != t.tv_sec) {
		rb_raise(rb_eRangeError, "%f out of Time range", RFLOAT_VALUE(num));
	    }
	}
	break;

      case T_BIGNUM:
	t.tv_sec = NUM2TIMET(num);
	if (interval && t.tv_sec < 0)
	    rb_raise(rb_eArgError, "%s must be positive", tstr);
	t.tv_nsec = 0;
	break;

      default:
	i = INT2FIX(1);
	ary = rb_check_funcall(num, id_divmod, 1, &i);
	if (ary != Qundef && !NIL_P(ary = rb_check_array_type(ary))) {
            i = rb_ary_entry(ary, 0);
            f = rb_ary_entry(ary, 1);
            t.tv_sec = NUM2TIMET(i);
            if (interval && t.tv_sec < 0)
                rb_raise(rb_eArgError, "%s must be positive", tstr);
            f = rb_funcall(f, id_mul, 1, INT2FIX(1000000000));
            t.tv_nsec = NUM2LONG(f);
        }
        else {
            rb_raise(rb_eTypeError, "can't convert %s into %s",
                     rb_obj_classname(num), tstr);
        }
	break;
    }
    return t;
}

static struct timeval
time_timeval(VALUE num, int interval)
{
    struct timespec ts;
    struct timeval tv;

    ts = time_timespec(num, interval);
    tv.tv_sec = (TYPEOF_TIMEVAL_TV_SEC)ts.tv_sec;
    tv.tv_usec = (TYPEOF_TIMEVAL_TV_USEC)(ts.tv_nsec / 1000);

    return tv;
}

struct timeval
rb_time_interval(VALUE num)
{
    return time_timeval(num, TRUE);
}

struct timeval
rb_time_timeval(VALUE time)
{
    struct time_object *tobj;
    struct timeval t;
    struct timespec ts;

    if (IsTimeval(time)) {
	GetTimeval(time, tobj);
        ts = timew2timespec(tobj->timew);
        t.tv_sec = (TYPEOF_TIMEVAL_TV_SEC)ts.tv_sec;
        t.tv_usec = (TYPEOF_TIMEVAL_TV_USEC)(ts.tv_nsec / 1000);
	return t;
    }
    return time_timeval(time, FALSE);
}

struct timespec
rb_time_timespec(VALUE time)
{
    struct time_object *tobj;
    struct timespec t;

    if (IsTimeval(time)) {
	GetTimeval(time, tobj);
        t = timew2timespec(tobj->timew);
	return t;
    }
    return time_timespec(time, FALSE);
}

/*
 *  call-seq:
 *     Time.now -> time
 *
 *  Creates a new time object for the current time.
 *
 *     Time.now            #=> 2009-06-24 12:39:54 +0900
 */

static VALUE
time_s_now(VALUE klass)
{
    return rb_class_new_instance(0, NULL, klass);
}

/*
 *  call-seq:
 *     Time.at(time) -> time
 *     Time.at(seconds_with_frac) -> time
 *     Time.at(seconds, microseconds_with_frac) -> time
 *
 *  Creates a new time object with the value given by <i>time</i>,
 *  the given number of <i>seconds_with_frac</i>, or
 *  <i>seconds</i> and <i>microseconds_with_frac</i> from the Epoch.
 *  <i>seconds_with_frac</i> and <i>microseconds_with_frac</i>
 *  can be Integer, Float, Rational, or other Numeric.
 *  non-portable feature allows the offset to be negative on some systems.
 *
 *  If a numeric argument is given, the result is in local time.
 *
 *     Time.at(0)            #=> 1969-12-31 18:00:00 -0600
 *     Time.at(Time.at(0))   #=> 1969-12-31 18:00:00 -0600
 *     Time.at(946702800)    #=> 1999-12-31 23:00:00 -0600
 *     Time.at(-284061600)   #=> 1960-12-31 00:00:00 -0600
 *     Time.at(946684800.2).usec #=> 200000
 *     Time.at(946684800, 123456.789).nsec #=> 123456789
 */

static VALUE
time_s_at(int argc, VALUE *argv, VALUE klass)
{
    VALUE time, t;
    wideval_t timew;

    if (rb_scan_args(argc, argv, "11", &time, &t) == 2) {
        time = num_exact(time);
        t = num_exact(t);
        timew = wadd(rb_time_magnify(v2w(time)), wmulquoll(v2w(t), TIME_SCALE, 1000000));
        t = time_new_timew(klass, timew);
    }
    else if (IsTimeval(time)) {
	struct time_object *tobj, *tobj2;
        GetTimeval(time, tobj);
        t = time_new_timew(klass, tobj->timew);
	GetTimeval(t, tobj2);
        TIME_COPY_GMT(tobj2, tobj);
    }
    else {
        timew = rb_time_magnify(v2w(num_exact(time)));
        t = time_new_timew(klass, timew);
    }

    return t;
}

static const char months[][4] = {
    "jan", "feb", "mar", "apr", "may", "jun",
    "jul", "aug", "sep", "oct", "nov", "dec",
};

static int
obj2int(VALUE obj)
{
    if (RB_TYPE_P(obj, T_STRING)) {
	obj = rb_str_to_inum(obj, 10, FALSE);
    }

    return NUM2INT(obj);
}

static VALUE
obj2vint(VALUE obj)
{
    if (RB_TYPE_P(obj, T_STRING)) {
	obj = rb_str_to_inum(obj, 10, FALSE);
    }
    else {
        obj = rb_to_int(obj);
    }

    return obj;
}

static int
obj2subsecx(VALUE obj, VALUE *subsecx)
{
    VALUE subsec;

    if (RB_TYPE_P(obj, T_STRING)) {
	obj = rb_str_to_inum(obj, 10, FALSE);
        *subsecx = INT2FIX(0);
        return NUM2INT(obj);
    }

    divmodv(num_exact(obj), INT2FIX(1), &obj, &subsec);
    *subsecx = w2v(rb_time_magnify(v2w(subsec)));
    return NUM2INT(obj);
}

static long
usec2subsecx(VALUE obj)
{
    if (RB_TYPE_P(obj, T_STRING)) {
	obj = rb_str_to_inum(obj, 10, FALSE);
    }

    return mulquo(num_exact(obj), INT2FIX(TIME_SCALE), INT2FIX(1000000));
}

static int
month_arg(VALUE arg)
{
    int i, mon;

    VALUE s = rb_check_string_type(arg);
    if (!NIL_P(s)) {
        mon = 0;
        for (i=0; i<12; i++) {
            if (RSTRING_LEN(s) == 3 &&
                STRCASECMP(months[i], RSTRING_PTR(s)) == 0) {
                mon = i+1;
                break;
            }
        }
        if (mon == 0) {
            char c = RSTRING_PTR(s)[0];

            if ('0' <= c && c <= '9') {
                mon = obj2int(s);
            }
        }
    }
    else {
        mon = obj2int(arg);
    }
    return mon;
}

static void
validate_utc_offset(VALUE utc_offset)
{
    if (le(utc_offset, INT2FIX(-86400)) || ge(utc_offset, INT2FIX(86400)))
	rb_raise(rb_eArgError, "utc_offset out of range");
}

static void
validate_vtm(struct vtm *vtm)
{
    if (   vtm->mon  < 1 || vtm->mon  > 12
	|| vtm->mday < 1 || vtm->mday > 31
	|| vtm->hour < 0 || vtm->hour > 24
	|| (vtm->hour == 24 && (vtm->min > 0 || vtm->sec > 0))
	|| vtm->min  < 0 || vtm->min  > 59
	|| vtm->sec  < 0 || vtm->sec  > 60
        || lt(vtm->subsecx, INT2FIX(0)) || ge(vtm->subsecx, INT2FIX(TIME_SCALE))
        || (!NIL_P(vtm->utc_offset) && (validate_utc_offset(vtm->utc_offset), 0)))
	rb_raise(rb_eArgError, "argument out of range");
}

static void
time_arg(int argc, VALUE *argv, struct vtm *vtm)
{
    VALUE v[8];

    vtm->year = INT2FIX(0);
    vtm->mon = 0;
    vtm->mday = 0;
    vtm->hour = 0;
    vtm->min = 0;
    vtm->sec = 0;
    vtm->subsecx = INT2FIX(0);
    vtm->utc_offset = Qnil;
    vtm->wday = 0;
    vtm->yday = 0;
    vtm->isdst = 0;
    vtm->zone = "";

    if (argc == 10) {
	v[0] = argv[5];
	v[1] = argv[4];
	v[2] = argv[3];
	v[3] = argv[2];
	v[4] = argv[1];
	v[5] = argv[0];
	v[6] = Qnil;
	vtm->isdst = RTEST(argv[8]) ? 1 : 0;
    }
    else {
	rb_scan_args(argc, argv, "17", &v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7]);
	/* v[6] may be usec or zone (parsedate) */
	/* v[7] is wday (parsedate; ignored) */
	vtm->wday = -1;
	vtm->isdst = -1;
    }

    vtm->year = obj2vint(v[0]);

    if (NIL_P(v[1])) {
        vtm->mon = 1;
    }
    else {
        vtm->mon = month_arg(v[1]);
    }

    if (NIL_P(v[2])) {
	vtm->mday = 1;
    }
    else {
	vtm->mday = obj2int(v[2]);
    }

    vtm->hour = NIL_P(v[3])?0:obj2int(v[3]);

    vtm->min  = NIL_P(v[4])?0:obj2int(v[4]);

    if (!NIL_P(v[6]) && argc == 7) {
        vtm->sec  = NIL_P(v[5])?0:obj2int(v[5]);
        vtm->subsecx  = usec2subsecx(v[6]);
    }
    else {
	/* when argc == 8, v[6] is timezone, but ignored */
        vtm->sec  = NIL_P(v[5])?0:obj2subsecx(v[5], &vtm->subsecx);
    }

    validate_vtm(vtm);
}

static int
leap_year_p(long y)
{
    return ((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0);
}

static time_t
timegm_noleapsecond(struct tm *tm)
{
    long tm_year = tm->tm_year;
    int tm_yday = tm->tm_mday;
    if (leap_year_p(tm_year + 1900))
	tm_yday += leap_year_yday_offset[tm->tm_mon];
    else
	tm_yday += common_year_yday_offset[tm->tm_mon];

    /*
     *  `Seconds Since the Epoch' in SUSv3:
     *  tm_sec + tm_min*60 + tm_hour*3600 + tm_yday*86400 +
     *  (tm_year-70)*31536000 + ((tm_year-69)/4)*86400 -
     *  ((tm_year-1)/100)*86400 + ((tm_year+299)/400)*86400
     */
    return tm->tm_sec + tm->tm_min*60 + tm->tm_hour*3600 +
	   (time_t)(tm_yday +
		    (tm_year-70)*365 +
		    DIV(tm_year-69,4) -
		    DIV(tm_year-1,100) +
		    DIV(tm_year+299,400))*86400;
}

#if 0
#define DEBUG_FIND_TIME_NUMGUESS
#define DEBUG_GUESSRANGE
#endif

#ifdef DEBUG_GUESSRANGE
#define DEBUG_REPORT_GUESSRANGE fprintf(stderr, "find time guess range: %ld - %ld : %lu\n", guess_lo, guess_hi, (unsigned_time_t)(guess_hi-guess_lo))
#else
#define DEBUG_REPORT_GUESSRANGE
#endif

#ifdef DEBUG_FIND_TIME_NUMGUESS
#define DEBUG_FIND_TIME_NUMGUESS_INC find_time_numguess++,
static unsigned long long find_time_numguess;

static VALUE find_time_numguess_getter(void)
{
    return ULL2NUM(find_time_numguess);
}
#else
#define DEBUG_FIND_TIME_NUMGUESS_INC
#endif

static const char *
find_time_t(struct tm *tptr, int utc_p, time_t *tp)
{
    time_t guess, guess0, guess_lo, guess_hi;
    struct tm *tm, tm0, tm_lo, tm_hi;
    int d;
    int find_dst;
    struct tm result;
    int status;
    int tptr_tm_yday;

#define GUESS(p) (DEBUG_FIND_TIME_NUMGUESS_INC (utc_p ? gmtime_with_leapsecond((p), &result) : LOCALTIME((p), result)))

    guess_lo = TIMET_MIN;
    guess_hi = TIMET_MAX;

    find_dst = 0 < tptr->tm_isdst;

#if defined(HAVE_MKTIME)
    tm0 = *tptr;
    if (!utc_p && (guess = mktime(&tm0)) != -1) {
        tm = GUESS(&guess);
        if (tm && tmcmp(tptr, tm) == 0) {
            goto found;
        }
    }
#endif

    tm0 = *tptr;
    if (tm0.tm_mon < 0) {
      tm0.tm_mon = 0;
      tm0.tm_mday = 1;
      tm0.tm_hour = 0;
      tm0.tm_min = 0;
      tm0.tm_sec = 0;
    }
    else if (11 < tm0.tm_mon) {
      tm0.tm_mon = 11;
      tm0.tm_mday = 31;
      tm0.tm_hour = 23;
      tm0.tm_min = 59;
      tm0.tm_sec = 60;
    }
    else if (tm0.tm_mday < 1) {
      tm0.tm_mday = 1;
      tm0.tm_hour = 0;
      tm0.tm_min = 0;
      tm0.tm_sec = 0;
    }
    else if ((d = (leap_year_p(1900 + tm0.tm_year) ?
                   leap_year_days_in_month :
		   common_year_days_in_month)[tm0.tm_mon]) < tm0.tm_mday) {
      tm0.tm_mday = d;
      tm0.tm_hour = 23;
      tm0.tm_min = 59;
      tm0.tm_sec = 60;
    }
    else if (tm0.tm_hour < 0) {
      tm0.tm_hour = 0;
      tm0.tm_min = 0;
      tm0.tm_sec = 0;
    }
    else if (23 < tm0.tm_hour) {
      tm0.tm_hour = 23;
      tm0.tm_min = 59;
      tm0.tm_sec = 60;
    }
    else if (tm0.tm_min < 0) {
      tm0.tm_min = 0;
      tm0.tm_sec = 0;
    }
    else if (59 < tm0.tm_min) {
      tm0.tm_min = 59;
      tm0.tm_sec = 60;
    }
    else if (tm0.tm_sec < 0) {
      tm0.tm_sec = 0;
    }
    else if (60 < tm0.tm_sec) {
      tm0.tm_sec = 60;
    }

    DEBUG_REPORT_GUESSRANGE;
    guess0 = guess = timegm_noleapsecond(&tm0);
    tm = GUESS(&guess);
    if (tm) {
	d = tmcmp(tptr, tm);
	if (d == 0) { goto found; }
	if (d < 0) {
	    guess_hi = guess;
	    guess -= 24 * 60 * 60;
	}
	else {
	    guess_lo = guess;
	    guess += 24 * 60 * 60;
	}
        DEBUG_REPORT_GUESSRANGE;
	if (guess_lo < guess && guess < guess_hi && (tm = GUESS(&guess)) != NULL) {
	    d = tmcmp(tptr, tm);
	    if (d == 0) { goto found; }
	    if (d < 0)
		guess_hi = guess;
	    else
		guess_lo = guess;
            DEBUG_REPORT_GUESSRANGE;
	}
    }

    tm = GUESS(&guess_lo);
    if (!tm) goto error;
    d = tmcmp(tptr, tm);
    if (d < 0) goto out_of_range;
    if (d == 0) { guess = guess_lo; goto found; }
    tm_lo = *tm;

    tm = GUESS(&guess_hi);
    if (!tm) goto error;
    d = tmcmp(tptr, tm);
    if (d > 0) goto out_of_range;
    if (d == 0) { guess = guess_hi; goto found; }
    tm_hi = *tm;

    DEBUG_REPORT_GUESSRANGE;

    status = 1;

    while (guess_lo + 1 < guess_hi) {
        if (status == 0) {
          binsearch:
            guess = guess_lo / 2 + guess_hi / 2;
            if (guess <= guess_lo)
                guess = guess_lo + 1;
            else if (guess >= guess_hi)
                guess = guess_hi - 1;
            status = 1;
        }
        else {
            if (status == 1) {
                time_t guess0_hi = timegm_noleapsecond(&tm_hi);
                guess = guess_hi - (guess0_hi - guess0);
                if (guess == guess_hi) /* hh:mm:60 tends to cause this condition. */
                    guess--;
                status = 2;
            }
            else if (status == 2) {
                time_t guess0_lo = timegm_noleapsecond(&tm_lo);
                guess = guess_lo + (guess0 - guess0_lo);
                if (guess == guess_lo)
                    guess++;
                status = 0;
            }
            if (guess <= guess_lo || guess_hi <= guess) {
                /* Precious guess is invalid. try binary search. */
#ifdef DEBUG_GUESSRANGE
                if (guess <= guess_lo) fprintf(stderr, "too small guess: %ld <= %ld\n", guess, guess_lo);
                if (guess_hi <= guess) fprintf(stderr, "too big guess: %ld <= %ld\n", guess_hi, guess);
#endif
                goto binsearch;
            }
        }

	tm = GUESS(&guess);
	if (!tm) goto error;

	d = tmcmp(tptr, tm);

        if (d < 0) {
            guess_hi = guess;
            tm_hi = *tm;
            DEBUG_REPORT_GUESSRANGE;
        }
        else if (d > 0) {
            guess_lo = guess;
            tm_lo = *tm;
            DEBUG_REPORT_GUESSRANGE;
        }
        else {
          found:
	    if (!utc_p) {
		/* If localtime is nonmonotonic, another result may exist. */
		time_t guess2;
		if (find_dst) {
		    guess2 = guess - 2 * 60 * 60;
		    tm = LOCALTIME(&guess2, result);
		    if (tm) {
			if (tptr->tm_hour != (tm->tm_hour + 2) % 24 ||
			    tptr->tm_min != tm->tm_min ||
			    tptr->tm_sec != tm->tm_sec) {
			    guess2 -= (tm->tm_hour - tptr->tm_hour) * 60 * 60 +
				      (tm->tm_min - tptr->tm_min) * 60 +
				      (tm->tm_sec - tptr->tm_sec);
			    if (tptr->tm_mday != tm->tm_mday)
				guess2 += 24 * 60 * 60;
			    if (guess != guess2) {
				tm = LOCALTIME(&guess2, result);
				if (tm && tmcmp(tptr, tm) == 0) {
				    if (guess < guess2)
					*tp = guess;
				    else
					*tp = guess2;
                                    return NULL;
				}
			    }
			}
		    }
		}
		else {
		    guess2 = guess + 2 * 60 * 60;
		    tm = LOCALTIME(&guess2, result);
		    if (tm) {
			if ((tptr->tm_hour + 2) % 24 != tm->tm_hour ||
			    tptr->tm_min != tm->tm_min ||
			    tptr->tm_sec != tm->tm_sec) {
			    guess2 -= (tm->tm_hour - tptr->tm_hour) * 60 * 60 +
				      (tm->tm_min - tptr->tm_min) * 60 +
				      (tm->tm_sec - tptr->tm_sec);
			    if (tptr->tm_mday != tm->tm_mday)
				guess2 -= 24 * 60 * 60;
			    if (guess != guess2) {
				tm = LOCALTIME(&guess2, result);
				if (tm && tmcmp(tptr, tm) == 0) {
				    if (guess < guess2)
					*tp = guess2;
				    else
					*tp = guess;
                                    return NULL;
				}
			    }
			}
		    }
		}
	    }
            *tp = guess;
            return NULL;
	}
    }

    /* Given argument has no corresponding time_t. Let's outerpolation. */
    /*
     *  `Seconds Since the Epoch' in SUSv3:
     *  tm_sec + tm_min*60 + tm_hour*3600 + tm_yday*86400 +
     *  (tm_year-70)*31536000 + ((tm_year-69)/4)*86400 -
     *  ((tm_year-1)/100)*86400 + ((tm_year+299)/400)*86400
     */

    tptr_tm_yday = calc_tm_yday(tptr->tm_year, tptr->tm_mon, tptr->tm_mday);

    *tp = guess_lo +
          ((tptr->tm_year - tm_lo.tm_year) * 365 +
           ((tptr->tm_year-69)/4) -
           ((tptr->tm_year-1)/100) +
           ((tptr->tm_year+299)/400) -
           ((tm_lo.tm_year-69)/4) +
           ((tm_lo.tm_year-1)/100) -
           ((tm_lo.tm_year+299)/400) +
           tptr_tm_yday -
           tm_lo.tm_yday) * 86400 +
          (tptr->tm_hour - tm_lo.tm_hour) * 3600 +
          (tptr->tm_min - tm_lo.tm_min) * 60 +
          (tptr->tm_sec - (tm_lo.tm_sec == 60 ? 59 : tm_lo.tm_sec));

    return NULL;

  out_of_range:
    return "time out of range";

  error:
    return "gmtime/localtime error";
}

static int
vtmcmp(struct vtm *a, struct vtm *b)
{
    if (ne(a->year, b->year))
	return lt(a->year, b->year) ? -1 : 1;
    else if (a->mon != b->mon)
	return a->mon < b->mon ? -1 : 1;
    else if (a->mday != b->mday)
	return a->mday < b->mday ? -1 : 1;
    else if (a->hour != b->hour)
	return a->hour < b->hour ? -1 : 1;
    else if (a->min != b->min)
	return a->min < b->min ? -1 : 1;
    else if (a->sec != b->sec)
	return a->sec < b->sec ? -1 : 1;
    else if (ne(a->subsecx, b->subsecx))
	return lt(a->subsecx, b->subsecx) ? -1 : 1;
    else
        return 0;
}

static int
tmcmp(struct tm *a, struct tm *b)
{
    if (a->tm_year != b->tm_year)
	return a->tm_year < b->tm_year ? -1 : 1;
    else if (a->tm_mon != b->tm_mon)
	return a->tm_mon < b->tm_mon ? -1 : 1;
    else if (a->tm_mday != b->tm_mday)
	return a->tm_mday < b->tm_mday ? -1 : 1;
    else if (a->tm_hour != b->tm_hour)
	return a->tm_hour < b->tm_hour ? -1 : 1;
    else if (a->tm_min != b->tm_min)
	return a->tm_min < b->tm_min ? -1 : 1;
    else if (a->tm_sec != b->tm_sec)
	return a->tm_sec < b->tm_sec ? -1 : 1;
    else
        return 0;
}

static VALUE
time_utc_or_local(int argc, VALUE *argv, int utc_p, VALUE klass)
{
    struct vtm vtm;
    VALUE time;

    time_arg(argc, argv, &vtm);
    if (utc_p)
        time = time_new_timew(klass, timegmw(&vtm));
    else
        time = time_new_timew(klass, timelocalw(&vtm));
    if (utc_p) return time_gmtime(time);
    return time_localtime(time);
}

/*
 *  call-seq:
 *    Time.utc(year) -> time
 *    Time.utc(year, month) -> time
 *    Time.utc(year, month, day) -> time
 *    Time.utc(year, month, day, hour) -> time
 *    Time.utc(year, month, day, hour, min) -> time
 *    Time.utc(year, month, day, hour, min, sec_with_frac) -> time
 *    Time.utc(year, month, day, hour, min, sec, usec_with_frac) -> time
 *    Time.utc(sec, min, hour, day, month, year, wday, yday, isdst, tz) -> time
 *    Time.gm(year) -> time
 *    Time.gm(year, month) -> time
 *    Time.gm(year, month, day) -> time
 *    Time.gm(year, month, day, hour) -> time
 *    Time.gm(year, month, day, hour, min) -> time
 *    Time.gm(year, month, day, hour, min, sec_with_frac) -> time
 *    Time.gm(year, month, day, hour, min, sec, usec_with_frac) -> time
 *    Time.gm(sec, min, hour, day, month, year, wday, yday, isdst, tz) -> time
 *
 *  Creates a time based on given values, interpreted as UTC (GMT). The
 *  year must be specified. Other values default to the minimum value
 *  for that field (and may be <code>nil</code> or omitted). Months may
 *  be specified by numbers from 1 to 12, or by the three-letter English
 *  month names. Hours are specified on a 24-hour clock (0..23). Raises
 *  an <code>ArgumentError</code> if any values are out of range. Will
 *  also accept ten arguments in the order output by
 *  <code>Time#to_a</code>.
 *  <i>sec_with_frac</i> and <i>usec_with_frac</i> can have a fractional part.
 *
 *     Time.utc(2000,"jan",1,20,15,1)  #=> 2000-01-01 20:15:01 UTC
 *     Time.gm(2000,"jan",1,20,15,1)   #=> 2000-01-01 20:15:01 UTC
 */
static VALUE
time_s_mkutc(int argc, VALUE *argv, VALUE klass)
{
    return time_utc_or_local(argc, argv, TRUE, klass);
}

/*
 *  call-seq:
 *   Time.local(year) -> time
 *   Time.local(year, month) -> time
 *   Time.local(year, month, day) -> time
 *   Time.local(year, month, day, hour) -> time
 *   Time.local(year, month, day, hour, min) -> time
 *   Time.local(year, month, day, hour, min, sec_with_frac) -> time
 *   Time.local(year, month, day, hour, min, sec, usec_with_frac) -> time
 *   Time.local(sec, min, hour, day, month, year, wday, yday, isdst, tz) -> time
 *   Time.mktime(year) -> time
 *   Time.mktime(year, month) -> time
 *   Time.mktime(year, month, day) -> time
 *   Time.mktime(year, month, day, hour) -> time
 *   Time.mktime(year, month, day, hour, min) -> time
 *   Time.mktime(year, month, day, hour, min, sec_with_frac) -> time
 *   Time.mktime(year, month, day, hour, min, sec, usec_with_frac) -> time
 *   Time.mktime(sec, min, hour, day, month, year, wday, yday, isdst, tz) -> time
 *
 *  Same as <code>Time::gm</code>, but interprets the values in the
 *  local time zone.
 *
 *     Time.local(2000,"jan",1,20,15,1)   #=> 2000-01-01 20:15:01 -0600
 */

static VALUE
time_s_mktime(int argc, VALUE *argv, VALUE klass)
{
    return time_utc_or_local(argc, argv, FALSE, klass);
}

/*
 *  call-seq:
 *     time.to_i   -> int
 *     time.tv_sec -> int
 *
 *  Returns the value of <i>time</i> as an integer number of seconds
 *  since the Epoch.
 *
 *     t = Time.now
 *     "%10.5f" % t.to_f   #=> "1270968656.89607"
 *     t.to_i              #=> 1270968656
 */

static VALUE
time_to_i(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    return w2v(wdiv(tobj->timew, WINT2FIXWV(TIME_SCALE)));
}

/*
 *  call-seq:
 *     time.to_f -> float
 *
 *  Returns the value of <i>time</i> as a floating point number of
 *  seconds since the Epoch.
 *
 *     t = Time.now
 *     "%10.5f" % t.to_f   #=> "1270968744.77658"
 *     t.to_i              #=> 1270968744
 *
 *  Note that IEEE 754 double is not accurate enough to represent
 *  number of nanoseconds from the Epoch.
 */

static VALUE
time_to_f(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    return rb_Float(rb_time_unmagnify_to_float(tobj->timew));
}

/*
 *  call-seq:
 *     time.to_r -> a_rational
 *
 *  Returns the value of <i>time</i> as a rational number of seconds
 *  since the Epoch.
 *
 *     t = Time.now
 *     p t.to_r            #=> (1270968792716287611/1000000000)
 *
 *  This methods is intended to be used to get an accurate value
 *  representing nanoseconds from the Epoch.  You can use this
 *  to convert time to another Epoch.
 */

static VALUE
time_to_r(VALUE time)
{
    struct time_object *tobj;
    VALUE v;

    GetTimeval(time, tobj);
    v = w2v(rb_time_unmagnify(tobj->timew));
    if (!RB_TYPE_P(v, T_RATIONAL)) {
        v = rb_Rational1(v);
    }
    return v;
}

/*
 *  call-seq:
 *     time.usec    -> int
 *     time.tv_usec -> int
 *
 *  Returns just the number of microseconds for <i>time</i>.
 *
 *     t = Time.now        #=> 2007-11-19 08:03:26 -0600
 *     "%10.6f" % t.to_f   #=> "1195481006.775195"
 *     t.usec              #=> 775195
 */

static VALUE
time_usec(VALUE time)
{
    struct time_object *tobj;
    wideval_t w, q, r;

    GetTimeval(time, tobj);

    w = wmod(tobj->timew, WINT2WV(TIME_SCALE));
    wmuldivmod(w, WINT2FIXWV(1000000), WINT2FIXWV(TIME_SCALE), &q, &r);
    return rb_to_int(w2v(q));
}

/*
 *  call-seq:
 *     time.nsec    -> int
 *     time.tv_nsec -> int
 *
 *  Returns just the number of nanoseconds for <i>time</i>.
 *
 *     t = Time.now        #=> 2007-11-17 15:18:03 +0900
 *     "%10.9f" % t.to_f   #=> "1195280283.536151409"
 *     t.nsec              #=> 536151406
 *
 *  The lowest digit of to_f and nsec is different because
 *  IEEE 754 double is not accurate enough to represent
 *  nanoseconds from the Epoch.
 *  The accurate value is returned by nsec.
 */

static VALUE
time_nsec(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    return rb_to_int(w2v(wmulquoll(wmod(tobj->timew, WINT2WV(TIME_SCALE)), 1000000000, TIME_SCALE)));
}

/*
 *  call-seq:
 *     time.subsec    -> number
 *
 *  Returns just the fraction for <i>time</i>.
 *
 *  The result is possibly rational.
 *
 *     t = Time.now        #=> 2009-03-26 22:33:12 +0900
 *     "%10.9f" % t.to_f   #=> "1238074392.940563917"
 *     t.subsec            #=> (94056401/100000000)
 *
 *  The lowest digit of to_f and subsec is different because
 *  IEEE 754 double is not accurate enough to represent
 *  the rational.
 *  The accurate value is returned by subsec.
 */

static VALUE
time_subsec(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    return quo(w2v(wmod(tobj->timew, WINT2FIXWV(TIME_SCALE))), INT2FIX(TIME_SCALE));
}

/*
 *  call-seq:
 *     time <=> other_time -> -1, 0, +1 or nil
 *
 *  Comparison---Compares <i>time</i> with <i>other_time</i>.
 *
 *     t = Time.now       #=> 2007-11-19 08:12:12 -0600
 *     t2 = t + 2592000   #=> 2007-12-19 08:12:12 -0600
 *     t <=> t2           #=> -1
 *     t2 <=> t           #=> 1
 *
 *     t = Time.now       #=> 2007-11-19 08:13:38 -0600
 *     t2 = t + 0.1       #=> 2007-11-19 08:13:38 -0600
 *     t.nsec             #=> 98222999
 *     t2.nsec            #=> 198222999
 *     t <=> t2           #=> -1
 *     t2 <=> t           #=> 1
 *     t <=> t            #=> 0
 */

static VALUE
time_cmp(VALUE time1, VALUE time2)
{
    struct time_object *tobj1, *tobj2;
    int n;

    GetTimeval(time1, tobj1);
    if (IsTimeval(time2)) {
	GetTimeval(time2, tobj2);
	n = wcmp(tobj1->timew, tobj2->timew);
    }
    else {
	VALUE tmp;

	tmp = rb_funcall(time2, rb_intern("<=>"), 1, time1);
	if (NIL_P(tmp)) return Qnil;

	n = -rb_cmpint(tmp, time1, time2);
    }
    if (n == 0) return INT2FIX(0);
    if (n > 0) return INT2FIX(1);
    return INT2FIX(-1);
}

/*
 * call-seq:
 *  time.eql?(other_time)
 *
 * Return <code>true</code> if <i>time</i> and <i>other_time</i> are
 * both <code>Time</code> objects with the same seconds and fractional
 * seconds.
 */

static VALUE
time_eql(VALUE time1, VALUE time2)
{
    struct time_object *tobj1, *tobj2;

    GetTimeval(time1, tobj1);
    if (IsTimeval(time2)) {
	GetTimeval(time2, tobj2);
        return rb_equal(w2v(tobj1->timew), w2v(tobj2->timew));
    }
    return Qfalse;
}

/*
 *  call-seq:
 *     time.utc? -> true or false
 *     time.gmt? -> true or false
 *
 *  Returns <code>true</code> if <i>time</i> represents a time in UTC
 *  (GMT).
 *
 *     t = Time.now                        #=> 2007-11-19 08:15:23 -0600
 *     t.utc?                              #=> false
 *     t = Time.gm(2000,"jan",1,20,15,1)   #=> 2000-01-01 20:15:01 UTC
 *     t.utc?                              #=> true
 *
 *     t = Time.now                        #=> 2007-11-19 08:16:03 -0600
 *     t.gmt?                              #=> false
 *     t = Time.gm(2000,1,1,20,15,1)       #=> 2000-01-01 20:15:01 UTC
 *     t.gmt?                              #=> true
 */

static VALUE
time_utc_p(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    if (TIME_UTC_P(tobj)) return Qtrue;
    return Qfalse;
}

/*
 * call-seq:
 *   time.hash   -> fixnum
 *
 * Return a hash code for this time object.
 */

static VALUE
time_hash(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    return rb_hash(w2v(tobj->timew));
}

/* :nodoc: */
static VALUE
time_init_copy(VALUE copy, VALUE time)
{
    struct time_object *tobj, *tcopy;

    if (copy == time) return copy;
    time_modify(copy);
    GetTimeval(time, tobj);
    GetTimeval(copy, tcopy);
    MEMCPY(tcopy, tobj, struct time_object, 1);

    return copy;
}

static VALUE
time_dup(VALUE time)
{
    VALUE dup = time_s_alloc(rb_obj_class(time));
    time_init_copy(dup, time);
    return dup;
}

static VALUE
time_localtime(VALUE time)
{
    struct time_object *tobj;
    struct vtm vtm;

    GetTimeval(time, tobj);
    if (TIME_LOCALTIME_P(tobj)) {
	if (tobj->tm_got)
	    return time;
    }
    else {
	time_modify(time);
    }

    if (!localtimew(tobj->timew, &vtm))
	rb_raise(rb_eArgError, "localtime error");
    tobj->vtm = vtm;

    tobj->tm_got = 1;
    TIME_SET_LOCALTIME(tobj);
    return time;
}

/*
 *  call-seq:
 *     time.localtime -> time
 *     time.localtime(utc_offset) -> time
 *
 *  Converts <i>time</i> to local time (using the local time zone in
 *  effect for this process) modifying the receiver.
 *
 *  If _utc_offset_ is given, it is used instead of the local time.
 *
 *     t = Time.utc(2000, "jan", 1, 20, 15, 1) #=> 2000-01-01 20:15:01 UTC
 *     t.utc?                                  #=> true
 *
 *     t.localtime                             #=> 2000-01-01 14:15:01 -0600
 *     t.utc?                                  #=> false
 *
 *     t.localtime("+09:00")                   #=> 2000-01-02 05:15:01 +0900
 *     t.utc?                                  #=> false
 */

static VALUE
time_localtime_m(int argc, VALUE *argv, VALUE time)
{
    VALUE off;
    rb_scan_args(argc, argv, "01", &off);

    if (!NIL_P(off)) {
        off = utc_offset_arg(off);
        validate_utc_offset(off);

        time_set_utc_offset(time, off);
        return time_fixoff(time);
    }

    return time_localtime(time);
}

/*
 *  call-seq:
 *     time.gmtime    -> time
 *     time.utc       -> time
 *
 *  Converts <i>time</i> to UTC (GMT), modifying the receiver.
 *
 *     t = Time.now   #=> 2007-11-19 08:18:31 -0600
 *     t.gmt?         #=> false
 *     t.gmtime       #=> 2007-11-19 14:18:31 UTC
 *     t.gmt?         #=> true
 *
 *     t = Time.now   #=> 2007-11-19 08:18:51 -0600
 *     t.utc?         #=> false
 *     t.utc          #=> 2007-11-19 14:18:51 UTC
 *     t.utc?         #=> true
 */

static VALUE
time_gmtime(VALUE time)
{
    struct time_object *tobj;
    struct vtm vtm;

    GetTimeval(time, tobj);
    if (TIME_UTC_P(tobj)) {
	if (tobj->tm_got)
	    return time;
    }
    else {
	time_modify(time);
    }

    if (!gmtimew(tobj->timew, &vtm))
	rb_raise(rb_eArgError, "gmtime error");
    tobj->vtm = vtm;

    tobj->tm_got = 1;
    TIME_SET_UTC(tobj);
    return time;
}

static VALUE
time_fixoff(VALUE time)
{
    struct time_object *tobj;
    struct vtm vtm;
    VALUE off;

    GetTimeval(time, tobj);
    if (TIME_FIXOFF_P(tobj)) {
       if (tobj->tm_got)
           return time;
    }
    else {
       time_modify(time);
    }

    if (TIME_FIXOFF_P(tobj))
        off = tobj->vtm.utc_offset;
    else
        off = INT2FIX(0);

    if (!gmtimew(tobj->timew, &vtm))
       rb_raise(rb_eArgError, "gmtime error");

    tobj->vtm = vtm;
    vtm_add_offset(&tobj->vtm, off);

    tobj->tm_got = 1;
    TIME_SET_FIXOFF(tobj, off);
    return time;
}

/*
 *  call-seq:
 *     time.getlocal -> new_time
 *     time.getlocal(utc_offset) -> new_time
 *
 *  Returns a new <code>new_time</code> object representing <i>time</i> in
 *  local time (using the local time zone in effect for this process).
 *
 *  If _utc_offset_ is given, it is used instead of the local time.
 *
 *     t = Time.utc(2000,1,1,20,15,1)  #=> 2000-01-01 20:15:01 UTC
 *     t.utc?                          #=> true
 *
 *     l = t.getlocal                  #=> 2000-01-01 14:15:01 -0600
 *     l.utc?                          #=> false
 *     t == l                          #=> true
 *
 *     j = t.getlocal("+09:00")        #=> 2000-01-02 05:15:01 +0900
 *     j.utc?                          #=> false
 *     t == j                          #=> true
 */

static VALUE
time_getlocaltime(int argc, VALUE *argv, VALUE time)
{
    VALUE off;
    rb_scan_args(argc, argv, "01", &off);

    if (!NIL_P(off)) {
        off = utc_offset_arg(off);
        validate_utc_offset(off);

        time = time_dup(time);
        time_set_utc_offset(time, off);
        return time_fixoff(time);
    }

    return time_localtime(time_dup(time));
}

/*
 *  call-seq:
 *     time.getgm  -> new_time
 *     time.getutc -> new_time
 *
 *  Returns a new <code>new_time</code> object representing <i>time</i> in
 *  UTC.
 *
 *     t = Time.local(2000,1,1,20,15,1)   #=> 2000-01-01 20:15:01 -0600
 *     t.gmt?                             #=> false
 *     y = t.getgm                        #=> 2000-01-02 02:15:01 UTC
 *     y.gmt?                             #=> true
 *     t == y                             #=> true
 */

static VALUE
time_getgmtime(VALUE time)
{
    return time_gmtime(time_dup(time));
}

static VALUE
time_get_tm(VALUE time, struct time_object *tobj)
{
    if (TIME_UTC_P(tobj)) return time_gmtime(time);
    if (TIME_FIXOFF_P(tobj)) return time_fixoff(time);
    return time_localtime(time);
}

static VALUE strftimev(const char *fmt, VALUE time, rb_encoding *enc);

/*
 *  call-seq:
 *     time.asctime -> string
 *     time.ctime   -> string
 *
 *  Returns a canonical string representation of <i>time</i>.
 *
 *     Time.now.asctime   #=> "Wed Apr  9 08:56:03 2003"
 */

static VALUE
time_asctime(VALUE time)
{
    return strftimev("%a %b %e %T %Y", time, rb_usascii_encoding());
}

/*
 *  call-seq:
 *     time.inspect -> string
 *     time.to_s    -> string
 *
 *  Returns a string representing <i>time</i>. Equivalent to calling
 *  <code>Time#strftime</code> with a format string of
 *  ``<code>%Y-%m-%d</code> <code>%H:%M:%S</code> <code>%z</code>''
 *  for a local time and
 *  ``<code>%Y-%m-%d</code> <code>%H:%M:%S</code> <code>UTC</code>''
 *  for a UTC time.
 *
 *     Time.now.to_s       #=> "2007-10-05 16:09:51 +0900"
 *     Time.now.utc.to_s   #=> "2007-10-05 07:09:51 UTC"
 */

static VALUE
time_to_s(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    if (TIME_UTC_P(tobj))
        return strftimev("%Y-%m-%d %H:%M:%S UTC", time, rb_usascii_encoding());
    else
        return strftimev("%Y-%m-%d %H:%M:%S %z", time, rb_usascii_encoding());
}

static VALUE
time_add(struct time_object *tobj, VALUE offset, int sign)
{
    VALUE result;
    offset = num_exact(offset);
    if (sign < 0)
        result = time_new_timew(rb_cTime, wsub(tobj->timew, rb_time_magnify(v2w(offset))));
    else
        result = time_new_timew(rb_cTime, wadd(tobj->timew, rb_time_magnify(v2w(offset))));
    if (TIME_UTC_P(tobj)) {
	GetTimeval(result, tobj);
        TIME_SET_UTC(tobj);
    }
    else if (TIME_FIXOFF_P(tobj)) {
        VALUE off = tobj->vtm.utc_offset;
        GetTimeval(result, tobj);
        TIME_SET_FIXOFF(tobj, off);
    }
    return result;
}

/*
 *  call-seq:
 *     time + numeric -> time
 *
 *  Addition---Adds some number of seconds (possibly fractional) to
 *  <i>time</i> and returns that value as a new time.
 *
 *     t = Time.now         #=> 2007-11-19 08:22:21 -0600
 *     t + (60 * 60 * 24)   #=> 2007-11-20 08:22:21 -0600
 */

static VALUE
time_plus(VALUE time1, VALUE time2)
{
    struct time_object *tobj;
    GetTimeval(time1, tobj);

    if (IsTimeval(time2)) {
	rb_raise(rb_eTypeError, "time + time?");
    }
    return time_add(tobj, time2, 1);
}

/*
 *  call-seq:
 *     time - other_time -> float
 *     time - numeric    -> time
 *
 *  Difference---Returns a new time that represents the difference
 *  between two times, or subtracts the given number of seconds in
 *  <i>numeric</i> from <i>time</i>.
 *
 *     t = Time.now       #=> 2007-11-19 08:23:10 -0600
 *     t2 = t + 2592000   #=> 2007-12-19 08:23:10 -0600
 *     t2 - t             #=> 2592000.0
 *     t2 - 2592000       #=> 2007-11-19 08:23:10 -0600
 */

static VALUE
time_minus(VALUE time1, VALUE time2)
{
    struct time_object *tobj;

    GetTimeval(time1, tobj);
    if (IsTimeval(time2)) {
	struct time_object *tobj2;

	GetTimeval(time2, tobj2);
        return rb_Float(rb_time_unmagnify_to_float(wsub(tobj->timew, tobj2->timew)));
    }
    return time_add(tobj, time2, -1);
}

/*
 * call-seq:
 *   time.succ   -> new_time
 *
 * Return a new time object, one second later than <code>time</code>.
 * Time#succ is obsolete since 1.9.2 for time is not a discrete value.
 *
 *     t = Time.now       #=> 2007-11-19 08:23:57 -0600
 *     t.succ             #=> 2007-11-19 08:23:58 -0600
 */

VALUE
rb_time_succ(VALUE time)
{
    struct time_object *tobj;
    struct time_object *tobj2;

    rb_warn("Time#succ is obsolete; use time + 1");
    GetTimeval(time, tobj);
    time = time_new_timew(rb_cTime, wadd(tobj->timew, WINT2FIXWV(TIME_SCALE)));
    GetTimeval(time, tobj2);
    TIME_COPY_GMT(tobj2, tobj);
    return time;
}

#define time_succ rb_time_succ

/*
 * call-seq:
 *   time.round([ndigits])   -> new_time
 *
 * Rounds sub seconds to a given precision in decimal digits (0 digits by default).
 * It returns a new time object.
 * _ndigits_ should be zero or positive integer.
 *
 *     require 'time'
 *
 *     t = Time.utc(2010,3,30, 5,43,"25.123456789".to_r)
 *     p t.iso8601(10)           #=> "2010-03-30T05:43:25.1234567890Z"
 *     p t.round.iso8601(10)     #=> "2010-03-30T05:43:25.0000000000Z"
 *     p t.round(0).iso8601(10)  #=> "2010-03-30T05:43:25.0000000000Z"
 *     p t.round(1).iso8601(10)  #=> "2010-03-30T05:43:25.1000000000Z"
 *     p t.round(2).iso8601(10)  #=> "2010-03-30T05:43:25.1200000000Z"
 *     p t.round(3).iso8601(10)  #=> "2010-03-30T05:43:25.1230000000Z"
 *     p t.round(4).iso8601(10)  #=> "2010-03-30T05:43:25.1235000000Z"
 *     p t.round(5).iso8601(10)  #=> "2010-03-30T05:43:25.1234600000Z"
 *     p t.round(6).iso8601(10)  #=> "2010-03-30T05:43:25.1234570000Z"
 *     p t.round(7).iso8601(10)  #=> "2010-03-30T05:43:25.1234568000Z"
 *     p t.round(8).iso8601(10)  #=> "2010-03-30T05:43:25.1234567900Z"
 *     p t.round(9).iso8601(10)  #=> "2010-03-30T05:43:25.1234567890Z"
 *     p t.round(10).iso8601(10) #=> "2010-03-30T05:43:25.1234567890Z"
 *
 *     t = Time.utc(1999,12,31, 23,59,59)
 *     p((t + 0.4).round.iso8601(3))    #=> "1999-12-31T23:59:59.000Z"
 *     p((t + 0.49).round.iso8601(3))   #=> "1999-12-31T23:59:59.000Z"
 *     p((t + 0.5).round.iso8601(3))    #=> "2000-01-01T00:00:00.000Z"
 *     p((t + 1.4).round.iso8601(3))    #=> "2000-01-01T00:00:00.000Z"
 *     p((t + 1.49).round.iso8601(3))   #=> "2000-01-01T00:00:00.000Z"
 *     p((t + 1.5).round.iso8601(3))    #=> "2000-01-01T00:00:01.000Z"
 *
 *     t = Time.utc(1999,12,31, 23,59,59)
 *     p (t + 0.123456789).round(4).iso8601(6)  #=> "1999-12-31T23:59:59.123500Z"
 */

static VALUE
time_round(int argc, VALUE *argv, VALUE time)
{
    VALUE ndigits, v, a, b, den;
    long nd;
    struct time_object *tobj;

    rb_scan_args(argc, argv, "01", &ndigits);

    if (NIL_P(ndigits))
        ndigits = INT2FIX(0);
    else
        ndigits = rb_to_int(ndigits);

    nd = NUM2LONG(ndigits);
    if (nd < 0)
	rb_raise(rb_eArgError, "negative ndigits given");

    GetTimeval(time, tobj);
    v = w2v(rb_time_unmagnify(tobj->timew));

    a = INT2FIX(1);
    b = INT2FIX(10);
    while (0 < nd) {
        if (nd & 1)
            a = mul(a, b);
        b = mul(b, b);
        nd = nd >> 1;
    }
    den = quo(INT2FIX(1), a);
    v = mod(v, den);
    if (lt(v, quo(den, INT2FIX(2))))
        return time_add(tobj, v, -1);
    else
        return time_add(tobj, sub(den, v), 1);
}

/*
 *  call-seq:
 *     time.sec -> fixnum
 *
 *  Returns the second of the minute (0..60)<em>[Yes, seconds really can
 *  range from zero to 60. This allows the system to inject leap seconds
 *  every now and then to correct for the fact that years are not really
 *  a convenient number of hours long.]</em> for <i>time</i>.
 *
 *     t = Time.now   #=> 2007-11-19 08:25:02 -0600
 *     t.sec          #=> 2
 */

static VALUE
time_sec(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    return INT2FIX(tobj->vtm.sec);
}

/*
 *  call-seq:
 *     time.min -> fixnum
 *
 *  Returns the minute of the hour (0..59) for <i>time</i>.
 *
 *     t = Time.now   #=> 2007-11-19 08:25:51 -0600
 *     t.min          #=> 25
 */

static VALUE
time_min(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    return INT2FIX(tobj->vtm.min);
}

/*
 *  call-seq:
 *     time.hour -> fixnum
 *
 *  Returns the hour of the day (0..23) for <i>time</i>.
 *
 *     t = Time.now   #=> 2007-11-19 08:26:20 -0600
 *     t.hour         #=> 8
 */

static VALUE
time_hour(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    return INT2FIX(tobj->vtm.hour);
}

/*
 *  call-seq:
 *     time.day  -> fixnum
 *     time.mday -> fixnum
 *
 *  Returns the day of the month (1..n) for <i>time</i>.
 *
 *     t = Time.now   #=> 2007-11-19 08:27:03 -0600
 *     t.day          #=> 19
 *     t.mday         #=> 19
 */

static VALUE
time_mday(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    return INT2FIX(tobj->vtm.mday);
}

/*
 *  call-seq:
 *     time.mon   -> fixnum
 *     time.month -> fixnum
 *
 *  Returns the month of the year (1..12) for <i>time</i>.
 *
 *     t = Time.now   #=> 2007-11-19 08:27:30 -0600
 *     t.mon          #=> 11
 *     t.month        #=> 11
 */

static VALUE
time_mon(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    return INT2FIX(tobj->vtm.mon);
}

/*
 *  call-seq:
 *     time.year -> fixnum
 *
 *  Returns the year for <i>time</i> (including the century).
 *
 *     t = Time.now   #=> 2007-11-19 08:27:51 -0600
 *     t.year         #=> 2007
 */

static VALUE
time_year(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    return tobj->vtm.year;
}

/*
 *  call-seq:
 *     time.wday -> fixnum
 *
 *  Returns an integer representing the day of the week, 0..6, with
 *  Sunday == 0.
 *
 *     t = Time.now   #=> 2007-11-20 02:35:35 -0600
 *     t.wday         #=> 2
 *     t.sunday?      #=> false
 *     t.monday?      #=> false
 *     t.tuesday?     #=> true
 *     t.wednesday?   #=> false
 *     t.thursday?    #=> false
 *     t.friday?      #=> false
 *     t.saturday?    #=> false
 */

static VALUE
time_wday(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    return INT2FIX(tobj->vtm.wday);
}

#define wday_p(n) {\
    struct time_object *tobj;\
    GetTimeval(time, tobj);\
    MAKE_TM(time, tobj);\
    return (tobj->vtm.wday == (n)) ? Qtrue : Qfalse;\
}

/*
 *  call-seq:
 *     time.sunday? -> true or false
 *
 *  Returns <code>true</code> if <i>time</i> represents Sunday.
 *
 *     t = Time.local(1990, 4, 1)       #=> 1990-04-01 00:00:00 -0600
 *     t.sunday?                        #=> true
 */

static VALUE
time_sunday(VALUE time)
{
    wday_p(0);
}

/*
 *  call-seq:
 *     time.monday? -> true or false
 *
 *  Returns <code>true</code> if <i>time</i> represents Monday.
 *
 *     t = Time.local(2003, 8, 4)       #=> 2003-08-04 00:00:00 -0500
 *     p t.monday?                      #=> true
 */

static VALUE
time_monday(VALUE time)
{
    wday_p(1);
}

/*
 *  call-seq:
 *     time.tuesday? -> true or false
 *
 *  Returns <code>true</code> if <i>time</i> represents Tuesday.
 *
 *     t = Time.local(1991, 2, 19)      #=> 1991-02-19 00:00:00 -0600
 *     p t.tuesday?                     #=> true
 */

static VALUE
time_tuesday(VALUE time)
{
    wday_p(2);
}

/*
 *  call-seq:
 *     time.wednesday? -> true or false
 *
 *  Returns <code>true</code> if <i>time</i> represents Wednesday.
 *
 *     t = Time.local(1993, 2, 24)      #=> 1993-02-24 00:00:00 -0600
 *     p t.wednesday?                   #=> true
 */

static VALUE
time_wednesday(VALUE time)
{
    wday_p(3);
}

/*
 *  call-seq:
 *     time.thursday? -> true or false
 *
 *  Returns <code>true</code> if <i>time</i> represents Thursday.
 *
 *     t = Time.local(1995, 12, 21)     #=> 1995-12-21 00:00:00 -0600
 *     p t.thursday?                    #=> true
 */

static VALUE
time_thursday(VALUE time)
{
    wday_p(4);
}

/*
 *  call-seq:
 *     time.friday? -> true or false
 *
 *  Returns <code>true</code> if <i>time</i> represents Friday.
 *
 *     t = Time.local(1987, 12, 18)     #=> 1987-12-18 00:00:00 -0600
 *     t.friday?                        #=> true
 */

static VALUE
time_friday(VALUE time)
{
    wday_p(5);
}

/*
 *  call-seq:
 *     time.saturday? -> true or false
 *
 *  Returns <code>true</code> if <i>time</i> represents Saturday.
 *
 *     t = Time.local(2006, 6, 10)      #=> 2006-06-10 00:00:00 -0500
 *     t.saturday?                      #=> true
 */

static VALUE
time_saturday(VALUE time)
{
    wday_p(6);
}

/*
 *  call-seq:
 *     time.yday -> fixnum
 *
 *  Returns an integer representing the day of the year, 1..366.
 *
 *     t = Time.now   #=> 2007-11-19 08:32:31 -0600
 *     t.yday         #=> 323
 */

static VALUE
time_yday(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    return INT2FIX(tobj->vtm.yday);
}

/*
 *  call-seq:
 *     time.isdst -> true or false
 *     time.dst?  -> true or false
 *
 *  Returns <code>true</code> if <i>time</i> occurs during Daylight
 *  Saving Time in its time zone.
 *
 *   # CST6CDT:
 *     Time.local(2000, 1, 1).zone    #=> "CST"
 *     Time.local(2000, 1, 1).isdst   #=> false
 *     Time.local(2000, 1, 1).dst?    #=> false
 *     Time.local(2000, 7, 1).zone    #=> "CDT"
 *     Time.local(2000, 7, 1).isdst   #=> true
 *     Time.local(2000, 7, 1).dst?    #=> true
 *
 *   # Asia/Tokyo:
 *     Time.local(2000, 1, 1).zone    #=> "JST"
 *     Time.local(2000, 1, 1).isdst   #=> false
 *     Time.local(2000, 1, 1).dst?    #=> false
 *     Time.local(2000, 7, 1).zone    #=> "JST"
 *     Time.local(2000, 7, 1).isdst   #=> false
 *     Time.local(2000, 7, 1).dst?    #=> false
 */

static VALUE
time_isdst(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    return tobj->vtm.isdst ? Qtrue : Qfalse;
}

/*
 *  call-seq:
 *     time.zone -> string
 *
 *  Returns the name of the time zone used for <i>time</i>. As of Ruby
 *  1.8, returns ``UTC'' rather than ``GMT'' for UTC times.
 *
 *     t = Time.gm(2000, "jan", 1, 20, 15, 1)
 *     t.zone   #=> "UTC"
 *     t = Time.local(2000, "jan", 1, 20, 15, 1)
 *     t.zone   #=> "CST"
 */

static VALUE
time_zone(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);

    if (TIME_UTC_P(tobj)) {
	return rb_obj_untaint(rb_locale_str_new_cstr("UTC"));
    }
    if (tobj->vtm.zone == NULL)
        return Qnil;
    return rb_obj_untaint(rb_locale_str_new_cstr(tobj->vtm.zone));
}

/*
 *  call-seq:
 *     time.gmt_offset -> fixnum
 *     time.gmtoff     -> fixnum
 *     time.utc_offset -> fixnum
 *
 *  Returns the offset in seconds between the timezone of <i>time</i>
 *  and UTC.
 *
 *     t = Time.gm(2000,1,1,20,15,1)   #=> 2000-01-01 20:15:01 UTC
 *     t.gmt_offset                    #=> 0
 *     l = t.getlocal                  #=> 2000-01-01 14:15:01 -0600
 *     l.gmt_offset                    #=> -21600
 */

static VALUE
time_utc_offset(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);

    if (TIME_UTC_P(tobj)) {
	return INT2FIX(0);
    }
    else {
	return tobj->vtm.utc_offset;
    }
}

/*
 *  call-seq:
 *     time.to_a -> array
 *
 *  Returns a ten-element <i>array</i> of values for <i>time</i>:
 *  {<code>[ sec, min, hour, day, month, year, wday, yday, isdst, zone
 *  ]</code>}. See the individual methods for an explanation of the
 *  valid ranges of each value. The ten elements can be passed directly
 *  to <code>Time::utc</code> or <code>Time::local</code> to create a
 *  new <code>Time</code>.
 *
 *     t = Time.now     #=> 2007-11-19 08:36:01 -0600
 *     now = t.to_a     #=> [1, 36, 8, 19, 11, 2007, 1, 323, false, "CST"]
 */

static VALUE
time_to_a(VALUE time)
{
    struct time_object *tobj;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    return rb_ary_new3(10,
		    INT2FIX(tobj->vtm.sec),
		    INT2FIX(tobj->vtm.min),
		    INT2FIX(tobj->vtm.hour),
		    INT2FIX(tobj->vtm.mday),
		    INT2FIX(tobj->vtm.mon),
		    tobj->vtm.year,
		    INT2FIX(tobj->vtm.wday),
		    INT2FIX(tobj->vtm.yday),
		    tobj->vtm.isdst?Qtrue:Qfalse,
		    time_zone(time));
}

#define SMALLBUF 100
static size_t
rb_strftime_alloc(char **buf, VALUE formatv, const char *format, rb_encoding *enc,
                  struct vtm *vtm, wideval_t timew, int gmt)
{
    size_t size, len, flen;
    VALUE timev = Qnil;
    struct timespec ts;

    if (!timew2timespec_exact(timew, &ts))
        timev = w2v(rb_time_unmagnify(timew));

    (*buf)[0] = '\0';
    flen = strlen(format);
    if (flen == 0) {
	return 0;
    }
    errno = 0;
    if (timev == Qnil)
        len = rb_strftime_timespec(*buf, SMALLBUF, format, enc, vtm, &ts, gmt);
    else
        len = rb_strftime(*buf, SMALLBUF, format, enc, vtm, timev, gmt);
    if (len != 0 || (**buf == '\0' && errno != ERANGE)) return len;
    for (size=1024; ; size*=2) {
	*buf = xmalloc(size);
	(*buf)[0] = '\0';
        if (timev == Qnil)
            len = rb_strftime_timespec(*buf, size, format, enc, vtm, &ts, gmt);
        else
            len = rb_strftime(*buf, size, format, enc, vtm, timev, gmt);
	/*
	 * buflen can be zero EITHER because there's not enough
	 * room in the string, or because the control command
	 * goes to the empty string. Make a reasonable guess that
	 * if the buffer is 1024 times bigger than the length of the
	 * format string, it's not failing for lack of room.
	 */
	if (len > 0) break;
	xfree(*buf);
	if (size >= 1024 * flen) {
	    if (!NIL_P(formatv)) rb_sys_fail_str(formatv);
	    rb_sys_fail(format);
	    break;
	}
    }
    return len;
}

static VALUE
strftimev(const char *fmt, VALUE time, rb_encoding *enc)
{
    struct time_object *tobj;
    char buffer[SMALLBUF], *buf = buffer;
    long len;
    VALUE str;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    len = rb_strftime_alloc(&buf, Qnil, fmt, enc, &tobj->vtm, tobj->timew, TIME_UTC_P(tobj));
    str = rb_enc_str_new(buf, len, enc);
    if (buf != buffer) xfree(buf);
    return str;
}

/*
 *  call-seq:
 *     time.strftime( string ) -> string
 *
 *  Formats <i>time</i> according to the directives in the given format
 *  string.
 *  The directives begins with a percent (%) character.
 *  Any text not listed as a directive will be passed through to the
 *  output string.
 *
 *  The directive consists of a percent (%) character,
 *  zero or more flags, optional minimum field width,
 *  optional modifier and a conversion specifier
 *  as follows.
 *
 *    %<flags><width><modifier><conversion>
 *
 *  Flags:
 *    -  don't pad a numerical output.
 *    _  use spaces for padding.
 *    0  use zeros for padding.
 *    ^  upcase the result string.
 *    #  change case.
 *    :  use colons for %z.
 *
 *  The minimum field width specifies the minimum width.
 *
 *  The modifier is "E" and "O".
 *  They are ignored.
 *
 *  Format directives:
 *
 *    Date (Year, Month, Day):
 *      %Y - Year with century (can be negative, 4 digits at least)
 *              -0001, 0000, 1995, 2009, 14292, etc.
 *      %C - year / 100 (round down.  20 in 2009)
 *      %y - year % 100 (00..99)
 *
 *      %m - Month of the year, zero-padded (01..12)
 *              %_m  blank-padded ( 1..12)
 *              %-m  no-padded (1..12)
 *      %B - The full month name (``January'')
 *              %^B  uppercased (``JANUARY'')
 *      %b - The abbreviated month name (``Jan'')
 *              %^b  uppercased (``JAN'')
 *      %h - Equivalent to %b
 *
 *      %d - Day of the month, zero-padded (01..31)
 *              %-d  no-padded (1..31)
 *      %e - Day of the month, blank-padded ( 1..31)
 *
 *      %j - Day of the year (001..366)
 *
 *    Time (Hour, Minute, Second, Subsecond):
 *      %H - Hour of the day, 24-hour clock, zero-padded (00..23)
 *      %k - Hour of the day, 24-hour clock, blank-padded ( 0..23)
 *      %I - Hour of the day, 12-hour clock, zero-padded (01..12)
 *      %l - Hour of the day, 12-hour clock, blank-padded ( 1..12)
 *      %P - Meridian indicator, lowercase (``am'' or ``pm'')
 *      %p - Meridian indicator, uppercase (``AM'' or ``PM'')
 *
 *      %M - Minute of the hour (00..59)
 *
 *      %S - Second of the minute (00..60)
 *
 *      %L - Millisecond of the second (000..999)
 *      %N - Fractional seconds digits, default is 9 digits (nanosecond)
 *              %3N  milli second (3 digits)
 *              %6N  micro second (6 digits)
 *              %9N  nano second (9 digits)
 *              %12N pico second (12 digits)
 *              %15N femto second (15 digits)
 *              %18N atto second (18 digits)
 *              %21N zepto second (21 digits)
 *              %24N yocto second (24 digits)
 *
 *    Time zone:
 *      %z - Time zone as hour and minute offset from UTC (e.g. +0900)
 *              %:z - hour and minute offset from UTC with a colon (e.g. +09:00)
 *              %::z - hour, minute and second offset from UTC (e.g. +09:00:00)
 *      %Z - Time zone abbreviation name
 *
 *    Weekday:
 *      %A - The full weekday name (``Sunday'')
 *              %^A  uppercased (``SUNDAY'')
 *      %a - The abbreviated name (``Sun'')
 *              %^a  uppercased (``SUN'')
 *      %u - Day of the week (Monday is 1, 1..7)
 *      %w - Day of the week (Sunday is 0, 0..6)
 *
 *    ISO 8601 week-based year and week number:
 *    The week 1 of YYYY starts with a Monday and includes YYYY-01-04.
 *    The days in the year before the first week are in the last week of
 *    the previous year.
 *      %G - The week-based year
 *      %g - The last 2 digits of the week-based year (00..99)
 *      %V - Week number of the week-based year (01..53)
 *
 *    Week number:
 *    The week 1 of YYYY starts with a Sunday or Monday (according to %U
 *    or %W).  The days in the year before the first week are in week 0.
 *      %U - Week number of the year.  The week starts with Sunday.  (00..53)
 *      %W - Week number of the year.  The week starts with Monday.  (00..53)
 *
 *    Seconds since the Epoch:
 *      %s - Number of seconds since 1970-01-01 00:00:00 UTC.
 *
 *    Literal string:
 *      %n - Newline character (\n)
 *      %t - Tab character (\t)
 *      %% - Literal ``%'' character
 *
 *    Combination:
 *      %c - date and time (%a %b %e %T %Y)
 *      %D - Date (%m/%d/%y)
 *      %F - The ISO 8601 date format (%Y-%m-%d)
 *      %v - VMS date (%e-%^b-%4Y)
 *      %x - Same as %D
 *      %X - Same as %T
 *      %r - 12-hour time (%I:%M:%S %p)
 *      %R - 24-hour time (%H:%M)
 *      %T - 24-hour time (%H:%M:%S)
 *
 *  This method is similar to strftime() function defined in ISO C and POSIX.
 *  Several directives (%a, %A, %b, %B, %c, %p, %r, %x, %X, %E*, %O* and %Z)
 *  are locale dependent in the function.
 *  However this method is locale independent since Ruby 1.9.
 *  (%Z is platform dependent, though.)
 *  So, the result may differ even if a same format string is used in other
 *  systems such as C.
 *  It is good practice to avoid %x and %X because there are corresponding
 *  locale independent representations, %D and %T.
 *
 *  %z is recommended over %Z.
 *  %Z doesn't identify the timezone.
 *  For example, "CST" is used at America/Chicago (-06:00),
 *  America/Havana (-05:00), Asia/Harbin (+08:00), Australia/Darwin (+09:30)
 *  and Australia/Adelaide (+10:30).
 *  Also, %Z is highly dependent for OS.
 *  For example, it may generate a non ASCII string on Japanese Windows.
 *  i.e. the result can be different to "JST".
 *  So the numeric time zone offset, %z, is recommended.
 *
 *  Examples:
 *
 *    t = Time.new(2007,11,19,8,37,48,"-06:00") #=> 2007-11-19 08:37:48 -0600
 *    t.strftime("Printed on %m/%d/%Y")   #=> "Printed on 11/19/2007"
 *    t.strftime("at %I:%M%p")            #=> "at 08:37AM"
 *
 *  Various ISO 8601 formats:
 *    %Y%m%d           => 20071119                  Calendar date (basic)
 *    %F               => 2007-11-19                Calendar date (extended)
 *    %Y-%m            => 2007-11                   Calendar date, reduced accuracy, specific month
 *    %Y               => 2007                      Calendar date, reduced accuracy, specific year
 *    %C               => 20                        Calendar date, reduced accuracy, specific century
 *    %Y%j             => 2007323                   Ordinal date (basic)
 *    %Y-%j            => 2007-323                  Ordinal date (extended)
 *    %GW%V%u          => 2007W471                  Week date (basic)
 *    %G-W%V-%u        => 2007-W47-1                Week date (extended)
 *    %GW%V            => 2007W47                   Week date, reduced accuracy, specific week (basic)
 *    %G-W%V           => 2007-W47                  Week date, reduced accuracy, specific week (extended)
 *    %H%M%S           => 083748                    Local time (basic)
 *    %T               => 08:37:48                  Local time (extended)
 *    %H%M             => 0837                      Local time, reduced accuracy, specific minute (basic)
 *    %H:%M            => 08:37                     Local time, reduced accuracy, specific minute (extended)
 *    %H               => 08                        Local time, reduced accuracy, specific hour
 *    %H%M%S,%L        => 083748,000                Local time with decimal fraction, comma as decimal sign (basic)
 *    %T,%L            => 08:37:48,000              Local time with decimal fraction, comma as decimal sign (extended)
 *    %H%M%S.%L        => 083748.000                Local time with decimal fraction, full stop as decimal sign (basic)
 *    %T.%L            => 08:37:48.000              Local time with decimal fraction, full stop as decimal sign (extended)
 *    %H%M%S%z         => 083748-0600               Local time and the difference from UTC (basic)
 *    %T%:z            => 08:37:48-06:00            Local time and the difference from UTC (extended)
 *    %Y%m%dT%H%M%S%z  => 20071119T083748-0600      Date and time of day for calendar date (basic)
 *    %FT%T%:z         => 2007-11-19T08:37:48-06:00 Date and time of day for calendar date (extended)
 *    %Y%jT%H%M%S%z    => 2007323T083748-0600       Date and time of day for ordinal date (basic)
 *    %Y-%jT%T%:z      => 2007-323T08:37:48-06:00   Date and time of day for ordinal date (extended)
 *    %GW%V%uT%H%M%S%z => 2007W471T083748-0600      Date and time of day for week date (basic)
 *    %G-W%V-%uT%T%:z  => 2007-W47-1T08:37:48-06:00 Date and time of day for week date (extended)
 *    %Y%m%dT%H%M      => 20071119T0837             Calendar date and local time (basic)
 *    %FT%R            => 2007-11-19T08:37          Calendar date and local time (extended)
 *    %Y%jT%H%MZ       => 2007323T0837Z             Ordinal date and UTC of day (basic)
 *    %Y-%jT%RZ        => 2007-323T08:37Z           Ordinal date and UTC of day (extended)
 *    %GW%V%uT%H%M%z   => 2007W471T0837-0600        Week date and local time and difference from UTC (basic)
 *    %G-W%V-%uT%R%:z  => 2007-W47-1T08:37-06:00    Week date and local time and difference from UTC (extended)
 *
 */

static VALUE
time_strftime(VALUE time, VALUE format)
{
    struct time_object *tobj;
    char buffer[SMALLBUF], *buf = buffer;
    const char *fmt;
    long len;
    rb_encoding *enc;
    VALUE str;

    GetTimeval(time, tobj);
    MAKE_TM(time, tobj);
    StringValue(format);
    if (!rb_enc_str_asciicompat_p(format)) {
	rb_raise(rb_eArgError, "format should have ASCII compatible encoding");
    }
    format = rb_str_new4(format);
    fmt = RSTRING_PTR(format);
    len = RSTRING_LEN(format);
    enc = rb_enc_get(format);
    if (len == 0) {
	rb_warning("strftime called with empty format string");
    }
    else if (memchr(fmt, '\0', len)) {
	/* Ruby string may contain \0's. */
	const char *p = fmt, *pe = fmt + len;

	str = rb_str_new(0, 0);
	while (p < pe) {
	    len = rb_strftime_alloc(&buf, format, p, enc,
				    &tobj->vtm, tobj->timew, TIME_UTC_P(tobj));
	    rb_str_cat(str, buf, len);
	    p += strlen(p);
	    if (buf != buffer) {
		xfree(buf);
		buf = buffer;
	    }
	    for (fmt = p; p < pe && !*p; ++p);
	    if (p > fmt) rb_str_cat(str, fmt, p - fmt);
	}
	return str;
    }
    else {
	len = rb_strftime_alloc(&buf, format, RSTRING_PTR(format), enc,
				&tobj->vtm, tobj->timew, TIME_UTC_P(tobj));
    }
    str = rb_enc_str_new(buf, len, enc);
    if (buf != buffer) xfree(buf);
    return str;
}

/*
 * undocumented
 */

static VALUE
time_mdump(VALUE time)
{
    struct time_object *tobj;
    unsigned long p, s;
    char buf[8];
    int i;
    VALUE str;

    struct vtm vtm;
    long year;
    long usec, nsec;
    VALUE subsecx, nano, subnano, v;

    GetTimeval(time, tobj);

    gmtimew(tobj->timew, &vtm);

    if (FIXNUM_P(vtm.year)) {
        year = FIX2LONG(vtm.year);
        if (year < 1900 || 1900+0xffff < year)
            rb_raise(rb_eArgError, "year too big to marshal: %ld UTC", year);
    }
    else {
        rb_raise(rb_eArgError, "year too big to marshal");
    }

    subsecx = vtm.subsecx;

    nano = mulquo(subsecx, INT2FIX(1000000000), INT2FIX(TIME_SCALE));
    divmodv(nano, INT2FIX(1), &v, &subnano);
    nsec = FIX2LONG(v);
    usec = nsec / 1000;
    nsec = nsec % 1000;

    nano = add(LONG2FIX(nsec), subnano);

    p = 0x1UL            << 31 | /*  1 */
	TIME_UTC_P(tobj) << 30 | /*  1 */
	(year-1900)      << 14 | /* 16 */
	(vtm.mon-1)      << 10 | /*  4 */
	vtm.mday         <<  5 | /*  5 */
	vtm.hour;                /*  5 */
    s = vtm.min          << 26 | /*  6 */
	vtm.sec          << 20 | /*  6 */
	usec;    /* 20 */

    for (i=0; i<4; i++) {
	buf[i] = (unsigned char)p;
	p = RSHIFT(p, 8);
    }
    for (i=4; i<8; i++) {
	buf[i] = (unsigned char)s;
	s = RSHIFT(s, 8);
    }

    str = rb_str_new(buf, 8);
    rb_copy_generic_ivar(str, time);
    if (!rb_equal(nano, INT2FIX(0))) {
        if (RB_TYPE_P(nano, T_RATIONAL)) {
            rb_ivar_set(str, id_nano_num, RRATIONAL(nano)->num);
            rb_ivar_set(str, id_nano_den, RRATIONAL(nano)->den);
        }
        else {
            rb_ivar_set(str, id_nano_num, nano);
            rb_ivar_set(str, id_nano_den, INT2FIX(1));
        }
    }
    if (nsec) { /* submicro is only for Ruby 1.9.1 compatibility */
        /*
         * submicro is formatted in fixed-point packed BCD (without sign).
         * It represent digits under microsecond.
         * For nanosecond resolution, 3 digits (2 bytes) are used.
         * However it can be longer.
         * Extra digits are ignored for loading.
         */
        char buf[2];
        int len = (int)sizeof(buf);
        buf[1] = (char)((nsec % 10) << 4);
        nsec /= 10;
        buf[0] = (char)(nsec % 10);
        nsec /= 10;
        buf[0] |= (char)((nsec % 10) << 4);
        if (buf[1] == 0)
            len = 1;
        rb_ivar_set(str, id_submicro, rb_str_new(buf, len));
    }
    if (!TIME_UTC_P(tobj)) {
	VALUE off = time_utc_offset(time), div, mod;
	divmodv(off, INT2FIX(1), &div, &mod);
	if (rb_equal(mod, INT2FIX(0)))
	    off = rb_Integer(div);
	rb_ivar_set(str, id_offset, off);
    }
    return str;
}

/*
 * call-seq:
 *   time._dump   -> string
 *
 * Dump _time_ for marshaling.
 */

static VALUE
time_dump(int argc, VALUE *argv, VALUE time)
{
    VALUE str;

    rb_scan_args(argc, argv, "01", 0);
    str = time_mdump(time);

    return str;
}

/*
 * undocumented
 */

static VALUE
time_mload(VALUE time, VALUE str)
{
    struct time_object *tobj;
    unsigned long p, s;
    time_t sec;
    long usec;
    unsigned char *buf;
    struct vtm vtm;
    int i, gmt;
    long nsec;
    VALUE submicro, nano_num, nano_den, offset;
    wideval_t timew;
    st_data_t data;

    time_modify(time);

#define get_attr(attr, iffound) \
    attr = rb_attr_get(str, id_##attr); \
    if (!NIL_P(attr)) { \
	data = id_##attr; \
	iffound; \
        st_delete(rb_generic_ivar_table(str), &data, 0); \
    }

    get_attr(nano_num, {});
    get_attr(nano_den, {});
    get_attr(submicro, {});
    get_attr(offset, validate_utc_offset(offset));
#undef get_attr

    rb_copy_generic_ivar(time, str);

    StringValue(str);
    buf = (unsigned char *)RSTRING_PTR(str);
    if (RSTRING_LEN(str) != 8) {
	rb_raise(rb_eTypeError, "marshaled time format differ");
    }

    p = s = 0;
    for (i=0; i<4; i++) {
	p |= buf[i]<<(8*i);
    }
    for (i=4; i<8; i++) {
	s |= buf[i]<<(8*(i-4));
    }

    if ((p & (1UL<<31)) == 0) {
        gmt = 0;
	offset = Qnil;
	sec = p;
	usec = s;
        nsec = usec * 1000;
        timew = wadd(rb_time_magnify(TIMET2WV(sec)), wmulquoll(WINT2FIXWV(usec), TIME_SCALE, 1000000));
    }
    else {
	p &= ~(1UL<<31);
	gmt        = (int)((p >> 30) & 0x1);

	vtm.year = INT2FIX(((int)(p >> 14) & 0xffff) + 1900);
	vtm.mon  = ((int)(p >> 10) & 0xf) + 1;
	vtm.mday = (int)(p >>  5) & 0x1f;
	vtm.hour = (int) p        & 0x1f;
	vtm.min  = (int)(s >> 26) & 0x3f;
	vtm.sec  = (int)(s >> 20) & 0x3f;
        vtm.utc_offset = INT2FIX(0);
	vtm.yday = vtm.wday = 0;
	vtm.isdst = 0;
	vtm.zone = "";

	usec = (long)(s & 0xfffff);
        nsec = usec * 1000;


        vtm.subsecx = mulquo(LONG2FIX(nsec), INT2FIX(TIME_SCALE), LONG2FIX(1000000000));
        if (nano_num != Qnil) {
            VALUE nano = quo(num_exact(nano_num), num_exact(nano_den));
            vtm.subsecx = add(vtm.subsecx, mulquo(nano, INT2FIX(TIME_SCALE), LONG2FIX(1000000000)));
        }
        else if (submicro != Qnil) { /* for Ruby 1.9.1 compatibility */
            unsigned char *ptr;
            long len;
            int digit;
            ptr = (unsigned char*)StringValuePtr(submicro);
            len = RSTRING_LEN(submicro);
            nsec = 0;
            if (0 < len) {
                if (10 <= (digit = ptr[0] >> 4)) goto end_submicro;
                nsec += digit * 100;
                if (10 <= (digit = ptr[0] & 0xf)) goto end_submicro;
                nsec += digit * 10;
            }
            if (1 < len) {
                if (10 <= (digit = ptr[1] >> 4)) goto end_submicro;
                nsec += digit;
            }
            vtm.subsecx = add(vtm.subsecx, mulquo(LONG2FIX(nsec), INT2FIX(TIME_SCALE), LONG2FIX(1000000000)));
end_submicro: ;
        }
        timew = timegmw(&vtm);
    }

    GetTimeval(time, tobj);
    tobj->tm_got = 0;
    tobj->timew = timew;
    if (gmt) {
	TIME_SET_UTC(tobj);
    }
    else if (!NIL_P(offset)) {
	time_set_utc_offset(time, offset);
	time_fixoff(time);
    }

    return time;
}

/*
 * call-seq:
 *   Time._load(string)   -> time
 *
 * Unmarshal a dumped +Time+ object.
 */

static VALUE
time_load(VALUE klass, VALUE str)
{
    VALUE time = time_s_alloc(klass);

    time_mload(time, str);
    return time;
}

/*
 *  Time is an abstraction of dates and times. Time is stored internally as
 *  the number of seconds with fraction since the _Epoch_, January 1, 1970
 *  00:00 UTC. Also see the library modules Date. The Time class treats GMT
 *  (Greenwich Mean Time) and UTC (Coordinated Universal Time) as equivalent.
 *  GMT is the older way of referring to these baseline times but persists in
 *  the names of calls on POSIX systems.
 *
 *  All times may have fraction. Be aware of this fact when comparing times
 *  with each other -- times that are apparently equal when displayed may be
 *  different when compared.
 *
 *  Since Ruby 1.9.2, Time implementation uses a signed 63 bit integer, Bignum or Rational.
 *  The integer is a number of nanoseconds since the _Epoch_ which can
 *  represent 1823-11-12 to 2116-02-20.
 *  When Bignum or Rational is used (before 1823, after 2116, under nanosecond),
 *  Time works slower than the integer is used.
 *
 *  = Examples
 *
 *  All of these examples were done using the EST timezone which is GMT-5.
 *
 *  == Creating a new Time instance
 *
 *  You can create a new instance of time with Time.new. This will use the
 *  current system time. Time.now is a synonym for this. You can also
 *  pass parts of the time to Time.new such as year, month, minute, etc. When
 *  you want to construct a time this way you must pass at least a year. If you
 *  pass the year with nothing else time with default to January 1 of that year
 *  at 00:00:00 with the current system timezone. Here are some examples:
 *
 *    Time.new(2002)         #=> 2002-01-01 00:00:00 -0500
 *    Time.new(2002, 10)     #=> 2002-10-01 00:00:00 -0500
 *    Time.new(2002, 10, 31) #=> 2002-10-31 00:00:00 -0500
 *    Time.new(2002, 10, 31, 2, 2, 2, "+02:00") #=> 2002-10-31 02:02:02 -0200
 *
 *  You can also use #gm, #local and #utc to infer GMT, local and UTC
 *  timezones instead of using the current system setting.
 *
 *  You can also create a new time using Time.at which takes the number of
 *  seconds (or fraction of seconds) since the {Unix
 *  Epoch}[http://en.wikipedia.org/wiki/Unix_time].
 *
 *    Time.at(628232400) #=> 1989-11-28 00:00:00 -0500
 *
 *  == Working with an instance of Time
 *
 *  Once you have an instance of time there is a multitude of things you can do
 *  with it. Below are some examples. For all of the following examples, we
 *  will work on the assumption that you have done the following:
 *
 *    t = Time.new(1993, 02, 24, 12, 0, 0, "+09:00")
 *
 *  Was that a monday?
 *
 *    t.monday? #=> false
 *
 *  What year was that again?
 *
 *    t.year #=> 1993
 *
 *  Was is daylight savings at the time?
 *
 *    t.dst? #=> false
 *
 *  What's the day a year later?
 *
 *    t + (60*60*24*365) #=> 1994-02-24 12:00:00 +0900
 *
 *  How many second was that from the Unix Epoch?
 *
 *    t.to_i #=> 730522800
 *
 *  You can also do standard functions like compare two times.
 *
 *    t1 = Time.new(2010)
 *    t2 = Time.new(2011)
 *
 *    t1 == t2 #=> false
 *    t1 == t1 #=> true
 *    t1 <  t2 #=> true
 *    t1 >  t2 #=> false
 *
 *    Time.new(2010,10,31).between?(t1, t2) #=> true
 */

void
Init_Time(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    id_eq = rb_intern("==");
    id_ne = rb_intern("!=");
    id_quo = rb_intern("quo");
    id_div = rb_intern("div");
    id_cmp = rb_intern("<=>");
    id_lshift = rb_intern("<<");
    id_divmod = rb_intern("divmod");
    id_mul = rb_intern("*");
    id_submicro = rb_intern("submicro");
    id_nano_num = rb_intern("nano_num");
    id_nano_den = rb_intern("nano_den");
    id_offset = rb_intern("offset");

    rb_cTime = rb_define_class("Time", rb_cObject);
    rb_include_module(rb_cTime, rb_mComparable);

    rb_define_alloc_func(rb_cTime, time_s_alloc);
    rb_define_singleton_method(rb_cTime, "now", time_s_now, 0);
    rb_define_singleton_method(rb_cTime, "at", time_s_at, -1);
    rb_define_singleton_method(rb_cTime, "utc", time_s_mkutc, -1);
    rb_define_singleton_method(rb_cTime, "gm", time_s_mkutc, -1);
    rb_define_singleton_method(rb_cTime, "local", time_s_mktime, -1);
    rb_define_singleton_method(rb_cTime, "mktime", time_s_mktime, -1);

    rb_define_method(rb_cTime, "to_i", time_to_i, 0);
    rb_define_method(rb_cTime, "to_f", time_to_f, 0);
    rb_define_method(rb_cTime, "to_r", time_to_r, 0);
    rb_define_method(rb_cTime, "<=>", time_cmp, 1);
    rb_define_method(rb_cTime, "eql?", time_eql, 1);
    rb_define_method(rb_cTime, "hash", time_hash, 0);
    rb_define_method(rb_cTime, "initialize", time_init, -1);
    rb_define_method(rb_cTime, "initialize_copy", time_init_copy, 1);

    rb_define_method(rb_cTime, "localtime", time_localtime_m, -1);
    rb_define_method(rb_cTime, "gmtime", time_gmtime, 0);
    rb_define_method(rb_cTime, "utc", time_gmtime, 0);
    rb_define_method(rb_cTime, "getlocal", time_getlocaltime, -1);
    rb_define_method(rb_cTime, "getgm", time_getgmtime, 0);
    rb_define_method(rb_cTime, "getutc", time_getgmtime, 0);

    rb_define_method(rb_cTime, "ctime", time_asctime, 0);
    rb_define_method(rb_cTime, "asctime", time_asctime, 0);
    rb_define_method(rb_cTime, "to_s", time_to_s, 0);
    rb_define_method(rb_cTime, "inspect", time_to_s, 0);
    rb_define_method(rb_cTime, "to_a", time_to_a, 0);

    rb_define_method(rb_cTime, "+", time_plus, 1);
    rb_define_method(rb_cTime, "-", time_minus, 1);

    rb_define_method(rb_cTime, "succ", time_succ, 0);
    rb_define_method(rb_cTime, "round", time_round, -1);

    rb_define_method(rb_cTime, "sec", time_sec, 0);
    rb_define_method(rb_cTime, "min", time_min, 0);
    rb_define_method(rb_cTime, "hour", time_hour, 0);
    rb_define_method(rb_cTime, "mday", time_mday, 0);
    rb_define_method(rb_cTime, "day", time_mday, 0);
    rb_define_method(rb_cTime, "mon", time_mon, 0);
    rb_define_method(rb_cTime, "month", time_mon, 0);
    rb_define_method(rb_cTime, "year", time_year, 0);
    rb_define_method(rb_cTime, "wday", time_wday, 0);
    rb_define_method(rb_cTime, "yday", time_yday, 0);
    rb_define_method(rb_cTime, "isdst", time_isdst, 0);
    rb_define_method(rb_cTime, "dst?", time_isdst, 0);
    rb_define_method(rb_cTime, "zone", time_zone, 0);
    rb_define_method(rb_cTime, "gmtoff", time_utc_offset, 0);
    rb_define_method(rb_cTime, "gmt_offset", time_utc_offset, 0);
    rb_define_method(rb_cTime, "utc_offset", time_utc_offset, 0);

    rb_define_method(rb_cTime, "utc?", time_utc_p, 0);
    rb_define_method(rb_cTime, "gmt?", time_utc_p, 0);

    rb_define_method(rb_cTime, "sunday?", time_sunday, 0);
    rb_define_method(rb_cTime, "monday?", time_monday, 0);
    rb_define_method(rb_cTime, "tuesday?", time_tuesday, 0);
    rb_define_method(rb_cTime, "wednesday?", time_wednesday, 0);
    rb_define_method(rb_cTime, "thursday?", time_thursday, 0);
    rb_define_method(rb_cTime, "friday?", time_friday, 0);
    rb_define_method(rb_cTime, "saturday?", time_saturday, 0);

    rb_define_method(rb_cTime, "tv_sec", time_to_i, 0);
    rb_define_method(rb_cTime, "tv_usec", time_usec, 0);
    rb_define_method(rb_cTime, "usec", time_usec, 0);
    rb_define_method(rb_cTime, "tv_nsec", time_nsec, 0);
    rb_define_method(rb_cTime, "nsec", time_nsec, 0);
    rb_define_method(rb_cTime, "subsec", time_subsec, 0);

    rb_define_method(rb_cTime, "strftime", time_strftime, 1);

    /* methods for marshaling */
    rb_define_method(rb_cTime, "_dump", time_dump, -1);
    rb_define_singleton_method(rb_cTime, "_load", time_load, 1);
#if 0
    /* Time will support marshal_dump and marshal_load in the future (1.9 maybe) */
    rb_define_method(rb_cTime, "marshal_dump", time_mdump, 0);
    rb_define_method(rb_cTime, "marshal_load", time_mload, 1);
#endif

#ifdef DEBUG_FIND_TIME_NUMGUESS
    rb_define_virtual_variable("$find_time_numguess", find_time_numguess_getter, NULL);
#endif
}
