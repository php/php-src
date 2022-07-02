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
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_SOAP_H
#define PHP_SOAP_H

#include "php.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"
#if defined(HAVE_PHP_SESSION) && !defined(COMPILE_DL_SESSION)
#include "ext/session/php_session.h"
#endif
#include "zend_smart_str.h"
#include "php_ini.h"
#include "SAPI.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>

#define PHP_SOAP_VERSION PHP_VERSION

#ifndef PHP_WIN32
# define TRUE 1
# define FALSE 0
# define stricmp strcasecmp
#endif

extern int le_url;

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

struct _soapMapping {
	zval to_xml;
	zval to_zval;
};

struct _soapHeader;

struct _soapService {
	sdlPtr sdl;

	struct _soap_functions {
		HashTable *ft;
		int functions_all;
	} soap_functions;

	struct _soap_class {
		zend_class_entry *ce;
		zval *argv;
		int argc;
		int persistence;
	} soap_class;

	zval soap_object;

	HashTable *typemap;
	int        version;
	int        type;
	char      *actor;
	char      *uri;
	xmlCharEncodingHandlerPtr encoding;
	HashTable *class_map;
	int        features;
	struct _soapHeader **soap_headers_ptr;
	int send_errors;
};

#define SOAP_CLASS 1
#define SOAP_FUNCTIONS 2
#define SOAP_OBJECT 3
#define SOAP_FUNCTIONS_ALL 999

#define SOAP_MAP_FUNCTION 1
#define SOAP_MAP_CLASS 2

#define SOAP_PERSISTENCE_SESSION 1
#define SOAP_PERSISTENCE_REQUEST 2

#define SOAP_1_1 1
#define SOAP_1_2 2

#define SOAP_ACTOR_NEXT             1
#define SOAP_ACTOR_NONE             2
#define SOAP_ACTOR_UNLIMATERECEIVER 3

#define SOAP_1_1_ACTOR_NEXT             "http://schemas.xmlsoap.org/soap/actor/next"

#define SOAP_1_2_ACTOR_NEXT             "http://www.w3.org/2003/05/soap-envelope/role/next"
#define SOAP_1_2_ACTOR_NONE             "http://www.w3.org/2003/05/soap-envelope/role/none"
#define SOAP_1_2_ACTOR_UNLIMATERECEIVER "http://www.w3.org/2003/05/soap-envelope/role/ultimateReceiver"

#define SOAP_COMPRESSION_ACCEPT  0x20
#define SOAP_COMPRESSION_GZIP    0x00
#define SOAP_COMPRESSION_DEFLATE 0x10

#define SOAP_AUTHENTICATION_BASIC   0
#define SOAP_AUTHENTICATION_DIGEST  1

#define SOAP_SINGLE_ELEMENT_ARRAYS  (1<<0)
#define SOAP_WAIT_ONE_WAY_CALLS     (1<<1)
#define SOAP_USE_XSI_ARRAY_TYPE     (1<<2)

#define WSDL_CACHE_NONE     0x0
#define WSDL_CACHE_DISK     0x1
#define WSDL_CACHE_MEMORY   0x2
#define WSDL_CACHE_BOTH     0x3

/* New SOAP SSL Method Constants */
#define SOAP_SSL_METHOD_TLS     0
#define SOAP_SSL_METHOD_SSLv2   1
#define SOAP_SSL_METHOD_SSLv3   2
#define SOAP_SSL_METHOD_SSLv23  3


ZEND_BEGIN_MODULE_GLOBALS(soap)
	HashTable  defEncNs;     /* mapping of default namespaces to prefixes */
	HashTable  defEnc;
	HashTable  defEncIndex;
	HashTable *typemap;
	int        cur_uniq_ns;
	int        soap_version;
	sdlPtr     sdl;
	bool  use_soap_error_handler;
	char*      error_code;
	zval       error_object;
	char       cache;
	char       cache_mode;
	char       cache_enabled;
	char*      cache_dir;
	zend_long       cache_ttl;
	zend_long       cache_limit;
	HashTable *mem_cache;
	xmlCharEncodingHandlerPtr encoding;
	HashTable *class_map;
	int        features;
	HashTable  wsdl_cache;
	int        cur_uniq_ref;
	HashTable *ref_map;
