/* 
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrea Faulds <ajf@ajf.me>                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_OPERATORS_H
#define PHP_OPERATORS_H

PHP_NAMED_FUNCTION(php_operator_add);
PHP_NAMED_FUNCTION(php_operator_sub);
PHP_NAMED_FUNCTION(php_operator_mul);
PHP_NAMED_FUNCTION(php_operator_div);
PHP_NAMED_FUNCTION(php_operator_mod);
PHP_NAMED_FUNCTION(php_operator_pow);
PHP_NAMED_FUNCTION(php_operator_bitwise_and);
PHP_NAMED_FUNCTION(php_operator_bitwise_or);
PHP_NAMED_FUNCTION(php_operator_bitwise_xor);
PHP_NAMED_FUNCTION(php_operator_bitwise_not);
PHP_NAMED_FUNCTION(php_operator_shift_left);
PHP_NAMED_FUNCTION(php_operator_shift_right);
PHP_NAMED_FUNCTION(php_operator_equal);
PHP_NAMED_FUNCTION(php_operator_identical);
PHP_NAMED_FUNCTION(php_operator_not_equal);
PHP_NAMED_FUNCTION(php_operator_not_identical);
PHP_NAMED_FUNCTION(php_operator_less_than);
PHP_NAMED_FUNCTION(php_operator_greater_than);
PHP_NAMED_FUNCTION(php_operator_less_than_or_equal_to);
PHP_NAMED_FUNCTION(php_operator_greater_than_or_equal_to);
PHP_NAMED_FUNCTION(php_operator_spaceship);
PHP_NAMED_FUNCTION(php_operator_boolean_and);
PHP_NAMED_FUNCTION(php_operator_boolean_or);
PHP_NAMED_FUNCTION(php_operator_boolean_xor);
PHP_NAMED_FUNCTION(php_operator_boolean_not);
PHP_NAMED_FUNCTION(php_operator_concat);

#endif /* PHP_OPERATORS_H */
