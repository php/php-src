/*
  date_core.c: Coded by Tadayoshi Funaba 2010-2012
*/

#include "ruby.h"
#include "ruby/encoding.h"
#include <math.h>
#include <time.h>

#define NDEBUG
#include <assert.h>

#ifdef RUBY_EXTCONF_H
#include RUBY_EXTCONF_H
#endif

#define USE_PACK

static ID id_cmp, id_le_p, id_ge_p, id_eqeq_p;
static VALUE cDate, cDateTime;
static VALUE half_days_in_day, day_in_nanoseconds;
static double positive_inf, negative_inf;

#define f_boolcast(x) ((x) ? Qtrue : Qfalse)

#define f_abs(x) rb_funcall(x, rb_intern("abs"), 0)
#define f_negate(x) rb_funcall(x, rb_intern("-@"), 0)
#define f_add(x,y) rb_funcall(x, '+', 1, y)
#define f_sub(x,y) rb_funcall(x, '-', 1, y)
#define f_mul(x,y) rb_funcall(x, '*', 1, y)
#define f_div(x,y) rb_funcall(x, '/', 1, y)
#define f_quo(x,y) rb_funcall(x, rb_intern("quo"), 1, y)
#define f_idiv(x,y) rb_funcall(x, rb_intern("div"), 1, y)
#define f_mod(x,y) rb_funcall(x, '%', 1, y)
#define f_remainder(x,y) rb_funcall(x, rb_intern("remainder"), 1, y)
#define f_expt(x,y) rb_funcall(x, rb_intern("**"), 1, y)
#define f_floor(x) rb_funcall(x, rb_intern("floor"), 0)
#define f_ceil(x) rb_funcall(x, rb_intern("ceil"), 0)
#define f_truncate(x) rb_funcall(x, rb_intern("truncate"), 0)
#define f_round(x) rb_funcall(x, rb_intern("round"), 0)

#define f_to_i(x) rb_funcall(x, rb_intern("to_i"), 0)
#define f_to_r(x) rb_funcall(x, rb_intern("to_r"), 0)
#define f_to_s(x) rb_funcall(x, rb_intern("to_s"), 0)
#define f_inspect(x) rb_funcall(x, rb_intern("inspect"), 0)

#define f_add3(x,y,z) f_add(f_add(x, y), z)
#define f_sub3(x,y,z) f_sub(f_sub(x, y), z)

inline static VALUE
f_cmp(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y)) {
	long c = FIX2LONG(x) - FIX2LONG(y);
	if (c > 0)
	    c = 1;
	else if (c < 0)
	    c = -1;
	return INT2FIX(c);
    }
    return rb_funcall(x, id_cmp, 1, y);
}

inline static VALUE
f_lt_p(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y))
	return f_boolcast(FIX2LONG(x) < FIX2LONG(y));
    return rb_funcall(x, '<', 1, y);
}

inline static VALUE
f_gt_p(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y))
	return f_boolcast(FIX2LONG(x) > FIX2LONG(y));
    return rb_funcall(x, '>', 1, y);
}

inline static VALUE
f_le_p(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y))
	return f_boolcast(FIX2LONG(x) <= FIX2LONG(y));
    return rb_funcall(x, id_le_p, 1, y);
}

inline static VALUE
f_ge_p(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y))
	return f_boolcast(FIX2LONG(x) >= FIX2LONG(y));
    return rb_funcall(x, rb_intern(">="), 1, y);
}

inline static VALUE
f_eqeq_p(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y))
	return f_boolcast(FIX2LONG(x) == FIX2LONG(y));
    return rb_funcall(x, rb_intern("=="), 1, y);
}

inline static VALUE
f_zero_p(VALUE x)
{
    switch (TYPE(x)) {
      case T_FIXNUM:
	return f_boolcast(FIX2LONG(x) == 0);
      case T_BIGNUM:
	return Qfalse;
      case T_RATIONAL:
	{
	    VALUE num = RRATIONAL(x)->num;
	    return f_boolcast(FIXNUM_P(num) && FIX2LONG(num) == 0);
	}
    }
    return rb_funcall(x, id_eqeq_p, 1, INT2FIX(0));
}

#define f_nonzero_p(x) (!f_zero_p(x))

inline static VALUE
f_negative_p(VALUE x)
{
    if (FIXNUM_P(x))
	return f_boolcast(FIX2LONG(x) < 0);
    return rb_funcall(x, '<', 1, INT2FIX(0));
}

#define f_positive_p(x) (!f_negative_p(x))

#define f_ajd(x) rb_funcall(x, rb_intern("ajd"), 0)
#define f_jd(x) rb_funcall(x, rb_intern("jd"), 0)
#define f_year(x) rb_funcall(x, rb_intern("year"), 0)
#define f_mon(x) rb_funcall(x, rb_intern("mon"), 0)
#define f_mday(x) rb_funcall(x, rb_intern("mday"), 0)
#define f_wday(x) rb_funcall(x, rb_intern("wday"), 0)
#define f_hour(x) rb_funcall(x, rb_intern("hour"), 0)
#define f_min(x) rb_funcall(x, rb_intern("min"), 0)
#define f_sec(x) rb_funcall(x, rb_intern("sec"), 0)

/* copied from time.c */
#define NDIV(x,y) (-(-((x)+1)/(y))-1)
#define NMOD(x,y) ((y)-(-((x)+1)%(y))-1)
#define DIV(n,d) ((n)<0 ? NDIV((n),(d)) : (n)/(d))
#define MOD(n,d) ((n)<0 ? NMOD((n),(d)) : (n)%(d))

#define HAVE_JD     (1 << 0)
#define HAVE_DF     (1 << 1)
#define HAVE_CIVIL  (1 << 2)
#define HAVE_TIME   (1 << 3)
#define COMPLEX_DAT (1 << 7)

#define have_jd_p(x) ((x)->flags & HAVE_JD)
#define have_df_p(x) ((x)->flags & HAVE_DF)
#define have_civil_p(x) ((x)->flags & HAVE_CIVIL)
#define have_time_p(x) ((x)->flags & HAVE_TIME)
#define complex_dat_p(x) ((x)->flags & COMPLEX_DAT)
#define simple_dat_p(x) (!complex_dat_p(x))

#define ITALY 2299161 /* 1582-10-15 */
#define ENGLAND 2361222 /* 1752-09-14 */
#define JULIAN positive_inf
#define GREGORIAN negative_inf
#define DEFAULT_SG ITALY

#define UNIX_EPOCH_IN_CJD INT2FIX(2440588) /* 1970-01-01 */

#define MINUTE_IN_SECONDS 60
#define HOUR_IN_SECONDS 3600
#define DAY_IN_SECONDS 86400
#define SECOND_IN_MILLISECONDS 1000
#define SECOND_IN_NANOSECONDS 1000000000

#define JC_PERIOD0 1461		/* 365.25 * 4 */
#define GC_PERIOD0 146097	/* 365.2425 * 400 */
#define CM_PERIOD0 71149239	/* (lcm 7 1461 146097) */
#define CM_PERIOD (0xfffffff / CM_PERIOD0 * CM_PERIOD0)
#define CM_PERIOD_JCY (CM_PERIOD / JC_PERIOD0 * 4)
#define CM_PERIOD_GCY (CM_PERIOD / GC_PERIOD0 * 400)

#define REFORM_BEGIN_YEAR 1582
#define REFORM_END_YEAR   1930
#define REFORM_BEGIN_JD 2298874	/* ns 1582-01-01 */
#define REFORM_END_JD   2426355	/* os 1930-12-31 */

#ifdef USE_PACK
#define SEC_WIDTH  6
#define MIN_WIDTH  6
#define HOUR_WIDTH 5
#define MDAY_WIDTH 5
#define MON_WIDTH  4

#define SEC_SHIFT  0
#define MIN_SHIFT  SEC_WIDTH
#define HOUR_SHIFT (MIN_WIDTH + SEC_WIDTH)
#define MDAY_SHIFT (HOUR_WIDTH + MIN_WIDTH + SEC_WIDTH)
#define MON_SHIFT  (MDAY_WIDTH + HOUR_WIDTH + MIN_WIDTH + SEC_WIDTH)

#define PK_MASK(x) ((1 << (x)) - 1)

#define EX_SEC(x)  (((x) >> SEC_SHIFT)  & PK_MASK(SEC_WIDTH))
#define EX_MIN(x)  (((x) >> MIN_SHIFT)  & PK_MASK(MIN_WIDTH))
#define EX_HOUR(x) (((x) >> HOUR_SHIFT) & PK_MASK(HOUR_WIDTH))
#define EX_MDAY(x) (((x) >> MDAY_SHIFT) & PK_MASK(MDAY_WIDTH))
#define EX_MON(x)  (((x) >> MON_SHIFT)  & PK_MASK(MON_WIDTH))

#define PACK5(m,d,h,min,s) \
    (((m) << MON_SHIFT) | ((d) << MDAY_SHIFT) |\
     ((h) << HOUR_SHIFT) | ((min) << MIN_SHIFT) | ((s) << SEC_SHIFT))

#define PACK2(m,d) \
    (((m) << MON_SHIFT) | ((d) << MDAY_SHIFT))
#endif

#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

#if defined(FLT_RADIX) && defined(FLT_MANT_DIG)
#if FLT_RADIX == 2 && FLT_MANT_DIG > 22
#define USE_FLOAT
#define sg_cast float
#else
#define sg_cast double
#endif
#endif

/* A set of nth, jd, df and sf denote ajd + 1/2.  Each ajd begin at
 * noon of GMT (assume equal to UTC).  However, this begins at
 * midnight.
 */

struct SimpleDateData
{
    unsigned flags;
    VALUE nth;	/* not always canonicalized */
    int jd;	/* as utc */
    /* df is zero */
    /* sf is zero */
    /* of is zero */
#ifndef USE_FLOAT
    double sg;  /* 2298874..2426355 or -/+oo */
#else
    float sg;	/* at most 22 bits */
#endif
    /* decoded as utc=local */
    int year;	/* truncated */
#ifndef USE_PACK
    int mon;
    int mday;
    /* hour is zero */
    /* min is zero */
    /* sec is zero */
#else
    /* packed civil */
    unsigned pc;
#endif
};

struct ComplexDateData
{
    unsigned flags;
    VALUE nth;	/* not always canonicalized */
    int jd; 	/* as utc */
    int df;	/* as utc, in secs */
    VALUE sf;	/* in nano secs */
    int of;	/* in secs */
#ifndef USE_FLOAT
    double sg;  /* 2298874..2426355 or -/+oo */
#else
    float sg;	/* at most 22 bits */
#endif
    /* decoded as local */
    int year;	/* truncated */
#ifndef USE_PACK
    int mon;
    int mday;
    int hour;
    int min;
    int sec;
#else
    /* packed civil */
    unsigned pc;
#endif
};

union DateData {
    unsigned flags;
    struct SimpleDateData s;
    struct ComplexDateData c;
};

#define get_d1(x)\
    union DateData *dat;\
    Data_Get_Struct(x, union DateData, dat);

#define get_d1a(x)\
    union DateData *adat;\
    Data_Get_Struct(x, union DateData, adat);

#define get_d1b(x)\
    union DateData *bdat;\
    Data_Get_Struct(x, union DateData, bdat);

#define get_d2(x,y)\
    union DateData *adat, *bdat;\
    Data_Get_Struct(x, union DateData, adat);\
    Data_Get_Struct(y, union DateData, bdat);

inline static VALUE
canon(VALUE x)
{
    if (TYPE(x) == T_RATIONAL) {
	VALUE den = RRATIONAL(x)->den;
	if (FIXNUM_P(den) && FIX2LONG(den) == 1)
	    return RRATIONAL(x)->num;
    }
    return x;
}

#ifndef USE_PACK
#define set_to_simple(x, _nth, _jd ,_sg, _year, _mon, _mday, _flags) \
{\
    (x)->nth = canon(_nth);\
    (x)->jd = _jd;\
    (x)->sg = (sg_cast)(_sg);\
    (x)->year = _year;\
    (x)->mon = _mon;\
    (x)->mday = _mday;\
    (x)->flags = _flags;\
}
#else
#define set_to_simple(x, _nth, _jd ,_sg, _year, _mon, _mday, _flags) \
{\
    (x)->nth = canon(_nth);\
    (x)->jd = _jd;\
    (x)->sg = (sg_cast)(_sg);\
    (x)->year = _year;\
    (x)->pc = PACK2(_mon, _mday);\
    (x)->flags = _flags;\
}
#endif

#ifndef USE_PACK
#define set_to_complex(x, _nth, _jd ,_df, _sf, _of, _sg,\
_year, _mon, _mday, _hour, _min, _sec, _flags) \
{\
    (x)->nth = canon(_nth);\
    (x)->jd = _jd;\
    (x)->df = _df;\
    (x)->sf = canon(_sf);\
    (x)->of = _of;\
    (x)->sg = (sg_cast)(_sg);\
    (x)->year = _year;\
    (x)->mon = _mon;\
    (x)->mday = _mday;\
    (x)->hour = _hour;\
    (x)->min = _min;\
    (x)->sec = _sec;\
    (x)->flags = _flags;\
}
#else
#define set_to_complex(x, _nth, _jd ,_df, _sf, _of, _sg,\
_year, _mon, _mday, _hour, _min, _sec, _flags) \
{\
    (x)->nth = canon(_nth);\
    (x)->jd = _jd;\
    (x)->df = _df;\
    (x)->sf = canon(_sf);\
    (x)->of = _of;\
    (x)->sg = (sg_cast)(_sg);\
    (x)->year = _year;\
    (x)->pc = PACK5(_mon, _mday, _hour, _min, _sec);\
    (x)->flags = _flags;\
}
#endif

#ifndef USE_PACK
#define copy_simple_to_complex(x, y) \
{\
    (x)->nth = (y)->nth;\
    (x)->jd = (y)->jd;\
    (x)->df = 0;\
    (x)->sf = INT2FIX(0);\
    (x)->of = 0;\
    (x)->sg = (sg_cast)((y)->sg);\
    (x)->year = (y)->year;\
    (x)->mon = (y)->mon;\
    (x)->mday = (y)->mday;\
    (x)->hour = 0;\
    (x)->min = 0;\
    (x)->sec = 0;\
    (x)->flags = (y)->flags;\
}
#else
#define copy_simple_to_complex(x, y) \
{\
    (x)->nth = (y)->nth;\
    (x)->jd = (y)->jd;\
    (x)->df = 0;\
    (x)->sf = INT2FIX(0);\
    (x)->of = 0;\
    (x)->sg = (sg_cast)((y)->sg);\
    (x)->year = (y)->year;\
    (x)->pc = PACK5(EX_MON((y)->pc), EX_MDAY((y)->pc), 0, 0, 0);\
    (x)->flags = (y)->flags;\
}
#endif

#ifndef USE_PACK
#define copy_complex_to_simple(x, y) \
{\
    (x)->nth = (y)->nth;\
    (x)->jd = (y)->jd;\
    (x)->sg = (sg_cast)((y)->sg);\
    (x)->year = (y)->year;\
    (x)->mon = (y)->mon;\
    (x)->mday = (y)->mday;\
    (x)->flags = (y)->flags;\
}
#else
#define copy_complex_to_simple(x, y) \
{\
    (x)->nth = (y)->nth;\
    (x)->jd = (y)->jd;\
    (x)->sg = (sg_cast)((y)->sg);\
    (x)->year = (y)->year;\
    (x)->pc = PACK2(EX_MON((y)->pc), EX_MDAY((y)->pc));\
    (x)->flags = (y)->flags;\
}
#endif

/* base */

static int c_valid_civil_p(int, int, int, double,
			   int *, int *, int *, int *);

static int
c_find_fdoy(int y, double sg, int *rjd, int *ns)
{
    int d, rm, rd;

    for (d = 1; d < 31; d++)
	if (c_valid_civil_p(y, 1, d, sg, &rm, &rd, rjd, ns))
	    return 1;
    return 0;
}

static int
c_find_ldoy(int y, double sg, int *rjd, int *ns)
{
    int i, rm, rd;

    for (i = 0; i < 30; i++)
	if (c_valid_civil_p(y, 12, 31 - i, sg, &rm, &rd, rjd, ns))
	    return 1;
    return 0;
}

#ifndef NDEBUG
static int
c_find_fdom(int y, int m, double sg, int *rjd, int *ns)
{
    int d, rm, rd;

    for (d = 1; d < 31; d++)
	if (c_valid_civil_p(y, m, d, sg, &rm, &rd, rjd, ns))
	    return 1;
    return 0;
}
#endif

static int
c_find_ldom(int y, int m, double sg, int *rjd, int *ns)
{
    int i, rm, rd;

    for (i = 0; i < 30; i++)
	if (c_valid_civil_p(y, m, 31 - i, sg, &rm, &rd, rjd, ns))
	    return 1;
    return 0;
}

static void
c_civil_to_jd(int y, int m, int d, double sg, int *rjd, int *ns)
{
    double a, b, jd;

    if (m <= 2) {
	y -= 1;
	m += 12;
    }
    a = floor(y / 100.0);
    b = 2 - a + floor(a / 4.0);
    jd = floor(365.25 * (y + 4716)) +
	floor(30.6001 * (m + 1)) +
	d + b - 1524;
    if (jd < sg) {
	jd -= b;
	*ns = 0;
    }
    else
	*ns = 1;

    *rjd = (int)jd;
}

static void
c_jd_to_civil(int jd, double sg, int *ry, int *rm, int *rdom)
{
    double x, a, b, c, d, e, y, m, dom;

    if (jd < sg)
	a = jd;
    else {
	x = floor((jd - 1867216.25) / 36524.25);
	a = jd + 1 + x - floor(x / 4.0);
    }
    b = a + 1524;
    c = floor((b - 122.1) / 365.25);
    d = floor(365.25 * c);
    e = floor((b - d) / 30.6001);
    dom = b - d - floor(30.6001 * e);
    if (e <= 13) {
	m = e - 1;
	y = c - 4716;
    }
    else {
	m = e - 13;
	y = c - 4715;
    }

    *ry = (int)y;
    *rm = (int)m;
    *rdom = (int)dom;
}

static void
c_ordinal_to_jd(int y, int d, double sg, int *rjd, int *ns)
{
    int ns2;

    c_find_fdoy(y, sg, rjd, &ns2);
    *rjd += d - 1;
    *ns = (*rjd < sg) ? 0 : 1;
}

static void
c_jd_to_ordinal(int jd, double sg, int *ry, int *rd)
{
    int rm2, rd2, rjd, ns;

    c_jd_to_civil(jd, sg, ry, &rm2, &rd2);
    c_find_fdoy(*ry, sg, &rjd, &ns);
    *rd = (jd - rjd) + 1;
}

static void
c_commercial_to_jd(int y, int w, int d, double sg, int *rjd, int *ns)
{
    int rjd2, ns2;

    c_find_fdoy(y, sg, &rjd2, &ns2);
    rjd2 += 3;
    *rjd =
	(rjd2 - MOD((rjd2 - 1) + 1, 7)) +
	7 * (w - 1) +
	(d - 1);
    *ns = (*rjd < sg) ? 0 : 1;
}

static void
c_jd_to_commercial(int jd, double sg, int *ry, int *rw, int *rd)
{
    int ry2, rm2, rd2, a, rjd2, ns2;

    c_jd_to_civil(jd - 3, sg, &ry2, &rm2, &rd2);
    a = ry2;
    c_commercial_to_jd(a + 1, 1, 1, sg, &rjd2, &ns2);
    if (jd >= rjd2)
	*ry = a + 1;
    else {
	c_commercial_to_jd(a, 1, 1, sg, &rjd2, &ns2);
	*ry = a;
    }
    *rw = 1 + DIV(jd - rjd2, 7);
    *rd = MOD(jd + 1, 7);
    if (*rd == 0)
	*rd = 7;
}

static void
c_weeknum_to_jd(int y, int w, int d, int f, double sg, int *rjd, int *ns)
{
    int rjd2, ns2;

    c_find_fdoy(y, sg, &rjd2, &ns2);
    rjd2 += 6;
    *rjd = (rjd2 - MOD(((rjd2 - f) + 1), 7) - 7) + 7 * w + d;
    *ns = (*rjd < sg) ? 0 : 1;
}

static void
c_jd_to_weeknum(int jd, int f, double sg, int *ry, int *rw, int *rd)
{
    int rm, rd2, rjd, ns, j;

    c_jd_to_civil(jd, sg, ry, &rm, &rd2);
    c_find_fdoy(*ry, sg, &rjd, &ns);
    rjd += 6;
    j = jd - (rjd - MOD((rjd - f) + 1, 7)) + 7;
    *rw = (int)DIV(j, 7);
    *rd = (int)MOD(j, 7);
}

#ifndef NDEBUG
static void
c_nth_kday_to_jd(int y, int m, int n, int k, double sg, int *rjd, int *ns)
{
    int rjd2, ns2;

    if (n > 0) {
	c_find_fdom(y, m, sg, &rjd2, &ns2);
	rjd2 -= 1;
    }
    else {
	c_find_ldom(y, m, sg, &rjd2, &ns2);
	rjd2 += 7;
    }
    *rjd = (rjd2 - MOD((rjd2 - k) + 1, 7)) + 7 * n;
    *ns = (*rjd < sg) ? 0 : 1;
}
#endif

inline static int
c_jd_to_wday(int jd)
{
    return MOD(jd + 1, 7);
}

#ifndef NDEBUG
static void
c_jd_to_nth_kday(int jd, double sg, int *ry, int *rm, int *rn, int *rk)
{
    int rd, rjd, ns2;

    c_jd_to_civil(jd, sg, ry, rm, &rd);
    c_find_fdom(*ry, *rm, sg, &rjd, &ns2);
    *rn = DIV(jd - rjd, 7) + 1;
    *rk = c_jd_to_wday(jd);
}
#endif

static int
c_valid_ordinal_p(int y, int d, double sg,
		  int *rd, int *rjd, int *ns)
{
    int ry2, rd2;

    if (d < 0) {
	int rjd2, ns2;

	if (!c_find_ldoy(y, sg, &rjd2, &ns2))
	    return 0;
	c_jd_to_ordinal(rjd2 + d + 1, sg, &ry2, &rd2);
	if (ry2 != y)
	    return 0;
	d = rd2;
    }
    c_ordinal_to_jd(y, d, sg, rjd, ns);
    c_jd_to_ordinal(*rjd, sg, &ry2, &rd2);
    if (ry2 != y || rd2 != d)
	return 0;
    return 1;
}

