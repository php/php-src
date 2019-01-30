/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
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

#ifndef PHP_ENCODING_H
#define PHP_ENCODING_H

#define XSD_1999_NAMESPACE "http://www.w3.org/1999/XMLSchema"
#define XSD_1999_TIMEINSTANT 401
#define XSD_1999_TIMEINSTANT_STRING "timeInstant"

#define SOAP_1_1_ENV_NAMESPACE "http://schemas.xmlsoap.org/soap/envelope/"
#define SOAP_1_1_ENV_NS_PREFIX "SOAP-ENV"

#define SOAP_1_2_ENV_NAMESPACE "http://www.w3.org/2003/05/soap-envelope"
#define SOAP_1_2_ENV_NS_PREFIX "env"

#define SOAP_1_1_ENC_NAMESPACE "http://schemas.xmlsoap.org/soap/encoding/"
#define SOAP_1_1_ENC_NS_PREFIX "SOAP-ENC"

#define SOAP_1_2_ENC_NAMESPACE "http://www.w3.org/2003/05/soap-encoding"
#define SOAP_1_2_ENC_NS_PREFIX "enc"

#define SCHEMA_NAMESPACE "http://www.w3.org/2001/XMLSchema"
#define XSD_NAMESPACE "http://www.w3.org/2001/XMLSchema"
#define XSD_NS_PREFIX "xsd"
#define XSI_NAMESPACE "http://www.w3.org/2001/XMLSchema-instance"
#define XSI_NS_PREFIX "xsi"
#define XML_NAMESPACE "http://www.w3.org/XML/1998/namespace"
#define XML_NS_PREFIX "xml"

#define XSD_STRING 101
#define XSD_STRING_STRING "string"
#define XSD_BOOLEAN 102
#define XSD_BOOLEAN_STRING "boolean"
#define XSD_DECIMAL 103
#define XSD_DECIMAL_STRING "decimal"
#define XSD_FLOAT 104
#define XSD_FLOAT_STRING "float"
#define XSD_DOUBLE 105
#define XSD_DOUBLE_STRING "double"
#define XSD_DURATION 106
#define XSD_DURATION_STRING "duration"
#define XSD_DATETIME 107
#define XSD_DATETIME_STRING "dateTime"
#define XSD_TIME 108
#define XSD_TIME_STRING "time"
#define XSD_DATE 109
#define XSD_DATE_STRING "date"
#define XSD_GYEARMONTH 110
#define XSD_GYEARMONTH_STRING "gYearMonth"
#define XSD_GYEAR 111
#define XSD_GYEAR_STRING "gYear"
#define XSD_GMONTHDAY 112
#define XSD_GMONTHDAY_STRING "gMonthDay"
#define XSD_GDAY 113
#define XSD_GDAY_STRING "gDay"
#define XSD_GMONTH 114
#define XSD_GMONTH_STRING "gMonth"
#define XSD_HEXBINARY 115
#define XSD_HEXBINARY_STRING "hexBinary"
#define XSD_BASE64BINARY 116
#define XSD_BASE64BINARY_STRING "base64Binary"
#define XSD_ANYURI 117
#define XSD_ANYURI_STRING "anyURI"
#define XSD_QNAME 118
#define XSD_QNAME_STRING "QName"
#define XSD_NOTATION 119
#define XSD_NOTATION_STRING "NOTATION"
#define XSD_NORMALIZEDSTRING 120
#define XSD_NORMALIZEDSTRING_STRING "normalizedString"
#define XSD_TOKEN 121
#define XSD_TOKEN_STRING "token"
#define XSD_LANGUAGE 122
#define XSD_LANGUAGE_STRING "language"
#define XSD_NMTOKEN 123
#define XSD_NMTOKEN_STRING "NMTOKEN"
#define XSD_NAME 124
#define XSD_NAME_STRING "Name"
#define XSD_NCNAME 125
#define XSD_NCNAME_STRING "NCName"
#define XSD_ID 126
#define XSD_ID_STRING "ID"
#define XSD_IDREF 127
#define XSD_IDREF_STRING "IDREF"
#define XSD_IDREFS 128
#define XSD_IDREFS_STRING "IDREFS"
#define XSD_ENTITY 129
#define XSD_ENTITY_STRING "ENTITY"
#define XSD_ENTITIES 130
#define XSD_ENTITIES_STRING "ENTITIES"
#define XSD_INTEGER 131
#define XSD_INTEGER_STRING "integer"
#define XSD_NONPOSITIVEINTEGER 132
#define XSD_NONPOSITIVEINTEGER_STRING "nonPositiveInteger"
#define XSD_NEGATIVEINTEGER 133
#define XSD_NEGATIVEINTEGER_STRING "negativeInteger"
#define XSD_LONG 134
#define XSD_LONG_STRING "long"
#define XSD_INT 135
#define XSD_INT_STRING "int"
#define XSD_SHORT 136
#define XSD_SHORT_STRING "short"
#define XSD_BYTE 137
#define XSD_BYTE_STRING "byte"
#define XSD_NONNEGATIVEINTEGER 138
#define XSD_NONNEGATIVEINTEGER_STRING "nonNegativeInteger"
#define XSD_UNSIGNEDLONG 139
#define XSD_UNSIGNEDLONG_STRING "unsignedLong"
#define XSD_UNSIGNEDINT 140
#define XSD_UNSIGNEDINT_STRING "unsignedInt"
#define XSD_UNSIGNEDSHORT 141
#define XSD_UNSIGNEDSHORT_STRING "unsignedShort"
#define XSD_UNSIGNEDBYTE 142
#define XSD_UNSIGNEDBYTE_STRING "unsignedByte"
#define XSD_POSITIVEINTEGER 143
#define XSD_POSITIVEINTEGER_STRING "positiveInteger"
#define XSD_NMTOKENS 144
#define XSD_NMTOKENS_STRING "NMTOKENS"
#define XSD_ANYTYPE 145
#define XSD_ANYTYPE_STRING "anyType"
#define XSD_UR_TYPE 146
#define XSD_UR_TYPE_STRING "ur-type"

