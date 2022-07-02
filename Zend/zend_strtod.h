/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
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
ZEND_API void zend_freedtoa(char *s);
ZEND_API char *zend_dtoa(double _d, int mode, int ndigits, int *decpt, bool *sign, char **rve);
ZEND_API char *zend_gcvt(double value, int ndigit, char dec_point, char exponent, char *buf);
ZEND_API double zend_strtod(const char *s00, const char **se);
ZEND_API double zend_hex_strtod(const char *str, const char **endptr);
ZEND_API double zend_oct_strtod(const char *str, const char **endptr);
ZEND_API double zend_bin_strtod(const char *str, const char **endptr);
ZEND_API int zend_startup_strtod(void);
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
