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

/* Linux-specific includes */
#ifdef HAVE_COPY_FILE_RANGE
#include <sys/syscall.h>
#endif

#ifdef HAVE_SENDFILE
#include <sys/sendfile.h>
#endif

/* Forward declaration for ring registration function */
php_io_ring_ops* php_io_ring_register_linux(void);

/* copy_file_range wrapper for older systems */
#ifdef HAVE_COPY_FILE_RANGE
static ssize_t copy_file_range_wrapper(int fd_in, off_t *off_in, int fd_out, off_t *off_out, 
                                      size_t len, unsigned int flags)
{
    return syscall(SYS_copy_file_range, fd_in, off_in, fd_out, off_out, len, flags);
}
#endif

zend_result php_io_copy_linux_file_to_file(int src_fd, int dest_fd, size_t len, size_t *copied)
{
#ifdef HAVE_COPY_FILE_RANGE
    /* Try copy_file_range first - kernel-level optimization */
    ssize_t result = copy_file_range_wrapper(src_fd, NULL, dest_fd, NULL, len, 0);
    
    if (result > 0) {
        *copied = (size_t)result;
        return SUCCESS;
    }
    
    /* If copy_file_range fails, fall through to generic implementation */
    if (result == -1) {
        switch (errno) {
            case EINVAL:
            case EXDEV:
            case ENOSYS:
                /* Expected failures - fall back to generic copy */
                break;
            default:
                /* Unexpected error */
                *copied = 0;
                return FAILURE;
        }
    }
#endif
    
    /* Fallback to generic read/write loop */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

zend_result php_io_copy_linux_file_to_socket(int src_fd, int dest_fd, size_t len, size_t *copied)
{
#ifdef HAVE_SENDFILE
    /* Use sendfile for zero-copy file to socket transfer */
    off_t offset = 0;
    ssize_t result = sendfile(dest_fd, src_fd, &offset, len);
    
    if (result > 0) {
        *copied = (size_t)result;
        return SUCCESS;
    }
    
    /* Handle partial sends and errors */
    if (result == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            /* Would block - for now, fall back to generic copy */
            /* TODO: Could implement epoll-based retry here */
        }
        /* Other errors fall through to generic copy */
    }
#endif
    
    /* Fallback to generic read/write loop */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

zend_result php_io_copy_linux_socket_to_fd(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    /* No Linux-specific optimization for socket to fd */
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

void php_io_register_copy(php_io_copy_ops *ops, uint32_t *capabilities)
{
    ops->file_to_file = php_io_copy_linux_file_to_file;
    ops->file_to_socket = php_io_copy_linux_file_to_socket;
    ops->socket_to_fd = php_io_copy_linux_socket_to_fd;
    
    *capabilities |= PHP_IO_CAP_ZERO_COPY | PHP_IO_CAP_SENDFILE;
}

#endif /* __linux__ */
