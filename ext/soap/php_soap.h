#ifndef PHP_SOAP_H
#define PHP_SOAP_H

#include "php.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"
#include "ext/session/php_session.h"
#include "ext/standard/php_smart_str.h"
#include "php_ini.h"
#include "SAPI.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>

#ifdef HAVE_PHP_DOMXML
# include "ext/domxml/php_domxml.h"
#endif

/*
 PHP_STREAMS were introduced php-4.2.0.. i think
 Make this part of configure
*/
#ifdef STREAMS_DC
# define PHP_STREAMS
#endif

#ifdef PHP_WIN32
# ifdef PHP_STREAMS
#  define SOAP_STREAM php_stream *
# else
#  define SOAP_STREAM SOCKET
# endif
#else
# ifdef PHP_STREAMS
#  define SOAP_STREAM php_stream *
# else
#  define SOCKET unsigned int
#  define SOAP_STREAM SOCKET
# endif
# define TRUE 1
# define FALSE 0
# define stricmp strcasecmp
#endif

typedef struct _encodeType encodeType, *encodeTypePtr;
typedef struct _encode encode, *encodePtr;

typedef struct _sdl sdl, *sdlPtr;
typedef struct _sdlRestrictionInt sdlRestrictionInt, *sdlRestrictionIntPtr;
typedef struct _sdlRestrictionChar sdlRestrictionChar, *sdlRestrictionCharPtr;
typedef struct _sdlRestrictions sdlRestrictions, *sdlRestrictionsPtr;
typedef struct _sdlType sdlType, *sdlTypePtr;
typedef struct _sdlParam sdlParam, *sdlParamPtr;
typedef struct _sdlFunction sdlFunction, *sdlFunctionPtr;
typedef struct _sdlAttribute sdlAttribute, *sdlAttributePtr;
typedef struct _sdlBinding sdlBinding, *sdlBindingPtr;
typedef struct _sdlSoapBinding sdlSoapBinding, *sdlSoapBindingPtr;
typedef struct _sdlSoapBindingFunction sdlSoapBindingFunction, *sdlSoapBindingFunctionPtr;
typedef struct _sdlSoapBindingFunctionBody sdlSoapBindingFunctionBody, *sdlSoapBindingFunctionBodyPtr;

typedef struct _soapMapping soapMapping, *soapMappingPtr;
typedef struct _soapService soapService, *soapServicePtr;

#include "php_xml.h"
#include "php_encoding.h"
#include "php_sdl.h"
#include "php_schema.h"
#include "php_http.h"
#include "php_packet_soap.h"

extern int le_sdl;
extern int le_http_socket;
extern int le_url;
extern int le_service;


struct _soapHeaderHandler
{
	char *ns;
	int type;

	struct _function_handler
	{
		char *functionName;
		char *type;
	} function_handler;

	struct _class_handler
	{
		zend_class_entry *ce;
	} class_handler;
};

struct _soapMapping
{
	char *ns;
	char *ctype;
	int type;

	struct _map_functions
	{
		zval *to_xml_before;
		zval *to_xml;
		zval *to_xml_after;
		zval *to_zval_before;
		zval *to_zval;
		zval *to_zval_after;
	} map_functions;

	struct _map_class
	{
		int type;
		zend_class_entry *ce;
	} map_class;
};

struct _soapService
{
	sdlPtr sdl;

	struct _soap_functions
	{
		HashTable *ft;
		int functions_all;
	} soap_functions;

	struct _soap_class
	{
		zend_class_entry *ce;
		zval **argv;
		int argc;
		int persistance;
	} soap_class;

	HashTable *mapping;
	int type;
	int enabled;
	char *uri;
};

#define SOAP_CLASS 1
#define SOAP_FUNCTIONS 2
#define SOAP_FUNCTIONS_ALL 999

#define SOAP_MAP_FUNCTION 1
#define SOAP_MAP_CLASS 2

#define SOAP_PERSISTENCE_SESSION 1
#define SOAP_PERSISTENCE_REQUEST 2

ZEND_BEGIN_MODULE_GLOBALS(soap)
	HashTable *defEncNs;
	HashTable *defEncPrefix;
	HashTable *defEnc;
	HashTable *defEncIndex;
	HashTable *sdls;
	HashTable *services;
	HashTable *overrides;
	int cur_uniq_ns;
ZEND_END_MODULE_GLOBALS(soap)

ZEND_EXTERN_MODULE_GLOBALS(soap);

#ifdef ZTS
# define SOAP_GLOBAL(v) TSRMG(soap_globals_id, zend_soap_globals *, v)
#else
# define SOAP_GLOBAL(v) (soap_globals.v)
#endif

#define PHP_SOAP_SERVER_CLASSNAME "soapserver"
#define PHP_SOAP_CLASSNAME "soapobject"
#define PHP_SOAP_VAR_CLASSNAME "soapvar"
#define PHP_SOAP_FAULT_CLASSNAME "soapfault"
#define PHP_SOAP_PARAM_CLASSNAME "soapparam"


