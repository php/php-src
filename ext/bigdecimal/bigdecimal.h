/*
 *
 * Ruby BigDecimal(Variable decimal precision) extension library.
 *
 * Copyright(C) 2002 by Shigeo Kobayashi(shigeo@tinyforest.gr.jp)
 *
 * You may distribute under the terms of either the GNU General Public
 * License or the Artistic License, as specified in the README file
 * of this BigDecimal distribution.
 *
 * NOTES:
 *   2003-03-28 V1.0 checked in.
 *
 */

#ifndef  RUBY_BIG_DECIMAL_H
#define  RUBY_BIG_DECIMAL_H 1

#include "ruby/ruby.h"
#include <float.h>

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

#ifndef HAVE_LABS
static inline long
labs(long const x)
{
    if (x < 0) return -x;
    return x;
}
#endif

#ifndef HAVE_LLABS
static inline LONG_LONG
llabs(LONG_LONG const x)
{
    if (x < 0) return -x;
    return x;
}
#endif

#ifdef vabs
# undef vabs
#endif
#if SIZEOF_VALUE <= SIZEOF_INT
# define vabs abs
#elif SIZEOF_VALUE <= SIZEOF_LONG
# define vabs labs
#elif SIZEOF_VALUE <= SIZEOF_LONG_LONG
# define vabs llabs
#endif

extern VALUE rb_cBigDecimal;

#if 0 || SIZEOF_BDIGITS >= 16
# define RMPD_COMPONENT_FIGURES 38
# define RMPD_BASE ((BDIGIT)100000000000000000000000000000000000000U)
#elif SIZEOF_BDIGITS >= 8
# define RMPD_COMPONENT_FIGURES 19
# define RMPD_BASE ((BDIGIT)10000000000000000000U)
#elif SIZEOF_BDIGITS >= 4
# define RMPD_COMPONENT_FIGURES 9
# define RMPD_BASE ((BDIGIT)1000000000U)
#elif SIZEOF_BDIGITS >= 2
# define RMPD_COMPONENT_FIGURES 4
# define RMPD_BASE ((BDIGIT)10000U)
#else
# define RMPD_COMPONENT_FIGURES 2
# define RMPD_BASE ((BDIGIT)100U)
#endif


/*
 *  NaN & Infinity
 */
#define SZ_NaN  "NaN"
#define SZ_INF  "Infinity"
#define SZ_PINF "+Infinity"
#define SZ_NINF "-Infinity"

/*
 *   #define VP_EXPORT other than static to let VP_ routines
 *   be called from outside of this module.
 */
#define VP_EXPORT static

/* Exception codes */
#define VP_EXCEPTION_ALL        ((unsigned short)0x00FF)
#define VP_EXCEPTION_INFINITY   ((unsigned short)0x0001)
#define VP_EXCEPTION_NaN        ((unsigned short)0x0002)
#define VP_EXCEPTION_UNDERFLOW  ((unsigned short)0x0004)
#define VP_EXCEPTION_OVERFLOW   ((unsigned short)0x0001) /* 0x0008) */
#define VP_EXCEPTION_ZERODIVIDE ((unsigned short)0x0010)

/* Following 2 exceptions cann't controlled by user */
#define VP_EXCEPTION_OP         ((unsigned short)0x0020)
#define VP_EXCEPTION_MEMORY     ((unsigned short)0x0040)

#define RMPD_EXCEPTION_MODE_DEFAULT 0U

/* Computation mode */
#define VP_ROUND_MODE            ((unsigned short)0x0100)
#define VP_ROUND_UP         1
#define VP_ROUND_DOWN       2
#define VP_ROUND_HALF_UP    3
#define VP_ROUND_HALF_DOWN  4
#define VP_ROUND_CEIL       5
#define VP_ROUND_FLOOR      6
#define VP_ROUND_HALF_EVEN  7

#define RMPD_ROUNDING_MODE_DEFAULT  VP_ROUND_HALF_UP

#define VP_SIGN_NaN                0 /* NaN                      */
#define VP_SIGN_POSITIVE_ZERO      1 /* Positive zero            */
#define VP_SIGN_NEGATIVE_ZERO     -1 /* Negative zero            */
#define VP_SIGN_POSITIVE_FINITE    2 /* Positive finite number   */
#define VP_SIGN_NEGATIVE_FINITE   -2 /* Negative finite number   */
#define VP_SIGN_POSITIVE_INFINITE  3 /* Positive infinite number */
#define VP_SIGN_NEGATIVE_INFINITE -3 /* Negative infinite number */

