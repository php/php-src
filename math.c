/**********************************************************************

  math.c -

  $Author$
  created at: Tue Jan 25 14:12:56 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#include "ruby/ruby.h"
#include "internal.h"
#include <math.h>
#include <errno.h>

#if defined(HAVE_SIGNBIT) && defined(__GNUC__) && defined(__sun) && \
    !defined(signbit)
    extern int signbit(double);
#endif

#define numberof(array) (int)(sizeof(array) / sizeof((array)[0]))

VALUE rb_mMath;
VALUE rb_eMathDomainError;

#define Need_Float(x) do {if (!RB_TYPE_P(x, T_FLOAT)) {(x) = rb_to_float(x);}} while(0)
#define Need_Float2(x,y) do {\
    Need_Float(x);\
    Need_Float(y);\
} while (0)

#define domain_error(msg) \
    rb_raise(rb_eMathDomainError, "Numerical argument is out of domain - " #msg);

/*
 *  call-seq:
 *     Math.atan2(y, x)  -> float
 *
 *  Computes the arc tangent given <i>y</i> and <i>x</i>. Returns
 *  -PI..PI.
 *
 *    Math.atan2(-0.0, -1.0) #=> -3.141592653589793
 *    Math.atan2(-1.0, -1.0) #=> -2.356194490192345
 *    Math.atan2(-1.0, 0.0)  #=> -1.5707963267948966
 *    Math.atan2(-1.0, 1.0)  #=> -0.7853981633974483
 *    Math.atan2(-0.0, 1.0)  #=> -0.0
 *    Math.atan2(0.0, 1.0)   #=> 0.0
 *    Math.atan2(1.0, 1.0)   #=> 0.7853981633974483
 *    Math.atan2(1.0, 0.0)   #=> 1.5707963267948966
 *    Math.atan2(1.0, -1.0)  #=> 2.356194490192345
 *    Math.atan2(0.0, -1.0)  #=> 3.141592653589793
 *
 */

static VALUE
math_atan2(VALUE obj, VALUE y, VALUE x)
{
#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif
    double dx, dy;
    Need_Float2(y, x);
    dx = RFLOAT_VALUE(x);
    dy = RFLOAT_VALUE(y);
    if (dx == 0.0 && dy == 0.0) {
	if (!signbit(dx))
	    return DBL2NUM(dy);
        if (!signbit(dy))
	    return DBL2NUM(M_PI);
	return DBL2NUM(-M_PI);
    }
    if (isinf(dx) && isinf(dy)) domain_error("atan2");
    return DBL2NUM(atan2(dy, dx));
}


/*
 *  call-seq:
 *     Math.cos(x)    -> float
 *
 *  Computes the cosine of <i>x</i> (expressed in radians). Returns
 *  -1..1.
 */

static VALUE
math_cos(VALUE obj, VALUE x)
{
    Need_Float(x);
    return DBL2NUM(cos(RFLOAT_VALUE(x)));
}

/*
 *  call-seq:
 *     Math.sin(x)    -> float
 *
 *  Computes the sine of <i>x</i> (expressed in radians). Returns
 *  -1..1.
 */

static VALUE
math_sin(VALUE obj, VALUE x)
{
    Need_Float(x);

    return DBL2NUM(sin(RFLOAT_VALUE(x)));
}


/*
 *  call-seq:
 *     Math.tan(x)    -> float
 *
 *  Returns the tangent of <i>x</i> (expressed in radians).
 */

static VALUE
math_tan(VALUE obj, VALUE x)
{
    Need_Float(x);

    return DBL2NUM(tan(RFLOAT_VALUE(x)));
}

/*
 *  call-seq:
 *     Math.acos(x)    -> float
 *
 *  Computes the arc cosine of <i>x</i>. Returns 0..PI.
 */

static VALUE
math_acos(VALUE obj, VALUE x)
{
    double d0, d;

    Need_Float(x);
    d0 = RFLOAT_VALUE(x);
    /* check for domain error */
    if (d0 < -1.0 || 1.0 < d0) domain_error("acos");
    d = acos(d0);
    return DBL2NUM(d);
}

/*
 *  call-seq:
 *     Math.asin(x)    -> float
 *
 *  Computes the arc sine of <i>x</i>. Returns -{PI/2} .. {PI/2}.
 */

