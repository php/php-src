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

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

#include "php_io_internal.h"
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_SENDFILE
#include <sys/socket.h>
#include <sys/uio.h>
#endif

ssize_t php_io_bsd_copy_file_to_generic(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_SENDFILE
	/* BSD sendfile signature: sendfile(fd, s, offset, nbytes, hdtr, sbytes, flags) */
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		off_t to_send = (remaining < OFF_MAX) ? (off_t) remaining : OFF_MAX;
		off_t sbytes = 0;
		int result = sendfile(src_fd, dest_fd, 0, to_send, NULL, &sbytes, 0);

		if (result == 0 || sbytes > 0) {
			/* Success or partial send */
			total_copied += sbytes;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= sbytes;
			}

			/* If result == 0, entire amount was sent, continue if needed */
			/* If result == -1 but sbytes > 0, partial send occurred */
			if (result == -1 || sbytes < to_send) {
				return (ssize_t) total_copied;
			}
		} else {
			/* Error occurred with no data transferred */
			switch (errno) {
				case EAGAIN:
				case EBUSY:
				case EINVAL:
				case ENOTCONN:
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

#endif /* FreeBSD, OpenBSD, NetBSD */
