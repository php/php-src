/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:  Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */


#ifndef PHP_INCOMPLETE_CLASS_H
#define PHP_INCOMPLETE_CLASS_H

#define PHP_IC_ENTRY \
	BG(incomplete_class)

#define PHP_IC_ENTRY_READ \
	(PHP_IC_ENTRY ? PHP_IC_ENTRY : php_create_incomplete_class(BLS_C))


#define PHP_SET_CLASS_ATTRIBUTES() 				\
	if ((*struc)->value.obj.ce == BG(incomplete_class)) {				\
		class_name = php_lookup_class_name(struc, &name_len, 1);		\
		free_class_name = 1;											\
	} else {															\
		class_name = (*struc)->value.obj.ce->name;						\
		name_len   = (*struc)->value.obj.ce->name_length;				\
	}

#define PHP_CLEANUP_CLASS_ATTRIBUTES()									\
	if (free_class_name) efree(class_name)

#define PHP_CLASS_ATTRIBUTES											\
	char *class_name;													\
	size_t name_len;													\
	zend_bool free_class_name = 0										\



#ifdef __cplusplus
extern "C" {
#endif

zend_class_entry *php_create_incomplete_class(BLS_D);

char *php_lookup_class_name(zval **object, size_t *nlen, zend_bool del);
void  php_store_class_name(zval **object, const char *name, size_t len);

#ifdef __cplusplus
};
#endif

#endif
