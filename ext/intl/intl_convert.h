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
   +----------------------------------------------------------------------+
 */

#ifndef INTL_CONVERT_H
#define INTL_CONVERT_H

#include <unicode/ustring.h>

void intl_convert_utf8_to_utf16(
	UChar**     target, int32_t* target_len,
	const char* src,    size_t  src_len,
	UErrorCode* status );

zend_string* intl_convert_utf16_to_utf8(
	const UChar* src,    int32_t  src_len,
	UErrorCode*  status );

#endif // INTL_CONVERT_H
