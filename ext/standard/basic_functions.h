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

PHP_MINIT_FUNCTION(basic);
PHP_MSHUTDOWN_FUNCTION(basic);
PHP_RINIT_FUNCTION(basic);
PHP_RSHUTDOWN_FUNCTION(basic);

PHP_FUNCTION(intval);
PHP_FUNCTION(doubleval);
PHP_FUNCTION(strval);
PHP_FUNCTION(toggle_short_open_tag);
PHP_FUNCTION(sleep);
PHP_FUNCTION(usleep);
PHP_FUNCTION(ksort);
PHP_FUNCTION(krsort);
PHP_FUNCTION(asort);
PHP_FUNCTION(arsort);
PHP_FUNCTION(sort);
PHP_FUNCTION(rsort);
PHP_FUNCTION(usort);
PHP_FUNCTION(uasort);
PHP_FUNCTION(uksort);
PHP_FUNCTION(array_walk);
PHP_FUNCTION(count);
PHP_FUNCTION(flush);
PHP_FUNCTION(end);
PHP_FUNCTION(prev);
PHP_FUNCTION(next);
PHP_FUNCTION(reset);
PHP_FUNCTION(current);
PHP_FUNCTION(key);
PHP_FUNCTION(gettype);
PHP_FUNCTION(settype);
PHP_FUNCTION(min);
PHP_FUNCTION(max);

/* system functions */
PHP_FUNCTION(getenv);
PHP_FUNCTION(putenv);

PHP_FUNCTION(get_current_user);
PHP_FUNCTION(set_time_limit);

PHP_FUNCTION(get_cfg_var);
PHP_FUNCTION(set_magic_quotes_runtime);
PHP_FUNCTION(get_magic_quotes_runtime);
PHP_FUNCTION(get_magic_quotes_gpc);

void php3_is_type(INTERNAL_FUNCTION_PARAMETERS, int type);
PHP_FUNCTION(is_long);
PHP_FUNCTION(is_double);
PHP_FUNCTION(is_string);
PHP_FUNCTION(is_array);
PHP_FUNCTION(is_object);

PHP_FUNCTION(error_log);

PHP_FUNCTION(call_user_func);
PHP_FUNCTION(call_user_method);

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

PHP_FUNCTION(connection_aborted);
PHP_FUNCTION(connection_timeout);
PHP_FUNCTION(connection_status);
PHP_FUNCTION(ignore_user_abort);

PHP_FUNCTION(function_exists);
PHP_FUNCTION(in_array);
PHP_FUNCTION(extract);
PHP_FUNCTION(compact);
PHP_FUNCTION(array_push);
PHP_FUNCTION(array_pop);
PHP_FUNCTION(array_shift);
PHP_FUNCTION(array_unshift);
PHP_FUNCTION(array_splice);
PHP_FUNCTION(array_slice);
PHP_FUNCTION(array_merge);
PHP_FUNCTION(array_keys);
PHP_FUNCTION(array_values);
PHP_FUNCTION(array_count_values);

#if HAVE_PUTENV
typedef struct {
	char *putenv_string;
	char *previous_value;
	char *key;
	int key_len;
} putenv_entry;
#endif

#endif /* _BASIC_FUNCTIONS_H */
