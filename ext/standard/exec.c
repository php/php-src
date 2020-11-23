/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@php.net>                              |
   |         Ilia Alshanetsky <iliaa@php.net>                             |
   +----------------------------------------------------------------------+
 */

#include <stdio.h>
#include "php.h"
#include <ctype.h>
#include "php_string.h"
#include "ext/standard/head.h"
#include "ext/standard/file.h"
#include "basic_functions.h"
#include "exec.h"
#include "php_globals.h"
#include "SAPI.h"

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#include <signal.h>

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <limits.h>

#ifdef PHP_WIN32
# include "win32/nice.h"
#endif

static size_t cmd_max_len;

/* {{{ PHP_MINIT_FUNCTION(exec) */
PHP_MINIT_FUNCTION(exec)
{
#ifdef _SC_ARG_MAX
	cmd_max_len = sysconf(_SC_ARG_MAX);
	if ((size_t)-1 == cmd_max_len) {
#ifdef _POSIX_ARG_MAX
		cmd_max_len = _POSIX_ARG_MAX;
#else
		cmd_max_len = 4096;
#endif
	}
#elif defined(ARG_MAX)
	cmd_max_len = ARG_MAX;
#elif defined(PHP_WIN32)
	/* Executed commands will run through cmd.exe. As long as it's the case,
		it's just the constant limit.*/
	cmd_max_len = 8192;
#else
	/* This is just an arbitrary value for the fallback case. */
	cmd_max_len = 4096;
#endif

	return SUCCESS;
}
/* }}} */

static size_t strip_trailing_whitespace(char *buf, size_t bufl) {
	size_t l = bufl;
	while (l-- > 0 && isspace(((unsigned char *)buf)[l]));
	if (l != (bufl - 1)) {
		bufl = l + 1;
		buf[bufl] = '\0';
	}
	return bufl;
}

static size_t handle_line(int type, zval *array, char *buf, size_t bufl) {
	if (type == 1) {
		PHPWRITE(buf, bufl);
		if (php_output_get_level() < 1) {
			sapi_flush();
		}
	} else if (type == 2) {
		bufl = strip_trailing_whitespace(buf, bufl);
		add_next_index_stringl(array, buf, bufl);
	}
	return bufl;
}

/* {{{ php_exec
 * If type==0, only last line of output is returned (exec)
 * If type==1, all lines will be printed and last lined returned (system)
 * If type==2, all lines will be saved to given array (exec with &$array)
 * If type==3, output will be printed binary, no lines will be saved or returned (passthru)
 *
 */
PHPAPI int php_exec(int type, const char *cmd, zval *array, zval *return_value)
{
	FILE *fp;
	char *buf;
	int pclose_return;
	char *b, *d=NULL;
	php_stream *stream;
	size_t buflen, bufl = 0;
#if PHP_SIGCHILD
	void (*sig_handler)() = NULL;
#endif

#if PHP_SIGCHILD
	sig_handler = signal (SIGCHLD, SIG_DFL);
#endif

#ifdef PHP_WIN32
	fp = VCWD_POPEN(cmd, "rb");
#else
	fp = VCWD_POPEN(cmd, "r");
#endif
	if (!fp) {
		php_error_docref(NULL, E_WARNING, "Unable to fork [%s]", cmd);
		goto err;
	}

	stream = php_stream_fopen_from_pipe(fp, "rb");

	buf = (char *) emalloc(EXEC_INPUT_BUF);
	buflen = EXEC_INPUT_BUF;

	if (type != 3) {
		b = buf;

		while (php_stream_get_line(stream, b, EXEC_INPUT_BUF, &bufl)) {
			/* no new line found, let's read some more */
			if (b[bufl - 1] != '\n' && !php_stream_eof(stream)) {
				if (buflen < (bufl + (b - buf) + EXEC_INPUT_BUF)) {
					bufl += b - buf;
					buflen = bufl + EXEC_INPUT_BUF;
					buf = erealloc(buf, buflen);
					b = buf + bufl;
				} else {
					b += bufl;
				}
				continue;
			} else if (b != buf) {
				bufl += b - buf;
			}

			bufl = handle_line(type, array, buf, bufl);
			b = buf;
		}
		if (bufl) {
			if (buf != b) {
				/* Process remaining output */
				bufl = handle_line(type, array, buf, bufl);
			}

			/* Return last line from the shell command */
			bufl = strip_trailing_whitespace(buf, bufl);
			RETVAL_STRINGL(buf, bufl);
		} else { /* should return NULL, but for BC we return "" */
			RETVAL_EMPTY_STRING();
		}
	} else {
		ssize_t read;
		while ((read = php_stream_read(stream, buf, EXEC_INPUT_BUF)) > 0) {
			PHPWRITE(buf, read);
		}
	}

	pclose_return = php_stream_close(stream);
	efree(buf);

done:
#if PHP_SIGCHILD
	if (sig_handler) {
		signal(SIGCHLD, sig_handler);
	}
#endif
	if (d) {
		efree(d);
	}
	return pclose_return;
err:
	pclose_return = -1;
	RETVAL_FALSE;
	goto done;
}
/* }}} */

