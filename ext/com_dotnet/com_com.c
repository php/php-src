/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

/* {{{ com_create_instance - ctor for COM class */
PHP_FUNCTION(com_create_instance)
{
	zval *object = getThis();
	zval *server_params = NULL;
	php_com_dotnet_object *obj;
	char *module_name, *typelib_name = NULL, *server_name = NULL;
	char *user_name = NULL, *domain_name = NULL, *password = NULL;
	int module_name_len, typelib_name_len, server_name_len,
		user_name_len, domain_name_len, password_len;
	OLECHAR *moniker;
	CLSID clsid;
	CLSCTX ctx = CLSCTX_SERVER;
	HRESULT res = E_FAIL;
	int mode = COMG(autoreg_case_sensitive) ? CONST_CS : 0;
	ITypeLib *TL = NULL;
	COSERVERINFO	info;
	COAUTHIDENTITY	authid = {0};
	COAUTHINFO		authinfo = {
		RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
		&authid, EOAC_NONE
	};

	php_com_initialize(TSRMLS_C);
	obj = CDNO_FETCH(object);

	if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS() TSRMLS_CC, "s|s!ls",
			&module_name, &module_name_len, &server_name, &server_name_len,
			&obj->code_page, &typelib_name, &typelib_name_len) &&
		FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS() TSRMLS_CC, "sa|ls",
			&module_name, &module_name_len, &server_params, &obj->code_page,
			&typelib_name, &typelib_name_len)) {

		php_com_throw_exception(E_INVALIDARG, "Could not create COM object - invalid arguments!" TSRMLS_CC);
		ZVAL_NULL(object);
		return;
	}

	if (server_name) {
		ctx = CLSCTX_REMOTE_SERVER;
	} else if (server_params) {
		zval **tmp;

		/* decode the data from the array */

		if (SUCCESS == zend_hash_find(HASH_OF(server_params),
				"Server", sizeof("Server"), (void**)&tmp)) {
			convert_to_string_ex(tmp);
			server_name = Z_STRVAL_PP(tmp);
			server_name_len = Z_STRLEN_PP(tmp);
			ctx = CLSCTX_REMOTE_SERVER;
		}

		if (SUCCESS == zend_hash_find(HASH_OF(server_params),
				"Username", sizeof("Username"), (void**)&tmp)) {
			convert_to_string_ex(tmp);
			user_name = Z_STRVAL_PP(tmp);
			user_name_len = Z_STRLEN_PP(tmp);
		}

		if (SUCCESS == zend_hash_find(HASH_OF(server_params),
				"Password", sizeof("Password"), (void**)&tmp)) {
			convert_to_string_ex(tmp);
			password = Z_STRVAL_PP(tmp);
			password_len = Z_STRLEN_PP(tmp);
		}

		if (SUCCESS == zend_hash_find(HASH_OF(server_params),
				"Domain", sizeof("Domain"), (void**)&tmp)) {
			convert_to_string_ex(tmp);
			domain_name = Z_STRVAL_PP(tmp);
			domain_name_len = Z_STRLEN_PP(tmp);
		}

		if (SUCCESS == zend_hash_find(HASH_OF(server_params),
				"Flags", sizeof("Flags"), (void**)&tmp)) {
			convert_to_long_ex(tmp);
			ctx = (CLSCTX)Z_LVAL_PP(tmp);
		}
	}

	if (server_name && !COMG(allow_dcom)) {
		php_com_throw_exception(E_ERROR, "DCOM has been disabled by your administrator [com.allow_dcom=0]" TSRMLS_CC);
		return;
	}

	moniker = php_com_string_to_olestring(module_name, module_name_len, obj->code_page TSRMLS_CC);

	/* if instantiating a remote object, either directly, or via
	 * a moniker, fill in the relevant info */
	if (server_name) {
		info.dwReserved1 = 0;
		info.dwReserved2 = 0;
		info.pwszName = php_com_string_to_olestring(server_name, server_name_len, obj->code_page TSRMLS_CC);

		if (user_name) {
			authid.User = php_com_string_to_olestring(user_name, -1, obj->code_page TSRMLS_CC);
			authid.UserLength = user_name_len;

			if (password) {
				authid.Password = (OLECHAR*)password;
				authid.PasswordLength = password_len;
			} else {
				authid.Password = (OLECHAR*)"";
				authid.PasswordLength = 0;
			}

			if (domain_name) {
				authid.Domain = (OLECHAR*)domain_name;
				authid.DomainLength = domain_name_len;
			} else {
				authid.Domain = (OLECHAR*)"";
				authid.DomainLength = 0;
			}
			authid.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
			info.pAuthInfo = &authinfo;
		} else {
			info.pAuthInfo = NULL;
		}
	}

	if (FAILED(CLSIDFromString(moniker, &clsid))) {
		/* try to use it as a moniker */
		IBindCtx *pBindCtx = NULL;
		IMoniker *pMoniker = NULL;
		ULONG ulEaten;
		BIND_OPTS2 bopt = {0};

		if (SUCCEEDED(res = CreateBindCtx(0, &pBindCtx))) {
			if (server_name) {
				/* fill in the remote server info.
				 * MSDN docs indicate that this might be ignored in
				 * current win32 implementations, but at least we are
				 * doing the right thing in readiness for the day that
				 * it does work */
				bopt.cbStruct = sizeof(bopt);
				IBindCtx_GetBindOptions(pBindCtx, (BIND_OPTS*)&bopt);
				bopt.pServerInfo = &info;
				/* apparently, GetBindOptions will only ever return
				 * a regular BIND_OPTS structure.  My gut feeling is
				 * that it will modify the size field to reflect that
				 * so lets be safe and set it to the BIND_OPTS2 size
				 * again */
				bopt.cbStruct = sizeof(bopt);
				IBindCtx_SetBindOptions(pBindCtx, (BIND_OPTS*)&bopt);
			}
			
			if (SUCCEEDED(res = MkParseDisplayName(pBindCtx, moniker, &ulEaten, &pMoniker))) {
				res = IMoniker_BindToObject(pMoniker, pBindCtx,
					NULL, &IID_IDispatch, (LPVOID*)&V_DISPATCH(&obj->v));
			
				if (SUCCEEDED(res)) {
					V_VT(&obj->v) = VT_DISPATCH;
				}

				IMoniker_Release(pMoniker);
			}
		}
		if (pBindCtx) {
			IBindCtx_Release(pBindCtx);
		}
	} else if (server_name) {
		MULTI_QI		qi;

		qi.pIID = &IID_IDispatch;
		qi.pItf = NULL;
		qi.hr = S_OK;

		res = CoCreateInstanceEx(&clsid, NULL, ctx, &info, 1, &qi);

		if (SUCCEEDED(res)) {
			res = qi.hr;
			V_DISPATCH(&obj->v) = (IDispatch*)qi.pItf;
			V_VT(&obj->v) = VT_DISPATCH;
		}
	} else {
		res = CoCreateInstance(&clsid, NULL, CLSCTX_SERVER, &IID_IDispatch, (LPVOID*)&V_DISPATCH(&obj->v));
		if (SUCCEEDED(res)) {
			V_VT(&obj->v) = VT_DISPATCH;
		}
	}

	if (server_name) {
		STR_FREE((char*)info.pwszName);
		STR_FREE((char*)authid.User);
	}

	efree(moniker);

	if (FAILED(res)) {
		char *werr, *msg;

		werr = php_win32_error_to_msg(res);
		spprintf(&msg, 0, "Failed to create COM object `%s': %s", module_name, werr);
		LocalFree(werr);

		php_com_throw_exception(res, msg TSRMLS_CC);
		efree(msg);
		ZVAL_NULL(object);
		return;
	}

	/* we got the object and it lives ! */

	/* see if it has TypeInfo available */
	if (FAILED(IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &obj->typeinfo)) && typelib_name) {
		/* load up the library from the named file */
		int cached;

		TL = php_com_load_typelib_via_cache(typelib_name, obj->code_page, &cached TSRMLS_CC);

		if (TL) {
			if (COMG(autoreg_on) && !cached) {
				php_com_import_typelib(TL, mode, obj->code_page TSRMLS_CC);
			}

			/* cross your fingers... there is no guarantee that this ITypeInfo
			 * instance has any relation to this IDispatch instance... */
			ITypeLib_GetTypeInfo(TL, 0, &obj->typeinfo);
			ITypeLib_Release(TL);
		}
	} else if (obj->typeinfo && COMG(autoreg_on)) {
		int idx;

		if (SUCCEEDED(ITypeInfo_GetContainingTypeLib(obj->typeinfo, &TL, &idx))) {
			/* check if the library is already in the cache by getting its name */
			BSTR name;

			if (SUCCEEDED(ITypeLib_GetDocumentation(TL, -1, &name, NULL, NULL, NULL))) {
				typelib_name = php_com_olestring_to_string(name, &typelib_name_len, obj->code_page TSRMLS_CC);

				if (SUCCESS == zend_ts_hash_add(&php_com_typelibraries, typelib_name, typelib_name_len+1, (void*)&TL, sizeof(ITypeLib*), NULL)) {
					php_com_import_typelib(TL, mode, obj->code_page TSRMLS_CC);

					/* add a reference for the hash */
					ITypeLib_AddRef(TL);
				}

			} else {
				/* try it anyway */
				php_com_import_typelib(TL, mode, obj->code_page TSRMLS_CC);
			}

			ITypeLib_Release(TL);
		}
	}

}
/* }}} */

