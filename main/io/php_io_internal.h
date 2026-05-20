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

#ifndef PHP_IO_INTERNAL_H
#define PHP_IO_INTERNAL_H

#include "php_io.h"

ssize_t php_io_generic_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen);
ssize_t php_io_generic_copy_fallback(int src_fd, int dest_fd, size_t maxlen);

#ifdef __linux__
#include "php_io_linux.h"
#elif defined(PHP_WIN32)
#include "php_io_windows.h"
#else
#include "php_io_generic.h"
#endif

#endif /* PHP_IO_INTERNAL_H */
