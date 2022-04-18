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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"
#include "Zend/zend_exceptions.h"

struct php_com_iterator {
	zend_object_iterator iter;
	IEnumVARIANT *ev;
	zend_ulong key;
	VARIANT v; /* cached element */
	int code_page;
	VARIANT safe_array;
	VARTYPE sa_type;
	LONG sa_max;
	zval zdata;
};

static void com_iter_dtor(zend_object_iterator *iter)
{
	struct php_com_iterator *I = (struct php_com_iterator*)Z_PTR(iter->data);

	if (I->ev) {
		IEnumVARIANT_Release(I->ev);
	}
	VariantClear(&I->v);
	VariantClear(&I->safe_array);
	zval_ptr_dtor(&I->zdata);
}

static int com_iter_valid(zend_object_iterator *iter)
{
	struct php_com_iterator *I = (struct php_com_iterator*)Z_PTR(iter->data);

	if (Z_TYPE(I->zdata) != IS_UNDEF) {
		return SUCCESS;
	}

	return FAILURE;
}

static zval* com_iter_get_data(zend_object_iterator *iter)
{
	struct php_com_iterator *I = (struct php_com_iterator*)Z_PTR(iter->data);

	return &I->zdata;
}

static void com_iter_get_key(zend_object_iterator *iter, zval *key)
{
	struct php_com_iterator *I = (struct php_com_iterator*)Z_PTR(iter->data);

	if (I->key == (zend_ulong)-1) {
		ZVAL_NULL(key);
	} else {
		ZVAL_LONG(key, I->key);
	}
}

static void com_iter_move_forwards(zend_object_iterator *iter)
{
	struct php_com_iterator *I = (struct php_com_iterator*)Z_PTR(iter->data);
	unsigned long n_fetched;
	zval ptr;

	/* release current cached element */
	VariantClear(&I->v);

	if (Z_TYPE(I->zdata) != IS_UNDEF) {
		zval_ptr_dtor(&I->zdata);
		ZVAL_UNDEF(&I->zdata);
	}

	if (I->ev) {
		/* Get the next element */
		if (SUCCEEDED(IEnumVARIANT_Next(I->ev, 1, &I->v, &n_fetched)) && n_fetched > 0) {
			I->key++;
		} else {
			/* indicate that there are no more items */
			I->key = (zend_ulong)-1;
			return;
		}
	} else {
		/* safe array */
		if (I->key >= (ULONG) I->sa_max) {
			I->key = (zend_ulong)-1;
			return;
		}
		I->key++;
		if (!php_com_safearray_get_elem(&I->safe_array, &I->v, (LONG)I->key)) {
			I->key = (zend_ulong)-1;
			return;
		}
	}

	ZVAL_NULL(&ptr);
	php_com_zval_from_variant(&ptr, &I->v, I->code_page);
	/* php_com_wrap_variant(ptr, &I->v, I->code_page); */
	ZVAL_COPY_VALUE(&I->zdata, &ptr);
}


static const zend_object_iterator_funcs com_iter_funcs = {
	com_iter_dtor,
	com_iter_valid,
	com_iter_get_data,
	com_iter_get_key,
	com_iter_move_forwards,
	NULL,
	NULL, /* get_gc */
};

zend_object_iterator *php_com_iter_get(zend_class_entry *ce, zval *object, int by_ref)
{
	php_com_dotnet_object *obj;
	struct php_com_iterator *I;
	IEnumVARIANT *iev = NULL;
	DISPPARAMS dp;
	VARIANT v;
	unsigned long n_fetched;
	zval ptr;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) != VT_DISPATCH && !V_ISARRAY(&obj->v)) {
		/* TODO Promote to TypeError? */
		php_error_docref(NULL, E_WARNING, "Variant is not an object or array VT=%d", V_VT(&obj->v));
		return NULL;
	}

	memset(&dp, 0, sizeof(dp));
	VariantInit(&v);

	I = (struct php_com_iterator*)ecalloc(1, sizeof(*I));
	zend_iterator_init(&I->iter);
	I->iter.funcs = &com_iter_funcs;
	Z_PTR(I->iter.data) = I;
	I->code_page = obj->code_page;
	ZVAL_UNDEF(&I->zdata);
	VariantInit(&I->safe_array);
	VariantInit(&I->v);

	if (V_ISARRAY(&obj->v)) {
		LONG bound;
		UINT dims;

		dims = SafeArrayGetDim(V_ARRAY(&obj->v));

		if (dims != 1) {
			/* TODO Promote to ValueError? */
			php_error_docref(NULL, E_WARNING,
				   "Can only handle single dimension variant arrays (this array has %d)", dims);
			goto fail;
		}

		/* same semantics as foreach on a PHP array;
		 * make a copy and enumerate that copy */
		VariantCopy(&I->safe_array, &obj->v);

		/* determine the key value for the array */
		SafeArrayGetLBound(V_ARRAY(&I->safe_array), 1, &bound);
		SafeArrayGetUBound(V_ARRAY(&I->safe_array), 1, &I->sa_max);

		/* pre-fetch the element */
		if (I->sa_max >= bound && php_com_safearray_get_elem(&I->safe_array, &I->v, bound)) {
			I->key = bound;
			ZVAL_NULL(&ptr);
			php_com_zval_from_variant(&ptr, &I->v, I->code_page);
			ZVAL_COPY_VALUE(&I->zdata, &ptr);
		} else {
			I->key = (zend_ulong)-1;
		}

	} else {
		/* can we enumerate it? */
		if (FAILED(IDispatch_Invoke(V_DISPATCH(&obj->v), DISPID_NEWENUM,
						&IID_NULL, LOCALE_NEUTRAL, DISPATCH_METHOD|DISPATCH_PROPERTYGET,
						&dp, &v, NULL, NULL))) {
			goto fail;
		}

		/* get something useful out of it */
		if (V_VT(&v) == VT_UNKNOWN) {
			IUnknown_QueryInterface(V_UNKNOWN(&v), &IID_IEnumVARIANT, (void**)&iev);
		} else if (V_VT(&v) == VT_DISPATCH) {
			IDispatch_QueryInterface(V_DISPATCH(&v), &IID_IEnumVARIANT, (void**)&iev);
		}

		VariantClear(&v);

		if (iev == NULL) {
			goto fail;
		}

		I->ev = iev;

		/* Get the first element now */
		if (SUCCEEDED(IEnumVARIANT_Next(I->ev, 1, &I->v, &n_fetched)) && n_fetched > 0) {
			/* indicate that we have element 0 */
			I->key = 0;
			ZVAL_NULL(&ptr);
			php_com_zval_from_variant(&ptr, &I->v, I->code_page);
			ZVAL_COPY_VALUE(&I->zdata, &ptr);
		} else {
			/* indicate that there are no more items */
			I->key = (zend_ulong)-1;
		}
	}

	return &I->iter;

fail:
	if (I) {
		VariantClear(&I->safe_array);
		VariantClear(&I->v);
		efree(I);
	}
	return NULL;
}
