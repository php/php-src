#include "php_soap.h"

int le_sdl = 0;
int le_http_socket = 0;
int le_url = 0;
int le_service = 0;

// Local functions
static void function_to_string(sdlFunctionPtr function, smart_str *buf);
static void type_to_string(sdlTypePtr type, smart_str *buf, int level);

static zend_class_entry soap_class_entry;
static zend_class_entry soap_server_class_entry;
static zend_class_entry soap_fault_class_entry;
zend_class_entry soap_var_class_entry;
zend_class_entry soap_param_class_entry;

ZEND_DECLARE_MODULE_GLOBALS(soap);

static void (*old_handler)(int, const char *, const uint, const char*, va_list);

static zend_function_entry soap_functions[] = {
#ifdef HAVE_PHP_DOMXML
	PHP_FE(soap_encode_to_xml, NULL)
	PHP_FE(soap_encode_to_zval, NULL)
#endif
	{NULL, NULL, NULL}
};

static zend_function_entry soap_fault_functions[] = {
	PHP_FE(soapfault, NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry soap_server_functions[] = {
	PHP_FE(soapserver,NULL)
	PHP_FE(setpersistence,NULL)
	PHP_FE(setclass,NULL)
	PHP_FE(addfunction,NULL)
	PHP_FE(getfunctions,NULL)
	PHP_FE(handle,NULL)
	PHP_FE(bind,NULL)
#ifdef HAVE_PHP_DOMXML
	PHP_FE(map, NULL)
#endif
	{NULL, NULL, NULL}
};

static zend_function_entry soap_client_functions[] = {
	PHP_FE(soapobject, NULL)
	PHP_FE(__isfault, NULL)
	PHP_FE(__getfault, NULL)
	PHP_FE(__use, NULL)
	PHP_FE(__style, NULL)
	PHP_FE(__call, NULL)
	PHP_FE(__parse, NULL)
	PHP_FE(__generate, NULL)
	PHP_FE(__trace, NULL)
	PHP_FE(__headerclass, NULL)
	PHP_FE(__headerfunction, NULL)
	PHP_FE(__getlastrequest, NULL)
	PHP_FE(__getlastresponse, NULL)
	PHP_FE(__getfunctions, NULL)
	PHP_FE(__gettypes, NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry soap_var_functions[] = {
	PHP_FE(soapvar, NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry soap_param_functions[] = {
	PHP_FE(soapparam, NULL)
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
  NULL,
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

static void php_soap_init_globals(zend_soap_globals *soap_globals)
{
	int i;
	long enc;

	soap_globals->sdls = malloc(sizeof(HashTable));
	zend_hash_init(soap_globals->sdls, 0, NULL, delete_sdl, 1);

	soap_globals->services = malloc(sizeof(HashTable));
	zend_hash_init(soap_globals->services, 0, NULL, delete_service, 1);

	soap_globals->defEnc = malloc(sizeof(HashTable));
	zend_hash_init(soap_globals->defEnc, 0, NULL, NULL, 1);

	soap_globals->defEncIndex = malloc(sizeof(HashTable));
	zend_hash_init(soap_globals->defEncIndex, 0, NULL, NULL, 1);

	soap_globals->defEncNs = malloc(sizeof(HashTable));
	zend_hash_init(soap_globals->defEncNs, 0, NULL, NULL, 1);

	soap_globals->defEncPrefix = malloc(sizeof(HashTable));
	zend_hash_init(soap_globals->defEncPrefix, 0, NULL, NULL, 1);

	soap_globals->overrides = NULL;

	i = 0;
	do
	{
		enc = (long)&defaultEncoding[i];

		//If has a ns and a str_type then index it
		if(defaultEncoding[i].details.type_str)
		{
			if(defaultEncoding[i].details.ns != NULL)
			{
				char *ns_type;
				ns_type = emalloc(strlen(defaultEncoding[i].details.ns) + strlen(defaultEncoding[i].details.type_str) + 2);
				sprintf(ns_type, "%s:%s", defaultEncoding[i].details.ns, defaultEncoding[i].details.type_str);
				zend_hash_add(soap_globals->defEnc, ns_type, strlen(ns_type), &enc, sizeof(encodePtr), NULL);
				efree(ns_type);
			}
			else
			{
				zend_hash_add(soap_globals->defEnc, defaultEncoding[i].details.type_str, strlen(defaultEncoding[i].details.type_str), &enc, sizeof(encodePtr), NULL);
			}
		}
		//Index everything by number
		zend_hash_index_update(soap_globals->defEncIndex, defaultEncoding[i].details.type, &enc, sizeof(encodePtr), NULL);
		i++;
	}
	while(defaultEncoding[i].details.type != END_KNOWN_TYPES);

	//hash by namespace
	zend_hash_add(soap_globals->defEncNs, XSD_1999_NAMESPACE, sizeof(XSD_1999_NAMESPACE), XSD_NS_PREFIX, sizeof(XSD_NS_PREFIX), NULL);
	zend_hash_add(soap_globals->defEncNs, XSD_NAMESPACE, sizeof(XSD_NAMESPACE), XSD_NS_PREFIX, sizeof(XSD_NS_PREFIX), NULL);
	zend_hash_add(soap_globals->defEncNs, APACHE_NAMESPACE, sizeof(APACHE_NAMESPACE), APACHE_NS_PREFIX, sizeof(APACHE_NS_PREFIX), NULL);
	zend_hash_add(soap_globals->defEncNs, SOAP_ENC_NAMESPACE, sizeof(SOAP_ENC_NAMESPACE), SOAP_ENC_NS_PREFIX, sizeof(SOAP_ENC_NS_PREFIX), NULL);
	//and by prefix
	zend_hash_add(soap_globals->defEncPrefix, XSD_NS_PREFIX, sizeof(XSD_NS_PREFIX), XSD_NAMESPACE, sizeof(XSD_NAMESPACE), NULL);
	zend_hash_add(soap_globals->defEncPrefix, APACHE_NS_PREFIX, sizeof(APACHE_NS_PREFIX), APACHE_NAMESPACE, sizeof(APACHE_NAMESPACE), NULL);
	zend_hash_add(soap_globals->defEncPrefix, SOAP_ENC_NS_PREFIX, sizeof(SOAP_ENC_NS_PREFIX), SOAP_ENC_NAMESPACE, sizeof(SOAP_ENC_NAMESPACE), NULL);

}

static void php_soap_del_globals(zend_soap_globals *soap_globals)
{
/*	zend_hash_destroy(soap_globals->sdls);
	zend_hash_destroy(soap_globals->services);
	zend_hash_destroy(soap_globals->defEnc);
	zend_hash_destroy(soap_globals->defEncIndex);
	zend_hash_destroy(soap_globals->defEncNs);*/
}

PHP_MSHUTDOWN_FUNCTION(soap)
{
	zend_hash_destroy(SOAP_GLOBAL(sdls));
	zend_hash_destroy(SOAP_GLOBAL(services));
	zend_hash_destroy(SOAP_GLOBAL(defEnc));
	zend_hash_destroy(SOAP_GLOBAL(defEncIndex));
	zend_hash_destroy(SOAP_GLOBAL(defEncNs));
	zend_hash_destroy(SOAP_GLOBAL(defEncPrefix));
	return SUCCESS;
}

PHP_MINIT_FUNCTION(soap)
{
	//TODO: add ini entry for always use soap errors
	ZEND_INIT_MODULE_GLOBALS(soap, php_soap_init_globals, php_soap_del_globals);

	//Register SoapObject class
	INIT_OVERLOADED_CLASS_ENTRY(soap_class_entry, PHP_SOAP_CLASSNAME, soap_client_functions, soap_call_function_handler, NULL, NULL);
	zend_register_internal_class(&soap_class_entry TSRMLS_CC);

	//Register SoapVar class
	INIT_CLASS_ENTRY(soap_var_class_entry, PHP_SOAP_VAR_CLASSNAME, soap_var_functions);
	zend_register_internal_class(&soap_var_class_entry TSRMLS_CC);

	//Register SoapServer class
	INIT_CLASS_ENTRY(soap_server_class_entry, PHP_SOAP_SERVER_CLASSNAME, soap_server_functions);
	zend_register_internal_class(&soap_server_class_entry TSRMLS_CC);

	//Register SoapFault class
	INIT_CLASS_ENTRY(soap_fault_class_entry, PHP_SOAP_FAULT_CLASSNAME, soap_fault_functions);
	zend_register_internal_class(&soap_fault_class_entry TSRMLS_CC);

	//Register SoapParam class
	INIT_CLASS_ENTRY(soap_param_class_entry, PHP_SOAP_PARAM_CLASSNAME, soap_param_functions);
	zend_register_internal_class(&soap_param_class_entry TSRMLS_CC);

	le_sdl = register_list_destructors(NULL, NULL);
	le_http_socket = register_list_destructors(delete_http_socket, NULL);
	le_url = register_list_destructors(delete_url, NULL);
	le_service = register_list_destructors(delete_service, NULL);

	REGISTER_LONG_CONSTANT("SOAP_PERSISTENCE_SESSION", SOAP_PERSISTENCE_SESSION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_PERSISTENCE_REQUEST", SOAP_PERSISTENCE_REQUEST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_FUNCTIONS_ALL", SOAP_FUNCTIONS_ALL, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOAP_ENCODED", SOAP_ENCODED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_LITERAL", SOAP_LITERAL, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOAP_RPC", SOAP_RPC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_DOCUMENT", SOAP_DOCUMENT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("XSD_1999_TIMEINSTANT", XSD_1999_TIMEINSTANT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("UNKNOWN_TYPE", UNKNOWN_TYPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_ENC_OBJECT", SOAP_ENC_OBJECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOAP_ENC_ARRAY", SOAP_ENC_ARRAY, CONST_CS | CONST_PERSISTENT);

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
	REGISTER_LONG_CONSTANT("XSD_ENTITYS", XSD_ENTITYS, CONST_CS | CONST_PERSISTENT);
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

	old_handler = zend_error_cb;

#if HAVE_PHP_SESSION
	php_session_register_serializer("soap",	PS_SERIALIZER_ENCODE_NAME(soap), PS_SERIALIZER_DECODE_NAME(soap));
#endif

	return SUCCESS;
}

#if HAVE_PHP_SESSION
PS_SERIALIZER_ENCODE_FUNC(soap)
{
/*
	char *key;													\
	uint key_length;											\
	ulong num_key;												\
	zval **struc;

    wddx_packet *packet;
	PS_ENCODE_VARS;

	packet = php_wddx_constructor();
	if (!packet)
		return FAILURE;

	php_wddx_packet_start(packet, NULL, 0);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);

	PS_ENCODE_LOOP(

		php_wddx_serialize_var(packet, *struc, key, key_length);
	);

	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	php_wddx_packet_end(packet);
	*newstr = php_wddx_gather(packet);
	php_wddx_destructor(packet);

	if (newlen)
		*newlen = strlen(*newstr);
*/
	return SUCCESS;
}

PS_SERIALIZER_DECODE_FUNC(soap)
{
/*	zval *retval;
	zval **ent;
	char *key;
	uint key_length;
	char tmp[128];
	ulong idx;
	int hash_type;
	int ret;

	if (vallen == 0)
		return SUCCESS;

	MAKE_STD_ZVAL(retval);

	if ((ret = php_wddx_deserialize_ex((char *)val, vallen, retval)) == SUCCESS) {

		for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(retval));
			 zend_hash_get_current_data(Z_ARRVAL_P(retval), (void **) &ent) == SUCCESS;
			 zend_hash_move_forward(Z_ARRVAL_P(retval))) {
			hash_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(retval), &key, &key_length, &idx, 0, NULL);

			switch (hash_type) {
				case HASH_KEY_IS_LONG:
					sprintf(tmp, "%ld", idx);
					key = tmp;
				case HASH_KEY_IS_STRING:
					php_set_session_var(key, key_length-1, *ent, NULL TSRMLS_CC);
					PS_ADD_VAR(key);
			}
		}
	}

	zval_ptr_dtor(&retval);
*/
	return TRUE;
}
#endif


PHP_MINFO_FUNCTION(soap)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Soap Client", "enabled");
	php_info_print_table_row(2, "Soap Server", "enabled");
#if HAVE_PHP_SESSION
	php_info_print_table_row(2, "Soap Serializer", "enabled");
#endif
	php_info_print_table_end();
}

#ifdef HAVE_PHP_DOMXML
PHP_FUNCTION(soap_encode_to_xml)
{
	zval *pzval, *ret;
	encodePtr enc;
	char *name;
	int found, name_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &name_len, &pzval) == FAILURE)
		php_error(E_ERROR, "wrong number of parameters to soap_encode_to_xml");

	enc = get_conversion(Z_TYPE_P(pzval));
	ret = php_domobject_new(seralize_zval(pzval, NULL, name, SOAP_ENCODED), &found, NULL TSRMLS_CC);
	*return_value = *ret;
	zval_copy_ctor(return_value);
	zval_ptr_dtor(&ret);
}

PHP_FUNCTION(soap_encode_to_zval)
{
	zval *dom, **addr, *ret;
	xmlNodePtr node;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &dom) == FAILURE)
		php_error(E_ERROR, "wrong number of parameters to soap_encode_to_zval");

	if(zend_hash_index_find(Z_OBJPROP_P(dom), 1, (void **)&addr) == FAILURE)
		php_error(E_ERROR, "Cannot find domaddress to parameter passed to soap_encode_to_zval");

	node = (xmlNodePtr)Z_LVAL_PP(addr);
	ret = master_to_zval(get_conversion(UNKNOWN_TYPE), node);
	*return_value = *ret;
}
#endif

//SoapParam functions
PHP_FUNCTION(soapparam)
{
	zval *thisObj, *data;
	char *name;
	int name_length;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &data, &name, &name_length) == FAILURE)
		php_error(E_ERROR, "Invalid arguments to SoapParam constructor");

	GET_THIS_OBJECT(thisObj);

	zval_add_ref(&data);
	add_property_stringl(thisObj, "param_name", name, name_length, 1);
	add_property_zval(thisObj, "param_data", data);
}

//SoapFault functions
PHP_FUNCTION(soapfault)
{
	char *fault_string = NULL, *fault_code = NULL, *fault_actor = NULL;
	int fault_string_len, fault_code_len, fault_actor_len;
	zval *thisObj, *details = NULL;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|zs", &fault_string, &fault_string_len,
		&fault_code, &fault_code_len, &details, &fault_actor, &fault_actor_len) == FAILURE)
		php_error(E_ERROR, "Invalid arguments to SoapFault constructor");

	GET_THIS_OBJECT(thisObj);

	if(details)
		zval_add_ref(&details);
	set_soap_fault(thisObj, fault_code, fault_string, fault_actor, details);
}

