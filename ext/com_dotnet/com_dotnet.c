/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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

#if HAVE_MSCOREE_H
# include "php_ini.h"
# include "ext/standard/info.h"
# include "php_com_dotnet.h"
# include "php_com_dotnet_internal.h"
# include "Zend/zend_exceptions.h"
# include <mscoree.h>

/* Since there is no official public mscorlib.h header file, and since
 * generating your own version from the elusive binary .tlb file takes a lot of
 * hacking and results in a 3MB header file (!), we opt for this slightly
 * voodoo approach.  The following is just enough definition to be able to
 * reach the _AppDomain::CreateInstance method that we need to use to be able
 * to fire up .Net objects.  We used to use IDispatch for this, but it would
 * not always work.
 *
 * The following info was obtained using OleView to export the IDL from
 * mscorlib.tlb.  Note that OleView is unable to generate C headers for this
 * particular tlb... hence this mess.
 */

const GUID IID_mscorlib_System_AppDomain = {
0x05F696DC, 0x2B29, 0x3663, {0xAD, 0x8B, 0xC4, 0x38, 0x9C, 0xF2, 0xA7, 0x13 }};

typedef struct _Imscorlib_System_AppDomain IAppDomain;

struct _Imscorlib_System_AppDomainVtbl {
	BEGIN_INTERFACE

	HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
		IAppDomain * This,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void **ppvObject);

	ULONG ( STDMETHODCALLTYPE *AddRef )(
		IAppDomain * This);

	ULONG ( STDMETHODCALLTYPE *Release )(
		IAppDomain * This);

	/* this is padding to get CreateInstance into the correct position */
#define DUMMY_METHOD(x)		HRESULT ( STDMETHODCALLTYPE *dummy_##x )(IAppDomain *This)

	DUMMY_METHOD(GetTypeInfoCount);
	DUMMY_METHOD(GetTypeInfo);
	DUMMY_METHOD(GetIDsOfNames);
	DUMMY_METHOD(Invoke);
	DUMMY_METHOD(ToString);
	DUMMY_METHOD(Equals);
	DUMMY_METHOD(GetHashCode);
	DUMMY_METHOD(GetType);
	DUMMY_METHOD(InitializeLifetimeService);
	DUMMY_METHOD(GetLifetimeService);
	DUMMY_METHOD(Evidence);
	DUMMY_METHOD(add_DomainUnload);
	DUMMY_METHOD(remove_DomainUnload);
	DUMMY_METHOD(add_AssemblyLoad);
	DUMMY_METHOD(remove_AssemblyLoad);
	DUMMY_METHOD(add_ProcessExit);
	DUMMY_METHOD(remove_ProcessExit);
	DUMMY_METHOD(add_TypeResolve);
	DUMMY_METHOD(remove_TypeResolve);
	DUMMY_METHOD(add_ResourceResolve);
	DUMMY_METHOD(remove_ResourceResolve);
	DUMMY_METHOD(add_AssemblyResolve);
	DUMMY_METHOD(remove_AssemblyResolve);
	DUMMY_METHOD(add_UnhandledException);
	DUMMY_METHOD(remove_UnhandledException);
	DUMMY_METHOD(DefineDynamicAssembly);
	DUMMY_METHOD(DefineDynamicAssembly_2);
	DUMMY_METHOD(DefineDynamicAssembly_3);
	DUMMY_METHOD(DefineDynamicAssembly_4);
	DUMMY_METHOD(DefineDynamicAssembly_5);
	DUMMY_METHOD(DefineDynamicAssembly_6);
	DUMMY_METHOD(DefineDynamicAssembly_7);
	DUMMY_METHOD(DefineDynamicAssembly_8);
	DUMMY_METHOD(DefineDynamicAssembly_9);

	HRESULT ( STDMETHODCALLTYPE *CreateInstance )(IAppDomain * This, BSTR AssemblyName, BSTR typeName, IUnknown **pRetVal);
	HRESULT ( STDMETHODCALLTYPE *CreateInstanceFrom )(IAppDomain * This, BSTR AssemblyFile, BSTR typeName, IUnknown **pRetVal);

	/* more methods live here */

	END_INTERFACE
};

struct _Imscorlib_System_AppDomain {
	struct _Imscorlib_System_AppDomainVtbl *lpVtbl;
};


struct dotnet_runtime_stuff {
	ICorRuntimeHost *dotnet_host;
	IAppDomain 		*dotnet_domain;
	DISPID create_instance;
};

/* We link dynamically to mscoree.dll to avoid the hard dependency on .NET
 * framework, which is only required if a dotnet instance is to be created.
 */
