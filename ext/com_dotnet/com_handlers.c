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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"
#include "Zend/zend_default_classes.h"

static zval *com_property_read(zval *object, zval *member, zend_bool silent TSRMLS_DC)
{
	zval *return_value;
	php_com_dotnet_object *obj;
	VARIANT v;

	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		VariantInit(&v);

		convert_to_string_ex(&member);
		if (SUCCESS == php_com_do_invoke(obj, Z_STRVAL_P(member), Z_STRLEN_P(member),
				DISPATCH_PROPERTYGET, &v, 0, NULL TSRMLS_CC)) {
			php_com_zval_from_variant(return_value, &v, obj->code_page TSRMLS_CC);
			VariantClear(&v);
		}
	} else {
		if (!silent) {
			php_com_throw_exception(E_INVALIDARG, "this variant has no properties" TSRMLS_CC);
		}
	}

	return return_value;
}

static void com_property_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
	php_com_dotnet_object *obj;
	VARIANT v;

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		VariantInit(&v);

		convert_to_string_ex(&member);
		if (SUCCESS == php_com_do_invoke(obj, Z_STRVAL_P(member), Z_STRLEN_P(member),
				DISPATCH_PROPERTYPUT, &v, 1, &value TSRMLS_CC)) {
			VariantClear(&v);
		}
	} else {
		php_com_throw_exception(E_INVALIDARG, "this variant has no properties" TSRMLS_CC);
	}
}

static HRESULT com_get_default_binding(php_com_dotnet_object *obj TSRMLS_DC)
{
	VARDESC *vardesc;
	int i;

	if (!obj->typeinfo) {
		return FAILURE;
	}

	for (i = 0; !obj->have_default_bind; i++) {
		if (FAILED(ITypeInfo_GetVarDesc(obj->typeinfo, i, &vardesc))) {
			return FAILURE;
		}

		if (vardesc->wVarFlags & VARFLAG_FDEFAULTBIND) {
			obj->default_bind = (DISPID)vardesc->memid;
			obj->have_default_bind = 1;
		}

		ITypeInfo_ReleaseVarDesc(obj->typeinfo, vardesc);
	}
	return obj->have_default_bind ? SUCCESS : FAILURE;
}

static zval *com_read_dimension(zval *object, zval *offset TSRMLS_DC)
{
	zval *return_value;
	php_com_dotnet_object *obj;
	VARIANT v;

	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		if (!obj->have_default_bind && !com_get_default_binding(obj TSRMLS_CC)) {
			php_com_throw_exception(E_INVALIDARG, "this COM object has no default property" TSRMLS_CC);
			return return_value;
		}

		VariantInit(&v);

		if (SUCCESS == php_com_do_invoke_by_id(obj, obj->default_bind,
				DISPATCH_METHOD|DISPATCH_PROPERTYGET, &v, 1, &offset TSRMLS_CC)) {
			php_com_zval_from_variant(return_value, &v, obj->code_page TSRMLS_CC);
			VariantClear(&v);
		}
	} else if (V_ISARRAY(&obj->v)) {
		convert_to_long(offset);

		if (SafeArrayGetDim(V_ARRAY(&obj->v)) == 1) {	
			if (php_com_safearray_get_elem(&obj->v, &v, Z_LVAL_P(offset) TSRMLS_CC)) {
				php_com_wrap_variant(return_value, &v, obj->code_page TSRMLS_CC);
				VariantClear(&v);
			}
		} else {
			php_com_saproxy_create(object, return_value, Z_LVAL_P(offset) TSRMLS_CC);
		}

	} else {
		php_com_throw_exception(E_INVALIDARG, "this variant is not an array type" TSRMLS_CC);
	}

	return return_value;
}

static void com_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC)
{
	php_com_dotnet_object *obj;
	zval *args[2];
	VARIANT v;

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		if (!obj->have_default_bind && !com_get_default_binding(obj TSRMLS_CC)) {
			php_com_throw_exception(E_INVALIDARG, "this COM object has no default property" TSRMLS_CC);
			return;
		}

		args[0] = offset;
		args[1] = value;

		VariantInit(&v);

		if (SUCCESS == php_com_do_invoke_by_id(obj, obj->default_bind,
				DISPATCH_METHOD|DISPATCH_PROPERTYPUT, &v, 2, args TSRMLS_CC)) {
			VariantClear(&v);
		}
	} else {
		/* TODO: check for safearray */
		php_com_throw_exception(E_INVALIDARG, "this variant is not an array type" TSRMLS_CC);
	}
}