/* {{{ proto object com_get_active_object(string progid [, int code_page ])
   Returns a handle to an already running instance of a COM object */
PHP_FUNCTION(com_get_active_object)
{
	CLSID clsid;
	char *module_name;
	int module_name_len;
	long code_page = COMG(code_page);
	IUnknown *unk = NULL;
	IDispatch *obj = NULL;
	HRESULT res;
	OLECHAR *module = NULL;

	php_com_initialize(TSRMLS_C);
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l",
				&module_name, &module_name_len, &code_page)) {
		php_com_throw_exception(E_INVALIDARG, "Invalid arguments!" TSRMLS_CC);
		return;
	}

	module = php_com_string_to_olestring(module_name, module_name_len, code_page TSRMLS_CC);

	res = CLSIDFromString(module, &clsid);

	if (FAILED(res)) {
		php_com_throw_exception(res, NULL TSRMLS_CC);
	} else {
		res = GetActiveObject(&clsid, NULL, &unk);

		if (FAILED(res)) {
			php_com_throw_exception(res, NULL TSRMLS_CC);
		} else {
			res = IUnknown_QueryInterface(unk, &IID_IDispatch, &obj);

			if (FAILED(res)) {
				php_com_throw_exception(res, NULL TSRMLS_CC);
			} else if (obj) {
				/* we got our dispatchable object */
				php_com_wrap_dispatch(return_value, obj, code_page TSRMLS_CC);
			}
		}
	}

	if (obj) {
		IDispatch_Release(obj);
	}
	if (unk) {
		IUnknown_Release(obj);
	}
	efree(module);
}
/* }}} */

