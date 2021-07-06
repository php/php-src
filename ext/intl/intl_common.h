/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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

BEGIN_EXTERN_C()
#include <php.h>
END_EXTERN_C()
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

#define INTL_ZSTR_VAL(str) (UChar*) ZSTR_VAL(str)
#define INTL_ZSTR_LEN(str) UCHARS(ZSTR_LEN(str))

BEGIN_EXTERN_C()
extern zend_class_entry *IntlException_ce_ptr;
END_EXTERN_C()

#endif /* INTL_COMMON_H */