static void com_object_set(zval **property, zval *value TSRMLS_DC)
{
	/* Not yet implemented in the engine */
}

static zval *com_object_get(zval *property TSRMLS_DC)
{
	/* Not yet implemented in the engine */
	return NULL;
}

static int com_property_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	DISPID dispid;
	php_com_dotnet_object *obj;

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		convert_to_string_ex(&member);
		if (SUCCEEDED(php_com_get_id_of_name(obj, Z_STRVAL_P(member), Z_STRLEN_P(member), &dispid TSRMLS_CC))) {
			/* TODO: distinguish between property and method! */
			return 1;
		}
	} else {
		/* TODO: check for safearray */
	}

	return 0;
}

static int com_dimension_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Operation not yet supported on a COM object");
	return 0;
}

static void com_property_delete(zval *object, zval *member TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot delete properties from a COM object");
}

static void com_dimension_delete(zval *object, zval *offset TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot delete properties from a COM object");
}

static HashTable *com_properties_get(zval *object TSRMLS_DC)
{
	/* TODO: use type-info to get all the names and values ?
	 * DANGER: if we do that, there is a strong possibility for
	 * infinite recursion when the hash is displayed via var_dump().
	 * Perhaps it is best to leave it un-implemented.
	 */
	return NULL;
}

static union _zend_function *com_method_get(zval *object, char *name, int len TSRMLS_DC)
{
	zend_internal_function *f;
	php_com_dotnet_object *obj;

	/* TODO: cache this */

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) != VT_DISPATCH) {
		return NULL;
	}

	f = emalloc(sizeof(zend_internal_function));
	f->type = ZEND_OVERLOADED_FUNCTION_TEMPORARY;
	f->num_args = 0;
	f->arg_info = NULL;
	f->scope = obj->ce;
	f->fn_flags = 0;
	f->function_name = estrndup(name, len);

	if (obj->typeinfo) {
		/* look for byref params */
		ITypeComp *comp;
		ITypeInfo *TI = NULL;
		DESCKIND kind;
		BINDPTR bindptr;
		OLECHAR *olename;
		ULONG lhash;
		int i;

		if (SUCCEEDED(ITypeInfo_GetTypeComp(obj->typeinfo, &comp))) {
			olename = php_com_string_to_olestring(name, len, obj->code_page TSRMLS_CC);
			lhash = LHashValOfNameSys(SYS_WIN32, LOCALE_SYSTEM_DEFAULT, olename);

			if (SUCCEEDED(ITypeComp_Bind(comp, olename, lhash, INVOKE_FUNC, &TI, &kind, &bindptr))) {
				switch (kind) {
					case DESCKIND_FUNCDESC:
						f->arg_info = ecalloc(bindptr.lpfuncdesc->cParams, sizeof(zend_arg_info));

						for (i = 0; i < bindptr.lpfuncdesc->cParams; i++) {
							f->arg_info[i].allow_null = 1;
							if (bindptr.lpfuncdesc->lprgelemdescParam[i].paramdesc.wParamFlags & PARAMFLAG_FOUT) {
								f->arg_info[i].pass_by_reference = 1;
							}
						}

						f->num_args = bindptr.lpfuncdesc->cParams;

						ITypeInfo_ReleaseFuncDesc(TI, bindptr.lpfuncdesc);
						break;

					/* these should not happen, but *might* happen if the user
					 * screws up; lets avoid a leak in that case */
					case DESCKIND_VARDESC:
						ITypeInfo_ReleaseVarDesc(TI, bindptr.lpvardesc);
						break;
					case DESCKIND_TYPECOMP:
						ITypeComp_Release(bindptr.lptcomp);
						break;
				}
				if (TI) {
					ITypeInfo_Release(TI);
				}
			}
			ITypeComp_Release(comp);
			efree(olename);
		}
	}

	return (union _zend_function*)f;
}

