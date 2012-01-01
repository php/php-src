/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
 * SafeArray types.
 * In addition, the proxy is now able to handle properties of COM objects
 * that smell like PHP arrays.
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"
#include "Zend/zend_exceptions.h"

typedef struct {
	/* the object we a proxying for; we hold a refcount to it */
	zval *zobj;
	php_com_dotnet_object *obj;

	/* how many dimensions we are indirecting to get into this element */
	LONG dimensions;
	
	/* this is an array whose size_is(dimensions) */
	zval **indices;

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

static inline void clone_indices(php_com_saproxy *dest, php_com_saproxy *src, int ndims)
{
	int i;

	for (i = 0; i < ndims; i++) {
		MAKE_STD_ZVAL(dest->indices[i]);
		*dest->indices[i] = *src->indices[i];
		zval_copy_ctor(dest->indices[i]);
	}
}

static zval *saproxy_property_read(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC)
{
	zval *return_value;
	
	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	php_com_throw_exception(E_INVALIDARG, "safearray has no properties" TSRMLS_CC);

	return return_value;
}

static void saproxy_property_write(zval *object, zval *member, zval *value, const zend_literal *key TSRMLS_DC)
{
	php_com_throw_exception(E_INVALIDARG, "safearray has no properties" TSRMLS_CC);
}

static zval *saproxy_read_dimension(zval *object, zval *offset, int type TSRMLS_DC)
{
	php_com_saproxy *proxy = SA_FETCH(object);
	zval *return_value;
	UINT dims, i;
	SAFEARRAY *sa;
	LONG ubound, lbound;
	HRESULT res;
	
	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);
	
	if (V_VT(&proxy->obj->v) == VT_DISPATCH) {
		VARIANT v;
		zval **args;

		/* prop-get using first dimension as the property name,
		 * all subsequent dimensions and the offset as parameters */

		args = safe_emalloc(proxy->dimensions + 1, sizeof(zval *), 0);

		for (i = 1; i < (UINT) proxy->dimensions; i++) {
			args[i-1] = proxy->indices[i];
		}
		args[i-1] = offset;

		convert_to_string(proxy->indices[0]);
		VariantInit(&v);

		res = php_com_do_invoke(proxy->obj, Z_STRVAL_P(proxy->indices[0]),
			   	Z_STRLEN_P(proxy->indices[0]), DISPATCH_METHOD|DISPATCH_PROPERTYGET, &v,
			   	proxy->dimensions, args, 0 TSRMLS_CC);

		if (res == SUCCESS) {
			php_com_zval_from_variant(return_value, &v, proxy->obj->code_page TSRMLS_CC);
			VariantClear(&v);
		} else if (res == DISP_E_BADPARAMCOUNT) {
			/* return another proxy */
			php_com_saproxy_create(object, return_value, offset TSRMLS_CC);
		}

		return return_value;

	} else if (!V_ISARRAY(&proxy->obj->v)) {
		php_com_throw_exception(E_INVALIDARG, "invalid read from com proxy object" TSRMLS_CC);
		return return_value;
	}

	/* the SafeArray case */
	
	/* offset/index must be an integer */
	convert_to_long(offset);
	
	sa = V_ARRAY(&proxy->obj->v);
	dims = SafeArrayGetDim(sa);

	if ((UINT) proxy->dimensions >= dims) {
		/* too many dimensions */
		php_com_throw_exception(E_INVALIDARG, "too many dimensions!" TSRMLS_CC);
		return return_value;
	}

	/* bounds check */
	SafeArrayGetLBound(sa, proxy->dimensions, &lbound);
	SafeArrayGetUBound(sa, proxy->dimensions, &ubound);

	if (Z_LVAL_P(offset) < lbound || Z_LVAL_P(offset) > ubound) {
		php_com_throw_exception(DISP_E_BADINDEX, "index out of bounds" TSRMLS_CC);
		return return_value;
	}
	
	if (dims - 1 == proxy->dimensions) {
		LONG *indices;
		VARTYPE vt;
		VARIANT v;
		
		VariantInit(&v);
		
		/* we can return a real value */
		indices = safe_emalloc(dims, sizeof(LONG), 0);

		/* copy indices from proxy */
		for (i = 0; i < dims; i++) {
			convert_to_long(proxy->indices[i]);
			indices[i] = Z_LVAL_P(proxy->indices[i]);
		}

		/* add user-supplied index */
		indices[dims-1] = Z_LVAL_P(offset);

		/* now fetch the value */
		if (FAILED(SafeArrayGetVartype(sa, &vt)) || vt == VT_EMPTY) {
			vt = V_VT(&proxy->obj->v) & ~VT_ARRAY;
		}

		if (vt == VT_VARIANT) {
			res = SafeArrayGetElement(sa, indices, &v);
		} else {
			V_VT(&v) = vt;
			res = SafeArrayGetElement(sa, indices, &v.lVal);
		}

		efree(indices);

		if (SUCCEEDED(res)) {
			php_com_wrap_variant(return_value, &v, proxy->obj->code_page TSRMLS_CC);
		} else {
			php_com_throw_exception(res, NULL TSRMLS_CC);
		}

		VariantClear(&v);
		
	} else {
		/* return another proxy */
		php_com_saproxy_create(object, return_value, offset TSRMLS_CC);
	}

	return return_value;
}

