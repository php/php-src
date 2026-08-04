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
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <errno.h>

static zend_result php_io_solaris_sendfile(int src_fd, int dest_fd, size_t maxlen, size_t *copied)
{
#ifdef HAVE_SENDFILE
	/* Solaris/illumos sendfile() takes an explicit offset and does not
	 * advance the source descriptor, so remember the starting position and
	 * restore it afterwards to keep the streams-layer contract. */
	off_t start_offset = lseek(src_fd, 0, SEEK_CUR);
	if (start_offset == (off_t) -1) {
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen, copied);
	}

	/* Unlike Linux, Solaris sendfile() returns -1/EINVAL (rather than 0) when
	 * called with an offset at or past EOF, so a trailing call issued after the
	 * source has been fully consumed would spuriously fail an otherwise complete
	 * copy. Bound the loop to the bytes actually available in the source file so
	 * we stop exactly at EOF and never call sendfile() past it. */
	struct stat st;
	if (fstat(src_fd, &st) != 0 || !S_ISREG(st.st_mode)) {
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen, copied);
	}

	size_t available = (st.st_size > start_offset) ? (size_t) (st.st_size - start_offset) : 0;
	size_t target = (maxlen == PHP_IO_COPY_ALL || maxlen > available) ? available : maxlen;

	off_t offset = start_offset;
	size_t total_copied = 0;
	zend_result result = SUCCESS;

	while (total_copied < target) {
		size_t remaining = target - total_copied;
		size_t to_send = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		/* offset is updated in place by sendfile() */
		ssize_t sent = sendfile(dest_fd, src_fd, &offset, to_send);

		if (sent > 0) {
			total_copied += (size_t) sent;
		} else if (sent == 0) {
			/* Source shrank under us; stop with what we have. */
			break;
		} else {
			if (errno == EINTR) {
				continue;
			}
			if (errno == EAGAIN) {
				break;
			}
			if (total_copied == 0) {
				return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen, copied);
			}
			result = FAILURE;
			break;
		}
	}

	if (total_copied > 0) {
		/* best effort: keep reporting the delivered bytes even if this fails */
		lseek(src_fd, start_offset + (off_t) total_copied, SEEK_SET);
	}

	*copied = total_copied;
	return result;
#else
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen, copied);
#endif
}

zend_result php_io_solaris_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen, size_t *copied)
{
	/* Unlike FreeBSD, Solaris/illumos sendfile() accepts both a socket and a
	 * regular file as the output descriptor, so file->socket and file->file
	 * can both be offloaded. */
	if (src->fd_type == PHP_IO_FD_FILE &&
			(dest->fd_type == PHP_IO_FD_SOCKET || dest->fd_type == PHP_IO_FD_FILE)) {
		return php_io_solaris_sendfile(src->fd, dest->fd, maxlen, copied);
	}

	/* php_io_generic_copy honours the stream timeout for socket sources */
	return php_io_generic_copy(src, dest, maxlen, copied);
}

#endif
