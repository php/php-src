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

typedef ssize_t (*php_io_win_read_fn)(void *handle, char *buf, size_t len);
typedef ssize_t (*php_io_win_write_fn)(void *handle, const char *buf, size_t len);

static ssize_t php_io_win_read_handle(void *handle, char *buf, size_t len)
{
	DWORD to_read = (len > MAXDWORD) ? MAXDWORD : (DWORD) len;
	DWORD bytes_read;
	if (!ReadFile((HANDLE) handle, buf, to_read, &bytes_read, NULL)) {
		return -1;
	}
	return (ssize_t) bytes_read;
}

static ssize_t php_io_win_write_handle(void *handle, const char *buf, size_t len)
{
	DWORD to_write = (len > MAXDWORD) ? MAXDWORD : (DWORD) len;
	DWORD bytes_written;
	if (!WriteFile((HANDLE) handle, buf, to_write, &bytes_written, NULL)) {
		return -1;
	}
	return (ssize_t) bytes_written;
}

static ssize_t php_io_win_read_socket(void *handle, char *buf, size_t len)
{
	int to_recv = (len > INT_MAX) ? INT_MAX : (int) len;
	int result = recv((SOCKET)(uintptr_t) handle, buf, to_recv, 0);
	if (result == SOCKET_ERROR) {
		return -1;
	}
	return (ssize_t) result;
}

static ssize_t php_io_win_write_socket(void *handle, const char *buf, size_t len)
{
	int to_send = (len > INT_MAX) ? INT_MAX : (int) len;
	int result = send((SOCKET)(uintptr_t) handle, buf, to_send, 0);
	if (result == SOCKET_ERROR) {
		return -1;
	}
	return (ssize_t) result;
}

static ssize_t php_io_win_copy_loop(
		void *src_handle, php_io_win_read_fn read_fn,
		void *dest_handle, php_io_win_write_fn write_fn,
		size_t maxlen)
{
	char buf[8192];
	size_t total_copied = 0;
	size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? SIZE_MAX : maxlen;

	while (remaining > 0) {
		size_t to_read = (remaining < sizeof(buf)) ? remaining : sizeof(buf);
		ssize_t bytes_read = read_fn(src_handle, buf, to_read);

		if (bytes_read < 0) {
			return total_copied > 0 ? (ssize_t) total_copied : -1;
		} else if (bytes_read == 0) {
			return (ssize_t) total_copied;
		}

		const char *writeptr = buf;
		size_t to_write = (size_t) bytes_read;

		while (to_write > 0) {
			ssize_t bytes_written = write_fn(dest_handle, writeptr, to_write);
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

static ssize_t php_io_win_copy_handle_to_handle(int src_fd, int dest_fd, size_t maxlen)
{
	HANDLE src_handle = (HANDLE) _get_osfhandle(src_fd);
	HANDLE dest_handle = (HANDLE) _get_osfhandle(dest_fd);

	if (src_handle == INVALID_HANDLE_VALUE || dest_handle == INVALID_HANDLE_VALUE) {
		return -1;
	}

	return php_io_win_copy_loop(
			(void *) src_handle, php_io_win_read_handle,
			(void *) dest_handle, php_io_win_write_handle,
			maxlen);
}

static ssize_t php_io_win_copy_handle_to_socket(int src_fd, SOCKET dest_sock, size_t maxlen)
{
	HANDLE file_handle = (HANDLE) _get_osfhandle(src_fd);

	if (file_handle == INVALID_HANDLE_VALUE) {
		return -1;
	}

	/* Try TransmitFile for file to socket */
	if (dest_sock != INVALID_SOCKET) {
		LARGE_INTEGER file_pos;
		file_pos.QuadPart = 0;
		if (SetFilePointerEx(file_handle, file_pos, &file_pos, FILE_CURRENT)) {
			DWORD bytes_to_send;

			if (maxlen == PHP_IO_COPY_ALL) {
				LARGE_INTEGER file_size;
				if (GetFileSizeEx(file_handle, &file_size)) {
					LONGLONG available = file_size.QuadPart - file_pos.QuadPart;
					bytes_to_send = (available > MAXDWORD) ? 0 : (DWORD) available;
				} else {
					bytes_to_send = 0;
				}
			} else {
				bytes_to_send = (DWORD) min(maxlen, MAXDWORD);
			}

			if (TransmitFile(dest_sock, file_handle, bytes_to_send, 0, NULL, NULL, 0)) {
				if (bytes_to_send == 0 && maxlen == PHP_IO_COPY_ALL) {
					LARGE_INTEGER new_pos;
					LARGE_INTEGER zero = {0};
					if (SetFilePointerEx(file_handle, zero, &new_pos, FILE_CURRENT)) {
						return (ssize_t)(new_pos.QuadPart - file_pos.QuadPart);
					}
					return 0;
				}
				return (ssize_t) bytes_to_send;
			}

			if (WSAGetLastError() == WSAENOTSOCK) {
				SetFilePointerEx(file_handle, file_pos, NULL, FILE_BEGIN);
			}
		}
	}

	return php_io_win_copy_loop(
			(void *) file_handle, php_io_win_read_handle,
			(void *)(uintptr_t) dest_sock, php_io_win_write_socket,
			maxlen);
}

static ssize_t php_io_win_copy_socket_to_handle(SOCKET src_sock, int dest_fd, size_t maxlen)
{
	HANDLE dest_handle = (HANDLE) _get_osfhandle(dest_fd);

	if (dest_handle == INVALID_HANDLE_VALUE) {
		return -1;
	}

	return php_io_win_copy_loop(
			(void *)(uintptr_t) src_sock, php_io_win_read_socket,
			(void *) dest_handle, php_io_win_write_handle,
			maxlen);
}

static ssize_t php_io_win_copy_socket_to_socket(SOCKET src_sock, SOCKET dest_sock, size_t maxlen)
{
	return php_io_win_copy_loop(
			(void *)(uintptr_t) src_sock, php_io_win_read_socket,
			(void *)(uintptr_t) dest_sock, php_io_win_write_socket,
			maxlen);
}

ssize_t php_io_windows_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen)
{
	if (src->fd_type == PHP_IO_FD_SOCKET && dest->fd_type == PHP_IO_FD_SOCKET) {
		return php_io_win_copy_socket_to_socket(src->socket, dest->socket, maxlen);
	}

	if (src->fd_type == PHP_IO_FD_SOCKET) {
		return php_io_win_copy_socket_to_handle(src->socket, dest->fd, maxlen);
	}

	if (dest->fd_type == PHP_IO_FD_SOCKET) {
		return php_io_win_copy_handle_to_socket(src->fd, dest->socket, maxlen);
	}

	return php_io_win_copy_handle_to_handle(src->fd, dest->fd, maxlen);
}

#endif /* PHP_WIN32 */