static VALUE
math_asin(VALUE obj, VALUE x)
{
    double d0, d;

    Need_Float(x);
    d0 = RFLOAT_VALUE(x);
    /* check for domain error */
    if (d0 < -1.0 || 1.0 < d0) domain_error("asin");
    d = asin(d0);
    return DBL2NUM(d);
}

/*
 *  call-seq:
 *     Math.atan(x)    -> float
 *
 *  Computes the arc tangent of <i>x</i>. Returns -{PI/2} .. {PI/2}.
 */

static VALUE
math_atan(VALUE obj, VALUE x)
{
    Need_Float(x);
    return DBL2NUM(atan(RFLOAT_VALUE(x)));
}

#ifndef HAVE_COSH
double
cosh(double x)
{
    return (exp(x) + exp(-x)) / 2;
}
#endif

/*
 *  call-seq:
 *     Math.cosh(x)    -> float
 *
 *  Computes the hyperbolic cosine of <i>x</i> (expressed in radians).
 */

static VALUE
math_cosh(VALUE obj, VALUE x)
{
    Need_Float(x);

    return DBL2NUM(cosh(RFLOAT_VALUE(x)));
}

#ifndef HAVE_SINH
double
sinh(double x)
{
    return (exp(x) - exp(-x)) / 2;
}
#endif

/*
 *  call-seq:
 *     Math.sinh(x)    -> float
 *
 *  Computes the hyperbolic sine of <i>x</i> (expressed in
 *  radians).
 */

static VALUE
math_sinh(VALUE obj, VALUE x)
{
    Need_Float(x);
    return DBL2NUM(sinh(RFLOAT_VALUE(x)));
}

#ifndef HAVE_TANH
double
tanh(double x)
{
    return sinh(x) / cosh(x);
}
#endif

/*
 *  call-seq:
 *     Math.tanh()    -> float
 *
 *  Computes the hyperbolic tangent of <i>x</i> (expressed in
 *  radians).
 */

static VALUE
math_tanh(VALUE obj, VALUE x)
{
    Need_Float(x);
    return DBL2NUM(tanh(RFLOAT_VALUE(x)));
}

/*
 *  call-seq:
 *     Math.acosh(x)    -> float
 *
 *  Computes the inverse hyperbolic cosine of <i>x</i>.
 */

static VALUE
math_acosh(VALUE obj, VALUE x)
{
    double d0, d;

    Need_Float(x);
    d0 = RFLOAT_VALUE(x);
    /* check for domain error */
    if (d0 < 1.0) domain_error("acosh");
    d = acosh(d0);
    return DBL2NUM(d);
}

/*
 *  call-seq:
 *     Math.asinh(x)    -> float
 *
 *  Computes the inverse hyperbolic sine of <i>x</i>.
 */

static VALUE
math_asinh(VALUE obj, VALUE x)
{
    Need_Float(x);
    return DBL2NUM(asinh(RFLOAT_VALUE(x)));
}

/*
 *  call-seq:
 *     Math.atanh(x)    -> float
 *
 *  Computes the inverse hyperbolic tangent of <i>x</i>.
 */

static VALUE
math_atanh(VALUE obj, VALUE x)
{
    double d0, d;

    Need_Float(x);
    d0 = RFLOAT_VALUE(x);
    /* check for domain error */
    if (d0 <  -1.0 || +1.0 <  d0) domain_error("atanh");
    /* check for pole error */
    if (d0 == -1.0) return DBL2NUM(-INFINITY);
    if (d0 == +1.0) return DBL2NUM(+INFINITY);
    d = atanh(d0);
    return DBL2NUM(d);
}

/*
 *  call-seq:
 *     Math.exp(x)    -> float
 *
 *  Returns e**x.
 *
 *    Math.exp(0)       #=> 1.0
 *    Math.exp(1)       #=> 2.718281828459045
 *    Math.exp(1.5)     #=> 4.4816890703380645
 *
 */

static VALUE
math_exp(VALUE obj, VALUE x)
{
    Need_Float(x);
    return DBL2NUM(exp(RFLOAT_VALUE(x)));
}

