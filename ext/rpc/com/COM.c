/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
 */


/*
 * This module implements support for COM components that support the IDispatch
 * interface.  Both local (COM) and remote (DCOM) components can be accessed.
 *
 * Type libraries can be loaded (in order for PHP to recognize automation constants)
 * by specifying a typelib_file in the PHP .ini file.  That file should contain
 * paths to type libraries, one in every line.  By default, constants are registered
 * as case-sensitive.  If you want them to be defined as case-insensitive, add
 * #case_insensitive or #cis at the end of the type library path.
 *
 * This is also the first module to demonstrate Zend's OO syntax overloading
 * capabilities.  CORBA coders are invited to write a CORBA module as well!
 *
 * Zeev
 */

#ifdef PHP_WIN32

#define _WIN32_DCOM

#include "php.h"
#include "php_COM.h"
#include "zend_compile.h"
#include "php_ini.h"
#include "php_reentrancy.h"

#include "objbase.h"
#include "olestd.h" 
#include <ctype.h> 
 

static int le_idispatch;

static zend_class_entry com_class_entry;

function_entry COM_functions[] = {
	PHP_FE(COM_load,								NULL)
	PHP_FE(COM_invoke,								NULL)

	PHP_FE(com_propget,								NULL)
	PHP_FE(com_propput,								NULL)

	PHP_FALIAS(com_get,			com_propget,		NULL)
	PHP_FALIAS(com_propset,		com_propput,		NULL)
	PHP_FALIAS(com_set,			com_propput,		NULL)

	{NULL, NULL, NULL}
};

__declspec(dllexport)
int php_COM_get_le_idispatch() {
	return le_idispatch;
}

static PHP_MINFO_FUNCTION(COM)
{
	DISPLAY_INI_ENTRIES();
}


zend_module_entry COM_module_entry = {
	"com", COM_functions, PHP_MINIT(COM), PHP_MSHUTDOWN(COM), NULL, NULL, PHP_MINFO(COM), STANDARD_MODULE_PROPERTIES
};

void php_register_COM_class();

static int php_COM_load_typelib(char *typelib_name, int mode);

__declspec(dllexport)
char *php_COM_error_message(HRESULT hr)
{
	void *pMsgBuf;

	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
		hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &pMsgBuf, 0, NULL)) {
		char error_string[] = "No description available";
		
		pMsgBuf = LocalAlloc(LMEM_FIXED, sizeof(error_string));
		memcpy(pMsgBuf, error_string, sizeof(error_string));
	}

	return pMsgBuf;
}


static OLECHAR *php_char_to_OLECHAR(char *C_str, uint strlen)
{
	OLECHAR *unicode_str = (OLECHAR *) emalloc(sizeof(OLECHAR)*(strlen+1));
	OLECHAR *unicode_ptr = unicode_str;

	while (*C_str) {
		*unicode_ptr++ = (unsigned short) *C_str++;
	}
	*unicode_ptr = 0;

	return unicode_str;
}


char *php_OLECHAR_to_char(OLECHAR *unicode_str, uint *out_length, int persistent)
{
	uint length = OLESTRLEN(unicode_str);
	char *C_str = (char *) pemalloc(length+1, persistent), *p = C_str;

	while (*unicode_str) {
		*p++ = (char) *unicode_str++;
	}
	*p = 0;

	if (out_length) {
		*out_length = length;
	}

	return C_str;
}


static char *php_string_from_clsid(CLSID *clsid)
{
	LPOLESTR ole_clsid;
	char *clsid_str;

	StringFromCLSID(clsid, &ole_clsid);
	/*s_clsid = OLE2A(ole_clsid);*/
	clsid_str = php_OLECHAR_to_char(ole_clsid, NULL, 0);
	LocalFree(ole_clsid);

	return clsid_str;
}


static void php_idispatch_destructor(zend_rsrc_list_entry *rsrc)
{
	IDispatch *i_dispatch = (IDispatch *)rsrc->ptr;
	i_dispatch->lpVtbl->Release(i_dispatch);
}


