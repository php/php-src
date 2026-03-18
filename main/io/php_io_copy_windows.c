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

#include "php_io_internal.h"

#ifdef PHP_WIN32

#include <io.h>
#include <winsock2.h>
#include <mswsock.h>

static inline ssize_t php_io_win_read(php_io_fd *fd, char *buf, int len)
{
	if (fd->fd_type == PHP_IO_FD_SOCKET) {
		int result = recv(fd->socket, buf, len, 0);
		return (result == SOCKET_ERROR) ? -1 : (ssize_t) result;
	}
	return (ssize_t) _read(fd->fd, buf, len);
}

static inline ssize_t php_io_win_write(php_io_fd *fd, const char *buf, int len)
{
	if (fd->fd_type == PHP_IO_FD_SOCKET) {
		int result = send(fd->socket, buf, len, 0);
		return (result == SOCKET_ERROR) ? -1 : (ssize_t) result;
	}
	return (ssize_t) _write(fd->fd, buf, len);
}

static inline int php_io_win_wait_for_data(php_io_fd *fd)
{
	if (fd->fd_type != PHP_IO_FD_SOCKET || !fd->is_blocked) {
		return 1;
	}

	int timeout_ms;
	if (fd->timeout.tv_sec == -1) {
		timeout_ms = -1;
	} else {
		timeout_ms = fd->timeout.tv_sec * 1000 + fd->timeout.tv_usec / 1000;
	}

	WSAPOLLFD pfd;
	pfd.fd = fd->socket;
	pfd.events = POLLIN;

	int ret;
	do {
		ret = WSAPoll(&pfd, 1, timeout_ms);
	} while (ret == SOCKET_ERROR && WSAGetLastError() == WSAEINTR);

	return (ret == SOCKET_ERROR) ? -1 : ret;
}

static ssize_t php_io_win_copy_loop(php_io_fd *src, php_io_fd *dest, size_t maxlen)
{
	char buf[8192];
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		int ready = php_io_win_wait_for_data(src);
		if (ready == 0) {
			break;
		} else if (ready < 0) {
			return total_copied > 0 ? (ssize_t) total_copied : -1;
		}

		int to_read = (remaining < sizeof(buf)) ? (int) remaining : (int) sizeof(buf);
		ssize_t bytes_read = php_io_win_read(src, buf, to_read);

		if (bytes_read < 0) {
			return total_copied > 0 ? (ssize_t) total_copied : -1;
		} else if (bytes_read == 0) {
			return (ssize_t) total_copied;
		}

		const char *writeptr = buf;
		size_t to_write = (size_t) bytes_read;

		while (to_write > 0) {
			ssize_t bytes_written = php_io_win_write(dest, writeptr, (int) to_write);
			if (bytes_written <= 0) {
				return total_copied > 0 ? (ssize_t) total_copied : -1;
			}
			total_copied += bytes_written;
			writeptr += bytes_written;
			to_write -= bytes_written;
		}

		if (maxlen != PHP_IO_COPY_ALL) {
			remaining -= bytes_read;
		}
	}

	return (ssize_t) total_copied;
}

static ssize_t php_io_win_transmit_file(int src_fd, SOCKET dest_sock, size_t maxlen)
{
	HANDLE file_handle = (HANDLE) _get_osfhandle(src_fd);

	if (file_handle == INVALID_HANDLE_VALUE ||
			dest_sock == INVALID_SOCKET ||
			GetFileType(file_handle) != FILE_TYPE_DISK) {
		return -1;
	}

	LARGE_INTEGER file_pos;
	file_pos.QuadPart = 0;
	if (!SetFilePointerEx(file_handle, file_pos, &file_pos, FILE_CURRENT)) {
		return -1;
	}

	DWORD bytes_to_send = (maxlen == PHP_IO_COPY_ALL)
			? 0 : (DWORD) min(maxlen, MAXDWORD);

	if (!TransmitFile(dest_sock, file_handle, bytes_to_send, 0, NULL, NULL, 0)) {
		SetFilePointerEx(file_handle, file_pos, NULL, FILE_BEGIN);
		return -1;
	}

	LARGE_INTEGER new_pos;
	LARGE_INTEGER zero = {0};
	if (SetFilePointerEx(file_handle, zero, &new_pos, FILE_CURRENT)) {
		return (ssize_t)(new_pos.QuadPart - file_pos.QuadPart);
	}
	return (ssize_t) bytes_to_send;
}

ssize_t php_io_windows_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen)
{
	if (src->fd_type != PHP_IO_FD_SOCKET && dest->fd_type == PHP_IO_FD_SOCKET) {
		ssize_t result = php_io_win_transmit_file(src->fd, dest->socket, maxlen);
		if (result >= 0) {
			return result;
		}
	}

	return php_io_win_copy_loop(src, dest, maxlen);
}

#endif /* PHP_WIN32 */
