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

zend_result php_io_windows_copy_file_to_file(int src_fd, int dest_fd, size_t len, size_t *copied)
{
	/* Use ReadFile/WriteFile for file-to-file copying */
	HANDLE src_handle = (HANDLE) _get_osfhandle(src_fd);
	HANDLE dest_handle = (HANDLE) _get_osfhandle(dest_fd);

	if (src_handle != INVALID_HANDLE_VALUE && dest_handle != INVALID_HANDLE_VALUE) {
		/* Get source file size to determine copy length */
		LARGE_INTEGER file_size;
		if (GetFileSizeEx(src_handle, &file_size)) {
			DWORD bytes_to_copy = (DWORD) min(len, (size_t) file_size.QuadPart);

			/* Use ReadFile/WriteFile for partial copies */
			char buffer[65536];
			DWORD total_copied = 0;

			while (total_copied < bytes_to_copy) {
				DWORD to_read = min(sizeof(buffer), bytes_to_copy - total_copied);
				DWORD bytes_read, bytes_written;

				if (!ReadFile(src_handle, buffer, to_read, &bytes_read, NULL)) {
					break;
				}

				if (bytes_read == 0) {
					break; /* EOF */
				}

				if (!WriteFile(dest_handle, buffer, bytes_read, &bytes_written, NULL)) {
					break;
				}

				total_copied += bytes_written;

				if (bytes_written != bytes_read) {
					break; /* Partial write */
				}
			}

			*copied = total_copied;
			return total_copied > 0 ? SUCCESS : FAILURE;
		}
	}

	/* Fallback to generic implementation */
	return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

zend_result php_io_windows_copy_file_to_generic(int src_fd, int dest_fd, size_t len, size_t *copied)
{
	/* Use TransmitFile for zero-copy file to socket transfer */
	HANDLE file_handle = (HANDLE) _get_osfhandle(src_fd);
	SOCKET sock = (SOCKET) dest_fd;

	if (file_handle != INVALID_HANDLE_VALUE && sock != INVALID_SOCKET) {
		/* TransmitFile can send entire file or partial */
		DWORD bytes_to_send = (DWORD) len;

		if (TransmitFile(sock, file_handle, bytes_to_send, 0, NULL, NULL, 0)) {
			*copied = bytes_to_send;
			return SUCCESS;
		}

		/* TransmitFile failed, check if it's a recoverable error */
		int error = WSAGetLastError();
		if (error == WSAENOTSOCK) {
			/* dest_fd is not a socket, fall back to generic copy */
		} else {
			/* Other TransmitFile error, could be network related */
		}
	}

	/* Fallback to generic implementation */
	return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

#endif /* PHP_WIN32 */
