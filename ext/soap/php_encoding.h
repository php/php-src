#ifndef PHP_ENCODING_H
#define PHP_ENCODING_H

#define XSD_1999_NAMESPACE "http://www.w3.org/1999/XMLSchema"
#define XSD_1999_TIMEINSTANT 401
#define XSD_1999_TIMEINSTANT_STRING "timeInstant"

#define XSD_NAMESPACE "http://www.w3.org/2001/XMLSchema"
#define XSD_NS_PREFIX "xsd"
#define XSD_STRING 101
#define XSD_STRING_STRING "string"
#define XSD_BOOLEAN 103
#define XSD_BOOLEAN_STRING "boolean"
#define XSD_DECIMAL 104
#define XSD_DECIMAL_STRING "decimal"
#define XSD_FLOAT 105
#define XSD_FLOAT_STRING "float"
#define XSD_DOUBLE 106
#define XSD_DOUBLE_STRING "double"
#define XSD_DURATION 107
#define XSD_DURATION_STRING "duration"
#define XSD_DATETIME 108
#define XSD_DATETIME_STRING "dateTime"
#define XSD_TIME 109
#define XSD_TIME_STRING "time"
#define XSD_DATE 110
#define XSD_DATE_STRING "date"
#define XSD_GYEARMONTH 111
#define XSD_GYEARMONTH_STRING "gYearMonth"
#define XSD_GYEAR 112
#define XSD_GYEAR_STRING "gYear"
#define XSD_GMONTHDAY 113
#define XSD_GMONTHDAY_STRING "gMonthDay"
#define XSD_GDAY 114
#define XSD_GDAY_STRING "gDay"
#define XSD_GMONTH 115
#define XSD_GMONTH_STRING "gMonth"
#define XSD_HEXBINARY 116
#define XSD_HEXBINARY_STRING "hexBinary"
#define XSD_BASE64BINARY 117
#define XSD_BASE64BINARY_STRING "base64Binary"
#define XSD_ANYURI 118
#define XSD_ANYURI_STRING "anyURI"
#define XSD_QNAME 119
#define XSD_QNAME_STRING "QName"
#define XSD_NOTATION 120
#define XSD_NOTATION_STRING "NOTATION"
#define XSD_NORMALIZEDSTRING 121
#define XSD_NORMALIZEDSTRING_STRING "normalizedString"
#define XSD_TOKEN 122
#define XSD_TOKEN_STRING "token"
#define XSD_LANGUAGE 123
#define XSD_LANGUAGE_STRING "language"
#define XSD_NMTOKEN 124
#define XSD_NMTOKEN_STRING "NMTOKEN"
#define XSD_NAME 124
#define XSD_NAME_STRING "Name"
#define XSD_NCNAME 125
#define XSD_NCNAME_STRING "NCName"
#define XSD_ID 126
#define XSD_ID_STRING "ID"
#define XSD_IDREF 127
#define XSD_IDREF_STRING "IDREF"
#define XSD_IDREFS 127
#define XSD_IDREFS_STRING "IDREFS"
#define XSD_ENTITY 128
#define XSD_ENTITY_STRING "ENTITY"
#define XSD_ENTITYS 129
#define XSD_ENTITYS_STRING "ENTITYS"
#define XSD_INTEGER 130
#define XSD_INTEGER_STRING "integer"
#define XSD_NONPOSITIVEINTEGER 131
#define XSD_NONPOSITIVEINTEGER_STRING "nonPositiveInteger"
#define XSD_NEGATIVEINTEGER 132
#define XSD_NEGATIVEINTEGER_STRING "negativeInteger"
#define XSD_LONG 133
#define XSD_LONG_STRING "long"
#define XSD_INT 134
#define XSD_INT_STRING "int"
#define XSD_SHORT 135
#define XSD_SHORT_STRING "short"
#define XSD_BYTE 136
#define XSD_BYTE_STRING "byte"
#define XSD_NONNEGATIVEINTEGER 137
#define XSD_NONNEGATIVEINTEGER_STRING "nonNegativeInteger"
#define XSD_UNSIGNEDLONG 138
#define XSD_UNSIGNEDLONG_STRING "unsignedLong"
#define XSD_UNSIGNEDINT 139
#define XSD_UNSIGNEDINT_STRING "unsignedInt"
#define XSD_UNSIGNEDSHORT 140
#define XSD_UNSIGNEDSHORT_STRING "unsignedShort"
#define XSD_UNSIGNEDBYTE 141
#define XSD_UNSIGNEDBYTE_STRING "unsignedByte"
#define XSD_POSITIVEINTEGER 142
#define XSD_POSITIVEINTEGER_STRING "positiveInteger"

#define APACHE_NAMESPACE "http://xml.apache.org/xml-soap"
#define APACHE_NS_PREFIX "apache"
#define APACHE_MAP 200
#define APACHE_MAP_STRING "Map"

#define SOAP_ENC_NAMESPACE "http://schemas.xmlsoap.org/soap/encoding/"
#define SOAP_ENC_NS_PREFIX "SOAP-ENC"
#define SOAP_ENC_ARRAY 300
#define SOAP_ENC_ARRAY_STRING "Array"
#define SOAP_ENC_OBJECT 301
#define SOAP_ENC_OBJECT_STRING "Struct"

#define SCHEMA_NAMESPACE "http://www.w3.org/2001/XMLSchema"
#define SCHEMA_NS_PREFIX "s"

#define WSDL_NAMESPACE "http://schemas.xmlsoap.org/wsdl/"
#define WSDL_NS_PREFIX "wsdl"

#define WSDL_SOAP_NAMESPACE "http://schemas.xmlsoap.org/wsdl/soap/"
#define WSDL_SOAP_NS_PREFIX "wsdlSoap"

