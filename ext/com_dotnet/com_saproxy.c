/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong  <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* This module implements a SafeArray proxy which is used internally
 * by the engine when resolving multi-dimensional array accesses on
 * SafeArray types
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"
#include "Zend/zend_default_classes.h"

typedef struct {
	/* the object we a proxying for; we hold a refcount to it */
	zval *zobj;
	php_com_dotnet_object *obj;

	/* how many dimensions we are indirecting to get into this element */
	LONG dimensions;
	
	/* this is an array whose size_is(dimensions) */
	LONG *indices;

} php_com_saproxy;

typedef struct {
	zend_object_iterator iter;
	zval *proxy_obj;
	php_com_saproxy *proxy;
	LONG key;
	LONG imin, imax;
	LONG *indices;
} php_com_saproxy_iter;

#define SA_FETCH(zv)			(php_com_saproxy*)zend_object_store_get_object(zv TSRMLS_CC)

static zval *saproxy_property_read(zval *object, zval *member, zend_bool silent TSRMLS_DC)
{
	zval *return_value;
	
	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	if (!silent) {
		php_com_throw_exception(E_INVALIDARG, "safearray has no properties" TSRMLS_CC);
	}

	return return_value;
}

static void saproxy_property_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
	php_com_throw_exception(E_INVALIDARG, "safearray has no properties" TSRMLS_CC);
}

static zval *saproxy_read_dimension(zval *object, zval *offset TSRMLS_DC)
{
	php_com_saproxy *proxy = SA_FETCH(object);
	zval *return_value;
	UINT dims;
	SAFEARRAY *sa;
	LONG ubound, lbound;
	
	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);
	
	if (!V_ISARRAY(&proxy->obj->v)) {
		php_com_throw_exception(E_INVALIDARG, "proxied object is no longer a safe array!" TSRMLS_CC);
		return return_value;
	}

	/* offset/index must be an integer */
	convert_to_long(offset);
	
	sa = V_ARRAY(&proxy->obj->v);
	dims = SafeArrayGetDim(sa);

	if (proxy->dimensions >= dims) {
		/* too many dimensions */
		php_com_throw_exception(E_INVALIDARG, "too many dimensions!" TSRMLS_CC);
		return return_value;
	}

	/* bounds check */
	SafeArrayGetLBound(sa, proxy->dimensions, &lbound);
	SafeArrayGetUBound(sa, proxy->dimensions, &ubound);

	if (Z_LVAL_P(offset) < lbound || Z_LVAL_P(offset) > ubound) {
		php_com_throw_exception(E_INVALIDARG, "index out of bounds" TSRMLS_CC);
		return return_value;
	}
	
	if (dims - 1 == proxy->dimensions) {
		LONG *indices;
		VARTYPE vt;
		VARIANT v;
		
		VariantInit(&v);
		
		/* we can return a real value */
		indices = do_alloca(dims * sizeof(LONG));

		/* copy indices from proxy */
		memcpy(indices, proxy->indices, (dims-1) * sizeof(LONG));

		/* add user-supplied index */
		indices[dims-1] = Z_LVAL_P(offset);

		/* now fetch the value */
		if (FAILED(SafeArrayGetVartype(sa, &vt)) || vt == VT_EMPTY) {
			vt = V_VT(&proxy->obj->v) & ~VT_ARRAY;
		}

		if (vt == VT_VARIANT) {
			SafeArrayGetElement(sa, indices, &v);
		} else {
			V_VT(&v) = vt;
			SafeArrayGetElement(sa, indices, &v.lVal);
		}

		free_alloca(indices);

		php_com_wrap_variant(return_value, &v, proxy->obj->code_page TSRMLS_CC);

		VariantClear(&v);
		
	} else {
		/* return another proxy */
		php_com_saproxy_create(object, return_value, Z_LVAL_P(offset) TSRMLS_CC);
	}

	return return_value;
}

