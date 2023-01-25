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
 */

#include "php.h"

PHPAPI int php_exec(int type, const char *cmd, zval *array, zval *return_value) /* {{{ */
{
	return -1;
}
/* }}} */

PHPAPI void php_shell_exec_ex(INTERNAL_FUNCTION_PARAMETERS) /* {{{ */
{
	RETURN_FALSE;
}
/* }}} */