static int com_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	zval **args = NULL;
	php_com_dotnet_object *obj;
	int nargs;
	VARIANT v;
	int ret = FAILURE;

	obj = CDNO_FETCH(getThis());

	if (V_VT(&obj->v) != VT_DISPATCH) {
		return FAILURE;
	}
	
	nargs = ZEND_NUM_ARGS();

	if (nargs) {
		args = (zval **)safe_emalloc(sizeof(zval *), nargs, 0);
		zend_get_parameters_array(ht, nargs, args);
	}

	VariantInit(&v);

	if (SUCCESS == php_com_do_invoke(obj, method, -1, DISPATCH_METHOD|DISPATCH_PROPERTYGET, &v, nargs, args TSRMLS_CC)) {
		php_com_zval_from_variant(return_value, &v, obj->code_page TSRMLS_CC);
		ret = SUCCESS;
		VariantClear(&v);
	}

	if (args) {
		efree(args);
	}

	return ret;
}

static union _zend_function *com_constructor_get(zval *object TSRMLS_DC)
{
	php_com_dotnet_object *obj;
	zend_internal_function *f;

	obj = CDNO_FETCH(object);

	/* TODO: this leaks */
	f = emalloc(sizeof(zend_internal_function));
	f->type = ZEND_INTERNAL_FUNCTION;

	f->function_name = obj->ce->name;
	f->scope = obj->ce;
	f->arg_info = NULL;
	f->num_args = 0;
	f->fn_flags = 0;
#if HAVE_MSCOREE_H
	if (f->function_name[0] == 'd') { /* 'd'otnet */
		f->handler = ZEND_FN(com_dotnet_create_instance);
	} else 
#endif
	if (f->function_name[0] == 'c') { /* 'c'om */
		f->handler = ZEND_FN(com_create_instance);
	} else { /* 'v'ariant */
		f->handler = ZEND_FN(com_variant_create_instance);
	}

	return (union _zend_function*)f;
}

static zend_class_entry *com_class_entry_get(zval *object TSRMLS_DC)
{
	php_com_dotnet_object *obj;
	obj = CDNO_FETCH(object);

	return obj->ce;
}

static int com_class_name_get(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
	php_com_dotnet_object *obj;
	obj = CDNO_FETCH(object);

	*class_name = estrndup(obj->ce->name, obj->ce->name_length);
	*class_name_len = obj->ce->name_length;

	return 0;
}

/* This compares two variants for equality */
static int com_objects_compare(zval *object1, zval *object2 TSRMLS_DC)
{
	php_com_dotnet_object *obja, *objb;
	int ret;
	/* strange header bug problem here... the headers define the proto without the
	 * flags parameter.  However, the MSDN docs state that there is a flags parameter,
	 * and my VC6 won't link unless the code uses the version with 4 parameters.
	 * So, we have this declaration here to fix it */
	STDAPI VarCmp(LPVARIANT pvarLeft, LPVARIANT pvarRight, LCID lcid, DWORD flags);

	obja = CDNO_FETCH(object1);
	objb = CDNO_FETCH(object2);

	switch (VarCmp(&obja->v, &objb->v, LOCALE_SYSTEM_DEFAULT, 0)) {
		case VARCMP_LT:
			ret = -1;
			break;
		case VARCMP_GT:
			ret = 1;
			break;
		case VARCMP_EQ:
			ret = 0;
			break;
		default:
			/* either or both operands are NULL...
			 * not 100% sure how to handle this */
			ret = -2;
	}

	return ret;
}

