/*
  complex.c: Coded by Tadayoshi Funaba 2008-2011

  This implementation is based on Keiju Ishitsuka's Complex library
  which is written in ruby.
*/

#include "ruby.h"
#include "internal.h"
#include <math.h>

#define NDEBUG
#include <assert.h>

#define ZERO INT2FIX(0)
#define ONE INT2FIX(1)
#define TWO INT2FIX(2)

VALUE rb_cComplex;

static ID id_abs, id_abs2, id_arg, id_cmp, id_conj, id_convert,
    id_denominator, id_divmod, id_eqeq_p, id_expt, id_fdiv,  id_floor,
    id_idiv, id_imag, id_inspect, id_negate, id_numerator, id_quo,
    id_real, id_real_p, id_to_f, id_to_i, id_to_r, id_to_s;

#define f_boolcast(x) ((x) ? Qtrue : Qfalse)

#define binop(n,op) \
inline static VALUE \
f_##n(VALUE x, VALUE y)\
{\
    return rb_funcall(x, (op), 1, y);\
}

#define fun1(n) \
inline static VALUE \
f_##n(VALUE x)\
{\
    return rb_funcall(x, id_##n, 0);\
}

#define fun2(n) \
inline static VALUE \
f_##n(VALUE x, VALUE y)\
{\
    return rb_funcall(x, id_##n, 1, y);\
}

#define math1(n) \
inline static VALUE \
m_##n(VALUE x)\
{\
    return rb_funcall(rb_mMath, id_##n, 1, x);\
}

#define math2(n) \
inline static VALUE \
m_##n(VALUE x, VALUE y)\
{\
    return rb_funcall(rb_mMath, id_##n, 2, x, y);\
}

#define PRESERVE_SIGNEDZERO

inline static VALUE
f_add(VALUE x, VALUE y)
{
#ifndef PRESERVE_SIGNEDZERO
    if (FIXNUM_P(y) && FIX2LONG(y) == 0)
	return x;
    else if (FIXNUM_P(x) && FIX2LONG(x) == 0)
	return y;
#endif
    return rb_funcall(x, '+', 1, y);
}

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
f_div(VALUE x, VALUE y)
{
    if (FIXNUM_P(y) && FIX2LONG(y) == 1)
	return x;
    return rb_funcall(x, '/', 1, y);
}

inline static VALUE
f_gt_p(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y))
	return f_boolcast(FIX2LONG(x) > FIX2LONG(y));
    return rb_funcall(x, '>', 1, y);
}

inline static VALUE
f_lt_p(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y))
	return f_boolcast(FIX2LONG(x) < FIX2LONG(y));
    return rb_funcall(x, '<', 1, y);
}

binop(mod, '%')

inline static VALUE
f_mul(VALUE x, VALUE y)
{
#ifndef PRESERVE_SIGNEDZERO
    if (FIXNUM_P(y)) {
	long iy = FIX2LONG(y);
	if (iy == 0) {
	    if (FIXNUM_P(x) || RB_TYPE_P(x, T_BIGNUM))
		return ZERO;
	}
	else if (iy == 1)
	    return x;
    }
    else if (FIXNUM_P(x)) {
	long ix = FIX2LONG(x);
	if (ix == 0) {
	    if (FIXNUM_P(y) || RB_TYPE_P(y, T_BIGNUM))
		return ZERO;
	}
	else if (ix == 1)
	    return y;
    }
#endif
    return rb_funcall(x, '*', 1, y);
}

inline static VALUE
f_sub(VALUE x, VALUE y)
{
#ifndef PRESERVE_SIGNEDZERO
    if (FIXNUM_P(y) && FIX2LONG(y) == 0)
	return x;
#endif
    return rb_funcall(x, '-', 1, y);
}

fun1(abs)
fun1(abs2)
fun1(arg)
fun1(conj)
fun1(denominator)
fun1(floor)
fun1(imag)
fun1(inspect)
fun1(negate)
fun1(numerator)
fun1(real)
fun1(real_p)

inline static VALUE
f_to_i(VALUE x)
{
    if (RB_TYPE_P(x, T_STRING))
	return rb_str_to_inum(x, 10, 0);
    return rb_funcall(x, id_to_i, 0);
}
inline static VALUE
f_to_f(VALUE x)
{
    if (RB_TYPE_P(x, T_STRING))
	return DBL2NUM(rb_str_to_dbl(x, 0));
    return rb_funcall(x, id_to_f, 0);
}

fun1(to_r)
fun1(to_s)

fun2(divmod)

inline static VALUE
f_eqeq_p(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y))
	return f_boolcast(FIX2LONG(x) == FIX2LONG(y));
    return rb_funcall(x, id_eqeq_p, 1, y);
}

fun2(expt)
fun2(fdiv)
fun2(idiv)
fun2(quo)

inline static VALUE
f_negative_p(VALUE x)
{
    if (FIXNUM_P(x))
	return f_boolcast(FIX2LONG(x) < 0);
    return rb_funcall(x, '<', 1, ZERO);
}

#define f_positive_p(x) (!f_negative_p(x))

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
    return rb_funcall(x, id_eqeq_p, 1, ZERO);
}

#define f_nonzero_p(x) (!f_zero_p(x))

inline static VALUE
f_one_p(VALUE x)
{
    switch (TYPE(x)) {
      case T_FIXNUM:
	return f_boolcast(FIX2LONG(x) == 1);
      case T_BIGNUM:
	return Qfalse;
      case T_RATIONAL:
      {
	  VALUE num = RRATIONAL(x)->num;
	  VALUE den = RRATIONAL(x)->den;

	  return f_boolcast(FIXNUM_P(num) && FIX2LONG(num) == 1 &&
			    FIXNUM_P(den) && FIX2LONG(den) == 1);
      }
    }
    return rb_funcall(x, id_eqeq_p, 1, ONE);
}

inline static VALUE
f_kind_of_p(VALUE x, VALUE c)
{
    return rb_obj_is_kind_of(x, c);
}

inline static VALUE
k_numeric_p(VALUE x)
{
    return f_kind_of_p(x, rb_cNumeric);
}

inline static VALUE
k_integer_p(VALUE x)
{
    return f_kind_of_p(x, rb_cInteger);
}

inline static VALUE
k_fixnum_p(VALUE x)
{
    return f_kind_of_p(x, rb_cFixnum);
}

inline static VALUE
k_bignum_p(VALUE x)
{
    return f_kind_of_p(x, rb_cBignum);
}

inline static VALUE
k_float_p(VALUE x)
{
    return f_kind_of_p(x, rb_cFloat);
}

inline static VALUE
k_rational_p(VALUE x)
{
    return f_kind_of_p(x, rb_cRational);
}

inline static VALUE
k_complex_p(VALUE x)
{
    return f_kind_of_p(x, rb_cComplex);
}

#define k_exact_p(x) (!k_float_p(x))
#define k_inexact_p(x) k_float_p(x)

#define k_exact_zero_p(x) (k_exact_p(x) && f_zero_p(x))
#define k_exact_one_p(x) (k_exact_p(x) && f_one_p(x))

#define get_dat1(x) \
    struct RComplex *dat;\
    dat = ((struct RComplex *)(x))

