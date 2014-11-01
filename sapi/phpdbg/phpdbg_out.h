/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

#ifdef ZTS
PHPDBG_API int phpdbg_print(int severity TSRMLS_DC, int fd, const char *tag, const char *xmlfmt, const char *strfmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 6, 7);
PHPDBG_API int phpdbg_xml_internal(int fd TSRMLS_DC, const char *fmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 3, 4);
PHPDBG_API int phpdbg_log_internal(int fd TSRMLS_DC, const char *fmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 3, 4);
PHPDBG_API int phpdbg_out_internal(int fd TSRMLS_DC, const char *fmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 3, 4);
PHPDBG_API int phpdbg_rlog_internal(int fd TSRMLS_DC, const char *fmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 3, 4);
#else
PHPDBG_API int phpdbg_print(int severity, int fd, const char *tag, const char *xmlfmt, const char *strfmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 5, 6);
PHPDBG_API int phpdbg_xml_internal(int fd, const char *fmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 2, 3);
PHPDBG_API int phpdbg_log_internal(int fd, const char *fmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 2, 3);
PHPDBG_API int phpdbg_out_internal(int fd, const char *fmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 2, 3);
PHPDBG_API int phpdbg_rlog_internal(int fd, const char *fmt, ...) PHP_ATTRIBUTE_FORMAT(printf, 2, 3);
#endif


#define phpdbg_error(tag, xmlfmt, strfmt, ...)              phpdbg_print(P_ERROR   TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDOUT].fd, tag,  xmlfmt, strfmt, ##__VA_ARGS__)
#define phpdbg_notice(tag, xmlfmt, strfmt, ...)             phpdbg_print(P_NOTICE  TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDOUT].fd, tag,  xmlfmt, strfmt, ##__VA_ARGS__)
#define phpdbg_writeln(tag, xmlfmt, strfmt, ...)            phpdbg_print(P_WRITELN TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDOUT].fd, tag,  xmlfmt, strfmt, ##__VA_ARGS__)
#define phpdbg_write(tag, xmlfmt, strfmt, ...)              phpdbg_print(P_WRITE   TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDOUT].fd, tag,  xmlfmt, strfmt, ##__VA_ARGS__)
#define phpdbg_script(type, fmt, ...)                       phpdbg_print(type      TSRMLS_CC, PHPDBG_G(io)[PHPDBG_STDOUT].fd, NULL, NULL,   fmt,    ##__VA_ARGS__)
#define phpdbg_log(fmt, ...) phpdbg_log_internal(PHPDBG_G(io)[PHPDBG_STDOUT].fd TSRMLS_CC, fmt, ##__VA_ARGS__)
#define phpdbg_xml(fmt, ...) phpdbg_xml_internal(PHPDBG_G(io)[PHPDBG_STDOUT].fd TSRMLS_CC, fmt, ##__VA_ARGS__)
#define phpdbg_out(fmt, ...) phpdbg_out_internal(PHPDBG_G(io)[PHPDBG_STDOUT].fd TSRMLS_CC, fmt, ##__VA_ARGS__)

#define phpdbg_error_ex(out, tag, xmlfmt, strfmt, ...)      phpdbg_print(P_ERROR   TSRMLS_CC, out, tag,  xmlfmt, strfmt, ##__VA_ARGS__)
#define phpdbg_notice_ex(out, tag, xmlfmt, strfmt, ...)     phpdbg_print(P_NOTICE  TSRMLS_CC, out, tag,  xmlfmt, strfmt, ##__VA_ARGS__)
#define phpdbg_writeln_ex(out, tag, xmlfmt, strfmt, ...)    phpdbg_print(P_WRITELN TSRMLS_CC, out, tag,  xmlfmt, strfmt, ##__VA_ARGS__)
#define phpdbg_write_ex(out, tag, xmlfmt, strfmt, ...)      phpdbg_print(P_WRITE   TSRMLS_CC, out, tag,  xmlfmt, strfmt, ##__VA_ARGS__)
#define phpdbg_script_ex(out, type, fmt, ...)               phpdbg_print(type      TSRMLS_CC, out, NULL, NULL,   fmt,    ##__VA_ARGS__)
#define phpdbg_log_ex(out, fmt, ...) phpdbg_log_internal(PHPDBG_G(io)[PHPDBG_STDOUT].fd TSRMLS_CC, fmt, ##__VA_ARGS__)
#define phpdbg_xml_ex(out, fmt, ...) phpdbg_xml_internal(PHPDBG_G(io)[PHPDBG_STDOUT].fd TSRMLS_CC, fmt, ##__VA_ARGS__)
#define phpdbg_out_ex(out, fmt, ...) phpdbg_out_internal(PHPDBG_G(io)[PHPDBG_STDOUT].fd TSRMLS_CC, fmt, ##__VA_ARGS__)

#define phpdbg_rlog(fd, fmt, ...) phpdbg_rlog_internal(fd TSRMLS_CC, fmt, ##__VA_ARGS__)

#define phpdbg_xml_asprintf(buf, ...) _phpdbg_xml_asprintf(buf TSRMLS_CC, ##__VA_ARGS__)
PHPDBG_API int _phpdbg_xml_asprintf(char **buf TSRMLS_DC, const char *format, zend_bool escape_xml, ...);

#define phpdbg_asprintf(buf, ...) _phpdbg_asprintf(buf TSRMLS_CC, ##__VA_ARGS__)
PHPDBG_API int _phpdbg_asprintf(char **buf TSRMLS_DC, const char *format, ...);


#if PHPDBG_DEBUG
#	define phpdbg_debug(fmt, ...) phpdbg_log_ex(PHPDBG_G(io)[PHPDBG_STDERR].fd TSRMLS_CC, fmt, ##__VA_ARGS__)
#else
#	define phpdbg_debug(fmt, ...)
#endif

PHPDBG_API void phpdbg_free_err_buf(TSRMLS_D);
PHPDBG_API void phpdbg_activate_err_buf(zend_bool active TSRMLS_DC);
PHPDBG_API int phpdbg_output_err_buf(const char *tag, const char *xmlfmt, const char *strfmt TSRMLS_DC, ...);


/* {{{ For separation */
#define SEPARATE "------------------------------------------------" /* }}} */

#endif /* PHPDBG_OUT_H */
