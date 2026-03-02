/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Calvin Buckley <calvin@cmpct.info>                          |
   +----------------------------------------------------------------------+
*/

#include "php.h"

PHPAPI bool php_odbc_connstr_is_quoted(const char *str);
PHPAPI bool php_odbc_connstr_should_quote(const char *str);
PHPAPI size_t php_odbc_connstr_estimate_quote_length(const char *in_str);
PHPAPI size_t php_odbc_connstr_quote(char *out_str, const char *in_str, size_t out_str_size);