/* Performs an Invoke on the given com object.
 * returns a failure code and creates an exception if there was an error */
HRESULT php_com_invoke_helper(php_com_dotnet_object *obj, DISPID id_member,
		WORD flags, DISPPARAMS *disp_params, VARIANT *v, int silent, int allow_noarg TSRMLS_DC)
{
	HRESULT hr;
	unsigned int arg_err;
	EXCEPINFO e = {0};

	hr = IDispatch_Invoke(V_DISPATCH(&obj->v), id_member,
		&IID_NULL, LOCALE_SYSTEM_DEFAULT, flags, disp_params, v, &e, &arg_err);

	if (silent == 0 && FAILED(hr)) {
		char *source = NULL, *desc = NULL, *msg = NULL;
		int source_len, desc_len;

		switch (hr) {
			case DISP_E_EXCEPTION:
				if (e.bstrSource) {
					source = php_com_olestring_to_string(e.bstrSource, &source_len, obj->code_page TSRMLS_CC);
					SysFreeString(e.bstrSource);
				}
				if (e.bstrDescription) {
					desc = php_com_olestring_to_string(e.bstrDescription, &desc_len, obj->code_page TSRMLS_CC);
					SysFreeString(e.bstrDescription);
				}
				if (PG(html_errors)) {
					spprintf(&msg, 0, "<b>Source:</b> %s<br/><b>Description:</b> %s",
						source ? source : "Unknown",
						desc ? desc : "Unknown");
				} else {
					spprintf(&msg, 0, "Source: %s\nDescription: %s",
						source ? source : "Unknown",
						desc ? desc : "Unknown");
				}
				if (desc) {
					efree(desc);
				}
				if (source) {
					efree(source);
				}
				if (e.bstrHelpFile) {
					SysFreeString(e.bstrHelpFile);
				}
				break;

			case DISP_E_PARAMNOTFOUND:
			case DISP_E_TYPEMISMATCH:
				desc = php_win32_error_to_msg(hr);
				spprintf(&msg, 0, "Parameter %d: %s", arg_err, desc);
				LocalFree(desc);
				break;

			case DISP_E_BADPARAMCOUNT:
				if ((disp_params->cArgs + disp_params->cNamedArgs == 0) && (allow_noarg == 1)) {
					/* if getting a property and they are missing all parameters,
					 * we want to create a proxy object for them; so lets not create an
					 * exception here */
					msg = NULL;
					break;
				}
				/* else fall through */
				
			default:
				desc = php_win32_error_to_msg(hr);
				spprintf(&msg, 0, "Error [0x%08x] %s", hr, desc);
				LocalFree(desc);
				break;
		}

		if (msg) {
			php_com_throw_exception(hr, msg TSRMLS_CC);
			efree(msg);
		}
	}

	return hr;
}

