/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */

#define _WIN32_DCOM
#define COBJMACROS

#include "../rpc.h"
#include "../handler.h"

#include "com.h"
#include "com_wrapper.h"
#include "conversion.h"
#include "variant.h"
#include "ext/standard/php_smart_str.h"
#include <oleauto.h>


/* protos */
static int com_hash(rpc_string, rpc_string *, void *, int, char *, int);
static int com_name(rpc_string, rpc_string *, void *, int);
static int com_ctor(rpc_string, void **, int , zval ***);
static int com_dtor(void *);
static int com_describe(rpc_string, void *, char **, unsigned char **);
static int com_call(rpc_string, void **, zval *, int, zval ***);
static int com_get(rpc_string, zval *, void **);
static int com_set(rpc_string, zval *, void **);
static int com_compare(void **, void **);
static int com_has_property(rpc_string, void **);
static int com_unset_property(rpc_string, void **);
static int com_get_properties(HashTable **, void **);

static PHP_INI_MH(com_typelib_file_change);

/* globals */
static IBindCtx *pBindCtx;
static unsigned char arg1and2_force_ref[] = { 2, BYREF_FORCE, BYREF_FORCE };

/* register rpc callback function */
RPC_REGISTER_HANDLERS_BEGIN(com)
TRUE,							/* poolable */
HASH_AS_INT_WITH_SIGNATURE,
com_hash,
com_name,
com_ctor,
com_dtor,
com_describe,
com_call,
com_get,
com_set,
com_compare,
com_has_property,
com_unset_property,
com_get_properties
RPC_REGISTER_HANDLERS_END()

