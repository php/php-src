/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "php_uri_common.h"

void uri_register_property_handler(HashTable *property_handlers, zend_string *name, const uri_property_handler_t *handler)
{
	zend_hash_add_new_ptr(property_handlers, name, (void *) handler);
	zend_string_release_ex(name, true);
}

uri_property_handler_t *uri_property_handler_from_internal_uri(const uri_internal_t *internal_uri, zend_string *name)
{
	return zend_hash_find_ptr(internal_uri->handler->property_handlers, name);
}

void throw_invalid_uri_exception(zval *errors)
{
	zval exception_zv;
	object_init_ex(&exception_zv, invalid_uri_exception_ce);

	zend_update_property_string(
		invalid_uri_exception_ce,
		Z_OBJ(exception_zv),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_MESSAGE)),
		ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_MESSAGE)),
		"URI parsing failed"
	);

	if (Z_TYPE_P(errors) == IS_ARRAY) {
		zend_update_property(invalid_uri_exception_ce, Z_OBJ(exception_zv), "errors", sizeof("errors") - 1, errors);
	} else {
		ZEND_ASSERT(Z_TYPE_P(errors) == IS_UNDEF);
		ZVAL_EMPTY_ARRAY(errors);
		zend_update_property(invalid_uri_exception_ce, Z_OBJ(exception_zv), "errors", sizeof("errors") - 1, errors);
	}

	zend_throw_exception_object(&exception_zv);
}
