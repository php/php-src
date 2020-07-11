/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/

#ifndef LOCALE_LOCALE_H
#define LOCALE_LOCALE_H

#include <php.h>

void locale_register_constants( INIT_FUNC_ARGS );

#define OPTION_DEFAULT NULL
#define LOC_LANG_TAG "language"
#define LOC_SCRIPT_TAG "script"
#define LOC_REGION_TAG "region"
#define LOC_VARIANT_TAG "variant"
#define LOC_EXTLANG_TAG "extlang"
#define LOC_GRANDFATHERED_LANG_TAG "grandfathered"
#define LOC_PRIVATE_TAG "private"
#define LOC_CANONICALIZE_TAG "canonicalize"

#define LOCALE_INI_NAME "intl.default_locale"

#endif // LOCALE_LOCALE_H