/* register ini settings */
PHP_INI_BEGIN()
PHP_INI_ENTRY_EX("com.allow_dcom", "0", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY_EX("com.autoregister_typelib", "0", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY_EX("com.autoregister_verbose", "0", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY_EX("com.autoregister_casesensitive", "1", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY("com.typelib_file", "", PHP_INI_SYSTEM, com_typelib_file_change)
PHP_INI_END()

/* register userspace functions */
RPC_FUNCTION_ENTRY_BEGIN(com)
	ZEND_FALIAS(com_invoke,	rpc_call,		NULL)
	ZEND_FE(com_addref,			NULL)
	ZEND_FE(com_release,		NULL)
	ZEND_FE(com_next,			NULL)
	ZEND_FE(com_all,			NULL)
	ZEND_FE(com_reset,			NULL)
	ZEND_FE(com_skip,			NULL)
	ZEND_FE(com_event_sink,		arg1and2_force_ref)
	ZEND_FE(com_message_pump,	NULL)
	ZEND_FE(com_print_typeinfo,	NULL)
RPC_FUNCTION_ENTRY_END()

zend_module_entry com_module_entry = {
	ZE2_STANDARD_MODULE_HEADER,
	"com",
	RPC_FUNCTION_ENTRY(com),
	ZEND_MINIT(com),
	ZEND_MSHUTDOWN(com),
	NULL,
	NULL,
	ZEND_MINFO(com),
	"0.1a",
	STANDARD_MODULE_PROPERTIES
};

/* register class methods */
RPC_METHOD_ENTRY_BEGIN(com)
	ZEND_FALIAS(addref,		com_addref,		NULL)
	ZEND_FALIAS(release,	com_release,	NULL)
	ZEND_FALIAS(next,		com_next,		NULL)
	ZEND_FALIAS(all,		com_all,		NULL)
	ZEND_FALIAS(reset,		com_reset,		NULL)
	ZEND_FALIAS(skip,		com_skip,		NULL)
RPC_METHOD_ENTRY_END()


ZEND_MINIT_FUNCTION(com)
{
	CreateBindCtx(0, &pBindCtx);
	php_variant_init(module_number TSRMLS_CC);

	RPC_REGISTER_LAYER(com);
	REGISTER_INI_ENTRIES();

	return SUCCESS;
}

ZEND_MSHUTDOWN_FUNCTION(com)
{
	php_variant_shutdown(TSRMLS_C);
	pBindCtx->lpVtbl->Release(pBindCtx);

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

ZEND_MINFO_FUNCTION(com)
{
	DISPLAY_INI_ENTRIES();
}

#ifdef COMPILE_DL_COM
ZEND_GET_MODULE(com);
#endif

/* rpc handler functions */

static int com_hash(rpc_string name, rpc_string *hash, void *data, int num_args, char *arg_types, int type)
{
	OLECHAR *olestr = php_char_to_OLECHAR(name.str, name.len, CP_ACP, FALSE);

	switch (type) {
		case CLASS:
		{
			CLSID *clsid = malloc(sizeof(CLSID));

			if (FAILED(CLSIDFromString(olestr, clsid))) {
				/* Perhaps this is a Moniker? */
				free(clsid);

				hash->str = strdup(name.str);
				hash->len = name.len;
			} else {
				hash->str = (char *) clsid;
				/* str is actually not a string but a CLSID struct, thus set len to 0.
				 * nevertheless clsid is freed by the rpc_string_dtor
				 */
				hash->len = 0;
			}

			efree(olestr);

			return SUCCESS;
		}

		case METHOD:
		case PROPERTY:
		{
			DISPID *dispid = malloc(sizeof(DISPID));
			
			if(SUCCEEDED(php_COM_get_ids_of_names((comval *) data, olestr, dispid))) {
				hash->str = (char *) dispid;
				/* str is actually not a string but a DISPID struct, thus set len to 0.
				 * nevertheless dispid is freed by the rpc_string_dtor
				 */
				hash->len = 0;

				efree(olestr);

				return SUCCESS;
			} else {
				free(dispid);
				efree(olestr);

				return FAILURE;
			}
		}
	}

	efree(olestr);

	return FAILURE;
}

static int com_name(rpc_string hash, rpc_string *name, void *data, int type)
{
	if (hash.len != 0) {
		/* not a GUID, perhaps a Moniker */
		name->str = strdup(hash.str);
		name->len = hash.len;

		return SUCCESS;
	} else {
		switch (type) {
			case CLASS:
			{
				OLECHAR *olestr;

				StringFromCLSID((CLSID *) hash.str, &olestr);
				name->str = php_OLECHAR_to_char(olestr, &(name->len), CP_ACP, TRUE);
				CoTaskMemFree(olestr);

				return SUCCESS;
			}

			case METHOD:
			case PROPERTY:
				/* not used yet */
				break;
		}
	}

	return FAILURE;
}

static int com_ctor(rpc_string class_name, void **data, int num_args, zval **args[])
{
	zval **server_name = NULL;
	zval **code_page = NULL;
	zval **typelib = NULL;
	zval **user_name=NULL;
	zval **password=NULL;
	zval **domain=NULL;
	int mode = 0;
	comval *obj;
	HRESULT hr;
	CLSCTX flags = CLSCTX_SERVER;

	switch (num_args) {
		case 3:
			typelib = args[2];
			convert_to_string_ex(typelib);
			/* break missing intentionally */
		case 2:
			code_page = args[1];
			convert_to_long_ex(code_page);
			/* break missing intentionally */
		case 1:
			server_name = args[0];
			/* break missing intentionally */
			break;

		case 0:
			/* nothing to do */
			break;

		default:
			/* exception */
			return FAILURE;
	}

	if (server_name != NULL) {
		/* What is server name? A String or an array? */
		if (Z_TYPE_PP(server_name) == IS_NULL) {
			server_name = NULL;
		} else if (Z_TYPE_PP(server_name) == IS_ARRAY) {
			zval **tmp;
			/* Aha - we have a number of possible arguments.
			 * They are in the hash By name: Server, Domain, Username, Password
			 * Flags.
			 * This has been crafted to maintian maximum backward compatability.
			 * If the server name is specified as a string, then the function
			 * should behave as before by defaulting username and password and
			 * using the (I believe) incorrect CLSCTX_SERVER instantiation
			 * paramter. However if server is specified in this array then we
			 * use either CLSCTX_REMOTE_SERVER or whatever flags are specified
			 * in the array
			 */
			HashTable *ht = Z_ARRVAL_PP(server_name);
			if (FAILURE == zend_hash_find(ht, "Server", 7, (void **) &tmp)) {
				server_name = NULL;
			} else {
				server_name = tmp;
				convert_to_string_ex(server_name);
				/* CLSCTX_SERVER includes INPROC and LOCAL SERVER. This means
				 * that any local server will be instantiated BEFORE even
				 * looking on a remote server. Thus if we have a server name,
				 * probably we want to access a remote machine or we would not
				 * have bothered specifying it. So it would be wrong to to
				 * connect locally. Futher, unless the name passed is a GUID,
				 * there has to be something to map the Prog.Id to GUID and
				 * unless that has been modified to remove the information
				 * about local instantiation CLSCTX_SERVER would force a local
				 * instantiation This setting can be overridden below if the
				 * user specifies a flags element */
				flags = CLSCTX_REMOTE_SERVER;
			}
			if (FAILURE == zend_hash_find(ht, "username", 9, (void **) &tmp)) {
				user_name = NULL;
			} else {
				user_name = tmp;
				convert_to_string_ex(user_name);
			}
			if (FAILURE == zend_hash_find(ht, "domain", 7, (void **) &tmp)) {
				domain = NULL;
			} else {
				domain = tmp;
				convert_to_string_ex(domain);
			}
			if (FAILURE == zend_hash_find(ht, "password", 9, (void **) &tmp)) {
				password=NULL;
			} else {
				password = tmp;
				convert_to_string_ex(password);
			}
			if (SUCCESS == zend_hash_find(ht, "flags", 6, (void **) &tmp)) {
				convert_to_long_ex(tmp);
				flags = (CLSCTX) Z_LVAL_PP(tmp);
			}
		}

		if (server_name != NULL) {
			if (!INI_INT("com.allow_dcom")) {
				rpc_error(E_WARNING, "DCOM is disabled");
				return FAILURE;
			} else {
				flags = CLSCTX_REMOTE_SERVER;
				convert_to_string_ex(server_name);
			}
		}
	}

	ALLOC_COM(obj);
	*data = obj;

	if (code_page != NULL) {
		C_CODEPAGE(obj) = Z_LVAL_PP(code_page);
	}

	if (class_name.len) {
		/* Perhaps this is a Moniker? */
		IMoniker *pMoniker;
		ULONG ulEaten;

		if (server_name) {
			hr = MK_E_SYNTAX;
		} else {
			OLECHAR *olestr = php_char_to_OLECHAR(class_name.str, class_name.len, C_CODEPAGE(obj), FALSE);

			if (SUCCEEDED(hr = MkParseDisplayNameEx(pBindCtx, olestr, &ulEaten, &pMoniker))) {
				hr = pMoniker->lpVtbl->BindToObject(pMoniker, pBindCtx, NULL, &IID_IDispatch, (LPVOID *) &C_DISPATCH(obj));
				pMoniker->lpVtbl->Release(pMoniker);
			}

			efree(olestr);
		}

		if (FAILED(hr)) {
			char *error_message;

			php_COM_destruct(obj);
			error_message = php_COM_error_message(hr);  
			rpc_error(E_WARNING,"Invalid ProgID, GUID string, or Moniker: %s", error_message);
			LocalFree(error_message);
			
			return FAILURE;
		}
	} else {
		/* obtain IDispatch */
		if (!server_name) {
			hr = CoCreateInstance((CLSID *) class_name.str, NULL, flags, &IID_IDispatch, (LPVOID *) &C_DISPATCH(obj));
		} else {
			COSERVERINFO server_info;
			MULTI_QI pResults;
			COAUTHIDENTITY authid;
			COAUTHINFO authinfo = {RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, &authid, EOAC_NONE};

			server_info.dwReserved1=0;
			server_info.dwReserved2=0;
			server_info.pwszName = php_char_to_OLECHAR(Z_STRVAL_PP(server_name), Z_STRLEN_PP(server_name), C_CODEPAGE(obj), FALSE);
			if (user_name) {
				/* Parse Username into domain\username */
				authid.User = (WCHAR *) Z_STRVAL_PP(user_name);
				authid.UserLength = Z_STRLEN_PP(user_name);
				if (password) {
					authid.Password = (USHORT *) Z_STRVAL_PP(password);
					authid.PasswordLength = Z_STRLEN_PP(password);
				} else {
					authid.Password = (USHORT *) "";
					authid.PasswordLength = 0;
				}
				if (domain) {
					authid.Domain = (USHORT *) Z_STRVAL_PP(domain);
					authid.DomainLength = Z_STRLEN_PP(domain);
				} else {
					authid.Domain = (USHORT *) "";
					authid.DomainLength = 0;
				}
				authid.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;

				server_info.pAuthInfo=&authinfo;
			} else {
				server_info.pAuthInfo=NULL;
			}

			pResults.pIID = &IID_IDispatch;
			pResults.pItf = NULL;
			pResults.hr = S_OK;
			hr=CoCreateInstanceEx((CLSID *) class_name.str, NULL, flags, &server_info, 1, &pResults);
			if (SUCCEEDED(hr)) {
				hr = pResults.hr;
				C_DISPATCH(obj) = (IDispatch *) pResults.pItf;
			}
			efree(server_info.pwszName);
		}

		if (FAILED(hr)) {
			char *error_message, *clsid;

			php_COM_destruct(obj);
			error_message = php_COM_error_message(hr);
			clsid = php_COM_string_from_CLSID((CLSID *)class_name.str);
			rpc_error(E_WARNING,"Unable to obtain IDispatch interface for CLSID %s: %s", clsid, error_message);
			LocalFree(error_message);
			efree(clsid);

			return FAILURE;
		}
	}

	php_COM_set(obj, &C_DISPATCH(obj), TRUE);

	if (INI_INT("com.autoregister_casesensitive")) {
		mode |= CONST_CS;
	}

	if (C_HASTLIB(obj)) {
		if (INI_INT("com.autoregister_typelib")) {
			ITypeLib *pTL;
			unsigned int idx;

			/* @todo check if typlib isn't already loaded */
			if (C_TYPEINFO_VT(obj)->GetContainingTypeLib(C_TYPEINFO(obj), &pTL, &idx) == S_OK) {
				php_COM_load_typelib(pTL, mode);
				pTL->lpVtbl->Release(pTL);
			}
		}
	} else {
		if (typelib != NULL) {
			ITypeLib *pTL;

			if ((pTL = php_COM_find_typelib(Z_STRVAL_PP(typelib), mode)) != NULL) {
				C_HASTLIB(obj) = SUCCEEDED(pTL->lpVtbl->GetTypeInfo(pTL, 0, &C_TYPEINFO(obj)));
				/* idx 0 should deliver the ITypeInfo for the IDispatch Interface */
				if (INI_INT("com.autoregister_typelib")) {
					php_COM_load_typelib(pTL, mode);
				}
				pTL->lpVtbl->Release(pTL);
			}
		}
	}

	return SUCCESS;
}

static int com_dtor(void *data)
{
	php_COM_destruct((comval *) data);

	return SUCCESS;
}

static inline void vt_type_to_zpp_string(ELEMDESC *elem, smart_str *argtypes_str, unsigned char *argflags)
{
	int ref = 0;
	int nullable = 0;
	char zppflag = 'z';
	WORD vt, flags;

	vt = elem->tdesc.vt;
	flags = elem->paramdesc.wParamFlags;

	if (vt == VT_PTR) {
		nullable = 1;
		ref = 0;
		vt = elem->tdesc.lptdesc->vt;
	} else {
		ref = vt & VT_BYREF;
	}
	
	if (vt & VT_ARRAY) {
		zppflag = 'a';	
	} else {
		switch(vt & ~(VT_BYREF | VT_ARRAY)) {
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_I1:
		case VT_I2:
		case VT_I4:
			zppflag = 'l';
			break;

		case VT_R8:
		case VT_CY:
		case VT_DATE:
			zppflag = 'd';
			break;

		case VT_BSTR:
			zppflag = 's';
			break;

		case VT_BOOL:
			zppflag = 'b';
			break;

		case VT_DISPATCH:
		case VT_UNKNOWN:
			zppflag = 'o';
			break;

		case VT_VARIANT:
		default:
			zppflag = 'z';

		}
	}

	smart_str_appendl(argtypes_str, &zppflag, 1);
	if (ref) {
		smart_str_appendl(argtypes_str, "/", 1);
		*argflags = BYREF_FORCE;
	} else {
		*argflags = BYREF_NONE;
	}
	if (nullable) {
		smart_str_appendl(argtypes_str, "!", 1);
	}
}

static int com_describe(rpc_string method_name, void *data, char **arg_types, unsigned char **ref_types)
{
	rpc_internal *intern;
	comval *obj;
	ITypeInfo *typeinfo;
	FUNCDESC *funcdesc;
	MEMBERID fid;
	OLECHAR *olename;
	int retval = FAILURE, arg_count;
	int i, type_len = 0;
	smart_str argtypes_str = {0};
	unsigned char *func_arg_types;
	TSRMLS_FETCH();

	GET_INTERNAL_EX(intern, data);
	obj = (comval*)data;

	if (C_TYPEINFO(obj)) {
		typeinfo = C_TYPEINFO(obj);
		ITypeInfo_AddRef(typeinfo);
	} else if (FAILED(C_DISPATCH_VT(obj)->GetTypeInfo(C_DISPATCH(obj), 0, LOCALE_SYSTEM_DEFAULT, &typeinfo))) {
		return FAILURE;
	}

	olename = php_char_to_OLECHAR(method_name.str, method_name.len, CP_ACP, FALSE);

	if (SUCCEEDED(ITypeInfo_GetIDsOfNames(typeinfo, &olename, 1, &fid)) && SUCCEEDED(ITypeInfo_GetFuncDesc(typeinfo, fid, &funcdesc))) {

		arg_count = funcdesc->cParams + (funcdesc->cParamsOpt == -1 ? 1 : funcdesc->cParamsOpt);
		
		func_arg_types = (unsigned char*)malloc((1 + arg_count) * sizeof(unsigned char));

		func_arg_types[0] = arg_count;

		/* required parameters first */
		for (i = 0; i < funcdesc->cParams; i++) {
			ELEMDESC *elem = &funcdesc->lprgelemdescParam[i];

			vt_type_to_zpp_string(elem, &argtypes_str, &func_arg_types[i+1]);
		}

		if (funcdesc->cParamsOpt == -1) {
			/* needs to be a SAFEARRAY of VARIANTS */
			smart_str_appendl(&argtypes_str, "|z", 2);
			func_arg_types[funcdesc->cParams+1] = BYREF_NONE;
		} else if (funcdesc->cParamsOpt > 0) {
			smart_str_appendl(&argtypes_str, "|", 1);

			for (i = funcdesc->cParams; i < funcdesc->cParams + funcdesc->cParamsOpt; i++) {
				ELEMDESC *elem = &funcdesc->lprgelemdescParam[i];

				vt_type_to_zpp_string(elem, &argtypes_str, &func_arg_types[i+1]);
			}
		}

		*ref_types = func_arg_types;
		smart_str_0(&argtypes_str);
		*arg_types = strdup(argtypes_str.c);
		smart_str_free(&argtypes_str);

		retval = SUCCESS;
		ITypeInfo_ReleaseFuncDesc(typeinfo, funcdesc);
	}
	
	efree(olename);
	ITypeInfo_Release(typeinfo);

	return retval;
}

static int com_call(rpc_string method_name, void **data, zval *return_value, int num_args, zval **args[])
{
	DISPID dispid;
	DISPPARAMS dispparams;
	HRESULT hr;
	OLECHAR *funcname = NULL;
	VARIANT *variant_args;
	VARIANT result;
	int current_arg, current_variant;
	char *ErrString = NULL;
	TSRMLS_FETCH();

	/* if the length of the name is 0, we are dealing with a pointer to a dispid */
	assert(method_name.len == 0);
	dispid = *(DISPID*)method_name.str;

	variant_args = num_args ? (VARIANT *) emalloc(sizeof(VARIANT) * num_args) : NULL;

	for (current_arg = 0; current_arg < num_args; current_arg++) {
		current_variant = num_args - current_arg - 1;
		php_zval_to_variant(*args[current_arg], &variant_args[current_variant], C_CODEPAGE((comval *) *data));
	}

	dispparams.rgvarg = variant_args;
	dispparams.rgdispidNamedArgs = NULL;
	dispparams.cArgs = num_args;
	dispparams.cNamedArgs = 0;

	VariantInit(&result);

	hr = php_COM_invoke((comval *) *data, dispid, DISPATCH_METHOD|DISPATCH_PROPERTYGET, &dispparams, &result, &ErrString);

	for (current_arg=0;current_arg<num_args;current_arg++) {
		/* don't release IDispatch pointers as they are used afterwards */
		if (V_VT(&variant_args[current_arg]) != VT_DISPATCH) {
			/* @todo review this: what happens to an array of IDispatchs or a VARIANT->IDispatch */
			VariantClear(&variant_args[current_arg]);
		}
	}

	if (variant_args) {
		efree(variant_args);
		variant_args = NULL;
	}

	if (FAILED(hr)) {
		char *error_message;

		error_message = php_COM_error_message(hr);
		if (ErrString) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invoke() failed: %s %s", error_message, ErrString);
			efree(ErrString);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"Invoke() failed: %s", error_message);
		}
		LocalFree(error_message);
		return FAILURE;
	}

	RETVAL_VARIANT(&result, C_CODEPAGE((comval *) *data));

	return SUCCESS;
}

static int com_get(rpc_string property_name, zval *return_value, void **data)
{
	char *ErrString = NULL;
	VARIANT *result;
	OLECHAR *propname;
	DISPID dispid;
	DISPPARAMS dispparams;
	HRESULT hr;
	TSRMLS_FETCH();

	/* obtain property handler */
	propname = php_char_to_OLECHAR(property_name.str, property_name.len, CP_ACP, FALSE);

	if (FAILED(hr = php_COM_get_ids_of_names((comval *) *data, propname, &dispid))) {
		char *error_message;

		efree(propname);
		error_message = php_COM_error_message(hr);
		rpc_error(E_WARNING,"Unable to lookup %s: %s", property_name.str, error_message);
		LocalFree(error_message);

		return FAILURE;
	}

	efree(propname);

	result = (VARIANT *) emalloc(sizeof(VARIANT));
	VariantInit(result);

	dispparams.cArgs = 0;
	dispparams.cNamedArgs = 0;

	if (FAILED(hr = php_COM_invoke((comval *) data, dispid, DISPATCH_PROPERTYGET, &dispparams, result, &ErrString))) {
		char *error_message;

		efree(result);
		error_message = php_COM_error_message(hr);
		if (ErrString != NULL) {
			rpc_error(E_WARNING,"PropGet() failed: %s %s", error_message, ErrString);
			efree(ErrString);
		} else {
			rpc_error(E_WARNING,"PropGet() failed: %s", error_message);
		}
		LocalFree(error_message);

		return FAILURE;
	}

	RETVAL_VARIANT(result, C_CODEPAGE((comval *) *data));

	return SUCCESS;
}

static int com_set(rpc_string property_name, zval *value, void **data)
{
	HRESULT hr;
	OLECHAR *propname;
	DISPID dispid, mydispid = DISPID_PROPERTYPUT;
	DISPPARAMS dispparams;
	VARIANT *var;
	char *error_message, *ErrString = NULL;
	TSRMLS_FETCH();

	/* obtain property handler */
	propname = php_char_to_OLECHAR(property_name.str, property_name.len, CP_ACP, FALSE);

	if (FAILED(hr = php_COM_get_ids_of_names((comval *) *data, propname, &dispid))) {
		error_message = php_COM_error_message(hr);
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Unable to lookup %s: %s", property_name.str, error_message);
		LocalFree(error_message);
		efree(propname);

		return FAILURE;
	}

	efree(propname);

	var = (VARIANT *) emalloc(sizeof(VARIANT));
	VariantInit(var);

	php_zval_to_variant(value, var, C_CODEPAGE((comval *) *data));
	dispparams.rgvarg = var;
	dispparams.rgdispidNamedArgs = &mydispid;
	dispparams.cArgs = 1;
	dispparams.cNamedArgs = 1;

	if (FAILED(hr = php_COM_invoke((comval *) *data, dispid, DISPATCH_PROPERTYPUT, &dispparams, NULL, &ErrString))) {
		error_message = php_COM_error_message(hr);
		if (ErrString) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"PropPut() failed: %s %s", error_message, ErrString);
			efree(ErrString);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"PropPut() failed: %s", error_message);
		}
		LocalFree(error_message);
		VariantClear(var);
		efree(var);

		return FAILURE;
	}


	VariantClear(var);
	efree(var);

	return SUCCESS;
}