#ifdef __GNUC__
#define	FLEXIBLE_ARRAY_SIZE 0
#else
#define	FLEXIBLE_ARRAY_SIZE 1
#endif

/*
 * VP representation
 *  r = 0.xxxxxxxxx *BASE**exponent
 */
typedef struct {
    VALUE  obj;     /* Back pointer(VALUE) for Ruby object.     */
    size_t MaxPrec; /* Maximum precision size                   */
                    /* This is the actual size of pfrac[]       */
                    /*(frac[0] to frac[MaxPrec] are available). */
    size_t Prec;    /* Current precision size.                  */
                    /* This indicates how much the.             */
                    /* the array frac[] is actually used.       */
    SIGNED_VALUE exponent; /* Exponent part.                    */
    short  sign;    /* Attributes of the value.                 */
                    /*
                     *        ==0 : NaN
                     *          1 : Positive zero
                     *         -1 : Negative zero
                     *          2 : Positive number
                     *         -2 : Negative number
                     *          3 : Positive infinite number
                     *         -3 : Negative infinite number
                     */
    short  flag;    /* Not used in vp_routines,space for user.  */
    BDIGIT frac[FLEXIBLE_ARRAY_SIZE]; /* Array of fraction part. */
} Real;

/*
 *  ------------------
 *   EXPORTables.
 *  ------------------
 */

VP_EXPORT  Real *
VpNewRbClass(size_t mx, char const *str, VALUE klass);

VP_EXPORT  Real *VpCreateRbObject(size_t mx,const char *str);

static inline BDIGIT
rmpd_base_value(void) { return RMPD_BASE; }
static inline size_t
rmpd_component_figures(void) { return RMPD_COMPONENT_FIGURES; }
static inline size_t
rmpd_double_figures(void) { return 1+DBL_DIG; }

#define VpBaseFig() rmpd_component_figures()
#define VpDblFig() rmpd_double_figures()
#define VpBaseVal() rmpd_base_value()

/* Zero,Inf,NaN (isinf(),isnan() used to check) */
VP_EXPORT double VpGetDoubleNaN(void);
VP_EXPORT double VpGetDoublePosInf(void);
VP_EXPORT double VpGetDoubleNegInf(void);
VP_EXPORT double VpGetDoubleNegZero(void);

/* These 2 functions added at v1.1.7 */
VP_EXPORT size_t VpGetPrecLimit(void);
VP_EXPORT size_t VpSetPrecLimit(size_t n);

/* Round mode */
VP_EXPORT int            VpIsRoundMode(unsigned short n);
VP_EXPORT unsigned short VpGetRoundMode(void);
VP_EXPORT unsigned short VpSetRoundMode(unsigned short n);

VP_EXPORT int VpException(unsigned short f,const char *str,int always);
#if 0  /* unused */
VP_EXPORT int VpIsNegDoubleZero(double v);
#endif
VP_EXPORT size_t VpNumOfChars(Real *vp,const char *pszFmt);
VP_EXPORT size_t VpInit(BDIGIT BaseVal);
VP_EXPORT void *VpMemAlloc(size_t mb);
VP_EXPORT void *VpMemRealloc(void *ptr, size_t mb);
VP_EXPORT void VpFree(Real *pv);
VP_EXPORT Real *VpAlloc(size_t mx, const char *szVal);
VP_EXPORT size_t VpAsgn(Real *c, Real *a, int isw);
VP_EXPORT size_t VpAddSub(Real *c,Real *a,Real *b,int operation);
VP_EXPORT size_t VpMult(Real *c,Real *a,Real *b);
VP_EXPORT size_t VpDivd(Real *c,Real *r,Real *a,Real *b);
VP_EXPORT int VpComp(Real *a,Real *b);
VP_EXPORT ssize_t VpExponent10(Real *a);
VP_EXPORT void VpSzMantissa(Real *a,char *psz);
VP_EXPORT int VpToSpecialString(Real *a,char *psz,int fPlus);
VP_EXPORT void VpToString(Real *a, char *psz, size_t fFmt, int fPlus);
VP_EXPORT void VpToFString(Real *a, char *psz, size_t fFmt, int fPlus);
VP_EXPORT int VpCtoV(Real *a, const char *int_chr, size_t ni, const char *frac, size_t nf, const char *exp_chr, size_t ne);
VP_EXPORT int VpVtoD(double *d, SIGNED_VALUE *e, Real *m);
VP_EXPORT void VpDtoV(Real *m,double d);
#if 0  /* unused */
VP_EXPORT void VpItoV(Real *m,S_INT ival);
#endif
VP_EXPORT int VpSqrt(Real *y,Real *x);
VP_EXPORT int VpActiveRound(Real *y, Real *x, unsigned short f, ssize_t il);
VP_EXPORT int VpMidRound(Real *y, unsigned short f, ssize_t nf);
VP_EXPORT int VpLeftRound(Real *y, unsigned short f, ssize_t nf);
VP_EXPORT void VpFrac(Real *y, Real *x);
VP_EXPORT int VpPower(Real *y, Real *x, SIGNED_VALUE n);

