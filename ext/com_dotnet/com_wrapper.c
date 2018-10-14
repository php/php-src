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

/* This module exports a PHP object as a COM object by wrapping it
 * using IDispatchEx */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"

typedef struct {
	/* This first part MUST match the declaration
	 * of interface IDispatchEx */
	CONST_VTBL struct IDispatchExVtbl *lpVtbl;

	/* now the PHP stuff */

	DWORD engine_thread; /* for sanity checking */
	zval object;			/* the object exported */
	LONG refcount;			/* COM reference count */

	HashTable *dispid_to_name;	/* keep track of dispid -> name mappings */
	HashTable *name_to_dispid;	/* keep track of name -> dispid mappings */

	GUID sinkid;	/* iid that we "implement" for event sinking */

	zend_resource *res;
} php_dispatchex;

static int le_dispatch;

static void disp_destructor(php_dispatchex *disp);

static void dispatch_dtor(zend_resource *rsrc)
{
	php_dispatchex *disp = (php_dispatchex *)rsrc->ptr;
	disp_destructor(disp);
}

int php_com_wrapper_minit(INIT_FUNC_ARGS)
{
	le_dispatch = zend_register_list_destructors_ex(dispatch_dtor,
		NULL, "com_dotnet_dispatch_wrapper", module_number);
	return le_dispatch;
}


/* {{{ trace */
static inline void trace(char *fmt, ...)
{
	va_list ap;
	char buf[4096];

	snprintf(buf, sizeof(buf), "T=%08x ", GetCurrentThreadId());
	OutputDebugString(buf);

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);

	OutputDebugString(buf);

	va_end(ap);
}
/* }}} */

#define FETCH_DISP(methname)																			\
	php_dispatchex *disp = (php_dispatchex*)This; 														\
	if (COMG(rshutdown_started)) {																		\
		trace(" PHP Object:%p (name:unknown) %s\n", Z_OBJ(disp->object),  methname); 							\
	} else {																							\
		trace(" PHP Object:%p (name:%s) %s\n", Z_OBJ(disp->object), Z_OBJCE(disp->object)->name->val, methname); 	\
	}																									\
	if (GetCurrentThreadId() != disp->engine_thread) {													\
		return RPC_E_WRONG_THREAD;																		\
	}

static HRESULT STDMETHODCALLTYPE disp_queryinterface(
	IDispatchEx *This,
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void **ppvObject)
{
	FETCH_DISP("QueryInterface");

	if (IsEqualGUID(&IID_IUnknown, riid) ||
			IsEqualGUID(&IID_IDispatch, riid) ||
			IsEqualGUID(&IID_IDispatchEx, riid) ||
			IsEqualGUID(&disp->sinkid, riid)) {
		*ppvObject = This;
		InterlockedIncrement(&disp->refcount);
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE disp_addref(IDispatchEx *This)
{
	FETCH_DISP("AddRef");

	return InterlockedIncrement(&disp->refcount);
}

static ULONG STDMETHODCALLTYPE disp_release(IDispatchEx *This)
{
	ULONG ret;
	FETCH_DISP("Release");

	ret = InterlockedDecrement(&disp->refcount);
	trace("-- refcount now %d\n", ret);
	if (ret == 0) {
		/* destroy it */
		if (disp->res)
			zend_list_delete(disp->res);
	}
	return ret;
}

static HRESULT STDMETHODCALLTYPE disp_gettypeinfocount(
	IDispatchEx *This,
	/* [out] */ UINT *pctinfo)
{
	FETCH_DISP("GetTypeInfoCount");

	*pctinfo = 0;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE disp_gettypeinfo(
	IDispatchEx *This,
	/* [in] */ UINT iTInfo,
	/* [in] */ LCID lcid,
	/* [out] */ ITypeInfo **ppTInfo)
{
	FETCH_DISP("GetTypeInfo");

	*ppTInfo = NULL;
	return DISP_E_BADINDEX;
}

static HRESULT STDMETHODCALLTYPE disp_getidsofnames(
	IDispatchEx *This,
	/* [in] */ REFIID riid,
	/* [size_is][in] */ LPOLESTR *rgszNames,
	/* [in] */ UINT cNames,
	/* [in] */ LCID lcid,
	/* [size_is][out] */ DISPID *rgDispId)
{
	UINT i;
	HRESULT ret = S_OK;
	FETCH_DISP("GetIDsOfNames");

	for (i = 0; i < cNames; i++) {
		char *name;
		size_t namelen;
		zval *tmp;

		name = php_com_olestring_to_string(rgszNames[i], &namelen, COMG(code_page));

		/* Lookup the name in the hash */
		if ((tmp = zend_hash_str_find(disp->name_to_dispid, name, namelen)) == NULL) {
			ret = DISP_E_UNKNOWNNAME;
			rgDispId[i] = 0;
		} else {
			rgDispId[i] = (DISPID)Z_LVAL_P(tmp);
		}

		efree(name);

	}

	return ret;
}

static HRESULT STDMETHODCALLTYPE disp_invoke(
	IDispatchEx *This,
	/* [in] */ DISPID dispIdMember,
	/* [in] */ REFIID riid,
	/* [in] */ LCID lcid,
	/* [in] */ WORD wFlags,
	/* [out][in] */ DISPPARAMS *pDispParams,
	/* [out] */ VARIANT *pVarResult,
	/* [out] */ EXCEPINFO *pExcepInfo,
	/* [out] */ UINT *puArgErr)
{
	return This->lpVtbl->InvokeEx(This, dispIdMember,
			lcid, wFlags, pDispParams,
			pVarResult, pExcepInfo, NULL);
}

static HRESULT STDMETHODCALLTYPE disp_getdispid(
	IDispatchEx *This,
	/* [in] */ BSTR bstrName,
	/* [in] */ DWORD grfdex,
	/* [out] */ DISPID *pid)
{
	HRESULT ret = DISP_E_UNKNOWNNAME;
	char *name;
	size_t namelen;
	zval *tmp;
	FETCH_DISP("GetDispID");

	name = php_com_olestring_to_string(bstrName, &namelen, COMG(code_page));

	trace("Looking for %s, namelen=%d in %p\n", name, namelen, disp->name_to_dispid);

	/* Lookup the name in the hash */
	if ((tmp = zend_hash_str_find(disp->name_to_dispid, name, namelen)) != NULL) {
		trace("found it\n");
		*pid = (DISPID)Z_LVAL_P(tmp);
		ret = S_OK;
	}

	efree(name);

	return ret;
}

static HRESULT STDMETHODCALLTYPE disp_invokeex(
	IDispatchEx *This,
	/* [in] */ DISPID id,
	/* [in] */ LCID lcid,
	/* [in] */ WORD wFlags,
	/* [in] */ DISPPARAMS *pdp,
	/* [out] */ VARIANT *pvarRes,
	/* [out] */ EXCEPINFO *pei,
	/* [unique][in] */ IServiceProvider *pspCaller)
{
	zval *name;
	UINT i;
	zval rv, *retval = NULL;
	zval *params = NULL;
	HRESULT ret = DISP_E_MEMBERNOTFOUND;
	FETCH_DISP("InvokeEx");

	if (NULL != (name = zend_hash_index_find(disp->dispid_to_name, id))) {
		/* TODO: add support for overloaded objects */

		trace("-- Invoke: %d %20s [%d] flags=%08x args=%d\n", id, Z_STRVAL_P(name), Z_STRLEN_P(name), wFlags, pdp->cArgs);

		/* convert args into zvals.
		 * Args are in reverse order */
		if (pdp->cArgs) {
			params = (zval *)safe_emalloc(sizeof(zval), pdp->cArgs, 0);
			for (i = 0; i < pdp->cArgs; i++) {
				VARIANT *arg;

				arg = &pdp->rgvarg[ pdp->cArgs - 1 - i];

				trace("alloc zval for arg %d VT=%08x\n", i, V_VT(arg));

				php_com_wrap_variant(&params[i], arg, COMG(code_page));
			}
		}

		trace("arguments processed, prepare to do some work\n");

		/* TODO: if PHP raises an exception here, we should catch it
		 * and expose it as a COM exception */

		if (wFlags & DISPATCH_PROPERTYGET) {
			retval = zend_read_property(Z_OBJCE(disp->object), &disp->object, Z_STRVAL_P(name), Z_STRLEN_P(name)+1, 1, &rv);
		} else if (wFlags & DISPATCH_PROPERTYPUT) {
			zend_update_property(Z_OBJCE(disp->object), &disp->object, Z_STRVAL_P(name), Z_STRLEN_P(name), &params[0]);
		} else if (wFlags & DISPATCH_METHOD) {
			zend_try {
				retval = &rv;
				if (SUCCESS == call_user_function(EG(function_table), &disp->object, name,
							retval, pdp->cArgs, params)) {
					ret = S_OK;
					trace("function called ok\n");

					/* Copy any modified values to callers copy of variant*/
					for (i = 0; i < pdp->cArgs; i++) {
						php_com_dotnet_object *obj = CDNO_FETCH(&params[i]);
						VARIANT *srcvar = &obj->v;
						VARIANT *dstvar = &pdp->rgvarg[ pdp->cArgs - 1 - i];
						if ((V_VT(dstvar) & VT_BYREF) && obj->modified ) {
							trace("percolate modified value for arg %d VT=%08x\n", i, V_VT(dstvar));
							php_com_copy_variant(dstvar, srcvar);
						}
					}
				} else {
					trace("failed to call func\n");
					ret = DISP_E_EXCEPTION;
				}
			} zend_catch {
				trace("something blew up\n");
				ret = DISP_E_EXCEPTION;
			} zend_end_try();
		} else {
			trace("Don't know how to handle this invocation %08x\n", wFlags);
		}

		/* release arguments */
		if (params) {
			for (i = 0; i < pdp->cArgs; i++) {
				zval_ptr_dtor(&params[i]);
			}
			efree(params);
		}

		/* return value */
		if (retval) {
			if (pvarRes) {
				VariantInit(pvarRes);
				php_com_variant_from_zval(pvarRes, retval, COMG(code_page));
			}
			zval_ptr_dtor(retval);
		} else if (pvarRes) {
			VariantInit(pvarRes);
		}

	} else {
		trace("InvokeEx: I don't support DISPID=%d\n", id);
	}

	return ret;
}

static HRESULT STDMETHODCALLTYPE disp_deletememberbyname(
	IDispatchEx *This,
	/* [in] */ BSTR bstrName,
	/* [in] */ DWORD grfdex)
{
	FETCH_DISP("DeleteMemberByName");

	/* TODO: unset */

	return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE disp_deletememberbydispid(
	IDispatchEx *This,
	/* [in] */ DISPID id)
{
	FETCH_DISP("DeleteMemberByDispID");

	/* TODO: unset */

	return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE disp_getmemberproperties(
	IDispatchEx *This,
	/* [in] */ DISPID id,
	/* [in] */ DWORD grfdexFetch,
	/* [out] */ DWORD *pgrfdex)
{
	FETCH_DISP("GetMemberProperties");

	return DISP_E_UNKNOWNNAME;
}

static HRESULT STDMETHODCALLTYPE disp_getmembername(
	IDispatchEx *This,
	/* [in] */ DISPID id,
	/* [out] */ BSTR *pbstrName)
{
	zval *name;
	FETCH_DISP("GetMemberName");

	if (NULL != (name = zend_hash_index_find(disp->dispid_to_name, id))) {
		OLECHAR *olestr = php_com_string_to_olestring(Z_STRVAL_P(name), Z_STRLEN_P(name), COMG(code_page));
		*pbstrName = SysAllocString(olestr);
		efree(olestr);
		return S_OK;
	} else {
		return DISP_E_UNKNOWNNAME;
	}
}

static HRESULT STDMETHODCALLTYPE disp_getnextdispid(
	IDispatchEx *This,
	/* [in] */ DWORD grfdex,
	/* [in] */ DISPID id,
	/* [out] */ DISPID *pid)
{
	ulong next = id+1;
	FETCH_DISP("GetNextDispID");

	while(!zend_hash_index_exists(disp->dispid_to_name, next))
		next++;

	if (zend_hash_index_exists(disp->dispid_to_name, next)) {
		*pid = next;
		return S_OK;
	}
	return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE disp_getnamespaceparent(
	IDispatchEx *This,
	/* [out] */ IUnknown **ppunk)
{
	FETCH_DISP("GetNameSpaceParent");

	*ppunk = NULL;
	return E_NOTIMPL;
}

static struct IDispatchExVtbl php_dispatch_vtbl = {
	disp_queryinterface,
	disp_addref,
	disp_release,
	disp_gettypeinfocount,
	disp_gettypeinfo,
	disp_getidsofnames,
	disp_invoke,
	disp_getdispid,
	disp_invokeex,
	disp_deletememberbyname,
	disp_deletememberbydispid,
	disp_getmemberproperties,
	disp_getmembername,
	disp_getnextdispid,
	disp_getnamespaceparent
};


/* enumerate functions and properties of the object and assign
 * dispatch ids */
static void generate_dispids(php_dispatchex *disp)
{
	HashPosition pos;
	zend_string *name = NULL;
	zval *tmp, tmp2;
	int keytype;
	zend_ulong pid;

	if (disp->dispid_to_name == NULL) {
		ALLOC_HASHTABLE(disp->dispid_to_name);
		ALLOC_HASHTABLE(disp->name_to_dispid);
		zend_hash_init(disp->name_to_dispid, 0, NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_init(disp->dispid_to_name, 0, NULL, ZVAL_PTR_DTOR, 0);
	}

	/* properties */
	if (Z_OBJPROP(disp->object)) {
		zend_hash_internal_pointer_reset_ex(Z_OBJPROP(disp->object), &pos);
		while (HASH_KEY_NON_EXISTENT != (keytype =
				zend_hash_get_current_key_ex(Z_OBJPROP(disp->object), &name,
				&pid, &pos))) {
			char namebuf[32];
			if (keytype == HASH_KEY_IS_LONG) {
				snprintf(namebuf, sizeof(namebuf), ZEND_ULONG_FMT, pid);
				name = zend_string_init(namebuf, strlen(namebuf), 0);
			} else {
				zend_string_addref(name);
			}

			zend_hash_move_forward_ex(Z_OBJPROP(disp->object), &pos);

			/* Find the existing id */
			if ((tmp = zend_hash_find(disp->name_to_dispid, name)) != NULL) {
				zend_string_release_ex(name, 0);
				continue;
			}

			/* add the mappings */
			ZVAL_STR_COPY(&tmp2, name);
			pid = zend_hash_next_free_element(disp->dispid_to_name);
			zend_hash_index_update(disp->dispid_to_name, pid, &tmp2);

			ZVAL_LONG(&tmp2, pid);
			zend_hash_update(disp->name_to_dispid, name, &tmp2);

			zend_string_release_ex(name, 0);
		}
	}

	/* functions */
	if (Z_OBJCE(disp->object)) {
		zend_hash_internal_pointer_reset_ex(&Z_OBJCE(disp->object)->function_table, &pos);
		while (HASH_KEY_NON_EXISTENT != (keytype =
				zend_hash_get_current_key_ex(&Z_OBJCE(disp->object)->function_table,
			 	&name, &pid, &pos))) {

			char namebuf[32];
			if (keytype == HASH_KEY_IS_LONG) {
				snprintf(namebuf, sizeof(namebuf), "%d", pid);
				name = zend_string_init(namebuf, strlen(namebuf), 0);
			} else {
				zend_string_addref(name);
			}

			zend_hash_move_forward_ex(&Z_OBJCE(disp->object)->function_table, &pos);

			/* Find the existing id */
			if ((tmp = zend_hash_find(disp->name_to_dispid, name)) != NULL) {
				zend_string_release_ex(name, 0);
				continue;
			}

			/* add the mappings */
			ZVAL_STR_COPY(&tmp2, name);
			pid = zend_hash_next_free_element(disp->dispid_to_name);
			zend_hash_index_update(disp->dispid_to_name, pid, &tmp2);

			ZVAL_LONG(&tmp2, pid);
			zend_hash_update(disp->name_to_dispid, name, &tmp2);

			zend_string_release_ex(name, 0);
		}
	}
}

static php_dispatchex *disp_constructor(zval *object)
{
	php_dispatchex *disp = (php_dispatchex*)CoTaskMemAlloc(sizeof(php_dispatchex));
	zval *tmp;

	trace("constructing a COM wrapper for PHP object %p (%s)\n", object, Z_OBJCE_P(object)->name);

	if (disp == NULL)
		return NULL;

	memset(disp, 0, sizeof(php_dispatchex));

	disp->engine_thread = GetCurrentThreadId();
	disp->lpVtbl = &php_dispatch_vtbl;
	disp->refcount = 1;


	if (object) {
		ZVAL_COPY(&disp->object, object);
	} else {
		ZVAL_UNDEF(&disp->object);
	}

	tmp = zend_list_insert(disp, le_dispatch);
	disp->res = Z_RES_P(tmp);

	return disp;
}

static void disp_destructor(php_dispatchex *disp)
{
	/* Object store not available during request shutdown */
	if (COMG(rshutdown_started)) {
		trace("destroying COM wrapper for PHP object %p (name:unknown)\n", Z_OBJ(disp->object));
	} else {
		trace("destroying COM wrapper for PHP object %p (name:%s)\n", Z_OBJ(disp->object), Z_OBJCE(disp->object)->name->val);
	}

	disp->res = NULL;

	if (disp->refcount > 0)
		CoDisconnectObject((IUnknown*)disp, 0);

	zend_hash_destroy(disp->dispid_to_name);
	zend_hash_destroy(disp->name_to_dispid);
	FREE_HASHTABLE(disp->dispid_to_name);
	FREE_HASHTABLE(disp->name_to_dispid);

	zval_ptr_dtor(&disp->object);

	CoTaskMemFree(disp);
}

PHP_COM_DOTNET_API IDispatch *php_com_wrapper_export_as_sink(zval *val, GUID *sinkid,
	   HashTable *id_to_name)
{
	php_dispatchex *disp = disp_constructor(val);
	HashPosition pos;
	zend_string *name = NULL;
	zval tmp, *ntmp;
	int keytype;
	zend_ulong pid;

	disp->dispid_to_name = id_to_name;

	memcpy(&disp->sinkid, sinkid, sizeof(disp->sinkid));

	/* build up the reverse mapping */
	ALLOC_HASHTABLE(disp->name_to_dispid);
	zend_hash_init(disp->name_to_dispid, 0, NULL, ZVAL_PTR_DTOR, 0);

	zend_hash_internal_pointer_reset_ex(id_to_name, &pos);
	while (HASH_KEY_NON_EXISTENT != (keytype =
				zend_hash_get_current_key_ex(id_to_name, &name, &pid, &pos))) {

		if (keytype == HASH_KEY_IS_LONG) {

			ntmp = zend_hash_get_current_data_ex(id_to_name, &pos);

			ZVAL_LONG(&tmp, pid);
			zend_hash_update(disp->name_to_dispid, Z_STR_P(ntmp), &tmp);
		}

		zend_hash_move_forward_ex(id_to_name, &pos);
	}

	return (IDispatch*)disp;
}

PHP_COM_DOTNET_API IDispatch *php_com_wrapper_export(zval *val)
{
	php_dispatchex *disp = NULL;

	if (Z_TYPE_P(val) != IS_OBJECT) {
		return NULL;
	}

	if (php_com_is_valid_object(val)) {
		/* pass back its IDispatch directly */
		php_com_dotnet_object *obj = CDNO_FETCH(val);

		if (obj == NULL)
			return NULL;

		if (V_VT(&obj->v) == VT_DISPATCH && V_DISPATCH(&obj->v)) {
			IDispatch_AddRef(V_DISPATCH(&obj->v));
			return V_DISPATCH(&obj->v);
		}

		return NULL;
	}

	disp = disp_constructor(val);
	generate_dispids(disp);

	return (IDispatch*)disp;
}
