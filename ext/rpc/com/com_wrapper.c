/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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
   |         Harald Radi  <h.radi@nme.at>
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

/*
 * 28.12.2000
 * unicode conversion fixed by Harald Radi <h.radi@nme.at>
 *
 * now all these strange '?'s should be disapeared
 */

/*
 * 28.1.2001
 * VARIANT datatype and pass_by_reference support
 */

#ifdef PHP_WIN32

#define _WIN32_DCOM

#include <iostream.h>
#include <math.h>

#include "php.h"
#include "php_ini.h"
#include "com.h"

#include "conversion.h"

zend_class_entry com_class_entry;

PHP_FUNCTION(com_load);
PHP_FUNCTION(com_invoke);
PHP_FUNCTION(com_propget);
PHP_FUNCTION(com_propput);

static int le_idispatch;
static int codepage;

function_entry COM_functions[] = {
	PHP_FE(com_load,								NULL)
	PHP_FE(com_invoke,								NULL)

	PHP_FE(com_propget,								NULL)
	PHP_FE(com_propput,								NULL)

	PHP_FALIAS(com_get,			com_propget,		NULL)
	PHP_FALIAS(com_propset,		com_propput,		NULL)
	PHP_FALIAS(com_set,			com_propput,		NULL)

	{NULL, NULL, NULL}
};


static PHP_MINFO_FUNCTION(COM)
{
	DISPLAY_INI_ENTRIES();
}

static int php_COM_load_typelib(char *typelib_name, int mode);

PHPAPI HRESULT php_COM_invoke(i_dispatch *obj, DISPID dispIdMember, WORD wFlags, DISPPARAMS FAR*  pDispParams, VARIANT FAR*  pVarResult)
{
/* TODO: doesn't work
	if(obj->typelib) {
		return obj->i.typeinfo->lpVtbl->Invoke(obj->i.typeinfo, obj->i.dispatch, dispIdMember,
											   wFlags, pDispParams, pVarResult, NULL, NULL);
	} else {
		return obj->i.dispatch->lpVtbl->Invoke(obj->i.dispatch, dispIdMember, &IID_NULL, LOCALE_SYSTEM_DEFAULT,
											   wFlags, pDispParams, pVarResult, NULL, NULL);
	}*/
		return obj->i.dispatch->lpVtbl->Invoke(obj->i.dispatch, dispIdMember, &IID_NULL, LOCALE_SYSTEM_DEFAULT,
											   wFlags, pDispParams, pVarResult, NULL, NULL);
}

PHPAPI HRESULT php_COM_get_ids_of_names(i_dispatch *obj, OLECHAR FAR* FAR* rgszNames, DISPID FAR* rgDispId)
{
/* TODO: doesn't work
	if(obj->typelib) {
		return obj->i.typeinfo->lpVtbl->GetIDsOfNames(obj->i.typeinfo, rgszNames, 1, rgDispId);
	} else {
		return obj->i.dispatch->lpVtbl->GetIDsOfNames(obj->i.dispatch, &IID_NULL, rgszNames, 1, LOCALE_SYSTEM_DEFAULT, rgDispId);
	}*/
		return obj->i.dispatch->lpVtbl->GetIDsOfNames(obj->i.dispatch, &IID_NULL, rgszNames, 1, LOCALE_SYSTEM_DEFAULT, rgDispId);
}

PHPAPI HRESULT php_COM_release(i_dispatch *obj)
{
	HRESULT hr;
	
	hr = obj->i.dispatch->lpVtbl->Release(obj->i.dispatch);
	obj->i.dispatch = NULL;
	obj->i.typeinfo = NULL;
	obj->typelib = FALSE;

	return hr;
}

PHPAPI HRESULT php_COM_set(i_dispatch *obj, IDispatch FAR* pDisp, int cleanup)
{
	HRESULT hr;

	if((obj->i.dispatch = pDisp) == NULL)
	{
		php_error(E_ERROR, "NULL pointer exception");
	}
	else
	{
		obj->typelib = !FAILED(obj->i.dispatch->lpVtbl->GetTypeInfo(obj->i.dispatch, 0, LANG_NEUTRAL, &(obj->i.typeinfo)));

		if(cleanup) {
			pDisp = NULL;
		} else {
			hr = obj->i.dispatch->lpVtbl->AddRef(obj->i.dispatch);
		}
	}

	return hr;
}