static void saproxy_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC)
{
	php_com_saproxy *proxy = SA_FETCH(object);
	UINT dims, i;
	HRESULT res;
	VARIANT v;
	
	if (V_VT(&proxy->obj->v) == VT_DISPATCH) {
		/* We do a prop-set using the first dimension as the property name,
		 * all subsequent dimensions and offset as parameters, with value as
		 * the final value */
		zval **args = safe_emalloc(proxy->dimensions + 2, sizeof(zval *), 0);

		for (i = 1; i < (UINT) proxy->dimensions; i++) {
			args[i-1] = proxy->indices[i];
		}
		args[i-1] = offset;
		args[i] = value;

		convert_to_string(proxy->indices[0]);
		VariantInit(&v);
		if (SUCCESS == php_com_do_invoke(proxy->obj, Z_STRVAL_P(proxy->indices[0]),
					Z_STRLEN_P(proxy->indices[0]), DISPATCH_PROPERTYPUT, &v, proxy->dimensions + 1,
					args, 0 TSRMLS_CC)) {
			VariantClear(&v);
		}

		efree(args);
		
	} else if (V_ISARRAY(&proxy->obj->v)) {
		LONG *indices;
		VARTYPE vt;

		dims = SafeArrayGetDim(V_ARRAY(&proxy->obj->v));
		indices = safe_emalloc(dims, sizeof(LONG), 0);
		/* copy indices from proxy */
		for (i = 0; i < dims; i++) {
			convert_to_long(proxy->indices[i]);
			indices[i] = Z_LVAL_P(proxy->indices[i]);
		}

		/* add user-supplied index */
		convert_to_long(offset);
		indices[dims-1] = Z_LVAL_P(offset);

		if (FAILED(SafeArrayGetVartype(V_ARRAY(&proxy->obj->v), &vt)) || vt == VT_EMPTY) {
			vt = V_VT(&proxy->obj->v) & ~VT_ARRAY;
		}

		VariantInit(&v);
		php_com_variant_from_zval(&v, value, proxy->obj->code_page TSRMLS_CC);

		if (V_VT(&v) != vt) {
			VariantChangeType(&v, &v, 0, vt);
		}

		if (vt == VT_VARIANT) {
			res = SafeArrayPutElement(V_ARRAY(&proxy->obj->v), indices, &v);
		} else {
			res = SafeArrayPutElement(V_ARRAY(&proxy->obj->v), indices, &v.lVal);
		}
	
		efree(indices);
		VariantClear(&v);

		if (FAILED(res)) {
			php_com_throw_exception(res, NULL TSRMLS_CC);
		}
	} else {
		php_com_throw_exception(E_NOTIMPL, "invalid write to com proxy object" TSRMLS_CC);
	}
}

#if 0
static void saproxy_object_set(zval **property, zval *value TSRMLS_DC)
{
}

static zval *saproxy_object_get(zval *property TSRMLS_DC)
{
	/* Not yet implemented in the engine */
	return NULL;
}
#endif

static int saproxy_property_exists(zval *object, zval *member, int check_empty, const zend_literal *key TSRMLS_DC)
{
	/* no properties */
	return 0;
}

static int saproxy_dimension_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Operation not yet supported on a COM object");
	return 0;
}

static void saproxy_property_delete(zval *object, zval *member, const zend_literal *key TSRMLS_DC)
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

static union _zend_function *saproxy_method_get(zval **object, const char *name, int len, const zend_literal *key TSRMLS_DC)
{
	/* no methods */
	return NULL;
}

static int saproxy_call_method(const char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	return FAILURE;
}

static union _zend_function *saproxy_constructor_get(zval *object TSRMLS_DC)
{
	/* user cannot instantiate */
	return NULL;
}

static zend_class_entry *saproxy_class_entry_get(const zval *object TSRMLS_DC)
{
	return php_com_saproxy_class_entry;
}

static int saproxy_class_name_get(const zval *object, const char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
	*class_name = estrndup(php_com_saproxy_class_entry->name, php_com_saproxy_class_entry->name_length);
	*class_name_len = php_com_saproxy_class_entry->name_length;
	return 0;
}

static int saproxy_objects_compare(zval *object1, zval *object2 TSRMLS_DC)
{
	return -1;
}

static int saproxy_object_cast(zval *readobj, zval *writeobj, int type TSRMLS_DC)
{
	return FAILURE;
}

