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

/* Linux-specific includes */
#ifdef HAVE_COPY_FILE_RANGE
#include <sys/syscall.h>
#endif

#ifdef HAVE_SENDFILE
#include <sys/sendfile.h>
#endif

#ifdef HAVE_SPLICE
#include <fcntl.h>
#endif

/* copy_file_range wrapper for older systems */
#ifdef HAVE_COPY_FILE_RANGE
static ssize_t copy_file_range_wrapper(
		int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t len, unsigned int flags)
{
	return syscall(SYS_copy_file_range, fd_in, off_in, fd_out, off_out, len, flags);
}
#endif

zend_result php_io_linux_copy_file_to_file(int src_fd, int dest_fd, size_t len, size_t *copied)
{
#ifdef HAVE_COPY_FILE_RANGE
	/* Try copy_file_range first - kernel-level optimization */
	ssize_t result = copy_file_range_wrapper(src_fd, NULL, dest_fd, NULL, len, 0);

	if (result > 0) {
		*copied = (size_t) result;
		return SUCCESS;
	}

	/* If copy_file_range fails, fall through to generic implementation */
	if (result == -1) {
		switch (errno) {
			case EINVAL:
			case EXDEV:
			case ENOSYS:
				/* Expected failures - fall back to generic copy */
				break;
			default:
				/* Unexpected error */
				*copied = 0;
				return FAILURE;
		}
	}
#endif

	/* Fallback to generic read/write loop */
	return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

zend_result php_io_linux_copy_file_to_socket(int src_fd, int dest_fd, size_t len, size_t *copied)
{
#ifdef HAVE_SENDFILE
	/* Use sendfile for zero-copy file to socket transfer */
	off_t offset = 0;
	ssize_t result = sendfile(dest_fd, src_fd, &offset, len);

	if (result > 0) {
		*copied = (size_t) result;
		return SUCCESS;
	}

	/* Handle partial sends and errors */
	if (result == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			/* Would block - for now, fall back to generic copy */
			/* TODO: Could implement epoll-based retry here */
		}
		/* Other errors fall through to generic copy */
	}
#endif

	/* Fallback to generic read/write loop */
	return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

zend_result php_io_linux_copy_socket_to_fd(int src_fd, int dest_fd, size_t len, size_t *copied)
{
#ifdef HAVE_SPLICE
	/* Use splice for zero-copy socket to fd transfer */
	/* splice() can work directly between socket and file/pipe */
	size_t total_copied = 0;
	size_t remaining = len;

	while (remaining > 0) {
		/* splice from socket to destination fd */
		ssize_t result
				= splice(src_fd, NULL, dest_fd, NULL, remaining, SPLICE_F_MOVE | SPLICE_F_MORE);

		if (result > 0) {
			total_copied += result;
			remaining -= result;
		} else if (result == 0) {
			/* EOF */
			break;
		} else {
			/* Error occurred */
			switch (errno) {
				case EAGAIN:
				case EWOULDBLOCK:
					/* Would block - return what we've copied so far */
					if (total_copied > 0) {
						*copied = total_copied;
						return SUCCESS;
					}
					/* Fall through to generic if nothing copied yet */
					break;
				case EINVAL:
					/* splice not supported for these fds */
					if (total_copied > 0) {
						/* We already copied some data, return success */
						*copied = total_copied;
						return SUCCESS;
					}
					/* Fall through to generic */
					break;
				case EPIPE:
					/* Broken pipe */
					if (total_copied > 0) {
						*copied = total_copied;
						return SUCCESS;
					}
					*copied = 0;
					return FAILURE;
				default:
					/* Other errors */
					if (total_copied > 0) {
						*copied = total_copied;
						return SUCCESS;
					}
					break;
			}
			break;
		}
	}

	if (total_copied > 0) {
		*copied = total_copied;
		return SUCCESS;
	}
#endif /* HAVE_SPLICE */

	/* Fallback to generic read/write loop */
	return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

#endif /* __linux__ */
