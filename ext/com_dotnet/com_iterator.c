/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

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
	ulong key;
	VARIANT v; /* cached element */
	int code_page;
	VARIANT safe_array;
	VARTYPE sa_type;
	LONG sa_max;
	zval *zdata;
};

static void com_iter_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	struct php_com_iterator *I = (struct php_com_iterator*)iter->data;
	
	if (I->ev) {
		IEnumVARIANT_Release(I->ev);
	}
	VariantClear(&I->v);
	VariantClear(&I->safe_array);
	if (I->zdata) {
		zval_ptr_dtor((zval**)&I->zdata);
	}
	efree(I);
}

static int com_iter_valid(zend_object_iterator *iter TSRMLS_DC)
{
	struct php_com_iterator *I = (struct php_com_iterator*)iter->data;

	if (I->zdata) {
		return SUCCESS;
	}

	return FAILURE;
}

static void com_iter_get_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	struct php_com_iterator *I = (struct php_com_iterator*)iter->data;

	*data = &I->zdata;
}

static void com_iter_get_key(zend_object_iterator *iter, zval *key TSRMLS_DC)
{
	struct php_com_iterator *I = (struct php_com_iterator*)iter->data;

	if (I->key == (ulong)-1) {
		ZVAL_NULL(key);
	} else {
		ZVAL_LONG(key, I->key);
	}
}

static int com_iter_move_forwards(zend_object_iterator *iter TSRMLS_DC)
{
	struct php_com_iterator *I = (struct php_com_iterator*)iter->data;
	unsigned long n_fetched;
	zval *ptr;

	/* release current cached element */
	VariantClear(&I->v);

	if (I->zdata) {
		zval_ptr_dtor((zval**)&I->zdata);
		I->zdata = NULL;
	}

	if (I->ev) {
		/* Get the next element */
		if (SUCCEEDED(IEnumVARIANT_Next(I->ev, 1, &I->v, &n_fetched)) && n_fetched > 0) {
			I->key++;
		} else {
			/* indicate that there are no more items */
			I->key = (ulong)-1;
			return FAILURE;
		}
	} else {
		/* safe array */
		if (I->key >= (ULONG) I->sa_max) {
			I->key = (ulong)-1;
			return FAILURE;
		}
		I->key++;
		if (php_com_safearray_get_elem(&I->safe_array, &I->v, (LONG)I->key TSRMLS_CC) == 0) {
			I->key = (ulong)-1;
			return FAILURE;
		}
	}

	MAKE_STD_ZVAL(ptr);
	php_com_zval_from_variant(ptr, &I->v, I->code_page TSRMLS_CC);
	/* php_com_wrap_variant(ptr, &I->v, I->code_page TSRMLS_CC); */
	I->zdata = ptr;
	return SUCCESS;
}


static zend_object_iterator_funcs com_iter_funcs = {
	com_iter_dtor,
	com_iter_valid,
	com_iter_get_data,
	com_iter_get_key,
	com_iter_move_forwards,
	NULL
};

zend_object_iterator *php_com_iter_get(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	php_com_dotnet_object *obj;
	struct php_com_iterator *I;
	IEnumVARIANT *iev = NULL;
	DISPPARAMS dp;
	VARIANT v;
	unsigned long n_fetched;
	zval *ptr;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) != VT_DISPATCH && !V_ISARRAY(&obj->v)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "variant is not an object or array VT=%d", V_VT(&obj->v));
		return NULL;
	}

	memset(&dp, 0, sizeof(dp));
	VariantInit(&v);	

	I = (struct php_com_iterator*)ecalloc(1, sizeof(*I));
	I->iter.funcs = &com_iter_funcs;
	I->iter.data = I;
	I->code_page = obj->code_page;
	I->zdata = NULL;
	VariantInit(&I->safe_array);
	VariantInit(&I->v);

	if (V_ISARRAY(&obj->v)) {
		LONG bound;
		UINT dims;
	
		dims = SafeArrayGetDim(V_ARRAY(&obj->v));

		if (dims != 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
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
		if (php_com_safearray_get_elem(&I->safe_array, &I->v, bound TSRMLS_CC)) {
			I->key = bound;
			MAKE_STD_ZVAL(ptr);
			php_com_zval_from_variant(ptr, &I->v, I->code_page TSRMLS_CC);
			I->zdata = ptr;
		} else {
			I->key = (ulong)-1;
		}
		
	} else {
		/* can we enumerate it? */
		if (FAILED(IDispatch_Invoke(V_DISPATCH(&obj->v), DISPID_NEWENUM,
						&IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD|DISPATCH_PROPERTYGET,
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
			MAKE_STD_ZVAL(ptr);
			php_com_zval_from_variant(ptr, &I->v, I->code_page TSRMLS_CC);
			I->zdata = ptr;
		} else {
			/* indicate that there are no more items */
			I->key = (ulong)-1;
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