#if defined __CYGWIN__
# include <cygwin/version.h>
# if CYGWIN_VERSION_DLL_MAJOR < 1005
#  define nan(x) nan()
# endif
# define log(x) ((x) < 0.0 ? nan("") : log(x))
# define log10(x) ((x) < 0.0 ? nan("") : log10(x))
#endif

/*
 *  call-seq:
 *     Math.log(numeric)    -> float
 *     Math.log(num,base)   -> float
 *
 *  Returns the natural logarithm of <i>numeric</i>.
 *  If additional second argument is given, it will be the base
 *  of logarithm.
 *
 *    Math.log(1)          #=> 0.0
 *    Math.log(Math::E)    #=> 1.0
 *    Math.log(Math::E**3) #=> 3.0
 *    Math.log(12,3)       #=> 2.2618595071429146
 *
 */

static VALUE
math_log(int argc, VALUE *argv)
{
    VALUE x, base;
    double d0, d;

    rb_scan_args(argc, argv, "11", &x, &base);
    Need_Float(x);
    d0 = RFLOAT_VALUE(x);
    /* check for domain error */
    if (d0 < 0.0) domain_error("log");
    /* check for pole error */
    if (d0 == 0.0) return DBL2NUM(-INFINITY);
    d = log(d0);
    if (argc == 2) {
	Need_Float(base);
	d /= log(RFLOAT_VALUE(base));
    }
    return DBL2NUM(d);
}

#ifndef log2
#ifndef HAVE_LOG2
double
log2(double x)
{
    return log10(x)/log10(2.0);
}
#else
extern double log2(double);
#endif
#endif

/*
 *  call-seq:
 *     Math.log2(numeric)    -> float
 *
 *  Returns the base 2 logarithm of <i>numeric</i>.
 *
 *    Math.log2(1)      #=> 0.0
 *    Math.log2(2)      #=> 1.0
 *    Math.log2(32768)  #=> 15.0
 *    Math.log2(65536)  #=> 16.0
 *
 */

static VALUE
math_log2(VALUE obj, VALUE x)
{
    double d0, d;

    Need_Float(x);
    d0 = RFLOAT_VALUE(x);
    /* check for domain error */
    if (d0 < 0.0) domain_error("log2");
    /* check for pole error */
    if (d0 == 0.0) return DBL2NUM(-INFINITY);
    d = log2(d0);
    return DBL2NUM(d);
}

/*
 *  call-seq:
 *     Math.log10(numeric)    -> float
 *
 *  Returns the base 10 logarithm of <i>numeric</i>.
 *
 *    Math.log10(1)       #=> 0.0
 *    Math.log10(10)      #=> 1.0
 *    Math.log10(10**100) #=> 100.0
 *
 */

static VALUE
math_log10(VALUE obj, VALUE x)
{
    double d0, d;

    Need_Float(x);
    d0 = RFLOAT_VALUE(x);
    /* check for domain error */
    if (d0 < 0.0) domain_error("log10");
    /* check for pole error */
    if (d0 == 0.0) return DBL2NUM(-INFINITY);
    d = log10(d0);
    return DBL2NUM(d);
}

/*
 *  call-seq:
 *     Math.sqrt(numeric)    -> float
 *
 *  Returns the non-negative square root of <i>numeric</i>.
 *
 *    0.upto(10) {|x|
 *      p [x, Math.sqrt(x), Math.sqrt(x)**2]
 *    }
 *    #=>
 *    [0, 0.0, 0.0]
 *    [1, 1.0, 1.0]
 *    [2, 1.4142135623731, 2.0]
 *    [3, 1.73205080756888, 3.0]
 *    [4, 2.0, 4.0]
 *    [5, 2.23606797749979, 5.0]
 *    [6, 2.44948974278318, 6.0]
 *    [7, 2.64575131106459, 7.0]
 *    [8, 2.82842712474619, 8.0]
 *    [9, 3.0, 9.0]
 *    [10, 3.16227766016838, 10.0]
 *
 */

static VALUE
math_sqrt(VALUE obj, VALUE x)
{
    double d0, d;

    Need_Float(x);
    d0 = RFLOAT_VALUE(x);
    /* check for domain error */
    if (d0 < 0.0) domain_error("sqrt");
    if (d0 == 0.0) return DBL2NUM(0.0);
    d = sqrt(d0);
    return DBL2NUM(d);
}