//SoapVar functions
PHP_FUNCTION(soapvar)
{
	zval *data, *thisObj, *type;
	char *stype = NULL, *ns = NULL, *name = NULL, *namens = NULL;
	int stype_len, ns_len, name_len, namens_len;

	GET_THIS_OBJECT(thisObj);
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z!z|ssss", &data, &type, &stype, &stype_len, &ns, &ns_len, &name, &name_len, &namens, &namens_len) == FAILURE)
			php_error(E_ERROR, "Invalid arguments to SoapVal constructor");

	zval_add_ref(&data);
	if(Z_TYPE_P(type) == IS_NULL)
		add_property_long(thisObj, "enc_type", UNKNOWN_TYPE);
	else
	{
		if(zend_hash_index_exists(SOAP_GLOBAL(defEncIndex), Z_LVAL_P(type)))
			add_property_long(thisObj, "enc_type", Z_LVAL_P(type));
		else
			php_error(E_ERROR, "Cannot find encoding for SoapVar");
	}

  	add_property_zval(thisObj, "enc_value", data);

	if(stype && strlen(stype) > 0)
		add_property_stringl(thisObj, "enc_stype", stype, stype_len, 1);
	if(ns && strlen(ns) > 0)
		add_property_stringl(thisObj, "enc_ns", ns, ns_len, 1);
	if(name && strlen(name) > 0)
		add_property_stringl(thisObj, "enc_name", name, name_len, 1);
	if(namens && strlen(namens) > 0)
		add_property_stringl(thisObj, "enc_namens", namens, namens_len, 1);
}

//SoapServer functions
PHP_FUNCTION(soapserver)
{
	zval *thisObj;
	soapServicePtr service;
	char *uri;
	int ret, uri_len;

	SOAP_SERVER_BEGIN_CODE();

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &uri, &uri_len) == FAILURE)
		php_error(E_ERROR, "Wrong number of parameters to SoapServer constructor");

	GET_THIS_OBJECT(thisObj);

	service = emalloc(sizeof(soapService));
	memset(service, 0, sizeof(soapService));

	service->uri = estrndup(uri, uri_len);
	service->type = SOAP_FUNCTIONS;
	service->soap_functions.functions_all = FALSE;
	service->soap_functions.ft = emalloc(sizeof(HashTable));
	zend_hash_init(service->soap_functions.ft, 0, NULL, ZVAL_PTR_DTOR, 0);

	ret = zend_list_insert(service, le_service);
	add_property_resource(thisObj, "service", ret);
	zend_list_addref(ret);

	SOAP_SERVER_END_CODE();
}

#define NULL_OR_STRING(zval) \
	(!zval || Z_TYPE_P(zval) == IS_NULL || Z_TYPE_P(zval) == IS_STRING)

#define IS_VALID_FUNCTION(zval) \
	(zval && Z_TYPE_P(zval) != IS_NULL)

