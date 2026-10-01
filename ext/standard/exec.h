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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

#ifndef EXEC_H
#define EXEC_H

PHP_MINIT_FUNCTION(proc_open);
PHP_MINIT_FUNCTION(exec);

PHPAPI zend_string *php_escape_shell_cmd(const zend_string *unescaped_cmd);
PHPAPI zend_string *php_escape_shell_arg(const zend_string *unescaped_arg);
PHPAPI int php_exec(int type, const char *cmd, zval *array, zval *return_value);

#endif /* EXEC_H */
