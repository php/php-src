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

static zval *com_property_read(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv)
{
	php_com_dotnet_object *obj;
	VARIANT v;
	HRESULT res;

	ZVAL_NULL(rv);

	obj = (php_com_dotnet_object*) object;

	if (V_VT(&obj->v) == VT_DISPATCH) {
		VariantInit(&v);

		res = php_com_do_invoke(obj, ZSTR_VAL(member), ZSTR_LEN(member),
				DISPATCH_METHOD|DISPATCH_PROPERTYGET, &v, 0, NULL, 1);

		if (res == SUCCESS) {
			php_com_zval_from_variant(rv, &v, obj->code_page);
			VariantClear(&v);
		} else if (res == DISP_E_BADPARAMCOUNT) {
			zval zv;

			ZVAL_STR(&zv, member);
			php_com_saproxy_create(object, rv, &zv);
		}
	} else {
		php_com_throw_exception(E_INVALIDARG, "this variant has no properties");
	}

	return rv;
}

static zval *com_property_write(zend_object *object, zend_string *member, zval *value, void **cache_slot)
{
	php_com_dotnet_object *obj;
	VARIANT v;

	obj = (php_com_dotnet_object*) object;

	if (V_VT(&obj->v) == VT_DISPATCH) {
		VariantInit(&v);

		if (SUCCESS == php_com_do_invoke(obj, ZSTR_VAL(member), ZSTR_LEN(member),
				DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF, &v, 1, value, 0)) {
			VariantClear(&v);
		}
	} else {
		php_com_throw_exception(E_INVALIDARG, "this variant has no properties");
	}
	return value;
}

static zval *com_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
	php_com_dotnet_object *obj;
	VARIANT v;

	ZVAL_NULL(rv);

	obj = (php_com_dotnet_object*) object;

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

static void com_write_dimension(zend_object *object, zval *offset, zval *value)
{
	php_com_dotnet_object *obj;
	zval args[2];
	VARIANT v;
	HRESULT res;

	obj = (php_com_dotnet_object*) object;

	if (offset == NULL) {
		php_com_throw_exception(DISP_E_BADINDEX, "appending to variants is not supported");
		return;
	}

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

static zval *com_get_property_ptr_ptr(zend_object *object, zend_string *member, int type, void **cache_slot)
{
	return NULL;
}

static int com_property_exists(zend_object *object, zend_string *member, int check_empty, void **cache_slot)
{
	DISPID dispid;
	php_com_dotnet_object *obj;

	obj = (php_com_dotnet_object*) object;

	if (V_VT(&obj->v) == VT_DISPATCH) {
		if (SUCCEEDED(php_com_get_id_of_name(obj, ZSTR_VAL(member), ZSTR_LEN(member), &dispid))) {
			/* TODO: distinguish between property and method! */
			return 1;
		}
	} else {
		/* TODO: check for safearray */
	}

	return 0;
}

static int com_dimension_exists(zend_object *object, zval *member, int check_empty)
{
	/* TODO Add support */
	zend_throw_error(NULL, "Cannot check dimension on a COM object");
	return 0;
}

static void com_property_delete(zend_object *object, zend_string *member, void **cache_slot)
{
	zend_throw_error(NULL, "Cannot delete properties from a COM object");
}

static void com_dimension_delete(zend_object *object, zval *offset)
{
	zend_throw_error(NULL, "Cannot delete dimension from a COM object");
}

static HashTable *com_properties_get(zend_object *object)
{
	/* TODO: use type-info to get all the names and values ?
	 * DANGER: if we do that, there is a strong possibility for
	 * infinite recursion when the hash is displayed via var_dump().
	 * Perhaps it is best to leave it un-implemented.
	 */
	return (HashTable *) &zend_empty_array;
}

static HashTable *com_get_gc(zend_object *object, zval **table, int *n)
{
	*table = NULL;
	*n = 0;
	return NULL;
}

static void function_dtor(zval *zv)
{
	zend_internal_function *f = (zend_internal_function*)Z_PTR_P(zv);

	zend_string_release_ex(f->function_name, 0);
	if (f->arg_info) {
		efree(f->arg_info);
	}
	efree(f);
}

static PHP_FUNCTION(com_method_handler)
{
	zval *object = getThis();
	zend_string *method = EX(func)->common.function_name;
	zval *args = NULL;
	php_com_dotnet_object *obj = CDNO_FETCH(object);
	int nargs;
	VARIANT v;
	int ret = FAILURE;

	if (V_VT(&obj->v) != VT_DISPATCH) {
		goto exit;
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

exit:
	/* Cleanup trampoline */
	ZEND_ASSERT(EX(func)->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE);
	zend_string_release(EX(func)->common.function_name);
	zend_free_trampoline(EX(func));
	EX(func) = NULL;
}

static zend_function *com_method_get(zend_object **object_ptr, zend_string *name, const zval *key)
{
	zend_internal_function f, *fptr = NULL;
	zend_function *func;
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
		memset(&f, 0, sizeof(zend_internal_function));
		f.type = ZEND_INTERNAL_FUNCTION;
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
								zend_bool by_ref = (bindptr.lpfuncdesc->lprgelemdescParam[i].paramdesc.wParamFlags & PARAMFLAG_FOUT) != 0;
								f.arg_info[i].type = (zend_type) ZEND_TYPE_INIT_NONE(_ZEND_ARG_INFO_FLAGS(by_ref, 0));
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
		zend_string_addref(fptr->function_name);
		func = emalloc(sizeof(*fptr));
		memcpy(func, fptr, sizeof(*fptr));

		return func;
	}

	return NULL;
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

	ZEND_COMPARE_OBJECTS_FALLBACK(object1, object2);

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

static int com_object_cast(zend_object *readobj, zval *writeobj, int type)
{
	php_com_dotnet_object *obj;
	VARIANT v;
	VARTYPE vt = VT_EMPTY;
	HRESULT res = S_OK;

	obj = (php_com_dotnet_object*) readobj;
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
		case _IS_NUMBER:
#if SIZEOF_ZEND_LONG == 4
			vt = VT_I4;
#else
			vt = VT_I8;
#endif
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

static int com_object_count(zend_object *object, zend_long *count)
{
	php_com_dotnet_object *obj;
	LONG ubound = 0, lbound = 0;

	obj = (php_com_dotnet_object*) object;

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
	com_get_property_ptr_ptr,
	com_property_exists,
	com_property_delete,
	com_dimension_exists,
	com_dimension_delete,
	com_properties_get,
	com_method_get,
	zend_std_get_constructor,
	com_class_name_get,
	com_object_cast,
	com_object_count,
	NULL,									/* get_debug_info */
	NULL,									/* get_closure */
	com_get_gc,								/* get_gc */
	NULL,									/* do_operation */
	com_objects_compare,					/* compare */
	NULL,									/* get_properties_for */
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

	zend_object_std_dtor(object);
}

zend_object* php_com_object_clone(zend_object *object)
{
	php_com_dotnet_object *cloneobj, *origobject;

	origobject = (php_com_dotnet_object*) object;
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