static int com_compare(void **data1, void **data2)
{
	return SUCCESS;
}

static int com_has_property(rpc_string property_name, void **data)
{
	return SUCCESS;
}

static int com_unset_property(rpc_string property_name, void **data)
{
	return SUCCESS;
}

static int com_get_properties(HashTable **properties, void **data)
{
	return SUCCESS;
}


/* custom functions */

/* {{{ proto mixed com_addref(int module)
   Increases the reference counter on a COM object */
ZEND_FUNCTION(com_addref)
{
	zval *object = getThis();
	rpc_internal *intern;

	if (object == NULL) {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &object, com_class_entry);
	} else {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "");
	}

	if (GET_INTERNAL_EX(intern, object) != SUCCESS) {
		/* TODO: exception */
	}

	RETURN_LONG(php_COM_addref((comval *) intern->data));
}
/* }}} */

/* {{{ proto mixed com_release(int module)
   Releases a COM object */
ZEND_FUNCTION(com_release)
{
	zval *object = getThis();
	rpc_internal *intern;

	if (object == NULL) {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &object, com_class_entry);
	} else {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "");
	}

	if (GET_INTERNAL_EX(intern, object) != SUCCESS) {
		/* TODO: exception */
	}

	RETURN_LONG(php_COM_release((comval *) intern->data));
}
/* }}} */

