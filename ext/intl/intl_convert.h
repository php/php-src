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
   +----------------------------------------------------------------------+
 */

#ifndef INTL_CONVERT_H
#define INTL_CONVERT_H

#include <unicode/ustring.h>

void intl_convert_utf8_to_utf16(
	UChar**     target, int32_t* target_len,
	const char* src,    size_t  src_len,
	UErrorCode* status );

zend_string *intl_convert_utf8_to_utf16_zstr(
	const char* src,    size_t  src_len,
	UErrorCode* status );

zend_string* intl_convert_utf16_to_utf8(
	const UChar* src,    int32_t  src_len,
	UErrorCode*  status );

#endif // INTL_CONVERT_H
