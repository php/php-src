/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@zend.com>                             |
  +----------------------------------------------------------------------+
*/
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php_soap.h"
#if HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION)
#include "ext/session/php_session.h"
#endif
#ifdef ZEND_ENGINE_2
#  include "zend_exceptions.h"
#endif

static int le_sdl = 0;
int le_url = 0;
static int le_service = 0;

typedef struct _soapHeader {
	sdlFunctionPtr                    function;
	zval                              function_name;
	int                               mustUnderstand;
	int                               num_params;
	zval                            **parameters;
	zval                              retval;
	sdlSoapBindingFunctionHeaderPtr   hdr;
	struct _soapHeader               *next;
} soapHeader;

/* Local functions */
static void function_to_string(sdlFunctionPtr function, smart_str *buf);
static void type_to_string(sdlTypePtr type, smart_str *buf, int level);

static void clear_soap_fault(zval *obj TSRMLS_DC);
static void set_soap_fault(zval *obj, char *fault_code_ns, char *fault_code, char *fault_string, char *fault_actor, zval *fault_detail, char *name TSRMLS_DC);
static void soap_server_fault(char* code, char* string, char *actor, zval* details, char *name TSRMLS_DC);
static void soap_server_fault_ex(sdlFunctionPtr function, zval* fault, soapHeader* hdr TSRMLS_DC);

static sdlParamPtr get_param(sdlFunctionPtr function, char *param_name, int index, int);
static sdlFunctionPtr get_function(sdlPtr sdl, const char *function_name);
static sdlFunctionPtr get_doc_function(sdlPtr sdl, xmlNodePtr node);

static sdlFunctionPtr deserialize_function_call(sdlPtr sdl, xmlDocPtr request, char* actor, zval *function_name, int *num_params, zval **parameters[], int *version, soapHeader **headers TSRMLS_DC);
static xmlDocPtr serialize_response_call(sdlFunctionPtr function, char *function_name,char *uri,zval *ret, soapHeader *headers, int version TSRMLS_DC);
static xmlDocPtr serialize_function_call(zval *this_ptr, sdlFunctionPtr function, char *function_name, char *uri, zval **arguments, int arg_count, int version, HashTable *soap_headers TSRMLS_DC);
static xmlNodePtr serialize_parameter(sdlParamPtr param,zval *param_val,int index,char *name, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr serialize_zval(zval *val, sdlParamPtr param, char *paramName, int style, xmlNodePtr parent TSRMLS_DC);

static void delete_service(void *service);
static void delete_url(void *handle);

#ifndef ZEND_ENGINE_2
static void soap_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);
#endif

static void soap_error_handler(int error_num, const char *error_filename, const uint error_lineno, const char *format, va_list args);

#define SOAP_SERVER_BEGIN_CODE() \
	zend_bool _old_handler = SOAP_GLOBAL(use_soap_error_handler);\
	char* _old_error_code = SOAP_GLOBAL(error_code);\
	zval* _old_error_object = SOAP_GLOBAL(error_object);\
	int _old_soap_version = SOAP_GLOBAL(soap_version);\
	SOAP_GLOBAL(use_soap_error_handler) = 1;\
	SOAP_GLOBAL(error_code) = "Server";\
	SOAP_GLOBAL(error_object) = this_ptr;

#define SOAP_SERVER_END_CODE() \
	SOAP_GLOBAL(use_soap_error_handler) = _old_handler;\
	SOAP_GLOBAL(error_code) = _old_error_code;\
	SOAP_GLOBAL(error_object) = _old_error_object;\
	SOAP_GLOBAL(soap_version) = _old_soap_version;

#ifdef ZEND_ENGINE_2
#define SOAP_CLIENT_BEGIN_CODE() \
	zend_bool _old_handler = SOAP_GLOBAL(use_soap_error_handler);\
	char* _old_error_code = SOAP_GLOBAL(error_code);\
	zval* _old_error_object = SOAP_GLOBAL(error_object);\
	int _old_soap_version = SOAP_GLOBAL(soap_version);\
	zend_bool _old_in_compilation = CG(in_compilation); \
	zend_bool _old_in_execution = EG(in_execution); \
	zend_execute_data *_old_current_execute_data = EG(current_execute_data); \
	int _bailout = 0;\
	SOAP_GLOBAL(use_soap_error_handler) = 1;\
	SOAP_GLOBAL(error_code) = "Client";\
	SOAP_GLOBAL(error_object) = this_ptr;\
	zend_try {

#define SOAP_CLIENT_END_CODE() \
	} zend_catch {\
		CG(in_compilation) = _old_in_compilation; \
		EG(in_execution) = _old_in_execution; \
		EG(current_execute_data) = _old_current_execute_data; \
		if (EG(exception) == NULL || \
		    Z_TYPE_P(EG(exception)) != IS_OBJECT || \
		    !instanceof_function(Z_OBJCE_P(EG(exception)), soap_fault_class_entry TSRMLS_CC)) {\
			_bailout = 1;\
		}\
	} zend_end_try();\
	SOAP_GLOBAL(use_soap_error_handler) = _old_handler;\
	SOAP_GLOBAL(error_code) = _old_error_code;\
	SOAP_GLOBAL(error_object) = _old_error_object;\
	SOAP_GLOBAL(soap_version) = _old_soap_version;\
	if (_bailout) {\
		zend_bailout();\
	}
#else
#define SOAP_CLIENT_BEGIN_CODE() \
	zend_bool _old_handler = SOAP_GLOBAL(use_soap_error_handler);\
	char* _old_error_code = SOAP_GLOBAL(error_code);\
	zval* _old_error_object = SOAP_GLOBAL(error_object);\
	int _old_soap_version = SOAP_GLOBAL(soap_version);\
	SOAP_GLOBAL(use_soap_error_handler) = 1;\
	SOAP_GLOBAL(error_code) = "Client";\
	SOAP_GLOBAL(error_object) = this_ptr;

#define SOAP_CLIENT_END_CODE() \
	SOAP_GLOBAL(use_soap_error_handler) = _old_handler;\
	SOAP_GLOBAL(error_code) = _old_error_code;\
	SOAP_GLOBAL(error_object) = _old_error_object;\
	SOAP_GLOBAL(soap_version) = _old_soap_version;
#endif

#define HTTP_RAW_POST_DATA "HTTP_RAW_POST_DATA"

#define ZERO_PARAM() \
	if (ZEND_NUM_ARGS() != 0) \
 		WRONG_PARAM_COUNT;

#define FETCH_THIS_SDL(ss) \
	{ \
		zval **__tmp; \
		if(FIND_SDL_PROPERTY(this_ptr,__tmp) != FAILURE) { \
			FETCH_SDL_RES(ss,__tmp); \
		} else { \
			ss = NULL; \
		} \
	}

#define FIND_SDL_PROPERTY(ss,tmp) zend_hash_find(Z_OBJPROP_P(ss), "sdl", sizeof("sdl"), (void **)&tmp)
#define FETCH_SDL_RES(ss,tmp) ss = (sdlPtr) zend_fetch_resource(tmp TSRMLS_CC, -1, "sdl", NULL, 1, le_sdl)

#define FETCH_THIS_SERVICE(ss) \
	{ \
		zval **tmp; \
		if (zend_hash_find(Z_OBJPROP_P(this_ptr),"service", sizeof("service"), (void **)&tmp) != FAILURE) { \
			ss = (soapServicePtr)zend_fetch_resource(tmp TSRMLS_CC, -1, "service", NULL, 1, le_service); \
		} else { \
			ss = NULL; \
		} \
	}

static zend_class_entry* soap_class_entry;
static zend_class_entry* soap_server_class_entry;
static zend_class_entry* soap_fault_class_entry;
static zend_class_entry* soap_header_class_entry;
static zend_class_entry* soap_param_class_entry;
zend_class_entry* soap_var_class_entry;

ZEND_DECLARE_MODULE_GLOBALS(soap)

static void (*old_error_handler)(int, const char *, const uint, const char*, va_list);

#ifdef va_copy
#define call_old_error_handler(error_num, error_filename, error_lineno, format, args) \
{ \
	va_list copy; \
	va_copy(copy, args); \
	old_error_handler(error_num, error_filename, error_lineno, format, copy); \
	va_end(copy); \
}
#else
#define call_old_error_handler(error_num, error_filename, error_lineno, format, args) \
{ \
	old_error_handler(error_num, error_filename, error_lineno, format, args); \
}
#endif

#define PHP_SOAP_SERVER_CLASSNAME "SoapServer"
#define PHP_SOAP_CLIENT_CLASSNAME "SoapClient"
#define PHP_SOAP_VAR_CLASSNAME    "SoapVar"
#define PHP_SOAP_FAULT_CLASSNAME  "SoapFault"
#define PHP_SOAP_PARAM_CLASSNAME  "SoapParam"
#define PHP_SOAP_HEADER_CLASSNAME "SoapHeader"

PHP_RINIT_FUNCTION(soap);
PHP_MINIT_FUNCTION(soap);
PHP_MSHUTDOWN_FUNCTION(soap);
PHP_MINFO_FUNCTION(soap);

#ifndef ZEND_ENGINE_2
# ifndef PHP_METHOD
#  define PHP_METHOD(classname, name)	ZEND_NAMED_FUNCTION(ZEND_FN(classname##_##name))
#  define PHP_ME(classname, name, arg_info, flags)	ZEND_NAMED_FE(name, ZEND_FN(classname##_##name), arg_info)
# endif

static char *zend_str_tolower_copy(char *dest, const char *source, unsigned int length)
{
	register unsigned char *str = (unsigned char*)source;
	register unsigned char *result = (unsigned char*)dest;
	register unsigned char *end = str + length;

	while (str < end) {
		*result++ = tolower((int)*str++);
	}
	*result = *end;

	return dest;
}
#endif

/*
  Registry Functions
  TODO: this!
*/
PHP_FUNCTION(soap_encode_to_xml);
PHP_FUNCTION(soap_encode_to_zval);
PHP_FUNCTION(use_soap_error_handler);
PHP_FUNCTION(is_soap_fault);


/* Server Functions */
PHP_METHOD(SoapServer, SoapServer);
PHP_METHOD(SoapServer, setClass);
PHP_METHOD(SoapServer, addFunction);
PHP_METHOD(SoapServer, getFunctions);
PHP_METHOD(SoapServer, handle);
PHP_METHOD(SoapServer, setPersistence);
PHP_METHOD(SoapServer, fault);
PHP_METHOD(SoapServer, addSoapHeader);
#ifdef HAVE_PHP_DOMXML
PHP_METHOD(PHP_SOAP_SERVER_CLASS, map);
#endif

/* Client Functions */
PHP_METHOD(SoapClient, SoapClient);
PHP_METHOD(SoapClient, __call);
PHP_METHOD(SoapClient, __getLastRequest);
PHP_METHOD(SoapClient, __getLastResponse);
PHP_METHOD(SoapClient, __getLastRequestHeaders);
PHP_METHOD(SoapClient, __getLastResponseHeaders);
PHP_METHOD(SoapClient, __getFunctions);
PHP_METHOD(SoapClient, __getTypes);
PHP_METHOD(SoapClient, __doRequest);
PHP_METHOD(SoapClient, __setCookie);
PHP_METHOD(SoapClient, __setLocation);
PHP_METHOD(SoapClient, __setSoapHeaders);

/* SoapVar Functions */
PHP_METHOD(SoapVar, SoapVar);

/* SoapFault Functions */
PHP_METHOD(SoapFault, SoapFault);
#ifdef ZEND_ENGINE_2
PHP_METHOD(SoapFault, __toString);
#endif

/* SoapParam Functions */
PHP_METHOD(SoapParam, SoapParam);

/* SoapHeader Functions */
PHP_METHOD(SoapHeader, SoapHeader);

#ifdef ZEND_ENGINE_2
#define SOAP_CTOR(class_name, func_name, arginfo, flags) ZEND_FENTRY(__construct, ZEND_MN(class_name##_##func_name), arginfo, flags)
#else
#define SOAP_CTOR(class_name, func_name, arginfo, flags) PHP_ME(class_name, func_name, arginfo, flags)
#endif