#ifdef HAVE_PHP_DOMXML
PHP_FUNCTION(map)
{
	char *type, *class_name;
	zval *to_xml_before = NULL, *to_xml = NULL, *to_xml_after = NULL,
		*to_zval_before = NULL, *to_zval = NULL, *to_zval_after = NULL;
	int type_len, class_name_len;
	char *ns, *ctype;
	soapServicePtr service;

	SOAP_SERVER_BEGIN_CODE();

	if(zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sz|zzzzz",
		&type, &type_len, &to_xml_before, &to_xml, &to_xml_after, &to_zval_before, &to_zval,
		&to_zval_after) == SUCCESS && NULL_OR_STRING(to_xml_before) && NULL_OR_STRING(to_xml) &&
		NULL_OR_STRING(to_xml_after) && NULL_OR_STRING(to_zval_before) && NULL_OR_STRING(to_zval) &&
		NULL_OR_STRING(to_zval_after))
	{
		soapMappingPtr map;
		encodePtr enc, new_enc;
		smart_str resloved_ns = {0};

		FETCH_THIS_SERVICE(service);

		new_enc = malloc(sizeof(encode));
		memset(new_enc, 0, sizeof(encode));

		ctype = strrchr(type, ':');
		if(ctype) {
			smart_str_appendl(&resloved_ns, type, ctype - type);
			smart_str_0(&resloved_ns);
			ctype++;
		} else {
			ns = NULL;
		}

		if(ns)
		{
			if(zend_hash_find(SOAP_GLOBAL(defEncPrefix),  resloved_ns.c, resloved_ns.len + 1, &ns) == SUCCESS)
			{
				enc = get_encoder(service->sdl, ns, ctype);
				smart_str_free(&resloved_ns);
				smart_str_appendl(&resloved_ns, ns, strlen(ns));
				smart_str_appendc(&resloved_ns, ':');
				smart_str_appendl(&resloved_ns, ctype, strlen(ctype));
				smart_str_0(&resloved_ns);
				type = resloved_ns.c;
				type_len = resloved_ns.len;
			}
			else
				enc = get_encoder_ex(service->sdl, type);
		}
		else
			enc = get_encoder_ex(service->sdl, type);

		new_enc->details.type = enc->details.type;
		new_enc->details.ns = strdup(enc->details.ns);
		new_enc->details.type_str = strdup(enc->details.type_str);
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
		if(IS_VALID_FUNCTION(to_xml_before))
		{
			zval_add_ref(&to_xml_before);
			map->map_functions.to_xml_before = to_xml_before;
			new_enc->to_xml_before = to_xml_before_user;
		}
		if(IS_VALID_FUNCTION(to_xml))
		{
			zval_add_ref(&to_xml);
			map->map_functions.to_xml = to_xml;
			new_enc->to_xml = to_xml_user;
		}
		if(IS_VALID_FUNCTION(to_xml_after))
		{
			zval_add_ref(&to_xml_after);
			map->map_functions.to_xml_after = to_xml_after;
			new_enc->to_xml_after = to_xml_after_user;
		}
		if(IS_VALID_FUNCTION(to_zval_before))
		{
			zval_add_ref(&to_zval_before);
			map->map_functions.to_zval_before = to_zval_before;
			new_enc->to_zval_before = to_zval_before_user;
		}
		if(IS_VALID_FUNCTION(to_zval))
		{
			zval_add_ref(&to_zval);
			map->map_functions.to_zval = to_zval;
			new_enc->to_zval = to_zval_user;
		}
		if(IS_VALID_FUNCTION(to_zval_after))
		{
			zval_add_ref(&to_zval_after);
			map->map_functions.to_zval_after = to_zval_after;
			new_enc->to_zval_after = to_zval_after_user;
		}

		new_enc->details.map = map;

		if(!service->mapping)
		{
			service->mapping = emalloc(sizeof(HashTable));
			zend_hash_init(service->mapping, 0, NULL, delete_encoder, 0);
		}
		zend_hash_update(service->mapping, type, type_len + 1, &new_enc, sizeof(encodePtr), NULL);
		smart_str_free(&resloved_ns);
	}
	else if(zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &type, &type_len, &class_name, &class_name_len, &type) == SUCCESS)
	{
	}
	else
		php_error(E_ERROR, "Wrong number of parameters to SoapServer->map");
}
#endif

PHP_FUNCTION(bind)
{
	char *wsdl;
	int wsdl_len;
	soapServicePtr service;

	SOAP_SERVER_BEGIN_CODE();

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &wsdl, &wsdl_len) == FAILURE)
		php_error(E_ERROR, "Wrong number of parameters to SoapServer->bind");

	FETCH_THIS_SERVICE(service);
	service->sdl = get_sdl(wsdl);

	SOAP_SERVER_END_CODE();
}

PHP_FUNCTION(setpersistence)
{
	soapServicePtr service;
	int value;

	SOAP_SERVER_BEGIN_CODE();
	FETCH_THIS_SERVICE(service);

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) != FAILURE)
	{
		if(service->type == SOAP_CLASS)
		{
			if(value == SOAP_PERSISTENCE_SESSION ||
				value == SOAP_PERSISTENCE_REQUEST)
				service->soap_class.persistance = value;
			else
				php_error(E_ERROR, "Tried to set persistence with bogus value (%ld)", value);
		}
		else
			php_error(E_ERROR, "Tried to set persistence when you are using you SOAP SERVER in function mode, no persistence needed");
	}

	SOAP_SERVER_END_CODE();
}

PHP_FUNCTION(setclass)
{
	soapServicePtr service;
	zend_class_entry *ce;
	char *class_name = NULL;
	int found, argc;
	zval ***argv;

	SOAP_SERVER_BEGIN_CODE();

	FETCH_THIS_SERVICE(service);

	argc = ZEND_NUM_ARGS();
	argv = emalloc(argc * sizeof(zval **));

	if (argc < 1 || zend_get_parameters_array_ex(argc, argv) == FAILURE)
	{
		efree(argv);
		WRONG_PARAM_COUNT;
	}

	if(Z_TYPE_PP(argv[0]) == IS_STRING)
	{
		class_name = estrdup(Z_STRVAL_PP(argv[0]));

		found = zend_hash_find(EG(class_table), php_strtolower(class_name, Z_STRLEN_PP(argv[0])), Z_STRLEN_PP(argv[0])	 + 1, (void **)&ce);
		efree(class_name);
		if(found != FAILURE)
		{
			service->type = SOAP_CLASS;
			service->soap_class.ce = ce;
			service->soap_class.persistance = SOAP_PERSISTENCE_REQUEST;
			service->soap_class.argc = argc - 1;
			if(service->soap_class.argc > 0)
			{
				int i;
				service->soap_class.argv = emalloc(sizeof(zval) * service->soap_class.argc);
				for(i = 0;i < service->soap_class.argc;i++)
				{
					service->soap_class.argv[i] = *(argv[i + 1]);
					zval_add_ref(&service->soap_class.argv[i]);
				}
			}
		}
		else
			php_error(E_ERROR, "Tried to set a non existant class (%s)", Z_STRVAL_PP(argv[0]));
	}
	else
		php_error(E_ERROR, "You must pass in a string to setclass");

	efree(argv);
	SOAP_SERVER_END_CODE();
}

PHP_FUNCTION(getfunctions)
{
	soapServicePtr service;

	SOAP_SERVER_BEGIN_CODE();

	FETCH_THIS_SERVICE(service);

	array_init(return_value);
	if(service->type == SOAP_CLASS)
	{
		zend_function *f;
		zend_hash_internal_pointer_reset(&service->soap_class.ce->function_table);
		while(zend_hash_get_current_data(&service->soap_class.ce->function_table, (void **)&f) != FAILURE)
		{
			add_next_index_string(return_value, f->common.function_name, 1);
			zend_hash_move_forward(&service->soap_class.ce->function_table);
		}
	}
	else if(service->soap_functions.functions_all == TRUE)
	{
		zend_function *f;
		zend_hash_internal_pointer_reset(EG(function_table));
		while(zend_hash_get_current_data(EG(function_table), (void **)&f) != FAILURE)
		{
			add_next_index_string(return_value, f->common.function_name, 1);
			zend_hash_move_forward(EG(function_table));
		}
	}
	else if(service->soap_functions.ft != NULL)
		zend_hash_copy(Z_ARRVAL_P(return_value), service->soap_functions.ft, (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));

	SOAP_SERVER_END_CODE();
}

PHP_FUNCTION(addfunction)
{
	soapServicePtr service;
	zval *function_name, *function_copy;

	SOAP_SERVER_BEGIN_CODE();

	FETCH_THIS_SERVICE(service);

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &function_name) == FAILURE)
		php_error(E_ERROR, "Invalid parameters passed to addfunction");

	if(function_name->type == IS_ARRAY)
	{
		if(service->type == SOAP_FUNCTIONS)
		{
			zval **tmp_function, *function_copy;

			if(service->soap_functions.ft == NULL)
			{
				service->soap_functions.ft = emalloc(sizeof(HashTable));
				zend_hash_init(service->soap_functions.ft, 0, NULL, ZVAL_PTR_DTOR, 0);
			}

			zend_hash_internal_pointer_reset(Z_ARRVAL_P(function_name));
			while(zend_hash_get_current_data(Z_ARRVAL_P(function_name), (void **)&tmp_function) != FAILURE)
			{
				if(Z_TYPE_PP(tmp_function) != IS_STRING)
					php_error(E_ERROR, "Tried to add a function that isn't a string");

				MAKE_STD_ZVAL(function_copy);
				*function_copy = *(*tmp_function);
				zval_copy_ctor(function_copy);

				php_strtolower(Z_STRVAL_P(function_copy), Z_STRLEN_P(function_copy));

				if(zend_hash_exists(EG(function_table), Z_STRVAL_P(function_copy), Z_STRLEN_P(function_copy) + 1) == FALSE)
					php_error(E_ERROR, "Tried to add a non existant function (\"%s\")", Z_STRVAL_PP(tmp_function));

				zend_hash_update(service->soap_functions.ft, Z_STRVAL_P(function_copy), Z_STRLEN_P(function_copy) + 1, &function_copy, sizeof(zval *), NULL);
				zend_hash_move_forward(Z_ARRVAL_P(function_name));
			}
		}
	}
	else if(function_name->type == IS_STRING)
	{
		MAKE_STD_ZVAL(function_copy);
		*function_copy = *function_name;
		zval_copy_ctor(function_copy);

		php_strtolower(Z_STRVAL_P(function_copy), Z_STRLEN_P(function_copy));

		if(zend_hash_exists(EG(function_table), Z_STRVAL_P(function_copy), Z_STRLEN_P(function_copy) + 1) == FALSE)
			php_error(E_ERROR, "Tried to add a non existant function (\"%s\")", Z_STRVAL_P(function_name));
		if(service->soap_functions.ft == NULL)
		{
			service->soap_functions.functions_all = FALSE;
			service->soap_functions.ft = emalloc(sizeof(HashTable));
			zend_hash_init(service->soap_functions.ft, 0, NULL, ZVAL_PTR_DTOR, 0);
		}

		zend_hash_update(service->soap_functions.ft, Z_STRVAL_P(function_copy), Z_STRLEN_P(function_copy) + 1, &function_copy, sizeof(zval *), NULL);
	}
	else if(function_name->type == IS_LONG)
	{
		if(Z_LVAL_P(function_name) == SOAP_FUNCTIONS_ALL)
		{
			if(service->soap_functions.ft != NULL)
			{
				zend_hash_destroy(service->soap_functions.ft);
				efree(service->soap_functions.ft);
				service->soap_functions.ft = NULL;
			}
			service->soap_functions.functions_all = TRUE;
		}
		else
			php_error(E_ERROR, "Invalid value passed to addfunction (%ld)", Z_LVAL_P(function_name));
	}

	SOAP_SERVER_END_CODE();
}

