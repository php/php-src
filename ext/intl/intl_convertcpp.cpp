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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "intl_cppshims.h"

#include "intl_convertcpp.h"
#include <unicode/ustring.h>
extern "C" {
#include <php.h>
}

/* {{{ intl_stringFromChar */
int intl_stringFromChar(UnicodeString &ret, char *str, size_t str_len, UErrorCode *status)
{
	if(str_len > INT32_MAX) {
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
 * faster than doing intl_convert_utf16_to_utf8(&res, &res_len,
 *		from.getBuffer(), from.length(), &status),
 * but consumes more memory */
int intl_charFromString(const UnicodeString &from, char **res, size_t *res_len, UErrorCode *status)
{
	if (from.isBogus()) {
		return FAILURE;
	}

	//the number of UTF-8 code units is not larger than that of UTF-16 code
	//units * 3 + 1 for the terminator
	int32_t capacity = from.length() * 3 + 1;

	if (from.isEmpty()) {
		*res = (char*)emalloc(1);
		**res = '\0';
		*res_len = 0;
		return SUCCESS;
	}

	*res = (char*)emalloc(capacity);
	*res_len = 0; //tbd

	const UChar *utf16buf = from.getBuffer();
	int32_t actual_len;
	u_strToUTF8WithSub(*res, capacity - 1, &actual_len, utf16buf, from.length(),
		U_SENTINEL, NULL, status);

	if (U_FAILURE(*status)) {
		efree(*res);
		*res = NULL;
		return FAILURE;
	}
	(*res)[actual_len] = '\0';
	*res_len = actual_len;

	return SUCCESS;
}
/* }}} */