/*
 *  call-seq:
 *     Math.cbrt(numeric)    -> float
 *
 *  Returns the cube root of <i>numeric</i>.
 *
 *    -9.upto(9) {|x|
 *      p [x, Math.cbrt(x), Math.cbrt(x)**3]
 *    }
 *    #=>
 *    [-9, -2.0800838230519, -9.0]
 *    [-8, -2.0, -8.0]
 *    [-7, -1.91293118277239, -7.0]
 *    [-6, -1.81712059283214, -6.0]
 *    [-5, -1.7099759466767, -5.0]
 *    [-4, -1.5874010519682, -4.0]
 *    [-3, -1.44224957030741, -3.0]
 *    [-2, -1.25992104989487, -2.0]
 *    [-1, -1.0, -1.0]
 *    [0, 0.0, 0.0]
 *    [1, 1.0, 1.0]
 *    [2, 1.25992104989487, 2.0]
 *    [3, 1.44224957030741, 3.0]
 *    [4, 1.5874010519682, 4.0]
 *    [5, 1.7099759466767, 5.0]
 *    [6, 1.81712059283214, 6.0]
 *    [7, 1.91293118277239, 7.0]
 *    [8, 2.0, 8.0]
 *    [9, 2.0800838230519, 9.0]
 *
 */

static VALUE
math_cbrt(VALUE obj, VALUE x)
{
    Need_Float(x);
    return DBL2NUM(cbrt(RFLOAT_VALUE(x)));
}

/*
 *  call-seq:
 *     Math.frexp(numeric)    -> [ fraction, exponent ]
 *
 *  Returns a two-element array containing the normalized fraction (a
 *  <code>Float</code>) and exponent (a <code>Fixnum</code>) of
 *  <i>numeric</i>.
 *
 *     fraction, exponent = Math.frexp(1234)   #=> [0.6025390625, 11]
 *     fraction * 2**exponent                  #=> 1234.0
 */

static VALUE
math_frexp(VALUE obj, VALUE x)
{
    double d;
    int exp;

    Need_Float(x);

    d = frexp(RFLOAT_VALUE(x), &exp);
    return rb_assoc_new(DBL2NUM(d), INT2NUM(exp));
}

/*
 *  call-seq:
 *     Math.ldexp(flt, int) -> float
 *
 *  Returns the value of <i>flt</i>*(2**<i>int</i>).
 *
 *     fraction, exponent = Math.frexp(1234)
 *     Math.ldexp(fraction, exponent)   #=> 1234.0
 */

static VALUE
math_ldexp(VALUE obj, VALUE x, VALUE n)
{
    Need_Float(x);
    return DBL2NUM(ldexp(RFLOAT_VALUE(x), NUM2INT(n)));
}

/*
 *  call-seq:
 *     Math.hypot(x, y)    -> float
 *
 *  Returns sqrt(x**2 + y**2), the hypotenuse of a right-angled triangle
 *  with sides <i>x</i> and <i>y</i>.
 *
 *     Math.hypot(3, 4)   #=> 5.0
 */

static VALUE
math_hypot(VALUE obj, VALUE x, VALUE y)
{
    Need_Float2(x, y);
    return DBL2NUM(hypot(RFLOAT_VALUE(x), RFLOAT_VALUE(y)));
}

/*
 * call-seq:
 *    Math.erf(x)  -> float
 *
 *  Calculates the error function of x.
 */

static VALUE
math_erf(VALUE obj, VALUE x)
{
    Need_Float(x);
    return DBL2NUM(erf(RFLOAT_VALUE(x)));
}

/*
 * call-seq:
 *    Math.erfc(x)  -> float
 *
 *  Calculates the complementary error function of x.
 */

static VALUE
math_erfc(VALUE obj, VALUE x)
{
    Need_Float(x);
    return DBL2NUM(erfc(RFLOAT_VALUE(x)));
}

