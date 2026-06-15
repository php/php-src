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
*/

/* Str — internal backing class for scalar string methods.
 *
 * Each method is static and takes the subject string as its first argument,
 * returning a new value (immutable). This class is the dispatch target for
 * the scalar-objects compiler feature; userland should not instantiate it.
 */

#include "php.h"
#include "php_string.h"
#include "str_arginfo.h"

zend_class_entry *str_ce;

/* {{{ Str::trim(string $string, string $characters = " \n\r\t\v\0"): string */
PHP_METHOD(Str, trim)
{
	zend_string *str;
	zend_string *what = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(what)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_STR(php_trim(str,
		what ? ZSTR_VAL(what) : NULL,
		what ? ZSTR_LEN(what) : 0,
		3 /* both ends */));
}
/* }}} */

/* {{{ Str::upper(string $string): string */
PHP_METHOD(Str, upper)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_STR(zend_string_toupper(str));
}
/* }}} */

/* {{{ Str::lower(string $string): string */
PHP_METHOD(Str, lower)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_STR(zend_string_tolower(str));
}
/* }}} */

/* {{{ Str::length(string $string): int */
PHP_METHOD(Str, length)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG((zend_long) ZSTR_LEN(str));
}
/* }}} */

PHP_MINIT_FUNCTION(str) /* {{{ */
{
	str_ce = register_class_Str();
	return SUCCESS;
}
/* }}} */