static zend_function_entry soap_functions[] = {
#ifdef HAVE_PHP_DOMXML
	PHP_FE(soap_encode_to_xml, NULL)
	PHP_FE(soap_encode_to_zval, NULL)
#endif
	PHP_FE(use_soap_error_handler, NULL)
	PHP_FE(is_soap_fault, NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry soap_fault_functions[] = {
	SOAP_CTOR(SoapFault, SoapFault, NULL, 0)
#ifdef ZEND_ENGINE_2
	PHP_ME(SoapFault, __toString, NULL, 0)
#endif
	{NULL, NULL, NULL}
};

static zend_function_entry soap_server_functions[] = {
	SOAP_CTOR(SoapServer, SoapServer, NULL, 0)
	PHP_ME(SoapServer, setPersistence, NULL, 0)
	PHP_ME(SoapServer, setClass, NULL, 0)
	PHP_ME(SoapServer, addFunction, NULL, 0)
	PHP_ME(SoapServer, getFunctions, NULL, 0)
	PHP_ME(SoapServer, handle, NULL, 0)
	PHP_ME(SoapServer, fault, NULL, 0)
	PHP_ME(SoapServer, addSoapHeader, NULL, 0)
#ifdef HAVE_PHP_DOMXML
	PHP_ME(SoapServer, map, NULL, 0)
#endif
	{NULL, NULL, NULL}
};

#ifdef ZEND_ENGINE_2
ZEND_BEGIN_ARG_INFO(__call_args, 0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO(__soap_call_args, 0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO()
#else
unsigned char __call_args[] = { 2, BYREF_NONE, BYREF_NONE };
unsigned char __soap_call_args[] = { 5, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
#endif

static zend_function_entry soap_client_functions[] = {
	SOAP_CTOR(SoapClient, SoapClient, NULL, 0)
	PHP_ME(SoapClient, __call, __call_args, 0)
#ifdef ZEND_ENGINE_2
	ZEND_FENTRY(__soapCall, ZEND_MN(SoapClient___call), __soap_call_args, 0)
#else
	ZEND_NAMED_FE(__soapCall, ZEND_MN(SoapClient___call), __soap_call_args)
#endif
	PHP_ME(SoapClient, __getLastRequest, NULL, 0)
	PHP_ME(SoapClient, __getLastResponse, NULL, 0)
	PHP_ME(SoapClient, __getLastRequestHeaders, NULL, 0)
	PHP_ME(SoapClient, __getLastResponseHeaders, NULL, 0)
	PHP_ME(SoapClient, __getFunctions, NULL, 0)
	PHP_ME(SoapClient, __getTypes, NULL, 0)
	PHP_ME(SoapClient, __doRequest, NULL, 0)
	PHP_ME(SoapClient, __setCookie, NULL, 0)
	PHP_ME(SoapClient, __setLocation, NULL, 0)
	PHP_ME(SoapClient, __setSoapHeaders, NULL, 0)
	{NULL, NULL, NULL}
};

static zend_function_entry soap_var_functions[] = {
	SOAP_CTOR(SoapVar, SoapVar, NULL, 0)
	{NULL, NULL, NULL}
};

static zend_function_entry soap_param_functions[] = {
	SOAP_CTOR(SoapParam, SoapParam, NULL, 0)
	{NULL, NULL, NULL}
};

static zend_function_entry soap_header_functions[] = {
	SOAP_CTOR(SoapHeader, SoapHeader, NULL, 0)
	{NULL, NULL, NULL}
};

zend_module_entry soap_module_entry = {
#ifdef STANDARD_MODULE_HEADER
  STANDARD_MODULE_HEADER,
#endif
  "soap",
  soap_functions,
  PHP_MINIT(soap),
  PHP_MSHUTDOWN(soap),
  PHP_RINIT(soap),
  NULL,
  PHP_MINFO(soap),
#ifdef STANDARD_MODULE_HEADER
  NO_VERSION_YET,
#endif
  STANDARD_MODULE_PROPERTIES,
};

#ifdef COMPILE_DL_SOAP
ZEND_GET_MODULE(soap)
#endif

#ifndef ZEND_ENGINE_2
# define OnUpdateLong OnUpdateInt
#endif

ZEND_INI_MH(OnUpdateCacheEnabled)
{
	long *p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	p = (long*) (base+(size_t) mh_arg1);

	if (new_value_length==2 && strcasecmp("on", new_value)==0) {
		*p = 1;
	} 
	else if (new_value_length==3 && strcasecmp("yes", new_value)==0) {
		*p = 1;
	} 
	else if (new_value_length==4 && strcasecmp("true", new_value)==0) {
		*p = 1;
	} 
	else {
		*p = (long) (atoi(new_value) != 0);
	}
	return SUCCESS;
}

PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("soap.wsdl_cache_enabled",     "1", PHP_INI_ALL, OnUpdateCacheEnabled,
                  cache, zend_soap_globals, soap_globals)
STD_PHP_INI_ENTRY("soap.wsdl_cache_dir",         "/tmp", PHP_INI_ALL, OnUpdateString,
                  cache_dir, zend_soap_globals, soap_globals)
STD_PHP_INI_ENTRY("soap.wsdl_cache_ttl",         "86400", PHP_INI_ALL, OnUpdateLong,
                  cache_ttl, zend_soap_globals, soap_globals)
STD_PHP_INI_ENTRY("soap.wsdl_cache",             "1", PHP_INI_ALL, OnUpdateLong,
                  cache, zend_soap_globals, soap_globals)
STD_PHP_INI_ENTRY("soap.wsdl_cache_limit",       "5", PHP_INI_ALL, OnUpdateLong,
                  cache_limit, zend_soap_globals, soap_globals)
PHP_INI_END()

static HashTable defEnc, defEncIndex, defEncNs;

static void php_soap_prepare_globals()
{
	int i;
	encodePtr enc;

	zend_hash_init(&defEnc, 0, NULL, NULL, 1);
	zend_hash_init(&defEncIndex, 0, NULL, NULL, 1);
	zend_hash_init(&defEncNs, 0, NULL, NULL, 1);

	i = 0;
	do {
		enc = &defaultEncoding[i];

		/* If has a ns and a str_type then index it */
		if (defaultEncoding[i].details.type_str) {
			if (defaultEncoding[i].details.ns != NULL) {
				char *ns_type;
				ns_type = emalloc(strlen(defaultEncoding[i].details.ns) + strlen(defaultEncoding[i].details.type_str) + 2);
				sprintf(ns_type, "%s:%s", defaultEncoding[i].details.ns, defaultEncoding[i].details.type_str);
				zend_hash_add(&defEnc, ns_type, strlen(ns_type) + 1, &enc, sizeof(encodePtr), NULL);
				efree(ns_type);
			} else {
				zend_hash_add(&defEnc, defaultEncoding[i].details.type_str, strlen(defaultEncoding[i].details.type_str) + 1, &enc, sizeof(encodePtr), NULL);
			}
		}
		/* Index everything by number */
		if (!zend_hash_index_exists(&defEncIndex, defaultEncoding[i].details.type)) {
			zend_hash_index_update(&defEncIndex, defaultEncoding[i].details.type, &enc, sizeof(encodePtr), NULL);
		}
		i++;
	} while (defaultEncoding[i].details.type != END_KNOWN_TYPES);

	/* hash by namespace */
	zend_hash_add(&defEncNs, XSD_1999_NAMESPACE, sizeof(XSD_1999_NAMESPACE), XSD_NS_PREFIX, sizeof(XSD_NS_PREFIX), NULL);
	zend_hash_add(&defEncNs, XSD_NAMESPACE, sizeof(XSD_NAMESPACE), XSD_NS_PREFIX, sizeof(XSD_NS_PREFIX), NULL);
	zend_hash_add(&defEncNs, XSI_NAMESPACE, sizeof(XSI_NAMESPACE), XSI_NS_PREFIX, sizeof(XSI_NS_PREFIX), NULL);
	zend_hash_add(&defEncNs, XML_NAMESPACE, sizeof(XML_NAMESPACE), XML_NS_PREFIX, sizeof(XML_NS_PREFIX), NULL);
	zend_hash_add(&defEncNs, SOAP_1_1_ENC_NAMESPACE, sizeof(SOAP_1_1_ENC_NAMESPACE), SOAP_1_1_ENC_NS_PREFIX, sizeof(SOAP_1_1_ENC_NS_PREFIX), NULL);
	zend_hash_add(&defEncNs, SOAP_1_2_ENC_NAMESPACE, sizeof(SOAP_1_2_ENC_NAMESPACE), SOAP_1_2_ENC_NS_PREFIX, sizeof(SOAP_1_2_ENC_NS_PREFIX), NULL);
}

static void php_soap_init_globals(zend_soap_globals *soap_globals TSRMLS_DC)
{
	soap_globals->defEnc = defEnc;
	soap_globals->defEncIndex = defEncIndex;
	soap_globals->defEncNs = defEncNs;
	soap_globals->overrides = NULL;
	soap_globals->use_soap_error_handler = 0;
	soap_globals->error_code = NULL;
	soap_globals->error_object = NULL;
	soap_globals->sdl = NULL;
	soap_globals->soap_version = SOAP_1_1;
	soap_globals->mem_cache = NULL;
}

PHP_MSHUTDOWN_FUNCTION(soap)
{
	zend_error_cb = old_error_handler;
	zend_hash_destroy(&SOAP_GLOBAL(defEnc));
	zend_hash_destroy(&SOAP_GLOBAL(defEncIndex));
	zend_hash_destroy(&SOAP_GLOBAL(defEncNs));
	if (SOAP_GLOBAL(mem_cache)) {
		zend_hash_destroy(SOAP_GLOBAL(mem_cache));
		free(SOAP_GLOBAL(mem_cache));
	}
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_RINIT_FUNCTION(soap)
{
	SOAP_GLOBAL(overrides) = NULL;
	SOAP_GLOBAL(use_soap_error_handler) = 0;
	SOAP_GLOBAL(error_code) = NULL;
	SOAP_GLOBAL(error_object) = NULL;
	SOAP_GLOBAL(sdl) = NULL;
	SOAP_GLOBAL(soap_version) = SOAP_1_1;
	SOAP_GLOBAL(encoding) = NULL;
	SOAP_GLOBAL(class_map) = NULL;
	SOAP_GLOBAL(features) = 0;
	return SUCCESS;
}

PHP_MINIT_FUNCTION(soap)
{
	zend_class_entry ce;

	/* TODO: add ini entry for always use soap errors */
	php_soap_prepare_globals();
	ZEND_INIT_MODULE_GLOBALS(soap, php_soap_init_globals, NULL);
	REGISTER_INI_ENTRIES();

#ifndef ZEND_ENGINE_2
	/* Enable php stream/wrapper support for libxml */
	xmlRegisterDefaultInputCallbacks();
	xmlRegisterInputCallbacks(php_stream_xmlIO_match_wrapper, php_stream_xmlIO_open_wrapper,
			php_stream_xmlIO_read, php_stream_xmlIO_close);
#endif

	/* Register SoapClient class */
	/* BIG NOTE : THIS EMITS AN COMPILATION WARNING UNDER ZE2 - handle_function_call deprecated.
		soap_call_function_handler should be of type struct _zend_function, not (*handle_function_call).
	*/
#ifdef ZEND_ENGINE_2
	{
		zend_internal_function fe;

		fe.type = ZEND_INTERNAL_FUNCTION;
		fe.handler = ZEND_MN(SoapClient___call);
		fe.function_name.v = NULL;
		fe.scope = NULL;
		fe.fn_flags = 0;
		fe.prototype = NULL;
		fe.num_args = 2;
		fe.arg_info = NULL;
		fe.pass_rest_by_reference = 0;

		INIT_OVERLOADED_CLASS_ENTRY(ce, PHP_SOAP_CLIENT_CLASSNAME, soap_client_functions,
			(zend_function *)&fe, NULL, NULL);
		soap_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	}
#else
	INIT_OVERLOADED_CLASS_ENTRY(ce, PHP_SOAP_CLIENT_CLASSNAME, soap_client_functions, soap_call_function_handler, NULL, NULL);
	soap_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
#endif

	/* Register SoapVar class */
	INIT_CLASS_ENTRY(ce, PHP_SOAP_VAR_CLASSNAME, soap_var_functions);
	soap_var_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	/* Register SoapServer class */
	INIT_CLASS_ENTRY(ce, PHP_SOAP_SERVER_CLASSNAME, soap_server_functions);
	soap_server_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	/* Register SoapFault class */
	INIT_CLASS_ENTRY(ce, PHP_SOAP_FAULT_CLASSNAME, soap_fault_functions);
#ifdef ZEND_ENGINE_2
	soap_fault_class_entry = zend_register_internal_class_ex(&ce, zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);
#else
	soap_fault_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
#endif

	/* Register SoapParam class */
	INIT_CLASS_ENTRY(ce, PHP_SOAP_PARAM_CLASSNAME, soap_param_functions);
	soap_param_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, PHP_SOAP_HEADER_CLASSNAME, soap_header_functions);
	soap_header_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	le_sdl = register_list_destructors(delete_sdl, NULL);
	le_url = register_list_destructors(delete_url, NULL);
	le_service = register_list_destructors(delete_service, NULL);

	REGISTER_LONG_CONSTANT("SOAP_1_1", SOAP_1_1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_1_2", SOAP_1_2, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOAP_PERSISTENCE_SESSION", SOAP_PERSISTENCE_SESSION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_PERSISTENCE_REQUEST", SOAP_PERSISTENCE_REQUEST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_FUNCTIONS_ALL", SOAP_FUNCTIONS_ALL, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOAP_ENCODED", SOAP_ENCODED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_LITERAL", SOAP_LITERAL, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOAP_RPC", SOAP_RPC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_DOCUMENT", SOAP_DOCUMENT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOAP_ACTOR_NEXT", SOAP_ACTOR_NEXT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_ACTOR_NONE", SOAP_ACTOR_NONE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_ACTOR_UNLIMATERECEIVER", SOAP_ACTOR_UNLIMATERECEIVER, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOAP_COMPRESSION_ACCEPT", SOAP_COMPRESSION_ACCEPT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_COMPRESSION_GZIP", SOAP_COMPRESSION_GZIP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_COMPRESSION_DEFLATE", SOAP_COMPRESSION_DEFLATE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOAP_AUTHENTICATION_BASIC", SOAP_AUTHENTICATION_BASIC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_AUTHENTICATION_DIGEST", SOAP_AUTHENTICATION_DIGEST, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("UNKNOWN_TYPE", UNKNOWN_TYPE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("XSD_STRING", XSD_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_BOOLEAN", XSD_BOOLEAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_DECIMAL", XSD_DECIMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_FLOAT", XSD_FLOAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_DOUBLE", XSD_DOUBLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_DURATION", XSD_DURATION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_DATETIME", XSD_DATETIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_TIME", XSD_TIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_DATE", XSD_DATE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_GYEARMONTH", XSD_GYEARMONTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_GYEAR", XSD_GYEAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_GMONTHDAY", XSD_GMONTHDAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_GDAY", XSD_GDAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_GMONTH", XSD_GMONTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_HEXBINARY", XSD_HEXBINARY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_BASE64BINARY", XSD_BASE64BINARY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_ANYURI", XSD_ANYURI, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_QNAME", XSD_QNAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_NOTATION", XSD_NOTATION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_NORMALIZEDSTRING", XSD_NORMALIZEDSTRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_TOKEN", XSD_TOKEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_LANGUAGE", XSD_LANGUAGE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_NMTOKEN", XSD_NMTOKEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_NAME", XSD_NAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_NCNAME", XSD_NCNAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_ID", XSD_ID, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_IDREF", XSD_IDREF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_IDREFS", XSD_IDREFS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_ENTITY", XSD_ENTITY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_ENTITIES", XSD_ENTITIES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_INTEGER", XSD_INTEGER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_NONPOSITIVEINTEGER", XSD_NONPOSITIVEINTEGER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_NEGATIVEINTEGER", XSD_NEGATIVEINTEGER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_LONG", XSD_LONG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_INT", XSD_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_SHORT", XSD_SHORT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_BYTE", XSD_BYTE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_NONNEGATIVEINTEGER", XSD_NONNEGATIVEINTEGER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_UNSIGNEDLONG", XSD_UNSIGNEDLONG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_UNSIGNEDINT", XSD_UNSIGNEDINT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_UNSIGNEDSHORT", XSD_UNSIGNEDSHORT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_UNSIGNEDBYTE", XSD_UNSIGNEDBYTE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_POSITIVEINTEGER", XSD_POSITIVEINTEGER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_NMTOKENS", XSD_NMTOKENS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_ANYTYPE", XSD_ANYTYPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSD_ANYXML", XSD_ANYXML, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOAP_ENC_OBJECT", SOAP_ENC_OBJECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_ENC_ARRAY", SOAP_ENC_ARRAY, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("XSD_1999_TIMEINSTANT", XSD_1999_TIMEINSTANT, CONST_CS | CONST_PERSISTENT);

	REGISTER_STRING_CONSTANT("XSD_NAMESPACE", XSD_NAMESPACE, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("XSD_1999_NAMESPACE", XSD_1999_NAMESPACE,  CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOAP_SINGLE_ELEMENT_ARRAYS", SOAP_SINGLE_ELEMENT_ARRAYS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_WAIT_ONE_WAY_CALLS", SOAP_WAIT_ONE_WAY_CALLS, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("WSDL_CACHE_NONE",   WSDL_CACHE_NONE,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WSDL_CACHE_DISK",   WSDL_CACHE_DISK,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WSDL_CACHE_MEMORY", WSDL_CACHE_MEMORY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WSDL_CACHE_BOTH",   WSDL_CACHE_BOTH,   CONST_CS | CONST_PERSISTENT);

	old_error_handler = zend_error_cb;
	zend_error_cb = soap_error_handler;

	return SUCCESS;
}

PHP_MINFO_FUNCTION(soap)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Soap Client", "enabled");
	php_info_print_table_row(2, "Soap Server", "enabled");
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}

#ifdef HAVE_PHP_DOMXML
PHP_FUNCTION(soap_encode_to_xml)
{
	zval *pzval, *ret;
	encodePtr enc;
	char *name;
	int found, name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &name_len, &pzval) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}

	enc = get_conversion(Z_TYPE_P(pzval));
	ret = php_domobject_new(serialize_zval(pzval, NULL, name, SOAP_ENCODED), &found, NULL TSRMLS_CC);
	*return_value = *ret;
	zval_copy_ctor(return_value);
	zval_ptr_dtor(&ret);
}

PHP_FUNCTION(soap_encode_to_zval)
{
	zval *dom, **addr, *ret;
	xmlNodePtr node;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &dom) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}

	if (zend_hash_index_find(Z_OBJPROP_P(dom), 1, (void **)&addr) == FAILURE) {
	}

	node = (xmlNodePtr)Z_LVAL_PP(addr);
	ret = master_to_zval(NULL, node);
	*return_value = *ret;
}
#endif

/* {{{ proto object SoapParam::SoapParam ( mixed data, string name)
   SoapParam constructor */
PHP_METHOD(SoapParam, SoapParam)
{
	zval *data;
	char *name;
	int name_length;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &data, &name, &name_length) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}
	if (name_length == 0) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters. Invalid parameter name.");
	}

#ifndef ZEND_ENGINE_2
	zval_add_ref(&data);
#endif
	add_property_stringl(this_ptr, "param_name", name, name_length, 1);
	add_property_zval(this_ptr, "param_data", data);
}
/* }}} */


/* {{{ proto object SoapHeader::SoapHeader ( string namespace, string name [, mixed data [, bool mustUnderstand [, mixed actor]]])
   SoapHeader constructor */
PHP_METHOD(SoapHeader, SoapHeader)
{
	zval *data = NULL, *actor = NULL;
	char *name, *ns;
	int name_len, ns_len;
	zend_bool must_understand = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|zbz", &ns, &ns_len, &name, &name_len, &data, &must_understand, &actor) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}
	if (ns_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters. Invalid namespace.");
	}
	if (name_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters. Invalid header name.");
	}

	add_property_stringl(this_ptr, "namespace", ns, ns_len, 1);
	add_property_stringl(this_ptr, "name", name, name_len, 1);
	if (data) {
#ifndef ZEND_ENGINE_2
		zval_add_ref(&data);
#endif
		add_property_zval(this_ptr, "data", data);
	}
	add_property_bool(this_ptr, "mustUnderstand", must_understand);
	if (actor == NULL) {
	} else if (Z_TYPE_P(actor) == IS_LONG &&
	  (Z_LVAL_P(actor) == SOAP_ACTOR_NEXT ||
	   Z_LVAL_P(actor) == SOAP_ACTOR_NONE ||
	   Z_LVAL_P(actor) == SOAP_ACTOR_UNLIMATERECEIVER)) {
		add_property_long(this_ptr, "actor", Z_LVAL_P(actor));
	} else if (Z_TYPE_P(actor) == IS_STRING && Z_STRLEN_P(actor) > 0) {
		add_property_stringl(this_ptr, "actor", Z_STRVAL_P(actor), Z_STRLEN_P(actor), 1);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters. Invalid actor.");
	}
}

/* {{{ proto object SoapFault::SoapFault ( string faultcode, string faultstring [, string faultactor [, mixed detail [, string faultname [, mixed headerfault]]]])
   SoapFault constructor */
PHP_METHOD(SoapFault, SoapFault)
{
	char *fault_string = NULL, *fault_code = NULL, *fault_actor = NULL, *name = NULL, *fault_code_ns = NULL;
	int fault_string_len, fault_actor_len, name_len, fault_code_len = 0;
	zval *code = NULL, *details = NULL, *headerfault = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs|s!z!s!z",
		&code,
		&fault_string, &fault_string_len,
		&fault_actor, &fault_actor_len,
		&details, &name, &name_len, &headerfault) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}
	if (Z_TYPE_P(code) == IS_NULL) {
	} else if (Z_TYPE_P(code) == IS_STRING) {
		fault_code = Z_STRVAL_P(code);
		fault_code_len = Z_STRLEN_P(code);
	} else if (Z_TYPE_P(code) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(code)) == 2) {
		zval **t_ns, **t_code;

		zend_hash_internal_pointer_reset(Z_ARRVAL_P(code));
		zend_hash_get_current_data(Z_ARRVAL_P(code), (void**)&t_ns);
		zend_hash_move_forward(Z_ARRVAL_P(code));
		zend_hash_get_current_data(Z_ARRVAL_P(code), (void**)&t_code);
		if (Z_TYPE_PP(t_ns) == IS_STRING && Z_TYPE_PP(t_code) == IS_STRING) {
		  fault_code_ns = Z_STRVAL_PP(t_ns);
		  fault_code = Z_STRVAL_PP(t_code);
		  fault_code_len = Z_STRLEN_PP(t_code);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters. Invalid fault code.");
		}
	} else  {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters. Invalid fault code.");
	}
	if (fault_code != NULL && fault_code_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters. Invalid fault code.");
	}
	if (name != NULL && name_len == 0) {
		name = NULL;
	}

	set_soap_fault(this_ptr, fault_code_ns, fault_code, fault_string, fault_actor, details, name TSRMLS_CC);
	if (headerfault != NULL) {
		add_property_zval(this_ptr, "headerfault", headerfault);
	}
}
/* }}} */


#ifdef ZEND_ENGINE_2
/* {{{ proto object SoapFault::__toString ()
 */
