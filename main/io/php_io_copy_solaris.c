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

#if defined(__sun) && defined(__SVR4)

#include "php_io_internal.h"
#include <sys/types.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <errno.h>

static ssize_t php_io_solaris_sendfile(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_SENDFILE
	/* Solaris/illumos sendfile() takes an explicit offset and does not
	 * advance the source descriptor, so remember the starting position and
	 * restore it afterwards to keep the streams-layer contract. */
	off_t start_offset = lseek(src_fd, 0, SEEK_CUR);
	if (start_offset == (off_t) -1) {
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
	}

	off_t offset = start_offset;
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		size_t to_send = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		/* offset is updated in place by sendfile() */
		ssize_t result = sendfile(dest_fd, src_fd, &offset, to_send);

		if (result > 0) {
			total_copied += (size_t) result;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= (size_t) result;
			}
		} else if (result == 0) {
			break;
		} else {
			if (errno == EINTR) {
				continue;
			}
			if (errno == EAGAIN) {
				break;
			}
			if (total_copied == 0) {
				return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
			}
			break;
		}
	}

	if (total_copied > 0 && lseek(src_fd, start_offset + (off_t) total_copied, SEEK_SET) == (off_t) -1) {
		return -1;
	}

	return (ssize_t) total_copied;
#else
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
#endif
}

ssize_t php_io_solaris_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen)
{
	/* Unlike FreeBSD, Solaris/illumos sendfile() accepts both a socket and a
	 * regular file as the output descriptor, so file->socket and file->file
	 * can both be offloaded. */
	if (src->fd_type == PHP_IO_FD_FILE &&
			(dest->fd_type == PHP_IO_FD_SOCKET || dest->fd_type == PHP_IO_FD_FILE)) {
		return php_io_solaris_sendfile(src->fd, dest->fd, maxlen);
	}

	return php_io_generic_copy_fallback(src->fd, dest->fd, maxlen);
}

#endif
