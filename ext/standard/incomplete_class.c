/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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


/* $Id$ */

#include "php.h"
#include "basic_functions.h"
#include "php_incomplete_class.h"

#define INCOMPLETE_CLASS_MSG \
		"The script tried to execute a method or "  \
		"access a property of an incomplete object. " \
		"Please ensure that the class definition <b>%s</b> of the object " \
		"you are trying to operate on was loaded _before_ " \
		"the session was started"


/* {{{ incomplete_class_message
 */
static void incomplete_class_message(zend_property_reference *ref, int error_type)
{
	char buf[1024];
	char *class_name;
	TSRMLS_FETCH();

	class_name = php_lookup_class_name(ref->object, NULL, 0);
	
	if (!class_name)
		class_name = estrdup("unknown");
	
	snprintf(buf, sizeof(buf)-1, INCOMPLETE_CLASS_MSG, class_name);
	
	efree(class_name);

	php_error_docref(NULL TSRMLS_CC, error_type, "%s", buf);
}
/* }}} */

/* {{{ incomplete_class_call_func
 */
static void incomplete_class_call_func(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	incomplete_class_message(property_reference, E_ERROR);
}
/* }}} */

/* {{{ incomplete_class_set_property
 */
static int incomplete_class_set_property(zend_property_reference *property_reference, zval *value)
{
	incomplete_class_message(property_reference, E_NOTICE);
	
	/* does not reach this point */
	return (0);
}
/* }}} */

/* {{{ incomplete_class_get_property
 */
static zval incomplete_class_get_property(zend_property_reference *property_reference)
{
	zval foo;
	
	incomplete_class_message(property_reference, E_NOTICE);

	/* does not reach this point */
	memset(&foo, 0, sizeof(zval)); /* shut warnings up */
	return (foo);
}
/* }}} */

/* {{{ php_create_incomplete_class
 */
zend_class_entry *php_create_incomplete_class(TSRMLS_D)
{
	zend_class_entry incomplete_class;

	INIT_OVERLOADED_CLASS_ENTRY(incomplete_class, INCOMPLETE_CLASS, NULL,
			incomplete_class_call_func,
			incomplete_class_get_property,
			incomplete_class_set_property);

	return zend_register_internal_class(&incomplete_class TSRMLS_CC);
}
/* }}} */

/* {{{ php_lookup_class_name
 */
char *php_lookup_class_name(zval *object, size_t *nlen, zend_bool del)
{
	zval **val;
	char *retval = NULL;
	HashTable *object_properties;

	object_properties = Z_OBJPROP_P(object);

	if (zend_hash_find(object_properties, MAGIC_MEMBER, sizeof(MAGIC_MEMBER), (void **) &val) == SUCCESS) {
		retval = estrndup(Z_STRVAL_PP(val), Z_STRLEN_PP(val));

		if (nlen)
			*nlen = Z_STRLEN_PP(val);

		if (del)
			zend_hash_del(object_properties, MAGIC_MEMBER, sizeof(MAGIC_MEMBER));
	}

	return (retval);
}
/* }}} */

/* {{{ php_store_class_name
 */
void php_store_class_name(zval *object, const char *name, size_t len)
{
	zval *val;

	MAKE_STD_ZVAL(val);

	Z_TYPE_P(val)   = IS_STRING;
	Z_STRVAL_P(val) = estrndup(name, len);
	Z_STRLEN_P(val) = len;

	zend_hash_update(Z_OBJPROP_P(object), MAGIC_MEMBER, sizeof(MAGIC_MEMBER), &val, sizeof(val), NULL);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