/* map an ID to a name */
HRESULT php_com_get_id_of_name(php_com_dotnet_object *obj, char *name,
		int namelen, DISPID *dispid TSRMLS_DC)
{
	OLECHAR *olename;
	HRESULT hr;
	DISPID *dispid_ptr;

	if (namelen == -1) {
		namelen = strlen(name);
	}

	if (obj->id_of_name_cache && SUCCESS == zend_hash_find(obj->id_of_name_cache, name, namelen, (void**)&dispid_ptr)) {
		*dispid = *dispid_ptr;
		return S_OK;
	}
	
	olename = php_com_string_to_olestring(name, namelen, obj->code_page TSRMLS_CC);

	if (obj->typeinfo) {
		hr = ITypeInfo_GetIDsOfNames(obj->typeinfo, &olename, 1, dispid);
		if (FAILED(hr)) {
			hr = IDispatch_GetIDsOfNames(V_DISPATCH(&obj->v), &IID_NULL, &olename, 1, LOCALE_SYSTEM_DEFAULT, dispid);
			if (SUCCEEDED(hr)) {
				/* fall back on IDispatch direct */
				ITypeInfo_Release(obj->typeinfo);
				obj->typeinfo = NULL;
			}
		}
	} else {
		hr = IDispatch_GetIDsOfNames(V_DISPATCH(&obj->v), &IID_NULL, &olename, 1, LOCALE_SYSTEM_DEFAULT, dispid);
	}
	efree(olename);

	if (SUCCEEDED(hr)) {
		/* cache the mapping */
		if (!obj->id_of_name_cache) {
			ALLOC_HASHTABLE(obj->id_of_name_cache);
			zend_hash_init(obj->id_of_name_cache, 2, NULL, NULL, 0);
		}
		zend_hash_update(obj->id_of_name_cache, name, namelen, dispid, sizeof(*dispid), NULL);
	}
	
	return hr;
}

