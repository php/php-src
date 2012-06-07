/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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
   +----------------------------------------------------------------------+
 */

#ifndef INTL_COMMON_ENUM_H
#define INTL_COMMON_ENUM_H

#include <unicode/umachine.h>
#ifdef __cplusplus
#include <unicode/strenum.h>
extern "C" {
#include <math.h>
#endif
#include <php.h>
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
U_CFUNC void IntlIterator_from_StringEnumeration(StringEnumeration *se, zval *object TSRMLS_DC);
#endif

U_CFUNC void intl_register_IntlIterator_class(TSRMLS_D);

#endif // INTL_COMMON_ENUM_H