static PHP_INI_MH(OnTypelibFileChange)
{
	FILE *typelib_file;
	char *typelib_name_buffer;
	char *strtok_buf = NULL;
#if SUPPORT_INTERACTIVE
	int interactive;
	ELS_FETCH();

	interactive = EG(interactive);
#endif


	if (!new_value || (typelib_file=V_FOPEN(new_value, "r"))==NULL) {
		return FAILURE;
	}

#if SUPPORT_INTERACTIVE
	if (interactive) {
		printf("Loading type libraries...");
		fflush(stdout);
	}
#endif

	typelib_name_buffer = (char *) malloc(sizeof(char)*1024);

	while (fgets(typelib_name_buffer, 1024, typelib_file)) {
		char *typelib_name;
		char *modifier;
		int mode = CONST_PERSISTENT|CONST_CS;

		if (typelib_name_buffer[0]==';') {
			continue;
		}
		typelib_name = php_strtok_r(typelib_name_buffer, "\r\n", &strtok_buf); /* get rid of newlines */
		typelib_name = php_strtok_r(typelib_name, "#", &strtok_buf);
		modifier = php_strtok_r(NULL, "#", &strtok_buf);
		if (modifier) {
			if (!strcmp(modifier, "cis") || !strcmp(modifier, "case_insensitive")) {
				mode &= ~CONST_CS;
			}
		}
#if SUPPORT_INTERACTIVE
		if (interactive) {
			printf("\rLoading %-60s\r", typelib_name);
		}
#endif
		php_COM_load_typelib(typelib_name, mode);
	}

	free(typelib_name_buffer);
	fclose(typelib_file);

#if SUPPORT_INTERACTIVE
	if (interactive) {
		printf("\r%70s\r", "");
	}
#endif

	return SUCCESS;
}


PHP_INI_BEGIN()
	PHP_INI_ENTRY1_EX("allow_dcom",		"0",		PHP_INI_SYSTEM,		NULL,					NULL,	php_ini_boolean_displayer_cb)
	PHP_INI_ENTRY1("typelib_file",		NULL,		PHP_INI_SYSTEM,		OnTypelibFileChange,	NULL)
PHP_INI_END()


PHP_MINIT_FUNCTION(COM)
{
	CoInitialize(NULL);
	le_idispatch = zend_register_list_destructors_ex(php_idispatch_destructor, NULL, "COM", module_number);
	php_register_COM_class();
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(COM)
{
	CoUninitialize();
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}


/* {{{ proto int com_load(string module_name)
   Loads a COM module */
PHP_FUNCTION(COM_load)
{
	pval *module_name, *server_name=NULL;
	CLSID clsid;
	HRESULT hr;
	OLECHAR *ProgID;
	IDispatch FAR *i_dispatch = NULL;
	char *error_message;
	char *clsid_str;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			getParameters(ht, 1, &module_name);
			break;
		case 2:
			if (!INI_INT("allow_dcom")) {
				php_error(E_WARNING, "DCOM is disabled");
				RETURN_FALSE;
			}
			getParameters(ht, 2, &module_name, &server_name);
			convert_to_string(server_name);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	convert_to_string(module_name);
	ProgID = php_char_to_OLECHAR(module_name->value.str.val, module_name->value.str.len);
	hr=CLSIDFromProgID(ProgID, &clsid);
	efree(ProgID);

	/* obtain CLSID */
	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);	
		php_error(E_WARNING,"Invalid ProgID:  %s\n", error_message);
		LocalFree(error_message);
		RETURN_FALSE;
	}

	/* obtain IDispatch */
	if (!server_name) {
		hr=CoCreateInstance(&clsid, NULL, CLSCTX_SERVER, &IID_IDispatch, (void **) &i_dispatch);
	} else {
		COSERVERINFO server_info;
		MULTI_QI pResults;

		server_info.dwReserved1=0;
		server_info.dwReserved2=0;
		server_info.pwszName = php_char_to_OLECHAR(server_name->value.str.val, server_name->value.str.len);
		server_info.pAuthInfo=NULL;

		pResults.pIID = &IID_IDispatch;
		pResults.pItf = NULL;
		pResults.hr = S_OK;
		hr=CoCreateInstanceEx(&clsid, NULL, CLSCTX_SERVER, &server_info, 1, &pResults);
		if (SUCCEEDED(hr)) {
			hr = pResults.hr;
			i_dispatch = (IDispatch *) pResults.pItf;
		}
		efree(server_info.pwszName);
	}
	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		clsid_str = php_string_from_clsid(&clsid);
		php_error(E_WARNING,"Unable to obtain IDispatch interface for CLSID %s:  %s",clsid_str,error_message);
		LocalFree(error_message);
		efree(clsid_str);
		RETURN_FALSE;
	}

	RETURN_LONG(zend_list_insert(i_dispatch,le_idispatch));
}
/* }}} */


