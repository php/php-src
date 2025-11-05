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

ssize_t php_io_windows_copy_file_to_file(int src_fd, int dest_fd, size_t maxlen)
{
	/* Use ReadFile/WriteFile for file-to-file copying */
	HANDLE src_handle = (HANDLE) _get_osfhandle(src_fd);
	HANDLE dest_handle = (HANDLE) _get_osfhandle(dest_fd);

	if (src_handle != INVALID_HANDLE_VALUE && dest_handle != INVALID_HANDLE_VALUE) {
		char buffer[65536];
		DWORD total_copied = 0;
		DWORD remaining = (maxlen == PHP_IO_COPY_ALL) ? MAXDWORD : (DWORD) min(maxlen, MAXDWORD);

		while (remaining > 0) {
			DWORD to_read = min(sizeof(buffer), remaining);
			DWORD bytes_read, bytes_written;

			if (!ReadFile(src_handle, buffer, to_read, &bytes_read, NULL)) {
				/* Read error */
				return total_copied > 0 ? (ssize_t) total_copied : -1;
			}

			if (bytes_read == 0) {
				/* EOF */
				return (ssize_t) total_copied;
			}

			if (!WriteFile(dest_handle, buffer, bytes_read, &bytes_written, NULL)) {
				/* Write error */
				return total_copied > 0 ? (ssize_t) total_copied : -1;
			}

			total_copied += bytes_written;
			if (maxlen != PHP_IO_COPY_ALL) {
				remaining -= bytes_written;
			}

			if (bytes_written != bytes_read) {
				/* Partial write */
				return (ssize_t) total_copied;
			}
		}

		return (ssize_t) total_copied;
	}

	/* Fallback to generic implementation */
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

ssize_t php_io_windows_copy_file_to_generic(int src_fd, int dest_fd, size_t maxlen)
{
	/* Use TransmitFile for zero-copy file to socket transfer */
	HANDLE file_handle = (HANDLE) _get_osfhandle(src_fd);
	SOCKET sock = (SOCKET) dest_fd;

	if (file_handle != INVALID_HANDLE_VALUE && sock != INVALID_SOCKET) {
		/* TransmitFile can send entire file or partial */
		DWORD bytes_to_send = (maxlen == PHP_IO_COPY_ALL) ? 0 : (DWORD) min(maxlen, MAXDWORD);

		if (TransmitFile(sock, file_handle, bytes_to_send, 0, NULL, NULL, 0)) {
			/* TransmitFile succeeded - but we don't know exactly how much was sent without extra
			 * syscalls */
			/* For simplicity, assume the requested amount was sent */
			return (maxlen == PHP_IO_COPY_ALL) ? 0 : (ssize_t) bytes_to_send;
		}

		/* TransmitFile failed, check if it's a recoverable error */
		int error = WSAGetLastError();
		if (error == WSAENOTSOCK) {
			/* dest_fd is not a socket, fall back to generic copy */
		}
	}

	/* Fallback to generic implementation */
	return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
}

#endif /* PHP_WIN32 */
