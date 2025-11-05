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

/* Copy as much as possible */
#define PHP_IO_COPY_ALL SIZE_MAX

/* Synchronous copy operations vtable */
typedef struct php_io_copy_ops {
	ssize_t (*file_to_file)(int src_fd, int dest_fd, size_t maxlen);
	ssize_t (*file_to_generic)(int src_fd, int dest_fd, size_t maxlen);
	ssize_t (*generic_to_file)(int src_fd, int dest_fd, size_t maxlen);
	ssize_t (*generic_to_generic)(int src_fd, int dest_fd, size_t maxlen);
} php_io_copy_ops;

/* Main php_io structure */
typedef struct php_io {
	php_io_copy_ops copy;
	const char *platform_name;
} php_io;

/* IO struct accessor function */
PHPAPI php_io *php_io_get(void);

/* High-level copy function - automatically selects best method based on fd types
 * Copies up to maxlen bytes from src_fd to dest_fd
 * If maxlen is PHP_IO_COPY_ALL, copies until EOF
 *
 * Returns:
 *   >= 0 - number of bytes copied (may be less than maxlen if EOF reached)
 *   -1   - I/O error occurred (errno is set)
 */
PHPAPI ssize_t php_io_copy(
		int src_fd, php_io_fd_type src_type, int dest_fd, php_io_fd_type dest_type, size_t maxlen);

#endif /* PHP_IO_H */