PHP_METHOD(SoapFault, __toString)
{
	zval *faultcode, *faultstring, *file, *line, *trace;
	char *str;
	int len;
	zend_fcall_info fci;
	zval fname;

	if (ZEND_NUM_ARGS() > 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	faultcode   = zend_read_property(soap_fault_class_entry, this_ptr, "faultcode", sizeof("faultcode")-1, 1 TSRMLS_CC);
	faultstring = zend_read_property(soap_fault_class_entry, this_ptr, "faultstring", sizeof("faultstring")-1, 1 TSRMLS_CC);
	file = zend_read_property(soap_fault_class_entry, this_ptr, "file", sizeof("file")-1, 1 TSRMLS_CC);
	line = zend_read_property(soap_fault_class_entry, this_ptr, "line", sizeof("line")-1, 1 TSRMLS_CC);

	ZVAL_STRINGL(&fname, "gettraceasstring", sizeof("gettraceasstring")-1, 0);

	fci.size = sizeof(fci);
	fci.function_table = &Z_OBJCE_P(getThis())->function_table;
	fci.function_name = &fname;
	fci.symbol_table = NULL;
	fci.object_pp = &getThis();
	fci.retval_ptr_ptr = &trace;
	fci.param_count = 0;
	fci.params = NULL;
	fci.no_separation = 1;

	zend_call_function(&fci, NULL TSRMLS_CC);

	len = spprintf(&str, 0, "SoapFault exception: [%s] %s in %s:%ld\nStack trace:\n%s",
	               Z_STRVAL_P(faultcode), Z_STRVAL_P(faultstring), Z_STRVAL_P(file), Z_LVAL_P(line),
	               Z_STRLEN_P(trace) ? Z_STRVAL_P(trace) : "#0 {main}\n");

	zval_ptr_dtor(&trace);

	RETURN_STRINGL(str, len, 0);
}
/* }}} */
#endif

/* {{{ proto object SoapVar::SoapVar ( mixed data, int encoding [, string type_name [, string type_namespace [, string node_name [, string node_namespace]]]])
   SoapVar constructor */
PHP_METHOD(SoapVar, SoapVar)
{
	zval *data, *type;
	char *stype = NULL, *ns = NULL, *name = NULL, *namens = NULL;
	int stype_len, ns_len, name_len, namens_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z!z|ssss", &data, &type, &stype, &stype_len, &ns, &ns_len, &name, &name_len, &namens, &namens_len) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}

	if (Z_TYPE_P(type) == IS_NULL) {
		add_property_long(this_ptr, "enc_type", UNKNOWN_TYPE);
	} else {
		if (zend_hash_index_exists(&SOAP_GLOBAL(defEncIndex), Z_LVAL_P(type))) {
			add_property_long(this_ptr, "enc_type", Z_LVAL_P(type));
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid type ID");
		}
	}

	if (data) {
#ifndef ZEND_ENGINE_2
		zval_add_ref(&data);
#endif
		add_property_zval(this_ptr, "enc_value", data);
	}

	if (stype && stype_len > 0) {
		add_property_stringl(this_ptr, "enc_stype", stype, stype_len, 1);
	}
	if (ns && ns_len > 0) {
		add_property_stringl(this_ptr, "enc_ns", ns, ns_len, 1);
	}
	if (name && name_len > 0) {
		add_property_stringl(this_ptr, "enc_name", name, name_len, 1);
	}
	if (namens && namens_len > 0) {
		add_property_stringl(this_ptr, "enc_namens", namens, namens_len, 1);
	}
}
/* }}} */


/* {{{ proto object SoapServer::SoapServer ( mixed wsdl [, array options])
   SoapServer constructor */
PHP_METHOD(SoapServer, SoapServer)
{
	soapServicePtr service;
	zval *wsdl, *options = NULL;
	int ret;
	int version = SOAP_1_1;
	zend_bool cache_wsdl;

	SOAP_SERVER_BEGIN_CODE();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|a", &wsdl, &options) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}

	if (Z_TYPE_P(wsdl) == IS_STRING) {
	} else if (Z_TYPE_P(wsdl) == IS_NULL) {
		wsdl = NULL;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}

	service = emalloc(sizeof(soapService));
	memset(service, 0, sizeof(soapService));

	cache_wsdl = SOAP_GLOBAL(cache);

	if (options != NULL) {
		HashTable *ht = Z_ARRVAL_P(options);
		zval **tmp;

		if (zend_hash_find(ht, "soap_version", sizeof("soap_version"), (void**)&tmp) == SUCCESS) {
			if (Z_TYPE_PP(tmp) == IS_LONG ||
			    (Z_LVAL_PP(tmp) == SOAP_1_1 && Z_LVAL_PP(tmp) == SOAP_1_2)) {
				version = Z_LVAL_PP(tmp);
			}
		}

		if (zend_hash_find(ht, "uri", sizeof("uri"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
			service->uri = estrndup(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
		} else if (wsdl == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid arguments. 'uri' option is required in nonWSDL mode.");
			return;
		}

		if (zend_hash_find(ht, "actor", sizeof("actor"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
			service->actor = estrndup(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
		}

		if (zend_hash_find(ht, "encoding", sizeof("encoding"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
			xmlCharEncodingHandlerPtr encoding;
		
			encoding = xmlFindCharEncodingHandler(Z_STRVAL_PP(tmp));
    	if (encoding == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid arguments. Invalid 'encoding' option - '%s'.", Z_STRVAL_PP(tmp));
	    } else {
	      service->encoding = encoding;
	    }
		}

		if (zend_hash_find(ht, "classmap", sizeof("classmap"), (void**)&tmp) == SUCCESS &&
			Z_TYPE_PP(tmp) == IS_ARRAY) {
			zval *ztmp;

			ALLOC_HASHTABLE(service->class_map);
			zend_hash_init(service->class_map, 0, NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_copy(service->class_map, (*tmp)->value.ht, (copy_ctor_func_t) zval_add_ref, (void *) &ztmp, sizeof(zval *));
		}

		if (zend_hash_find(ht, "features", sizeof("features"), (void**)&tmp) == SUCCESS &&
			Z_TYPE_PP(tmp) == IS_LONG) {
			service->features = Z_LVAL_PP(tmp);
		}

		if (zend_hash_find(ht, "cache_wsdl", sizeof("cache_wsdl"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_LONG) {
			cache_wsdl = Z_LVAL_PP(tmp);
		}

	} else if (wsdl == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid arguments. 'uri' option is required in nonWSDL mode.");
	}

	service->version = version;
	service->type = SOAP_FUNCTIONS;
	service->soap_functions.functions_all = FALSE;
	service->soap_functions.ft = emalloc(sizeof(HashTable));
	zend_hash_init(service->soap_functions.ft, 0, NULL, ZVAL_PTR_DTOR, 0);

	if (wsdl) {
		service->sdl = get_sdl(this_ptr, Z_STRVAL_P(wsdl), cache_wsdl TSRMLS_CC);
		if (service->uri == NULL) {
			if (service->sdl->target_ns) {
				service->uri = estrdup(service->sdl->target_ns);
			} else {
				/*FIXME*/
				service->uri = estrdup("http://unknown-uri/");
			}
		}
	}

	ret = zend_list_insert(service, le_service);
	add_property_resource(this_ptr, "service", ret);

	SOAP_SERVER_END_CODE();
}
/* }}} */


#define NULL_OR_STRING(zval) \
	(!zval || Z_TYPE_P(zval) == IS_NULL || Z_TYPE_P(zval) == IS_STRING)

#define IS_VALID_FUNCTION(zval) \
	(zval && Z_TYPE_P(zval) != IS_NULL)

#ifdef HAVE_PHP_DOMXML
PHP_FUNCTION(PHP_SOAP_SERVER_CLASS, map)
{
	char *type, *class_name;
	zval *to_xml_before = NULL, *to_xml = NULL, *to_xml_after = NULL,
		*to_zval_before = NULL, *to_zval = NULL, *to_zval_after = NULL;
	int type_len, class_name_len;
	char *ns, *ctype;
	soapServicePtr service;

	SOAP_SERVER_BEGIN_CODE();

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sz|zzzzz",
		&type, &type_len, &to_xml_before, &to_xml, &to_xml_after, &to_zval_before, &to_zval,
		&to_zval_after) == SUCCESS && NULL_OR_STRING(to_xml_before) && NULL_OR_STRING(to_xml) &&
		NULL_OR_STRING(to_xml_after) && NULL_OR_STRING(to_zval_before) && NULL_OR_STRING(to_zval) &&
		NULL_OR_STRING(to_zval_after)) {

		soapMappingPtr map;
		encodePtr enc, new_enc;
		smart_str resloved_ns = {0};

		FETCH_THIS_SERVICE(service);

		new_enc = emalloc(sizeof(encode));
		memset(new_enc, 0, sizeof(encode));

		ctype = strrchr(type, ':');
		if (ctype) {
			smart_str_appendl(&resloved_ns, type, ctype - type);
			smart_str_0(&resloved_ns);
			ctype++;
		} else {
			ns = NULL;
		}

		if (ns) {
			if (zend_hash_find(SOAP_GLOBAL(defEncPrefix),  resloved_ns.c, resloved_ns.len + 1, &ns) == SUCCESS) {
				enc = get_encoder(service->sdl, ns, ctype);
				smart_str_free(&resloved_ns);
				smart_str_appendl(&resloved_ns, ns, strlen(ns));
				smart_str_appendc(&resloved_ns, ':');
				smart_str_appendl(&resloved_ns, ctype, strlen(ctype));
				smart_str_0(&resloved_ns);
				type = resloved_ns.c;
				type_len = resloved_ns.len;
			} else {
				enc = get_encoder_ex(service->sdl, type);
			}
		} else {
			enc = get_encoder_ex(service->sdl, type);
		}

		new_enc->details.type = enc->details.type;
		new_enc->details.ns = estrdup(enc->details.ns);
		new_enc->details.type_str = estrdup(enc->details.type_str);
		new_enc->details.sdl_type = enc->details.sdl_type;
		new_enc->to_xml = enc->to_xml;
		new_enc->to_zval = enc->to_zval;
		new_enc->to_xml_before = enc->to_xml_before;
		new_enc->to_zval_before = enc->to_zval_before;
		new_enc->to_xml_after = enc->to_xml_after;
		new_enc->to_zval_after = enc->to_zval_after;

		map = emalloc(sizeof(soapMapping));
		memset(map, 0, sizeof(soapMapping));

		map->type = SOAP_MAP_FUNCTION;
		if (IS_VALID_FUNCTION(to_xml_before)) {
			zval_add_ref(&to_xml_before);
			map->map_functions.to_xml_before = to_xml_before;
			new_enc->to_xml_before = to_xml_before_user;
		}
		if (IS_VALID_FUNCTION(to_xml)) {
			zval_add_ref(&to_xml);
			map->map_functions.to_xml = to_xml;
			new_enc->to_xml = to_xml_user;
		}
		if (IS_VALID_FUNCTION(to_xml_after)) {
			zval_add_ref(&to_xml_after);
			map->map_functions.to_xml_after = to_xml_after;
			new_enc->to_xml_after = to_xml_after_user;
		}
		if (IS_VALID_FUNCTION(to_zval_before)) {
			zval_add_ref(&to_zval_before);
			map->map_functions.to_zval_before = to_zval_before;
			new_enc->to_zval_before = to_zval_before_user;
		}
		if (IS_VALID_FUNCTION(to_zval)) {
			zval_add_ref(&to_zval);
			map->map_functions.to_zval = to_zval;
			new_enc->to_zval = to_zval_user;
		}
		if (IS_VALID_FUNCTION(to_zval_after)) {
			zval_add_ref(&to_zval_after);
			map->map_functions.to_zval_after = to_zval_after;
			new_enc->to_zval_after = to_zval_after_user;
		}

		new_enc->details.map = map;

		if (!service->mapping) {
			service->mapping = emalloc(sizeof(HashTable));
			zend_hash_init(service->mapping, 0, NULL, delete_encoder, 0);
		}
		zend_hash_update(service->mapping, type, type_len + 1, &new_enc, sizeof(encodePtr), NULL);
		smart_str_free(&resloved_ns);
	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &type, &type_len, &class_name, &class_name_len, &type) == SUCCESS) {
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}
}
#endif


/* {{{ proto object SoapServer::setPersistence ( int mode )
   Sets persistence mode of SoapServer */
PHP_METHOD(SoapServer, setPersistence)
{
	soapServicePtr service;
	long value;

	SOAP_SERVER_BEGIN_CODE();

	FETCH_THIS_SERVICE(service);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) != FAILURE) {
		if (service->type == SOAP_CLASS) {
			if (value == SOAP_PERSISTENCE_SESSION ||
				value == SOAP_PERSISTENCE_REQUEST) {
				service->soap_class.persistance = value;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Tried to set persistence with bogus value (%ld)", value);
			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Tried to set persistence when you are using you SOAP SERVER in function mode, no persistence needed");
		}
	}

	SOAP_SERVER_END_CODE();
}
/* }}} */


/* {{{ proto void SoapServer::setClass(string class_name [, mixed args])
   Sets class which will handle SOAP requests */
PHP_METHOD(SoapServer, setClass)
{
	soapServicePtr service;
#ifdef ZEND_ENGINE_2		
	zend_class_entry **ce;
#else
	zend_class_entry *ce;
#endif
	int found, argc;
	zval ***argv;

	SOAP_SERVER_BEGIN_CODE();

	FETCH_THIS_SERVICE(service);

	argc = ZEND_NUM_ARGS();
	argv = safe_emalloc(argc, sizeof(zval **), 0);

	if (argc < 1 || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		efree(argv);
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(argv[0]) == IS_STRING) {
#ifdef ZEND_ENGINE_2		
		found = zend_lookup_class(Z_STRVAL_PP(argv[0]), Z_STRLEN_PP(argv[0]), &ce TSRMLS_CC);
#else
		char *class_name = estrdup(Z_STRVAL_PP(argv[0]));
		found = zend_hash_find(EG(class_table), php_strtolower(class_name, Z_STRLEN_PP(argv[0])), Z_STRLEN_PP(argv[0])	 + 1, (void **)&ce);
		efree(class_name);
#endif
		if (found != FAILURE) {
			service->type = SOAP_CLASS;
#ifdef ZEND_ENGINE_2
			service->soap_class.ce = *ce;
#else
			service->soap_class.ce = ce;
#endif
			service->soap_class.persistance = SOAP_PERSISTENCE_REQUEST;
			service->soap_class.argc = argc - 1;
			if (service->soap_class.argc > 0) {
				int i;
				service->soap_class.argv = safe_emalloc(sizeof(zval), service->soap_class.argc, 0);
				for (i = 0;i < service->soap_class.argc;i++) {
					service->soap_class.argv[i] = *(argv[i + 1]);
					zval_add_ref(&service->soap_class.argv[i]);
				}
			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Tried to set a non existant class (%s)", Z_STRVAL_PP(argv[0]));
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "You must pass in a string");
	}

	efree(argv);

	SOAP_SERVER_END_CODE();
}
/* }}} */


/* {{{ proto array SoapServer::getFunctions(void)
   Returns list of defined functions */
PHP_METHOD(SoapServer, getFunctions)
{
	soapServicePtr  service;
	HashTable      *ft = NULL;

	SOAP_SERVER_BEGIN_CODE();

	ZERO_PARAM()
	FETCH_THIS_SERVICE(service);

	array_init(return_value);
	if (service->type == SOAP_CLASS) {
		ft = &service->soap_class.ce->function_table;
	} else if (service->soap_functions.functions_all == TRUE) {
		ft = EG(function_table);
	} else if (service->soap_functions.ft != NULL) {
		zval **name;
		HashPosition pos;

		zend_hash_internal_pointer_reset_ex(service->soap_functions.ft, &pos);
		while (zend_hash_get_current_data_ex(service->soap_functions.ft, (void **)&name, &pos) != FAILURE) {
			add_next_index_string(return_value, Z_STRVAL_PP(name), 1);
			zend_hash_move_forward_ex(service->soap_functions.ft, &pos);
		}
	}
	if (ft != NULL) {
		zend_function *f;
		HashPosition pos;
		zend_hash_internal_pointer_reset_ex(ft, &pos);
		while (zend_hash_get_current_data_ex(ft, (void **)&f, &pos) != FAILURE) {
			if ((service->type != SOAP_CLASS) || (f->common.fn_flags & ZEND_ACC_PUBLIC)) {
				/* FIXME: Unicode support??? */
				add_next_index_string(return_value, f->common.function_name.s, 1);
			}
			zend_hash_move_forward_ex(ft, &pos);
		}
	}

	SOAP_SERVER_END_CODE();
}
/* }}} */


/* {{{ proto void SoapServer::addFunction(mixed functions)
   Adds one or several functions those will handle SOAP requests */
PHP_METHOD(SoapServer, addFunction)
{
	soapServicePtr service;
	zval *function_name, *function_copy;
	HashPosition pos;

	SOAP_SERVER_BEGIN_CODE();

	FETCH_THIS_SERVICE(service);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &function_name) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}

	/* TODO: could use zend_is_callable here */

	if (function_name->type == IS_ARRAY) {
		if (service->type == SOAP_FUNCTIONS) {
			zval **tmp_function, *function_copy;

			if (service->soap_functions.ft == NULL) {
				service->soap_functions.functions_all = FALSE;
				service->soap_functions.ft = emalloc(sizeof(HashTable));
				zend_hash_init(service->soap_functions.ft, 0, NULL, ZVAL_PTR_DTOR, 0);
			}

			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(function_name), &pos);
			while (zend_hash_get_current_data_ex(Z_ARRVAL_P(function_name), (void **)&tmp_function, &pos) != FAILURE) {
				char *key;
				int   key_len;
				zend_function *f;

				if (Z_TYPE_PP(tmp_function) != IS_STRING) {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "Tried to add a function that isn't a string");
				}

				key_len = Z_STRLEN_PP(tmp_function);
				key = emalloc(key_len + 1);
				zend_str_tolower_copy(key, Z_STRVAL_PP(tmp_function), key_len);

				if (zend_hash_find(EG(function_table), key, key_len+1, (void**)&f) == FAILURE) {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "Tried to add a non existant function '%s'", Z_STRVAL_PP(tmp_function));
				}

				MAKE_STD_ZVAL(function_copy);
				ZVAL_TEXT(function_copy, f->common.function_name, 1);
				zend_hash_update(service->soap_functions.ft, key, key_len+1, &function_copy, sizeof(zval *), NULL);

				efree(key);
				zend_hash_move_forward_ex(Z_ARRVAL_P(function_name), &pos);
			}
		}
	} else if (function_name->type == IS_STRING) {
		char *key;
		int   key_len;
		zend_function *f;

		key_len = Z_STRLEN_P(function_name);
		key = emalloc(key_len + 1);
		zend_str_tolower_copy(key, Z_STRVAL_P(function_name), key_len);

		if (zend_hash_find(EG(function_table), key, key_len+1, (void**)&f) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Tried to add a non existant function '%s'", Z_STRVAL_P(function_name));
		}
		if (service->soap_functions.ft == NULL) {
			service->soap_functions.functions_all = FALSE;
			service->soap_functions.ft = emalloc(sizeof(HashTable));
			zend_hash_init(service->soap_functions.ft, 0, NULL, ZVAL_PTR_DTOR, 0);
		}

		MAKE_STD_ZVAL(function_copy);
		ZVAL_TEXT(function_copy, f->common.function_name, 1);
		zend_hash_update(service->soap_functions.ft, key, key_len+1, &function_copy, sizeof(zval *), NULL);
		efree(key);
	} else if (function_name->type == IS_LONG) {
		if (Z_LVAL_P(function_name) == SOAP_FUNCTIONS_ALL) {
			if (service->soap_functions.ft != NULL) {
				zend_hash_destroy(service->soap_functions.ft);
				efree(service->soap_functions.ft);
				service->soap_functions.ft = NULL;
			}
			service->soap_functions.functions_all = TRUE;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid value passed");
		}
	}

	SOAP_SERVER_END_CODE();
}
/* }}} */


/* {{{ proto void SoapServer::handle ( [string soap_request])
   Handles a SOAP request */
PHP_METHOD(SoapServer, handle)
{
	int soap_version, old_soap_version;
	sdlPtr old_sdl = NULL;
	soapServicePtr service;
	xmlDocPtr doc_request=NULL, doc_return;
	zval function_name, **params, **raw_post, *soap_obj, retval;
	char *fn_name, cont_len[30];
	int num_params = 0, size, i, call_status = 0;
	xmlChar *buf;
	HashTable *function_table;
	soapHeader *soap_headers = NULL;
	sdlFunctionPtr function;
	char *arg = NULL;
	int arg_len;
	xmlCharEncodingHandlerPtr old_encoding;
	HashTable *old_class_map;
	int old_features;

	SOAP_SERVER_BEGIN_CODE();

	FETCH_THIS_SERVICE(service);
	SOAP_GLOBAL(soap_version) = service->version;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &arg, &arg_len) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}
	INIT_ZVAL(retval);

	if (SG(request_info).request_method &&
	    strcmp(SG(request_info).request_method, "GET") == 0 &&
	    SG(request_info).query_string &&
	    stricmp(SG(request_info).query_string, "wsdl") == 0) {

		if (service->sdl) {
/*
			char *hdr = emalloc(sizeof("Location: ")+strlen(service->sdl->source));
			strcpy(hdr,"Location: ");
			strcat(hdr,service->sdl->source);
			sapi_add_header(hdr, sizeof("Location: ")+strlen(service->sdl->source)-1, 1);
			efree(hdr);
*/
			zval readfile, readfile_ret, *param;

			INIT_ZVAL(readfile);
			INIT_ZVAL(readfile_ret);
			MAKE_STD_ZVAL(param);

			sapi_add_header("Content-Type: text/xml; charset=utf-8", sizeof("Content-Type: text/xml; charset=utf-8")-1, 1);
			ZVAL_STRING(param, service->sdl->source, 1);
			ZVAL_STRING(&readfile, "readfile", 1);
			if (call_user_function(EG(function_table), NULL, &readfile, &readfile_ret, 1, &param  TSRMLS_CC) == FAILURE) {
				soap_server_fault("Server", "Couldn't find WSDL", NULL, NULL, NULL TSRMLS_CC);
			}

			zval_ptr_dtor(&param);
			zval_dtor(&readfile);
			zval_dtor(&readfile_ret);

			SOAP_SERVER_END_CODE();
			return;
		} else {
			soap_server_fault("Server", "WSDL generation is not supported yet", NULL, NULL, NULL TSRMLS_CC);
/*
			sapi_add_header("Content-Type: text/xml; charset=utf-8", sizeof("Content-Type: text/xml; charset=utf-8"), 1);
			PUTS("<?xml version=\"1.0\" ?>\n<definitions\n");
			PUTS("    xmlns=\"http://schemas.xmlsoap.org/wsdl/\"\n");
			PUTS("    targetNamespace=\"");
			PUTS(service->uri);
			PUTS("\">\n");
			PUTS("</definitions>");
*/
			SOAP_SERVER_END_CODE();
			return;
		}
	}

	if (php_output_start_default(TSRMLS_C) != SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR,"ob_start failed");
	}

	if (ZEND_NUM_ARGS() == 0) {
		if (zend_hash_find(&EG(symbol_table), HTTP_RAW_POST_DATA, sizeof(HTTP_RAW_POST_DATA), (void **) &raw_post)!=FAILURE
			&& ((*raw_post)->type==IS_STRING)) {
			zval **server_vars, **encoding;

			zend_is_auto_global("_SERVER", sizeof("_SERVER")-1 TSRMLS_CC);
			if (zend_hash_find(&EG(symbol_table), "_SERVER", sizeof("_SERVER"), (void **) &server_vars) == SUCCESS &&
			    Z_TYPE_PP(server_vars) == IS_ARRAY &&
			    zend_hash_find(Z_ARRVAL_PP(server_vars), "HTTP_CONTENT_ENCODING", sizeof("HTTP_CONTENT_ENCODING"), (void **) &encoding)==SUCCESS &&
			    Z_TYPE_PP(encoding) == IS_STRING) {
				zval func;
				zval retval;
			  zval param;
				zval *params[1];

				if ((strcmp(Z_STRVAL_PP(encoding),"gzip") == 0 ||
				     strcmp(Z_STRVAL_PP(encoding),"x-gzip") == 0) &&
				    zend_hash_exists(EG(function_table), "gzinflate", sizeof("gzinflate"))) {
					ZVAL_STRING(&func, "gzinflate", 0);
					params[0] = &param;
					ZVAL_STRINGL(params[0], Z_STRVAL_PP(raw_post)+10, Z_STRLEN_PP(raw_post)-10, 0);
					INIT_PZVAL(params[0]);
				} else if (strcmp(Z_STRVAL_PP(encoding),"deflate") == 0 &&
		           zend_hash_exists(EG(function_table), "gzuncompress", sizeof("gzuncompress"))) {
					ZVAL_STRING(&func, "gzuncompress", 0);
					params[0] = &param;
					ZVAL_STRINGL(params[0], Z_STRVAL_PP(raw_post), Z_STRLEN_PP(raw_post), 0);
					INIT_PZVAL(params[0]);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_ERROR,"Request is compressed with unknown compression '%s'",Z_STRVAL_PP(encoding));
				}
				if (call_user_function(CG(function_table), (zval**)NULL, &func, &retval, 1, params TSRMLS_CC) == SUCCESS &&
				    Z_TYPE(retval) == IS_STRING) {
					doc_request = soap_xmlParseMemory(Z_STRVAL(retval),Z_STRLEN(retval));
					zval_dtor(&retval);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_ERROR,"Can't uncompress compressed request");
				}
			} else {
				doc_request = soap_xmlParseMemory(Z_STRVAL_PP(raw_post),Z_STRLEN_PP(raw_post));
			}
		} else {
			if (SG(request_info).request_method &&
	    		strcmp(SG(request_info).request_method, "POST") == 0) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "PHP-SOAP requires 'always_populate_raw_post_data' to be on please check your php.ini file");
			}
			soap_server_fault("Server", "Bad Request. Can't find HTTP_RAW_POST_DATA", NULL, NULL, NULL TSRMLS_CC);
			return;
		}
	} else {
		doc_request = soap_xmlParseMemory(arg,arg_len);
	}

	if (doc_request == NULL) {
		soap_server_fault("Client", "Bad Request", NULL, NULL, NULL TSRMLS_CC);
	}
	if (xmlGetIntSubset(doc_request) != NULL) {
		xmlNodePtr env = get_node(doc_request->children,"Envelope");
		if (env && env->ns) {
			if (strcmp(env->ns->href,SOAP_1_1_ENV_NAMESPACE) == 0) {
				SOAP_GLOBAL(soap_version) = SOAP_1_1;
			} else if (strcmp(env->ns->href,SOAP_1_2_ENV_NAMESPACE) == 0) {
				SOAP_GLOBAL(soap_version) = SOAP_1_2;
			}
		}
		xmlFreeDoc(doc_request);
		soap_server_fault("Server", "DTD are not supported by SOAP", NULL, NULL, NULL TSRMLS_CC);
	}

	old_sdl = SOAP_GLOBAL(sdl);
	SOAP_GLOBAL(sdl) = service->sdl;
	old_encoding = SOAP_GLOBAL(encoding);
	SOAP_GLOBAL(encoding) = service->encoding;
	old_class_map = SOAP_GLOBAL(class_map);
	SOAP_GLOBAL(class_map) = service->class_map;
	old_features = SOAP_GLOBAL(features);
	SOAP_GLOBAL(features) = service->features;
	old_soap_version = SOAP_GLOBAL(soap_version);
	function = deserialize_function_call(service->sdl, doc_request, service->actor, &function_name, &num_params, &params, &soap_version, &soap_headers TSRMLS_CC);
	xmlFreeDoc(doc_request);

	service->soap_headers_ptr = &soap_headers;

	soap_obj = NULL;
	if (service->type == SOAP_CLASS) {
#if HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION)
		/* If persistent then set soap_obj from from the previous created session (if available) */
		if (service->soap_class.persistance == SOAP_PERSISTENCE_SESSION) {
			zval **tmp_soap;

			if (PS(session_status) != php_session_active &&
			    PS(session_status) != php_session_disabled) {
				php_session_start(TSRMLS_C);
			}

			/* Find the soap object and assign */
			if (zend_hash_find(Z_ARRVAL_P(PS(http_session_vars)), "_bogus_session_name", sizeof("_bogus_session_name"), (void **) &tmp_soap) == SUCCESS &&
			    Z_TYPE_PP(tmp_soap) == IS_OBJECT &&
			    Z_OBJCE_PP(tmp_soap) == service->soap_class.ce) {
				soap_obj = *tmp_soap;
			}
		}
#endif
		/* If new session or something wierd happned */
		if (soap_obj == NULL) {
			zval *tmp_soap;

			MAKE_STD_ZVAL(tmp_soap);
			object_init_ex(tmp_soap, service->soap_class.ce);

			/* Call constructor */
#ifdef ZEND_ENGINE_2
			if (zend_hash_exists(&Z_OBJCE_P(tmp_soap)->function_table, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME))) {
				zval c_ret, constructor;

				INIT_ZVAL(c_ret);
				INIT_ZVAL(constructor);

				ZVAL_STRING(&constructor, ZEND_CONSTRUCTOR_FUNC_NAME, 1);
				if (call_user_function(NULL, &tmp_soap, &constructor, &c_ret, service->soap_class.argc, service->soap_class.argv TSRMLS_CC) == FAILURE) {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "Error calling constructor");
				}
				if (EG(exception)) {
					php_output_discard(TSRMLS_C);
					if (Z_TYPE_P(EG(exception)) == IS_OBJECT &&
					    instanceof_function(Z_OBJCE_P(EG(exception)), soap_fault_class_entry TSRMLS_CC)) {
						soap_server_fault_ex(function, EG(exception), NULL TSRMLS_CC);
					}
					zval_dtor(&constructor);
					zval_dtor(&c_ret);
					zval_ptr_dtor(&tmp_soap);
					goto fail;
				}
				zval_dtor(&constructor);
				zval_dtor(&c_ret);
			} else {
#else
      {
#endif
				/* FIXME: Unicode support??? */
				int class_name_len = strlen(service->soap_class.ce->name.s);
				char *class_name = emalloc(class_name_len+1);

				memcpy(class_name, service->soap_class.ce->name.s, class_name_len+1);
				if (zend_hash_exists(&Z_OBJCE_P(tmp_soap)->function_table, php_strtolower(class_name, class_name_len), class_name_len+1)) {
					zval c_ret, constructor;

					INIT_ZVAL(c_ret);
					INIT_ZVAL(constructor);

					ZVAL_TEXT(&constructor, service->soap_class.ce->name, 1);
					if (call_user_function(NULL, &tmp_soap, &constructor, &c_ret, service->soap_class.argc, service->soap_class.argv TSRMLS_CC) == FAILURE) {
						php_error_docref(NULL TSRMLS_CC, E_ERROR, "Error calling constructor");
					}
#ifdef ZEND_ENGINE_2
					if (EG(exception)) {
						php_output_discard(TSRMLS_C);
						if (Z_TYPE_P(EG(exception)) == IS_OBJECT &&
						    instanceof_function(Z_OBJCE_P(EG(exception)), soap_fault_class_entry TSRMLS_CC)) {
							soap_server_fault_ex(function, EG(exception), NULL TSRMLS_CC);
						}
						zval_dtor(&constructor);
						zval_dtor(&c_ret);
						efree(class_name);
						zval_ptr_dtor(&tmp_soap);
						goto fail;
					}
#endif
					zval_dtor(&constructor);
					zval_dtor(&c_ret);
				}
				efree(class_name);
			}
#if HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION)
			/* If session then update session hash with new object */
			if (service->soap_class.persistance == SOAP_PERSISTENCE_SESSION) {
				zval **tmp_soap_pp;
				if (zend_hash_update(Z_ARRVAL_P(PS(http_session_vars)), "_bogus_session_name", sizeof("_bogus_session_name"), &tmp_soap, sizeof(zval *), (void **)&tmp_soap_pp) == SUCCESS) {
					soap_obj = *tmp_soap_pp;
				}
			} else {
				soap_obj = tmp_soap;
			}
#else
			soap_obj = tmp_soap;
#endif

		}