PHP_FUNCTION(handle)
{
	soapServicePtr service;
	xmlDocPtr doc_request, doc_return;
	zval function_name, **params, **raw_post, *soap_obj, retval, **server_vars;
	char *fn_name, cont_len[30], *response_name;
	int num_params = 0, size, i, call_status;
	xmlChar *buf;
	HashTable *function_table;

	FETCH_THIS_SERVICE(service);

	SOAP_SERVER_BEGIN_CODE();
	ZERO_PARAM();

	INIT_ZVAL(retval);

	if(zend_hash_find(&EG(symbol_table), "_SERVER", sizeof("_SERVER"), (void **)&server_vars) == SUCCESS)
	{
		zval **req_method, **query_string;
		if(zend_hash_find(Z_ARRVAL_PP(server_vars), "REQUEST_METHOD", sizeof("REQUEST_METHOD"), (void **)&req_method) == SUCCESS)
		{
			if(!strcmp(Z_STRVAL_PP(req_method), "GET") && zend_hash_find(Z_ARRVAL_PP(server_vars), "QUERY_STRING", sizeof("QUERY_STRING"), (void **)&query_string) == SUCCESS)
			{
				if(!strcmp(Z_STRVAL_PP(query_string), "WSDL"))
				{
					if(service->sdl)
					{
						zval readfile, readfile_ret, *param;

						INIT_ZVAL(readfile);
						INIT_ZVAL(readfile_ret);
						MAKE_STD_ZVAL(param);

						sapi_add_header("Content-Type: text/xml", sizeof("Content-Type: text/xml"), 1);
						ZVAL_STRING(param, service->sdl->source, 1);
						ZVAL_STRING(&readfile, "readfile", 1);
						if(call_user_function(EG(function_table), NULL, &readfile, &readfile_ret, 1, &param  TSRMLS_CC) == FAILURE)
							php_error(E_ERROR, "Couldn't find WSDL");

						zval_ptr_dtor(&param);
						zval_dtor(&readfile);
						zval_dtor(&readfile_ret);
						SOAP_SERVER_END_CODE();
						return;
					}
				}
			}
		}
	}

//Turn on output buffering... we don't want people print in their methods
//#if PHP_API_VERSION <= 20010901
//	if(php_start_ob_buffer(NULL, 0 TSRMLS_CC) != SUCCESS)
//#else
	if(php_start_ob_buffer(NULL, 0, 0 TSRMLS_CC) != SUCCESS)
//#endif
		php_error(E_ERROR,"ob_start failed");

	if (zend_hash_find(&EG(symbol_table), HTTP_RAW_POST_DATA, sizeof(HTTP_RAW_POST_DATA), (void **) &raw_post)!=FAILURE
		&& ((*raw_post)->type==IS_STRING))
	{
		doc_request = xmlParseMemory(Z_STRVAL_PP(raw_post),Z_STRLEN_PP(raw_post));
		xmlCleanupParser();

		deseralize_function_call(service->sdl, doc_request, &function_name, &num_params, &params);
		xmlFreeDoc(doc_request);

		fn_name = estrndup(Z_STRVAL(function_name),Z_STRLEN(function_name));
		response_name = emalloc(Z_STRLEN(function_name) + strlen("Response") + 1);
		sprintf(response_name,"%sResponse\0",fn_name);

		if(service->type == SOAP_CLASS)
		{
			soap_obj = NULL;
			//If persistent then set soap_obj from from the previous created session (if available)
			if(service->soap_class.persistance == SOAP_PERSISTENCE_SESSION)
			{
				zval **tmp_soap;

				//Try and call session regiser for our dummy session object
				//The only reason that i use call_user_function is that
				//their isn't a way to start the session from an extension
				//so calling session_register will both register the var
				//and start the session
				{
					zval *bogus_session_name, session_register, sess_retval;

					INIT_ZVAL(session_register);
					INIT_ZVAL(sess_retval);

					if(!zend_ini_long("register_globals", sizeof("register_globals"), 0))
						php_error(E_ERROR, "PHP-SOAP requires 'register_globals' to be on when using persistent objects please check your php.ini file");

					MAKE_STD_ZVAL(bogus_session_name);

					ZVAL_STRING(bogus_session_name, "_bogus_session_name", 1);
					ZVAL_STRING(&session_register, "session_register", 1);

					if(call_user_function(EG(function_table), NULL, &session_register, &sess_retval, 1, &bogus_session_name  TSRMLS_CC) == FAILURE)
						php_error(E_ERROR,"session_register failed");

					zval_ptr_dtor(&bogus_session_name);
					zval_dtor(&session_register);
					zval_dtor(&sess_retval);
				}

				//Find the soap object and assign
				if(zend_hash_find(&EG(symbol_table), "_bogus_session_name", sizeof("_bogus_session_name"), (void **) &tmp_soap) == SUCCESS)
					soap_obj = *tmp_soap;
			}

			//If new session or something wierd happned
			if(soap_obj == NULL)
			{
				zval *tmp_soap;
				MAKE_STD_ZVAL(tmp_soap);
				object_init_ex(tmp_soap, service->soap_class.ce);

				//Call constructor
				if(zend_hash_exists(&Z_OBJCE_P(tmp_soap)->function_table, service->soap_class.ce->name, strlen(service->soap_class.ce->name) + 1))
				{
					zval c_ret, constructor;

					INIT_ZVAL(c_ret);
					INIT_ZVAL(constructor);

					ZVAL_STRING(&constructor, service->soap_class.ce->name, 1);
					if(call_user_function(NULL, &tmp_soap, &constructor, &c_ret, service->soap_class.argc, service->soap_class.argv TSRMLS_CC) == FAILURE)
						php_error(E_ERROR, "Error calling constructor");
					zval_dtor(&constructor);
					zval_dtor(&c_ret);
				}

				//If session then update session hash with new object
				if(service->soap_class.persistance == SOAP_PERSISTENCE_SESSION)
				{
					zval **tmp_soap_pp;
					if(zend_hash_update(&EG(symbol_table), "_bogus_session_name", sizeof("_bogus_session_name"), &tmp_soap, sizeof(zval *), (void **)&tmp_soap_pp) == SUCCESS)
						soap_obj = *tmp_soap_pp;
				}
				else
					soap_obj = tmp_soap;
			}
 			function_table = &(soap_obj->value.obj.ce->function_table);
		}
		else
		{
			if(service->soap_functions.functions_all == TRUE)
				function_table = EG(function_table);
			else
				function_table = service->soap_functions.ft;
		}

		doc_return = NULL;
		if(zend_hash_exists(function_table, php_strtolower(Z_STRVAL(function_name), Z_STRLEN(function_name)), Z_STRLEN(function_name) + 1))
		{
 			if(service->type == SOAP_CLASS)
			{
				call_status = call_user_function(NULL, &soap_obj, &function_name, &retval, num_params, params TSRMLS_CC);
				if(service->soap_class.persistance != SOAP_PERSISTENCE_SESSION)
					zval_ptr_dtor(&soap_obj);
			}
			else
				call_status = call_user_function(EG(function_table), NULL, &function_name, &retval, num_params, params TSRMLS_CC);
		}
		else
			php_error(E_ERROR, "Function (%s) doesn't exist", Z_STRVAL(function_name));

		if(call_status == SUCCESS)
		{
			sdlFunctionPtr function;
			function = get_function(get_binding_from_type(service->sdl, BINDING_SOAP), Z_STRVAL(function_name));
			SOAP_GLOBAL(overrides) = service->mapping;
			doc_return = seralize_response_call(function, response_name, service->uri, &retval);
			SOAP_GLOBAL(overrides) = NULL;
		}
		else
			php_error(E_ERROR, "Function (%s) call failed", Z_STRVAL(function_name));

		//Flush buffer
		php_end_ob_buffer(0, 0 TSRMLS_CC);

		//xmlDocDumpMemoryEnc(doc_return, &buf, &size, XML_CHAR_ENCODING_UTF8);
		xmlDocDumpMemory(doc_return, &buf, &size);

		if(size == 0)
			php_error(E_ERROR, "Dump memory failed");

		sprintf(cont_len, "Content-Length: %d\0", size);
		sapi_add_header("Content-Type: text/xml", sizeof("Content-Type: text/xml"), 1);
		sapi_add_header(cont_len, strlen(cont_len) + 1, 1);

		//Free Memory
		if(num_params > 0)
		{
			for(i = 0; i < num_params;i++)
				zval_ptr_dtor(&params[i]);
			efree(params);
		}

		zval_dtor(&function_name);
		xmlFreeDoc(doc_return);
		efree(response_name);
		efree(fn_name);

		php_write(buf, size TSRMLS_CC);
		xmlFree(buf);
	}
	else
	{
		if(!zend_ini_long("always_populate_raw_post_data", sizeof("always_populate_raw_post_data"), 0))
			php_error(E_ERROR, "PHP-SOAP requires 'always_populate_raw_post_data' to be on please check your php.ini file");

		php_error(E_ERROR, "Couln't find HTTP_RAW_POST_DATA");
	}

	zval_dtor(&retval);
	SOAP_SERVER_END_CODE();
}

