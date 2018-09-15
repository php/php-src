/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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
	zend_object std;
	/* the object we a proxying for; we hold a refcount to it */
	zval *zobj;
	php_com_dotnet_object *obj;

	/* how many dimensions we are indirecting to get into this element */
	LONG dimensions;

	/* this is an array whose size_is(dimensions) */
	zval *indices;

} php_com_saproxy;

typedef struct {
	zend_object_iterator iter;
	zval proxy_obj;
	zval data;
	php_com_saproxy *proxy;
	LONG key;
	LONG imin, imax;
	LONG *indices;
} php_com_saproxy_iter;

#define SA_FETCH(zv)			(php_com_saproxy*)Z_OBJ_P(zv)

static inline void clone_indices(php_com_saproxy *dest, php_com_saproxy *src, int ndims)
{
	int i;

	for (i = 0; i < ndims; i++) {
		ZVAL_DUP(&dest->indices[i], &src->indices[i]);
	}
}

static zval *saproxy_property_read(zval *object, zval *member, int type, void **cahce_slot, zval *rv)
{
	ZVAL_NULL(rv);

	php_com_throw_exception(E_INVALIDARG, "safearray has no properties");

	return rv;
}

static void saproxy_property_write(zval *object, zval *member, zval *value, void **cache_slot)
{
	php_com_throw_exception(E_INVALIDARG, "safearray has no properties");
}

static zval *saproxy_read_dimension(zval *object, zval *offset, int type, zval *rv)
{
	php_com_saproxy *proxy = SA_FETCH(object);
	UINT dims, i;
	SAFEARRAY *sa;
	LONG ubound, lbound;
	HRESULT res;

	ZVAL_NULL(rv);

	if (V_VT(&proxy->obj->v) == VT_DISPATCH) {
		VARIANT v;
		zval *args;

		/* prop-get using first dimension as the property name,
		 * all subsequent dimensions and the offset as parameters */

		args = safe_emalloc(proxy->dimensions + 1, sizeof(zval), 0);

		for (i = 1; i < (UINT) proxy->dimensions; i++) {
			args[i-1] = proxy->indices[i];
		}
		ZVAL_COPY_VALUE(&args[i-1], offset);

		convert_to_string(&proxy->indices[0]);
		VariantInit(&v);

		res = php_com_do_invoke(proxy->obj, Z_STRVAL(proxy->indices[0]),
			   	Z_STRLEN(proxy->indices[0]), DISPATCH_METHOD|DISPATCH_PROPERTYGET, &v,
			   	proxy->dimensions, args, 0);

		if (res == SUCCESS) {
			php_com_zval_from_variant(rv, &v, proxy->obj->code_page);
			VariantClear(&v);
		} else if (res == DISP_E_BADPARAMCOUNT) {
			/* return another proxy */
			php_com_saproxy_create(object, rv, offset);
		}

		return rv;

	} else if (!V_ISARRAY(&proxy->obj->v)) {
		php_com_throw_exception(E_INVALIDARG, "invalid read from com proxy object");
		return rv;
	}

	/* the SafeArray case */

	/* offset/index must be an integer */
	convert_to_long(offset);

	sa = V_ARRAY(&proxy->obj->v);
	dims = SafeArrayGetDim(sa);

	if ((UINT) proxy->dimensions >= dims) {
		/* too many dimensions */
		php_com_throw_exception(E_INVALIDARG, "too many dimensions!");
		return rv;
	}

	/* bounds check */
	SafeArrayGetLBound(sa, proxy->dimensions, &lbound);
	SafeArrayGetUBound(sa, proxy->dimensions, &ubound);

	if (Z_LVAL_P(offset) < lbound || Z_LVAL_P(offset) > ubound) {
		php_com_throw_exception(DISP_E_BADINDEX, "index out of bounds");
		return rv;
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
			convert_to_long(&proxy->indices[i]);
			indices[i] = (LONG)Z_LVAL(proxy->indices[i]);
		}

		/* add user-supplied index */
		indices[dims-1] = (LONG)Z_LVAL_P(offset);

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
			php_com_wrap_variant(rv, &v, proxy->obj->code_page);
		} else {
			php_com_throw_exception(res, NULL);
		}

		VariantClear(&v);

	} else {
		/* return another proxy */
		php_com_saproxy_create(object, rv, offset);
	}

	return rv;
}