PHP_FUNCTION(com_next)
{
	zval *object = getThis();
	rpc_internal *intern;
	comval *obj;
	unsigned long count = 1;

	if (object == NULL) {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|l", &object, com_class_entry, &count);
	} else {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &count);
	}

	if (GET_INTERNAL_EX(intern, object) != SUCCESS) {
		/* TODO: exception */
	}

	obj = (comval *) intern->data;

	if (C_HASENUM(obj)) {
		SAFEARRAY *pSA;
		SAFEARRAYBOUND rgsabound[1];
		VARIANT *result;
		HRESULT hr;

		/* Grab one argument off the stack, allocate enough
		 * VARIANTs
		 * Get the IEnumVariant interface and call ->Next();
		 */

		rgsabound[0].lLbound = 0;
		rgsabound[0].cElements = count;

		result = (VARIANT *) emalloc(sizeof(VARIANT));
		VariantInit(result);

		if ((pSA = SafeArrayCreate(VT_VARIANT, 1, rgsabound)) == NULL) {
			efree(result);
			/* @todo exception */
			
			RETURN_NULL();
		} else {
			V_ARRAY(result) = pSA;
			V_VT(result) = VT_VARIANT|VT_ARRAY;
		}

		if (FAILED(hr = C_ENUMVARIANT_VT(obj)->Next(C_ENUMVARIANT(obj), count, pSA->pvData, &count))) {
			char *error_message;
				
			efree(result);
			error_message = php_COM_error_message(hr);
			rpc_error(E_WARNING,"IEnumVariant::Next() failed: %s", error_message);
			efree(error_message);

			RETURN_NULL();
		}

		if (count != rgsabound[0].cElements) {
			rgsabound[0].cElements = count;
			if (FAILED(SafeArrayRedim(pSA, rgsabound))) {
				char *error_message;
				
				efree(result);
				error_message = php_COM_error_message(hr);
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"IEnumVariant::Next() failed: %s", error_message);
				efree(error_message);

				RETURN_NULL();
			}
		}

		/* return a single element if next() was called without count */
		if ((ZEND_NUM_ARGS() == 0) && (count == 1)) {
			long index[] = {0};

			SafeArrayGetElement(pSA, index, result);
			SafeArrayDestroy(pSA);
		}

		RETURN_VARIANT(result, C_CODEPAGE(obj));
	}

	/* @todo exception */
	RETURN_NULL();
}