#define get_dat2(x,y) \
    struct RComplex *adat, *bdat;\
    adat = ((struct RComplex *)(x));\
    bdat = ((struct RComplex *)(y))

inline static VALUE
nucomp_s_new_internal(VALUE klass, VALUE real, VALUE imag)
{
    NEWOBJ(obj, struct RComplex);
    OBJSETUP(obj, klass, T_COMPLEX);

    obj->real = real;
    obj->imag = imag;

    return (VALUE)obj;
}

static VALUE
nucomp_s_alloc(VALUE klass)
{
    return nucomp_s_new_internal(klass, ZERO, ZERO);
}

#if 0
static VALUE
nucomp_s_new_bang(int argc, VALUE *argv, VALUE klass)
{
    VALUE real, imag;

    switch (rb_scan_args(argc, argv, "11", &real, &imag)) {
      case 1:
	if (!k_numeric_p(real))
	    real = f_to_i(real);
	imag = ZERO;
	break;
      default:
	if (!k_numeric_p(real))
	    real = f_to_i(real);
	if (!k_numeric_p(imag))
	    imag = f_to_i(imag);
	break;
    }

    return nucomp_s_new_internal(klass, real, imag);
}
#endif

inline static VALUE
f_complex_new_bang1(VALUE klass, VALUE x)
{
    assert(!k_complex_p(x));
    return nucomp_s_new_internal(klass, x, ZERO);
}

inline static VALUE
f_complex_new_bang2(VALUE klass, VALUE x, VALUE y)
{
    assert(!k_complex_p(x));
    assert(!k_complex_p(y));
    return nucomp_s_new_internal(klass, x, y);
}

#ifdef CANONICALIZATION_FOR_MATHN
#define CANON
#endif

#ifdef CANON
static int canonicalization = 0;

RUBY_FUNC_EXPORTED void
nucomp_canonicalization(int f)
{
    canonicalization = f;
}
#endif

inline static void
nucomp_real_check(VALUE num)
{
    switch (TYPE(num)) {
      case T_FIXNUM:
      case T_BIGNUM:
      case T_FLOAT:
      case T_RATIONAL:
	break;
      default:
	if (!k_numeric_p(num) || !f_real_p(num))
	    rb_raise(rb_eTypeError, "not a real");
    }
}

inline static VALUE
nucomp_s_canonicalize_internal(VALUE klass, VALUE real, VALUE imag)
{
#ifdef CANON
#define CL_CANON
#ifdef CL_CANON
    if (k_exact_zero_p(imag) && canonicalization)
	return real;
#else
    if (f_zero_p(imag) && canonicalization)
	return real;
#endif
#endif
    if (f_real_p(real) && f_real_p(imag))
	return nucomp_s_new_internal(klass, real, imag);
    else if (f_real_p(real)) {
	get_dat1(imag);

	return nucomp_s_new_internal(klass,
				     f_sub(real, dat->imag),
				     f_add(ZERO, dat->real));
    }
    else if (f_real_p(imag)) {
	get_dat1(real);

	return nucomp_s_new_internal(klass,
				     dat->real,
				     f_add(dat->imag, imag));
    }
    else {
	get_dat2(real, imag);

	return nucomp_s_new_internal(klass,
				     f_sub(adat->real, bdat->imag),
				     f_add(adat->imag, bdat->real));
    }
}

/*
 * call-seq:
 *    Complex.rect(real[, imag])         ->  complex
 *    Complex.rectangular(real[, imag])  ->  complex
 *
 * Returns a complex object which denotes the given rectangular form.
 */
static VALUE
nucomp_s_new(int argc, VALUE *argv, VALUE klass)
{
    VALUE real, imag;

    switch (rb_scan_args(argc, argv, "11", &real, &imag)) {
      case 1:
	nucomp_real_check(real);
	imag = ZERO;
	break;
      default:
	nucomp_real_check(real);
	nucomp_real_check(imag);
	break;
    }

    return nucomp_s_canonicalize_internal(klass, real, imag);
}

inline static VALUE
f_complex_new1(VALUE klass, VALUE x)
{
    assert(!k_complex_p(x));
    return nucomp_s_canonicalize_internal(klass, x, ZERO);
}

inline static VALUE
f_complex_new2(VALUE klass, VALUE x, VALUE y)
{
    assert(!k_complex_p(x));
    return nucomp_s_canonicalize_internal(klass, x, y);
}

/*
 * call-seq:
 *    Complex(x[, y])  ->  numeric
 *
 * Returns x+i*y;
 */
static VALUE
nucomp_f_complex(int argc, VALUE *argv, VALUE klass)
{
    return rb_funcall2(rb_cComplex, id_convert, argc, argv);
}

#define imp1(n) \
inline static VALUE \
m_##n##_bang(VALUE x)\
{\
    return rb_math_##n(x);\
}

#define imp2(n) \
inline static VALUE \
m_##n##_bang(VALUE x, VALUE y)\
{\
    return rb_math_##n(x, y);\
}

imp2(atan2)
imp1(cos)
imp1(cosh)
imp1(exp)
imp2(hypot)

#define m_hypot(x,y) m_hypot_bang((x),(y))

static VALUE
m_log_bang(VALUE x)
{
    return rb_math_log(1, &x);
}

imp1(sin)
imp1(sinh)
imp1(sqrt)

static VALUE
m_cos(VALUE x)
{
    if (f_real_p(x))
	return m_cos_bang(x);
    {
	get_dat1(x);
	return f_complex_new2(rb_cComplex,
			      f_mul(m_cos_bang(dat->real),
				    m_cosh_bang(dat->imag)),
			      f_mul(f_negate(m_sin_bang(dat->real)),
				    m_sinh_bang(dat->imag)));
    }
}

static VALUE
m_sin(VALUE x)
{
    if (f_real_p(x))
	return m_sin_bang(x);
    {
	get_dat1(x);
	return f_complex_new2(rb_cComplex,
			      f_mul(m_sin_bang(dat->real),
				    m_cosh_bang(dat->imag)),
			      f_mul(m_cos_bang(dat->real),
				    m_sinh_bang(dat->imag)));
    }
}

#if 0
static VALUE
m_sqrt(VALUE x)
{
    if (f_real_p(x)) {
	if (f_positive_p(x))
	    return m_sqrt_bang(x);
	return f_complex_new2(rb_cComplex, ZERO, m_sqrt_bang(f_negate(x)));
    }
    else {
	get_dat1(x);

	if (f_negative_p(dat->imag))
	    return f_conj(m_sqrt(f_conj(x)));
	else {
	    VALUE a = f_abs(x);
	    return f_complex_new2(rb_cComplex,
				  m_sqrt_bang(f_div(f_add(a, dat->real), TWO)),
				  m_sqrt_bang(f_div(f_sub(a, dat->real), TWO)));
	}
    }
}
#endif

inline static VALUE
f_complex_polar(VALUE klass, VALUE x, VALUE y)
{
    assert(!k_complex_p(x));
    assert(!k_complex_p(y));
    return nucomp_s_canonicalize_internal(klass,
					  f_mul(x, m_cos(y)),
					  f_mul(x, m_sin(y)));
}

