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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/

#ifndef INTL_CONVERTCPP_H
#define INTL_CONVERTCPP_H

#ifndef __cplusplus
#error Should be included only in C++ Files
#endif

#include <unicode/unistr.h>
#include <zend_types.h>

using icu::UnicodeString;

int intl_stringFromChar(UnicodeString &ret, char *str, size_t str_len, UErrorCode *status);

zend_string* intl_charFromString(const UnicodeString &from, UErrorCode *status);

#endif /* INTL_CONVERTCPP_H */
