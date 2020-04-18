/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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
  |          Dmitry Stogov <dmitry@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_SDL_H
#define PHP_SDL_H

#define XSD_WHITESPACE_COLLAPSE 1
#define XSD_WHITESPACE_PRESERVE 1
#define XSD_WHITESPACE_REPLACE  1

typedef enum _sdlBindingType {
	BINDING_SOAP = 1,
	BINDING_HTTP = 2
} sdlBindingType;

typedef enum _sdlEncodingStyle {
	SOAP_RPC      = 1,
	SOAP_DOCUMENT = 2
} sdlEncodingStyle;

typedef enum _sdlRpcEncodingStyle {
	SOAP_ENCODING_DEFAULT = 0,
	SOAP_ENCODING_1_1     = 1,
	SOAP_ENCODING_1_2     = 2
} sdlRpcEncodingStyle;

typedef enum _sdlEncodingUse {
	SOAP_ENCODED = 1,
	SOAP_LITERAL = 2
} sdlEncodingUse;

typedef enum _sdlTransport {
	SOAP_TRANSPORT_HTTP = 1
} sdlTransport;

struct _sdl {
	HashTable  functions;        /* array of sdlFunction */
	HashTable *types;            /* array of sdlTypesPtr */
	HashTable *elements;         /* array of sdlTypesPtr */
	HashTable *encoders;         /* array of encodePtr */
	HashTable *bindings;         /* array of sdlBindings (key'd by name) */
	HashTable *requests;         /* array of sdlFunction (references) */
	HashTable *groups;           /* array of sdlTypesPtr */
	char      *target_ns;
	char      *source;
	zend_bool  is_persistent;
};

typedef struct sdlCtx {
	sdlPtr     sdl;

	HashTable  docs;             /* array of xmlDocPtr */

	HashTable  messages;         /* array of xmlNodePtr */
	HashTable  bindings;         /* array of xmlNodePtr */
	HashTable  portTypes;        /* array of xmlNodePtr */
	HashTable  services;         /* array of xmlNodePtr */

	HashTable *attributes;       /* array of sdlAttributePtr */
	HashTable *attributeGroups;  /* array of sdlTypesPtr */
	php_stream_context *context;
	zval               old_header;
} sdlCtx;

struct _sdlBinding {
	char           *name;
	char           *location;
	sdlBindingType  bindingType;
	void           *bindingAttributes; /* sdlSoapBindingPtr */
};

/* Soap Binding Specific stuff */
struct _sdlSoapBinding {
	sdlEncodingStyle  style;
	sdlTransport      transport; /* not implemented yet */
};

typedef struct _sdlSoapBindingFunctionHeader {
	char                *name;
	char                *ns;
	sdlEncodingUse       use;
	sdlTypePtr           element;
	encodePtr            encode;
	sdlRpcEncodingStyle  encodingStyle; /* not implemented yet */
	HashTable           *headerfaults;  /* array of sdlSoapBindingFunctionHeaderPtr */
} sdlSoapBindingFunctionHeader, *sdlSoapBindingFunctionHeaderPtr;

typedef struct _sdlSoapBindingFunctionFault {
	char                *ns;
	sdlEncodingUse       use;
	sdlRpcEncodingStyle  encodingStyle; /* not implemented yet */
} sdlSoapBindingFunctionFault, *sdlSoapBindingFunctionFaultPtr;

struct _sdlSoapBindingFunctionBody {
	char                *ns;
	sdlEncodingUse       use;
	sdlRpcEncodingStyle  encodingStyle;  /* not implemented yet */
	HashTable           *headers;        /* array of sdlSoapBindingFunctionHeaderPtr */
};

struct _sdlSoapBindingFunction {
	char                       *soapAction;
	sdlEncodingStyle            style;

	sdlSoapBindingFunctionBody  input;
	sdlSoapBindingFunctionBody  output;
};

struct _sdlRestrictionInt {
	int   value;
	char  fixed;
};

struct _sdlRestrictionChar {
	char *value;
	char  fixed;
};

struct _sdlRestrictions {
	HashTable *enumeration;              /* array of sdlRestrictionCharPtr */
	sdlRestrictionIntPtr minExclusive;
	sdlRestrictionIntPtr minInclusive;
	sdlRestrictionIntPtr maxExclusive;
	sdlRestrictionIntPtr maxInclusive;
	sdlRestrictionIntPtr totalDigits;
	sdlRestrictionIntPtr fractionDigits;
	sdlRestrictionIntPtr length;
	sdlRestrictionIntPtr minLength;
	sdlRestrictionIntPtr maxLength;
	sdlRestrictionCharPtr whiteSpace;
	sdlRestrictionCharPtr pattern;
};

