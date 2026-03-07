/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_IO_H
#define PHP_IO_H

#include "php.h"
#include "php_network.h"

#define PHP_IO_COPY_ALL SIZE_MAX

#define PHP_IO_FD_FILE   1
#define PHP_IO_FD_SOCKET 2
#define PHP_IO_FD_PIPE   3

typedef struct {
	union {
		int fd;
		php_socket_t socket;
	};
	int fd_type;
} php_io_fd;

typedef ssize_t (*php_io_copy_fn)(php_io_fd *src, php_io_fd *dest, size_t maxlen);

typedef struct php_io {
	php_io_copy_fn copy;
	const char *platform_name;
} php_io;

PHPAPI php_io *php_io_get(void);

/* Returns bytes copied on success, -1 on error */
PHPAPI ssize_t php_io_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen);

#endif /* PHP_IO_H */