static void saproxy_write_dimension(zval *object, zval *offset, zval *value)
{
	php_com_saproxy *proxy = SA_FETCH(object);
	UINT dims, i;
	HRESULT res;
	VARIANT v;

	if (V_VT(&proxy->obj->v) == VT_DISPATCH) {
		/* We do a prop-set using the first dimension as the property name,
		 * all subsequent dimensions and offset as parameters, with value as
		 * the final value */
		zval *args = safe_emalloc(proxy->dimensions + 2, sizeof(zval), 0);

		for (i = 1; i < (UINT) proxy->dimensions; i++) {
			ZVAL_COPY_VALUE(&args[i-1], &proxy->indices[i]);
		}
		ZVAL_COPY_VALUE(&args[i-1], offset);
		ZVAL_COPY_VALUE(&args[i], value);

		convert_to_string(&proxy->indices[0]);
		VariantInit(&v);
		if (SUCCESS == php_com_do_invoke(proxy->obj, Z_STRVAL(proxy->indices[0]),
					Z_STRLEN(proxy->indices[0]), DISPATCH_PROPERTYPUT, &v, proxy->dimensions + 1,
					args, 0)) {
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
			convert_to_long(&proxy->indices[i]);
			indices[i] = (LONG)Z_LVAL(proxy->indices[i]);
		}

		/* add user-supplied index */
		convert_to_long(offset);
		indices[dims-1] = (LONG)Z_LVAL_P(offset);

		if (FAILED(SafeArrayGetVartype(V_ARRAY(&proxy->obj->v), &vt)) || vt == VT_EMPTY) {
			vt = V_VT(&proxy->obj->v) & ~VT_ARRAY;
		}

		VariantInit(&v);
		php_com_variant_from_zval(&v, value, proxy->obj->code_page);

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
			php_com_throw_exception(res, NULL);
		}
	} else {
		php_com_throw_exception(E_NOTIMPL, "invalid write to com proxy object");
	}
}

#if 0
static void saproxy_object_set(zval **property, zval *value)
{
}

static zval *saproxy_object_get(zval *property)
{
	/* Not yet implemented in the engine */
	return NULL;
}
#endif

static int saproxy_property_exists(zval *object, zval *member, int check_empty, void **cache_slot)
{
	/* no properties */
	return 0;
}

static int saproxy_dimension_exists(zval *object, zval *member, int check_empty)
{
	php_error_docref(NULL, E_WARNING, "Operation not yet supported on a COM object");
	return 0;
}

static void saproxy_property_delete(zval *object, zval *member, void **cache_slot)
{
	php_error_docref(NULL, E_WARNING, "Cannot delete properties from a COM object");
}

static void saproxy_dimension_delete(zval *object, zval *offset)
{
	php_error_docref(NULL, E_WARNING, "Cannot delete properties from a COM object");
}

static HashTable *saproxy_properties_get(zval *object)
{
	/* no properties */
	return NULL;
}

static union _zend_function *saproxy_method_get(zend_object **object, zend_string *name, const zval *key)
{
	/* no methods */
	return NULL;
}

static int saproxy_call_method(zend_string *method, zend_object *object, INTERNAL_FUNCTION_PARAMETERS)
{
	return FAILURE;
}

static union _zend_function *saproxy_constructor_get(zend_object *object)
{
	/* user cannot instantiate */
	return NULL;
}

static zend_string* saproxy_class_name_get(const zend_object *object)
{
	return zend_string_copy(php_com_saproxy_class_entry->name);
}

static int saproxy_objects_compare(zval *object1, zval *object2)
{
	return -1;
}

static int saproxy_object_cast(zval *readobj, zval *writeobj, int type)
{
	return FAILURE;
}

static int saproxy_count_elements(zval *object, zend_long *count)
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