static const int monthtab[2][13] = {
    { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

inline static int
c_julian_leap_p(int y)
{
    return MOD(y, 4) == 0;
}

inline static int
c_gregorian_leap_p(int y)
{
    return MOD(y, 4) == 0 && y % 100 != 0 || MOD(y, 400) == 0;
}

static int
c_julian_last_day_of_month(int y, int m)
{
    assert(m >= 1 && m <= 12);
    return monthtab[c_julian_leap_p(y) ? 1 : 0][m];
}

static int
c_gregorian_last_day_of_month(int y, int m)
{
    assert(m >= 1 && m <= 12);
    return monthtab[c_gregorian_leap_p(y) ? 1 : 0][m];
}

static int
c_valid_julian_p(int y, int m, int d, int *rm, int *rd)
{
    int last;

    if (m < 0)
	m += 13;
    if (m < 1 || m > 12)
	return 0;
    last = c_julian_last_day_of_month(y, m);
    if (d < 0)
	d = last + d + 1;
    if (d < 1 || d > last)
	return 0;
    *rm = m;
    *rd = d;
    return 1;
}

static int
c_valid_gregorian_p(int y, int m, int d, int *rm, int *rd)
{
    int last;

    if (m < 0)
	m += 13;
    if (m < 1 || m > 12)
	return 0;
    last = c_gregorian_last_day_of_month(y, m);
    if (d < 0)
	d = last + d + 1;
    if (d < 1 || d > last)
	return 0;
    *rm = m;
    *rd = d;
    return 1;
}

static int
c_valid_civil_p(int y, int m, int d, double sg,
		int *rm, int *rd, int *rjd, int *ns)
{
    int ry;

    if (m < 0)
	m += 13;
    if (d < 0) {
	if (!c_find_ldom(y, m, sg, rjd, ns))
	    return 0;
	c_jd_to_civil(*rjd + d + 1, sg, &ry, rm, rd);
	if (ry != y || *rm != m)
	    return 0;
	d = *rd;
    }
    c_civil_to_jd(y, m, d, sg, rjd, ns);
    c_jd_to_civil(*rjd, sg, &ry, rm, rd);
    if (ry != y || *rm != m || *rd != d)
	return 0;
    return 1;
}

static int
c_valid_commercial_p(int y, int w, int d, double sg,
		     int *rw, int *rd, int *rjd, int *ns)
{
    int ns2, ry2, rw2, rd2;

    if (d < 0)
	d += 8;
    if (w < 0) {
	int rjd2;

	c_commercial_to_jd(y + 1, 1, 1, sg, &rjd2, &ns2);
	c_jd_to_commercial(rjd2 + w * 7, sg, &ry2, &rw2, &rd2);
	if (ry2 != y)
	    return 0;
	w = rw2;
    }
    c_commercial_to_jd(y, w, d, sg, rjd, ns);
    c_jd_to_commercial(*rjd, sg, &ry2, rw, rd);
    if (y != ry2 || w != *rw || d != *rd)
	return 0;
    return 1;
}

static int
c_valid_weeknum_p(int y, int w, int d, int f, double sg,
		  int *rw, int *rd, int *rjd, int *ns)
{
    int ns2, ry2, rw2, rd2;

    if (d < 0)
	d += 7;
    if (w < 0) {
	int rjd2;

	c_weeknum_to_jd(y + 1, 1, f, f, sg, &rjd2, &ns2);
	c_jd_to_weeknum(rjd2 + w * 7, f, sg, &ry2, &rw2, &rd2);
	if (ry2 != y)
	    return 0;
	w = rw2;
    }
    c_weeknum_to_jd(y, w, d, f, sg, rjd, ns);
    c_jd_to_weeknum(*rjd, f, sg, &ry2, rw, rd);
    if (y != ry2 || w != *rw || d != *rd)
	return 0;
    return 1;
}

#ifndef NDEBUG
static int
c_valid_nth_kday_p(int y, int m, int n, int k, double sg,
		   int *rm, int *rn, int *rk, int *rjd, int *ns)
{
    int ns2, ry2, rm2, rn2, rk2;

    if (k < 0)
	k += 7;
    if (n < 0) {
	int t, ny, nm, rjd2;

	t = y * 12 + m;
	ny = DIV(t, 12);
	nm = MOD(t, 12) + 1;

	c_nth_kday_to_jd(ny, nm, 1, k, sg, &rjd2, &ns2);
	c_jd_to_nth_kday(rjd2 + n * 7, sg, &ry2, &rm2, &rn2, &rk2);
	if (ry2 != y || rm2 != m)
	    return 0;
	n = rn2;
    }
    c_nth_kday_to_jd(y, m, n, k, sg, rjd, ns);
    c_jd_to_nth_kday(*rjd, sg, &ry2, rm, rn, rk);
    if (y != ry2 || m != *rm || n != *rn || k != *rk)
	return 0;
    return 1;
}
#endif

static int
c_valid_time_p(int h, int min, int s, int *rh, int *rmin, int *rs)
{
    if (h < 0)
	h += 24;
    if (min < 0)
	min += 60;
    if (s < 0)
	s += 60;
    *rh = h;
    *rmin = min;
    *rs = s;
    return !(h   < 0 || h   > 24 ||
	     min < 0 || min > 59 ||
	     s   < 0 || s   > 59 ||
	     (h == 24 && (min > 0 || s > 0)));
}

inline static int
c_valid_start_p(double sg)
{
    if (isnan(sg))
	return 0;
    if (isinf(sg))
	return 1;
    if (sg < REFORM_BEGIN_JD || sg > REFORM_END_JD)
	return 0;
    return 1;
}

inline static int
df_local_to_utc(int df, int of)
{
    df -= of;
    if (df < 0)
	df += DAY_IN_SECONDS;
    else if (df >= DAY_IN_SECONDS)
	df -= DAY_IN_SECONDS;
    return df;
}

inline static int
df_utc_to_local(int df, int of)
{
    df += of;
    if (df < 0)
	df += DAY_IN_SECONDS;
    else if (df >= DAY_IN_SECONDS)
	df -= DAY_IN_SECONDS;
    return df;
}

inline static int
jd_local_to_utc(int jd, int df, int of)
{
    df -= of;
    if (df < 0)
	jd -= 1;
    else if (df >= DAY_IN_SECONDS)
	jd += 1;
    return jd;
}

inline static int
jd_utc_to_local(int jd, int df, int of)
{
    df += of;
    if (df < 0)
	jd -= 1;
    else if (df >= DAY_IN_SECONDS)
	jd += 1;
    return jd;
}

inline static int
time_to_df(int h, int min, int s)
{
    return h * HOUR_IN_SECONDS + min * MINUTE_IN_SECONDS + s;
}

inline static void
df_to_time(int df, int *h, int *min, int *s)
{
    *h = df / HOUR_IN_SECONDS;
    df %= HOUR_IN_SECONDS;
    *min = df / MINUTE_IN_SECONDS;
    *s = df % MINUTE_IN_SECONDS;
}

static VALUE
sec_to_day(VALUE s)
{
    if (FIXNUM_P(s))
	return rb_rational_new2(s, INT2FIX(DAY_IN_SECONDS));
    return f_quo(s, INT2FIX(DAY_IN_SECONDS));
}

inline static VALUE
isec_to_day(int s)
{
    return sec_to_day(INT2FIX(s));
}

static VALUE
ns_to_day(VALUE n)
{
    if (FIXNUM_P(n))
	return rb_rational_new2(n, day_in_nanoseconds);
    return f_quo(n, day_in_nanoseconds);
}

#ifndef NDEBUG
static VALUE
ms_to_sec(VALUE m)
{
    if (FIXNUM_P(m))
	return rb_rational_new2(m, INT2FIX(SECOND_IN_MILLISECONDS));
    return f_quo(m, INT2FIX(SECOND_IN_MILLISECONDS));
}
#endif

static VALUE
ns_to_sec(VALUE n)
{
    if (FIXNUM_P(n))
	return rb_rational_new2(n, INT2FIX(SECOND_IN_NANOSECONDS));
    return f_quo(n, INT2FIX(SECOND_IN_NANOSECONDS));
}

#ifndef NDEBUG
inline static VALUE
ins_to_day(int n)
{
    return ns_to_day(INT2FIX(n));
}
#endif

static int
safe_mul_p(VALUE x, long m)
{
    long ix;

    if (!FIXNUM_P(x))
	return 0;
    ix = FIX2LONG(x);
    if (ix >= (FIXNUM_MAX / m))
	return 0;
    return 1;
}

static VALUE
day_to_sec(VALUE d)
{
    if (safe_mul_p(d, DAY_IN_SECONDS))
	return LONG2FIX(FIX2LONG(d) * DAY_IN_SECONDS);
    return f_mul(d, INT2FIX(DAY_IN_SECONDS));
}

#ifndef NDEBUG
static VALUE
day_to_ns(VALUE d)
{
    return f_mul(d, day_in_nanoseconds);
}
#endif

static VALUE
sec_to_ms(VALUE s)
{
    if (safe_mul_p(s, SECOND_IN_MILLISECONDS))
	return LONG2FIX(FIX2LONG(s) * SECOND_IN_MILLISECONDS);
    return f_mul(s, INT2FIX(SECOND_IN_MILLISECONDS));
}

static VALUE
sec_to_ns(VALUE s)
{
    if (safe_mul_p(s, SECOND_IN_NANOSECONDS))
	return LONG2FIX(FIX2LONG(s) * SECOND_IN_NANOSECONDS);
    return f_mul(s, INT2FIX(SECOND_IN_NANOSECONDS));
}

#ifndef NDEBUG
static VALUE
isec_to_ns(int s)
{
    return sec_to_ns(INT2FIX(s));
}
#endif

static VALUE
div_day(VALUE d, VALUE *f)
{
    if (f)
	*f = f_mod(d, INT2FIX(1));
    return f_floor(d);
}

static VALUE
div_df(VALUE d, VALUE *f)
{
    VALUE s = day_to_sec(d);

    if (f)
	*f = f_mod(s, INT2FIX(1));
    return f_floor(s);
}

#ifndef NDEBUG
static VALUE
div_sf(VALUE s, VALUE *f)
{
    VALUE n = sec_to_ns(s);

    if (f)
	*f = f_mod(n, INT2FIX(1));
    return f_floor(n);
}
#endif

static void
decode_day(VALUE d, VALUE *jd, VALUE *df, VALUE *sf)
{
    VALUE f;

    *jd = div_day(d, &f);
    *df = div_df(f, &f);
    *sf = sec_to_ns(f);
}

inline static double
s_virtual_sg(union DateData *x)
{
    if (isinf(x->s.sg))
	return x->s.sg;
    if (f_zero_p(x->s.nth))
	return x->s.sg;
    else if (f_negative_p(x->s.nth))
	return positive_inf;
    return negative_inf;
}

inline static double
c_virtual_sg(union DateData *x)
{
    if (isinf(x->c.sg))
	return x->c.sg;
    if (f_zero_p(x->c.nth))
	return x->c.sg;
    else if (f_negative_p(x->c.nth))
	return positive_inf;
    return negative_inf;
}

inline static double
m_virtual_sg(union DateData *x)
{
    if (simple_dat_p(x))
	return s_virtual_sg(x);
    else
	return c_virtual_sg(x);
}

inline static void
get_s_jd(union DateData *x)
{
    assert(simple_dat_p(x));
    if (!have_jd_p(x)) {
	int jd, ns;

	assert(have_civil_p(x));
#ifndef USE_PACK
	c_civil_to_jd(x->s.year, x->s.mon, x->s.mday,
		      s_virtual_sg(x), &jd, &ns);
#else
	c_civil_to_jd(x->s.year, EX_MON(x->s.pc), EX_MDAY(x->s.pc),
		      s_virtual_sg(x), &jd, &ns);
#endif
	x->s.jd = jd;
	x->s.flags |= HAVE_JD;
    }
}

inline static void
get_s_civil(union DateData *x)
{
    assert(simple_dat_p(x));
    if (!have_civil_p(x)) {
	int y, m, d;

	assert(have_jd_p(x));
	c_jd_to_civil(x->s.jd, s_virtual_sg(x), &y, &m, &d);
	x->s.year = y;
#ifndef USE_PACK
	x->s.mon = m;
	x->s.mday = d;
#else
	x->s.pc = PACK2(m, d);
#endif
	x->s.flags |= HAVE_CIVIL;
    }
}

inline static void
get_c_df(union DateData *x)
{
    assert(complex_dat_p(x));
    if (!have_df_p(x)) {
	assert(have_time_p(x));
#ifndef USE_PACK
	x->c.df = df_local_to_utc(time_to_df(x->c.hour, x->c.min, x->c.sec),
				  x->c.of);
#else
	x->c.df = df_local_to_utc(time_to_df(EX_HOUR(x->c.pc),
					     EX_MIN(x->c.pc),
					     EX_SEC(x->c.pc)),
				  x->c.of);
#endif
	x->c.flags |= HAVE_DF;
    }
}

inline static void
get_c_time(union DateData *x)
{
    assert(complex_dat_p(x));
    if (!have_time_p(x)) {
#ifndef USE_PACK
	int r;
	assert(have_df_p(x));
	r = df_utc_to_local(x->c.df, x->c.of);
	df_to_time(r, &x->c.hour, &x->c.min, &x->c.sec);
	x->c.flags |= HAVE_TIME;
#else
	int r, m, d, h, min, s;

	assert(have_df_p(x));
	m = EX_MON(x->c.pc);
	d = EX_MDAY(x->c.pc);
	r = df_utc_to_local(x->c.df, x->c.of);
	df_to_time(r, &h, &min, &s);
	x->c.pc = PACK5(m, d, h, min, s);
	x->c.flags |= HAVE_TIME;
#endif
    }
}

inline static void
get_c_jd(union DateData *x)
{
    assert(complex_dat_p(x));
    if (!have_jd_p(x)) {
	int jd, ns;

	assert(have_civil_p(x));
#ifndef USE_PACK
	c_civil_to_jd(x->c.year, x->c.mon, x->c.mday,
		      c_virtual_sg(x), &jd, &ns);
#else
	c_civil_to_jd(x->c.year, EX_MON(x->c.pc), EX_MDAY(x->c.pc),
		      c_virtual_sg(x), &jd, &ns);
#endif

	get_c_time(x);
#ifndef USE_PACK
	x->c.jd = jd_local_to_utc(jd,
				  time_to_df(x->c.hour, x->c.min, x->c.sec),
				  x->c.of);
#else
	x->c.jd = jd_local_to_utc(jd,
				  time_to_df(EX_HOUR(x->c.pc),
					     EX_MIN(x->c.pc),
					     EX_SEC(x->c.pc)),
				  x->c.of);
#endif
	x->c.flags |= HAVE_JD;
    }
}

inline static void
get_c_civil(union DateData *x)
{
    assert(complex_dat_p(x));
    if (!have_civil_p(x)) {
#ifndef USE_PACK
	int jd, y, m, d;
#else
	int jd, y, m, d, h, min, s;
#endif

	assert(have_jd_p(x));
	get_c_df(x);
	jd = jd_utc_to_local(x->c.jd, x->c.df, x->c.of);
	c_jd_to_civil(jd, c_virtual_sg(x), &y, &m, &d);
	x->c.year = y;
#ifndef USE_PACK
	x->c.mon = m;
	x->c.mday = d;
#else
	h = EX_HOUR(x->c.pc);
	min = EX_MIN(x->c.pc);
	s = EX_SEC(x->c.pc);
	x->c.pc = PACK5(m, d, h, min, s);
#endif
	x->c.flags |= HAVE_CIVIL;
    }
}

inline static int
local_jd(union DateData *x)
{
    assert(complex_dat_p(x));
    assert(have_jd_p(x));
    assert(have_df_p(x));
    return jd_utc_to_local(x->c.jd, x->c.df, x->c.of);
}

inline static int
local_df(union DateData *x)
{
    assert(complex_dat_p(x));
    assert(have_df_p(x));
    return df_utc_to_local(x->c.df, x->c.of);
}

static void
decode_year(VALUE y, double style,
	    VALUE *nth, int *ry)
{
    int period;
    VALUE t;

    period = (style < 0) ?
	CM_PERIOD_GCY :
	CM_PERIOD_JCY;
    if (FIXNUM_P(y)) {
	long iy, it, inth;

	iy = FIX2LONG(y);
	if (iy >= (FIXNUM_MAX - 4712))
	    goto big;
	it = iy + 4712; /* shift */
	inth = DIV(it, ((long)period));
	*nth = LONG2FIX(inth);
	if (inth)
	    it = MOD(it, ((long)period));
	*ry = (int)it - 4712; /* unshift */
	return;
    }
  big:
    t = f_add(y, INT2FIX(4712)); /* shift */
    *nth = f_idiv(t, INT2FIX(period));
    if (f_nonzero_p(*nth))
	t = f_mod(t, INT2FIX(period));
    *ry = FIX2INT(t) - 4712; /* unshift */
}

static void
encode_year(VALUE nth, int y, double style,
	    VALUE *ry)
{
    int period;
    VALUE t;

    period = (style < 0) ?
	CM_PERIOD_GCY :
	CM_PERIOD_JCY;
    if (f_zero_p(nth))
	*ry = INT2FIX(y);
    else {
	t = f_mul(INT2FIX(period), nth);
	t = f_add(t, INT2FIX(y));
	*ry = t;
    }
}

static void
decode_jd(VALUE jd, VALUE *nth, int *rjd)
{
    assert(FIXNUM_P(jd) || RB_TYPE_P(jd, T_BIGNUM));
    *nth = f_idiv(jd, INT2FIX(CM_PERIOD));
    if (f_zero_p(*nth)) {
	assert(FIXNUM_P(jd));
	*rjd = FIX2INT(jd);
	return;
    }
    *rjd = FIX2INT(f_mod(jd, INT2FIX(CM_PERIOD)));
}

static void
encode_jd(VALUE nth, int jd, VALUE *rjd)
{
    if (f_zero_p(nth)) {
	*rjd = INT2FIX(jd);
	return;
    }
    *rjd = f_add(f_mul(INT2FIX(CM_PERIOD), nth), INT2FIX(jd));
}

inline static double
guess_style(VALUE y, double sg) /* -/+oo or zero */
{
    double style = 0;

    if (isinf(sg))
	style = sg;
    else if (!FIXNUM_P(y))
	style = f_positive_p(y) ? negative_inf : positive_inf;
    else {
	long iy = FIX2LONG(y);

	assert(FIXNUM_P(y));
	if (iy < REFORM_BEGIN_YEAR)
	    style = positive_inf;
	else if (iy > REFORM_END_YEAR)
	    style = negative_inf;
    }
    return style;
}

inline static VALUE
m_nth(union DateData *x)
{
    if (simple_dat_p(x))
	return x->s.nth;
    else {
	get_c_civil(x);
	return x->c.nth;
    }
}

inline static int
m_jd(union DateData *x)
{
    if (simple_dat_p(x)) {
	get_s_jd(x);
	return x->s.jd;
    }
    else {
	get_c_jd(x);
	return x->c.jd;
    }
}

static VALUE
m_real_jd(union DateData *x)
{
    VALUE nth, rjd;
    int jd;

    nth = m_nth(x);
    jd = m_jd(x);

    encode_jd(nth, jd, &rjd);
    return rjd;
}

static int
m_local_jd(union DateData *x)
{
    if (simple_dat_p(x)) {
	get_s_jd(x);
	return x->s.jd;
    }
    else {
	get_c_jd(x);
	get_c_df(x);
	return local_jd(x);
    }
}

static VALUE
m_real_local_jd(union DateData *x)
{
    VALUE nth, rjd;
    int jd;

    nth = m_nth(x);
    jd = m_local_jd(x);

    encode_jd(nth, jd, &rjd);
    return rjd;
}

inline static int
m_df(union DateData *x)
{
    if (simple_dat_p(x))
	return 0;
    else {
	get_c_df(x);
	return x->c.df;
    }
}

#ifndef NDEBUG
static VALUE
m_df_in_day(union DateData *x)
{
    return isec_to_day(m_df(x));
}
#endif

static int
m_local_df(union DateData *x)
{
    if (simple_dat_p(x))
	return 0;
    else {
	get_c_df(x);
	return local_df(x);
    }
}

#ifndef NDEBUG
static VALUE
m_local_df_in_day(union DateData *x)
{
    return isec_to_day(m_local_df(x));
}
#endif

inline static VALUE
m_sf(union DateData *x)
{
    if (simple_dat_p(x))
	return INT2FIX(0);
    else
	return x->c.sf;
}

#ifndef NDEBUG
static VALUE
m_sf_in_day(union DateData *x)
{
    return ns_to_day(m_sf(x));
}
#endif

static VALUE
m_sf_in_sec(union DateData *x)
{
    return ns_to_sec(m_sf(x));
}

static VALUE
m_fr(union DateData *x)
{
    if (simple_dat_p(x))
	return INT2FIX(0);
    else {
	int df;
	VALUE sf, fr;

	df = m_local_df(x);
	sf = m_sf(x);
	fr = isec_to_day(df);
	if (f_nonzero_p(sf))
	    fr = f_add(fr, ns_to_day(sf));
	return fr;
    }
}

#define HALF_DAYS_IN_SECONDS (DAY_IN_SECONDS / 2)

static VALUE
m_ajd(union DateData *x)
{
    VALUE r, sf;
    int df;

    if (simple_dat_p(x)) {
	r = m_real_jd(x);
	if (FIXNUM_P(r) && FIX2LONG(r) <= (FIXNUM_MAX / 2)) {
	    long ir = FIX2LONG(r);
	    ir = ir * 2 - 1;
	    return rb_rational_new2(LONG2FIX(ir), INT2FIX(2));
	}
	else
	    return rb_rational_new2(f_sub(f_mul(r,
						INT2FIX(2)),
					  INT2FIX(1)),
				    INT2FIX(2));
    }

    r = m_real_jd(x);
    df = m_df(x);
    df -= HALF_DAYS_IN_SECONDS;
    if (df)
	r = f_add(r, isec_to_day(df));
    sf = m_sf(x);
    if (f_nonzero_p(sf))
	r = f_add(r, ns_to_day(sf));

    return r;
}

static VALUE
m_amjd(union DateData *x)
{
    VALUE r, sf;
    int df;

    r = m_real_jd(x);
    if (FIXNUM_P(r) && FIX2LONG(r) >= (FIXNUM_MIN + 2400001)) {
	long ir = FIX2LONG(r);
	ir -= 2400001;
	r = rb_rational_new1(LONG2FIX(ir));
    }
    else
	r = rb_rational_new1(f_sub(m_real_jd(x),
				   INT2FIX(2400001)));

    if (simple_dat_p(x))
	return r;

    df = m_df(x);
    if (df)
	r = f_add(r, isec_to_day(df));
    sf = m_sf(x);
    if (f_nonzero_p(sf))
	r = f_add(r, ns_to_day(sf));

    return r;
}

inline static int
m_of(union DateData *x)
{
    if (simple_dat_p(x))
	return 0;
    else {
	get_c_jd(x);
	return x->c.of;
    }
}

static VALUE
m_of_in_day(union DateData *x)
{
    return isec_to_day(m_of(x));
}

inline static double
m_sg(union DateData *x)
{
    if (simple_dat_p(x))
	return x->s.sg;
    else {
	get_c_jd(x);
	return x->c.sg;
    }
}

static int
m_julian_p(union DateData *x)
{
    int jd;
    double sg;

    if (simple_dat_p(x)) {
	get_s_jd(x);
	jd = x->s.jd;
	sg = s_virtual_sg(x);
    }
    else {
	get_c_jd(x);
	jd = x->c.jd;
	sg = c_virtual_sg(x);
    }
    if (isinf(sg))
	return sg == positive_inf;
    return jd < sg;
}

inline static int
m_gregorian_p(union DateData *x)
{
    return !m_julian_p(x);
}

inline static int
m_proleptic_julian_p(union DateData *x)
{
    double sg;

    sg = m_sg(x);
    if (isinf(sg) && sg > 0)
	return 1;
    return 0;
}

inline static int
m_proleptic_gregorian_p(union DateData *x)
{
    double sg;

    sg = m_sg(x);
    if (isinf(sg) && sg < 0)
	return 1;
    return 0;
}

inline static int
m_year(union DateData *x)
{
    if (simple_dat_p(x)) {
	get_s_civil(x);
	return x->s.year;
    }
    else {
	get_c_civil(x);
	return x->c.year;
    }
}

static VALUE
m_real_year(union DateData *x)
{
    VALUE nth, ry;
    int year;

    nth = m_nth(x);
    year = m_year(x);

    if (f_zero_p(nth))
	return INT2FIX(year);

    encode_year(nth, year,
		m_gregorian_p(x) ? -1 : +1,
		&ry);
    return ry;
}


#ifdef USE_PACK
inline static int
m_pc(union DateData *x)
{
    if (simple_dat_p(x)) {
	get_s_civil(x);
	return x->s.pc;
    }
    else {
	get_c_civil(x);
	get_c_time(x);
	return x->c.pc;
    }
}
#endif

inline static int
m_mon(union DateData *x)
{
    if (simple_dat_p(x)) {
	get_s_civil(x);
#ifndef USE_PACK
	return x->s.mon;
#else
	return EX_MON(x->s.pc);
#endif
    }
    else {
	get_c_civil(x);
#ifndef USE_PACK
	return x->c.mon;
#else
	return EX_MON(x->c.pc);
#endif
    }
}

inline static int
m_mday(union DateData *x)
{
    if (simple_dat_p(x)) {
	get_s_civil(x);
#ifndef USE_PACK
	return x->s.mday;
#else
	return EX_MDAY(x->s.pc);
#endif
    }
    else {
	get_c_civil(x);
#ifndef USE_PACK
	return x->c.mday;
#else
	return EX_MDAY(x->c.pc);
#endif
    }
}

static const int yeartab[2][13] = {
    { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
    { 0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
};

static int
c_julian_to_yday(int y, int m, int d)
{
    assert(m >= 1 && m <= 12);
    return yeartab[c_julian_leap_p(y) ? 1 : 0][m] + d;
}

static int
c_gregorian_to_yday(int y, int m, int d)
{
    assert(m >= 1 && m <= 12);
    return yeartab[c_gregorian_leap_p(y) ? 1 : 0][m] + d;
}

static int
m_yday(union DateData *x)
{
    int jd, ry, rd;
    double sg;

    jd = m_local_jd(x);
    sg = m_virtual_sg(x); /* !=m_sg() */

    if (m_proleptic_gregorian_p(x) ||
	(jd - sg) > 366)
	return c_gregorian_to_yday(m_year(x), m_mon(x), m_mday(x));
    if (m_proleptic_julian_p(x))
	return c_julian_to_yday(m_year(x), m_mon(x), m_mday(x));
    c_jd_to_ordinal(jd, sg, &ry, &rd);
    return rd;
}

static int
m_wday(union DateData *x)
{
    return c_jd_to_wday(m_local_jd(x));
}

static int
m_cwyear(union DateData *x)
{
    int ry, rw, rd;

    c_jd_to_commercial(m_local_jd(x), m_virtual_sg(x), /* !=m_sg() */
		       &ry, &rw, &rd);
    return ry;
}

static VALUE
m_real_cwyear(union DateData *x)
{
    VALUE nth, ry;
    int year;

    nth = m_nth(x);
    year = m_cwyear(x);

    if (f_zero_p(nth))
	return INT2FIX(year);

    encode_year(nth, year,
		m_gregorian_p(x) ? -1 : +1,
		&ry);
    return ry;
}

static int
m_cweek(union DateData *x)
{
    int ry, rw, rd;

    c_jd_to_commercial(m_local_jd(x), m_virtual_sg(x), /* !=m_sg() */
		       &ry, &rw, &rd);
    return rw;
}

static int
m_cwday(union DateData *x)
{
    int w;

    w = m_wday(x);
    if (w == 0)
	w = 7;
    return w;
}

static int
m_wnumx(union DateData *x, int f)
{
    int ry, rw, rd;

    c_jd_to_weeknum(m_local_jd(x), f, m_virtual_sg(x), /* !=m_sg() */
		    &ry, &rw, &rd);
    return rw;
}

static int
m_wnum0(union DateData *x)
{
    return m_wnumx(x, 0);
}

static int
m_wnum1(union DateData *x)
{
    return m_wnumx(x, 1);
}

inline static int
m_hour(union DateData *x)
{
    if (simple_dat_p(x))
	return 0;
    else {
	get_c_time(x);
#ifndef USE_PACK
	return x->c.hour;
#else
	return EX_HOUR(x->c.pc);
#endif
    }
}

inline static int
m_min(union DateData *x)
{
    if (simple_dat_p(x))
	return 0;
    else {
	get_c_time(x);
#ifndef USE_PACK
	return x->c.min;
#else
	return EX_MIN(x->c.pc);
#endif
    }
}

inline static int
m_sec(union DateData *x)
{
    if (simple_dat_p(x))
	return 0;
    else {
	get_c_time(x);
#ifndef USE_PACK
	return x->c.sec;
#else
	return EX_SEC(x->c.pc);
#endif
    }
}

#define decode_offset(of,s,h,m)\
{\
    int a;\
    s = (of < 0) ? '-' : '+';\
    a = (of < 0) ? -of : of;\
    h = a / HOUR_IN_SECONDS;\
    m = a % HOUR_IN_SECONDS / MINUTE_IN_SECONDS;\
}

static VALUE
of2str(int of)
{
    int s, h, m;

    decode_offset(of, s, h, m);
    return rb_enc_sprintf(rb_usascii_encoding(), "%c%02d:%02d", s, h, m);
}

static VALUE
m_zone(union DateData *x)
{
    if (simple_dat_p(x))
	return rb_usascii_str_new2("+00:00");
    return of2str(m_of(x));
}

inline static VALUE
f_kind_of_p(VALUE x, VALUE c)
{
    return rb_obj_is_kind_of(x, c);
}

inline static VALUE
k_date_p(VALUE x)
{
    return f_kind_of_p(x, cDate);
}

inline static VALUE
k_datetime_p(VALUE x)
{
    return f_kind_of_p(x, cDateTime);
}

inline static VALUE
k_numeric_p(VALUE x)
{
    return f_kind_of_p(x, rb_cNumeric);
}

inline static VALUE
k_rational_p(VALUE x)
{
    return f_kind_of_p(x, rb_cRational);
}

#ifndef NDEBUG
static void
civil_to_jd(VALUE y, int m, int d, double sg,
	    VALUE *nth, int *ry,
	    int *rjd,
	    int *ns)
{
    double style = guess_style(y, sg);

    if (style == 0) {
	int jd;

	c_civil_to_jd(FIX2INT(y), m, d, sg, &jd, ns);
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	c_civil_to_jd(*ry, m, d, style, rjd, ns);
    }
}

static void
jd_to_civil(VALUE jd, double sg,
	    VALUE *nth, int *rjd,
	    int *ry, int *rm, int *rd)
{
    decode_jd(jd, nth, rjd);
    c_jd_to_civil(*rjd, sg, ry, rm, rd);
}

static void
ordinal_to_jd(VALUE y, int d, double sg,
	      VALUE *nth, int *ry,
	      int *rjd,
	      int *ns)
{
    double style = guess_style(y, sg);

    if (style == 0) {
	int jd;

	c_ordinal_to_jd(FIX2INT(y), d, sg, &jd, ns);
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	c_ordinal_to_jd(*ry, d, style, rjd, ns);
    }
}

static void
jd_to_ordinal(VALUE jd, double sg,
	      VALUE *nth, int *rjd,
	      int *ry, int *rd)
{
    decode_jd(jd, nth, rjd);
    c_jd_to_ordinal(*rjd, sg, ry, rd);
}

static void
commercial_to_jd(VALUE y, int w, int d, double sg,
		 VALUE *nth, int *ry,
		 int *rjd,
		 int *ns)
{
    double style = guess_style(y, sg);

    if (style == 0) {
	int jd;

	c_commercial_to_jd(FIX2INT(y), w, d, sg, &jd, ns);
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	c_commercial_to_jd(*ry, w, d, style, rjd, ns);
    }
}

static void
jd_to_commercial(VALUE jd, double sg,
		 VALUE *nth, int *rjd,
		 int *ry, int *rw, int *rd)
{
    decode_jd(jd, nth, rjd);
    c_jd_to_commercial(*rjd, sg, ry, rw, rd);
}

static void
weeknum_to_jd(VALUE y, int w, int d, int f, double sg,
	      VALUE *nth, int *ry,
	      int *rjd,
	      int *ns)
{
    double style = guess_style(y, sg);

    if (style == 0) {
	int jd;

	c_weeknum_to_jd(FIX2INT(y), w, d, f, sg, &jd, ns);
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	c_weeknum_to_jd(*ry, w, d, f, style, rjd, ns);
    }
}

static void
jd_to_weeknum(VALUE jd, int f, double sg,
	      VALUE *nth, int *rjd,
	      int *ry, int *rw, int *rd)
{
    decode_jd(jd, nth, rjd);
    c_jd_to_weeknum(*rjd, f, sg, ry, rw, rd);
}

static void
nth_kday_to_jd(VALUE y, int m, int n, int k, double sg,
	       VALUE *nth, int *ry,
	       int *rjd,
	       int *ns)
{
    double style = guess_style(y, sg);

    if (style == 0) {
	int jd;

	c_nth_kday_to_jd(FIX2INT(y), m, n, k, sg, &jd, ns);
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	c_nth_kday_to_jd(*ry, m, n, k, style, rjd, ns);
    }
}

static void
jd_to_nth_kday(VALUE jd, double sg,
	       VALUE *nth, int *rjd,
	       int *ry, int *rm, int *rn, int *rk)
{
    decode_jd(jd, nth, rjd);
    c_jd_to_nth_kday(*rjd, sg, ry, rm, rn, rk);
}
#endif

static int
valid_ordinal_p(VALUE y, int d, double sg,
		VALUE *nth, int *ry,
		int *rd, int *rjd,
		int *ns)
{
    double style = guess_style(y, sg);
    int r;

    if (style == 0) {
	int jd;

	r = c_valid_ordinal_p(FIX2INT(y), d, sg, rd, &jd, ns);
	if (!r)
	    return 0;
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	r = c_valid_ordinal_p(*ry, d, style, rd, rjd, ns);
    }
    return r;
}

static int
valid_gregorian_p(VALUE y, int m, int d,
		  VALUE *nth, int *ry,
		  int *rm, int *rd)
{
    decode_year(y, -1, nth, ry);
    return c_valid_gregorian_p(*ry, m, d, rm, rd);
}

static int
valid_civil_p(VALUE y, int m, int d, double sg,
	      VALUE *nth, int *ry,
	      int *rm, int *rd, int *rjd,
	      int *ns)
{
    double style = guess_style(y, sg);
    int r;

    if (style == 0) {
	int jd;

	r = c_valid_civil_p(FIX2INT(y), m, d, sg, rm, rd, &jd, ns);
	if (!r)
	    return 0;
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	if (style < 0)
	    r = c_valid_gregorian_p(*ry, m, d, rm, rd);
	else
	    r = c_valid_julian_p(*ry, m, d, rm, rd);
	if (!r)
	    return 0;
	c_civil_to_jd(*ry, *rm, *rd, style, rjd, ns);
    }
    return r;
}

static int
valid_commercial_p(VALUE y, int w, int d, double sg,
		   VALUE *nth, int *ry,
		   int *rw, int *rd, int *rjd,
		   int *ns)
{
    double style = guess_style(y, sg);
    int r;

    if (style == 0) {
	int jd;

	r = c_valid_commercial_p(FIX2INT(y), w, d, sg, rw, rd, &jd, ns);
	if (!r)
	    return 0;
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	r = c_valid_commercial_p(*ry, w, d, style, rw, rd, rjd, ns);
    }
    return r;
}

static int
valid_weeknum_p(VALUE y, int w, int d, int f, double sg,
		VALUE *nth, int *ry,
		int *rw, int *rd, int *rjd,
		int *ns)
{
    double style = guess_style(y, sg);
    int r;

    if (style == 0) {
	int jd;

	r = c_valid_weeknum_p(FIX2INT(y), w, d, f, sg, rw, rd, &jd, ns);
	if (!r)
	    return 0;
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	r = c_valid_weeknum_p(*ry, w, d, f, style, rw, rd, rjd, ns);
    }
    return r;
}

#ifndef NDEBUG
static int
valid_nth_kday_p(VALUE y, int m, int n, int k, double sg,
		 VALUE *nth, int *ry,
		 int *rm, int *rn, int *rk, int *rjd,
		 int *ns)
{
    double style = guess_style(y, sg);
    int r;

    if (style == 0) {
	int jd;

	r = c_valid_nth_kday_p(FIX2INT(y), m, n, k, sg, rm, rn, rk, &jd, ns);
	if (!r)
	    return 0;
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	r = c_valid_nth_kday_p(*ry, m, n, k, style, rm, rn, rk, rjd, ns);
    }
    return r;
}
#endif

VALUE date_zone_to_diff(VALUE);

static int
offset_to_sec(VALUE vof, int *rof)
{
    switch (TYPE(vof)) {
      case T_FIXNUM:
	{
	    long n;

	    n = FIX2LONG(vof);
	    if (n != -1 && n != 0 && n != 1)
		return 0;
	    *rof = (int)n * DAY_IN_SECONDS;
	    return 1;
	}
      case T_FLOAT:
	{
	    double n;

	    n = RFLOAT_VALUE(vof) * DAY_IN_SECONDS;
	    if (n < -DAY_IN_SECONDS || n > DAY_IN_SECONDS)
		return 0;
	    *rof = (int)round(n);
	    if (*rof != n)
		rb_warning("fraction of offset is ignored");
	    return 1;
	}
      default:
	if (!k_numeric_p(vof))
	    rb_raise(rb_eTypeError, "expected numeric");
	vof = f_to_r(vof);
#ifdef CANONICALIZATION_FOR_MATHN
	if (!k_rational_p(vof))
	    return offset_to_sec(vof, rof);
#endif
	/* fall through */
      case T_RATIONAL:
	{
	    VALUE vs, vn, vd;
	    long n;

	    vs = day_to_sec(vof);

#ifdef CANONICALIZATION_FOR_MATHN
	    if (!k_rational_p(vs)) {
		if (!FIXNUM_P(vs))
		    return 0;
		n = FIX2LONG(vs);
		if (n < -DAY_IN_SECONDS || n > DAY_IN_SECONDS)
		    return 0;
		*rof = (int)n;
		return 1;
	    }
#endif
	    vn = RRATIONAL(vs)->num;
	    vd = RRATIONAL(vs)->den;

	    if (FIXNUM_P(vn) && FIXNUM_P(vd) && (FIX2LONG(vd) == 1))
		n = FIX2LONG(vn);
	    else {
		vn = f_round(vs);
		if (!f_eqeq_p(vn, vs))
		    rb_warning("fraction of offset is ignored");
		if (!FIXNUM_P(vn))
		    return 0;
		n = FIX2LONG(vn);
		if (n < -DAY_IN_SECONDS || n > DAY_IN_SECONDS)
		    return 0;
	    }
	    *rof = (int)n;
	    return 1;
	}
      case T_STRING:
	{
	    VALUE vs = date_zone_to_diff(vof);
	    long n;

	    if (!FIXNUM_P(vs))
		return 0;
	    n = FIX2LONG(vs);
	    if (n < -DAY_IN_SECONDS || n > DAY_IN_SECONDS)
		return 0;
	    *rof = (int)n;
	    return 1;
	}
    }
    return 0;
}

/* date */

#define valid_sg(sg) \
{\
    if (!c_valid_start_p(sg)) {\
	sg = 0;\
	rb_warning("invalid start is ignored");\
    }\
}

static VALUE
valid_jd_sub(int argc, VALUE *argv, VALUE klass, int need_jd)
{
    double sg = NUM2DBL(argv[1]);
    valid_sg(sg);
    return argv[0];
}

#ifndef NDEBUG
static VALUE
date_s__valid_jd_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vjd, vsg;
    VALUE argv2[2];

    rb_scan_args(argc, argv, "11", &vjd, &vsg);

    argv2[0] = vjd;
    if (argc < 2)
	argv2[1] = DBL2NUM(GREGORIAN);
    else
	argv2[1] = vsg;

    return valid_jd_sub(2, argv2, klass, 1);
}
#endif

/*
 * call-seq:
 *    Date.valid_jd?(jd[, start=Date::ITALY])  ->  bool
 *
 * Just returns true.  It's nonsense, but is for symmetry.
 *
 * For example:
 *
 *    Date.valid_jd?(2451944)		#=> true
 *
 * See also jd.
 */
static VALUE
date_s_valid_jd_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vjd, vsg;
    VALUE argv2[2];

    rb_scan_args(argc, argv, "11", &vjd, &vsg);

    argv2[0] = vjd;
    if (argc < 2)
	argv2[1] = INT2FIX(DEFAULT_SG);
    else
	argv2[1] = vsg;

    if (NIL_P(valid_jd_sub(2, argv2, klass, 0)))
	return Qfalse;
    return Qtrue;
}

static VALUE
valid_civil_sub(int argc, VALUE *argv, VALUE klass, int need_jd)
{
    VALUE nth, y;
    int m, d, ry, rm, rd;
    double sg;

    y = argv[0];
    m = NUM2INT(argv[1]);
    d = NUM2INT(argv[2]);
    sg = NUM2DBL(argv[3]);

    valid_sg(sg);

    if (!need_jd && (guess_style(y, sg) < 0)) {
	if (!valid_gregorian_p(y, m, d,
			       &nth, &ry,
			       &rm, &rd))
	    return Qnil;
	return INT2FIX(0); /* dummy */
    }
    else {
	int rjd, ns;
	VALUE rjd2;

	if (!valid_civil_p(y, m, d, sg,
			   &nth, &ry,
			   &rm, &rd, &rjd,
			   &ns))
	    return Qnil;
	if (!need_jd)
	    return INT2FIX(0); /* dummy */
	encode_jd(nth, rjd, &rjd2);
	return rjd2;
    }
}

#ifndef NDEBUG
static VALUE
date_s__valid_civil_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vm, vd, vsg;
    VALUE argv2[4];

    rb_scan_args(argc, argv, "31", &vy, &vm, &vd, &vsg);

    argv2[0] = vy;
    argv2[1] = vm;
    argv2[2] = vd;
    if (argc < 4)
	argv2[3] = DBL2NUM(GREGORIAN);
    else
	argv2[3] = vsg;

    return valid_civil_sub(4, argv2, klass, 1);
}
#endif

/*
 * call-seq:
 *    Date.valid_civil?(year, month, mday[, start=Date::ITALY])  ->  bool
 *    Date.valid_date?(year, month, mday[, start=Date::ITALY])   ->  bool
 *
 * Returns true if the given calendar date is valid, and false if not.
 *
 * For example:
 *
 *    Date.valid_date?(2001,2,3)	#=> true
 *    Date.valid_date?(2001,2,29)	#=> false
 *
 * See also jd and civil.
 */
static VALUE
date_s_valid_civil_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vm, vd, vsg;
    VALUE argv2[4];

    rb_scan_args(argc, argv, "31", &vy, &vm, &vd, &vsg);

    argv2[0] = vy;
    argv2[1] = vm;
    argv2[2] = vd;
    if (argc < 4)
	argv2[3] = INT2FIX(DEFAULT_SG);
    else
	argv2[3] = vsg;

    if (NIL_P(valid_civil_sub(4, argv2, klass, 0)))
	return Qfalse;
    return Qtrue;
}