void soap_error_handler(int error_num, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
	char buffer[1024];
	int buffer_len;
	TSRMLS_FETCH();

	buffer_len = vsnprintf(buffer, sizeof(buffer)-1, format, args);
	buffer[sizeof(buffer)-1]=0;
	if(buffer_len > sizeof(buffer) - 1 || buffer_len < 0) {
		buffer_len = sizeof(buffer) - 1;
	}

	//Trap all errors
	//What do do with these warnings
	//    E_WARNING, E_NOTICE, E_CORE_WARNING, E_COMPILE_WARNING, E_USER_WARNING, E_USER_NOTICE
	if(error_num == E_USER_ERROR || error_num == E_COMPILE_ERROR || error_num == E_CORE_ERROR ||
		error_num == E_ERROR || error_num == E_PARSE)
	{
		zval outbuf, outbuflen, ret;
		xmlChar *buf, cont_len[30];
		int size;
		xmlDocPtr doc_return;

		INIT_ZVAL(outbuf);
		INIT_ZVAL(outbuflen);
		INIT_ZVAL(ret);

		//Get output buffer and send as fault detials
		if(php_ob_get_length(&outbuflen TSRMLS_CC) != FAILURE && Z_LVAL(outbuflen) != 0)
			php_ob_get_buffer(&outbuf TSRMLS_CC);
		php_end_ob_buffer(0, 0 TSRMLS_CC);

		set_soap_fault(&ret, "SOAP-ENV:Server", buffer, NULL, &outbuf);
		doc_return = seralize_response_call(NULL, NULL, NULL, &ret);

		//Build and send our headers + http 500 status
		//xmlDocDumpMemoryEnc(doc_return, &buf, &size, XML_CHAR_ENCODING_UTF8);
		xmlDocDumpMemory(doc_return, &buf, &size);
		sprintf(cont_len,"Content-Length: %d\0", size);
		sapi_add_header(cont_len, strlen(cont_len) + 1, 1);
		sapi_add_header("Content-Type: text/xml", sizeof("Content-Type: text/xml"), 1);

		// Want to return HTTP 500 but apache wants to over write
		// our fault code with their own handling... Figure this out later
		// sapi_add_header("HTTP/1.1 500 Internal Service Error", sizeof("HTTP/1.1 500 Internal Service Error"), 1);
		php_write(buf, size TSRMLS_CC);

		xmlFreeDoc(doc_return);
		xmlFree(buf);

		zval_dtor(&outbuf);
		zval_dtor(&outbuflen);
		zval_dtor(&ret);

		zend_bailout();
	}
}

// SoapObject functions
PHP_FUNCTION(soapobject)
{
	char *location, *uri = NULL;
	int location_len, uri_len = 0;
	zval *thisObj;

	GET_THIS_OBJECT(thisObj);

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &location, &location_len, &uri, &uri_len) == SUCCESS)
	{
		if(uri)
		{
			// if two parameters assume 'proxy' and 'uri'
			add_property_stringl(thisObj, "location", location, location_len, 1);
			add_property_stringl(thisObj, "uri", uri, uri_len, 1);
		}
		else
		{
			// if one parameter assume 'wsdl'
			sdlPtr sdl;
			int ret;

			sdl = get_sdl(location);
			ret = zend_list_insert(sdl, le_sdl);

			add_property_resource(thisObj, "sdl", ret);
			add_property_resource(thisObj, "port", (long)get_binding_from_type(sdl, BINDING_SOAP));
			zend_list_addref(ret);
		}
	}
}

PHP_FUNCTION(__headerclass)
{
	char *classname, *ns;
	int classname_len, ns_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &classname, &classname_len, &ns, &ns_len) == FAILURE)
		php_error(E_ERROR, "Invalid arguments to SoapObject->__headerclass");
}

PHP_FUNCTION(__headerfunction)
{
	char *functionname, *headername, *ns;
	int functionname_len, *headername_len, ns_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &functionname, &functionname_len, &headername,
		&headername_len, &ns, &ns_len) == FAILURE)
		php_error(E_ERROR, "Invalid arguments to SoapObject->__headerfunction");
}

PHP_FUNCTION(__use)
{
	int use;
	zval *thisObj;

	GET_THIS_OBJECT(thisObj);

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &use) == FAILURE)
		php_error(E_ERROR, "Invalid arguments to SoapObject->__use");

	if(use == SOAP_DOCUMENT || use == SOAP_LITERAL)
	{
		add_property_long(thisObj, "use", use);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

PHP_FUNCTION(__style)
{
	int style;
	zval *thisObj;

	GET_THIS_OBJECT(thisObj);

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &style))
		php_error(E_ERROR, "Invalid arguments to SoapObject->__style");

	if(style == SOAP_RPC || style == SOAP_DOCUMENT)
	{
		add_property_long(thisObj, "style", style);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

PHP_FUNCTION(__trace)
{
	int level;
	zval *thisObj;

	GET_THIS_OBJECT(thisObj);

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &level))
		php_error(E_ERROR, "Invalid arguments to SoapObject->__trace");

	add_property_long(thisObj, "trace", level);
	RETURN_TRUE;
}

PHP_FUNCTION(__generate)
{
	char *function, *soap_action, *uri;
	int function_len, soap_action_len, uri_len, i = 0;
	zval *args;
	zval **real_args;
	zval **param;
	xmlDocPtr request = NULL;
	int arg_count;
	xmlChar *buf;
	int size;
	sdlPtr sdl;

	FETCH_THIS_SDL(sdl);

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa|ss",
		&function, &function_len, &args, &soap_action, &soap_action_len, &uri, &uri_len) == FAILURE)
		php_error(E_ERROR, "Invalid arguments to SoapObject->__call");

	arg_count = zend_hash_num_elements(Z_ARRVAL_P(args));

	real_args = emalloc(sizeof(zval *) * arg_count);
	for(zend_hash_internal_pointer_reset(Z_ARRVAL_P(args));
		zend_hash_get_current_data(Z_ARRVAL_P(args), (void **) &param) == SUCCESS;
		zend_hash_move_forward(Z_ARRVAL_P(args)))
	{
			zval_add_ref(param);
			real_args[i++] = *param;
	}


	if(sdl)
	{
		sdlBindingPtr binding;
		sdlFunctionPtr sdlFunction;
		FETCH_THIS_PORT(binding);

		php_strtolower(function, function_len);
		sdlFunction = get_function(binding, function);
		request = seralize_function_call(this_ptr, sdlFunction, NULL, uri, real_args, arg_count);
	}
	else
	{
		request = seralize_function_call(this_ptr, NULL, function, uri, real_args, arg_count);
	}

	xmlDocDumpMemory(request, &buf, &size);
	ZVAL_STRINGL(return_value, buf, size, 1);
	xmlFree(buf);
}

PHP_FUNCTION(__parse)
{
	char *message, *function;
	int message_len, function_len;
	int num_params;
	zval **ret_params;
	sdlPtr sdl;
	sdlFunctionPtr fn;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &message, &message_len, &function, &function_len) == FAILURE)
		php_error(E_ERROR, "Invalid arguments to SoapObject->__parse");

	FETCH_THIS_SDL(sdl);

	if(sdl != NULL)
	{
		sdlBindingPtr binding;

		FETCH_THIS_PORT(binding);
		fn = get_function(binding, function);
		if(fn != NULL)
			parse_packet_soap(getThis(), message, message_len, fn, NULL, &ret_params, &num_params);
	}
	else
			parse_packet_soap(getThis(), message, message_len, NULL, function, &ret_params, &num_params);

	if(num_params > 0)
	{
		*return_value = *ret_params[0];
		zval_add_ref(&return_value);
		efree(ret_params);
	}
	else
		ZVAL_NULL(return_value)
}

PHP_FUNCTION(__call)
{
	char *function, *soap_action, *uri;
	int function_len, soap_action_len, uri_len, i = 0;
	zval *args;
	zval **real_args;
	zval **param;
	xmlDocPtr request = NULL;
	int num_params, arg_count;
	zval **ret_params;
	char *buffer;
	int len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa|ss",
		&function, &function_len, &args, &soap_action, &soap_action_len, &uri, &uri_len) == FAILURE)
		php_error(E_ERROR, "Invalid arguments to SoapObject->__call");

	arg_count = zend_hash_num_elements(Z_ARRVAL_P(args));

	real_args = emalloc(sizeof(zval *) * arg_count);
	for(zend_hash_internal_pointer_reset(Z_ARRVAL_P(args));
		zend_hash_get_current_data(Z_ARRVAL_P(args), (void **) &param) == SUCCESS;
		zend_hash_move_forward(Z_ARRVAL_P(args)))
	{
			zval_add_ref(param);
			real_args[i++] = *param;
	}

	request = seralize_function_call(this_ptr, NULL, function, uri, real_args, arg_count);
	send_http_soap_request(getThis(), request, function, soap_action);
	xmlFreeDoc(request);

	get_http_soap_response(getThis(), &buffer, &len);
	parse_packet_soap(getThis(), buffer, len, NULL, function, &ret_params, &num_params);
	efree(buffer);

	if(num_params > 0)
	{
		*return_value = *ret_params[0];
		zval_add_ref(&return_value);
		efree(ret_params);
	}
	else
		ZVAL_NULL(return_value)
}