PHP_FUNCTION(com_all)
{
#if 0
		} else if (C_HASENUM(obj) && strstr(Z_STRVAL_P(function_name), "all")) {
#define FETCH_BLOCKSIZE 10 /* fetch blocks of 10 elements */

		count = FETCH_BLOCKSIZE;

		rgsabound[0].lLbound = 0;
		rgsabound[0].cElements = count;

		if ((pSA = SafeArrayCreate(VT_VARIANT, 1, rgsabound)) == NULL) {
			VariantInit(var_result);
			return FAILURE;
		} else {
			V_ARRAY(var_result) = pSA;
			V_VT(var_result) = VT_VARIANT|VT_ARRAY;
		}

		/* blah*/
#endif
}

PHP_FUNCTION(com_reset)
{
	zval *object = getThis();
	rpc_internal *intern;
	comval *obj;

	if (object == NULL) {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &object, com_class_entry);
	} else {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "");
	}

	if (GET_INTERNAL_EX(intern, object) != SUCCESS) {
		/* TODO: exception */
	}

	obj = (comval *) intern->data;

	if (C_HASENUM(obj)) {
		HRESULT hr;

		if (FAILED(hr = C_ENUMVARIANT_VT(obj)->Reset(C_ENUMVARIANT(obj)))) {
			char *error_message = php_COM_error_message(hr);
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"IEnumVariant::Next() failed: %s", error_message);
			efree(error_message);

			RETURN_FALSE;
		}
		
		RETURN_TRUE;
	}

	/* @todo exception */
	RETURN_FALSE;
}

