/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
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

#ifndef EXEC_H
#define EXEC_H

PHP_FUNCTION(system);
PHP_FUNCTION(exec);
PHP_FUNCTION(escapeshellcmd);
PHP_FUNCTION(escapeshellarg);
PHP_FUNCTION(passthru);
PHP_FUNCTION(shell_exec);
PHP_FUNCTION(proc_open);
PHP_FUNCTION(proc_close);
PHP_MINIT_FUNCTION(proc_open);

PHPAPI char *php_escape_shell_cmd(char *);
PHPAPI char *php_escape_shell_arg(char *);
int php_Exec(int type, char *cmd, pval *array, pval *return_value TSRMLS_DC);

#define PHP_EMPTY_EXEC_PARAM { php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot execute a blank command"); RETURN_FALSE; }

#endif /* EXEC_H */
