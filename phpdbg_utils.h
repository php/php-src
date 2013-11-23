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

/**
 * Input scan functions
 */
int phpdbg_is_numeric(const char*);
int phpdbg_is_empty(const char*);
int phpdbg_is_addr(const char*);
int phpdbg_is_class_method(const char*, size_t, char**, char**);
const char *phpdbg_current_file(TSRMLS_D);
char *phpdbg_resolve_path(const char* TSRMLS_DC);
char *phpdbg_trim(const char*, size_t, size_t*);

/**
 * Error/notice/formatting helper
 */
enum {
	P_ERROR  = 1,
	P_NOTICE,
	P_WRITELN,
	P_WRITE,
	P_LOG
};

#ifdef ZTS
int phpdbg_print(int TSRMLS_DC, FILE*, const char*, ...) PHP_ATTRIBUTE_FORMAT(printf, 4, 5);
#else
int phpdbg_print(int TSRMLS_DC, FILE*, const char*, ...) PHP_ATTRIBUTE_FORMAT(printf, 3, 4);
#endif

#define phpdbg_error(fmt, ...)              phpdbg_print(P_ERROR   TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDOUT], fmt, ##__VA_ARGS__)
#define phpdbg_notice(fmt, ...)             phpdbg_print(P_NOTICE  TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDOUT], fmt, ##__VA_ARGS__)
#define phpdbg_writeln(fmt, ...)            phpdbg_print(P_WRITELN TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDOUT], fmt, ##__VA_ARGS__)
#define phpdbg_write(fmt, ...)              phpdbg_print(P_WRITE   TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDOUT], fmt, ##__VA_ARGS__)
#define phpdbg_log(fmt, ...)                phpdbg_print(P_LOG     TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDOUT], fmt, ##__VA_ARGS__)

#define phpdbg_error_ex(out, fmt, ...)      phpdbg_print(P_ERROR   TSRMLS_CC, out, fmt, ##__VA_ARGS__)
#define phpdbg_notice_ex(out, fmt, ...)     phpdbg_print(P_NOTICE  TSRMLS_CC, out, fmt, ##__VA_ARGS__)
#define phpdbg_writeln_ex(out, fmt, ...)    phpdbg_print(P_WRITELN TSRMLS_CC, out, fmt, ##__VA_ARGS__)
#define phpdbg_write_ex(out, fmt, ...)      phpdbg_print(P_WRITE   TSRMLS_CC, out, fmt, ##__VA_ARGS__)
#define phpdbg_log_ex(out, fmt, ...)        phpdbg_print(P_LOG     TSRMLS_CC, out, fmt, ##__VA_ARGS__)

#if PHPDBG_DEBUG
#   define phpdbg_debug(fmt, ...)           phpdbg_print(P_LOG   TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDERR], fmt, ##__VA_ARGS__)
#else
#   define phpdbg_debug(fmt, ...)
#endif

/* {{{ For writing blank lines */
#define EMPTY "" /* }}} */

/* {{{ For prompt lines */
#define PROMPT ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED) ? "\033[1;64mphpdbg>\033[0m " : "phpdbg> ") /* }}} */

/* {{{ For separation */
#define SEPARATE "------------------------------------------------" /* }}} */

#endif /* PHPDBG_UTILS_H */