static HRESULT dotnet_bind_runtime(LPVOID FAR *ppv)
{
	HRESULT hr;
	HMODULE mscoree;
	typedef HRESULT (STDAPICALLTYPE *cbtr_t)(LPCWSTR pwszVersion, LPCWSTR pwszBuildFlavor, REFCLSID rclsid, REFIID riid, LPVOID FAR *ppv);
	cbtr_t CorBindToRuntime;
	OLECHAR *oleversion;
	char *version;

	mscoree = LoadLibraryA("mscoree.dll");
	if (mscoree == NULL) {
		return S_FALSE;
	}

	CorBindToRuntime = (cbtr_t) GetProcAddress(mscoree, "CorBindToRuntime");
	if (CorBindToRuntime == NULL) {
		FreeLibrary(mscoree);
		return S_FALSE;
	}

	version = INI_STR("com.dotnet_version");
	if (version == NULL || *version == '\0') {
		oleversion = NULL;
	} else {
		oleversion = php_com_string_to_olestring(version, strlen(version), COMG(code_page));
	}

	hr = CorBindToRuntime(oleversion, NULL, &CLSID_CorRuntimeHost, &IID_ICorRuntimeHost, ppv);

	efree(oleversion);
	FreeLibrary(mscoree);

	return hr;
}

static HRESULT dotnet_init(char **p_where)
{
	HRESULT hr;
	struct dotnet_runtime_stuff *stuff;
	IUnknown *unk = NULL;
	char *where = "";

	stuff = malloc(sizeof(*stuff));
	if (!stuff) {
		return S_FALSE;
	}
	memset(stuff, 0, sizeof(*stuff));

	where = "dotnet_bind_runtime";
	hr = dotnet_bind_runtime((LPVOID*)&stuff->dotnet_host);
	if (FAILED(hr))
		goto out;

	/* fire up the host and get the domain object */
	where = "ICorRuntimeHost_Start\n";
	hr = ICorRuntimeHost_Start(stuff->dotnet_host);
	if (FAILED(hr))
		goto out;

	where = "ICorRuntimeHost_GetDefaultDomain";
	hr = ICorRuntimeHost_GetDefaultDomain(stuff->dotnet_host, &unk);
	if (FAILED(hr))
		goto out;

	where = "QI: System._AppDomain";
	hr = IUnknown_QueryInterface(unk, &IID_mscorlib_System_AppDomain, (LPVOID*)&stuff->dotnet_domain);
	if (FAILED(hr))
		goto out;

	COMG(dotnet_runtime_stuff) = stuff;

out:
	if (unk) {
		IUnknown_Release(unk);
	}
	if (COMG(dotnet_runtime_stuff) == NULL) {
		/* clean up */
		if (stuff->dotnet_domain) {
			IUnknown_Release(stuff->dotnet_domain);
		}
		if (stuff->dotnet_host) {
			ICorRuntimeHost_Stop(stuff->dotnet_host);
			ICorRuntimeHost_Release(stuff->dotnet_host);
		}
		free(stuff);

		*p_where = where;

		return hr;
	}

	return S_OK;
}