/* the core of COM */
int php_com_do_invoke_byref(php_com_dotnet_object *obj, char *name, int namelen,
		WORD flags,	VARIANT *v, int nargs, zval ***args TSRMLS_DC)
{
	DISPID dispid, altdispid;
	DISPPARAMS disp_params;
	HRESULT hr;
	VARIANT *vargs = NULL, *byref_vals = NULL;
	int i, byref_count = 0, j;
	zend_internal_function *f = (zend_internal_function*)EG(current_execute_data)->function_state.function;

	/* assumption: that the active function (f) is the function we generated for the engine */
	if (!f || f->arg_info == NULL) {
	   f = NULL;
	}
	
	hr = php_com_get_id_of_name(obj, name, namelen, &dispid TSRMLS_CC);

	if (FAILED(hr)) {
		char *winerr = NULL;
		char *msg = NULL;
		winerr = php_win32_error_to_msg(hr);
		spprintf(&msg, 0, "Unable to lookup `%s': %s", name, winerr);
		LocalFree(winerr);
		php_com_throw_exception(hr, msg TSRMLS_CC);
		efree(msg);
		return FAILURE;
	}


	if (nargs) {
		vargs = (VARIANT*)safe_emalloc(sizeof(VARIANT), nargs, 0);
	}

	if (f) {
		for (i = 0; i < nargs; i++) {
			if (f->arg_info[nargs - i - 1].pass_by_reference) {
				byref_count++;
			}
		}
	}

	if (byref_count) {
		byref_vals = (VARIANT*)safe_emalloc(sizeof(VARIANT), byref_count, 0);
		for (j = 0, i = 0; i < nargs; i++) {
			if (f->arg_info[nargs - i - 1].pass_by_reference) {
				/* put the value into byref_vals instead */
				php_com_variant_from_zval(&byref_vals[j], *args[nargs - i - 1], obj->code_page TSRMLS_CC);

				/* if it is already byref, "move" it into the vargs array, otherwise
				 * make vargs a reference to this value */
				if (V_VT(&byref_vals[j]) & VT_BYREF) {
					memcpy(&vargs[i], &byref_vals[j], sizeof(vargs[i]));
					VariantInit(&byref_vals[j]); /* leave the variant slot empty to simplify cleanup */
				} else {
					VariantInit(&vargs[i]);
					V_VT(&vargs[i]) = V_VT(&byref_vals[j]) | VT_BYREF;
					/* union magic ensures that this works out */
					vargs[i].byref = &V_UINT(&byref_vals[j]);
				}
				j++;
			} else {
				php_com_variant_from_zval(&vargs[i], *args[nargs - i - 1], obj->code_page TSRMLS_CC);
			}
		}
		
	} else {
		/* Invoke'd args are in reverse order */
		for (i = 0; i < nargs; i++) {
			php_com_variant_from_zval(&vargs[i], *args[nargs - i - 1], obj->code_page TSRMLS_CC);
		}
	}

	disp_params.cArgs = nargs;
	disp_params.cNamedArgs = 0;
	disp_params.rgvarg = vargs;
	disp_params.rgdispidNamedArgs = NULL;

	if (flags & DISPATCH_PROPERTYPUT) {
		altdispid = DISPID_PROPERTYPUT;
		disp_params.rgdispidNamedArgs = &altdispid;
		disp_params.cNamedArgs = 1;
	}

	/* this will create an exception if needed */
	hr = php_com_invoke_helper(obj, dispid, flags, &disp_params, v, 0, 0 TSRMLS_CC);	

	/* release variants */
	if (vargs) {
		for (i = 0, j = 0; i < nargs; i++) {
			/* if this was byref, update the zval */
			if (f && f->arg_info[nargs - i - 1].pass_by_reference) {
				SEPARATE_ZVAL_IF_NOT_REF(args[nargs - i - 1]);

				/* if the variant is pointing at the byref_vals, we need to map
				 * the pointee value as a zval; otherwise, the value is pointing
				 * into an existing PHP variant record */
				if (V_VT(&vargs[i]) & VT_BYREF) {
					if (vargs[i].byref == &V_UINT(&byref_vals[j])) {
						/* copy that value */
						php_com_zval_from_variant(*args[nargs - i - 1], &byref_vals[j],
							obj->code_page TSRMLS_CC);
					}
				} else {
					/* not sure if this can ever happen; the variant we marked as BYREF
					 * is no longer BYREF - copy its value */
					php_com_zval_from_variant(*args[nargs - i - 1], &vargs[i],
						obj->code_page TSRMLS_CC);
				}
				VariantClear(&byref_vals[j]);
				j++;
			}	
			VariantClear(&vargs[i]);
		}
		efree(vargs);
	}

	return SUCCEEDED(hr) ? SUCCESS : FAILURE;
}



