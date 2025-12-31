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
#ifdef HAVE_SENDFILE
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;
	off_t src_offset;

	/* Get current source file position */
	src_offset = lseek(src_fd, 0, SEEK_CUR);
	if (src_offset == (off_t) -1) {
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
	}

	while (remaining > 0) {
		size_t to_send = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		ssize_t result = sendfile(dest_fd, src_fd, &src_offset, to_send);

		if (result > 0) {
			total_copied += result;
			/* src_offset is automatically updated by sendfile */

			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= result;
			}
		} else if (result == 0) {
			/* EOF */
			break;
		} else {
			/* Error occurred */
			switch (errno) {
				case EINVAL:
				case ENOSYS:
				case EAFNOSUPPORT:
					/* sendfile not supported - fall back */
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					/* Continue with fallback for remaining data */
					if (maxlen != PHP_IO_COPY_ALL) {
						remaining = (total_copied < maxlen) ? maxlen - total_copied : 0;
					}
					if (remaining > 0) {
						ssize_t fallback_result
								= php_io_generic_copy_fallback(src_fd, dest_fd, remaining);
						if (fallback_result > 0) {
							total_copied += fallback_result;
						}
					}
					return total_copied > 0 ? (ssize_t) total_copied : -1;
				case EAGAIN:
					return total_copied > 0 ? (ssize_t) total_copied : -1;
				default:
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					return total_copied > 0 ? (ssize_t) total_copied : -1;
			}
		}

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

#endif /* __sun */