ZEND_END_MODULE_GLOBALS(soap)

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry soap_module_entry;
#define soap_module_ptr &soap_module_entry
#define phpext_soap_ptr soap_module_ptr

ZEND_EXTERN_MODULE_GLOBALS(soap)
#define SOAP_GLOBAL(v) ZEND_MODULE_GLOBALS_ACCESSOR(soap, v)

#if defined(ZTS) && defined(COMPILE_DL_SOAP)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

extern zend_class_entry* soap_class_entry;
extern zend_class_entry* soap_var_class_entry;

void add_soap_fault(zval *obj, char *fault_code, char *fault_string, char *fault_actor, zval *fault_detail);

#define soap_error0(severity, format) \
	php_error(severity, "SOAP-ERROR: " format)

#define soap_error1(severity, format, param1) \
	php_error(severity, "SOAP-ERROR: " format, param1)

#define soap_error2(severity, format, param1, param2) \
	php_error(severity, "SOAP-ERROR: " format, param1, param2)

#define soap_error3(severity, format, param1, param2, param3) \
	php_error(severity, "SOAP-ERROR: " format, param1, param2, param3)

static zend_always_inline zval *php_soap_deref(zval *zv) {
	if (UNEXPECTED(Z_TYPE_P(zv) == IS_REFERENCE)) {
		return Z_REFVAL_P(zv);
	}
	return zv;
}

#define Z_CLIENT_URI_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 0))
#define Z_CLIENT_STYLE_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 1))
#define Z_CLIENT_USE_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 2))
#define Z_CLIENT_LOCATION_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 3))
#define Z_CLIENT_TRACE_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 4))
#define Z_CLIENT_COMPRESSION_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 5))
#define Z_CLIENT_SDL_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 6))
#define Z_CLIENT_TYPEMAP_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 7))
#define Z_CLIENT_HTTPSOCKET_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 8))
#define Z_CLIENT_HTTPURL_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 9))
#define Z_CLIENT_LOGIN_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 10))
#define Z_CLIENT_PASSWORD_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 11))
#define Z_CLIENT_USE_DIGEST_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 12))
#define Z_CLIENT_DIGEST_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 13))
#define Z_CLIENT_PROXY_HOST_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 14))
#define Z_CLIENT_PROXY_PORT_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 15))
#define Z_CLIENT_PROXY_LOGIN_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 16))
#define Z_CLIENT_PROXY_PASSWORD_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 17))
#define Z_CLIENT_EXCEPTIONS_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 18))
#define Z_CLIENT_ENCODING_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 19))
#define Z_CLIENT_CLASSMAP_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 20))
#define Z_CLIENT_FEATURES_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 21))
#define Z_CLIENT_CONNECTION_TIMEOUT_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 22))
#define Z_CLIENT_STREAM_CONTEXT_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 23))
#define Z_CLIENT_USER_AGENT_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 24))
#define Z_CLIENT_KEEP_ALIVE_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 25))
#define Z_CLIENT_SSL_METHOD_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 26))
#define Z_CLIENT_SOAP_VERSION_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 27))
#define Z_CLIENT_USE_PROXY_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 28))
#define Z_CLIENT_COOKIES_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 29))
#define Z_CLIENT_DEFAULT_HEADERS_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 30))
#define Z_CLIENT_SOAP_FAULT_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 31))
#define Z_CLIENT_LAST_REQUEST_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 32))
#define Z_CLIENT_LAST_RESPONSE_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 33))
#define Z_CLIENT_LAST_REQUEST_HEADERS_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 34))
#define Z_CLIENT_LAST_RESPONSE_HEADERS_P(zv) php_soap_deref(OBJ_PROP_NUM(Z_OBJ_P(zv), 35))

#endif
