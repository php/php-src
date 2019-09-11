/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_INHERITANCE_H
#define ZEND_INHERITANCE_H

#include "zend.h"

BEGIN_EXTERN_C()

ZEND_API void zend_do_implement_interface(zend_class_entry *ce, zend_class_entry *iface);
ZEND_API void zend_do_inheritance_ex(zend_class_entry *ce, zend_class_entry *parent_ce, zend_bool checked);

#define zend_do_inheritance(ce, parent_ce) \
	zend_do_inheritance_ex(ce, parent_ce, 0)

ZEND_API int zend_do_link_class(zend_class_entry *ce, zend_string *lc_parent_name);

void zend_verify_abstract_class(zend_class_entry *ce);
void zend_check_deprecated_constructor(const zend_class_entry *ce);
void zend_build_properties_info_table(zend_class_entry *ce);
zend_bool zend_try_early_bind(zend_class_entry *ce, zend_class_entry *parent_ce, zend_string *lcname, zval *delayed_early_binding);

END_EXTERN_C()

#endif