/* {{{ com_dotnet_create_instance - ctor for DOTNET class */
PHP_METHOD(dotnet, __construct)
{
	zval *object = getThis();
	php_com_dotnet_object *obj;
	char *assembly_name, *datatype_name;
	size_t assembly_name_len, datatype_name_len;
	struct dotnet_runtime_stuff *stuff;
	OLECHAR *oleassembly, *oletype;
	BSTR oleassembly_sys, oletype_sys;
	HRESULT hr;
	int ret = FAILURE;
	char *where = "";
	IUnknown *unk = NULL;
	zend_long cp = GetACP();
	const struct php_win32_cp *cp_it;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "ss|l",
			&assembly_name, &assembly_name_len,
			&datatype_name, &datatype_name_len,
			&cp)) {
		RETURN_THROWS();
	}

	php_com_initialize();
	stuff = (struct dotnet_runtime_stuff*)COMG(dotnet_runtime_stuff);
	if (stuff == NULL) {
		hr = dotnet_init(&where);
		if (FAILED(hr)) {
			char buf[1024];
			char *err = php_win32_error_to_msg(hr);
			snprintf(buf, sizeof(buf), "Failed to init .Net runtime [%s] %s", where, err);
			php_win32_error_msg_free(err);
			php_com_throw_exception(hr, buf);
			RETURN_THROWS();
		}
		stuff = (struct dotnet_runtime_stuff*)COMG(dotnet_runtime_stuff);

	} else if (stuff->dotnet_domain == NULL) {
		where = "ICorRuntimeHost_GetDefaultDomain";
		hr = ICorRuntimeHost_GetDefaultDomain(stuff->dotnet_host, &unk);
		if (FAILED(hr)) {
			char buf[1024];
			char *err = php_win32_error_to_msg(hr);
			snprintf(buf, sizeof(buf), "Failed to re-init .Net domain [%s] %s", where, err);
			php_win32_error_msg_free(err);
			php_com_throw_exception(hr, buf);
			ZVAL_NULL(object);
			RETURN_THROWS();
		}

		where = "QI: System._AppDomain";
		hr = IUnknown_QueryInterface(unk, &IID_mscorlib_System_AppDomain, (LPVOID*)&stuff->dotnet_domain);
		if (FAILED(hr)) {
			char buf[1024];
			char *err = php_win32_error_to_msg(hr);
			snprintf(buf, sizeof(buf), "Failed to re-init .Net domain [%s] %s", where, err);
			php_win32_error_msg_free(err);
			php_com_throw_exception(hr, buf);
			ZVAL_NULL(object);
			RETURN_THROWS();
		}
	}

	obj = CDNO_FETCH(object);

	cp_it = php_win32_cp_get_by_id((DWORD)cp);
	if (!cp_it) {
		php_com_throw_exception(E_INVALIDARG, "Could not create .Net object - invalid codepage!");
		RETURN_THROWS();
	}
	obj->code_page = (int)cp_it->id;

	oletype = php_com_string_to_olestring(datatype_name, datatype_name_len, obj->code_page);
	oleassembly = php_com_string_to_olestring(assembly_name, assembly_name_len, obj->code_page);
	oletype_sys = SysAllocString(oletype);
	oleassembly_sys = SysAllocString(oleassembly);
	where = "CreateInstance";
	hr = stuff->dotnet_domain->lpVtbl->CreateInstance(stuff->dotnet_domain, oleassembly_sys, oletype_sys, &unk);
	efree(oletype);
	efree(oleassembly);
	SysFreeString(oletype_sys);
	SysFreeString(oleassembly_sys);

	if (SUCCEEDED(hr)) {
		VARIANT unwrapped;
		IObjectHandle *handle = NULL;

		where = "QI: IObjectHandle";
		hr = IUnknown_QueryInterface(unk, &IID_IObjectHandle, &handle);

		if (SUCCEEDED(hr)) {
			where = "IObjectHandle_Unwrap";
			hr = IObjectHandle_Unwrap(handle, &unwrapped);
			if (SUCCEEDED(hr)) {

				if (V_VT(&unwrapped) == VT_UNKNOWN) {
					where = "Unwrapped, QI for IDispatch";
					hr = IUnknown_QueryInterface(V_UNKNOWN(&unwrapped), &IID_IDispatch, &V_DISPATCH(&obj->v));

					if (SUCCEEDED(hr)) {
						V_VT(&obj->v) = VT_DISPATCH;

						/* get its type-info */
						IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &obj->typeinfo);
						ret = SUCCESS;
					}
				} else if (V_VT(&unwrapped) == VT_DISPATCH) {
					/* unwrapped is now the dispatch pointer we want */
					V_DISPATCH(&obj->v) = V_DISPATCH(&unwrapped);
					V_VT(&obj->v) = VT_DISPATCH;

					/* get its type-info */
					IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &obj->typeinfo);

					ret = SUCCESS;
				} else {
					/* shouldn't happen, but let's be ready for it */
					VariantClear(&unwrapped);
					hr = E_INVALIDARG;
				}
			}
			IObjectHandle_Release(handle);
		}
		IUnknown_Release(unk);
	}

	if (ret == FAILURE) {
		char buf[1024];
		char *err = php_win32_error_to_msg(hr);
		snprintf(buf, sizeof(buf), "Failed to instantiate .Net object [%s] [0x%08x] %s", where, hr, err);
		php_win32_error_msg_free(err);
		php_com_throw_exception(hr, buf);
		RETURN_THROWS();
	}
}
/* }}} */

void php_com_dotnet_mshutdown(void)
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

void php_com_dotnet_rshutdown(void)
{
	struct dotnet_runtime_stuff *stuff = COMG(dotnet_runtime_stuff);

	if (stuff->dotnet_domain) {
		IDispatch_Release(stuff->dotnet_domain);
		stuff->dotnet_domain = NULL;
	}
}

#endif /* HAVE_MSCOREE_H */