/*
 * call-seq:
 *    Math.gamma(x)  -> float
 *
 *  Calculates the gamma function of x.
 *
 *  Note that gamma(n) is same as fact(n-1) for integer n > 0.
 *  However gamma(n) returns float and can be an approximation.
 *
 *   def fact(n) (1..n).inject(1) {|r,i| r*i } end
 *   1.upto(26) {|i| p [i, Math.gamma(i), fact(i-1)] }
 *   #=> [1, 1.0, 1]
 *   #   [2, 1.0, 1]
 *   #   [3, 2.0, 2]
 *   #   [4, 6.0, 6]
 *   #   [5, 24.0, 24]
 *   #   [6, 120.0, 120]
 *   #   [7, 720.0, 720]
 *   #   [8, 5040.0, 5040]
 *   #   [9, 40320.0, 40320]
 *   #   [10, 362880.0, 362880]
 *   #   [11, 3628800.0, 3628800]
 *   #   [12, 39916800.0, 39916800]
 *   #   [13, 479001600.0, 479001600]
 *   #   [14, 6227020800.0, 6227020800]
 *   #   [15, 87178291200.0, 87178291200]
 *   #   [16, 1307674368000.0, 1307674368000]
 *   #   [17, 20922789888000.0, 20922789888000]
 *   #   [18, 355687428096000.0, 355687428096000]
 *   #   [19, 6.402373705728e+15, 6402373705728000]
 *   #   [20, 1.21645100408832e+17, 121645100408832000]
 *   #   [21, 2.43290200817664e+18, 2432902008176640000]
 *   #   [22, 5.109094217170944e+19, 51090942171709440000]
 *   #   [23, 1.1240007277776077e+21, 1124000727777607680000]
 *   #   [24, 2.5852016738885062e+22, 25852016738884976640000]
 *   #   [25, 6.204484017332391e+23, 620448401733239439360000]
 *   #   [26, 1.5511210043330954e+25, 15511210043330985984000000]
 *
 */

static VALUE
math_gamma(VALUE obj, VALUE x)
{
    static const double fact_table[] = {
        /* fact(0) */ 1.0,
        /* fact(1) */ 1.0,
        /* fact(2) */ 2.0,
        /* fact(3) */ 6.0,
        /* fact(4) */ 24.0,
        /* fact(5) */ 120.0,
        /* fact(6) */ 720.0,
        /* fact(7) */ 5040.0,
        /* fact(8) */ 40320.0,
        /* fact(9) */ 362880.0,
        /* fact(10) */ 3628800.0,
        /* fact(11) */ 39916800.0,
        /* fact(12) */ 479001600.0,
        /* fact(13) */ 6227020800.0,
        /* fact(14) */ 87178291200.0,
        /* fact(15) */ 1307674368000.0,
        /* fact(16) */ 20922789888000.0,
        /* fact(17) */ 355687428096000.0,
        /* fact(18) */ 6402373705728000.0,
        /* fact(19) */ 121645100408832000.0,
        /* fact(20) */ 2432902008176640000.0,
        /* fact(21) */ 51090942171709440000.0,
        /* fact(22) */ 1124000727777607680000.0,
        /* fact(23)=25852016738884976640000 needs 56bit mantissa which is
         * impossible to represent exactly in IEEE 754 double which have
         * 53bit mantissa. */
    };
    double d0, d;
    double intpart, fracpart;
    Need_Float(x);
    d0 = RFLOAT_VALUE(x);
    /* check for domain error */
    if (isinf(d0) && signbit(d0)) domain_error("gamma");
    fracpart = modf(d0, &intpart);
    if (fracpart == 0.0) {
	if (intpart < 0) domain_error("gamma");
	if (0 < intpart &&
	    intpart - 1 < (double)numberof(fact_table)) {
	    return DBL2NUM(fact_table[(int)intpart - 1]);
	}
    }
    d = tgamma(d0);
    return DBL2NUM(d);
}

/*
 * call-seq:
 *    Math.lgamma(x)  -> [float, -1 or 1]
 *
 *  Calculates the logarithmic gamma of x and
 *  the sign of gamma of x.
 *
 *  Math.lgamma(x) is same as
 *   [Math.log(Math.gamma(x).abs), Math.gamma(x) < 0 ? -1 : 1]
 *  but avoid overflow by Math.gamma(x) for large x.
 */