static void php_variant_to_pval(VARIANTARG *var_arg, pval *pval_arg, int persistent)
{
	
	switch (var_arg->vt & ~VT_BYREF) {
		case VT_EMPTY:
			var_uninit(pval_arg);
			break;
		case VT_UI1:
			if (pval_arg->is_ref == 0 || (var_arg->vt & VT_BYREF) != VT_BYREF) {
				pval_arg->value.lval = (long) var_arg->bVal;
			} else {
				pval_arg->value.lval = (long)*(var_arg->pbVal);
			}
			pval_arg->type = IS_LONG;
			break;
		case VT_I2:
			if (pval_arg->is_ref == 0 || (var_arg->vt & VT_BYREF) != VT_BYREF) {
				pval_arg->value.lval = (long) var_arg->iVal;
			} else {
				pval_arg->value.lval = (long )*(var_arg->piVal);
			}
			pval_arg->type = IS_LONG;
			break;
		case VT_I4:
			if (pval_arg->is_ref == 0 || (var_arg->vt & VT_BYREF) != VT_BYREF) {
				pval_arg->value.lval = var_arg->lVal;
			} else {
				pval_arg->value.lval = *(var_arg->plVal);
			}
			pval_arg->type = IS_LONG;
			break;
		case VT_R4:
			if (pval_arg->is_ref == 0 || (var_arg->vt & VT_BYREF) != VT_BYREF) {
				pval_arg->value.dval = (double) var_arg->fltVal;
			} else {
				pval_arg->value.dval = (double)*(var_arg->pfltVal);
			}
			pval_arg->type = IS_DOUBLE;
			break;
		case VT_R8:
			if (pval_arg->is_ref == 0 || (var_arg->vt & VT_BYREF) != VT_BYREF) {
				pval_arg->value.dval = var_arg->dblVal;
			} else {
				pval_arg->value.dval = *(var_arg->pdblVal);
			}
			pval_arg->type = IS_DOUBLE;
			break;
		case VT_BOOL:
			if (pval_arg->is_ref == 0 || (var_arg->vt & VT_BYREF) != VT_BYREF) {
				if (var_arg->boolVal & 0xFFFF) {
					pval_arg->value.lval = 1;
				} else {
					pval_arg->value.lval = 0;
				}
			} else {
				if (*(var_arg->pboolVal) & 0xFFFF) {
					pval_arg->value.lval = 1;
				} else {
					pval_arg->value.lval = 0;
				}
			}
			pval_arg->type = IS_BOOL;
			break;
		case VT_BSTR:
			if (pval_arg->is_ref == 0  || (var_arg->vt & VT_BYREF) != VT_BYREF) {
				pval_arg->value.str.val = php_OLECHAR_to_char(var_arg->bstrVal, &pval_arg->value.str.len, persistent);
				SysFreeString(var_arg->bstrVal);
			} else {
				pval_arg->value.str.val = php_OLECHAR_to_char(*(var_arg->pbstrVal), &pval_arg->value.str.len, persistent);
				SysFreeString(*(var_arg->pbstrVal));
				efree(var_arg->pbstrVal);
			}
			pval_arg->type = IS_STRING;
			break;
		case VT_DATE: {
				SYSTEMTIME wintime;
				struct tm phptime;

				VariantTimeToSystemTime(var_arg->date, &wintime);
				phptime.tm_year = wintime.wYear-1900;
				phptime.tm_mon  = wintime.wMonth-1;
				phptime.tm_mday = wintime.wDay;
				phptime.tm_hour = wintime.wHour;
				phptime.tm_min  = wintime.wMinute;
				phptime.tm_sec  = wintime.wSecond;
				phptime.tm_isdst= -1;

				tzset();
				pval_arg->value.lval = mktime(&phptime);
				pval_arg->type = IS_LONG;
			}
			break;
		case VT_DISPATCH: {
				pval *handle;

				pval_arg->type=IS_OBJECT;
				pval_arg->value.obj.ce=&com_class_entry;
				pval_arg->value.obj.properties = (HashTable *) emalloc(sizeof(HashTable));
				pval_arg->is_ref=1;
				pval_arg->refcount=1;
				zend_hash_init(pval_arg->value.obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);

				ALLOC_ZVAL(handle);
				handle->type = IS_LONG;
				handle->value.lval = zend_list_insert(var_arg->pdispVal, le_idispatch);
				pval_copy_constructor(handle);
				INIT_PZVAL(handle);
				zend_hash_index_update(pval_arg->value.obj.properties, 0, &handle, sizeof(pval *), NULL);
			}
			break;
		case VT_UNKNOWN:
			var_arg->pdispVal->lpVtbl->Release(var_arg->pdispVal);
			/* fallthru */
		default:
			php_error(E_WARNING,"Unsupported variant type: %d (0x%X)", var_arg->vt, var_arg->vt);
			var_reset(pval_arg);
			break;
	}
}


