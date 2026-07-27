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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

static inline int php_io_linux_wait_for_data(php_io_fd *fd)
{
	if (fd->fd_type != PHP_IO_FD_SOCKET || !fd->is_blocked) {
		return 1;
	}

	struct timeval *ptimeout = (fd->timeout.tv_sec == -1) ? NULL : &fd->timeout;
	int timeout_ms;

	if (ptimeout == NULL) {
		timeout_ms = -1;
	} else {
		timeout_ms = ptimeout->tv_sec * 1000 + ptimeout->tv_usec / 1000;
	}

	int ret;
	do {
		ret = php_pollfd_for_ms(fd->fd, POLLIN, timeout_ms);
	} while (ret == -1 && errno == EINTR);

	return ret;
}

static zend_result php_io_linux_copy_file_to_file(int src_fd, int dest_fd, size_t maxlen, size_t *copied)
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
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen, copied);
					}
					break;
				default:
					*copied = total_copied;
					return FAILURE;
			}
			break;
		}
	}

	if (total_copied > 0) {
		*copied = total_copied;
		return SUCCESS;
	}
#endif

	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen, copied);
}

static zend_result php_io_linux_sendfile(int src_fd, int dest_fd, size_t maxlen, size_t *copied)
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
				case EINTR:
					continue;
				case EINVAL:
				case ENOSYS:
					if (total_copied == 0) {
						return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen, copied);
					}
					break;
				case EAGAIN:
					break;
				default:
					*copied = total_copied;
					return FAILURE;
			}
			break;
		}
	}

	if (total_copied > 0) {
		*copied = total_copied;
		return SUCCESS;
	}
#endif

	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen, copied);
}

#ifdef HAVE_SPLICE
/* Enlarge the intermediate pipe so socket transfers move more data per splice
 * round-trip. Capped by /proc/sys/fs/pipe-max-size (1 MiB by default); a failed
 * fcntl() simply leaves the kernel default (64 KiB) in place. */
#define PHP_IO_PIPE_SIZE (1 << 20)

/* Kernel MAX_RW_COUNT; a larger len fails the pos + len overflow check with
 * EINVAL once a file destination sits at a non-zero offset. */
#define PHP_IO_SPLICE_MAX ((size_t) 0x7ffff000)

/* SPLICE_F_MORE corks the socket the same way MSG_MORE does, letting the kernel
 * coalesce splices into full segments. The final partial segment is not flushed
 * by the kernel until the cork timer (<= 200 ms) expires or the socket is next
 * written/closed, so the copy loops below clear the cork once they are done. */
static inline unsigned int php_io_linux_out_flags(const php_io_fd *dest)
{
	return (dest->fd_type == PHP_IO_FD_SOCKET) ? SPLICE_F_MORE : 0;
}

/* Clearing TCP_CORK pushes out any segment still held by SPLICE_F_MORE;
 * on non-TCP sockets the setsockopt() harmlessly fails. */
static inline void php_io_linux_socket_uncork(const php_io_fd *dest, size_t total_copied)
{
	if (dest->fd_type == PHP_IO_FD_SOCKET && total_copied > 0) {
		int off = 0;
		setsockopt(dest->fd, IPPROTO_TCP, TCP_CORK, &off, sizeof(off));
	}
}

static zend_result php_io_linux_splice_from_pipe(php_io_fd *src, php_io_fd *dest, size_t maxlen, size_t *copied)
{
	int dest_fd = dest->fd;
	unsigned int out_flags = php_io_linux_out_flags(dest);
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;
	zend_result result = SUCCESS;

	while (remaining > 0) {
		int ready = php_io_linux_wait_for_data(src);
		if (ready == 0) {
			break;
		} else if (ready < 0) {
			result = FAILURE;
			break;
		}

		size_t to_copy = (remaining < PHP_IO_SPLICE_MAX) ? remaining : PHP_IO_SPLICE_MAX;
		ssize_t spliced = splice(src->fd, NULL, dest_fd, NULL, to_copy, out_flags);

		if (spliced > 0) {
			total_copied += spliced;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= spliced;
			}
		} else if (spliced == 0) {
			break;
		} else {
			if (total_copied == 0) {
				return php_io_generic_copy_fallback(src->fd, dest_fd, maxlen, copied);
			}
			result = FAILURE;
			break;
		}
	}

	php_io_linux_socket_uncork(dest, total_copied);
	*copied = total_copied;
	return result;
}