/*
 * call-seq:
 *    Complex.polar(abs[, arg])  ->  complex
 *
 * Returns a complex object which denotes the given polar form.
 *
 *   Complex.polar(3, 0)           #=> (3.0+0.0i)
 *   Complex.polar(3, Math::PI/2)  #=> (1.836909530733566e-16+3.0i)
 *   Complex.polar(3, Math::PI)    #=> (-3.0+3.673819061467132e-16i)
 *   Complex.polar(3, -Math::PI/2) #=> (1.836909530733566e-16-3.0i)
 */
static VALUE
nucomp_s_polar(int argc, VALUE *argv, VALUE klass)
{
    VALUE abs, arg;

    switch (rb_scan_args(argc, argv, "11", &abs, &arg)) {
      case 1:
	nucomp_real_check(abs);
	arg = ZERO;
	break;
      default:
	nucomp_real_check(abs);
	nucomp_real_check(arg);
	break;
    }
    return f_complex_polar(klass, abs, arg);
}

/*
 * call-seq:
 *    cmp.real  ->  real
 *
 * Returns the real part.
 */
static VALUE
nucomp_real(VALUE self)
{
    get_dat1(self);
    return dat->real;
}

/*
 * call-seq:
 *    cmp.imag       ->  real
 *    cmp.imaginary  ->  real
 *
 * Returns the imaginary part.
 */
static VALUE
nucomp_imag(VALUE self)
{
    get_dat1(self);
    return dat->imag;
}

/*
 * call-seq:
 *    -cmp  ->  complex
 *
 * Returns negation of the value.
 */
static VALUE
nucomp_negate(VALUE self)
{
  get_dat1(self);
  return f_complex_new2(CLASS_OF(self),
			f_negate(dat->real), f_negate(dat->imag));
}

inline static VALUE
f_addsub(VALUE self, VALUE other,
	 VALUE (*func)(VALUE, VALUE), ID id)
{
    if (k_complex_p(other)) {
	VALUE real, imag;

	get_dat2(self, other);

	real = (*func)(adat->real, bdat->real);
	imag = (*func)(adat->imag, bdat->imag);

	return f_complex_new2(CLASS_OF(self), real, imag);
    }
    if (k_numeric_p(other) && f_real_p(other)) {
	get_dat1(self);

	return f_complex_new2(CLASS_OF(self),
			      (*func)(dat->real, other), dat->imag);
    }
    return rb_num_coerce_bin(self, other, id);
}

/*
 * call-seq:
 *    cmp + numeric  ->  complex
 *
 * Performs addition.
 */
static VALUE
nucomp_add(VALUE self, VALUE other)
{
    return f_addsub(self, other, f_add, '+');
}

/*
 * call-seq:
 *    cmp - numeric  ->  complex
 *
 * Performs subtraction.
 */
static VALUE
nucomp_sub(VALUE self, VALUE other)
{
    return f_addsub(self, other, f_sub, '-');
}

/*
 * call-seq:
 *    cmp * numeric  ->  complex
 *
 * Performs multiplication.
 */
static VALUE
nucomp_mul(VALUE self, VALUE other)
{
    if (k_complex_p(other)) {
	VALUE real, imag;

	get_dat2(self, other);

	real = f_sub(f_mul(adat->real, bdat->real),
		     f_mul(adat->imag, bdat->imag));
	imag = f_add(f_mul(adat->real, bdat->imag),
		     f_mul(adat->imag, bdat->real));

	return f_complex_new2(CLASS_OF(self), real, imag);
    }
    if (k_numeric_p(other) && f_real_p(other)) {
	get_dat1(self);

	return f_complex_new2(CLASS_OF(self),
			      f_mul(dat->real, other),
			      f_mul(dat->imag, other));
    }
    return rb_num_coerce_bin(self, other, '*');
}

inline static VALUE
f_divide(VALUE self, VALUE other,
	 VALUE (*func)(VALUE, VALUE), ID id)
{
    if (k_complex_p(other)) {
	int flo;
	get_dat2(self, other);

	flo = (k_float_p(adat->real) || k_float_p(adat->imag) ||
	       k_float_p(bdat->real) || k_float_p(bdat->imag));

	if (f_gt_p(f_abs(bdat->real), f_abs(bdat->imag))) {
	    VALUE r, n;

	    r = (*func)(bdat->imag, bdat->real);
	    n = f_mul(bdat->real, f_add(ONE, f_mul(r, r)));
	    if (flo)
		return f_complex_new2(CLASS_OF(self),
				      (*func)(self, n),
				      (*func)(f_negate(f_mul(self, r)), n));
	    return f_complex_new2(CLASS_OF(self),
				  (*func)(f_add(adat->real,
						f_mul(adat->imag, r)), n),
				  (*func)(f_sub(adat->imag,
						f_mul(adat->real, r)), n));
	}
	else {
	    VALUE r, n;

	    r = (*func)(bdat->real, bdat->imag);
	    n = f_mul(bdat->imag, f_add(ONE, f_mul(r, r)));
	    if (flo)
		return f_complex_new2(CLASS_OF(self),
				      (*func)(f_mul(self, r), n),
				      (*func)(f_negate(self), n));
	    return f_complex_new2(CLASS_OF(self),
				  (*func)(f_add(f_mul(adat->real, r),
						adat->imag), n),
				  (*func)(f_sub(f_mul(adat->imag, r),
						adat->real), n));
	}
    }
    if (k_numeric_p(other) && f_real_p(other)) {
	get_dat1(self);

	return f_complex_new2(CLASS_OF(self),
			      (*func)(dat->real, other),
			      (*func)(dat->imag, other));
    }
    return rb_num_coerce_bin(self, other, id);
}

#define rb_raise_zerodiv() rb_raise(rb_eZeroDivError, "divided by 0")

/*
 * call-seq:
 *    cmp / numeric     ->  complex
 *    cmp.quo(numeric)  ->  complex
 *
 * Performs division.
 *
 * For example:
 *
 *     Complex(10.0) / 3  #=> (3.3333333333333335+(0/1)*i)
 *     Complex(10)   / 3  #=> ((10/3)+(0/1)*i)  # not (3+0i)
 */
static VALUE
nucomp_div(VALUE self, VALUE other)
{
    return f_divide(self, other, f_quo, id_quo);
}

#define nucomp_quo nucomp_div

/*
 * call-seq:
 *    cmp.fdiv(numeric)  ->  complex
 *
 * Performs division as each part is a float, never returns a float.
 *
 * For example:
 *
 *     Complex(11,22).fdiv(3)  #=> (3.6666666666666665+7.333333333333333i)
 */
static VALUE
nucomp_fdiv(VALUE self, VALUE other)
{
    return f_divide(self, other, f_fdiv, id_fdiv);
}

inline static VALUE
f_reciprocal(VALUE x)
{
    return f_quo(ONE, x);
}

/*
 * call-seq:
 *    cmp ** numeric  ->  complex
 *
 * Performs exponentiation.
 *
 * For example:
 *
 *     Complex('i') ** 2             #=> (-1+0i)
 *     Complex(-8) ** Rational(1,3)  #=> (1.0000000000000002+1.7320508075688772i)
 */
