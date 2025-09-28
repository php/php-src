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
zend_result php_io_generic_copy_fallback(int src_fd, int dest_fd, size_t len, size_t *copied);

/* Single registration functions - implemented once per platform file */
void php_io_register_copy(php_io_copy_ops *ops, uint32_t *capabilities);
void php_io_register_ring(php_io_ring_ops *ops, uint32_t *capabilities);


#endif /* PHP_IO_INTERNAL_H */
