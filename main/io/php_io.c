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

#include "php.h"
#include "php_io.h"
#include "php_io_internal.h"
#include <sys/stat.h>
#include <sys/socket.h>

#ifdef PHP_WIN32
#include <io.h>
#include <winsock2.h>
#else
#include <unistd.h>
#endif

/* Global instance pointer */
static php_io *g_php_io = NULL;

/* Global instance */
static php_io g_php_io_instance;
static zend_bool g_php_io_initialized = 0;

/* Factory function - selects best implementation for platform */
PHPAPI php_io* php_io_create(void)
{
    if (g_php_io_initialized) {
        return &g_php_io_instance;
    }
    
    /* Initialize copy operations */
    php_io_register_copy(&g_php_io_instance.copy, &g_php_io_instance.capabilities);
    
    /* Initialize ring operations */
    php_io_register_ring(&g_php_io_instance.ring, &g_php_io_instance.capabilities);
    
    /* Set platform name */
#ifdef __linux__
    g_php_io_instance.platform_name = "linux";
#elif defined(_WIN32)
    g_php_io_instance.platform_name = "windows";
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__APPLE__) || defined(__sun)
    g_php_io_instance.platform_name = "bsd";
#else
    g_php_io_instance.platform_name = "generic";
#endif
    
    g_php_io_initialized = 1;
    return &g_php_io_instance;
}

/* Get global instance (lazy initialization) */
PHPAPI php_io* php_io_get(void)
{
    if (g_php_io == NULL) {
        g_php_io = php_io_create();
    }
    return g_php_io;
}

/* Detect file descriptor type */
PHPAPI php_io_fd_type php_io_detect_fd_type(int fd)
{
    struct stat st;
    
    if (fstat(fd, &st) != 0) {
        return PHP_IO_FD_UNKNOWN;
    }
    
    if (S_ISREG(st.st_mode)) {
        return PHP_IO_FD_FILE;
    } else if (S_ISSOCK(st.st_mode)) {
        return PHP_IO_FD_SOCKET;
    } else if (S_ISFIFO(st.st_mode)) {
        return PHP_IO_FD_PIPE;
    }
    
    /* Additional socket detection for systems where S_ISSOCK doesn't work */
    int sock_type;
    socklen_t sock_type_len = sizeof(sock_type);
    if (getsockopt(fd, SOL_SOCKET, SO_TYPE, &sock_type, &sock_type_len) == 0) {
        return PHP_IO_FD_SOCKET;
    }
    
    return PHP_IO_FD_UNKNOWN;
}

/* Extract file descriptor and type from php_stream */
PHPAPI zend_result php_io_get_fd_and_type(php_stream *stream, int *fd, php_io_fd_type *type)
{
    if (php_stream_cast(stream, PHP_STREAM_AS_FD, (void**)fd, 0) != SUCCESS) {
        return FAILURE;
    }
    
    *type = php_io_detect_fd_type(*fd);
    return SUCCESS;
}

/* Generic read/write fallback implementation */
zend_result php_io_generic_copy_fallback(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    char buf[8192];
    size_t total_copied = 0;
    size_t remaining = len;
    
    while (remaining > 0 && total_copied < len) {
        size_t to_read = remaining < sizeof(buf) ? remaining : sizeof(buf);
        ssize_t bytes_read = read(src_fd, buf, to_read);
        
        if (bytes_read <= 0) {
            break; /* EOF or error */
        }
        
        ssize_t bytes_written = write(dest_fd, buf, bytes_read);
        if (bytes_written <= 0) {
            break; /* Error */
        }
        
        total_copied += bytes_written;
        remaining -= bytes_written;
        
        if (bytes_written != bytes_read) {
            break; /* Partial write */
        }
    }
    
    *copied = total_copied;
    return total_copied > 0 ? SUCCESS : FAILURE;
}
