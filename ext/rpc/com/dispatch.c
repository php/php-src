/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* 
 * This module is used to export PHP objects to COM and DOTNET by exposing
 * them as objects implementing IDispatch.
 * */

#include "php.h"
#include "php_COM.h"
#include "php_VARIANT.h"
#include "conversion.h"
#include "variant.h"

#define COBJMACROS
#include <unknwn.h> /* IDispatch */
#include <dispex.h> /* IDispatchEx */


typedef struct {
	/* This first part MUST match the declaration
	 * of interface IDispatchEx */
	CONST_VTBL struct IDispatchExVtbl *lpVtbl;

	/* now the PHP stuff */
	
	THREAD_T engine_thread; /* for sanity checking */
	zval *object;			/* the object exported */
	LONG refcount;			/* COM reference count */

	HashTable dispid_to_name;	/* keep track of dispid -> name mappings */
	HashTable name_to_dispid;	/* keep track of name -> dispid mappings */

	int id;
} php_dispatchex;

static void disp_destructor(php_dispatchex *disp);

static void dispatch_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_dispatchex *disp = (php_dispatchex *)rsrc->ptr;
	disp_destructor(disp);
}

static int le_dispatch;
int php_COM_dispatch_init(int module_number TSRMLS_DC)
{
	le_dispatch = zend_register_list_destructors_ex(dispatch_dtor, NULL, "COM:Dispatch", module_number);
	return le_dispatch;
}


/* {{{ trace */
static inline void trace(char *fmt, ...)
{
	va_list ap;
	char buf[4096];

	sprintf(buf, "T=%08x ", tsrm_thread_id());
	OutputDebugString(buf);
	
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);

	OutputDebugString(buf);
	
	va_end(ap);
}
/* }}} */

#define FETCH_DISP(methname)	\
	php_dispatchex *disp = (php_dispatchex*)This; \
	trace(" PHP:%s %s\n", Z_OBJCE_P(disp->object)->name, methname); \
	if (tsrm_thread_id() != disp->engine_thread) \
		return E_UNEXPECTED;


