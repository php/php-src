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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_TYPE_H
#define PHP_TYPE_H

PHP_FUNCTION(intval);
PHP_FUNCTION(floatval);
PHP_FUNCTION(strval);
PHP_FUNCTION(gettype);
PHP_FUNCTION(settype);
PHP_FUNCTION(is_null);
PHP_FUNCTION(is_resource);
PHP_FUNCTION(is_bool);
PHP_FUNCTION(is_long);
PHP_FUNCTION(is_float);
PHP_FUNCTION(is_numeric);
PHP_FUNCTION(is_string);
PHP_FUNCTION(is_array);
PHP_FUNCTION(is_object);
PHP_FUNCTION(is_scalar);
PHP_FUNCTION(is_callable);

#endif
