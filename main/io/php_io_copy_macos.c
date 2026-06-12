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
   | Authors: David Carlier <devnexen@gmail.com>                          |
   +----------------------------------------------------------------------+
*/

#if defined(__APPLE__)

#include "php_io_internal.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

static ssize_t php_io_macos_sendfile(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_SENDFILE
	/* macOS sendfile() takes an explicit offset and does not advance the
	 * source descriptor, so remember the starting position and restore it
	 * afterwards to keep the streams-layer contract. */
	off_t start_offset = lseek(src_fd, 0, SEEK_CUR);
	if (start_offset == (off_t) -1) {
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
	}

	off_t total_sent = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? 0 : maxlen;

	while (maxlen == PHP_IO_COPY_ALL || remaining > 0) {
		/* len is in/out: on input the number of bytes to send (0 means
		 * until end of file), on output the number of bytes actually sent
		 * (populated even when sendfile() reports EINTR/EAGAIN). */
		off_t len = (maxlen == PHP_IO_COPY_ALL) ? 0 : (off_t) remaining;
		int result = sendfile(src_fd, dest_fd, start_offset + total_sent, &len, NULL, 0);

		if (len > 0) {
			total_sent += len;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= (size_t) len;
			}
		}

		if (result == 0) {
			/* Reached EOF or sent the whole requested range. */
			if (len == 0 || maxlen == PHP_IO_COPY_ALL || remaining == 0) {
				break;
			}
		} else {
			if (errno == EINTR) {
				continue;
			}
			if (errno == EAGAIN) {
				if (len > 0) {
					continue;
				}
				break;
			}
			if (total_sent == 0) {
				return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
			}
			break;
		}
	}

	if (total_sent > 0 && lseek(src_fd, start_offset + total_sent, SEEK_SET) == (off_t) -1) {
		return -1;
	}

	return (ssize_t) total_sent;
#else
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
#endif
}

ssize_t php_io_macos_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen)
{
	/* Like FreeBSD, macOS sendfile() works only with a regular file source
	 * and a socket destination. */
	if (src->fd_type == PHP_IO_FD_FILE && dest->fd_type == PHP_IO_FD_SOCKET) {
		return php_io_macos_sendfile(src->fd, dest->fd, maxlen);
	}

	return php_io_generic_copy_fallback(src->fd, dest->fd, maxlen);
}

#endif