/* 			function_table = &(soap_obj->value.obj.ce->function_table);*/
		function_table = &((Z_OBJCE_P(soap_obj))->function_table);
	} else {
		if (service->soap_functions.functions_all == TRUE) {
			function_table = EG(function_table);
		} else {
			function_table = service->soap_functions.ft;
		}
	}

	doc_return = NULL;

	/* Process soap headers */
	if (soap_headers != NULL) {
		soapHeader *header = soap_headers;
		while (header != NULL) {
			soapHeader *h = header;

			header = header->next;
			if (service->sdl && !h->function && !h->hdr) {
				if (h->mustUnderstand) {
					soap_server_fault("MustUnderstand","Header not understood", NULL, NULL, NULL TSRMLS_CC);
				} else {
					continue;
				}
			}

			fn_name = estrndup(Z_STRVAL(h->function_name),Z_STRLEN(h->function_name));
			if (zend_hash_exists(function_table, php_strtolower(fn_name, Z_STRLEN(h->function_name)), Z_STRLEN(h->function_name) + 1) ||
			    (service->type == SOAP_CLASS &&
			     zend_hash_exists(function_table, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME)))) {
				if (service->type == SOAP_CLASS) {
					call_status = call_user_function(NULL, &soap_obj, &h->function_name, &h->retval, h->num_params, h->parameters TSRMLS_CC);
				} else {
					call_status = call_user_function(EG(function_table), NULL, &h->function_name, &h->retval, h->num_params, h->parameters TSRMLS_CC);
				}
				if (call_status != SUCCESS) {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "Function '%s' call failed", Z_STRVAL(h->function_name));
				}
				if (Z_TYPE(h->retval) == IS_OBJECT &&
				    instanceof_function(Z_OBJCE(h->retval), soap_fault_class_entry TSRMLS_CC)) {
					zval *headerfault = NULL, **tmp;

					if (zend_hash_find(Z_OBJPROP(h->retval), "headerfault", sizeof("headerfault"), (void**)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) != IS_NULL) {
						headerfault = *tmp;
					}
					php_output_discard(TSRMLS_C);
					soap_server_fault_ex(function, &h->retval, h TSRMLS_CC);
					efree(fn_name);
					if (soap_obj) {zval_ptr_dtor(&soap_obj);}
					goto fail;
#ifdef ZEND_ENGINE_2
				} else if (EG(exception)) {
					php_output_discard(TSRMLS_C);
					if (Z_TYPE_P(EG(exception)) == IS_OBJECT &&
					    instanceof_function(Z_OBJCE_P(EG(exception)), soap_fault_class_entry TSRMLS_CC)) {
						zval *headerfault = NULL, **tmp;

						if (zend_hash_find(Z_OBJPROP_P(EG(exception)), "headerfault", sizeof("headerfault"), (void**)&tmp) == SUCCESS &&
						    Z_TYPE_PP(tmp) != IS_NULL) {
							headerfault = *tmp;
						}
						soap_server_fault_ex(function, EG(exception), h TSRMLS_CC);
					}
					efree(fn_name);
					if (soap_obj) {zval_ptr_dtor(&soap_obj);}
					goto fail;
#endif
				}
			} else if (h->mustUnderstand) {
				soap_server_fault("MustUnderstand","Header not understood", NULL, NULL, NULL TSRMLS_CC);
			}
			efree(fn_name);
		}
	}

	fn_name = estrndup(Z_STRVAL(function_name),Z_STRLEN(function_name));
	if (zend_hash_exists(function_table, php_strtolower(fn_name, Z_STRLEN(function_name)), Z_STRLEN(function_name) + 1) ||
	    (service->type == SOAP_CLASS &&
	     zend_hash_exists(function_table, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME)))) {
		if (service->type == SOAP_CLASS) {
			call_status = call_user_function(NULL, &soap_obj, &function_name, &retval, num_params, params TSRMLS_CC);
#if HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION)
			if (service->soap_class.persistance != SOAP_PERSISTENCE_SESSION) {
				zval_ptr_dtor(&soap_obj);
			}
#else
			zval_ptr_dtor(&soap_obj);
#endif
		} else {
			call_status = call_user_function(EG(function_table), NULL, &function_name, &retval, num_params, params TSRMLS_CC);
		}
	} else {
		php_error(E_ERROR, "Function '%s' doesn't exist", Z_STRVAL(function_name));
	}
	efree(fn_name);

#ifdef ZEND_ENGINE_2
	if (EG(exception)) {
		php_output_discard(TSRMLS_C);
		if (Z_TYPE_P(EG(exception)) == IS_OBJECT &&
		    instanceof_function(Z_OBJCE_P(EG(exception)), soap_fault_class_entry TSRMLS_CC)) {
			soap_server_fault_ex(function, EG(exception), NULL TSRMLS_CC);
		}
#if HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION)
		if (soap_obj && service->soap_class.persistance != SOAP_PERSISTENCE_SESSION) {
#else
		if (soap_obj) {
#endif
		  zval_ptr_dtor(&soap_obj);
		}
		goto fail;
	}
#endif
	if (call_status == SUCCESS) {
		char *response_name;

		if (Z_TYPE(retval) == IS_OBJECT &&
		    instanceof_function(Z_OBJCE(retval), soap_fault_class_entry TSRMLS_CC)) {
			php_output_discard(TSRMLS_C);
			soap_server_fault_ex(function, &retval, NULL TSRMLS_CC);
			goto fail;
		}

		if (function && function->responseName) {
			response_name = estrdup(function->responseName);
		} else {
			response_name = emalloc(Z_STRLEN(function_name) + sizeof("Response"));
			memcpy(response_name,Z_STRVAL(function_name),Z_STRLEN(function_name));
			memcpy(response_name+Z_STRLEN(function_name),"Response",sizeof("Response"));
		}
		SOAP_GLOBAL(overrides) = service->mapping;
		doc_return = serialize_response_call(function, response_name, service->uri, &retval, soap_headers, soap_version TSRMLS_CC);
		SOAP_GLOBAL(overrides) = NULL;
		efree(response_name);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Function '%s' call failed", Z_STRVAL(function_name));
	}

	/* Flush buffer */
	php_output_discard(TSRMLS_C);

	if (doc_return) {
		/* xmlDocDumpMemoryEnc(doc_return, &buf, &size, XML_CHAR_ENCODING_UTF8); */
		xmlDocDumpMemory(doc_return, &buf, &size);

		if (size == 0) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Dump memory failed");
		} 	

		sprintf(cont_len, "Content-Length: %d", size);
		sapi_add_header(cont_len, strlen(cont_len), 1);
		if (soap_version == SOAP_1_2) {
			sapi_add_header("Content-Type: application/soap+xml; charset=utf-8", sizeof("Content-Type: application/soap+xml; charset=utf-8")-1, 1);
		} else {
			sapi_add_header("Content-Type: text/xml; charset=utf-8", sizeof("Content-Type: text/xml; charset=utf-8")-1, 1);
		}

		xmlFreeDoc(doc_return);
		php_write(buf, size TSRMLS_CC);
		xmlFree(buf);
	} else {
		sapi_add_header("HTTP/1.1 202 Accepted", sizeof("HTTP/1.1 202 Accepted")-1, 1);
		sapi_add_header("Content-Length: 0", sizeof("Content-Length: 0")-1, 1);
	}

fail:
	SOAP_GLOBAL(soap_version) = old_soap_version;
	SOAP_GLOBAL(encoding) = old_encoding;
	SOAP_GLOBAL(sdl) = old_sdl;
	SOAP_GLOBAL(class_map) = old_class_map;
	SOAP_GLOBAL(features) = old_features;

	/* Free soap headers */
	zval_dtor(&retval);
	while (soap_headers != NULL) {
		soapHeader *h = soap_headers;
		int i;

		soap_headers = soap_headers->next;
		if (h->parameters) {
			i = h->num_params;
			while (i > 0) {
				zval_ptr_dtor(&h->parameters[--i]);
			}
			efree(h->parameters);
		}
		zval_dtor(&h->function_name);
		zval_dtor(&h->retval);
		efree(h);
	}
	service->soap_headers_ptr = NULL;

	/* Free Memory */
	if (num_params > 0) {
		for (i = 0; i < num_params;i++) {
			zval_ptr_dtor(&params[i]);
		}
		efree(params);
	}
	zval_dtor(&function_name);

	SOAP_SERVER_END_CODE();
}
/* }}} */


/* {{{ proto SoapServer::fault ( staring code, string string [, string actor [, mixed details [, string name]]] )
   Issue SoapFault indicating an error */
PHP_METHOD(SoapServer, fault)
{
	char *code, *string, *actor=NULL, *name=NULL;
	int code_len, string_len, actor_len, name_len;
	zval* details = NULL;

	SOAP_SERVER_BEGIN_CODE();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|szs",
	    &code, &code_len, &string, &string_len, &actor, &actor_len, &details,
	    &name, &name_len) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}

	soap_server_fault(code, string, actor, details, name TSRMLS_CC);
	SOAP_SERVER_END_CODE();
}
/* }}} */

PHP_METHOD(SoapServer, addSoapHeader)
{
	soapServicePtr service;
	zval *fault;
	soapHeader **p;

	SOAP_SERVER_BEGIN_CODE();

	FETCH_THIS_SERVICE(service);

	if (!service || !service->soap_headers_ptr) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "The SoapServer::addSoapHeader function may be called only during SOAP request processing");
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &fault, soap_header_class_entry) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}

	p = service->soap_headers_ptr;
	while (*p != NULL) {
		p = &(*p)->next;
	}
	*p = emalloc(sizeof(soapHeader));
	memset(*p, 0, sizeof(soapHeader));
	ZVAL_NULL(&(*p)->function_name);
	(*p)->retval = *fault;
	zval_copy_ctor(&(*p)->retval);

	SOAP_SERVER_END_CODE();
}

static void soap_server_fault_ex(sdlFunctionPtr function, zval* fault, soapHeader *hdr TSRMLS_DC)
{
	int soap_version;
	xmlChar *buf, cont_len[30];
	int size;
	xmlDocPtr doc_return;

	soap_version = SOAP_GLOBAL(soap_version);

	doc_return = serialize_response_call(function, NULL, NULL, fault, hdr, soap_version TSRMLS_CC);

	xmlDocDumpMemory(doc_return, &buf, &size);

	/*
	   Want to return HTTP 500 but apache wants to over write
	   our fault code with their own handling... Figure this out later
	*/
	sapi_add_header("HTTP/1.1 500 Internal Service Error", sizeof("HTTP/1.1 500 Internal Service Error")-1, 1);
	sprintf(cont_len,"Content-Length: %d", size);
	sapi_add_header(cont_len, strlen(cont_len), 1);
	if (soap_version == SOAP_1_2) {
		sapi_add_header("Content-Type: application/soap+xml; charset=utf-8", sizeof("Content-Type: application/soap+xml; charset=utf-8")-1, 1);
	} else {
		sapi_add_header("Content-Type: text/xml; charset=utf-8", sizeof("Content-Type: text/xml; charset=utf-8")-1, 1);
	}
	php_write(buf, size TSRMLS_CC);

	xmlFreeDoc(doc_return);
	xmlFree(buf);
	zend_clear_exception(TSRMLS_C);
}

static void soap_server_fault(char* code, char* string, char *actor, zval* details, char* name TSRMLS_DC)
{
	zval ret;

	INIT_ZVAL(ret);

	set_soap_fault(&ret, NULL, code, string, actor, details, name TSRMLS_CC);
	/* TODO: Which function */
	soap_server_fault_ex(NULL, &ret, NULL TSRMLS_CC);
	zend_bailout();
}

static void soap_error_handler(int error_num, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
	zend_bool _old_in_compilation, _old_in_execution;
	zend_execute_data *_old_current_execute_data;
	TSRMLS_FETCH();

	_old_in_compilation = CG(in_compilation);
	_old_in_execution = EG(in_execution);
	_old_current_execute_data = EG(current_execute_data);

	if (!SOAP_GLOBAL(use_soap_error_handler)) {
		call_old_error_handler(error_num, error_filename, error_lineno, format, args);
		return;
	}

	if (SOAP_GLOBAL(error_object) &&
	    Z_TYPE_P(SOAP_GLOBAL(error_object)) == IS_OBJECT &&
	    instanceof_function(Z_OBJCE_P(SOAP_GLOBAL(error_object)), soap_class_entry TSRMLS_CC)) {
#ifdef ZEND_ENGINE_2
		zval **tmp;

		if ((error_num == E_USER_ERROR || 
		     error_num == E_COMPILE_ERROR || 
		     error_num == E_CORE_ERROR ||
		     error_num == E_ERROR || 
		     error_num == E_PARSE) &&
		    (zend_hash_find(Z_OBJPROP_P(SOAP_GLOBAL(error_object)), "_exceptions", sizeof("_exceptions"), (void **) &tmp) != SUCCESS ||
		     Z_TYPE_PP(tmp) != IS_BOOL || Z_LVAL_PP(tmp) != 0)) {
			zval *fault, *exception;
			char* code = SOAP_GLOBAL(error_code);
			char *buffer;
			int buffer_len;
			zval outbuf, outbuflen;
			va_list argcopy;
			int old = PG(display_errors);

			INIT_ZVAL(outbuf);
			INIT_ZVAL(outbuflen);
#ifdef va_copy
			va_copy(argcopy, args);
			buffer_len = vspprintf(&buffer, 0, format, argcopy);
			va_end(argcopy);
#else
			buffer_len = vspprintf(&buffer, 0, format, args);
#endif

			if (code == NULL) {
				code = "Client";
			}
			fault = add_soap_fault(SOAP_GLOBAL(error_object), code, buffer, NULL, NULL TSRMLS_CC);
			efree(buffer);
			MAKE_STD_ZVAL(exception);
			*exception = *fault;
			zval_copy_ctor(exception);
			INIT_PZVAL(exception);
			zend_throw_exception_object(exception TSRMLS_CC);

			PG(display_errors) = 0;
			zend_try {
				call_old_error_handler(error_num, error_filename, error_lineno, format, args);
			} zend_catch {
				CG(in_compilation) = _old_in_compilation;
				EG(in_execution) = _old_in_execution;
				EG(current_execute_data) = _old_current_execute_data;
			} zend_end_try();
			PG(display_errors) = old;
			zend_bailout();
		} else {
			call_old_error_handler(error_num, error_filename, error_lineno, format, args);
		}
#else
		call_old_error_handler(error_num, error_filename, error_lineno, format, args);
#endif
	} else {
		int old = PG(display_errors);
		int fault = 0;
		zval fault_obj;
		va_list argcopy;

		if (error_num == E_USER_ERROR || 
		    error_num == E_COMPILE_ERROR || 
		    error_num == E_CORE_ERROR ||
		    error_num == E_ERROR || 
		    error_num == E_PARSE) {

			char* code = SOAP_GLOBAL(error_code);
			char *buffer;
			int buffer_len;
			zval *outbuf = NULL;
			zval outbuflen;

			INIT_ZVAL(outbuflen);

#ifdef va_copy
			va_copy(argcopy, args);
			buffer_len = vspprintf(&buffer, 0, format, argcopy);
			va_end(argcopy);
#else
			buffer_len = vspprintf(&buffer, 0, format, args);
#endif

			if (code == NULL) {
				code = "Server";
			}
			/* Get output buffer and send as fault detials */
			if (php_output_get_length(&outbuflen TSRMLS_CC) != FAILURE && Z_LVAL(outbuflen) != 0) {
				ALLOC_INIT_ZVAL(outbuf);
				php_output_get_contents(outbuf TSRMLS_CC);
			}
			php_output_discard(TSRMLS_C);

			INIT_ZVAL(fault_obj);
			set_soap_fault(&fault_obj, NULL, code, buffer, NULL, outbuf, NULL TSRMLS_CC);
			efree(buffer);
			fault = 1;
		}

		PG(display_errors) = 0;
		zend_try {
			call_old_error_handler(error_num, error_filename, error_lineno, format, args);
		} zend_catch {
			CG(in_compilation) = _old_in_compilation;
			EG(in_execution) = _old_in_execution;
			EG(current_execute_data) = _old_current_execute_data;
		} zend_end_try();
		PG(display_errors) = old;

		if (fault) {
			soap_server_fault_ex(NULL, &fault_obj, NULL TSRMLS_CC);
			zend_bailout();
		}
	}
}

PHP_FUNCTION(use_soap_error_handler)
{
	zend_bool handler = 1;

	ZVAL_BOOL(return_value, SOAP_GLOBAL(use_soap_error_handler));
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &handler) == SUCCESS) {
		SOAP_GLOBAL(use_soap_error_handler) = handler;
	}
}

PHP_FUNCTION(is_soap_fault)
{
	zval *fault;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &fault) == SUCCESS &&
	    Z_TYPE_P(fault) == IS_OBJECT &&
	    instanceof_function(Z_OBJCE_P(fault), soap_fault_class_entry TSRMLS_CC)) {
		RETURN_TRUE;
	}
	RETURN_FALSE
}

/* SoapClient functions */

/* {{{ proto object SoapClient::SoapClient ( mixed wsdl [, array options])
   SoapClient constructor */