static void php_pval_to_variant(pval *pval_arg, VARIANTARG *var_arg)
{
	OLECHAR *unicode_str;

	switch (pval_arg->type) {
	case IS_OBJECT:
	case IS_ARRAY:
		var_arg->vt = VT_EMPTY;
		break;
	case IS_LONG:
	case IS_BOOL:
		if (pval_arg->is_ref == 0) {
			var_arg->vt = VT_I4;	/* assuming 32-bit platform */
			var_arg->lVal = pval_arg->value.lval;
		} else {
			var_arg->vt = VT_I4 | VT_BYREF; /* assuming 32-bit platform */
			var_arg->plVal = &(pval_arg->value.lval);
		}
		break;
	case IS_DOUBLE:
		var_arg->vt = VT_R8;  /* assuming 64-bit double precision */
		var_arg->dblVal = pval_arg->value.dval;
		break;
	case IS_STRING:
		unicode_str = php_char_to_OLECHAR(pval_arg->value.str.val, pval_arg->value.str.len);
		if (pval_arg->is_ref == 0) {
			var_arg->bstrVal = SysAllocString(unicode_str);
			var_arg->vt = VT_BSTR;
		} else {
			var_arg->pbstrVal = (BSTR *)emalloc(sizeof(BSTR *));
			*(var_arg->pbstrVal) = SysAllocString(unicode_str);
			var_arg->vt = VT_BSTR | VT_BYREF;
			break;
		}
		efree(unicode_str);
	}
}


int do_COM_invoke(IDispatch *i_dispatch, pval *function_name, VARIANTARG *var_result, pval **arguments, int arg_count)
{
	DISPID dispid;
	HRESULT hr;
	OLECHAR *funcname;
	char *error_message;
	VARIANTARG *variant_args;
	int current_arg, current_variant;
	DISPPARAMS dispparams;

	funcname = php_char_to_OLECHAR(function_name->value.str.val, function_name->value.str.len);

	hr = i_dispatch->lpVtbl->GetIDsOfNames(i_dispatch, &IID_NULL, &funcname,
											1, LOCALE_SYSTEM_DEFAULT, &dispid);

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"Unable to lookup %s:  %s\n", function_name->value.str.val, error_message);
		LocalFree(error_message);
		efree(funcname);
		return FAILURE;
	}

	variant_args = (VARIANTARG *) emalloc(sizeof(VARIANTARG)*arg_count);

	for (current_arg=0; current_arg<arg_count; current_arg++) {
		current_variant = arg_count - current_arg - 1;
		php_pval_to_variant(arguments[current_arg], &variant_args[current_variant]);
	}


	dispparams.rgvarg = variant_args;
	dispparams.rgdispidNamedArgs = NULL;
	dispparams.cArgs = arg_count;
	dispparams.cNamedArgs = 0;

	hr = i_dispatch->lpVtbl->Invoke(i_dispatch, dispid, &IID_NULL,
									LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD|DISPATCH_PROPERTYGET,
									&dispparams, var_result, NULL, 0);

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"Invoke() failed:  %s\n", error_message);
		LocalFree(error_message);
		efree(funcname);
		efree(variant_args);
		return FAILURE;
	}

