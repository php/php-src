/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
PHP_FUNCTION(proc_get_status);
PHP_FUNCTION(proc_close);
PHP_FUNCTION(proc_terminate);
PHP_FUNCTION(proc_nice);
PHP_MINIT_FUNCTION(proc_open);

char *php_escape_shell_cmd(char *);
char *php_escape_shell_arg(char *);
int php_exec(int type, char *cmd, pval *array, pval *return_value TSRMLS_DC);

#endif /* EXEC_H */