static void php_exec_ex(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	char *cmd;
	size_t cmd_len;
	zval *ret_code=NULL, *ret_array=NULL;
	int ret;

	ZEND_PARSE_PARAMETERS_START(1, (mode ? 2 : 3))
		Z_PARAM_STRING(cmd, cmd_len)
		Z_PARAM_OPTIONAL
		if (!mode) {
			Z_PARAM_ZVAL(ret_array)
		}
		Z_PARAM_ZVAL(ret_code)
	ZEND_PARSE_PARAMETERS_END();

	if (!cmd_len) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}
	if (strlen(cmd) != cmd_len) {
		zend_argument_value_error(1, "must not contain any null bytes");
		RETURN_THROWS();
	}

	if (!ret_array) {
		ret = php_exec(mode, cmd, NULL, return_value);
	} else {
		if (Z_TYPE_P(Z_REFVAL_P(ret_array)) == IS_ARRAY) {
			ZVAL_DEREF(ret_array);
			SEPARATE_ARRAY(ret_array);
		} else {
			ret_array = zend_try_array_init(ret_array);
			if (!ret_array) {
				RETURN_THROWS();
			}
		}

		ret = php_exec(2, cmd, ret_array, return_value);
	}
	if (ret_code) {
		ZEND_TRY_ASSIGN_REF_LONG(ret_code, ret);
	}
}
/* }}} */

