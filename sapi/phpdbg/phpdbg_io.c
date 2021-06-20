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
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "phpdbg_io.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

/* is easy to generalize ... but not needed for now */
PHPDBG_API int phpdbg_consume_stdin_line(char *buf) {
	int bytes = PHPDBG_G(input_buflen), len = 0;

	if (PHPDBG_G(input_buflen)) {
		memcpy(buf, PHPDBG_G(input_buffer), bytes);
	}

	PHPDBG_G(last_was_newline) = 1;

	do {
		int i;
		if (bytes <= 0) {
			continue;
		}

		for (i = len; i < len + bytes; i++) {
			if (buf[i] == '\x03') {
				if (i != len + bytes - 1) {
					memmove(buf + i, buf + i + 1, len + bytes - i - 1);
				}
				len--;
				i--;
				continue;
			}
			if (buf[i] == '\n') {
				PHPDBG_G(input_buflen) = len + bytes - 1 - i;
				if (PHPDBG_G(input_buflen)) {
					memcpy(PHPDBG_G(input_buffer), buf + i + 1, PHPDBG_G(input_buflen));
				}
				if (i != PHPDBG_MAX_CMD - 1) {
					buf[i + 1] = 0;
				}
				return i;
			}
		}

		len += bytes;
	} while ((bytes = phpdbg_mixed_read(PHPDBG_G(io)[PHPDBG_STDIN].fd, buf + len, PHPDBG_MAX_CMD - len, -1)) > 0);

	if (bytes <= 0) {
		PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
		zend_bailout();
	}

	return bytes;
}

PHPDBG_API int phpdbg_mixed_read(int fd, char *ptr, int len, int tmo) {
	int ret;

	do {
		ret = read(fd, ptr, len);
	} while (ret == -1 && errno == EINTR);

	return ret;
}

static int phpdbg_output_pager(int fd, const char *ptr, int len) {
	int count = 0, bytes = 0;
	const char *p = ptr, *endp = ptr + len;

	while ((p = memchr(p, '\n', endp - p))) {
		count++;
		p++;

		if (count % PHPDBG_G(lines) == 0) {
			bytes += write(fd, ptr + bytes, (p - ptr) - bytes);

			if (memchr(p, '\n', endp - p)) {
				char buf[PHPDBG_MAX_CMD];
				zend_quiet_write(fd, ZEND_STRL("\r---Type <return> to continue or q <return> to quit---"));
				phpdbg_consume_stdin_line(buf);
				if (*buf == 'q') {
					break;
				}
				zend_quiet_write(fd, "\r", 1);
			} else break;
		}
	}
	if (bytes && count % PHPDBG_G(lines) != 0) {
		bytes += write(fd, ptr + bytes, len - bytes);
	} else if (!bytes) {
		bytes += write(fd, ptr, len);
	}
	return bytes;
}

PHPDBG_API int phpdbg_mixed_write(int fd, const char *ptr, int len) {
	if ((PHPDBG_G(flags) & PHPDBG_HAS_PAGINATION)
	 && PHPDBG_G(io)[PHPDBG_STDOUT].fd == fd
	 && PHPDBG_G(lines) > 0) {
		return phpdbg_output_pager(fd, ptr, len);
	}

	return write(fd, ptr, len);
}
