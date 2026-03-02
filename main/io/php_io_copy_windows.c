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

/* Read from a socket using recv() */
static inline ssize_t php_io_windows_socket_read(SOCKET sock, char *buf, size_t len)
{
	int to_recv = (len > INT_MAX) ? INT_MAX : (int) len;
	int result = recv(sock, buf, to_recv, 0);
	if (result == SOCKET_ERROR) {
		return -1;
	}
	return (ssize_t) result;
}

/* Write to a socket using send() */
static inline ssize_t php_io_windows_socket_write(SOCKET sock, const char *buf, size_t len)
{
	int to_send = (len > INT_MAX) ? INT_MAX : (int) len;
	int result = send(sock, buf, to_send, 0);
	if (result == SOCKET_ERROR) {
		return -1;
	}
	return (ssize_t) result;
}

/* Read from a file HANDLE using ReadFile() */
static inline ssize_t php_io_windows_file_read(HANDLE handle, char *buf, size_t len)
{
	DWORD to_read = (len > MAXDWORD) ? MAXDWORD : (DWORD) len;
	DWORD bytes_read;
	if (!ReadFile(handle, buf, to_read, &bytes_read, NULL)) {
		return -1;
	}
	return (ssize_t) bytes_read;
}

/* Write to a file HANDLE using WriteFile() */
static inline ssize_t php_io_windows_file_write(HANDLE handle, const char *buf, size_t len)
{
	DWORD to_write = (len > MAXDWORD) ? MAXDWORD : (DWORD) len;
	DWORD bytes_written;
	if (!WriteFile(handle, buf, to_write, &bytes_written, NULL)) {
		return -1;
	}
	return (ssize_t) bytes_written;
}

/* Generic copy loop parameterized by read/write function pointers */
typedef ssize_t (*php_io_windows_read_fn)(void *handle, char *buf, size_t len);
typedef ssize_t (*php_io_windows_write_fn)(void *handle, const char *buf, size_t len);

static ssize_t php_io_windows_copy_loop(
		void *src_handle, php_io_windows_read_fn read_fn,
		void *dest_handle, php_io_windows_write_fn write_fn,
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

/* Wrapper functions to match the generic function pointer signatures */
static ssize_t php_io_windows_read_file(void *handle, char *buf, size_t len)
{
	return php_io_windows_file_read((HANDLE) handle, buf, len);
}

static ssize_t php_io_windows_write_file(void *handle, const char *buf, size_t len)
{
	return php_io_windows_file_write((HANDLE) handle, buf, len);
}

static ssize_t php_io_windows_read_socket(void *handle, char *buf, size_t len)
{
	return php_io_windows_socket_read((SOCKET)(uintptr_t) handle, buf, len);
}

static ssize_t php_io_windows_write_socket(void *handle, const char *buf, size_t len)
{
	return php_io_windows_socket_write((SOCKET)(uintptr_t) handle, buf, len);
}

ssize_t php_io_windows_copy_file_to_file(int src_fd, int dest_fd, size_t maxlen)
{
	HANDLE src_handle = (HANDLE) _get_osfhandle(src_fd);
	HANDLE dest_handle = (HANDLE) _get_osfhandle(dest_fd);

	if (src_handle == INVALID_HANDLE_VALUE || dest_handle == INVALID_HANDLE_VALUE) {
		return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
	}

	return php_io_windows_copy_loop(
			(void *) src_handle, php_io_windows_read_file,
			(void *) dest_handle, php_io_windows_write_file,
			maxlen);
}

ssize_t php_io_windows_copy_file_to_generic(int src_fd, int dest_fd, size_t maxlen)
{
	HANDLE file_handle = (HANDLE) _get_osfhandle(src_fd);
	SOCKET sock = (SOCKET) dest_fd;

	if (file_handle == INVALID_HANDLE_VALUE) {
		return -1;
	}

	/* Try TransmitFile for zero-copy transfer first */
	if (sock != INVALID_SOCKET) {
		LARGE_INTEGER file_size;
		LARGE_INTEGER file_pos;

		/* Get current file position to calculate bytes available */
		file_pos.QuadPart = 0;
		if (SetFilePointerEx(file_handle, file_pos, &file_pos, FILE_CURRENT)) {
			DWORD bytes_to_send;

			if (maxlen == PHP_IO_COPY_ALL) {
				if (GetFileSizeEx(file_handle, &file_size)) {
					LONGLONG available = file_size.QuadPart - file_pos.QuadPart;
					bytes_to_send = (available > MAXDWORD) ? 0 : (DWORD) available;
				} else {
					bytes_to_send = 0; /* Let TransmitFile send everything */
				}
			} else {
				bytes_to_send = (DWORD) min(maxlen, MAXDWORD);
			}

			if (TransmitFile(sock, file_handle, bytes_to_send, 0, NULL, NULL, 0)) {
				/* For COPY_ALL with bytes_to_send=0, we need to figure out how much was sent */
				if (bytes_to_send == 0 && maxlen == PHP_IO_COPY_ALL) {
					LARGE_INTEGER new_pos;
					LARGE_INTEGER zero = {0};
					if (SetFilePointerEx(file_handle, zero, &new_pos, FILE_CURRENT)) {
						return (ssize_t)(new_pos.QuadPart - file_pos.QuadPart);
					}
					/* Can't determine size, but succeeded */
					return 0;
				}
				return (ssize_t) bytes_to_send;
			}

			/* TransmitFile failed - check if dest is actually a socket */
			if (WSAGetLastError() == WSAENOTSOCK) {
				/* Reset file position for fallback */
				SetFilePointerEx(file_handle, file_pos, NULL, FILE_BEGIN);
			}
		}
	}

	/* Fallback: file read → socket send */
	return php_io_windows_copy_loop(
			(void *) file_handle, php_io_windows_read_file,
			(void *)(uintptr_t) sock, php_io_windows_write_socket,
			maxlen);
}

ssize_t php_io_windows_copy_generic_to_file(int src_fd, int dest_fd, size_t maxlen)
{
	HANDLE dest_handle = (HANDLE) _get_osfhandle(dest_fd);
	SOCKET sock = (SOCKET) src_fd;

	if (dest_handle == INVALID_HANDLE_VALUE) {
		return -1;
	}

	return php_io_windows_copy_loop(
			(void *)(uintptr_t) sock, php_io_windows_read_socket,
			(void *) dest_handle, php_io_windows_write_file,
			maxlen);
}

ssize_t php_io_windows_copy_generic_to_generic(int src_fd, int dest_fd, size_t maxlen)
{
	SOCKET src_sock = (SOCKET) src_fd;
	SOCKET dest_sock = (SOCKET) dest_fd;

	return php_io_windows_copy_loop(
			(void *)(uintptr_t) src_sock, php_io_windows_read_socket,
			(void *)(uintptr_t) dest_sock, php_io_windows_write_socket,
			maxlen);
}

#endif /* PHP_WIN32 */