static VALUE
nucomp_expt(VALUE self, VALUE other)
{
    if (k_numeric_p(other) && k_exact_zero_p(other))
	return f_complex_new_bang1(CLASS_OF(self), ONE);

    if (k_rational_p(other) && f_one_p(f_denominator(other)))
	other = f_numerator(other); /* c14n */

    if (k_complex_p(other)) {
	get_dat1(other);

	if (k_exact_zero_p(dat->imag))
	    other = dat->real; /* c14n */
    }

    if (k_complex_p(other)) {
	VALUE r, theta, nr, ntheta;

	get_dat1(other);

	r = f_abs(self);
	theta = f_arg(self);

	nr = m_exp_bang(f_sub(f_mul(dat->real, m_log_bang(r)),
			      f_mul(dat->imag, theta)));
	ntheta = f_add(f_mul(theta, dat->real),
		       f_mul(dat->imag, m_log_bang(r)));
	return f_complex_polar(CLASS_OF(self), nr, ntheta);
    }
    if (k_fixnum_p(other)) {
	if (f_gt_p(other, ZERO)) {
	    VALUE x, z;
	    long n;

	    x = self;
	    z = x;
	    n = FIX2LONG(other) - 1;

	    while (n) {
		long q, r;

		while (1) {
		    get_dat1(x);

		    q = n / 2;
		    r = n % 2;

		    if (r)
			break;

		    x = f_complex_new2(CLASS_OF(self),
				       f_sub(f_mul(dat->real, dat->real),
					     f_mul(dat->imag, dat->imag)),
				       f_mul(f_mul(TWO, dat->real), dat->imag));
		    n = q;
		}
		z = f_mul(z, x);
		n--;
	    }
	    return z;
	}
	return f_expt(f_reciprocal(self), f_negate(other));
    }
    if (k_numeric_p(other) && f_real_p(other)) {
	VALUE r, theta;

	if (k_bignum_p(other))
	    rb_warn("in a**b, b may be too big");

	r = f_abs(self);
	theta = f_arg(self);

	return f_complex_polar(CLASS_OF(self), f_expt(r, other),
			       f_mul(theta, other));
    }
    return rb_num_coerce_bin(self, other, id_expt);
}

/*
 * call-seq:
 *    cmp == object  ->  true or false
 *
 * Returns true if cmp equals object numerically.
 */
static VALUE
nucomp_eqeq_p(VALUE self, VALUE other)
{
    if (k_complex_p(other)) {
	get_dat2(self, other);

	return f_boolcast(f_eqeq_p(adat->real, bdat->real) &&
			  f_eqeq_p(adat->imag, bdat->imag));
    }
    if (k_numeric_p(other) && f_real_p(other)) {
	get_dat1(self);

	return f_boolcast(f_eqeq_p(dat->real, other) && f_zero_p(dat->imag));
    }
    return f_eqeq_p(other, self);
}

/* :nodoc: */
static VALUE
nucomp_coerce(VALUE self, VALUE other)
{
    if (k_numeric_p(other) && f_real_p(other))
	return rb_assoc_new(f_complex_new_bang1(CLASS_OF(self), other), self);
    if (RB_TYPE_P(other, T_COMPLEX))
	return rb_assoc_new(other, self);

    rb_raise(rb_eTypeError, "%s can't be coerced into %s",
	     rb_obj_classname(other), rb_obj_classname(self));
    return Qnil;
}

/*
 * call-seq:
 *    cmp.abs        ->  real
 *    cmp.magnitude  ->  real
 *
 * Returns the absolute part of its polar form.
 */
static VALUE
nucomp_abs(VALUE self)
{
    get_dat1(self);

    if (f_zero_p(dat->real)) {
	VALUE a = f_abs(dat->imag);
	if (k_float_p(dat->real) && !k_float_p(dat->imag))
	    a = f_to_f(a);
	return a;
    }
    if (f_zero_p(dat->imag)) {
	VALUE a = f_abs(dat->real);
	if (!k_float_p(dat->real) && k_float_p(dat->imag))
	    a = f_to_f(a);
	return a;
    }
    return m_hypot(dat->real, dat->imag);
}

/*
 * call-seq:
 *    cmp.abs2  ->  real
 *
 * Returns square of the absolute value.
 */
static VALUE
nucomp_abs2(VALUE self)
{
    get_dat1(self);
    return f_add(f_mul(dat->real, dat->real),
		 f_mul(dat->imag, dat->imag));
}

/*
 * call-seq:
 *    cmp.arg    ->  float
 *    cmp.angle  ->  float
 *    cmp.phase  ->  float
 *
 * Returns the angle part of its polar form.
 *
 *   Complex.polar(3, Math::PI/2).arg #=> 1.5707963267948966
 *
 */
static VALUE
nucomp_arg(VALUE self)
{
    get_dat1(self);
    return m_atan2_bang(dat->imag, dat->real);
}

/*
 * call-seq:
 *    cmp.rect         ->  array
 *    cmp.rectangular  ->  array
 *
 * Returns an array; [cmp.real, cmp.imag].
 */
static VALUE
nucomp_rect(VALUE self)
{
    get_dat1(self);
    return rb_assoc_new(dat->real, dat->imag);
}

/*
 * call-seq:
 *    cmp.polar  ->  array
 *
 * Returns an array; [cmp.abs, cmp.arg].
 */
static VALUE
nucomp_polar(VALUE self)
{
    return rb_assoc_new(f_abs(self), f_arg(self));
}

/*
 * call-seq:
 *    cmp.conj       ->  complex
 *    cmp.conjugate  ->  complex
 *
 * Returns the complex conjugate.
 */
static VALUE
nucomp_conj(VALUE self)
{
    get_dat1(self);
    return f_complex_new2(CLASS_OF(self), dat->real, f_negate(dat->imag));
}

#if 0
/* :nodoc: */
static VALUE
nucomp_true(VALUE self)
{
    return Qtrue;
}
#endif

/*
 * call-seq:
 *    cmp.real?  ->  false
 *
 * Returns false.
 */
static VALUE
nucomp_false(VALUE self)
{
    return Qfalse;
}

#if 0
/* :nodoc: */
static VALUE
nucomp_exact_p(VALUE self)
{
    get_dat1(self);
    return f_boolcast(k_exact_p(dat->real) && k_exact_p(dat->imag));
}

/* :nodoc: */
static VALUE
nucomp_inexact_p(VALUE self)
{
    return f_boolcast(!nucomp_exact_p(self));
}
#endif

/*
 * call-seq:
 *    cmp.denominator  ->  integer
 *
 * Returns the denominator (lcm of both denominator - real and imag).
 *
 * See numerator.
 */
static VALUE
nucomp_denominator(VALUE self)
{
    get_dat1(self);
    return rb_lcm(f_denominator(dat->real), f_denominator(dat->imag));
}

/*
 * call-seq:
 *    cmp.numerator  ->  numeric
 *
 * Returns the numerator.
 *
 * For example:
 *
 *        1   2       3+4i  <-  numerator
 *        - + -i  ->  ----
 *        2   3        6    <-  denominator
 *
 *    c = Complex('1/2+2/3i')  #=> ((1/2)+(2/3)*i)
 *    n = c.numerator          #=> (3+4i)
 *    d = c.denominator        #=> 6
 *    n / d                    #=> ((1/2)+(2/3)*i)
 *    Complex(Rational(n.real, d), Rational(n.imag, d))
 *                             #=> ((1/2)+(2/3)*i)
 * See denominator.
 */