/* {{{ Execute an external program */
PHP_FUNCTION(exec)
{
	php_exec_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Execute an external program and display output */
PHP_FUNCTION(system)
{
	php_exec_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Execute an external program and display raw output */
PHP_FUNCTION(passthru)
{
	php_exec_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ php_escape_shell_cmd
   Escape all chars that could possibly be used to
   break out of a shell command

   This function emalloc's a string and returns the pointer.
   Remember to efree it when done with it.

   *NOT* safe for binary strings
*/
PHPAPI zend_string *php_escape_shell_cmd(const char *str)
{
	register size_t x, y;
	size_t l = strlen(str);
	uint64_t estimate = (2 * (uint64_t)l) + 1;
	zend_string *cmd;
#ifndef PHP_WIN32
	char *p = NULL;
#endif

	/* max command line length - two single quotes - \0 byte length */
	if (l > cmd_max_len - 2 - 1) {
		php_error_docref(NULL, E_ERROR, "Command exceeds the allowed length of %zu bytes", cmd_max_len);
		return ZSTR_EMPTY_ALLOC();
	}

	cmd = zend_string_safe_alloc(2, l, 0, 0);

	for (x = 0, y = 0; x < l; x++) {
		int mb_len = php_mblen(str + x, (l - x));

		/* skip non-valid multibyte characters */
		if (mb_len < 0) {
			continue;
		} else if (mb_len > 1) {
			memcpy(ZSTR_VAL(cmd) + y, str + x, mb_len);
			y += mb_len;
			x += mb_len - 1;
			continue;
		}

		switch (str[x]) {
#ifndef PHP_WIN32
			case '"':
			case '\'':
				if (!p && (p = memchr(str + x + 1, str[x], l - x - 1))) {
					/* noop */
				} else if (p && *p == str[x]) {
					p = NULL;
				} else {
					ZSTR_VAL(cmd)[y++] = '\\';
				}
				ZSTR_VAL(cmd)[y++] = str[x];
				break;
#else
			/* % is Windows specific for environmental variables, ^%PATH% will
				output PATH while ^%PATH^% will not. escapeshellcmd->val will escape all % and !.
			*/
			case '%':
			case '!':
			case '"':
			case '\'':
#endif
			case '#': /* This is character-set independent */
			case '&':
			case ';':
			case '`':
			case '|':
			case '*':
			case '?':
			case '~':
			case '<':
			case '>':
			case '^':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '$':
			case '\\':
			case '\x0A': /* excluding these two */
			case '\xFF':
#ifdef PHP_WIN32
				ZSTR_VAL(cmd)[y++] = '^';
#else
				ZSTR_VAL(cmd)[y++] = '\\';
#endif
				/* fall-through */
			default:
				ZSTR_VAL(cmd)[y++] = str[x];

		}
	}
	ZSTR_VAL(cmd)[y] = '\0';

	if (y > cmd_max_len + 1) {
		php_error_docref(NULL, E_ERROR, "Escaped command exceeds the allowed length of %zu bytes", cmd_max_len);
		zend_string_release_ex(cmd, 0);
		return ZSTR_EMPTY_ALLOC();
	}

	if ((estimate - y) > 4096) {
		/* realloc if the estimate was way overill
		 * Arbitrary cutoff point of 4096 */
		cmd = zend_string_truncate(cmd, y, 0);
	}

	ZSTR_LEN(cmd) = y;

	return cmd;
}
/* }}} */

/* {{{ php_escape_shell_arg */
PHPAPI zend_string *php_escape_shell_arg(const char *str)
{
	size_t x, y = 0;
	size_t l = strlen(str);
	zend_string *cmd;
	uint64_t estimate = (4 * (uint64_t)l) + 3;

	/* max command line length - two single quotes - \0 byte length */
	if (l > cmd_max_len - 2 - 1) {
		php_error_docref(NULL, E_ERROR, "Argument exceeds the allowed length of %zu bytes", cmd_max_len);
		return ZSTR_EMPTY_ALLOC();
	}

	cmd = zend_string_safe_alloc(4, l, 2, 0); /* worst case */

#ifdef PHP_WIN32
	ZSTR_VAL(cmd)[y++] = '"';
#else
	ZSTR_VAL(cmd)[y++] = '\'';
#endif

	for (x = 0; x < l; x++) {
		int mb_len = php_mblen(str + x, (l - x));

		/* skip non-valid multibyte characters */
		if (mb_len < 0) {
			continue;
		} else if (mb_len > 1) {
			memcpy(ZSTR_VAL(cmd) + y, str + x, mb_len);
			y += mb_len;
			x += mb_len - 1;
			continue;
		}

		switch (str[x]) {
#ifdef PHP_WIN32
		case '"':
		case '%':
		case '!':
			ZSTR_VAL(cmd)[y++] = ' ';
			break;
#else
		case '\'':
			ZSTR_VAL(cmd)[y++] = '\'';
			ZSTR_VAL(cmd)[y++] = '\\';
			ZSTR_VAL(cmd)[y++] = '\'';
#endif
			/* fall-through */
		default:
			ZSTR_VAL(cmd)[y++] = str[x];
		}
	}
#ifdef PHP_WIN32
	if (y > 0 && '\\' == ZSTR_VAL(cmd)[y - 1]) {
		int k = 0, n = y - 1;
		for (; n >= 0 && '\\' == ZSTR_VAL(cmd)[n]; n--, k++);
		if (k % 2) {
			ZSTR_VAL(cmd)[y++] = '\\';
		}
	}

	ZSTR_VAL(cmd)[y++] = '"';
#else
	ZSTR_VAL(cmd)[y++] = '\'';
#endif
	ZSTR_VAL(cmd)[y] = '\0';

	if (y > cmd_max_len + 1) {
		php_error_docref(NULL, E_ERROR, "Escaped argument exceeds the allowed length of %zu bytes", cmd_max_len);
		zend_string_release_ex(cmd, 0);
		return ZSTR_EMPTY_ALLOC();
	}

	if ((estimate - y) > 4096) {
		/* realloc if the estimate was way overill
		 * Arbitrary cutoff point of 4096 */
		cmd = zend_string_truncate(cmd, y, 0);
	}
	ZSTR_LEN(cmd) = y;
	return cmd;
}
/* }}} */

/* {{{ Escape shell metacharacters */
PHP_FUNCTION(escapeshellcmd)
{
	char *command;
	size_t command_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(command, command_len)
	ZEND_PARSE_PARAMETERS_END();

	if (command_len) {
		if (command_len != strlen(command)) {
			zend_argument_value_error(1, "must not contain any null bytes");
			RETURN_THROWS();
		}
		RETVAL_STR(php_escape_shell_cmd(command));
	} else {
		RETVAL_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Quote and escape an argument for use in a shell command */
PHP_FUNCTION(escapeshellarg)
{
	char *argument;
	size_t argument_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(argument, argument_len)
	ZEND_PARSE_PARAMETERS_END();

	if (argument_len != strlen(argument)) {
		zend_argument_value_error(1, "must not contain any null bytes");
		RETURN_THROWS();
	}

	RETVAL_STR(php_escape_shell_arg(argument));
}
/* }}} */

/* {{{ Execute command via shell and return complete output as string */
PHP_FUNCTION(shell_exec)
{
	FILE *in;
	char *command;
	size_t command_len;
	zend_string *ret;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(command, command_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!command_len) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}
	if (strlen(command) != command_len) {
		zend_argument_value_error(1, "must not contain any null bytes");
		RETURN_THROWS();
	}

#ifdef PHP_WIN32
	if ((in=VCWD_POPEN(command, "rt"))==NULL) {
#else
	if ((in=VCWD_POPEN(command, "r"))==NULL) {
#endif
		php_error_docref(NULL, E_WARNING, "Unable to execute '%s'", command);
		RETURN_FALSE;
	}

	stream = php_stream_fopen_from_pipe(in, "rb");
	ret = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);
	php_stream_close(stream);

	if (ret && ZSTR_LEN(ret) > 0) {
		RETVAL_STR(ret);
	}
}
/* }}} */

#ifdef HAVE_NICE
/* {{{ Change the priority of the current process */
PHP_FUNCTION(proc_nice)
{
	zend_long pri;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pri)
	ZEND_PARSE_PARAMETERS_END();

	errno = 0;
	php_ignore_value(nice(pri));
	if (errno) {
#ifdef PHP_WIN32
		char *err = php_win_err();
		php_error_docref(NULL, E_WARNING, "%s", err);
		php_win_err_free(err);
#else
		php_error_docref(NULL, E_WARNING, "Only a super user may attempt to increase the priority of a process");
#endif
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif
