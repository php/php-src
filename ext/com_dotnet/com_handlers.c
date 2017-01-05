/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"
#include "Zend/zend_exceptions.h"

static zval *com_property_read(zval *object, zval *member, int type, void **cahce_slot, zval *rv)
{
	php_com_dotnet_object *obj;
	VARIANT v;
	HRESULT res;

	ZVAL_NULL(rv);

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		VariantInit(&v);

		convert_to_string_ex(member);

		res = php_com_do_invoke(obj, Z_STRVAL_P(member), Z_STRLEN_P(member),
				DISPATCH_METHOD|DISPATCH_PROPERTYGET, &v, 0, NULL, 1);

		if (res == SUCCESS) {
			php_com_zval_from_variant(rv, &v, obj->code_page);
			VariantClear(&v);
		} else if (res == DISP_E_BADPARAMCOUNT) {
			php_com_saproxy_create(object, rv, member);
		}
	} else {
		php_com_throw_exception(E_INVALIDARG, "this variant has no properties");
	}

	return rv;
}

static void com_property_write(zval *object, zval *member, zval *value, void **cache_slot)
{
	php_com_dotnet_object *obj;
	VARIANT v;

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		VariantInit(&v);

		convert_to_string_ex(member);
		if (SUCCESS == php_com_do_invoke(obj, Z_STRVAL_P(member), Z_STRLEN_P(member),
				DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF, &v, 1, value, 0)) {
			VariantClear(&v);
		}
	} else {
		php_com_throw_exception(E_INVALIDARG, "this variant has no properties");
	}
}

static zval *com_read_dimension(zval *object, zval *offset, int type, zval *rv)
{
	php_com_dotnet_object *obj;
	VARIANT v;

	ZVAL_NULL(rv);

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		VariantInit(&v);

		if (SUCCESS == php_com_do_invoke_by_id(obj, DISPID_VALUE,
				DISPATCH_METHOD|DISPATCH_PROPERTYGET, &v, 1, offset, 0, 0)) {
			php_com_zval_from_variant(rv, &v, obj->code_page);
			VariantClear(&v);
		}
	} else if (V_ISARRAY(&obj->v)) {
		convert_to_long(offset);

		if (SafeArrayGetDim(V_ARRAY(&obj->v)) == 1) {
			if (php_com_safearray_get_elem(&obj->v, &v, (LONG)Z_LVAL_P(offset))) {
				php_com_wrap_variant(rv, &v, obj->code_page);
				VariantClear(&v);
			}
		} else {
			php_com_saproxy_create(object, rv, offset);
		}

	} else {
		php_com_throw_exception(E_INVALIDARG, "this variant is not an array type");
	}

	return rv;
}

static void com_write_dimension(zval *object, zval *offset, zval *value)
{
	php_com_dotnet_object *obj;
	zval args[2];
	VARIANT v;
	HRESULT res;

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		ZVAL_COPY_VALUE(&args[0], offset);
		ZVAL_COPY_VALUE(&args[1], value);

		VariantInit(&v);

		if (SUCCESS == php_com_do_invoke_by_id(obj, DISPID_VALUE,
				DISPATCH_METHOD|DISPATCH_PROPERTYPUT, &v, 2, args, 0, 0)) {
			VariantClear(&v);
		}
	} else if (V_ISARRAY(&obj->v)) {
		LONG indices = 0;
		VARTYPE vt;

		if (SafeArrayGetDim(V_ARRAY(&obj->v)) == 1) {
			if (FAILED(SafeArrayGetVartype(V_ARRAY(&obj->v), &vt)) || vt == VT_EMPTY) {
				vt = V_VT(&obj->v) & ~VT_ARRAY;
			}

			convert_to_long(offset);
			indices = (LONG)Z_LVAL_P(offset);

			VariantInit(&v);
			php_com_variant_from_zval(&v, value, obj->code_page);

			if (V_VT(&v) != vt) {
				VariantChangeType(&v, &v, 0, vt);
			}

			if (vt == VT_VARIANT) {
				res = SafeArrayPutElement(V_ARRAY(&obj->v), &indices, &v);
			} else {
				res = SafeArrayPutElement(V_ARRAY(&obj->v), &indices, &v.lVal);
			}

			VariantClear(&v);

			if (FAILED(res)) {
				php_com_throw_exception(res, NULL);
			}

		} else {
			php_com_throw_exception(DISP_E_BADINDEX, "this variant has multiple dimensions; you can't set a new value without specifying *all* dimensions");
		}

	} else {
		php_com_throw_exception(E_INVALIDARG, "this variant is not an array type");
	}
}

#if 0
static void com_object_set(zval **property, zval *value)
{
	/* Not yet implemented in the engine */
}

static zval *com_object_get(zval *property)
{
	/* Not yet implemented in the engine */
	return NULL;
}
#endif

static int com_property_exists(zval *object, zval *member, int check_empty, void **cache_slot)
{
	DISPID dispid;
	php_com_dotnet_object *obj;

	obj = CDNO_FETCH(object);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		convert_to_string_ex(member);
		if (SUCCEEDED(php_com_get_id_of_name(obj, Z_STRVAL_P(member), Z_STRLEN_P(member), &dispid))) {
			/* TODO: distinguish between property and method! */
			return 1;
		}
	} else {
		/* TODO: check for safearray */
	}

	return 0;
}

static int com_dimension_exists(zval *object, zval *member, int check_empty)
{
	php_error_docref(NULL, E_WARNING, "Operation not yet supported on a COM object");
	return 0;
}

static void com_property_delete(zval *object, zval *member, void **cache_slot)
{
	php_error_docref(NULL, E_WARNING, "Cannot delete properties from a COM object");
}

static void com_dimension_delete(zval *object, zval *offset)
{
	php_error_docref(NULL, E_WARNING, "Cannot delete properties from a COM object");
}

static HashTable *com_properties_get(zval *object)
{
	/* TODO: use type-info to get all the names and values ?
	 * DANGER: if we do that, there is a strong possibility for
	 * infinite recursion when the hash is displayed via var_dump().
	 * Perhaps it is best to leave it un-implemented.
	 */
	return NULL;
}

static void function_dtor(zval *zv)
{
	zend_internal_function *f = (zend_internal_function*)Z_PTR_P(zv);

	zend_string_release(f->function_name);
	if (f->arg_info) {
		efree(f->arg_info);
	}
	efree(f);
}