typedef enum _sdlContentKind {
	XSD_CONTENT_ELEMENT,
	XSD_CONTENT_SEQUENCE,
	XSD_CONTENT_ALL,
	XSD_CONTENT_CHOICE,
	XSD_CONTENT_GROUP_REF,
	XSD_CONTENT_GROUP,
	XSD_CONTENT_ANY
} sdlContentKind;


typedef struct _sdlContentModel sdlContentModel, *sdlContentModelPtr;

struct _sdlContentModel {
	sdlContentKind kind;
	int min_occurs;
	int max_occurs;
	union {
		sdlTypePtr          element;      /* pointer to element */
		sdlTypePtr          group;        /* pointer to group */
		HashTable          *content;      /* array of sdlContentModel for sequnce,all,choice*/
		char               *group_ref;    /* reference to group */
	} u;
};

typedef enum _sdlTypeKind {
	XSD_TYPEKIND_SIMPLE,
	XSD_TYPEKIND_LIST,
	XSD_TYPEKIND_UNION,
	XSD_TYPEKIND_COMPLEX,
	XSD_TYPEKIND_RESTRICTION,
	XSD_TYPEKIND_EXTENSION
} sdlTypeKind;

typedef enum _sdlUse {
	XSD_USE_DEFAULT,
	XSD_USE_OPTIONAL,
	XSD_USE_PROHIBITED,
	XSD_USE_REQUIRED
} sdlUse;

typedef enum _sdlForm {
	XSD_FORM_DEFAULT,
	XSD_FORM_QUALIFIED,
	XSD_FORM_UNQUALIFIED
} sdlForm;

struct _sdlType {
	sdlTypeKind         kind;
	char               *name;
	char               *namens;
	char                nillable;
	HashTable          *elements;             /* array of sdlTypePtr */
	HashTable          *attributes;           /* array of sdlAttributePtr */
	sdlRestrictionsPtr  restrictions;
	encodePtr           encode;
	sdlContentModelPtr  model;
	char               *def;
	char               *fixed;
	char               *ref;
	sdlForm             form;
};

struct _sdlParam {
	int        order;
	sdlTypePtr element;
	encodePtr  encode;
	char      *paramName;
};

typedef struct _sdlFault {
	char      *name;
	HashTable *details;            /* array of sdlParamPtr */
	void      *bindingAttributes;  /* sdlSoapBindingFunctionFaultPtr */
} sdlFault, *sdlFaultPtr;

struct _sdlFunction {
	char               *functionName;
	char               *requestName;
	char               *responseName;
	HashTable          *requestParameters;  /* array of sdlParamPtr */
	HashTable          *responseParameters; /* array of sdlParamPtr (this should only be one) */
	struct _sdlBinding *binding;
	void               *bindingAttributes;  /* sdlSoapBindingFunctionPtr */
	HashTable          *faults;             /* array of sdlFaultPtr */
};

typedef struct _sdlExtraAttribute {
	char *ns;
	char *val;
} sdlExtraAttribute, *sdlExtraAttributePtr;

struct _sdlAttribute {
	char      *name;
	char      *namens;
	char      *ref;
	char      *def;
	char      *fixed;
	sdlForm    form;
	sdlUse     use;
	HashTable *extraAttributes;			/* array of sdlExtraAttribute */
	encodePtr  encode;
};


sdlPtr get_sdl(zval *this_ptr, char *uri, zend_long cache_wsdl);

encodePtr get_encoder_from_prefix(sdlPtr sdl, xmlNodePtr data, const xmlChar *type);
encodePtr get_encoder(sdlPtr sdl, const char *ns, const char *type);
encodePtr get_encoder_ex(sdlPtr sdl, const char *nscat, int len);

sdlBindingPtr get_binding_from_type(sdlPtr sdl, sdlBindingType type);
sdlBindingPtr get_binding_from_name(sdlPtr sdl, char *name, char *ns);

void delete_sdl(void *handle);
void delete_sdl_impl(void *handle);

void sdl_set_uri_credentials(sdlCtx *ctx, char *uri);
void sdl_restore_uri_credentials(sdlCtx *ctx);

#endif
