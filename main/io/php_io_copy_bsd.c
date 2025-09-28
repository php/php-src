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

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

#include "php_io_internal.h"
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_SENDFILE
#include <sys/socket.h>
#include <sys/uio.h>
#endif

static zend_result php_io_copy_bsd_file_to_file(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    /* BSD variants don't have a special file-to-file copy optimization */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

static zend_result php_io_copy_bsd_file_to_socket(int src_fd, int dest_fd, size_t len, size_t *copied)
{
#ifdef HAVE_SENDFILE
    /* BSD sendfile signature: sendfile(fd, s, offset, nbytes, hdtr, sbytes, flags) */
    /* This signature is shared by FreeBSD, OpenBSD, and NetBSD */
    off_t sbytes = 0;
    int result = sendfile(src_fd, dest_fd, 0, len, NULL, &sbytes, 0);
    
    if (result == 0) {
        /* Success - entire amount was sent */
        *copied = len;
        return SUCCESS;
    } else if (result == -1 && sbytes > 0) {
        /* Partial send - some data was transferred */
        *copied = sbytes;
        return SUCCESS;
    } else if (result == -1) {
        /* Error occurred */
        switch (errno) {
            case EAGAIN:
            case EBUSY:
                /* Would block or busy - could retry, but fall back for now */
                break;
            case EINVAL:
                /* Invalid arguments - likely not a regular file or socket */
                break;
            case ENOTCONN:
                /* Socket not connected */
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

static zend_result php_io_copy_bsd_socket_to_fd(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    /* No BSD-specific optimization for socket to fd */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

void php_io_register_copy(php_io_copy_ops *ops, uint32_t *capabilities)
{
    ops->file_to_file = php_io_copy_bsd_file_to_file;
    ops->file_to_socket = php_io_copy_bsd_file_to_socket;
    ops->socket_to_fd = php_io_copy_bsd_socket_to_fd;
    
#ifdef HAVE_SENDFILE
    *capabilities |= PHP_IO_CAP_SENDFILE | PHP_IO_CAP_ZERO_COPY;
#endif
}

#endif /* FreeBSD, OpenBSD, NetBSD */
