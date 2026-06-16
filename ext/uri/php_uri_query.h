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
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_URI_QUERY_H
#define PHP_URI_QUERY_H

#define php_uri_query_params_object_from_obj(object) ZEND_CONTAINER_OF(object, php_uri_query_params_object, std)

#define Z_URI_QUERY_PARAMS_OBJECT_P(zv) php_uri_query_params_object_from_obj(Z_OBJ_P((zv)))

#define php_uri_query_param_options_object_from_obj(object) ZEND_CONTAINER_OF(object, php_uri_query_param_options_object, std)

#define Z_URI_QUERY_PARAM_OPTIONS_OBJECT_P(zv) php_uri_query_param_options_object_from_obj(Z_OBJ_P((zv)))

ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_object_query_params_create_object(zend_class_entry *ce);
ZEND_ATTRIBUTE_NONNULL void php_uri_query_params_object_free(php_uri_query_params_object *uri_query_params_object);
ZEND_ATTRIBUTE_NONNULL void php_uri_query_params_object_handler_free(zend_object *object);
ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_query_params_object_handler_clone(zend_object *object);
ZEND_ATTRIBUTE_NONNULL zend_object_iterator *php_uri_object_query_params_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_object_create_query_param_options(zend_class_entry *ce);
ZEND_ATTRIBUTE_NONNULL void php_uri_query_param_options_object_free(php_uri_query_param_options_object *uri_query_param_options_object);
ZEND_ATTRIBUTE_NONNULL void php_uri_query_param_options_object_handler_free(zend_object *object);
ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_query_param_options_object_handler_clone(zend_object *object);
ZEND_ATTRIBUTE_NONNULL int php_uri_query_param_options_object_handler_has_property(zend_object *obj, zend_string *name, int check_empty, void **cache_slot);
ZEND_ATTRIBUTE_NONNULL zval *php_uri_query_param_options_object_handler_read_property(zend_object *obj, zend_string *name, int type, void **cache_slot, zval *rv);
ZEND_ATTRIBUTE_NONNULL zval *php_uri_query_param_options_object_handler_write_property(zend_object *obj, zend_string *name, zval *value, void **cache_slot);
ZEND_ATTRIBUTE_NONNULL zval *php_uri_query_param_options_object_handler_get_property_ptr_ptr(zend_object *object, zend_string *member, int type, void **cache_slot);
ZEND_ATTRIBUTE_NONNULL void php_uri_query_param_options_object_handler_unset_property(zend_object *obj, zend_string *name, void **cache_slot);

#endif