static HRESULT STDMETHODCALLTYPE disp_queryinterface( 
	IDispatchEx *This,
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void **ppvObject)
{
	FETCH_DISP("QueryInterface");

	if (IsEqualGUID(&IID_IUnknown, riid) ||
			IsEqualGUID(&IID_IDispatch, riid) ||
			IsEqualGUID(&IID_IDispatchEx, riid)) {
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
	TSRMLS_FETCH();
	FETCH_DISP("Release");

	ret = InterlockedDecrement(&disp->refcount);
	trace("-- refcount now %d\n", ret);
	if (ret == 0) {
		/* destroy it */
		if (disp->id)
			zend_list_delete(disp->id);
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
	TSRMLS_FETCH();
	FETCH_DISP("GetIDsOfNames");

	for (i = 0; i < cNames; i++) {
		char *name;
		unsigned int namelen;
		zval *tmp;
		
		name = php_OLECHAR_to_char(rgszNames[i], &namelen, CP_ACP TSRMLS_CC);
		
		/* Lookup the name in the hash */
		if (zend_hash_find(&disp->name_to_dispid, name, namelen+1, (void**)&tmp) == FAILURE) {
			ret = DISP_E_UNKNOWNNAME;
			rgDispId[i] = 0;
		} else {
			rgDispId[i] = Z_LVAL_P(tmp);
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
	unsigned int namelen;
	zval *tmp;
	TSRMLS_FETCH();
	FETCH_DISP("GetDispID");

	name = php_OLECHAR_to_char(bstrName, &namelen, CP_ACP TSRMLS_CC);

	/* Lookup the name in the hash */
	if (zend_hash_find(&disp->name_to_dispid, name, namelen+1, (void**)&tmp) == SUCCESS) {
		*pid = Z_LVAL_P(tmp);
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
	int codepage = CP_ACP;
	zval **retval = NULL;
	zval ***params = NULL;
	TSRMLS_FETCH();
	FETCH_DISP("InvokeEx");

	if (SUCCESS == zend_hash_index_find(&disp->dispid_to_name, id, (void**)&name)) {
		/* TODO: add support for overloaded objects */

		trace("-- Invoke: %d %s\n", id, Z_STRVAL_P(name));
		
		/* convert args into zvals.
		 * Args are in reverse order */
		params = (zval ***)emalloc(sizeof(zval **) * pdp->cArgs);
		for (i = 0; i < pdp->cArgs; i++) {
			ALLOC_INIT_ZVAL(*params[i]);

			if (V_VT(&pdp->rgvarg[pdp->cArgs-i]) == VT_DISPATCH) {
				php_COM_object_from_dispatch(V_DISPATCH(&pdp->rgvarg[pdp->cArgs-i]), *params[i] TSRMLS_CC);
			} else {
				ZVAL_VARIANT(*params[i], &pdp->rgvarg[pdp->cArgs-i]);
			}
		}
		
		if (wFlags & DISPATCH_PROPERTYGET) {
			zend_hash_find(Z_OBJPROP_P(disp->object), Z_STRVAL_P(name), Z_STRLEN_P(name)+1, (void**)&retval);
		} else if (wFlags & DISPATCH_PROPERTYPUT) {
			add_property_zval(disp->object, Z_STRVAL_P(name), *params[0]);
		} else if (wFlags & DISPATCH_METHOD) {
			
			call_user_function_ex(EG(function_table), &disp->object, name,
					retval, pdp->cArgs, params, 1, NULL TSRMLS_CC);
		}
	
		/* release arguments */
		for (i = 0; i < pdp->cArgs; i++)
			zval_ptr_dtor(params[i]);
		
		/* return value */
		if (retval) {
			if (pvarRes) {
				if (Z_TYPE_PP(retval) == IS_OBJECT) {
					/* export the object using a dispatch like ourselves */
					VariantInit(pvarRes);
					V_VT(pvarRes) = VT_DISPATCH;
					V_DISPATCH(pvarRes) = php_COM_export_object(*retval TSRMLS_CC);
				} else {
					php_pval_to_variant(*retval, pvarRes, codepage TSRMLS_CC);
				}
			}
			zval_ptr_dtor(retval);
		} else if (pvarRes) {
			VariantInit(pvarRes);
		}
		
		return S_OK;
	} else {
		trace("InvokeEx: I don't support DISPID=%d\n", id);
	}

	return DISP_E_MEMBERNOTFOUND;
}

static HRESULT STDMETHODCALLTYPE disp_deletememberbyname( 
	IDispatchEx *This,
	/* [in] */ BSTR bstrName,
	/* [in] */ DWORD grfdex)
{
	FETCH_DISP("DeleteMemberByName");

	return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE disp_deletememberbydispid( 
	IDispatchEx *This,
	/* [in] */ DISPID id)
{
	FETCH_DISP("DeleteMemberByDispID");
	
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
	TSRMLS_FETCH();
	FETCH_DISP("GetMemberName");

	if (SUCCESS == zend_hash_index_find(&disp->dispid_to_name, id, (void**)&name)) {
		OLECHAR *olestr = php_char_to_OLECHAR(Z_STRVAL_P(name), Z_STRLEN_P(name), CP_ACP TSRMLS_CC);
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

	while(!zend_hash_index_exists(&disp->dispid_to_name, next))
		next++;

	if (zend_hash_index_exists(&disp->dispid_to_name, next)) {
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
static void update_dispids(php_dispatchex *disp TSRMLS_DC)
{
	HashPosition pos;
	char *name = NULL;
	zval *tmp;
	int namelen;
	int keytype;
	ulong pid;

	/* properties */
	zend_hash_internal_pointer_reset_ex(Z_OBJPROP_PP(&disp->object), &pos);
	while (HASH_KEY_NON_EXISTANT != (keytype =
				zend_hash_get_current_key_ex(Z_OBJPROP_PP(&disp->object), &name, &namelen, &pid, 0, &pos))) {
		char namebuf[32];
		if (keytype == HASH_KEY_IS_LONG) {
			sprintf(namebuf, "%d", pid);
			name = namebuf;
			namelen = strlen(namebuf);
		}

		zend_hash_move_forward_ex(Z_OBJPROP_PP(&disp->object), &pos);
		
		/* Find the existing id */
		if (zend_hash_find(&disp->name_to_dispid, name, namelen+1, (void**)&tmp) == SUCCESS)
			continue;
		
		/* add the mappings */
		MAKE_STD_ZVAL(tmp);
		ZVAL_STRINGL(tmp, name, namelen, 1);
		zend_hash_index_update(&disp->dispid_to_name, pid, (void*)&tmp, sizeof(zval *), NULL);

		MAKE_STD_ZVAL(tmp);
		ZVAL_LONG(tmp, pid);
		zend_hash_update(&disp->name_to_dispid, name, namelen+1, (void*)&tmp, sizeof(zval *), NULL);

	}
	
	/* functions */
	zend_hash_internal_pointer_reset_ex(&Z_OBJCE_PP(&disp->object)->function_table, &pos);
	while (HASH_KEY_NON_EXISTANT != (keytype =
				zend_hash_get_current_key_ex(&Z_OBJCE_PP(&disp->object)->function_table, &name, &namelen, &pid, 0, &pos))) {

		char namebuf[32];
		if (keytype == HASH_KEY_IS_LONG) {
			sprintf(namebuf, "%d", pid);
			name = namebuf;
			namelen = strlen(namebuf);
		}

		zend_hash_move_forward_ex(Z_OBJPROP_PP(&disp->object), &pos);
		
		/* Find the existing id */
		if (zend_hash_find(&disp->name_to_dispid, name, namelen+1, (void**)&tmp) == SUCCESS)
			continue;
		
		/* add the mappings */
		MAKE_STD_ZVAL(tmp);
		ZVAL_STRINGL(tmp, name, namelen, 1);
		zend_hash_index_update(&disp->dispid_to_name, pid, (void*)&tmp, sizeof(zval *), NULL);

		MAKE_STD_ZVAL(tmp);
		ZVAL_LONG(tmp, pid);
		zend_hash_update(&disp->name_to_dispid, name, namelen+1, (void*)&tmp, sizeof(zval *), NULL);
	}
}

static php_dispatchex *disp_constructor(zval *object TSRMLS_DC)
{
	php_dispatchex *disp = (php_dispatchex*)CoTaskMemAlloc(sizeof(php_dispatchex));
	
	if (disp == NULL)
		return NULL;

	memset(disp, 0, sizeof(php_dispatchex));

	disp->engine_thread = tsrm_thread_id();
	disp->lpVtbl = &php_dispatch_vtbl;
	disp->refcount = 1;

	zend_hash_init(&disp->dispid_to_name, 0, NULL, NULL, TRUE);
	zend_hash_init(&disp->name_to_dispid, 0, NULL, NULL, TRUE);

	if (object)
		ZVAL_ADDREF(object);
	disp->object = object;

	update_dispids(disp TSRMLS_CC);

	disp->id = zend_list_insert(disp, le_dispatch);
	
	return disp;
}

static void disp_destructor(php_dispatchex *disp)
{
	TSRMLS_FETCH();
	
	trace("destroying COM wrapper for PHP object %s\n", Z_OBJCE_P(disp->object)->name);

	disp->id = 0;
	
	if (disp->refcount > 0)
		CoDisconnectObject((IUnknown*)disp, 0);

	zend_hash_destroy(&disp->dispid_to_name);
	zend_hash_destroy(&disp->name_to_dispid);

	if (disp->object)
		zval_ptr_dtor(&disp->object);


	CoTaskMemFree(disp);
}


PHPAPI IDispatch *php_COM_export_object(zval *val TSRMLS_DC)
{
	if (Z_TYPE_P(val) != IS_OBJECT)
		return NULL;

	if (Z_OBJCE_P(val) == &COM_class_entry) {
		/* pass back it's IDispatch directly */
		zval **tmp;
		comval *obj;
		int type;
		
		zend_hash_index_find(Z_OBJPROP_P(val), 0, (void**)&tmp);
		obj = (comval *)zend_list_find(Z_LVAL_PP(tmp), &type);
		if (type != IS_COM)
			return NULL;

		C_DISPATCH(obj)->lpVtbl->AddRef(C_DISPATCH(obj));
		return C_DISPATCH(obj);
	}
	return (IDispatch*)disp_constructor(val TSRMLS_CC);
}


