/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
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

#endif /* SNPRINTF_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