static void saproxy_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC)
{
	php_com_throw_exception(E_NOTIMPL, "writing to safearray not yet implemented" TSRMLS_CC);
}

static void saproxy_object_set(zval **property, zval *value TSRMLS_DC)
{
}

static zval *saproxy_object_get(zval *property TSRMLS_DC)
{
	/* Not yet implemented in the engine */
	return NULL;
}

static int saproxy_property_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	/* no properties */
	return 0;
}

static int saproxy_dimension_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Operation not yet supported on a COM object");
	return 0;
}

static void saproxy_property_delete(zval *object, zval *member TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot delete properties from a COM object");
}

static void saproxy_dimension_delete(zval *object, zval *offset TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot delete properties from a COM object");
}

static HashTable *saproxy_properties_get(zval *object TSRMLS_DC)
{
	/* no properties */
	return NULL;
}

static union _zend_function *saproxy_method_get(zval *object, char *name, int len TSRMLS_DC)
{
	/* no methods */
	return NULL;
}

static int saproxy_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	return FAILURE;
}

static union _zend_function *saproxy_constructor_get(zval *object TSRMLS_DC)
{
	/* user cannot instanciate */
	return NULL;
}

static zend_class_entry *saproxy_class_entry_get(zval *object TSRMLS_DC)
{
	return php_com_saproxy_class_entry;
}

static int saproxy_class_name_get(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
	*class_name = estrndup(php_com_saproxy_class_entry->name, php_com_saproxy_class_entry->name_length);
	*class_name_len = php_com_saproxy_class_entry->name_length;
	return 0;
}

static int saproxy_objects_compare(zval *object1, zval *object2 TSRMLS_DC)
{
	return -1;
}

static int saproxy_object_cast(zval *readobj, zval *writeobj, int type, int should_free TSRMLS_DC)
{
	return FAILURE;
}

zend_object_handlers php_com_saproxy_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	saproxy_property_read,
	saproxy_property_write,
	saproxy_read_dimension,
	saproxy_write_dimension,
	NULL,
	saproxy_object_get,
	saproxy_object_set,
	saproxy_property_exists,
	saproxy_property_delete,
	saproxy_dimension_exists,
	saproxy_dimension_delete,
	saproxy_properties_get,
	saproxy_method_get,
	saproxy_call_method,
	saproxy_constructor_get,
	saproxy_class_entry_get,
	saproxy_class_name_get,
	saproxy_objects_compare,
	saproxy_object_cast
};

static void saproxy_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	php_com_saproxy *proxy = (php_com_saproxy *)object;

	ZVAL_DELREF(proxy->zobj);
	efree(proxy->indices);
	efree(proxy);
}

static void saproxy_clone(void *object, void **clone_ptr TSRMLS_DC)
{
	php_com_saproxy *proxy = (php_com_saproxy *)object;
	php_com_saproxy *cloneproxy;

	cloneproxy = emalloc(sizeof(*cloneproxy));
	memcpy(cloneproxy, proxy, sizeof(*cloneproxy));

	ZVAL_ADDREF(cloneproxy->zobj);
	cloneproxy->indices = safe_emalloc(cloneproxy->dimensions, sizeof(LONG), 0);
	memcpy(cloneproxy->indices, proxy->indices, cloneproxy->dimensions * sizeof(LONG));

	*clone_ptr = cloneproxy;
}

int php_com_saproxy_create(zval *com_object, zval *proxy_out, long index TSRMLS_DC)
{
	php_com_saproxy *proxy, *rel = NULL;
	php_com_dotnet_object *obj;

	proxy = ecalloc(1, sizeof(*proxy));
	proxy->dimensions = 1;

	if (Z_OBJCE_P(com_object) == php_com_saproxy_class_entry) {
		rel = SA_FETCH(com_object);
		obj = rel->obj;
		proxy->zobj = rel->zobj;
		proxy->dimensions += rel->dimensions;
	} else {
		obj = CDNO_FETCH(com_object);
		proxy->zobj = com_object;
	}

	ZVAL_ADDREF(proxy->zobj);
	proxy->indices = safe_emalloc(proxy->dimensions, sizeof(LONG), 0);

	if (rel) {
		memcpy(proxy->indices, rel->indices, (proxy->dimensions-1) * sizeof(LONG));
	}

	proxy->indices[proxy->dimensions-1] = index;

	Z_TYPE_P(proxy_out) = IS_OBJECT;
	Z_OBJ_HANDLE_P(proxy_out) = zend_objects_store_put(proxy, saproxy_dtor, saproxy_clone TSRMLS_CC);
	Z_OBJ_HT_P(proxy_out) = &php_com_saproxy_handlers;
	
	return 1;
}

