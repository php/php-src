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

#ifdef __sun

#include "php_io_internal.h"
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_SENDFILEV
#include <sys/sendfile.h>
#endif

ssize_t php_io_solaris_copy_file_to_generic(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_SENDFILEV
	/* Solaris sendfilev - very powerful but complex API */
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		struct sendfilevec sfv;
		size_t xferred = 0;
		size_t to_send = (remaining < SIZE_MAX) ? remaining : SIZE_MAX;

		/* Set up the sendfile vector */
		sfv.sfv_fd = src_fd;
		sfv.sfv_flag = SFV_FD;
		sfv.sfv_off = 0;
		sfv.sfv_len = to_send;

		/* Perform the sendfile operation */
		ssize_t result = sendfilev(dest_fd, &sfv, 1, &xferred);

		if (result == 0 || xferred > 0) {
			/* Success or partial transfer */
			total_copied += xferred;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= xferred;
			}

			/* If we got less than requested or error occurred, stop */
			if (result != 0 || xferred < to_send) {
				return (ssize_t) total_copied;
			}
		} else {
			/* Error occurred with no data transferred */
			switch (errno) {
				case EAGAIN:
				case EINVAL:
				case ENOTCONN:
				case EPIPE:
				case EAFNOSUPPORT:
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
#endif /* HAVE_SENDFILEV */

	/* Fallback to generic implementation */
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

#endif /* __sun */
