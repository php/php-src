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
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_OUT_H
#define PHPDBG_OUT_H

/**
 * Error/notice/formatting helpers
 */
enum {
	P_ERROR  = 1,
	P_NOTICE,
	P_WRITELN,
	P_WRITE,
	P_STDOUT,
	P_STDERR,
	P_LOG
};

PHPDBG_API int phpdbg_print(int severity, int fd, const char *strfmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 3, 4);
PHPDBG_API int phpdbg_log_internal(int fd, const char *fmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 2, 3);
PHPDBG_API int phpdbg_out_internal(int fd, const char *fmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 2, 3);

#define phpdbg_error(strfmt, ...)              phpdbg_print(P_ERROR  , PHPDBG_G(io)[PHPDBG_STDOUT].fd, strfmt, ##__VA_ARGS__)
#define phpdbg_notice(strfmt, ...)             phpdbg_print(P_NOTICE , PHPDBG_G(io)[PHPDBG_STDOUT].fd, strfmt, ##__VA_ARGS__)
#define phpdbg_writeln(strfmt, ...)            phpdbg_print(P_WRITELN, PHPDBG_G(io)[PHPDBG_STDOUT].fd, strfmt, ##__VA_ARGS__)
#define phpdbg_write(strfmt, ...)              phpdbg_print(P_WRITE  , PHPDBG_G(io)[PHPDBG_STDOUT].fd, strfmt, ##__VA_ARGS__)

#define phpdbg_log(fmt, ...)                   phpdbg_log_internal(PHPDBG_G(io)[PHPDBG_STDOUT].fd, fmt, ##__VA_ARGS__)
#define phpdbg_out(fmt, ...)                   phpdbg_out_internal(PHPDBG_G(io)[PHPDBG_STDOUT].fd, fmt, ##__VA_ARGS__)

#define phpdbg_script(type, strfmt, ...)       phpdbg_print(type,      PHPDBG_G(io)[PHPDBG_STDOUT].fd, strfmt, ##__VA_ARGS__)

#define phpdbg_asprintf(buf, ...) _phpdbg_asprintf(buf, ##__VA_ARGS__)
PHPDBG_API int _phpdbg_asprintf(char **buf, const char *format, ...);

#if PHPDBG_DEBUG
#	define phpdbg_debug(strfmt, ...) phpdbg_log_internal(PHPDBG_G(io)[PHPDBG_STDERR].fd, strfmt, ##__VA_ARGS__)
#else
#	define phpdbg_debug(strfmt, ...)
#endif

PHPDBG_API void phpdbg_free_err_buf(void);
PHPDBG_API void phpdbg_activate_err_buf(bool active);
PHPDBG_API int phpdbg_output_err_buf(const char *strfmt, ...);


/* {{{ For separation */
#define SEPARATE "------------------------------------------------" /* }}} */

#endif /* PHPDBG_OUT_H */