PHPAPI HRESULT php_COM_clone(i_dispatch *obj, i_dispatch *clone, int cleanup)
{
	HRESULT hr;

	obj->typelib = clone->typelib;
	obj->i.dispatch = clone->i.dispatch;
	obj->i.typeinfo = clone->i.typeinfo;

	if(cleanup) {
		obj->i.dispatch = NULL;
		obj->i.typeinfo = NULL;
		obj->typelib = FALSE;
	} else {
		hr = obj->i.dispatch->lpVtbl->AddRef(obj->i.dispatch);
	}

	return hr;
}

PHPAPI char *php_COM_error_message(HRESULT hr)
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

static char *php_string_from_clsid(const CLSID *clsid)
{
	LPOLESTR ole_clsid;
	char *clsid_str;

	StringFromCLSID(clsid, &ole_clsid);
	clsid_str = php_OLECHAR_to_char(ole_clsid, NULL, 0, codepage);
	LocalFree(ole_clsid);

	return clsid_str;
}

static void php_idispatch_destructor(zend_rsrc_list_entry *rsrc)
{
	i_dispatch *obj = (i_dispatch *)rsrc->ptr;
	php_COM_release(obj);
	efree(obj);
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


	if (!new_value || (typelib_file=VCWD_FOPEN(new_value, "r"))==NULL) {
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
	PHP_INI_ENTRY_EX("com.allow_dcom", "0", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
	PHP_INI_ENTRY("com.typelib_file", "", PHP_INI_SYSTEM, OnTypelibFileChange)
PHP_INI_END()


/* {{{ proto int com_load(string module_name [, string remote_host [, int codepage]])
   Loads a COM module */
PHP_FUNCTION(com_load)
{
	pval *module_name, *server_name=NULL, *code_page;
	CLSID clsid;
	HRESULT hr;
	OLECHAR *ProgID;
	i_dispatch *obj;
	char *error_message;
	char *clsid_str;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			getParameters(ht, 1, &module_name);
			codepage = CP_ACP;
			break;
		case 2:
			if (!INI_INT("com.allow_dcom")) {
				php_error(E_WARNING, "DCOM is disabled");
				RETURN_FALSE;
			}
			getParameters(ht, 2, &module_name, &server_name);
			convert_to_string(server_name);
			codepage = CP_ACP;
			break;
		case 3:
			getParameters(ht, 3, &module_name, &server_name, &code_page);

			if (server_name->type == IS_NULL) {
				efree(server_name);
				server_name = NULL;
			} else {
				if (!INI_INT("com.allow_dcom")) {
					php_error(E_WARNING, "DCOM is disabled");
					RETURN_FALSE;
				}
				convert_to_string(server_name);
			}

			convert_to_long(code_page);
			codepage = code_page->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	convert_to_string(module_name);
	ProgID = php_char_to_OLECHAR(module_name->value.str.val, module_name->value.str.len, codepage);
	hr=CLSIDFromProgID(ProgID, &clsid);
	efree(ProgID);

	/* obtain CLSID */
	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);	
		php_error(E_WARNING,"Invalid ProgID:  %s\n", error_message);
		LocalFree(error_message);
		RETURN_FALSE;
	}

	obj = (i_dispatch *) emalloc(sizeof(i_dispatch));

	/* obtain IDispatch */
	if (!server_name) {
		hr = CoCreateInstance(&clsid, NULL, CLSCTX_SERVER, &IID_IDispatch, (LPVOID *) &(obj->i.dispatch));
		php_COM_set(obj, obj->i.dispatch, TRUE);
	} else {
		COSERVERINFO server_info;
		MULTI_QI pResults;

		server_info.dwReserved1=0;
		server_info.dwReserved2=0;
		server_info.pwszName = php_char_to_OLECHAR(server_name->value.str.val, server_name->value.str.len, codepage);
		server_info.pAuthInfo=NULL;

		pResults.pIID = &IID_IDispatch;
		pResults.pItf = NULL;
		pResults.hr = S_OK;
		hr=CoCreateInstanceEx(&clsid, NULL, CLSCTX_SERVER, &server_info, 1, &pResults);
		if (SUCCEEDED(hr)) {
			hr = pResults.hr;
			obj->i.dispatch = (IDispatch *) pResults.pItf;
			php_COM_set(obj, obj->i.dispatch, TRUE);
		}
		efree(server_info.pwszName);
	}

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		clsid_str = php_string_from_clsid(&clsid);
		php_error(E_WARNING,"Unable to obtain IDispatch interface for CLSID %s:  %s",clsid_str,error_message);
		LocalFree(error_message);
		efree(clsid_str);
		efree(obj);
		RETURN_FALSE;
	}

	RETURN_LONG(zend_list_insert(obj, le_idispatch));
}
/* }}} */


