/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