static VALUE
nucomp_numerator(VALUE self)
{
    VALUE cd;

    get_dat1(self);

    cd = f_denominator(self);
    return f_complex_new2(CLASS_OF(self),
			  f_mul(f_numerator(dat->real),
				f_div(cd, f_denominator(dat->real))),
			  f_mul(f_numerator(dat->imag),
				f_div(cd, f_denominator(dat->imag))));
}

/* :nodoc: */
static VALUE
nucomp_hash(VALUE self)
{
    st_index_t v, h[2];
    VALUE n;

    get_dat1(self);
    n = rb_hash(dat->real);
    h[0] = NUM2LONG(n);
    n = rb_hash(dat->imag);
    h[1] = NUM2LONG(n);
    v = rb_memhash(h, sizeof(h));
    return LONG2FIX(v);
}

/* :nodoc: */
static VALUE
nucomp_eql_p(VALUE self, VALUE other)
{
    if (k_complex_p(other)) {
	get_dat2(self, other);

	return f_boolcast((CLASS_OF(adat->real) == CLASS_OF(bdat->real)) &&
			  (CLASS_OF(adat->imag) == CLASS_OF(bdat->imag)) &&
			  f_eqeq_p(self, other));

    }
    return Qfalse;
}

inline static VALUE
f_signbit(VALUE x)
{
#if defined(HAVE_SIGNBIT) && defined(__GNUC__) && defined(__sun) && \
    !defined(signbit)
    extern int signbit(double);
#endif
    switch (TYPE(x)) {
      case T_FLOAT: {
	double f = RFLOAT_VALUE(x);
	return f_boolcast(!isnan(f) && signbit(f));
      }
    }
    return f_negative_p(x);
}

inline static VALUE
f_tpositive_p(VALUE x)
{
    return f_boolcast(!f_signbit(x));
}

static VALUE
f_format(VALUE self, VALUE (*func)(VALUE))
{
    VALUE s, impos;

    get_dat1(self);

    impos = f_tpositive_p(dat->imag);

    s = (*func)(dat->real);
    rb_str_cat2(s, !impos ? "-" : "+");

    rb_str_concat(s, (*func)(f_abs(dat->imag)));
    if (!rb_isdigit(RSTRING_PTR(s)[RSTRING_LEN(s) - 1]))
	rb_str_cat2(s, "*");
    rb_str_cat2(s, "i");

    return s;
}

/*
 * call-seq:
 *    cmp.to_s  ->  string
 *
 * Returns the value as a string.
 */
static VALUE
nucomp_to_s(VALUE self)
{
    return f_format(self, f_to_s);
}

/*
 * call-seq:
 *    cmp.inspect  ->  string
 *
 * Returns the value as a string for inspection.
 */
static VALUE
nucomp_inspect(VALUE self)
{
    VALUE s;

    s = rb_usascii_str_new2("(");
    rb_str_concat(s, f_format(self, f_inspect));
    rb_str_cat2(s, ")");

    return s;
}

/* :nodoc: */
static VALUE
nucomp_marshal_dump(VALUE self)
{
    VALUE a;
    get_dat1(self);

    a = rb_assoc_new(dat->real, dat->imag);
    rb_copy_generic_ivar(a, self);
    return a;
}

/* :nodoc: */
static VALUE
nucomp_marshal_load(VALUE self, VALUE a)
{
    get_dat1(self);
    Check_Type(a, T_ARRAY);
    if (RARRAY_LEN(a) != 2)
	rb_raise(rb_eArgError, "marshaled complex must have an array whose length is 2 but %ld", RARRAY_LEN(a));
    dat->real = RARRAY_PTR(a)[0];
    dat->imag = RARRAY_PTR(a)[1];
    rb_copy_generic_ivar(self, a);
    return self;
}

/* --- */

VALUE
rb_complex_raw(VALUE x, VALUE y)
{
    return nucomp_s_new_internal(rb_cComplex, x, y);
}

VALUE
rb_complex_new(VALUE x, VALUE y)
{
    return nucomp_s_canonicalize_internal(rb_cComplex, x, y);
}

VALUE
rb_complex_polar(VALUE x, VALUE y)
{
    return f_complex_polar(rb_cComplex, x, y);
}

static VALUE nucomp_s_convert(int argc, VALUE *argv, VALUE klass);

VALUE
rb_Complex(VALUE x, VALUE y)
{
    VALUE a[2];
    a[0] = x;
    a[1] = y;
    return nucomp_s_convert(2, a, rb_cComplex);
}

/*
 * call-seq:
 *    cmp.to_i  ->  integer
 *
 * Returns the value as an integer if possible.
 */
static VALUE
nucomp_to_i(VALUE self)
{
    get_dat1(self);

    if (k_inexact_p(dat->imag) || f_nonzero_p(dat->imag)) {
	VALUE s = f_to_s(self);
	rb_raise(rb_eRangeError, "can't convert %s into Integer",
		 StringValuePtr(s));
    }
    return f_to_i(dat->real);
}

/*
 * call-seq:
 *    cmp.to_f  ->  float
 *
 * Returns the value as a float if possible.
 */
static VALUE
nucomp_to_f(VALUE self)
{
    get_dat1(self);

    if (k_inexact_p(dat->imag) || f_nonzero_p(dat->imag)) {
	VALUE s = f_to_s(self);
	rb_raise(rb_eRangeError, "can't convert %s into Float",
		 StringValuePtr(s));
    }
    return f_to_f(dat->real);
}

/*
 * call-seq:
 *    cmp.to_r  ->  rational
 *
 * If the imaginary part is exactly 0, returns the real part as a Rational,
 * otherwise a RangeError is raised.
 */
static VALUE
nucomp_to_r(VALUE self)
{
    get_dat1(self);

    if (k_inexact_p(dat->imag) || f_nonzero_p(dat->imag)) {
	VALUE s = f_to_s(self);
	rb_raise(rb_eRangeError, "can't convert %s into Rational",
		 StringValuePtr(s));
    }
    return f_to_r(dat->real);
}

/*
 * call-seq:
 *    cmp.rationalize([eps])  ->  rational
 *
 * If the imaginary part is exactly 0, returns the real part as a Rational,
 * otherwise a RangeError is raised.
 */
static VALUE
nucomp_rationalize(int argc, VALUE *argv, VALUE self)
{
    get_dat1(self);

    rb_scan_args(argc, argv, "01", NULL);

    if (k_inexact_p(dat->imag) || f_nonzero_p(dat->imag)) {
       VALUE s = f_to_s(self);
       rb_raise(rb_eRangeError, "can't convert %s into Rational",
                StringValuePtr(s));
    }
    return rb_funcall2(dat->real, rb_intern("rationalize"), argc, argv);
}

/*
 * call-seq:
 *    nil.to_c  ->  (0+0i)
 *
 * Returns zero as a complex.
 */
static VALUE
nilclass_to_c(VALUE self)
{
    return rb_complex_new1(INT2FIX(0));
}

/*
 * call-seq:
 *    num.to_c  ->  complex
 *
 * Returns the value as a complex.
 */
