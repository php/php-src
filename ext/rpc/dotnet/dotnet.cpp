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
   | Author: Sam Ruby <rubys@us.ibm.com>                                  |
   +----------------------------------------------------------------------+
 */


/*
 * This module implements support for Microsoft .Net components.
 */

#ifdef PHP_WIN32

#include <iostream.h>
#include <math.h>
#include <comdef.h>

extern "C" {
#include "php.h"
#include "php_ini.h"

pval php_COM_get_property_handler(zend_property_reference *property_reference);


int php_COM_set_property_handler(zend_property_reference *property_reference, pval *value);

char *php_COM_error_message(HRESULT hr);

void php_COM_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);

int php_COM_get_le_idispatch();
}


#include <stdio.h>

#include "Mscoree.h"
#include "mscorlib.h"

using namespace mscorlib;

static ICorRuntimeHost *pHost;
static mscorlib::_AppDomain *pDomain;

static zend_class_entry dotnet_class_entry;

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

HRESULT dotnet_init() {
  HRESULT hr;

  hr = CoCreateInstance(CLSID_CorRuntimeHost, NULL, CLSCTX_ALL,
    IID_ICorRuntimeHost, (void **)&pHost);
  if (FAILED(hr)) return hr;
 
  hr = pHost->Start();
  if (FAILED(hr)) return hr;

  IUnknown *uDomain;
  hr = pHost->GetDefaultDomain(&uDomain);
  if (FAILED(hr)) return hr;

  hr = uDomain->QueryInterface(__uuidof(_AppDomain), (void**) &pDomain);
  if (FAILED(hr)) return -1;

  uDomain->Release();

  return ERROR_SUCCESS;
}

HRESULT dotnet_create(OLECHAR *assembly, OLECHAR *datatype, struct IDispatch **object) {
  HRESULT hr;

  _ObjectHandle *pHandle;
  hr = pDomain->CreateInstance(_bstr_t(assembly), _bstr_t(datatype), &pHandle); 
  if (FAILED(hr)) return hr;
  if (!pHandle) return hr;

  _variant_t unwrapped;
  hr = pHandle->Unwrap(&unwrapped);
  pHandle->Release();
  if (FAILED(hr)) return hr;
 
  *object = unwrapped.pdispVal;
  return ERROR_SUCCESS;
}
  
void dotnet_term() {
  if (pHost) pHost->Stop();
  if (pHost) pHost->Release();
  if (pDomain) pDomain->Release();

  pHost = 0;
  pDomain = 0;
}

/* {{{ proto int dotnet_load(string module_name)
   Loads a DOTNET module */
PHP_FUNCTION(DOTNET_load)
{
	HRESULT hr;
	pval *assembly_name, *datatype_name;
	OLECHAR *assembly, *datatype;
	IDispatch FAR *i_dispatch = NULL;

	if (ZEND_NUM_ARGS() != 2) WRONG_PARAM_COUNT;

	getParameters(ht, 2, &assembly_name, &datatype_name);
	convert_to_string(assembly_name);
	assembly = php_char_to_OLECHAR(assembly_name->value.str.val, assembly_name->value.str.len);

	convert_to_string(datatype_name);
	datatype = php_char_to_OLECHAR(datatype_name->value.str.val, datatype_name->value.str.len);

	/* obtain IDispatch */
	hr=dotnet_create(assembly, datatype, &i_dispatch);
	efree(assembly);
	efree(datatype);
	if (FAILED(hr)) {
		char *error_message;
		error_message = php_COM_error_message(hr);
		php_error(E_WARNING,"Error obtaining .Net class for %s in assembly %s: %s",datatype_name->value.str.val,assembly_name->value.str.val,error_message);
		LocalFree(error_message);
		RETURN_FALSE;
	}
	if (!i_dispatch) {
		php_error(E_WARNING,"Unable to locate %s in assembly %s",datatype_name->value.str.val,assembly_name->value.str.val);
		RETURN_FALSE;
	}

	RETURN_LONG(zend_list_insert(i_dispatch,php_COM_get_le_idispatch()));
}
/* }}} */



void php_DOTNET_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	pval *object = property_reference->object;
	zend_overloaded_element *function_name = (zend_overloaded_element *) property_reference->elements_list->tail->data;

	if (zend_llist_count(property_reference->elements_list)==1
		&& !strcmp(function_name->element.value.str.val, "dotnet")) { /* constructor */
		pval *object_handle;

		PHP_FN(DOTNET_load)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
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
		php_COM_call_function_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, property_reference);
	}
}


void php_register_DOTNET_class()
{
	INIT_OVERLOADED_CLASS_ENTRY(dotnet_class_entry, "DOTNET", NULL,
								php_DOTNET_call_function_handler,
								php_COM_get_property_handler,
								php_COM_set_property_handler);

	zend_register_internal_class(&dotnet_class_entry);
}

function_entry DOTNET_functions[] = {
	{NULL, NULL, NULL}
};


static PHP_MINFO_FUNCTION(DOTNET)
{
	DISPLAY_INI_ENTRIES();
}

PHP_INI_BEGIN()
PHP_INI_END()

PHP_MINIT_FUNCTION(DOTNET)
{

	HRESULT hr;
	CoInitialize(0);
	hr = dotnet_init();
	if (FAILED(hr)) return hr;

	php_register_DOTNET_class();
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(DOTNET)
{
	dotnet_term();
	CoUninitialize();
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}


zend_module_entry dotnet_module_entry = {
	"dotnet", DOTNET_functions, PHP_MINIT(DOTNET), PHP_MSHUTDOWN(DOTNET), NULL, NULL, PHP_MINFO(DOTNET), STANDARD_MODULE_PROPERTIES
};

extern "C" {
	ZEND_GET_MODULE(dotnet)
}

void php_register_DOTNET_class();


#endif