#define XSD_ANYXML 147

#define APACHE_NAMESPACE "http://xml.apache.org/xml-soap"
#define APACHE_MAP 200
#define APACHE_MAP_STRING "Map"

#define SOAP_ENC_ARRAY 300
#define SOAP_ENC_ARRAY_STRING "Array"
#define SOAP_ENC_OBJECT 301
#define SOAP_ENC_OBJECT_STRING "Struct"

#define WSDL_NAMESPACE "http://schemas.xmlsoap.org/wsdl/"
#define WSDL_NS_PREFIX "wsdl"

#define WSDL_SOAP11_NAMESPACE "http://schemas.xmlsoap.org/wsdl/soap/"
#define WSDL_SOAP12_NAMESPACE "http://schemas.xmlsoap.org/wsdl/soap12/"
#define RPC_SOAP12_NAMESPACE  "http://www.w3.org/2003/05/soap-rpc"
#define RPC_SOAP12_NS_PREFIX  "rpc"

#define WSDL_HTTP11_NAMESPACE "http://schemas.xmlsoap.org/wsdl/http/"
#define WSDL_HTTP12_NAMESPACE	"http://www.w3.org/2003/05/soap/bindings/HTTP/"
#define WSDL_HTTP_NS_PREFIX "http"

#define WSDL_HTTP_TRANSPORT "http://schemas.xmlsoap.org/soap/http"

#define WSDL_MIME_NAMESPACE "http://schemas.xmlsoap.org/wsdl/mime/"

#define WSDL_DIME_NAMESPACE "http://schemas.xmlsoap.org/ws/2002/04/dime/wsdl/"
#define WSDL_DIME_OPEN      "http://schemas.xmlsoap.org/ws/2002/04/dime/open-layout"
#define WSDL_DIME_CLOSED    "http://schemas.xmlsoap.org/ws/2002/04/dime/closed-layout"

#define UNKNOWN_TYPE 999998
#define END_KNOWN_TYPES 999999

struct _encodeType {
	int type;
	char *type_str;
	char *ns;
	sdlTypePtr sdl_type;
	soapMappingPtr map;
};

struct _encode {
	encodeType details;
	zval *(*to_zval)(zval *ret, encodeTypePtr type, xmlNodePtr data);
	xmlNodePtr (*to_xml)(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
};

/* Master functions all encode/decode should be called thur these functions */
xmlNodePtr master_to_xml(encodePtr encode, zval *data, int style, xmlNodePtr parent);
zval *master_to_zval(zval *ret, encodePtr encode, xmlNodePtr data);

/* user defined mapping */
xmlNodePtr to_xml_user(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
zval *to_zval_user(zval *ret, encodeTypePtr type, xmlNodePtr node);

void whiteSpace_replace(xmlChar* str);
void whiteSpace_collapse(xmlChar* str);

xmlNodePtr sdl_guess_convert_xml(encodeTypePtr enc, zval* data, int style, xmlNodePtr parent);
zval *sdl_guess_convert_zval(zval *ret, encodeTypePtr enc, xmlNodePtr data);

void encode_finish();
void encode_reset_ns();
xmlNsPtr encode_add_ns(xmlNodePtr node, const char* ns);

encodePtr get_conversion(int encode);

void delete_encoder(zval *zv);
void delete_encoder_persistent(zval *zv);

extern const encode defaultEncoding[];
extern int numDefaultEncodings;

#endif