PHP_METHOD(SoapClient, SoapClient)
{

	zval *wsdl;
	zval *options = NULL;
	int  soap_version = SOAP_1_1;
	php_stream_context *context = NULL;
	zend_bool cache_wsdl;

	SOAP_CLIENT_BEGIN_CODE();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|a", &wsdl, &options) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
		return;
	}

	if (Z_TYPE_P(wsdl) == IS_STRING) {
	} else if (Z_TYPE_P(wsdl) != IS_NULL ) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "$wsdl must be string or null");
		return;
	} else {
		wsdl = NULL;
	}

	cache_wsdl = SOAP_GLOBAL(cache);

	if (options != NULL) {
		HashTable *ht = Z_ARRVAL_P(options);
		zval **tmp;

		if (wsdl == NULL) {
			/* Fetching non-WSDL mode options */
			if (zend_hash_find(ht, "uri", sizeof("uri"), (void**)&tmp) == SUCCESS &&
			    Z_TYPE_PP(tmp) == IS_STRING) {
				add_property_stringl(this_ptr, "uri", Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "'uri' option is requred in nonWSDL mode");
				return;
			}

			if (zend_hash_find(ht, "style", sizeof("style"), (void**)&tmp) == SUCCESS &&
					Z_TYPE_PP(tmp) == IS_LONG &&
					(Z_LVAL_PP(tmp) == SOAP_RPC || Z_LVAL_PP(tmp) == SOAP_DOCUMENT)) {
				add_property_long(this_ptr, "style", Z_LVAL_PP(tmp));
			}

			if (zend_hash_find(ht, "use", sizeof("use"), (void**)&tmp) == SUCCESS &&
					Z_TYPE_PP(tmp) == IS_LONG &&
					(Z_LVAL_PP(tmp) == SOAP_LITERAL || Z_LVAL_PP(tmp) == SOAP_ENCODED)) {
				add_property_long(this_ptr, "use", Z_LVAL_PP(tmp));
			}
		}

		if (zend_hash_find(ht, "stream_context", sizeof("stream_context"), (void**)&tmp) == SUCCESS &&
				Z_TYPE_PP(tmp) == IS_RESOURCE) {
			context = php_stream_context_from_zval(*tmp, 1);
		}

		if (zend_hash_find(ht, "location", sizeof("location"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
			add_property_stringl(this_ptr, "location", Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
		} else if (wsdl == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "'location' option is requred in nonWSDL mode");
			return;
		}

		if (zend_hash_find(ht, "soap_version", sizeof("soap_version"), (void**)&tmp) == SUCCESS) {
			if (Z_TYPE_PP(tmp) == IS_LONG ||
			    (Z_LVAL_PP(tmp) == SOAP_1_1 && Z_LVAL_PP(tmp) == SOAP_1_2)) {
				soap_version = Z_LVAL_PP(tmp);
			}
		}
		if (zend_hash_find(ht, "login", sizeof("login"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
			add_property_stringl(this_ptr, "_login", Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
			if (zend_hash_find(ht, "password", sizeof("password"), (void**)&tmp) == SUCCESS &&
			    Z_TYPE_PP(tmp) == IS_STRING) {
				add_property_stringl(this_ptr, "_password", Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
			}
			if (zend_hash_find(ht, "authentication", sizeof("authentication"), (void**)&tmp) == SUCCESS &&
			    Z_TYPE_PP(tmp) == IS_LONG &&
			    Z_LVAL_PP(tmp) == SOAP_AUTHENTICATION_DIGEST) {
				add_property_null(this_ptr, "_digest");
			}
		}
		if (zend_hash_find(ht, "proxy_host", sizeof("proxy_host"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
			add_property_stringl(this_ptr, "_proxy_host", Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
			if (zend_hash_find(ht, "proxy_port", sizeof("proxy_port"), (void**)&tmp) == SUCCESS &&
			    Z_TYPE_PP(tmp) == IS_LONG) {
				add_property_long(this_ptr, "_proxy_port", Z_LVAL_PP(tmp));
			}
			if (zend_hash_find(ht, "proxy_login", sizeof("proxy_login"), (void**)&tmp) == SUCCESS &&
			    Z_TYPE_PP(tmp) == IS_STRING) {
				add_property_stringl(this_ptr, "_proxy_login", Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
				if (zend_hash_find(ht, "proxy_password", sizeof("proxy_password"), (void**)&tmp) == SUCCESS &&
				    Z_TYPE_PP(tmp) == IS_STRING) {
					add_property_stringl(this_ptr, "_proxy_password", Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
				}
			}
		}
		if (zend_hash_find(ht, "local_cert", sizeof("local_cert"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
		  if (!context) {
  			context = php_stream_context_alloc();
		  }
 			php_stream_context_set_option(context, "ssl", "local_cert", *tmp);
			if (zend_hash_find(ht, "passphrase", sizeof("passphrase"), (void**)&tmp) == SUCCESS &&
			    Z_TYPE_PP(tmp) == IS_STRING) {
				php_stream_context_set_option(context, "ssl", "passphrase", *tmp);
			}
		}
		if (zend_hash_find(ht, "trace", sizeof("trace"), (void**)&tmp) == SUCCESS &&
		    (Z_TYPE_PP(tmp) == IS_BOOL || Z_TYPE_PP(tmp) == IS_LONG) &&
				Z_LVAL_PP(tmp) == 1) {
			add_property_long(this_ptr, "trace", 1);
		}
#ifdef ZEND_ENGINE_2
		if (zend_hash_find(ht, "exceptions", sizeof("exceptions"), (void**)&tmp) == SUCCESS &&
		    (Z_TYPE_PP(tmp) == IS_BOOL || Z_TYPE_PP(tmp) == IS_LONG) &&
				Z_LVAL_PP(tmp) == 0) {
			add_property_bool(this_ptr, "_exceptions", 0);
		}
#endif
		if (zend_hash_find(ht, "compression", sizeof("compression"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_LONG &&
	      zend_hash_exists(EG(function_table), "gzinflate", sizeof("gzinflate")) &&
	      zend_hash_exists(EG(function_table), "gzdeflate", sizeof("gzdeflate")) &&
	      zend_hash_exists(EG(function_table), "gzuncompress", sizeof("gzuncompress")) &&
	      zend_hash_exists(EG(function_table), "gzcompress", sizeof("gzcompress")) &&
	      zend_hash_exists(EG(function_table), "gzencode", sizeof("gzencode"))) {
			add_property_long(this_ptr, "compression", Z_LVAL_PP(tmp));
		}
		if (zend_hash_find(ht, "encoding", sizeof("encoding"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
			xmlCharEncodingHandlerPtr encoding;
		
			encoding = xmlFindCharEncodingHandler(Z_STRVAL_PP(tmp));
    	if (encoding == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid 'encoding' option - '%s'", Z_STRVAL_PP(tmp));
	    } else {
		    xmlCharEncCloseFunc(encoding);
				add_property_stringl(this_ptr, "_encoding", Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);			
			}
		}
		if (zend_hash_find(ht, "classmap", sizeof("classmap"), (void**)&tmp) == SUCCESS &&
			Z_TYPE_PP(tmp) == IS_ARRAY) {
			zval *class_map;

			MAKE_STD_ZVAL(class_map);
			*class_map = **tmp;
			INIT_PZVAL(class_map);
			zval_copy_ctor(class_map);
#ifdef ZEND_ENGINE_2
			class_map->refcount--;
#endif
			add_property_zval(this_ptr, "_classmap", class_map);
		}

		if (zend_hash_find(ht, "features", sizeof("features"), (void**)&tmp) == SUCCESS &&
			Z_TYPE_PP(tmp) == IS_LONG) {
			add_property_long(this_ptr, "_features", Z_LVAL_PP(tmp));
	    }

		if (zend_hash_find(ht, "connection_timeout", sizeof("connection_timeout"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_LONG && Z_LVAL_PP(tmp) > 0) {
			add_property_long(this_ptr, "_connection_timeout", Z_LVAL_PP(tmp));
		}

		if (context) {
			add_property_resource(this_ptr, "_stream_context", context->rsrc_id);
		}
	
		if (zend_hash_find(ht, "cache_wsdl", sizeof("cache_wsdl"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_LONG) {
			cache_wsdl = Z_LVAL_PP(tmp);
		}

		if (zend_hash_find(ht, "user_agent", sizeof("user_agent"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
			add_property_stringl(this_ptr, "_user_agent", Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
		}

	} else if (wsdl == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "'location' and 'uri' options are requred in nonWSDL mode");
		return;
	}

	add_property_long(this_ptr, "_soap_version", soap_version);

	if (wsdl) {
		int    old_soap_version, ret;
		sdlPtr sdl;

		old_soap_version = SOAP_GLOBAL(soap_version);
		SOAP_GLOBAL(soap_version) = soap_version;

		sdl = get_sdl(this_ptr, Z_STRVAL_P(wsdl), cache_wsdl TSRMLS_CC);
		ret = zend_list_insert(sdl, le_sdl);

		add_property_resource(this_ptr, "sdl", ret);

		SOAP_GLOBAL(soap_version) = old_soap_version;
	}

	SOAP_CLIENT_END_CODE();
}
/* }}} */

static int do_request(zval *this_ptr, xmlDoc *request, char *location, char *action, int version, int one_way, zval *response TSRMLS_DC)
{
	int    ret = TRUE;
	char  *buf;
	int    buf_size;
	zval   func, param0, param1, param2, param3, param4;
	zval  *params[5];
	zval **trace;
	zval **fault;

	INIT_ZVAL(*response);

	xmlDocDumpMemory(request, (xmlChar**)&buf, &buf_size);
	if (!buf) {
		add_soap_fault(this_ptr, "HTTP", "Error build soap request", NULL, NULL TSRMLS_CC);
		return FALSE;
	}

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS &&
	    Z_LVAL_PP(trace) > 0) {
		add_property_stringl(this_ptr, "__last_request", buf, buf_size, 1);
	}

	INIT_ZVAL(func);
	ZVAL_STRINGL(&func,"__doRequest",sizeof("__doRequest")-1,0);
	INIT_ZVAL(param0);
	params[0] = &param0;
	ZVAL_STRINGL(params[0], buf, buf_size, 0);
	INIT_ZVAL(param1);
	params[1] = &param1;
	if (location == NULL) {
		ZVAL_NULL(params[1]);
	} else {
		ZVAL_STRING(params[1], location, 0);
	}
	INIT_ZVAL(param2);
	params[2] = &param2;
	if (action == NULL) {
		ZVAL_NULL(params[2]);
	} else {
		ZVAL_STRING(params[2], action, 0);
	}
	INIT_ZVAL(param3);
	params[3] = &param3;
	ZVAL_LONG(params[3], version);

	INIT_ZVAL(param4);
	params[4] = &param4;
	ZVAL_LONG(params[4], one_way);

	if (call_user_function(NULL, &this_ptr, &func, response, 5, params TSRMLS_CC) != SUCCESS) {
		add_soap_fault(this_ptr, "Client", "SoapSlient::__doRequest() failed", NULL, NULL TSRMLS_CC);
		ret = FALSE;
	} else if (Z_TYPE_P(response) != IS_STRING) {
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "__soap_fault", sizeof("__soap_fault"), (void **) &fault) == FAILURE) {
			add_soap_fault(this_ptr, "Client", "SoapSlient::__doRequest() returned non string value", NULL, NULL TSRMLS_CC);
		}
		ret = FALSE;
	} else if (zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS &&
	    Z_LVAL_PP(trace) > 0) {
		add_property_stringl(this_ptr, "__last_response", Z_STRVAL_P(response), Z_STRLEN_P(response), 1);
	}
	xmlFree(buf);
	if (ret && zend_hash_find(Z_OBJPROP_P(this_ptr), "__soap_fault", sizeof("__soap_fault"), (void **) &fault) == SUCCESS) {
	  return FALSE;
	}	  
  return ret;
}

static void do_soap_call(zval* this_ptr,
                         char* function,
                         int function_len,
                         int arg_count,
                         zval** real_args,
                         zval* return_value,
                         char* location,
                         char* soap_action,
                         char* call_uri,
                         HashTable* soap_headers,
                         zval* output_headers
                         TSRMLS_DC)
{
	zval **tmp;
	zval **trace;
 	sdlPtr sdl = NULL;
 	sdlPtr old_sdl = NULL;
 	sdlFunctionPtr fn;
	xmlDocPtr request = NULL;
	int ret = FALSE;
	int soap_version;
	zval response;
	xmlCharEncodingHandlerPtr old_encoding;
	HashTable *old_class_map;
	int old_features;

	SOAP_CLIENT_BEGIN_CODE();

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS
		&& Z_LVAL_PP(trace) > 0) {
		zend_hash_del(Z_OBJPROP_P(this_ptr), "__last_request", sizeof("__last_request"));
		zend_hash_del(Z_OBJPROP_P(this_ptr), "__last_response", sizeof("__last_response"));
	}
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_soap_version", sizeof("_soap_version"), (void **) &tmp) == SUCCESS
		&& Z_LVAL_PP(tmp) == SOAP_1_2) {
		soap_version = SOAP_1_2;
	} else {
		soap_version = SOAP_1_1;
	}

	if (location == NULL) {
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "location", sizeof("location"),(void **) &tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
		  location = Z_STRVAL_PP(tmp);
		}
	}

	if (FIND_SDL_PROPERTY(this_ptr,tmp) != FAILURE) {
		FETCH_SDL_RES(sdl,tmp);
	}

 	clear_soap_fault(this_ptr TSRMLS_CC);

	SOAP_GLOBAL(soap_version) = soap_version;
	old_sdl = SOAP_GLOBAL(sdl);
	SOAP_GLOBAL(sdl) = sdl;
	old_encoding = SOAP_GLOBAL(encoding);
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_encoding", sizeof("_encoding"), (void **) &tmp) == SUCCESS &&
	    Z_TYPE_PP(tmp) == IS_STRING) {
		SOAP_GLOBAL(encoding) = xmlFindCharEncodingHandler(Z_STRVAL_PP(tmp));
	} else {
		SOAP_GLOBAL(encoding) = NULL;
	}
	old_class_map = SOAP_GLOBAL(class_map);
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_classmap", sizeof("_classmap"), (void **) &tmp) == SUCCESS &&
	    Z_TYPE_PP(tmp) == IS_ARRAY) {
		SOAP_GLOBAL(class_map) = (*tmp)->value.ht;
	} else {
		SOAP_GLOBAL(class_map) = NULL;
	}
	old_features = SOAP_GLOBAL(features);
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_features", sizeof("_features"), (void **) &tmp) == SUCCESS &&
	    Z_TYPE_PP(tmp) == IS_LONG) {
		SOAP_GLOBAL(features) = Z_LVAL_PP(tmp);
	} else {
		SOAP_GLOBAL(features) = 0;
	}

 	if (sdl != NULL) {
 		fn = get_function(sdl, function);
 		if (fn != NULL) {
			sdlBindingPtr binding = fn->binding;
			int one_way = 0;

			if (fn->responseName == NULL &&
			    fn->responseParameters == NULL &&
			    soap_headers == NULL) {
				one_way = 1;
			}

			if (location == NULL) {
				location = binding->location;
			}
			if (binding->bindingType == BINDING_SOAP) {
				sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)fn->bindingAttributes;
 				request = serialize_function_call(this_ptr, fn, NULL, fnb->input.ns, real_args, arg_count, soap_version, soap_headers TSRMLS_CC);
 				ret = do_request(this_ptr, request, location, fnb->soapAction, soap_version, one_way, &response TSRMLS_CC);
 			}	else {
 				request = serialize_function_call(this_ptr, fn, NULL, sdl->target_ns, real_args, arg_count, soap_version, soap_headers TSRMLS_CC);
 				ret = do_request(this_ptr, request, location, NULL, soap_version, one_way, &response TSRMLS_CC);
 			}

			xmlFreeDoc(request);

			if (ret && Z_TYPE(response) == IS_STRING) {
				ret = parse_packet_soap(this_ptr, Z_STRVAL(response), Z_STRLEN(response), fn, NULL, return_value, output_headers TSRMLS_CC);
			}

			zval_dtor(&response);

 		} else {
 			smart_str error = {0};
 			smart_str_appends(&error,"Function (\"");
 			smart_str_appends(&error,function);
 			smart_str_appends(&error,"\") is not a valid method for this service");
 			smart_str_0(&error);
			add_soap_fault(this_ptr, "Client", error.c, NULL, NULL TSRMLS_CC);
			smart_str_free(&error);
		}
	} else {
		zval **uri;
		smart_str action = {0};

		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "uri", sizeof("uri"), (void *)&uri) == FAILURE) {
			add_soap_fault(this_ptr, "Client", "Error finding \"uri\" property", NULL, NULL TSRMLS_CC);
		} else if (location == NULL) {
			add_soap_fault(this_ptr, "Client", "Error could not find \"location\" property", NULL, NULL TSRMLS_CC);
		} else {
			if (call_uri == NULL) {
				call_uri = Z_STRVAL_PP(uri);
			}
	 		request = serialize_function_call(this_ptr, NULL, function, call_uri, real_args, arg_count, soap_version, soap_headers TSRMLS_CC);

	 		if (soap_action == NULL) {
				smart_str_appends(&action, call_uri);
				smart_str_appendc(&action, '#');
				smart_str_appends(&action, function);
			} else {
				smart_str_appends(&action, soap_action);
			}
			smart_str_0(&action);

			ret = do_request(this_ptr, request, location, action.c, soap_version, 0, &response TSRMLS_CC);

	 		smart_str_free(&action);
			xmlFreeDoc(request);

			if (ret && Z_TYPE(response) == IS_STRING) {
				ret = parse_packet_soap(this_ptr, Z_STRVAL(response), Z_STRLEN(response), NULL, function, return_value, output_headers TSRMLS_CC);
			}

			zval_dtor(&response);
		}
 	}

	if (!ret) {
		zval** fault;
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "__soap_fault", sizeof("__soap_fault"), (void **) &fault) == SUCCESS) {
			*return_value = **fault;
			zval_copy_ctor(return_value);
		} else {
			*return_value = *add_soap_fault(this_ptr, "Client", "Unknown Error", NULL, NULL TSRMLS_CC);
			zval_copy_ctor(return_value);
		}
	} else {
		zval** fault;
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "__soap_fault", sizeof("__soap_fault"), (void **) &fault) == SUCCESS) {
			*return_value = **fault;
			zval_copy_ctor(return_value);
		}
	}
#ifdef ZEND_ENGINE_2
	if (Z_TYPE_P(return_value) == IS_OBJECT &&
	    instanceof_function(Z_OBJCE_P(return_value), soap_fault_class_entry TSRMLS_CC) &&
	    (zend_hash_find(Z_OBJPROP_P(this_ptr), "_exceptions", sizeof("_exceptions"), (void **) &tmp) != SUCCESS ||
		   Z_TYPE_PP(tmp) != IS_BOOL || Z_LVAL_PP(tmp) != 0)) {
		zval *exception;

		MAKE_STD_ZVAL(exception);
		*exception = *return_value;
		zval_copy_ctor(exception);
		INIT_PZVAL(exception);
		zend_throw_exception_object(exception TSRMLS_CC);
	}
#endif
	if (SOAP_GLOBAL(encoding) != NULL) {
		xmlCharEncCloseFunc(SOAP_GLOBAL(encoding));
	}
	SOAP_GLOBAL(features) = old_features;
	SOAP_GLOBAL(class_map) = old_class_map;
	SOAP_GLOBAL(encoding) = old_encoding;
	SOAP_GLOBAL(sdl) = old_sdl;
	SOAP_CLIENT_END_CODE();
}

static void verify_soap_headers_array(HashTable *ht TSRMLS_DC)
{
	zval **tmp;

	zend_hash_internal_pointer_reset(ht);
	while (zend_hash_get_current_data(ht, (void**)&tmp) == SUCCESS) {
		if (Z_TYPE_PP(tmp) != IS_OBJECT ||
		    !instanceof_function(Z_OBJCE_PP(tmp), soap_header_class_entry TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid SOAP header");
		}
		zend_hash_move_forward(ht);
	}
}


/* {{{ proto mixed SoapClient::__call ( string function_name [, array arguments [, array options [, array input_headers [, array output_headers]]]])
   Calls a SOAP function */
PHP_METHOD(SoapClient, __call)
{
	char *function, *location=NULL, *soap_action = NULL, *uri = NULL;
	int function_len, i = 0;
	HashTable* soap_headers = NULL;
	zval *options = NULL;
	zval *headers = NULL;
	zval *output_headers = NULL;
	zval *args;
	zval **real_args = NULL;
	zval **param;
	int arg_count;
	zval **tmp;
	zend_bool free_soap_headers = 0;

	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa|zzz",
		&function, &function_len, &args, &options, &headers, &output_headers) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}

	if (options) {
		if (Z_TYPE_P(options) == IS_ARRAY) {
			HashTable *ht = Z_ARRVAL_P(options);
			if (zend_hash_find(ht, "location", sizeof("location"), (void**)&tmp) == SUCCESS &&
			    Z_TYPE_PP(tmp) == IS_STRING) {
				location = Z_STRVAL_PP(tmp);
			}

			if (zend_hash_find(ht, "soapaction", sizeof("soapaction"), (void**)&tmp) == SUCCESS &&
			    Z_TYPE_PP(tmp) == IS_STRING) {
				soap_action = Z_STRVAL_PP(tmp);
			}

			if (zend_hash_find(ht, "uri", sizeof("uri"), (void**)&tmp) == SUCCESS &&
			    Z_TYPE_PP(tmp) == IS_STRING) {
				uri = Z_STRVAL_PP(tmp);
			}
		} else if (Z_TYPE_P(options) != IS_NULL) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "first parameter must be string or null");
		}
	}

	if (headers == NULL || Z_TYPE_P(headers) == IS_NULL) {
	} else if (Z_TYPE_P(headers) == IS_ARRAY) {
		soap_headers = Z_ARRVAL_P(headers);
		verify_soap_headers_array(soap_headers TSRMLS_CC);
		free_soap_headers = 0;
	} else if (Z_TYPE_P(headers) == IS_OBJECT &&
	           instanceof_function(Z_OBJCE_P(headers), soap_header_class_entry TSRMLS_CC)) {
	    soap_headers = emalloc(sizeof(HashTable));
		zend_hash_init(soap_headers, 0, NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_next_index_insert(soap_headers, &headers, sizeof(zval*), NULL);
		ZVAL_ADDREF(headers);
		free_soap_headers = 1;
	} else{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid SOAP header");
	}

	/* Add default headers */
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "__default_headers", sizeof("__default_headers"), (void **) &tmp)==SUCCESS) {
		HashTable *default_headers = Z_ARRVAL_P(*tmp);
		if (soap_headers) {
			if (!free_soap_headers) {
				HashTable *tmp =  emalloc(sizeof(HashTable));
				zend_hash_init(tmp, 0, NULL, ZVAL_PTR_DTOR, 0);
				zend_hash_copy(tmp, soap_headers, (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));
				soap_headers = tmp;
				free_soap_headers = 1;
			}
			zend_hash_internal_pointer_reset(default_headers);
			while (zend_hash_get_current_data(default_headers, (void**)&tmp) == SUCCESS) {
				ZVAL_ADDREF(*tmp);
				zend_hash_next_index_insert(soap_headers, tmp, sizeof(zval *), NULL);
				zend_hash_move_forward(default_headers);
			}
		} else {
			soap_headers = Z_ARRVAL_P(*tmp);
			free_soap_headers = 0;
		}
	}
	
	arg_count = zend_hash_num_elements(Z_ARRVAL_P(args));

	if (arg_count > 0) {
		real_args = safe_emalloc(sizeof(zval *), arg_count, 0);
		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(args), &pos);
			zend_hash_get_current_data_ex(Z_ARRVAL_P(args), (void **) &param, &pos) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_P(args), &pos)) {
				/*zval_add_ref(param);*/
				real_args[i++] = *param;
		}
	}
	if (output_headers) {
		array_init(output_headers);
	}
	do_soap_call(this_ptr, function, function_len, arg_count, real_args, return_value, location, soap_action, uri, soap_headers, output_headers TSRMLS_CC);
	if (arg_count > 0) {
		efree(real_args);
	}

	if (soap_headers && free_soap_headers) {
		zend_hash_destroy(soap_headers);
		efree(soap_headers);
	}
}
/* }}} */


/* {{{ proto array SoapClient::__getFunctions ( void )
   Returns list of SOAP functions */
PHP_METHOD(SoapClient, __getFunctions)
{
	sdlPtr sdl;
	HashPosition pos;

	FETCH_THIS_SDL(sdl);

	if (sdl) {
		smart_str buf = {0};
		sdlFunctionPtr *function;

		array_init(return_value);
 		zend_hash_internal_pointer_reset_ex(&sdl->functions, &pos);
		while (zend_hash_get_current_data_ex(&sdl->functions, (void **)&function, &pos) != FAILURE) {
			function_to_string((*function), &buf);
			add_next_index_stringl(return_value, buf.c, buf.len, 1);
			smart_str_free(&buf);
			zend_hash_move_forward_ex(&sdl->functions, &pos);
		}
	}
}
/* }}} */


/* {{{ proto array SoapClient::__getTypes ( void )
   Returns list of SOAP types */
PHP_METHOD(SoapClient, __getTypes)
{
	sdlPtr sdl;
	HashPosition pos;

	FETCH_THIS_SDL(sdl);

	if (sdl) {
		sdlTypePtr *type;
		smart_str buf = {0};

		array_init(return_value);
		if (sdl->types) {
			zend_hash_internal_pointer_reset_ex(sdl->types, &pos);
			while (zend_hash_get_current_data_ex(sdl->types, (void **)&type, &pos) != FAILURE) {
				type_to_string((*type), &buf, 0);
				add_next_index_stringl(return_value, buf.c, buf.len, 1);
				zend_hash_move_forward_ex(sdl->types, &pos);
				smart_str_free(&buf);
			}
		}
	}
}
/* }}} */


/* {{{ proto string SoapClient::__getLastRequest ( void )
   Returns last SOAP request */
PHP_METHOD(SoapClient, __getLastRequest)
{
	zval **tmp;

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "__last_request", sizeof("__last_request"), (void **)&tmp) == SUCCESS) {
		RETURN_STRINGL(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
	}
	RETURN_NULL();
}
/* }}} */


/* {{{ proto object SoapClient::__getLastResponse ( void )
   Returns last SOAP response */
PHP_METHOD(SoapClient, __getLastResponse)
{
	zval **tmp;

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "__last_response", sizeof("__last_response"), (void **)&tmp) == SUCCESS) {
		RETURN_STRINGL(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
	}
	RETURN_NULL();
}
/* }}} */


/* {{{ proto string SoapClient::__getLastRequestHeaders(void)
   Returns last SOAP request headers */
PHP_METHOD(SoapClient, __getLastRequestHeaders)
{
	zval **tmp;

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "__last_request_headers", sizeof("__last_request_headers"), (void **)&tmp) == SUCCESS) {
		RETURN_STRINGL(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
	}
	RETURN_NULL();
}
/* }}} */


/* {{{ proto string SoapClient::__getLastResponseHeaders(void)
   Returns last SOAP response headers */
PHP_METHOD(SoapClient, __getLastResponseHeaders)
{
	zval **tmp;

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "__last_response_headers", sizeof("__last_response_headers"), (void **)&tmp) == SUCCESS) {
		RETURN_STRINGL(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
	}
	RETURN_NULL();
}
/* }}} */


/* {{{ proto string SoapClient::__doRequest()
   SoapClient::__doRequest() */
PHP_METHOD(SoapClient, __doRequest)
{
  char *buf, *location, *action;
  int   buf_size, location_size, action_size;
  long  version;
  long  one_way = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssl|l",
	    &buf, &buf_size,
	    &location, &location_size,
	    &action, &action_size,
	    &version, &one_way) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	}
	if (SOAP_GLOBAL(features) & SOAP_WAIT_ONE_WAY_CALLS) {
		one_way = 0;
	}
	if (one_way && make_http_soap_request(this_ptr, buf, buf_size, location, action, version, NULL, NULL TSRMLS_CC)) {
		RETURN_EMPTY_STRING();
	} else if (make_http_soap_request(this_ptr, buf, buf_size, location, action, version,
	    &Z_STRVAL_P(return_value), &Z_STRLEN_P(return_value) TSRMLS_CC)) {
		return_value->type = IS_STRING;
		return;
	}
	RETURN_NULL();
}
/* }}} */

