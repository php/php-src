/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Sæther Bakken <ssb@fast.no>                             |
   +----------------------------------------------------------------------+
 */

#ifndef SNPRINTF_H
#define SNPRINTF_H

#if !defined(HAVE_SNPRINTF) || defined(BROKEN_SNPRINTF)
extern int ap_php_snprintf(char *, size_t, const char *, ...);
#define snprintf ap_php_snprintf
#endif

#if !defined(HAVE_VSNPRINTF) || defined(BROKEN_VSNPRINTF)
extern int ap_php_vsnprintf(char *, size_t, const char *, va_list ap);
#define vsnprintf ap_php_vsnprintf
#endif

#if PHP_BROKEN_SPRINTF
int php_sprintf (char* s, const char* format, ...);
#define sprintf php_sprintf
#endif

#ifdef HAVE_GCVT

#define ap_php_ecvt ecvt
#define ap_php_fcvt fcvt
#define ap_php_gcvt gcvt

#else

extern char * ap_php_cvt(double arg, int ndigits, int *decpt, int *sign, int eflag);
extern char * ap_php_ecvt(double arg, int ndigits, int *decpt, int *sign);
extern char * ap_php_fcvt(double arg, int ndigits, int *decpt, int *sign);
extern char * ap_php_gcvt(double number, int ndigit, char *buf);

#endif

typedef enum {
	NO = 0, YES = 1
} boolean_e;

#define WIDE_INT		long
typedef WIDE_INT wide_int;
typedef unsigned WIDE_INT u_wide_int;

typedef int bool_int;

extern char * ap_php_conv_10(register wide_int num, register bool_int is_unsigned,
	   register bool_int * is_negative, char *buf_end, register int *len);

extern char * ap_php_conv_fp(register char format, register double num,
		 boolean_e add_dp, int precision, bool_int * is_negative, char *buf, int *len);

extern char * ap_php_conv_p2(register u_wide_int num, register int nbits,
		 char format, char *buf_end, register int *len);


#endif /* SNPRINTF_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
