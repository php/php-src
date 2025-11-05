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

#ifdef __linux__

#include "php_io_internal.h"
#include <unistd.h>
#include <errno.h>

#include <sys/syscall.h>

/* Provide copy_file_range wrapper if libc doesn't have it but kernel does */
#if !defined(HAVE_COPY_FILE_RANGE) && defined(__NR_copy_file_range)
#define HAVE_COPY_FILE_RANGE 1
static inline ssize_t copy_file_range(
		int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t len, unsigned int flags)
{
	return syscall(__NR_copy_file_range, fd_in, off_in, fd_out, off_out, len, flags);
}
#endif

#ifdef HAVE_SENDFILE
#include <sys/sendfile.h>
#endif

#ifdef HAVE_SPLICE
#include <fcntl.h>
#endif

ssize_t php_io_linux_copy_file_to_file(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_COPY_FILE_RANGE
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;
	loff_t src_offset = 0;
	loff_t dest_offset = 0;

	/* Get current file positions */
	off_t current_src = lseek(src_fd, 0, SEEK_CUR);
	off_t current_dest = lseek(dest_fd, 0, SEEK_CUR);

	if (current_src == (off_t) -1 || current_dest == (off_t) -1) {
		/* Can't get positions, fall back to generic copy */
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
	}

	src_offset = current_src;
	dest_offset = current_dest;

	while (remaining > 0) {
		/* Clamp to SSIZE_MAX to avoid issues */
		size_t to_copy = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		ssize_t result = copy_file_range(src_fd, &src_offset, dest_fd, &dest_offset, to_copy, 0);

		if (result > 0) {
			total_copied += result;
			/* Offsets are automatically updated by copy_file_range */

			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= result;
			}
		} else if (result == 0) {
			/* EOF - done */
			break;
		} else {
			/* Error occurred */
			switch (errno) {
				case EINVAL:
				case EXDEV:
				case ENOSYS:
					/* Expected failures - fall back to generic copy */
					if (total_copied == 0) {
						/* Haven't copied anything yet, can safely fall back */
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					/* If we already copied some, return what we have */
					break;
				default:
					/* Unexpected error */
					return total_copied > 0 ? (ssize_t) total_copied : -1;
			}
			break;
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

	/* Fallback to generic read/write loop */
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

ssize_t php_io_linux_copy_file_to_generic(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_SENDFILE
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;
	off_t src_offset = 0;

	/* Get current source file position */
	src_offset = lseek(src_fd, 0, SEEK_CUR);

	if (src_offset == (off_t) -1) {
		/* Can't get position, fall back to generic copy */
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
	}

	while (remaining > 0) {
		/* Clamp to SSIZE_MAX */
		size_t to_send = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		ssize_t result = sendfile(dest_fd, src_fd, &src_offset, to_send);

		if (result > 0) {
			total_copied += result;
			/* src_offset is automatically updated by sendfile */

			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= result;
			}
		} else if (result == 0) {
			/* EOF - done */
			break;
		} else {
			/* Error occurred */
			if (errno == EAGAIN) {
				/* Would block - return what we have */
				return total_copied > 0 ? (ssize_t) total_copied : -1;
			}
			/* Other errors - fall back if we haven't copied anything yet */
			if (total_copied == 0) {
				return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
			}
			/* Already copied some, return what we have */
			break;
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

	/* Fallback to generic read/write loop */
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

ssize_t php_io_linux_copy_generic_to_any(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_SPLICE
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	/* splice doesn't take offsets - it uses fd's current position */
	while (remaining > 0) {
		/* Clamp to SSIZE_MAX */
		size_t to_splice = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		ssize_t result
				= splice(src_fd, NULL, dest_fd, NULL, to_splice, SPLICE_F_MOVE | SPLICE_F_MORE);

		if (result > 0) {
			total_copied += result;

			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= result;
			}
		} else if (result == 0) {
			/* EOF - done */
			break;
		} else {
			/* Error occurred */
			switch (errno) {
				case EAGAIN:
					/* Would block */
					return total_copied > 0 ? (ssize_t) total_copied : -1;
				case EINVAL:
					/* splice not supported for these fds */
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					/* Already copied some, return what we have */
					break;
				case EPIPE:
					/* Broken pipe */
					return total_copied > 0 ? (ssize_t) total_copied : -1;
				default:
					/* Other errors */
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					/* Already copied some, return what we have */
					break;
			}
			break;
		}

		/* For bounded copies, stop if we reached maxlen */
		if (maxlen != PHP_IO_COPY_ALL && remaining == 0) {
			break;
		}
	}

	if (total_copied > 0) {
		return (ssize_t) total_copied;
	}
#endif /* HAVE_SPLICE */

	/* Fallback to generic read/write loop */
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

#endif /* __linux__ */
