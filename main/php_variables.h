/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_VARIABLES_H
#define PHP_VARIABLES_H

#include "php.h"
#include "SAPI.h"

#define PARSE_POST 0
#define PARSE_GET 1
#define PARSE_COOKIE 2
#define PARSE_STRING 3

void php_treat_data(int arg, char *str, zval* destArray ELS_DC PLS_DC SLS_DC);
PHPAPI void php_import_environment_variables(zval *array_ptr ELS_DC PLS_DC);
PHPAPI void php_register_variable(char *var, char *val, pval *track_vars_array ELS_DC PLS_DC);
/* binary-safe version */
PHPAPI void php_register_variable_safe(char *var, char *val, int val_len, pval *track_vars_array ELS_DC PLS_DC);
PHPAPI void php_register_variable_ex(char *var, zval *val, pval *track_vars_array ELS_DC PLS_DC);


#endif /* PHP_VARIABLES_H */
