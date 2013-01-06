/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:  Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_INCOMPLETE_CLASS_H
#define PHP_INCOMPLETE_CLASS_H

#include "ext/standard/basic_functions.h"

#define PHP_IC_ENTRY \
	BG(incomplete_class)

#define PHP_SET_CLASS_ATTRIBUTES(struc) \
	/* OBJECTS_FIXME: Fix for new object model */	\
	if (Z_OBJ_HT_P(struc)->get_class_entry && \
            Z_OBJCE_P(struc) == BG(incomplete_class)) {	\
		class_name = php_lookup_class_name(struc, &name_len); \
		if (!class_name) { \
			name_len = sizeof(INCOMPLETE_CLASS) - 1; \
			class_name = estrndup(INCOMPLETE_CLASS, name_len); \
		} \
		free_class_name = 1; \
		incomplete_class = 1; \
	} else { \
		free_class_name = !zend_get_object_classname(struc, (const char **)&class_name, &name_len TSRMLS_CC);\
	}

#define PHP_CLEANUP_CLASS_ATTRIBUTES()	\
	if (free_class_name) efree(class_name)

#define PHP_CLASS_ATTRIBUTES											\
	char *class_name;													\
	zend_uint name_len;													\
	zend_bool free_class_name = 0;										\
	zend_bool incomplete_class = 0

#define INCOMPLETE_CLASS "__PHP_Incomplete_Class"
#define MAGIC_MEMBER "__PHP_Incomplete_Class_Name"

#ifdef __cplusplus
extern "C" {
#endif

PHPAPI zend_class_entry *php_create_incomplete_class(TSRMLS_D);
PHPAPI char *php_lookup_class_name(zval *object, zend_uint *nlen);
PHPAPI void  php_store_class_name(zval *object, const char *name, zend_uint len);

#ifdef __cplusplus
};
#endif

#endif