static int saproxy_count_elements(zval *object, long *count TSRMLS_DC)
{
	php_com_saproxy *proxy = SA_FETCH(object);
	LONG ubound, lbound;
	
	if (!V_ISARRAY(&proxy->obj->v)) {
		return FAILURE;
	}

	SafeArrayGetLBound(V_ARRAY(&proxy->obj->v), proxy->dimensions, &lbound);
	SafeArrayGetUBound(V_ARRAY(&proxy->obj->v), proxy->dimensions, &ubound);

	*count = ubound - lbound + 1;

	return SUCCESS;
}

zend_object_handlers php_com_saproxy_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	saproxy_property_read,
	saproxy_property_write,
	saproxy_read_dimension,
	saproxy_write_dimension,
	NULL,
	NULL, /* saproxy_object_get, */
	NULL, /* saproxy_object_set, */
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
	saproxy_object_cast,
	saproxy_count_elements
};

static void saproxy_free_storage(void *object TSRMLS_DC)
{
	php_com_saproxy *proxy = (php_com_saproxy *)object;
	int i;

	for (i = 0; i < proxy->dimensions; i++) {
		if (proxy->indices) {
				FREE_ZVAL(proxy->indices[i]);
		}
	}

	zval_ptr_dtor(&proxy->zobj);
	efree(proxy->indices);
	efree(proxy);
}

static void saproxy_clone(void *object, void **clone_ptr TSRMLS_DC)
{
	php_com_saproxy *proxy = (php_com_saproxy *)object;
	php_com_saproxy *cloneproxy;

	cloneproxy = emalloc(sizeof(*cloneproxy));
	memcpy(cloneproxy, proxy, sizeof(*cloneproxy));

	Z_ADDREF_P(cloneproxy->zobj);
	cloneproxy->indices = safe_emalloc(cloneproxy->dimensions, sizeof(zval *), 0);
	clone_indices(cloneproxy, proxy, proxy->dimensions);

	*clone_ptr = cloneproxy;
}

int php_com_saproxy_create(zval *com_object, zval *proxy_out, zval *index TSRMLS_DC)
{
	php_com_saproxy *proxy, *rel = NULL;

	proxy = ecalloc(1, sizeof(*proxy));
	proxy->dimensions = 1;

	if (Z_OBJCE_P(com_object) == php_com_saproxy_class_entry) {
		rel = SA_FETCH(com_object);
		proxy->obj = rel->obj;
		proxy->zobj = rel->zobj;
		proxy->dimensions += rel->dimensions;
	} else {
		proxy->obj = CDNO_FETCH(com_object);
		proxy->zobj = com_object;
	}

	Z_ADDREF_P(proxy->zobj);
	proxy->indices = safe_emalloc(proxy->dimensions, sizeof(zval *), 0);

	if (rel) {
		clone_indices(proxy, rel, rel->dimensions);
	}

	MAKE_STD_ZVAL(proxy->indices[proxy->dimensions-1]);
	*proxy->indices[proxy->dimensions-1] = *index;
	zval_copy_ctor(proxy->indices[proxy->dimensions-1]);

	Z_TYPE_P(proxy_out) = IS_OBJECT;
	Z_OBJ_HANDLE_P(proxy_out) = zend_objects_store_put(proxy, NULL, saproxy_free_storage, saproxy_clone TSRMLS_CC);
	Z_OBJ_HT_P(proxy_out) = &php_com_saproxy_handlers;
	
	return 1;
}

/* iterator */

static void saproxy_iter_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)iter->data;

	zval_ptr_dtor(&I->proxy_obj);

	efree(I->indices);
	efree(I);
}

static int saproxy_iter_valid(zend_object_iterator *iter TSRMLS_DC)
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
	saproxy_iter_valid,
	saproxy_iter_get_data,
	saproxy_iter_get_key,
	saproxy_iter_move_forwards,
	NULL
};


zend_object_iterator *php_com_saproxy_iter_get(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	php_com_saproxy *proxy = SA_FETCH(object);
	php_com_saproxy_iter *I;
	int i;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}

	I = ecalloc(1, sizeof(*I));
	I->iter.funcs = &saproxy_iter_funcs;
	I->iter.data = I;

	I->proxy = proxy;
	I->proxy_obj = object;
	Z_ADDREF_P(I->proxy_obj);

	I->indices = safe_emalloc(proxy->dimensions + 1, sizeof(LONG), 0);
	for (i = 0; i < proxy->dimensions; i++) {
		convert_to_long(proxy->indices[i]);
		I->indices[i] = Z_LVAL_P(proxy->indices[i]);
	}

	SafeArrayGetLBound(V_ARRAY(&proxy->obj->v), proxy->dimensions, &I->imin);
	SafeArrayGetUBound(V_ARRAY(&proxy->obj->v), proxy->dimensions, &I->imax);

	I->key = I->imin;	
	
	return &I->iter;
}

