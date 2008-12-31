/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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
/* $Id$ */

#include <stdio.h>
#include "php.h"
#include <ctype.h>
#include "php_string.h"
#include "safe_mode.h"
#include "ext/standard/head.h"
#include "ext/standard/file.h"
#include "basic_functions.h"
#include "exec.h"
#include "php_globals.h"
#include "SAPI.h"

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_NICE && HAVE_UNISTD_H
#include <unistd.h>
#endif

/* {{{ php_exec
 * If type==0, only last line of output is returned (exec)
 * If type==1, all lines will be printed and last lined returned (system)
 * If type==2, all lines will be saved to given array (exec with &$array)
 * If type==3, output will be printed binary, no lines will be saved or returned (passthru)
 *
 */
int php_exec(int type, char *cmd, zval *array, zval *return_value TSRMLS_DC)
{
	FILE *fp;
	char *buf, *tmp=NULL;
	int l, pclose_return;
	char *cmd_p, *b, *c, *d=NULL;
	php_stream *stream;
	size_t buflen, bufl = 0;
#if PHP_SIGCHILD
	void (*sig_handler)() = NULL;
#endif

	if (PG(safe_mode)) {
		if ((c = strchr(cmd, ' '))) {
			*c = '\0';
			c++;
		}
		if (strstr(cmd, "..")) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No '..' components allowed in path");
			goto err;
		}
		b = strrchr(cmd, PHP_DIR_SEPARATOR);
		spprintf(&d, 0, "%s%s%s%s%s", PG(safe_mode_exec_dir), (b ? "" : "/"), (b ? b : cmd), (c ? " " : ""), (c ? c : ""));
		if (c) {
			*(c - 1) = ' ';
		}
		cmd_p = php_escape_shell_cmd(d);
		efree(d);
		d = cmd_p;
	} else {
		cmd_p = cmd;
	}

#if PHP_SIGCHILD
	sig_handler = signal (SIGCHLD, SIG_DFL);
#endif

#ifdef PHP_WIN32
	fp = VCWD_POPEN(cmd_p, "rb");
#else
	fp = VCWD_POPEN(cmd_p, "r");
#endif
	if (!fp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to fork [%s]", cmd);
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

			if (type == 1) {
				PHPWRITE(buf, bufl);
				sapi_flush(TSRMLS_C);
			} else if (type == 2) {
				/* strip trailing whitespaces */	
				l = bufl;
				while (l-- && isspace(((unsigned char *)buf)[l]));
				if (l != (int)(bufl - 1)) {
					bufl = l + 1;
					buf[bufl] = '\0';
				}
				add_next_index_stringl(array, buf, bufl, 1);
			}
			b = buf;
		}
		if (bufl) {
			/* strip trailing whitespaces if we have not done so already */	
			if (type != 2) {
				l = bufl;
				while (l-- && isspace(((unsigned char *)buf)[l]));
				if (l != (int)(bufl - 1)) {
					bufl = l + 1;
					buf[bufl] = '\0';
				}
			}

			/* Return last line from the shell command */
			if (PG(magic_quotes_runtime)) {
				int len;
	
				tmp = php_addslashes(buf, bufl, &len, 0 TSRMLS_CC);
				RETVAL_STRINGL(tmp, len, 0);
			} else {
				RETVAL_STRINGL(buf, bufl, 1);
			}
		} else { /* should return NULL, but for BC we return "" */
			RETVAL_EMPTY_STRING();
		}
	} else {
		while((bufl = php_stream_read(stream, buf, EXEC_INPUT_BUF)) > 0) {
			PHPWRITE(buf, bufl);
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
	goto done;
}
/* }}} */

static void php_exec_ex(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	char *cmd;
	int cmd_len;
	zval *ret_code=NULL, *ret_array=NULL;
	int ret;

	if (mode) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z/", &cmd, &cmd_len, &ret_code) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z/z/", &cmd, &cmd_len, &ret_array, &ret_code) == FAILURE) {
			RETURN_FALSE;
		}
	}
	if (!cmd_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot execute a blank command");
		RETURN_FALSE;
	}

	if (!ret_array) {
		ret = php_exec(mode, cmd, NULL, return_value TSRMLS_CC);
	} else {
		if (Z_TYPE_P(ret_array) != IS_ARRAY) {
			zval_dtor(ret_array);
			array_init(ret_array);
		}
		ret = php_exec(2, cmd, ret_array, return_value TSRMLS_CC);
	}
	if (ret_code) {
		zval_dtor(ret_code);
		ZVAL_LONG(ret_code, ret);
	}
}

/* {{{ proto string exec(string command [, array &output [, int &return_value]])
   Execute an external program */