/* {{{ proto void SoapClient::__setCookie(string name [, strung value])
   Sets cookie thet will sent with SOAP request.
   The call to this function will effect all folowing calls of SOAP methods.
   If value is not specified cookie is removed. */
PHP_METHOD(SoapClient, __setCookie)
{
	char *name;
	char *val = NULL;
	int  name_len, val_len;
	zval **cookies;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s",
	    &name, &name_len, &val, &val_len) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	  RETURN_NULL();
	}

	if (val == NULL) {
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), (void **)&cookies) == SUCCESS) {
			zend_hash_del(Z_ARRVAL_PP(cookies), name, name_len+1);
		}
	} else {
		zval *zcookie;

		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), (void **)&cookies) == FAILURE) {
			zval *tmp_cookies;

			MAKE_STD_ZVAL(tmp_cookies);
			array_init(tmp_cookies);
			zend_hash_update(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), &tmp_cookies, sizeof(zval *), (void **)&cookies);
		}

		ALLOC_INIT_ZVAL(zcookie);
		array_init(zcookie);
		add_index_stringl(zcookie, 0, val, val_len, 1);
		add_assoc_zval_ex(*cookies, name, name_len+1, zcookie);
	}
}
/* }}} */

/* {{{ proto void SoapClient::__setSoapHeaders(array SoapHeaders)
   Sets SOAP headers for subsequent calls (replaces any previous
   values).
   If no value is specified, all of the headers are removed. */
PHP_METHOD(SoapClient, __setSoapHeaders)
{
	zval *headers;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &headers) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	  RETURN_NULL();
	}

	if (headers == NULL || Z_TYPE_P(headers) == IS_NULL) {
		zend_hash_del(Z_OBJPROP_P(this_ptr), "__default_headers", sizeof("__default_headers"));
	} else if (Z_TYPE_P(headers) == IS_ARRAY) {
		zval *default_headers;

		verify_soap_headers_array(Z_ARRVAL_P(headers) TSRMLS_CC);
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "__default_headers", sizeof("__default_headers"), (void **) &default_headers)==FAILURE) {
			add_property_zval(this_ptr, "__default_headers", headers);
		}
	} else if (Z_TYPE_P(headers) == IS_OBJECT &&
	           instanceof_function(Z_OBJCE_P(headers), soap_header_class_entry TSRMLS_CC)) {
		zval *default_headers;
		ALLOC_INIT_ZVAL(default_headers);
		array_init(default_headers);
		add_next_index_zval(default_headers, headers);
		add_property_zval(this_ptr, "__default_headers", default_headers);
	} else{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid SOAP header");
	}
	RETURN_TRUE;
}
/* }}} */



/* {{{ proto string SoapClient::__setLocation([string new_location])
   Sets the location option (the endpoint URL that will be touched by the 
   following SOAP requests).
   If new_location is not specified or null then SoapClient will use endpoint
   from WSDL file. 
   The function returns old value of location options. */
PHP_METHOD(SoapClient, __setLocation)
{
	char *location = NULL;
	int  location_len;
	zval **tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s",
	    &location, &location_len) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters");
	  RETURN_NULL();
	}
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "location", sizeof("location"),(void **) &tmp) == SUCCESS &&
	    Z_TYPE_PP(tmp) == IS_STRING) {
	  RETVAL_STRINGL(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 1);
	} else {
	  RETVAL_NULL();
	}
	if (location && location_len) {
		add_property_stringl(this_ptr, "location", location, location_len, 1);
	} else {
		zend_hash_del(Z_OBJPROP_P(this_ptr), "location", sizeof("location"));
	}
}
/* }}} */

#ifndef ZEND_ENGINE_2
static void soap_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	zval *object = property_reference->object;
	zend_overloaded_element *function_name = (zend_overloaded_element *)property_reference->elements_list->tail->data;
	char *function = Z_STRVAL(function_name->element);
	zend_function *builtin_function;

	/*
	   Find if the function being called is already defined...
	  ( IMHO: zend should handle this functionality )
	*/
	if (zend_hash_find(&Z_OBJCE_P(this_ptr)->function_table, function, Z_STRLEN(function_name->element) + 1, (void **) &builtin_function) == SUCCESS) {
		builtin_function->internal_function.handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	} else {
		int arg_count = ZEND_NUM_ARGS();
		zval **arguments = (zval **) safe_emalloc(sizeof(zval *), arg_count, 0);
		zval **soap_headers_p
		HashTable *soap_headers;

		zend_get_parameters_array(ht, arg_count, arguments);

		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "__default_headers", sizeof("__default_properties"), (void **) soap_headers_p)==SUCCESS
			&& Z_TYPE_P(soap_headers_p)==IS_ARRAY) {
			soap_headers = Z_ARRVAL_P(soap_headers_p);
		} else {
			soap_headers = NULL;
		}
		do_soap_call(this_ptr, function, Z_STRLEN(function_name->element) + 1, arg_count, arguments, return_value, NULL, NULL, NULL, soap_headers, NULL TSRMLS_CC);
		efree(arguments);
	}
	zval_dtor(&function_name->element);
}
#endif

static void clear_soap_fault(zval *obj TSRMLS_DC)
{
	if (obj != NULL && obj->type == IS_OBJECT) {
		zend_hash_del(Z_OBJPROP_P(obj), "__soap_fault", sizeof("__soap_fault"));
	}
}

zval* add_soap_fault(zval *obj, char *fault_code, char *fault_string, char *fault_actor, zval *fault_detail TSRMLS_DC)
{
	zval *fault;
	ALLOC_INIT_ZVAL(fault);
	set_soap_fault(fault, NULL, fault_code, fault_string, fault_actor, fault_detail, NULL TSRMLS_CC);
#ifdef ZEND_ENGINE_2
	fault->refcount--;
#endif
	add_property_zval(obj, "__soap_fault", fault);
	return fault;
}

static void set_soap_fault(zval *obj, char *fault_code_ns, char *fault_code, char *fault_string, char *fault_actor, zval *fault_detail, char *name TSRMLS_DC)
{
	if (Z_TYPE_P(obj) != IS_OBJECT) {
		object_init_ex(obj, soap_fault_class_entry);
	}
	if (fault_string != NULL) {
		add_property_string(obj, "faultstring", fault_string, 1);
#ifdef ZEND_ENGINE_2
		zend_update_property_string(zend_exception_get_default(TSRMLS_C), obj, "message", sizeof("message")-1, fault_string TSRMLS_CC);
#endif
	}
	if (fault_code != NULL) {
		int soap_version = SOAP_GLOBAL(soap_version);

		if (fault_code_ns) {
			add_property_string(obj, "faultcode", fault_code, 1);
			add_property_string(obj, "faultcodens", fault_code_ns, 1);
		} else {
			if (soap_version == SOAP_1_1) {
				add_property_string(obj, "faultcode", fault_code, 1);
				if (strcmp(fault_code,"Client") == 0 ||
				    strcmp(fault_code,"Server") == 0 ||
				    strcmp(fault_code,"VersionMismatch") == 0 ||
			  	  strcmp(fault_code,"MustUnderstand") == 0) {
					add_property_string(obj, "faultcodens", SOAP_1_1_ENV_NAMESPACE, 1);
				}
			} else if (soap_version == SOAP_1_2) {
				if (strcmp(fault_code,"Client") == 0) {
					add_property_string(obj, "faultcode", "Sender", 1);
					add_property_string(obj, "faultcodens", SOAP_1_2_ENV_NAMESPACE, 1);
				} else if (strcmp(fault_code,"Server") == 0) {
					add_property_string(obj, "faultcode", "Receiver", 1);
					add_property_string(obj, "faultcodens", SOAP_1_2_ENV_NAMESPACE, 1);
				} else if (strcmp(fault_code,"VersionMismatch") == 0 ||
				           strcmp(fault_code,"MustUnderstand") == 0 ||
				           strcmp(fault_code,"DataEncodingUnknown") == 0) {
					add_property_string(obj, "faultcode", fault_code, 1);
					add_property_string(obj, "faultcodens", SOAP_1_2_ENV_NAMESPACE, 1);
				} else {
					add_property_string(obj, "faultcode", fault_code, 1);
				}
			}
		}
	}
	if (fault_actor != NULL) {
		add_property_string(obj, "faultactor", fault_actor, 1);
	}
	if (fault_detail != NULL) {
		add_property_zval(obj, "detail", fault_detail);
	}
	if (name != NULL) {
		add_property_string(obj, "_name", name, 1);
	}
}

static void deserialize_parameters(xmlNodePtr params, sdlFunctionPtr function, int *num_params, zval ***parameters)
{
	int cur_param = 0,num_of_params = 0;
	zval **tmp_parameters = NULL;

	if (function != NULL) {
		sdlParamPtr *param;
		xmlNodePtr val;
		int	use_names = 0;

		if (function->requestParameters == NULL) {
			return;
		}
		num_of_params = zend_hash_num_elements(function->requestParameters);
		zend_hash_internal_pointer_reset(function->requestParameters);
		while (zend_hash_get_current_data(function->requestParameters, (void **)&param) == SUCCESS) {
			if (get_node(params, (*param)->paramName) != NULL) {
				use_names = 1;
			}
			zend_hash_move_forward(function->requestParameters);
		}
		if (use_names) {
			tmp_parameters = safe_emalloc(num_of_params, sizeof(zval *), 0);
			zend_hash_internal_pointer_reset(function->requestParameters);
			while (zend_hash_get_current_data(function->requestParameters, (void **)&param) == SUCCESS) {
				val = get_node(params, (*param)->paramName);
				if (!val) {
					/* TODO: may be "nil" is not OK? */
					MAKE_STD_ZVAL(tmp_parameters[cur_param]);
					ZVAL_NULL(tmp_parameters[cur_param]);
				} else {
					tmp_parameters[cur_param] = master_to_zval((*param)->encode, val);
				}
				cur_param++;

				zend_hash_move_forward(function->requestParameters);
			}
			(*parameters) = tmp_parameters;
			(*num_params) = num_of_params;
			return;
		}
	}
	if (params) {
		xmlNodePtr trav;

		num_of_params = 0;
		trav = params;
		while (trav != NULL) {
			if (trav->type == XML_ELEMENT_NODE) {
				num_of_params++;
			}
			trav = trav->next;
		}
		if (num_of_params > 0) {
			tmp_parameters = safe_emalloc(num_of_params, sizeof(zval *), 0);

			trav = params;
			while (trav != 0 && cur_param < num_of_params) {
				if (trav->type == XML_ELEMENT_NODE) {
					encodePtr enc;
					sdlParamPtr *param = NULL;
					if (function != NULL &&
					    zend_hash_index_find(function->requestParameters, cur_param, (void **)&param) == FAILURE) {
						TSRMLS_FETCH();
						soap_server_fault("Client", "Error cannot find parameter", NULL, NULL, NULL TSRMLS_CC);
					}
					if (param == NULL) {
						enc = NULL;
					} else {
						enc = (*param)->encode;
					}
					tmp_parameters[cur_param] = master_to_zval(enc, trav);
					cur_param++;
				}
				trav = trav->next;
			}
		}
	}
	if (num_of_params > cur_param) {
		TSRMLS_FETCH();
		soap_server_fault("Client","Missing parameter", NULL, NULL, NULL TSRMLS_CC);
	}
	(*parameters) = tmp_parameters;
	(*num_params) = num_of_params;
}

static sdlFunctionPtr find_function(sdlPtr sdl, xmlNodePtr func, zval* function_name)
{
	sdlFunctionPtr function;

	function = get_function(sdl, func->name);
	if (function && function->binding && function->binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)function->bindingAttributes;
		if (fnb->style == SOAP_DOCUMENT) {
			function = NULL;
		}
	}
	if (sdl != NULL && function == NULL) {
		function = get_doc_function(sdl, func);
	}

	INIT_ZVAL(*function_name);
	if (function != NULL) {
		ZVAL_STRING(function_name, (char *)function->functionName, 1);
	} else {
		ZVAL_STRING(function_name, (char *)func->name, 1);
	}

	return function;
}

static sdlFunctionPtr deserialize_function_call(sdlPtr sdl, xmlDocPtr request, char* actor, zval *function_name, int *num_params, zval ***parameters, int *version, soapHeader **headers TSRMLS_DC)
{
	char* envelope_ns = NULL;
	xmlNodePtr trav,env,head,body,func;
	xmlAttrPtr attr;
	sdlFunctionPtr function;

	/* Get <Envelope> element */
	env = NULL;
	trav = request->children;
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (env == NULL && node_is_equal_ex(trav,"Envelope",SOAP_1_1_ENV_NAMESPACE)) {
				env = trav;
				*version = SOAP_1_1;
				envelope_ns = SOAP_1_1_ENV_NAMESPACE;
				SOAP_GLOBAL(soap_version) = SOAP_1_1;
			} else if (env == NULL && node_is_equal_ex(trav,"Envelope",SOAP_1_2_ENV_NAMESPACE)) {
				env = trav;
				*version = SOAP_1_2;
				envelope_ns = SOAP_1_2_ENV_NAMESPACE;
				SOAP_GLOBAL(soap_version) = SOAP_1_2;
			} else {
				soap_server_fault("VersionMismatch", "Wrong Version", NULL, NULL, NULL TSRMLS_CC);
			}
		}
		trav = trav->next;
	}
	if (env == NULL) {
		soap_server_fault("Client", "looks like we got XML without \"Envelope\" element", NULL, NULL, NULL TSRMLS_CC);
	}

	attr = env->properties;
	while (attr != NULL) {
		if (attr->ns == NULL) {
			soap_server_fault("Client", "A SOAP Envelope element cannot have non Namespace qualified attributes", NULL, NULL, NULL TSRMLS_CC);
		} else if (attr_is_equal_ex(attr,"encodingStyle",SOAP_1_2_ENV_NAMESPACE)) {
			if (*version == SOAP_1_2) {
				soap_server_fault("Client", "encodingStyle cannot be specified on the Envelope", NULL, NULL, NULL TSRMLS_CC);
			} else if (strcmp(attr->children->content,SOAP_1_1_ENC_NAMESPACE) != 0) {
				soap_server_fault("Client", "Unknown data encoding style", NULL, NULL, NULL TSRMLS_CC);
			}
		}
		attr = attr->next;
	}

	/* Get <Header> element */
	head = NULL;
	trav = env->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal_ex(trav,"Header",envelope_ns)) {
		head = trav;
		trav = trav->next;
	}

	/* Get <Body> element */
	body = NULL;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal_ex(trav,"Body",envelope_ns)) {
		body = trav;
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (body == NULL) {
		soap_server_fault("Client", "Body must be present in a SOAP envelope", NULL, NULL, NULL TSRMLS_CC);
	}
	attr = body->properties;
	while (attr != NULL) {
		if (attr->ns == NULL) {
			if (*version == SOAP_1_2) {
				soap_server_fault("Client", "A SOAP Body element cannot have non Namespace qualified attributes", NULL, NULL, NULL TSRMLS_CC);
			}
		} else if (attr_is_equal_ex(attr,"encodingStyle",SOAP_1_2_ENV_NAMESPACE)) {
			if (*version == SOAP_1_2) {
				soap_server_fault("Client", "encodingStyle cannot be specified on the Body", NULL, NULL, NULL TSRMLS_CC);
			} else if (strcmp(attr->children->content,SOAP_1_1_ENC_NAMESPACE) != 0) {
				soap_server_fault("Client", "Unknown data encoding style", NULL, NULL, NULL TSRMLS_CC);
			}
		}
		attr = attr->next;
	}

	if (trav != NULL && *version == SOAP_1_2) {
		soap_server_fault("Client", "A SOAP 1.2 envelope can contain only Header and Body", NULL, NULL, NULL TSRMLS_CC);
	}

	func = NULL;
	trav = body->children;
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
/*
			if (func != NULL) {
				soap_server_fault("Client", "looks like we got \"Body\" with several functions call", NULL, NULL, NULL TSRMLS_CC);
			}
*/
			func = trav;
			break; /* FIXME: the rest of body is ignored */
		}
		trav = trav->next;
	}
	if (func == NULL) {
		function = get_doc_function(sdl, NULL);
		if (function != NULL) {
			INIT_ZVAL(*function_name);
			ZVAL_STRING(function_name, (char *)function->functionName, 1);
		} else {
			soap_server_fault("Client", "looks like we got \"Body\" without function call", NULL, NULL, NULL TSRMLS_CC);
		}
	} else {
		if (*version == SOAP_1_1) {
			attr = get_attribute_ex(func->properties,"encodingStyle",SOAP_1_1_ENV_NAMESPACE);
			if (attr && strcmp(attr->children->content,SOAP_1_1_ENC_NAMESPACE) != 0) {
				soap_server_fault("Client","Unknown Data Encoding Style", NULL, NULL, NULL TSRMLS_CC);
			}
		} else {
			attr = get_attribute_ex(func->properties,"encodingStyle",SOAP_1_2_ENV_NAMESPACE);
			if (attr && strcmp(attr->children->content,SOAP_1_2_ENC_NAMESPACE) != 0) {
				soap_server_fault("DataEncodingUnknown","Unknown Data Encoding Style", NULL, NULL, NULL TSRMLS_CC);
			}
		}
		function = find_function(sdl, func, function_name);
		if (sdl != NULL && function == NULL) {
			if (*version == SOAP_1_2) {
				soap_server_fault("rpc:ProcedureNotPresent","Procedure not present", NULL, NULL, NULL TSRMLS_CC);
			} else {
				php_error(E_ERROR, "Procedure '%s' not present", func->name);
			}
		}
	}

	*headers = NULL;
	if (head) {
		soapHeader *h, *last = NULL;

		attr = head->properties;
		while (attr != NULL) {
			if (attr->ns == NULL) {
				soap_server_fault("Client", "A SOAP Header element cannot have non Namespace qualified attributes", NULL, NULL, NULL TSRMLS_CC);
			} else if (attr_is_equal_ex(attr,"encodingStyle",SOAP_1_2_ENV_NAMESPACE)) {
				if (*version == SOAP_1_2) {
					soap_server_fault("Client", "encodingStyle cannot be specified on the Header", NULL, NULL, NULL TSRMLS_CC);
				} else if (strcmp(attr->children->content,SOAP_1_1_ENC_NAMESPACE) != 0) {
					soap_server_fault("Client", "Unknown data encoding style", NULL, NULL, NULL TSRMLS_CC);
				}
			}
			attr = attr->next;
		}
		trav = head->children;
		while (trav != NULL) {
			if (trav->type == XML_ELEMENT_NODE) {
				xmlNodePtr hdr_func = trav;
				xmlAttrPtr attr;
				int mustUnderstand = 0;

				if (*version == SOAP_1_1) {
					attr = get_attribute_ex(hdr_func->properties,"encodingStyle",SOAP_1_1_ENV_NAMESPACE);
					if (attr && strcmp(attr->children->content,SOAP_1_1_ENC_NAMESPACE) != 0) {
						soap_server_fault("Client","Unknown Data Encoding Style", NULL, NULL, NULL TSRMLS_CC);
					}
					attr = get_attribute_ex(hdr_func->properties,"actor",envelope_ns);
					if (attr != NULL) {
						if (strcmp(attr->children->content,SOAP_1_1_ACTOR_NEXT) != 0 &&
						    (actor == NULL || strcmp(attr->children->content,actor) != 0)) {
						  goto ignore_header;
						}
					}
				} else if (*version == SOAP_1_2) {
					attr = get_attribute_ex(hdr_func->properties,"encodingStyle",SOAP_1_2_ENV_NAMESPACE);
					if (attr && strcmp(attr->children->content,SOAP_1_2_ENC_NAMESPACE) != 0) {
						soap_server_fault("DataEncodingUnknown","Unknown Data Encoding Style", NULL, NULL, NULL TSRMLS_CC);
					}
					attr = get_attribute_ex(hdr_func->properties,"role",envelope_ns);
					if (attr != NULL) {
						if (strcmp(attr->children->content,SOAP_1_2_ACTOR_UNLIMATERECEIVER) != 0 &&
						    strcmp(attr->children->content,SOAP_1_2_ACTOR_NEXT) != 0 &&
						    (actor == NULL || strcmp(attr->children->content,actor) != 0)) {
						  goto ignore_header;
						}
					}
				}
				attr = get_attribute_ex(hdr_func->properties,"mustUnderstand",envelope_ns);
				if (attr) {
					if (strcmp(attr->children->content,"1") == 0 ||
					    strcmp(attr->children->content,"true") == 0) {
						mustUnderstand = 1;
					} else if (strcmp(attr->children->content,"0") == 0 ||
					           strcmp(attr->children->content,"false") == 0) {
						mustUnderstand = 0;
					} else {
						soap_server_fault("Client","mustUnderstand value is not boolean", NULL, NULL, NULL TSRMLS_CC);
					}
				}
				h = emalloc(sizeof(soapHeader));
				memset(h, 0, sizeof(soapHeader));
				h->mustUnderstand = mustUnderstand;
				h->function = find_function(sdl, hdr_func, &h->function_name);
				if (!h->function && sdl && function && function->binding && function->binding->bindingType == BINDING_SOAP) {
					sdlSoapBindingFunctionHeaderPtr *hdr;
					sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)function->bindingAttributes;
					if (fnb->input.headers) {
						smart_str key = {0};

						if (hdr_func->ns) {
							smart_str_appends(&key, hdr_func->ns->href);
							smart_str_appendc(&key, ':');
						}
						smart_str_appendl(&key, Z_STRVAL(h->function_name), Z_STRLEN(h->function_name));
						smart_str_0(&key);
						if (zend_hash_find(fnb->input.headers, key.c, key.len+1, (void**)&hdr) == SUCCESS) {
							h->hdr = *hdr;
						}
						smart_str_free(&key);
					}
				}
				if (h->hdr) {
					h->num_params = 1;
					h->parameters = emalloc(sizeof(zval*));
					h->parameters[0] = master_to_zval(h->hdr->encode, hdr_func);
				} else {
					if (h->function && h->function->binding && h->function->binding->bindingType == BINDING_SOAP) {
						sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)h->function->bindingAttributes;
						if (fnb->style == SOAP_RPC) {
							hdr_func = hdr_func->children;
						}
					}
					deserialize_parameters(hdr_func, h->function, &h->num_params, &h->parameters);
				}
				INIT_ZVAL(h->retval);
				if (last == NULL) {
					*headers = h;
				} else {
					last->next = h;
				}
				last = h;
			}
