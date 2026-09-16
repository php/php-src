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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_VARIABLES_H
#define PHP_VARIABLES_H

#include "php.h"
#include "SAPI.h"

#define PARSE_POST 0
#define PARSE_GET 1
#define PARSE_COOKIE 2
#define PARSE_STRING 3
#define PARSE_ENV 4
#define PARSE_SERVER 5

BEGIN_EXTERN_C()
void php_startup_auto_globals(void);
extern PHPAPI void (*php_import_environment_variables)(zval *array_ptr);
extern PHPAPI void (*php_load_environment_variables)(zval *array_ptr);
PHPAPI void php_register_variable(const char *var, const char *val, zval *track_vars_array);
/* binary-safe version */
PHPAPI void php_register_variable_safe(const char *var, const char *val, size_t val_len, zval *track_vars_array);
PHPAPI void php_register_variable_ex(const char *var, zval *val, zval *track_vars_array);
PHPAPI void php_register_known_variable(const char *var, size_t var_len, zval *value, zval *track_vars_array);

PHPAPI void php_build_argv(const char *s, zval *track_vars_array);
PHPAPI zend_result php_hash_environment(void);
END_EXTERN_C()

#define NUM_TRACK_VARS	6

#endif /* PHP_VARIABLES_H */
