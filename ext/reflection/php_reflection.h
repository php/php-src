/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2005 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: George Schlossnagle <george@omniti.com>                     |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_REFLECTION_H
#define PHP_REFLECTION_H

#include "php.h"

extern zend_module_entry reflection_module_entry;
#define phpext_reflection_ptr &reflection_module_entry

BEGIN_EXTERN_C()

/* Class entry pointers */
extern PHPAPI zend_class_entry *reflector_ptr;
extern PHPAPI zend_class_entry *reflection_exception_ptr;
extern PHPAPI zend_class_entry *reflection_ptr;
extern PHPAPI zend_class_entry *reflection_function_ptr;
extern PHPAPI zend_class_entry *reflection_parameter_ptr;
extern PHPAPI zend_class_entry *reflection_class_ptr;
extern PHPAPI zend_class_entry *reflection_object_ptr;
extern PHPAPI zend_class_entry *reflection_method_ptr;
extern PHPAPI zend_class_entry *reflection_property_ptr;
extern PHPAPI zend_class_entry *reflection_extension_ptr;

PHPAPI void zend_reflection_class_factory(zend_class_entry *ce, zval *object TSRMLS_DC);
	
END_EXTERN_C()

#endif /* PHP_REFLECTION_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