/*	variant_args = dispparams.rgvarg; */

	for (current_arg=0; current_arg<arg_count; current_arg++) {
		current_variant = arg_count - current_arg - 1;
		zval_dtor(arguments[current_arg]);
		php_variant_to_pval(&variant_args[current_variant], arguments[current_arg], 0);
	}


	efree(variant_args);
	efree(funcname);
	return SUCCESS;
}


/* {{{ proto mixed com_invoke(int module, string handler_name [, mixed arg [, ...]])
   Invokes a COM module */
PHP_FUNCTION(COM_invoke)
{
	pval **arguments;
	pval *object, *function_name;
	IDispatch *i_dispatch;
	int type;
	int arg_count = ZEND_NUM_ARGS();
	VARIANTARG var_result;

	if (arg_count<2) {
		WRONG_PARAM_COUNT;
	}
	arguments = (pval **) emalloc(sizeof(pval *)*arg_count);
	if (getParametersArray(ht, arg_count, arguments)==FAILURE) {
		RETURN_FALSE;
	}

	object = arguments[0];
	function_name = arguments[1];

	/* obtain i_dispatch interface */
	convert_to_long(object);
	i_dispatch = zend_list_find(object->value.lval, &type);
	if (!i_dispatch || (type!=le_idispatch)) {
		php_error(E_WARNING,"%d is not a COM object handler", function_name->value.str.val);
		RETURN_FALSE;
	}

	/* obtain property/method handler */
	convert_to_string(function_name);

	if (do_COM_invoke(i_dispatch, function_name, &var_result, arguments+2, arg_count-2)==FAILURE) {
		RETURN_FALSE;
	}
	efree(arguments);

	php_variant_to_pval(&var_result, return_value, 0);
}
/* }}} */



static int do_COM_offget(VARIANTARG *var_result, VARIANTARG *array, pval *arg_property, int cleanup)
{
	switch (array->vt) {
		case VT_DISPATCH:	{	/* a Collection, possibly */
			pval function_name;
			IDispatch *i_dispatch = array->pdispVal;
			int retval;

			function_name.value.str.val = "Item";
			function_name.value.str.len = 4;
			function_name.type = IS_STRING;
			retval = do_COM_invoke(i_dispatch, &function_name, var_result, &arg_property, 1);
			if (cleanup) {
				i_dispatch->lpVtbl->Release(i_dispatch);
			}
			return retval;
		}
	}
	return FAILURE;
}


static int do_COM_propget(VARIANTARG *var_result, IDispatch *i_dispatch, pval *arg_property, int cleanup)
{
	DISPID dispid;
	HRESULT hr;
	OLECHAR *propname;
	char *error_message;
	DISPPARAMS dispparams;


	/* obtain property handler */
	propname = php_char_to_OLECHAR(arg_property->value.str.val, arg_property->value.str.len);

	hr = i_dispatch->lpVtbl->GetIDsOfNames(i_dispatch, &IID_NULL, &propname,
											1, LOCALE_SYSTEM_DEFAULT, &dispid);

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"Unable to lookup %s:  %s\n", arg_property->value.str.val, error_message);
		LocalFree(error_message);
		efree(propname);
		if (cleanup) {
			i_dispatch->lpVtbl->Release(i_dispatch);
		}
		return FAILURE;
	}

	dispparams.cArgs = 0;
	dispparams.cNamedArgs = 0;

	hr = i_dispatch->lpVtbl->Invoke(i_dispatch, dispid, &IID_NULL,
									LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET,
									&dispparams, var_result, NULL, 0);

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"PropGet() failed:  %s\n", error_message);
		LocalFree(error_message);
		efree(propname);
		if (cleanup) {
			i_dispatch->lpVtbl->Release(i_dispatch);
		}
		return FAILURE;
	}

	efree(propname);
	if (cleanup) {
		i_dispatch->lpVtbl->Release(i_dispatch);
	}
	return SUCCESS;
}


