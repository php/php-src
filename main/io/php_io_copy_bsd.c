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
	off_t src_offset;

	/* Get current source file position */
	src_offset = lseek(src_fd, 0, SEEK_CUR);
	if (src_offset == (off_t) -1) {
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
	}

	while (remaining > 0) {
		off_t to_send = (remaining < OFF_MAX) ? (off_t) remaining : OFF_MAX;
		off_t sbytes = 0;
		int result = sendfile(src_fd, dest_fd, src_offset, to_send, NULL, &sbytes, 0);

		if (sbytes > 0) {
			/* Some data was transferred */
			total_copied += sbytes;
			src_offset += sbytes;

			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= sbytes;
			}
		}

		if (result == 0) {
			/* Success - continue */
			if (sbytes == 0) {
				/* No data transferred and success = EOF */
				break;
			}
		} else {
			/* Error occurred */
			switch (errno) {
				case EINVAL:
				case ENOTSOCK:
				case ENOTCONN:
					/* sendfile not supported - fall back */
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					/* Continue with fallback for remaining data */
					if (maxlen != PHP_IO_COPY_ALL) {
						remaining = (total_copied < maxlen) ? maxlen - total_copied : 0;
					}
					if (remaining > 0) {
						/* Update file position for fallback */
						if (lseek(src_fd, src_offset, SEEK_SET) != (off_t) -1) {
							ssize_t fallback_result
									= php_io_generic_copy_fallback(src_fd, dest_fd, remaining);
							if (fallback_result > 0) {
								total_copied += fallback_result;
							}
						}
					}
					return total_copied > 0 ? (ssize_t) total_copied : -1;
				case EAGAIN:
				case EBUSY:
					/* Would block - return what we have */
					return total_copied > 0 ? (ssize_t) total_copied : -1;
				default:
					/* Other errors */
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					return total_copied > 0 ? (ssize_t) total_copied : -1;
			}
		}

		/* For bounded copies, stop if we reached maxlen */
		if (maxlen != PHP_IO_COPY_ALL && remaining == 0) {
			break;
		}
	}

	if (total_copied > 0) {
		return (ssize_t) total_copied;
	}
#endif

	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

#endif /* FreeBSD, OpenBSD, NetBSD */
