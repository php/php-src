/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend by Perforce and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Derick Rethans <derick@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* This is a header file for the strtod implementation by David M. Gay which
 * can be found in zend_strtod.c */
#ifndef ZEND_STRTOD_H
#define ZEND_STRTOD_H
#include <zend.h>

BEGIN_EXTERN_C()
#define ZEND_STRTOD_K_MAX 7
typedef struct _zend_strtod_bigint zend_strtod_bigint;
typedef struct _zend_strtod_state {
	zend_strtod_bigint *freelist[ZEND_STRTOD_K_MAX+1];
	zend_strtod_bigint *p5s;
	char *result;
} zend_strtod_state;
ZEND_API void zend_freedtoa(char *s);
ZEND_API char *zend_dtoa(double _d, int mode, int ndigits, int *decpt, bool *sign, char **rve);
ZEND_API char *zend_gcvt(double value, int ndigit, char dec_point, char exponent, char *buf);
ZEND_API double zend_strtod(const char *s00, const char **se);
ZEND_API double zend_hex_strtod(const char *str, const char **endptr);
ZEND_API double zend_oct_strtod(const char *str, const char **endptr);
ZEND_API double zend_bin_strtod(const char *str, const char **endptr);
ZEND_API int zend_shutdown_strtod(void);
END_EXTERN_C()

/* double limits */
#include <float.h>
#if defined(DBL_MANT_DIG) && defined(DBL_MIN_EXP)
#define ZEND_DOUBLE_MAX_LENGTH (3 + DBL_MANT_DIG - DBL_MIN_EXP)
#else
#define ZEND_DOUBLE_MAX_LENGTH 1080
#endif

#endif