int do_COM_invoke(i_dispatch *obj, pval *function_name, VARIANTARG *var_result, pval **arguments, int arg_count)
{
	DISPID dispid;
	HRESULT hr;
	OLECHAR *funcname;
	char *error_message;
	VARIANTARG *variant_args;
	int current_arg, current_variant;
	DISPPARAMS dispparams;

	funcname = php_char_to_OLECHAR(function_name->value.str.val, function_name->value.str.len, codepage);

	hr = php_COM_get_ids_of_names(obj, &funcname, &dispid);

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
		php_pval_to_variant(arguments[current_arg], &variant_args[current_variant], codepage);
	}

	dispparams.rgvarg = variant_args;
	dispparams.rgdispidNamedArgs = NULL;
	dispparams.cArgs = arg_count;
	dispparams.cNamedArgs = 0;

	hr = php_COM_invoke(obj, dispid, DISPATCH_METHOD|DISPATCH_PROPERTYGET, &dispparams, var_result);

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"Invoke() failed:  %s\n", error_message);
		LocalFree(error_message);
		efree(funcname);
		efree(variant_args);
		return FAILURE;
	}

	efree(variant_args);
	efree(funcname);
	return SUCCESS;
}


/* {{{ proto mixed com_invoke(int module, string handler_name [, mixed arg [, ...]])
   Invokes a COM module */
PHP_FUNCTION(com_invoke)
{
	pval **arguments;
	pval *object, *function_name;
	i_dispatch *obj;
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

	/* obtain IDispatch interface */
	convert_to_long(object);
	obj = (i_dispatch *)zend_list_find(object->value.lval, &type);
	if (!obj || (type!=le_idispatch)) {
		php_error(E_WARNING,"%d is not a COM object handler", function_name->value.str.val);
		RETURN_FALSE;
	}

	/* obtain property/method handler */
	convert_to_string(function_name);

	if (do_COM_invoke(obj, function_name, &var_result, arguments+2, arg_count-2)==FAILURE) {
		RETURN_FALSE;
	}
	efree(arguments);

	php_variant_to_pval(&var_result, return_value, 0, codepage);
}
/* }}} */

static int do_COM_offget(VARIANTARG *var_result, VARIANTARG *array, pval *arg_property, int cleanup)
{
	switch (array->vt) {
		case VT_DISPATCH:	{	/* a Collection, possibly */
			pval function_name;
			i_dispatch *obj;
			int retval;

			obj = (i_dispatch *) emalloc(sizeof(i_dispatch));
			php_COM_set(obj, array->pdispVal, TRUE);

			function_name.value.str.val = "Item";
			function_name.value.str.len = 4;
			function_name.type = IS_STRING;
			retval = do_COM_invoke(obj, &function_name, var_result, &arg_property, 1);
			if (cleanup) {
				php_COM_release(obj);
			}
			efree(obj);

			return retval;
		}
	}
	return FAILURE;
}


