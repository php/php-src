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

#if HAVE_MSCOREE_H
# include "php_ini.h"
# include "ext/standard/info.h"
# include "php_com_dotnet.h"
# include "php_com_dotnet_internal.h"
# include "Zend/zend_default_classes.h"
# include <mscoree.h>

struct dotnet_runtime_stuff {
	ICorRuntimeHost *dotnet_host;
	IDispatch *dotnet_domain;
	DISPID create_instance;
};

/* Since there is no official public mscorlib.h header file, and since
 * generating your own version from the binary .tlb file results in a 3MB
 * header file (!), we opt for the Dispatch-able approach.  This is slightly
 * slower for creating new objects, but not too bad */
static int dotnet_init(TSRMLS_D)
{
	HRESULT hr;
	struct dotnet_runtime_stuff *stuff;
	IUnknown *unk = NULL;
	OLECHAR *olename;

	stuff = malloc(sizeof(*stuff));
	memset(stuff, 0, sizeof(*stuff));

	if (SUCCEEDED(CoCreateInstance(&CLSID_CorRuntimeHost, NULL, CLSCTX_ALL,
			&IID_ICorRuntimeHost, (LPVOID*)&stuff->dotnet_host))) {

		/* fire up the host and get the domain object */
		if (SUCCEEDED(ICorRuntimeHost_Start(stuff->dotnet_host)) &&
				SUCCEEDED(ICorRuntimeHost_GetDefaultDomain(stuff->dotnet_host, &unk)) &&
				SUCCEEDED(IUnknown_QueryInterface(unk, &IID_IDispatch, (LPVOID*)&stuff->dotnet_domain))) {

			/* locate the create-instance member */
			olename = php_com_string_to_olestring("CreateInstance", sizeof("CreateInstance")-1, CP_ACP TSRMLS_CC);
			hr = IDispatch_GetIDsOfNames(stuff->dotnet_domain, &IID_NULL, &olename, 1, LOCALE_SYSTEM_DEFAULT, &stuff->create_instance);
			efree(olename);

			if (SUCCEEDED(hr)) {
				COMG(dotnet_runtime_stuff) = stuff;
			}
		}

		if (unk) {
			IUnknown_Release(unk);
		}
	}

	if (COMG(dotnet_runtime_stuff) == NULL) {
		/* clean up */
		if (stuff->dotnet_domain) {
			IDispatch_Release(stuff->dotnet_domain);
		}
		if (stuff->dotnet_host) {
			ICorRuntimeHost_Stop(stuff->dotnet_host);
			ICorRuntimeHost_Release(stuff->dotnet_host);
		}
		free(stuff);

		return FAILURE;
	}

	return SUCCESS;
}

/* {{{ com_dotnet_create_instance - ctor for DOTNET class */
PHP_FUNCTION(com_dotnet_create_instance)
{
	zval *object = getThis();
	php_com_dotnet_object *obj;
	char *assembly_name, *datatype_name;
	long assembly_name_len, datatype_name_len;
	struct dotnet_runtime_stuff *stuff;
	IObjectHandle *handle;
	DISPPARAMS params;
	VARIANT vargs[2];
	VARIANT retval;
	HRESULT hr;
	int ret = FAILURE;

	if (COMG(dotnet_runtime_stuff) == NULL) {
		if (FAILURE == dotnet_init(TSRMLS_C)) {
			php_com_throw_exception(E_ERROR, "Failed to initialize .Net runtime" TSRMLS_CC);
			ZVAL_NULL(object);
			return;
		}
	}

	stuff = (struct dotnet_runtime_stuff*)COMG(dotnet_runtime_stuff);

	obj = CDNO_FETCH(object);

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l",
			&assembly_name, &assembly_name_len,
			&datatype_name, &datatype_name_len,
			&obj->code_page)) {
		php_com_throw_exception(E_INVALIDARG, "Could not create .Net object - invalid arguments!" TSRMLS_CC);
		ZVAL_NULL(object);
		return;
	}

	params.cArgs = 2;
	params.cNamedArgs = 0;
	params.rgdispidNamedArgs = NULL;
	params.rgvarg = vargs;

	VariantInit(&vargs[0]);
	VariantInit(&vargs[1]);
	VariantInit(&retval);

	V_VT(&vargs[0]) = VT_BSTR;
	V_BSTR(&vargs[0]) = php_com_string_to_olestring(datatype_name, datatype_name_len, obj->code_page TSRMLS_CC);

	V_VT(&vargs[1]) = VT_BSTR;
	V_BSTR(&vargs[1]) = php_com_string_to_olestring(assembly_name, assembly_name_len, obj->code_page TSRMLS_CC);

	hr = IDispatch_Invoke(stuff->dotnet_domain, stuff->create_instance, &IID_NULL, LOCALE_SYSTEM_DEFAULT,
		DISPATCH_METHOD, &params, &retval, NULL, NULL);

	if (SUCCEEDED(hr)) {
		/* retval should now be an IUnknown/IDispatch representation of an IObjectHandle interface */
		if ((V_VT(&retval) == VT_UNKNOWN || V_VT(&retval) == VT_DISPATCH) &&
				SUCCEEDED(IUnknown_QueryInterface(V_UNKNOWN(&retval), &IID_IObjectHandle, &handle))) {
			VARIANT unwrapped;

			if (SUCCEEDED(IObjectHandle_Unwrap(handle, &unwrapped))) {
				/* unwrapped is now the dispatch pointer we want */
				V_DISPATCH(&obj->v) = V_DISPATCH(&unwrapped);
				V_VT(&obj->v) = VT_DISPATCH;

				/* get its type-info */
				IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &obj->typeinfo);

				ret = SUCCESS;
			}
			IObjectHandle_Release(handle);
		}
		VariantClear(&retval);
	}

	VariantClear(&vargs[0]);
	VariantClear(&vargs[1]);

	if (ret == FAILURE) {
		php_com_throw_exception(hr, "Failed to instantiate .Net object" TSRMLS_CC);
		ZVAL_NULL(object);
		return;
	}
}
/* }}} */

void php_com_dotnet_mshutdown(TSRMLS_D)
{
	struct dotnet_runtime_stuff *stuff = COMG(dotnet_runtime_stuff);
	
	if (stuff->dotnet_domain) {
		IDispatch_Release(stuff->dotnet_domain);
	}
	if (stuff->dotnet_host) {
		ICorRuntimeHost_Stop(stuff->dotnet_host);
		ICorRuntimeHost_Release(stuff->dotnet_host);
		stuff->dotnet_host = NULL;
	}
	free(stuff);
	COMG(dotnet_runtime_stuff) = NULL;
}

void php_com_dotnet_rshutdown(TSRMLS_D)
{
	struct dotnet_runtime_stuff *stuff = COMG(dotnet_runtime_stuff);
	
	if (stuff->dotnet_domain) {
		IDispatch_Release(stuff->dotnet_domain);
		stuff->dotnet_domain = NULL;
	}
}

#endif /* HAVE_MSCOREE_H */
