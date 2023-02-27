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

#include "zend.h"
#include "php.h"
#include "spprintf.h"
#include "phpdbg.h"
#include "phpdbg_io.h"
#include "ext/standard/html.h"

#ifdef _WIN32
#	include "win32/time.h"
#endif

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

PHPDBG_API int _phpdbg_asprintf(char **buf, const char *format, ...) {
	int ret;
	va_list va;

	va_start(va, format);
	ret = vasprintf(buf, format, va);
	va_end(va);

	return ret;
}

static int phpdbg_process_print(int fd, int type, const char *msg, int msglen) {
	char *msgout = NULL;
	int msgoutlen = FAILURE;

	switch (type) {
		case P_ERROR:
			if (!PHPDBG_G(last_was_newline)) {
				phpdbg_mixed_write(fd, ZEND_STRL("\n"));
				PHPDBG_G(last_was_newline) = 1;
			}
			if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
				msgoutlen = phpdbg_asprintf(&msgout, "\033[%sm[%.*s]\033[0m\n", PHPDBG_G(colors)[PHPDBG_COLOR_ERROR]->code, msglen, msg);
			} else {
				msgoutlen = phpdbg_asprintf(&msgout, "[%.*s]\n", msglen, msg);
			}
			break;

		case P_NOTICE:
			if (!PHPDBG_G(last_was_newline)) {
				phpdbg_mixed_write(fd, ZEND_STRL("\n"));
				PHPDBG_G(last_was_newline) = 1;
			}
			if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
				msgoutlen = phpdbg_asprintf(&msgout, "\033[%sm[%.*s]\033[0m\n", PHPDBG_G(colors)[PHPDBG_COLOR_NOTICE]->code, msglen, msg);
			} else {
				msgoutlen = phpdbg_asprintf(&msgout, "[%.*s]\n", msglen, msg);
			}
			break;

		case P_WRITELN:
			if (msg) {
				msgoutlen = phpdbg_asprintf(&msgout, "%.*s\n", msglen, msg);
			} else {
				msgoutlen = 1;
				msgout = strdup("\n");
			}
			PHPDBG_G(last_was_newline) = 1;
			break;

		case P_WRITE:
			if (msg) {
				msgout = pestrndup(msg, msglen, 1);
				msgoutlen = msglen;
				PHPDBG_G(last_was_newline) = msg[msglen - 1] == '\n';
			} else {
				msgoutlen = 0;
				msgout = strdup("");
			}
			break;

		case P_STDOUT:
		case P_STDERR:
			if (msg) {
				PHPDBG_G(last_was_newline) = msg[msglen - 1] == '\n';
				phpdbg_mixed_write(fd, msg, msglen);
			}
			return msglen;

		/* no formatting on logging output */
		case P_LOG:
			if (msg) {
				struct timeval tp;
				if (gettimeofday(&tp, NULL) == SUCCESS) {
					msgoutlen = phpdbg_asprintf(&msgout, "[%ld %.8F]: %.*s\n", tp.tv_sec, tp.tv_usec / 1000000., msglen, msg);
				} else {
					msgoutlen = FAILURE;
				}
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	if (msgoutlen != FAILURE) {
		phpdbg_mixed_write(fd, msgout, msgoutlen);

		free(msgout);
	}
	return msgoutlen;
} /* }}} */

PHPDBG_API int phpdbg_vprint(int type, int fd, const char *strfmt, va_list args) {
	char *msg = NULL;
	int msglen = 0;
	int len;
	va_list argcpy;

	if (strfmt != NULL && strlen(strfmt) > 0L) {
		va_copy(argcpy, args);
		msglen = vasprintf(&msg, strfmt, argcpy);
		va_end(argcpy);
	}

	if (PHPDBG_G(err_buf).active && type != P_STDOUT && type != P_STDERR) {
		phpdbg_free_err_buf();

		PHPDBG_G(err_buf).type = type;
		PHPDBG_G(err_buf).fd = fd;
		PHPDBG_G(err_buf).msg = msg;
		PHPDBG_G(err_buf).msglen = msglen;

		return msglen;
	}

	if (UNEXPECTED(msglen == 0)) {
		len = 0;
	} else {
		len = phpdbg_process_print(fd, type, msg, msglen);
	}

	if (msg) {
		free(msg);
	}

	return len;
}

PHPDBG_API void phpdbg_free_err_buf(void) {
	if (PHPDBG_G(err_buf).type == 0) {
		return;
	}

	free(PHPDBG_G(err_buf).msg);

	PHPDBG_G(err_buf).type = 0;
}

PHPDBG_API void phpdbg_activate_err_buf(bool active) {
	PHPDBG_G(err_buf).active = active;
}

PHPDBG_API int phpdbg_output_err_buf(const char *strfmt, ...) {
	int len;
	va_list args;
	int errbuf_active = PHPDBG_G(err_buf).active;

	if (PHPDBG_G(flags) & PHPDBG_DISCARD_OUTPUT) {
		return 0;
	}

	PHPDBG_G(err_buf).active = 0;

	va_start(args, strfmt);
	len = phpdbg_vprint(PHPDBG_G(err_buf).type, PHPDBG_G(err_buf).fd, strfmt, args);
	va_end(args);

	PHPDBG_G(err_buf).active = errbuf_active;
	phpdbg_free_err_buf();

	return len;
}

PHPDBG_API int phpdbg_print(int type, int fd, const char *strfmt, ...) {
	va_list args;
	int len;

	if (PHPDBG_G(flags) & PHPDBG_DISCARD_OUTPUT) {
		return 0;
	}

	va_start(args, strfmt);
	len = phpdbg_vprint(type, fd, strfmt, args);
	va_end(args);

	return len;
}

PHPDBG_API int phpdbg_log_internal(int fd, const char *fmt, ...) {
	va_list args;
	char *buffer;
	int buflen;
	int len = 0;

	va_start(args, fmt);
	buflen = vasprintf(&buffer, fmt, args);
	va_end(args);

	len = phpdbg_mixed_write(fd, buffer, buflen);
	free(buffer);

	return len;
}

PHPDBG_API int phpdbg_out_internal(int fd, const char *fmt, ...) {
	va_list args;
	char *buffer;
	int buflen;
	int len = 0;

	if (PHPDBG_G(flags) & PHPDBG_DISCARD_OUTPUT) {
		return 0;
	}

	va_start(args, fmt);
	buflen = vasprintf(&buffer, fmt, args);
	va_end(args);

	len = phpdbg_mixed_write(fd, buffer, buflen);

	free(buffer);
	return len;
}