static void do_COM_propput(pval *return_value, IDispatch *i_dispatch, pval *arg_property, pval *value)
{
	DISPID dispid;
	HRESULT hr;
	OLECHAR *propname;
	char *error_message;
	VARIANTARG var_result;
	DISPPARAMS dispparams;
	VARIANTARG new_value;
	DISPID mydispid = DISPID_PROPERTYPUT;


	/* obtain property handler */
	propname = php_char_to_OLECHAR(arg_property->value.str.val, arg_property->value.str.len);

	hr = i_dispatch->lpVtbl->GetIDsOfNames(i_dispatch, &IID_NULL, &propname,
											1, LOCALE_SYSTEM_DEFAULT, &dispid);

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"Unable to lookup %s:  %s\n", arg_property->value.str.val, error_message);
		LocalFree(error_message);
		efree(propname);
		RETURN_FALSE;
	}


	php_pval_to_variant(value, &new_value);
	dispparams.rgvarg = &new_value;
	dispparams.rgdispidNamedArgs = &mydispid;
	dispparams.cArgs = 1;
	dispparams.cNamedArgs = 1;

	hr = i_dispatch->lpVtbl->Invoke(i_dispatch, dispid, &IID_NULL,
									LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT,
									&dispparams, NULL, NULL, 0);
	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"PropPut() failed:  %s\n", error_message);
		LocalFree(error_message);
		efree(propname);
		RETURN_FALSE;
	}

	dispparams.cArgs = 0;
	dispparams.cNamedArgs = 0;

	hr = i_dispatch->lpVtbl->Invoke(i_dispatch, dispid, &IID_NULL,
									LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET,
									&dispparams, &var_result, NULL, 0);


	if (SUCCEEDED(hr)) {
		php_variant_to_pval(&var_result, return_value, 0);
	} else {
		*return_value = *value;
		zval_copy_ctor(return_value);
	}

	efree(propname);
}


/* {{{ proto mixed com_propget(int module, string property_name)
   Gets properties from a COM module */
