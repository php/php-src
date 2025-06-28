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
   | Author: Calvin Buckley <calvin@cmpct.info>                           |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_odbc_utils.h"

/*
 * Utility functions for dealing with ODBC connection strings and other common
 * functionality.
 *
 * While useful for PDO_ODBC too, this lives in ext/odbc because there isn't a
 * better place for it.
 */

PHP_FUNCTION(odbc_connection_string_is_quoted)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	bool is_quoted = php_odbc_connstr_is_quoted(ZSTR_VAL(str));

	RETURN_BOOL(is_quoted);
}

PHP_FUNCTION(odbc_connection_string_should_quote)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	bool should_quote = php_odbc_connstr_should_quote(ZSTR_VAL(str));

	RETURN_BOOL(should_quote);
}

PHP_FUNCTION(odbc_connection_string_quote)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	size_t new_size = php_odbc_connstr_estimate_quote_length(ZSTR_VAL(str));
	zend_string *new_string = zend_string_alloc(new_size, 0);
	php_odbc_connstr_quote(ZSTR_VAL(new_string), ZSTR_VAL(str), new_size);
	/* reset length */
	ZSTR_LEN(new_string) = strlen(ZSTR_VAL(new_string));
	RETURN_STR(new_string);
}