#define WSDL_HTTP_NAMESPACE "http://schemas.xmlsoap.org/wsdl/http/"
#define WSDL_HTTP_NS_PREFIX "http"

#define WSDL_HTTP_TRANSPORT "http://schemas.xmlsoap.org/soap/http"

#define UNKNOWN_TYPE 999998
#define END_KNOWN_TYPES 999999

struct _encodeType
{
	int type;
	char *type_str;
	char *ns;
	sdlTypePtr sdl_type;
	soapMappingPtr map;
};

struct _encode
{
	encodeType details;
	zval *(*to_zval)(encodeType type, xmlNodePtr data);
	xmlNodePtr (*to_xml)(encodeType type, zval *data, int style);

	xmlNodePtr (*to_zval_before)(encodeType type, xmlNodePtr data, int style);
	zval *(*to_zval_after)(encodeType type, zval *data);

	zval *(*to_xml_before)(encodeType type, zval *data);
	xmlNodePtr (*to_xml_after)(encodeType type, xmlNodePtr data, int style);
};

smart_str *build_soap_action(zval *this_ptr, char *soapaction);

// Master functions all encode/decode should be called thur these functions
xmlNodePtr master_to_xml(encodePtr encode, zval *data, int style);
zval *master_to_zval(encodePtr encode, xmlNodePtr data);

#ifdef HAVE_PHP_DOMXML
//user defined mapping
zval *to_xml_before_user(encodeType type, zval *data);
xmlNodePtr to_xml_user(encodeType type, zval *data, int style);
xmlNodePtr to_xml_after_user(encodeType type, xmlNodePtr node, int style);
xmlNodePtr to_zval_before_user(encodeType type, xmlNodePtr node, int style);
zval *to_zval_user(encodeType type, xmlNodePtr node);
zval *to_zval_after_user(encodeType type, zval *data);
#endif

//zval type decode
zval *to_zval_double(encodeType type, xmlNodePtr data);
zval *to_zval_long(encodeType type, xmlNodePtr data);
zval *to_zval_bool(encodeType type, xmlNodePtr data);
zval *to_zval_object(encodeType type, xmlNodePtr data);
zval *to_zval_string(encodeType type, xmlNodePtr data);
zval *to_zval_array(encodeType type, xmlNodePtr data);
zval *to_zval_map(encodeType type, xmlNodePtr data);
zval *to_zval_null(encodeType type, xmlNodePtr data);
zval *guess_zval_convert(encodeType type, xmlNodePtr data);

xmlNodePtr to_xml_long(encodeType type, zval *data, int style);
xmlNodePtr to_xml_bool(encodeType type, zval *data, int style);

//String encode
xmlNodePtr to_xml_string(encodeType type, zval *data, int style);
xmlNodePtr to_xml_stringl(encodeType type, zval *data, int style);

//Null encode
xmlNodePtr to_xml_null(encodeType type, zval *data, int style);

//Struct encode
xmlNodePtr to_xml_object(encodeType type, zval *data, int style);

//Array encode
xmlNodePtr guess_array_map(encodeType type, zval *data, int style);
xmlNodePtr to_xml_array(encodeType type, zval *data, int style);
xmlNodePtr to_xml_map(encodeType type, zval *data, int style);

//Try and guess for non-wsdl clients and servers
xmlNodePtr guess_xml_convert(encodeType type, zval *data, int style);

//Datetime encode/decode
xmlNodePtr to_xml_datetime_ex(encodeType type, zval *data, char *format, int style);
xmlNodePtr to_xml_datetime(encodeType type, zval *data, int style);
xmlNodePtr to_xml_time(encodeType type, zval *data, int style);
xmlNodePtr to_xml_date(encodeType type, zval *data, int style);
xmlNodePtr to_xml_gyearmonth(encodeType type, zval *data, int style);
xmlNodePtr to_xml_gyear(encodeType type, zval *data, int style);
xmlNodePtr to_xml_gmonthday(encodeType type, zval *data, int style);
xmlNodePtr to_xml_gday(encodeType type, zval *data, int style);
xmlNodePtr to_xml_gmonth(encodeType type, zval *data, int style);

#define get_conversion(e) get_conversion_ex(SOAP_GLOBAL(defEncIndex), e)
#define get_conversion_from_type(n, t) get_conversion_from_type_ex(SOAP_GLOBAL(defEnc), n, t)
#define get_conversion_from_href_type(t) get_conversion_from_href_type_ex(SOAP_GLOBAL(defEnc), t)

void encode_reset_ns();
smart_str *encode_new_ns();

void set_ns_and_type(xmlNodePtr node, encodeType type);
void set_ns_and_type_ex(xmlNodePtr node, char *ns, char *type);
encodePtr get_conversion_ex(HashTable *encoding, int encode);
encodePtr get_conversion_from_type_ex(HashTable *encoding, xmlNodePtr node, char *type);
encodePtr get_conversion_from_href_type_ex(HashTable *encoding, char *type);

int is_map(zval *array);
void get_array_type(zval *array, smart_str *out_type);

void delete_encoder(void *handle);

extern encode defaultEncoding[];

#define FIND_XML_NULL(xml,zval) \
	{ \
		xmlAttrPtr null; \
		if(!xml || !xml->children) \
		{ \
			ZVAL_NULL(zval); \
			return zval; \
		} \
		if(xml->properties) \
		{ \
			null = get_attribute(xml->properties, "null"); \
			if(null) \
			{ \
				ZVAL_NULL(zval); \
				return zval; \
			} \
		} \
	}

#define FIND_ZVAL_NULL(zval, xml) \
{ \
	if(!zval) \
	{ \
		xmlSetProp(xml, "xsi:null", "1"); \
		return xml; \
	} \
}


#endif
