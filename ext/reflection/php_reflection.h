/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: George Schlossnagle <george@omniti.com>                     |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_REFLECTION_H
#define PHP_REFLECTION_H

#include "php.h"

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

PHPAPI void zend_reflection_class_factory(zend_class_entry *ce, zval *object);

END_EXTERN_C()

#endif /* PHP_REFLECTION_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
