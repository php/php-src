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
   | Author: Sam Ruby <rubys@us.ibm.com>                                  |
   |         Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */

/*
 * This module implements support for Microsoft .Net components.
 */

/*
 * 28.1.2001
 * use external unicode conversion functions
 *
 * harald radi <h.radi@nme.at>
 */

#ifdef PHP_WIN32

#include <iostream>
#include <math.h>
#include <comdef.h>

extern "C"
{
#include "php.h"
#include "ext/standard/info.h"
}

#include "ext/com/conversion.h"
#include "ext/com/php_COM.h"

#include "Mscoree.h"
#include "mscorlib.h"

using namespace mscorlib;

static ICorRuntimeHost *pHost;
static mscorlib::_AppDomain *pDomain;

static zend_class_entry dotnet_class_entry;
static int codepage;

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

HRESULT dotnet_create(OLECHAR *assembly, OLECHAR *datatype, comval *obj TSRMLS_DC) {
  HRESULT hr;

  _ObjectHandle *pHandle;
  hr = pDomain->CreateInstance(_bstr_t(assembly), _bstr_t(datatype), &pHandle); 
  if (FAILED(hr)) return hr;
  if (!pHandle) return hr;

  _variant_t unwrapped;
  hr = pHandle->Unwrap(&unwrapped);
  pHandle->Release();
  if (FAILED(hr)) return hr;
 
  php_COM_set(obj, &unwrapped.pdispVal, TRUE TSRMLS_CC);
  return ERROR_SUCCESS;
}
  
void dotnet_term() {
  if (pHost) pHost->Stop();
  if (pHost) pHost->Release();
  if (pDomain) pDomain->Release();

  pHost = 0;
  pDomain = 0;
}

/* {{{ proto int dotnet_load(string assembly_name [, string datatype_name, int codepage])
   Loads a DOTNET module */
PHP_FUNCTION(dotnet_load)
{
	HRESULT hr;
	zval **assembly_name, **datatype_name, **code_page;
	OLECHAR *assembly, *datatype;
	comval *obj;

	switch(ZEND_NUM_ARGS())
	{
		case 2:
			zend_get_parameters_ex(2, &assembly_name, &datatype_name);
			codepage = CP_ACP;
			break;
		case 3:
			zend_get_parameters_ex(3, &assembly_name, &datatype_name, &code_page);

			convert_to_long_ex(code_page);
			codepage = Z_LVAL_PP(code_page);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	convert_to_string_ex(assembly_name);
	assembly = php_char_to_OLECHAR(Z_STRVAL_PP(assembly_name), Z_STRLEN_PP(assembly_name), codepage TSRMLS_CC);

	convert_to_string_ex(datatype_name);
	datatype = php_char_to_OLECHAR(Z_STRVAL_PP(datatype_name), Z_STRLEN_PP(datatype_name), codepage TSRMLS_CC);

	ALLOC_COM(obj);

	/* obtain IDispatch */
	hr = dotnet_create(assembly, datatype, obj TSRMLS_CC);
	efree(assembly);
	efree(datatype);
	if (FAILED(hr)) {
		char *error_message;
		error_message = php_COM_error_message(hr TSRMLS_CC);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error obtaining .Net class for %s in assembly %s: %s", datatype_name->value.str.val, assembly_name->value.str.val, error_message);
		LocalFree(error_message);
		efree(obj);
		RETURN_FALSE;
	}
	if (C_DISPATCH(obj) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to locate %s in assembly %s", datatype_name->value.str.val, assembly_name->value.str.val);
		efree(obj);
		RETURN_FALSE;
	}

	RETURN_LONG(zend_list_insert(obj, IS_COM));
}
/* }}} */


void php_DOTNET_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	pval *object = property_reference->object;
	zend_overloaded_element *function_name = (zend_overloaded_element *) property_reference->elements_list->tail->data;

	if (zend_llist_count(property_reference->elements_list)==1
		&& !strcmp(Z_STRVAL(function_name->element), "dotnet")) { /* constructor */
		pval *object_handle;

		PHP_FN(dotnet_load)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
		if (!Z_LVAL_P(return_value)) {
			ZVAL_FALSE(object);
			return;
		}
		ALLOC_ZVAL(object_handle);
		*object_handle = *return_value;
		pval_copy_constructor(object_handle);
		INIT_PZVAL(object_handle);
		zend_hash_index_update(Z_OBJPROP_P(object), 0, &object_handle, sizeof(pval *), NULL);
		pval_destructor(&function_name->element);
	} else {
		php_COM_call_function_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, property_reference);
	}
}

void php_register_DOTNET_class(TSRMLS_D)
{
	INIT_OVERLOADED_CLASS_ENTRY(dotnet_class_entry, "DOTNET", NULL,
								php_DOTNET_call_function_handler,
								php_COM_get_property_handler,
								php_COM_set_property_handler);

	zend_register_internal_class(&dotnet_class_entry TSRMLS_CC);
}

function_entry DOTNET_functions[] = {
	{NULL, NULL, NULL}
};

static PHP_MINFO_FUNCTION(DOTNET)
{
	php_info_print_table_start();
	php_info_print_table_row(2, ".NET support", "enabled");
	php_info_print_table_end();
}

PHP_MINIT_FUNCTION(DOTNET)
{
	HRESULT hr;

	if (FAILED(hr = dotnet_init())) {
		return hr;
	}

	php_register_DOTNET_class(TSRMLS_C);
	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(DOTNET)
{
	dotnet_term();
	return SUCCESS;
}


zend_module_entry dotnet_module_entry = {
	STANDARD_MODULE_HEADER,
	"dotnet", DOTNET_functions, PHP_MINIT(DOTNET), PHP_MSHUTDOWN(DOTNET), NULL, NULL, PHP_MINFO(DOTNET), NO_VERSION_YET, STANDARD_MODULE_PROPERTIES
};

BEGIN_EXTERN_C()
ZEND_GET_MODULE(dotnet)
END_EXTERN_C()

#endif
