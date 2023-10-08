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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/

#include "intl_cppshims.h"

#include "intl_convertcpp.h"
#include <unicode/ustring.h>
extern "C" {
#include <php.h>
}

/* {{{ intl_stringFromChar */
int intl_stringFromChar(UnicodeString &ret, char *str, size_t str_len, UErrorCode *status)
{
	if(UNEXPECTED(str_len > INT32_MAX)) {
		*status = U_BUFFER_OVERFLOW_ERROR;
		ret.setToBogus();
		return FAILURE;
	}
	//the number of UTF-16 code units is not larger than that of UTF-8 code
	//units, + 1 for the terminator
	int32_t capacity = (int32_t)str_len + 1;

	//no check necessary -- if NULL will fail ahead
	UChar	*utf16 = ret.getBuffer(capacity);
	int32_t utf16_len = 0;
	*status = U_ZERO_ERROR;
	u_strFromUTF8WithSub(utf16, ret.getCapacity(), &utf16_len,
		str, str_len, U_SENTINEL /* no substitution */, NULL,
		status);
	ret.releaseBuffer(utf16_len);
	if (U_FAILURE(*status)) {
		ret.setToBogus();
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ intl_charFromString
 * faster than doing intl_convert_utf16_to_utf8(
 *		from.getBuffer(), from.length(), &status),
 * but consumes more memory */
zend_string* intl_charFromString(const UnicodeString &from, UErrorCode *status)
{
	zend_string *u8res;

	if (UNEXPECTED(from.isBogus())) {
		return NULL;
	}

	//the number of UTF-8 code units is not larger than that of UTF-16 code
	//units * 3
	int32_t capacity = from.length() * 3;

	if (from.isEmpty()) {
		return ZSTR_EMPTY_ALLOC();
	}

	u8res = zend_string_alloc(capacity, 0);

	const UChar *utf16buf = from.getBuffer();
	int32_t actual_len;
	u_strToUTF8WithSub(ZSTR_VAL(u8res), capacity, &actual_len, utf16buf, from.length(),
		U_SENTINEL, NULL, status);

	if (U_FAILURE(*status)) {
		zend_string_free(u8res);
		return NULL;
	}
	u8res = zend_string_truncate(u8res, actual_len, 0);

	return u8res;
}
/* }}} */