static VALUE
valid_ordinal_sub(int argc, VALUE *argv, VALUE klass, int need_jd)
{
    VALUE nth, y;
    int d, ry, rd;
    double sg;

    y = argv[0];
    d = NUM2INT(argv[1]);
    sg = NUM2DBL(argv[2]);

    valid_sg(sg);

    {
	int rjd, ns;
	VALUE rjd2;

	if (!valid_ordinal_p(y, d, sg,
			     &nth, &ry,
			     &rd, &rjd,
			     &ns))
	    return Qnil;
	if (!need_jd)
	    return INT2FIX(0); /* dummy */
	encode_jd(nth, rjd, &rjd2);
	return rjd2;
    }
}

#ifndef NDEBUG
static VALUE
date_s__valid_ordinal_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vd, vsg;
    VALUE argv2[3];

    rb_scan_args(argc, argv, "21", &vy, &vd, &vsg);

    argv2[0] = vy;
    argv2[1] = vd;
    if (argc < 3)
	argv2[2] = DBL2NUM(GREGORIAN);
    else
	argv2[2] = vsg;

    return valid_ordinal_sub(3, argv2, klass, 1);
}
#endif

/*
 * call-seq:
 *    Date.valid_ordinal?(year, yday[, start=Date::ITALY])  ->  bool
 *
 * Returns true if the given ordinal date is valid, and false if not.
 *
 * For example:
 *
 *    Date.valid_ordinal?(2001,34)	#=> true
 *    Date.valid_ordinal?(2001,366)	#=> false
 *
 * See also jd and ordinal.
 */
static VALUE
date_s_valid_ordinal_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vd, vsg;
    VALUE argv2[3];

    rb_scan_args(argc, argv, "21", &vy, &vd, &vsg);

    argv2[0] = vy;
    argv2[1] = vd;
    if (argc < 3)
	argv2[2] = INT2FIX(DEFAULT_SG);
    else
	argv2[2] = vsg;

    if (NIL_P(valid_ordinal_sub(3, argv2, klass, 0)))
	return Qfalse;
    return Qtrue;
}

static VALUE
valid_commercial_sub(int argc, VALUE *argv, VALUE klass, int need_jd)
{
    VALUE nth, y;
    int w, d, ry, rw, rd;
    double sg;

    y = argv[0];
    w = NUM2INT(argv[1]);
    d = NUM2INT(argv[2]);
    sg = NUM2DBL(argv[3]);

    valid_sg(sg);

    {
	int rjd, ns;
	VALUE rjd2;

	if (!valid_commercial_p(y, w, d, sg,
				&nth, &ry,
				&rw, &rd, &rjd,
				&ns))
	    return Qnil;
	if (!need_jd)
	    return INT2FIX(0); /* dummy */
	encode_jd(nth, rjd, &rjd2);
	return rjd2;
    }
}

#ifndef NDEBUG
static VALUE
date_s__valid_commercial_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vw, vd, vsg;
    VALUE argv2[4];

    rb_scan_args(argc, argv, "31", &vy, &vw, &vd, &vsg);

    argv2[0] = vy;
    argv2[1] = vw;
    argv2[2] = vd;
    if (argc < 4)
	argv2[3] = DBL2NUM(GREGORIAN);
    else
	argv2[3] = vsg;

    return valid_commercial_sub(4, argv2, klass, 1);
}
#endif

/*
 * call-seq:
 *    Date.valid_commercial?(cwyear, cweek, cwday[, start=Date::ITALY])  ->  bool
 *
 * Returns true if the given week date is valid, and false if not.
 *
 * For example:
 *
 *    Date.valid_commercial?(2001,5,6)	#=> true
 *    Date.valid_commercial?(2001,5,8)	#=> false
 *
 * See also jd and commercial.
 */
static VALUE
date_s_valid_commercial_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vw, vd, vsg;
    VALUE argv2[4];

    rb_scan_args(argc, argv, "31", &vy, &vw, &vd, &vsg);

    argv2[0] = vy;
    argv2[1] = vw;
    argv2[2] = vd;
    if (argc < 4)
	argv2[3] = INT2FIX(DEFAULT_SG);
    else
	argv2[3] = vsg;

    if (NIL_P(valid_commercial_sub(4, argv2, klass, 0)))
	return Qfalse;
    return Qtrue;
}

#ifndef NDEBUG
static VALUE
valid_weeknum_sub(int argc, VALUE *argv, VALUE klass, int need_jd)
{
    VALUE nth, y;
    int w, d, f, ry, rw, rd;
    double sg;

    y = argv[0];
    w = NUM2INT(argv[1]);
    d = NUM2INT(argv[2]);
    f = NUM2INT(argv[3]);
    sg = NUM2DBL(argv[4]);

    valid_sg(sg);

    {
	int rjd, ns;
	VALUE rjd2;

	if (!valid_weeknum_p(y, w, d, f, sg,
			     &nth, &ry,
			     &rw, &rd, &rjd,
			     &ns))
	    return Qnil;
	if (!need_jd)
	    return INT2FIX(0); /* dummy */
	encode_jd(nth, rjd, &rjd2);
	return rjd2;
    }
}

static VALUE
date_s__valid_weeknum_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vw, vd, vf, vsg;
    VALUE argv2[5];

    rb_scan_args(argc, argv, "41", &vy, &vw, &vd, &vf, &vsg);

    argv2[0] = vy;
    argv2[1] = vw;
    argv2[2] = vd;
    argv2[3] = vf;
    if (argc < 5)
	argv2[4] = DBL2NUM(GREGORIAN);
    else
	argv2[4] = vsg;

    return valid_weeknum_sub(5, argv2, klass, 1);
}

static VALUE
date_s_valid_weeknum_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vw, vd, vf, vsg;
    VALUE argv2[5];

    rb_scan_args(argc, argv, "41", &vy, &vw, &vd, &vf, &vsg);

    argv2[0] = vy;
    argv2[1] = vw;
    argv2[2] = vd;
    argv2[3] = vf;
    if (argc < 5)
	argv2[4] = INT2FIX(DEFAULT_SG);
    else
	argv2[4] = vsg;

    if (NIL_P(valid_weeknum_sub(5, argv2, klass, 0)))
	return Qfalse;
    return Qtrue;
}

static VALUE
valid_nth_kday_sub(int argc, VALUE *argv, VALUE klass, int need_jd)
{
    VALUE nth, y;
    int m, n, k, ry, rm, rn, rk;
    double sg;

    y = argv[0];
    m = NUM2INT(argv[1]);
    n = NUM2INT(argv[2]);
    k = NUM2INT(argv[3]);
    sg = NUM2DBL(argv[4]);

    {
	int rjd, ns;
	VALUE rjd2;

	if (!valid_nth_kday_p(y, m, n, k, sg,
			      &nth, &ry,
			      &rm, &rn, &rk, &rjd,
			      &ns))
	    return Qnil;
	if (!need_jd)
	    return INT2FIX(0); /* dummy */
	encode_jd(nth, rjd, &rjd2);
	return rjd2;
    }
}

static VALUE
date_s__valid_nth_kday_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vm, vn, vk, vsg;
    VALUE argv2[5];

    rb_scan_args(argc, argv, "41", &vy, &vm, &vn, &vk, &vsg);

    argv2[0] = vy;
    argv2[1] = vm;
    argv2[2] = vn;
    argv2[3] = vk;
    if (argc < 5)
	argv2[4] = DBL2NUM(GREGORIAN);
    else
	argv2[4] = vsg;

    return valid_nth_kday_sub(5, argv2, klass, 1);
}

static VALUE
date_s_valid_nth_kday_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vm, vn, vk, vsg;
    VALUE argv2[5];

    rb_scan_args(argc, argv, "41", &vy, &vm, &vn, &vk, &vsg);

    argv2[0] = vy;
    argv2[1] = vm;
    argv2[2] = vn;
    argv2[3] = vk;
    if (argc < 5)
	argv2[4] = INT2FIX(DEFAULT_SG);
    else
	argv2[4] = vsg;

    if (NIL_P(valid_nth_kday_sub(5, argv2, klass, 0)))
	return Qfalse;
    return Qtrue;
}

static VALUE
date_s_zone_to_diff(VALUE klass, VALUE str)
{
    return date_zone_to_diff(str);
}
#endif

/*
 * call-seq:
 *    Date.julian_leap?(year)  ->  bool
 *
 * Returns true if the given year is a leap year of the proleptic
 * Julian calendar.
 *
 * For example:
 *
 *    Date.julian_leap?(1900)		#=> true
 *    Date.julian_leap?(1901)		#=> false
 */
static VALUE
date_s_julian_leap_p(VALUE klass, VALUE y)
{
    VALUE nth;
    int ry;

    decode_year(y, +1, &nth, &ry);
    return f_boolcast(c_julian_leap_p(ry));
}

/*
 * call-seq:
 *    Date.gregorian_leap?(year)  ->  bool
 *    Date.leap?(year)            ->  bool
 *
 * Returns true if the given year is a leap year of the proleptic
 * Gregorian calendar.
 *
 * For example:
 *
 *    Date.gregorian_leap?(1900)	#=> false
 *    Date.gregorian_leap?(2000)	#=> true
 */
static VALUE
date_s_gregorian_leap_p(VALUE klass, VALUE y)
{
    VALUE nth;
    int ry;

    decode_year(y, -1, &nth, &ry);
    return f_boolcast(c_gregorian_leap_p(ry));
}

static void
d_lite_gc_mark(union DateData *dat)
{
    if (simple_dat_p(dat))
	rb_gc_mark(dat->s.nth);
    else {
	rb_gc_mark(dat->c.nth);
	rb_gc_mark(dat->c.sf);

    }
}

inline static VALUE
d_simple_new_internal(VALUE klass,
		      VALUE nth, int jd,
		      double sg,
		      int y, int m, int d,
		      unsigned flags)
{
    struct SimpleDateData *dat;
    VALUE obj;

    obj = Data_Make_Struct(klass, struct SimpleDateData,
			   d_lite_gc_mark, -1, dat);
    set_to_simple(dat, nth, jd, sg, y, m, d, flags & ~COMPLEX_DAT);

    assert(have_jd_p(dat) || have_civil_p(dat));

    return obj;
}

inline static VALUE
d_complex_new_internal(VALUE klass,
		       VALUE nth, int jd,
		       int df, VALUE sf,
		       int of, double sg,
		       int y, int m, int d,
		       int h, int min, int s,
		       unsigned flags)
{
    struct ComplexDateData *dat;
    VALUE obj;

    obj = Data_Make_Struct(klass, struct ComplexDateData,
			   d_lite_gc_mark, -1, dat);
    set_to_complex(dat, nth, jd, df, sf, of, sg,
		   y, m, d, h, min, s, flags | COMPLEX_DAT);

    assert(have_jd_p(dat) || have_civil_p(dat));
    assert(have_df_p(dat) || have_time_p(dat));

    return obj;
}

static VALUE
d_lite_s_alloc_simple(VALUE klass)
{
    return d_simple_new_internal(klass,
				 INT2FIX(0), 0,
				 DEFAULT_SG,
				 0, 0, 0,
				 HAVE_JD);
}

static VALUE
d_lite_s_alloc_complex(VALUE klass)
{
    return d_complex_new_internal(klass,
				  INT2FIX(0), 0,
				  0, INT2FIX(0),
				  0, DEFAULT_SG,
				  0, 0, 0,
				  0, 0, 0,
				  HAVE_JD | HAVE_DF);
}

static VALUE
d_lite_s_alloc(VALUE klass)
{
    return d_lite_s_alloc_complex(klass);
}

static void
old_to_new(VALUE ajd, VALUE of, VALUE sg,
	   VALUE *rnth, int *rjd, int *rdf, VALUE *rsf,
	   int *rof, double *rsg)
{
    VALUE jd, df, sf, of2, t;

    decode_day(f_add(ajd, half_days_in_day),
	       &jd, &df, &sf);
    t = day_to_sec(of);
    of2 = f_round(t);

    if (!f_eqeq_p(of2, t))
	rb_warning("fraction of offset is ignored");

    decode_jd(jd, rnth, rjd);

    *rdf = NUM2INT(df);
    *rsf = sf;
    *rof = NUM2INT(of2);
    *rsg = NUM2DBL(sg);

    if (*rdf < 0 || *rdf >= DAY_IN_SECONDS)
	rb_raise(rb_eArgError, "invalid day fraction");

    if (f_lt_p(*rsf, INT2FIX(0)) ||
	f_ge_p(*rsf, INT2FIX(SECOND_IN_NANOSECONDS)))

    if (*rof < -DAY_IN_SECONDS || *rof > DAY_IN_SECONDS) {
	*rof = 0;
	rb_warning("invalid offset is ignored");
    }

    if (!c_valid_start_p(*rsg)) {
	*rsg = DEFAULT_SG;
	rb_warning("invalid start is ignored");
    }
}

#ifndef NDEBUG
static VALUE
date_s_new_bang(int argc, VALUE *argv, VALUE klass)
{
    VALUE ajd, of, sg, nth, sf;
    int jd, df, rof;
    double rsg;

    rb_scan_args(argc, argv, "03", &ajd, &of, &sg);

    switch (argc) {
      case 0:
	ajd = INT2FIX(0);
      case 1:
	of = INT2FIX(0);
      case 2:
	sg = INT2FIX(DEFAULT_SG);
    }

    old_to_new(ajd, of, sg,
	       &nth, &jd, &df, &sf, &rof, &rsg);

    if (!df && f_zero_p(sf) && !rof)
	return d_simple_new_internal(klass,
				     nth, jd,
				     rsg,
				     0, 0, 0,
				     HAVE_JD);
    else
	return d_complex_new_internal(klass,
				      nth, jd,
				      df, sf,
				      rof, rsg,
				      0, 0, 0,
				      0, 0, 0,
				      HAVE_JD | HAVE_DF);
}
#endif

inline static int
wholenum_p(VALUE x)
{
    if (FIXNUM_P(x))
	return 1;
    switch (TYPE(x)) {
      case T_BIGNUM:
	return 1;
      case T_FLOAT:
	{
	    double d = RFLOAT_VALUE(x);
	    return round(d) == d;
	}
	break;
      case T_RATIONAL:
	{
	    VALUE den = RRATIONAL(x)->den;
	    return FIXNUM_P(den) && FIX2LONG(den) == 1;
	}
	break;
    }
    return 0;
}

inline static VALUE
to_integer(VALUE x)
{
    if (FIXNUM_P(x) || RB_TYPE_P(x, T_BIGNUM))
	return x;
    return f_to_i(x);
}

inline static VALUE
d_trunc(VALUE d, VALUE *fr)
{
    VALUE rd;

    if (wholenum_p(d)) {
	rd = to_integer(d);
	*fr = INT2FIX(0);
    }
    else {
	rd = f_idiv(d, INT2FIX(1));
	*fr = f_mod(d, INT2FIX(1));
    }
    return rd;
}

#define jd_trunc d_trunc
#define k_trunc d_trunc

inline static VALUE
h_trunc(VALUE h, VALUE *fr)
{
    VALUE rh;

    if (wholenum_p(h)) {
	rh = to_integer(h);
	*fr = INT2FIX(0);
    }
    else {
	rh = f_idiv(h, INT2FIX(1));
	*fr = f_mod(h, INT2FIX(1));
	*fr = f_quo(*fr, INT2FIX(24));
    }
    return rh;
}

inline static VALUE
min_trunc(VALUE min, VALUE *fr)
{
    VALUE rmin;

    if (wholenum_p(min)) {
	rmin = to_integer(min);
	*fr = INT2FIX(0);
    }
    else {
	rmin = f_idiv(min, INT2FIX(1));
	*fr = f_mod(min, INT2FIX(1));
	*fr = f_quo(*fr, INT2FIX(1440));
    }
    return rmin;
}

inline static VALUE
s_trunc(VALUE s, VALUE *fr)
{
    VALUE rs;

    if (wholenum_p(s)) {
	rs = to_integer(s);
	*fr = INT2FIX(0);
    }
    else {
	rs = f_idiv(s, INT2FIX(1));
	*fr = f_mod(s, INT2FIX(1));
	*fr = f_quo(*fr, INT2FIX(86400));
    }
    return rs;
}

#define num2num_with_frac(s,n) \
{\
    s = s##_trunc(v##s, &fr);\
    if (f_nonzero_p(fr)) {\
	if (argc > n)\
	    rb_raise(rb_eArgError, "invalid fraction");\
	fr2 = fr;\
    }\
}

