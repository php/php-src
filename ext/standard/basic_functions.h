/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _BASIC_FUNCTIONS_H
#define _BASIC_FUNCTIONS_H

#include "zend_highlight.h"

extern php3_module_entry basic_functions_module;
#define basic_functions_module_ptr &basic_functions_module

extern int php3_minit_basic(INIT_FUNC_ARGS);
extern int php3_mshutdown_basic(SHUTDOWN_FUNC_ARGS);
extern int php3_rinit_basic(INIT_FUNC_ARGS);
extern int php3_rshutdown_basic(SHUTDOWN_FUNC_ARGS);
extern void int_value(INTERNAL_FUNCTION_PARAMETERS);
extern void double_value(INTERNAL_FUNCTION_PARAMETERS);
extern void string_value(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_toggle_short_open_tag(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sleep(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_usleep(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_key_sort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_asort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_arsort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rsort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_user_sort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_auser_sort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_user_key_sort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_array_walk(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_count(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_flush(INTERNAL_FUNCTION_PARAMETERS);
extern void array_end(INTERNAL_FUNCTION_PARAMETERS);
extern void array_prev(INTERNAL_FUNCTION_PARAMETERS);
extern void array_next(INTERNAL_FUNCTION_PARAMETERS);
extern void array_each(INTERNAL_FUNCTION_PARAMETERS);
extern void array_reset(INTERNAL_FUNCTION_PARAMETERS);
extern void array_current(INTERNAL_FUNCTION_PARAMETERS);
extern void array_current_key(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_gettype(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_settype(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_min(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_max(INTERNAL_FUNCTION_PARAMETERS);

/* system functions */
extern void php3_getenv(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_putenv(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_error_reporting(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_get_current_user(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_set_time_limit(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_get_cfg_var(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_set_magic_quotes_runtime(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_get_magic_quotes_runtime(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_get_magic_quotes_gpc(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_is_type(INTERNAL_FUNCTION_PARAMETERS, int type);
extern void php3_is_long(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_is_double(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_is_string(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_is_array(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_is_object(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_leak(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_error_log(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_call_user_func(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_call_user_method(INTERNAL_FUNCTION_PARAMETERS);

PHP_FUNCTION(register_shutdown_function);
PHP_FUNCTION(highlight_file);
PHP_FUNCTION(highlight_string);
ZEND_API void php_get_highlight_struct(zend_syntax_highlighter_ini *syntax_highlighter_ini);

PHP_FUNCTION(ob_start);
PHP_FUNCTION(ob_end_flush);
PHP_FUNCTION(ob_end_clean);
PHP_FUNCTION(ob_get_contents);

PHP_FUNCTION(ini_get);
PHP_FUNCTION(ini_alter);
PHP_FUNCTION(ini_restore);

PHP_FUNCTION(print_r);

PHP_FUNCTION(define);
PHP_FUNCTION(defined);

#if HAVE_PUTENV
typedef struct {
	char *putenv_string;
	char *previous_value;
	char *key;
	int key_len;
} putenv_entry;
#endif

#endif /* _BASIC_FUNCTIONS_H */
