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

#ifndef PHP_IO_GENERIC_H
#define PHP_IO_GENERIC_H

/* Instance initialization macros - all use the generic fallback */
#define PHP_IO_PLATFORM_COPY_OPS \
	{ \
		.file_to_file = php_io_generic_copy_fallback, \
		.file_to_generic = php_io_generic_copy_fallback, \
		.generic_to_file = php_io_generic_copy_fallback, \
		.generic_to_generic = php_io_generic_copy_fallback, \
	}

#define PHP_IO_PLATFORM_NAME "generic"

#endif /* PHP_IO_GENERIC_H */