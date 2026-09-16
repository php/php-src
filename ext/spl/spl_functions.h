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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_FUNCTIONS_H
#define PHP_FUNCTIONS_H

#include "php.h"

void spl_set_private_debug_info_property(const zend_class_entry *ce, const char *property, size_t property_len, HashTable *debug_info, zval *value);

#endif /* PHP_FUNCTIONS_H */