ignore_header:
			trav = trav->next;
		}
	}

	if (function && function->binding && function->binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)function->bindingAttributes;
		if (fnb->style == SOAP_RPC) {
			func = func->children;
		}
	} else {
		func = func->children;
	}
	deserialize_parameters(func, function, num_params, parameters);
	return function;
}

static int serialize_response_call2(xmlNodePtr body, sdlFunctionPtr function, char *function_name, char *uri, zval *ret, int version, int main TSRMLS_DC)
{
	xmlNodePtr method = NULL, param;
	sdlParamPtr parameter = NULL;
	int param_count;
	int style, use;
	xmlNsPtr ns = NULL;

	if (function != NULL && function->binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)function->bindingAttributes;

		style = fnb->style;
		use = fnb->output.use;
		if (style == SOAP_RPC) {
			ns = encode_add_ns(body, fnb->output.ns);
			if (function->responseName) {
				method = xmlNewChild(body, ns, function->responseName, NULL);
			} else if (function->responseParameters) {
				method = xmlNewChild(body, ns, function->functionName, NULL);
			}
		}
	} else {
		style = main?SOAP_RPC:SOAP_DOCUMENT;
		use = main?SOAP_ENCODED:SOAP_LITERAL;
		if (style == SOAP_RPC) {
			ns = encode_add_ns(body, uri);
			method = xmlNewChild(body, ns, function_name, NULL);
		}
	}

	if (function != NULL) {
		if (function->responseParameters) {
			param_count = zend_hash_num_elements(function->responseParameters);
		} else {
		  param_count = 0;
		}
	} else {
	  param_count = 1;
	}

	if (param_count == 1) {
		parameter = get_param(function, NULL, 0, TRUE);

		if (style == SOAP_RPC) {
		  xmlNode *rpc_result;
			if (main && version == SOAP_1_2) {
				xmlNs *rpc_ns = xmlNewNs(body, RPC_SOAP12_NAMESPACE, RPC_SOAP12_NS_PREFIX);
				rpc_result = xmlNewChild(method, rpc_ns, "result", NULL);
				param = serialize_parameter(parameter, ret, 0, "return", use, method TSRMLS_CC);
				xmlNodeSetContent(rpc_result,param->name);
			} else {
				param = serialize_parameter(parameter, ret, 0, "return", use, method TSRMLS_CC);
			}
		} else {
			param = serialize_parameter(parameter, ret, 0, "return", use, body TSRMLS_CC);
			if (function && function->binding->bindingType == BINDING_SOAP) {
				if (parameter && parameter->element) {
					ns = encode_add_ns(param, parameter->element->namens);
					xmlNodeSetName(param, parameter->element->name);
					xmlSetNs(param, ns);
				}
			} else if (strcmp(param->name,"return") == 0) {
				ns = encode_add_ns(param, uri);
				xmlNodeSetName(param, function_name);
				xmlSetNs(param, ns);
			}
		}
	} else if (param_count > 1 && Z_TYPE_P(ret) == IS_ARRAY) {
		HashPosition pos;
		zval **data;
		int i = 0;

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(ret), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(ret), (void **)&data, &pos) != FAILURE) {
			char *param_name = NULL;
			int   param_name_len;
			long  param_index = i;

			zend_hash_get_current_key_ex(Z_ARRVAL_P(ret), &param_name, &param_name_len, &param_index, 0, &pos);
			parameter = get_param(function, param_name, param_index, TRUE);
			if (style == SOAP_RPC) {
				param = serialize_parameter(parameter, *data, i, param_name, use, method TSRMLS_CC);
			} else {
				param = serialize_parameter(parameter, *data, i, param_name, use, body TSRMLS_CC);
				if (function && function->binding->bindingType == BINDING_SOAP) {
					if (parameter && parameter->element) {
						ns = encode_add_ns(param, parameter->element->namens);
						xmlNodeSetName(param, parameter->element->name);
						xmlSetNs(param, ns);
					}
				}
			}

			zend_hash_move_forward_ex(Z_ARRVAL_P(ret), &pos);
			i++;
		}
	}
	if (use == SOAP_ENCODED && version == SOAP_1_2 && method != NULL) {
		xmlSetNsProp(method, body->ns, "encodingStyle", SOAP_1_2_ENC_NAMESPACE);
	}
	return use;
}

static xmlDocPtr serialize_response_call(sdlFunctionPtr function, char *function_name, char *uri, zval *ret, soapHeader* headers, int version TSRMLS_DC)
{
	xmlDocPtr doc;
	xmlNodePtr envelope = NULL, body, param;
	xmlNsPtr ns = NULL;
	int use = SOAP_LITERAL;
	xmlNodePtr head = NULL;

	encode_reset_ns();

	doc = xmlNewDoc("1.0");
	doc->charset = XML_CHAR_ENCODING_UTF8;
	doc->encoding = xmlStrdup((xmlChar*)"UTF-8");

	if (version == SOAP_1_1) {
		envelope = xmlNewDocNode(doc, NULL, "Envelope", NULL);
		ns = xmlNewNs(envelope, SOAP_1_1_ENV_NAMESPACE, SOAP_1_1_ENV_NS_PREFIX);
		xmlSetNs(envelope,ns);
	} else if (version == SOAP_1_2) {
		envelope = xmlNewDocNode(doc, NULL, "Envelope", NULL);
		ns = xmlNewNs(envelope, SOAP_1_2_ENV_NAMESPACE, SOAP_1_2_ENV_NS_PREFIX);
		xmlSetNs(envelope,ns);
	} else {
		soap_server_fault("Server", "Unknown SOAP version", NULL, NULL, NULL TSRMLS_CC);
	}
	xmlDocSetRootElement(doc, envelope);

	if (Z_TYPE_P(ret) == IS_OBJECT &&
	    instanceof_function(Z_OBJCE_P(ret), soap_fault_class_entry TSRMLS_CC)) {
	  char *detail_name;
		HashTable* prop;
		zval **tmp;
		sdlFaultPtr fault = NULL;
		char *fault_ns = NULL;

		prop = Z_OBJPROP_P(ret);

		if (headers &&
		    zend_hash_find(prop, "headerfault", sizeof("headerfault"), (void**)&tmp) == SUCCESS) {
			xmlNodePtr head;
			encodePtr hdr_enc = NULL;
			int hdr_use = SOAP_LITERAL;
			zval *hdr_ret  = *tmp;
			char *hdr_ns   = headers->hdr?headers->hdr->ns:NULL;
			char *hdr_name = Z_STRVAL(headers->function_name);

			head = xmlNewChild(envelope, ns, "Header", NULL);
			if (Z_TYPE_P(hdr_ret) == IS_OBJECT &&
			    instanceof_function(Z_OBJCE_P(hdr_ret), soap_header_class_entry TSRMLS_CC)) {
				HashTable* ht = Z_OBJPROP_P(hdr_ret);
				zval **tmp;
				sdlSoapBindingFunctionHeaderPtr *hdr;
				smart_str key = {0};

				if (zend_hash_find(ht, "namespace", sizeof("namespace"), (void**)&tmp) == SUCCESS &&
			      Z_TYPE_PP(tmp) == IS_STRING) {
					smart_str_appendl(&key, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					smart_str_appendc(&key, ':');
					hdr_ns = Z_STRVAL_PP(tmp);
				}
				if (zend_hash_find(ht, "name", sizeof("name"), (void**)&tmp) == SUCCESS &&
				    Z_TYPE_PP(tmp) == IS_STRING) {
					smart_str_appendl(&key, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					hdr_name = Z_STRVAL_PP(tmp);
				}
				smart_str_0(&key);
				if (headers->hdr && headers->hdr->headerfaults &&
				    zend_hash_find(headers->hdr->headerfaults, key.c, key.len+1, (void**)&hdr) == SUCCESS) {
					hdr_enc = (*hdr)->encode;
					hdr_use = (*hdr)->use;
				}
				smart_str_free(&key);
				if (zend_hash_find(ht, "data", sizeof("data"), (void**)&tmp) == SUCCESS) {
					hdr_ret = *tmp;
				} else {
					hdr_ret = NULL;
				}
			}

			if (headers->function) {
				if (serialize_response_call2(head, headers->function, Z_STRVAL(headers->function_name), uri, hdr_ret, version, 0 TSRMLS_CC) == SOAP_ENCODED) {
					use = SOAP_ENCODED;
				}
			} else {
				xmlNodePtr xmlHdr = master_to_xml(hdr_enc, hdr_ret, hdr_use, head);
				if (hdr_name) {
					xmlNodeSetName(xmlHdr,hdr_name);
				}
				if (hdr_ns) {
					xmlNsPtr nsptr = encode_add_ns(xmlHdr,hdr_ns);
					xmlSetNs(xmlHdr, nsptr);
				}
			}
		}

		body = xmlNewChild(envelope, ns, "Body", NULL);
		param = xmlNewChild(body, ns, "Fault", NULL);

		if (zend_hash_find(prop, "faultcodens", sizeof("faultcodens"), (void**)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_STRING) {
			fault_ns = Z_STRVAL_PP(tmp);
		}
		use = SOAP_LITERAL;
		if (zend_hash_find(prop, "_name", sizeof("_name"), (void**)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_STRING) {
			sdlFaultPtr *tmp_fault;
			if (function && function->faults &&
			    zend_hash_find(function->faults, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp)+1, (void**)&tmp_fault) == SUCCESS) {
			  fault = *tmp_fault;
				if (function->binding &&
				    function->binding->bindingType == BINDING_SOAP &&
				    fault->bindingAttributes) {
					sdlSoapBindingFunctionFaultPtr fb = (sdlSoapBindingFunctionFaultPtr)fault->bindingAttributes;
					use = fb->use;
					if (fault_ns == NULL) {
						fault_ns = fb->ns;
					}
				}
			}
		} else if (function && function->faults &&
		           zend_hash_num_elements(function->faults) == 1) {

			zend_hash_internal_pointer_reset(function->faults);
			zend_hash_get_current_data(function->faults, (void**)&fault);
			fault = *(sdlFaultPtr*)fault;
			if (function->binding &&
			    function->binding->bindingType == BINDING_SOAP &&
			    fault->bindingAttributes) {
				sdlSoapBindingFunctionFaultPtr fb = (sdlSoapBindingFunctionFaultPtr)fault->bindingAttributes;
				use = fb->use;
				if (fault_ns == NULL) {
				  fault_ns = fb->ns;
				}
			}
		}

		if (fault_ns == NULL &&
		    fault && 
		    fault->details && 
		    zend_hash_num_elements(fault->details) == 1) {
			sdlParamPtr sparam;

			zend_hash_internal_pointer_reset(fault->details);
			zend_hash_get_current_data(fault->details, (void**)&sparam);
			sparam = *(sdlParamPtr*)sparam;
			if (sparam->element) {
				fault_ns = sparam->element->namens;
			}
		}

		if (version == SOAP_1_1) {
			if (zend_hash_find(prop, "faultcode", sizeof("faultcode"), (void**)&tmp) == SUCCESS) {
				int new_len;
				xmlNodePtr node = xmlNewNode(NULL, "faultcode");
				char *str = php_escape_html_entities(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), &new_len, 0, 0, NULL TSRMLS_CC);
				xmlAddChild(param, node);
				if (fault_ns) {
					xmlNsPtr nsptr = encode_add_ns(node, fault_ns);
					xmlNodeSetContent(node, xmlBuildQName(str, nsptr->prefix, NULL, 0));
				} else {	
					xmlNodeSetContentLen(node, str, new_len);
				}
				efree(str);
			}
			if (zend_hash_find(prop, "faultstring", sizeof("faultstring"), (void**)&tmp) == SUCCESS) {
				xmlNodePtr node = master_to_xml(get_conversion(IS_STRING), *tmp, SOAP_LITERAL, param);
				xmlNodeSetName(node, "faultstring");
			}
			if (zend_hash_find(prop, "faultactor", sizeof("faultactor"), (void**)&tmp) == SUCCESS) {
				xmlNodePtr node = master_to_xml(get_conversion(IS_STRING), *tmp, SOAP_LITERAL, param);
				xmlNodeSetName(node, "faultactor");
			}
			detail_name = "detail";
		} else {
			if (zend_hash_find(prop, "faultcode", sizeof("faultcode"), (void**)&tmp) == SUCCESS) {
				int new_len;
				xmlNodePtr node = xmlNewChild(param, ns, "Code", NULL);
				char *str = php_escape_html_entities(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), &new_len, 0, 0, NULL TSRMLS_CC);
				node = xmlNewChild(node, ns, "Value", NULL);
				if (fault_ns) {
					xmlNsPtr nsptr = encode_add_ns(node, fault_ns);
					xmlNodeSetContent(node, xmlBuildQName(str, nsptr->prefix, NULL, 0));
				} else {	
					xmlNodeSetContentLen(node, str, new_len);
				}
				efree(str);
			}
			if (zend_hash_find(prop, "faultstring", sizeof("faultstring"), (void**)&tmp) == SUCCESS) {
				xmlNodePtr node = xmlNewChild(param, ns, "Reason", NULL);
				node = master_to_xml(get_conversion(IS_STRING), *tmp, SOAP_LITERAL, node);
				xmlNodeSetName(node, "Text");
				xmlSetNs(node, ns);
			}
			detail_name = SOAP_1_2_ENV_NS_PREFIX":Detail";
		}
		if (fault && fault->details && zend_hash_num_elements(fault->details) == 1) {
			xmlNodePtr node;
			zval *detail = NULL;
			sdlParamPtr sparam;
			xmlNodePtr x;

			if (zend_hash_find(prop, "detail", sizeof("detail"), (void**)&tmp) == SUCCESS &&
			    Z_TYPE_PP(tmp) != IS_NULL) {
				detail = *tmp;
			}
			node = xmlNewNode(NULL, detail_name);
			xmlAddChild(param, node);

			zend_hash_internal_pointer_reset(fault->details);
			zend_hash_get_current_data(fault->details, (void**)&sparam);
			sparam = *(sdlParamPtr*)sparam;

			if (detail &&
			    Z_TYPE_P(detail) == IS_OBJECT &&
			    sparam->element &&
			    zend_hash_num_elements(Z_OBJPROP_P(detail)) == 1 &&
			    zend_hash_find(Z_OBJPROP_P(detail), sparam->element->name, strlen(sparam->element->name)+1, (void**)&tmp) == SUCCESS) {
				detail = *tmp;
			}

			x = serialize_parameter(sparam, detail, 1, NULL, use, node TSRMLS_CC);

			if (function &&
			    function->binding &&
			    function->binding->bindingType == BINDING_SOAP &&
			    function->bindingAttributes) {
				sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)function->bindingAttributes;
				if (fnb->style == SOAP_RPC && !sparam->element) {
				  if (fault->bindingAttributes) {
						sdlSoapBindingFunctionFaultPtr fb = (sdlSoapBindingFunctionFaultPtr)fault->bindingAttributes;
						if (fb->ns) {
							xmlNsPtr ns = encode_add_ns(x, fb->ns);
							xmlSetNs(x, ns);
						}
					}
				} else {
					if (sparam->element) {
						xmlNsPtr ns = encode_add_ns(x, sparam->element->namens);
						xmlNodeSetName(x, sparam->element->name);
						xmlSetNs(x, ns);
					}
				}
			}
			if (use == SOAP_ENCODED && version == SOAP_1_2) {
				xmlSetNsProp(x, envelope->ns, "encodingStyle", SOAP_1_2_ENC_NAMESPACE);
			}
		} else if (zend_hash_find(prop, "detail", sizeof("detail"), (void**)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) != IS_NULL) {
			serialize_zval(*tmp, NULL, detail_name, use, param TSRMLS_CC);
		}
	} else {

		if (headers) {
			soapHeader *h;

			head = xmlNewChild(envelope, ns, "Header", NULL);
			h = headers;
			while (h != NULL) {
				if (Z_TYPE(h->retval) != IS_NULL) {
					encodePtr hdr_enc = NULL;
					int hdr_use = SOAP_LITERAL;
					zval *hdr_ret = &h->retval;
					char *hdr_ns   = h->hdr?h->hdr->ns:NULL;
					char *hdr_name = Z_STRVAL(h->function_name);


					if (Z_TYPE(h->retval) == IS_OBJECT &&
					    instanceof_function(Z_OBJCE(h->retval), soap_header_class_entry TSRMLS_CC)) {
						HashTable* ht = Z_OBJPROP(h->retval);
						zval **tmp;
						sdlSoapBindingFunctionHeaderPtr *hdr;
						smart_str key = {0};

						if (zend_hash_find(ht, "namespace", sizeof("namespace"), (void**)&tmp) == SUCCESS &&
					      Z_TYPE_PP(tmp) == IS_STRING) {
							smart_str_appendl(&key, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
							smart_str_appendc(&key, ':');
							hdr_ns = Z_STRVAL_PP(tmp);
						}
						if (zend_hash_find(ht, "name", sizeof("name"), (void**)&tmp) == SUCCESS &&
						    Z_TYPE_PP(tmp) == IS_STRING) {
							smart_str_appendl(&key, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
							hdr_name = Z_STRVAL_PP(tmp);
						}
						smart_str_0(&key);
						if (function && function->binding && function->binding->bindingType == BINDING_SOAP) {
							sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)function->bindingAttributes;

							if (fnb->output.headers &&
							    zend_hash_find(fnb->output.headers, key.c, key.len+1, (void**)&hdr) == SUCCESS) {
								hdr_enc = (*hdr)->encode;
								hdr_use = (*hdr)->use;
							}
						}
						smart_str_free(&key);
						if (zend_hash_find(ht, "data", sizeof("data"), (void**)&tmp) == SUCCESS) {
							hdr_ret = *tmp;
						} else {
							hdr_ret = NULL;
						}
					}

					if (h->function) {
						if (serialize_response_call2(head, h->function, Z_STRVAL(h->function_name), uri, hdr_ret, version, 0 TSRMLS_CC) == SOAP_ENCODED) {
							use = SOAP_ENCODED;
						}
					} else {
						xmlNodePtr xmlHdr = master_to_xml(hdr_enc, hdr_ret, hdr_use, head);
						if (hdr_name) {
							xmlNodeSetName(xmlHdr,hdr_name);
						}
						if (hdr_ns) {
							xmlNsPtr nsptr = encode_add_ns(xmlHdr,hdr_ns);
							xmlSetNs(xmlHdr, nsptr);
						}
					}
				}
				h = h->next;
			}

			if (head->children == NULL) {
				xmlUnlinkNode(head);
				xmlFreeNode(head);
			}
		}

		body = xmlNewChild(envelope, ns, "Body", NULL);

		if (serialize_response_call2(body, function, function_name, uri, ret, version, 1 TSRMLS_CC) == SOAP_ENCODED) {
			use = SOAP_ENCODED;
		}

	}

	if (use == SOAP_ENCODED) {
		xmlNewNs(envelope, XSD_NAMESPACE, XSD_NS_PREFIX);
		if (version == SOAP_1_1) {
			xmlNewNs(envelope, SOAP_1_1_ENC_NAMESPACE, SOAP_1_1_ENC_NS_PREFIX);
			xmlSetNsProp(envelope, envelope->ns, "encodingStyle", SOAP_1_1_ENC_NAMESPACE);
		} else if (version == SOAP_1_2) {
			xmlNewNs(envelope, SOAP_1_2_ENC_NAMESPACE, SOAP_1_2_ENC_NS_PREFIX);
		}
	}

	if (function && function->responseName == NULL && 
	    body->children == NULL && head == NULL) {
		xmlFreeDoc(doc);
		return NULL;
	}
	return doc;
}

static xmlDocPtr serialize_function_call(zval *this_ptr, sdlFunctionPtr function, char *function_name, char *uri, zval **arguments, int arg_count, int version, HashTable *soap_headers TSRMLS_DC)
{
	xmlDoc *doc;
	xmlNodePtr envelope = NULL, body, method = NULL, head = NULL;
	xmlNsPtr ns = NULL;
	zval **zstyle, **zuse;
	int i, style, use;
	HashTable *hdrs = NULL;

	encode_reset_ns();

	doc = xmlNewDoc("1.0");
	doc->encoding = xmlStrdup((xmlChar*)"UTF-8");
	doc->charset = XML_CHAR_ENCODING_UTF8;
	if (version == SOAP_1_1) {
		envelope = xmlNewDocNode(doc, NULL, "Envelope", NULL);
		ns = xmlNewNs(envelope, SOAP_1_1_ENV_NAMESPACE, SOAP_1_1_ENV_NS_PREFIX);
		xmlSetNs(envelope,ns);
	} else if (version == SOAP_1_2) {
		envelope = xmlNewDocNode(doc, NULL, "Envelope", NULL);
		ns = xmlNewNs(envelope, SOAP_1_2_ENV_NAMESPACE, SOAP_1_2_ENV_NS_PREFIX);
		xmlSetNs(envelope,ns);
	} else {
		soap_error0(E_ERROR, "Unknown SOAP version");
	}
	xmlDocSetRootElement(doc, envelope);

	if (soap_headers) {
		head = xmlNewChild(envelope, ns, "Header", NULL);
	}

	body = xmlNewChild(envelope, ns, "Body", NULL);

	if (function && function->binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)function->bindingAttributes;

		hdrs = fnb->input.headers;
		style = fnb->style;
		/*FIXME: how to pass method name if style is SOAP_DOCUMENT */
		/*style = SOAP_RPC;*/
		use = fnb->input.use;
		if (style == SOAP_RPC) {
			ns = encode_add_ns(body, fnb->input.ns);
			if (function->requestName) {
				method = xmlNewChild(body, ns, function->requestName, NULL);
			} else {
				method = xmlNewChild(body, ns, function->functionName, NULL);
			}
		}
	} else {
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "style", sizeof("style"), (void **)&zstyle) == SUCCESS) {
			style = Z_LVAL_PP(zstyle);
		} else {
			style = SOAP_RPC;
		}
		/*FIXME: how to pass method name if style is SOAP_DOCUMENT */
		/*style = SOAP_RPC;*/
		if (style == SOAP_RPC) {
			ns = encode_add_ns(body, uri);
			method = xmlNewChild(body, ns, function_name, NULL);
		}

		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "use", sizeof("use"), (void **)&zuse) == SUCCESS &&
			  Z_LVAL_PP(zuse) == SOAP_LITERAL) {
			use = SOAP_LITERAL;
		} else {
			use = SOAP_ENCODED;
		}
	}

	for (i = 0;i < arg_count;i++) {
		xmlNodePtr param;
		sdlParamPtr parameter = get_param(function, NULL, i, FALSE);

		if (style == SOAP_RPC) {
			param = serialize_parameter(parameter, arguments[i], i, NULL, use, method TSRMLS_CC);
		} else if (style == SOAP_DOCUMENT) {
			param = serialize_parameter(parameter, arguments[i], i, NULL, use, body TSRMLS_CC);
			if (function && function->binding->bindingType == BINDING_SOAP) {
				if (parameter && parameter->element) {
					ns = encode_add_ns(param, parameter->element->namens);
					xmlNodeSetName(param, parameter->element->name);
					xmlSetNs(param, ns);
				}
			}
		}
	}

	if (function && function->requestParameters) {
		int n = zend_hash_num_elements(function->requestParameters);

		if (n > arg_count) {
			for (i = arg_count; i < n; i++) {
				xmlNodePtr param;
				sdlParamPtr parameter = get_param(function, NULL, i, FALSE);

				if (style == SOAP_RPC) {
					param = serialize_parameter(parameter, NULL, i, NULL, use, method TSRMLS_CC);
				} else if (style == SOAP_DOCUMENT) {
					param = serialize_parameter(parameter, NULL, i, NULL, use, body TSRMLS_CC);
					if (function && function->binding->bindingType == BINDING_SOAP) {
						if (parameter && parameter->element) {
							ns = encode_add_ns(param, parameter->element->namens);
							xmlNodeSetName(param, parameter->element->name);
							xmlSetNs(param, ns);
						}
					}
				}
			}
		}
	}

	if (head) {
		zval** header;

		zend_hash_internal_pointer_reset(soap_headers);
		while (zend_hash_get_current_data(soap_headers,(void**)&header) == SUCCESS) {
			HashTable *ht = Z_OBJPROP_PP(header);
			zval **name, **ns, **tmp;

			if (zend_hash_find(ht, "name", sizeof("name"), (void**)&name) == SUCCESS &&
			    Z_TYPE_PP(name) == IS_STRING &&
			    zend_hash_find(ht, "namespace", sizeof("namespace"), (void**)&ns) == SUCCESS &&
			    Z_TYPE_PP(ns) == IS_STRING) {
				xmlNodePtr h;
				xmlNsPtr nsptr;
				int hdr_use = SOAP_LITERAL;
				encodePtr enc = NULL;

				if (hdrs) {
					smart_str key = {0};
					sdlSoapBindingFunctionHeaderPtr *hdr;

					smart_str_appendl(&key, Z_STRVAL_PP(ns), Z_STRLEN_PP(ns));
					smart_str_appendc(&key, ':');
					smart_str_appendl(&key, Z_STRVAL_PP(name), Z_STRLEN_PP(name));
					smart_str_0(&key);
					if (zend_hash_find(hdrs, key.c, key.len+1,(void**)&hdr) == SUCCESS) {
						hdr_use = (*hdr)->use;
						enc = (*hdr)->encode;
						if (hdr_use == SOAP_ENCODED) {
							use = SOAP_ENCODED;
						}
					}
					smart_str_free(&key);
				}

				if (zend_hash_find(ht, "data", sizeof("data"), (void**)&tmp) == SUCCESS) {
					h = master_to_xml(enc, *tmp, hdr_use, head);
					xmlNodeSetName(h, Z_STRVAL_PP(name));
				} else {
					h = xmlNewNode(NULL, Z_STRVAL_PP(name));
					xmlAddChild(head,h);
				}
				nsptr = encode_add_ns(h,Z_STRVAL_PP(ns));
				xmlSetNs(h, nsptr);

				if (zend_hash_find(ht, "mustUnderstand", sizeof("mustUnderstand"), (void**)&tmp) == SUCCESS &&
				    Z_TYPE_PP(tmp) == IS_BOOL && Z_LVAL_PP(tmp)) {
					if (version == SOAP_1_1) {
						xmlSetProp(h, SOAP_1_1_ENV_NS_PREFIX":mustUnderstand","1");
					} else {
						xmlSetProp(h, SOAP_1_2_ENV_NS_PREFIX":mustUnderstand","true");
					}
				}
				if (zend_hash_find(ht, "actor", sizeof("actor"), (void**)&tmp) == SUCCESS) {
					if (Z_TYPE_PP(tmp) == IS_STRING) {
						if (version == SOAP_1_1) {
							xmlSetProp(h, SOAP_1_1_ENV_NS_PREFIX":actor",Z_STRVAL_PP(tmp));
						} else {
							xmlSetProp(h, SOAP_1_2_ENV_NS_PREFIX":role",Z_STRVAL_PP(tmp));
						}
					} else if (Z_TYPE_PP(tmp) == IS_LONG) {
						if (version == SOAP_1_1) {
							if (Z_LVAL_PP(tmp) == SOAP_ACTOR_NEXT) {
								xmlSetProp(h, SOAP_1_1_ENV_NS_PREFIX":actor",SOAP_1_1_ACTOR_NEXT);
							}
						} else {
							if (Z_LVAL_PP(tmp) == SOAP_ACTOR_NEXT) {
								xmlSetProp(h, SOAP_1_2_ENV_NS_PREFIX":role",SOAP_1_2_ACTOR_NEXT);
							} else if (Z_LVAL_PP(tmp) == SOAP_ACTOR_NONE) {
								xmlSetProp(h, SOAP_1_2_ENV_NS_PREFIX":role",SOAP_1_2_ACTOR_NONE);
							} else if (Z_LVAL_PP(tmp) == SOAP_ACTOR_UNLIMATERECEIVER) {
								xmlSetProp(h, SOAP_1_2_ENV_NS_PREFIX":role",SOAP_1_2_ACTOR_UNLIMATERECEIVER);
							}
						}
					}
				}
			}
			zend_hash_move_forward(soap_headers);
		}
	}

	if (use == SOAP_ENCODED) {
		xmlNewNs(envelope, XSD_NAMESPACE, XSD_NS_PREFIX);
		if (version == SOAP_1_1) {
			xmlNewNs(envelope, SOAP_1_1_ENC_NAMESPACE, SOAP_1_1_ENC_NS_PREFIX);
			xmlSetNsProp(envelope, envelope->ns, "encodingStyle", SOAP_1_1_ENC_NAMESPACE);
		} else if (version == SOAP_1_2) {
			xmlNewNs(envelope, SOAP_1_2_ENC_NAMESPACE, SOAP_1_2_ENC_NS_PREFIX);
			if (method) {
				xmlSetNsProp(method, envelope->ns, "encodingStyle", SOAP_1_2_ENC_NAMESPACE);
			}
		}
	}

	return doc;
}

