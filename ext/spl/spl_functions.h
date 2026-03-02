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

/* sub: whether to allow subclasses/interfaces
   allow = 0: allow all classes and interfaces
   allow > 0: allow all that match and mask ce_flags
   allow < 0: disallow all that match and mask ce_flags
 */
void spl_add_class_name(zval * list, zend_class_entry * pce, int allow, int ce_flags);
void spl_add_interfaces(zval * list, zend_class_entry * pce, int allow, int ce_flags);
void spl_add_traits(zval * list, zend_class_entry * pce, int allow, int ce_flags);
void spl_add_classes(zend_class_entry *pce, zval *list, bool sub, int allow, int ce_flags);

void spl_set_private_debug_info_property(const zend_class_entry *ce, const char *property, size_t property_len, HashTable *debug_info, zval *value);

#endif /* PHP_FUNCTIONS_H */
