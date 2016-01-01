/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jim Winstead <jimw@php.net>                                 |
   |          Stig Sæther Bakken <ssb@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_MATH_H
#define PHP_MATH_H

PHPAPI char *_php_math_number_format(double, int, char, char);
PHPAPI char *_php_math_number_format_ex(double, int, char *, size_t, char *, size_t);
PHPAPI char * _php_math_longtobase(zval *arg, int base);
PHPAPI long _php_math_basetolong(zval *arg, int base);
PHPAPI int _php_math_basetozval(zval *arg, int base, zval *ret);
PHPAPI char * _php_math_zvaltobase(zval *arg, int base TSRMLS_DC);

PHP_FUNCTION(sin);
PHP_FUNCTION(cos);
PHP_FUNCTION(tan);
PHP_FUNCTION(asin);
PHP_FUNCTION(acos);
PHP_FUNCTION(atan);
PHP_FUNCTION(atan2);
PHP_FUNCTION(pi);
PHP_FUNCTION(exp);
PHP_FUNCTION(log);
PHP_FUNCTION(log10);
PHP_FUNCTION(is_finite);
PHP_FUNCTION(is_infinite);
PHP_FUNCTION(is_nan);
PHP_FUNCTION(pow);
PHP_FUNCTION(sqrt);
PHP_FUNCTION(srand);
PHP_FUNCTION(rand);
PHP_FUNCTION(getrandmax);
PHP_FUNCTION(mt_srand);
PHP_FUNCTION(mt_rand);
PHP_FUNCTION(mt_getrandmax);
PHP_FUNCTION(abs);
PHP_FUNCTION(ceil);
PHP_FUNCTION(floor);
PHP_FUNCTION(round);
PHP_FUNCTION(decbin);
PHP_FUNCTION(dechex);
PHP_FUNCTION(decoct);
PHP_FUNCTION(bindec);
PHP_FUNCTION(hexdec);
PHP_FUNCTION(octdec);
PHP_FUNCTION(base_convert);
PHP_FUNCTION(number_format);
PHP_FUNCTION(fmod);
PHP_FUNCTION(deg2rad);
PHP_FUNCTION(rad2deg);

   /*
   WARNING: these functions are expermental: they could change their names or 
   disappear in the next version of PHP!
   */
PHP_FUNCTION(hypot);
PHP_FUNCTION(expm1);
PHP_FUNCTION(log1p);

PHP_FUNCTION(sinh);
PHP_FUNCTION(cosh);
PHP_FUNCTION(tanh);

PHP_FUNCTION(asinh);
PHP_FUNCTION(acosh);
PHP_FUNCTION(atanh);

#include <math.h>

#ifndef M_E
#define M_E            2.7182818284590452354   /* e */
#endif

#ifndef M_LOG2E
#define M_LOG2E        1.4426950408889634074   /* log_2 e */
#endif

#ifndef M_LOG10E
#define M_LOG10E       0.43429448190325182765  /* log_10 e */
#endif

#ifndef M_LN2
#define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif

#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif

#ifndef M_PI_4
#define M_PI_4         0.78539816339744830962  /* pi/4 */
#endif

#ifndef M_1_PI
#define M_1_PI         0.31830988618379067154  /* 1/pi */
#endif

#ifndef M_2_PI
#define M_2_PI         0.63661977236758134308  /* 2/pi */
#endif

#ifndef M_SQRTPI
#define M_SQRTPI       1.77245385090551602729  /* sqrt(pi) */
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI     1.12837916709551257390  /* 2/sqrt(pi) */
#endif

#ifndef M_LNPI
#define M_LNPI         1.14472988584940017414  /* ln(pi) */
#endif

#ifndef M_EULER
#define M_EULER        0.57721566490153286061 /* Euler constant */
#endif

#ifndef M_SQRT2
#define M_SQRT2        1.41421356237309504880  /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2      0.70710678118654752440  /* 1/sqrt(2) */
#endif

#ifndef M_SQRT3
#define M_SQRT3	       1.73205080756887729352  /* sqrt(3) */
#endif

/* Define rounding modes (all are round-to-nearest) */
#ifndef PHP_ROUND_HALF_UP
#define PHP_ROUND_HALF_UP        0x01    /* Arithmetic rounding, up == away from zero */
#endif

#ifndef PHP_ROUND_HALF_DOWN
#define PHP_ROUND_HALF_DOWN      0x02    /* Down == towards zero */
#endif

#ifndef PHP_ROUND_HALF_EVEN
#define PHP_ROUND_HALF_EVEN      0x03    /* Banker's rounding */
#endif

#ifndef PHP_ROUND_HALF_ODD
#define PHP_ROUND_HALF_ODD       0x04
#endif

#endif /* PHP_MATH_H */