static void saproxy_free_storage(zend_object *object)
{
	php_com_saproxy *proxy = (php_com_saproxy *)object;
//???	int i;
//???
//???	for (i = 0; i < proxy->dimensions; i++) {
//???		if (proxy->indices) {
//???				FREE_ZVAL(proxy->indices[i]);
//???		}
//???	}

	zval_ptr_dtor(proxy->zobj);
	efree(proxy->indices);
}

static zend_object* saproxy_clone(zval *object)
{
	php_com_saproxy *proxy = (php_com_saproxy *)Z_OBJ_P(object);
	php_com_saproxy *cloneproxy;

	cloneproxy = emalloc(sizeof(*cloneproxy));
	memcpy(cloneproxy, proxy, sizeof(*cloneproxy));

	Z_ADDREF_P(cloneproxy->zobj);
	cloneproxy->indices = safe_emalloc(cloneproxy->dimensions, sizeof(zval *), 0);
	clone_indices(cloneproxy, proxy, proxy->dimensions);

	return &cloneproxy->std;
}

zend_object_handlers php_com_saproxy_handlers = {
	0,
	saproxy_free_storage,
	zend_objects_destroy_object,
	saproxy_clone,
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
	saproxy_class_name_get,
	saproxy_objects_compare,
	saproxy_object_cast,
	saproxy_count_elements
};

int php_com_saproxy_create(zval *com_object, zval *proxy_out, zval *index)
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

	ZVAL_DUP(&proxy->indices[proxy->dimensions-1], index);

	zend_object_std_init(&proxy->std, php_com_saproxy_class_entry);
	proxy->std.handlers = &php_com_saproxy_handlers;
	ZVAL_OBJ(proxy_out, &proxy->std);

	return 1;
}

/* iterator */

static void saproxy_iter_dtor(zend_object_iterator *iter)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)Z_PTR(iter->data);

	zval_ptr_dtor(&I->proxy_obj);

	efree(I->indices);
	efree(I);
}

static int saproxy_iter_valid(zend_object_iterator *iter)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)Z_PTR(iter->data);

	return (I->key < I->imax) ? SUCCESS : FAILURE;
}

static zval* saproxy_iter_get_data(zend_object_iterator *iter)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)Z_PTR(iter->data);
	VARIANT v;
	VARTYPE vt;
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

	ZVAL_NULL(&I->data);
	php_com_wrap_variant(&I->data, &v, I->proxy->obj->code_page);
	VariantClear(&v);

	return &I->data;
}

static void saproxy_iter_get_key(zend_object_iterator *iter, zval *key)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)Z_PTR(iter->data);

	if (I->key == -1) {
		ZVAL_NULL(key);
	} else {
		ZVAL_LONG(key, I->key);
	}
}

static void saproxy_iter_move_forwards(zend_object_iterator *iter)
{
	php_com_saproxy_iter *I = (php_com_saproxy_iter*)Z_PTR(iter->data);

	if (++I->key >= I->imax) {
		I->key = -1;
	}
}

static const zend_object_iterator_funcs saproxy_iter_funcs = {
	saproxy_iter_dtor,
	saproxy_iter_valid,
	saproxy_iter_get_data,
	saproxy_iter_get_key,
	saproxy_iter_move_forwards,
	NULL
};


zend_object_iterator *php_com_saproxy_iter_get(zend_class_entry *ce, zval *object, int by_ref)
{
	php_com_saproxy *proxy = SA_FETCH(object);
	php_com_saproxy_iter *I;
	int i;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	I = ecalloc(1, sizeof(*I));
	I->iter.funcs = &saproxy_iter_funcs;
	Z_PTR(I->iter.data) = I;

	I->proxy = proxy;
	ZVAL_COPY(&I->proxy_obj, object);

	I->indices = safe_emalloc(proxy->dimensions + 1, sizeof(LONG), 0);
	for (i = 0; i < proxy->dimensions; i++) {
		convert_to_long(&proxy->indices[i]);
		I->indices[i] = (LONG)Z_LVAL(proxy->indices[i]);
	}

	SafeArrayGetLBound(V_ARRAY(&proxy->obj->v), proxy->dimensions, &I->imin);
	SafeArrayGetUBound(V_ARRAY(&proxy->obj->v), proxy->dimensions, &I->imax);

	I->key = I->imin;

	return &I->iter;
}
