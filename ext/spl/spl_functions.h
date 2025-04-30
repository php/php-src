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
