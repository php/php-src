/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   |          Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef INTL_COMMON_H
#define INTL_COMMON_H
/* Auxiliary macros */

#include <php.h>
#include <unicode/utypes.h>

#ifndef UBYTES
# define UBYTES(len) ((len) * sizeof(UChar))
#endif

#ifndef eumalloc
# define eumalloc(size)  (UChar*)safe_emalloc(size, sizeof(UChar), 0)
#endif

#ifndef eurealloc
# define eurealloc(ptr, size)  (UChar*)erealloc((ptr), size * sizeof(UChar))
#endif

#define USIZE(data) sizeof((data))/sizeof(UChar)
#define UCHARS(len) ((len) / sizeof(UChar))

#define INTL_Z_STRVAL_P(str) (UChar*) Z_STRVAL_P(str)
#define INTL_Z_STRLEN_P(str) UCHARS( Z_STRLEN_P(str) )

#endif /* INTL_COMMON_H */
