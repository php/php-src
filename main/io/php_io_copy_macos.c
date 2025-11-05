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

#ifdef __APPLE__

#include "php_io_internal.h"
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_SENDFILE
#include <sys/socket.h>
#include <sys/uio.h>
#endif

#ifdef HAVE_COPYFILE
#include <copyfile.h>
#endif

ssize_t php_io_macos_copy_file_to_generic(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_SENDFILE
	/* macOS sendfile signature: sendfile(fd, s, offset, len, hdtr, flags) */
	/* Note: len is passed by reference and updated with bytes sent */
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		off_t to_send = (remaining < OFF_MAX) ? (off_t) remaining : OFF_MAX;
		off_t len_sent = to_send;
		int result = sendfile(src_fd, dest_fd, 0, &len_sent, NULL, 0);

		if (result == 0 || len_sent > 0) {
			/* Success or partial send */
			total_copied += len_sent;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= len_sent;
			}

			/* If we got less than requested or result != 0, stop */
			if (len_sent < to_send || result != 0) {
				return (ssize_t) total_copied;
			}
		} else {
			/* Error occurred */
			switch (errno) {
				case EAGAIN:
					/* Would block */
					return total_copied > 0 ? (ssize_t) total_copied : -1;
				case EINVAL:
				case ENOTCONN:
				case EPIPE:
					/* Various errors */
					if (total_copied > 0) {
						return (ssize_t) total_copied;
					}
					break;
				default:
					/* Other errors */
					if (total_copied > 0) {
						return (ssize_t) total_copied;
					}
					break;
			}
			break;
		}

		/* For bounded copies, stop if we reached maxlen */
		if (maxlen != PHP_IO_COPY_ALL && remaining == 0) {
			return (ssize_t) total_copied;
		}
	}

	if (total_copied > 0) {
		return (ssize_t) total_copied;
	}
#endif /* HAVE_SENDFILE */

	/* Fallback to generic implementation */
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

#endif /* __APPLE__ */
