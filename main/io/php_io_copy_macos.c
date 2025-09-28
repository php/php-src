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

#ifdef __APPLE__

#include "php_io_internal.h"
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_SENDFILE
#include <sys/socket.h>
#include <sys/uio.h>
#endif

#ifdef HAVE_COPYFILE
#include <copyfile.h>
#endif

static zend_result php_io_copy_macos_file_to_file(int src_fd, int dest_fd, size_t len, size_t *copied)
{
#ifdef HAVE_COPYFILE
    /* macOS copyfile() can be used, but it's designed for whole files */
    /* For partial copies, it's complex to use properly */
    /* TODO: Could implement copyfile() for whole-file copies in the future */
#endif
    
    /* For now, use generic implementation for file-to-file */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

static zend_result php_io_copy_macos_file_to_socket(int src_fd, int dest_fd, size_t len, size_t *copied)
{
#ifdef HAVE_SENDFILE
    /* macOS sendfile signature: sendfile(fd, s, offset, len, hdtr, flags) */
    /* Note: len is passed by reference and updated with bytes sent */
    off_t len_sent = len;
    int result = sendfile(src_fd, dest_fd, 0, &len_sent, NULL, 0);
    
    if (result == 0) {
        /* Success */
        *copied = len_sent;
        return SUCCESS;
    } else if (result == -1) {
        /* Error occurred */
        switch (errno) {
            case EAGAIN:
                /* Would block - could be partial send */
                if (len_sent > 0) {
                    *copied = len_sent;
                    return SUCCESS;
                }
                break;
            case EINVAL:
                /* Invalid arguments - likely not a regular file or socket */
                break;
            case ENOTCONN:
                /* Socket not connected */
                break;
            case EPIPE:
                /* Broken pipe */
                break;
            default:
                /* Other errors */
                break;
        }
    }
#endif /* HAVE_SENDFILE */
    
    /* Fallback to generic implementation */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

static zend_result php_io_copy_macos_socket_to_fd(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    /* No macOS-specific optimization for socket to fd */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

void php_io_register_copy(php_io_copy_ops *ops, uint32_t *capabilities)
{
    ops->file_to_file = php_io_copy_macos_file_to_file;
    ops->file_to_socket = php_io_copy_macos_file_to_socket;
    ops->socket_to_fd = php_io_copy_macos_socket_to_fd;
    
#ifdef HAVE_SENDFILE
    *capabilities |= PHP_IO_CAP_SENDFILE | PHP_IO_CAP_ZERO_COPY;
#endif

#ifdef HAVE_COPYFILE
    /* Could add a capability flag for copyfile support */
#endif
}

#endif /* __APPLE__ */
