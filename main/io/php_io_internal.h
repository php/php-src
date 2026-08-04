/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#ifndef PHP_IO_INTERNAL_H
#define PHP_IO_INTERNAL_H

#include "php_io.h"

/* Buffer size for the userspace read/write copy loops. Larger than the stream
 * layer CHUNK_SIZE (8 KiB) to cut the number of syscalls on bulk transfers. */
#define PHP_IO_COPY_BUFSIZE (64 * 1024)

zend_result php_io_generic_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen, size_t *copied);
zend_result php_io_generic_copy_fallback(int src_fd, int dest_fd, size_t maxlen, size_t *copied);

#ifdef __linux__
#include "php_io_linux.h"
#elif defined(PHP_WIN32)
#include "php_io_windows.h"
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#include "php_io_freebsd.h"
#elif defined(__sun) && defined(__SVR4)
#include "php_io_solaris.h"
#elif defined(__APPLE__)
#include "php_io_macos.h"
#else
#include "php_io_generic.h"
#endif

#endif /* PHP_IO_INTERNAL_H */