static VALUE
numeric_to_c(VALUE self)
{
    return rb_complex_new1(self);
}

static VALUE comp_pat0, comp_pat1, comp_pat2, a_slash, a_dot_and_an_e,
    null_string, underscores_pat, an_underscore;

#define WS "\\s*"
#define DIGITS "(?:[0-9](?:_[0-9]|[0-9])*)"
#define NUMERATOR "(?:" DIGITS "?\\.)?" DIGITS "(?:[eE][-+]?" DIGITS ")?"
#define DENOMINATOR DIGITS
#define NUMBER "[-+]?" NUMERATOR "(?:\\/" DENOMINATOR ")?"
#define NUMBERNOS NUMERATOR "(?:\\/" DENOMINATOR ")?"
#define PATTERN0 "\\A" WS "(" NUMBER ")@(" NUMBER ")" WS
#define PATTERN1 "\\A" WS "([-+])?(" NUMBER ")?[iIjJ]" WS
#define PATTERN2 "\\A" WS "(" NUMBER ")(([-+])(" NUMBERNOS ")?[iIjJ])?" WS

static void
make_patterns(void)
{
    static const char comp_pat0_source[] = PATTERN0;
    static const char comp_pat1_source[] = PATTERN1;
    static const char comp_pat2_source[] = PATTERN2;
    static const char underscores_pat_source[] = "_+";

    if (comp_pat0) return;

    comp_pat0 = rb_reg_new(comp_pat0_source, sizeof comp_pat0_source - 1, 0);
    rb_gc_register_mark_object(comp_pat0);

    comp_pat1 = rb_reg_new(comp_pat1_source, sizeof comp_pat1_source - 1, 0);
    rb_gc_register_mark_object(comp_pat1);

    comp_pat2 = rb_reg_new(comp_pat2_source, sizeof comp_pat2_source - 1, 0);
    rb_gc_register_mark_object(comp_pat2);

    a_slash = rb_usascii_str_new2("/");
    rb_gc_register_mark_object(a_slash);

    a_dot_and_an_e = rb_usascii_str_new2(".eE");
    rb_gc_register_mark_object(a_dot_and_an_e);

    null_string = rb_usascii_str_new2("");
    rb_gc_register_mark_object(null_string);

    underscores_pat = rb_reg_new(underscores_pat_source,
				 sizeof underscores_pat_source - 1, 0);
    rb_gc_register_mark_object(underscores_pat);

    an_underscore = rb_usascii_str_new2("_");
    rb_gc_register_mark_object(an_underscore);
}

#define id_match rb_intern("match")
#define f_match(x,y) rb_funcall((x), id_match, 1, (y))

#define id_gsub_bang rb_intern("gsub!")
#define f_gsub_bang(x,y,z) rb_funcall((x), id_gsub_bang, 2, (y), (z))

static VALUE
string_to_c_internal(VALUE self)
{
    VALUE s;

    s = self;

    if (RSTRING_LEN(s) == 0)
	return rb_assoc_new(Qnil, self);

    {
	VALUE m, sr, si, re, r, i;
	int po;

	m = f_match(comp_pat0, s);
	if (!NIL_P(m)) {
	    sr = rb_reg_nth_match(1, m);
	    si = rb_reg_nth_match(2, m);
	    re = rb_reg_match_post(m);
	    po = 1;
	}
	if (NIL_P(m)) {
	    m = f_match(comp_pat1, s);
	    if (!NIL_P(m)) {
		sr = Qnil;
		si = rb_reg_nth_match(1, m);
		if (NIL_P(si))
		    si = rb_usascii_str_new2("");
		{
		    VALUE t;

		    t = rb_reg_nth_match(2, m);
		    if (NIL_P(t))
			t = rb_usascii_str_new2("1");
		    rb_str_concat(si, t);
		}
		re = rb_reg_match_post(m);
		po = 0;
	    }
	}
	if (NIL_P(m)) {
	    m = f_match(comp_pat2, s);
	    if (NIL_P(m))
		return rb_assoc_new(Qnil, self);
	    sr = rb_reg_nth_match(1, m);
	    if (NIL_P(rb_reg_nth_match(2, m)))
		si = Qnil;
	    else {
		VALUE t;

		si = rb_reg_nth_match(3, m);
		t = rb_reg_nth_match(4, m);
		if (NIL_P(t))
		    t = rb_usascii_str_new2("1");
		rb_str_concat(si, t);
	    }
	    re = rb_reg_match_post(m);
	    po = 0;
	}
	r = INT2FIX(0);
	i = INT2FIX(0);
	if (!NIL_P(sr)) {
	    if (strchr(RSTRING_PTR(sr), '/'))
		r = f_to_r(sr);
	    else if (strpbrk(RSTRING_PTR(sr), ".eE"))
		r = f_to_f(sr);
	    else
		r = f_to_i(sr);
	}
	if (!NIL_P(si)) {
	    if (strchr(RSTRING_PTR(si), '/'))
		i = f_to_r(si);
	    else if (strpbrk(RSTRING_PTR(si), ".eE"))
		i = f_to_f(si);
	    else
		i = f_to_i(si);
	}
	if (po)
	    return rb_assoc_new(rb_complex_polar(r, i), re);
	else
	    return rb_assoc_new(rb_complex_new2(r, i), re);
    }
}

static VALUE
string_to_c_strict(VALUE self)
{
    VALUE a = string_to_c_internal(self);
    if (NIL_P(RARRAY_PTR(a)[0]) || RSTRING_LEN(RARRAY_PTR(a)[1]) > 0) {
	VALUE s = f_inspect(self);
	rb_raise(rb_eArgError, "invalid value for convert(): %s",
		 StringValuePtr(s));
    }
    return RARRAY_PTR(a)[0];
}

#define id_gsub rb_intern("gsub")
#define f_gsub(x,y,z) rb_funcall((x), id_gsub, 2, (y), (z))

/*
 * call-seq:
 *    str.to_c  ->  complex
 *
 * Returns a complex which denotes the string form.  The parser
 * ignores leading whitespaces and trailing garbage.  Any digit
 * sequences can be separated by an underscore.  Returns zero for null
 * or garbage string.
 *
 * For example:
 *
 *    '9'.to_c           #=> (9+0i)
 *    '2.5'.to_c         #=> (2.5+0i)
 *    '2.5/1'.to_c       #=> ((5/2)+0i)
 *    '-3/2'.to_c        #=> ((-3/2)+0i)
 *    '-i'.to_c          #=> (0-1i)
 *    '45i'.to_c         #=> (0+45i)
 *    '3-4i'.to_c        #=> (3-4i)
 *    '-4e2-4e-2i'.to_c  #=> (-400.0-0.04i)
 *    '-0.0-0.0i'.to_c   #=> (-0.0-0.0i)
 *    '1/2+3/4i'.to_c    #=> ((1/2)+(3/4)*i)
 *    'ruby'.to_c        #=> (0+0i)
 */
static VALUE
string_to_c(VALUE self)
{
    VALUE s, a, backref;

    backref = rb_backref_get();
    rb_match_busy(backref);

    s = f_gsub(self, underscores_pat, an_underscore);
    a = string_to_c_internal(s);

    rb_backref_set(backref);

    if (!NIL_P(RARRAY_PTR(a)[0]))
	return RARRAY_PTR(a)[0];
    return rb_complex_new1(INT2FIX(0));
}

