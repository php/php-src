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
   | Author: Hartmut Holzgraefe <hholzgra@php.net>                        |
   +----------------------------------------------------------------------+
 */

#include "php.h"

/* {{{ Calculate Levenshtein distance between two strings */
PHP_FUNCTION(levenshtein)
{
	zend_string *string1, *string2;
	zend_long cost_ins = 1;
	zend_long cost_rep = 1;
	zend_long cost_del = 1;

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_STR(string1)
		Z_PARAM_STR(string2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(cost_ins)
		Z_PARAM_LONG(cost_rep)
		Z_PARAM_LONG(cost_del)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(zend_levenshtein(ZSTR_VAL(string1), ZSTR_LEN(string1), ZSTR_VAL(string2), ZSTR_LEN(string2), cost_ins, cost_rep, cost_del));
}
/* }}} */