PHP_FUNCTION(__isfault)
{
	zval *thisObj;

	GET_THIS_OBJECT(thisObj);

	if(zend_hash_exists(Z_OBJPROP_P(thisObj), "__soap_fault", sizeof("__soap_fault")))
		RETURN_TRUE
	else
		RETURN_FALSE
}

PHP_FUNCTION(__getfault)
{
	zval *thisObj;
	zval **tmp;

	GET_THIS_OBJECT(thisObj);

	if(zend_hash_find(Z_OBJPROP_P(thisObj), "__soap_fault", sizeof("__soap_fault"), (void **)&tmp) == SUCCESS)
	{
		*return_value = *(*tmp);
		zval_copy_ctor(return_value);
		return;
	}
	RETURN_NULL();
}

PHP_FUNCTION(__getfunctions)
{
	sdlPtr sdl;
	zval *thisObj;

	GET_THIS_OBJECT(thisObj);

	FETCH_THIS_SDL(sdl);

	if(sdl)
	{
		smart_str buf = {0};
		sdlFunctionPtr *function;
		sdlBindingPtr binding;
		
		FETCH_THIS_PORT(binding);

		array_init(return_value);
 		zend_hash_internal_pointer_reset(binding->functions);
		while(zend_hash_get_current_data(binding->functions, (void **)&function) != FAILURE)
		{
			function_to_string((*function), &buf);
			add_next_index_stringl(return_value, buf.c, buf.len, 1);
			zend_hash_move_forward(binding->functions);
			smart_str_free(&buf);
		}
	}
}

PHP_FUNCTION(__gettypes)
{
	sdlPtr sdl;
	zval *thisObj;

	GET_THIS_OBJECT(thisObj);

	FETCH_THIS_SDL(sdl);

	if(sdl)
	{
		sdlTypePtr *type;
		smart_str buf = {0};

		array_init(return_value);
		if(sdl->types)
		{
			zend_hash_internal_pointer_reset(sdl->types);
			while(zend_hash_get_current_data(sdl->types, (void **)&type) != FAILURE)
			{
				type_to_string((*type), &buf, 0);
				add_next_index_stringl(return_value, buf.c, buf.len, 1);
				zend_hash_move_forward(sdl->types);
				smart_str_free(&buf);
			}
		}
	}
}

PHP_FUNCTION(__getlastrequest)
{
	zval *thisObj;
	zval **tmp;

	GET_THIS_OBJECT(thisObj);

	if(zend_hash_find(Z_OBJPROP_P(thisObj), "__last_request", sizeof("__last_request"), (void **)&tmp) == SUCCESS)
	{
		*return_value = *(*tmp);
		zval_copy_ctor(return_value);
		return;
	}
	RETURN_NULL();
}

PHP_FUNCTION(__getlastresponse)
{
	zval *thisObj;
	zval **tmp;

	GET_THIS_OBJECT(thisObj);

	if(zend_hash_find(Z_OBJPROP_P(thisObj), "__last_response", sizeof("__last_response"), (void **)&tmp) == SUCCESS)
	{
		*return_value = *(*tmp);
		zval_copy_ctor(return_value);
		return;
	}
	RETURN_NULL();
}

void soap_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	pval *object = property_reference->object;
	zend_overloaded_element *function_name = (zend_overloaded_element *)property_reference->elements_list->tail->data;
	zval *thisObj;
	char *function = Z_STRVAL(function_name->element);
	zend_function *builtin_function;

	GET_THIS_OBJECT(thisObj);

	//Find if the function being called is already defined...
	//  ( IMHO: zend should handle this functionality )
	if(zend_hash_find(&Z_OBJCE_P(thisObj)->function_table, function, Z_STRLEN(function_name->element) + 1, (void **) &builtin_function) == SUCCESS)
	{
		builtin_function->internal_function.handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}
	else
	{
		zval **arguments = (zval **) emalloc(sizeof(zval *) * ZEND_NUM_ARGS());
		int arg_count = ZEND_NUM_ARGS();
		xmlDocPtr request = NULL;
		sdlPtr sdl;
		sdlFunctionPtr fn;

		zend_get_parameters_array(ht, arg_count, arguments);

		FETCH_THIS_SDL(sdl);

		clear_soap_fault(thisObj);

		if(sdl != NULL)
		{
			sdlBindingPtr binding;

			FETCH_THIS_PORT(binding);
			fn = get_function(binding, function);
			if(fn != NULL)
			{
				int num_params;
				zval **ret_params;
				char *buffer;
				char *ns;
				int len;

				if(binding->bindingType == BINDING_SOAP)
				{
					sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)fn->bindingAttributes;
					request = seralize_function_call(this_ptr, fn, NULL, fnb->input.ns, arguments, arg_count);
					send_http_soap_request(getThis(), request, fn->functionName, fnb->soapAction);
				}
				else
				{
					request = seralize_function_call(this_ptr, fn, NULL, sdl->target_ns, arguments, arg_count);
					send_http_soap_request(getThis(), request, fn->functionName, NULL);
				}

				xmlFreeDoc(request);

				get_http_soap_response(getThis(), &buffer, &len);
				parse_packet_soap(getThis(), buffer, len, fn, NULL, &ret_params, &num_params);
				efree(buffer);

				if(num_params > 0)
				{
					*return_value = *ret_params[0];
					zval_add_ref(&return_value);
					efree(ret_params);
				}
				else
					ZVAL_NULL(return_value);
			}
			else
			{
				php_error(E_WARNING,"Function (\"%s\") not is not a valid method for this service", function);
			}
		}
		else
		{
			int num_params;
			zval **ret_params;
			zval **uri;
			smart_str *action;
			char *buffer;
			int len;

			if(zend_hash_find(Z_OBJPROP_P(thisObj), "uri", sizeof("uri"), (void *)&uri) == FAILURE)
				php_error(E_ERROR, "Error finding uri in soap_call_function_handler");

			request = seralize_function_call(this_ptr, NULL, function, Z_STRVAL_PP(uri), arguments, arg_count);
			action = build_soap_action(thisObj, function);
			send_http_soap_request(getThis(), request, function, action->c);

			smart_str_free(action);
			xmlFreeDoc(request);

			get_http_soap_response(getThis(), &buffer, &len);
			parse_packet_soap(getThis(), buffer, len, NULL, function, &ret_params, &num_params);
			efree(buffer);

			if(num_params > 0)
			{
				*return_value = *ret_params[0];
				zval_add_ref(&return_value);
				efree(ret_params);
			}
			else
				ZVAL_NULL(return_value);
		}
		efree(arguments);
	}

	zval_dtor(&function_name->element);
}

void clear_soap_fault(zval *obj)
{
	if(obj != NULL && obj->type == IS_OBJECT)
		zend_hash_del(obj->value.obj.properties, "__soap_fault", sizeof("__soap_fault"));
}

void add_soap_fault(zval *obj, char *fault_code, char *fault_string, char *fault_actor, zval *fault_detail)
{
	zval *fault;
	MAKE_STD_ZVAL(fault);
	set_soap_fault(fault, fault_string, fault_code, fault_actor, fault_detail);
	add_property_zval(obj, "__soap_fault", fault);
}

void set_soap_fault(zval *obj, char *fault_code, char *fault_string, char *fault_actor, zval *fault_detail)
{
	TSRMLS_FETCH();

	if(Z_TYPE_P(obj) != IS_OBJECT)
		object_init_ex(obj, &soap_fault_class_entry);

	if(fault_string != NULL)
		add_property_string(obj, "faultstring", fault_string, 1);

	if(fault_code != NULL)
		add_property_string(obj, "faultcode", fault_code, 1);

	if(fault_actor != NULL)
		add_property_string(obj, "faultactor", fault_actor, 1);

	if(fault_detail != NULL)
	{
		zval_add_ref(&fault_detail);
		add_property_zval(obj, "detail", fault_detail);
	}
}

void deseralize_function_call(sdlPtr sdl, xmlDocPtr request, zval *function_name, int *num_params, zval ***parameters)
{
	xmlNodePtr trav,trav2,trav3,trav4,env,body;
	int cur_param = 0,num_of_params = 0;
	TSRMLS_FETCH();

	trav = request->children;
	FOREACHNODE(trav,"Envelope",env)
	{
		trav2 = env->children;
		FOREACHNODE(trav2,"Body",body)
		{
			trav3 = body->children;
			do
			{
				// TODO: make 'strict' (use th sdl defnintions)
				if(trav3->type == XML_ELEMENT_NODE)
				{
					zval tmp_function_name, **tmp_parameters;
					sdlFunctionPtr function;
					sdlBindingPtr binding = get_binding_from_type(sdl, BINDING_SOAP);

					INIT_ZVAL(tmp_function_name);
					ZVAL_STRING(&tmp_function_name, (char *)trav3->name, 1);

					(*function_name) = tmp_function_name;

					function = get_function(binding, php_strtolower((char *)trav3->name, strlen(trav3->name)));
					if(sdl != NULL && function == NULL)
						php_error(E_ERROR, "Error function \"%s\" doesn't exists for this service \"%s\"", trav3->name, binding->location);

					if(trav3->children)
					{
						trav4 = trav3->children;
						if(function == NULL)
						{
							do
							{
								if(trav4->type == XML_ELEMENT_NODE)
									num_of_params++;

							}while(trav4 = trav4->next);
						}
						else
							num_of_params = zend_hash_num_elements(function->requestParameters);

						tmp_parameters = emalloc(num_of_params * sizeof(zval *));
						trav4 = trav3->children;
						do
						{
							if(trav4->type == XML_ELEMENT_NODE)
							{
								encodePtr enc;
								sdlParamPtr *param = NULL;

								if(function != NULL && zend_hash_index_find(function->requestParameters, cur_param, (void **)&param) == FAILURE)
									php_error(E_ERROR, "Error cannot find parameter");
								if(param == NULL)
									enc = get_conversion(UNKNOWN_TYPE);
								else
									enc = (*param)->encode;

								tmp_parameters[cur_param] = master_to_zval(enc, trav4);
								cur_param++;
							}

						}while(trav4 = trav4->next);
					}
					(*parameters) = tmp_parameters;
					(*num_params) = num_of_params;
					break;
				}
			}while(trav3 = trav3->next);

		}
		ENDFOREACH(trav2);
	}
	ENDFOREACH(trav);
}

