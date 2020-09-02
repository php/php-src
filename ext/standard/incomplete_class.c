/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#include "php.h"
#include "basic_functions.h"
#include "php_incomplete_class.h"

#define INCOMPLETE_CLASS_MSG \
		"The script tried to %s on an incomplete object. " \
		"Please ensure that the class definition \"%s\" of the object " \
		"you are trying to operate on was loaded _before_ " \
		"unserialize() gets called or provide an autoloader " \
		"to load the class definition"

PHPAPI zend_class_entry *php_ce_incomplete_class;
static zend_object_handlers php_incomplete_object_handlers;

static void incomplete_class_message(zend_object *object)
{
	zend_string *class_name = php_lookup_class_name(object);
	php_error_docref(NULL, E_WARNING, INCOMPLETE_CLASS_MSG,
		"access a property", class_name ? ZSTR_VAL(class_name) : "unknown");
	if (class_name) {
		zend_string_release_ex(class_name, 0);
	}
}

static void throw_incomplete_class_error(zend_object *object, const char *what)
{
	zend_string *class_name = php_lookup_class_name(object);
	zend_throw_error(NULL, INCOMPLETE_CLASS_MSG,
		what, class_name ? ZSTR_VAL(class_name) : "unknown");
	if (class_name) {
		zend_string_release_ex(class_name, 0);
	}
}

static zval *incomplete_class_get_property(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv) /* {{{ */
{
	incomplete_class_message(object);

	if (type == BP_VAR_W || type == BP_VAR_RW) {
		ZVAL_ERROR(rv);
		return rv;
	} else {
		return &EG(uninitialized_zval);
	}
}
/* }}} */

static zval *incomplete_class_write_property(zend_object *object, zend_string *member, zval *value, void **cache_slot) /* {{{ */
{
	throw_incomplete_class_error(object, "modify a property");
	return value;
}
/* }}} */

static zval *incomplete_class_get_property_ptr_ptr(zend_object *object, zend_string *member, int type, void **cache_slot) /* {{{ */
{
	throw_incomplete_class_error(object, "modify a property");
	return &EG(error_zval);
}
/* }}} */

static void incomplete_class_unset_property(zend_object *object, zend_string *member, void **cache_slot) /* {{{ */
{
	throw_incomplete_class_error(object, "modify a property");
}
/* }}} */

static int incomplete_class_has_property(zend_object *object, zend_string *member, int check_empty, void **cache_slot) /* {{{ */
{
	incomplete_class_message(object);
	return 0;
}
/* }}} */

static zend_function *incomplete_class_get_method(zend_object **object, zend_string *method, const zval *key) /* {{{ */
{
	throw_incomplete_class_error(*object, "call a method");
	return NULL;
}
/* }}} */

/* {{{ php_create_incomplete_class */
static zend_object *php_create_incomplete_object(zend_class_entry *class_type)
{
	zend_object *object;

	object = zend_objects_new( class_type);
	object->handlers = &php_incomplete_object_handlers;

	object_properties_init(object, class_type);

	return object;
}

PHPAPI void php_register_incomplete_class(void)
{
	zend_class_entry incomplete_class;

	INIT_CLASS_ENTRY(incomplete_class, INCOMPLETE_CLASS, NULL);

	incomplete_class.create_object = php_create_incomplete_object;

	memcpy(&php_incomplete_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_incomplete_object_handlers.read_property = incomplete_class_get_property;
	php_incomplete_object_handlers.has_property = incomplete_class_has_property;
	php_incomplete_object_handlers.unset_property = incomplete_class_unset_property;
	php_incomplete_object_handlers.write_property = incomplete_class_write_property;
	php_incomplete_object_handlers.get_property_ptr_ptr = incomplete_class_get_property_ptr_ptr;
    php_incomplete_object_handlers.get_method = incomplete_class_get_method;

	php_ce_incomplete_class = zend_register_internal_class(&incomplete_class);
	php_ce_incomplete_class->ce_flags |= ZEND_ACC_FINAL;
}
/* }}} */

/* {{{ php_lookup_class_name */
PHPAPI zend_string *php_lookup_class_name(zend_object *object)
{
	if (object->properties) {
		zval *val = zend_hash_str_find(object->properties, MAGIC_MEMBER, sizeof(MAGIC_MEMBER)-1);

		if (val != NULL && Z_TYPE_P(val) == IS_STRING) {
			return zend_string_copy(Z_STR_P(val));
		}
	}

	return NULL;
}
/* }}} */

/* {{{ php_store_class_name */
PHPAPI void php_store_class_name(zval *object, zend_string *name)
{
	zval val;

	ZVAL_STR_COPY(&val, name);
	zend_hash_str_update(Z_OBJPROP_P(object), MAGIC_MEMBER, sizeof(MAGIC_MEMBER)-1, &val);
}
/* }}} */
