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

	while (remaining > 0) {
		/* Clamp to SSIZE_MAX to avoid issues */
		size_t to_copy = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		ssize_t result = copy_file_range(src_fd, NULL, dest_fd, NULL, to_copy, 0);

		if (result > 0) {
			total_copied += result;
			/* File positions are automatically updated by copy_file_range */
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

	while (remaining > 0) {
		/* Clamp to SSIZE_MAX */
		size_t to_send = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;
		ssize_t result = sendfile(dest_fd, src_fd, NULL, to_send);

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
				case EINVAL:
				case ENOSYS:
					/* sendfile not supported - fall back */
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
					}
					/* Already copied some, continue with fallback for the rest */
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

	/* Fallback to generic read/write loop */
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

ssize_t php_io_linux_copy_generic_to_any(int src_fd, int dest_fd, size_t maxlen)
{
#ifdef HAVE_SPLICE
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
	}

	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		size_t to_copy = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;

		/* src_fd → pipe */
		ssize_t in_pipe = splice(src_fd, NULL, pipefd[1], NULL, to_copy, 0);

		if (in_pipe < 0) {
			/* Error on splice in - drain pipe if anything is there, then fall back */
			char drain_buf[1024];
			ssize_t drained;
			while ((drained = read(pipefd[0], drain_buf, sizeof(drain_buf))) > 0) {
				ssize_t written = write(dest_fd, drain_buf, drained);
				if (written <= 0) {
					close(pipefd[0]);
					close(pipefd[1]);
					return total_copied > 0 ? (ssize_t) total_copied : -1;
				}
				total_copied += written;
			}
			close(pipefd[0]);
			close(pipefd[1]);

			/* Continue with generic fallback for remaining data */
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining = (total_copied < maxlen) ? maxlen - total_copied : 0;
			}
			if (remaining > 0) {
				ssize_t fallback_result = php_io_generic_copy_fallback(src_fd, dest_fd, remaining);
				if (fallback_result > 0) {
					total_copied += fallback_result;
				}
			}
			return total_copied > 0 ? (ssize_t) total_copied : -1;
		}

		if (in_pipe == 0) {
			/* EOF */
			break;
		}

		/* pipe → dest_fd */
		size_t pipe_remaining = in_pipe;
		while (pipe_remaining > 0) {
			ssize_t out = splice(pipefd[0], NULL, dest_fd, NULL, pipe_remaining, 0);
			if (out <= 0) {
				/* Error on splice out - need to drain the pipe first */
				char drain_buf[1024];
				while (pipe_remaining > 0) {
					size_t to_drain = (pipe_remaining < sizeof(drain_buf)) ? pipe_remaining
																		   : sizeof(drain_buf);
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

				/* Continue with generic fallback for remaining data */
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
			}
			pipe_remaining -= out;
			total_copied += out;
		}

		if (maxlen != PHP_IO_COPY_ALL) {
			remaining -= in_pipe;
		}

		if (maxlen != PHP_IO_COPY_ALL && remaining == 0) {
			break;
		}
	}

	close(pipefd[0]);
	close(pipefd[1]);

	return total_copied > 0 ? (ssize_t) total_copied : -1;
#endif

	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

#endif /* __linux__ */