/* iterator */

static void saproxy_iter_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)iter->data;

	ZVAL_DELREF(I->proxy_obj);

	efree(I->indices);
	efree(I);
}

static int saproxy_iter_has_more(zend_object_iterator *iter TSRMLS_DC)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)iter->data;

	return (I->key < I->imax) ? SUCCESS : FAILURE;
}

static void saproxy_iter_get_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)iter->data;
	VARIANT v;
	VARTYPE vt;
	zval *return_value, **ptr_ptr;
	SAFEARRAY *sa;

	I->indices[I->proxy->dimensions-1] = I->key;
	
	sa = V_ARRAY(&I->proxy->obj->v);
	
	if (FAILED(SafeArrayGetVartype(sa, &vt)) || vt == VT_EMPTY) {
		vt = V_VT(&I->proxy->obj->v) & ~VT_ARRAY;
	}

	VariantInit(&v);
	if (vt == VT_VARIANT) {
		SafeArrayGetElement(sa, I->indices, &v);
	} else {
		V_VT(&v) = vt;
		SafeArrayGetElement(sa, I->indices, &v.lVal);
	}

	MAKE_STD_ZVAL(return_value);
	php_com_wrap_variant(return_value, &v, I->proxy->obj->code_page TSRMLS_CC);
	VariantClear(&v);

	ptr_ptr = emalloc(sizeof(*ptr_ptr));
	*ptr_ptr = return_value;
	*data = ptr_ptr;
}

static int saproxy_iter_get_key(zend_object_iterator *iter, char **str_key, uint *str_key_len,
	ulong *int_key TSRMLS_DC)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)iter->data;

	if (I->key == -1) {
		return HASH_KEY_NON_EXISTANT;
	}
	*int_key = (ulong)I->key;
	return HASH_KEY_IS_LONG;
}

static int saproxy_iter_move_forwards(zend_object_iterator *iter TSRMLS_DC)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)iter->data;

	if (++I->key >= I->imax) {
		I->key = -1;
		return FAILURE;
	}
	return SUCCESS;
}

static zend_object_iterator_funcs saproxy_iter_funcs = {
	saproxy_iter_dtor,
	saproxy_iter_has_more,
	saproxy_iter_get_data,
	saproxy_iter_get_key,
	saproxy_iter_move_forwards,
	NULL
};


zend_object_iterator *php_com_saproxy_iter_get(zend_class_entry *ce, zval *object TSRMLS_DC)
{
	php_com_saproxy *proxy = SA_FETCH(object);
	php_com_saproxy_iter *I;

	I = ecalloc(1, sizeof(*I));
	I->iter.funcs = &saproxy_iter_funcs;
	I->iter.data = I;

	I->proxy = proxy;
	I->proxy_obj = object;
	ZVAL_ADDREF(I->proxy_obj);

	I->indices = safe_emalloc(proxy->dimensions + 1, sizeof(LONG), 0);
	memcpy(I->indices, proxy->indices, proxy->dimensions * sizeof(LONG));

	SafeArrayGetLBound(V_ARRAY(&proxy->obj->v), proxy->dimensions, &I->imin);
	SafeArrayGetUBound(V_ARRAY(&proxy->obj->v), proxy->dimensions, &I->imax);

	I->key = I->imin;	
	
	return &I->iter;
}