PHP_FUNCTION(com_skip)
{
	zval *object = getThis();
	rpc_internal *intern;
	comval *obj;
	unsigned long count = 1;

	if (object == NULL) {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|l", &object, com_class_entry, &count);
	} else {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &count);
	}

	if (GET_INTERNAL_EX(intern, object) != SUCCESS) {
		/* TODO: exception */
	}

	obj = (comval *) intern->data;

	if (C_HASENUM(obj)) {
		HRESULT hr;

		if (FAILED(hr = C_ENUMVARIANT_VT(obj)->Skip(C_ENUMVARIANT(obj), count))) {
			char *error_message = php_COM_error_message(hr);
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"IEnumVariant::Next() failed: %s", error_message);
			efree(error_message);
			RETURN_FALSE;
		}
		
		RETURN_TRUE;
	}

	/* @todo exception */
	RETURN_FALSE;
}

/* {{{ proto bool com_isenum(object com_module)
   Grabs an IEnumVariant */
ZEND_FUNCTION(com_isenum)
{
	zval *object = getThis();
	rpc_internal *intern;

	if (object == NULL) {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &object, com_class_entry);
	} else {
		zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "");
	}

	if (GET_INTERNAL_EX(intern, object) != SUCCESS) {
		/* TODO: exception */
	}

	RETURN_BOOL(C_HASENUM((comval *) intern->data));
}
/* }}} */

