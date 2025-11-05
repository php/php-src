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

#ifndef PHP_IO_INTERNAL_H
#define PHP_IO_INTERNAL_H

#include "php_io.h"

/* Internal utility functions */
ssize_t php_io_generic_copy_fallback(int src_fd, int dest_fd, size_t maxlen);

/* Platform-specific headers */
#ifdef __linux__
#include "php_io_linux.h"
#elif defined(PHP_WIN32)
#include "php_io_windows.h"
#elif defined(__APPLE__)
#include "php_io_macos.h"
#elif defined(__sun)
#include "php_io_solaris.h"
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include "php_io_bsd.h"
#else
#include "php_io_generic.h"
#endif

#endif /* PHP_IO_INTERNAL_H */