/* VP constants */
VP_EXPORT Real *VpOne(void);

/*
 *  ------------------
 *  MACRO definitions.
 *  ------------------
 */
#define Abs(a)     (((a)>= 0)?(a):(-(a)))
#define Max(a, b)  (((a)>(b))?(a):(b))
#define Min(a, b)  (((a)>(b))?(b):(a))

#define VpMaxPrec(a)   ((a)->MaxPrec)
#define VpPrec(a)      ((a)->Prec)
#define VpGetFlag(a)   ((a)->flag)

/* Sign */

/* VpGetSign(a) returns 1,-1 if a>0,a<0 respectively */
#define VpGetSign(a) (((a)->sign>0)?1:(-1))
/* Change sign of a to a>0,a<0 if s = 1,-1 respectively */
#define VpChangeSign(a,s) {if((s)>0) (a)->sign=(short)Abs((ssize_t)(a)->sign);else (a)->sign=-(short)Abs((ssize_t)(a)->sign);}
/* Sets sign of a to a>0,a<0 if s = 1,-1 respectively */
#define VpSetSign(a,s)    {if((s)>0) (a)->sign=(short)VP_SIGN_POSITIVE_FINITE;else (a)->sign=(short)VP_SIGN_NEGATIVE_FINITE;}

/* 1 */
#define VpSetOne(a)       {(a)->Prec=(a)->exponent=(a)->frac[0]=1;(a)->sign=VP_SIGN_POSITIVE_FINITE;}

/* ZEROs */
#define VpIsPosZero(a)  ((a)->sign==VP_SIGN_POSITIVE_ZERO)
#define VpIsNegZero(a)  ((a)->sign==VP_SIGN_NEGATIVE_ZERO)
#define VpIsZero(a)     (VpIsPosZero(a) || VpIsNegZero(a))
#define VpSetPosZero(a) ((a)->frac[0]=0,(a)->Prec=1,(a)->sign=VP_SIGN_POSITIVE_ZERO)
#define VpSetNegZero(a) ((a)->frac[0]=0,(a)->Prec=1,(a)->sign=VP_SIGN_NEGATIVE_ZERO)
#define VpSetZero(a,s)  ( ((s)>0)?VpSetPosZero(a):VpSetNegZero(a) )

/* NaN */
#define VpIsNaN(a)      ((a)->sign==VP_SIGN_NaN)
#define VpSetNaN(a)     ((a)->frac[0]=0,(a)->Prec=1,(a)->sign=VP_SIGN_NaN)

/* Infinity */
#define VpIsPosInf(a)   ((a)->sign==VP_SIGN_POSITIVE_INFINITE)
#define VpIsNegInf(a)   ((a)->sign==VP_SIGN_NEGATIVE_INFINITE)
#define VpIsInf(a)      (VpIsPosInf(a) || VpIsNegInf(a))
#define VpIsDef(a)      ( !(VpIsNaN(a)||VpIsInf(a)) )
#define VpSetPosInf(a)  ((a)->frac[0]=0,(a)->Prec=1,(a)->sign=VP_SIGN_POSITIVE_INFINITE)
#define VpSetNegInf(a)  ((a)->frac[0]=0,(a)->Prec=1,(a)->sign=VP_SIGN_NEGATIVE_INFINITE)
#define VpSetInf(a,s)   ( ((s)>0)?VpSetPosInf(a):VpSetNegInf(a) )
#define VpHasVal(a)     (a->frac[0])
#define VpIsOne(a)      ((a->Prec==1)&&(a->frac[0]==1)&&(a->exponent==1))
#define VpExponent(a)   (a->exponent)
#ifdef BIGDECIMAL_DEBUG
int VpVarCheck(Real * v);
VP_EXPORT int VPrint(FILE *fp,const char *cntl_chr,Real *a);
#endif /* BIGDECIMAL_DEBUG */

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif
#endif /* RUBY_BIG_DECIMAL_H */