int php_com_do_invoke_by_id(php_com_dotnet_object *obj, DISPID dispid,
		WORD flags,	VARIANT *v, int nargs, zval **args, int silent, int allow_noarg TSRMLS_DC)
{
	DISPID altdispid;
	DISPPARAMS disp_params;
	HRESULT hr;
	VARIANT *vargs = NULL;
	int i;

	if (nargs) {
		vargs = (VARIANT*)safe_emalloc(sizeof(VARIANT), nargs, 0);
	}

	/* Invoke'd args are in reverse order */
	for (i = 0; i < nargs; i++) {
		php_com_variant_from_zval(&vargs[i], args[nargs - i - 1], obj->code_page TSRMLS_CC);
	}

	disp_params.cArgs = nargs;
	disp_params.cNamedArgs = 0;
	disp_params.rgvarg = vargs;
	disp_params.rgdispidNamedArgs = NULL;

	if (flags & DISPATCH_PROPERTYPUT) {
		altdispid = DISPID_PROPERTYPUT;
		disp_params.rgdispidNamedArgs = &altdispid;
		disp_params.cNamedArgs = 1;
	}

	/* this will create an exception if needed */
	hr = php_com_invoke_helper(obj, dispid, flags, &disp_params, v, silent, allow_noarg TSRMLS_CC);	

	/* release variants */
	if (vargs) {
		for (i = 0; i < nargs; i++) {
			VariantClear(&vargs[i]);
		}
		efree(vargs);
	}

	/* a bit of a hack this, but it's needed for COM array access. */
	if (hr == DISP_E_BADPARAMCOUNT)
		return hr;
	
	return SUCCEEDED(hr) ? SUCCESS : FAILURE;
}

int php_com_do_invoke(php_com_dotnet_object *obj, char *name, int namelen,
		WORD flags,	VARIANT *v, int nargs, zval **args, int allow_noarg TSRMLS_DC)
{
	DISPID dispid;
	HRESULT hr;
	char *winerr = NULL;
	char *msg = NULL;

	hr = php_com_get_id_of_name(obj, name, namelen, &dispid TSRMLS_CC);

	if (FAILED(hr)) {
		winerr = php_win32_error_to_msg(hr);
		spprintf(&msg, 0, "Unable to lookup `%s': %s", name, winerr);
		LocalFree(winerr);
		php_com_throw_exception(hr, msg TSRMLS_CC);
		efree(msg);
		return FAILURE;
	}

	return php_com_do_invoke_by_id(obj, dispid, flags, v, nargs, args, 0, allow_noarg TSRMLS_CC);
}

/* {{{ proto string com_create_guid()
   Generate a globally unique identifier (GUID) */
PHP_FUNCTION(com_create_guid)
{
	GUID retval;
	OLECHAR *guid_string;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	php_com_initialize(TSRMLS_C);
	if (CoCreateGuid(&retval) == S_OK && StringFromCLSID(&retval, &guid_string) == S_OK) {
		Z_TYPE_P(return_value) = IS_STRING;
		Z_STRVAL_P(return_value) = php_com_olestring_to_string(guid_string, &Z_STRLEN_P(return_value), CP_ACP TSRMLS_CC);

		CoTaskMemFree(guid_string);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool com_event_sink(object comobject, object sinkobject [, mixed sinkinterface])
   Connect events from a COM object to a PHP object */
PHP_FUNCTION(com_event_sink)
{
	zval *object, *sinkobject, *sink=NULL;
	char *dispname = NULL, *typelibname = NULL;
	zend_bool gotguid = 0;
	php_com_dotnet_object *obj;
	ITypeInfo *typeinfo = NULL;

	RETVAL_FALSE;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Oo|z/",
			&object, php_com_variant_class_entry, &sinkobject, &sink)) {
		RETURN_FALSE;
	}

	php_com_initialize(TSRMLS_C);
	obj = CDNO_FETCH(object);
	
	if (sink && Z_TYPE_P(sink) == IS_ARRAY) {
		/* 0 => typelibname, 1 => dispname */
		zval **tmp;

		if (zend_hash_index_find(Z_ARRVAL_P(sink), 0, (void**)&tmp) == SUCCESS)
			typelibname = Z_STRVAL_PP(tmp);
		if (zend_hash_index_find(Z_ARRVAL_P(sink), 1, (void**)&tmp) == SUCCESS)
			dispname = Z_STRVAL_PP(tmp);
	} else if (sink != NULL) {
		convert_to_string(sink);
		dispname = Z_STRVAL_P(sink);
	}
	
	typeinfo = php_com_locate_typeinfo(typelibname, obj, dispname, 1 TSRMLS_CC);

	if (typeinfo) {
		HashTable *id_to_name;
		
		ALLOC_HASHTABLE(id_to_name);
		
		if (php_com_process_typeinfo(typeinfo, id_to_name, 0, &obj->sink_id, obj->code_page TSRMLS_CC)) {

			/* Create the COM wrapper for this sink */
			obj->sink_dispatch = php_com_wrapper_export_as_sink(sinkobject, &obj->sink_id, id_to_name TSRMLS_CC);

			/* Now hook it up to the source */
			php_com_object_enable_event_sink(obj, TRUE TSRMLS_CC);
			RETVAL_TRUE;

		} else {
			FREE_HASHTABLE(id_to_name);
		}
	}
	
	if (typeinfo) {
		ITypeInfo_Release(typeinfo);
	}

}
/* }}} */