static zend_result php_io_linux_splice_via_pipe(php_io_fd *src, php_io_fd *dest, size_t maxlen, size_t *copied)
{
	int dest_fd = dest->fd;
	unsigned int out_flags = php_io_linux_out_flags(dest);
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		return php_io_generic_copy_fallback(src->fd, dest_fd, maxlen, copied);
	}

#ifdef F_SETPIPE_SZ
	fcntl(pipefd[1], F_SETPIPE_SZ, PHP_IO_PIPE_SIZE);
#endif

	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;
	zend_result result = SUCCESS;

	while (remaining > 0) {
		int ready = php_io_linux_wait_for_data(src);
		if (ready == 0) {
			/* timeout */
			break;
		} else if (ready < 0) {
			result = FAILURE;
			break;
		}

		size_t to_copy = (remaining < SSIZE_MAX) ? remaining : SSIZE_MAX;

		ssize_t in_pipe = splice(src->fd, NULL, pipefd[1], NULL, to_copy, 0);
		if (in_pipe < 0) {
			if (total_copied == 0) {
				close(pipefd[0]);
				close(pipefd[1]);
				return php_io_generic_copy_fallback(src->fd, dest_fd, maxlen, copied);
			}
			result = FAILURE;
			break;
		}
		if (in_pipe == 0) {
			break;
		}

		size_t pipe_remaining = in_pipe;
		while (pipe_remaining > 0) {
			ssize_t out = splice(pipefd[0], NULL, dest_fd, NULL, pipe_remaining, out_flags);
			if (out <= 0) {
				/* the dest refused the splice; salvage what already sits in
				 * the pipe with a plain read/write loop before failing */
				char drain_buf[1024];
				while (pipe_remaining > 0) {
					size_t to_drain = (pipe_remaining < sizeof(drain_buf))
							? pipe_remaining : sizeof(drain_buf);
					ssize_t drained;
					do {
						drained = read(pipefd[0], drain_buf, to_drain);
					} while (drained < 0 && errno == EINTR);
					if (drained <= 0) {
						break;
					}
					ssize_t drain_written = 0;
					while (drain_written < drained) {
						ssize_t written;
						do {
							written = write(dest_fd, drain_buf + drain_written, drained - drain_written);
						} while (written < 0 && errno == EINTR);
						if (written <= 0) {
							total_copied += drain_written;
							result = FAILURE;
							goto out;
						}
						drain_written += written;
					}
					pipe_remaining -= drain_written;
					total_copied += drain_written;
				}
				result = FAILURE;
				goto out;
			}
			pipe_remaining -= out;
			total_copied += out;
		}

		if (maxlen != PHP_IO_COPY_ALL) {
			remaining -= in_pipe;
		}
	}

out:
	close(pipefd[0]);
	close(pipefd[1]);
	php_io_linux_socket_uncork(dest, total_copied);
	*copied = total_copied;
	return result;
}
#endif /* HAVE_SPLICE */

zend_result php_io_linux_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen, size_t *copied)
{
	if (src->fd_type == PHP_IO_FD_FILE && dest->fd_type == PHP_IO_FD_FILE) {
		return php_io_linux_copy_file_to_file(src->fd, dest->fd, maxlen, copied);
	}

	if (src->fd_type == PHP_IO_FD_FILE && dest->fd_type == PHP_IO_FD_SOCKET) {
		return php_io_linux_sendfile(src->fd, dest->fd, maxlen, copied);
	}

	if (src->fd_type == PHP_IO_FD_FILE && dest->fd_type == PHP_IO_FD_PIPE) {
		return php_io_linux_sendfile(src->fd, dest->fd, maxlen, copied);
	}

#ifdef HAVE_SPLICE
	if (src->fd_type == PHP_IO_FD_PIPE) {
		return php_io_linux_splice_from_pipe(src, dest, maxlen, copied);
	}

	if (src->fd_type == PHP_IO_FD_SOCKET) {
		return php_io_linux_splice_via_pipe(src, dest, maxlen, copied);
	}
#endif

	/* php_io_generic_copy honours the stream timeout for socket sources */
	return php_io_generic_copy(src, dest, maxlen, copied);
}

#endif /* __linux__ */
