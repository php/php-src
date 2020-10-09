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
   | Author: Wez Furlong  <wez@thebrainroom.com>                          |
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

void php_com_throw_exception(HRESULT code, char *message)
{
	int free_msg = 0;
	if (message == NULL) {
		message = php_win32_error_to_msg(code);
		free_msg = 1;
	}
#if SIZEOF_ZEND_LONG == 8
	zend_throw_exception(php_com_exception_class_entry, message, (zend_long)(uint32_t)code);
#else
	zend_throw_exception(php_com_exception_class_entry, message, (zend_long)code);
#endif
	if (free_msg) {
		php_win32_error_msg_free(message);
	}
}

PHP_COM_DOTNET_API void php_com_wrap_dispatch(zval *z, IDispatch *disp,
		int codepage)
{
	php_com_dotnet_object *obj;

	obj = emalloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));
	obj->code_page = codepage;
	obj->ce = php_com_variant_class_entry;
	obj->zo.ce = php_com_variant_class_entry;

	VariantInit(&obj->v);
	V_VT(&obj->v) = VT_DISPATCH;
	V_DISPATCH(&obj->v) = disp;

	IDispatch_AddRef(V_DISPATCH(&obj->v));
	IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &obj->typeinfo);

	zend_object_std_init(&obj->zo, php_com_variant_class_entry);
	obj->zo.handlers = &php_com_object_handlers;
	ZVAL_OBJ(z, &obj->zo);
}

PHP_COM_DOTNET_API void php_com_wrap_variant(zval *z, VARIANT *v,
		int codepage)
{
	php_com_dotnet_object *obj;

	obj = emalloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));
	obj->code_page = codepage;
	obj->ce = php_com_variant_class_entry;
	obj->zo.ce = php_com_variant_class_entry;

	VariantInit(&obj->v);
	VariantCopyInd(&obj->v, v);
	obj->modified = 0;

	if ((V_VT(&obj->v) == VT_DISPATCH) && (V_DISPATCH(&obj->v) != NULL)) {
		IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &obj->typeinfo);
	}

	zend_object_std_init(&obj->zo, php_com_variant_class_entry);
	obj->zo.handlers = &php_com_object_handlers;
	ZVAL_OBJ(z, &obj->zo);
}

/* this is a convenience function for fetching a particular
 * element from a (possibly multi-dimensional) safe array */
PHP_COM_DOTNET_API int php_com_safearray_get_elem(VARIANT *array, VARIANT *dest, LONG dim1)
{
	UINT dims;
	LONG lbound, ubound;
	LONG indices[1];
	VARTYPE vt;

	if (!V_ISARRAY(array)) {
		return 0;
	}

	dims = SafeArrayGetDim(V_ARRAY(array));

	if (dims != 1) {
		/* TODO Promote to ValueError? */
		php_error_docref(NULL, E_WARNING,
			   "Can only handle single dimension variant arrays (this array has %d)", dims);
		return 0;
	}

	if (FAILED(SafeArrayGetVartype(V_ARRAY(array), &vt)) || vt == VT_EMPTY) {
		vt = V_VT(array) & ~VT_ARRAY;
	}

	/* determine the bounds */
	SafeArrayGetLBound(V_ARRAY(array), 1, &lbound);
	SafeArrayGetUBound(V_ARRAY(array), 1, &ubound);

	/* check bounds */
	if (dim1 < lbound || dim1 > ubound) {
		php_com_throw_exception(DISP_E_BADINDEX, "index out of bounds");
		return 0;
	}

	/* now fetch that element */
	VariantInit(dest);

	indices[0] = dim1;

	if (vt == VT_VARIANT) {
		SafeArrayGetElement(V_ARRAY(array), indices, dest);
	} else {
		V_VT(dest) = vt;
		/* store the value into "lVal" member of the variant.
		 * This works because it is a union; since we know the variant
		 * type, we end up with a working variant */
		SafeArrayGetElement(V_ARRAY(array), indices, &dest->lVal);
	}

	return 1;
}
