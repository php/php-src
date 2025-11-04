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

/* Copy operations */
zend_result php_io_windows_copy_file_to_file(int src_fd, int dest_fd, size_t len, size_t *copied);
zend_result php_io_windows_copy_file_to_generic(
		int src_fd, int dest_fd, size_t len, size_t *copied);

/* Instance initialization macros */
#define PHP_IO_PLATFORM_COPY_OPS \
	{ \
		.file_to_file = php_io_windows_copy_file_to_file, \
		.file_to_generic = php_io_windows_copy_file_to_generic, \
		.generic_to_file = php_io_generic_copy_fallback, \
		.generic_to_generic = php_io_generic_copy_fallback, \
	}

#define PHP_IO_PLATFORM_NAME "windows"

#endif /* PHP_IO_WINDOWS_H */
