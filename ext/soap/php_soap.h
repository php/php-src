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

#ifndef PHP_HAVE_STREAMS
# error You lose - must be compiled against PHP 4.3.0 or later
#endif

#ifndef PHP_WIN32
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

struct _soapHeaderHandler {
	char *ns;
	int type;

	struct _function_handler {
		char *functionName;
		char *type;
	} function_handler;

	struct _class_handler {
		zend_class_entry *ce;
	} class_handler;
};

struct _soapMapping {
	char *ns;
	char *ctype;
	int type;

	struct _map_functions {
		zval *to_xml_before;
		zval *to_xml;
		zval *to_xml_after;
		zval *to_zval_before;
		zval *to_zval;
		zval *to_zval_after;
	} map_functions;

	struct _map_class {
		int type;
		zend_class_entry *ce;
	} map_class;
};

struct _soapService {
	sdlPtr sdl;

	struct _soap_functions {
		HashTable *ft;
		int functions_all;
	} soap_functions;

	struct _soap_class {
		zend_class_entry *ce;
		zval **argv;
		int argc;
		int persistance;
	} soap_class;

	HashTable *mapping;
	int type;
	char *uri;
};

#define SOAP_CLASS 1
#define SOAP_FUNCTIONS 2
#define SOAP_FUNCTIONS_ALL 999

#define SOAP_MAP_FUNCTION 1
#define SOAP_MAP_CLASS 2

#define SOAP_PERSISTENCE_SESSION 1
#define SOAP_PERSISTENCE_REQUEST 2

#define SOAP_1_1 1
#define SOAP_1_2 2

ZEND_BEGIN_MODULE_GLOBALS(soap)
	HashTable *defEncNs;
	HashTable *defEncPrefix;
	HashTable *defEnc;
	HashTable *defEncIndex;
	HashTable *sdls;
	HashTable *services;
	HashTable *overrides;
	int cur_uniq_ns;
	int soap_version;
	sdlPtr sdl;
	zend_bool use_soap_error_handler;
ZEND_END_MODULE_GLOBALS(soap)

#ifdef PHP_WIN32
#define PHP_SOAP_API __declspec(dllexport)
#else
#define PHP_SOAP_API
#endif
#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry soap_module_entry;
#define soap_module_ptr &soap_module_entry
#define phpext_soap_ptr soap_module_ptr

ZEND_EXTERN_MODULE_GLOBALS(soap);

#ifdef ZTS
# define SOAP_GLOBAL(v) TSRMG(soap_globals_id, zend_soap_globals *, v)
#else
# define SOAP_GLOBAL(v) (soap_globals.v)
#endif

extern zend_class_entry* soap_var_class_entry;

zval* add_soap_fault(zval *obj, char *fault_code, char *fault_string, char *fault_actor, zval *fault_detail TSRMLS_DC);

#endif
