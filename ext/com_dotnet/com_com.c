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

/* {{{ com_create_instance - ctor for COM class */
PHP_METHOD(com, __construct)
{
	zval *object = getThis();
	zend_string *server_name = NULL;
	HashTable *server_params = NULL;
	php_com_dotnet_object *obj;
	char *module_name, *typelib_name = NULL;
	size_t module_name_len = 0, typelib_name_len = 0;
	zend_string *user_name = NULL, *password = NULL, *domain_name = NULL;
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
	zend_long cp = GetACP();
	const struct php_win32_cp *cp_it;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(module_name, module_name_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(server_params, server_name)
		Z_PARAM_LONG(cp)
		Z_PARAM_STRING(typelib_name, typelib_name_len)
	ZEND_PARSE_PARAMETERS_END();

	php_com_initialize();
	obj = CDNO_FETCH(object);

	cp_it = php_win32_cp_get_by_id((DWORD)cp);
	if (!cp_it) {
		php_com_throw_exception(E_INVALIDARG, "Could not create COM object - invalid codepage!");
		RETURN_THROWS();
	}
	obj->code_page = (int)cp;

	if (server_name) {
		ctx = CLSCTX_REMOTE_SERVER;
	} else if (server_params) {
		zval *tmp;

		/* decode the data from the array */

		if (NULL != (tmp = zend_hash_str_find(server_params,
				"Server", sizeof("Server")-1))) {
			server_name = zval_get_string(tmp);
			ctx = CLSCTX_REMOTE_SERVER;
		}

		if (NULL != (tmp = zend_hash_str_find(server_params,
				"Username", sizeof("Username")-1))) {
			user_name = zval_get_string(tmp);
		}

		if (NULL != (tmp = zend_hash_str_find(server_params,
				"Password", sizeof("Password")-1))) {
			password = zval_get_string(tmp);
		}

		if (NULL != (tmp = zend_hash_str_find(server_params,
				"Domain", sizeof("Domain")-1))) {
			domain_name = zval_get_string(tmp);
		}

		if (NULL != (tmp = zend_hash_str_find(server_params,
				"Flags", sizeof("Flags")-1))) {
			ctx = (CLSCTX)zval_get_long(tmp);
		}
	}

	if (server_name && !COMG(allow_dcom)) {
		php_com_throw_exception(E_ERROR, "DCOM has been disabled by your administrator [com.allow_dcom=0]");
		RETURN_THROWS();
	}

	moniker = php_com_string_to_olestring(module_name, module_name_len, obj->code_page);

	/* if instantiating a remote object, either directly, or via
	 * a moniker, fill in the relevant info */
	if (server_name) {
		info.dwReserved1 = 0;
		info.dwReserved2 = 0;
		info.pwszName = php_com_string_to_olestring(ZSTR_VAL(server_name), ZSTR_LEN(server_name), obj->code_page);

		if (user_name) {
			authid.User = (OLECHAR*) ZSTR_VAL(user_name);
			authid.UserLength = (ULONG) ZSTR_LEN(user_name);

			if (password) {
				authid.Password = (OLECHAR*) ZSTR_VAL(password);
				authid.PasswordLength = (ULONG) ZSTR_LEN(password);
			} else {
				authid.Password = (OLECHAR*) "";
				authid.PasswordLength = 0;
			}

			if (domain_name) {
				authid.Domain = (OLECHAR*) ZSTR_VAL(domain_name);
				authid.DomainLength = (ULONG) ZSTR_LEN(domain_name);
			} else {
				authid.Domain = (OLECHAR*) "";
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
		if (info.pwszName) efree(info.pwszName);
		if (server_params) zend_string_release(server_name);
	}
	if (user_name) zend_string_release(user_name);
	if (password) zend_string_release(password);
	if (domain_name) zend_string_release(domain_name);

	efree(moniker);

	if (FAILED(res)) {
		char *werr, *msg;

		werr = php_win32_error_to_msg(res);
		spprintf(&msg, 0, "Failed to create COM object `%s': %s", module_name, werr);
		php_win32_error_msg_free(werr);

		php_com_throw_exception(res, msg);
		efree(msg);
		RETURN_THROWS();
	}

	/* we got the object and it lives ! */

	/* see if it has TypeInfo available */
	if (FAILED(IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &obj->typeinfo)) && typelib_name) {
		/* load up the library from the named file */
		TL = php_com_load_typelib_via_cache(typelib_name, obj->code_page);

		if (TL) {
			if (COMG(autoreg_on)) {
				php_com_import_typelib(TL, mode, obj->code_page);
			}

			/* cross your fingers... there is no guarantee that this ITypeInfo
			 * instance has any relation to this IDispatch instance... */
			ITypeLib_GetTypeInfo(TL, 0, &obj->typeinfo);
			ITypeLib_Release(TL);
		}
	} else if (obj->typeinfo && COMG(autoreg_on)) {
		UINT idx;

		if (SUCCEEDED(ITypeInfo_GetContainingTypeLib(obj->typeinfo, &TL, &idx))) {
			/* check if the library is already in the cache by getting its name */
			BSTR name;

			if (SUCCEEDED(ITypeLib_GetDocumentation(TL, -1, &name, NULL, NULL, NULL))) {
				typelib_name = php_com_olestring_to_string(name, &typelib_name_len, obj->code_page);

				if (NULL != php_com_cache_typelib(TL, typelib_name, typelib_name_len)) {
					php_com_import_typelib(TL, mode, obj->code_page);

					/* add a reference for the hash */
					ITypeLib_AddRef(TL);
				}
				efree(typelib_name);
			} else {
				/* try it anyway */
				php_com_import_typelib(TL, mode, obj->code_page);
			}

			ITypeLib_Release(TL);
		}
	}

}
/* }}} */

/* {{{ Returns a handle to an already running instance of a COM object */
PHP_FUNCTION(com_get_active_object)
{
	CLSID clsid;
	char *module_name;
	size_t module_name_len;
	zend_long code_page;
	zend_bool code_page_is_null = 1;
	IUnknown *unk = NULL;
	IDispatch *obj = NULL;
	HRESULT res;
	OLECHAR *module = NULL;

	php_com_initialize();
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|l!",
				&module_name, &module_name_len, &code_page, &code_page_is_null)) {
		RETURN_THROWS();
	}

	if (code_page_is_null) {
		code_page = COMG(code_page);
	}

	module = php_com_string_to_olestring(module_name, module_name_len, (int)code_page);

	res = CLSIDFromString(module, &clsid);

	if (FAILED(res)) {
		php_com_throw_exception(res, NULL);
	} else {
		res = GetActiveObject(&clsid, NULL, &unk);

		if (FAILED(res)) {
			php_com_throw_exception(res, NULL);
		} else {
			res = IUnknown_QueryInterface(unk, &IID_IDispatch, &obj);

			if (FAILED(res)) {
				php_com_throw_exception(res, NULL);
			} else if (obj) {
				/* we got our dispatchable object */
				php_com_wrap_dispatch(return_value, obj, (int)code_page);
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
		WORD flags, DISPPARAMS *disp_params, VARIANT *v, int silent, int allow_noarg)
{
	HRESULT hr;
	unsigned int arg_err;
	EXCEPINFO e = {0};

	hr = IDispatch_Invoke(V_DISPATCH(&obj->v), id_member,
		&IID_NULL, LOCALE_SYSTEM_DEFAULT, flags, disp_params, v, &e, &arg_err);

	if (silent == 0 && FAILED(hr)) {
		char *source = NULL, *desc = NULL, *msg = NULL;
		size_t source_len, desc_len;

		switch (hr) {
			case DISP_E_EXCEPTION:
				if (e.bstrSource) {
					source = php_com_olestring_to_string(e.bstrSource, &source_len, obj->code_page);
					SysFreeString(e.bstrSource);
				}
				if (e.bstrDescription) {
					desc = php_com_olestring_to_string(e.bstrDescription, &desc_len, obj->code_page);
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
				php_win32_error_msg_free(desc);
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
				php_win32_error_msg_free(desc);
				break;
		}

		if (msg) {
			php_com_throw_exception(hr, msg);
			efree(msg);
		}
	}

	return hr;
}

/* map an ID to a name */
HRESULT php_com_get_id_of_name(php_com_dotnet_object *obj, char *name,
		size_t namelen, DISPID *dispid)
{
	OLECHAR *olename;
	HRESULT hr;
	zval *tmp;

	if (namelen == -1) {
		namelen = strlen(name);
	}

	if (obj->id_of_name_cache && NULL != (tmp = zend_hash_str_find(obj->id_of_name_cache, name, namelen))) {
		*dispid = (DISPID)Z_LVAL_P(tmp);
		return S_OK;
	}

	olename = php_com_string_to_olestring(name, namelen, obj->code_page);

	if (obj->typeinfo) {
		hr = ITypeInfo_GetIDsOfNames(obj->typeinfo, &olename, 1, dispid);
		if (FAILED(hr)) {
			HRESULT hr1 = hr;
			hr = IDispatch_GetIDsOfNames(V_DISPATCH(&obj->v), &IID_NULL, &olename, 1, LOCALE_SYSTEM_DEFAULT, dispid);
			if (SUCCEEDED(hr) && hr1 != E_NOTIMPL) {
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
		zval tmp;

		/* cache the mapping */
		if (!obj->id_of_name_cache) {
			ALLOC_HASHTABLE(obj->id_of_name_cache);
			zend_hash_init(obj->id_of_name_cache, 2, NULL, NULL, 0);
		}
		ZVAL_LONG(&tmp, *dispid);
		zend_hash_str_update(obj->id_of_name_cache, name, namelen, &tmp);
	}

	return hr;
}

/* the core of COM */
int php_com_do_invoke_byref(php_com_dotnet_object *obj, zend_internal_function *f,
		WORD flags,	VARIANT *v, int nargs, zval *args)
{
	DISPID dispid, altdispid;
	DISPPARAMS disp_params;
	HRESULT hr;
	VARIANT *vargs = NULL, *byref_vals = NULL;
	int i, byref_count = 0, j;

	/* assumption: that the active function (f) is the function we generated for the engine */
	if (!f) {
		return FAILURE;
	}

	hr = php_com_get_id_of_name(obj, f->function_name->val, f->function_name->len, &dispid);

	if (FAILED(hr)) {
		char *msg = NULL;
		char *winerr = php_win32_error_to_msg(hr);
		spprintf(&msg, 0, "Unable to lookup `%s': %s", f->function_name->val, winerr);
		php_win32_error_msg_free(winerr);
		php_com_throw_exception(hr, msg);
		efree(msg);
		return FAILURE;
	}


	if (nargs) {
		vargs = (VARIANT*)safe_emalloc(sizeof(VARIANT), nargs, 0);
	}

	if (f->arg_info) {
		for (i = 0; i < nargs; i++) {
			if (ZEND_ARG_SEND_MODE(&f->arg_info[nargs - i - 1])) {
				byref_count++;
			}
		}
	}

	if (byref_count) {
		byref_vals = (VARIANT*)safe_emalloc(sizeof(VARIANT), byref_count, 0);
		for (j = 0, i = 0; i < nargs; i++) {
			if (ZEND_ARG_SEND_MODE(&f->arg_info[nargs - i - 1])) {
				/* put the value into byref_vals instead */
				php_com_variant_from_zval(&byref_vals[j], &args[nargs - i - 1], obj->code_page);

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
				php_com_variant_from_zval(&vargs[i], &args[nargs - i - 1], obj->code_page);
			}
		}

	} else {
		/* Invoke'd args are in reverse order */
		for (i = 0; i < nargs; i++) {
			php_com_variant_from_zval(&vargs[i], &args[nargs - i - 1], obj->code_page);
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
	hr = php_com_invoke_helper(obj, dispid, flags, &disp_params, v, 0, 0);

	/* release variants */
	if (vargs) {
		if (f && f->arg_info) {
			for (i = 0, j = 0; i < nargs; i++) {
				/* if this was byref, update the zval */
				if (ZEND_ARG_SEND_MODE(&f->arg_info[nargs - i - 1])) {
					zval *arg = &args[nargs - i - 1];

					ZVAL_DEREF(arg);
					zval_ptr_dtor(arg);
					ZVAL_NULL(arg);

					/* if the variant is pointing at the byref_vals, we need to map
					 * the pointee value as a zval; otherwise, the value is pointing
					 * into an existing PHP variant record */
					if (V_VT(&vargs[i]) & VT_BYREF) {
						if (vargs[i].byref == &V_UINT(&byref_vals[j])) {
							/* copy that value */
							php_com_zval_from_variant(arg, &byref_vals[j], obj->code_page);
						}
					} else {
						/* not sure if this can ever happen; the variant we marked as BYREF
						 * is no longer BYREF - copy its value */
						php_com_zval_from_variant(arg, &vargs[i], obj->code_page);
					}
					VariantClear(&byref_vals[j]);
					j++;
				}
				VariantClear(&vargs[i]);
			}
		} else {
			for (i = 0, j = 0; i < nargs; i++) {
				VariantClear(&vargs[i]);
			}
		}
		efree(vargs);
		if (byref_vals) efree(byref_vals);
	}

	return SUCCEEDED(hr) ? SUCCESS : FAILURE;
}



int php_com_do_invoke_by_id(php_com_dotnet_object *obj, DISPID dispid,
		WORD flags,	VARIANT *v, int nargs, zval *args, int silent, int allow_noarg)
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
		php_com_variant_from_zval(&vargs[i], &args[nargs - i - 1], obj->code_page);
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
	hr = php_com_invoke_helper(obj, dispid, flags, &disp_params, v, silent, allow_noarg);

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

int php_com_do_invoke(php_com_dotnet_object *obj, char *name, size_t namelen,
		WORD flags,	VARIANT *v, int nargs, zval *args, int allow_noarg)
{
	DISPID dispid;
	HRESULT hr;
	char *msg = NULL;

	hr = php_com_get_id_of_name(obj, name, namelen, &dispid);

	if (FAILED(hr)) {
		char *winerr = php_win32_error_to_msg(hr);
		spprintf(&msg, 0, "Unable to lookup `%s': %s", name, winerr);
		php_win32_error_msg_free(winerr);
		php_com_throw_exception(hr, msg);
		efree(msg);
		return FAILURE;
	}

	return php_com_do_invoke_by_id(obj, dispid, flags, v, nargs, args, 0, allow_noarg);
}

/* {{{ Generate a globally unique identifier (GUID) */
PHP_FUNCTION(com_create_guid)
{
	GUID retval;
	OLECHAR *guid_string;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	php_com_initialize();
	if (CoCreateGuid(&retval) == S_OK && StringFromCLSID(&retval, &guid_string) == S_OK) {
		size_t len;
		char *str;

		str = php_com_olestring_to_string(guid_string, &len, CP_ACP);
		RETVAL_STRINGL(str, len);
		// TODO: avoid reallocation ???
		efree(str);

		CoTaskMemFree(guid_string);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Connect events from a COM object to a PHP object */
PHP_FUNCTION(com_event_sink)
{
	zval *object, *sinkobject;
	zend_string *sink_str = NULL;
	HashTable *sink_ht = NULL;
	char *dispname = NULL, *typelibname = NULL;
	php_com_dotnet_object *obj;
	ITypeInfo *typeinfo = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(object, php_com_variant_class_entry)
		Z_PARAM_OBJECT(sinkobject)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(sink_ht, sink_str)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	php_com_initialize();
	obj = CDNO_FETCH(object);

	if (sink_ht) {
		/* 0 => typelibname, 1 => dispname */
		zval *tmp;

		if ((tmp = zend_hash_index_find(sink_ht, 0)) != NULL && Z_TYPE_P(tmp) == IS_STRING)
			typelibname = Z_STRVAL_P(tmp);
		if ((tmp = zend_hash_index_find(sink_ht, 1)) != NULL && Z_TYPE_P(tmp) == IS_STRING)
			dispname = Z_STRVAL_P(tmp);
	} else if (sink_str) {
		dispname = ZSTR_VAL(sink_str);
	}

	typeinfo = php_com_locate_typeinfo(typelibname, obj, dispname, 1);

	if (typeinfo) {
		HashTable *id_to_name;

		ALLOC_HASHTABLE(id_to_name);

		if (php_com_process_typeinfo(typeinfo, id_to_name, 0, &obj->sink_id, obj->code_page)) {

			/* Create the COM wrapper for this sink */
			obj->sink_dispatch = php_com_wrapper_export_as_sink(sinkobject, &obj->sink_id, id_to_name);

			/* Now hook it up to the source */
			php_com_object_enable_event_sink(obj, TRUE);
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

/* {{{ Print out a PHP class definition for a dispatchable interface */
PHP_FUNCTION(com_print_typeinfo)
{
	zend_object *object_zpp;
	zend_string *typelib_name_zpp = NULL;
	char *ifacename = NULL;
	char *typelibname = NULL;
	size_t ifacelen;
	zend_bool wantsink = 0;
	php_com_dotnet_object *obj = NULL;
	ITypeInfo *typeinfo;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(object_zpp, php_com_variant_class_entry, typelib_name_zpp)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(ifacename, ifacelen)
		Z_PARAM_BOOL(wantsink)
	ZEND_PARSE_PARAMETERS_END();

	php_com_initialize();
	if (object_zpp) {
		obj = (php_com_dotnet_object*)object_zpp;
	} else {
		typelibname = ZSTR_VAL(typelib_name_zpp);
	}

	typeinfo = php_com_locate_typeinfo(typelibname, obj, ifacename, wantsink ? 1 : 0);
	if (typeinfo) {
		php_com_process_typeinfo(typeinfo, NULL, 1, NULL, obj ? obj->code_page : COMG(code_page));
		ITypeInfo_Release(typeinfo);
		RETURN_TRUE;
	}

	php_error_docref(NULL, E_WARNING, "Unable to find typeinfo using the parameters supplied");
	RETURN_FALSE;
}
/* }}} */

/* {{{ Process COM messages, sleeping for up to timeoutms milliseconds */
PHP_FUNCTION(com_message_pump)
{
	zend_long timeoutms = 0;
	MSG msg;
	DWORD result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &timeoutms) == FAILURE)
		RETURN_THROWS();

	php_com_initialize();
	result = MsgWaitForMultipleObjects(0, NULL, FALSE, (DWORD)timeoutms, QS_ALLINPUT);

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

/* {{{ Loads a Typelibrary and registers its constants */
PHP_FUNCTION(com_load_typelib)
{
	char *name;
	size_t namelen;
	ITypeLib *pTL = NULL;
	zend_bool cs = TRUE;
	int codepage = COMG(code_page);

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|b", &name, &namelen, &cs)) {
		RETURN_THROWS();
	}

	if (!cs) {
		php_error_docref(NULL, E_WARNING, "com_load_typelib(): Argument #2 ($case_insensitive) is ignored since declaration of case-insensitive constants is no longer supported");
	}

	RETVAL_FALSE;

	php_com_initialize();
	pTL = php_com_load_typelib_via_cache(name, codepage);
	if (pTL) {
		if (php_com_import_typelib(pTL, cs ? CONST_CS : 0, codepage) == SUCCESS) {
			RETVAL_TRUE;
		}

		ITypeLib_Release(pTL);
		pTL = NULL;
	}
}
/* }}} */
