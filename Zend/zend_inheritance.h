/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend by Perforce and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_INHERITANCE_H
#define ZEND_INHERITANCE_H

#include "zend.h"
#include "zend_compile.h"

BEGIN_EXTERN_C()

ZEND_API void zend_do_implement_interface(zend_class_entry *ce, zend_class_entry *iface);
ZEND_API void zend_do_inheritance_ex(zend_class_entry *ce, zend_class_entry *parent_ce, bool checked);

static zend_always_inline void zend_do_inheritance(zend_class_entry *ce, zend_class_entry *parent_ce) {
	zend_do_inheritance_ex(ce, parent_ce, 0);
}

ZEND_API zend_class_entry *zend_do_link_class(zend_class_entry *ce, zend_string *lc_parent_name, const zend_string *key);

void zend_verify_abstract_class(zend_class_entry *ce);
void zend_build_properties_info_table(zend_class_entry *ce);
ZEND_API zend_class_entry *zend_try_early_bind(zend_class_entry *ce, zend_class_entry *parent_ce, zend_string *lcname, zval *delayed_early_binding);

void zend_inheritance_check_override(const zend_class_entry *ce);

ZEND_API extern zend_class_entry* (*zend_inheritance_cache_get)(zend_class_entry *ce, zend_class_entry *parent, zend_class_entry **traits_and_interfaces);
ZEND_API extern zend_class_entry* (*zend_inheritance_cache_add)(zend_class_entry *ce, zend_class_entry *proto, zend_class_entry *parent, zend_class_entry **traits_and_interfaces, HashTable *dependencies);

typedef enum {
	INHERITANCE_UNRESOLVED = -1,
	INHERITANCE_ERROR = 0,
	INHERITANCE_WARNING = 1,
	INHERITANCE_SUCCESS = 2,
} zend_inheritance_status;

ZEND_API zend_inheritance_status zend_verify_property_hook_variance(const zend_property_info *prop_info, const zend_function *func);
ZEND_API ZEND_COLD ZEND_NORETURN void zend_hooked_property_variance_error(const zend_property_info *prop_info);
ZEND_API ZEND_COLD ZEND_NORETURN void zend_hooked_property_variance_error_ex(zend_string *value_param_name, zend_string *class_name, zend_string *prop_name);
ZEND_API void zend_verify_hooked_property(const zend_class_entry *ce, zend_property_info *prop_info, zend_string *prop_name);

END_EXTERN_C()

#endif
