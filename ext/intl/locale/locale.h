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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/

#ifndef LOCALE_LOCALE_H
#define LOCALE_LOCALE_H

#include <php.h>

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