#define num2int_with_frac(s,n) \
{\
    s = NUM2INT(s##_trunc(v##s, &fr));\
    if (f_nonzero_p(fr)) {\
	if (argc > n)\
	    rb_raise(rb_eArgError, "invalid fraction");\
	fr2 = fr;\
    }\
}

#define canon24oc() \
{\
    if (rh == 24) {\
	rh = 0;\
	fr2 = f_add(fr2, INT2FIX(1));\
    }\
}

#define add_frac() \
{\
    if (f_nonzero_p(fr2))\
	ret = d_lite_plus(ret, fr2);\
}

#define val2sg(vsg,dsg) \
{\
    dsg = NUM2DBL(vsg);\
    if (!c_valid_start_p(dsg)) {\
	dsg = DEFAULT_SG;\
	rb_warning("invalid start is ignored");\
    }\
}

static VALUE d_lite_plus(VALUE, VALUE);

/*
 * call-seq:
 *    Date.jd([jd=0[, start=Date::ITALY]])  ->  date
 *
 * Creates a date object denoting the given chronological Julian day
 * number.
 *
 * For example:
 *
 *    Date.jd(2451944)		#=> #<Date: 2001-02-03 ...>
 *    Date.jd(2451945)		#=> #<Date: 2001-02-04 ...>
 *    Date.jd(0)		#=> #<Date: -4712-01-01 ...>
 *
 * See also new.
 */
static VALUE
date_s_jd(int argc, VALUE *argv, VALUE klass)
{
    VALUE vjd, vsg, jd, fr, fr2, ret;
    double sg;

    rb_scan_args(argc, argv, "02", &vjd, &vsg);

    jd = INT2FIX(0);
    fr2 = INT2FIX(0);
    sg = DEFAULT_SG;

    switch (argc) {
      case 2:
	val2sg(vsg, sg);
      case 1:
	num2num_with_frac(jd, positive_inf);
    }

    {
	VALUE nth;
	int rjd;

	decode_jd(jd, &nth, &rjd);
	ret = d_simple_new_internal(klass,
				    nth, rjd,
				    sg,
				    0, 0, 0,
				    HAVE_JD);
    }
    add_frac();
    return ret;
}

/*
 * call-seq:
 *    Date.ordinal([year=-4712[, yday=1[, start=Date::ITALY]]])  ->  date
 *
 * Creates a date object denoting the given ordinal date.
 *
 * The day of year should be a negative or a positive number (as a
 * relative day from the end of year when negative).  It should not be
 * zero.
 *
 * For example:
 *
 *    Date.ordinal(2001)	#=> #<Date: 2001-01-01 ...>
 *    Date.ordinal(2001,34)	#=> #<Date: 2001-02-03 ...>
 *    Date.ordinal(2001,-1)	#=> #<Date: 2001-12-31 ...>
 *
 * See also jd and new.
 */
static VALUE
date_s_ordinal(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vd, vsg, y, fr, fr2, ret;
    int d;
    double sg;

    rb_scan_args(argc, argv, "03", &vy, &vd, &vsg);

    y = INT2FIX(-4712);
    d = 1;
    fr2 = INT2FIX(0);
    sg = DEFAULT_SG;

    switch (argc) {
      case 3:
	val2sg(vsg, sg);
      case 2:
	num2int_with_frac(d, positive_inf);
      case 1:
	y = vy;
    }

    {
	VALUE nth;
	int ry, rd, rjd, ns;

	if (!valid_ordinal_p(y, d, sg,
			     &nth, &ry,
			     &rd, &rjd,
			     &ns))
	    rb_raise(rb_eArgError, "invalid date");

	ret = d_simple_new_internal(klass,
				     nth, rjd,
				     sg,
				     0, 0, 0,
				     HAVE_JD);
    }
    add_frac();
    return ret;
}

/*
 * call-seq:
 *    Date.civil([year=-4712[, month=1[, mday=1[, start=Date::ITALY]]]])  ->  date
 *    Date.new([year=-4712[, month=1[, mday=1[, start=Date::ITALY]]]])    ->  date
 *
 * Creates a date object denoting the given calendar date.
 *
 * In this class, BCE years are counted astronomically.  Thus, the
 * year before the year 1 is the year zero, and the year preceding the
 * year zero is the year -1.  The month and the day of month should be
 * a negative or a positive number (as a relative month/day from the
 * end of year/month when negative).  They should not be zero.
 *
 * The last argument should be a Julian day number which denotes the
 * day of calendar reform.  Date::ITALY (2299161=1582-10-15),
 * Date::ENGLAND (2361222=1752-09-14), Date::GREGORIAN (the proleptic
 * Gregorian calendar) and Date::JULIAN (the proleptic Julian
 * calendar) can be specified as a day of calendar reform.
 *
 * For example:
 *
 *    Date.new(2001)		#=> #<Date: 2001-01-01 ...>
 *    Date.new(2001,2,3)	#=> #<Date: 2001-02-03 ...>
 *    Date.new(2001,2,-1)	#=> #<Date: 2001-02-28 ...>
 *
 * See also jd.
 */
static VALUE
date_s_civil(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vm, vd, vsg, y, fr, fr2, ret;
    int m, d;
    double sg;

    rb_scan_args(argc, argv, "04", &vy, &vm, &vd, &vsg);

    y = INT2FIX(-4712);
    m = 1;
    d = 1;
    fr2 = INT2FIX(0);
    sg = DEFAULT_SG;

    switch (argc) {
      case 4:
	val2sg(vsg, sg);
      case 3:
	num2int_with_frac(d, positive_inf);
      case 2:
	m = NUM2INT(vm);
      case 1:
	y = vy;
    }

    if (guess_style(y, sg) < 0) {
	VALUE nth;
	int ry, rm, rd;

	if (!valid_gregorian_p(y, m, d,
			       &nth, &ry,
			       &rm, &rd))
	    rb_raise(rb_eArgError, "invalid date");

	ret = d_simple_new_internal(klass,
				    nth, 0,
				    sg,
				    ry, rm, rd,
				    HAVE_CIVIL);
    }
    else {
	VALUE nth;
	int ry, rm, rd, rjd, ns;

	if (!valid_civil_p(y, m, d, sg,
			   &nth, &ry,
			   &rm, &rd, &rjd,
			   &ns))
	    rb_raise(rb_eArgError, "invalid date");

	ret = d_simple_new_internal(klass,
				    nth, rjd,
				    sg,
				    ry, rm, rd,
				    HAVE_JD | HAVE_CIVIL);
    }
    add_frac();
    return ret;
}

/*
 * call-seq:
 *    Date.commercial([cwyear=-4712[, cweek=1[, cwday=1[, start=Date::ITALY]]]])  ->  date
 *
 * Creates a date object denoting the given week date.
 *
 * The week and the day of week should be a negative or a positive
 * number (as a relative week/day from the end of year/week when
 * negative).  They should not be zero.
 *
 * For example:
 *
 *    Date.commercial(2001)	#=> #<Date: 2001-01-01 ...>
 *    Date.commercial(2002)	#=> #<Date: 2001-12-31 ...>
 *    Date.commercial(2001,5,6)	#=> #<Date: 2001-02-03 ...>
 *
 * See also jd and new.
 */
static VALUE
date_s_commercial(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vw, vd, vsg, y, fr, fr2, ret;
    int w, d;
    double sg;

    rb_scan_args(argc, argv, "04", &vy, &vw, &vd, &vsg);

    y = INT2FIX(-4712);
    w = 1;
    d = 1;
    fr2 = INT2FIX(0);
    sg = DEFAULT_SG;

    switch (argc) {
      case 4:
	val2sg(vsg, sg);
      case 3:
	num2int_with_frac(d, positive_inf);
      case 2:
	w = NUM2INT(vw);
      case 1:
	y = vy;
    }

    {
	VALUE nth;
	int ry, rw, rd, rjd, ns;

	if (!valid_commercial_p(y, w, d, sg,
				&nth, &ry,
				&rw, &rd, &rjd,
				&ns))
	    rb_raise(rb_eArgError, "invalid date");

	ret = d_simple_new_internal(klass,
				    nth, rjd,
				    sg,
				    0, 0, 0,
				    HAVE_JD);
    }
    add_frac();
    return ret;
}

#ifndef NDEBUG
static VALUE
date_s_weeknum(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vw, vd, vf, vsg, y, fr, fr2, ret;
    int w, d, f;
    double sg;

    rb_scan_args(argc, argv, "05", &vy, &vw, &vd, &vf, &vsg);

    y = INT2FIX(-4712);
    w = 0;
    d = 1;
    f = 0;
    fr2 = INT2FIX(0);
    sg = DEFAULT_SG;

    switch (argc) {
      case 5:
	val2sg(vsg, sg);
      case 4:
	f = NUM2INT(vf);
      case 3:
	num2int_with_frac(d, positive_inf);
      case 2:
	w = NUM2INT(vw);
      case 1:
	y = vy;
    }

    {
	VALUE nth;
	int ry, rw, rd, rjd, ns;

	if (!valid_weeknum_p(y, w, d, f, sg,
			     &nth, &ry,
			     &rw, &rd, &rjd,
			     &ns))
	    rb_raise(rb_eArgError, "invalid date");

	ret = d_simple_new_internal(klass,
				    nth, rjd,
				    sg,
				    0, 0, 0,
				    HAVE_JD);
    }
    add_frac();
    return ret;
}

static VALUE
date_s_nth_kday(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vm, vn, vk, vsg, y, fr, fr2, ret;
    int m, n, k;
    double sg;

    rb_scan_args(argc, argv, "05", &vy, &vm, &vn, &vk, &vsg);

    y = INT2FIX(-4712);
    m = 1;
    n = 1;
    k = 1;
    fr2 = INT2FIX(0);
    sg = DEFAULT_SG;

    switch (argc) {
      case 5:
	val2sg(vsg, sg);
      case 4:
	num2int_with_frac(k, positive_inf);
      case 3:
	n = NUM2INT(vn);
      case 2:
	m = NUM2INT(vm);
      case 1:
	y = vy;
    }

    {
	VALUE nth;
	int ry, rm, rn, rk, rjd, ns;

	if (!valid_nth_kday_p(y, m, n, k, sg,
			      &nth, &ry,
			      &rm, &rn, &rk, &rjd,
			      &ns))
	    rb_raise(rb_eArgError, "invalid date");

	ret = d_simple_new_internal(klass,
				    nth, rjd,
				    sg,
				    0, 0, 0,
				    HAVE_JD);
    }
    add_frac();
    return ret;
}
#endif

#if !defined(HAVE_GMTIME_R)
static struct tm*
gmtime_r(const time_t *t, struct tm *tm)
{
    auto struct tm *tmp = gmtime(t);
    if (tmp)
	*tm = *tmp;
    return tmp;
}

static struct tm*
localtime_r(const time_t *t, struct tm *tm)
{
    auto struct tm *tmp = localtime(t);
    if (tmp)
	*tm = *tmp;
    return tmp;
}
#endif

static void set_sg(union DateData *, double);

/*
 * call-seq:
 *    Date.today([start=Date::ITALY])  ->  date
 *
 * For example:
 *
 *    Date.today		#=> #<Date: 2011-06-11 ..>
 *
 * Creates a date object denoting the present day.
 */
static VALUE
date_s_today(int argc, VALUE *argv, VALUE klass)
{
    VALUE vsg, nth, ret;
    double sg;
    time_t t;
    struct tm tm;
    int y, ry, m, d;

    rb_scan_args(argc, argv, "01", &vsg);

    if (argc < 1)
	sg = DEFAULT_SG;
    else
	val2sg(vsg, sg);

    if (time(&t) == -1)
	rb_sys_fail("time");
    tzset();
    if (!localtime_r(&t, &tm))
	rb_sys_fail("localtime");

    y = tm.tm_year + 1900;
    m = tm.tm_mon + 1;
    d = tm.tm_mday;

    decode_year(INT2FIX(y), -1, &nth, &ry);

    ret = d_simple_new_internal(klass,
				nth, 0,
				GREGORIAN,
				ry, m, d,
				HAVE_CIVIL);
    {
	get_d1(ret);
	set_sg(dat, sg);
    }
    return ret;
}

#define set_hash0(k,v) rb_hash_aset(hash, k, v)
#define ref_hash0(k) rb_hash_aref(hash, k)
#define del_hash0(k) rb_hash_delete(hash, k)

#define set_hash(k,v) rb_hash_aset(hash, ID2SYM(rb_intern(k)), v)
#define ref_hash(k) rb_hash_aref(hash, ID2SYM(rb_intern(k)))
#define del_hash(k) rb_hash_delete(hash, ID2SYM(rb_intern(k)))

static VALUE
rt_rewrite_frags(VALUE hash)
{
    VALUE seconds;

    seconds = ref_hash("seconds");
    if (!NIL_P(seconds)) {
	VALUE d, h, min, s, fr;

	d = f_idiv(seconds, INT2FIX(DAY_IN_SECONDS));
	fr = f_mod(seconds, INT2FIX(DAY_IN_SECONDS));

	h = f_idiv(fr, INT2FIX(HOUR_IN_SECONDS));
	fr = f_mod(fr, INT2FIX(HOUR_IN_SECONDS));

	min = f_idiv(fr, INT2FIX(MINUTE_IN_SECONDS));
	fr = f_mod(fr, INT2FIX(MINUTE_IN_SECONDS));

	s = f_idiv(fr, INT2FIX(1));
	fr = f_mod(fr, INT2FIX(1));

	set_hash("jd", f_add(UNIX_EPOCH_IN_CJD, d));
	set_hash("hour", h);
	set_hash("min", min);
	set_hash("sec", s);
	set_hash("sec_fraction", fr);
	del_hash("seconds");
	del_hash("offset");
    }
    return hash;
}

#define sym(x) ID2SYM(rb_intern(x))

static VALUE d_lite_year(VALUE);
static VALUE d_lite_wday(VALUE);
static VALUE d_lite_jd(VALUE);

static VALUE
rt_complete_frags(VALUE klass, VALUE hash)
{
    static VALUE tab = Qnil;
    int g, e;
    VALUE k, a, d;

    if (NIL_P(tab)) {
	tab = rb_ary_new3(11,
			  rb_ary_new3(2,
				      sym("time"),
				      rb_ary_new3(3,
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  rb_ary_new3(2,
				      Qnil,
				      rb_ary_new3(1,
						  sym("jd"))),
			  rb_ary_new3(2,
				      sym("ordinal"),
				      rb_ary_new3(5,
						  sym("year"),
						  sym("yday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  rb_ary_new3(2,
				      sym("civil"),
				      rb_ary_new3(6,
						  sym("year"),
						  sym("mon"),
						  sym("mday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  rb_ary_new3(2,
				      sym("commercial"),
				      rb_ary_new3(6,
						  sym("cwyear"),
						  sym("cweek"),
						  sym("cwday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  rb_ary_new3(2,
				      sym("wday"),
				      rb_ary_new3(4,
						  sym("wday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  rb_ary_new3(2,
				      sym("wnum0"),
				      rb_ary_new3(6,
						  sym("year"),
						  sym("wnum0"),
						  sym("wday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  rb_ary_new3(2,
				      sym("wnum1"),
				      rb_ary_new3(6,
						  sym("year"),
						  sym("wnum1"),
						  sym("wday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  rb_ary_new3(2,
				      Qnil,
				      rb_ary_new3(6,
						  sym("cwyear"),
						  sym("cweek"),
						  sym("wday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  rb_ary_new3(2,
				      Qnil,
				      rb_ary_new3(6,
						  sym("year"),
						  sym("wnum0"),
						  sym("cwday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  rb_ary_new3(2,
				      Qnil,
				      rb_ary_new3(6,
						  sym("year"),
						  sym("wnum1"),
						  sym("cwday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))));
	rb_gc_register_mark_object(tab);
    }

    {
	int i, eno = 0, idx = 0;

	for (i = 0; i < RARRAY_LENINT(tab); i++) {
	    VALUE x, a;

	    x = RARRAY_PTR(tab)[i];
	    a = RARRAY_PTR(x)[1];

	    {
		int j, n = 0;

		for (j = 0; j < RARRAY_LENINT(a); j++)
		    if (!NIL_P(ref_hash0(RARRAY_PTR(a)[j])))
			n++;
		if (n > eno) {
		    eno = n;
		    idx = i;
		}
	    }
	}
	if (eno == 0)
	    g = 0;
	else {
	    g = 1;
	    k = RARRAY_PTR(RARRAY_PTR(tab)[idx])[0];
	    a = RARRAY_PTR(RARRAY_PTR(tab)[idx])[1];
	    e =	eno;
	}
    }

    d = Qnil;

    if (g && !NIL_P(k) && (RARRAY_LENINT(a) - e)) {
	if (k == sym("ordinal")) {
	    if (NIL_P(ref_hash("year"))) {
		if (NIL_P(d))
		    d = date_s_today(0, (VALUE *)0, cDate);
		set_hash("year", d_lite_year(d));
	    }
	    if (NIL_P(ref_hash("yday")))
		set_hash("yday", INT2FIX(1));
	}
	else if (k == sym("civil")) {
	    int i;

	    for (i = 0; i < RARRAY_LENINT(a); i++) {
		VALUE e = RARRAY_PTR(a)[i];

		if (!NIL_P(ref_hash0(e)))
		    break;
		if (NIL_P(d))
		    d = date_s_today(0, (VALUE *)0, cDate);
		set_hash0(e, rb_funcall(d, SYM2ID(e), 0));
	    }
	    if (NIL_P(ref_hash("mon")))
		set_hash("mon", INT2FIX(1));
	    if (NIL_P(ref_hash("mday")))
		set_hash("mday", INT2FIX(1));
	}
	else if (k == sym("commercial")) {
	    int i;

	    for (i = 0; i < RARRAY_LENINT(a); i++) {
		VALUE e = RARRAY_PTR(a)[i];

		if (!NIL_P(ref_hash0(e)))
		    break;
		if (NIL_P(d))
		    d = date_s_today(0, (VALUE *)0, cDate);
		set_hash0(e, rb_funcall(d, SYM2ID(e), 0));
	    }
	    if (NIL_P(ref_hash("cweek")))
		set_hash("cweek", INT2FIX(1));
	    if (NIL_P(ref_hash("cwday")))
		set_hash("cwday", INT2FIX(1));
	}
	else if (k == sym("wday")) {
	    if (NIL_P(d))
		d = date_s_today(0, (VALUE *)0, cDate);
	    set_hash("jd", d_lite_jd(f_add(f_sub(d,
						 d_lite_wday(d)),
					   ref_hash("wday"))));
	}
	else if (k == sym("wnum0")) {
	    int i;

	    for (i = 0; i < RARRAY_LENINT(a); i++) {
		VALUE e = RARRAY_PTR(a)[i];

		if (!NIL_P(ref_hash0(e)))
		    break;
		if (NIL_P(d))
		    d = date_s_today(0, (VALUE *)0, cDate);
		set_hash0(e, rb_funcall(d, SYM2ID(e), 0));
	    }
	    if (NIL_P(ref_hash("wnum0")))
		set_hash("wnum0", INT2FIX(0));
	    if (NIL_P(ref_hash("wday")))
		set_hash("wday", INT2FIX(0));
	}
	else if (k == sym("wnum1")) {
	    int i;

	    for (i = 0; i < RARRAY_LENINT(a); i++) {
		VALUE e = RARRAY_PTR(a)[i];

		if (!NIL_P(ref_hash0(e)))
		    break;
		if (NIL_P(d))
		    d = date_s_today(0, (VALUE *)0, cDate);
		set_hash0(e, rb_funcall(d, SYM2ID(e), 0));
	    }
	    if (NIL_P(ref_hash("wnum1")))
		set_hash("wnum1", INT2FIX(0));
	    if (NIL_P(ref_hash("wday")))
		set_hash("wday", INT2FIX(1));
	}
    }

    if (g && k == sym("time")) {
	if (f_le_p(klass, cDateTime)) {
	    if (NIL_P(d))
		d = date_s_today(0, (VALUE *)0, cDate);
	    if (NIL_P(ref_hash("jd")))
		set_hash("jd", d_lite_jd(d));
	}
    }

    if (NIL_P(ref_hash("hour")))
	set_hash("hour", INT2FIX(0));
    if (NIL_P(ref_hash("min")))
	set_hash("min", INT2FIX(0));
    if (NIL_P(ref_hash("sec")))
	set_hash("sec", INT2FIX(0));
    else if (f_gt_p(ref_hash("sec"), INT2FIX(59)))
	set_hash("sec", INT2FIX(59));

    return hash;
}

static VALUE
rt__valid_jd_p(VALUE jd, VALUE sg)
{
    return jd;
}

static VALUE
rt__valid_ordinal_p(VALUE y, VALUE d, VALUE sg)
{
    VALUE nth, rjd2;
    int ry, rd, rjd, ns;

    if (!valid_ordinal_p(y, NUM2INT(d), NUM2DBL(sg),
			 &nth, &ry,
			 &rd, &rjd,
			 &ns))
	return Qnil;
    encode_jd(nth, rjd, &rjd2);
    return rjd2;
}

static VALUE
rt__valid_civil_p(VALUE y, VALUE m, VALUE d, VALUE sg)
{
    VALUE nth, rjd2;
    int ry, rm, rd, rjd, ns;

    if (!valid_civil_p(y, NUM2INT(m), NUM2INT(d), NUM2DBL(sg),
		       &nth, &ry,
		       &rm, &rd, &rjd,
		       &ns))
	return Qnil;
    encode_jd(nth, rjd, &rjd2);
    return rjd2;
}

static VALUE
rt__valid_commercial_p(VALUE y, VALUE w, VALUE d, VALUE sg)
{
    VALUE nth, rjd2;
    int ry, rw, rd, rjd, ns;

    if (!valid_commercial_p(y, NUM2INT(w), NUM2INT(d), NUM2DBL(sg),
			    &nth, &ry,
			    &rw, &rd, &rjd,
			    &ns))
	return Qnil;
    encode_jd(nth, rjd, &rjd2);
    return rjd2;
}

static VALUE
rt__valid_weeknum_p(VALUE y, VALUE w, VALUE d, VALUE f, VALUE sg)
{
    VALUE nth, rjd2;
    int ry, rw, rd, rjd, ns;

    if (!valid_weeknum_p(y, NUM2INT(w), NUM2INT(d), NUM2INT(f), NUM2DBL(sg),
			 &nth, &ry,
			 &rw, &rd, &rjd,
			 &ns))
	return Qnil;
    encode_jd(nth, rjd, &rjd2);
    return rjd2;
}

static VALUE
rt__valid_date_frags_p(VALUE hash, VALUE sg)
{
    {
	VALUE vjd;

	if (!NIL_P(vjd = ref_hash("jd"))) {
	    VALUE jd = rt__valid_jd_p(vjd, sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }

    {
	VALUE year, yday;

	if (!NIL_P(yday = ref_hash("yday")) &&
	    !NIL_P(year = ref_hash("year"))) {
	    VALUE jd = rt__valid_ordinal_p(year, yday, sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }

    {
	VALUE year, mon, mday;

	if (!NIL_P(mday = ref_hash("mday")) &&
	    !NIL_P(mon = ref_hash("mon")) &&
	    !NIL_P(year = ref_hash("year"))) {
	    VALUE jd = rt__valid_civil_p(year, mon, mday, sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }

    {
	VALUE year, week, wday;

	wday = ref_hash("cwday");
	if (NIL_P(wday)) {
	    wday = ref_hash("wday");
	    if (!NIL_P(wday))
		if (f_zero_p(wday))
		    wday = INT2FIX(7);
	}

	if (!NIL_P(wday) &&
	    !NIL_P(week = ref_hash("cweek")) &&
	    !NIL_P(year = ref_hash("cwyear"))) {
	    VALUE jd = rt__valid_commercial_p(year, week, wday, sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }

    {
	VALUE year, week, wday;

	wday = ref_hash("wday");
	if (NIL_P(wday)) {
	    wday = ref_hash("cwday");
	    if (!NIL_P(wday))
		if (f_eqeq_p(wday, INT2FIX(7)))
		    wday = INT2FIX(0);
	}

	if (!NIL_P(wday) &&
	    !NIL_P(week = ref_hash("wnum0")) &&
	    !NIL_P(year = ref_hash("year"))) {
	    VALUE jd = rt__valid_weeknum_p(year, week, wday, INT2FIX(0), sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }

    {
	VALUE year, week, wday;

	wday = ref_hash("wday");
	if (NIL_P(wday))
	    wday = ref_hash("cwday");
	if (!NIL_P(wday))
	    wday = f_mod(f_sub(wday, INT2FIX(1)),
			 INT2FIX(7));

	if (!NIL_P(wday) &&
	    !NIL_P(week = ref_hash("wnum1")) &&
	    !NIL_P(year = ref_hash("year"))) {
	    VALUE jd = rt__valid_weeknum_p(year, week, wday, INT2FIX(1), sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }
    return Qnil;
}

static VALUE
d_new_by_frags(VALUE klass, VALUE hash, VALUE sg)
{
    VALUE jd;

    if (!c_valid_start_p(NUM2DBL(sg))) {
	sg = INT2FIX(DEFAULT_SG);
	rb_warning("invalid start is ignored");
    }

    if (NIL_P(hash))
	rb_raise(rb_eArgError, "invalid date");

    if (NIL_P(ref_hash("jd")) &&
	NIL_P(ref_hash("yday")) &&
	!NIL_P(ref_hash("year")) &&
	!NIL_P(ref_hash("mon")) &&
	!NIL_P(ref_hash("mday")))
	jd = rt__valid_civil_p(ref_hash("year"),
			       ref_hash("mon"),
			       ref_hash("mday"), sg);
    else {
	hash = rt_rewrite_frags(hash);
	hash = rt_complete_frags(klass, hash);
	jd = rt__valid_date_frags_p(hash, sg);
    }

    if (NIL_P(jd))
	rb_raise(rb_eArgError, "invalid date");
    {
	VALUE nth;
	int rjd;

	decode_jd(jd, &nth, &rjd);
	return d_simple_new_internal(klass,
				     nth, rjd,
				     NUM2DBL(sg),
				     0, 0, 0,
				     HAVE_JD);
    }
}

VALUE date__strptime(const char *str, size_t slen,
		     const char *fmt, size_t flen, VALUE hash);

static VALUE
date_s__strptime_internal(int argc, VALUE *argv, VALUE klass,
			  const char *default_fmt)
{
    VALUE vstr, vfmt, hash;
    const char *str, *fmt;
    size_t slen, flen;

    rb_scan_args(argc, argv, "11", &vstr, &vfmt);

    StringValue(vstr);
    if (!rb_enc_str_asciicompat_p(vstr))
	rb_raise(rb_eArgError,
		 "string should have ASCII compatible encoding");
    str = RSTRING_PTR(vstr);
    slen = RSTRING_LEN(vstr);
    if (argc < 2) {
	fmt = default_fmt;
	flen = strlen(default_fmt);
    }
    else {
	StringValue(vfmt);
	if (!rb_enc_str_asciicompat_p(vfmt))
	    rb_raise(rb_eArgError,
		     "format should have ASCII compatible encoding");
	fmt = RSTRING_PTR(vfmt);
	flen = RSTRING_LEN(vfmt);
    }
    hash = rb_hash_new();
    if (NIL_P(date__strptime(str, slen, fmt, flen, hash)))
	return Qnil;

    {
	VALUE zone = ref_hash("zone");
	VALUE left = ref_hash("leftover");

	if (!NIL_P(zone)) {
	    rb_enc_copy(zone, vstr);
	    OBJ_INFECT(zone, vstr);
	    set_hash("zone", zone);
	}
	if (!NIL_P(left)) {
	    rb_enc_copy(left, vstr);
	    OBJ_INFECT(left, vstr);
	    set_hash("leftover", left);
	}
    }

    return hash;
}

/*
 * call-seq:
 *    Date._strptime(string[, format='%F'])  ->  hash
 *
 * Parses the given representation of date and time with the given
 * template, and returns a hash of parsed elements.
 *
 * For example:
 *
 *    Date._strptime('2001-02-03', '%Y-%m-%d')
 *				#=> {:year=>2001, :mon=>2, :mday=>3}
 *
 *  See also strptime(3) and strftime.
 */
static VALUE
date_s__strptime(int argc, VALUE *argv, VALUE klass)
{
    return date_s__strptime_internal(argc, argv, klass, "%F");
}

/*
 * call-seq:
 *    Date.strptime([string='-4712-01-01'[, format='%F'[, start=ITALY]]])  ->  date
 *
 * Parses the given representation of date and time with the given
 * template, and creates a date object.
 *
 * For example:
 *
 *    Date.strptime('2001-02-03', '%Y-%m-%d')	#=> #<Date: 2001-02-03 ...>
 *    Date.strptime('03-02-2001', '%d-%m-%Y')	#=> #<Date: 2001-02-03 ...>
 *    Date.strptime('2001-034', '%Y-%j')	#=> #<Date: 2001-02-03 ...>
 *    Date.strptime('2001-W05-6', '%G-W%V-%u')	#=> #<Date: 2001-02-03 ...>
 *    Date.strptime('2001 04 6', '%Y %U %w')	#=> #<Date: 2001-02-03 ...>
 *    Date.strptime('2001 05 6', '%Y %W %u')	#=> #<Date: 2001-02-03 ...>
 *    Date.strptime('sat3feb01', '%a%d%b%y')	#=> #<Date: 2001-02-03 ...>
 *
 * See also strptime(3) and strftime.
 */
static VALUE
date_s_strptime(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, fmt, sg;

    rb_scan_args(argc, argv, "03", &str, &fmt, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01");
      case 1:
	fmt = rb_str_new2("%F");
      case 2:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE argv2[2], hash;

	argv2[0] = str;
	argv2[1] = fmt;
	hash = date_s__strptime(2, argv2, klass);
	return d_new_by_frags(klass, hash, sg);
    }
}

VALUE date__parse(VALUE str, VALUE comp);

static VALUE
date_s__parse_internal(int argc, VALUE *argv, VALUE klass)
{
    VALUE vstr, vcomp, hash;

    rb_scan_args(argc, argv, "11", &vstr, &vcomp);
    StringValue(vstr);
    if (!rb_enc_str_asciicompat_p(vstr))
	rb_raise(rb_eArgError,
		 "string should have ASCII compatible encoding");
    if (argc < 2)
	vcomp = Qtrue;

    hash = date__parse(vstr, vcomp);

    {
	VALUE zone = ref_hash("zone");

	if (!NIL_P(zone)) {
	    rb_enc_copy(zone, vstr);
	    OBJ_INFECT(zone, vstr);
	    set_hash("zone", zone);
	}
    }

    return hash;
}

/*
 * call-seq:
 *    Date._parse(string[, comp=true])  ->  hash
 *
 * Parses the given representation of date and time, and returns a
 * hash of parsed elements.
 *
 * If the optional second argument is true and the detected year is in
 * the range "00" to "99", considers the year a 2-digit form and makes
 * it full.
 *
 * For example:
 *
 *    Date._parse('2001-02-03')	#=> {:year=>2001, :mon=>2, :mday=>3}
 */
static VALUE
date_s__parse(int argc, VALUE *argv, VALUE klass)
{
    return date_s__parse_internal(argc, argv, klass);
}

/*
 * call-seq:
 *    Date.parse(string='-4712-01-01'[, comp=true[, start=ITALY]])  ->  date
 *
 * Parses the given representation of date and time, and creates a
 * date object.
 *
 * If the optional second argument is true and the detected year is in
 * the range "00" to "99", considers the year a 2-digit form and makes
 * it full.
 *
 * For example:
 *
 *    Date.parse('2001-02-03')		#=> #<Date: 2001-02-03 ...>
 *    Date.parse('20010203')		#=> #<Date: 2001-02-03 ...>
 *    Date.parse('3rd Feb 2001')	#=> #<Date: 2001-02-03 ...>
 */
static VALUE
date_s_parse(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, comp, sg;

    rb_scan_args(argc, argv, "03", &str, &comp, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01");
      case 1:
	comp = Qtrue;
      case 2:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE argv2[2], hash;

	argv2[0] = str;
	argv2[1] = comp;
	hash = date_s__parse(2, argv2, klass);
	return d_new_by_frags(klass, hash, sg);
    }
}

VALUE date__iso8601(VALUE);
VALUE date__rfc3339(VALUE);
VALUE date__xmlschema(VALUE);
VALUE date__rfc2822(VALUE);
VALUE date__httpdate(VALUE);
VALUE date__jisx0301(VALUE);

/*
 * call-seq:
 *    Date._iso8601(string)  ->  hash
 *
 * Returns a hash of parsed elements.
 */
static VALUE
date_s__iso8601(VALUE klass, VALUE str)
{
    return date__iso8601(str);
}

/*
 * call-seq:
 *    Date.iso8601(string='-4712-01-01'[, start=ITALY])  ->  date
 *
 * Creates a new Date object by parsing from a string according to
 * some typical ISO 8601 formats.
 *
 * For example:
 *
 *    Date.iso8601('2001-02-03')	#=> #<Date: 2001-02-03 ...>
 *    Date.iso8601('20010203')		#=> #<Date: 2001-02-03 ...>
 *    Date.iso8601('2001-W05-6')	#=> #<Date: 2001-02-03 ...>
 */
static VALUE
date_s_iso8601(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__iso8601(klass, str);
	return d_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    Date._rfc3339(string)  ->  hash
 *
 * Returns a hash of parsed elements.
 */
static VALUE
date_s__rfc3339(VALUE klass, VALUE str)
{
    return date__rfc3339(str);
}

/*
 * call-seq:
 *    Date.rfc3339(string='-4712-01-01T00:00:00+00:00'[, start=ITALY])  ->  date
 *
 * Creates a new Date object by parsing from a string according to
 * some typical RFC 3339 formats.
 *
 * For example:
 *
 *    Date.rfc3339('2001-02-03T04:05:06+07:00')	#=> #<Date: 2001-02-03 ...>
 */
static VALUE
date_s_rfc3339(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01T00:00:00+00:00");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__rfc3339(klass, str);
	return d_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    Date._xmlschema(string)  ->  hash
 *
 * Returns a hash of parsed elements.
 */
static VALUE
date_s__xmlschema(VALUE klass, VALUE str)
{
    return date__xmlschema(str);
}

/*
 * call-seq:
 *    Date.xmlschema(string='-4712-01-01'[, start=ITALY])  ->  date
 *
 * Creates a new Date object by parsing from a string according to
 * some typical XML Schema formats.
 *
 * For example:
 *
 *    Date.xmlschema('2001-02-03')	#=> #<Date: 2001-02-03 ...>
 */
static VALUE
date_s_xmlschema(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__xmlschema(klass, str);
	return d_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    Date._rfc2822(string)  ->  hash
 *    Date._rfc822(string)   ->  hash
 *
 * Returns a hash of parsed elements.
 */
static VALUE
date_s__rfc2822(VALUE klass, VALUE str)
{
    return date__rfc2822(str);
}

/*
 * call-seq:
 *    Date.rfc2822(string='Mon, 1 Jan -4712 00:00:00 +0000'[, start=ITALY])  ->  date
 *    Date.rfc822(string='Mon, 1 Jan -4712 00:00:00 +0000'[, start=ITALY])   ->  date
 *
 * Creates a new Date object by parsing from a string according to
 * some typical RFC 2822 formats.
 *
 * For example:
 *
 *    Date.rfc2822('Sat, 3 Feb 2001 00:00:00 +0000')
 *						#=> #<Date: 2001-02-03 ...>
 */
static VALUE
date_s_rfc2822(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("Mon, 1 Jan -4712 00:00:00 +0000");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__rfc2822(klass, str);
	return d_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    Date._httpdate(string)  ->  hash
 *
 * Returns a hash of parsed elements.
 */
static VALUE
date_s__httpdate(VALUE klass, VALUE str)
{
    return date__httpdate(str);
}

/*
 * call-seq:
 *    Date.httpdate(string='Mon, 01 Jan -4712 00:00:00 GMT'[, start=ITALY])  ->  date
 *
 * Creates a new Date object by parsing from a string according to
 * some RFC 2616 format.
 *
 * For example:
 *
 *    Date.httpdate('Sat, 03 Feb 2001 00:00:00 GMT')
 *						#=> #<Date: 2001-02-03 ...>
 *
 */
static VALUE
date_s_httpdate(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("Mon, 01 Jan -4712 00:00:00 GMT");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__httpdate(klass, str);
	return d_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    Date._jisx0301(string)  ->  hash
 *
 * Returns a hash of parsed elements.
 */
static VALUE
date_s__jisx0301(VALUE klass, VALUE str)
{
    return date__jisx0301(str);
}

/*
 * call-seq:
 *    Date.jisx0301(string='-4712-01-01'[, start=ITALY])  ->  date
 *
 * Creates a new Date object by parsing from a string according to
 * some typical JIS X 0301 formats.
 *
 * For example:
 *
 *    Date.jisx0301('H13.02.03')		#=> #<Date: 2001-02-03 ...>
 */
static VALUE
date_s_jisx0301(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__jisx0301(klass, str);
	return d_new_by_frags(klass, hash, sg);
    }
}

static VALUE
dup_obj(VALUE self)
{
    get_d1a(self);

    if (simple_dat_p(adat)) {
	VALUE new = d_lite_s_alloc_simple(rb_obj_class(self));
	{
	    get_d1b(new);
	    bdat->s = adat->s;
	    return new;
	}
    }
    else {
	VALUE new = d_lite_s_alloc_complex(rb_obj_class(self));
	{
	    get_d1b(new);
	    bdat->c = adat->c;
	    return new;
	}
    }
}

static VALUE
dup_obj_as_complex(VALUE self)
{
    get_d1a(self);

    if (simple_dat_p(adat)) {
	VALUE new = d_lite_s_alloc_complex(rb_obj_class(self));
	{
	    get_d1b(new);
	    copy_simple_to_complex(&bdat->c, &adat->s);
	    bdat->c.flags |= HAVE_DF | COMPLEX_DAT;
	    return new;
	}
    }
    else {
	VALUE new = d_lite_s_alloc_complex(rb_obj_class(self));
	{
	    get_d1b(new);
	    bdat->c = adat->c;
	    return new;
	}
    }
}

#define val2off(vof,iof) \
{\
    if (!offset_to_sec(vof, &iof)) {\
	iof = 0;\
	rb_warning("invalid offset is ignored");\
    }\
}

#ifndef NDEBUG
static VALUE
d_lite_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE jd, vjd, vdf, sf, vsf, vof, vsg;
    int df, of;
    double sg;

    rb_scan_args(argc, argv, "05", &vjd, &vdf, &vsf, &vof, &vsg);

    jd = INT2FIX(0);
    df = 0;
    sf = INT2FIX(0);
    of = 0;
    sg = DEFAULT_SG;

    switch (argc) {
      case 5:
	val2sg(vsg, sg);
      case 4:
	val2off(vof, of);
      case 3:
	sf = vsf;
	if (f_lt_p(sf, INT2FIX(0)) ||
	    f_ge_p(sf, INT2FIX(SECOND_IN_NANOSECONDS)))
	    rb_raise(rb_eArgError, "invalid second fraction");
      case 2:
	df = NUM2INT(vdf);
	if (df < 0 || df >= DAY_IN_SECONDS)
	    rb_raise(rb_eArgError, "invalid day fraction");
      case 1:
	jd = vjd;
    }

    {
	VALUE nth;
	int rjd;

	get_d1(self);

	decode_jd(jd, &nth, &rjd);
	if (!df && f_zero_p(sf) && !of) {
	    set_to_simple(&dat->s, nth, rjd, sg, 0, 0, 0, HAVE_JD);
	}
	else {
	    if (!complex_dat_p(dat))
		rb_raise(rb_eArgError,
			 "cannot load complex into simple");

	    set_to_complex(&dat->c, nth, rjd, df, sf, of, sg,
			   0, 0, 0, 0, 0, 0, HAVE_JD | HAVE_DF | COMPLEX_DAT);
	}
    }
    return self;
}
#endif

/* :nodoc: */
static VALUE
d_lite_initialize_copy(VALUE copy, VALUE date)
{
    if (copy == date)
	return copy;
    {
	get_d2(copy, date);
	if (simple_dat_p(bdat)) {
	    adat->s = bdat->s;
	    adat->s.flags &= ~COMPLEX_DAT;
	}
	else {
	    if (!complex_dat_p(adat))
		rb_raise(rb_eArgError,
			 "cannot load complex into simple");

	    adat->c = bdat->c;
	    adat->c.flags |= COMPLEX_DAT;
	}
    }
    return copy;
}

#ifndef NDEBUG
static VALUE
d_lite_fill(VALUE self)
{
    get_d1(self);

    if (simple_dat_p(dat)) {
	get_s_jd(dat);
	get_s_civil(dat);
    }
    else {
	get_c_jd(dat);
	get_c_civil(dat);
	get_c_df(dat);
	get_c_time(dat);
    }
    return self;
}
#endif

/*
 * call-seq:
 *    d.ajd  ->  rational
 *
 * Returns the astronomical Julian day number.  This is a fractional
 * number, which is not adjusted by the offset.
 *
 * For example:
 *
 *    DateTime.new(2001,2,3,4,5,6,'+7').ajd	#=> (11769328217/4800)
 *    DateTime.new(2001,2,2,14,5,6,'-7').ajd	#=> (11769328217/4800)
 */
static VALUE
d_lite_ajd(VALUE self)
{
    get_d1(self);
    return m_ajd(dat);
}

/*
 * call-seq:
 *    d.amjd  ->  rational
 *
 * Returns the astronomical modified Julian day number.  This is
 * a fractional number, which is not adjusted by the offset.
 *
 * For example:
 *
 *    DateTime.new(2001,2,3,4,5,6,'+7').amjd	#=> (249325817/4800)
 *    DateTime.new(2001,2,2,14,5,6,'-7').amjd	#=> (249325817/4800)
 */
static VALUE
d_lite_amjd(VALUE self)
{
    get_d1(self);
    return m_amjd(dat);
}

/*
 * call-seq:
 *    d.jd  ->  integer
 *
 * Returns the Julian day number.  This is a whole number, which is
 * adjusted by the offset as the local time.
 *
 * For example:
 *
 *    DateTime.new(2001,2,3,4,5,6,'+7').jd	#=> 2451944
 *    DateTime.new(2001,2,3,4,5,6,'-7').jd	#=> 2451944
 */
static VALUE
d_lite_jd(VALUE self)
{
    get_d1(self);
    return m_real_local_jd(dat);
}

/*
 * call-seq:
 *    d.mjd  ->  integer
 *
 * Returns the modified Julian day number.  This is a whole number,
 * which is adjusted by the offset as the local time.
 *
 * For example:
 *
 *    DateTime.new(2001,2,3,4,5,6,'+7').mjd	#=> 51943
 *    DateTime.new(2001,2,3,4,5,6,'-7').mjd	#=> 51943
 */
static VALUE
d_lite_mjd(VALUE self)
{
    get_d1(self);
    return f_sub(m_real_local_jd(dat), INT2FIX(2400001));
}

/*
 * call-seq:
 *    d.ld  ->  integer
 *
 * Returns the Lilian day number.  This is a whole number, which is
 * adjusted by the offset as the local time.
 *
 * For example:
 *
 *     Date.new(2001,2,3).ld		#=> 152784
 */
static VALUE
d_lite_ld(VALUE self)
{
    get_d1(self);
    return f_sub(m_real_local_jd(dat), INT2FIX(2299160));
}

/*
 * call-seq:
 *    d.year  ->  integer
 *
 * Returns the year.
 *
 * For example:
 *
 *    Date.new(2001,2,3).year		#=> 2001
 *    (Date.new(1,1,1) - 1).year	#=> 0
 */
static VALUE
d_lite_year(VALUE self)
{
    get_d1(self);
    return m_real_year(dat);
}

/*
 * call-seq:
 *    d.yday  ->  fixnum
 *
 * Returns the day of the year (1-366).
 *
 * For example:
 *
 *    Date.new(2001,2,3).yday		#=> 34
 */
static VALUE
d_lite_yday(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_yday(dat));
}

/*
 * call-seq:
 *    d.mon    ->  fixnum
 *    d.month  ->  fixnum
 *
 * Returns the month (1-12).
 *
 * For example:
 *
 *    Date.new(2001,2,3).mon		#=> 2
 */
static VALUE
d_lite_mon(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_mon(dat));
}

/*
 * call-seq:
 *    d.mday  ->  fixnum
 *    d.day   ->  fixnum
 *
 * Returns the day of the month (1-31).
 *
 * For example:
 *
 *    Date.new(2001,2,3).mday		#=> 3
 */
static VALUE
d_lite_mday(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_mday(dat));
}

/*
 * call-seq:
 *    d.day_fraction  ->  rational
 *
 * Returns the fractional part of the day.
 *
 * For example:
 *
 *    DateTime.new(2001,2,3,12).day_fraction	#=> (1/2)
 */
static VALUE
d_lite_day_fraction(VALUE self)
{
    get_d1(self);
    if (simple_dat_p(dat))
	return INT2FIX(0);
    return m_fr(dat);
}

/*
 * call-seq:
 *    d.cwyear  ->  integer
 *
 * Returns the calendar week based year.
 *
 * For example:
 *
 *    Date.new(2001,2,3).cwyear		#=> 2001
 *    Date.new(2000,1,1).cwyear		#=> 1999
 */
static VALUE
d_lite_cwyear(VALUE self)
{
    get_d1(self);
    return m_real_cwyear(dat);
}

/*
 * call-seq:
 *    d.cweek  ->  fixnum
 *
 * Returns the calendar week number (1-53).
 *
 * For example:
 *
 *    Date.new(2001,2,3).cweek		#=> 5
 */
static VALUE
d_lite_cweek(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_cweek(dat));
}

/*
 * call-seq:
 *    d.cwday  ->  fixnum
 *
 * Returns the day of calendar week (1-7, Monday is 1).
 *
 * For example:
 *
 *    Date.new(2001,2,3).cwday		#=> 6
 */
static VALUE
d_lite_cwday(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_cwday(dat));
}

#ifndef NDEBUG
static VALUE
d_lite_wnum0(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_wnum0(dat));
}

static VALUE
d_lite_wnum1(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_wnum1(dat));
}
#endif

/*
 * call-seq:
 *    d.wday  ->  fixnum
 *
 * Returns the day of week (0-6, Sunday is zero).
 *
 * For example:
 *
 *    Date.new(2001,2,3).wday		#=> 6
 */
static VALUE
d_lite_wday(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_wday(dat));
}

/*
 * call-seq:
 *    d.sunday?  ->  bool
 *
 * Returns true if the date is Sunday.
 */
static VALUE
d_lite_sunday_p(VALUE self)
{
    get_d1(self);
    return f_boolcast(m_wday(dat) == 0);
}

/*
 * call-seq:
 *    d.monday?  ->  bool
 *
 * Returns true if the date is Monday.
 */
static VALUE
d_lite_monday_p(VALUE self)
{
    get_d1(self);
    return f_boolcast(m_wday(dat) == 1);
}

/*
 * call-seq:
 *    d.tuesday?  ->  bool
 *
 * Returns true if the date is Tuesday.
 */
static VALUE
d_lite_tuesday_p(VALUE self)
{
    get_d1(self);
    return f_boolcast(m_wday(dat) == 2);
}

/*
 * call-seq:
 *    d.wednesday?  ->  bool
 *
 * Returns true if the date is Wednesday.
 */
static VALUE
d_lite_wednesday_p(VALUE self)
{
    get_d1(self);
    return f_boolcast(m_wday(dat) == 3);
}

/*
 * call-seq:
 *    d.thursday?  ->  bool
 *
 * Returns true if the date is Thursday.
 */
static VALUE
d_lite_thursday_p(VALUE self)
{
    get_d1(self);
    return f_boolcast(m_wday(dat) == 4);
}

/*
 * call-seq:
 *    d.friday?  ->  bool
 *
 * Returns true if the date is Friday.
 */
static VALUE
d_lite_friday_p(VALUE self)
{
    get_d1(self);
    return f_boolcast(m_wday(dat) == 5);
}

/*
 * call-seq:
 *    d.saturday?  ->  bool
 *
 * Returns true if the date is Saturday.
 */
static VALUE
d_lite_saturday_p(VALUE self)
{
    get_d1(self);
    return f_boolcast(m_wday(dat) == 6);
}

#ifndef NDEBUG
static VALUE
d_lite_nth_kday_p(VALUE self, VALUE n, VALUE k)
{
    int rjd, ns;

    get_d1(self);

    if (NUM2INT(k) != m_wday(dat))
	return Qfalse;

    c_nth_kday_to_jd(m_year(dat), m_mon(dat),
		     NUM2INT(n), NUM2INT(k), m_virtual_sg(dat), /* !=m_sg() */
		     &rjd, &ns);
    if (m_local_jd(dat) != rjd)
	return Qfalse;
    return Qtrue;
}
#endif

/*
 * call-seq:
 *    d.hour  ->  fixnum
 *
 * Returns the hour (0-23).
 *
 * For example:
 *
 *    DateTime.new(2001,2,3,4,5,6).hour		#=> 4
 */
static VALUE
d_lite_hour(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_hour(dat));
}

/*
 * call-seq:
 *    d.min     ->  fixnum
 *    d.minute  ->  fixnum
 *
 * Returns the minute (0-59).
 *
 * For example:
 *
 *    DateTime.new(2001,2,3,4,5,6).min		#=> 5
 */
static VALUE
d_lite_min(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_min(dat));
}

/*
 * call-seq:
 *    d.sec     ->  fixnum
 *    d.second  ->  fixnum
 *
 * Returns the second (0-59).
 *
 * For example:
 *
 *    DateTime.new(2001,2,3,4,5,6).sec		#=> 6
 */
static VALUE
d_lite_sec(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_sec(dat));
}

/*
 * call-seq:
 *    d.sec_fraction     ->  rational
 *    d.second_fraction  ->  rational
 *
 * Returns the fractional part of the second.
 *
 * For example:
 *
 *    DateTime.new(2001,2,3,4,5,6.5).sec_fraction	#=> (1/2)
 */
static VALUE
d_lite_sec_fraction(VALUE self)
{
    get_d1(self);
    return m_sf_in_sec(dat);
}

/*
 * call-seq:
 *    d.offset  ->  rational
 *
 * Returns the offset.
 *
 * For example:
 *
 *    DateTime.parse('04pm+0730').offset	#=> (5/16)
 */
static VALUE
d_lite_offset(VALUE self)
{
    get_d1(self);
    return m_of_in_day(dat);
}

/*
 * call-seq:
 *    d.zone  ->  string
 *
 * Returns the timezone.
 *
 * For example:
 *
 *    DateTime.parse('04pm+0730').zone		#=> "+07:30"
 */
static VALUE
d_lite_zone(VALUE self)
{
    get_d1(self);
    return m_zone(dat);
}

/*
 * call-seq:
 *    d.julian?  ->  bool
 *
 * Retruns true if the date is before the day of calendar reform.
 *
 * For example:
 *
 *     Date.new(1582,10,15).julian?		#=> false
 *     (Date.new(1582,10,15) - 1).julian?	#=> true
 */
static VALUE
d_lite_julian_p(VALUE self)
{
    get_d1(self);
    return f_boolcast(m_julian_p(dat));
}

/*
 * call-seq:
 *    d.gregorian?  ->  bool
 *
 * Retunrs true if the date is on or after the day of calendar reform.
 *
 * For example:
 *
 *     Date.new(1582,10,15).gregorian?		#=> true
 *     (Date.new(1582,10,15) - 1).gregorian?	#=> false
 */
static VALUE
d_lite_gregorian_p(VALUE self)
{
    get_d1(self);
    return f_boolcast(m_gregorian_p(dat));
}

/*
 * call-seq:
 *    d.leap?  ->  bool
 *
 * Returns true if the year is a leap year.
 *
 * For example:
 *
 *    Date.new(2000).leap?	#=> true
 *    Date.new(2001).leap?	#=> false
 */
static VALUE
d_lite_leap_p(VALUE self)
{
    int rjd, ns, ry, rm, rd;

    get_d1(self);
    if (m_gregorian_p(dat))
	return f_boolcast(c_gregorian_leap_p(m_year(dat)));

    c_civil_to_jd(m_year(dat), 3, 1, m_virtual_sg(dat),
		  &rjd, &ns);
    c_jd_to_civil(rjd - 1, m_virtual_sg(dat), &ry, &rm, &rd);
    return f_boolcast(rd == 29);
}

/*
 * call-seq:
 *    d.start  ->  float
 *
 * Returns the Julian day number denoting the day of calendar reform.
 *
 * For example:
 *
 *    Date.new(2001,2,3).start			#=> 2299161.0
 *    Date.new(2001,2,3,Date::GREGORIAN).start	#=> -Infinity
 */
static VALUE
d_lite_start(VALUE self)
{
    get_d1(self);
    return DBL2NUM(m_sg(dat));
}

static void
clear_civil(union DateData *x)
{
    if (simple_dat_p(x)) {
	x->s.year = 0;
#ifndef USE_PACK
	x->s.mon = 0;
	x->s.mday = 0;
#else
	x->s.pc = 0;
#endif
	x->s.flags &= ~HAVE_CIVIL;
    }
    else {
	x->c.year = 0;
#ifndef USE_PACK
	x->c.mon = 0;
	x->c.mday = 0;
	x->c.hour = 0;
	x->c.min = 0;
	x->c.sec = 0;
#else
	x->c.pc = 0;
#endif
	x->c.flags &= ~(HAVE_CIVIL | HAVE_TIME);
    }
}

static void
set_sg(union DateData *x, double sg)
{
    if (simple_dat_p(x)) {
	get_s_jd(x);
	clear_civil(x);
	x->s.sg = (sg_cast)sg;
    } else {
	get_c_jd(x);
	get_c_df(x);
	clear_civil(x);
	x->c.sg = (sg_cast)sg;
    }
}

static VALUE
dup_obj_with_new_start(VALUE obj, double sg)
{
    volatile VALUE dup = dup_obj(obj);
    {
	get_d1(dup);
	set_sg(dat, sg);
    }
    return dup;
}

/*
 * call-seq:
 *    d.new_start([start=Date::ITALY])  ->  date
 *
 * Duplicates self and resets its the day of calendar reform.
 *
 * For example:
 *
 *    d = Date.new(1582,10,15)
 *    d.new_start(Date::JULIAN)		#=> #<Date: 1582-10-05 ...>
 */
static VALUE
d_lite_new_start(int argc, VALUE *argv, VALUE self)
{
    VALUE vsg;
    double sg;

    rb_scan_args(argc, argv, "01", &vsg);

    sg = DEFAULT_SG;
    if (argc >= 1)
	val2sg(vsg, sg);

    return dup_obj_with_new_start(self, sg);
}

/*
 * call-seq:
 *    d.italy  ->  date
 *
 * This method is equivalent to new_start(Date::ITALY).
 */
static VALUE
d_lite_italy(VALUE self)
{
    return dup_obj_with_new_start(self, ITALY);
}

/*
 * call-seq:
 *    d.england  ->  date
 *
 * This method is equivalent to new_start(Date::ENGLAND).
 */
static VALUE
d_lite_england(VALUE self)
{
    return dup_obj_with_new_start(self, ENGLAND);
}

/*
 * call-seq:
 *    d.julian  ->  date
 *
 * This method is equivalent to new_start(Date::JULIAN).
 */
static VALUE
d_lite_julian(VALUE self)
{
    return dup_obj_with_new_start(self, JULIAN);
}

/*
 * call-seq:
 *    d.gregorian  ->  date
 *
 * This method is equivalent to new_start(Date::GREGORIAN).
 */
static VALUE
d_lite_gregorian(VALUE self)
{
    return dup_obj_with_new_start(self, GREGORIAN);
}

static void
set_of(union DateData *x, int of)
{
    assert(complex_dat_p(x));
    get_c_jd(x);
    get_c_df(x);
    clear_civil(x);
    x->c.of = of;
}

static VALUE
dup_obj_with_new_offset(VALUE obj, int of)
{
    volatile VALUE dup = dup_obj_as_complex(obj);
    {
	get_d1(dup);
	set_of(dat, of);
    }
    return dup;
}

/*
 * call-seq:
 *    d.new_offset([offset=0])  ->  date
 *
 * Duplicates self and resets its offset.
 *
 * For example:
 *
 *    d = DateTime.new(2001,2,3,4,5,6,'-02:00')
 *				#=> #<DateTime: 2001-02-03T04:05:06-02:00 ...>
 *    d.new_offset('+09:00')	#=> #<DateTime: 2001-02-03T15:05:06+09:00 ...>
 */
static VALUE
d_lite_new_offset(int argc, VALUE *argv, VALUE self)
{
    VALUE vof;
    int rof;

    rb_scan_args(argc, argv, "01", &vof);

    rof = 0;
    if (argc >= 1)
	val2off(vof, rof);

    return dup_obj_with_new_offset(self, rof);
}

/*
 * call-seq:
 *    d + other  ->  date
 *
 * Returns a date object pointing other days after self.  The other
 * should be a numeric value.  If the other is flonum, assumes its
 * precision is at most nanosecond.
 *
 * For example:
 *
 *    Date.new(2001,2,3) + 1	#=> #<Date: 2001-02-04 ...>
 *    DateTime.new(2001,2,3) + Rational(1,2)
 *				#=> #<DateTime: 2001-02-03T12:00:00+00:00 ...>
 *    DateTime.new(2001,2,3) + Rational(-1,2)
 *				#=> #<DateTime: 2001-02-02T12:00:00+00:00 ...>
 *    DateTime.jd(0,12) + DateTime.new(2001,2,3).ajd
 *				#=> #<DateTime: 2001-02-03T00:00:00+00:00 ...>
 */
static VALUE
d_lite_plus(VALUE self, VALUE other)
{
    get_d1(self);

    switch (TYPE(other)) {
      case T_FIXNUM:
	{
	    VALUE nth;
	    long t;
	    int jd;

	    nth = m_nth(dat);
	    t = FIX2LONG(other);
	    if (DIV(t, CM_PERIOD)) {
		nth = f_add(nth, INT2FIX(DIV(t, CM_PERIOD)));
		t = MOD(t, CM_PERIOD);
	    }

	    if (!t)
		jd = m_jd(dat);
	    else {
		jd = m_jd(dat) + (int)t;

		if (jd < 0) {
		    nth = f_sub(nth, INT2FIX(1));
		    jd += CM_PERIOD;
		}
		else if (jd >= CM_PERIOD) {
		    nth = f_add(nth, INT2FIX(1));
		    jd -= CM_PERIOD;
		}
	    }

	    if (simple_dat_p(dat))
		return d_simple_new_internal(rb_obj_class(self),
					     nth, jd,
					     dat->s.sg,
					     0, 0, 0,
					     (dat->s.flags | HAVE_JD) &
					     ~HAVE_CIVIL);
	    else
		return d_complex_new_internal(rb_obj_class(self),
					      nth, jd,
					      dat->c.df, dat->c.sf,
					      dat->c.of, dat->c.sg,
					      0, 0, 0,
#ifndef USE_PACK
					      dat->c.hour,
					      dat->c.min,
					      dat->c.sec,
#else
					      EX_HOUR(dat->c.pc),
					      EX_MIN(dat->c.pc),
					      EX_SEC(dat->c.pc),
#endif
					      (dat->c.flags | HAVE_JD) &
					      ~HAVE_CIVIL);
	}
	break;
      case T_BIGNUM:
	{
	    VALUE nth;
	    int jd, s;

	    if (f_positive_p(other))
		s = +1;
	    else {
		s = -1;
		other = f_negate(other);
	    }

	    nth = f_idiv(other, INT2FIX(CM_PERIOD));
	    jd = FIX2INT(f_mod(other, INT2FIX(CM_PERIOD)));

	    if (s < 0) {
		nth = f_negate(nth);
		jd = -jd;
	    }

	    if (!jd)
		jd = m_jd(dat);
	    else {
		jd = m_jd(dat) + jd;
		if (jd < 0) {
		    nth = f_sub(nth, INT2FIX(1));
		    jd += CM_PERIOD;
		}
		else if (jd >= CM_PERIOD) {
		    nth = f_add(nth, INT2FIX(1));
		    jd -= CM_PERIOD;
		}
	    }

	    if (f_zero_p(nth))
		nth = m_nth(dat);
	    else
		nth = f_add(m_nth(dat), nth);

	    if (simple_dat_p(dat))
		return d_simple_new_internal(rb_obj_class(self),
					     nth, jd,
					     dat->s.sg,
					     0, 0, 0,
					     (dat->s.flags | HAVE_JD) &
					     ~HAVE_CIVIL);
	    else
		return d_complex_new_internal(rb_obj_class(self),
					      nth, jd,
					      dat->c.df, dat->c.sf,
					      dat->c.of, dat->c.sg,
					      0, 0, 0,
#ifndef USE_PACK
					      dat->c.hour,
					      dat->c.min,
					      dat->c.sec,
#else
					      EX_HOUR(dat->c.pc),
					      EX_MIN(dat->c.pc),
					      EX_SEC(dat->c.pc),
#endif
					      (dat->c.flags | HAVE_JD) &
					      ~HAVE_CIVIL);
	}
	break;
      case T_FLOAT:
	{
	    double jd, o, tmp;
	    int s, df;
	    VALUE nth, sf;

	    o = RFLOAT_VALUE(other);

	    if (o > 0)
		s = +1;
	    else {
		s = -1;
		o = -o;
	    }

	    o = modf(o, &tmp);

	    if (!floor(tmp / CM_PERIOD)) {
		nth = INT2FIX(0);
		jd = (int)tmp;
	    }
	    else {
		double i, f;

		f = modf(tmp / CM_PERIOD, &i);
		nth = f_floor(DBL2NUM(i));
		jd = (int)(f * CM_PERIOD);
	    }

	    o *= DAY_IN_SECONDS;
	    o = modf(o, &tmp);
	    df = (int)tmp;
	    o *= SECOND_IN_NANOSECONDS;
	    sf = INT2FIX((int)round(o));

	    if (s < 0) {
		jd = -jd;
		df = -df;
		sf = f_negate(sf);
	    }

	    if (f_zero_p(sf))
		sf = m_sf(dat);
	    else {
		sf = f_add(m_sf(dat), sf);
		if (f_lt_p(sf, INT2FIX(0))) {
		    df -= 1;
		    sf = f_add(sf, INT2FIX(SECOND_IN_NANOSECONDS));
		}
		else if (f_ge_p(sf, INT2FIX(SECOND_IN_NANOSECONDS))) {
		    df += 1;
		    sf = f_sub(sf, INT2FIX(SECOND_IN_NANOSECONDS));
		}
	    }

	    if (!df)
		df = m_df(dat);
	    else {
		df = m_df(dat) + df;
		if (df < 0) {
		    jd -= 1;
		    df += DAY_IN_SECONDS;
		}
		else if (df >= DAY_IN_SECONDS) {
		    jd += 1;
		    df -= DAY_IN_SECONDS;
		}
	    }

	    if (!jd)
		jd = m_jd(dat);
	    else {
		jd = m_jd(dat) + jd;
		if (jd < 0) {
		    nth = f_sub(nth, INT2FIX(1));
		    jd += CM_PERIOD;
		}
		else if (jd >= CM_PERIOD) {
		    nth = f_add(nth, INT2FIX(1));
		    jd -= CM_PERIOD;
		}
	    }

	    if (f_zero_p(nth))
		nth = m_nth(dat);
	    else
		nth = f_add(m_nth(dat), nth);

	    if (!df && f_zero_p(sf) && !m_of(dat))
		return d_simple_new_internal(rb_obj_class(self),
					     nth, (int)jd,
					     m_sg(dat),
					     0, 0, 0,
					     (dat->s.flags | HAVE_JD) &
					     ~(HAVE_CIVIL | HAVE_TIME |
					       COMPLEX_DAT));
	    else
		return d_complex_new_internal(rb_obj_class(self),
					      nth, (int)jd,
					      df, sf,
					      m_of(dat), m_sg(dat),
					      0, 0, 0,
					      0, 0, 0,
					      (dat->c.flags |
					       HAVE_JD | HAVE_DF) &
					      ~(HAVE_CIVIL | HAVE_TIME));
	}
	break;
      default:
	if (!k_numeric_p(other))
	    rb_raise(rb_eTypeError, "expected numeric");
	other = f_to_r(other);
#ifdef CANONICALIZATION_FOR_MATHN
	if (!k_rational_p(other))
	    return d_lite_plus(self, other);
#endif
	/* fall through */
      case T_RATIONAL:
	{
	    VALUE nth, sf, t;
	    int jd, df, s;

	    if (wholenum_p(other))
		return d_lite_plus(self, RRATIONAL(other)->num);

	    if (f_positive_p(other))
		s = +1;
	    else {
		s = -1;
		other = f_negate(other);
	    }

	    nth = f_idiv(other, INT2FIX(CM_PERIOD));
	    t = f_mod(other, INT2FIX(CM_PERIOD));

	    jd = FIX2INT(f_idiv(t, INT2FIX(1)));
	    t = f_mod(t, INT2FIX(1));

	    t = f_mul(t, INT2FIX(DAY_IN_SECONDS));
	    df = FIX2INT(f_idiv(t, INT2FIX(1)));
	    t = f_mod(t, INT2FIX(1));

	    sf = f_mul(t, INT2FIX(SECOND_IN_NANOSECONDS));

	    if (s < 0) {
		nth = f_negate(nth);
		jd = -jd;
		df = -df;
		sf = f_negate(sf);
	    }

	    if (f_zero_p(sf))
		sf = m_sf(dat);
	    else {
		sf = f_add(m_sf(dat), sf);
		if (f_lt_p(sf, INT2FIX(0))) {
		    df -= 1;
		    sf = f_add(sf, INT2FIX(SECOND_IN_NANOSECONDS));
		}
		else if (f_ge_p(sf, INT2FIX(SECOND_IN_NANOSECONDS))) {
		    df += 1;
		    sf = f_sub(sf, INT2FIX(SECOND_IN_NANOSECONDS));
		}
	    }

	    if (!df)
		df = m_df(dat);
	    else {
		df = m_df(dat) + df;
		if (df < 0) {
		    jd -= 1;
		    df += DAY_IN_SECONDS;
		}
		else if (df >= DAY_IN_SECONDS) {
		    jd += 1;
		    df -= DAY_IN_SECONDS;
		}
	    }

	    if (!jd)
		jd = m_jd(dat);
	    else {
		jd = m_jd(dat) + jd;
		if (jd < 0) {
		    nth = f_sub(nth, INT2FIX(1));
		    jd += CM_PERIOD;
		}
		else if (jd >= CM_PERIOD) {
		    nth = f_add(nth, INT2FIX(1));
		    jd -= CM_PERIOD;
		}
	    }

	    if (f_zero_p(nth))
		nth = m_nth(dat);
	    else
		nth = f_add(m_nth(dat), nth);

	    if (!df && f_zero_p(sf) && !m_of(dat))
		return d_simple_new_internal(rb_obj_class(self),
					     nth, jd,
					     m_sg(dat),
					     0, 0, 0,
					     (dat->s.flags | HAVE_JD) &
					     ~(HAVE_CIVIL | HAVE_TIME |
					       COMPLEX_DAT));
	    else
		return d_complex_new_internal(rb_obj_class(self),
					      nth, jd,
					      df, sf,
					      m_of(dat), m_sg(dat),
					      0, 0, 0,
					      0, 0, 0,
					      (dat->c.flags |
					       HAVE_JD | HAVE_DF) &
					      ~(HAVE_CIVIL | HAVE_TIME));
	}
	break;
    }
}

static VALUE
minus_dd(VALUE self, VALUE other)
{
    get_d2(self, other);

    {
	int d, df;
	VALUE n, sf, r;

	n = f_sub(m_nth(adat), m_nth(bdat));
	d = m_jd(adat) - m_jd(bdat);
	df = m_df(adat) - m_df(bdat);
	sf = f_sub(m_sf(adat), m_sf(bdat));

	if (d < 0) {
	    n = f_sub(n, INT2FIX(1));
	    d += CM_PERIOD;
	}
	else if (d >= CM_PERIOD) {
	    n = f_add(n, INT2FIX(1));
	    d -= CM_PERIOD;
	}

	if (df < 0) {
	    d -= 1;
	    df += DAY_IN_SECONDS;
	}
	else if (df >= DAY_IN_SECONDS) {
	    d += 1;
	    df -= DAY_IN_SECONDS;
	}

	if (f_lt_p(sf, INT2FIX(0))) {
	    df -= 1;
	    sf = f_add(sf, INT2FIX(SECOND_IN_NANOSECONDS));
	}
	else if (f_ge_p(sf, INT2FIX(SECOND_IN_NANOSECONDS))) {
	    df += 1;
	    sf = f_sub(sf, INT2FIX(SECOND_IN_NANOSECONDS));
	}

	if (f_zero_p(n))
	    r = INT2FIX(0);
	else
	    r = f_mul(n, INT2FIX(CM_PERIOD));

	if (d)
	    r = f_add(r, rb_rational_new1(INT2FIX(d)));
	if (df)
	    r = f_add(r, isec_to_day(df));
	if (f_nonzero_p(sf))
	    r = f_add(r, ns_to_day(sf));

	if (TYPE(r) == T_RATIONAL)
	    return r;
	return rb_rational_new1(r);
    }
}

/*
 * call-seq:
 *    d - other  ->  date or rational
 *
 * Returns the difference between the two dates if the other is a date
 * object.  If the other is a numeric value, returns a date object
 * pointing other days before self.  If the other is flonum, assumes
 * its precision is at most nanosecond.
 *
 * For example:
 *
 *     Date.new(2001,2,3) - 1	#=> #<Date: 2001-02-02 ...>
 *     DateTime.new(2001,2,3) - Rational(1,2)
 *				#=> #<DateTime: 2001-02-02T12:00:00+00:00 ...>
 *     Date.new(2001,2,3) - Date.new(2001)
 *				#=> (33/1)
 *     DateTime.new(2001,2,3) - DateTime.new(2001,2,2,12)
 *				#=> (1/2)
 */
static VALUE
d_lite_minus(VALUE self, VALUE other)
{
    if (k_date_p(other))
	return minus_dd(self, other);

    switch (TYPE(other)) {
      case T_FIXNUM:
	return d_lite_plus(self, LONG2NUM(-FIX2LONG(other)));
      case T_FLOAT:
	return d_lite_plus(self, DBL2NUM(-RFLOAT_VALUE(other)));
      default:
	if (!k_numeric_p(other))
	    rb_raise(rb_eTypeError, "expected numeric");
	/* fall through */
      case T_BIGNUM:
      case T_RATIONAL:
	return d_lite_plus(self, f_negate(other));
    }
}

/*
 * call-seq:
 *    d.next_day([n=1])  ->  date
 *
 * This method is equivalent to d + n.
 */
static VALUE
d_lite_next_day(int argc, VALUE *argv, VALUE self)
{
    VALUE n;

    rb_scan_args(argc, argv, "01", &n);
    if (argc < 1)
	n = INT2FIX(1);
    return d_lite_plus(self, n);
}

/*
 * call-seq:
 *    d.prev_day([n=1])  ->  date
 *
 * This method is equivalent to d - n.
 */
static VALUE
d_lite_prev_day(int argc, VALUE *argv, VALUE self)
{
    VALUE n;

    rb_scan_args(argc, argv, "01", &n);
    if (argc < 1)
	n = INT2FIX(1);
    return d_lite_minus(self, n);
}

/*
 * call-seq:
 *    d.next  ->  date
 *
 * Returns a date object denoting the following day.
 */
static VALUE
d_lite_next(VALUE self)
{
    return d_lite_next_day(0, (VALUE *)NULL, self);
}

/*
 * call-seq:
 *    d >> n  ->  date
 *
 * Returns a date object pointing n months after self.  The n should
 * be a numeric value.
 *
 * For example:
 *
 *    Date.new(2001,2,3) >> 1	#=> #<Date: 2001-03-03 ...>
 *    Date.new(2001,1,31) >> 1	#=> #<Date: 2001-02-28 ...>
 *    Date.new(2001,2,3) >> -2	#=> #<Date: 2000-12-03 ...>
 */
static VALUE
d_lite_rshift(VALUE self, VALUE other)
{
    VALUE t, y, nth, rjd2;
    int m, d, rjd;
    double sg;

    get_d1(self);
    t = f_add3(f_mul(m_real_year(dat), INT2FIX(12)),
	       INT2FIX(m_mon(dat) - 1),
	       other);
    if (FIXNUM_P(t)) {
	long it = FIX2LONG(t);
	y = LONG2NUM(DIV(it, 12));
	it = MOD(it, 12);
	m = (int)it + 1;
    }
    else {
	y = f_idiv(t, INT2FIX(12));
	t = f_mod(t, INT2FIX(12));
	m = FIX2INT(t) + 1;
    }
    d = m_mday(dat);
    sg = m_sg(dat);

    while (1) {
	int ry, rm, rd, ns;

	if (valid_civil_p(y, m, d, sg,
			  &nth, &ry,
			  &rm, &rd, &rjd, &ns))
	    break;
	if (--d < 1)
	    rb_raise(rb_eArgError, "invalid date");
    }
    encode_jd(nth, rjd, &rjd2);
    return d_lite_plus(self, f_sub(rjd2, m_real_local_jd(dat)));
}

/*
 * call-seq:
 *    d << n  ->  date
 *
 * Returns a date object pointing n months before self.  The n should
 * be a numeric value.
 *
 * For example:
 *
 *    Date.new(2001,2,3) << 1	#=> #<Date: 2001-01-03 ...>
 *    Date.new(2001,1,31) << 11	#=> #<Date: 2000-02-29 ...>
 *    Date.new(2001,2,3) << -1	#=> #<Date: 2001-03-03 ...>
 */
static VALUE
d_lite_lshift(VALUE self, VALUE other)
{
    return d_lite_rshift(self, f_negate(other));
}

/*
 * call-seq:
 *    d.next_month([n=1])  ->  date
 *
 * This method is equivalent to d >> n
 */
static VALUE
d_lite_next_month(int argc, VALUE *argv, VALUE self)
{
    VALUE n;

    rb_scan_args(argc, argv, "01", &n);
    if (argc < 1)
	n = INT2FIX(1);
    return d_lite_rshift(self, n);
}

/*
 * call-seq:
 *    d.prev_month([n=1])  ->  date
 *
 * This method is equivalent to d << n
 */
static VALUE
d_lite_prev_month(int argc, VALUE *argv, VALUE self)
{
    VALUE n;

    rb_scan_args(argc, argv, "01", &n);
    if (argc < 1)
	n = INT2FIX(1);
    return d_lite_lshift(self, n);
}

/*
 * call-seq:
 *    d.next_year([n=1])  ->  date
 *
 * This method is equivalent to d >> (n * 12)
 */
static VALUE
d_lite_next_year(int argc, VALUE *argv, VALUE self)
{
    VALUE n;

    rb_scan_args(argc, argv, "01", &n);
    if (argc < 1)
	n = INT2FIX(1);
    return d_lite_rshift(self, f_mul(n, INT2FIX(12)));
}

/*
 * call-seq:
 *    d.prev_year([n=1])  ->  date
 *
 * This method is equivalent to d << (n * 12)
 */
static VALUE
d_lite_prev_year(int argc, VALUE *argv, VALUE self)
{
    VALUE n;

    rb_scan_args(argc, argv, "01", &n);
    if (argc < 1)
	n = INT2FIX(1);
    return d_lite_lshift(self, f_mul(n, INT2FIX(12)));
}

static VALUE d_lite_cmp(VALUE, VALUE);

/*
 * call-seq:
 *    d.step(limit[, step=1])              ->  enumerator
 *    d.step(limit[, step=1]){|date| ...}  ->  self
 *
 * Iterates evaluation of the given block, which takes a date object.
 * The limit should be a date object.
 *
 * For example:
 *
 *    Date.new(2001).step(Date.new(2001,-1,-1)).select{|d| d.sunday?}.size
 *				#=> 52
 */
static VALUE
d_lite_step(int argc, VALUE *argv, VALUE self)
{
    VALUE limit, step, date;

    rb_scan_args(argc, argv, "11", &limit, &step);

    if (argc < 2)
	step = INT2FIX(1);

#if 0
    if (f_zero_p(step))
	rb_raise(rb_eArgError, "step can't be 0");
#endif

    RETURN_ENUMERATOR(self, argc, argv);

    date = self;
    switch (FIX2INT(f_cmp(step, INT2FIX(0)))) {
      case -1:
	while (FIX2INT(d_lite_cmp(date, limit)) >= 0) {
	    rb_yield(date);
	    date = d_lite_plus(date, step);
	}
	break;
      case 0:
	while (1)
	    rb_yield(date);
	break;
      case 1:
	while (FIX2INT(d_lite_cmp(date, limit)) <= 0) {
	    rb_yield(date);
	    date = d_lite_plus(date, step);
	}
	break;
      default:
	abort();
    }
    return self;
}

/*
 * call-seq:
 *    d.upto(max)              ->  enumerator
 *    d.upto(max){|date| ...}  ->  self
 *
 * This method is equivalent to step(max, 1){|date| ...}.
 */
static VALUE
d_lite_upto(VALUE self, VALUE max)
{
    VALUE date;

    RETURN_ENUMERATOR(self, 1, &max);

    date = self;
    while (FIX2INT(d_lite_cmp(date, max)) <= 0) {
	rb_yield(date);
	date = d_lite_plus(date, INT2FIX(1));
    }
    return self;
}

/*
 * call-seq:
 *    d.downto(min)              ->  enumerator
 *    d.downto(min){|date| ...}  ->  self
 *
 * This method is equivalent to step(min, -1){|date| ...}.
 */
static VALUE
d_lite_downto(VALUE self, VALUE min)
{
    VALUE date;

    RETURN_ENUMERATOR(self, 1, &min);

    date = self;
    while (FIX2INT(d_lite_cmp(date, min)) >= 0) {
	rb_yield(date);
	date = d_lite_plus(date, INT2FIX(-1));
    }
    return self;
}

static VALUE
cmp_gen(VALUE self, VALUE other)
{
    get_d1(self);

    if (k_numeric_p(other))
	return f_cmp(m_ajd(dat), other);
    else if (k_date_p(other))
	return f_cmp(m_ajd(dat), f_ajd(other));
    return rb_num_coerce_cmp(self, other, rb_intern("<=>"));
}

static VALUE
cmp_dd(VALUE self, VALUE other)
{
    get_d2(self, other);

    {
	VALUE a_nth, b_nth,
	    a_sf, b_sf;
	int a_jd, b_jd,
	    a_df, b_df;

	a_nth = m_nth(adat);
	b_nth = m_nth(bdat);
	if (f_eqeq_p(a_nth, b_nth)) {
	    a_jd = m_jd(adat);
	    b_jd = m_jd(bdat);
	    if (a_jd == b_jd) {
		a_df = m_df(adat);
		b_df = m_df(bdat);
		if (a_df == b_df) {
		    a_sf = m_sf(adat);
		    b_sf = m_sf(bdat);
		    if (f_eqeq_p(a_sf, b_sf)) {
			return INT2FIX(0);
		    }
		    else if (f_lt_p(a_sf, b_sf)) {
			return INT2FIX(-1);
		    }
		    else {
			return INT2FIX(1);
		    }
		}
		else if (a_df < b_df) {
		    return INT2FIX(-1);
		}
		else {
		    return INT2FIX(1);
		}
	    }
	    else if (a_jd < b_jd) {
		return INT2FIX(-1);
	    }
	    else {
		return INT2FIX(1);
	    }
	}
	else if (f_lt_p(a_nth, b_nth)) {
	    return INT2FIX(-1);
	}
	else {
	    return INT2FIX(1);
	}
    }
}

/*
 * call-seq:
 *    d <=> other  -> -1, 0, +1 or nil
 *
 * Compares the two dates and returns -1, zero, 1 or nil.  The other
 * should be a date object or a numeric value as an astronomical
 * Julian day number.
 *
 * For example:
 *
 *    Date.new(2001,2,3) <=> Date.new(2001,2,4)	#=> -1
 *    Date.new(2001,2,3) <=> Date.new(2001,2,3)	#=> 0
 *    Date.new(2001,2,3) <=> Date.new(2001,2,2)	#=> 1
 *    Date.new(2001,2,3) <=> Object.new		#=> nil
 *    Date.new(2001,2,3) <=> Rational(4903887,2)#=> 0
 *
 * See also Comparable.
 */
static VALUE
d_lite_cmp(VALUE self, VALUE other)
{
    if (!k_date_p(other))
	return cmp_gen(self, other);

    {
	get_d2(self, other);

	if (!(simple_dat_p(adat) && simple_dat_p(bdat) &&
	      m_gregorian_p(adat) == m_gregorian_p(bdat)))
	    return cmp_dd(self, other);

	if (have_jd_p(adat) &&
	    have_jd_p(bdat)) {
	    VALUE a_nth, b_nth;
	    int a_jd, b_jd;

	    a_nth = m_nth(adat);
	    b_nth = m_nth(bdat);
	    if (f_eqeq_p(a_nth, b_nth)) {
		a_jd = m_jd(adat);
		b_jd = m_jd(bdat);
		if (a_jd == b_jd) {
		    return INT2FIX(0);
		}
		else if (a_jd < b_jd) {
		    return INT2FIX(-1);
		}
		else {
		    return INT2FIX(1);
		}
	    }
	    else if (a_nth < b_nth) {
		return INT2FIX(-1);
	    }
	    else {
		return INT2FIX(1);
	    }
	}
	else {
#ifndef USE_PACK
	    VALUE a_nth, b_nth;
	    int a_year, b_year,
		a_mon, b_mon,
		a_mday, b_mday;
#else
	    VALUE a_nth, b_nth;
	    int a_year, b_year,
		a_pd, b_pd;
#endif

	    a_nth = m_nth(adat);
	    b_nth = m_nth(bdat);
	    if (f_eqeq_p(a_nth, b_nth)) {
		a_year = m_year(adat);
		b_year = m_year(bdat);
		if (a_year == b_year) {
#ifndef USE_PACK
		    a_mon = m_mon(adat);
		    b_mon = m_mon(bdat);
		    if (a_mon == b_mon) {
			a_mday = m_mday(adat);
			b_mday = m_mday(bdat);
			if (a_mday == b_mday) {
			    return INT2FIX(0);
			}
			else if (a_mday < b_mday) {
			    return INT2FIX(-1);
			}
			else {
			    return INT2FIX(1);
			}
		    }
		    else if (a_mon < b_mon) {
			return INT2FIX(-1);
		    }
		    else {
			return INT2FIX(1);
		    }
#else
		    a_pd = m_pc(adat);
		    b_pd = m_pc(bdat);
		    if (a_pd == b_pd) {
			return INT2FIX(0);
		    }
		    else if (a_pd < b_pd) {
			return INT2FIX(-1);
		    }
		    else {
			return INT2FIX(1);
		    }
#endif
		}
		else if (a_year < b_year) {
		    return INT2FIX(-1);
		}
		else {
		    return INT2FIX(1);
		}
	    }
	    else if (f_lt_p(a_nth, b_nth)) {
		return INT2FIX(-1);
	    }
	    else {
		return INT2FIX(1);
	    }
	}
    }
}

static VALUE
equal_gen(VALUE self, VALUE other)
{
    get_d1(self);

    if (k_numeric_p(other))
	return f_eqeq_p(m_real_local_jd(dat), other);
    else if (k_date_p(other))
	return f_eqeq_p(m_real_local_jd(dat), f_jd(other));
    return rb_num_coerce_cmp(self, other, rb_intern("=="));
}

/*
 * call-seq:
 *    d === other  ->  bool
 *
 * Returns true if they are the same day.
 *
 * For example:
 *
 *    Date.new(2001,2,3) === Date.new(2001,2,3)
 * 					#=> true
 *    Date.new(2001,2,3) === Date.new(2001,2,4)
 *					#=> false
 *    DateTime.new(2001,2,3) === DateTime.new(2001,2,3,12)
 *					#=> true
 *    DateTime.new(2001,2,3) === DateTime.new(2001,2,3,0,0,0,'+24:00')
 *					#=> true
 *    DateTime.new(2001,2,3) === DateTime.new(2001,2,4,0,0,0,'+24:00')
 *					#=> false
 */
static VALUE
d_lite_equal(VALUE self, VALUE other)
{
    if (!k_date_p(other))
	return equal_gen(self, other);

    {
	get_d2(self, other);

	if (!(m_gregorian_p(adat) == m_gregorian_p(bdat)))
	    return equal_gen(self, other);

	if (have_jd_p(adat) &&
	    have_jd_p(bdat)) {
	    VALUE a_nth, b_nth;
	    int a_jd, b_jd;

	    a_nth = m_nth(adat);
	    b_nth = m_nth(bdat);
	    a_jd = m_local_jd(adat);
	    b_jd = m_local_jd(bdat);
	    if (f_eqeq_p(a_nth, b_nth) &&
		a_jd == b_jd)
		return Qtrue;
	    return Qfalse;
	}
	else {
#ifndef USE_PACK
	    VALUE a_nth, b_nth;
	    int a_year, b_year,
		a_mon, b_mon,
		a_mday, b_mday;
#else
	    VALUE a_nth, b_nth;
	    int a_year, b_year,
		a_pd, b_pd;
#endif

	    a_nth = m_nth(adat);
	    b_nth = m_nth(bdat);
	    if (f_eqeq_p(a_nth, b_nth)) {
		a_year = m_year(adat);
		b_year = m_year(bdat);
		if (a_year == b_year) {
#ifndef USE_PACK
		    a_mon = m_mon(adat);
		    b_mon = m_mon(bdat);
		    if (a_mon == b_mon) {
			a_mday = m_mday(adat);
			b_mday = m_mday(bdat);
			if (a_mday == b_mday)
			    return Qtrue;
		    }
#else
		    /* mon and mday only */
		    a_pd = (m_pc(adat) >> MDAY_SHIFT);
		    b_pd = (m_pc(bdat) >> MDAY_SHIFT);
		    if (a_pd == b_pd) {
			return Qtrue;
		    }
#endif
		}
	    }
	    return Qfalse;
	}
    }
}

/* :nodoc: */
static VALUE
d_lite_eql_p(VALUE self, VALUE other)
{
    if (!k_date_p(other))
	return Qfalse;
    return f_zero_p(d_lite_cmp(self, other));
}

/* :nodoc: */
static VALUE
d_lite_hash(VALUE self)
{
    st_index_t v, h[4];

    get_d1(self);
    h[0] = m_nth(dat);
    h[1] = m_jd(dat);
    h[2] = m_df(dat);
    h[3] = m_sf(dat);
    v = rb_memhash(h, sizeof(h));
    return LONG2FIX(v);
}

#include "date_tmx.h"
static void set_tmx(VALUE, struct tmx *);
static VALUE strftimev(const char *, VALUE,
		       void (*)(VALUE, struct tmx *));

/*
 * call-seq:
 *    d.to_s  ->  string
 *
 * Returns a string in an ISO 8601 format (This method doesn't use the
 * expanded representations).
 *
 * For example:
 *
 *     Date.new(2001,2,3).to_s	#=> "2001-02-03"
 */
static VALUE
d_lite_to_s(VALUE self)
{
    return strftimev("%Y-%m-%d", self, set_tmx);
}

#ifndef NDEBUG
static VALUE
mk_inspect_flags(union DateData *x)
{
    return rb_enc_sprintf(rb_usascii_encoding(),
			  "%c%c%c%c%c",
			  (x->flags & COMPLEX_DAT) ? 'C' : 'S',
			  (x->flags & HAVE_JD)     ? 'j' : '-',
			  (x->flags & HAVE_DF)     ? 'd' : '-',
			  (x->flags & HAVE_CIVIL)  ? 'c' : '-',
			  (x->flags & HAVE_TIME)   ? 't' : '-');
}

static VALUE
mk_inspect_raw(union DateData *x, const char *klass)
{
    if (simple_dat_p(x)) {
	VALUE nth, flags;

	RB_GC_GUARD(nth) = f_inspect(x->s.nth);
	RB_GC_GUARD(flags) = mk_inspect_flags(x);

	return rb_enc_sprintf(rb_usascii_encoding(),
			      "#<%s: "
			      "(%sth,%dj),+0s,%.0fj; "
			      "%dy%dm%dd; %s>",
			      klass ? klass : "?",
			      RSTRING_PTR(nth), x->s.jd, x->s.sg,
#ifndef USE_PACK
			      x->s.year, x->s.mon, x->s.mday,
#else
			      x->s.year,
			      EX_MON(x->s.pc), EX_MDAY(x->s.pc),
#endif
			      RSTRING_PTR(flags));
    }
    else {
	VALUE nth, sf, flags;

	RB_GC_GUARD(nth) = f_inspect(x->c.nth);
	RB_GC_GUARD(sf) = f_inspect(x->c.sf);
	RB_GC_GUARD(flags) = mk_inspect_flags(x);

	return rb_enc_sprintf(rb_usascii_encoding(),
			      "#<%s: "
			      "(%sth,%dj,%ds,%sn),%+ds,%.0fj; "
			      "%dy%dm%dd %dh%dm%ds; %s>",
			      klass ? klass : "?",
			      RSTRING_PTR(nth), x->c.jd, x->c.df,
			      RSTRING_PTR(sf),
			      x->c.of, x->c.sg,
#ifndef USE_PACK
			      x->c.year, x->c.mon, x->c.mday,
			      x->c.hour, x->c.min, x->c.sec,
#else
			      x->c.year,
			      EX_MON(x->c.pc), EX_MDAY(x->c.pc),
			      EX_HOUR(x->c.pc), EX_MIN(x->c.pc),
			      EX_SEC(x->c.pc),
#endif
			      RSTRING_PTR(flags));
    }
}

static VALUE
d_lite_inspect_raw(VALUE self)
{
    get_d1(self);
    return mk_inspect_raw(dat, rb_obj_classname(self));
}
#endif

static VALUE
mk_inspect(union DateData *x, const char *klass, const char *to_s)
{
    VALUE jd, sf;

    RB_GC_GUARD(jd) = f_inspect(m_real_jd(x));
    RB_GC_GUARD(sf) = f_inspect(m_sf(x));

    return rb_enc_sprintf(rb_usascii_encoding(),
			  "#<%s: %s ((%sj,%ds,%sn),%+ds,%.0fj)>",
			  klass ? klass : "?",
			  to_s ? to_s : "?",
			  RSTRING_PTR(jd), m_df(x), RSTRING_PTR(sf),
			  m_of(x), m_sg(x));
}

/*
 * call-seq:
 *    d.inspect  ->  string
 *
 * Returns the value as a string for inspection.
 *
 * For example:
 *
 *    Date.new(2001,2,3).inspect
 *		#=> "#<Date: 2001-02-03 ((2451944j,0s,0n),+0s,2299161j)>"
 *    DateTime.new(2001,2,3,4,5,6,'-7').inspect
 *		#=> "#<DateTime: 2001-02-03T04:05:06-07:00 ((2451944j,39906s,0n),-25200s,2299161j)>"
 *
 */
static VALUE
d_lite_inspect(VALUE self)
{
    get_d1(self);
    {
	VALUE to_s;

	RB_GC_GUARD(to_s) = f_to_s(self);
	return mk_inspect(dat, rb_obj_classname(self), RSTRING_PTR(to_s));
    }
}

#include <errno.h>
#include "date_tmx.h"

size_t date_strftime(char *s, size_t maxsize, const char *format,
		     const struct tmx *tmx);

#define SMALLBUF 100
static size_t
date_strftime_alloc(char **buf, const char *format,
		    struct tmx *tmx)
{
    size_t size, len, flen;

    (*buf)[0] = '\0';
    flen = strlen(format);
    if (flen == 0) {
	return 0;
    }
    errno = 0;
    len = date_strftime(*buf, SMALLBUF, format, tmx);
    if (len != 0 || (**buf == '\0' && errno != ERANGE)) return len;
    for (size=1024; ; size*=2) {
	*buf = xmalloc(size);
	(*buf)[0] = '\0';
	len = date_strftime(*buf, size, format, tmx);
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
	    rb_sys_fail(format);
	    break;
	}
    }
    return len;
}

static VALUE
tmx_m_secs(union DateData *x)
{
    VALUE s;
    int df;

    s = day_to_sec(f_sub(m_real_jd(x),
			 UNIX_EPOCH_IN_CJD));
    if (simple_dat_p(x))
	return s;
    df = m_df(x);
    if (df)
	s = f_add(s, INT2FIX(df));
    return s;
}

#define MILLISECOND_IN_NANOSECONDS 1000000

static VALUE
tmx_m_msecs(union DateData *x)
{
    VALUE s, sf;

    s = sec_to_ms(tmx_m_secs(x));
    if (simple_dat_p(x))
	return s;
    sf = m_sf(x);
    if (f_nonzero_p(sf))
	s = f_add(s, f_div(sf, INT2FIX(MILLISECOND_IN_NANOSECONDS)));
    return s;
}

static VALUE
tmx_m_of(union DateData *x)
{
    return INT2FIX(m_of(x));
}

static char *
tmx_m_zone(union DateData *x)
{
    return RSTRING_PTR(m_zone(x));
}

static struct tmx_funcs tmx_funcs = {
    (VALUE (*)(void *))m_real_year,
    (int (*)(void *))m_yday,
    (int (*)(void *))m_mon,
    (int (*)(void *))m_mday,
    (VALUE (*)(void *))m_real_cwyear,
    (int (*)(void *))m_cweek,
    (int (*)(void *))m_cwday,
    (int (*)(void *))m_wnum0,
    (int (*)(void *))m_wnum1,
    (int (*)(void *))m_wday,
    (int (*)(void *))m_hour,
    (int (*)(void *))m_min,
    (int (*)(void *))m_sec,
    (VALUE (*)(void *))m_sf_in_sec,
    (VALUE (*)(void *))tmx_m_secs,
    (VALUE (*)(void *))tmx_m_msecs,
    (VALUE (*)(void *))tmx_m_of,
    (char *(*)(void *))tmx_m_zone
};

static void
set_tmx(VALUE self, struct tmx *tmx)
{
    get_d1(self);
    tmx->dat = (void *)dat;
    tmx->funcs = &tmx_funcs;
}

static VALUE
date_strftime_internal(int argc, VALUE *argv, VALUE self,
		       const char *default_fmt,
		       void (*func)(VALUE, struct tmx *))
{
    VALUE vfmt;
    const char *fmt;
    long len;
    char buffer[SMALLBUF], *buf = buffer;
    struct tmx tmx;
    VALUE str;

    rb_scan_args(argc, argv, "01", &vfmt);

    if (argc < 1)
	vfmt = rb_usascii_str_new2(default_fmt);
    else {
	StringValue(vfmt);
	if (!rb_enc_str_asciicompat_p(vfmt)) {
	    rb_raise(rb_eArgError,
		     "format should have ASCII compatible encoding");
	}
    }
    fmt = RSTRING_PTR(vfmt);
    len = RSTRING_LEN(vfmt);
    (*func)(self, &tmx);
    if (memchr(fmt, '\0', len)) {
	/* Ruby string may contain \0's. */
	const char *p = fmt, *pe = fmt + len;

	str = rb_str_new(0, 0);
	while (p < pe) {
	    len = date_strftime_alloc(&buf, p, &tmx);
	    rb_str_cat(str, buf, len);
	    p += strlen(p);
	    if (buf != buffer) {
		xfree(buf);
		buf = buffer;
	    }
	    for (fmt = p; p < pe && !*p; ++p);
	    if (p > fmt) rb_str_cat(str, fmt, p - fmt);
	}
	rb_enc_copy(str, vfmt);
	OBJ_INFECT(str, vfmt);
	return str;
    }
    else
	len = date_strftime_alloc(&buf, fmt, &tmx);

    str = rb_str_new(buf, len);
    if (buf != buffer) xfree(buf);
    rb_enc_copy(str, vfmt);
    OBJ_INFECT(str, vfmt);
    return str;
}

/*
 * call-seq:
 *    d.strftime([format='%F'])  ->  string
 *
 *  Formats date according to the directives in the given format
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
 *      %S - Second of the minute (00..59)
 *
 *      %L - Millisecond of the second (000..999)
 *      %N - Fractional seconds digits, default is 9 digits (nanosecond)
 *              %3N  millisecond (3 digits)
 *              %6N  microsecond (6 digits)
 *              %9N  nanosecond (9 digits)
 *              %12N picosecond (12 digits)
 *
 *    Time zone:
 *      %z - Time zone as hour and minute offset from UTC (e.g. +0900)
 *              %:z - hour and minute offset from UTC with a colon (e.g. +09:00)
 *              %::z - hour, minute and second offset from UTC (e.g. +09:00:00)
 *              %:::z - hour, minute and second offset from UTC
 *                                                (e.g. +09, +09:30, +09:30:30)
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
 *    Seconds since the Unix Epoch:
 *      %s - Number of seconds since 1970-01-01 00:00:00 UTC.
 *      %Q - Number of microseconds since 1970-01-01 00:00:00 UTC.
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
 *      %v - VMS date (%e-%b-%Y)
 *      %x - Same as %D
 *      %X - Same as %T
 *      %r - 12-hour time (%I:%M:%S %p)
 *      %R - 24-hour time (%H:%M)
 *      %T - 24-hour time (%H:%M:%S)
 *      %+ - date(1) (%a %b %e %H:%M:%S %Z %Y)
 *
 *  This method is similar to strftime() function defined in ISO C and POSIX.
 *  Several directives (%a, %A, %b, %B, %c, %p, %r, %x, %X, %E*, %O* and %Z)
 *  are locale dependent in the function.
 *  However this method is locale independent.
 *  So, the result may differ even if a same format string is used in other
 *  systems such as C.
 *  It is good practice to avoid %x and %X because there are corresponding
 *  locale independent representations, %D and %T.
 *
 *  Examples:
 *
 *    d = DateTime.new(2007,11,19,8,37,48,"-06:00")
 *				#=> #<DateTime: 2007-11-19T08:37:48-0600 ...>
 *    d.strftime("Printed on %m/%d/%Y")   #=> "Printed on 11/19/2007"
 *    d.strftime("at %I:%M%p")            #=> "at 08:37AM"
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
 * See also strftime(3) and strptime.
 */
static VALUE
d_lite_strftime(int argc, VALUE *argv, VALUE self)
{
    return date_strftime_internal(argc, argv, self,
				  "%Y-%m-%d", set_tmx);
}

static VALUE
strftimev(const char *fmt, VALUE self,
	  void (*func)(VALUE, struct tmx *))
{
    char buffer[SMALLBUF], *buf = buffer;
    struct tmx tmx;
    long len;
    VALUE str;

    (*func)(self, &tmx);
    len = date_strftime_alloc(&buf, fmt, &tmx);
    str = rb_usascii_str_new(buf, len);
    if (buf != buffer) xfree(buf);
    return str;
}

/*
 * call-seq:
 *    d.asctime  ->  string
 *    d.ctime    ->  string
 *
 * Returns a string in asctime(3) format (but without "\n\0" at the
 * end).  This method is equivalent to strftime('%c').
 *
 * See also asctime(3) or ctime(3).
 */
static VALUE
d_lite_asctime(VALUE self)
{
    return strftimev("%a %b %e %H:%M:%S %Y", self, set_tmx);
}

/*
 * call-seq:
 *    d.iso8601    ->  string
 *    d.xmlschema  ->  string
 *
 * This method is equivalent to strftime('%F').
 */
static VALUE
d_lite_iso8601(VALUE self)
{
    return strftimev("%Y-%m-%d", self, set_tmx);
}

/*
 * call-seq:
 *    d.rfc3339  ->  string
 *
 * This method is equivalent to strftime('%FT%T%:z').
 */
static VALUE
d_lite_rfc3339(VALUE self)
{
    return strftimev("%Y-%m-%dT%H:%M:%S%:z", self, set_tmx);
}

/*
 * call-seq:
 *    d.rfc2822  ->  string
 *    d.rfc822   ->  string
 *
 * This method is equivalent to strftime('%a, %-d %b %Y %T %z').
 */
static VALUE
d_lite_rfc2822(VALUE self)
{
    return strftimev("%a, %-d %b %Y %T %z", self, set_tmx);
}

/*
 * call-seq:
 *    d.httpdate  ->  string
 *
 * This method is equivalent to strftime('%a, %d %b %Y %T GMT').
 * See also RFC 2616.
 */
static VALUE
d_lite_httpdate(VALUE self)
{
    volatile VALUE dup = dup_obj_with_new_offset(self, 0);
    return strftimev("%a, %d %b %Y %T GMT", dup, set_tmx);
}

static VALUE
jisx0301_date(VALUE jd, VALUE y)
{
    VALUE a[2];

    if (f_lt_p(jd, INT2FIX(2405160)))
	return rb_usascii_str_new2("%Y-%m-%d");
    if (f_lt_p(jd, INT2FIX(2419614))) {
	a[0] = rb_usascii_str_new2("M%02d" ".%%m.%%d");
	a[1] = f_sub(y, INT2FIX(1867));
    }
    else if (f_lt_p(jd, INT2FIX(2424875))) {
	a[0] = rb_usascii_str_new2("T%02d" ".%%m.%%d");
	a[1] = f_sub(y, INT2FIX(1911));
    }
    else if (f_lt_p(jd, INT2FIX(2447535))) {
	a[0] = rb_usascii_str_new2("S%02d" ".%%m.%%d");
	a[1] = f_sub(y, INT2FIX(1925));
    }
    else {
	a[0] = rb_usascii_str_new2("H%02d" ".%%m.%%d");
	a[1] = f_sub(y, INT2FIX(1988));
    }
    return rb_f_sprintf(2, a);
}

/*
 * call-seq:
 *    d.jisx0301  ->  string
 *
 * Returns a string in a JIS X 0301 format.
 *
 * For example:
 *
 *    Date.new(2001,2,3).jisx0301	#=> "H13.02.03"
 */
static VALUE
d_lite_jisx0301(VALUE self)
{
    VALUE s;

    get_d1(self);
    s = jisx0301_date(m_real_local_jd(dat),
		      m_real_year(dat));
    return strftimev(RSTRING_PTR(s), self, set_tmx);
}

#ifndef NDEBUG
static VALUE
d_lite_marshal_dump_old(VALUE self)
{
    VALUE a;

    get_d1(self);

    a = rb_ary_new3(3,
		    m_ajd(dat),
		    m_of_in_day(dat),
		    DBL2NUM(m_sg(dat)));

    if (FL_TEST(self, FL_EXIVAR)) {
	rb_copy_generic_ivar(a, self);
	FL_SET(a, FL_EXIVAR);
    }

    return a;
}
#endif

/* :nodoc: */
static VALUE
d_lite_marshal_dump(VALUE self)
{
    VALUE a;

    get_d1(self);

    a = rb_ary_new3(6,
		    m_nth(dat),
		    INT2FIX(m_jd(dat)),
		    INT2FIX(m_df(dat)),
		    m_sf(dat),
		    INT2FIX(m_of(dat)),
		    DBL2NUM(m_sg(dat)));

    if (FL_TEST(self, FL_EXIVAR)) {
	rb_copy_generic_ivar(a, self);
	FL_SET(a, FL_EXIVAR);
    }

    return a;
}

/* :nodoc: */
static VALUE
d_lite_marshal_load(VALUE self, VALUE a)
{
    get_d1(self);

    if (TYPE(a) != T_ARRAY)
	rb_raise(rb_eTypeError, "expected an array");

    switch (RARRAY_LEN(a)) {
      case 3:
	{
	    VALUE ajd, of, sg, nth, sf;
	    int jd, df, rof;
	    double rsg;

	    ajd = RARRAY_PTR(a)[0];
	    of = RARRAY_PTR(a)[1];
	    sg = RARRAY_PTR(a)[2];

	    old_to_new(ajd, of, sg,
		       &nth, &jd, &df, &sf, &rof, &rsg);

	    if (!df && f_zero_p(sf) && !rof) {
		set_to_simple(&dat->s, nth, jd, rsg, 0, 0, 0, HAVE_JD);
	    } else {
		if (!complex_dat_p(dat))
		    rb_raise(rb_eArgError,
			     "cannot load complex into simple");

		set_to_complex(&dat->c, nth, jd, df, sf, rof, rsg,
			       0, 0, 0, 0, 0, 0,
			       HAVE_JD | HAVE_DF | COMPLEX_DAT);
	    }
	}
	break;
      case 6:
	{
	    VALUE nth, sf;
	    int jd, df, of;
	    double sg;

	    nth = RARRAY_PTR(a)[0];
	    jd = NUM2INT(RARRAY_PTR(a)[1]);
	    df = NUM2INT(RARRAY_PTR(a)[2]);
	    sf = RARRAY_PTR(a)[3];
	    of = NUM2INT(RARRAY_PTR(a)[4]);
	    sg = NUM2DBL(RARRAY_PTR(a)[5]);
	    if (!df && f_zero_p(sf) && !of) {
		set_to_simple(&dat->s, nth, jd, sg, 0, 0, 0, HAVE_JD);
	    } else {
		if (!complex_dat_p(dat))
		    rb_raise(rb_eArgError,
			     "cannot load complex into simple");

		set_to_complex(&dat->c, nth, jd, df, sf, of, sg,
			       0, 0, 0, 0, 0, 0,
			       HAVE_JD | HAVE_DF | COMPLEX_DAT);
	    }
	}
	break;
      default:
	rb_raise(rb_eTypeError, "invalid size");
	break;
    }

    if (FL_TEST(a, FL_EXIVAR)) {
	rb_copy_generic_ivar(self, a);
	FL_SET(self, FL_EXIVAR);
    }

    return self;
}


/* datetime */

/*
 * call-seq:
 *    DateTime.jd([jd=0[, hour=0[, minute=0[, second=0[, offset=0[, start=Date::ITALY]]]]]])  ->  datetime
 *
 * Creates a datetime object denoting the given chronological Julian
 * day number.
 *
 * For example:
 *
 *    DateTime.jd(2451944)	#=> #<DateTime: 2001-02-03T00:00:00+00:00 ...>
 *    DateTime.jd(2451945)	#=> #<DateTime: 2001-02-04T00:00:00+00:00 ...>
 *    DateTime.jd(Rational('0.5'))
 * 				#=> #<DateTime: -4712-01-01T12:00:00+00:00 ...>
 */
static VALUE
datetime_s_jd(int argc, VALUE *argv, VALUE klass)
{
    VALUE vjd, vh, vmin, vs, vof, vsg, jd, fr, fr2, ret;
    int h, min, s, rof;
    double sg;

    rb_scan_args(argc, argv, "06", &vjd, &vh, &vmin, &vs, &vof, &vsg);

    jd = INT2FIX(0);

    h = min = s = 0;
    fr2 = INT2FIX(0);
    rof = 0;
    sg = DEFAULT_SG;

    switch (argc) {
      case 6:
	val2sg(vsg, sg);
      case 5:
	val2off(vof, rof);
      case 4:
	num2int_with_frac(s, positive_inf);
      case 3:
	num2int_with_frac(min, 3);
      case 2:
	num2int_with_frac(h, 2);
      case 1:
	num2num_with_frac(jd, 1);
    }

    {
	VALUE nth;
	int rh, rmin, rs, rjd, rjd2;

	if (!c_valid_time_p(h, min, s, &rh, &rmin, &rs))
	    rb_raise(rb_eArgError, "invalid date");
	canon24oc();

	decode_jd(jd, &nth, &rjd);
	rjd2 = jd_local_to_utc(rjd,
			       time_to_df(rh, rmin, rs),
			       rof);

	ret = d_complex_new_internal(klass,
				     nth, rjd2,
				     0, INT2FIX(0),
				     rof, sg,
				     0, 0, 0,
				     rh, rmin, rs,
				     HAVE_JD | HAVE_TIME);
    }
    add_frac();
    return ret;
}

/*
 * call-seq:
 *    DateTime.ordinal([year=-4712[, yday=1[, hour=0[, minute=0[, second=0[, offset=0[, start=Date::ITALY]]]]]]])  ->  datetime
 *
 * Creates a date-time object denoting the given ordinal date.
 *
 * For example:
 *
 *    DateTime.ordinal(2001,34)	#=> #<DateTime: 2001-02-03T00:00:00+00:00 ...>
 *    DateTime.ordinal(2001,34,4,5,6,'+7')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 *    DateTime.ordinal(2001,-332,-20,-55,-54,'+7')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 */
static VALUE
datetime_s_ordinal(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vd, vh, vmin, vs, vof, vsg, y, fr, fr2, ret;
    int d, h, min, s, rof;
    double sg;

    rb_scan_args(argc, argv, "07", &vy, &vd, &vh, &vmin, &vs, &vof, &vsg);

    y = INT2FIX(-4712);
    d = 1;

    h = min = s = 0;
    fr2 = INT2FIX(0);
    rof = 0;
    sg = DEFAULT_SG;

    switch (argc) {
      case 7:
	val2sg(vsg, sg);
      case 6:
	val2off(vof, rof);
      case 5:
	num2int_with_frac(s, positive_inf);
      case 4:
	num2int_with_frac(min, 4);
      case 3:
	num2int_with_frac(h, 3);
      case 2:
	num2int_with_frac(d, 2);
      case 1:
	y = vy;
    }

    {
	VALUE nth;
	int ry, rd, rh, rmin, rs, rjd, rjd2, ns;

	if (!valid_ordinal_p(y, d, sg,
			     &nth, &ry,
			     &rd, &rjd,
			     &ns))
	    rb_raise(rb_eArgError, "invalid date");
	if (!c_valid_time_p(h, min, s, &rh, &rmin, &rs))
	    rb_raise(rb_eArgError, "invalid date");
	canon24oc();

	rjd2 = jd_local_to_utc(rjd,
			       time_to_df(rh, rmin, rs),
			       rof);

	ret = d_complex_new_internal(klass,
				     nth, rjd2,
				     0, INT2FIX(0),
				     rof, sg,
				     0, 0, 0,
				     rh, rmin, rs,
				     HAVE_JD | HAVE_TIME);
    }
    add_frac();
    return ret;
}

/*
 * call-seq:
 *    DateTime.civil([year=-4712[, month=1[, mday=1[, hour=0[, minute=0[, second=0[, offset=0[, start=Date::ITALY]]]]]]]])  ->  datetime
 *    DateTime.new([year=-4712[, month=1[, mday=1[, hour=0[, minute=0[, second=0[, offset=0[, start=Date::ITALY]]]]]]]])    ->  datetime
 *
 * Creates a date-time object denoting the given calendar date.
 *
 * For example:
 *
 *    DateTime.new(2001,2,3)	#=> #<DateTime: 2001-02-03T00:00:00+00:00 ...>
 *    DateTime.new(2001,2,3,4,5,6,'+7')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 *    DateTime.new(2001,-11,-26,-20,-55,-54,'+7')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 */
static VALUE
datetime_s_civil(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vm, vd, vh, vmin, vs, vof, vsg, y, fr, fr2, ret;
    int m, d, h, min, s, rof;
    double sg;

    rb_scan_args(argc, argv, "08", &vy, &vm, &vd, &vh, &vmin, &vs, &vof, &vsg);

    y = INT2FIX(-4712);
    m = 1;
    d = 1;

    h = min = s = 0;
    fr2 = INT2FIX(0);
    rof = 0;
    sg = DEFAULT_SG;

    switch (argc) {
      case 8:
	val2sg(vsg, sg);
      case 7:
	val2off(vof, rof);
      case 6:
	num2int_with_frac(s, positive_inf);
      case 5:
	num2int_with_frac(min, 5);
      case 4:
	num2int_with_frac(h, 4);
      case 3:
	num2int_with_frac(d, 3);
      case 2:
	m = NUM2INT(vm);
      case 1:
	y = vy;
    }

    if (guess_style(y, sg) < 0) {
	VALUE nth;
	int ry, rm, rd, rh, rmin, rs;

	if (!valid_gregorian_p(y, m, d,
			       &nth, &ry,
			       &rm, &rd))
	    rb_raise(rb_eArgError, "invalid date");
	if (!c_valid_time_p(h, min, s, &rh, &rmin, &rs))
	    rb_raise(rb_eArgError, "invalid date");
	canon24oc();

	ret = d_complex_new_internal(klass,
				     nth, 0,
				     0, INT2FIX(0),
				     rof, sg,
				     ry, rm, rd,
				     rh, rmin, rs,
				     HAVE_CIVIL | HAVE_TIME);
    }
    else {
	VALUE nth;
	int ry, rm, rd, rh, rmin, rs, rjd, rjd2, ns;

	if (!valid_civil_p(y, m, d, sg,
			   &nth, &ry,
			   &rm, &rd, &rjd,
			   &ns))
	    rb_raise(rb_eArgError, "invalid date");
	if (!c_valid_time_p(h, min, s, &rh, &rmin, &rs))
	    rb_raise(rb_eArgError, "invalid date");
	canon24oc();

	rjd2 = jd_local_to_utc(rjd,
			       time_to_df(rh, rmin, rs),
			       rof);

	ret = d_complex_new_internal(klass,
				     nth, rjd2,
				     0, INT2FIX(0),
				     rof, sg,
				     ry, rm, rd,
				     rh, rmin, rs,
				     HAVE_JD | HAVE_CIVIL | HAVE_TIME);
    }
    add_frac();
    return ret;
}

/*
 * call-seq:
 *    DateTime.commercial([cwyear=-4712[, cweek=1[, cwday=1[, hour=0[, minute=0[, second=0[, offset=0[, start=Date::ITALY]]]]]]]])  ->  datetime
 *
 * Creates a date-time object denoting the given week date.
 *
 * For example:
 *
 *    DateTime.commercial(2001)	#=> #<DateTime: 2001-01-01T00:00:00+00:00 ...>
 *    DateTime.commercial(2002)	#=> #<DateTime: 2001-12-31T00:00:00+00:00 ...>
 *    DateTime.commercial(2001,5,6,4,5,6,'+7')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 */
static VALUE
datetime_s_commercial(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vw, vd, vh, vmin, vs, vof, vsg, y, fr, fr2, ret;
    int w, d, h, min, s, rof;
    double sg;

    rb_scan_args(argc, argv, "08", &vy, &vw, &vd, &vh, &vmin, &vs, &vof, &vsg);

    y = INT2FIX(-4712);
    w = 1;
    d = 1;

    h = min = s = 0;
    fr2 = INT2FIX(0);
    rof = 0;
    sg = DEFAULT_SG;

    switch (argc) {
      case 8:
	val2sg(vsg, sg);
      case 7:
	val2off(vof, rof);
      case 6:
	num2int_with_frac(s, positive_inf);
      case 5:
	num2int_with_frac(min, 5);
      case 4:
	num2int_with_frac(h, 4);
      case 3:
	num2int_with_frac(d, 3);
      case 2:
	w = NUM2INT(vw);
      case 1:
	y = vy;
    }

    {
	VALUE nth;
	int ry, rw, rd, rh, rmin, rs, rjd, rjd2, ns;

	if (!valid_commercial_p(y, w, d, sg,
				&nth, &ry,
				&rw, &rd, &rjd,
				&ns))
	    rb_raise(rb_eArgError, "invalid date");
	if (!c_valid_time_p(h, min, s, &rh, &rmin, &rs))
	    rb_raise(rb_eArgError, "invalid date");
	canon24oc();

	rjd2 = jd_local_to_utc(rjd,
			       time_to_df(rh, rmin, rs),
			       rof);

	ret = d_complex_new_internal(klass,
				     nth, rjd2,
				     0, INT2FIX(0),
				     rof, sg,
				     0, 0, 0,
				     rh, rmin, rs,
				     HAVE_JD | HAVE_TIME);
    }
    add_frac();
    return ret;
}

#ifndef NDEBUG
static VALUE
datetime_s_weeknum(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vw, vd, vf, vh, vmin, vs, vof, vsg, y, fr, fr2, ret;
    int w, d, f, h, min, s, rof;
    double sg;

    rb_scan_args(argc, argv, "09", &vy, &vw, &vd, &vf,
		 &vh, &vmin, &vs, &vof, &vsg);

    y = INT2FIX(-4712);
    w = 0;
    d = 1;
    f = 0;

    h = min = s = 0;
    fr2 = INT2FIX(0);
    rof = 0;
    sg = DEFAULT_SG;

    switch (argc) {
      case 9:
	val2sg(vsg, sg);
      case 8:
	val2off(vof, rof);
      case 7:
	num2int_with_frac(s, positive_inf);
      case 6:
	num2int_with_frac(min, 6);
      case 5:
	num2int_with_frac(h, 5);
      case 4:
	f = NUM2INT(vf);
      case 3:
	num2int_with_frac(d, 4);
      case 2:
	w = NUM2INT(vw);
      case 1:
	y = vy;
    }

    {
	VALUE nth;
	int ry, rw, rd, rh, rmin, rs, rjd, rjd2, ns;

	if (!valid_weeknum_p(y, w, d, f, sg,
			     &nth, &ry,
			     &rw, &rd, &rjd,
			     &ns))
	    rb_raise(rb_eArgError, "invalid date");
	if (!c_valid_time_p(h, min, s, &rh, &rmin, &rs))
	    rb_raise(rb_eArgError, "invalid date");
	canon24oc();

	rjd2 = jd_local_to_utc(rjd,
			       time_to_df(rh, rmin, rs),
			       rof);
	ret = d_complex_new_internal(klass,
				     nth, rjd2,
				     0, INT2FIX(0),
				     rof, sg,
				     0, 0, 0,
				     rh, rmin, rs,
				     HAVE_JD | HAVE_TIME);
    }
    add_frac();
    return ret;
}

static VALUE
datetime_s_nth_kday(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vm, vn, vk, vh, vmin, vs, vof, vsg, y, fr, fr2, ret;
    int m, n, k, h, min, s, rof;
    double sg;

    rb_scan_args(argc, argv, "09", &vy, &vm, &vn, &vk,
		 &vh, &vmin, &vs, &vof, &vsg);

    y = INT2FIX(-4712);
    m = 1;
    n = 1;
    k = 1;

    h = min = s = 0;
    fr2 = INT2FIX(0);
    rof = 0;
    sg = DEFAULT_SG;

    switch (argc) {
      case 9:
	val2sg(vsg, sg);
      case 8:
	val2off(vof, rof);
      case 7:
	num2int_with_frac(s, positive_inf);
      case 6:
	num2int_with_frac(min, 6);
      case 5:
	num2int_with_frac(h, 5);
      case 4:
	num2int_with_frac(k, 4);
      case 3:
	n = NUM2INT(vn);
      case 2:
	m = NUM2INT(vm);
      case 1:
	y = vy;
    }

    {
	VALUE nth;
	int ry, rm, rn, rk, rh, rmin, rs, rjd, rjd2, ns;

	if (!valid_nth_kday_p(y, m, n, k, sg,
			      &nth, &ry,
			      &rm, &rn, &rk, &rjd,
			      &ns))
	    rb_raise(rb_eArgError, "invalid date");
	if (!c_valid_time_p(h, min, s, &rh, &rmin, &rs))
	    rb_raise(rb_eArgError, "invalid date");
	canon24oc();

	rjd2 = jd_local_to_utc(rjd,
			       time_to_df(rh, rmin, rs),
			       rof);
	ret = d_complex_new_internal(klass,
				     nth, rjd2,
				     0, INT2FIX(0),
				     rof, sg,
				     0, 0, 0,
				     rh, rmin, rs,
				     HAVE_JD | HAVE_TIME);
    }
    add_frac();
    return ret;
}
#endif

/*
 * call-seq:
 *    DateTime.now([start=Date::ITALY])  ->  datetime
 *
 * Creates a date-time object denoting the present time.
 *
 * For example:
 *
 *    DateTime.now		#=> #<DateTime: 2011-06-11T21:20:44+09:00 ...>
 */
static VALUE
datetime_s_now(int argc, VALUE *argv, VALUE klass)
{
    VALUE vsg, nth, ret;
    double sg;
#ifdef HAVE_CLOCK_GETTIME
    struct timespec ts;
#else
    struct timeval tv;
#endif
    time_t sec;
    struct tm tm;
    long sf, of;
    int y, ry, m, d, h, min, s;

    rb_scan_args(argc, argv, "01", &vsg);

    if (argc < 1)
	sg = DEFAULT_SG;
    else
	sg = NUM2DBL(vsg);

#ifdef HAVE_CLOCK_GETTIME
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	rb_sys_fail("clock_gettime");
    sec = ts.tv_sec;
#else
    if (gettimeofday(&tv, NULL) == -1)
	rb_sys_fail("gettimeofday");
    sec = tv.tv_sec;
#endif
    tzset();
    if (!localtime_r(&sec, &tm))
	rb_sys_fail("localtime");

    y = tm.tm_year + 1900;
    m = tm.tm_mon + 1;
    d = tm.tm_mday;
    h = tm.tm_hour;
    min = tm.tm_min;
    s = tm.tm_sec;
    if (s == 60)
	s = 59;
#ifdef HAVE_STRUCT_TM_TM_GMTOFF
    of = tm.tm_gmtoff;
#elif defined(HAVE_VAR_TIMEZONE)
#ifdef HAVE_VAR_ALTZONE
    of = (long)((tm.tm_isdst > 0) ? altzone : timezone);
#else
    of = (long)-timezone;
    if (tm.tm_isdst) {
	time_t sec2;

	tm.tm_isdst = 0;
	sec2 = mktime(&tm);
	of += (long)difftime(sec2, sec);
    }
#endif
#elif defined(HAVE_TIMEGM)
    {
	time_t sec2;

	sec2 = timegm(&tm);
	of = (long)difftime(sec2, sec);
    }
#else
    {
	struct tm tm2;
	time_t sec2;

	if (!gmtime_r(&sec, &tm2))
	    rb_sys_fail("gmtime");
	tm2.tm_isdst = tm.tm_isdst;
	sec2 = mktime(&tm2);
	of = (long)difftime(sec, sec2);
    }
#endif
#ifdef HAVE_CLOCK_GETTIME
    sf = ts.tv_nsec;
#else
    sf = tv.tv_usec * 1000;
#endif

    if (of < -DAY_IN_SECONDS || of > DAY_IN_SECONDS) {
	of = 0;
	rb_warning("invalid offset is ignored");
    }

    decode_year(INT2FIX(y), -1, &nth, &ry);

    ret = d_complex_new_internal(klass,
				 nth, 0,
				 0, LONG2NUM(sf),
				 (int)of, GREGORIAN,
				 ry, m, d,
				 h, min, s,
				 HAVE_CIVIL | HAVE_TIME);
    {
	get_d1(ret);
	set_sg(dat, sg);
    }
    return ret;
}

static VALUE
dt_new_by_frags(VALUE klass, VALUE hash, VALUE sg)
{
    VALUE jd, sf, t;
    int df, of;

    if (!c_valid_start_p(NUM2DBL(sg))) {
	sg = INT2FIX(DEFAULT_SG);
	rb_warning("invalid start is ignored");
    }

    if (NIL_P(hash))
	rb_raise(rb_eArgError, "invalid date");

    if (NIL_P(ref_hash("jd")) &&
	NIL_P(ref_hash("yday")) &&
	!NIL_P(ref_hash("year")) &&
	!NIL_P(ref_hash("mon")) &&
	!NIL_P(ref_hash("mday"))) {
	jd = rt__valid_civil_p(ref_hash("year"),
			       ref_hash("mon"),
			       ref_hash("mday"), sg);

	if (NIL_P(ref_hash("hour")))
	    set_hash("hour", INT2FIX(0));
	if (NIL_P(ref_hash("min")))
	    set_hash("min", INT2FIX(0));
	if (NIL_P(ref_hash("sec")))
	    set_hash("sec", INT2FIX(0));
	else if (f_gt_p(ref_hash("sec"), INT2FIX(59)))
	    set_hash("sec", INT2FIX(59));
    }
    else {
	hash = rt_rewrite_frags(hash);
	hash = rt_complete_frags(klass, hash);
	jd = rt__valid_date_frags_p(hash, sg);
    }

    if (NIL_P(jd))
	rb_raise(rb_eArgError, "invalid date");

    {
	int rh, rmin, rs;

	if (!c_valid_time_p(NUM2INT(ref_hash("hour")),
			    NUM2INT(ref_hash("min")),
			    NUM2INT(ref_hash("sec")),
			    &rh, &rmin, &rs))
	    rb_raise(rb_eArgError, "invalid date");

	df = time_to_df(rh, rmin, rs);
    }

    t = ref_hash("sec_fraction");
    if (NIL_P(t))
	sf = INT2FIX(0);
    else
	sf = sec_to_ns(t);

    t = ref_hash("offset");
    if (NIL_P(t))
	of = 0;
    else {
	of = NUM2INT(t);
	if (of < -DAY_IN_SECONDS || of > DAY_IN_SECONDS) {
	    of = 0;
	    rb_warning("invalid offset is ignored");
	}
    }
    {
	VALUE nth;
	int rjd, rjd2;

	decode_jd(jd, &nth, &rjd);
	rjd2 = jd_local_to_utc(rjd, df, of);
	df = df_local_to_utc(df, of);

	return d_complex_new_internal(klass,
				      nth, rjd2,
				      df, sf,
				      of, NUM2DBL(sg),
				      0, 0, 0,
				      0, 0, 0,
				      HAVE_JD | HAVE_DF);
    }
}

/*
 * call-seq:
 *    DateTime._strptime(string[, format='%FT%T%z'])  ->  hash
 *
 * Parses the given representation of date and time with the given
 * template, and returns a hash of parsed elements.
 *
 *  See also strptime(3) and strftime.
 */
static VALUE
datetime_s__strptime(int argc, VALUE *argv, VALUE klass)
{
    return date_s__strptime_internal(argc, argv, klass, "%FT%T%z");
}

/*
 * call-seq:
 *    DateTime.strptime([string='-4712-01-01T00:00:00+00:00'[, format='%FT%T%z'[ ,start=ITALY]]])  ->  datetime
 *
 * Parses the given representation of date and time with the given
 * template, and creates a date object.
 *
 * For example:
 *
 *    DateTime.strptime('2001-02-03T04:05:06+07:00', '%Y-%m-%dT%H:%M:%S%z')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 *    DateTime.strptime('03-02-2001 04:05:06 PM', '%d-%m-%Y %I:%M:%S %p')
 *				#=> #<DateTime: 2001-02-03T16:05:06+00:00 ...>
 *    DateTime.strptime('2001-W05-6T04:05:06+07:00', '%G-W%V-%uT%H:%M:%S%z')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 *    DateTime.strptime('2001 04 6 04 05 06 +7', '%Y %U %w %H %M %S %z')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 *    DateTime.strptime('2001 05 6 04 05 06 +7', '%Y %W %u %H %M %S %z')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 *    DateTime.strptime('-1', '%s')
 *				#=> #<DateTime: 1969-12-31T23:59:59+00:00 ...>
 *    DateTime.strptime('-1000', '%Q')
 *				#=> #<DateTime: 1969-12-31T23:59:59+00:00 ...>
 *    DateTime.strptime('sat3feb014pm+7', '%a%d%b%y%H%p%z')
 *				#=> #<DateTime: 2001-02-03T16:00:00+07:00 ...>
 *
 * See also strptime(3) and strftime.
 */
static VALUE
datetime_s_strptime(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, fmt, sg;

    rb_scan_args(argc, argv, "03", &str, &fmt, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01T00:00:00+00:00");
      case 1:
	fmt = rb_str_new2("%FT%T%z");
      case 2:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE argv2[2], hash;

	argv2[0] = str;
	argv2[1] = fmt;
	hash = date_s__strptime(2, argv2, klass);
	return dt_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    DateTime.parse(string='-4712-01-01T00:00:00+00:00'[, comp=true[, start=ITALY]])  ->  datetime
 *
 * Parses the given representation of date and time, and creates a
 * date object.
 *
 * If the optional second argument is true and the detected year is in
 * the range "00" to "99", makes it full.
 *
 * For example:
 *
 *    DateTime.parse('2001-02-03T04:05:06+07:00')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 *    DateTime.parse('20010203T040506+0700')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 *    DateTime.parse('3rd Feb 2001 04:05:06 PM')
 *				#=> #<DateTime: 2001-02-03T16:05:06+00:00 ...>
 */
static VALUE
datetime_s_parse(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, comp, sg;

    rb_scan_args(argc, argv, "03", &str, &comp, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01T00:00:00+00:00");
      case 1:
	comp = Qtrue;
      case 2:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE argv2[2], hash;

	argv2[0] = str;
	argv2[1] = comp;
	hash = date_s__parse(2, argv2, klass);
	return dt_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    DateTime.iso8601(string='-4712-01-01T00:00:00+00:00'[, start=ITALY])  ->  datetime
 *
 * Creates a new Date object by parsing from a string according to
 * some typical ISO 8601 formats.
 *
 * For example:
 *
 *    DateTime.iso8601('2001-02-03T04:05:06+07:00')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 *    DateTime.iso8601('20010203T040506+0700')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 *    DateTime.iso8601('2001-W05-6T04:05:06+07:00')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 */
static VALUE
datetime_s_iso8601(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01T00:00:00+00:00");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__iso8601(klass, str);
	return dt_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    DateTime.rfc3339(string='-4712-01-01T00:00:00+00:00'[, start=ITALY])  ->  datetime
 *
 * Creates a new Date object by parsing from a string according to
 * some typical RFC 3339 formats.
 *
 * For example:
 *
 *    DateTime.rfc3339('2001-02-03T04:05:06+07:00')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 */
static VALUE
datetime_s_rfc3339(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01T00:00:00+00:00");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__rfc3339(klass, str);
	return dt_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    DateTime.xmlschema(string='-4712-01-01T00:00:00+00:00'[, start=ITALY])  ->  datetime
 *
 * Creates a new Date object by parsing from a string according to
 * some typical XML Schema formats.
 *
 * For example:
 *
 *    DateTime.xmlschema('2001-02-03T04:05:06+07:00')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 */
static VALUE
datetime_s_xmlschema(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01T00:00:00+00:00");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__xmlschema(klass, str);
	return dt_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    DateTime.rfc2822(string='Mon, 1 Jan -4712 00:00:00 +0000'[, start=ITALY])  ->  datetime
 *    DateTime.rfc822(string='Mon, 1 Jan -4712 00:00:00 +0000'[, start=ITALY])   ->  datetime
 *
 * Creates a new Date object by parsing from a string according to
 * some typical RFC 2822 formats.
 *
 * For example:
 *
 *     DateTime.rfc2822('Sat, 3 Feb 2001 04:05:06 +0700')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 */
static VALUE
datetime_s_rfc2822(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("Mon, 1 Jan -4712 00:00:00 +0000");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__rfc2822(klass, str);
	return dt_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    DateTime.httpdate(string='Mon, 01 Jan -4712 00:00:00 GMT'[, start=ITALY])  ->  datetime
 *
 * Creates a new Date object by parsing from a string according to
 * some RFC 2616 format.
 *
 * For example:
 *
 *    DateTime.httpdate('Sat, 03 Feb 2001 04:05:06 GMT')
 *				#=> #<DateTime: 2001-02-03T04:05:06+00:00 ...>
 */
static VALUE
datetime_s_httpdate(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("Mon, 01 Jan -4712 00:00:00 GMT");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__httpdate(klass, str);
	return dt_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    DateTime.jisx0301(string='-4712-01-01T00:00:00+00:00'[, start=ITALY])  ->  datetime
 *
 * Creates a new Date object by parsing from a string according to
 * some typical JIS X 0301 formats.
 *
 * For example:
 *
 *    DateTime.jisx0301('H13.02.03T04:05:06+07:00')
 *				#=> #<DateTime: 2001-02-03T04:05:06+07:00 ...>
 */
static VALUE
datetime_s_jisx0301(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg;

    rb_scan_args(argc, argv, "02", &str, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01T00:00:00+00:00");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE hash = date_s__jisx0301(klass, str);
	return dt_new_by_frags(klass, hash, sg);
    }
}

/*
 * call-seq:
 *    dt.to_s  ->  string
 *
 * Returns a string in an ISO 8601 format (This method doesn't use the
 * expanded representations).
 *
 * For example:
 *
 *     DateTime.new(2001,2,3,4,5,6,'-7').to_s
 *				#=> "2001-02-03T04:05:06-07:00"
 */
static VALUE
dt_lite_to_s(VALUE self)
{
    return strftimev("%Y-%m-%dT%H:%M:%S%:z", self, set_tmx);
}

/*
 * call-seq:
 *    dt.strftime([format='%FT%T%:z'])  ->  string
 *
 *  Formats date according to the directives in the given format
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
 *      %S - Second of the minute (00..59)
 *
 *      %L - Millisecond of the second (000..999)
 *      %N - Fractional seconds digits, default is 9 digits (nanosecond)
 *              %3N  millisecond (3 digits)
 *              %6N  microsecond (6 digits)
 *              %9N  nanosecond (9 digits)
 *              %12N picosecond (12 digits)
 *
 *    Time zone:
 *      %z - Time zone as hour and minute offset from UTC (e.g. +0900)
 *              %:z - hour and minute offset from UTC with a colon (e.g. +09:00)
 *              %::z - hour, minute and second offset from UTC (e.g. +09:00:00)
 *              %:::z - hour, minute and second offset from UTC
 *                                                (e.g. +09, +09:30, +09:30:30)
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
 *    Seconds since the Unix Epoch:
 *      %s - Number of seconds since 1970-01-01 00:00:00 UTC.
 *      %Q - Number of microseconds since 1970-01-01 00:00:00 UTC.
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
 *      %v - VMS date (%e-%b-%Y)
 *      %x - Same as %D
 *      %X - Same as %T
 *      %r - 12-hour time (%I:%M:%S %p)
 *      %R - 24-hour time (%H:%M)
 *      %T - 24-hour time (%H:%M:%S)
 *      %+ - date(1) (%a %b %e %H:%M:%S %Z %Y)
 *
 *  This method is similar to strftime() function defined in ISO C and POSIX.
 *  Several directives (%a, %A, %b, %B, %c, %p, %r, %x, %X, %E*, %O* and %Z)
 *  are locale dependent in the function.
 *  However this method is locale independent.
 *  So, the result may differ even if a same format string is used in other
 *  systems such as C.
 *  It is good practice to avoid %x and %X because there are corresponding
 *  locale independent representations, %D and %T.
 *
 *  Examples:
 *
 *    d = DateTime.new(2007,11,19,8,37,48,"-06:00")
 *				#=> #<DateTime: 2007-11-19T08:37:48-0600 ...>
 *    d.strftime("Printed on %m/%d/%Y")   #=> "Printed on 11/19/2007"
 *    d.strftime("at %I:%M%p")            #=> "at 08:37AM"
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
 * See also strftime(3) and strptime.
 */
static VALUE
dt_lite_strftime(int argc, VALUE *argv, VALUE self)
{
    return date_strftime_internal(argc, argv, self,
				  "%Y-%m-%dT%H:%M:%S%:z", set_tmx);
}

static VALUE
iso8601_timediv(VALUE self, VALUE n)
{
    VALUE fmt;

    n = to_integer(n);
    fmt = rb_usascii_str_new2("T%H:%M:%S");
    if (f_gt_p(n, INT2FIX(0))) {
	VALUE argv[3];

	get_d1(self);

	argv[0] = rb_usascii_str_new2(".%0*d");
	argv[1] = n;
	argv[2] = f_round(f_quo(m_sf_in_sec(dat),
				f_quo(INT2FIX(1),
				      f_expt(INT2FIX(10), n))));
	rb_str_append(fmt, rb_f_sprintf(3, argv));
    }
    rb_str_append(fmt, rb_usascii_str_new2("%:z"));
    return strftimev(RSTRING_PTR(fmt), self, set_tmx);
}

/*
 * call-seq:
 *    dt.iso8601([n=0])    ->  string
 *    dt.xmlschema([n=0])  ->  string
 *
 * This method is equivalent to strftime('%FT%T').  The optional
 * argument n is length of fractional seconds.
 *
 * For example:
 *
 *    DateTime.parse('2001-02-03T04:05:06.123456789+07:00').iso8601(9)
 *				#=> "2001-02-03T04:05:06.123456789+07:00"
 */
static VALUE
dt_lite_iso8601(int argc, VALUE *argv, VALUE self)
{
    VALUE n;

    rb_scan_args(argc, argv, "01", &n);

    if (argc < 1)
	n = INT2FIX(0);

    return f_add(strftimev("%Y-%m-%d", self, set_tmx),
		 iso8601_timediv(self, n));
}

/*
 * call-seq:
 *    dt.rfc3339([n=0])  ->  string
 *
 * This method is equivalent to strftime('%FT%T').  The optional
 * argument n is length of fractional seconds.
 *
 * For example:
 *
 *    DateTime.parse('2001-02-03T04:05:06.123456789+07:00').rfc3339(9)
 *				#=> "2001-02-03T04:05:06.123456789+07:00"
 */
static VALUE
dt_lite_rfc3339(int argc, VALUE *argv, VALUE self)
{
    return dt_lite_iso8601(argc, argv, self);
}

/*
 * call-seq:
 *    dt.jisx0301([n=0])  ->  string
 *
 * Returns a string in a JIS X 0301 format.  The optional argument n
 * is length of fractional seconds.
 *
 * For example:
 *
 *    DateTime.parse('2001-02-03T04:05:06.123456789+07:00').jisx0301(9)
 *				#=> "H13.02.03T04:05:06.123456789+07:00"
 */
static VALUE
dt_lite_jisx0301(int argc, VALUE *argv, VALUE self)
{
    VALUE n, s;

    rb_scan_args(argc, argv, "01", &n);

    if (argc < 1)
	n = INT2FIX(0);

    {
	get_d1(self);
	s = jisx0301_date(m_real_local_jd(dat),
			  m_real_year(dat));
	return rb_str_append(strftimev(RSTRING_PTR(s), self, set_tmx),
			     iso8601_timediv(self, n));
    }
}

/* conversions */

#define f_getlocal(x) rb_funcall(x, rb_intern("getlocal"), 0)
#define f_subsec(x) rb_funcall(x, rb_intern("subsec"), 0)
#define f_utc_offset(x) rb_funcall(x, rb_intern("utc_offset"), 0)
#define f_local3(x,y,m,d) rb_funcall(x, rb_intern("local"), 3, y, m, d)
#define f_utc6(x,y,m,d,h,min,s) rb_funcall(x, rb_intern("utc"), 6,\
					   y, m, d, h, min, s)

/*
 * call-seq:
 *    t.to_time  ->  time
 *
 * Returns a copy of self as local mode.
 */
static VALUE
time_to_time(VALUE self)
{
    return rb_funcall(self, rb_intern("getlocal"), 0);
}

/*
 * call-seq:
 *    t.to_date  ->  date
 *
 * Returns a Date object which denotes self.
 */
static VALUE
time_to_date(VALUE self)
{
    VALUE y, nth, ret;
    int ry, m, d;

    y = f_year(self);
    m = FIX2INT(f_mon(self));
    d = FIX2INT(f_mday(self));

    decode_year(y, -1, &nth, &ry);

    ret = d_simple_new_internal(cDate,
				nth, 0,
				GREGORIAN,
				ry, m, d,
				HAVE_CIVIL);
    {
	get_d1(ret);
	set_sg(dat, DEFAULT_SG);
    }
    return ret;
}

/*
 * call-seq:
 *    t.to_datetime  ->  datetime
 *
 * Returns a DateTime object which denotes self.
 */
static VALUE
time_to_datetime(VALUE self)
{
    VALUE y, sf, nth, ret;
    int ry, m, d, h, min, s, of;

    y = f_year(self);
    m = FIX2INT(f_mon(self));
    d = FIX2INT(f_mday(self));

    h = FIX2INT(f_hour(self));
    min = FIX2INT(f_min(self));
    s = FIX2INT(f_sec(self));
    if (s == 60)
	s = 59;

    sf = sec_to_ns(f_subsec(self));
    of = FIX2INT(f_utc_offset(self));

    decode_year(y, -1, &nth, &ry);

    ret = d_complex_new_internal(cDateTime,
				 nth, 0,
				 0, sf,
				 of, DEFAULT_SG,
				 ry, m, d,
				 h, min, s,
				 HAVE_CIVIL | HAVE_TIME);
    {
	get_d1(ret);
	set_sg(dat, DEFAULT_SG);
    }
    return ret;
}

/*
 * call-seq:
 *    d.to_time  ->  time
 *
 * Returns a Time object which denotes self.
 */
static VALUE
date_to_time(VALUE self)
{
    get_d1(self);

    return f_local3(rb_cTime,
		    m_real_year(dat),
		    INT2FIX(m_mon(dat)),
		    INT2FIX(m_mday(dat)));
}

/*
 * call-seq:
 *    d.to_date  ->  self
 *
 * Returns self;
 */
static VALUE
date_to_date(VALUE self)
{
    return self;
}

/*
 * call-seq:
 *    d.to_datetime  -> datetime
 *
 * Returns a DateTime object which denotes self.
 */
static VALUE
date_to_datetime(VALUE self)
{
    get_d1a(self);

    if (simple_dat_p(adat)) {
	VALUE new = d_lite_s_alloc_simple(cDateTime);
	{
	    get_d1b(new);
	    bdat->s = adat->s;
	    return new;
	}
    }
    else {
	VALUE new = d_lite_s_alloc_complex(cDateTime);
	{
	    get_d1b(new);
	    bdat->c = adat->c;
	    bdat->c.df = 0;
	    bdat->c.sf = INT2FIX(0);
#ifndef USE_PACK
	    bdat->c.hour = 0;
	    bdat->c.min = 0;
	    bdat->c.sec = 0;
#else
	    bdat->c.pc = PACK5(EX_MON(adat->c.pc), EX_MDAY(adat->c.pc),
			       0, 0, 0);
	    bdat->c.flags |= HAVE_DF | HAVE_TIME;
#endif
	    return new;
	}
    }
}

/*
 * call-seq:
 *    dt.to_time  ->  time
 *
 * Returns a Time object which denotes self.
 */
static VALUE
datetime_to_time(VALUE self)
{
    volatile VALUE dup = dup_obj_with_new_offset(self, 0);
    {
	VALUE t;

	get_d1(dup);

	t = f_utc6(rb_cTime,
		   m_real_year(dat),
		   INT2FIX(m_mon(dat)),
		   INT2FIX(m_mday(dat)),
		   INT2FIX(m_hour(dat)),
		   INT2FIX(m_min(dat)),
		   f_add(INT2FIX(m_sec(dat)),
			 m_sf_in_sec(dat)));
	return f_getlocal(t);
    }
}

/*
 * call-seq:
 *    dt.to_date  ->  date
 *
 * Returns a Date object which denotes self.
 */
static VALUE
datetime_to_date(VALUE self)
{
    get_d1a(self);

    if (simple_dat_p(adat)) {
	VALUE new = d_lite_s_alloc_simple(cDate);
	{
	    get_d1b(new);
	    bdat->s = adat->s;
	    bdat->s.jd = m_local_jd(adat);
	    return new;
	}
    }
    else {
	VALUE new = d_lite_s_alloc_simple(cDate);
	{
	    get_d1b(new);
	    copy_complex_to_simple(&bdat->s, &adat->c)
	    bdat->s.jd = m_local_jd(adat);
	    bdat->s.flags &= ~(HAVE_DF | HAVE_TIME | COMPLEX_DAT);
	    return new;
	}
    }
}

/*
 * call-seq:
 *    dt.to_datetime  ->  self
 *
 * Returns self.
 */
static VALUE
datetime_to_datetime(VALUE self)
{
    return self;
}

#ifndef NDEBUG
/* tests */

#define MIN_YEAR -4713
#define MAX_YEAR 1000000
#define MIN_JD -327
#define MAX_JD 366963925

static int
test_civil(int from, int to, double sg)
{
    int j;

    fprintf(stderr, "test_civil: %d...%d (%d) - %.0f\n",
	    from, to, to - from, sg);
    for (j = from; j <= to; j++) {
	int y, m, d, rj, ns;

	c_jd_to_civil(j, sg, &y, &m, &d);
	c_civil_to_jd(y, m, d, sg, &rj, &ns);
	if (j != rj) {
	    fprintf(stderr, "%d != %d\n", j, rj);
	    return 0;
	}
    }
    return 1;
}

static VALUE
date_s_test_civil(VALUE klass)
{
    if (!test_civil(MIN_JD, MIN_JD + 366, GREGORIAN))
	return Qfalse;
    if (!test_civil(2305814, 2598007, GREGORIAN))
	return Qfalse;
    if (!test_civil(MAX_JD - 366, MAX_JD, GREGORIAN))
	return Qfalse;

    if (!test_civil(MIN_JD, MIN_JD + 366, ITALY))
	return Qfalse;
    if (!test_civil(2305814, 2598007, ITALY))
	return Qfalse;
    if (!test_civil(MAX_JD - 366, MAX_JD, ITALY))
	return Qfalse;

    return Qtrue;
}

static int
test_ordinal(int from, int to, double sg)
{
    int j;

    fprintf(stderr, "test_ordinal: %d...%d (%d) - %.0f\n",
	    from, to, to - from, sg);
    for (j = from; j <= to; j++) {
	int y, d, rj, ns;

	c_jd_to_ordinal(j, sg, &y, &d);
	c_ordinal_to_jd(y, d, sg, &rj, &ns);
	if (j != rj) {
	    fprintf(stderr, "%d != %d\n", j, rj);
	    return 0;
	}
    }
    return 1;
}

static VALUE
date_s_test_ordinal(VALUE klass)
{
    if (!test_ordinal(MIN_JD, MIN_JD + 366, GREGORIAN))
	return Qfalse;
    if (!test_ordinal(2305814, 2598007, GREGORIAN))
	return Qfalse;
    if (!test_ordinal(MAX_JD - 366, MAX_JD, GREGORIAN))
	return Qfalse;

    if (!test_ordinal(MIN_JD, MIN_JD + 366, ITALY))
	return Qfalse;
    if (!test_ordinal(2305814, 2598007, ITALY))
	return Qfalse;
    if (!test_ordinal(MAX_JD - 366, MAX_JD, ITALY))
	return Qfalse;

    return Qtrue;
}

static int
test_commercial(int from, int to, double sg)
{
    int j;

    fprintf(stderr, "test_commercial: %d...%d (%d) - %.0f\n",
	    from, to, to - from, sg);
    for (j = from; j <= to; j++) {
	int y, w, d, rj, ns;

	c_jd_to_commercial(j, sg, &y, &w, &d);
	c_commercial_to_jd(y, w, d, sg, &rj, &ns);
	if (j != rj) {
	    fprintf(stderr, "%d != %d\n", j, rj);
	    return 0;
	}
    }
    return 1;
}

static VALUE
date_s_test_commercial(VALUE klass)
{
    if (!test_commercial(MIN_JD, MIN_JD + 366, GREGORIAN))
	return Qfalse;
    if (!test_commercial(2305814, 2598007, GREGORIAN))
	return Qfalse;
    if (!test_commercial(MAX_JD - 366, MAX_JD, GREGORIAN))
	return Qfalse;

    if (!test_commercial(MIN_JD, MIN_JD + 366, ITALY))
	return Qfalse;
    if (!test_commercial(2305814, 2598007, ITALY))
	return Qfalse;
    if (!test_commercial(MAX_JD - 366, MAX_JD, ITALY))
	return Qfalse;

    return Qtrue;
}

static int
test_weeknum(int from, int to, int f, double sg)
{
    int j;

    fprintf(stderr, "test_weeknum: %d...%d (%d) - %.0f\n",
	    from, to, to - from, sg);
    for (j = from; j <= to; j++) {
	int y, w, d, rj, ns;

	c_jd_to_weeknum(j, f, sg, &y, &w, &d);
	c_weeknum_to_jd(y, w, d, f, sg, &rj, &ns);
	if (j != rj) {
	    fprintf(stderr, "%d != %d\n", j, rj);
	    return 0;
	}
    }
    return 1;
}

static VALUE
date_s_test_weeknum(VALUE klass)
{
    int f;

    for (f = 0; f <= 1; f++) {
	if (!test_weeknum(MIN_JD, MIN_JD + 366, f, GREGORIAN))
	    return Qfalse;
	if (!test_weeknum(2305814, 2598007, f, GREGORIAN))
	    return Qfalse;
	if (!test_weeknum(MAX_JD - 366, MAX_JD, f, GREGORIAN))
	    return Qfalse;

	if (!test_weeknum(MIN_JD, MIN_JD + 366, f, ITALY))
	    return Qfalse;
	if (!test_weeknum(2305814, 2598007, f, ITALY))
	    return Qfalse;
	if (!test_weeknum(MAX_JD - 366, MAX_JD, f, ITALY))
	    return Qfalse;
    }

    return Qtrue;
}

static int
test_nth_kday(int from, int to, double sg)
{
    int j;

    fprintf(stderr, "test_nth_kday: %d...%d (%d) - %.0f\n",
	    from, to, to - from, sg);
    for (j = from; j <= to; j++) {
	int y, m, n, k, rj, ns;

	c_jd_to_nth_kday(j, sg, &y, &m, &n, &k);
	c_nth_kday_to_jd(y, m, n, k, sg, &rj, &ns);
	if (j != rj) {
	    fprintf(stderr, "%d != %d\n", j, rj);
	    return 0;
	}
    }
    return 1;
}

static VALUE
date_s_test_nth_kday(VALUE klass)
{
    if (!test_nth_kday(MIN_JD, MIN_JD + 366, GREGORIAN))
	return Qfalse;
    if (!test_nth_kday(2305814, 2598007, GREGORIAN))
	return Qfalse;
    if (!test_nth_kday(MAX_JD - 366, MAX_JD, GREGORIAN))
	return Qfalse;

    if (!test_nth_kday(MIN_JD, MIN_JD + 366, ITALY))
	return Qfalse;
    if (!test_nth_kday(2305814, 2598007, ITALY))
	return Qfalse;
    if (!test_nth_kday(MAX_JD - 366, MAX_JD, ITALY))
	return Qfalse;

    return Qtrue;
}

static int
test_unit_v2v(VALUE i,
	      VALUE (* conv1)(VALUE),
	      VALUE (* conv2)(VALUE))
{
    VALUE c, o;
    c = (*conv1)(i);
    o = (*conv2)(c);
    return f_eqeq_p(o, i);
}

static int
test_unit_v2v_iter2(VALUE (* conv1)(VALUE),
		    VALUE (* conv2)(VALUE))
{
    if (!test_unit_v2v(INT2FIX(0), conv1, conv2))
	return 0;
    if (!test_unit_v2v(INT2FIX(1), conv1, conv2))
	return 0;
    if (!test_unit_v2v(INT2FIX(2), conv1, conv2))
	return 0;
    if (!test_unit_v2v(INT2FIX(3), conv1, conv2))
	return 0;
    if (!test_unit_v2v(INT2FIX(11), conv1, conv2))
	return 0;
    if (!test_unit_v2v(INT2FIX(65535), conv1, conv2))
	return 0;
    if (!test_unit_v2v(INT2FIX(1073741823), conv1, conv2))
	return 0;
    if (!test_unit_v2v(INT2NUM(1073741824), conv1, conv2))
	return 0;
    if (!test_unit_v2v(rb_rational_new2(INT2FIX(0), INT2FIX(1)), conv1, conv2))
	return 0;
    if (!test_unit_v2v(rb_rational_new2(INT2FIX(1), INT2FIX(1)), conv1, conv2))
	return 0;
    if (!test_unit_v2v(rb_rational_new2(INT2FIX(1), INT2FIX(2)), conv1, conv2))
	return 0;
    if (!test_unit_v2v(rb_rational_new2(INT2FIX(2), INT2FIX(3)), conv1, conv2))
	return 0;
    return 1;
}

static int
test_unit_v2v_iter(VALUE (* conv1)(VALUE),
		   VALUE (* conv2)(VALUE))
{
    if (!test_unit_v2v_iter2(conv1, conv2))
	return 0;
    if (!test_unit_v2v_iter2(conv2, conv1))
	return 0;
    return 1;
}

static VALUE
date_s_test_unit_conv(VALUE klass)
{
    if (!test_unit_v2v_iter(sec_to_day, day_to_sec))
	return Qfalse;
    if (!test_unit_v2v_iter(ms_to_sec, sec_to_ms))
	return Qfalse;
    if (!test_unit_v2v_iter(ns_to_day, day_to_ns))
	return Qfalse;
    if (!test_unit_v2v_iter(ns_to_sec, sec_to_ns))
	return Qfalse;
    return Qtrue;
}

static VALUE
date_s_test_all(VALUE klass)
{
    if (date_s_test_civil(klass) == Qfalse)
	return Qfalse;
    if (date_s_test_ordinal(klass) == Qfalse)
	return Qfalse;
    if (date_s_test_commercial(klass) == Qfalse)
	return Qfalse;
    if (date_s_test_weeknum(klass) == Qfalse)
	return Qfalse;
    if (date_s_test_nth_kday(klass) == Qfalse)
	return Qfalse;
    if (date_s_test_unit_conv(klass) == Qfalse)
	return Qfalse;
    return Qtrue;
}
#endif

static const char *monthnames[] = {
    NULL,
    "January", "February", "March",
    "April", "May", "June",
    "July", "August", "September",
    "October", "November", "December"
};

static const char *abbr_monthnames[] = {
    NULL,
    "Jan", "Feb", "Mar", "Apr",
    "May", "Jun", "Jul", "Aug",
    "Sep", "Oct", "Nov", "Dec"
};

static const char *daynames[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

static const char *abbr_daynames[] = {
    "Sun", "Mon", "Tue", "Wed",
    "Thu", "Fri", "Sat"
};

static VALUE
mk_ary_of_str(long len, const char *a[])
{
    VALUE o;
    long i;

    o = rb_ary_new2(len);
    for (i = 0; i < len; i++) {
	VALUE e;

	if (!a[i])
	    e = Qnil;
	else {
	    e = rb_usascii_str_new2(a[i]);
	    rb_obj_freeze(e);
	}
	rb_ary_push(o, e);
    }
    rb_obj_freeze(o);
    return o;
}

void
Init_date_core(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    assert(fprintf(stderr, "assert() is now active\n"));

    id_cmp = rb_intern("<=>");
    id_le_p = rb_intern("<=");
    id_ge_p = rb_intern(">=");
    id_eqeq_p = rb_intern("==");

    half_days_in_day = rb_rational_new2(INT2FIX(1), INT2FIX(2));

#if (LONG_MAX / DAY_IN_SECONDS) > SECOND_IN_NANOSECONDS
    day_in_nanoseconds = LONG2NUM((long)DAY_IN_SECONDS *
				  SECOND_IN_NANOSECONDS);
#elif defined HAVE_LONG_LONG
    day_in_nanoseconds = LL2NUM((LONG_LONG)DAY_IN_SECONDS *
				SECOND_IN_NANOSECONDS);
#else
    day_in_nanoseconds = f_mul(INT2FIX(DAY_IN_SECONDS),
			       INT2FIX(SECOND_IN_NANOSECONDS));
#endif

    rb_gc_register_mark_object(half_days_in_day);
    rb_gc_register_mark_object(day_in_nanoseconds);

    positive_inf = +INFINITY;
    negative_inf = -INFINITY;

    /*
     * date and datetime class - Tadayoshi Funaba 1998-2011
     *
     * 'date' provides two classes Date and DateTime.
     *
     * == Terms and definitions
     *
     * Some terms and definitions are based on ISO 8601 and JIS X 0301.
     *
     * === calendar date
     *
     * The calendar date is a particular day of a calendar year,
     * identified by its ordinal number within a calendar month within
     * that year.
     *
     * In those classes, this is so-called "civil".
     *
     * === ordinal date
     *
     * The ordinal date is a particular day of a calendar year identified
     * by its ordinal number within the year.
     *
     * In those classes, this is so-called "ordinal".
     *
     * === week date
     *
     * The week date is a date identified by calendar week and day numbers.
     *
     * The calendar week is a seven day period within a calendar year,
     * starting on a Monday and identified by its ordinal number within
     * the year; the first calendar week of the year is the one that
     * includes the first Thursday of that year.  In the Gregorian
     * calendar, this is equivalent to the week which includes January 4.
     *
     * In those classes, this so-called "commercial".
     *
     * === julian day number
     *
     * The Julian day number is in elapsed days since noon (Greenwich mean
     * time) on January 1, 4713 BCE (in the Julian calendar).
     *
     * In this document, the astronomical Julian day number is same as the
     * original Julian day number.  And the chronological Julian day
     * number is a variation of the Julian day number.  Its days begin at
     * midnight on local time.
     *
     * In this document, when the term "Julian day number" simply appears,
     * it just refers to "chronological Julian day number", not the
     * original.
     *
     * In those classes, those are so-called "ajd" and "jd".
     *
     * === modified julian day number
     *
     * The modified Julian day number is in elapsed days since midnight
     * (Coordinated universal time) on November 17, 1858 CE (in the
     * Gregorian calendar).
     *
     * In this document, the astronomical modified Julian day number is
     * same as the original modified Julian day number.  And the
     * chronological modified Julian day number is a variation of the
     * modified Julian day number.  Its days begin at midnight on local
     * time.
     *
     * In this document, when the term "modified Julian day number" simply
     * appears, it just refers to "chronological modified Julian day
     * number", not the original.
     *
     * In those classes, this is so-called "mjd".
     *
     *
     * == Date
     *
     * A subclass of Object includes Comparable module, easily handles
     * date.
     *
     * Date object is created with Date::new, Date::jd, Date::ordinal,
     * Date::commercial, Date::parse, Date::strptime, Date::today,
     * Time#to_date or etc.
     *
     *     require 'date'
     *
     *     Date.new(2001,2,3)		#=> #<Date: 2001-02-03 ...>
     *     Date.jd(2451944)		#=> #<Date: 2001-02-03 ...>
     *     Date.ordinal(2001,34)	#=> #<Date: 2001-02-03 ...>
     *     Date.commercial(2001,5,6)	#=> #<Date: 2001-02-03 ...>
     *     Date.parse('2001-02-03')	#=> #<Date: 2001-02-03 ...>
     *     Date.strptime('03-02-2001', '%d-%m-%Y')
     *					#=> #<Date: 2001-02-03 ...>
     *     Time.new(2001,2,3).to_date	#=> #<Date: 2001-02-03 ...>
     *
     * All date objects are immutable; hence cannot modify themselves.
     *
     * The concept of this date object can be represented as a tuple
     * of the day count, the offset and the day of calendar reform.
     *
     * The day count denotes the absolute position of a temporal
     * dimension.  The offset is relative adjustment, which determines
     * decoded local time with the day count.  The day of calendar
     * reform denotes the start day of the new style.  The old style
     * of the West is the Julian calendar which was adopted by
     * Caersar.  The new style is the Gregorian calendar, which is the
     * current civil calendar of many countries.
     *
     * The day count is virtually the astronomical Julian day number.
     * The offset in this class is usually zero, and cannot be
     * specified directly.
     *
     * An optional argument the day of calendar reform (start) as a
     * Julian day number, which should be 2298874 to 2426355 or -/+oo.
     * The default value is Date::ITALY (2299161=1582-10-15).  See
     * also sample/cal.rb.
     *
     *     $ ruby sample/cal.rb -c it 10 1582
     *         October 1582
     *      S  M Tu  W Th  F  S
     *         1  2  3  4 15 16
     *     17 18 19 20 21 22 23
     *     24 25 26 27 28 29 30
     *     31
     *
     *     $ ruby sample/cal.rb -c gb  9 1752
     *        September 1752
     *      S  M Tu  W Th  F  S
     *            1  2 14 15 16
     *     17 18 19 20 21 22 23
     *     24 25 26 27 28 29 30
     *
     * Date object has various methods. See each reference.
     *
     *     d = Date.parse('3rd Feb 2001')
     *					#=> #<Date: 2001-02-03 ...>
     *     d.year			#=> 2001
     *     d.mon			#=> 2
     *     d.mday			#=> 3
     *     d.wday			#=> 6
     *     d += 1			#=> #<Date: 2001-02-04 ...>
     *     d.strftime('%a %d %b %Y')	#=> "Sun 04 Feb 2001"
     *
     *
     * == DateTime
     *
     * A subclass of Date easily handles date, hour, minute, second and
     * offset.
     *
     * DateTime does not consider any leapseconds, does not track
     * any summer time rules.
     *
     * DateTime object is created with DateTime::new, DateTime::jd,
     * DateTime::ordinal, DateTime::commercial, DateTime::parse,
     * DateTime::strptime, DateTime::now, Time#to_datetime or etc.
     *
     *     require 'date'
     *
     *     DateTime.new(2001,2,3,4,5,6)
     *				#=> #<DateTime: 2001-02-03T04:05:06+00:00 ...>
     *
     * The last element of day, hour, minute or senond can be
     * fractional number. The fractional number's precision is assumed
     * at most nanosecond.
     *
     *     DateTime.new(2001,2,3.5)
     *				#=> #<DateTime: 2001-02-03T12:00:00+00:00 ...>
     *
     * An optional argument the offset indicates the difference
     * between the local time and UTC.  For example, Rational(3,24)
     * represents ahead of 3 hours of UTC, Rational(-5,24) represents
     * behind of 5 hours of UTC.  The offset should be -1 to +1, and
     * its precision is assumed at most second.  The default value is
     * zero (equals to UTC).
     *
     *     DateTime.new(2001,2,3,4,5,6,Rational(3,24))
     *				#=> #<DateTime: 2001-02-03T03:04:05+03:00 ...>
     * also accepts string form.
     *
     *     DateTime.new(2001,2,3,4,5,6,'+03:00')
     *				#=> #<DateTime: 2001-02-03T03:04:05+03:00 ...>
     *
     * An optional argument the day of calendar reform (start) denotes
     * a Julian day number, which should be 2298874 to 2426355 or
     * -/+oo.  The default value is Date::ITALY (2299161=1582-10-15).
     *
     * DateTime object has various methods. See each reference.
     *
     *     d = DateTime.parse('3rd Feb 2001 04:05:06+03:30')
     *				#=> #<DateTime: 2001-02-03T04:05:06+03:30 ...>
     *     d.hour		#=> 4
     *     d.min		#=> 5
     *     d.sec		#=> 6
     *     d.offset		#=> (7/48)
     *     d.zone		#=> "+03:30"
     *     d += Rational('1.5')
     *				#=> #<DateTime: 2001-02-04%16:05:06+03:30 ...>
     *     d = d.new_offset('+09:00')
     *				#=> #<DateTime: 2001-02-04%21:35:06+09:00 ...>
     *     d.strftime('%I:%M:%S %p')
     *				#=> "09:35:06 PM"
     *     d > DateTime.new(1999)
     *				#=> true
     */
    cDate = rb_define_class("Date", rb_cObject);

    rb_include_module(cDate, rb_mComparable);

    /* An array of stirng of full month name in English.  The first
     * element is nil.
     */
    rb_define_const(cDate, "MONTHNAMES", mk_ary_of_str(13, monthnames));

    /* An array of string of abbreviated month name in English.  The
     * first element is nil.
     */
    rb_define_const(cDate, "ABBR_MONTHNAMES",
		    mk_ary_of_str(13, abbr_monthnames));

    /* An array of string of full name of days of the week in English.
     * The first is "Sunday".
     */
    rb_define_const(cDate, "DAYNAMES", mk_ary_of_str(7, daynames));

    /* An array of string of abbreviated day name in English.  The
     * first is "Sun".
     */
    rb_define_const(cDate, "ABBR_DAYNAMES", mk_ary_of_str(7, abbr_daynames));

    /* The Julian day number of the day of calendar reform for Italy
     * and some catholic countries.
     */
    rb_define_const(cDate, "ITALY", INT2FIX(ITALY));

    /* The Julian day number of the day of calendar reform for England
     * and her colonies.
     */
    rb_define_const(cDate, "ENGLAND", INT2FIX(ENGLAND));

    /* The Julian day number of the day of calendar reform for the
     * proleptic Julian calendar
     */
    rb_define_const(cDate, "JULIAN", DBL2NUM(JULIAN));

    /* The Julian day number of the day of calendar reform for the
     * proleptic Gregorian calendar
     */
    rb_define_const(cDate, "GREGORIAN", DBL2NUM(GREGORIAN));

    rb_define_alloc_func(cDate, d_lite_s_alloc);

#ifndef NDEBUG
#define de_define_private_method rb_define_private_method
    de_define_private_method(CLASS_OF(cDate), "_valid_jd?",
			     date_s__valid_jd_p, -1);
    de_define_private_method(CLASS_OF(cDate), "_valid_ordinal?",
			     date_s__valid_ordinal_p, -1);
    de_define_private_method(CLASS_OF(cDate), "_valid_civil?",
			     date_s__valid_civil_p, -1);
    de_define_private_method(CLASS_OF(cDate), "_valid_date?",
			     date_s__valid_civil_p, -1);
    de_define_private_method(CLASS_OF(cDate), "_valid_commercial?",
			     date_s__valid_commercial_p, -1);
    de_define_private_method(CLASS_OF(cDate), "_valid_weeknum?",
			     date_s__valid_weeknum_p, -1);
    de_define_private_method(CLASS_OF(cDate), "_valid_nth_kday?",
			     date_s__valid_nth_kday_p, -1);
#endif

    rb_define_singleton_method(cDate, "valid_jd?", date_s_valid_jd_p, -1);
    rb_define_singleton_method(cDate, "valid_ordinal?",
			       date_s_valid_ordinal_p, -1);
    rb_define_singleton_method(cDate, "valid_civil?", date_s_valid_civil_p, -1);
    rb_define_singleton_method(cDate, "valid_date?", date_s_valid_civil_p, -1);
    rb_define_singleton_method(cDate, "valid_commercial?",
			       date_s_valid_commercial_p, -1);

#ifndef NDEBUG
    de_define_private_method(CLASS_OF(cDate), "valid_weeknum?",
			     date_s_valid_weeknum_p, -1);
    de_define_private_method(CLASS_OF(cDate), "valid_nth_kday?",
			     date_s_valid_nth_kday_p, -1);
    de_define_private_method(CLASS_OF(cDate), "zone_to_diff",
			     date_s_zone_to_diff, 1);
#endif

    rb_define_singleton_method(cDate, "julian_leap?", date_s_julian_leap_p, 1);
    rb_define_singleton_method(cDate, "gregorian_leap?",
			       date_s_gregorian_leap_p, 1);
    rb_define_singleton_method(cDate, "leap?",
			       date_s_gregorian_leap_p, 1);

#ifndef NDEBUG
#define de_define_singleton_method rb_define_singleton_method
#define de_define_alias rb_define_alias
    de_define_singleton_method(cDate, "new!", date_s_new_bang, -1);
    de_define_alias(rb_singleton_class(cDate), "new_l!", "new");
#endif

    rb_define_singleton_method(cDate, "jd", date_s_jd, -1);
    rb_define_singleton_method(cDate, "ordinal", date_s_ordinal, -1);
    rb_define_singleton_method(cDate, "civil", date_s_civil, -1);
    rb_define_singleton_method(cDate, "new", date_s_civil, -1);
    rb_define_singleton_method(cDate, "commercial", date_s_commercial, -1);

#ifndef NDEBUG
    de_define_singleton_method(cDate, "weeknum", date_s_weeknum, -1);
    de_define_singleton_method(cDate, "nth_kday", date_s_nth_kday, -1);
#endif

    rb_define_singleton_method(cDate, "today", date_s_today, -1);
    rb_define_singleton_method(cDate, "_strptime", date_s__strptime, -1);
    rb_define_singleton_method(cDate, "strptime", date_s_strptime, -1);
    rb_define_singleton_method(cDate, "_parse", date_s__parse, -1);
    rb_define_singleton_method(cDate, "parse", date_s_parse, -1);
    rb_define_singleton_method(cDate, "_iso8601", date_s__iso8601, 1);
    rb_define_singleton_method(cDate, "iso8601", date_s_iso8601, -1);
    rb_define_singleton_method(cDate, "_rfc3339", date_s__rfc3339, 1);
    rb_define_singleton_method(cDate, "rfc3339", date_s_rfc3339, -1);
    rb_define_singleton_method(cDate, "_xmlschema", date_s__xmlschema, 1);
    rb_define_singleton_method(cDate, "xmlschema", date_s_xmlschema, -1);
    rb_define_singleton_method(cDate, "_rfc2822", date_s__rfc2822, 1);
    rb_define_singleton_method(cDate, "_rfc822", date_s__rfc2822, 1);
    rb_define_singleton_method(cDate, "rfc2822", date_s_rfc2822, -1);
    rb_define_singleton_method(cDate, "rfc822", date_s_rfc2822, -1);
    rb_define_singleton_method(cDate, "_httpdate", date_s__httpdate, 1);
    rb_define_singleton_method(cDate, "httpdate", date_s_httpdate, -1);
    rb_define_singleton_method(cDate, "_jisx0301", date_s__jisx0301, 1);
    rb_define_singleton_method(cDate, "jisx0301", date_s_jisx0301, -1);

#ifndef NDEBUG
#define de_define_method rb_define_method
    de_define_method(cDate, "initialize", d_lite_initialize, -1);
#endif
    rb_define_method(cDate, "initialize_copy", d_lite_initialize_copy, 1);

#ifndef NDEBUG
    de_define_method(cDate, "fill", d_lite_fill, 0);
#endif

    rb_define_method(cDate, "ajd", d_lite_ajd, 0);
    rb_define_method(cDate, "amjd", d_lite_amjd, 0);
    rb_define_method(cDate, "jd", d_lite_jd, 0);
    rb_define_method(cDate, "mjd", d_lite_mjd, 0);
    rb_define_method(cDate, "ld", d_lite_ld, 0);

    rb_define_method(cDate, "year", d_lite_year, 0);
    rb_define_method(cDate, "yday", d_lite_yday, 0);
    rb_define_method(cDate, "mon", d_lite_mon, 0);
    rb_define_method(cDate, "month", d_lite_mon, 0);
    rb_define_method(cDate, "mday", d_lite_mday, 0);
    rb_define_method(cDate, "day", d_lite_mday, 0);
    rb_define_method(cDate, "day_fraction", d_lite_day_fraction, 0);

    rb_define_method(cDate, "cwyear", d_lite_cwyear, 0);
    rb_define_method(cDate, "cweek", d_lite_cweek, 0);
    rb_define_method(cDate, "cwday", d_lite_cwday, 0);

#ifndef NDEBUG
    de_define_private_method(cDate, "wnum0", d_lite_wnum0, 0);
    de_define_private_method(cDate, "wnum1", d_lite_wnum1, 0);
#endif

    rb_define_method(cDate, "wday", d_lite_wday, 0);

    rb_define_method(cDate, "sunday?", d_lite_sunday_p, 0);
    rb_define_method(cDate, "monday?", d_lite_monday_p, 0);
    rb_define_method(cDate, "tuesday?", d_lite_tuesday_p, 0);
    rb_define_method(cDate, "wednesday?", d_lite_wednesday_p, 0);
    rb_define_method(cDate, "thursday?", d_lite_thursday_p, 0);
    rb_define_method(cDate, "friday?", d_lite_friday_p, 0);
    rb_define_method(cDate, "saturday?", d_lite_saturday_p, 0);

#ifndef NDEBUG
    de_define_method(cDate, "nth_kday?", d_lite_nth_kday_p, 2);
#endif

    rb_define_private_method(cDate, "hour", d_lite_hour, 0);
    rb_define_private_method(cDate, "min", d_lite_min, 0);
    rb_define_private_method(cDate, "minute", d_lite_min, 0);
    rb_define_private_method(cDate, "sec", d_lite_sec, 0);
    rb_define_private_method(cDate, "second", d_lite_sec, 0);
    rb_define_private_method(cDate, "sec_fraction", d_lite_sec_fraction, 0);
    rb_define_private_method(cDate, "second_fraction", d_lite_sec_fraction, 0);
    rb_define_private_method(cDate, "offset", d_lite_offset, 0);
    rb_define_private_method(cDate, "zone", d_lite_zone, 0);

    rb_define_method(cDate, "julian?", d_lite_julian_p, 0);
    rb_define_method(cDate, "gregorian?", d_lite_gregorian_p, 0);
    rb_define_method(cDate, "leap?", d_lite_leap_p, 0);

    rb_define_method(cDate, "start", d_lite_start, 0);
    rb_define_method(cDate, "new_start", d_lite_new_start, -1);
    rb_define_method(cDate, "italy", d_lite_italy, 0);
    rb_define_method(cDate, "england", d_lite_england, 0);
    rb_define_method(cDate, "julian", d_lite_julian, 0);
    rb_define_method(cDate, "gregorian", d_lite_gregorian, 0);

    rb_define_private_method(cDate, "new_offset", d_lite_new_offset, -1);

    rb_define_method(cDate, "+", d_lite_plus, 1);
    rb_define_method(cDate, "-", d_lite_minus, 1);

    rb_define_method(cDate, "next_day", d_lite_next_day, -1);
    rb_define_method(cDate, "prev_day", d_lite_prev_day, -1);
    rb_define_method(cDate, "next", d_lite_next, 0);
    rb_define_method(cDate, "succ", d_lite_next, 0);

    rb_define_method(cDate, ">>", d_lite_rshift, 1);
    rb_define_method(cDate, "<<", d_lite_lshift, 1);

    rb_define_method(cDate, "next_month", d_lite_next_month, -1);
    rb_define_method(cDate, "prev_month", d_lite_prev_month, -1);
    rb_define_method(cDate, "next_year", d_lite_next_year, -1);
    rb_define_method(cDate, "prev_year", d_lite_prev_year, -1);

    rb_define_method(cDate, "step", d_lite_step, -1);
    rb_define_method(cDate, "upto", d_lite_upto, 1);
    rb_define_method(cDate, "downto", d_lite_downto, 1);

    rb_define_method(cDate, "<=>", d_lite_cmp, 1);
    rb_define_method(cDate, "===", d_lite_equal, 1);
    rb_define_method(cDate, "eql?", d_lite_eql_p, 1);
    rb_define_method(cDate, "hash", d_lite_hash, 0);

    rb_define_method(cDate, "to_s", d_lite_to_s, 0);
#ifndef NDEBUG
    de_define_method(cDate, "inspect_raw", d_lite_inspect_raw, 0);
#endif
    rb_define_method(cDate, "inspect", d_lite_inspect, 0);

    rb_define_method(cDate, "strftime", d_lite_strftime, -1);

    rb_define_method(cDate, "asctime", d_lite_asctime, 0);
    rb_define_method(cDate, "ctime", d_lite_asctime, 0);
    rb_define_method(cDate, "iso8601", d_lite_iso8601, 0);
    rb_define_method(cDate, "xmlschema", d_lite_iso8601, 0);
    rb_define_method(cDate, "rfc3339", d_lite_rfc3339, 0);
    rb_define_method(cDate, "rfc2822", d_lite_rfc2822, 0);
    rb_define_method(cDate, "rfc822", d_lite_rfc2822, 0);
    rb_define_method(cDate, "httpdate", d_lite_httpdate, 0);
    rb_define_method(cDate, "jisx0301", d_lite_jisx0301, 0);

#ifndef NDEBUG
    de_define_method(cDate, "marshal_dump_old", d_lite_marshal_dump_old, 0);
#endif
    rb_define_method(cDate, "marshal_dump", d_lite_marshal_dump, 0);
    rb_define_method(cDate, "marshal_load", d_lite_marshal_load, 1);

    /* datetime */

    cDateTime = rb_define_class("DateTime", cDate);

    rb_define_singleton_method(cDateTime, "jd", datetime_s_jd, -1);
    rb_define_singleton_method(cDateTime, "ordinal", datetime_s_ordinal, -1);
    rb_define_singleton_method(cDateTime, "civil", datetime_s_civil, -1);
    rb_define_singleton_method(cDateTime, "new", datetime_s_civil, -1);
    rb_define_singleton_method(cDateTime, "commercial",
			       datetime_s_commercial, -1);

#ifndef NDEBUG
    de_define_singleton_method(cDateTime, "weeknum",
			       datetime_s_weeknum, -1);
    de_define_singleton_method(cDateTime, "nth_kday",
			       datetime_s_nth_kday, -1);
#endif

    rb_undef_method(CLASS_OF(cDateTime), "today");

    rb_define_singleton_method(cDateTime, "now", datetime_s_now, -1);
    rb_define_singleton_method(cDateTime, "_strptime",
			       datetime_s__strptime, -1);
    rb_define_singleton_method(cDateTime, "strptime",
			       datetime_s_strptime, -1);
    rb_define_singleton_method(cDateTime, "parse",
			       datetime_s_parse, -1);
    rb_define_singleton_method(cDateTime, "iso8601",
			       datetime_s_iso8601, -1);
    rb_define_singleton_method(cDateTime, "rfc3339",
			       datetime_s_rfc3339, -1);
    rb_define_singleton_method(cDateTime, "xmlschema",
			       datetime_s_xmlschema, -1);
    rb_define_singleton_method(cDateTime, "rfc2822",
			       datetime_s_rfc2822, -1);
    rb_define_singleton_method(cDateTime, "rfc822",
			       datetime_s_rfc2822, -1);
    rb_define_singleton_method(cDateTime, "httpdate",
			       datetime_s_httpdate, -1);
    rb_define_singleton_method(cDateTime, "jisx0301",
			       datetime_s_jisx0301, -1);

#define f_public(m,s) rb_funcall(m, rb_intern("public"), 1,\
				 ID2SYM(rb_intern(s)))

    f_public(cDateTime, "hour");
    f_public(cDateTime, "min");
    f_public(cDateTime, "minute");
    f_public(cDateTime, "sec");
    f_public(cDateTime, "second");
    f_public(cDateTime, "sec_fraction");
    f_public(cDateTime, "second_fraction");
    f_public(cDateTime, "offset");
    f_public(cDateTime, "zone");
    f_public(cDateTime, "new_offset");

    rb_define_method(cDateTime, "to_s", dt_lite_to_s, 0);

    rb_define_method(cDateTime, "strftime", dt_lite_strftime, -1);

    rb_define_method(cDateTime, "iso8601", dt_lite_iso8601, -1);
    rb_define_method(cDateTime, "xmlschema", dt_lite_iso8601, -1);
    rb_define_method(cDateTime, "rfc3339", dt_lite_rfc3339, -1);
    rb_define_method(cDateTime, "jisx0301", dt_lite_jisx0301, -1);

    /* conversions */

    rb_define_method(rb_cTime, "to_time", time_to_time, 0);
    rb_define_method(rb_cTime, "to_date", time_to_date, 0);
    rb_define_method(rb_cTime, "to_datetime", time_to_datetime, 0);

    rb_define_method(cDate, "to_time", date_to_time, 0);
    rb_define_method(cDate, "to_date", date_to_date, 0);
    rb_define_method(cDate, "to_datetime", date_to_datetime, 0);

    rb_define_method(cDateTime, "to_time", datetime_to_time, 0);
    rb_define_method(cDateTime, "to_date", datetime_to_date, 0);
    rb_define_method(cDateTime, "to_datetime", datetime_to_datetime, 0);

#ifndef NDEBUG
    /* tests */

    de_define_singleton_method(cDate, "test_civil", date_s_test_civil, 0);
    de_define_singleton_method(cDate, "test_ordinal", date_s_test_ordinal, 0);
    de_define_singleton_method(cDate, "test_commercial",
			       date_s_test_commercial, 0);
    de_define_singleton_method(cDate, "test_weeknum", date_s_test_weeknum, 0);
    de_define_singleton_method(cDate, "test_nth_kday", date_s_test_nth_kday, 0);
    de_define_singleton_method(cDate, "test_unit_conv",
			       date_s_test_unit_conv, 0);
    de_define_singleton_method(cDate, "test_all", date_s_test_all, 0);
#endif
}

/*
Local variables:
c-file-style: "ruby"
End:
*/