static xmlNodePtr serialize_parameter(sdlParamPtr param, zval *param_val, int index, char *name, int style, xmlNodePtr parent TSRMLS_DC)
{
	char *paramName;
	xmlNodePtr xmlParam;
	char paramNameBuf[10];

	if (param_val &&
	    Z_TYPE_P(param_val) == IS_OBJECT &&
	    Z_OBJCE_P(param_val) == soap_param_class_entry) {
		zval **param_name;
		zval **param_data;

		if (zend_hash_find(Z_OBJPROP_P(param_val), "param_name", sizeof("param_name"), (void **)&param_name) == SUCCESS &&
		    zend_hash_find(Z_OBJPROP_P(param_val), "param_data", sizeof("param_data"), (void **)&param_data) == SUCCESS) {
			param_val = *param_data;
			name = Z_STRVAL_PP(param_name);
		}
	}

	if (param != NULL && param->paramName != NULL) {
		paramName = param->paramName;
	} else {
		if (name == NULL) {
			paramName = paramNameBuf;
			sprintf(paramName,"param%d",index);
		} else {
			paramName = name;
		}
	}

	xmlParam = serialize_zval(param_val, param, paramName, style, parent TSRMLS_CC);

	return xmlParam;
}

static xmlNodePtr serialize_zval(zval *val, sdlParamPtr param, char *paramName, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr xmlParam;
	encodePtr enc;
	zval defval;

	if (param != NULL) {
		enc = param->encode;
		if (val == NULL) {
			if (param->element) {
				if (param->element->fixed) {
					ZVAL_STRING(&defval, param->element->fixed, 0);
					val = &defval;
				} else if (param->element->def && !param->element->nillable) {
					ZVAL_STRING(&defval, param->element->def, 0);
					val = &defval;
				}
			}
		}
	} else {
		enc = NULL;
	}
	xmlParam = master_to_xml(enc, val, style, parent);
	if (!strcmp(xmlParam->name, "BOGUS")) {
		xmlNodeSetName(xmlParam, paramName);
	}
	return xmlParam;
}

static sdlParamPtr get_param(sdlFunctionPtr function, char *param_name, int index, int response)
{
	sdlParamPtr *tmp;
	HashTable   *ht;

	if (function == NULL) {
		return NULL;
	}

	if (response == FALSE) {
		ht = function->requestParameters;
	} else {
		ht = function->responseParameters;
	}

	if (ht == NULL) {
	  return NULL;
	}

	if (param_name != NULL) {
		if (zend_hash_find(ht, param_name, strlen(param_name), (void **)&tmp) != FAILURE) {
			return *tmp;
		} else {
			HashPosition pos;
		
			zend_hash_internal_pointer_reset_ex(ht, &pos);
			while (zend_hash_get_current_data_ex(ht, (void **)&tmp, &pos) != FAILURE) {
				if ((*tmp)->paramName && strcmp(param_name, (*tmp)->paramName) == 0) {
					return *tmp;
				}
				zend_hash_move_forward_ex(ht, &pos);
			}
		}
	} else {
		if (zend_hash_index_find(ht, index, (void **)&tmp) != FAILURE) {
			return (*tmp);
		}
	}
	return NULL;
}

static sdlFunctionPtr get_function(sdlPtr sdl, const char *function_name)
{
	sdlFunctionPtr *tmp;

	int len = strlen(function_name);
	char *str = estrndup(function_name,len);
	php_strtolower(str,len);
	if (sdl != NULL) {
		if (zend_hash_find(&sdl->functions, str, len+1, (void **)&tmp) != FAILURE) {
			efree(str);
			return (*tmp);
		} else if (sdl->requests != NULL && zend_hash_find(sdl->requests, str, len+1, (void **)&tmp) != FAILURE) {
			efree(str);
			return (*tmp);
		}
	}
	efree(str);
	return NULL;
}

static sdlFunctionPtr get_doc_function(sdlPtr sdl, xmlNodePtr params)
{
	if (sdl) {
		sdlFunctionPtr *tmp;
		sdlParamPtr    *param;

		zend_hash_internal_pointer_reset(&sdl->functions);
		while (zend_hash_get_current_data(&sdl->functions, (void**)&tmp) == SUCCESS) {
			if ((*tmp)->binding && (*tmp)->binding->bindingType == BINDING_SOAP) {
				sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)(*tmp)->bindingAttributes;
				if (fnb->style == SOAP_DOCUMENT) {
					if (params == NULL) {
						if ((*tmp)->requestParameters == NULL ||
						    zend_hash_num_elements((*tmp)->requestParameters) == 0) {
						  return *tmp;
						}
					} else if ((*tmp)->requestParameters != NULL &&
					           zend_hash_num_elements((*tmp)->requestParameters) > 0) {
						int ok = 1;
						xmlNodePtr node = params;

						zend_hash_internal_pointer_reset((*tmp)->requestParameters);
						while (zend_hash_get_current_data((*tmp)->requestParameters, (void**)&param) == SUCCESS) {
							if ((*param)->element) {
								if (strcmp((*param)->element->name,node->name) != 0) {
									ok = 0;
									break;
								}
								if ((*param)->element->namens != NULL && node->ns != NULL) {
									if (strcmp((*param)->element->namens,node->ns->href) != 0) {
										ok = 0;
										break;
									}
								} else if ((void*)(*param)->element->namens != (void*)node->ns) {
									ok = 0;
									break;
								}
							} else if (strcmp((*param)->paramName,node->name) != 0) {
								ok = 0;
								break;
							}
							zend_hash_move_forward((*tmp)->requestParameters);
						}
						if (ok /*&& node == NULL*/) {
							return (*tmp);
						}
					}
				}
			}
			zend_hash_move_forward(&sdl->functions);
		}
	}
	return NULL;
}

static void function_to_string(sdlFunctionPtr function, smart_str *buf)
{
	int i = 0;
	HashPosition pos;
	sdlParamPtr *param;

	if (function->responseParameters &&
	    zend_hash_num_elements(function->responseParameters) > 0) {
		if (zend_hash_num_elements(function->responseParameters) == 1) {
			zend_hash_internal_pointer_reset(function->responseParameters);
			zend_hash_get_current_data(function->responseParameters, (void**)&param);
			if ((*param)->encode && (*param)->encode->details.type_str) {
				smart_str_appendl(buf, (*param)->encode->details.type_str, strlen((*param)->encode->details.type_str));
				smart_str_appendc(buf, ' ');
			} else {
				smart_str_appendl(buf, "UNKNOWN ", 8);
			}
		} else {
			i = 0;
			smart_str_appendl(buf, "list(", 5);
			zend_hash_internal_pointer_reset_ex(function->responseParameters, &pos);
			while (zend_hash_get_current_data_ex(function->responseParameters, (void **)&param, &pos) != FAILURE) {
				if (i > 0) {
					smart_str_appendl(buf, ", ", 2);
				}
				if ((*param)->encode && (*param)->encode->details.type_str) {
					smart_str_appendl(buf, (*param)->encode->details.type_str, strlen((*param)->encode->details.type_str));
				} else {
					smart_str_appendl(buf, "UNKNOWN", 7);
				}
				smart_str_appendl(buf, " $", 2);
				smart_str_appendl(buf, (*param)->paramName, strlen((*param)->paramName));
				zend_hash_move_forward_ex(function->responseParameters, &pos);
				i++;
			}
			smart_str_appendl(buf, ") ", 2);
		}
	} else {
		smart_str_appendl(buf, "void ", 5);
	}

	smart_str_appendl(buf, function->functionName, strlen(function->functionName));

	smart_str_appendc(buf, '(');
	if (function->requestParameters) {
		i = 0;
		zend_hash_internal_pointer_reset_ex(function->requestParameters, &pos);
		while (zend_hash_get_current_data_ex(function->requestParameters, (void **)&param, &pos) != FAILURE) {
			if (i > 0) {
				smart_str_appendl(buf, ", ", 2);
			}
			if ((*param)->encode && (*param)->encode->details.type_str) {
				smart_str_appendl(buf, (*param)->encode->details.type_str, strlen((*param)->encode->details.type_str));
			} else {
				smart_str_appendl(buf, "UNKNOWN", 7);
			}
			smart_str_appendl(buf, " $", 2);
			smart_str_appendl(buf, (*param)->paramName, strlen((*param)->paramName));
			zend_hash_move_forward_ex(function->requestParameters, &pos);
			i++;
		}
	}
	smart_str_appendc(buf, ')');
	smart_str_0(buf);
}

static void model_to_string(sdlContentModelPtr model, smart_str *buf, int level)
{
	int i;

	switch (model->kind) {
		case XSD_CONTENT_ELEMENT:
			type_to_string(model->u.element, buf, level);
			smart_str_appendl(buf, ";\n", 2);
			break;
		case XSD_CONTENT_ANY:
			for (i = 0;i < level;i++) {
				smart_str_appendc(buf, ' ');
			}
			smart_str_appendl(buf, "<anyXML> any;\n", sizeof("<anyXML> any;\n")-1);
			break;
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_ALL:
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr *tmp;

			zend_hash_internal_pointer_reset(model->u.content);
			while (zend_hash_get_current_data(model->u.content, (void**)&tmp) == SUCCESS) {
				model_to_string(*tmp, buf, level);
				zend_hash_move_forward(model->u.content);
			}
			break;
		}
		case XSD_CONTENT_GROUP:
			model_to_string(model->u.group->model, buf, level);
		default:
		  break;
	}
}

static void type_to_string(sdlTypePtr type, smart_str *buf, int level)
{
	int i;
	smart_str spaces = {0};
	HashPosition pos;

	for (i = 0;i < level;i++) {
		smart_str_appendc(&spaces, ' ');
	}
	smart_str_appendl(buf, spaces.c, spaces.len);

	switch (type->kind) {
		case XSD_TYPEKIND_SIMPLE:
		case XSD_TYPEKIND_LIST:
		case XSD_TYPEKIND_UNION:
			if (type->encode) {
				smart_str_appendl(buf, type->encode->details.type_str, strlen(type->encode->details.type_str));
				smart_str_appendc(buf, ' ');
			} else {
				smart_str_appendl(buf, "anyType ", sizeof("anyType ")-1);
			}
			smart_str_appendl(buf, type->name, strlen(type->name));
			break;
		case XSD_TYPEKIND_COMPLEX:
		case XSD_TYPEKIND_RESTRICTION:
		case XSD_TYPEKIND_EXTENSION:
			if (type->encode &&
			    (type->encode->details.type == IS_ARRAY ||
			     type->encode->details.type == SOAP_ENC_ARRAY)) {
			  sdlAttributePtr *attr;
			  sdlExtraAttributePtr *ext;

				if (type->attributes &&
				    zend_hash_find(type->attributes, SOAP_1_1_ENC_NAMESPACE":arrayType",
				      sizeof(SOAP_1_1_ENC_NAMESPACE":arrayType"),
				      (void **)&attr) == SUCCESS &&
				      zend_hash_find((*attr)->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType"), (void **)&ext) == SUCCESS) {
					char *end = strchr((*ext)->val, '[');
					int len;
					if (end == NULL) {
						len = strlen((*ext)->val);
					} else {
						len = end-(*ext)->val;
					}
					if (len == 0) {
						smart_str_appendl(buf, "anyType", sizeof("anyType")-1);
					} else {
						smart_str_appendl(buf, (*ext)->val, len);
					}
					smart_str_appendc(buf, ' ');
					smart_str_appendl(buf, type->name, strlen(type->name));
					if (end != NULL) {
						smart_str_appends(buf, end);
					}
				} else {
					sdlTypePtr elementType;
					if (type->attributes &&
					    zend_hash_find(type->attributes, SOAP_1_2_ENC_NAMESPACE":itemType",
					      sizeof(SOAP_1_2_ENC_NAMESPACE":itemType"),
					      (void **)&attr) == SUCCESS &&
					      zend_hash_find((*attr)->extraAttributes, WSDL_NAMESPACE":itemType", sizeof(WSDL_NAMESPACE":arrayType"), (void **)&ext) == SUCCESS) {
						smart_str_appends(buf, (*ext)->val);
						smart_str_appendc(buf, ' ');
					} else if (type->elements &&
					           zend_hash_num_elements(type->elements) == 1 &&
					           (zend_hash_internal_pointer_reset(type->elements),
					            zend_hash_get_current_data(type->elements, (void**)&elementType) == SUCCESS) &&
					           (elementType = *(sdlTypePtr*)elementType) != NULL &&
					           elementType->encode && elementType->encode->details.type_str) {
						smart_str_appends(buf, elementType->encode->details.type_str);
						smart_str_appendc(buf, ' ');
					} else {
						smart_str_appendl(buf, "anyType ", 8);
					}
					smart_str_appendl(buf, type->name, strlen(type->name));
					if (type->attributes &&
					    zend_hash_find(type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
					      sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
					      (void **)&attr) == SUCCESS &&
					      zend_hash_find((*attr)->extraAttributes, WSDL_NAMESPACE":itemType", sizeof(WSDL_NAMESPACE":arraySize"), (void **)&ext) == SUCCESS) {
						smart_str_appendc(buf, '[');
						smart_str_appends(buf, (*ext)->val);
						smart_str_appendc(buf, ']');
					} else {
						smart_str_appendl(buf, "[]", 2);
					}
				}
			} else {
				smart_str_appendl(buf, "struct ", 7);
				smart_str_appendl(buf, type->name, strlen(type->name));
				smart_str_appendc(buf, ' ');
				smart_str_appendl(buf, "{\n", 2);
				if ((type->kind == XSD_TYPEKIND_RESTRICTION ||
				     type->kind == XSD_TYPEKIND_EXTENSION) && type->encode) {
					encodePtr enc = type->encode;
					while (enc && enc->details.sdl_type &&
					       enc->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
					       enc->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
					       enc->details.sdl_type->kind != XSD_TYPEKIND_UNION) {
						enc = enc->details.sdl_type->encode;
					}
					if (enc) {
						smart_str_appendl(buf, spaces.c, spaces.len);
						smart_str_appendc(buf, ' ');
						smart_str_appendl(buf, type->encode->details.type_str, strlen(type->encode->details.type_str));
						smart_str_appendl(buf, " _;\n", 4);
					}
				}
				if (type->model) {
					model_to_string(type->model, buf, level+1);
				}
				if (type->attributes) {
					sdlAttributePtr *attr;

					zend_hash_internal_pointer_reset_ex(type->attributes, &pos);
					while (zend_hash_get_current_data_ex(type->attributes, (void **)&attr, &pos) != FAILURE) {
						smart_str_appendl(buf, spaces.c, spaces.len);
						smart_str_appendc(buf, ' ');
						if ((*attr)->encode && (*attr)->encode->details.type_str) {
							smart_str_appends(buf, (*attr)->encode->details.type_str);
							smart_str_appendc(buf, ' ');
						} else {
							smart_str_appendl(buf, "UNKNOWN ", 8);
						}
						smart_str_appends(buf, (*attr)->name);
						smart_str_appendl(buf, ";\n", 2);
						zend_hash_move_forward_ex(type->attributes, &pos);
					}
				}
				smart_str_appendl(buf, spaces.c, spaces.len);
				smart_str_appendc(buf, '}');
			}
			break;
		default:
			break;
	}
	smart_str_free(&spaces);
	smart_str_0(buf);
}

static void delete_url(void *handle)
{
	php_url_free((php_url*)handle);
}

static void delete_service(void *data)
{
	soapServicePtr service = (soapServicePtr)data;

	if (service->soap_functions.ft) {
		zend_hash_destroy(service->soap_functions.ft);
		efree(service->soap_functions.ft);
	}

	if (service->mapping) {
		zend_hash_destroy(service->mapping);
		efree(service->mapping);
	}

	if (service->soap_class.argc) {
		int i;
		for (i = 0; i < service->soap_class.argc;i++) {
			zval_ptr_dtor(&service->soap_class.argv[i]);
		}
		efree(service->soap_class.argv);
	}

	if (service->actor) {
		efree(service->actor);
	}
	if (service->uri) {
		efree(service->uri);
	}
	if (service->sdl) {
		delete_sdl(service->sdl);
	}
	if (service->encoding) {
		xmlCharEncCloseFunc(service->encoding);
	}
	if (service->class_map) {
		zend_hash_destroy(service->class_map);
		FREE_HASHTABLE(service->class_map);
	}
	efree(service);
}