extern zend_module_entry soap_module_entry;
#define  soap_module_ptr & soap_module_entry

PHP_MINIT_FUNCTION(soap);
PHP_MSHUTDOWN_FUNCTION(soap);
PHP_MINFO_FUNCTION(soap);

/*
  Registry Functions
  TODO: this!
*/
PHP_FUNCTION(load_sdl);
PHP_FUNCTION(unload_sdl);
PHP_FUNCTION(unload_all_sdls);
PHP_FUNCTION(get_available_sdls);
PHP_FUNCTION(get_available_functions);
PHP_FUNCTION(get_function_parameters);
PHP_FUNCTION(soap_encode_to_xml);
PHP_FUNCTION(soap_encode_to_zval);


/* Server Functions */
PHP_FUNCTION(soapserver);
PHP_FUNCTION(setclass);
PHP_FUNCTION(addfunction);
PHP_FUNCTION(getfunctions);
PHP_FUNCTION(handle);
PHP_FUNCTION(setpersistence);
PHP_FUNCTION(bind);
#ifdef HAVE_PHP_DOMXML
PHP_FUNCTION(map);
#endif

/* Client Functions */
PHP_FUNCTION(soapobject);
PHP_FUNCTION(__use);
PHP_FUNCTION(__style);
PHP_FUNCTION(__isfault);
PHP_FUNCTION(__getfault);
PHP_FUNCTION(__call);
PHP_FUNCTION(__parse);
PHP_FUNCTION(__generate);
PHP_FUNCTION(__trace);
PHP_FUNCTION(__getfunctions);
PHP_FUNCTION(__gettypes);
PHP_FUNCTION(__getlastresponse);
PHP_FUNCTION(__getlastrequest);
PHP_FUNCTION(__headerclass);
PHP_FUNCTION(__headerfunction);

/* SoapVar Functions */
PHP_FUNCTION(soapvar);

/* SoapFault Functions */
PHP_FUNCTION(soapfault);

/* SoapParam Functions */
PHP_FUNCTION(soapparam);


#define DECLARE_TRACE(file) \
	FILE *trace_fp; \
	char *trace_file = file;

#define TRACE(place) \
	trace_fp = fopen(trace_file, "a+"); \
	fwrite(place, strlen(place), 1, trace_fp); \
	fclose(trace_fp);

extern zend_class_entry soap_var_class_entry;

PS_SERIALIZER_FUNCS(soap);

void clear_soap_fault(zval *obj);
void set_soap_fault(zval *obj, char *fault_code, char *fault_string, char *fault_actor, zval *fault_detail TSRMLS_DC);
void add_soap_fault(zval *obj, char *fault_code, char *fault_string, char *fault_actor, zval *fault_detail TSRMLS_DC);

sdlParamPtr get_param(sdlFunctionPtr function, char *param_name, int index, int);
sdlFunctionPtr get_function(sdlBindingPtr sdl, char *function_name);

void delete_sdl(void *handle);
void delete_binding(void *binding);
void delete_sdl_soap_binding_function_body(sdlSoapBindingFunctionBody body);
void delete_function(void *function);
void delete_paramater(void *paramater);
void delete_service(void *service);
void delete_http_socket(void *handle);
void delete_url(void *handle);
void delete_mapping(void *data);

void soap_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);
zval soap_get_property_handler(zend_property_reference *property_reference);
int soap_set_property_handler(zend_property_reference *property_reference, zval *value);
void soap_destructor(void *jobject);

void deseralize_function_call(sdlPtr sdl, xmlDocPtr request, zval *function_name, int *num_params, zval **parameters[] TSRMLS_CC);
xmlDocPtr seralize_response_call(sdlFunctionPtr function, char *function_name,char *uri,zval *ret);
xmlDocPtr seralize_function_call(zval *this_ptr, sdlFunctionPtr function, char *function_name, char *uri, zval **arguments, int arg_count);
xmlNodePtr seralize_parameter(sdlParamPtr param,zval *param_val,int index,char *name, int style);
xmlNodePtr seralize_zval(zval *val, sdlParamPtr param, char *paramName, int style);
zval *desearlize_zval(sdlPtr sdl, xmlNodePtr data, sdlParamPtr param);

void soap_error_handler(int error_num, const char *error_filename, const uint error_lineno, const char *format, va_list args);
#ifndef ZEND_ENGINE_2
int my_call_user_function(HashTable *function_table, zval **object_pp, zval *function_name, zval *retval_ptr, int param_count, zval *params[] TSRMLS_DC);
#endif

#define phpext_soap_ptr soap_module_ptr

#define HTTP_RAW_POST_DATA "HTTP_RAW_POST_DATA"

#define SOAP_SERVER_BEGIN_CODE() \
	zend_error_cb = soap_error_handler

#define SOAP_SERVER_END_CODE() \
	zend_error_cb = old_handler


