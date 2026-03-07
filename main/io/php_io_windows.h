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

#ifndef PHP_IO_WINDOWS_H
#define PHP_IO_WINDOWS_H

ssize_t php_io_windows_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen);

#define PHP_IO_PLATFORM_COPY php_io_windows_copy
#define PHP_IO_PLATFORM_NAME "windows"

#endif /* PHP_IO_WINDOWS_H */
