/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: PHP Development Team                                         |
   +----------------------------------------------------------------------+
*/

#include "php_io_internal.h"

#ifdef PHP_WIN32

#include <io.h>
#include <winsock2.h>
#include <mswsock.h>

static zend_result php_io_copy_windows_file_to_file(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    /* Try CopyFileEx for optimal file-to-file copying */
    HANDLE src_handle = (HANDLE)_get_osfhandle(src_fd);
    HANDLE dest_handle = (HANDLE)_get_osfhandle(dest_fd);
    
    if (src_handle != INVALID_HANDLE_VALUE && dest_handle != INVALID_HANDLE_VALUE) {
        /* Get source file size to determine copy length */
        LARGE_INTEGER file_size;
        if (GetFileSizeEx(src_handle, &file_size)) {
            DWORD bytes_to_copy = (DWORD)min(len, (size_t)file_size.QuadPart);
            
            /* Use ReadFile/WriteFile for partial copies since CopyFileEx copies entire files */
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

static zend_result php_io_copy_windows_file_to_socket(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    /* Use TransmitFile for zero-copy file to socket transfer */
    HANDLE file_handle = (HANDLE)_get_osfhandle(src_fd);
    SOCKET sock = (SOCKET)dest_fd;
    
    if (file_handle != INVALID_HANDLE_VALUE && sock != INVALID_SOCKET) {
        /* TransmitFile can send entire file or partial */
        DWORD bytes_to_send = (DWORD)len;
        
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

static zend_result php_io_copy_windows_socket_to_fd(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    /* No Windows-specific optimization for socket to fd */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

void php_io_register_copy(php_io_copy_ops *ops, uint32_t *capabilities)
{
    ops->file_to_file = php_io_copy_windows_file_to_file;
    ops->file_to_socket = php_io_copy_windows_file_to_socket;
    ops->socket_to_fd = php_io_copy_windows_socket_to_fd;
    
    *capabilities |= PHP_IO_CAP_ZERO_COPY | PHP_IO_CAP_SENDFILE;
}

#endif /* PHP_WIN32 */