static int com_object_cast(zval *readobj, zval *writeobj, int type, int should_free TSRMLS_DC)
{
	php_com_dotnet_object *obj;
	VARIANT v;
	VARTYPE vt = VT_EMPTY;

	if (should_free) {
		zval_dtor(writeobj);
	}

	ZVAL_NULL(writeobj);

	obj = CDNO_FETCH(readobj);
	VariantInit(&v);

	if (V_VT(&obj->v) == VT_DISPATCH) {

		if (!obj->have_default_bind && !com_get_default_binding(obj TSRMLS_CC)) {
			return FAILURE;
		}

		if (FAILURE == php_com_do_invoke_by_id(obj, obj->default_bind,
				DISPATCH_METHOD|DISPATCH_PROPERTYGET, &v, 0, NULL TSRMLS_CC)) {
			return FAILURE;
		}
	} else {
		VariantCopy(&v, &obj->v);
	}

	switch(type) {
		case IS_LONG:
			vt = VT_INT;
			break;
		case IS_DOUBLE:
			vt = VT_R8;
			break;
		case IS_BOOL:
			vt = VT_BOOL;
			break;
		case IS_STRING:
			vt = VT_BSTR;
			break;
	}

	if (vt != VT_EMPTY) {
		VariantChangeType(&v, &v, 0, vt);
	}

	php_com_zval_from_variant(writeobj, &v, obj->code_page TSRMLS_CC);
	VariantClear(&v);
	return SUCCESS;
}

zend_object_handlers php_com_object_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	com_property_read,
	com_property_write,
	com_read_dimension,
	com_write_dimension,
	NULL,
	com_object_get,
	com_object_set,
	com_property_exists,
	com_property_delete,
	com_dimension_exists,
	com_dimension_delete,
	com_properties_get,
	com_method_get,
	com_call_method,
	com_constructor_get,
	com_class_entry_get,
	com_class_name_get,
	com_objects_compare,
	com_object_cast
};

void php_com_object_enable_event_sink(php_com_dotnet_object *obj, int enable TSRMLS_DC)
{
	if (obj->sink_dispatch) {
		IConnectionPointContainer *cont;
		IConnectionPoint *point;
		
		if (SUCCEEDED(IDispatch_QueryInterface(V_DISPATCH(&obj->v),
				&IID_IConnectionPointContainer, (void**)&cont))) {
			
			if (SUCCEEDED(IConnectionPointContainer_FindConnectionPoint(cont,
					&obj->sink_id, &point))) {

				if (enable) {
					IConnectionPoint_Advise(point, (IUnknown*)obj->sink_dispatch, &obj->sink_cookie);
				} else {
					IConnectionPoint_Unadvise(point, obj->sink_cookie);
				}
				IConnectionPoint_Release(point);
			}
			IConnectionPointContainer_Release(cont);
		}
	}
}

void php_com_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	php_com_dotnet_object *obj = (php_com_dotnet_object*)object;

	if (obj->typeinfo) {
		ITypeInfo_Release(obj->typeinfo);
		obj->typeinfo = NULL;
	}

	if (obj->sink_dispatch) {
		php_com_object_enable_event_sink(obj, FALSE TSRMLS_CC);
		IDispatch_Release(obj->sink_dispatch);
		obj->sink_dispatch = NULL;
	}

	VariantClear(&obj->v);
	efree(obj);
}

void php_com_object_clone(void *object, void **clone_ptr TSRMLS_DC)
{
	php_com_dotnet_object *cloneobj, *origobject;

	origobject = (php_com_dotnet_object*)object;
	cloneobj = (php_com_dotnet_object*)emalloc(sizeof(php_com_dotnet_object));
	
	memcpy(cloneobj, origobject, sizeof(*cloneobj));

	/* VariantCopy will perform VariantClear; we don't want to clobber
	 * the IDispatch that we memcpy'd, so we init a new variant in the
	 * clone structure */
	VariantInit(&cloneobj->v);
	/* We use the Indirection-following version of the API since we
	 * want to clone as much as possible */
	VariantCopyInd(&cloneobj->v, &origobject->v); 

	if (cloneobj->typeinfo) {
		ITypeInfo_AddRef(cloneobj->typeinfo);
	}

	*clone_ptr = cloneobj;
}

zend_object_value php_com_object_new(zend_class_entry *ce TSRMLS_DC)
{
	php_com_dotnet_object *obj;
	zend_object_value retval;

	obj = emalloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));

	VariantInit(&obj->v);
	obj->code_page = CP_ACP;
	obj->ce = ce;

	retval.handle = zend_objects_store_put(obj, php_com_object_dtor, php_com_object_clone TSRMLS_CC);
	retval.handlers = &php_com_object_handlers;

	return retval;
}
