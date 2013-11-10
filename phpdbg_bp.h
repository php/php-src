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

#ifndef PHPDBG_BP_H
#define PHPDBG_BP_H

/**
 * Breakpoint file-based representation
 */
typedef struct _phpdbg_breakfile_t {
	const char *filename;
	long line;
	int id;
} phpdbg_breakfile_t;

/**
 * Breakpoint symbol-based representation
 */
typedef struct _phpdbg_breaksymbol_t {
	const char *symbol;
	int id;
} phpdbg_breaksymbol_t;

void phpdbg_set_breakpoint_file(const char*, long TSRMLS_DC);
void phpdbg_set_breakpoint_symbol(const char* TSRMLS_DC);

int phpdbg_find_breakpoint_file(zend_op_array* TSRMLS_DC);
int phpdbg_find_breakpoint_symbol(zend_function* TSRMLS_DC);

#endif /* PHPDBG_BP_H */
