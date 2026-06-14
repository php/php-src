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
   | Authors: George Schlossnagle <george@omniti.com>                     |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_REFLECTION_H
#define PHP_REFLECTION_H

#include "php.h"
#include "php_reflection_decl.h"

extern zend_module_entry reflection_module_entry;
#define phpext_reflection_ptr &reflection_module_entry

#define PHP_REFLECTION_VERSION PHP_VERSION

BEGIN_EXTERN_C()

/* Class entry pointers */
extern PHPAPI zend_class_entry *reflector_ptr;
extern PHPAPI zend_class_entry *reflection_exception_ptr;
extern PHPAPI zend_class_entry *reflection_ptr;
extern PHPAPI zend_class_entry *reflection_function_abstract_ptr;
extern PHPAPI zend_class_entry *reflection_function_ptr;
extern PHPAPI zend_class_entry *reflection_parameter_ptr;
extern PHPAPI zend_class_entry *reflection_type_ptr;
extern PHPAPI zend_class_entry *reflection_named_type_ptr;
extern PHPAPI zend_class_entry *reflection_class_ptr;
extern PHPAPI zend_class_entry *reflection_object_ptr;
extern PHPAPI zend_class_entry *reflection_method_ptr;
extern PHPAPI zend_class_entry *reflection_property_ptr;
extern PHPAPI zend_class_entry *reflection_extension_ptr;
extern PHPAPI zend_class_entry *reflection_zend_extension_ptr;
extern PHPAPI zend_class_entry *reflection_reference_ptr;
extern PHPAPI zend_class_entry *reflection_attribute_ptr;
extern PHPAPI zend_class_entry *reflection_enum_ptr;
extern PHPAPI zend_class_entry *reflection_enum_unit_case_ptr;
extern PHPAPI zend_class_entry *reflection_enum_backed_case_ptr;
extern PHPAPI zend_class_entry *reflection_fiber_ptr;
extern PHPAPI zend_class_entry *reflection_lazy_object_ptr;

PHPAPI void zend_reflection_class_factory(zend_class_entry *ce, zval *object);

END_EXTERN_C()

#endif /* PHP_REFLECTION_H */