static VALUE
nucomp_s_convert(int argc, VALUE *argv, VALUE klass)
{
    VALUE a1, a2, backref;

    rb_scan_args(argc, argv, "11", &a1, &a2);

    if (NIL_P(a1) || (argc == 2 && NIL_P(a2)))
	rb_raise(rb_eTypeError, "can't convert nil into Complex");

    backref = rb_backref_get();
    rb_match_busy(backref);

    switch (TYPE(a1)) {
      case T_FIXNUM:
      case T_BIGNUM:
      case T_FLOAT:
	break;
      case T_STRING:
	a1 = string_to_c_strict(a1);
	break;
    }

    switch (TYPE(a2)) {
      case T_FIXNUM:
      case T_BIGNUM:
      case T_FLOAT:
	break;
      case T_STRING:
	a2 = string_to_c_strict(a2);
	break;
    }

    rb_backref_set(backref);

    switch (TYPE(a1)) {
      case T_COMPLEX:
	{
	    get_dat1(a1);

	    if (k_exact_zero_p(dat->imag))
		a1 = dat->real;
	}
    }

    switch (TYPE(a2)) {
      case T_COMPLEX:
	{
	    get_dat1(a2);

	    if (k_exact_zero_p(dat->imag))
		a2 = dat->real;
	}
    }

    switch (TYPE(a1)) {
      case T_COMPLEX:
	if (argc == 1 || (k_exact_zero_p(a2)))
	    return a1;
    }

    if (argc == 1) {
	if (k_numeric_p(a1) && !f_real_p(a1))
	    return a1;
	/* should raise exception for consistency */
	if (!k_numeric_p(a1))
	    return rb_convert_type(a1, T_COMPLEX, "Complex", "to_c");
    }
    else {
	if ((k_numeric_p(a1) && k_numeric_p(a2)) &&
	    (!f_real_p(a1) || !f_real_p(a2)))
	    return f_add(a1,
			 f_mul(a2,
			       f_complex_new_bang2(rb_cComplex, ZERO, ONE)));
    }

    {
	VALUE argv2[2];
	argv2[0] = a1;
	argv2[1] = a2;
	return nucomp_s_new(argc, argv2, klass);
    }
}

/* --- */

/*
 * call-seq:
 *    num.real  ->  self
 *
 * Returns self.
 */
static VALUE
numeric_real(VALUE self)
{
    return self;
}

/*
 * call-seq:
 *    num.imag       ->  0
 *    num.imaginary  ->  0
 *
 * Returns zero.
 */
static VALUE
numeric_imag(VALUE self)
{
    return INT2FIX(0);
}

/*
 * call-seq:
 *    num.abs2  ->  real
 *
 * Returns square of self.
 */
static VALUE
numeric_abs2(VALUE self)
{
    return f_mul(self, self);
}

#define id_PI rb_intern("PI")

/*
 * call-seq:
 *    num.arg    ->  0 or float
 *    num.angle  ->  0 or float
 *    num.phase  ->  0 or float
 *
 * Returns 0 if the value is positive, pi otherwise.
 */
static VALUE
numeric_arg(VALUE self)
{
    if (f_positive_p(self))
	return INT2FIX(0);
    return rb_const_get(rb_mMath, id_PI);
}

/*
 * call-seq:
 *    num.rect  ->  array
 *
 * Returns an array; [num, 0].
 */
static VALUE
numeric_rect(VALUE self)
{
    return rb_assoc_new(self, INT2FIX(0));
}

/*
 * call-seq:
 *    num.polar  ->  array
 *
 * Returns an array; [num.abs, num.arg].
 */
static VALUE
numeric_polar(VALUE self)
{
    return rb_assoc_new(f_abs(self), f_arg(self));
}

/*
 * call-seq:
 *    num.conj       ->  self
 *    num.conjugate  ->  self
 *
 * Returns self.
 */
static VALUE
numeric_conj(VALUE self)
{
    return self;
}

/*
 * call-seq:
 *    flo.arg    ->  0 or float
 *    flo.angle  ->  0 or float
 *    flo.phase  ->  0 or float
 *
 * Returns 0 if the value is positive, pi otherwise.
 */
static VALUE
float_arg(VALUE self)
{
    if (isnan(RFLOAT_VALUE(self)))
	return self;
    if (f_tpositive_p(self))
	return INT2FIX(0);
    return rb_const_get(rb_mMath, id_PI);
}

/*
 * A complex number can be represented as a paired real number with
 * imaginary unit; a+bi.  Where a is real part, b is imaginary part
 * and i is imaginary unit.  Real a equals complex a+0i
 * mathematically.
 *
 * In ruby, you can create complex object with Complex, Complex::rect,
 * Complex::polar or to_c method.
 *
 *    Complex(1)           #=> (1+0i)
 *    Complex(2, 3)        #=> (2+3i)
 *    Complex.polar(2, 3)  #=> (-1.9799849932008908+0.2822400161197344i)
 *    3.to_c               #=> (3+0i)
 *
 * You can also create complex object from floating-point numbers or
 * strings.
 *
 *    Complex(0.3)         #=> (0.3+0i)
 *    Complex('0.3-0.5i')  #=> (0.3-0.5i)
 *    Complex('2/3+3/4i')  #=> ((2/3)+(3/4)*i)
 *    Complex('1@2')       #=> (-0.4161468365471424+0.9092974268256817i)
 *
 *    0.3.to_c             #=> (0.3+0i)
 *    '0.3-0.5i'.to_c      #=> (0.3-0.5i)
 *    '2/3+3/4i'.to_c      #=> ((2/3)+(3/4)*i)
 *    '1@2'.to_c           #=> (-0.4161468365471424+0.9092974268256817i)
 *
 * A complex object is either an exact or an inexact number.
 *
 *    Complex(1, 1) / 2    #=> ((1/2)+(1/2)*i)
 *    Complex(1, 1) / 2.0  #=> (0.5+0.5i)
 */