PHP_FUNCTION(exec)
{
	php_exec_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

/* }}} */

/* {{{ proto int system(string command [, int &return_value])
   Execute an external program and display output */
PHP_FUNCTION(system)
{
	php_exec_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto void passthru(string command [, int &return_value])
   Execute an external program and display raw output */
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
char *php_escape_shell_cmd(char *str) {
	register int x, y, l;
	char *cmd;
	char *p = NULL;
	
	TSRMLS_FETCH();

	l = strlen(str);
	cmd = safe_emalloc(2, l, 1);
	
	for (x = 0, y = 0; x < l; x++) {
		int mb_len = php_mblen(str + x, (l - x));

		/* skip non-valid multibyte characters */
		if (mb_len < 0) {
			continue;
		} else if (mb_len > 1) {
			memcpy(cmd + y, str + x, mb_len);
			y += mb_len;
			x += mb_len - 1;
			continue;
		}

		switch (str[x]) {
			case '"':
			case '\'':
#ifndef PHP_WIN32
				if (!p && (p = memchr(str + x + 1, str[x], l - x - 1))) {
					/* noop */
				} else if (p && *p == str[x]) {
					p = NULL;
				} else {
					cmd[y++] = '\\';
				}
				cmd[y++] = str[x];
				break;
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
			/* since Windows does not allow us to escape these chars, just remove them */
			case '%':
				cmd[y++] = ' ';
				break;
#endif
				cmd[y++] = '\\';
				/* fall-through */
			default:
				cmd[y++] = str[x];

		}
	}
	cmd[y] = '\0';
	return cmd;
}
/* }}} */

/* {{{ php_escape_shell_arg
 */
char *php_escape_shell_arg(char *str) {
	int x, y, l;
	char *cmd;
	TSRMLS_FETCH();

	y = 0;
	l = strlen(str);
	
	cmd = safe_emalloc(4, l, 3); /* worst case */
	
#ifdef PHP_WIN32
	cmd[y++] = '"';
#else
	cmd[y++] = '\'';
#endif

	for (x = 0; x < l; x++) {
		int mb_len = php_mblen(str + x, (l - x));

		/* skip non-valid multibyte characters */
		if (mb_len < 0) {
			continue;
		} else if (mb_len > 1) {
			memcpy(cmd + y, str + x, mb_len);
			y += mb_len;
			x += mb_len - 1;
			continue;
		}

		switch (str[x]) {
#ifdef PHP_WIN32
		case '"':
		case '%':
			cmd[y++] = ' ';
			break;
#else
		case '\'':
			cmd[y++] = '\'';
			cmd[y++] = '\\';
			cmd[y++] = '\'';
#endif
			/* fall-through */
		default:
			cmd[y++] = str[x];
		}
	}
#ifdef PHP_WIN32
	cmd[y++] = '"';
#else
	cmd[y++] = '\'';
#endif
	cmd[y] = '\0';
	return cmd;
}
/* }}} */

/* {{{ proto string escapeshellcmd(string command)
   Escape shell metacharacters */
PHP_FUNCTION(escapeshellcmd)
{
	char *command;
	int command_len;
	char *cmd = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &command, &command_len) == FAILURE) {
		return;
	}

	if (command_len) {
		cmd = php_escape_shell_cmd(command);
		RETVAL_STRING(cmd, 0);
	} else {
		RETVAL_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string escapeshellarg(string arg)
   Quote and escape an argument for use in a shell command */
PHP_FUNCTION(escapeshellarg)
{
	char *argument;
	int argument_len;
	char *cmd = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &argument, &argument_len) == FAILURE) {
		return;
	}

	if (argument) {
		cmd = php_escape_shell_arg(argument);
		RETVAL_STRING(cmd, 0);
	}
}
/* }}} */

/* {{{ proto string shell_exec(string cmd)
   Execute command via shell and return complete output as string */
PHP_FUNCTION(shell_exec)
{
	FILE *in;
	size_t total_readbytes;
	zval **cmd;
	char *ret;
	php_stream *stream;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &cmd)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (PG(safe_mode)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot execute using backquotes in Safe Mode");
		RETURN_FALSE;
	}

	convert_to_string_ex(cmd);
#ifdef PHP_WIN32
	if ((in=VCWD_POPEN(Z_STRVAL_PP(cmd), "rt"))==NULL) {
#else
	if ((in=VCWD_POPEN(Z_STRVAL_PP(cmd), "r"))==NULL) {
#endif
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to execute '%s'", Z_STRVAL_PP(cmd));
		RETURN_FALSE;
	}

	stream = php_stream_fopen_from_pipe(in, "rb");
	total_readbytes = php_stream_copy_to_mem(stream, &ret, PHP_STREAM_COPY_ALL, 0);
	php_stream_close(stream); 
	
	if (total_readbytes > 0) {
		RETURN_STRINGL(ret, total_readbytes, 0);
	} else {
		RETURN_NULL();	
	}
}
/* }}} */

#ifdef HAVE_NICE
/* {{{ proto bool proc_nice(int priority)
   Change the priority of the current process */
PHP_FUNCTION(proc_nice)
{
	long pri;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pri) == FAILURE) {
		RETURN_FALSE;
	}

	errno = 0;
	nice(pri);
	if (errno) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Only a super user may attempt to increase the priority of a process");
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