static int do_COM_propget(VARIANTARG *var_result, i_dispatch *obj, pval *arg_property, int cleanup)
{
	DISPID dispid;
	HRESULT hr;
	OLECHAR *propname;
	char *error_message;
	DISPPARAMS dispparams;


	/* obtain property handler */
	propname = php_char_to_OLECHAR(arg_property->value.str.val, arg_property->value.str.len, codepage);

	hr = php_COM_get_ids_of_names(obj, &propname, &dispid);

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"Unable to lookup %s:  %s\n", arg_property->value.str.val, error_message);
		LocalFree(error_message);
		efree(propname);
		if (cleanup) {
			php_COM_release(obj);
		}
		return FAILURE;
	}

	dispparams.cArgs = 0;
	dispparams.cNamedArgs = 0;

	hr = php_COM_invoke(obj, dispid, DISPATCH_PROPERTYGET, &dispparams, var_result);

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"PropGet() failed:  %s\n", error_message);
		LocalFree(error_message);
		efree(propname);
		if (cleanup) {
			php_COM_release(obj);
		}
		return FAILURE;
	}

	efree(propname);
	if (cleanup) {
		php_COM_release(obj);
	}
	return SUCCESS;
}


static void do_COM_propput(pval *return_value, i_dispatch *obj, pval *arg_property, pval *value)
{
	DISPID dispid;
	HRESULT hr;
	OLECHAR *propname;
	char *error_message;
	VARIANT *var_result;
	DISPPARAMS dispparams;
	VARIANTARG new_value;
	DISPID mydispid = DISPID_PROPERTYPUT;


	var_result = emalloc(sizeof(VARIANT));
	
	/* obtain property handler */
	propname = php_char_to_OLECHAR(arg_property->value.str.val, arg_property->value.str.len, codepage);

	hr = php_COM_get_ids_of_names(obj, &propname, &dispid);

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"Unable to lookup %s:  %s\n", arg_property->value.str.val, error_message);
		LocalFree(error_message);
		efree(propname);
		RETURN_FALSE;
	}

	php_pval_to_variant(value, &new_value, codepage);
	dispparams.rgvarg = &new_value;
	dispparams.rgdispidNamedArgs = &mydispid;
	dispparams.cArgs = 1;
	dispparams.cNamedArgs = 1;

	hr = php_COM_invoke(obj, dispid, DISPATCH_PROPERTYPUT, &dispparams, NULL);

	if (FAILED(hr)) {
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"PropPut() failed:  %s\n", error_message);
		LocalFree(error_message);
		efree(propname);
		RETURN_FALSE;
	}

	dispparams.cArgs = 0;
	dispparams.cNamedArgs = 0;

	hr = php_COM_invoke(obj, dispid, DISPATCH_PROPERTYGET, &dispparams, var_result);

	if (SUCCEEDED(hr)) {
		php_variant_to_pval(var_result, return_value, 0, codepage);
	} else {
		*return_value = *value;
		zval_copy_ctor(return_value);
	}

	efree(var_result);
	efree(propname);
}


/* {{{ proto mixed com_propget(int module, string property_name)
   Gets properties from a COM module */
