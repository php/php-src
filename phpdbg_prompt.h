/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_PROMPT_H
#define PHPDBG_PROMPT_H

/**
 * Maximum command length
 */
#define PHPDBG_MAX_CMD 500

void phpdbg_init(char *init_file, size_t init_file_len, zend_bool use_default TSRMLS_DC);
int phpdbg_interactive(TSRMLS_D);
void phpdbg_print_opline(zend_execute_data *execute_data, zend_bool ignore_flags TSRMLS_DC);
int phpdbg_compile(TSRMLS_D);
void phpdbg_clean(zend_bool full TSRMLS_DC);

#if PHP_VERSION_ID >= 50500
void phpdbg_execute_ex(zend_execute_data *execute_data TSRMLS_DC);
#else
void phpdbg_execute_ex(zend_op_array *op_array TSRMLS_DC);
#endif

#endif /* PHPDBG_PROMPT_H */
