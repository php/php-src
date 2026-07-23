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

#if defined(__FreeBSD__) || defined(__DragonFly__)

#include "php_io_internal.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

/* Hint the kernel to read ahead a few pages from the source file so the disk
 * I/O overlaps with the network send. SF_FLAGS() is FreeBSD-specific and may be
 * absent on DragonFly, in which case we pass plain 0 (no readahead hint). */
#ifdef SF_FLAGS
# define PHP_IO_FREEBSD_SF_FLAGS SF_FLAGS(16, 0)
#else
# define PHP_IO_FREEBSD_SF_FLAGS 0
#endif

static zend_result php_io_freebsd_sendfile(int src_fd, int dest_fd, size_t maxlen, size_t *copied)
{
	off_t start_offset = lseek(src_fd, 0, SEEK_CUR);
	if (start_offset == (off_t) -1) {
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen, copied);
	}

	off_t total_sent = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? 0 : maxlen;
	zend_result result = SUCCESS;

	while (maxlen == PHP_IO_COPY_ALL || remaining > 0) {
		off_t sent_in_this_call = 0;
		int sendfile_result = sendfile(src_fd, dest_fd, start_offset + total_sent, remaining, NULL,
				&sent_in_this_call, PHP_IO_FREEBSD_SF_FLAGS);

		if (sent_in_this_call > 0) {
			total_sent += sent_in_this_call;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= (size_t) sent_in_this_call;
			}
		}

		if (sendfile_result == 0) {
			if (sent_in_this_call == 0 || remaining == 0) {
				break;
			}
		} else {
			if (errno == EINTR) {
				continue;
			}
			if (errno == EAGAIN) {
				if (sent_in_this_call > 0) {
					continue;
				}
				break;
			}
			if (total_sent == 0) {
				return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen, copied);
			}
			result = FAILURE;
			break;
		}
	}

	if (total_sent > 0) {
		/* best effort: keep reporting the delivered bytes even if this fails */
		lseek(src_fd, start_offset + total_sent, SEEK_SET);
	}

	*copied = (size_t) total_sent;
	return result;
}

zend_result php_io_freebsd_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen, size_t *copied)
{
	/* unlike linux, sendfile on freebsd works only under those conditions */
	if (src->fd_type == PHP_IO_FD_FILE && dest->fd_type == PHP_IO_FD_SOCKET) {
		return php_io_freebsd_sendfile(src->fd, dest->fd, maxlen, copied);
	}

	/* php_io_generic_copy honours the stream timeout for socket sources */
	return php_io_generic_copy(src, dest, maxlen, copied);
}

#endif
