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
#include "php_io_hooks.h"
#include "php_io_ring.h"

#define PHP_IO_COPY_ALL SIZE_MAX

typedef enum php_io_fd_type {
	PHP_IO_FD_FILE = 1,
	PHP_IO_FD_SOCKET,
	PHP_IO_FD_PIPE,
} php_io_fd_type;

typedef struct php_io_fd {
	union {
		int fd;
		php_socket_t socket;
	};
	php_io_fd_type fd_type;
	struct timeval timeout;
	unsigned is_blocked:1;
	/* Windows: the position of a file opened overlapped, which the kernel keeps none for; the
	 * stream owns it and the copy moves it. NULL for a descriptor with a file position. */
	zend_off_t *position;
} php_io_fd;

typedef zend_result (*php_io_copy_fn)(php_io_fd *src, php_io_fd *dest, size_t maxlen, size_t *copied);

typedef struct php_io {
	php_io_copy_fn copy;
	const char *platform_name;
} php_io;

PHPAPI php_io *php_io_get(void);

/* Copies up to maxlen bytes from src to dest; *copied is set even on FAILURE */
PHPAPI zend_result php_io_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen, size_t *copied);

/* A pipe between the process and a child, as proc_open() makes it */
#ifdef PHP_WIN32
typedef HANDLE php_io_pipe_handle;
#else
typedef int php_io_pipe_handle;
#endif
/* pair as pipe() fills it, the read end then the write end; parent_reads says which is the
 * process's. *overlapped: Windows made the process's end an overlapped named pipe, whose ops a
 * provider performs (one is installed, or PHP_IO_OVERLAPPED_PIPES=1 is set). errno on failure. */
PHPAPI zend_result php_io_pipe_create(php_io_pipe_handle pair[2], bool parent_reads, bool *overlapped);
/* The stream over the process's end; mode_flags as open() takes them (O_RDONLY or O_WRONLY, and
 * O_BINARY on Windows) */
PHPAPI php_stream *php_io_pipe_stream(php_io_pipe_handle end, int mode_flags, const char *mode, bool overlapped);
/* A stream whose descriptor a child is about to get: the queues let go of it, so the child can
 * tie it to its own completion port (Windows; nothing elsewhere). FAILURE with an exception thrown
 * when a call of this process has an op on the stream. No PHP code may run between this and the
 * child's creation. */
PHPAPI zend_result php_io_pipe_stream_release(php_stream *stream);
/* ...and once the child has it: the process's own calls on an overlapped pipe stop using a
 * provider, whose completion the child may have turned off for the file object */
PHPAPI void php_io_pipe_stream_handed_out(php_stream *stream);

#endif /* PHP_IO_H */
