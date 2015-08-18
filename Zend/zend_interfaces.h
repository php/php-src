/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_INTERFACES_H
#define ZEND_INTERFACES_H

#include "zend.h"
#include "zend_API.h"

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_traversable;
extern ZEND_API zend_class_entry *zend_ce_aggregate;
extern ZEND_API zend_class_entry *zend_ce_iterator;
extern ZEND_API zend_class_entry *zend_ce_arrayaccess;
extern ZEND_API zend_class_entry *zend_ce_serializable;

typedef struct _zend_user_iterator {
	zend_object_iterator     it;
	zend_class_entry         *ce;
	zval                     value;
} zend_user_iterator;

ZEND_API zval* zend_call_method(zval *object_pp, zend_class_entry *obj_ce, zend_function **fn_proxy, const char *function_name, size_t function_name_len, zval *retval, int param_count, zval* arg1, zval* arg2);

#define zend_call_method_with_0_params(obj, obj_ce, fn_proxy, function_name, retval) \
	zend_call_method(obj, obj_ce, fn_proxy, function_name, sizeof(function_name)-1, retval, 0, NULL, NULL)

#define zend_call_method_with_1_params(obj, obj_ce, fn_proxy, function_name, retval, arg1) \
	zend_call_method(obj, obj_ce, fn_proxy, function_name, sizeof(function_name)-1, retval, 1, arg1, NULL)

#define zend_call_method_with_2_params(obj, obj_ce, fn_proxy, function_name, retval, arg1, arg2) \
	zend_call_method(obj, obj_ce, fn_proxy, function_name, sizeof(function_name)-1, retval, 2, arg1, arg2)

#define REGISTER_MAGIC_INTERFACE(class_name, class_name_str) \
	{\
		zend_class_entry ce;\
		INIT_CLASS_ENTRY(ce, # class_name_str, zend_funcs_ ## class_name) \
		zend_ce_ ## class_name = zend_register_internal_interface(&ce);\
		zend_ce_ ## class_name->interface_gets_implemented = zend_implement_ ## class_name;\
	}

#define REGISTER_MAGIC_IMPLEMENT(class_name, interface_name) \
	zend_class_implements(zend_ce_ ## class_name, 1, zend_ce_ ## interface_name)

ZEND_API void zend_user_it_rewind(zend_object_iterator *_iter);
ZEND_API int zend_user_it_valid(zend_object_iterator *_iter);
ZEND_API void zend_user_it_get_current_key(zend_object_iterator *_iter, zval *key);
ZEND_API zval *zend_user_it_get_current_data(zend_object_iterator *_iter);
ZEND_API void zend_user_it_move_forward(zend_object_iterator *_iter);
ZEND_API void zend_user_it_invalidate_current(zend_object_iterator *_iter);

ZEND_API void zend_user_it_new_iterator(zend_class_entry *ce, zval *object, zval *iterator);
ZEND_API zend_object_iterator *zend_user_it_get_new_iterator(zend_class_entry *ce, zval *object, int by_ref);

ZEND_API void zend_register_interfaces(void);

ZEND_API int zend_user_serialize(zval *object, unsigned char **buffer, size_t *buf_len, zend_serialize_data *data);
ZEND_API int zend_user_unserialize(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data);

ZEND_API int zend_class_serialize_deny(zval *object, unsigned char **buffer, size_t *buf_len, zend_serialize_data *data);
ZEND_API int zend_class_unserialize_deny(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data);

END_EXTERN_C()

#endif /* ZEND_INTERFACES_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