xmlDocPtr seralize_response_call(sdlFunctionPtr function, char *function_name, char *uri, zval *ret)
{
	xmlDoc *doc;
	xmlNode *envelope,*body,*method, *param;
	xmlNs *ns;
	sdlParamPtr parameter = NULL;
	smart_str *gen_ns;

	encode_reset_ns();

	doc = xmlNewDoc("1.0");
	doc->charset = XML_CHAR_ENCODING_UTF8;
	doc->encoding = xmlStrdup((xmlChar*)"UTF-8");
	doc->children = xmlNewDocNode(doc, NULL, "SOAP-ENV:Envelope", NULL);
	envelope = doc->children;

	xmlSetProp(envelope, "SOAP-ENV:encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
	xmlSetProp(envelope, "xmlns:SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/");
	xmlSetProp(envelope, "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	xmlSetProp(envelope, "xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	xmlSetProp(envelope, "xmlns:" APACHE_NS_PREFIX , APACHE_NAMESPACE);

	ns = xmlNewNs(envelope,"http://schemas.xmlsoap.org/soap/envelope/","SOAP-ENV");
	body = xmlNewChild(envelope, ns, "Body", NULL);

	if(Z_TYPE_P(ret) == IS_OBJECT &&
		Z_OBJCE_P(ret)->refcount == soap_fault_class_entry.refcount)
	{
		param = seralize_zval(ret, NULL, "SOAP-ENV:Fault", SOAP_ENCODED);
		xmlAddChild(body, param);
	}
	else
	{
		gen_ns = encode_new_ns();
		ns = xmlNewNs(envelope, uri, gen_ns->c);

		if(function != NULL)
			method = xmlNewChild(body, ns, function->responseName , NULL);
		else
			method = xmlNewChild(body, ns, function_name, NULL);

		if(uri)
			ns = xmlNewNs(method, uri, NULL);

		parameter = get_param(function, NULL, 0, TRUE);

		if(Z_TYPE_P(ret) == IS_OBJECT &&
			Z_OBJCE_P(ret)->refcount == soap_param_class_entry.refcount)
		{
			zval **ret_name;
			zval **ret_data;

			if(zend_hash_find(Z_OBJPROP_P(ret), "param_name", sizeof("param_name"), (void **)&ret_name) == SUCCESS &&
				zend_hash_find(Z_OBJPROP_P(ret), "param_data", sizeof("param_data"), (void **)&ret_data) == SUCCESS)
				param = seralize_parameter(parameter, *ret_data, 0, Z_STRVAL_PP(ret_name), SOAP_ENCODED);
			else
				param = seralize_parameter(parameter, ret, 0, "return", SOAP_ENCODED);
		}
		else
			param = seralize_parameter(parameter, ret, 0, "return", SOAP_ENCODED);

		xmlAddChild(method,param);
	}

	return doc;
}

xmlDocPtr seralize_function_call(zval *this_ptr, sdlFunctionPtr function, char *function_name, char *uri, zval **arguments, int arg_count)
{
	xmlDoc *doc;
	xmlNode *envelope, *body, *method;
	xmlNs *ns, *tmpns;
	zval **zstyle, **zuse;
	int i, style, use;
	smart_str *gen_ns;

	encode_reset_ns();

	doc = xmlNewDoc("1.0");
	doc->encoding = xmlStrdup((xmlChar*)"UTF-8");
	doc->charset = XML_CHAR_ENCODING_UTF8;
	envelope = xmlNewDocNode(doc, NULL, "SOAP-ENV:Envelope", NULL);
	xmlDocSetRootElement(doc, envelope);
	xmlSetProp(envelope, "SOAP-ENV:encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
	xmlSetProp(envelope, "xmlns:SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/");
	xmlSetProp(envelope, "xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	xmlSetProp(envelope, "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	
	ns = xmlNewNs(envelope, "http://schemas.xmlsoap.org/soap/envelope/", "SOAP-ENV");
	body = xmlNewChild(envelope, ns, "Body", NULL);

	gen_ns = encode_new_ns();

	if(function)
	{
		if(function->bindingType == BINDING_SOAP)
		{
			sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)function->bindingAttributes;

			ns = xmlNewNs(body, fnb->input.ns, gen_ns->c);
			style = fnb->style;
			use = fnb->input.use;
			if(style == SOAP_RPC)
				method = xmlNewChild(body, ns, function->requestName , NULL);
		}
	}
	else
	{
		if(zend_hash_find(Z_OBJPROP_P(this_ptr), "style", sizeof("style"), (void **)&zstyle) == SUCCESS)
			style = Z_LVAL_PP(zstyle);
		else
			style = SOAP_RPC;

		if(style == SOAP_RPC)
		{
			ns = xmlNewNs(body, uri, gen_ns->c);
			method = xmlNewChild(body, ns, function_name, NULL);
		}

		if(zend_hash_find(Z_OBJPROP_P(this_ptr), "use", sizeof("use"), (void **)&zuse) == SUCCESS)
			use = Z_LVAL_PP(zuse);
		else
			use = SOAP_ENCODED;
	}

	for(i = 0;i < arg_count;i++)
	{
		xmlNodePtr param;
		sdlParamPtr parameter = get_param(function, NULL, i, FALSE);

		if(Z_TYPE_P(arguments[i]) == IS_OBJECT &&
			Z_OBJCE_P(arguments[i])->refcount == soap_param_class_entry.refcount)
		{
			zval **ret_name;
			zval **ret_data;

			if(zend_hash_find(Z_OBJPROP_P(arguments[i]), "param_name", sizeof("param_name"), (void **)&ret_name) == SUCCESS &&
				zend_hash_find(Z_OBJPROP_P(arguments[i]), "param_data", sizeof("param_data"), (void **)&ret_data) == SUCCESS)
				param = seralize_parameter(parameter, *ret_data, i, Z_STRVAL_PP(ret_name), use);
			else
				param = seralize_parameter(parameter, arguments[i], i, NULL, use);
		}
		else
			param = seralize_parameter(parameter, arguments[i], i, NULL, use);

		if(style == SOAP_RPC)
			xmlAddChild(method, param);
		else if(style == SOAP_DOCUMENT)
		{
			if(function && function->bindingType == BINDING_SOAP)
			{
				sdlParamPtr *sparam;

				if(zend_hash_index_find(function->requestParameters, 0, (void **)&sparam) == SUCCESS)
				{
					ns = xmlNewNs(param, (*sparam)->encode->details.ns, gen_ns->c);
					xmlNodeSetName(param, (*sparam)->encode->details.type_str);
					xmlSetNs(param, ns);
				}
			}
			xmlAddChild(body, param);
		}
	}
	smart_str_free(gen_ns);

	return doc;
}

xmlNodePtr seralize_parameter(sdlParamPtr param, zval *param_val, int index, char *name, int style)
{
	int type = 0;
	char *paramName;
	xmlNodePtr xmlParam;

	if(param != NULL && param->paramName != NULL)
		paramName = estrdup(param->paramName);
	else
	{
		if(name == NULL)
		{
			paramName = emalloc(10);
			sprintf(paramName,"param%d",index);
		}
		else
			paramName = estrdup(name);
	}

	xmlParam = seralize_zval(param_val, param, paramName, style);

	efree(paramName);

	return xmlParam;
}

zval *desearlize_zval(sdlPtr sdl, xmlNodePtr data, sdlParamPtr param)
{
	encodePtr enc;
	TSRMLS_FETCH();

	if(param != NULL)
		enc = param->encode;
	else
		enc = get_conversion(UNKNOWN_TYPE);

	return enc->to_zval(enc->details, data);
}

xmlNodePtr seralize_zval(zval *val, sdlParamPtr param, char *paramName, int style)
{
	xmlNodePtr xmlParam;
	encodePtr enc;
	TSRMLS_FETCH();

	if(param != NULL)
		enc = param->encode;
	else
  		enc = get_conversion(val->type);

	xmlParam = master_to_xml(enc, val, style);
	if(!strcmp(xmlParam->name, "BOGUS"))
		xmlNodeSetName(xmlParam, paramName);

	return xmlParam;
}

sdlParamPtr get_param(sdlFunctionPtr function, char *param_name, int index, int response)
{
	sdlParamPtr *tmp = NULL;
	HashTable *h;

	if(function == NULL)
		return NULL;

	if(response == FALSE)
		h = function->requestParameters;
	else
		h = function->responseParameters;

	if (function != NULL && (param_name == NULL || zend_hash_find(h, param_name, strlen(param_name), (void **)&tmp) == FAILURE))
	{
		if(index != -1)
			if(zend_hash_index_find(h, index, (void **)&tmp) != FAILURE)
				return (*tmp);
	}
	else
		return (*tmp);

	return NULL;
}

sdlFunctionPtr get_function(sdlBindingPtr sdl, char *function_name)
{
	sdlFunctionPtr *tmp;
	if(sdl != NULL)
		if(zend_hash_find(sdl->functions, function_name, strlen(function_name), (void **)&tmp) != FAILURE)
			return (*tmp);
	return NULL;
}

static void function_to_string(sdlFunctionPtr function, smart_str *buf)
{
	int i = 0;

	if(function->responseParameters)
	{
		sdlParamPtr *param;
		param = function->responseParameters->pListHead->pData;
		smart_str_appendl(buf, (*param)->encode->details.type_str, strlen((*param)->encode->details.type_str));
		smart_str_appendc(buf, ' ');
	}
	else
		smart_str_appendl(buf, "void ", 5);

	smart_str_appendl(buf, function->functionName, strlen(function->functionName));

	smart_str_appendc(buf, '(');
	if(function->requestParameters)
	{
		sdlParamPtr *param;

		i = 0;
		zend_hash_internal_pointer_reset(function->requestParameters);
		while(zend_hash_get_current_data(function->requestParameters, (void **)&param) != FAILURE)
		{
			smart_str_appendl(buf, (*param)->encode->details.type_str, strlen((*param)->encode->details.type_str));
			smart_str_appendc(buf, ' ');
			smart_str_appendc(buf, '$');
			smart_str_appendl(buf, (*param)->paramName, strlen((*param)->paramName));
			if(zend_hash_num_elements(function->requestParameters) > i + 1)
				smart_str_appendl(buf, ", ", 2);
			zend_hash_move_forward(function->requestParameters);
			i++;
		}
	}
	smart_str_appendc(buf, ')');
}

static void type_to_string(sdlTypePtr type, smart_str *buf, int level)
{
	int i;
	smart_str spaces = {0};

	for(i = 0;i < level;i++)
		smart_str_appendc(&spaces, ' ');

	smart_str_appendl(buf, spaces.c, spaces.len);
	if(type->elements)
	{
		sdlTypePtr *t_type;

		smart_str_appendl(buf, "struct ", 7);
		smart_str_appendl(buf, type->name, strlen(type->name));
		smart_str_appendc(buf, '\n');
		smart_str_appendl(buf, spaces.c, spaces.len);
		smart_str_appendl(buf, "{\n", 2);

		zend_hash_internal_pointer_reset(type->elements);
		level++;
		while(zend_hash_get_current_data(type->elements, (void **)&t_type) != FAILURE)
		{
			type_to_string((*t_type), buf, level);
			zend_hash_move_forward(type->elements);
		}

		smart_str_appendl(buf, spaces.c, spaces.len);
		smart_str_appendl(buf, "}\n", 2);
	}
	else
	{
		smart_str_appendl(buf, type->encode->details.type_str, strlen(type->encode->details.type_str));
		smart_str_appendc(buf, ' ');
		smart_str_appendl(buf, type->name, strlen(type->name));
		smart_str_appendl(buf, ";\n", 2);
	}
}

//Deletes
void delete_sdl(void *handle)
{
	sdlPtr tmp = *((sdlPtr*)handle);

	xmlFreeDoc(tmp->doc);
	if(tmp->source)
		free(tmp->source);
	if(tmp->target_ns)
		free(tmp->target_ns);
	if(tmp->encoders)
	{
		zend_hash_destroy(tmp->encoders);
		free(tmp->encoders);
	}
	if(tmp->types)
	{
		zend_hash_destroy(tmp->types);
		free(tmp->types);
	}
	if(tmp->bindings)
	{
		zend_hash_destroy(tmp->bindings);
		free(tmp->bindings);
	}
	free(tmp);
}

void delete_http_socket(void *handle)
{
	SOAP_STREAM stream = (SOAP_STREAM)handle;
#ifdef PHP_STREAMS
	TSRMLS_FETCH();
	php_stream_close(stream);
#else
	SOCK_CLOSE(stream);
#endif
}

void delete_url(void *handle)
{
	php_url_free((php_url*)handle);
}

void delete_service(void *data)
{
	soapServicePtr service = (soapServicePtr)data;

	if(service->soap_functions.ft)
	{
		zend_hash_destroy(service->soap_functions.ft);
		efree(service->soap_functions.ft);
	}

	if(service->mapping)
	{
		zend_hash_destroy(service->mapping);
		efree(service->mapping);
	}

	if(service->soap_class.argc)
	{
		int i;
		for(i = 0; i < service->soap_class.argc;i++)
			zval_ptr_dtor(&service->soap_class.argv[i]);
		efree(service->soap_class.argv);
	}

	efree(service->uri);
	efree(service);
}

void delete_binding(void *data)
{
	sdlBindingPtr binding = *((sdlBindingPtr*)data);

	if(binding->functions)
	{
		zend_hash_destroy(binding->functions);
		free(binding->functions);
	}

	if(binding->location)
		free(binding->location);
	if(binding->name)
		free(binding->name);

	if(binding->bindingType == BINDING_SOAP)
	{
		sdlSoapBindingPtr soapBind = binding->bindingAttributes;
		free(soapBind->transport);
	}
}

void delete_function(void *data)
{
	sdlFunctionPtr function = *((sdlFunctionPtr*)data);

	if(function->functionName)
		free(function->functionName);
	if(function->requestName)
		free(function->requestName);
	if(function->responseName)
		free(function->responseName);

	if(function->requestParameters)
	{
		zend_hash_destroy(function->requestParameters);
		free(function->requestParameters);
	}
	if(function->responseParameters)
	{
		zend_hash_destroy(function->responseParameters);
		free(function->responseParameters);
	}

	if(function->bindingType == BINDING_SOAP)
	{
		sdlSoapBindingFunctionPtr soapFunction = function->bindingAttributes;
		if(soapFunction->soapAction)
			free(soapFunction->soapAction);
		delete_sdl_soap_binding_function_body(soapFunction->input);
		delete_sdl_soap_binding_function_body(soapFunction->output);
		delete_sdl_soap_binding_function_body(soapFunction->falut);
	}
}

void delete_sdl_soap_binding_function_body(sdlSoapBindingFunctionBody body)
{
	if(body.ns)
		free(body.ns);
	if(body.parts)
		free(body.parts);
	if(body.encodingStyle)
		free(body.encodingStyle);
}

void delete_paramater(void *data)
{
	sdlParamPtr param = *((sdlParamPtr*)data);
	if(param->paramName)
		free(param->paramName);
	free(param);
}

void delete_mapping(void *data)
{
	soapMappingPtr map = (soapMappingPtr)data;

	if(map->ns)
		efree(map->ns);
	if(map->ctype)
		efree(map->ctype);

	if(map->type == SOAP_MAP_FUNCTION)
	{
		if(map->map_functions.to_xml_before)
			zval_ptr_dtor(&map->map_functions.to_xml_before);
		if(map->map_functions.to_xml)
			zval_ptr_dtor(&map->map_functions.to_xml);
		if(map->map_functions.to_xml_after)
			zval_ptr_dtor(&map->map_functions.to_xml_after);

		if(map->map_functions.to_zval_before)
			zval_ptr_dtor(&map->map_functions.to_zval_before);
		if(map->map_functions.to_zval)
			zval_ptr_dtor(&map->map_functions.to_zval);
		if(map->map_functions.to_zval_after)
			zval_ptr_dtor(&map->map_functions.to_zval_after);
	}
	efree(map);
}

// Should not need
int my_call_user_function(HashTable *function_table, zval **object_pp, zval *function_name, zval *retval_ptr, int param_count, zval *params[] TSRMLS_DC)
{
	if(call_user_function(function_table, object_pp, function_name, retval_ptr, param_count, params TSRMLS_CC) == FAILURE)
	{
		if(Z_OBJCE_PP(object_pp)->handle_function_call != NULL)
		{
			zend_overloaded_element overloaded_element;
			zend_property_reference property_reference;
			zend_function_state function_state;
			zend_function_state *original_function_state_ptr;
			int i;

			overloaded_element.element = *function_name;
			overloaded_element.type = OE_IS_METHOD;

			function_state.function = (zend_function *) emalloc(sizeof(zend_function));
			function_state.function->type = ZEND_OVERLOADED_FUNCTION;
			function_state.function->common.arg_types = NULL;
			function_state.function->overloaded_function.function_name = Z_STRVAL_P(function_name);

			property_reference.object = *object_pp;
			property_reference.type = BP_VAR_NA;
			property_reference.elements_list = (zend_llist *)emalloc(sizeof(zend_llist));
			zend_llist_init(property_reference.elements_list, sizeof(zend_overloaded_element), NULL, 0);
			zend_llist_add_element(property_reference.elements_list, &overloaded_element);

			//Build argument stack
			for(i = 0;i < param_count;i++)
				zend_ptr_stack_push(&EG(argument_stack), params[i]);
			zend_ptr_stack_n_push(&EG(argument_stack), 2, (void *)param_count, NULL);

			original_function_state_ptr = EG(function_state_ptr);
			EG(function_state_ptr) = &function_state;
			Z_OBJCE_PP(object_pp)->handle_function_call(param_count, retval_ptr, *object_pp, 1 TSRMLS_CC, &property_reference);
			EG(function_state_ptr) = original_function_state_ptr;

			zend_llist_destroy(property_reference.elements_list);
			efree(property_reference.elements_list);
			efree(function_state.function);

			zend_ptr_stack_clear_multiple(TSRMLS_C);
			return SUCCESS;
		}
	}
	return FAILURE;
}