#define FOREACHATTRNODE(n,c,i) \
	do \
	{ \
		if(n == NULL) \
			break; \
		if(c) \
			i = get_attribute(n,c); \
		else \
			i = n; \
		if(i != NULL) \
		{ \
			n = i;

#define FOREACHNODE(n,c,i) \
	do \
	{ \
		if(n == NULL) \
			break; \
		if(c) \
			i = get_node(n,c); \
		else \
			i = n; \
		if(i != NULL) \
		{ \
			n = i;

#define ENDFOREACH(n) \
		} \
	} while(n = n->next);

#define ZERO_PARAM() \
	if(ZEND_NUM_ARGS() != 0) \
 		WRONG_PARAM_COUNT;

#define ONE_PARAM(p) \
	if(ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &p) == FAILURE) \
		WRONG_PARAM_COUNT;

#define TWO_PARAM(p,p1) \
	if(ZEND_NUM_ARGS() != 1 || getParameters(ht, 2, &p, &p1) == FAILURE) \
		WRONG_PARAM_COUNT;

#define THREE_PARAM(p,p1,p2) \
	if(ZEND_NUM_ARGS() != 1 || getParameters(ht, 3, &p, &p1, &p2) == FAILURE) \
		WRONG_PARAM_COUNT;

#define FETCH_THIS_PORT(ss) \
	{ \
		zval *__thisObj; zval *__port; sdlBindingPtr *__tmp; \
		GET_THIS_OBJECT(__thisObj) \
		if(FIND_PORT_PROPERTY(__thisObj, __port) == FAILURE) { \
			ss = NULL; \
			php_error(E_ERROR, "Error could find current port"); \
		} \
		__tmp = (sdlBindingPtr*)Z_LVAL_P(__port); \
		ss = *__tmp; \
	}

#define FIND_PORT_PROPERTY(ss,tmp) zend_hash_find(Z_OBJPROP_P(ss), "port", sizeof("port"), (void **)&tmp)

#define FETCH_THIS_SDL(ss) \
	{ \
		zval *__thisObj,**__tmp; \
		GET_THIS_OBJECT(__thisObj) \
		if(FIND_SDL_PROPERTY(__thisObj,__tmp) != FAILURE) \
		{ \
			FETCH_SDL_RES(ss,__tmp); \
		} \
		else \
			ss = NULL; \
	}

#define FIND_SDL_PROPERTY(ss,tmp) zend_hash_find(Z_OBJPROP_P(ss), "sdl", sizeof("sdl"), (void **)&tmp)
#define FETCH_SDL_RES(ss,tmp) ss = (sdlPtr) zend_fetch_resource(tmp TSRMLS_CC, -1, "sdl", NULL, 1, le_sdl)

#define FETCH_THIS_SERVICE(ss) \
	{ \
		zval *__thisObj,**__tmp; \
		GET_THIS_OBJECT(__thisObj) \
		if(FIND_SERVICE_PROPERTY(__thisObj,__tmp) != FAILURE) \
		{ \
			FETCH_SERVICE_RES(ss,__tmp); \
		} \
		else \
			ss = NULL; \
	}

#define FIND_SERVICE_PROPERTY(ss,tmp) zend_hash_find(Z_OBJPROP_P(ss), "service", sizeof("service"), (void **)&tmp)
#define FETCH_SERVICE_RES(ss,tmp) ss = (soapServicePtr) zend_fetch_resource(tmp TSRMLS_CC, -1, "service", NULL, 1, le_service)

#define FETCH_THIS_URL(ss) \
	{ \
		zval *__thisObj,**__tmp; \
		GET_THIS_OBJECT(__thisObj) \
		if(FIND_URL_PROPERTY(__thisObj,__tmp) != FAILURE) \
		{ \
			FETCH_URL_RES(ss,__tmp); \
		} \
		else \
			ss = NULL; \
	}

#define FIND_URL_PROPERTY(ss,tmp) zend_hash_find(Z_OBJPROP_P(ss), "httpurl", sizeof("httpurl"), (void **)&tmp)
#define FETCH_URL_RES(ss,tmp) ss = (php_url *) zend_fetch_resource(tmp TSRMLS_CC, -1, "httpurl", NULL, 1, le_url)

#define FETCH_THIS_SOCKET(ss) \
	{ \
		zval *__thisObj,**__tmp; \
		GET_THIS_OBJECT(__thisObj) \
		if(FIND_SOCKET_PROPERTY(__thisObj,__tmp) != FAILURE) \
		{ \
			FETCH_SOCKET_RES(ss,__tmp); \
		} \
		else \
			ss = NULL; \
	}

#define FIND_SOCKET_PROPERTY(ss,tmp) zend_hash_find(Z_OBJPROP_P(ss), "httpsocket", sizeof("httpsocket"), (void **)&tmp)
#define FETCH_SOCKET_RES(ss,tmp) ss = (SOAP_STREAM)zend_fetch_resource(tmp TSRMLS_CC, -1, "httpsocket", NULL, 1, le_http_socket)

#define GET_THIS_OBJECT(o) \
 	o = getThis(); \
	if (!o) \
	{ \
		php_error(E_WARNING, "Cannot Get Class Info"); \
		return; \
	}


#endif