PHP_FUNCTION(com_propget)
{
	pval *arg_idispatch, *arg_property;
	int type;
	IDispatch *i_dispatch;
	VARIANTARG var_result;

	if (ZEND_NUM_ARGS()!=2 || getParameters(ht, 2, &arg_idispatch, &arg_property)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* obtain i_dispatch interface */
	convert_to_long(arg_idispatch);
	/* obtain i_dispatch interface */
	i_dispatch = zend_list_find(arg_idispatch->value.lval,&type);
	if (!i_dispatch || (type!=le_idispatch)) {
		php_error(E_WARNING,"%d is not a COM object handler", arg_idispatch->value.lval);
	}	
	convert_to_string(arg_property);

	if (do_COM_propget(&var_result, i_dispatch, arg_property, 0)==FAILURE) {
		RETURN_FALSE;
	}
	php_variant_to_pval(&var_result, return_value, 0);
}
/* }}} */


/* {{{ proto bool com_propput(int module, string property_name, mixed value)
   Puts the properties for a module */
PHP_FUNCTION(com_propput)
{
	pval *arg_idispatch, *arg_property, *arg_value;
	int type;
	IDispatch *i_dispatch;

	if (ZEND_NUM_ARGS()!=3 || getParameters(ht, 3, &arg_idispatch, &arg_property, &arg_value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* obtain i_dispatch interface */
	convert_to_long(arg_idispatch);
	/* obtain i_dispatch interface */
	i_dispatch = zend_list_find(arg_idispatch->value.lval,&type);
	if (!i_dispatch || (type!=le_idispatch)) {
		php_error(E_WARNING,"%d is not a COM object handler", arg_idispatch->value.lval);
	}	
	convert_to_string(arg_property);

	do_COM_propput(return_value, i_dispatch, arg_property, arg_value);
}
/* }}} */


VARIANTARG _php_COM_get_property_handler(zend_property_reference *property_reference)
{
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	pval **idispatch_handle;
	pval *object = property_reference->object;
	IDispatch *i_dispatch;
	int type;
	VARIANTARG var_result;


	/* fetch the IDispatch interface */
	zend_hash_index_find(object->value.obj.properties, 0, (void **) &idispatch_handle);
	i_dispatch = zend_list_find((*idispatch_handle)->value.lval,&type);
	if (!i_dispatch || (type!=le_idispatch)) {
		var_result.vt = VT_EMPTY;
		return var_result;
	}

	var_result.vt = VT_DISPATCH;
	var_result.pdispVal = i_dispatch;

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case OE_IS_ARRAY:
				if (do_COM_offget(&var_result, &var_result, &overloaded_property->element, element!=property_reference->elements_list->head)==FAILURE) {
					var_result.vt = VT_EMPTY;
					return var_result;
				}
				/*printf("Array offset:  ");*/
				break;
			case OE_IS_OBJECT:
				if (var_result.vt != VT_DISPATCH) {
					var_result.vt = VT_EMPTY;
					return var_result;
				} else {
					if (do_COM_propget(&var_result, var_result.pdispVal, &overloaded_property->element, element!=property_reference->elements_list->head)==FAILURE) {
						var_result.vt = VT_EMPTY;
						return var_result;
					}
					/*printf("Object property:  ");*/
				}
				break;
			case OE_IS_METHOD:
				return var_result;
				break;
		}
		/*
		switch (overloaded_property->element.type) {
			case IS_LONG:
				printf("%d (numeric)\n", overloaded_property->element.value.lval);
				break;
			case IS_STRING:
				printf("'%s'\n", overloaded_property->element.value.str.val);
				break;
		}
		*/
		pval_destructor(&overloaded_property->element);
	}
	return var_result;
}


__declspec(dllexport)
pval php_COM_get_property_handler(zend_property_reference *property_reference)
{
	pval result;
	VARIANTARG var_result = _php_COM_get_property_handler(property_reference);

	php_variant_to_pval(&var_result, &result, 0);
	return result;
}


__declspec(dllexport)
int php_COM_set_property_handler(zend_property_reference *property_reference, pval *value)
{
	pval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	pval **idispatch_handle;
	pval *object = property_reference->object;
	IDispatch *i_dispatch;
	int type;
	VARIANTARG var_result;


	/* fetch the IDispatch interface */
	zend_hash_index_find(object->value.obj.properties, 0, (void **) &idispatch_handle);
	i_dispatch = zend_list_find((*idispatch_handle)->value.lval,&type);
	if (!i_dispatch || (type!=le_idispatch)) {
		return FAILURE;
	}
	var_result.vt = VT_DISPATCH;
	var_result.pdispVal = i_dispatch;

	for (element=property_reference->elements_list->head; element && element!=property_reference->elements_list->tail; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case OE_IS_ARRAY:
				/*printf("Array offset:  ");*/
				break;
			case OE_IS_OBJECT:
				if (var_result.vt != VT_DISPATCH) {
					return FAILURE;
				} else {
					do_COM_propget(&var_result, i_dispatch, &overloaded_property->element, element!=property_reference->elements_list->head);
					/*printf("Object property:  ");*/
				}
				break;
			case OE_IS_METHOD:
				/* this shouldn't happen */
				return FAILURE;
		}
		/*
		switch (overloaded_property->element.type) {
			case IS_LONG:
				printf("%d (numeric)\n", overloaded_property->element.valuepval_arglval);
				break;
			case IS_STRING:
				printf("'%s'\n", overloaded_property->element.value.str.val);
				break;
		}
		*/
		pval_destructor(&overloaded_property->element);
	}

	if (var_result.vt != VT_DISPATCH) {
		return FAILURE;
	}
	overloaded_property = (zend_overloaded_element *) element->data;
	do_COM_propput(&result, var_result.pdispVal, &overloaded_property->element, value);
	pval_destructor(&overloaded_property->element);
	return SUCCESS;
}



