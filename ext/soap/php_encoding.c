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

#include <time.h>

#include "php_soap.h"
#include "ext/libxml/php_libxml.h"
#include "ext/standard/base64.h"
#include <libxml/parserInternals.h>
#include "zend_strtod.h"
#include "zend_interfaces.h"

/* zval type decode */
static zval *to_zval_double(zval* ret, encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_long(zval* ret, encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_bool(zval* ret, encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_string(zval* ret, encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_stringr(zval* ret, encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_stringc(zval* ret, encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_map(zval* ret, encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_null(zval* ret, encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_base64(zval* ret, encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_hexbin(zval* ret, encodeTypePtr type, xmlNodePtr data);

static xmlNodePtr to_xml_long(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_double(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_bool(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);

/* String encode */
static xmlNodePtr to_xml_string(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_base64(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_hexbin(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);

/* Null encode */
static xmlNodePtr to_xml_null(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);

/* Array encode */
static xmlNodePtr guess_array_map(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_map(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);

static xmlNodePtr to_xml_list(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_list1(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent);

/* Datetime encode/decode */
static xmlNodePtr to_xml_datetime_ex(encodeTypePtr type, zval *data, char *format, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_datetime(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_time(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_date(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_gyearmonth(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_gyear(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_gmonthday(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_gday(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_gmonth(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_duration(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);

static zval *to_zval_object(zval *ret, encodeTypePtr type, xmlNodePtr data);
static zval *to_zval_array(zval *ret, encodeTypePtr type, xmlNodePtr data);

static xmlNodePtr to_xml_object(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);
static xmlNodePtr to_xml_array(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);

static zval *to_zval_any(zval *ret, encodeTypePtr type, xmlNodePtr data);
static xmlNodePtr to_xml_any(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);

/* Try and guess for non-wsdl clients and servers */
static zval *guess_zval_convert(zval *ret, encodeTypePtr type, xmlNodePtr data);
static xmlNodePtr guess_xml_convert(encodeTypePtr type, zval *data, int style, xmlNodePtr parent);

static int is_map(zval *array);
static encodePtr get_array_type(xmlNodePtr node, zval *array, smart_str *out_type);

static xmlNodePtr check_and_resolve_href(xmlNodePtr data);

static void set_ns_prop(xmlNodePtr node, char *ns, char *name, char *val);
static void set_xsi_nil(xmlNodePtr node);
static void set_xsi_type(xmlNodePtr node, char *type);

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

#define CHECK_XML_NULL(xml) \
	{ \
		xmlAttrPtr null; \
		if (!xml) { \
			ZVAL_NULL(ret); \
			return ret; \
		} \
		if (xml->properties) { \
			null = get_attribute(xml->properties, "nil"); \
			if (null) { \
				ZVAL_NULL(ret); \
				return ret; \
			} \
		} \
	}

#define FIND_ZVAL_NULL(zval, xml, style) \
{ \
	if (!zval || Z_TYPE_P(zval) == IS_NULL) { \
	  if (style == SOAP_ENCODED) {\
			set_xsi_nil(xml); \
		} \
		return xml; \
	} \
}

const encode defaultEncoding[] = {
	{{UNKNOWN_TYPE, NULL, NULL, NULL, NULL}, guess_zval_convert, guess_xml_convert},

	{{IS_NULL, "nil", XSI_NAMESPACE, NULL, NULL}, to_zval_null, to_xml_null},
	{{IS_STRING, XSD_STRING_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_string, to_xml_string},
	{{IS_LONG, XSD_INT_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{IS_DOUBLE, XSD_FLOAT_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_double, to_xml_double},
	{{IS_FALSE, XSD_BOOLEAN_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_bool, to_xml_bool},
	{{IS_TRUE, XSD_BOOLEAN_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_bool, to_xml_bool},
	{{IS_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_1_ENC_NAMESPACE, NULL, NULL}, to_zval_array, guess_array_map},
	{{IS_OBJECT, SOAP_ENC_OBJECT_STRING, SOAP_1_1_ENC_NAMESPACE, NULL, NULL}, to_zval_object, to_xml_object},
	{{IS_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_2_ENC_NAMESPACE, NULL, NULL}, to_zval_array, guess_array_map},
	{{IS_OBJECT, SOAP_ENC_OBJECT_STRING, SOAP_1_2_ENC_NAMESPACE, NULL, NULL}, to_zval_object, to_xml_object},

	{{XSD_STRING, XSD_STRING_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_string, to_xml_string},
	{{XSD_BOOLEAN, XSD_BOOLEAN_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_bool, to_xml_bool},
	{{XSD_DECIMAL, XSD_DECIMAL_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_FLOAT, XSD_FLOAT_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_double, to_xml_double},
	{{XSD_DOUBLE, XSD_DOUBLE_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_double, to_xml_double},

	{{XSD_DATETIME, XSD_DATETIME_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_datetime},
	{{XSD_TIME, XSD_TIME_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_time},
	{{XSD_DATE, XSD_DATE_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_date},
	{{XSD_GYEARMONTH, XSD_GYEARMONTH_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_gyearmonth},
	{{XSD_GYEAR, XSD_GYEAR_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_gyear},
	{{XSD_GMONTHDAY, XSD_GMONTHDAY_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_gmonthday},
	{{XSD_GDAY, XSD_GDAY_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_gday},
	{{XSD_GMONTH, XSD_GMONTH_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_gmonth},
	{{XSD_DURATION, XSD_DURATION_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_duration},

	{{XSD_HEXBINARY, XSD_HEXBINARY_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_hexbin, to_xml_hexbin},
	{{XSD_BASE64BINARY, XSD_BASE64BINARY_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_base64, to_xml_base64},

	{{XSD_LONG, XSD_LONG_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_INT, XSD_INT_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_SHORT, XSD_SHORT_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_BYTE, XSD_BYTE_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_NONPOSITIVEINTEGER, XSD_NONPOSITIVEINTEGER_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_POSITIVEINTEGER, XSD_POSITIVEINTEGER_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_NONNEGATIVEINTEGER, XSD_NONNEGATIVEINTEGER_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_NEGATIVEINTEGER, XSD_NEGATIVEINTEGER_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_UNSIGNEDBYTE, XSD_UNSIGNEDBYTE_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_UNSIGNEDSHORT, XSD_UNSIGNEDSHORT_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_UNSIGNEDINT, XSD_UNSIGNEDINT_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_UNSIGNEDLONG, XSD_UNSIGNEDLONG_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_INTEGER, XSD_INTEGER_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},

	{{XSD_ANYTYPE, XSD_ANYTYPE_STRING, XSD_NAMESPACE, NULL, NULL}, guess_zval_convert, guess_xml_convert},
	{{XSD_UR_TYPE, XSD_UR_TYPE_STRING, XSD_NAMESPACE, NULL, NULL}, guess_zval_convert, guess_xml_convert},
	{{XSD_ANYURI, XSD_ANYURI_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_QNAME, XSD_QNAME_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_NOTATION, XSD_NOTATION_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_NORMALIZEDSTRING, XSD_NORMALIZEDSTRING_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringr, to_xml_string},
	{{XSD_TOKEN, XSD_TOKEN_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_LANGUAGE, XSD_LANGUAGE_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_NMTOKEN, XSD_NMTOKEN_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_NMTOKENS, XSD_NMTOKENS_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_list1},
	{{XSD_NAME, XSD_NAME_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_NCNAME, XSD_NCNAME_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_ID, XSD_ID_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_IDREF, XSD_IDREF_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_IDREFS, XSD_IDREFS_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_list1},
	{{XSD_ENTITY, XSD_ENTITY_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_ENTITIES, XSD_ENTITIES_STRING, XSD_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_list1},

	{{APACHE_MAP, APACHE_MAP_STRING, APACHE_NAMESPACE, NULL, NULL}, to_zval_map, to_xml_map},

	{{SOAP_ENC_OBJECT, SOAP_ENC_OBJECT_STRING, SOAP_1_1_ENC_NAMESPACE, NULL, NULL}, to_zval_object, to_xml_object},
	{{SOAP_ENC_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_1_ENC_NAMESPACE, NULL, NULL}, to_zval_array, to_xml_array},
	{{SOAP_ENC_OBJECT, SOAP_ENC_OBJECT_STRING, SOAP_1_2_ENC_NAMESPACE, NULL, NULL}, to_zval_object, to_xml_object},
	{{SOAP_ENC_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_2_ENC_NAMESPACE, NULL, NULL}, to_zval_array, to_xml_array},

	/* support some of the 1999 data types */
	{{XSD_STRING, XSD_STRING_STRING, XSD_1999_NAMESPACE, NULL, NULL}, to_zval_string, to_xml_string},
	{{XSD_BOOLEAN, XSD_BOOLEAN_STRING, XSD_1999_NAMESPACE, NULL, NULL}, to_zval_bool, to_xml_bool},
	{{XSD_DECIMAL, XSD_DECIMAL_STRING, XSD_1999_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},
	{{XSD_FLOAT, XSD_FLOAT_STRING, XSD_1999_NAMESPACE, NULL, NULL}, to_zval_double, to_xml_double},
	{{XSD_DOUBLE, XSD_DOUBLE_STRING, XSD_1999_NAMESPACE, NULL, NULL}, to_zval_double, to_xml_double},

	{{XSD_LONG, XSD_LONG_STRING, XSD_1999_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_INT, XSD_INT_STRING, XSD_1999_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_SHORT, XSD_SHORT_STRING, XSD_1999_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_BYTE, XSD_BYTE_STRING, XSD_1999_NAMESPACE, NULL, NULL}, to_zval_long, to_xml_long},
	{{XSD_1999_TIMEINSTANT, XSD_1999_TIMEINSTANT_STRING, XSD_1999_NAMESPACE, NULL, NULL}, to_zval_stringc, to_xml_string},

	{{XSD_ANYXML, "<anyXML>", "<anyXML>", NULL, NULL}, to_zval_any, to_xml_any},

	{{END_KNOWN_TYPES, NULL, NULL, NULL, NULL}, guess_zval_convert, guess_xml_convert}
};

int numDefaultEncodings = sizeof(defaultEncoding)/sizeof(encode);


void whiteSpace_replace(xmlChar* str)
{
	while (*str != '\0') {
		if (*str == '\x9' || *str == '\xA' || *str == '\xD') {
			*str = ' ';
		}
		str++;
	}
}

void whiteSpace_collapse(xmlChar* str)
{
	xmlChar *pos;
	xmlChar old;

	pos = str;
	whiteSpace_replace(str);
	while (*str == ' ') {
		str++;
	}
	old = '\0';
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
}

static encodePtr find_encoder_by_type_name(sdlPtr sdl, const char *type)
{
	if (sdl && sdl->encoders) {
		encodePtr enc;

		ZEND_HASH_FOREACH_PTR(sdl->encoders, enc)  {
		    if (strcmp(enc->details.type_str, type) == 0) {
				return enc;
			}
		} ZEND_HASH_FOREACH_END();
	}
	return NULL;
}

static bool soap_check_zval_ref(zval *data, xmlNodePtr node) {
	xmlNodePtr node_ptr;

	if (SOAP_GLOBAL(ref_map)) {
		if (Z_TYPE_P(data) == IS_OBJECT) {
			data = (zval*)Z_OBJ_P(data);
		}
		if ((node_ptr = zend_hash_index_find_ptr(SOAP_GLOBAL(ref_map), (zend_ulong)data)) != NULL) {
			xmlAttrPtr attr = node_ptr->properties;
			char *id;
			smart_str prefix = {0};

			if (node_ptr == node) {
				return 0;
			}
			if (SOAP_GLOBAL(soap_version) == SOAP_1_1) {
				while (1) {
					attr = get_attribute(attr, "id");
					if (attr == NULL || attr->ns == NULL) {
						break;
					}
					attr = attr->next;
				}
				if (attr) {
					id = (char*)attr->children->content;
					smart_str_appendc(&prefix, '#');
					smart_str_appends(&prefix, id);
					smart_str_0(&prefix);
					id = ZSTR_VAL(prefix.s);
				} else {
					SOAP_GLOBAL(cur_uniq_ref)++;
					smart_str_appendl(&prefix, "#ref", 4);
					smart_str_append_long(&prefix, SOAP_GLOBAL(cur_uniq_ref));
					smart_str_0(&prefix);
					id = ZSTR_VAL(prefix.s);
					xmlSetProp(node_ptr, BAD_CAST("id"), BAD_CAST(id+1));
				}
				xmlSetProp(node, BAD_CAST("href"), BAD_CAST(id));
			} else {
				attr = get_attribute_ex(attr, "id", SOAP_1_2_ENC_NAMESPACE);
				if (attr) {
					id = (char*)attr->children->content;
					smart_str_appendc(&prefix, '#');
					smart_str_appends(&prefix, id);
					smart_str_0(&prefix);
					id = ZSTR_VAL(prefix.s);
				} else {
					SOAP_GLOBAL(cur_uniq_ref)++;
					smart_str_appendl(&prefix, "#ref", 4);
					smart_str_append_long(&prefix, SOAP_GLOBAL(cur_uniq_ref));
					smart_str_0(&prefix);
					id = ZSTR_VAL(prefix.s);
					set_ns_prop(node_ptr, SOAP_1_2_ENC_NAMESPACE, "id", id+1);
				}
				set_ns_prop(node, SOAP_1_2_ENC_NAMESPACE, "ref", id);
			}
			smart_str_free(&prefix);
			return 1;
		} else {
			zend_hash_index_update_ptr(SOAP_GLOBAL(ref_map), (zend_ulong)data, node);
		}
	}
	return 0;
}

static bool soap_check_xml_ref(zval *data, xmlNodePtr node)
{
	zval *data_ptr;

	if (SOAP_GLOBAL(ref_map)) {
		if ((data_ptr = zend_hash_index_find(SOAP_GLOBAL(ref_map), (zend_ulong)node)) != NULL) {
			if (!Z_REFCOUNTED_P(data) ||
			    !Z_REFCOUNTED_P(data_ptr) ||
			    Z_COUNTED_P(data) != Z_COUNTED_P(data_ptr)) {
				zval_ptr_dtor(data);
				ZVAL_COPY(data, data_ptr);
				return 1;
			}
		}
	}
	return 0;
}

static void soap_add_xml_ref(zval *data, xmlNodePtr node)
{
	if (SOAP_GLOBAL(ref_map)) {
		zend_hash_index_update(SOAP_GLOBAL(ref_map), (zend_ulong)node, data);
	}
}

static xmlNodePtr master_to_xml_int(encodePtr encode, zval *data, int style, xmlNodePtr parent, int check_class_map)
{
	xmlNodePtr node = NULL;
	int add_type = 0;

	if (data) {
		ZVAL_DEREF(data);
	}

	/* Special handling of class SoapVar */
	if (data && Z_TYPE_P(data) == IS_OBJECT && Z_OBJCE_P(data) == soap_var_class_entry) {
		encodePtr enc = NULL;

		zval *ztype = Z_VAR_ENC_TYPE_P(data);
		if (Z_TYPE_P(ztype) != IS_LONG) {
			soap_error0(E_ERROR, "Encoding: SoapVar has no 'enc_type' property");
		}

		zval *zstype = Z_VAR_ENC_STYPE_P(data);
		if (Z_TYPE_P(zstype) == IS_STRING) {
			zval *zns = Z_VAR_ENC_NS_P(data);
			if (Z_TYPE_P(zns) == IS_STRING) {
				enc = get_encoder(SOAP_GLOBAL(sdl), Z_STRVAL_P(zns), Z_STRVAL_P(zstype));
			} else {
				zns = NULL;
				enc = get_encoder_ex(SOAP_GLOBAL(sdl), Z_STRVAL_P(zstype), Z_STRLEN_P(zstype));
			}
			if (enc == NULL && SOAP_GLOBAL(typemap)) {
				smart_str nscat = {0};

				if (zns != NULL) {
					smart_str_append(&nscat, Z_STR_P(zns));
					smart_str_appendc(&nscat, ':');
				}
				smart_str_append(&nscat, Z_STR_P(zstype));
				smart_str_0(&nscat);
				enc = zend_hash_find_ptr(SOAP_GLOBAL(typemap), nscat.s);
				smart_str_free(&nscat);
			}
		}
		if (enc == NULL) {
			enc = get_conversion(Z_LVAL_P(ztype));
		}
		if (enc == NULL) {
			enc = encode;
		}

		node = master_to_xml(enc, Z_VAR_ENC_VALUE_P(data), style, parent);

		if (style == SOAP_ENCODED || (SOAP_GLOBAL(sdl) && encode != enc)) {
			zval *zstype = Z_VAR_ENC_STYPE_P(data);
			if (Z_TYPE_P(zstype) == IS_STRING) {
				zval *zns = Z_VAR_ENC_NS_P(data);
				if (Z_TYPE_P(zns) == IS_STRING) {
					set_ns_and_type_ex(node, Z_STRVAL_P(zns), Z_STRVAL_P(zstype));
				} else {
					set_ns_and_type_ex(node, NULL, Z_STRVAL_P(zstype));
				}
			}
		}

		zval *zname = Z_VAR_ENC_NAME_P(data);
		if (Z_TYPE_P(zname) == IS_STRING) {
			xmlNodeSetName(node, BAD_CAST(Z_STRVAL_P(zname)));
		}

		zval *znamens = Z_VAR_ENC_NAMENS_P(data);
		if (Z_TYPE_P(znamens) == IS_STRING) {
			xmlNsPtr nsp = encode_add_ns(node, Z_STRVAL_P(znamens));
			xmlSetNs(node, nsp);
		}
	} else {
		if (check_class_map && SOAP_GLOBAL(class_map) && data &&
		    Z_TYPE_P(data) == IS_OBJECT &&
		    !GC_IS_RECURSIVE(Z_OBJPROP_P(data))) {
			zend_class_entry *ce = Z_OBJCE_P(data);
			zval *tmp;
			zend_string *type_name;

			ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(SOAP_GLOBAL(class_map), type_name, tmp) {
				ZVAL_DEREF(tmp);
				if (Z_TYPE_P(tmp) == IS_STRING &&
				    ZSTR_LEN(ce->name) == Z_STRLEN_P(tmp) &&
				    zend_binary_strncasecmp(ZSTR_VAL(ce->name), ZSTR_LEN(ce->name), Z_STRVAL_P(tmp), ZSTR_LEN(ce->name), ZSTR_LEN(ce->name)) == 0 &&
				    type_name) {

					/* TODO: namespace isn't stored */
					encodePtr enc = NULL;
					if (SOAP_GLOBAL(sdl)) {
						enc = get_encoder(SOAP_GLOBAL(sdl), SOAP_GLOBAL(sdl)->target_ns, ZSTR_VAL(type_name));
						if (!enc) {
							enc = find_encoder_by_type_name(SOAP_GLOBAL(sdl), ZSTR_VAL(type_name));
						}
					}
					if (enc) {
						if (encode != enc && style == SOAP_LITERAL) {
							add_type = 1;
						}
						encode = enc;
					}
					break;
				}
			} ZEND_HASH_FOREACH_END();
		}

		if (encode == NULL) {
			encode = get_conversion(UNKNOWN_TYPE);
		}
		if (SOAP_GLOBAL(typemap) && encode->details.type_str) {
			smart_str nscat = {0};
			encodePtr new_enc;

			if (encode->details.ns) {
				smart_str_appends(&nscat, encode->details.ns);
				smart_str_appendc(&nscat, ':');
			}
			smart_str_appends(&nscat, encode->details.type_str);
			smart_str_0(&nscat);
			if ((new_enc = zend_hash_find_ptr(SOAP_GLOBAL(typemap), nscat.s)) != NULL) {
				encode = new_enc;
			}
			smart_str_free(&nscat);
		}
		if (encode->to_xml) {
			node = encode->to_xml(&encode->details, data, style, parent);
			if (add_type) {
				set_ns_and_type(node, &encode->details);
			}
		}
	}
	return node;
}

xmlNodePtr master_to_xml(encodePtr encode, zval *data, int style, xmlNodePtr parent)
{
	return master_to_xml_int(encode, data, style, parent, 1);
}

static zval *master_to_zval_int(zval *ret, encodePtr encode, xmlNodePtr data)
{
	if (SOAP_GLOBAL(typemap)) {
		if (encode->details.type_str) {
			smart_str nscat = {0};
			encodePtr new_enc;

			if (encode->details.ns) {
				smart_str_appends(&nscat, encode->details.ns);
				smart_str_appendc(&nscat, ':');
			}
			smart_str_appends(&nscat, encode->details.type_str);
			smart_str_0(&nscat);
			if ((new_enc = zend_hash_find_ptr(SOAP_GLOBAL(typemap), nscat.s)) != NULL) {
				encode = new_enc;
			}
			smart_str_free(&nscat);
		} else {
			xmlAttrPtr type_attr = get_attribute_ex(data->properties,"type", XSI_NAMESPACE);

			if (type_attr != NULL) {
				encodePtr new_enc;
				xmlNsPtr nsptr;
				char *ns, *cptype;
				smart_str nscat = {0};

				parse_namespace(type_attr->children->content, &cptype, &ns);
				nsptr = xmlSearchNs(data->doc, data, BAD_CAST(ns));
				if (nsptr != NULL) {
					smart_str_appends(&nscat, (char*)nsptr->href);
					smart_str_appendc(&nscat, ':');
				}
				smart_str_appends(&nscat, cptype);
				smart_str_0(&nscat);
				efree(cptype);
				if (ns) {efree(ns);}
				if ((new_enc = zend_hash_find_ptr(SOAP_GLOBAL(typemap), nscat.s)) != NULL) {
					encode = new_enc;
				}
				smart_str_free(&nscat);
			}
		}
	}
	if (encode->to_zval) {
		ret = encode->to_zval(ret, &encode->details, data);
	}
	return ret;
}

zval *master_to_zval(zval *ret, encodePtr encode, xmlNodePtr data)
{
	data = check_and_resolve_href(data);

	if (encode == NULL) {
		encode = get_conversion(UNKNOWN_TYPE);
	} else {
		/* Use xsi:type if it is defined */
		xmlAttrPtr type_attr = get_attribute_ex(data->properties,"type", XSI_NAMESPACE);

		if (type_attr != NULL) {
			encodePtr  enc = get_encoder_from_prefix(SOAP_GLOBAL(sdl), data, type_attr->children->content);

			if (enc != NULL && enc != encode) {
			  encodePtr tmp = enc;
			  while (tmp &&
			         tmp->details.sdl_type != NULL &&
			         tmp->details.sdl_type->kind != XSD_TYPEKIND_COMPLEX) {
			    if (enc == tmp->details.sdl_type->encode ||
			        tmp == tmp->details.sdl_type->encode) {
			    	enc = NULL;
			    	break;
			    }
			    tmp = tmp->details.sdl_type->encode;
			  }
			  if (enc != NULL) {
			    encode = enc;
			  }
			}
		}
	}
	return master_to_zval_int(ret, encode, data);
}

xmlNodePtr to_xml_user(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	xmlNodePtr ret = NULL;
	zval return_value;

	if (type && type->map && Z_TYPE(type->map->to_xml) != IS_UNDEF) {
		ZVAL_NULL(&return_value);

		if (call_user_function(NULL, NULL, &type->map->to_xml, &return_value, 1, data) == FAILURE) {
			soap_error0(E_ERROR, "Encoding: Error calling to_xml callback");
		}
		if (Z_TYPE(return_value) == IS_STRING) {
			xmlDocPtr doc = soap_xmlParseMemory(Z_STRVAL(return_value), Z_STRLEN(return_value));
			if (doc && doc->children) {
				ret = xmlDocCopyNode(doc->children, parent->doc, 1);
			}
			xmlFreeDoc(doc);
		}

		zval_ptr_dtor(&return_value);
	}
	if (!ret) {
		ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	}
	xmlAddChild(parent, ret);
	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

zval *to_zval_user(zval *ret, encodeTypePtr type, xmlNodePtr node)
{
	if (type && type->map && Z_TYPE(type->map->to_zval) != IS_UNDEF) {
		xmlBufferPtr buf;
		zval data;
		xmlNodePtr copy;

		copy = xmlCopyNode(node, 1);
		buf = xmlBufferCreate();
		xmlNodeDump(buf, NULL, copy, 0, 0);
		ZVAL_STRING(&data, (char*)xmlBufferContent(buf));
		xmlBufferFree(buf);
		xmlFreeNode(copy);

		if (call_user_function(NULL, NULL, &type->map->to_zval, ret, 1, &data) == FAILURE) {
			soap_error0(E_ERROR, "Encoding: Error calling from_xml callback");
		} else if (EG(exception)) {
			ZVAL_NULL(ret);
		}
		zval_ptr_dtor(&data);
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}

/* TODO: get rid of "bogus".. ither by passing in the already created xmlnode or passing in the node name */
/* String encode/decode */
static zval *to_zval_string(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	ZVAL_NULL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			if (SOAP_GLOBAL(encoding) != NULL) {
				xmlBufferPtr in  = xmlBufferCreateStatic(data->children->content, xmlStrlen(data->children->content));
				xmlBufferPtr out = xmlBufferCreate();
				int n = xmlCharEncOutFunc(SOAP_GLOBAL(encoding), out, in);

				if (n >= 0) {
					ZVAL_STRING(ret, (char*)xmlBufferContent(out));
				} else {
					ZVAL_STRING(ret, (char*)data->children->content);
				}
				xmlBufferFree(out);
				xmlBufferFree(in);
			} else {
				ZVAL_STRING(ret, (char*)data->children->content);
			}
		} else if (data->children->type == XML_CDATA_SECTION_NODE && data->children->next == NULL) {
			ZVAL_STRING(ret, (char*)data->children->content);
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static zval *to_zval_stringr(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	ZVAL_NULL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_replace(data->children->content);
			if (SOAP_GLOBAL(encoding) != NULL) {
				xmlBufferPtr in  = xmlBufferCreateStatic(data->children->content, xmlStrlen(data->children->content));
				xmlBufferPtr out = xmlBufferCreate();
				int n = xmlCharEncOutFunc(SOAP_GLOBAL(encoding), out, in);

				if (n >= 0) {
					ZVAL_STRING(ret, (char*)xmlBufferContent(out));
				} else {
					ZVAL_STRING(ret, (char*)data->children->content);
				}
				xmlBufferFree(out);
				xmlBufferFree(in);
			} else {
				ZVAL_STRING(ret, (char*)data->children->content);
			}
		} else if (data->children->type == XML_CDATA_SECTION_NODE && data->children->next == NULL) {
			ZVAL_STRING(ret, (char*)data->children->content);
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static zval *to_zval_stringc(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	ZVAL_NULL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			if (SOAP_GLOBAL(encoding) != NULL) {
				xmlBufferPtr in  = xmlBufferCreateStatic(data->children->content, xmlStrlen(data->children->content));
				xmlBufferPtr out = xmlBufferCreate();
				int n = xmlCharEncOutFunc(SOAP_GLOBAL(encoding), out, in);

				if (n >= 0) {
					ZVAL_STRING(ret, (char*)xmlBufferContent(out));
				} else {
					ZVAL_STRING(ret, (char*)data->children->content);
				}
				xmlBufferFree(out);
				xmlBufferFree(in);
			} else {
				ZVAL_STRING(ret, (char*)data->children->content);
			}
		} else if (data->children->type == XML_CDATA_SECTION_NODE && data->children->next == NULL) {
			ZVAL_STRING(ret, (char*)data->children->content);
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static zval *to_zval_base64(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	zend_string *str;

	ZVAL_NULL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			str = php_base64_decode(data->children->content, strlen((char*)data->children->content));
			if (!str) {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			ZVAL_STR(ret, str);
		} else if (data->children->type == XML_CDATA_SECTION_NODE && data->children->next == NULL) {
			str = php_base64_decode(data->children->content, strlen((char*)data->children->content));
			if (!str) {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			ZVAL_STR(ret, str);
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static zval *to_zval_hexbin(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	zend_string *str;
	size_t i, j;
	unsigned char c;

	ZVAL_NULL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
		} else if (data->children->type != XML_CDATA_SECTION_NODE || data->children->next != NULL) {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			return ret;
		}
		str = zend_string_alloc(strlen((char*)data->children->content) / 2, 0);
		for (i = j = 0; i < ZSTR_LEN(str); i++) {
			c = data->children->content[j++];
			if (c >= '0' && c <= '9') {
				ZSTR_VAL(str)[i] = (c - '0') << 4;
			} else if (c >= 'a' && c <= 'f') {
				ZSTR_VAL(str)[i] = (c - 'a' + 10) << 4;
			} else if (c >= 'A' && c <= 'F') {
				ZSTR_VAL(str)[i] = (c - 'A' + 10) << 4;
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			c = data->children->content[j++];
			if (c >= '0' && c <= '9') {
				ZSTR_VAL(str)[i] |= c - '0';
			} else if (c >= 'a' && c <= 'f') {
				ZSTR_VAL(str)[i] |= c - 'a' + 10;
			} else if (c >= 'A' && c <= 'F') {
				ZSTR_VAL(str)[i] |= c - 'A' + 10;
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
		}
		ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';
		ZVAL_NEW_STR(ret, str);
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static xmlNodePtr to_xml_string(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	xmlNodePtr ret, text;
	char *str;
	int new_len;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) == IS_STRING) {
		str = estrndup(Z_STRVAL_P(data), Z_STRLEN_P(data));
		new_len = Z_STRLEN_P(data);
	} else {
		zend_string *tmp = zval_get_string_func(data);
		str = estrndup(ZSTR_VAL(tmp), ZSTR_LEN(tmp));
		new_len = ZSTR_LEN(tmp);
		zend_string_release_ex(tmp, 0);
	}

	if (SOAP_GLOBAL(encoding) != NULL) {
		xmlBufferPtr in  = xmlBufferCreateStatic(str, new_len);
		xmlBufferPtr out = xmlBufferCreate();
		int n = xmlCharEncInFunc(SOAP_GLOBAL(encoding), out, in);

		if (n >= 0) {
			efree(str);
			str = estrdup((char*)xmlBufferContent(out));
			new_len = n;
		}
		xmlBufferFree(out);
		xmlBufferFree(in);
	}

	if (!php_libxml_xmlCheckUTF8(BAD_CAST(str))) {
		char *err = emalloc(new_len + 8);
		char c;
		int i;

		memcpy(err, str, new_len+1);
		for (i = 0; (c = err[i++]);) {
			if ((c & 0x80) == 0) {
			} else if ((c & 0xe0) == 0xc0) {
				if ((err[i] & 0xc0) != 0x80) {
					break;
				}
				i++;
			} else if ((c & 0xf0) == 0xe0) {
				if ((err[i] & 0xc0) != 0x80 || (err[i+1] & 0xc0) != 0x80) {
					break;
				}
				i += 2;
			} else if ((c & 0xf8) == 0xf0) {
				if ((err[i] & 0xc0) != 0x80 || (err[i+1] & 0xc0) != 0x80 || (err[i+2] & 0xc0) != 0x80) {
					break;
				}
				i += 3;
			} else {
				break;
			}
		}
		if (c) {
			err[i-1] = '\\';
			err[i++] = 'x';
			err[i++] = ((unsigned char)c >> 4) + ((((unsigned char)c >> 4) > 9) ? ('a' - 10) : '0');
			err[i++] = (c & 15) + (((c & 15) > 9) ? ('a' - 10) : '0');
			err[i++] = '.';
			err[i++] = '.';
			err[i++] = '.';
			err[i++] = 0;
		}

		soap_error1(E_ERROR,  "Encoding: string '%s' is not a valid utf-8 string", err);
	}

	text = xmlNewTextLen(BAD_CAST(str), new_len);
	xmlAddChild(ret, text);
	efree(str);

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static xmlNodePtr to_xml_base64(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	xmlNodePtr ret, text;
	zend_string *str;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) == IS_STRING) {
		str = php_base64_encode((unsigned char*)Z_STRVAL_P(data), Z_STRLEN_P(data));
	} else {
		zend_string *tmp = zval_get_string_func(data);
		str = php_base64_encode((unsigned char*) ZSTR_VAL(tmp), ZSTR_LEN(tmp));
		zend_string_release_ex(tmp, 0);
	}

	text = xmlNewTextLen(BAD_CAST(ZSTR_VAL(str)), ZSTR_LEN(str));
	xmlAddChild(ret, text);
	zend_string_release_ex(str, 0);

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static xmlNodePtr to_xml_hexbin(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	static const char hexconvtab[] = "0123456789ABCDEF";
	xmlNodePtr ret, text;
	unsigned char *str;
	zval tmp;
	size_t i, j;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) != IS_STRING) {
		ZVAL_STR(&tmp, zval_get_string_func(data));
		data = &tmp;
	}
	str = (unsigned char *) safe_emalloc(Z_STRLEN_P(data) * 2, sizeof(char), 1);

	for (i = j = 0; i < Z_STRLEN_P(data); i++) {
		str[j++] = hexconvtab[((unsigned char)Z_STRVAL_P(data)[i]) >> 4];
		str[j++] = hexconvtab[((unsigned char)Z_STRVAL_P(data)[i]) & 15];
	}
	str[j] = '\0';

	text = xmlNewTextLen(str, Z_STRLEN_P(data) * 2 * sizeof(char));
	xmlAddChild(ret, text);
	efree(str);
	if (data == &tmp) {
		zval_ptr_dtor_str(&tmp);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static zval *to_zval_double(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	ZVAL_NULL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			zend_long lval;
			double dval;

			whiteSpace_collapse(data->children->content);
			switch (is_numeric_string((char*)data->children->content, strlen((char*)data->children->content), &lval, &dval, 0)) {
				case IS_LONG:
					ZVAL_DOUBLE(ret, lval);
					break;
				case IS_DOUBLE:
					ZVAL_DOUBLE(ret, dval);
					break;
				default:
					if (strncasecmp((char*)data->children->content, "NaN", sizeof("NaN")-1) == 0) {
						ZVAL_DOUBLE(ret, php_get_nan());
					} else if (strncasecmp((char*)data->children->content, "INF", sizeof("INF")-1) == 0) {
						ZVAL_DOUBLE(ret, php_get_inf());
					} else if (strncasecmp((char*)data->children->content, "-INF", sizeof("-INF")-1) == 0) {
						ZVAL_DOUBLE(ret, -php_get_inf());
					} else {
						soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
					}
			}
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}

static zval *to_zval_long(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	ZVAL_NULL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			zend_long lval;
			double dval;

			whiteSpace_collapse(data->children->content);
			errno = 0;

			switch (is_numeric_string((char*)data->children->content, strlen((char*)data->children->content), &lval, &dval, 0)) {
				case IS_LONG:
					ZVAL_LONG(ret, lval);
					break;
				case IS_DOUBLE:
					ZVAL_DOUBLE(ret, dval);
					break;
				default:
					soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}

static xmlNodePtr to_xml_long(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	xmlNodePtr ret;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) == IS_DOUBLE) {
		char s[256];

		snprintf(s, sizeof(s), "%0.0F",floor(Z_DVAL_P(data)));
		xmlNodeSetContent(ret, BAD_CAST(s));
	} else {
		zend_string *str = zend_long_to_str(zval_get_long(data));
		xmlNodeSetContentLen(ret, BAD_CAST(ZSTR_VAL(str)), ZSTR_LEN(str));
		zend_string_release_ex(str, 0);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static xmlNodePtr to_xml_double(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	xmlNodePtr ret;
	zval tmp;
	char *str;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	ZVAL_DOUBLE(&tmp, zval_get_double(data));

	str = (char *) safe_emalloc(EG(precision) >= 0 ? EG(precision) : 17, 1, MAX_LENGTH_OF_DOUBLE + 1);
	zend_gcvt(Z_DVAL(tmp), EG(precision), '.', 'E', str);
	xmlNodeSetContentLen(ret, BAD_CAST(str), strlen(str));
	efree(str);

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static zval *to_zval_bool(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	ZVAL_NULL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			if (stricmp((char*)data->children->content, "true") == 0 ||
				stricmp((char*)data->children->content, "t") == 0 ||
				strcmp((char*)data->children->content, "1") == 0) {
				ZVAL_TRUE(ret);
			} else if (stricmp((char*)data->children->content, "false") == 0 ||
				stricmp((char*)data->children->content, "f") == 0 ||
				strcmp((char*)data->children->content, "0") == 0) {
				ZVAL_FALSE(ret);
			} else {
				ZVAL_STRING(ret, (char*)data->children->content);
				convert_to_boolean(ret);
			}
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}

static xmlNodePtr to_xml_bool(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	xmlNodePtr ret;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (zend_is_true(data)) {
		xmlNodeSetContent(ret, BAD_CAST("true"));
	} else {
		xmlNodeSetContent(ret, BAD_CAST("false"));
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

/* Null encode/decode */
static zval *to_zval_null(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	ZVAL_NULL(ret);
	return ret;
}

static xmlNodePtr to_xml_null(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	xmlNodePtr ret;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	if (style == SOAP_ENCODED) {
		set_xsi_nil(ret);
	}
	return ret;
}

static void set_zval_property(zval* object, char* name, zval* val)
{
	zend_update_property(Z_OBJCE_P(object), Z_OBJ_P(object), name, strlen(name), val);
	Z_TRY_DELREF_P(val);
}

static zval* get_zval_property(zval* object, char* name, zval *rv)
{
	if (Z_TYPE_P(object) == IS_OBJECT) {
		zval *data = zend_read_property(Z_OBJCE_P(object), Z_OBJ_P(object), name, strlen(name), 1, rv);
		if (data == &EG(uninitialized_zval)) {
			return NULL;
		}
		ZVAL_DEREF(data);
		return data;
	} else if (Z_TYPE_P(object) == IS_ARRAY) {
		return zend_hash_str_find_deref(Z_ARRVAL_P(object), name, strlen(name));
	}
	return NULL;
}

static void unset_zval_property(zval* object, char* name)
{
	if (Z_TYPE_P(object) == IS_OBJECT) {
		zend_unset_property(Z_OBJCE_P(object), Z_OBJ_P(object), name, strlen(name));
	} else if (Z_TYPE_P(object) == IS_ARRAY) {
		zend_hash_str_del(Z_ARRVAL_P(object), name, strlen(name));
	}
}

static void model_to_zval_any(zval *ret, xmlNodePtr node)
{
	zval rv, arr, val, keepVal;
	zval* any = NULL;
	char* name = NULL;

	while (node != NULL) {
		if (get_zval_property(ret, (char*)node->name, &rv) == NULL) {

			ZVAL_NULL(&val);
			master_to_zval(&val, get_conversion(XSD_ANYXML), node);

			if (any && Z_TYPE_P(any) != IS_ARRAY) {
				/* Convert into array */
				array_init(&arr);
				if (name) {
					add_assoc_zval(&arr, name, any);
				} else {
					add_next_index_zval(&arr, any);
				}
				any = &arr;
			}

			if (Z_TYPE(val) == IS_STRING && *Z_STRVAL(val) == '<') {
				name = NULL;
				while (node->next != NULL) {
					zval val2;

					ZVAL_NULL(&val2);
					master_to_zval(&val2, get_conversion(XSD_ANYXML), node->next);
					if (Z_TYPE(val2) != IS_STRING ||  *Z_STRVAL(val) != '<') {
						Z_TRY_DELREF(val2);
						break;
					}
					concat_function(&val, &val, &val2);
					zval_ptr_dtor(&val2);
					node = node->next;
				}
			} else {
				name = (char*)node->name;
			}

			if (any == NULL) {
				if (name) {
					/* Convert into array */
					array_init(&arr);
					add_assoc_zval(&arr, name, &val);
					any = &arr;
					name = NULL;
				} else {
					ZVAL_COPY_VALUE(&keepVal, &val);
					any = &keepVal;
				}
			} else {
				/* Add array element */
				if (name) {
					zval *el;
					if ((el = zend_hash_str_find(Z_ARRVAL_P(any), name, strlen(name))) != NULL) {
						if (Z_TYPE_P(el) != IS_ARRAY) {
							/* Convert into array */
							array_init(&arr);
							add_next_index_zval(&arr, el);
							el = &arr;
						}
						add_next_index_zval(el, &val);
					} else {
						add_assoc_zval(any, name, &val);
					}
				} else {
					add_next_index_zval(any, &val);
				}
				name = NULL;
			}
		}
		node = node->next;
	}
	if (any) {
		set_zval_property(ret, name ? name : "any", any);
	}
}

static void model_to_zval_object(zval *ret, sdlContentModelPtr model, xmlNodePtr data, sdlPtr sdl)
{
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT:
			if (model->u.element->name) {
				xmlNodePtr node = get_node(data->children, model->u.element->name);

				if (node) {
					zval val;
					xmlNodePtr r_node;

					ZVAL_NULL(&val);
					r_node = check_and_resolve_href(node);
					if (r_node && r_node->children && r_node->children->content) {
						if (model->u.element->fixed && strcmp(model->u.element->fixed, (char*)r_node->children->content) != 0) {
							soap_error3(E_ERROR, "Encoding: Element '%s' has fixed value '%s' (value '%s' is not allowed)", model->u.element->name, model->u.element->fixed, r_node->children->content);
						}
						master_to_zval(&val, model->u.element->encode, r_node);
					} else if (model->u.element->fixed) {
						xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlNodeSetContent(dummy, BAD_CAST(model->u.element->fixed));
						master_to_zval(&val, model->u.element->encode, dummy);
						xmlFreeNode(dummy);
					} else if (model->u.element->def && !model->u.element->nillable) {
						xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlNodeSetContent(dummy, BAD_CAST(model->u.element->def));
						master_to_zval(&val, model->u.element->encode, dummy);
						xmlFreeNode(dummy);
					} else {
						master_to_zval(&val, model->u.element->encode, r_node);
					}
					if ((node = get_node(node->next, model->u.element->name)) != NULL) {
						zval array;

						array_init(&array);
						add_next_index_zval(&array, &val);
						do {
							ZVAL_NULL(&val);
							if (node && node->children && node->children->content) {
								if (model->u.element->fixed && strcmp(model->u.element->fixed, (char*)node->children->content) != 0) {
									soap_error3(E_ERROR, "Encoding: Element '%s' has fixed value '%s' (value '%s' is not allowed)", model->u.element->name, model->u.element->fixed, node->children->content);
								}
								master_to_zval(&val, model->u.element->encode, node);
							} else if (model->u.element->fixed) {
								xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
								xmlNodeSetContent(dummy, BAD_CAST(model->u.element->fixed));
								master_to_zval(&val, model->u.element->encode, dummy);
								xmlFreeNode(dummy);
							} else if (model->u.element->def && !model->u.element->nillable) {
								xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
								xmlNodeSetContent(dummy, BAD_CAST(model->u.element->def));
								master_to_zval(&val, model->u.element->encode, dummy);
								xmlFreeNode(dummy);
							} else {
								master_to_zval(&val, model->u.element->encode, node);
							}
							add_next_index_zval(&array, &val);
						} while ((node = get_node(node->next, model->u.element->name)) != NULL);
						ZVAL_COPY_VALUE(&val, &array);
					} else if ((Z_TYPE(val) != IS_NULL || !model->u.element->nillable) &&
					           (SOAP_GLOBAL(features) & SOAP_SINGLE_ELEMENT_ARRAYS) &&
					           (model->max_occurs == -1 || model->max_occurs > 1)) {
						zval array;

						array_init(&array);
						add_next_index_zval(&array, &val);
						ZVAL_COPY_VALUE(&val, &array);
					}
					set_zval_property(ret, model->u.element->name, &val);
				}
			}
			break;
		case XSD_CONTENT_ALL:
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr tmp;
			sdlContentModelPtr any = NULL;

			ZEND_HASH_FOREACH_PTR(model->u.content, tmp) {
				if (tmp->kind == XSD_CONTENT_ANY) {
					any = tmp;
				} else {
					model_to_zval_object(ret, tmp, data, sdl);
				}
			} ZEND_HASH_FOREACH_END();
			if (any) {
				model_to_zval_any(ret, data->children);
			}
			break;
		}
		case XSD_CONTENT_GROUP:
			model_to_zval_object(ret, model->u.group->model, data, sdl);
			break;
		default:
		  break;
	}
}

/* Struct encode/decode */
static zval *to_zval_object_ex(zval *ret, encodeTypePtr type, xmlNodePtr data, zend_class_entry *pce)
{
	xmlNodePtr trav;
	sdlPtr sdl;
	sdlTypePtr sdlType = type->sdl_type;
	zend_class_entry *ce = ZEND_STANDARD_CLASS_DEF_PTR;
	zval *redo_any = NULL, rv, arr;

	if (pce) {
		ce = pce;
	} else if (SOAP_GLOBAL(class_map) && type->type_str) {
		zval              *classname;
		zend_class_entry  *tmp;

		if ((classname = zend_hash_str_find_deref(SOAP_GLOBAL(class_map), type->type_str, strlen(type->type_str))) != NULL &&
		    Z_TYPE_P(classname) == IS_STRING &&
		    (tmp = zend_fetch_class(Z_STR_P(classname), ZEND_FETCH_CLASS_AUTO)) != NULL) {
			ce = tmp;
		}
	}
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
				zval base;

				ZVAL_NULL(ret);
				if (soap_check_xml_ref(ret, data)) {
					return ret;
				}

				object_init_ex(ret, ce);
				master_to_zval_int(&base, enc, data);
				set_zval_property(ret, "_", &base);
			} else {
				ZVAL_NULL(ret);
				FIND_XML_NULL(data, ret);
				if (soap_check_xml_ref(ret, data)) {
					return ret;
				}
				object_init_ex(ret, ce);
				soap_add_xml_ref(ret, data);
			}
		} else if (sdlType->kind == XSD_TYPEKIND_EXTENSION &&
		           sdlType->encode &&
		           type != &sdlType->encode->details) {
			if (sdlType->encode->details.sdl_type &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_UNION) {

				CHECK_XML_NULL(data);
				if (soap_check_xml_ref(ret, data)) {
					return ret;
				}

			    if (ce != ZEND_STANDARD_CLASS_DEF_PTR &&
			        sdlType->encode->to_zval == sdl_guess_convert_zval &&
			        sdlType->encode->details.sdl_type != NULL &&
			        (sdlType->encode->details.sdl_type->kind == XSD_TYPEKIND_COMPLEX ||
			         sdlType->encode->details.sdl_type->kind == XSD_TYPEKIND_RESTRICTION ||
			         sdlType->encode->details.sdl_type->kind == XSD_TYPEKIND_EXTENSION) &&
			        (sdlType->encode->details.sdl_type->encode == NULL ||
			         (sdlType->encode->details.sdl_type->encode->details.type != IS_ARRAY &&
			          sdlType->encode->details.sdl_type->encode->details.type != SOAP_ENC_ARRAY))) {
					to_zval_object_ex(ret, &sdlType->encode->details, data, ce);
			    } else {
					master_to_zval_int(ret, sdlType->encode, data);
				}

				soap_add_xml_ref(ret, data);

				redo_any = get_zval_property(ret, "any", &rv);
				if (Z_TYPE_P(ret) == IS_OBJECT && ce != ZEND_STANDARD_CLASS_DEF_PTR) {
					zend_object *zobj = Z_OBJ_P(ret);
					zobj->ce = ce;
				}
			} else {
				zval base;

				ZVAL_NULL(ret);
				if (soap_check_xml_ref(ret, data)) {
					return ret;
				}

				object_init_ex(ret, ce);
				soap_add_xml_ref(ret, data);
				master_to_zval_int(&base, sdlType->encode, data);
				set_zval_property(ret, "_", &base);
			}
		} else {
			ZVAL_NULL(ret);
			FIND_XML_NULL(data, ret);
			if (soap_check_xml_ref(ret, data)) {
				return ret;
			}
			object_init_ex(ret, ce);
			soap_add_xml_ref(ret, data);
		}
		if (sdlType->model) {
			model_to_zval_object(ret, sdlType->model, data, sdl);
			if (redo_any) {
				if (!get_zval_property(ret, "any", &rv)) {
					model_to_zval_any(ret, data->children);
					soap_add_xml_ref(ret, data);
				} else {
					unset_zval_property(ret, "any");
				}
			}
		}
		if (sdlType->attributes) {
			sdlAttributePtr attr;

			ZEND_HASH_FOREACH_PTR(sdlType->attributes, attr) {
				if (attr->name) {
					xmlAttrPtr val = get_attribute(data->properties, attr->name);
					char *str_val = NULL;

					if (val && val->children && val->children->content) {
						str_val = (char*)val->children->content;
						if (attr->fixed && strcmp(attr->fixed, str_val) != 0) {
							soap_error3(E_ERROR, "Encoding: Attribute '%s' has fixed value '%s' (value '%s' is not allowed)", attr->name, attr->fixed, str_val);
						}
					} else if (attr->fixed) {
						str_val = attr->fixed;
					} else if (attr->def) {
						str_val = attr->def;
					}
					if (str_val) {
						xmlNodePtr dummy, text;
						zval data;

						dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						text = xmlNewText(BAD_CAST(str_val));
						xmlAddChild(dummy, text);
						ZVAL_NULL(&data);
						/* TODO: There are other places using dummy nodes -- generalize? */
						zend_try {
							master_to_zval(&data, attr->encode, dummy);
						} zend_catch {
							xmlFreeNode(dummy);
							zend_bailout();
						} zend_end_try();
						xmlFreeNode(dummy);
						set_zval_property(ret, attr->name, &data);
					}
				}
			} ZEND_HASH_FOREACH_END();
		}
	} else {
		ZVAL_NULL(ret);
		FIND_XML_NULL(data, ret);
		if (soap_check_xml_ref(ret, data)) {
			return ret;
		}

		object_init_ex(ret, ce);
		soap_add_xml_ref(ret, data);
		trav = data->children;

		while (trav != NULL) {
			if (trav->type == XML_ELEMENT_NODE) {
				zval  tmpVal, rv;
				zval *prop;

				ZVAL_NULL(&tmpVal);
				master_to_zval(&tmpVal, NULL, trav);

				prop = get_zval_property(ret, (char*)trav->name, &rv);
				if (!prop) {
					if (!trav->next || !get_node(trav->next, (char*)trav->name)) {
						set_zval_property(ret, (char*)trav->name, &tmpVal);
					} else {
						zval arr;

						array_init(&arr);
						add_next_index_zval(&arr, &tmpVal);
						set_zval_property(ret, (char*)trav->name, &arr);
					}
				} else {
					/* Property already exist - make array */
					if (Z_TYPE_P(prop) != IS_ARRAY) {
						/* Convert into array */
						array_init(&arr);
						Z_TRY_ADDREF_P(prop);
						add_next_index_zval(&arr, prop);
						set_zval_property(ret, (char*)trav->name, &arr);
						prop = &arr;
					} else {
						SEPARATE_ARRAY(prop);
					}
					/* Add array element */
					add_next_index_zval(prop, &tmpVal);
				}
			}
			trav = trav->next;
		}
	}
	return ret;
}

static zval *to_zval_object(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	return to_zval_object_ex(ret, type, data, NULL);
}


static int model_to_xml_object(xmlNodePtr node, sdlContentModelPtr model, zval *object, int style, int strict)
{
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT: {
			zval *data;
			xmlNodePtr property;
			encodePtr enc;
			zval rv;

			data = get_zval_property(object, model->u.element->name, &rv);
			if (data &&
			    Z_TYPE_P(data) == IS_NULL &&
			    !model->u.element->nillable &&
			    model->min_occurs > 0 &&
			    !strict) {
				return 0;
			}
			if (data) {
				enc = model->u.element->encode;
				if ((model->max_occurs == -1 || model->max_occurs > 1) &&
				    Z_TYPE_P(data) == IS_ARRAY &&
				    !is_map(data)) {
					HashTable *ht = Z_ARRVAL_P(data);
					zval *val;

					ZEND_HASH_FOREACH_VAL(ht, val) {
						ZVAL_DEREF(val);
						if (Z_TYPE_P(val) == IS_NULL && model->u.element->nillable) {
							property = xmlNewNode(NULL, BAD_CAST("BOGUS"));
							xmlAddChild(node, property);
							set_xsi_nil(property);
						} else {
							property = master_to_xml(enc, val, style, node);
							if (property->children && property->children->content &&
							    model->u.element->fixed && strcmp(model->u.element->fixed, (char*)property->children->content) != 0) {
								soap_error3(E_ERROR, "Encoding: Element '%s' has fixed value '%s' (value '%s' is not allowed)", model->u.element->name, model->u.element->fixed, property->children->content);
							}
						}
						xmlNodeSetName(property, BAD_CAST(model->u.element->name));
						if (style == SOAP_LITERAL &&
						    model->u.element->namens &&
						    model->u.element->form == XSD_FORM_QUALIFIED) {
							xmlNsPtr nsp = encode_add_ns(property, model->u.element->namens);
							xmlSetNs(property, nsp);
						}
					} ZEND_HASH_FOREACH_END();
				} else {
					if (Z_TYPE_P(data) == IS_NULL && model->u.element->nillable) {
						property = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlAddChild(node, property);
						set_xsi_nil(property);
					} else if (Z_TYPE_P(data) == IS_NULL && model->min_occurs == 0) {
						return 1;
					} else {
						property = master_to_xml(enc, data, style, node);
						if (property->children && property->children->content &&
						    model->u.element->fixed && strcmp(model->u.element->fixed, (char*)property->children->content) != 0) {
							soap_error3(E_ERROR, "Encoding: Element '%s' has fixed value '%s' (value '%s' is not allowed)", model->u.element->name, model->u.element->fixed, property->children->content);
						}
					}
					xmlNodeSetName(property, BAD_CAST(model->u.element->name));
					if (style == SOAP_LITERAL &&
					    model->u.element->namens &&
					    model->u.element->form == XSD_FORM_QUALIFIED) {
						xmlNsPtr nsp = encode_add_ns(property, model->u.element->namens);
						xmlSetNs(property, nsp);
					}
				}
				return 1;
			} else if (strict && model->u.element->nillable && model->min_occurs > 0) {
				property = xmlNewNode(NULL, BAD_CAST(model->u.element->name));
				xmlAddChild(node, property);
				set_xsi_nil(property);
				if (style == SOAP_LITERAL &&
				    model->u.element->namens &&
				    model->u.element->form == XSD_FORM_QUALIFIED) {
					xmlNsPtr nsp = encode_add_ns(property, model->u.element->namens);
					xmlSetNs(property, nsp);
				}
				return 1;
			} else if (model->min_occurs == 0) {
				return 2;
			} else {
				if (strict) {
					soap_error1(E_ERROR,  "Encoding: object has no '%s' property", model->u.element->name);
				}
				return 0;
			}
			break;
		}
		case XSD_CONTENT_ANY: {
			zval *data;
			encodePtr enc;
			zval rv;

			data = get_zval_property(object, "any", &rv);
			if (data) {
				enc = get_conversion(XSD_ANYXML);
				if ((model->max_occurs == -1 || model->max_occurs > 1) &&
				    Z_TYPE_P(data) == IS_ARRAY &&
				    !is_map(data)) {
					HashTable *ht = Z_ARRVAL_P(data);
					zval *val;

					ZEND_HASH_FOREACH_VAL(ht, val) {
						master_to_xml(enc, val, style, node);
					} ZEND_HASH_FOREACH_END();
				} else {
					master_to_xml(enc, data, style, node);
				}
				return 1;
			} else if (model->min_occurs == 0) {
				return 2;
			} else {
				if (strict) {
					soap_error0(E_ERROR,  "Encoding: object has no 'any' property");
				}
				return 0;
			}
			break;
		}
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_ALL: {
			sdlContentModelPtr tmp;

			ZEND_HASH_FOREACH_PTR(model->u.content, tmp) {
				if (!model_to_xml_object(node, tmp, object, style, strict && (tmp->min_occurs > 0))) {
					if (!strict || tmp->min_occurs > 0) {
						return 0;
					}
				}
				strict = 1;
			} ZEND_HASH_FOREACH_END();
			return 1;
		}
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr tmp;
			int ret = 0;

			ZEND_HASH_FOREACH_PTR(model->u.content, tmp) {
				int tmp_ret = model_to_xml_object(node, tmp, object, style, 0);
				if (tmp_ret == 1) {
					return 1;
				} else if (tmp_ret != 0) {
					ret = 1;
				}
			} ZEND_HASH_FOREACH_END();
			return ret;
		}
		case XSD_CONTENT_GROUP: {
			return model_to_xml_object(node, model->u.group->model, object, style, strict && model->min_occurs > 0);
		}
		default:
		  break;
	}
	return 1;
}

static sdlTypePtr model_array_element(sdlContentModelPtr model)
{
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT: {
			if (model->max_occurs == -1 || model->max_occurs > 1) {
			  return model->u.element;
			} else {
			  return NULL;
			}
		}
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_ALL:
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr tmp;

			if (zend_hash_num_elements(model->u.content) != 1) {
			  return NULL;
			}
			ZEND_HASH_FOREACH_PTR(model->u.content, tmp) {
				return model_array_element(tmp);
			} ZEND_HASH_FOREACH_END();
		}
		/* TODO Check this is correct */
		ZEND_FALLTHROUGH;
		case XSD_CONTENT_GROUP: {
			return model_array_element(model->u.group->model);
		}
		default:
		  break;
	}
	return NULL;
}

static xmlNodePtr to_xml_object(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	xmlNodePtr xmlParam;
	HashTable *prop = NULL;
	sdlTypePtr sdlType = type->sdl_type;

	if (!data || Z_TYPE_P(data) == IS_NULL) {
		xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
		xmlAddChild(parent, xmlParam);
		if (style == SOAP_ENCODED) {
			set_xsi_nil(xmlParam);
			set_ns_and_type(xmlParam, type);
		}
		return xmlParam;
	}

	if (Z_TYPE_P(data) == IS_OBJECT) {
		prop = Z_OBJPROP_P(data);
	} else if (Z_TYPE_P(data) == IS_ARRAY) {
		prop = Z_ARRVAL_P(data);
	}

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
				zval rv;
				zval *tmp = get_zval_property(data, "_", &rv);
				if (tmp) {
					xmlParam = master_to_xml(enc, tmp, style, parent);
				} else if (prop == NULL) {
					xmlParam = master_to_xml(enc, data, style, parent);
				} else {
					xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
					xmlAddChild(parent, xmlParam);
				}
			} else {
				xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
				xmlAddChild(parent, xmlParam);
			}
		} else if (sdlType->kind == XSD_TYPEKIND_EXTENSION &&
		           sdlType->encode && type != &sdlType->encode->details) {
			if (sdlType->encode->details.sdl_type &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_UNION) {

				if (prop) { GC_TRY_PROTECT_RECURSION(prop); }
				xmlParam = master_to_xml(sdlType->encode, data, style, parent);
				if (prop) { GC_TRY_UNPROTECT_RECURSION(prop); }
			} else {
				zval rv;
				zval *tmp = get_zval_property(data, "_", &rv);

				if (tmp) {
					xmlParam = master_to_xml(sdlType->encode, tmp, style, parent);
				} else if (prop == NULL) {
					xmlParam = master_to_xml(sdlType->encode, data, style, parent);
				} else {
					xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
					xmlAddChild(parent, xmlParam);
				}
			}
		} else {
			xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
			xmlAddChild(parent, xmlParam);
		}

		if (soap_check_zval_ref(data, xmlParam)) {
			return xmlParam;
		}
		if (prop != NULL) {
			sdlTypePtr array_el;

			if (Z_TYPE_P(data) == IS_ARRAY &&
		      !is_map(data) &&
		      sdlType->attributes == NULL &&
		      sdlType->model != NULL &&
		      (array_el = model_array_element(sdlType->model)) != NULL) {
				zval *val;

				ZEND_HASH_FOREACH_VAL(prop, val) {
					xmlNodePtr property;
					ZVAL_DEREF(val);
					if (Z_TYPE_P(val) == IS_NULL && array_el->nillable) {
						property = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlAddChild(xmlParam, property);
						set_xsi_nil(property);
					} else {
						property = master_to_xml(array_el->encode, val, style, xmlParam);
					}
					xmlNodeSetName(property, BAD_CAST(array_el->name));
					if (style == SOAP_LITERAL &&
					   array_el->namens &&
					   array_el->form == XSD_FORM_QUALIFIED) {
						xmlNsPtr nsp = encode_add_ns(property, array_el->namens);
						xmlSetNs(property, nsp);
					}
				} ZEND_HASH_FOREACH_END();
			} else if (sdlType->model) {
				model_to_xml_object(xmlParam, sdlType->model, data, style, 1);
			}
			if (sdlType->attributes) {
				sdlAttributePtr attr;
				zval *zattr, rv;

				ZEND_HASH_FOREACH_PTR(sdlType->attributes, attr) {
					if (attr->name) {
						zattr = get_zval_property(data, attr->name, &rv);
						if (zattr) {
							xmlNodePtr dummy;

							dummy = master_to_xml(attr->encode, zattr, SOAP_LITERAL, xmlParam);
							if (dummy->children && dummy->children->content) {
								if (attr->fixed && strcmp(attr->fixed, (char*)dummy->children->content) != 0) {
									soap_error3(E_ERROR, "Encoding: Attribute '%s' has fixed value '%s' (value '%s' is not allowed)", attr->name, attr->fixed, dummy->children->content);
								}
								/* we need to handle xml: namespace specially, since it is
								   an implicit schema. Otherwise, use form.
								*/
								if (attr->namens &&
								    (!strncmp(attr->namens, XML_NAMESPACE, sizeof(XML_NAMESPACE)) ||
								     attr->form == XSD_FORM_QUALIFIED)) {
									xmlNsPtr nsp = encode_add_ns(xmlParam, attr->namens);

									xmlSetNsProp(xmlParam, nsp, BAD_CAST(attr->name), dummy->children->content);
								} else {
									xmlSetProp(xmlParam, BAD_CAST(attr->name), dummy->children->content);
								}
							}
							xmlUnlinkNode(dummy);
							xmlFreeNode(dummy);
						}
					}
				} ZEND_HASH_FOREACH_END();
			}
		}
		if (style == SOAP_ENCODED) {
			set_ns_and_type(xmlParam, type);
		}
	} else {
		xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
		xmlAddChild(parent, xmlParam);

		if (soap_check_zval_ref(data, xmlParam)) {
			return xmlParam;
		}
		if (prop != NULL) {
			zval *zprop;
			zend_string *str_key;
			xmlNodePtr property;

			ZEND_HASH_FOREACH_STR_KEY_VAL_IND(prop, str_key, zprop) {
				ZVAL_DEREF(zprop);
				property = master_to_xml(get_conversion(Z_TYPE_P(zprop)), zprop, style, xmlParam);

				if (str_key) {
					const char *prop_name;

					if (Z_TYPE_P(data) == IS_OBJECT) {
						const char *class_name;

						zend_unmangle_property_name(str_key, &class_name, &prop_name);
					} else {
						prop_name = ZSTR_VAL(str_key);
					}
					if (prop_name) {
						xmlNodeSetName(property, BAD_CAST(prop_name));
					}
				}
			} ZEND_HASH_FOREACH_END();
		}
		if (style == SOAP_ENCODED) {
			set_ns_and_type(xmlParam, type);
		}
	}
	return xmlParam;
}

/* Array encode/decode */
static xmlNodePtr guess_array_map(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	encodePtr enc = NULL;

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

	return master_to_xml(enc, data, style, parent);
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
			soap_error0(E_ERROR, "Encoding: '*' may only be first arraySize value in list");
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

	pos = safe_emalloc(sizeof(int), dimension, 0);
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
			soap_error0(E_ERROR, "Encoding: '*' may only be first arraySize value in list");
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

	pos = safe_emalloc(sizeof(int), dimension, 0);
	get_position_ex(dimension, str, &pos);
	return pos;
}

static void add_xml_array_elements(xmlNodePtr xmlParam,
                                   sdlTypePtr type,
                                   encodePtr enc,
                                   xmlNsPtr ns,
                                   int dimension ,
                                   int* dims,
                                   zval* data,
                                   int style
                                  )
{
	int j = 0;
	zval *zdata;
	xmlNodePtr xparam;

	if (data && Z_TYPE_P(data) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL_IND(Z_ARRVAL_P(data), zdata) {
	 		if (j >= dims[0]) {
	 			break;
	 		}
			ZVAL_DEREF(zdata);
			if (dimension == 1) {
				if (enc == NULL) {
					xparam = master_to_xml(get_conversion(Z_TYPE_P(zdata)), zdata, style, xmlParam);
				} else {
					xparam = master_to_xml(enc, zdata, style, xmlParam);
	 			}

	 			if (type) {
					xmlNodeSetName(xparam, BAD_CAST(type->name));
				} else if (style == SOAP_LITERAL && enc && enc->details.type_str) {
					xmlNodeSetName(xparam, BAD_CAST(enc->details.type_str));
					xmlSetNs(xparam, ns);
				} else {
					xmlNodeSetName(xparam, BAD_CAST("item"));
				}
			} else {
				add_xml_array_elements(xmlParam, type, enc, ns, dimension-1, dims+1, zdata, style);
			}
			j++;
		} ZEND_HASH_FOREACH_END();

		if (dimension == 1) {
	 		while (j < dims[0]) {
				xparam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
				xmlAddChild(xmlParam, xparam);

	 			if (type) {
					xmlNodeSetName(xparam, BAD_CAST(type->name));
				} else if (style == SOAP_LITERAL && enc && enc->details.type_str) {
					xmlNodeSetName(xparam, BAD_CAST(enc->details.type_str));
					xmlSetNs(xparam, ns);
				} else {
					xmlNodeSetName(xparam, BAD_CAST("item"));
				}

				j++;
			}
		} else {
	 		while (j < dims[0]) {
				add_xml_array_elements(xmlParam, type, enc, ns, dimension-1, dims+1, NULL, style);
				j++;
			}
		}
	} else {
		for (j=0; j<dims[0]; j++) {
			if (dimension == 1) {
	 			xmlNodePtr xparam;

				xparam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
				xmlAddChild(xmlParam, xparam);
	 			if (type) {
					xmlNodeSetName(xparam, BAD_CAST(type->name));
				} else if (style == SOAP_LITERAL && enc && enc->details.type_str) {
					xmlNodeSetName(xparam, BAD_CAST(enc->details.type_str));
					xmlSetNs(xparam, ns);
				} else {
					xmlNodeSetName(xparam, BAD_CAST("item"));
				}
			} else {
			  add_xml_array_elements(xmlParam, type, enc, ns, dimension-1, dims+1, NULL, style);
			}
		}
	}
}

static xmlNodePtr to_xml_array(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	sdlTypePtr sdl_type = type->sdl_type;
	sdlTypePtr element_type = NULL;
	smart_str array_type = {0}, array_size = {0};
	int i;
	xmlNodePtr xmlParam;
	encodePtr enc = NULL;
	int dimension = 1;
	int* dims;
	int soap_version;
	zval array_copy;

	ZVAL_UNDEF(&array_copy);
	soap_version = SOAP_GLOBAL(soap_version);

	xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, xmlParam);

	if (!data || Z_TYPE_P(data) == IS_NULL) {
		if (style == SOAP_ENCODED) {
			set_xsi_nil(xmlParam);
			if (SOAP_GLOBAL(features) & SOAP_USE_XSI_ARRAY_TYPE) {
				set_ns_and_type_ex(xmlParam, (soap_version == SOAP_1_1) ? SOAP_1_1_ENC_NAMESPACE : SOAP_1_2_ENC_NAMESPACE, "Array");
			} else {
				set_ns_and_type(xmlParam, type);
			}
		}
		return xmlParam;
	}

	if (Z_TYPE_P(data) == IS_OBJECT && instanceof_function(Z_OBJCE_P(data), zend_ce_traversable)) {
		zend_object_iterator   *iter;
		zend_class_entry       *ce = Z_OBJCE_P(data);
		zval                   *val;

		array_init(&array_copy);

		iter = ce->get_iterator(ce, data, 0);

		if (EG(exception)) {
			goto iterator_done;
		}

		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter);
			if (EG(exception)) {
				goto iterator_done;
			}
		}

		while (iter->funcs->valid(iter) == SUCCESS) {
			if (EG(exception)) {
				goto iterator_done;
			}

			val = iter->funcs->get_current_data(iter);
			if (EG(exception)) {
				goto iterator_done;
			}
			if (iter->funcs->get_current_key) {
				zval key;
				iter->funcs->get_current_key(iter, &key);
				if (EG(exception)) {
					goto iterator_done;
				}
				array_set_zval_key(Z_ARRVAL(array_copy), &key, val);
				zval_ptr_dtor(val);
				zval_ptr_dtor(&key);
			} else {
				add_next_index_zval(&array_copy, val);
			}
			Z_TRY_ADDREF_P(val);

			iter->funcs->move_forward(iter);
			if (EG(exception)) {
				goto iterator_done;
			}
		}
iterator_done:
		OBJ_RELEASE(&iter->std);
		if (EG(exception)) {
			zval_ptr_dtor(&array_copy);
			ZVAL_UNDEF(&array_copy);
		} else {
			data = &array_copy;
		}
	}

	if (Z_TYPE_P(data) == IS_ARRAY) {
		sdlAttributePtr arrayType;
		sdlExtraAttributePtr ext;
		sdlTypePtr elementType;

		i = zend_hash_num_elements(Z_ARRVAL_P(data));

		if (sdl_type &&
		    sdl_type->attributes &&
		    (arrayType = zend_hash_str_find_ptr(sdl_type->attributes, SOAP_1_1_ENC_NAMESPACE":arrayType",
		      sizeof(SOAP_1_1_ENC_NAMESPACE":arrayType")-1)) != NULL &&
		    arrayType->extraAttributes &&
		    (ext = zend_hash_str_find_ptr(arrayType->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType")-1)) != NULL) {

			char *value, *end;
			zval *el;

			value = estrdup(ext->val);
			end = strrchr(value,'[');
			if (end) {
				*end = '\0';
				end++;
				dimension = calc_dimension(end);
			}
			if (ext->ns != NULL) {
				enc = get_encoder(SOAP_GLOBAL(sdl), ext->ns, value);
				get_type_str(xmlParam, ext->ns, value, &array_type);
			} else {
				smart_str_appends(&array_type, value);
			}

			dims = safe_emalloc(sizeof(int), dimension, 0);
			dims[0] = i;
			el = data;
			for (i = 1; i < dimension; i++) {
				if (el != NULL && Z_TYPE_P(el) == IS_ARRAY &&
				    zend_hash_num_elements(Z_ARRVAL_P(el)) > 0) {
				    ZEND_HASH_FOREACH_VAL_IND(Z_ARRVAL_P(el), el) {
				    	break;
				    } ZEND_HASH_FOREACH_END();
					ZVAL_DEREF(el);
					if (Z_TYPE_P(el) == IS_ARRAY) {
						dims[i] = zend_hash_num_elements(Z_ARRVAL_P(el));
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

		} else if (sdl_type &&
		           sdl_type->attributes &&
		           (arrayType = zend_hash_str_find_ptr(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":itemType",
		             sizeof(SOAP_1_2_ENC_NAMESPACE":itemType")-1)) != NULL &&
		           arrayType->extraAttributes &&
		           (ext = zend_hash_str_find_ptr(arrayType->extraAttributes, WSDL_NAMESPACE":itemType", sizeof(WSDL_NAMESPACE":itemType")-1)) != NULL) {
			if (ext->ns != NULL) {
				enc = get_encoder(SOAP_GLOBAL(sdl), ext->ns, ext->val);
				get_type_str(xmlParam, ext->ns, ext->val, &array_type);
			} else {
				smart_str_appends(&array_type, ext->val);
			}
			if ((arrayType = zend_hash_str_find_ptr(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
			                   sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize")-1)) != NULL &&
			    arrayType->extraAttributes &&
			    (ext = zend_hash_str_find_ptr(arrayType->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize")-1)) != NULL) {
				dimension = calc_dimension_12(ext->val);
				dims = get_position_12(dimension, ext->val);
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
		           (arrayType = zend_hash_str_find_ptr(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
		             sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize")-1)) != NULL &&
		           arrayType->extraAttributes &&
		           (ext = zend_hash_str_find_ptr(arrayType->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraySize")-1)) != NULL) {
			dimension = calc_dimension_12(ext->val);
			dims = get_position_12(dimension, ext->val);
			if (dims[0] == 0) {dims[0] = i;}

			smart_str_append_long(&array_size, dims[0]);
			for (i=1; i<dimension; i++) {
				smart_str_appendc(&array_size, ',');
				smart_str_append_long(&array_size, dims[i]);
			}

			if (sdl_type && sdl_type->elements &&
			    zend_hash_num_elements(sdl_type->elements) == 1 &&
			    (zend_hash_internal_pointer_reset(sdl_type->elements),
			     (elementType = zend_hash_get_current_data_ptr(sdl_type->elements)) != NULL) &&
			     elementType->encode && elementType->encode->details.type_str) {
				element_type = elementType;
				enc = elementType->encode;
				get_type_str(xmlParam, elementType->encode->details.ns, elementType->encode->details.type_str, &array_type);
			} else {
				enc = get_array_type(xmlParam, data, &array_type);
			}
		} else if (sdl_type && sdl_type->elements &&
		           zend_hash_num_elements(sdl_type->elements) == 1 &&
		           (zend_hash_internal_pointer_reset(sdl_type->elements),
		            (elementType = zend_hash_get_current_data_ptr(sdl_type->elements)) != NULL) &&
		           elementType->encode && elementType->encode->details.type_str) {

			element_type = elementType;
			enc = elementType->encode;
			get_type_str(xmlParam, elementType->encode->details.ns, elementType->encode->details.type_str, &array_type);

			smart_str_append_long(&array_size, i);

			dims = safe_emalloc(sizeof(int), dimension, 0);
			dims[0] = i;
		} else {

			enc = get_array_type(xmlParam, data, &array_type);
			smart_str_append_long(&array_size, i);
			dims = safe_emalloc(sizeof(int), dimension, 0);
			dims[0] = i;
		}

		if (style == SOAP_ENCODED) {
			if (soap_version == SOAP_1_1) {
				smart_str_0(&array_type);
				if (strcmp(ZSTR_VAL(array_type.s),"xsd:anyType") == 0) {
					smart_str_free(&array_type);
					smart_str_appendl(&array_type,"xsd:ur-type",sizeof("xsd:ur-type")-1);
				}
				smart_str_appendc(&array_type, '[');
				smart_str_append_smart_str(&array_type, &array_size);
				smart_str_appendc(&array_type, ']');
				smart_str_0(&array_type);
				set_ns_prop(xmlParam, SOAP_1_1_ENC_NAMESPACE, "arrayType", ZSTR_VAL(array_type.s));
			} else {
				size_t i = 0;
				while (i < ZSTR_LEN(array_size.s)) {
					if (ZSTR_VAL(array_size.s)[i] == ',') {ZSTR_VAL(array_size.s)[i] = ' ';}
					++i;
				}
				smart_str_0(&array_type);
				smart_str_0(&array_size);
				set_ns_prop(xmlParam, SOAP_1_2_ENC_NAMESPACE, "itemType", ZSTR_VAL(array_type.s));
				set_ns_prop(xmlParam, SOAP_1_2_ENC_NAMESPACE, "arraySize", ZSTR_VAL(array_size.s));
			}
		}
		smart_str_free(&array_type);
		smart_str_free(&array_size);

		add_xml_array_elements(xmlParam, element_type, enc, enc?encode_add_ns(xmlParam,enc->details.ns):NULL, dimension, dims, data, style);
		efree(dims);
	}
	if (style == SOAP_ENCODED) {
		if (SOAP_GLOBAL(features) & SOAP_USE_XSI_ARRAY_TYPE) {
			set_ns_and_type_ex(xmlParam, (soap_version == SOAP_1_1) ? SOAP_1_1_ENC_NAMESPACE : SOAP_1_2_ENC_NAMESPACE, "Array");
		} else {
			set_ns_and_type(xmlParam, type);
		}
	}

	zval_ptr_dtor(&array_copy);

	return xmlParam;
}

static zval *to_zval_array(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	xmlNodePtr trav;
	encodePtr enc = NULL;
	int dimension = 1;
	int* dims = NULL;
	int* pos = NULL;
	xmlAttrPtr attr;
	sdlAttributePtr arrayType;
	sdlExtraAttributePtr ext;
	sdlTypePtr elementType;

	ZVAL_NULL(ret);
	FIND_XML_NULL(data, ret);

	if (data &&
	    (attr = get_attribute(data->properties,"arrayType")) &&
	    attr->children && attr->children->content) {
		char *type, *end, *ns;
		xmlNsPtr nsptr;

		parse_namespace(attr->children->content, &type, &ns);
		nsptr = xmlSearchNs(attr->doc, attr->parent, BAD_CAST(ns));

		end = strrchr(type,'[');
		if (end) {
			*end = '\0';
			dimension = calc_dimension(end+1);
			dims = get_position(dimension, end+1);
		}
		if (nsptr != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), (char*)nsptr->href, type);
		}
		efree(type);
		if (ns) {efree(ns);}

	} else if ((attr = get_attribute(data->properties,"itemType")) &&
	    attr->children &&
	    attr->children->content) {
		char *type, *ns;
		xmlNsPtr nsptr;

		parse_namespace(attr->children->content, &type, &ns);
		nsptr = xmlSearchNs(attr->doc, attr->parent, BAD_CAST(ns));
		if (nsptr != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), (char*)nsptr->href, type);
		}
		efree(type);
		if (ns) {efree(ns);}

		if ((attr = get_attribute(data->properties,"arraySize")) &&
		    attr->children && attr->children->content) {
			dimension = calc_dimension_12((char*)attr->children->content);
			dims = get_position_12(dimension, (char*)attr->children->content);
		} else {
			dims = emalloc(sizeof(int));
			*dims = 0;
		}

	} else if ((attr = get_attribute(data->properties,"arraySize")) &&
	    attr->children && attr->children->content) {

		dimension = calc_dimension_12((char*)attr->children->content);
		dims = get_position_12(dimension, (char*)attr->children->content);

	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           (arrayType = zend_hash_str_find_ptr(type->sdl_type->attributes, SOAP_1_1_ENC_NAMESPACE":arrayType",
	                          sizeof(SOAP_1_1_ENC_NAMESPACE":arrayType")-1)) != NULL &&
	           arrayType->extraAttributes &&
	           (ext = zend_hash_str_find_ptr(arrayType->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType")-1)) != NULL) {
		char *type, *end;

		type = estrdup(ext->val);
		end = strrchr(type,'[');
		if (end) {
			*end = '\0';
		}
		if (ext->ns != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), ext->ns, type);
		}
		efree(type);

		dims = emalloc(sizeof(int));
		*dims = 0;

	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           (arrayType = zend_hash_str_find_ptr(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":itemType",
	                          sizeof(SOAP_1_2_ENC_NAMESPACE":itemType")-1)) != NULL &&
	           arrayType->extraAttributes &&
	           (ext = zend_hash_str_find_ptr(arrayType->extraAttributes, WSDL_NAMESPACE":itemType", sizeof(WSDL_NAMESPACE":itemType")-1)) != NULL) {

		if (ext->ns != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), ext->ns, ext->val);
		}

		if ((arrayType = zend_hash_str_find_ptr(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
		                   sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize")-1)) != NULL &&
		    arrayType->extraAttributes &&
		    (ext = zend_hash_str_find_ptr(arrayType->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize")-1)) != NULL) {
			dimension = calc_dimension_12(ext->val);
			dims = get_position_12(dimension, ext->val);
		} else {
			dims = emalloc(sizeof(int));
			*dims = 0;
		}
	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           (arrayType = zend_hash_str_find_ptr(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
	                          sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize")-1)) != NULL &&
	           arrayType->extraAttributes &&
	           (ext = zend_hash_str_find_ptr(arrayType->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize")-1)) != NULL) {

		dimension = calc_dimension_12(ext->val);
		dims = get_position_12(dimension, ext->val);
		if (type->sdl_type && type->sdl_type->elements &&
		    zend_hash_num_elements(type->sdl_type->elements) == 1 &&
		    (zend_hash_internal_pointer_reset(type->sdl_type->elements),
		     (elementType = zend_hash_get_current_data_ptr(type->sdl_type->elements)) != NULL) &&
		    elementType->encode) {
			enc = elementType->encode;
		}
	} else if (type->sdl_type && type->sdl_type->elements &&
	           zend_hash_num_elements(type->sdl_type->elements) == 1 &&
	           (zend_hash_internal_pointer_reset(type->sdl_type->elements),
	            (elementType = zend_hash_get_current_data_ptr(type->sdl_type->elements)) != NULL) &&
	           elementType->encode) {
		enc = elementType->encode;
	}
	if (dims == NULL) {
		dimension = 1;
		dims = emalloc(sizeof(int));
		*dims = 0;
	}
	pos = safe_emalloc(sizeof(int), dimension, 0);
	memset(pos,0,sizeof(int)*dimension);
	if (data &&
	    (attr = get_attribute(data->properties,"offset")) &&
	     attr->children && attr->children->content) {
		char* tmp = strrchr((char*)attr->children->content,'[');

		if (tmp == NULL) {
			tmp = (char*)attr->children->content;
		}
		get_position_ex(dimension, tmp, &pos);
	}

	array_init(ret);
	trav = data->children;
	while (trav) {
		if (trav->type == XML_ELEMENT_NODE) {
			int i;
			zval tmpVal, *ar;
			xmlAttrPtr position = get_attribute(trav->properties,"position");

			ZVAL_NULL(&tmpVal);
			master_to_zval(&tmpVal, enc, trav);
			if (position != NULL && position->children && position->children->content) {
				char* tmp = strrchr((char*)position->children->content, '[');
				if (tmp == NULL) {
					tmp = (char*)position->children->content;
				}
				get_position_ex(dimension, tmp, &pos);
			}

			/* Get/Create intermediate arrays for multidimensional arrays */
			i = 0;
			ar = ret;
			while (i < dimension-1) {
				zval* ar2;
				if ((ar2 = zend_hash_index_find(Z_ARRVAL_P(ar), pos[i])) != NULL) {
					ar = ar2;
				} else {
					zval tmpAr;
					array_init(&tmpAr);
					ar = zend_hash_index_update(Z_ARRVAL_P(ar), pos[i], &tmpAr);
				}
				i++;
			}
			zend_hash_index_update(Z_ARRVAL_P(ar), pos[i], &tmpVal);

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
static xmlNodePtr to_xml_map(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	zval *temp_data;
	zend_string *key_val;
	zend_ulong int_val;
	xmlNodePtr xmlParam;
	xmlNodePtr xparam, item;
	xmlNodePtr key;

	xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, xmlParam);
	FIND_ZVAL_NULL(data, xmlParam, style);

	if (Z_TYPE_P(data) == IS_ARRAY) {
		ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(data), int_val, key_val, temp_data) {
			item = xmlNewNode(NULL, BAD_CAST("item"));
			xmlAddChild(xmlParam, item);
			key = xmlNewNode(NULL, BAD_CAST("key"));
			xmlAddChild(item,key);
			if (key_val) {
				if (style == SOAP_ENCODED) {
					set_xsi_type(key, "xsd:string");
				}
				xmlNodeSetContent(key, BAD_CAST(ZSTR_VAL(key_val)));
			} else {
				smart_str tmp = {0};
				smart_str_append_long(&tmp, int_val);
				smart_str_0(&tmp);

				if (style == SOAP_ENCODED) {
					set_xsi_type(key, "xsd:int");
				}
				xmlNodeSetContentLen(key, BAD_CAST(ZSTR_VAL(tmp.s)), ZSTR_LEN(tmp.s));

				smart_str_free(&tmp);
			}

			ZVAL_DEREF(temp_data);
			xparam = master_to_xml(get_conversion(Z_TYPE_P(temp_data)), temp_data, style, item);
			xmlNodeSetName(xparam, BAD_CAST("value"));
		} ZEND_HASH_FOREACH_END();
	}
	if (style == SOAP_ENCODED) {
		set_ns_and_type(xmlParam, type);
	}

	return xmlParam;
}

static zval *to_zval_map(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	zval key, value;
	xmlNodePtr trav, item, xmlKey, xmlValue;

	ZVAL_NULL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		array_init(ret);
		trav = data->children;

		trav = data->children;
		FOREACHNODE(trav, "item", item) {
			xmlKey = get_node(item->children, "key");
			if (!xmlKey) {
				soap_error0(E_ERROR,  "Encoding: Can't decode apache map, missing key");
			}

			xmlValue = get_node(item->children, "value");
			if (!xmlKey) {
				soap_error0(E_ERROR,  "Encoding: Can't decode apache map, missing value");
			}

			ZVAL_NULL(&key);
			master_to_zval(&key, NULL, xmlKey);
			ZVAL_NULL(&value);
			master_to_zval(&value, NULL, xmlValue);

			if (Z_TYPE(key) == IS_STRING) {
				zend_symtable_update(Z_ARRVAL_P(ret), Z_STR(key), &value);
			} else if (Z_TYPE(key) == IS_LONG) {
				zend_hash_index_update(Z_ARRVAL_P(ret), Z_LVAL(key), &value);
			} else {
				soap_error0(E_ERROR,  "Encoding: Can't decode apache map, only Strings or Longs are allowed as keys");
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
static xmlNodePtr guess_xml_convert(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	encodePtr  enc;
	xmlNodePtr ret;

	if (data) {
		enc = get_conversion(Z_TYPE_P(data));
	} else {
		enc = get_conversion(IS_NULL);
	}
	ret = master_to_xml_int(enc, data, style, parent, 0);
/*
	if (style == SOAP_LITERAL && SOAP_GLOBAL(sdl)) {
		set_ns_and_type(ret, &enc->details);
	}
*/
	return ret;
}

static zval *guess_zval_convert(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	encodePtr enc = NULL;
	xmlAttrPtr tmpattr;
	xmlChar *type_name = NULL;

	data = check_and_resolve_href(data);

	if (data == NULL) {
		enc = get_conversion(IS_NULL);
	} else if (data->properties && get_attribute_ex(data->properties, "nil", XSI_NAMESPACE)) {
		enc = get_conversion(IS_NULL);
	} else {
		tmpattr = get_attribute_ex(data->properties,"type", XSI_NAMESPACE);
		if (tmpattr != NULL) {
		  type_name = tmpattr->children->content;
			enc = get_encoder_from_prefix(SOAP_GLOBAL(sdl), data, tmpattr->children->content);
			if (enc && type == &enc->details) {
				enc = NULL;
			}
			if (enc != NULL) {
			  encodePtr tmp = enc;
			  while (tmp &&
			         tmp->details.sdl_type != NULL &&
			         tmp->details.sdl_type->kind != XSD_TYPEKIND_COMPLEX) {
			    if (enc == tmp->details.sdl_type->encode ||
			        tmp == tmp->details.sdl_type->encode) {
			    	enc = NULL;
			    	break;
			    }
			    tmp = tmp->details.sdl_type->encode;
			  }
			}
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
	master_to_zval_int(ret, enc, data);
	if (SOAP_GLOBAL(sdl) && type_name && enc->details.sdl_type) {
		zval soapvar;
		char *ns, *cptype;
		xmlNsPtr nsptr;

		object_init_ex(&soapvar, soap_var_class_entry);
		ZVAL_LONG(Z_VAR_ENC_TYPE_P(&soapvar), enc->details.type);
		ZVAL_COPY_VALUE(Z_VAR_ENC_VALUE_P(&soapvar), ret);
		parse_namespace(type_name, &cptype, &ns);
		nsptr = xmlSearchNs(data->doc, data, BAD_CAST(ns));
		ZVAL_STRING(Z_VAR_ENC_STYPE_P(&soapvar), cptype);
		if (nsptr) {
			ZVAL_STRING(Z_VAR_ENC_NS_P(&soapvar), (char*)nsptr->href);
		}
		efree(cptype);
		if (ns) {efree(ns);}
		ZVAL_COPY_VALUE(ret, &soapvar);
	}
	return ret;
}

/* Time encode/decode */
static xmlNodePtr to_xml_datetime_ex(encodeTypePtr type, zval *data, char *format, int style, xmlNodePtr parent)
{
	/* logic hacked from ext/standard/datetime.c */
	struct tm *ta, tmbuf;
	time_t timestamp;
	int max_reallocs = 5;
	size_t buf_len=64, real_len;
	char *buf;
	char tzbuf[8];

	xmlNodePtr xmlParam;

	xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, xmlParam);
	FIND_ZVAL_NULL(data, xmlParam, style);

	if (Z_TYPE_P(data) == IS_LONG) {
		timestamp = Z_LVAL_P(data);
		ta = php_localtime_r(&timestamp, &tmbuf);
		/*ta = php_gmtime_r(&timestamp, &tmbuf);*/
		if (!ta) {
			soap_error1(E_ERROR, "Encoding: Invalid timestamp " ZEND_LONG_FMT, Z_LVAL_P(data));
		}

		buf = (char *) emalloc(buf_len);
		while ((real_len = strftime(buf, buf_len, format, ta)) == buf_len || real_len == 0) {
			buf_len *= 2;
			buf = (char *) erealloc(buf, buf_len);
			if (!--max_reallocs) break;
		}

		/* Time zone support */
#ifdef HAVE_STRUCT_TM_TM_GMTOFF
		snprintf(tzbuf, sizeof(tzbuf), "%c%02ld:%02ld",
			(ta->tm_gmtoff < 0) ? '-' : '+',
			labs(ta->tm_gmtoff / 3600), labs( (ta->tm_gmtoff % 3600) / 60 ));
#else
# if defined(__CYGWIN__) || (defined(PHP_WIN32) && defined(_MSC_VER) && _MSC_VER >= 1900)
		snprintf(tzbuf, sizeof(tzbuf), "%c%02d:%02d", ((ta->tm_isdst ? _timezone - 3600:_timezone)>0)?'-':'+', abs((ta->tm_isdst ? _timezone - 3600 : _timezone) / 3600), abs(((ta->tm_isdst ? _timezone - 3600 : _timezone) % 3600) / 60));
# else
		snprintf(tzbuf, sizeof(tzbuf), "%c%02d:%02d", ((ta->tm_isdst ? timezone - 3600:timezone)>0)?'-':'+', abs((ta->tm_isdst ? timezone - 3600 : timezone) / 3600), abs(((ta->tm_isdst ? timezone - 3600 : timezone) % 3600) / 60));
# endif
#endif
		if (strcmp(tzbuf,"+00:00") == 0) {
		  strcpy(tzbuf,"Z");
		  real_len++;
		} else {
			real_len += 6;
		}
		if (real_len >= buf_len) {
			buf = (char *) erealloc(buf, real_len+1);
		}
		strcat(buf, tzbuf);

		xmlNodeSetContent(xmlParam, BAD_CAST(buf));
		efree(buf);
	} else if (Z_TYPE_P(data) == IS_STRING) {
		xmlNodeSetContentLen(xmlParam, BAD_CAST(Z_STRVAL_P(data)), Z_STRLEN_P(data));
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(xmlParam, type);
	}
	return xmlParam;
}

static xmlNodePtr to_xml_duration(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	/* TODO: '-'?P([0-9]+Y)?([0-9]+M)?([0-9]+D)?T([0-9]+H)?([0-9]+M)?([0-9]+S)? */
	return to_xml_string(type, data, style, parent);
}

static xmlNodePtr to_xml_datetime(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	return to_xml_datetime_ex(type, data, "%Y-%m-%dT%H:%M:%S", style, parent);
}

static xmlNodePtr to_xml_time(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	/* TODO: microsecconds */
	return to_xml_datetime_ex(type, data, "%H:%M:%S", style, parent);
}

static xmlNodePtr to_xml_date(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	return to_xml_datetime_ex(type, data, "%Y-%m-%d", style, parent);
}

static xmlNodePtr to_xml_gyearmonth(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	return to_xml_datetime_ex(type, data, "%Y-%m", style, parent);
}

static xmlNodePtr to_xml_gyear(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	return to_xml_datetime_ex(type, data, "%Y", style, parent);
}

static xmlNodePtr to_xml_gmonthday(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	return to_xml_datetime_ex(type, data, "--%m-%d", style, parent);
}

static xmlNodePtr to_xml_gday(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	return to_xml_datetime_ex(type, data, "---%d", style, parent);
}

static xmlNodePtr to_xml_gmonth(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	return to_xml_datetime_ex(type, data, "--%m--", style, parent);
}

static zval* to_zval_list(zval *ret, encodeTypePtr enc, xmlNodePtr data) {
	/*FIXME*/
	return to_zval_stringc(ret, enc, data);
}

static xmlNodePtr to_xml_list(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent) {
	xmlNodePtr ret;
	encodePtr list_enc = NULL;

	if (enc->sdl_type && enc->sdl_type->kind == XSD_TYPEKIND_LIST && enc->sdl_type->elements) {
		sdlTypePtr type;

		ZEND_HASH_FOREACH_PTR(enc->sdl_type->elements, type) {
			list_enc = type->encode;
			break;
		} ZEND_HASH_FOREACH_END();
	}

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);
	if (Z_TYPE_P(data) == IS_ARRAY) {
		zval *tmp;
		smart_str list = {0};
		HashTable *ht = Z_ARRVAL_P(data);

		ZEND_HASH_FOREACH_VAL(ht, tmp) {
			xmlNodePtr dummy = master_to_xml(list_enc, tmp, SOAP_LITERAL, ret);
			if (dummy && dummy->children && dummy->children->content) {
				if (list.s && ZSTR_LEN(list.s) != 0) {
					smart_str_appendc(&list, ' ');
				}
				smart_str_appends(&list, (char*)dummy->children->content);
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			xmlUnlinkNode(dummy);
			xmlFreeNode(dummy);
		} ZEND_HASH_FOREACH_END();
		smart_str_0(&list);
		if (list.s) {
			xmlNodeSetContentLen(ret, BAD_CAST(ZSTR_VAL(list.s)), ZSTR_LEN(list.s));
		} else {
			xmlNodeSetContentLen(ret, BAD_CAST(""), 0);
		}
		smart_str_free(&list);
	} else {
		zval tmp;
		char *str, *start, *next;
		smart_str list = {0};

		if (Z_TYPE_P(data) != IS_STRING) {
			ZVAL_STR(&tmp, zval_get_string_func(data));
			data = &tmp;
		}
		str = estrndup(Z_STRVAL_P(data), Z_STRLEN_P(data));
		whiteSpace_collapse(BAD_CAST(str));
		start = str;
		while (start != NULL && *start != '\0') {
			xmlNodePtr dummy;
			zval dummy_zval;

			next = strchr(start,' ');
			if (next != NULL) {
			  *next = '\0';
			  next++;
			}
			ZVAL_STRING(&dummy_zval, start);
			dummy = master_to_xml(list_enc, &dummy_zval, SOAP_LITERAL, ret);
			zval_ptr_dtor(&dummy_zval);
			if (dummy && dummy->children && dummy->children->content) {
				if (list.s && ZSTR_LEN(list.s) != 0) {
					smart_str_appendc(&list, ' ');
				}
				smart_str_appends(&list, (char*)dummy->children->content);
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			xmlUnlinkNode(dummy);
			xmlFreeNode(dummy);

			start = next;
		}
		smart_str_0(&list);
		if (list.s) {
			xmlNodeSetContentLen(ret, BAD_CAST(ZSTR_VAL(list.s)), ZSTR_LEN(list.s));
		} else {
			xmlNodeSetContentLen(ret, BAD_CAST(""), 0);
		}
		smart_str_free(&list);
		efree(str);
		if (data == &tmp) {
			zval_ptr_dtor_str(&tmp);
		}
	}
	return ret;
}

static xmlNodePtr to_xml_list1(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent) {
	/*FIXME: minLength=1 */
	return to_xml_list(enc,data,style, parent);
}

static zval* to_zval_union(zval *ret, encodeTypePtr enc, xmlNodePtr data) {
	/*FIXME*/
	return to_zval_list(ret, enc, data);
}

static xmlNodePtr to_xml_union(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent) {
	/*FIXME*/
	return to_xml_list(enc,data,style, parent);
}

static zval *to_zval_any(zval *ret, encodeTypePtr type, xmlNodePtr data)
{
	xmlBufferPtr buf;

	if (SOAP_GLOBAL(sdl) && SOAP_GLOBAL(sdl)->elements && data->name) {
		smart_str nscat = {0};
		sdlTypePtr sdl_type;

		if (data->ns && data->ns->href) {
			smart_str_appends(&nscat, (char*)data->ns->href);
			smart_str_appendc(&nscat, ':');
		}
		smart_str_appends(&nscat, (char*)data->name);
		smart_str_0(&nscat);

		if ((sdl_type = zend_hash_find_ptr(SOAP_GLOBAL(sdl)->elements, nscat.s)) != NULL &&
		    sdl_type->encode) {
			smart_str_free(&nscat);
			return master_to_zval_int(ret, sdl_type->encode, data);
		}
		smart_str_free(&nscat);
	}

	buf = xmlBufferCreate();
	xmlNodeDump(buf, NULL, data, 0, 0);
	ZVAL_STRING(ret, (char*)xmlBufferContent(buf));
	xmlBufferFree(buf);
	return ret;
}

static xmlNodePtr to_xml_any(encodeTypePtr type, zval *data, int style, xmlNodePtr parent)
{
	xmlNodePtr ret = NULL;

	if (Z_TYPE_P(data) == IS_ARRAY) {
		zval *el;
		encodePtr enc = get_conversion(XSD_ANYXML);
		zend_string *name;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(data), name, el) {
			ret = master_to_xml(enc, el, style, parent);
		    if (ret &&
		        ret->name != xmlStringTextNoenc) {
				xmlNodeSetName(ret, BAD_CAST(ZSTR_VAL(name)));
		    }
		} ZEND_HASH_FOREACH_END();
		return ret;
	}
	if (Z_TYPE_P(data) == IS_STRING) {
		ret = xmlNewTextLen(BAD_CAST(Z_STRVAL_P(data)), Z_STRLEN_P(data));
	} else {
		zend_string *tmp = zval_get_string_func(data);
		ret = xmlNewTextLen(BAD_CAST(ZSTR_VAL(tmp)), ZSTR_LEN(tmp));
		zend_string_release_ex(tmp, 0);
	}

	ret->name = xmlStringTextNoenc;
	ret->parent = parent;
	ret->doc = parent->doc;
	ret->prev = parent->last;
	ret->next = NULL;
	if (parent->last) {
		parent->last->next = ret;
	} else {
		parent->children = ret;
	}
	parent->last = ret;

	return ret;
}

zval *sdl_guess_convert_zval(zval *ret, encodeTypePtr enc, xmlNodePtr data)
{
	sdlTypePtr type;

	type = enc->sdl_type;
	if (type == NULL) {
		return guess_zval_convert(ret, enc, data);
	}
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
				soap_error1(E_WARNING, "Encoding: Restriction: invalid enumeration value \"%s\"", data->children->content);
			}
		}
		if (type->restrictions->minLength &&
		    strlen(data->children->content) < type->restrictions->minLength->value) {
		  soap_error0(E_WARNING, "Encoding: Restriction: length less than 'minLength'");
		}
		if (type->restrictions->maxLength &&
		    strlen(data->children->content) > type->restrictions->maxLength->value) {
		  soap_error0(E_WARNING, "Encoding: Restriction: length greater than 'maxLength'");
		}
		if (type->restrictions->length &&
		    strlen(data->children->content) != type->restrictions->length->value) {
		  soap_error0(E_WARNING, "Encoding: Restriction: length is not equal to 'length'");
		}
	}
*/
	switch (type->kind) {
		case XSD_TYPEKIND_SIMPLE:
			if (type->encode && enc != &type->encode->details) {
				return master_to_zval_int(ret, type->encode, data);
			} else {
				return guess_zval_convert(ret, enc, data);
			}
			break;
		case XSD_TYPEKIND_LIST:
			return to_zval_list(ret, enc, data);
		case XSD_TYPEKIND_UNION:
			return to_zval_union(ret, enc, data);
		case XSD_TYPEKIND_COMPLEX:
		case XSD_TYPEKIND_RESTRICTION:
		case XSD_TYPEKIND_EXTENSION:
			if (type->encode &&
			    (type->encode->details.type == IS_ARRAY ||
			     type->encode->details.type == SOAP_ENC_ARRAY)) {
				return to_zval_array(ret, enc, data);
			}
			return to_zval_object(ret, enc, data);
		default:
	  	soap_error0(E_ERROR, "Encoding: Internal Error");
			return guess_zval_convert(ret, enc, data);
	}
}

xmlNodePtr sdl_guess_convert_xml(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent)
{
	sdlTypePtr type;
	xmlNodePtr ret = NULL;

	type = enc->sdl_type;

	if (type == NULL) {
		ret = guess_xml_convert(enc, data, style, parent);
		if (style == SOAP_ENCODED) {
			set_ns_and_type(ret, enc);
		}
		return ret;
	}
/*FIXME: restriction support
	if (type) {
		if (type->restrictions && Z_TYPE_P(data) == IS_STRING) {
			if (type->restrictions->enumeration) {
				if (!zend_hash_exists(type->restrictions->enumeration,Z_STRVAL_P(data),Z_STRLEN_P(data)+1)) {
					soap_error1(E_WARNING, "Encoding: Restriction: invalid enumeration value \"%s\".", Z_STRVAL_P(data));
				}
			}
			if (type->restrictions->minLength &&
			    Z_STRLEN_P(data) < type->restrictions->minLength->value) {
		  	soap_error0(E_WARNING, "Encoding: Restriction: length less than 'minLength'");
			}
			if (type->restrictions->maxLength &&
			    Z_STRLEN_P(data) > type->restrictions->maxLength->value) {
		  	soap_error0(E_WARNING, "Encoding: Restriction: length greater than 'maxLength'");
			}
			if (type->restrictions->length &&
			    Z_STRLEN_P(data) != type->restrictions->length->value) {
		  	soap_error0(E_WARNING, "Encoding: Restriction: length is not equal to 'length'");
			}
		}
	}
*/
	switch(type->kind) {
		case XSD_TYPEKIND_SIMPLE:
			if (type->encode && enc != &type->encode->details) {
				ret = master_to_xml(type->encode, data, style, parent);
			} else {
				ret = guess_xml_convert(enc, data, style, parent);
			}
			break;
		case XSD_TYPEKIND_LIST:
			ret = to_xml_list(enc, data, style, parent);
			break;
		case XSD_TYPEKIND_UNION:
			ret = to_xml_union(enc, data, style, parent);
			break;
		case XSD_TYPEKIND_COMPLEX:
		case XSD_TYPEKIND_RESTRICTION:
		case XSD_TYPEKIND_EXTENSION:
			if (type->encode &&
			    (type->encode->details.type == IS_ARRAY ||
			     type->encode->details.type == SOAP_ENC_ARRAY)) {
				return to_xml_array(enc, data, style, parent);
			} else {
				return to_xml_object(enc, data, style, parent);
			}
			break;
		default:
	  	soap_error0(E_ERROR, "Encoding: Internal Error");
			break;
	}
	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, enc);
	}
	return ret;
}

static xmlNodePtr check_and_resolve_href(xmlNodePtr data)
{
	if (data && data->properties) {
		xmlAttrPtr href;

		href = data->properties;
		while (1) {
			href = get_attribute(href, "href");
			if (href == NULL || href->ns == NULL) {break;}
			href = href->next;
		}
		if (href) {
			/*  Internal href try and find node */
			if (href->children->content[0] == '#') {
				xmlNodePtr ret = get_node_with_attribute_recursive(data->doc->children, NULL, "id", (char*)&href->children->content[1]);
				if (!ret) {
					soap_error1(E_ERROR, "Encoding: Unresolved reference '%s'", href->children->content);
				}
				return ret;
			} else {
				/*  TODO: External href....? */
				soap_error1(E_ERROR, "Encoding: External reference '%s'", href->children->content);
			}
		}
		/* SOAP 1.2 enc:id enc:ref */
		href = get_attribute_ex(data->properties, "ref", SOAP_1_2_ENC_NAMESPACE);
		if (href) {
			xmlChar* id;
			xmlNodePtr ret;

			if (href->children->content[0] == '#') {
				id = href->children->content+1;
			} else {
				id = href->children->content;
			}
			ret = get_node_with_attribute_recursive_ex(data->doc->children, NULL, NULL, "id", (char*)id, SOAP_1_2_ENC_NAMESPACE);
			if (!ret) {
				soap_error1(E_ERROR, "Encoding: Unresolved reference '%s'", href->children->content);
			} else if (ret == data) {
				soap_error1(E_ERROR, "Encoding: Violation of id and ref information items '%s'", href->children->content);
			}
			return ret;
		}
	}
	return data;
}

static void set_ns_and_type(xmlNodePtr node, encodeTypePtr type)
{
	set_ns_and_type_ex(node, type->ns, type->type_str);
}

static void set_ns_and_type_ex(xmlNodePtr node, char *ns, char *type)
{
	smart_str nstype = {0};
	get_type_str(node, ns, type, &nstype);
	set_xsi_type(node, ZSTR_VAL(nstype.s));
	smart_str_free(&nstype);
}

static xmlNsPtr xmlSearchNsPrefixByHref(xmlDocPtr doc, xmlNodePtr node, const xmlChar * href)
{
	xmlNsPtr cur;
	xmlNodePtr orig = node;

	while (node) {
		if (node->type == XML_ENTITY_REF_NODE ||
		    node->type == XML_ENTITY_NODE ||
		    node->type == XML_ENTITY_DECL) {
			return NULL;
		}
		if (node->type == XML_ELEMENT_NODE) {
			cur = node->nsDef;
			while (cur != NULL) {
				if (cur->prefix && cur->href && xmlStrEqual(cur->href, href)) {
					if (xmlSearchNs(doc, node, cur->prefix) == cur) {
						return cur;
					}
				}
				cur = cur->next;
			}
			if (orig != node) {
				cur = node->ns;
				if (cur != NULL) {
					if (cur->prefix && cur->href && xmlStrEqual(cur->href, href)) {
						if (xmlSearchNs(doc, node, cur->prefix) == cur) {
							return cur;
						}
					}
				}
			}
		}
		node = node->parent;
	}
	return NULL;
}

xmlNsPtr encode_add_ns(xmlNodePtr node, const char* ns)
{
	xmlNsPtr xmlns;

	if (ns == NULL) {
	  return NULL;
	}

	xmlns = xmlSearchNsByHref(node->doc, node, BAD_CAST(ns));
	if (xmlns != NULL && xmlns->prefix == NULL) {
		xmlns = xmlSearchNsPrefixByHref(node->doc, node, BAD_CAST(ns));
	}
	if (xmlns == NULL) {
		xmlChar* prefix;

		if ((prefix = zend_hash_str_find_ptr(&SOAP_GLOBAL(defEncNs), (char*)ns, strlen(ns))) != NULL) {
			xmlns = xmlNewNs(node->doc->children, BAD_CAST(ns), prefix);
		} else {
			smart_str prefix = {0};
			int num = ++SOAP_GLOBAL(cur_uniq_ns);
			xmlChar *enc_ns;

			while (1) {
				smart_str_appendl(&prefix, "ns", 2);
				smart_str_append_long(&prefix, num);
				smart_str_0(&prefix);
				if (xmlSearchNs(node->doc, node, BAD_CAST(ZSTR_VAL(prefix.s))) == NULL) {
					break;
				}
				smart_str_free(&prefix);
				prefix.s = NULL;
				num = ++SOAP_GLOBAL(cur_uniq_ns);
			}

			enc_ns = xmlEncodeSpecialChars(node->doc, BAD_CAST(ns));
			xmlns = xmlNewNs(node->doc->children, enc_ns, BAD_CAST(prefix.s ? ZSTR_VAL(prefix.s) : ""));
			xmlFree(enc_ns);
			smart_str_free(&prefix);
		}
	}
	return xmlns;
}

static void set_ns_prop(xmlNodePtr node, char *ns, char *name, char *val)
{
	xmlSetNsProp(node, encode_add_ns(node, ns), BAD_CAST(name), BAD_CAST(val));
}

static void set_xsi_nil(xmlNodePtr node)
{
	set_ns_prop(node, XSI_NAMESPACE, "nil", "true");
}

static void set_xsi_type(xmlNodePtr node, char *type)
{
	set_ns_prop(node, XSI_NAMESPACE, "type", type);
}

void encode_reset_ns(void)
{
	SOAP_GLOBAL(cur_uniq_ns) = 0;
	SOAP_GLOBAL(cur_uniq_ref) = 0;
	if (SOAP_GLOBAL(ref_map)) {
		zend_hash_destroy(SOAP_GLOBAL(ref_map));
	} else {
		SOAP_GLOBAL(ref_map) = emalloc(sizeof(HashTable));
	}
	zend_hash_init(SOAP_GLOBAL(ref_map), 0, NULL, NULL, 0);
}

void encode_finish(void)
{
	SOAP_GLOBAL(cur_uniq_ns) = 0;
	SOAP_GLOBAL(cur_uniq_ref) = 0;
	if (SOAP_GLOBAL(ref_map)) {
		zend_hash_destroy(SOAP_GLOBAL(ref_map));
		efree(SOAP_GLOBAL(ref_map));
		SOAP_GLOBAL(ref_map) = NULL;
	}
}

encodePtr get_conversion(int encode)
{
	encodePtr enc;

	if ((enc = zend_hash_index_find_ptr(&SOAP_GLOBAL(defEncIndex), encode)) == NULL) {
		soap_error0(E_ERROR,  "Encoding: Cannot find encoding");
		return NULL;
	} else {
		return enc;
	}
}

static int is_map(zval *array)
{
	zend_ulong index;
	zend_string *key;
	zend_ulong i = 0;

	if (HT_IS_PACKED(Z_ARRVAL_P(array)) && HT_IS_WITHOUT_HOLES(Z_ARRVAL_P(array))) {
		return FALSE;
	}

	ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(array), index, key) {
		if (key || index != i) {
			return TRUE;
		}
		i++;
	} ZEND_HASH_FOREACH_END();
	return FALSE;
}

static encodePtr get_array_type(xmlNodePtr node, zval *array, smart_str *type)
{
	HashTable *ht;
	int i, cur_type, prev_type, different;
	zval *tmp;
	char *prev_stype = NULL, *cur_stype = NULL, *prev_ns = NULL, *cur_ns = NULL;

	if (!array || Z_TYPE_P(array) != IS_ARRAY) {
		smart_str_appendl(type, "xsd:anyType", sizeof("xsd:anyType")-1);
		return get_conversion(XSD_ANYTYPE);
	}

	i = 0;
	different = FALSE;
	cur_type = prev_type = 0;
	ht = Z_ARRVAL_P(array);

	ZEND_HASH_FOREACH_VAL_IND(ht, tmp) {
		ZVAL_DEREF(tmp);
		if (Z_TYPE_P(tmp) == IS_OBJECT &&
		    Z_OBJCE_P(tmp) == soap_var_class_entry) {
			zval *ztype = Z_VAR_ENC_TYPE_P(tmp);
			if (Z_TYPE_P(ztype) != IS_LONG) {
				soap_error0(E_ERROR,  "Encoding: SoapVar has no 'enc_type' property");
			}
			cur_type = Z_LVAL_P(ztype);

			zval *zstype = Z_VAR_ENC_STYPE_P(tmp);
			if (Z_TYPE_P(zstype) == IS_STRING) {
				cur_stype = Z_STRVAL_P(zstype);
			} else {
				cur_stype = NULL;
			}

			zval *zns = Z_VAR_ENC_NS_P(tmp);
			if (Z_TYPE_P(zns) == IS_STRING) {
				cur_ns = Z_STRVAL_P(zns);
			} else {
				cur_ns = NULL;
			}

		} else if (Z_TYPE_P(tmp) == IS_ARRAY && is_map(tmp)) {
			cur_type = APACHE_MAP;
			cur_stype = NULL;
			cur_ns = NULL;
		} else {
			cur_type = Z_TYPE_P(tmp);
			cur_stype = NULL;
			cur_ns = NULL;
		}

		if (i > 0) {
			if ((cur_type != prev_type) ||
			    (cur_stype != NULL && prev_stype != NULL && strcmp(cur_stype,prev_stype) != 0) ||
			    (cur_stype == NULL && cur_stype != prev_stype) ||
			    (cur_ns != NULL && prev_ns != NULL && strcmp(cur_ns,prev_ns) != 0) ||
			    (cur_ns == NULL && cur_ns != prev_ns)) {
				different = TRUE;
				break;
			}
		}

		prev_type = cur_type;
		prev_stype = cur_stype;
		prev_ns = cur_ns;
		i++;
	} ZEND_HASH_FOREACH_END();

	if (different || i == 0) {
		smart_str_appendl(type, "xsd:anyType", sizeof("xsd:anyType")-1);
		return get_conversion(XSD_ANYTYPE);
	} else {
		encodePtr enc;

		if (cur_stype != NULL) {
			smart_str array_type = {0};

			if (cur_ns) {
				xmlNsPtr ns = encode_add_ns(node, cur_ns);

				smart_str_appends(type, (char*)ns->prefix);
				smart_str_appendc(type, ':');
				smart_str_appends(&array_type, cur_ns);
				smart_str_appendc(&array_type, ':');
			}
			smart_str_appends(type, cur_stype);
			smart_str_0(type);
			smart_str_appends(&array_type, cur_stype);
			smart_str_0(&array_type);

			enc = get_encoder_ex(SOAP_GLOBAL(sdl), ZSTR_VAL(array_type.s), ZSTR_LEN(array_type.s));
			smart_str_free(&array_type);
			return enc;
		} else {
			enc = get_conversion(cur_type);
			get_type_str(node, enc->details.ns, enc->details.type_str, type);
			return enc;
		}
	}
	return NULL;
}

static void get_type_str(xmlNodePtr node, const char* ns, const char* type, smart_str* ret)
{

	if (ns) {
		xmlNsPtr xmlns;
		if (SOAP_GLOBAL(soap_version) == SOAP_1_2 &&
		    strcmp(ns,SOAP_1_1_ENC_NAMESPACE) == 0) {
			ns = SOAP_1_2_ENC_NAMESPACE;
		} else if (SOAP_GLOBAL(soap_version) == SOAP_1_1 &&
		           strcmp(ns,SOAP_1_2_ENC_NAMESPACE) == 0) {
			ns = SOAP_1_1_ENC_NAMESPACE;
		}
		xmlns = encode_add_ns(node, ns);
		smart_str_appends(ret, (char*)xmlns->prefix);
		smart_str_appendc(ret, ':');
	}
	smart_str_appendl(ret, type, strlen(type));
	smart_str_0(ret);
}

static void delete_mapping(void *data)
{
	soapMappingPtr map = (soapMappingPtr)data;

	zval_ptr_dtor(&map->to_xml);
	zval_ptr_dtor(&map->to_zval);
	efree(map);
}

void delete_encoder(zval *zv)
{
	encodePtr t = Z_PTR_P(zv);
	if (t->details.ns) {
		efree(t->details.ns);
	}
	if (t->details.type_str) {
		efree(t->details.type_str);
	}
	if (t->details.map) {
		delete_mapping(t->details.map);
	}
	efree(t);
}

void delete_encoder_persistent(zval *zv)
{
	encodePtr t = Z_PTR_P(zv);
	if (t->details.ns) {
		free(t->details.ns);
	}
	if (t->details.type_str) {
		free(t->details.type_str);
	}
	/* we should never have mapping in persistent encoder */
	assert(t->details.map == NULL);
	free(t);
}
