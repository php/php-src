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
   | Author:  Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_INCOMPLETE_CLASS_H
#define PHP_INCOMPLETE_CLASS_H

#include "ext/standard/basic_functions.h"

extern PHPAPI zend_class_entry *php_ce_incomplete_class;

#define PHP_IC_ENTRY php_ce_incomplete_class

#define PHP_SET_CLASS_ATTRIBUTES(struc) \
	/* OBJECTS_FIXME: Fix for new object model */	\
	if (Z_OBJCE_P(struc) == php_ce_incomplete_class) {	\
		class_name = php_lookup_class_name(Z_OBJ_P(struc)); \
		if (!class_name) { \
			class_name = zend_string_init(INCOMPLETE_CLASS, sizeof(INCOMPLETE_CLASS) - 1, 0); \
		} \
		incomplete_class = 1; \
	} else { \
		class_name = zend_string_copy(Z_OBJCE_P(struc)->name); \
	}

#define PHP_CLEANUP_CLASS_ATTRIBUTES()	\
	zend_string_release_ex(class_name, 0)

#define PHP_CLASS_ATTRIBUTES											\
	zend_string *class_name;											\
	bool incomplete_class ZEND_ATTRIBUTE_UNUSED = 0

#define INCOMPLETE_CLASS "__PHP_Incomplete_Class"
#define MAGIC_MEMBER "__PHP_Incomplete_Class_Name"

#ifdef __cplusplus
extern "C" {
#endif

PHPAPI void php_register_incomplete_class_handlers(void);
PHPAPI zend_string *php_lookup_class_name(zend_object *object);
PHPAPI void php_store_class_name(zval *object, zend_string *name);

#ifdef __cplusplus
};
#endif

#endif
