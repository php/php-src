/* tgamma.c  - public domain implementation of function tgamma(3m)

reference - Haruhiko Okumura: C-gengo niyoru saishin algorithm jiten
            (New Algorithm handbook in C language) (Gijyutsu hyouron
            sha, Tokyo, 1991) [in Japanese]
            http://oku.edu.mie-u.ac.jp/~okumura/algo/
*/

/***********************************************************
    gamma.c -- Gamma function
***********************************************************/
#include "ruby/config.h"
#include <math.h>
#include <errno.h>

#ifdef HAVE_LGAMMA_R

double tgamma(double x)
{
    int sign;
    double d;
    if (x == 0.0) { /* Pole Error */
        errno = ERANGE;
        return 1/x < 0 ? -HUGE_VAL : HUGE_VAL;
    }
    if (x < 0) {
        static double zero = 0.0;
        double i, f;
        f = modf(-x, &i);
        if (f == 0.0) { /* Domain Error */
            errno = EDOM;
            return zero/zero;
        }
    }
    d = lgamma_r(x, &sign);
    return sign * exp(d);
}

#else

#include <errno.h>
#define PI      3.14159265358979324  /* $\pi$ */
#define LOG_2PI 1.83787706640934548  /* $\log 2\pi$ */
#define N       8

#define B0  1                 /* Bernoulli numbers */
#define B1  (-1.0 / 2.0)
#define B2  ( 1.0 / 6.0)
#define B4  (-1.0 / 30.0)
#define B6  ( 1.0 / 42.0)
#define B8  (-1.0 / 30.0)
#define B10 ( 5.0 / 66.0)
#define B12 (-691.0 / 2730.0)
#define B14 ( 7.0 / 6.0)
#define B16 (-3617.0 / 510.0)

static double
loggamma(double x)  /* the natural logarithm of the Gamma function. */
{
    double v, w;

    v = 1;
    while (x < N) {  v *= x;  x++;  }
    w = 1 / (x * x);
    return ((((((((B16 / (16 * 15))  * w + (B14 / (14 * 13))) * w
                + (B12 / (12 * 11))) * w + (B10 / (10 *  9))) * w
                + (B8  / ( 8 *  7))) * w + (B6  / ( 6 *  5))) * w
                + (B4  / ( 4 *  3))) * w + (B2  / ( 2 *  1))) / x
                + 0.5 * LOG_2PI - log(v) - x + (x - 0.5) * log(x);
}

double tgamma(double x)  /* Gamma function */
{
    if (x == 0.0) { /* Pole Error */
        errno = ERANGE;
        return 1/x < 0 ? -HUGE_VAL : HUGE_VAL;
    }
    if (x < 0) {
        int sign;
        static double zero = 0.0;
        double i, f;
        f = modf(-x, &i);
        if (f == 0.0) { /* Domain Error */
            errno = EDOM;
            return zero/zero;
        }
        sign = (fmod(i, 2.0) != 0.0) ? 1 : -1;
        return sign * PI / (sin(PI * f) * exp(loggamma(1 - x)));
    }
    return exp(loggamma(x));
}
#endif
