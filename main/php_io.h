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

#ifndef PHP_IO_H
#define PHP_IO_H

#include "php.h"

/* Forward declarations */
typedef struct php_io php_io;

/* File descriptor types */
typedef enum {
	PHP_IO_FD_FILE, /* Regular file - can use optimized file operations */
	PHP_IO_FD_GENERIC, /* Socket, pipe, or other - use generic operations */
} php_io_fd_type;

/* Synchronous copy operations vtable */
typedef struct php_io_copy_ops {
	zend_result (*file_to_file)(int src_fd, int dest_fd, size_t len, size_t *copied);
	zend_result (*file_to_generic)(int src_fd, int dest_fd, size_t len, size_t *copied);
	zend_result (*generic_to_file)(int src_fd, int dest_fd, size_t len, size_t *copied);
	zend_result (*generic_to_generic)(int src_fd, int dest_fd, size_t len, size_t *copied);
} php_io_copy_ops;

/* Main php_io structure */
typedef struct php_io {
	php_io_copy_ops copy;
	const char *platform_name;
} php_io;

/* IO struct accessor functions */
PHPAPI php_io *php_io_get(void);

/* Utility functions */
PHPAPI php_io_fd_type php_io_detect_fd_type(int fd);

/* High-level copy function - automatically selects best method based on fd types */
PHPAPI zend_result php_io_copy(int src_fd, php_io_fd_type src_type, int dest_fd,
		php_io_fd_type dest_type, size_t len, size_t *copied);

#endif /* PHP_IO_H */