/* {{{ proto bool com_print_typeinfo(object comobject | string typelib, string dispinterface, bool wantsink)
   Print out a PHP class definition for a dispatchable interface */
PHP_FUNCTION(com_print_typeinfo)
{
	zval *arg1;
	char *ifacename = NULL;
	char *typelibname = NULL;
	int ifacelen;
	zend_bool wantsink = 0;
	php_com_dotnet_object *obj = NULL;
	ITypeInfo *typeinfo;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/|s!b", &arg1, &ifacename,
				&ifacelen, &wantsink)) {
		RETURN_FALSE;
	}

	php_com_initialize(TSRMLS_C);
	if (Z_TYPE_P(arg1) == IS_OBJECT) {
		CDNO_FETCH_VERIFY(obj, arg1);
	} else {
		convert_to_string(arg1);
		typelibname = Z_STRVAL_P(arg1);
	}

	typeinfo = php_com_locate_typeinfo(typelibname, obj, ifacename, wantsink ? 1 : 0 TSRMLS_CC);
	if (typeinfo) {
		php_com_process_typeinfo(typeinfo, NULL, 1, NULL, obj ? obj->code_page : COMG(code_page) TSRMLS_CC);
		ITypeInfo_Release(typeinfo);
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Unable to find typeinfo using the parameters supplied");
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool com_message_pump([int timeoutms])
   Process COM messages, sleeping for up to timeoutms milliseconds */
PHP_FUNCTION(com_message_pump)
{
	long timeoutms = 0;
	MSG msg;
	DWORD result;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &timeoutms) == FAILURE)
		RETURN_FALSE;
	
	php_com_initialize(TSRMLS_C);
	result = MsgWaitForMultipleObjects(0, NULL, FALSE, timeoutms, QS_ALLINPUT);

	if (result == WAIT_OBJECT_0) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/* we processed messages */
		RETVAL_TRUE;
	} else {
		/* we did not process messages (timed out) */
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto bool com_load_typelib(string typelib_name [, int case_insensitive]) 
   Loads a Typelibrary and registers its constants */
PHP_FUNCTION(com_load_typelib)
{
	char *name;
	int namelen;
	ITypeLib *pTL = NULL;
	zend_bool cs = TRUE;
	int codepage = COMG(code_page);
	int cached = 0;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &name, &namelen, &cs)) {
		return;
	}

	RETVAL_FALSE;
	
	php_com_initialize(TSRMLS_C);
	pTL = php_com_load_typelib_via_cache(name, codepage, &cached TSRMLS_CC);
	if (pTL) {
		if (cached) {
			RETVAL_TRUE;
		} else if (php_com_import_typelib(pTL, cs ? CONST_CS : 0, codepage TSRMLS_CC) == SUCCESS) {
			RETVAL_TRUE;
		}

		ITypeLib_Release(pTL);
		pTL = NULL;
	}
}
/* }}} */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
