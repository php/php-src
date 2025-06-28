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
   | Authors: Calvin Buckley <calvin@cmpct.info>                          |
   +----------------------------------------------------------------------+
*/

#include "php.h"

PHPAPI bool php_odbc_connstr_is_quoted(const char *str);
PHPAPI bool php_odbc_connstr_should_quote(const char *str);
PHPAPI size_t php_odbc_connstr_estimate_quote_length(const char *in_str);
PHPAPI size_t php_odbc_connstr_quote(char *out_str, const char *in_str, size_t out_str_size);
