#include <time.h>

#include "php_soap.h"

/* zval type decode */
static zval *to_zval_double(encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_long(encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_ulong(encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_bool(encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_string(encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_stringr(encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_stringc(encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_map(encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_null(encodeTypePtr type, xmlNodePtr data);

static xmlNodePtr to_xml_long(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_ulong(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_double(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_bool(encodeTypePtr type, zval *data, int style);

/* String encode */
static xmlNodePtr to_xml_string(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_stringl(encodeTypePtr type, zval *data, int style);

/* Null encode */
static xmlNodePtr to_xml_null(encodeTypePtr type, zval *data, int style);

/* Array encode */
static xmlNodePtr guess_array_map(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_map(encodeTypePtr type, zval *data, int style);

static xmlNodePtr to_xml_list(encodeTypePtr enc, zval *data, int style);
static xmlNodePtr to_xml_list1(encodeTypePtr enc, zval *data, int style);

/* Datetime encode/decode */
static xmlNodePtr to_xml_datetime_ex(encodeTypePtr type, zval *data, char *format, int style);
static xmlNodePtr to_xml_datetime(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_time(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_date(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_gyearmonth(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_gyear(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_gmonthday(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_gday(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_gmonth(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_duration(encodeTypePtr type, zval *data, int style);

static zval *to_zval_object(encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_array(encodeTypePtr type, xmlNodePtr data);

static xmlNodePtr to_xml_object(encodeTypePtr type, zval *data, int style);
static xmlNodePtr to_xml_array(encodeTypePtr type, zval *data, int style);

/* Try and guess for non-wsdl clients and servers */
static zval *guess_zval_convert(encodeTypePtr type, xmlNodePtr data);
static xmlNodePtr guess_xml_convert(encodeTypePtr type, zval *data, int style);

static int is_map(zval *array);
static void get_array_type(xmlNodePtr node, zval *array, smart_str *out_type TSRMLS_DC);

static void get_type_str(xmlNodePtr node, const char* ns, const char* type, smart_str* ret);
static void set_ns_and_type_ex(xmlNodePtr node, char *ns, char *type);

static void set_ns_and_type(xmlNodePtr node, encodeTypePtr type);

#define FIND_XML_NULL(xml,zval) \
	{ \
		xmlAttrPtr null; \
		if (!xml) { \
			ZVAL_NULL(zval); \
			return zval; \
		} \
		if (xml->properties) { \
			null = get_attribute(xml->properties, "nil"); \
			if (null) { \
				ZVAL_NULL(zval); \
				return zval; \
			} \
		} \
	}

#define FIND_ZVAL_NULL(zval, xml, style) \
{ \
	if (!zval || Z_TYPE_P(zval) == IS_NULL) { \
	  if (style == SOAP_ENCODED) {\
			xmlSetProp(xml, "xsi:nil", "1"); \
		}\
		return xml; \
	} \
}

encode defaultEncoding[] = {
	{{UNKNOWN_TYPE, NULL, NULL, NULL}, guess_zval_convert, guess_xml_convert},

	{{IS_NULL, "nil", XSI_NAMESPACE, NULL}, to_zval_null, to_xml_null},
	{{IS_STRING, XSD_STRING_STRING, XSD_NAMESPACE, NULL}, to_zval_string, to_xml_string},
	{{IS_LONG, XSD_INT_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{IS_DOUBLE, XSD_FLOAT_STRING, XSD_NAMESPACE, NULL}, to_zval_double, to_xml_double},
	{{IS_BOOL, XSD_BOOLEAN_STRING, XSD_NAMESPACE, NULL}, to_zval_bool, to_xml_bool},
	{{IS_CONSTANT, XSD_STRING_STRING, XSD_NAMESPACE, NULL}, to_zval_string, to_xml_string},
	{{IS_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_1_ENC_NAMESPACE, NULL}, to_zval_array, guess_array_map},
	{{IS_CONSTANT_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_1_ENC_NAMESPACE, NULL}, to_zval_array, to_xml_array},
	{{IS_OBJECT, SOAP_ENC_OBJECT_STRING, SOAP_1_1_ENC_NAMESPACE, NULL}, to_zval_object, to_xml_object},
	{{IS_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_2_ENC_NAMESPACE, NULL}, to_zval_array, guess_array_map},
	{{IS_CONSTANT_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_2_ENC_NAMESPACE, NULL}, to_zval_array, to_xml_array},
	{{IS_OBJECT, SOAP_ENC_OBJECT_STRING, SOAP_1_2_ENC_NAMESPACE, NULL}, to_zval_object, to_xml_object},

	{{XSD_STRING, XSD_STRING_STRING, XSD_NAMESPACE, NULL}, to_zval_string, to_xml_string},
	{{XSD_BOOLEAN, XSD_BOOLEAN_STRING, XSD_NAMESPACE, NULL}, to_zval_bool, to_xml_bool},
	{{XSD_DECIMAL, XSD_DECIMAL_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_FLOAT, XSD_FLOAT_STRING, XSD_NAMESPACE, NULL}, to_zval_double, to_xml_double},
	{{XSD_DOUBLE, XSD_DOUBLE_STRING, XSD_NAMESPACE, NULL}, to_zval_double, to_xml_double},

	{{XSD_DATETIME, XSD_DATETIME_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_datetime},
	{{XSD_TIME, XSD_TIME_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_time},
	{{XSD_DATE, XSD_DATE_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_date},
	{{XSD_GYEARMONTH, XSD_GYEARMONTH_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_gyearmonth},
	{{XSD_GYEAR, XSD_GYEAR_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_gyear},
	{{XSD_GMONTHDAY, XSD_GMONTHDAY_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_gmonthday},
	{{XSD_GDAY, XSD_GDAY_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_gday},
	{{XSD_GMONTH, XSD_GMONTH_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_gmonth},
	{{XSD_DURATION, XSD_DURATION_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_duration},

	{{XSD_HEXBINARY, XSD_HEXBINARY_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_stringl},
	{{XSD_BASE64BINARY, XSD_BASE64BINARY_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_stringl},

	{{XSD_LONG, XSD_LONG_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_INT, XSD_INT_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_SHORT, XSD_SHORT_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_BYTE, XSD_BYTE_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_NONPOSITIVEINTEGER, XSD_NONPOSITIVEINTEGER_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_POSITIVEINTEGER, XSD_POSITIVEINTEGER_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_NONNEGATIVEINTEGER, XSD_NONNEGATIVEINTEGER_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_NEGATIVEINTEGER, XSD_NEGATIVEINTEGER_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_UNSIGNEDBYTE, XSD_UNSIGNEDBYTE_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_UNSIGNEDSHORT, XSD_UNSIGNEDSHORT_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_UNSIGNEDINT, XSD_UNSIGNEDINT_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_UNSIGNEDLONG, XSD_UNSIGNEDLONG_STRING, XSD_NAMESPACE, NULL}, to_zval_ulong, to_xml_ulong},

	{{XSD_ANYTYPE, XSD_ANYTYPE_STRING, XSD_NAMESPACE, NULL}, guess_zval_convert, guess_xml_convert},
	{{XSD_UR_TYPE, XSD_UR_TYPE_STRING, XSD_NAMESPACE, NULL}, guess_zval_convert, guess_xml_convert},
	{{XSD_ANYURI, XSD_ANYURI_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_QNAME, XSD_QNAME_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_NOTATION, XSD_NOTATION_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_NORMALIZEDSTRING, XSD_NORMALIZEDSTRING_STRING, XSD_NAMESPACE, NULL}, to_zval_stringr, to_xml_string},
	{{XSD_TOKEN, XSD_TOKEN_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_LANGUAGE, XSD_LANGUAGE_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_NMTOKEN, XSD_NMTOKEN_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_NMTOKENS, XSD_NMTOKENS_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_list1},
	{{XSD_NAME, XSD_NAME_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_NCNAME, XSD_NCNAME_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_ID, XSD_ID_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_IDREF, XSD_IDREF_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_IDREFS, XSD_IDREFS_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_list1},
	{{XSD_ENTITY, XSD_ENTITY_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_ENTITIES, XSD_ENTITIES_STRING, XSD_NAMESPACE, NULL}, to_zval_stringc, to_xml_list1},

	{{APACHE_MAP, APACHE_MAP_STRING, APACHE_NAMESPACE, NULL}, to_zval_map, to_xml_map},

	{{SOAP_ENC_OBJECT, SOAP_ENC_OBJECT_STRING, SOAP_1_1_ENC_NAMESPACE, NULL}, to_zval_object, to_xml_object},
	{{SOAP_ENC_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_1_ENC_NAMESPACE, NULL}, to_zval_array, to_xml_array},
	{{SOAP_ENC_OBJECT, SOAP_ENC_OBJECT_STRING, SOAP_1_2_ENC_NAMESPACE, NULL}, to_zval_object, to_xml_object},
	{{SOAP_ENC_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_2_ENC_NAMESPACE, NULL}, to_zval_array, to_xml_array},

	/* support some of the 1999 data types */
	{{XSD_STRING, XSD_STRING_STRING, XSD_1999_NAMESPACE, NULL}, to_zval_string, to_xml_string},
	{{XSD_BOOLEAN, XSD_BOOLEAN_STRING, XSD_1999_NAMESPACE, NULL}, to_zval_bool, to_xml_bool},
	{{XSD_DECIMAL, XSD_DECIMAL_STRING, XSD_1999_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_FLOAT, XSD_FLOAT_STRING, XSD_1999_NAMESPACE, NULL}, to_zval_double, to_xml_double},
	{{XSD_DOUBLE, XSD_DOUBLE_STRING, XSD_1999_NAMESPACE, NULL}, to_zval_double, to_xml_double},

	{{XSD_LONG, XSD_LONG_STRING, XSD_1999_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_INT, XSD_INT_STRING, XSD_1999_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_SHORT, XSD_SHORT_STRING, XSD_1999_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_BYTE, XSD_BYTE_STRING, XSD_1999_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_1999_TIMEINSTANT, XSD_1999_TIMEINSTANT_STRING, XSD_1999_NAMESPACE, NULL}, to_zval_stringc, to_xml_string},

	{{END_KNOWN_TYPES, NULL, NULL, NULL}, guess_zval_convert, guess_xml_convert}
};

void whiteSpace_replace(char* str)
{
	while (*str != '\0') {
		if (*str == '\x9' || *str == '\xA' || *str == '\xD') {
			*str = ' ';
		}
		str++;
	}
}

void whiteSpace_collapse(char* str)
{
	char *orig = str;
	char *tmp = do_alloca(strlen(str)+1);
	char *pos;
	char old;

	whiteSpace_replace(str);
	while (*str == ' ') {
		str++;
	}
	pos = tmp; old = '\0';
	while (*str != '\0') {
		if (*str != ' ' || old != ' ') {
			*pos = *str;
			pos++;
		}
		old = *str;
		str++;
	}
	if (old == ' ') {
	 	--pos;
	}
	*pos = '\0';
	memcpy(orig,tmp,(pos-tmp)+1);
	free_alloca(tmp);
}

xmlNodePtr master_to_xml(encodePtr encode, zval *data, int style)
{
	xmlNodePtr node = NULL;
	TSRMLS_FETCH();

	if (encode == NULL) {
		encode = get_conversion(UNKNOWN_TYPE);
	}
	if (encode->to_xml_before) {
		data = encode->to_xml_before(&encode->details, data);
	}
	if (encode->to_xml) {
		node = encode->to_xml(&encode->details, data, style);
	}
	if (encode->to_xml_after) {
		node = encode->to_xml_after(&encode->details, node, style);
	}
	return node;
}

zval *master_to_zval(encodePtr encode, xmlNodePtr data)
{
	zval *ret = NULL;
	TSRMLS_FETCH();

	if (encode == NULL) {
		encode = get_conversion(UNKNOWN_TYPE);
	}
	data = check_and_resolve_href(data);
	if (encode->to_zval_before) {
		data = encode->to_zval_before(&encode->details, data, 0);
	}
	if (encode->to_zval) {
		ret = encode->to_zval(&encode->details, data);
	}
	if (encode->to_zval_after) {
		ret = encode->to_zval_after(&encode->details, ret);
	}
	return ret;
}

#ifdef HAVE_PHP_DOMXML
zval *to_xml_before_user(encodeTypePtr type, zval *data)
{
	TSRMLS_FETCH();

	if (type.map->map_functions.to_xml_before) {
		if (call_user_function(EG(function_table), NULL, type.map->map_functions.to_xml_before, data, 1, &data  TSRMLS_CC) == FAILURE) {
			php_error(E_ERROR, "Error calling to_xml_before");
		}
	}
	return data;
}

xmlNodePtr to_xml_user(encodeTypePtr type, zval *data, int style)
{
	zval *ret, **addr;
	xmlNodePtr node;
	TSRMLS_FETCH();

	if (type.map->map_functions.to_xml) {
		MAKE_STD_ZVAL(ret);
		if (call_user_function(EG(function_table), NULL, type.map->map_functions.to_xml, ret, 1, &data  TSRMLS_CC) == FAILURE) {
			php_error(E_ERROR, "Error calling to_xml");
		}

		if (Z_TYPE_P(ret) != IS_OBJECT) {
			php_error(E_ERROR, "Error serializing object from to_xml_user");
		}

		if (zend_hash_index_find(Z_OBJPROP_P(ret), 1, (void **)&addr) == SUCCESS) {
			node = (xmlNodePtr)Z_LVAL_PP(addr);
			node = xmlCopyNode(node, 1);
			set_ns_and_type(node, type);
		}
		zval_ptr_dtor(&ret);
	}
	return node;
}

xmlNodePtr to_xml_after_user(encodeTypePtr type, xmlNodePtr node, int style)
{
	zval *ret, *param, **addr;
	int found;
	TSRMLS_FETCH();

	if (type.map->map_functions.to_xml_after) {
		MAKE_STD_ZVAL(ret);
		MAKE_STD_ZVAL(param);
		param = php_domobject_new(node, &found, NULL TSRMLS_CC);

		if (call_user_function(EG(function_table), NULL, type.map->map_functions.to_xml_after, ret, 1, &param  TSRMLS_CC) == FAILURE) {
			php_error(E_ERROR, "Error calling to_xml_after");
		}
		if (zend_hash_index_find(Z_OBJPROP_P(ret), 1, (void **)&addr) == SUCCESS) {
			node = (xmlNodePtr)Z_LVAL_PP(addr);
			set_ns_and_type(node, type);
		}
		zval_ptr_dtor(&ret);
		zval_ptr_dtor(&param);
	}
	return node;
}

xmlNodePtr to_zval_before_user(encodeTypePtr type, xmlNodePtr node, int style)
{
	zval *ret, *param, **addr;
	int found;
	TSRMLS_FETCH();

	if (type.map->map_functions.to_zval_before) {
		MAKE_STD_ZVAL(ret);
		MAKE_STD_ZVAL(param);
		param = php_domobject_new(node, &found, NULL TSRMLS_CC);

		if (call_user_function(EG(function_table), NULL, type.map->map_functions.to_zval_before, ret, 1, &param  TSRMLS_CC) == FAILURE) {
			php_error(E_ERROR, "Error calling to_zval_before");
		}
		if (zend_hash_index_find(Z_OBJPROP_P(ret), 1, (void **)&addr) == SUCCESS) {
			node = (xmlNodePtr)Z_LVAL_PP(addr);
			set_ns_and_type(node, type);
		}
		zval_ptr_dtor(&ret);
		zval_ptr_dtor(&param);
	}
	return node;
}

zval *to_zval_user(encodeTypePtr type, xmlNodePtr node)
{
	zval *ret, *param;
	int found;
	TSRMLS_FETCH();

	if (type.map->map_functions.to_zval) {
		MAKE_STD_ZVAL(ret);
		MAKE_STD_ZVAL(param);
		param = php_domobject_new(node, &found, NULL TSRMLS_CC);

		if (call_user_function(EG(function_table), NULL, type.map->map_functions.to_zval, ret, 1, &param  TSRMLS_CC) == FAILURE) {
			php_error(E_ERROR, "Error calling to_zval");
		}
		zval_ptr_dtor(&param);
		efree(param);
	}
	return ret;
}

zval *to_zval_after_user(encodeTypePtr type, zval *data)
{
	TSRMLS_FETCH();

	if (type.map->map_functions.to_zval_after) {
		if (call_user_function(EG(function_table), NULL, type.map->map_functions.to_zval_after, data, 1, &data  TSRMLS_CC) == FAILURE) {
			php_error(E_ERROR, "Error calling to_xml_before");
		}
	}
	return data;
}
#endif

/* TODO: get rid of "bogus".. ither by passing in the already created xmlnode or passing in the node name */
/* String encode/decode */
static zval *to_zval_string(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			ZVAL_STRING(ret, data->children->content, 1);
		} else {
			php_error(E_ERROR,"Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static zval *to_zval_stringr(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_replace(data->children->content);
			ZVAL_STRING(ret, data->children->content, 1);
		} else {
			php_error(E_ERROR,"Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static zval *to_zval_stringc(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			ZVAL_STRING(ret, data->children->content, 1);
		} else {
			php_error(E_ERROR,"Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static xmlNodePtr to_xml_string(encodeTypePtr type, zval *data, int style)
{
	xmlNodePtr ret;
	char *str, *pstr;
	int new_len;
	TSRMLS_FETCH();

	ret = xmlNewNode(NULL, "BOGUS");
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) == IS_STRING) {
		str = php_escape_html_entities(Z_STRVAL_P(data), Z_STRLEN_P(data), &new_len, 0, 0, NULL TSRMLS_CC);
	} else {
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		str = php_escape_html_entities(Z_STRVAL(tmp), Z_STRLEN(tmp), &new_len, 0, 0, NULL TSRMLS_CC);
		zval_dtor(&tmp);
	}

	pstr = malloc(new_len + 1);
	memcpy(pstr, str, new_len);
	pstr[new_len] = '\0';
	efree(str);

	xmlNodeSetContentLen(ret, pstr, new_len);

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static xmlNodePtr to_xml_stringl(encodeTypePtr type, zval *data, int style)
{
	xmlNodePtr ret;

	ret = xmlNewNode(NULL, "BOGUS");
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) == IS_STRING) {
		xmlNodeSetContentLen(ret, Z_STRVAL_P(data), Z_STRLEN_P(data));
	} else {
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		xmlNodeSetContentLen(ret, Z_STRVAL(tmp), Z_STRLEN(tmp));
		zval_dtor(&tmp);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static zval *to_zval_double(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			ZVAL_DOUBLE(ret, atof(data->children->content));
		} else {
			php_error(E_ERROR,"Violation of encoding rules");
		}
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}

static zval *to_zval_long(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			ZVAL_LONG(ret, atol(data->children->content));
		} else {
			php_error(E_ERROR,"Violation of encoding rules");
		}
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}

static zval *to_zval_ulong(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			errno = 0;
			ret->value.lval = strtol(data->children->content, NULL, 0);
			if (errno == ERANGE) { /* overflow */
				ret->value.dval = strtod(data->children->content, NULL);
				ret->type = IS_DOUBLE;
			} else {
				ret->type = IS_LONG;
			}
		} else {
			php_error(E_ERROR,"Violation of encoding rules");
		}
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}

static xmlNodePtr to_xml_long(encodeTypePtr type, zval *data, int style)
{
	xmlNodePtr ret;
	zval tmp;

	ret = xmlNewNode(NULL, "BOGUS");
	FIND_ZVAL_NULL(data, ret, style);

	tmp = *data;
	zval_copy_ctor(&tmp);
	if (Z_TYPE(tmp) != IS_LONG) {
		convert_to_long(&tmp);
	}
	convert_to_string(&tmp);
	xmlNodeSetContentLen(ret, Z_STRVAL(tmp), Z_STRLEN(tmp));
	zval_dtor(&tmp);

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static xmlNodePtr to_xml_ulong(encodeTypePtr type, zval *data, int style)
{
	xmlNodePtr ret;

	ret = xmlNewNode(NULL, "BOGUS");
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) == IS_DOUBLE) {
		char s[16];
		sprintf(s, "%0.0F",Z_DVAL_P(data));
		xmlNodeSetContent(ret, s);
	} else {
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		if (Z_TYPE(tmp) != IS_LONG) {
			convert_to_long(&tmp);
		}
		convert_to_string(&tmp);
		xmlNodeSetContentLen(ret, Z_STRVAL(tmp), Z_STRLEN(tmp));
		zval_dtor(&tmp);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static xmlNodePtr to_xml_double(encodeTypePtr type, zval *data, int style)
{
	xmlNodePtr ret;
	zval tmp;

	ret = xmlNewNode(NULL, "BOGUS");
	FIND_ZVAL_NULL(data, ret, style);

	tmp = *data;
	zval_copy_ctor(&tmp);
	if (Z_TYPE(tmp) != IS_DOUBLE) {
		convert_to_double(&tmp);
	}
	convert_to_string(&tmp);
	xmlNodeSetContentLen(ret, Z_STRVAL(tmp), Z_STRLEN(tmp));
	zval_dtor(&tmp);

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static zval *to_zval_bool(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			if (stricmp(data->children->content,"true") == 0 ||
				stricmp(data->children->content,"t") == 0 ||
				strcmp(data->children->content,"1") == 0) {
				ZVAL_BOOL(ret, 1);
			} else {
				ZVAL_BOOL(ret, 0);
			}
		} else {
			php_error(E_ERROR,"Violation of encoding rules");
		}
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}

static xmlNodePtr to_xml_bool(encodeTypePtr type, zval *data, int style)
{
	xmlNodePtr ret;
	zval tmp;

	ret = xmlNewNode(NULL, "BOGUS");
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) != IS_BOOL) {
		tmp = *data;
		zval_copy_ctor(&tmp);
		convert_to_boolean(data);
		data = &tmp;
	}

	if (data->value.lval == 1) {
		xmlNodeSetContent(ret, "1");
	} else {
		xmlNodeSetContent(ret, "0");
	}

	if (data == &tmp) {
		zval_dtor(&tmp);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

/* Null encode/decode */
static zval *to_zval_null(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	ZVAL_NULL(ret);
	return ret;
}

static xmlNodePtr to_xml_null(encodeTypePtr type, zval *data, int style)
{
	xmlNodePtr ret = xmlNewNode(NULL, "BOGUS");

	if (style == SOAP_ENCODED) {
		xmlSetProp(ret, "xsi:nil", "1");
	}
	return ret;
}

static void model_to_zval_object(zval *ret, sdlContentModelPtr model, xmlNodePtr data, sdlPtr sdl TSRMLS_DC)
{
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT:
		  if (model->u.element->name) {
		  	xmlNodePtr node = get_node(data->children, model->u.element->name);
		  	if (node) {
					xmlAttrPtr typeAttr = get_attribute(node->properties,"type");
			  	encodePtr  enc = NULL;
			  	zval *val;

					if (typeAttr != NULL && typeAttr->children && typeAttr->children->content) {
						enc = get_encoder_from_prefix(sdl, node, typeAttr->children->content);
					}
					if (enc == NULL) {
						enc = model->u.element->encode;
					}
					val = master_to_zval(enc, node);
					if ((node = get_node(node->next, model->u.element->name)) != NULL) {
						zval *array;
						MAKE_STD_ZVAL(array);
						array_init(array);
						add_next_index_zval(array, val);
						do {
							typeAttr = get_attribute(node->properties,"type");
			  	    enc = NULL;
							if (typeAttr != NULL && typeAttr->children && typeAttr->children->content) {
								enc = get_encoder_from_prefix(sdl, node, typeAttr->children->content);
							}
							if (enc == NULL) {
								enc = model->u.element->encode;
							}
							val = master_to_zval(enc, node);
							add_next_index_zval(array, val);
						} while ((node = get_node(node->next, model->u.element->name)) != NULL);
						val = array;
					}
#ifdef ZEND_ENGINE_2
					val->refcount--;
#endif
					add_property_zval(ret, model->u.element->name, val);
				}
			}
			break;
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_ALL:
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr *tmp;

			zend_hash_internal_pointer_reset(model->u.content);
			while (zend_hash_get_current_data(model->u.content, (void**)&tmp) == SUCCESS) {
				model_to_zval_object(ret, *tmp, data, sdl TSRMLS_CC);
				zend_hash_move_forward(model->u.content);
			}
		}
		case XSD_CONTENT_GROUP:
			model_to_zval_object(ret, model->u.group, data, sdl TSRMLS_CC);
		default:
		  break;
	}
}

/* Struct encode/decode */
static zval *to_zval_object(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret;
	xmlNodePtr trav;
	sdlPtr sdl;
	sdlTypePtr sdlType = type->sdl_type;
	TSRMLS_FETCH();

	sdl = SOAP_GLOBAL(sdl);
	if (sdlType) {
		if (sdlType->kind == XSD_TYPEKIND_RESTRICTION &&
		    sdlType->encode && type != &sdlType->encode->details) {
		  encodePtr enc;

			enc = sdlType->encode;
			while (enc && enc->details.sdl_type &&
		         enc->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
		         enc->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
		         enc->details.sdl_type->kind != XSD_TYPEKIND_UNION) {
		    enc = enc->details.sdl_type->encode;
		  }
		  if (enc) {
				zval *base;

				MAKE_STD_ZVAL(ret);

				object_init(ret);
				base = master_to_zval(enc, data);
#ifdef ZEND_ENGINE_2
				base->refcount--;
#endif
				add_property_zval(ret, "_", base);
			} else {
				MAKE_STD_ZVAL(ret);
				FIND_XML_NULL(data, ret);
				object_init(ret);
			}
		} else if (sdlType->kind == XSD_TYPEKIND_EXTENSION &&
		           sdlType->encode &&
		           type != &sdlType->encode->details) {
		  if (sdlType->encode->details.sdl_type &&
		      sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
		      sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
		      sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_UNION) {		  		
			  ret = master_to_zval(sdlType->encode, data);
				FIND_XML_NULL(data, ret);
			} else {
				zval *base;

				MAKE_STD_ZVAL(ret);

				object_init(ret);
				base = master_to_zval(sdlType->encode, data);
#ifdef ZEND_ENGINE_2
				base->refcount--;
#endif
				add_property_zval(ret, "_", base);
			}
		} else {
			MAKE_STD_ZVAL(ret);
			FIND_XML_NULL(data, ret);
			object_init(ret);
		}
		if (sdlType->model) {
			model_to_zval_object(ret, sdlType->model, data, sdl TSRMLS_CC);
		}
		if (sdlType->attributes) {
			sdlAttributePtr *attr;

			zend_hash_internal_pointer_reset(sdlType->attributes);
			while (zend_hash_get_current_data(sdlType->attributes, (void**)&attr) == SUCCESS) {
				if ((*attr)->name) {
					xmlAttrPtr val = get_attribute(data->properties, (*attr)->name);
					if (val && val->children && val->children->content) {
						xmlNodePtr dummy;
						zval *data;

						dummy = xmlNewNode(NULL, "BOGUS");
						xmlNodeSetContent(dummy, val->children->content);
						data = master_to_zval((*attr)->encode, dummy);
						xmlFreeNode(dummy);
#ifdef ZEND_ENGINE_2
						data->refcount--;
#endif
						add_property_zval(ret, (*attr)->name, data);
					}
				}
				zend_hash_move_forward(sdlType->attributes);
			}
		}
	} else {

		MAKE_STD_ZVAL(ret);
		FIND_XML_NULL(data, ret);
		object_init(ret);

		trav = data->children;

		while (trav != NULL) {
			if (trav->type == XML_ELEMENT_NODE) {
				encodePtr enc = NULL;
				zval *tmpVal;

				xmlAttrPtr typeAttr = get_attribute(trav->properties,"type");
				if (typeAttr != NULL && typeAttr->children && typeAttr->children->content) {
					enc = get_encoder_from_prefix(sdl, trav, typeAttr->children->content);
				}
				tmpVal = master_to_zval(enc, trav);
#ifdef ZEND_ENGINE_2
				tmpVal->refcount--;
#endif
				add_property_zval(ret, (char *)trav->name, tmpVal);
			}
			trav = trav->next;
		}
	}
	return ret;
}

static int model_to_xml_object(xmlNodePtr node, sdlContentModelPtr model, HashTable *prop, int style, int strict)
{
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT: {
			zval **data;
			xmlNodePtr property;
			encodePtr enc;

			if (zend_hash_find(prop, model->u.element->name, strlen(model->u.element->name)+1, (void**)&data) == SUCCESS) {
				enc = model->u.element->encode;
				if ((model->max_occurs == -1 || model->max_occurs > 1) && Z_TYPE_PP(data) == IS_ARRAY) {
					HashTable *ht = Z_ARRVAL_PP(data);
					zval **val;

					zend_hash_internal_pointer_reset(ht);
					while (zend_hash_get_current_data(ht,(void**)&val) == SUCCESS) {
						property = master_to_xml(enc, *val, style);
						xmlNodeSetName(property, model->u.element->name);
						xmlAddChild(node, property);
						zend_hash_move_forward(ht);
					}
				} else {
					property = master_to_xml(enc, *data, style);
					xmlNodeSetName(property, model->u.element->name);
					xmlAddChild(node, property);
				}
				return 1;
			} else if (model->min_occurs == 0) {
				return 1;
			} else {
				if (strict) {
					php_error(E_ERROR, "object hasn't '%s' property",model->u.element->name);
				}
				return 0;
			}
			break;
		}
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_ALL: {
			sdlContentModelPtr *tmp;

			zend_hash_internal_pointer_reset(model->u.content);
			while (zend_hash_get_current_data(model->u.content, (void**)&tmp) == SUCCESS) {
				if (!model_to_xml_object(node, *tmp, prop, style, model->min_occurs > 0)) {
					return 0;
				}
				zend_hash_move_forward(model->u.content);
			}
			return 1;
		}
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr *tmp;

			zend_hash_internal_pointer_reset(model->u.content);
			while (zend_hash_get_current_data(model->u.content, (void**)&tmp) == SUCCESS) {
				if (model_to_xml_object(node, *tmp, prop, style, 0)) {
					return 1;
				}
				zend_hash_move_forward(model->u.content);
			}
			return 0;
		}
		case XSD_CONTENT_GROUP: {
			return model_to_xml_object(node, model->u.group, prop, style, model->min_occurs > 0);
		}
		default:
		  break;
	}
	return 1;
}

static xmlNodePtr to_xml_object(encodeTypePtr type, zval *data, int style)
{
	xmlNodePtr xmlParam;
	HashTable *prop;
	int i;
	sdlTypePtr sdlType = type->sdl_type;
	TSRMLS_FETCH();

	/* Special handling of class SoapVar */
	if (data &&
	    Z_TYPE_P(data) == IS_OBJECT &&
	    Z_OBJCE_P(data) == soap_var_class_entry) {
		zval **ztype, **zdata, **zns, **zstype, **zname, **znamens;
		encodePtr enc;

		if (zend_hash_find(Z_OBJPROP_P(data), "enc_type", sizeof("enc_type"), (void **)&ztype) == FAILURE) {
			php_error(E_ERROR, "error encoding SoapVar");
		}

		enc = get_conversion(Z_LVAL_P(*ztype));

		if (zend_hash_find(Z_OBJPROP_P(data), "enc_value", sizeof("enc_value"), (void **)&zdata) == FAILURE) {
			xmlParam = master_to_xml(enc, NULL, style);
		} else {
			xmlParam = master_to_xml(enc, *zdata, style);
		}

		if (zend_hash_find(Z_OBJPROP_P(data), "enc_stype", sizeof("enc_stype"), (void **)&zstype) == SUCCESS) {
			if (zend_hash_find(Z_OBJPROP_P(data), "enc_ns", sizeof("enc_ns"), (void **)&zns) == SUCCESS) {
				set_ns_and_type_ex(xmlParam, Z_STRVAL_PP(zns), Z_STRVAL_PP(zstype));
			} else {
				set_ns_and_type_ex(xmlParam, NULL, Z_STRVAL_PP(zstype));
			}
		}

		if (zend_hash_find(Z_OBJPROP_P(data), "enc_name", sizeof("enc_name"), (void **)&zname) == SUCCESS) {
			xmlNodeSetName(xmlParam, Z_STRVAL_PP(zname));
		}
		if (zend_hash_find(Z_OBJPROP_P(data), "enc_namens", sizeof("enc_namens"), (void **)&znamens) == SUCCESS) {
			smart_str *ns;
			xmlNsPtr nsp;

			ns = encode_new_ns();
			nsp = xmlNewNs(xmlParam, Z_STRVAL_PP(znamens), ns->c);
			xmlSetNs(xmlParam, nsp);
			smart_str_free(ns);
			efree(ns);
		}
	} else if (sdlType) {
		prop = NULL;
		if (Z_TYPE_P(data) == IS_OBJECT) {
			prop = Z_OBJPROP_P(data);
		} else if (Z_TYPE_P(data) == IS_ARRAY) {
			prop = Z_ARRVAL_P(data);
		}
		if (sdlType->kind == XSD_TYPEKIND_RESTRICTION &&
		    sdlType->encode && type != &sdlType->encode->details) {
		  encodePtr enc;

			enc = sdlType->encode;
			while (enc && enc->details.sdl_type &&
		         enc->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
		         enc->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
		         enc->details.sdl_type->kind != XSD_TYPEKIND_UNION) {
		    enc = enc->details.sdl_type->encode;
		  }
		  if (enc) {
				zval **tmp;
				if (prop && zend_hash_find(prop, "_", sizeof("_"), (void**)&tmp) == SUCCESS) {
					xmlParam = master_to_xml(enc, *tmp, style);
				} else if (prop == NULL) {
					xmlParam = master_to_xml(enc, data, style);
				} else {
					xmlParam = xmlNewNode(NULL, "BOGUS");
				}
			} else {
				xmlParam = xmlNewNode(NULL, "BOGUS");
			}
		} else if (sdlType->kind == XSD_TYPEKIND_EXTENSION &&
		           sdlType->encode && type != &sdlType->encode->details) {
		  if (sdlType->encode->details.sdl_type &&
		      sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
		      sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
		      sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_UNION) {		  		
				xmlParam = master_to_xml(sdlType->encode, data, style);
			} else {
				zval **tmp;

				if (prop && zend_hash_find(prop, "_", sizeof("_"), (void**)&tmp) == SUCCESS) {
					xmlParam = master_to_xml(sdlType->encode, *tmp, style);
				} else if (prop == NULL) {
					xmlParam = master_to_xml(sdlType->encode, data, style);
				} else {
					xmlParam = xmlNewNode(NULL, "BOGUS");
				}
			}
		} else {
			xmlParam = xmlNewNode(NULL, "BOGUS");
		}
		FIND_ZVAL_NULL(data, xmlParam, style);

		if (prop != NULL) {
			if (sdlType->model) {
				model_to_xml_object(xmlParam, sdlType->model, prop, style, 1);
			}
			if (sdlType->attributes) {
				sdlAttributePtr *attr;
				zval **data;

				zend_hash_internal_pointer_reset(sdlType->attributes);
				while (zend_hash_get_current_data(sdlType->attributes, (void**)&attr) == SUCCESS) {
					if ((*attr)->name) {
						if (zend_hash_find(prop, (*attr)->name, strlen((*attr)->name)+1, (void**)&data) == SUCCESS) {
							xmlNodePtr dummy;

							dummy = master_to_xml((*attr)->encode, *data, SOAP_LITERAL);
							if (dummy->children && dummy->children->content) {
								xmlSetProp(xmlParam, (*attr)->name, dummy->children->content);
							}
							xmlFreeNode(dummy);
						}
					}
					zend_hash_move_forward(sdlType->attributes);
				}
			}
		}
		if (style == SOAP_ENCODED) {
			set_ns_and_type(xmlParam, type);
		}
	} else {
		xmlParam = xmlNewNode(NULL, "BOGUS");
		FIND_ZVAL_NULL(data, xmlParam, style);

		prop = NULL;
		if (Z_TYPE_P(data) == IS_OBJECT) {
			prop = Z_OBJPROP_P(data);
		} else if (Z_TYPE_P(data) == IS_ARRAY) {
			prop = Z_ARRVAL_P(data);
		}
		if (prop != NULL) {
			i = zend_hash_num_elements(prop);
			zend_hash_internal_pointer_reset(prop);

			for (;i > 0;i--) {
				xmlNodePtr property;
				zval **zprop;
				char *str_key;

				zend_hash_get_current_key(prop, &str_key, NULL, FALSE);
				zend_hash_get_current_data(prop, (void **)&zprop);

				property = master_to_xml(get_conversion((*zprop)->type), (*zprop), style);

				xmlNodeSetName(property, str_key);
				xmlAddChild(xmlParam, property);
				zend_hash_move_forward(prop);
			}
		}
		if (style == SOAP_ENCODED) {
			set_ns_and_type(xmlParam, type);
		}
	}
	return xmlParam;
}

/* Array encode/decode */
static xmlNodePtr guess_array_map(encodeTypePtr type, zval *data, int style)
{
	encodePtr enc = NULL;
	TSRMLS_FETCH();

	if (data && Z_TYPE_P(data) == IS_ARRAY) {
		if (is_map(data)) {
			enc = get_conversion(APACHE_MAP);
		} else {
			enc = get_conversion(SOAP_ENC_ARRAY);
		}
	}
	if (!enc) {
		enc = get_conversion(IS_NULL);
	}

	return master_to_xml(enc, data, style);
}

static int calc_dimension_12(const char* str)
{
	int i = 0, flag = 0;
	while (*str != '\0' && (*str < '0' || *str > '9') && (*str != '*')) {
		str++;
	}
	if (*str == '*') {
		i++;
		str++;
	}
	while (*str != '\0') {
		if (*str >= '0' && *str <= '9') {
			if (flag == 0) {
	   		i++;
	   		flag = 1;
	   	}
	  } else if (*str == '*') {
			php_error(E_ERROR,"* may only be first arraySize value in list");
		} else {
		  flag = 0;
		}
		str++;
	}
	return i;
}

static int* get_position_12(int dimension, const char* str)
{
	int *pos;
	int i = -1, flag = 0;

	pos = emalloc(sizeof(int)*dimension);
	memset(pos,0,sizeof(int)*dimension);
	while (*str != '\0' && (*str < '0' || *str > '9') && (*str != '*')) {
		str++;
	}
	if (*str == '*') {
		str++;
		i++;
	}
	while (*str != '\0') {
		if (*str >= '0' && *str <= '9') {
			if (flag == 0) {
	   		i++;
	   		flag = 1;
	   	}
	   	pos[i] = (pos[i]*10)+(*str-'0');
	  } else if (*str == '*') {
			php_error(E_ERROR,"* may only be first arraySize value in list");
		} else {
		  flag = 0;
		}
		str++;
	}
	return pos;
}

static int calc_dimension(const char* str)
{
	int i = 1;
	while (*str != ']' && *str != '\0') {
		if (*str == ',') {
    		i++;
		}
		str++;
	}
	return i;
}

static void get_position_ex(int dimension, const char* str, int** pos)
{
	int i = 0;

	memset(*pos,0,sizeof(int)*dimension);
	while (*str != ']' && *str != '\0' && i < dimension) {
		if (*str >= '0' && *str <= '9') {
			(*pos)[i] = ((*pos)[i]*10)+(*str-'0');
		} else if (*str == ',') {
			i++;
		}
		str++;
	}
}

static int* get_position(int dimension, const char* str)
{
	int *pos;

	pos = emalloc(sizeof(int)*dimension);
	get_position_ex(dimension, str, &pos);
	return pos;
}

static void add_xml_array_elements(xmlNodePtr xmlParam,
                                   sdlTypePtr type,
                                   encodePtr enc,
                                   int dimension ,
                                   int* dims,
                                   zval* data,
                                   int style)
{
	int j;

	if (data && Z_TYPE_P(data) == IS_ARRAY) {
	 	zend_hash_internal_pointer_reset(data->value.ht);
		for (j=0; j<dims[0]; j++) {
 			zval **zdata;
 			
 			if (zend_hash_get_current_data(data->value.ht, (void **)&zdata) != SUCCESS) {
 				zdata = NULL;
 			}
 			if (dimension == 1) {
	 			xmlNodePtr xparam;

	 			if (zdata) {
	 				if (enc == NULL) {
						TSRMLS_FETCH();
 						xparam = master_to_xml(get_conversion((*zdata)->type), (*zdata), style);
 					} else {
 						xparam = master_to_xml(enc, (*zdata), style);
		 			}
		 		} else {
					xparam = xmlNewNode(NULL, "BOGUS");
		 		}

	 			if (type) {
 					xmlNodeSetName(xparam, type->name);
 				} else if (style == SOAP_LITERAL && enc && enc->details.type_str) {
					xmlNodeSetName(xparam, enc->details.type_str);
				} else {
 					xmlNodeSetName(xparam, "val");
 				}
 				xmlAddChild(xmlParam, xparam);
 			} else {
 				if (zdata) {
	 			  add_xml_array_elements(xmlParam, type, enc, dimension-1, dims+1, *zdata, style);
	 			} else {
	 			  add_xml_array_elements(xmlParam, type, enc, dimension-1, dims+1, NULL, style);
	 			}
 			}
 			zend_hash_move_forward(data->value.ht);
 		}
 	} else {
		for (j=0; j<dims[0]; j++) {
 			if (dimension == 1) {
	 			xmlNodePtr xparam;

				xparam = xmlNewNode(NULL, "BOGUS");
	 			if (type) {
 					xmlNodeSetName(xparam, type->name);
 				} else if (style == SOAP_LITERAL && enc && enc->details.type_str) {
					xmlNodeSetName(xparam, enc->details.type_str);
				} else {
 					xmlNodeSetName(xparam, "val");
 				}
 				xmlAddChild(xmlParam, xparam);
 			} else {
 			  add_xml_array_elements(xmlParam, type, enc, dimension-1, dims+1, NULL, style);
 			}
		}	
 	}
}

static inline int array_num_elements(HashTable* ht)
{
	if (ht->pListTail && ht->pListTail->nKeyLength == 0) {
		return ht->pListTail->h-1;
	}
	return 0;
}

static xmlNodePtr to_xml_array(encodeTypePtr type, zval *data, int style)
{
	sdlTypePtr sdl_type = type->sdl_type;
	smart_str array_type = {0}, array_size = {0};
	int i;
	xmlNodePtr xmlParam;
	encodePtr enc = NULL;
	int dimension = 1;
	int* dims;
	int soap_version;

	TSRMLS_FETCH();

	soap_version = SOAP_GLOBAL(soap_version);

	xmlParam = xmlNewNode(NULL,"BOGUS");

	FIND_ZVAL_NULL(data, xmlParam, style);

	if (Z_TYPE_P(data) == IS_ARRAY) {
		sdlAttributePtr *arrayType;
		xmlAttrPtr *arrayTypeAttr;
		sdlTypePtr elementType;

		i = zend_hash_num_elements(Z_ARRVAL_P(data));

		if (sdl_type &&
		    sdl_type->attributes &&
		    zend_hash_find(sdl_type->attributes, SOAP_1_1_ENC_NAMESPACE":arrayType",
		      sizeof(SOAP_1_1_ENC_NAMESPACE":arrayType"),
		      (void **)&arrayType) == SUCCESS &&
		    zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType"), (void **)&arrayTypeAttr) == SUCCESS) {

			char *ns = NULL, *value, *end;
			xmlNsPtr myNs;
			zval** el;

			parse_namespace((*arrayTypeAttr)->children->content, &value, &ns);
			myNs = xmlSearchNs((*arrayTypeAttr)->doc, (*arrayTypeAttr)->parent, ns);

			end = strrchr(value,'[');
			if (end) {
				*end = '\0';
				end++;
				dimension = calc_dimension(end);
			}
			if (myNs != NULL) {
				enc = get_encoder(SOAP_GLOBAL(sdl), myNs->href, value);
				get_type_str(xmlParam, myNs->href, value, &array_type);
			} else {
				smart_str_appends(&array_type, value);
			}

			dims = emalloc(sizeof(int)*dimension);
			dims[0] = i;
			el = &data;
			for (i = 1; i < dimension; i++) {
				if (el != NULL && Z_TYPE_PP(el) == IS_ARRAY && Z_ARRVAL_PP(el)->pListHead) {
					el = (zval**)Z_ARRVAL_PP(el)->pListHead->pData;
					if (Z_TYPE_PP(el) == IS_ARRAY) {
						dims[i] = zend_hash_num_elements(Z_ARRVAL_PP(el));
					} else {
					  dims[i] = 0;
					}
				}
			}

			smart_str_append_long(&array_size, dims[0]);
			for (i=1; i<dimension; i++) {
				smart_str_appendc(&array_size, ',');
				smart_str_append_long(&array_size, dims[i]);
			}

			efree(value);
			if (ns) efree(ns);

		} else if (sdl_type &&
		           sdl_type->attributes &&
		           zend_hash_find(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":itemType",
		             sizeof(SOAP_1_2_ENC_NAMESPACE":itemType"),
		             (void **)&arrayType) == SUCCESS &&
		           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":itemType", sizeof(WSDL_NAMESPACE":itemType"), (void **)&arrayTypeAttr) == SUCCESS) {
			char *name, *ns;
			xmlNsPtr nsptr;
			xmlAttrPtr attr;

			attr = *arrayTypeAttr;
			parse_namespace(attr->children->content, &name, &ns);
			nsptr = xmlSearchNs(attr->doc, attr->parent, ns);

			if (nsptr != NULL) {
				enc = get_encoder(SOAP_GLOBAL(sdl), nsptr->href, name);
				get_type_str(xmlParam, nsptr->href, name, &array_type);
			} else {
				smart_str_appends(&array_type, name);
			}
			efree(name);
			if (ns) {efree(ns);}
			if (zend_hash_find(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
			                   sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
			                   (void **)&arrayType) == SUCCESS &&
			    zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize"), (void **)&arrayTypeAttr) == SUCCESS) {
				attr = *arrayTypeAttr;
				
				dimension = calc_dimension_12(attr->children->content);
				dims = get_position_12(dimension, attr->children->content);
				if (dims[0] == 0) {dims[0] = i;}
				
				smart_str_append_long(&array_size, dims[0]);
				for (i=1; i<dimension; i++) {
					smart_str_appendc(&array_size, ',');
					smart_str_append_long(&array_size, dims[i]);
				}
			} else {
				dims = emalloc(sizeof(int));
				*dims = 0;
				smart_str_append_long(&array_size, i);
			}
		} else if (sdl_type &&
		           sdl_type->attributes &&
		           zend_hash_find(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
		             sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
		             (void **)&arrayType) == SUCCESS &&
		           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraySize"), (void **)&arrayTypeAttr) == SUCCESS) {
			xmlAttrPtr attr;

			attr = *arrayTypeAttr;

			dimension = calc_dimension_12(attr->children->content);
			dims = get_position_12(dimension, attr->children->content);
			if (dims[0] == 0) {dims[0] = i;}

			smart_str_append_long(&array_size, dims[0]);
			for (i=1; i<dimension; i++) {
				smart_str_appendc(&array_size, ',');
				smart_str_append_long(&array_size, dims[i]);
			}
			
			if (sdl_type && sdl_type->elements &&
			    zend_hash_num_elements(sdl_type->elements) == 1 &&
			    (elementType = *(sdlTypePtr*)sdl_type->elements->pListHead->pData) != NULL &&
			     elementType->encode && elementType->encode->details.type_str) {
				enc = elementType->encode;
				get_type_str(xmlParam, elementType->encode->details.ns, elementType->encode->details.type_str, &array_type);
			} else {
				get_array_type(xmlParam, data, &array_type TSRMLS_CC);
				enc = get_encoder_ex(SOAP_GLOBAL(sdl), array_type.c);
			}
		} else if (sdl_type && sdl_type->elements &&
		           zend_hash_num_elements(sdl_type->elements) == 1 &&
		           (elementType = *(sdlTypePtr*)sdl_type->elements->pListHead->pData) != NULL &&
		           elementType->encode && elementType->encode->details.type_str) {

			enc = elementType->encode;

			get_type_str(xmlParam, elementType->encode->details.ns, elementType->encode->details.type_str, &array_type);

			smart_str_append_long(&array_size, i);

			dims = emalloc(sizeof(int)*dimension);
			dims[0] = i;
		} else {

			get_array_type(xmlParam, data, &array_type TSRMLS_CC);
			enc = get_encoder_ex(SOAP_GLOBAL(sdl), array_type.c);
			smart_str_append_long(&array_size, i);
			dims = emalloc(sizeof(int)*dimension);
			dims[0] = i;
		}

		if (style == SOAP_ENCODED) {
			if (soap_version == SOAP_1_1) {
				smart_str_0(&array_type);
				if (strcmp(array_type.c,"xsd:anyType") == 0) {
					smart_str_free(&array_type);
					smart_str_appendl(&array_type,"xsd:ur-type",sizeof("xsd:ur-type")-1);
				}
				smart_str_appendc(&array_type, '[');
				smart_str_append(&array_type, &array_size);
				smart_str_appendc(&array_type, ']');
				smart_str_0(&array_type);
				xmlSetProp(xmlParam, SOAP_1_1_ENC_NS_PREFIX":arrayType", array_type.c);
			} else {
				int i = 0;
				while (i < array_size.len) {
					if (array_size.c[i] == ',') {array_size.c[i] = ' ';}
					++i;
				}
				smart_str_0(&array_type);
				smart_str_0(&array_size);
				xmlSetProp(xmlParam, SOAP_1_2_ENC_NS_PREFIX":itemType", array_type.c);
				xmlSetProp(xmlParam, SOAP_1_2_ENC_NS_PREFIX":arraySize", array_size.c);
			}
		}
		smart_str_free(&array_type);
		smart_str_free(&array_size);

		add_xml_array_elements(xmlParam, sdl_type, enc, dimension, dims, data, style);
		efree(dims);
	}
	if (style == SOAP_ENCODED) {
		set_ns_and_type(xmlParam, type);
	}
	return xmlParam;
}

static zval *to_zval_array(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret;
	xmlNodePtr trav;
	encodePtr enc = NULL;
	int dimension = 1;
	int* dims = NULL;
	int* pos = NULL;
	xmlAttrPtr attr, *tmp;
	sdlPtr sdl;
	sdlAttributePtr *arrayType;
	sdlTypePtr elementType;

	TSRMLS_FETCH();

	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	sdl = SOAP_GLOBAL(sdl);

	if (data &&
	    (attr = get_attribute(data->properties,"arrayType")) &&
	    attr->children && attr->children->content) {
		char *type, *end, *ns;
		xmlNsPtr nsptr;

		parse_namespace(attr->children->content, &type, &ns);
		nsptr = xmlSearchNs(attr->doc, attr->parent, ns);

		end = strrchr(type,'[');
		if (end) {
			*end = '\0';
			dimension = calc_dimension(end+1);
			dims = get_position(dimension, end+1);
		}
		if (nsptr != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), nsptr->href, type);
		}
		efree(type);
		if (ns) {efree(ns);}

	} else if ((attr = get_attribute(data->properties,"itemType")) &&
	    attr->children &&
	    attr->children->content) {
		char *type, *ns;
		xmlNsPtr nsptr;

		parse_namespace(attr->children->content, &type, &ns);
		nsptr = xmlSearchNs(attr->doc, attr->parent, ns);
		if (nsptr != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), nsptr->href, type);
		}
		efree(type);
		if (ns) {efree(ns);}

		if ((attr = get_attribute(data->properties,"arraySize")) &&
		    attr->children && attr->children->content) {
			dimension = calc_dimension_12(attr->children->content);
			dims = get_position_12(dimension, attr->children->content);
		} else {
			dims = emalloc(sizeof(int));
			*dims = 0;
		}

	} else if ((attr = get_attribute(data->properties,"arraySize")) &&
	    attr->children && attr->children->content) {

		dimension = calc_dimension_12(attr->children->content);
		dims = get_position_12(dimension, attr->children->content);

	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           zend_hash_find(type->sdl_type->attributes, SOAP_1_1_ENC_NAMESPACE":arrayType",
	                          sizeof(SOAP_1_1_ENC_NAMESPACE":arrayType"),
	                          (void **)&arrayType) == SUCCESS &&
	           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType"), (void **)&tmp) == SUCCESS) {
		char *type, *end, *ns;
		xmlNsPtr nsptr;

		attr = *tmp;
		parse_namespace(attr->children->content, &type, &ns);
		nsptr = xmlSearchNs(attr->doc, attr->parent, ns);

		end = strrchr(type,'[');
		if (end) {
			*end = '\0';
		}
		if (nsptr != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), nsptr->href, type);
		}
		efree(type);
		if (ns) {efree(ns);}
		dims = emalloc(sizeof(int));
		*dims = 0;

	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           zend_hash_find(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":itemType",
	                          sizeof(SOAP_1_2_ENC_NAMESPACE":itemType"),
	                          (void **)&arrayType) == SUCCESS &&
	           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":itemType", sizeof(WSDL_NAMESPACE":itemType"), (void **)&tmp) == SUCCESS) {

		char *name, *ns;
		xmlNsPtr nsptr;

		attr = *tmp;
		parse_namespace(attr->children->content, &name, &ns);
		nsptr = xmlSearchNs(attr->doc, attr->parent, ns);

		if (nsptr != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), nsptr->href, name);
		}
		efree(name);
		if (ns) {efree(ns);}

		if (zend_hash_find(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
		                   sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
		                   (void **)&arrayType) == SUCCESS &&
		    zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize"), (void **)&tmp) == SUCCESS) {
			attr = *tmp;
			dimension = calc_dimension_12(attr->children->content);
			dims = get_position_12(dimension, attr->children->content);
		} else {
			dims = emalloc(sizeof(int));
			*dims = 0;
		}
	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           zend_hash_find(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
	                          sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
	                          (void **)&arrayType) == SUCCESS &&
	           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize"), (void **)&tmp) == SUCCESS) {

		attr = *tmp;
		dimension = calc_dimension_12(attr->children->content);
		dims = get_position_12(dimension, attr->children->content);
		if (type->sdl_type && type->sdl_type->elements &&
		    zend_hash_num_elements(type->sdl_type->elements) == 1 &&
		    (elementType = *(sdlTypePtr*)type->sdl_type->elements->pListHead->pData) != NULL &&
		    elementType->encode) {
			enc = elementType->encode;
		}
	} else if (type->sdl_type && type->sdl_type->elements &&
	           zend_hash_num_elements(type->sdl_type->elements) == 1 &&
	           (elementType = *(sdlTypePtr*)type->sdl_type->elements->pListHead->pData) != NULL &&
	           elementType->encode) {
		enc = elementType->encode;
	}
	if (dims == NULL) {
		dimension = 1;
		dims = emalloc(sizeof(int));
		*dims = 0;
	}
	pos = emalloc(sizeof(int)*dimension);
	memset(pos,0,sizeof(int)*dimension);
	if (data &&
	    (attr = get_attribute(data->properties,"offset")) &&
	     attr->children && attr->children->content) {
		char* tmp = strrchr(attr->children->content,'[');

		if (tmp == NULL) {
			tmp = attr->children->content;
		}
		get_position_ex(dimension, tmp, &pos);
	}
	if (enc == NULL) {
 		enc = get_conversion(UNKNOWN_TYPE);
 	}

	array_init(ret);
	trav = data->children;
	while (trav) {
		if (trav->type == XML_ELEMENT_NODE) {
			int i;
			zval *tmpVal, *ar;
			encodePtr typeEnc = NULL;
			xmlAttrPtr type = get_attribute(trav->properties,"type");
			xmlAttrPtr position = get_attribute(trav->properties,"position");
			if (type != NULL && type->children && type->children->content) {
				typeEnc = get_encoder_from_prefix(sdl, trav, type->children->content);
			}
			if (typeEnc) {
				tmpVal = master_to_zval(typeEnc, trav);
			} else {
				tmpVal = master_to_zval(enc, trav);
			}
			if (position != NULL && position->children && position->children->content) {
				char* tmp = strrchr(position->children->content,'[');
				if (tmp == NULL) {
					tmp = position->children->content;
				}
				get_position_ex(dimension, tmp, &pos);
			}

			/* Get/Create intermediate arrays for multidimensional arrays */
			i = 0;
			ar = ret;
			while (i < dimension-1) {
				zval** ar2;
				if (zend_hash_index_find(Z_ARRVAL_P(ar), pos[i], (void**)&ar2) == SUCCESS) {
					ar = *ar2;
				} else {
					zval *tmpAr;
					MAKE_STD_ZVAL(tmpAr);
					array_init(tmpAr);
					zend_hash_index_update(Z_ARRVAL_P(ar), pos[i], &tmpAr, sizeof(zval*), (void**)&ar2);
					ar = *ar2;
				}
				i++;
			}
			zend_hash_index_update(Z_ARRVAL_P(ar), pos[i], &tmpVal, sizeof(zval *), NULL);

			/* Increment position */
			i = dimension;
			while (i > 0) {
			  i--;
			  pos[i]++;
				if (pos[i] >= dims[i]) {
					if (i > 0) {
						pos[i] = 0;
					} else {
						/* TODO: Array index overflow */
					}
				} else {
				  break;
				}
			}
		}
		trav = trav->next;
	}
	efree(dims);
	efree(pos);
	return ret;
}

/* Map encode/decode */
static xmlNodePtr to_xml_map(encodeTypePtr type, zval *data, int style)
{
	xmlNodePtr xmlParam;
	int i;
	TSRMLS_FETCH();

	xmlParam = xmlNewNode(NULL, "BOGUS");
	FIND_ZVAL_NULL(data, xmlParam, style);

	if (Z_TYPE_P(data) == IS_ARRAY) {
		i = zend_hash_num_elements(Z_ARRVAL_P(data));
		/* TODO: Register namespace...??? */
//		xmlSetProp(xmlParam, APACHE_NS_PREFIX, APACHE_NAMESPACE);
		zend_hash_internal_pointer_reset(data->value.ht);
		for (;i > 0;i--) {
			xmlNodePtr xparam, item;
			xmlNodePtr key;
			zval **temp_data;
			char *key_val;
			int int_val;
			encodePtr enc;

			zend_hash_get_current_data(data->value.ht, (void **)&temp_data);
			if (Z_TYPE_PP(temp_data) != IS_NULL) {
				item = xmlNewNode(NULL, "item");
				key = xmlNewNode(NULL, "key");
				if (zend_hash_get_current_key(data->value.ht, &key_val, (long *)&int_val, FALSE) == HASH_KEY_IS_STRING) {
					if (style == SOAP_ENCODED) {
						xmlSetProp(key, "xsi:type", "xsd:string");
					}
					xmlNodeSetContent(key, key_val);
				} else {
					smart_str tmp = {0};
					smart_str_append_long(&tmp, int_val);
					smart_str_0(&tmp);

					if (style == SOAP_ENCODED) {
						xmlSetProp(key, "xsi:type", "xsd:int");
					}
					xmlNodeSetContentLen(key, tmp.c, tmp.len);

					smart_str_free(&tmp);
				}


				enc = get_conversion((*temp_data)->type);
				xparam = master_to_xml(enc, (*temp_data), style);

				xmlNodeSetName(xparam, "value");
				xmlAddChild(item, key);
				xmlAddChild(item, xparam);
				xmlAddChild(xmlParam, item);
			}
			zend_hash_move_forward(data->value.ht);
		}
	}
	if (style == SOAP_ENCODED) {
		set_ns_and_type(xmlParam, type);
	}

	return xmlParam;
}

static zval *to_zval_map(encodeTypePtr type, xmlNodePtr data)
{
	zval *ret, *key, *value;
	xmlNodePtr trav, item, xmlKey, xmlValue;
	encodePtr enc;
	TSRMLS_FETCH();

	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		array_init(ret);
		trav = data->children;

		enc = get_conversion(UNKNOWN_TYPE);
		trav = data->children;
		FOREACHNODE(trav, "item", item) {
			xmlKey = get_node(item->children, "key");
			if (!xmlKey) {
				php_error(E_ERROR, "Error encoding apache map, missing key");
			}

			xmlValue = get_node(item->children, "value");
			if (!xmlKey) {
				php_error(E_ERROR, "Error encoding apache map, missing value");
			}

			key = master_to_zval(enc, xmlKey);
			value = master_to_zval(enc, xmlValue);

			if (Z_TYPE_P(key) == IS_STRING) {
				zend_hash_update(Z_ARRVAL_P(ret), Z_STRVAL_P(key), Z_STRLEN_P(key) + 1, &value, sizeof(zval *), NULL);
			} else if (Z_TYPE_P(key) == IS_LONG) {
				zend_hash_index_update(Z_ARRVAL_P(ret), Z_LVAL_P(key), &value, sizeof(zval *), NULL);
			} else {
				php_error(E_ERROR, "Error encoding apache map, only Strings or Longs are allowd as keys");
			}
			zval_ptr_dtor(&key);
		}
		ENDFOREACH(trav);
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}

/* Unknown encode/decode */
static xmlNodePtr guess_xml_convert(encodeTypePtr type, zval *data, int style)
{
	encodePtr enc;
	TSRMLS_FETCH();

	if (data) {
		enc = get_conversion(data->type);
	} else {
		enc = get_conversion(IS_NULL);
	}
	return master_to_xml(enc, data, style);
}

static zval *guess_zval_convert(encodeTypePtr type, xmlNodePtr data)
{
	encodePtr enc = NULL;
	xmlAttrPtr tmpattr;
	TSRMLS_FETCH();

	data = check_and_resolve_href(data);

	if (data == NULL) {
		enc = get_conversion(IS_NULL);
	} else if (data->properties && get_attribute(data->properties, "nil")) {
		enc = get_conversion(IS_NULL);
	} else {
		tmpattr = get_attribute(data->properties,"type");
		if (tmpattr != NULL) {
		  if (tmpattr->children) {
				enc = get_conversion_from_type(data, tmpattr->children->content);
			} else {
				enc = get_conversion_from_type(data, "");
			}
		/*
			if (enc == NULL)
				php_error(E_ERROR, "Error (Don't know how to encode/decode \"%s\")", tmpattr->children->content);
		*/
		}

		if (enc == NULL) {
			/* Didn't have a type, totally guess here */
			/* Logic: has children = IS_OBJECT else IS_STRING */
			xmlNodePtr trav;

			if (get_attribute(data->properties, "arrayType") ||
			    get_attribute(data->properties, "itemType") ||
			    get_attribute(data->properties, "arraySize")) {
				enc = get_conversion(SOAP_ENC_ARRAY);
			} else {
				enc = get_conversion(XSD_STRING);
				trav = data->children;
				while (trav != NULL) {
					if (trav->type == XML_ELEMENT_NODE) {
						enc = get_conversion(SOAP_ENC_OBJECT);
						break;
					}
					trav = trav->next;
				}
			}
		}
	}
	return master_to_zval(enc, data);
}

/* Time encode/decode */
static xmlNodePtr to_xml_datetime_ex(encodeTypePtr type, zval *data, char *format, int style)
{
	/* logic hacked from ext/standard/datetime.c */
	struct tm *ta, tmbuf;
	time_t timestamp;
	int max_reallocs = 5;
	size_t buf_len=64, real_len;
	char *buf;
	char tzbuf[6];

	xmlNodePtr xmlParam;

	xmlParam = xmlNewNode(NULL, "BOGUS");
	FIND_ZVAL_NULL(data, xmlParam, style);

	if (Z_TYPE_P(data) == IS_LONG) {
		timestamp = Z_LVAL_P(data);
		ta = php_localtime_r(&timestamp, &tmbuf);
		/*ta = php_gmtime_r(&timestamp, &tmbuf);*/

		buf = (char *) emalloc(buf_len);
		while ((real_len = strftime(buf, buf_len, format, ta)) == buf_len || real_len == 0) {
			buf_len *= 2;
			buf = (char *) erealloc(buf, buf_len);
			if (!--max_reallocs) break;
		}

		/* Time zone support */
#if HAVE_TM_GMTOFF
		sprintf(tzbuf, "%c%02d%02d", (ta->tm_gmtoff < 0) ? '-' : '+', abs(ta->tm_gmtoff / 3600), abs( (ta->tm_gmtoff % 3600) / 60 ));
#else
		sprintf(tzbuf, "%c%02d%02d", ((ta->tm_isdst ? tzone - 3600:tzone)>0)?'-':'+', abs((ta->tm_isdst ? tzone - 3600 : tzone) / 3600), abs(((ta->tm_isdst ? tzone - 3600 : tzone) % 3600) / 60));
#endif
		if (strcmp(tzbuf,"+0000") == 0) {
		  strcpy(tzbuf,"Z");
		  real_len++;
		} else {
			real_len += 5;
		}
		if (real_len >= buf_len) {
			buf = (char *) erealloc(buf, real_len+1);
		}
		strcat(buf, tzbuf);

		xmlNodeSetContent(xmlParam, buf);
		efree(buf);
	} else if (Z_TYPE_P(data) == IS_STRING) {
		buf = malloc(Z_STRLEN_P(data)+1);
		strcpy(buf, Z_STRVAL_P(data));

		xmlNodeSetContentLen(xmlParam, buf, Z_STRLEN_P(data));
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(xmlParam, type);
	}
	return xmlParam;
}

static xmlNodePtr to_xml_duration(encodeTypePtr type, zval *data, int style)
{
	/* TODO: '-'?P([0-9]+Y)?([0-9]+M)?([0-9]+D)?T([0-9]+H)?([0-9]+M)?([0-9]+S)? */
	return to_xml_string(type, data, style);
}

static xmlNodePtr to_xml_datetime(encodeTypePtr type, zval *data, int style)
{
	return to_xml_datetime_ex(type, data, "%Y-%m-%dT%H:%M:%S", style);
}

static xmlNodePtr to_xml_time(encodeTypePtr type, zval *data, int style)
{
	/* TODO: microsecconds */
	return to_xml_datetime_ex(type, data, "%H:%M:%S", style);
}

static xmlNodePtr to_xml_date(encodeTypePtr type, zval *data, int style)
{
	return to_xml_datetime_ex(type, data, "%Y-%m-%d", style);
}

static xmlNodePtr to_xml_gyearmonth(encodeTypePtr type, zval *data, int style)
{
	return to_xml_datetime_ex(type, data, "%Y-%m", style);
}

static xmlNodePtr to_xml_gyear(encodeTypePtr type, zval *data, int style)
{
	return to_xml_datetime_ex(type, data, "%Y", style);
}

static xmlNodePtr to_xml_gmonthday(encodeTypePtr type, zval *data, int style)
{
	return to_xml_datetime_ex(type, data, "--%m-%d", style);
}

static xmlNodePtr to_xml_gday(encodeTypePtr type, zval *data, int style)
{
	return to_xml_datetime_ex(type, data, "---%d", style);
}

static xmlNodePtr to_xml_gmonth(encodeTypePtr type, zval *data, int style)
{
	return to_xml_datetime_ex(type, data, "--%m--", style);
}

static zval* to_zval_list(encodeTypePtr enc, xmlNodePtr data) {
	/*FIXME*/
	return to_zval_stringc(enc, data);
}

static xmlNodePtr to_xml_list(encodeTypePtr enc, zval *data, int style) {
	xmlNodePtr ret;

	ret = xmlNewNode(NULL, "BOGUS");
	FIND_ZVAL_NULL(data, ret, style);
	if (Z_TYPE_P(data) == IS_ARRAY) {
		zval **tmp;
		smart_str list = {0};
		HashTable *ht = Z_ARRVAL_P(data);

		zend_hash_internal_pointer_reset(ht);
		while (zend_hash_get_current_data(ht, (void**)&tmp) == SUCCESS) {
			if (list.len != 0) {
				smart_str_appendc(&list, ' ');
			}
			if (Z_TYPE_PP(tmp) == IS_STRING) {
				smart_str_appendl(&list, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
			} else {
				zval copy = **tmp;
				zval_copy_ctor(&copy);
				convert_to_string(&copy);
				smart_str_appendl(&list, Z_STRVAL(copy), Z_STRLEN(copy));
				zval_dtor(&copy);
			}
			zend_hash_move_forward(ht);
		}
		smart_str_0(&list);
		xmlNodeSetContentLen(ret, list.c, list.len);
		smart_str_free(&list);
	} else if (Z_TYPE_P(data) == IS_STRING) {
		xmlNodeSetContentLen(ret, Z_STRVAL_P(data), Z_STRLEN_P(data));
	} else {
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		xmlNodeSetContentLen(ret, Z_STRVAL(tmp), Z_STRLEN(tmp));
		zval_dtor(&tmp);
	}
	return ret;
}

static xmlNodePtr to_xml_list1(encodeTypePtr enc, zval *data, int style) {
	/*FIXME: minLength=1 */
	return to_xml_list(enc,data,style);
}

static zval* to_zval_union(encodeTypePtr enc, xmlNodePtr data) {
	/*FIXME*/
	return to_zval_list(enc, data);
}

static xmlNodePtr to_xml_union(encodeTypePtr enc, zval *data, int style) {
	/*FIXME*/
	return to_xml_list(enc,data,style);
}

zval *sdl_guess_convert_zval(encodeTypePtr enc, xmlNodePtr data)
{
	sdlTypePtr type;

	type = enc->sdl_type;
/*FIXME: restriction support
	if (type && type->restrictions &&
	    data &&  data->children && data->children->content) {
		if (type->restrictions->whiteSpace && type->restrictions->whiteSpace->value) {
			if (strcmp(type->restrictions->whiteSpace->value,"replace") == 0) {
				whiteSpace_replace(data->children->content);
			} else if (strcmp(type->restrictions->whiteSpace->value,"collapse") == 0) {
				whiteSpace_collapse(data->children->content);
			}
		}
		if (type->restrictions->enumeration) {
			if (!zend_hash_exists(type->restrictions->enumeration,data->children->content,strlen(data->children->content)+1)) {
				php_error(E_WARNING,"Restriction: invalid enumeration value \"%s\"",data->children->content);
			}
		}
		if (type->restrictions->minLength &&
		    strlen(data->children->content) < type->restrictions->minLength->value) {
		  php_error(E_WARNING,"Restriction: length less then 'minLength'");
		}
		if (type->restrictions->maxLength &&
		    strlen(data->children->content) > type->restrictions->maxLength->value) {
		  php_error(E_WARNING,"Restriction: length greater then 'maxLength'");
		}
		if (type->restrictions->length &&
		    strlen(data->children->content) != type->restrictions->length->value) {
		  php_error(E_WARNING,"Restriction: length is not equal to 'length'");
		}
	}
*/
	switch (type->kind) {
		case XSD_TYPEKIND_SIMPLE:
			if (type->encode && enc != &type->encode->details) {
				return master_to_zval(type->encode, data);
			}
			break;
		case XSD_TYPEKIND_LIST:
			return to_zval_list(enc, data);
		case XSD_TYPEKIND_UNION:
			return to_zval_union(enc, data);
		case XSD_TYPEKIND_COMPLEX:
		case XSD_TYPEKIND_RESTRICTION:
		case XSD_TYPEKIND_EXTENSION:
			if (type->encode &&
			    (type->encode->details.type == IS_ARRAY ||
			     type->encode->details.type == SOAP_ENC_ARRAY)) {
				return to_zval_array(enc, data);
			}
			return to_zval_object(enc, data);
		default:
			break;
	}
	return guess_zval_convert(enc, data);
}

xmlNodePtr sdl_guess_convert_xml(encodeTypePtr enc, zval *data, int style)
{
	sdlTypePtr type;
	xmlNodePtr ret = NULL;

	type = enc->sdl_type;

/*FIXME: restriction support
	if (type) {
		if (type->restrictions && Z_TYPE_P(data) == IS_STRING) {
			if (type->restrictions->enumeration) {
				if (!zend_hash_exists(type->restrictions->enumeration,Z_STRVAL_P(data),Z_STRLEN_P(data)+1)) {
					php_error(E_WARNING,"Restriction: invalid enumeration value \"%s\".",Z_STRVAL_P(data));
				}
			}
			if (type->restrictions->minLength &&
			    Z_STRLEN_P(data) < type->restrictions->minLength->value) {
		  	php_error(E_WARNING,"Restriction: length less then 'minLength'");
			}
			if (type->restrictions->maxLength &&
			    Z_STRLEN_P(data) > type->restrictions->maxLength->value) {
		  	php_error(E_WARNING,"Restriction: length greater then 'maxLength'");
			}
			if (type->restrictions->length &&
			    Z_STRLEN_P(data) != type->restrictions->length->value) {
		  	php_error(E_WARNING,"Restriction: length is not equal to 'length'");
			}
		}
	}
*/
	switch(type->kind) {
		case XSD_TYPEKIND_SIMPLE:
			if (type->encode && enc != &type->encode->details) {
				ret = master_to_xml(type->encode, data, style);
			}
			break;
		case XSD_TYPEKIND_LIST:
			ret = to_xml_list(enc, data, style);
			break;
		case XSD_TYPEKIND_UNION:
			ret = to_xml_union(enc, data, style);
			break;
		case XSD_TYPEKIND_COMPLEX:
		case XSD_TYPEKIND_RESTRICTION:
		case XSD_TYPEKIND_EXTENSION:
			if (type->encode &&
			    (type->encode->details.type == IS_ARRAY ||
			     type->encode->details.type == SOAP_ENC_ARRAY)) {
				ret = to_xml_array(enc, data, style);
			} else {
				ret = to_xml_object(enc, data, style);
			}
			break;
		default:
			break;
	}
	if (ret == NULL) {
		ret = guess_xml_convert(enc, data, style);
	}
	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, enc);
	}
	return ret;
}

static void set_ns_and_type(xmlNodePtr node, encodeTypePtr type)
{
	set_ns_and_type_ex(node, type->ns, type->type_str);
}

static void set_ns_and_type_ex(xmlNodePtr node, char *ns, char *type)
{
	smart_str nstype = {0};
	get_type_str(node, ns, type, &nstype);
	xmlSetProp(node, "xsi:type", nstype.c);
	smart_str_free(&nstype);
}

smart_str *encode_new_ns()
{
	int num;
	smart_str *ns = emalloc(sizeof(smart_str));

	TSRMLS_FETCH();

	memset(ns, 0, sizeof(smart_str));
	num = ++SOAP_GLOBAL(cur_uniq_ns);
	smart_str_appendl(ns, "ns", 2);
	smart_str_append_long(ns, num);
	smart_str_0(ns);
	return ns;
}

void encode_reset_ns()
{
	TSRMLS_FETCH();
	SOAP_GLOBAL(cur_uniq_ns) = 0;
}

encodePtr get_conversion_ex(HashTable *encoding, int encode)
{
	encodePtr *enc;
	TSRMLS_FETCH();

	if (zend_hash_index_find(encoding, encode, (void **)&enc) == FAILURE) {
		php_error(E_ERROR, "Cannot find encoding");
	}

	if (SOAP_GLOBAL(overrides)) {
		smart_str nscat = {0};

		smart_str_appendl(&nscat, (*enc)->details.ns, strlen((*enc)->details.ns));
		smart_str_appendc(&nscat, ':');
		smart_str_appendl(&nscat, (*enc)->details.type_str, strlen((*enc)->details.type_str));
		smart_str_0(&nscat);

		zend_hash_find(SOAP_GLOBAL(overrides), nscat.c, nscat.len + 1, (void **)&enc);
		smart_str_free(&nscat);
	}

	return *enc;
}

encodePtr get_conversion_from_href_type_ex(HashTable *encoding, const char *type, int len)
{
	encodePtr *enc = NULL;

	if (encoding == NULL) {
		return NULL;
	}

	if (zend_hash_find(encoding, (char*)type, len + 1, (void **)&enc) == FAILURE) {
		return NULL;
	}

	return (*enc);
}

encodePtr get_conversion_from_type_ex(HashTable *encoding, xmlNodePtr node, const char *type)
{
	encodePtr *enc = NULL;
	xmlNsPtr nsptr;
	char *ns, *cptype;
	smart_str nscat = {0};

	if (encoding == NULL) {
		return NULL;
	}

	parse_namespace(type, &cptype, &ns);
	nsptr = xmlSearchNs(node->doc, node, ns);
	if (nsptr != NULL) {
		smart_str_appends(&nscat, nsptr->href);
		smart_str_appendc(&nscat, ':');
		smart_str_appends(&nscat, cptype);
		smart_str_0(&nscat);

		if (zend_hash_find(encoding, nscat.c, nscat.len + 1, (void **)&enc) == FAILURE) {
			if (zend_hash_find(encoding, (char*)type, strlen(type) + 1, (void **)&enc) == FAILURE) {
				enc = NULL;
			}
		}
		smart_str_free(&nscat);
	} else {
		if (zend_hash_find(encoding, (char*)type, strlen(type) + 1, (void **)&enc) == FAILURE) {
			enc = NULL;
		}
	}

	if (cptype) {efree(cptype);}
	if (ns) {efree(ns);}
	if (enc == NULL) {
		return NULL;
	} else {
		return (*enc);
	}
}

static int is_map(zval *array)
{
	int i, count = zend_hash_num_elements(Z_ARRVAL_P(array));

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(array));
	for (i = 0;i < count;i++) {
		if (zend_hash_get_current_key_type(Z_ARRVAL_P(array)) == HASH_KEY_IS_STRING) {
			return TRUE;
		}
		zend_hash_move_forward(Z_ARRVAL_P(array));
	}
	return FALSE;
}

static void get_array_type(xmlNodePtr node, zval *array, smart_str *type TSRMLS_DC)
{
	HashTable *ht = HASH_OF(array);
	int i, count, cur_type, prev_type, different;
	zval **tmp;

	if (!array || Z_TYPE_P(array) != IS_ARRAY) {
		smart_str_appendl(type, "xsd:anyType", 11);
	}

	different = FALSE;
	cur_type = prev_type = 0;
	count = zend_hash_num_elements(ht);

	zend_hash_internal_pointer_reset(ht);
	for (i = 0;i < count;i++) {
		zend_hash_get_current_data(ht, (void **)&tmp);

		if (Z_TYPE_PP(tmp) == IS_OBJECT &&
		    Z_OBJCE_PP(tmp) == soap_var_class_entry) {
			zval **ztype;

			if (zend_hash_find(Z_OBJPROP_PP(tmp), "enc_type", sizeof("enc_type"), (void **)&ztype) == FAILURE) {
				php_error(E_ERROR, "error encoding SoapVar");
			}
			cur_type = Z_LVAL_P(*ztype);
		} else if (Z_TYPE_PP(tmp) == IS_ARRAY && is_map(*tmp)) {
			cur_type = APACHE_MAP;
		} else {
			cur_type = Z_TYPE_PP(tmp);
		}

		if (i > 0) {
			if (cur_type != prev_type) {
				different = TRUE;
				break;
			}
		}

		prev_type = cur_type;
		zend_hash_move_forward(ht);
	}

	if (different || count == 0) {
		smart_str_appendl(type, "xsd:anyType", 11);
	} else {
		encodePtr enc;

		enc = get_conversion(cur_type);
		get_type_str(node, enc->details.ns, enc->details.type_str, type);
	}
}

static void get_type_str(xmlNodePtr node, const char* ns, const char* type, smart_str* ret)
{
	char *prefix;
	TSRMLS_FETCH();
	if (ns) {
		if (SOAP_GLOBAL(soap_version) == SOAP_1_2 &&
		    strcmp(ns,SOAP_1_1_ENC_NAMESPACE) == 0) {
			ns = SOAP_1_2_ENC_NAMESPACE;
		} else if (SOAP_GLOBAL(soap_version) == SOAP_1_1 &&
		           strcmp(ns,SOAP_1_2_ENC_NAMESPACE) == 0) {
			ns = SOAP_1_1_ENC_NAMESPACE;
		}
		if (zend_hash_find(SOAP_GLOBAL(defEncNs), (char*)ns, strlen(ns) + 1, (void **)&prefix) == SUCCESS) {
			smart_str_appendl(ret, prefix, strlen(prefix));
			smart_str_appendc(ret, ':');
		} else  if (node != NULL) {
			xmlAttrPtr attr = node->properties;

			while (attr != NULL) {
				if (strncmp(attr->name,"xmlns:",sizeof("xmlns:")-1) == 0 &&
				    strcmp(attr->children->content,ns) == 0) {
					break;
				}
			  attr = attr->next;
			}
			if (attr == NULL) {
				smart_str* prefix = encode_new_ns();
				smart_str xmlns = {0};

				smart_str_appendl(&xmlns, "xmlns:", 6);
				smart_str_append(&xmlns, prefix);
				smart_str_0(&xmlns);

				xmlSetProp(node, xmlns.c, ns);

				smart_str_append(ret, prefix);
				smart_str_appendc(ret, ':');

				smart_str_free(&xmlns);
				smart_str_free(prefix);
				efree(prefix);
			} else {
				smart_str_appends(ret, attr->name + sizeof("xmlns:")-1);
				smart_str_appendc(ret, ':');
			}
		} else {
			php_error(E_ERROR,"Unknown namespace '%s'",ns);
		}
	}
	smart_str_appendl(ret, type, strlen(type));
	smart_str_0(ret);
}

smart_str *build_soap_action(zval *this_ptr, char *soapaction)
{
	zval **uri;
	smart_str *tmp;
	TSRMLS_FETCH();

	tmp = emalloc(sizeof(smart_str));
	memset(tmp, 0, sizeof(smart_str));

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "uri", sizeof("uri"), (void *)&uri) == FAILURE) {
		php_error(E_ERROR, "Error finding uri");
	}

	smart_str_appendl(tmp, Z_STRVAL_PP(uri), Z_STRLEN_PP(uri));
	smart_str_appends(tmp, "#");
	smart_str_appendl(tmp, soapaction, strlen(soapaction));
	smart_str_0(tmp);

	return tmp;
}

static void delete_mapping(void *data)
{
	soapMappingPtr map = (soapMappingPtr)data;

	if (map->ns) {
		efree(map->ns);
	}
	if (map->ctype) {
		efree(map->ctype);
	}

	if (map->type == SOAP_MAP_FUNCTION) {
		if (map->map_functions.to_xml_before) {
			zval_ptr_dtor(&map->map_functions.to_xml_before);
		}
		if (map->map_functions.to_xml) {
			zval_ptr_dtor(&map->map_functions.to_xml);
		}
		if (map->map_functions.to_xml_after) {
			zval_ptr_dtor(&map->map_functions.to_xml_after);
		}
		if (map->map_functions.to_zval_before) {
			zval_ptr_dtor(&map->map_functions.to_zval_before);
		}
		if (map->map_functions.to_zval) {
			zval_ptr_dtor(&map->map_functions.to_zval);
		}
		if (map->map_functions.to_zval_after) {
			zval_ptr_dtor(&map->map_functions.to_zval_after);
		}
	}
	efree(map);
}

void delete_encoder(void *encode)
{
	encodePtr t = *((encodePtr*)encode);
	if (t->details.ns) {
		free(t->details.ns);
	}
	if (t->details.type_str) {
		free(t->details.type_str);
	}
	if (t->details.map) {
		delete_mapping(t->details.map);
	}
	free(t);
}
