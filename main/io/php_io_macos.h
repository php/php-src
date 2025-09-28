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
   | Authors: David Carlier <devnexen@gmail.com>                          |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_IO_MACOS_H
#define PHP_IO_MACOS_H

#define PHP_IO_PLATFORM_COPY php_io_macos_copy
#define PHP_IO_PLATFORM_NAME "macos"

zend_result php_io_macos_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen, size_t *copied);

#endif /* PHP_IO_MACOS_H */