void
Init_Complex(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    assert(fprintf(stderr, "assert() is now active\n"));

    id_abs = rb_intern("abs");
    id_abs2 = rb_intern("abs2");
    id_arg = rb_intern("arg");
    id_cmp = rb_intern("<=>");
    id_conj = rb_intern("conj");
    id_convert = rb_intern("convert");
    id_denominator = rb_intern("denominator");
    id_divmod = rb_intern("divmod");
    id_eqeq_p = rb_intern("==");
    id_expt = rb_intern("**");
    id_fdiv = rb_intern("fdiv");
    id_floor = rb_intern("floor");
    id_idiv = rb_intern("div");
    id_imag = rb_intern("imag");
    id_inspect = rb_intern("inspect");
    id_negate = rb_intern("-@");
    id_numerator = rb_intern("numerator");
    id_quo = rb_intern("quo");
    id_real = rb_intern("real");
    id_real_p = rb_intern("real?");
    id_to_f = rb_intern("to_f");
    id_to_i = rb_intern("to_i");
    id_to_r = rb_intern("to_r");
    id_to_s = rb_intern("to_s");

    rb_cComplex = rb_define_class("Complex", rb_cNumeric);

    rb_define_alloc_func(rb_cComplex, nucomp_s_alloc);
    rb_undef_method(CLASS_OF(rb_cComplex), "allocate");

#if 0
    rb_define_private_method(CLASS_OF(rb_cComplex), "new!", nucomp_s_new_bang, -1);
    rb_define_private_method(CLASS_OF(rb_cComplex), "new", nucomp_s_new, -1);
#else
    rb_undef_method(CLASS_OF(rb_cComplex), "new");
#endif

    rb_define_singleton_method(rb_cComplex, "rectangular", nucomp_s_new, -1);
    rb_define_singleton_method(rb_cComplex, "rect", nucomp_s_new, -1);
    rb_define_singleton_method(rb_cComplex, "polar", nucomp_s_polar, -1);

    rb_define_global_function("Complex", nucomp_f_complex, -1);

    rb_undef_method(rb_cComplex, "%");
    rb_undef_method(rb_cComplex, "<");
    rb_undef_method(rb_cComplex, "<=");
    rb_undef_method(rb_cComplex, "<=>");
    rb_undef_method(rb_cComplex, ">");
    rb_undef_method(rb_cComplex, ">=");
    rb_undef_method(rb_cComplex, "between?");
    rb_undef_method(rb_cComplex, "div");
    rb_undef_method(rb_cComplex, "divmod");
    rb_undef_method(rb_cComplex, "floor");
    rb_undef_method(rb_cComplex, "ceil");
    rb_undef_method(rb_cComplex, "modulo");
    rb_undef_method(rb_cComplex, "remainder");
    rb_undef_method(rb_cComplex, "round");
    rb_undef_method(rb_cComplex, "step");
    rb_undef_method(rb_cComplex, "truncate");
    rb_undef_method(rb_cComplex, "i");

#if 0 /* NUBY */
    rb_undef_method(rb_cComplex, "//");
#endif

    rb_define_method(rb_cComplex, "real", nucomp_real, 0);
    rb_define_method(rb_cComplex, "imaginary", nucomp_imag, 0);
    rb_define_method(rb_cComplex, "imag", nucomp_imag, 0);

    rb_define_method(rb_cComplex, "-@", nucomp_negate, 0);
    rb_define_method(rb_cComplex, "+", nucomp_add, 1);
    rb_define_method(rb_cComplex, "-", nucomp_sub, 1);
    rb_define_method(rb_cComplex, "*", nucomp_mul, 1);
    rb_define_method(rb_cComplex, "/", nucomp_div, 1);
    rb_define_method(rb_cComplex, "quo", nucomp_quo, 1);
    rb_define_method(rb_cComplex, "fdiv", nucomp_fdiv, 1);
    rb_define_method(rb_cComplex, "**", nucomp_expt, 1);

    rb_define_method(rb_cComplex, "==", nucomp_eqeq_p, 1);
    rb_define_method(rb_cComplex, "coerce", nucomp_coerce, 1);

    rb_define_method(rb_cComplex, "abs", nucomp_abs, 0);
    rb_define_method(rb_cComplex, "magnitude", nucomp_abs, 0);
    rb_define_method(rb_cComplex, "abs2", nucomp_abs2, 0);
    rb_define_method(rb_cComplex, "arg", nucomp_arg, 0);
    rb_define_method(rb_cComplex, "angle", nucomp_arg, 0);
    rb_define_method(rb_cComplex, "phase", nucomp_arg, 0);
    rb_define_method(rb_cComplex, "rectangular", nucomp_rect, 0);
    rb_define_method(rb_cComplex, "rect", nucomp_rect, 0);
    rb_define_method(rb_cComplex, "polar", nucomp_polar, 0);
    rb_define_method(rb_cComplex, "conjugate", nucomp_conj, 0);
    rb_define_method(rb_cComplex, "conj", nucomp_conj, 0);
#if 0
    rb_define_method(rb_cComplex, "~", nucomp_conj, 0); /* gcc */
#endif

    rb_define_method(rb_cComplex, "real?", nucomp_false, 0);
#if 0
    rb_define_method(rb_cComplex, "complex?", nucomp_true, 0);
    rb_define_method(rb_cComplex, "exact?", nucomp_exact_p, 0);
    rb_define_method(rb_cComplex, "inexact?", nucomp_inexact_p, 0);
#endif

    rb_define_method(rb_cComplex, "numerator", nucomp_numerator, 0);
    rb_define_method(rb_cComplex, "denominator", nucomp_denominator, 0);

    rb_define_method(rb_cComplex, "hash", nucomp_hash, 0);
    rb_define_method(rb_cComplex, "eql?", nucomp_eql_p, 1);

    rb_define_method(rb_cComplex, "to_s", nucomp_to_s, 0);
    rb_define_method(rb_cComplex, "inspect", nucomp_inspect, 0);

    rb_define_method(rb_cComplex, "marshal_dump", nucomp_marshal_dump, 0);
    rb_define_method(rb_cComplex, "marshal_load", nucomp_marshal_load, 1);

    /* --- */

    rb_define_method(rb_cComplex, "to_i", nucomp_to_i, 0);
    rb_define_method(rb_cComplex, "to_f", nucomp_to_f, 0);
    rb_define_method(rb_cComplex, "to_r", nucomp_to_r, 0);
    rb_define_method(rb_cComplex, "rationalize", nucomp_rationalize, -1);
    rb_define_method(rb_cNilClass, "to_c", nilclass_to_c, 0);
    rb_define_method(rb_cNumeric, "to_c", numeric_to_c, 0);

    make_patterns();

    rb_define_method(rb_cString, "to_c", string_to_c, 0);

    rb_define_private_method(CLASS_OF(rb_cComplex), "convert", nucomp_s_convert, -1);

    /* --- */

    rb_define_method(rb_cNumeric, "real", numeric_real, 0);
    rb_define_method(rb_cNumeric, "imaginary", numeric_imag, 0);
    rb_define_method(rb_cNumeric, "imag", numeric_imag, 0);
    rb_define_method(rb_cNumeric, "abs2", numeric_abs2, 0);
    rb_define_method(rb_cNumeric, "arg", numeric_arg, 0);
    rb_define_method(rb_cNumeric, "angle", numeric_arg, 0);
    rb_define_method(rb_cNumeric, "phase", numeric_arg, 0);
    rb_define_method(rb_cNumeric, "rectangular", numeric_rect, 0);
    rb_define_method(rb_cNumeric, "rect", numeric_rect, 0);
    rb_define_method(rb_cNumeric, "polar", numeric_polar, 0);
    rb_define_method(rb_cNumeric, "conjugate", numeric_conj, 0);
    rb_define_method(rb_cNumeric, "conj", numeric_conj, 0);

    rb_define_method(rb_cFloat, "arg", float_arg, 0);
    rb_define_method(rb_cFloat, "angle", float_arg, 0);
    rb_define_method(rb_cFloat, "phase", float_arg, 0);

    /*
     * The imaginary unit.
     */
    rb_define_const(rb_cComplex, "I",
		    f_complex_new_bang2(rb_cComplex, ZERO, ONE));
}

/*
Local variables:
c-file-style: "ruby"
End:
*/
