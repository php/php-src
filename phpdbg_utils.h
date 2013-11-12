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

#ifndef PHPDBG_UTILS_H
#define PHPDBG_UTILS_H

#include "TSRM.h"

/**
 * Input scan functions
 */
int phpdbg_is_numeric(const char*);
int phpdbg_is_empty(const char*);
int phpdbg_is_addr(const char*);
int phpdbg_is_class_method(const char*, size_t, char**, char**);

/**
 * Error/notice/formatting helper
 */
enum {
	ERROR  = 1,
	NOTICE,
	WRITE
};

void phpdbg_print(int TSRMLS_DC, const char*, ...);

#define phpdbg_error(fmt, ...)  phpdbg_print(ERROR  TSRMLS_CC, fmt, ##__VA_ARGS__)
#define phpdbg_notice(fmt, ...) phpdbg_print(NOTICE TSRMLS_CC, fmt, ##__VA_ARGS__)
#define phpdbg_write(fmt, ...) phpdbg_print(WRITE TSRMLS_CC, fmt, ##__VA_ARGS__)

#endif /* PHPDBG_UTILS_H */
