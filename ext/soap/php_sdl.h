#ifndef PHP_SDL_H
#define PHP_SDL_H

#define XSD_WHITESPACE_COLLAPSE 1
#define XSD_WHITESPACE_PRESERVE 1
#define XSD_WHITESPACE_REPLACE 1

#define BINDING_SOAP 1
#define BINDING_HTTP 2

#define SOAP_RPC 1
#define SOAP_DOCUMENT 2

#define SOAP_ENCODED 1
#define SOAP_LITERAL 2

struct _sdl {
	HashTable docs;         /* pointer to the parsed xml file */
	HashTable functions;    /* array of sdlFunction */
	HashTable *types;       /* array of sdlTypesPtr */
	HashTable *elements;    /* array of sdlTypesPtr */
	HashTable *encoders;    /* array of encodePtr */
	HashTable *bindings;    /* array of sdlBindings (key'd by name) */
	HashTable *requests;    /* array of sdlFunction (references) */
	char *target_ns;
	char *source;
};

struct _sdlBinding {
	char *name;
	char *location;
	int bindingType;
	void *bindingAttributes; /* sdlSoapBindingPtr */
};

/* Soap Binding Specfic stuff */
struct _sdlSoapBinding {
	char *transport;
	int style;
};

struct _sdlSoapBindingFunctionBody {
	char *ns;
	int use;
	char *parts;			/* not implemented yet */
	char *encodingStyle;	/* not implemented yet */
};

struct _sdlSoapBindingFunction {
	char *soapAction;
	int style;

	sdlSoapBindingFunctionBody input;
	sdlSoapBindingFunctionBody output;
	sdlSoapBindingFunctionBody falut;
};

/* HTTP Binding Specfic stuff */
/*********** not implemented yet ************
struct _sdlHttpBinding
{
	int holder;
};
*********************************************/

struct _sdlRestrictionInt {
	int value;
	char fixed;
	char *id;
};

struct _sdlRestrictionChar {
	char *value;
	char fixed;
	char *id;
};

struct _sdlRestrictions {
	HashTable *enumeration;				/* array of sdlRestrictionCharPtr */
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

typedef enum _sdlTypeKind {
	XSD_TYPEKIND_UNKNOWN,
	XSD_TYPEKIND_SIMPLE,
	XSD_TYPEKIND_COMPLEX,
	XSD_TYPEKIND_LIST,
	XSD_TYPEKIND_UNION,
	XSD_TYPEKIND_ALL,
	XSD_TYPEKIND_SEQUENCE,
	XSD_TYPEKIND_CHOICE
} sdlTypeKind;

struct _sdlType {
	sdlTypeKind kind;
	char *name;
	char *namens;
	int nillable;
	int min_occurs;
	int max_occurs;
	HashTable *elements;				/* array of sdlTypePtr */
	HashTable *attributes;				/* array of sdlAttributePtr */
	sdlRestrictionsPtr restrictions;
	encodePtr encode;
	char *ref;
};

struct _sdlParam {
	int order;
	encodePtr encode;
	char *paramName;
};

struct _sdlFunction {
	char *functionName;
	char *requestName;
	char *responseName;
	HashTable *requestParameters;		/* array of sdlParamPtr */
	HashTable *responseParameters;		/* array of sdlParamPtr (this should only be one) */
	struct _sdlBinding* binding;
	void* bindingAttributes; /* sdlSoapBindingFunctionPtr */
};

struct _sdlAttribute {
	char *def;
	char *fixed;
	char *form;
	char *id;
	char *name;
	char *ref;
	char *type;
	char *use;
	HashTable *extraAttributes;			/* array of xmlNodePtr */
};

sdlPtr get_sdl(char *uri);

encodePtr get_encoder_from_prefix(sdlPtr sdl, xmlNodePtr data, const char *type);
encodePtr get_encoder(sdlPtr sdl, const char *ns, const char *type);
encodePtr get_encoder_ex(sdlPtr sdl, const char *nscat);
encodePtr get_create_encoder(sdlPtr sdl, sdlTypePtr cur_type, const char *ns, const char *type);
encodePtr create_encoder(sdlPtr sdl, sdlTypePtr cur_type, const char *ns, const char *type);

sdlBindingPtr get_binding_from_type(sdlPtr sdl, int type);
sdlBindingPtr get_binding_from_name(sdlPtr sdl, char *name, char *ns);

xmlNodePtr sdl_guess_convert_xml(encodeType enc, zval* data, int style);
zval *sdl_guess_convert_zval(encodeType enc, xmlNodePtr data);

void delete_sdl(void *handle);
void delete_type(void *type);
void delete_attribute(void *attribute);
void delete_mapping(void *data);
void delete_restriction_var_int(void *rvi);
void delete_schema_restriction_var_char(void *srvc);

#endif
