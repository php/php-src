/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"
#include "Zend/zend_default_classes.h"

zval *php_com_throw_exception(char *message TSRMLS_DC)
{
	zval *e, *tmp;
	
	ALLOC_ZVAL(e);
	Z_TYPE_P(e) = IS_OBJECT;
	object_init_ex(e, zend_exception_get_default());
	e->refcount = 1;
	e->is_ref = 1;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, message, 1);
	zend_hash_update(Z_OBJPROP_P(e), "message", sizeof("message"), (void**)&tmp, sizeof(zval*), NULL);

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, zend_get_executed_filename(TSRMLS_C), 1);
	zend_hash_update(Z_OBJPROP_P(e), "file", sizeof("file"), (void**)&tmp, sizeof(zval*), NULL);

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, zend_get_executed_lineno(TSRMLS_C));
	zend_hash_update(Z_OBJPROP_P(e), "line", sizeof("line"), (void**)&tmp, sizeof(zval*), NULL);

	EG(exception) = e;

	return e;
}

PHPAPI void php_com_wrap_dispatch(zval *z, IDispatch *disp,
		int codepage TSRMLS_DC)
{
	php_com_dotnet_object *obj;

	obj = emalloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));
	obj->code_page = codepage;
	obj->ce = php_com_variant_class_entry;

	VariantInit(&obj->v);
	V_VT(&obj->v) = VT_DISPATCH;
	V_DISPATCH(&obj->v) = disp;

	IDispatch_AddRef(V_DISPATCH(&obj->v));
	IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &obj->typeinfo);

	Z_TYPE_P(z) = IS_OBJECT;
	z->value.obj.handle = zend_objects_store_put(obj, php_com_object_dtor, php_com_object_clone TSRMLS_CC);
	z->value.obj.handlers = &php_com_object_handlers;
}

PHPAPI void php_com_wrap_variant(zval *z, VARIANT *v,
		int codepage TSRMLS_DC)
{
	php_com_dotnet_object *obj;

	obj = emalloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));
	obj->code_page = codepage;
	obj->ce = php_com_variant_class_entry;

	VariantInit(&obj->v);
	VariantCopy(&obj->v, v);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &obj->typeinfo);
	}

	Z_TYPE_P(z) = IS_OBJECT;
	
	z->value.obj.handle = zend_objects_store_put(obj, php_com_object_dtor, php_com_object_clone TSRMLS_CC);
	z->value.obj.handlers = &php_com_object_handlers;
}
