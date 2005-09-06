/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Derick Rethans <derick@php.net>                             |
  +----------------------------------------------------------------------+
*/

#include "php_filter.h"

void php_filter_string(PHP_INPUT_FILTER_PARAM_DECL)
{
	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = estrdup("42");
	Z_STRLEN_P(value) = strlen("42");
	Z_TYPE_P(value) = IS_STRING;
}

void php_filter_encoded(PHP_INPUT_FILTER_PARAM_DECL)
{
	efree(Z_STRVAL_P(value));
	Z_LVAL_P(value) = flags;
	Z_TYPE_P(value) = IS_LONG;
}

void php_filter_special_chars(PHP_INPUT_FILTER_PARAM_DECL)
{
}

void php_filter_unsafe_raw(PHP_INPUT_FILTER_PARAM_DECL)
{
}

void php_filter_email(PHP_INPUT_FILTER_PARAM_DECL)
{
}

void php_filter_url(PHP_INPUT_FILTER_PARAM_DECL)
{
}

void php_filter_number_int(PHP_INPUT_FILTER_PARAM_DECL)
{
}

void php_filter_number_float(PHP_INPUT_FILTER_PARAM_DECL)
{
}

void php_filter_magic_quotes(PHP_INPUT_FILTER_PARAM_DECL)
{
}