/* {{{ proto bool com_load_typelib(string typelib_name [, int case_insensitive]) 
   Loads a Typelib */
PHP_FUNCTION(com_load_typelib)
{
	char *typelib;
	int len, cis = FALSE;
	int mode = CONST_CS;
	ITypeLib *pTL;

	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &typelib, &len, &cis);

	if (cis) {
		mode &= ~CONST_CS;
	}

	pTL = php_COM_find_typelib(typelib, mode);
	if (php_COM_load_typelib(pTL, mode) == SUCCESS) {
		pTL->lpVtbl->Release(pTL);
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool com_print_typeinfo(mixed comobject | string typelib, string dispinterface, bool wantsink)
   Print out a PHP class definition for a dispatchable interface */
PHP_FUNCTION(com_print_typeinfo)
{
	zval *object;
	char *ifacename = NULL;
	char *typelibname = NULL;
	int typeliblen, ifacelen;
	zend_bool wantsink = 0;
	comval *obj = NULL;
	rpc_internal *intern;
	ITypeInfo *typeinfo;
	
	if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s/s!b",
											&typelibname, &typeliblen, &ifacename, &ifacelen, &wantsink)) {
		if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O/s!b",
											 &object, com_class_entry, &ifacename, &ifacelen, &wantsink)) {
			RETURN_FALSE;
		} else {
			if (GET_INTERNAL_EX(intern, object) != SUCCESS) {
				/* TODO: exception */
			}

			obj = (comval *) intern->data;
		}
	}

	typeinfo = php_COM_locate_typeinfo(typelibname, obj, ifacename, wantsink);
	if (typeinfo) {
		php_COM_process_typeinfo(typeinfo, NULL, 1, NULL);
		typeinfo->lpVtbl->Release(typeinfo);

		RETURN_TRUE;
	} else {
		rpc_error(E_WARNING, "Unable to find typeinfo using the parameters supplied");
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool com_event_sink(mixed comobject, object sinkobject [, mixed sinkinterface])
   Connect events from a COM object to a PHP object */
PHP_FUNCTION(com_event_sink)
{
	zval *object, *sinkobject, *sink=NULL;
	char *dispname = NULL, *typelibname = NULL;
	zend_bool gotguid = 0;
	comval *obj;
	rpc_internal *intern;
	ITypeInfo *typeinfo = NULL;

	RETVAL_FALSE;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Oz|z/", &object, com_class_entry, &sinkobject, &sink)) {
		RETURN_FALSE;
	}

	if (sink && Z_TYPE_P(sink) == IS_ARRAY) {
		/* 0 => typelibname, 1 => dispname */
		zval **tmp;

		if (zend_hash_index_find(Z_ARRVAL_P(sink), 0, (void**)&tmp) == SUCCESS)
			typelibname = Z_STRVAL_PP(tmp);
		if (zend_hash_index_find(Z_ARRVAL_P(sink), 1, (void**)&tmp) == SUCCESS)
			dispname = Z_STRVAL_PP(tmp);
	} else if (sink != NULL) {
		convert_to_string_ex(&sink);
		dispname = Z_STRVAL_P(sink);
	}
	
	if (GET_INTERNAL_EX(intern, object) != SUCCESS) {
		/* TODO: exception */
	}

	obj = (comval *) intern->data;

	typeinfo = php_COM_locate_typeinfo(typelibname, obj, dispname, 1);

	if (typeinfo) {
		HashTable *id_to_name;
		
		ALLOC_HASHTABLE(id_to_name);
		
		if (php_COM_process_typeinfo(typeinfo, id_to_name, 0, &obj->sinkid)) {

			/* Create the COM wrapper for this sink */
			obj->sinkdispatch = php_COM_export_as_sink(sinkobject, &obj->sinkid, id_to_name);

			/* Now hook it up to the source */
			php_COM_enable_events(obj, TRUE);

			RETVAL_TRUE;

		} else {
			FREE_HASHTABLE(id_to_name);
		}
	}
	
	if (typeinfo)
		typeinfo->lpVtbl->Release(typeinfo);
	
}
/* }}} */