PHP_FUNCTION(com_propget)
{
	pval *arg_idispatch, *arg_property;
	int type;
	i_dispatch *obj;
	VARIANTARG var_result;

	if (ZEND_NUM_ARGS()!=2 || getParameters(ht, 2, &arg_idispatch, &arg_property)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* obtain IDispatch interface */
	convert_to_long(arg_idispatch);
	obj = (i_dispatch *)zend_list_find(arg_idispatch->value.lval,&type);
	if (!obj || (type!=le_idispatch)) {
		php_error(E_WARNING,"%d is not a COM object handler", arg_idispatch->value.lval);
	}	
	convert_to_string(arg_property);

	if (do_COM_propget(&var_result, obj, arg_property, 0)==FAILURE) {
		RETURN_FALSE;
	}
	php_variant_to_pval(&var_result, return_value, 0, codepage);
}
/* }}} */


/* {{{ proto bool com_propput(int module, string property_name, mixed value)
   Puts the properties for a module */
PHP_FUNCTION(com_propput)
{
	pval *arg_idispatch, *arg_property, *arg_value;
	int type;
	i_dispatch *obj;

	if (ZEND_NUM_ARGS()!=3 || getParameters(ht, 3, &arg_idispatch, &arg_property, &arg_value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* obtain i_dispatch interface */
	convert_to_long(arg_idispatch);
	/* obtain i_dispatch interface */
	obj = (i_dispatch *)zend_list_find(arg_idispatch->value.lval,&type);
	if (!obj || (type!=le_idispatch)) {
		php_error(E_WARNING,"%d is not a COM object handler", arg_idispatch->value.lval);
	}	
	convert_to_string(arg_property);

	do_COM_propput(return_value, obj, arg_property, arg_value);
}
/* }}} */


VARIANT *_php_COM_get_property_handler(zend_property_reference *property_reference)
{
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	pval **idispatch_handle;
	pval *object = property_reference->object;
	int type;
	VARIANT *var_result;
	i_dispatch *obj, *obj_prop;


	/* fetch the IDispatch interface */
	zend_hash_index_find(object->value.obj.properties, 0, (void **) &idispatch_handle);
	obj = (i_dispatch *) zend_list_find((*idispatch_handle)->value.lval, &type);
	if (!obj || (type!=le_idispatch)) {
		return NULL;
	}

	obj_prop = (i_dispatch *) emalloc(sizeof(i_dispatch));
	php_COM_clone(obj_prop, obj, FALSE);

	//leak !!!
	var_result = (VARIANT *) emalloc(sizeof(VARIANT));
	var_result->vt = VT_DISPATCH;
	var_result->pdispVal = obj_prop->i.dispatch;

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case OE_IS_ARRAY:
				if (do_COM_offget(var_result, var_result, &overloaded_property->element, element!=property_reference->elements_list->head)==FAILURE) {
					var_result->vt = VT_EMPTY;
					efree(obj_prop);
					return var_result;
				}
				break;

			case OE_IS_OBJECT:
				if (var_result->vt != VT_DISPATCH) {
					var_result->vt = VT_EMPTY;
					efree(obj_prop);
					return var_result;
				} else {
					php_COM_set(obj_prop, var_result->pdispVal, TRUE);
					if (do_COM_propget(var_result, obj_prop, &overloaded_property->element, element!=property_reference->elements_list->head)==FAILURE) {
						var_result->vt = VT_EMPTY;
						efree(obj_prop);
						return var_result;
					}
					
				}
				break;

			case OE_IS_METHOD:
				efree(obj_prop);
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
	efree(obj_prop);
	return var_result;
}

PHPAPI pval php_COM_get_property_handler(zend_property_reference *property_reference)
{
	pval result;
	VARIANT *var_result = _php_COM_get_property_handler(property_reference);

	php_variant_to_pval(var_result, &result, 0, codepage);
	efree(var_result);
	return result;
}


PHPAPI int php_COM_set_property_handler(zend_property_reference *property_reference, pval *value)
{
	pval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	pval **idispatch_handle;
	pval *object = property_reference->object;
	i_dispatch *obj;
	int type;
	VARIANTARG var_result;


	/* fetch the IDispatch interface */
	zend_hash_index_find(object->value.obj.properties, 0, (void **) &idispatch_handle);
	obj = (i_dispatch *)zend_list_find((*idispatch_handle)->value.lval,&type);
	if (!obj || (type!=le_idispatch)) {
		return FAILURE;
	}
	var_result.vt = VT_DISPATCH;
	var_result.pdispVal = obj->i.dispatch;

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
					// ??????
					do_COM_propget(&var_result, obj, &overloaded_property->element, element!=property_reference->elements_list->head);
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
	obj = (i_dispatch *) emalloc(sizeof(i_dispatch));
	obj->typelib = FALSE;
	obj->i.dispatch = var_result.pdispVal;

	overloaded_property = (zend_overloaded_element *) element->data;
	do_COM_propput(&result, obj, &overloaded_property->element, value);
	pval_destructor(&overloaded_property->element);
	efree(obj);

	return SUCCESS;
}



PHPAPI void php_COM_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	zend_overloaded_element *overloaded_property;
	pval *object = property_reference->object;
	zend_overloaded_element *function_name = (zend_overloaded_element *) property_reference->elements_list->tail->data;

	if (zend_llist_count(property_reference->elements_list)==1
		&& !strcmp(function_name->element.value.str.val, "com")) { /* constructor */
		pval *object_handle;

		PHP_FN(com_load)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
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
		i_dispatch *obj;
		pval **arguments;
		int arg_count = ZEND_NUM_ARGS();
		VARIANT *var_result;

		obj = (i_dispatch *) emalloc(sizeof(i_dispatch));

		var_result = _php_COM_get_property_handler(property_reference);
		php_COM_set(obj, var_result->pdispVal, TRUE);

		var_result->vt = VT_EMPTY;
 
		arguments = (pval **) emalloc(sizeof(pval *)*arg_count);
		getParametersArray(ht, arg_count, arguments);
			
		if (do_COM_invoke(obj , &function_name->element, var_result, arguments, arg_count)==FAILURE) {
			RETVAL_FALSE;
		}
		
		pval_destructor(&function_name->element);
		php_COM_release(obj);
		php_variant_to_pval(var_result, return_value, 0, codepage);
		efree(obj);
		efree(var_result);
		efree(arguments);
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


static int php_COM_load_typelib(char *typelib_name, int mode)
{
	ITypeLib *TypeLib;
	ITypeComp *TypeComp;
	OLECHAR *p;
	CLSID clsid;
	char *strtok_buf, *major, *minor;
	int i;
	int interfaces;
	ELS_FETCH();


	typelib_name = php_strtok_r(typelib_name, ",", &strtok_buf);
	major = php_strtok_r(NULL, ",", &strtok_buf);
	minor = php_strtok_r(NULL, ",", &strtok_buf);
	
	p = php_char_to_OLECHAR(typelib_name, strlen(typelib_name), codepage);

	if (!FAILED(CLSIDFromString(p, &clsid))) {
		HRESULT hr;

		if (major && minor) {
			hr = LoadRegTypeLib((REFGUID) &clsid, 1, 0, LANG_NEUTRAL, &TypeLib);
		}
		
		if (!major || !minor || FAILED(hr)) {
			IDispatch *i_dispatch;
			ITypeInfo *TypeInfo;
			int idx;

			if (FAILED(CoCreateInstance(&clsid, NULL, CLSCTX_SERVER, &IID_IDispatch, (LPVOID *) &i_dispatch))) {
				efree(p);
				return FAILURE;
			}
			if (FAILED(i_dispatch->lpVtbl->GetTypeInfo(i_dispatch, 0, LANG_NEUTRAL, &TypeInfo))) {
				efree(p);
				return FAILURE;
			}
			if (FAILED(TypeInfo->lpVtbl->GetContainingTypeLib(TypeInfo, &TypeLib, &idx))) {
				efree(p);
				return FAILURE;
			}
		}
	} else {
		if (FAILED(LoadTypeLib(p, &TypeLib))) {
			efree(p);
			return FAILURE;
		}
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
			EnumId = php_OLECHAR_to_char(bstr_EnumId, NULL, 0, codepage);
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
				ids = php_OLECHAR_to_char(bstr_ids, NULL, 1, codepage);
				c.name_len = strlen(ids)+1;
				c.name = ids;
				php_variant_to_pval(pVarDesc->lpvarValue, &c.value, 1, codepage);
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

void php_register_COM_class()
{
	INIT_OVERLOADED_CLASS_ENTRY(com_class_entry, "COM", NULL,
								php_COM_call_function_handler,
								php_COM_get_property_handler,
								php_COM_set_property_handler);

	zend_register_internal_class(&com_class_entry);
}

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

// exports for external object creation

zend_module_entry COM_module_entry = {
	"com", COM_functions, PHP_MINIT(COM), PHP_MSHUTDOWN(COM), NULL, NULL, PHP_MINFO(COM), STANDARD_MODULE_PROPERTIES
};

PHPAPI int php_COM_get_le_idispatch() {
	return le_idispatch;
}

#endif
