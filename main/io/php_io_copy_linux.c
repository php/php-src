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

static ssize_t php_io_linux_copy_file_to_file(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_COPY_FILE_RANGE
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		size_t to_copy = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		ssize_t result = copy_file_range(src_fd, NULL, dest_fd, NULL, to_copy, 0);

		if (result > 0) {
			total_copied += result;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= result;
			}
		} else if (result == 0) {
			break;
		} else {
			switch (errno) {
				case EINVAL:
				case EXDEV:
				case ENOSYS:
				case EIO:
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					break;
				default:
					return total_copied > 0 ? (ssize_t) total_copied : -1;
			}
			break;
		}
	}

	if (total_copied > 0) {
		return (ssize_t) total_copied;
	}
#endif

	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

static ssize_t php_io_linux_sendfile(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_SENDFILE
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		size_t to_send = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		ssize_t result = sendfile(dest_fd, src_fd, NULL, to_send);

		if (result > 0) {
			total_copied += result;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= result;
			}
		} else if (result == 0) {
			break;
		} else {
			switch (errno) {
				case EINVAL:
				case ENOSYS:
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					break;
				case EAGAIN:
					break;
				default:
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					break;
			}
			break;
		}
	}

	if (total_copied > 0) {
		return (ssize_t) total_copied;
	}
#endif

	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

#ifdef HAVE_SPLICE
static ssize_t php_io_linux_splice_from_pipe(int pipe_fd, int dest_fd, size_t maxlen)
{
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		size_t to_copy = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		ssize_t result = splice(pipe_fd, NULL, dest_fd, NULL, to_copy, 0);

		if (result > 0) {
			total_copied += result;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= result;
			}
		} else if (result == 0) {
			break;
		} else {
			if (total_copied == 0) {
				return php_io_generic_copy_fallback(pipe_fd, dest_fd, maxlen);
			}
			break;
		}
	}

	return total_copied > 0 ? (ssize_t) total_copied : -1;
}

static ssize_t php_io_linux_splice_via_pipe(int src_fd, int dest_fd, size_t maxlen)
{
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
	}

	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		size_t to_copy = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;

		ssize_t in_pipe = splice(src_fd, NULL, pipefd[1], NULL, to_copy, 0);
		if (in_pipe < 0) {
			close(pipefd[0]);
			close(pipefd[1]);
			if (total_copied == 0) {
				return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
			}
			return (ssize_t) total_copied;
		}
		if (in_pipe == 0) {
			break;
		}

		size_t pipe_remaining = in_pipe;
		while (pipe_remaining > 0) {
			ssize_t out = splice(pipefd[0], NULL, dest_fd, NULL, pipe_remaining, 0);
			if (out <= 0) {
				/* drain pipe before closing */
				char drain_buf[1024];
				while (pipe_remaining > 0) {
					size_t to_drain = (pipe_remaining < sizeof(drain_buf))
							? pipe_remaining : sizeof(drain_buf);
					ssize_t drained = read(pipefd[0], drain_buf, to_drain);
					if (drained <= 0) {
						break;
					}
					ssize_t written = write(dest_fd, drain_buf, drained);
					if (written <= 0) {
						close(pipefd[0]);
						close(pipefd[1]);
						return total_copied > 0 ? (ssize_t) total_copied : -1;
					}
					pipe_remaining -= written;
					total_copied += written;
				}
				close(pipefd[0]);
				close(pipefd[1]);
				return total_copied > 0 ? (ssize_t) total_copied : -1;
			}
			pipe_remaining -= out;
			total_copied += out;
		}

		if (maxlen != PHP_IO_COPY_ALL) {
			remaining -= in_pipe;
		}
	}

	close(pipefd[0]);
	close(pipefd[1]);
	return total_copied > 0 ? (ssize_t) total_copied : -1;
}
#endif /* HAVE_SPLICE */

ssize_t php_io_linux_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen)
{
	if (src->fd_type == PHP_IO_FD_FILE && dest->fd_type == PHP_IO_FD_FILE) {
		return php_io_linux_copy_file_to_file(src->fd, dest->fd, maxlen);
	}

	if (src->fd_type == PHP_IO_FD_FILE && dest->fd_type == PHP_IO_FD_SOCKET) {
		return php_io_linux_sendfile(src->fd, dest->fd, maxlen);
	}

	/* sendfile also works for file to pipe on Linux */
	if (src->fd_type == PHP_IO_FD_FILE && dest->fd_type == PHP_IO_FD_PIPE) {
		return php_io_linux_sendfile(src->fd, dest->fd, maxlen);
	}

#ifdef HAVE_SPLICE
	if (src->fd_type == PHP_IO_FD_PIPE) {
		return php_io_linux_splice_from_pipe(src->fd, dest->fd, maxlen);
	}

	if (src->fd_type == PHP_IO_FD_SOCKET) {
		return php_io_linux_splice_via_pipe(src->fd, dest->fd, maxlen);
	}
#endif

	return php_io_generic_copy_fallback(src->fd, dest->fd, maxlen);
}

#endif /* __linux__ */
