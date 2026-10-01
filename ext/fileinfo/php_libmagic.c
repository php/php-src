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
  | Author: Anatol Belski <ab@php.net>                                   |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_libmagic.h"

zend_string* convert_libmagic_pattern(const char *val, size_t len, uint32_t options)
{
	size_t i, j;
	zend_string *t;

	for (i = j = 0; i < len; i++) {
		switch (val[i]) {
			case '~':
				j += 2;
				break;
			case '\0':
				j += 4;
				break;
			default:
				j++;
				break;
		}
	}
	t = zend_string_alloc(j + 4, 0);

	j = 0;
	ZSTR_VAL(t)[j++] = '~';

	for (i = 0; i < len; i++, j++) {
		switch (val[i]) {
			case '~':
				ZSTR_VAL(t)[j++] = '\\';
				ZSTR_VAL(t)[j] = '~';
				break;
			case '\0':
				ZSTR_VAL(t)[j++] = '\\';
				ZSTR_VAL(t)[j++] = 'x';
				ZSTR_VAL(t)[j++] = '0';
				ZSTR_VAL(t)[j] = '0';
				break;
			default:
				ZSTR_VAL(t)[j] = val[i];
				break;
		}
	}
	ZSTR_VAL(t)[j++] = '~';

	if (options & PCRE2_CASELESS)
		ZSTR_VAL(t)[j++] = 'i';

	if (options & PCRE2_MULTILINE)
		ZSTR_VAL(t)[j++] = 'm';

	ZSTR_VAL(t)[j]='\0';
	ZSTR_LEN(t) = j;

	return t;
}