static PHP_FUNCTION(com_method_handler)
{
	zval *object = getThis();

	Z_OBJ_HANDLER_P(object, call_method)(
			((zend_internal_function*)EX(func))->function_name,
			Z_OBJ_P(object),
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static union _zend_function *com_method_get(zend_object **object_ptr, zend_string *name, const zval *key)
{
	zend_internal_function f, *fptr = NULL;
	union _zend_function *func;
	DISPID dummy;
	php_com_dotnet_object *obj = (php_com_dotnet_object*)*object_ptr;

	if (V_VT(&obj->v) != VT_DISPATCH) {
		return NULL;
	}

	if (FAILED(php_com_get_id_of_name(obj, name->val, name->len, &dummy))) {
		return NULL;
	}

	/* check cache */
	if (obj->method_cache == NULL || NULL == (fptr = zend_hash_find_ptr(obj->method_cache, name))) {
		f.type = ZEND_OVERLOADED_FUNCTION;
		f.num_args = 0;
		f.arg_info = NULL;
		f.scope = obj->ce;
		f.fn_flags = ZEND_ACC_CALL_VIA_HANDLER;
		f.function_name = zend_string_copy(name);
		f.handler = PHP_FN(com_method_handler);

		fptr = &f;

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
				olename = php_com_string_to_olestring(name->val, name->len, obj->code_page);
				lhash = LHashValOfNameSys(SYS_WIN32, LOCALE_SYSTEM_DEFAULT, olename);

				if (SUCCEEDED(ITypeComp_Bind(comp, olename, lhash, INVOKE_FUNC, &TI, &kind, &bindptr))) {
					switch (kind) {
						case DESCKIND_FUNCDESC:
							f.arg_info = ecalloc(bindptr.lpfuncdesc->cParams, sizeof(zend_arg_info));

							for (i = 0; i < bindptr.lpfuncdesc->cParams; i++) {
								f.arg_info[i].allow_null = 1;
								if (bindptr.lpfuncdesc->lprgelemdescParam[i].paramdesc.wParamFlags & PARAMFLAG_FOUT) {
									f.arg_info[i].pass_by_reference = ZEND_SEND_BY_REF;
								}
							}

							f.num_args = bindptr.lpfuncdesc->cParams;

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

						case DESCKIND_NONE:
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

		zend_set_function_arg_flags((zend_function*)&f);
		/* save this method in the cache */
		if (!obj->method_cache) {
			ALLOC_HASHTABLE(obj->method_cache);
			zend_hash_init(obj->method_cache, 2, NULL, function_dtor, 0);
		}

		zend_hash_update_mem(obj->method_cache, name, &f, sizeof(f));
	}

	if (fptr) {
		/* duplicate this into a new chunk of emalloc'd memory,
		 * since the engine will efree it */
		func = emalloc(sizeof(*fptr));
		memcpy(func, fptr, sizeof(*fptr));

		return func;
	}

	return NULL;
}

static int com_call_method(zend_string *method, zend_object *object, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *args = NULL;
	php_com_dotnet_object *obj = (php_com_dotnet_object*)object;
	int nargs;
	VARIANT v;
	int ret = FAILURE;

	if (V_VT(&obj->v) != VT_DISPATCH) {
		return FAILURE;
	}

	nargs = ZEND_NUM_ARGS();

	if (nargs) {
		args = (zval *)safe_emalloc(sizeof(zval), nargs, 0);
		zend_get_parameters_array_ex(nargs, args);
	}

	VariantInit(&v);

	if (SUCCESS == php_com_do_invoke_byref(obj, (zend_internal_function*)EX(func), DISPATCH_METHOD|DISPATCH_PROPERTYGET, &v, nargs, args)) {
		php_com_zval_from_variant(return_value, &v, obj->code_page);
		ret = SUCCESS;
		VariantClear(&v);
	}

	if (args) {
		efree(args);
	}

	return ret;
}

static union _zend_function *com_constructor_get(zend_object *object)
{
	php_com_dotnet_object *obj = (php_com_dotnet_object *) object;
	static zend_internal_function c, d, v;

#define POPULATE_CTOR(f, fn)	\
	f.type = ZEND_INTERNAL_FUNCTION; \
	f.function_name = obj->ce->name; \
	f.scope = obj->ce; \
	f.arg_info = NULL; \
	f.num_args = 0; \
	f.fn_flags = 0; \
	f.handler = ZEND_FN(fn); \
	return (union _zend_function*)&f;

	switch (obj->ce->name->val[0]) {
#if HAVE_MSCOREE_H
		case 'd':
			POPULATE_CTOR(d, com_dotnet_create_instance);
#endif

		case 'c':
			POPULATE_CTOR(c, com_create_instance);

		case 'v':
			POPULATE_CTOR(v, com_variant_create_instance);

		default:
			return NULL;
	}
}

static zend_string* com_class_name_get(const zend_object *object)
{
	php_com_dotnet_object *obj = (php_com_dotnet_object *)object;

	return zend_string_copy(obj->ce->name);
}

/* This compares two variants for equality */
static int com_objects_compare(zval *object1, zval *object2)
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

static int com_object_cast(zval *readobj, zval *writeobj, int type)
{
	php_com_dotnet_object *obj;
	VARIANT v;
	VARTYPE vt = VT_EMPTY;
	HRESULT res = S_OK;

	obj = CDNO_FETCH(readobj);
	ZVAL_NULL(writeobj);
	VariantInit(&v);

	if (V_VT(&obj->v) == VT_DISPATCH) {
		if (SUCCESS != php_com_do_invoke_by_id(obj, DISPID_VALUE,
				DISPATCH_METHOD|DISPATCH_PROPERTYGET, &v, 0, NULL, 1, 0)) {
			VariantCopy(&v, &obj->v);
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
		case IS_FALSE:
		case IS_TRUE:
		case _IS_BOOL:
			vt = VT_BOOL;
			break;
		case IS_STRING:
			vt = VT_BSTR;
			break;
		default:
			;
	}

	if (vt != VT_EMPTY && vt != V_VT(&v)) {
		res = VariantChangeType(&v, &v, 0, vt);
	}

	if (SUCCEEDED(res)) {
		php_com_zval_from_variant(writeobj, &v, obj->code_page);
	}

	VariantClear(&v);

	if (SUCCEEDED(res)) {
		return SUCCESS;
	}

	return zend_std_cast_object_tostring(readobj, writeobj, type);
}

static int com_object_count(zval *object, zend_long *count)
{
	php_com_dotnet_object *obj;
	LONG ubound = 0, lbound = 0;

	obj = CDNO_FETCH(object);

	if (!V_ISARRAY(&obj->v)) {
		return FAILURE;
	}

	SafeArrayGetLBound(V_ARRAY(&obj->v), 1, &lbound);
	SafeArrayGetUBound(V_ARRAY(&obj->v), 1, &ubound);

	*count = ubound - lbound + 1;

	return SUCCESS;
}

zend_object_handlers php_com_object_handlers = {
	0,
	php_com_object_free_storage,
	zend_objects_destroy_object,
	php_com_object_clone,
	com_property_read,
	com_property_write,
	com_read_dimension,
	com_write_dimension,
	NULL,
	NULL, /* com_object_get, */
	NULL, /* com_object_set, */
	com_property_exists,
	com_property_delete,
	com_dimension_exists,
	com_dimension_delete,
	com_properties_get,
	com_method_get,
	com_call_method,
	com_constructor_get,
	com_class_name_get,
	com_objects_compare,
	com_object_cast,
	com_object_count,
	NULL,									/* get_debug_info */
	NULL,									/* get_closure */
	NULL,									/* get_gc */
};

void php_com_object_enable_event_sink(php_com_dotnet_object *obj, int enable)
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

void php_com_object_free_storage(zend_object *object)
{
	php_com_dotnet_object *obj = (php_com_dotnet_object*)object;

	if (obj->typeinfo) {
		ITypeInfo_Release(obj->typeinfo);
		obj->typeinfo = NULL;
	}

	if (obj->sink_dispatch) {
		php_com_object_enable_event_sink(obj, FALSE);
		IDispatch_Release(obj->sink_dispatch);
		obj->sink_dispatch = NULL;
	}

	VariantClear(&obj->v);

	if (obj->method_cache) {
		zend_hash_destroy(obj->method_cache);
		FREE_HASHTABLE(obj->method_cache);
	}
	if (obj->id_of_name_cache) {
		zend_hash_destroy(obj->id_of_name_cache);
		FREE_HASHTABLE(obj->id_of_name_cache);
	}
}

zend_object* php_com_object_clone(zval *object)
{
	php_com_dotnet_object *cloneobj, *origobject;

	origobject = (php_com_dotnet_object*)Z_OBJ_P(object);
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

	return (zend_object*)cloneobj;
}

zend_object* php_com_object_new(zend_class_entry *ce)
{
	php_com_dotnet_object *obj;

	php_com_initialize();
	obj = emalloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));

	VariantInit(&obj->v);
	obj->code_page = CP_ACP;
	obj->ce = ce;

	zend_object_std_init(&obj->zo, ce);
	obj->zo.handlers = &php_com_object_handlers;

	obj->typeinfo = NULL;

	return (zend_object*)obj;
}
