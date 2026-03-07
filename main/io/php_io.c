/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_io.h"
#include "php_io_internal.h"

#ifdef PHP_WIN32
#include <io.h>
#include <winsock2.h>
#else
#include <unistd.h>
#endif

static php_io php_io_instance = {
	.copy = PHP_IO_PLATFORM_COPY,
	.platform_name = PHP_IO_PLATFORM_NAME,
};

PHPAPI php_io *php_io_get(void)
{
	return &php_io_instance;
}

PHPAPI ssize_t php_io_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen)
{
	return php_io_get()->copy(src, dest, maxlen);
}

ssize_t php_io_generic_copy_fallback(int src_fd, int dest_fd, size_t maxlen)
{
	char buf[8192];
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		size_t to_read = (remaining < sizeof(buf)) ? remaining : sizeof(buf);
		ssize_t bytes_read = read(src_fd, buf, to_read);

		if (bytes_read < 0) {
			return total_copied > 0 ? (ssize_t) total_copied : -1;
		} else if (bytes_read == 0) {
			return (ssize_t) total_copied;
		}

		char *writeptr = buf;
		size_t to_write = (size_t) bytes_read;

		while (to_write > 0) {
			ssize_t bytes_written = write(dest_fd, writeptr, to_write);
			if (bytes_written <= 0) {
				return total_copied > 0 ? (ssize_t) total_copied : -1;
			}
			total_copied += bytes_written;
			writeptr += bytes_written;
			to_write -= bytes_written;
		}

		if (maxlen != PHP_IO_COPY_ALL) {
			remaining -= bytes_read;
		}
	}

	return (ssize_t) total_copied;
}

ssize_t php_io_generic_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen)
{
	return php_io_generic_copy_fallback(src->fd, dest->fd, maxlen);
}