/* ini callbacks */

static PHP_INI_MH(com_typelib_file_change)
{
	FILE *typelib_file;
	char *typelib_name_buffer;
	char *strtok_buf = NULL;
	int interactive;
	interactive = CG(interactive);

	if (!new_value || (typelib_file = VCWD_FOPEN(new_value, "r"))==NULL) {
		return FAILURE;
	}

	if (interactive) {
		printf("Loading type libraries...");
		fflush(stdout);
	}

	typelib_name_buffer = (char *) emalloc(sizeof(char)*1024);

	while (fgets(typelib_name_buffer, 1024, typelib_file)) {
		ITypeLib *pTL;
		char *typelib_name;
		char *modifier, *ptr;
		int mode = CONST_CS | CONST_PERSISTENT;	/* CONST_PERSISTENT is ok here */

		if (typelib_name_buffer[0]==';') {
			continue;
		}
		typelib_name = php_strtok_r(typelib_name_buffer, "\r\n", &strtok_buf); /* get rid of newlines */
		if (typelib_name == NULL) {
			continue;
		}
		typelib_name = php_strtok_r(typelib_name, "#", &strtok_buf);
		modifier = php_strtok_r(NULL, "#", &strtok_buf);
		if (modifier != NULL) {
			if (!strcmp(modifier, "cis") || !strcmp(modifier, "case_insensitive")) {
				mode &= ~CONST_CS;
			}
		}

		/* Remove leading/training white spaces on search_string */
		while (isspace(*typelib_name)) {/* Ends on '\0' in worst case */
			typelib_name ++;
		}
		ptr = typelib_name + strlen(typelib_name) - 1;
		while ((ptr != typelib_name) && isspace(*ptr)) {
			*ptr = '\0';
			ptr--;
		}

		if (interactive) {
			printf("\rLoading %-60s\r", typelib_name);
		}

		if ((pTL = php_COM_find_typelib(typelib_name, mode)) != NULL) {
			php_COM_load_typelib(pTL, mode);
			pTL->lpVtbl->Release(pTL);
		}
	}

	efree(typelib_name_buffer);
	fclose(typelib_file);

	if (interactive) {
		printf("\r%70s\r", "");
	}

	return SUCCESS;
}