static VALUE
math_lgamma(VALUE obj, VALUE x)
{
    double d0, d;
    int sign=1;
    VALUE v;
    Need_Float(x);
    d0 = RFLOAT_VALUE(x);
    /* check for domain error */
    if (isinf(d0)) {
	if (signbit(d0)) domain_error("lgamma");
	return rb_assoc_new(DBL2NUM(INFINITY), INT2FIX(1));
    }
    d = lgamma_r(d0, &sign);
    v = DBL2NUM(d);
    return rb_assoc_new(v, INT2FIX(sign));
}


#define exp1(n) \
VALUE \
rb_math_##n(VALUE x)\
{\
    return math_##n(rb_mMath, x);\
}

#define exp2(n) \
VALUE \
rb_math_##n(VALUE x, VALUE y)\
{\
    return math_##n(rb_mMath, x, y);\
}

exp2(atan2)
exp1(cos)
exp1(cosh)
exp1(exp)
exp2(hypot)

VALUE
rb_math_log(int argc, VALUE *argv)
{
    return math_log(argc, argv);
}

exp1(sin)
exp1(sinh)
exp1(sqrt)


/*
 *  Document-class: Math::DomainError
 *
 *  Raised when a mathematical function is evaluated outside of its
 *  domain of definition.
 *
 *  For example, since +cos+ returns values in the range -1..1,
 *  its inverse function +acos+ is only defined on that interval:
 *
 *     Math.acos(42)
 *
 *  <em>produces:</em>
 *
 *     Math::DomainError: Numerical argument is out of domain - "acos"
 */

/*
 *  Document-class: Math
 *
 *  The <code>Math</code> module contains module functions for basic
 *  trigonometric and transcendental functions. See class
 *  <code>Float</code> for a list of constants that
 *  define Ruby's floating point accuracy.
 */


void
Init_Math(void)
{
    rb_mMath = rb_define_module("Math");
    rb_eMathDomainError = rb_define_class_under(rb_mMath, "DomainError", rb_eStandardError);

#ifdef M_PI
    rb_define_const(rb_mMath, "PI", DBL2NUM(M_PI));
#else
    rb_define_const(rb_mMath, "PI", DBL2NUM(atan(1.0)*4.0));
#endif

#ifdef M_E
    rb_define_const(rb_mMath, "E", DBL2NUM(M_E));
#else
    rb_define_const(rb_mMath, "E", DBL2NUM(exp(1.0)));
#endif

    rb_define_module_function(rb_mMath, "atan2", math_atan2, 2);
    rb_define_module_function(rb_mMath, "cos", math_cos, 1);
    rb_define_module_function(rb_mMath, "sin", math_sin, 1);
    rb_define_module_function(rb_mMath, "tan", math_tan, 1);

    rb_define_module_function(rb_mMath, "acos", math_acos, 1);
    rb_define_module_function(rb_mMath, "asin", math_asin, 1);
    rb_define_module_function(rb_mMath, "atan", math_atan, 1);

    rb_define_module_function(rb_mMath, "cosh", math_cosh, 1);
    rb_define_module_function(rb_mMath, "sinh", math_sinh, 1);
    rb_define_module_function(rb_mMath, "tanh", math_tanh, 1);

    rb_define_module_function(rb_mMath, "acosh", math_acosh, 1);
    rb_define_module_function(rb_mMath, "asinh", math_asinh, 1);
    rb_define_module_function(rb_mMath, "atanh", math_atanh, 1);

    rb_define_module_function(rb_mMath, "exp", math_exp, 1);
    rb_define_module_function(rb_mMath, "log", math_log, -1);
    rb_define_module_function(rb_mMath, "log2", math_log2, 1);
    rb_define_module_function(rb_mMath, "log10", math_log10, 1);
    rb_define_module_function(rb_mMath, "sqrt", math_sqrt, 1);
    rb_define_module_function(rb_mMath, "cbrt", math_cbrt, 1);

    rb_define_module_function(rb_mMath, "frexp", math_frexp, 1);
    rb_define_module_function(rb_mMath, "ldexp", math_ldexp, 2);

    rb_define_module_function(rb_mMath, "hypot", math_hypot, 2);

    rb_define_module_function(rb_mMath, "erf",  math_erf,  1);
    rb_define_module_function(rb_mMath, "erfc", math_erfc, 1);

    rb_define_module_function(rb_mMath, "gamma", math_gamma, 1);
    rb_define_module_function(rb_mMath, "lgamma", math_lgamma, 1);
}
