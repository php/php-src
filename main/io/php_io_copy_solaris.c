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

#ifdef __sun

#include "php_io_internal.h"
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_SENDFILEV
#include <sys/sendfile.h>
#endif

static zend_result php_io_copy_solaris_file_to_file(int src_fd, int dest_fd, size_t len, size_t *copied)
{
#ifdef HAVE_SENDFILEV
    /* Solaris sendfilev can potentially do file-to-file transfers */
    /* But it's complex and mainly designed for file-to-socket */
    /* For now, use generic implementation */
#endif
    
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

static zend_result php_io_copy_solaris_file_to_socket(int src_fd, int dest_fd, size_t len, size_t *copied)
{
#ifdef HAVE_SENDFILEV
    /* Solaris sendfilev - very powerful but complex API */
    struct sendfilevec sfv;
    size_t xferred = 0;
    
    /* Set up the sendfile vector */
    sfv.sfv_fd = src_fd;           /* Source file descriptor */
    sfv.sfv_flag = SFV_FD;         /* sfv_fd is a file descriptor */
    sfv.sfv_off = 0;               /* Offset in the file */
    sfv.sfv_len = len;             /* Number of bytes to send */
    
    /* Perform the sendfile operation */
    ssize_t result = sendfilev(dest_fd, &sfv, 1, &xferred);
    
    if (result == 0) {
        /* Success - all data transferred */
        *copied = xferred;
        return SUCCESS;
    } else if (result == -1) {
        /* Error occurred */
        switch (errno) {
            case EAGAIN:
                /* Would block - partial transfer possible */
                if (xferred > 0) {
                    *copied = xferred;
                    return SUCCESS;
                }
                break;
            case EINVAL:
                /* Invalid arguments */
                break;
            case ENOTCONN:
                /* Socket not connected */
                break;
            case EPIPE:
                /* Broken pipe */
                break;
            case EAFNOSUPPORT:
                /* Address family not supported */
                break;
            default:
                /* Other errors */
                break;
        }
        
        /* Even on error, some data might have been transferred */
        if (xferred > 0) {
            *copied = xferred;
            return SUCCESS;
        }
    }
#endif /* HAVE_SENDFILEV */
    
    /* Fallback to generic implementation */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

static zend_result php_io_copy_solaris_socket_to_fd(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    /* No Solaris-specific optimization for socket to fd */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

void php_io_register_copy(php_io_copy_ops *ops, uint32_t *capabilities)
{
    ops->file_to_file = php_io_copy_solaris_file_to_file;
    ops->file_to_socket = php_io_copy_solaris_file_to_socket;
    ops->socket_to_fd = php_io_copy_solaris_socket_to_fd;
    
#ifdef HAVE_SENDFILEV
    *capabilities |= PHP_IO_CAP_SENDFILE | PHP_IO_CAP_ZERO_COPY;
#endif
}

#endif /* __sun */