__declspec(dllexport)
void php_COM_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	zend_overloaded_element *overloaded_property;
	pval *object = property_reference->object;
	zend_overloaded_element *function_name = (zend_overloaded_element *) property_reference->elements_list->tail->data;

	if (zend_llist_count(property_reference->elements_list)==1
		&& !strcmp(function_name->element.value.str.val, "com")) { /* constructor */
		pval *object_handle;

		PHP_FN(COM_load)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
		if (!zend_is_true(return_value)) {
			var_reset(object);
			return;
		}
		ALLOC_ZVAL(object_handle);
		*object_handle = *return_value;
		pval_copy_constructor(object_handle);
		INIT_PZVAL(object_handle);
		zend_hash_index_update(object->value.obj.properties, 0, &object_handle, sizeof(pval *), NULL);
		pval_destructor(&function_name->element);
	} else {
		VARIANTARG object_handle = _php_COM_get_property_handler(property_reference);
		pval **arguments;
		int arg_count = ZEND_NUM_ARGS();
		VARIANTARG var_result;

		var_result.vt = VT_EMPTY;

		if (object_handle.vt != VT_DISPATCH) {
			/* that shouldn't happen */
			return;
		}
		arguments = (pval **) emalloc(sizeof(pval *)*arg_count);
		getParametersArray(ht, arg_count, arguments);

		if (do_COM_invoke((IDispatch *) object_handle.pdispVal, &function_name->element, &var_result, arguments, arg_count)==FAILURE) {
			RETVAL_FALSE;
		}
		pval_destructor(&function_name->element);
		efree(arguments);
		php_variant_to_pval(&var_result, return_value, 0);
	}

	for (overloaded_property = (zend_overloaded_element *) zend_llist_get_first(property_reference->elements_list);
	     overloaded_property;
	     overloaded_property = (zend_overloaded_element *) zend_llist_get_next(property_reference->elements_list)) {
		switch (overloaded_property->type) {
		case OE_IS_ARRAY:
			break;
		case OE_IS_OBJECT:
			break;
		case OE_IS_METHOD:

			break;
		}
	}
}


void php_register_COM_class()
{
	INIT_OVERLOADED_CLASS_ENTRY(com_class_entry, "COM", NULL,
								php_COM_call_function_handler,
								php_COM_get_property_handler,
								php_COM_set_property_handler);

	zend_register_internal_class(&com_class_entry);
}


static int php_COM_load_typelib(char *typelib_name, int mode)
{
	ITypeLib *TypeLib;
	ITypeComp *TypeComp;
	OLECHAR *p;
	int i;
	int interfaces;
	ELS_FETCH();

	p = php_char_to_OLECHAR(typelib_name, strlen(typelib_name));

	if (FAILED(LoadTypeLib(p, &TypeLib))) {
		efree(p);
		return FAILURE;
	}

	interfaces = TypeLib->lpVtbl->GetTypeInfoCount(TypeLib);

	TypeLib->lpVtbl->GetTypeComp(TypeLib, &TypeComp);
	for (i=0; i<interfaces; i++) {
		TYPEKIND pTKind;

		TypeLib->lpVtbl->GetTypeInfoType(TypeLib, i, &pTKind);
		if (pTKind==TKIND_ENUM) {
			ITypeInfo *TypeInfo;
			VARDESC *pVarDesc;
			UINT NameCount;
			int j;
#if 0
			BSTR bstr_EnumId;
			char *EnumId;

			TypeLib->lpVtbl->GetDocumentation(TypeLib, i, &bstr_EnumId, NULL, NULL, NULL);
			EnumId = php_OLECHAR_to_char(bstr_EnumId, NULL, 0);
			printf("Enumeration %d - %s:\n", i, EnumId);
			efree(EnumId);
#endif

			TypeLib->lpVtbl->GetTypeInfo(TypeLib, i, &TypeInfo);

			j=0;
			while (TypeInfo->lpVtbl->GetVarDesc(TypeInfo, j, &pVarDesc)==S_OK) {
				BSTR bstr_ids;
				char *ids;
				zend_constant c;

				TypeInfo->lpVtbl->GetNames(TypeInfo, pVarDesc->memid, &bstr_ids, 1, &NameCount);
				if (NameCount!=1) {
					j++;
					continue;
				}
				LocalFree(bstr_ids);
				ids = php_OLECHAR_to_char(bstr_ids, NULL, 1);
				c.name_len = strlen(ids)+1;
				c.name = ids;
				php_variant_to_pval(pVarDesc->lpvarValue, &c.value, 1);
				c.flags = mode;

				zend_register_constant(&c ELS_CC);
				/*printf("%s -> %ld\n", ids, pVarDesc->lpvarValue->lVal);*/
				j++;
			}
			TypeInfo->lpVtbl->Release(TypeInfo);
		}
	}


	TypeLib->lpVtbl->Release(TypeLib);
	efree(p);
	return SUCCESS;
}

#endif
