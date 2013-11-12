/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <ctype.h>
#include "zend_alloc.h"
#include "phpdbg_utils.h"

int phpdbg_is_numeric(const char *str) /* {{{ */
{
	for (; *str; str++) {
		if (isspace(*str)) {
			continue;
		}
		return isdigit(*str);
	}
	return 0;
} /* }}} */

int phpdbg_is_empty(const char *str) /* {{{ */
{
	for (; *str; str++) {
		if (isspace(*str)) {
			continue;
		}
		return 0;
	}
	return 1;
} /* }}} */

int phpdbg_is_addr(const char *str) /* {{{ */
{
	return str[0] && str[1] && memcmp(str, "0x", 2) == 0;
} /* }}} */

int phpdbg_is_class_method(const char *str, size_t len, char **class, char **method) /* {{{ */
{
	const char *sep = strstr(str, "::");
	size_t class_len, method_len;

	if (!sep) {
		return 0;
	}

	class_len = sep - str;
	method_len = len - ((sep+2) - str);

	*class = estrndup(str, class_len);
	class[class_len] = 0;

	*method = estrndup(sep+2, method_len+1);

	return 1;
} /* }}} */
