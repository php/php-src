/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
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
  |          Dmitry Stogov <dmitry@zend.com>                             |
  +----------------------------------------------------------------------+
*/
/* $Id$ */

#include <time.h>

#include "php_soap.h"
#include "ext/libxml/php_libxml.h"
#include "ext/standard/base64.h"
#include <libxml/parserInternals.h>
#include "zend_strtod.h"
#include "zend_interfaces.h"

/* zval type decode */
static zval *to_zval_double(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static zval *to_zval_long(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static zval *to_zval_bool(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static zval *to_zval_string(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static zval *to_zval_stringr(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static zval *to_zval_stringc(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static zval *to_zval_map(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static zval *to_zval_null(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static zval *to_zval_base64(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static zval *to_zval_hexbin(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);

static xmlNodePtr to_xml_long(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_double(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_bool(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);

/* String encode */
static xmlNodePtr to_xml_string(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_base64(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_hexbin(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);

/* Null encode */
static xmlNodePtr to_xml_null(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);

/* Array encode */
static xmlNodePtr guess_array_map(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_map(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);

static xmlNodePtr to_xml_list(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_list1(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent TSRMLS_DC);

/* Datetime encode/decode */
static xmlNodePtr to_xml_datetime_ex(encodeTypePtr type, zval *data, char *format, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_datetime(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_time(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_date(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_gyearmonth(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_gyear(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_gmonthday(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_gday(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_gmonth(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_duration(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);

static zval *to_zval_object(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static zval *to_zval_array(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);

static xmlNodePtr to_xml_object(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);
static xmlNodePtr to_xml_array(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);

static zval *to_zval_any(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static xmlNodePtr to_xml_any(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);

/* Try and guess for non-wsdl clients and servers */
static zval *guess_zval_convert(encodeTypePtr type, xmlNodePtr data TSRMLS_DC);
static xmlNodePtr guess_xml_convert(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC);

static int is_map(zval *array);
static encodePtr get_array_type(xmlNodePtr node, zval *array, smart_str *out_type TSRMLS_DC);

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
			zval *ret; \
			ALLOC_INIT_ZVAL(ret); \
			ZVAL_NULL(ret); \
			return ret; \
		} \
		if (xml->properties) { \
			null = get_attribute(xml->properties, "nil"); \
			if (null) { \
				zval *ret; \
				ALLOC_INIT_ZVAL(ret); \
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

encode defaultEncoding[] = {
	{{UNKNOWN_TYPE, NULL, NULL, NULL}, guess_zval_convert, guess_xml_convert},

	{{IS_NULL, "nil", XSI_NAMESPACE, NULL}, to_zval_null, to_xml_null},
	{{IS_STRING, XSD_STRING_STRING, XSD_NAMESPACE, NULL}, to_zval_string, to_xml_string},
	{{IS_LONG, XSD_INT_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{IS_DOUBLE, XSD_FLOAT_STRING, XSD_NAMESPACE, NULL}, to_zval_double, to_xml_double},
	{{IS_BOOL, XSD_BOOLEAN_STRING, XSD_NAMESPACE, NULL}, to_zval_bool, to_xml_bool},
	{{IS_CONSTANT, XSD_STRING_STRING, XSD_NAMESPACE, NULL}, to_zval_string, to_xml_string},
	{{IS_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_1_ENC_NAMESPACE, NULL}, to_zval_array, guess_array_map},
	{{IS_OBJECT, SOAP_ENC_OBJECT_STRING, SOAP_1_1_ENC_NAMESPACE, NULL}, to_zval_object, to_xml_object},
	{{IS_ARRAY, SOAP_ENC_ARRAY_STRING, SOAP_1_2_ENC_NAMESPACE, NULL}, to_zval_array, guess_array_map},
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

	{{XSD_HEXBINARY, XSD_HEXBINARY_STRING, XSD_NAMESPACE, NULL}, to_zval_hexbin, to_xml_hexbin},
	{{XSD_BASE64BINARY, XSD_BASE64BINARY_STRING, XSD_NAMESPACE, NULL}, to_zval_base64, to_xml_base64},

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
	{{XSD_UNSIGNEDLONG, XSD_UNSIGNEDLONG_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},
	{{XSD_INTEGER, XSD_INTEGER_STRING, XSD_NAMESPACE, NULL}, to_zval_long, to_xml_long},

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

	{{XSD_ANYXML, "<anyXML>", "<anyXML>", NULL}, to_zval_any, to_xml_any},

	{{END_KNOWN_TYPES, NULL, NULL, NULL}, guess_zval_convert, guess_xml_convert}
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
		HashPosition pos;
		encodePtr *enc;

		for (zend_hash_internal_pointer_reset_ex(sdl->encoders, &pos);
		     zend_hash_get_current_data_ex(sdl->encoders, (void **) &enc, &pos) == SUCCESS;
		     zend_hash_move_forward_ex(sdl->encoders, &pos)) {
		    if (strcmp((*enc)->details.type_str, type) == 0) {
				return *enc;
			}
		}
	}
	return NULL;
}

static zend_bool soap_check_zval_ref(zval *data, xmlNodePtr node TSRMLS_DC) {
	xmlNodePtr *node_ptr;

	if (SOAP_GLOBAL(ref_map)) {
		if (Z_TYPE_P(data) == IS_OBJECT) {
			data = (zval*)zend_objects_get_address(data TSRMLS_CC);
		}
		if (zend_hash_index_find(SOAP_GLOBAL(ref_map), (ulong)data, (void**)&node_ptr) == SUCCESS) {
			xmlAttrPtr attr = (*node_ptr)->properties;
			char *id;
			smart_str prefix = {0};

			if (*node_ptr == node) {
				return 0;
			}
			xmlNodeSetName(node, (*node_ptr)->name);
			xmlSetNs(node, (*node_ptr)->ns);
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
					id = prefix.c;
				} else {
					SOAP_GLOBAL(cur_uniq_ref)++;
					smart_str_appendl(&prefix, "#ref", 4);
					smart_str_append_long(&prefix, SOAP_GLOBAL(cur_uniq_ref));
					smart_str_0(&prefix);
					id = prefix.c;
					xmlSetProp((*node_ptr), BAD_CAST("id"), BAD_CAST(id+1));
				}
				xmlSetProp(node, BAD_CAST("href"), BAD_CAST(id));
			} else {
				attr = get_attribute_ex(attr, "id", SOAP_1_2_ENC_NAMESPACE);
				if (attr) {
					id = (char*)attr->children->content;
					smart_str_appendc(&prefix, '#');
					smart_str_appends(&prefix, id);
					smart_str_0(&prefix);
					id = prefix.c;
				} else {
					SOAP_GLOBAL(cur_uniq_ref)++;
					smart_str_appendl(&prefix, "#ref", 4);
					smart_str_append_long(&prefix, SOAP_GLOBAL(cur_uniq_ref));
					smart_str_0(&prefix);
					id = prefix.c;
					set_ns_prop((*node_ptr), SOAP_1_2_ENC_NAMESPACE, "id", id+1);
				}
				set_ns_prop(node, SOAP_1_2_ENC_NAMESPACE, "ref", id);
			}
			smart_str_free(&prefix);
			return 1;
		} else {
			zend_hash_index_update(SOAP_GLOBAL(ref_map), (ulong)data, (void**)&node, sizeof(xmlNodePtr), NULL);
		}
	}
	return 0;
}

static zval* soap_find_xml_ref(xmlNodePtr node TSRMLS_DC)
{
	zval **data_ptr;

	if (SOAP_GLOBAL(ref_map) && 
	    zend_hash_index_find(SOAP_GLOBAL(ref_map), (ulong)node, (void**)&data_ptr) == SUCCESS) {
		Z_SET_ISREF_PP(data_ptr);
		Z_ADDREF_PP(data_ptr);
		return *data_ptr;
	}
	return NULL;
}

static zend_bool soap_check_xml_ref(zval **data, xmlNodePtr node TSRMLS_DC)
{
	zval **data_ptr;

	if (SOAP_GLOBAL(ref_map)) {
		if (zend_hash_index_find(SOAP_GLOBAL(ref_map), (ulong)node, (void**)&data_ptr) == SUCCESS) {
			if (*data != *data_ptr) {
				zval_ptr_dtor(data);
				*data = *data_ptr;
				Z_SET_ISREF_PP(data);
				Z_ADDREF_PP(data);
				return 1;
			}
		} else {
			zend_hash_index_update(SOAP_GLOBAL(ref_map), (ulong)node, (void**)data, sizeof(zval*), NULL);
		}
	}
	return 0;
}

static xmlNodePtr master_to_xml_int(encodePtr encode, zval *data, int style, xmlNodePtr parent, int check_class_map TSRMLS_DC)
{
	xmlNodePtr node = NULL;
	int add_type = 0;

	/* Special handling of class SoapVar */
	if (data &&
	    Z_TYPE_P(data) == IS_OBJECT &&
	    Z_OBJCE_P(data) == soap_var_class_entry) {
		zval **ztype, **zdata, **zns, **zstype, **zname, **znamens;
		encodePtr enc = NULL;
		HashTable *ht = Z_OBJPROP_P(data);

		if (zend_hash_find(ht, "enc_type", sizeof("enc_type"), (void **)&ztype) == FAILURE ||
		    Z_TYPE_PP(ztype) != IS_LONG) {
			soap_error0(E_ERROR, "Encoding: SoapVar has no 'enc_type' property");
		}

		if (zend_hash_find(ht, "enc_stype", sizeof("enc_stype"), (void **)&zstype) == SUCCESS &&
		    Z_TYPE_PP(zstype) == IS_STRING) {
			if (zend_hash_find(ht, "enc_ns", sizeof("enc_ns"), (void **)&zns) == SUCCESS &&
			    Z_TYPE_PP(zns) == IS_STRING) {
				enc = get_encoder(SOAP_GLOBAL(sdl), Z_STRVAL_PP(zns), Z_STRVAL_PP(zstype));
			} else {
				zns = NULL;
				enc = get_encoder_ex(SOAP_GLOBAL(sdl), Z_STRVAL_PP(zstype), Z_STRLEN_PP(zstype));
			}
			if (enc == NULL && SOAP_GLOBAL(typemap)) {
				encodePtr *new_enc;
				smart_str nscat = {0};

				if (zns != NULL) {
					smart_str_appendl(&nscat, Z_STRVAL_PP(zns), Z_STRLEN_PP(zns));
					smart_str_appendc(&nscat, ':');
				}
				smart_str_appendl(&nscat, Z_STRVAL_PP(zstype), Z_STRLEN_PP(zstype));
				smart_str_0(&nscat);
				if (zend_hash_find(SOAP_GLOBAL(typemap), nscat.c, nscat.len + 1, (void**)&new_enc) == SUCCESS) {
					enc = *new_enc;
				}
				smart_str_free(&nscat);			
			}
		}
		if (enc == NULL) {
			enc = get_conversion(Z_LVAL_P(*ztype));
		}
		if (enc == NULL) {
			enc = encode;
		}

		if (zend_hash_find(ht, "enc_value", sizeof("enc_value"), (void **)&zdata) == FAILURE) {
			node = master_to_xml(enc, NULL, style, parent TSRMLS_CC);
		} else {
			node = master_to_xml(enc, *zdata, style, parent TSRMLS_CC);
		}

		if (style == SOAP_ENCODED || (SOAP_GLOBAL(sdl) && encode != enc)) {
			if (zend_hash_find(ht, "enc_stype", sizeof("enc_stype"), (void **)&zstype) == SUCCESS &&
			    Z_TYPE_PP(zstype) == IS_STRING) {
				if (zend_hash_find(ht, "enc_ns", sizeof("enc_ns"), (void **)&zns) == SUCCESS &&
				    Z_TYPE_PP(zns) == IS_STRING) {
					set_ns_and_type_ex(node, Z_STRVAL_PP(zns), Z_STRVAL_PP(zstype));
				} else {
					set_ns_and_type_ex(node, NULL, Z_STRVAL_PP(zstype));
				}
			}
		}

		if (zend_hash_find(ht, "enc_name", sizeof("enc_name"), (void **)&zname) == SUCCESS &&
		    Z_TYPE_PP(zname) == IS_STRING) {
			xmlNodeSetName(node, BAD_CAST(Z_STRVAL_PP(zname)));
		}
		if (zend_hash_find(ht, "enc_namens", sizeof("enc_namens"), (void **)&znamens) == SUCCESS &&
		    Z_TYPE_PP(znamens) == IS_STRING) {
			xmlNsPtr nsp = encode_add_ns(node, Z_STRVAL_PP(znamens));
			xmlSetNs(node, nsp);
		}
	} else {
		if (check_class_map && SOAP_GLOBAL(class_map) && data &&
		    Z_TYPE_P(data) == IS_OBJECT &&
		    !Z_OBJPROP_P(data)->nApplyCount) {
			zend_class_entry *ce = Z_OBJCE_P(data);
			HashPosition pos;
			zval **tmp;
			char *type_name = NULL;
			uint type_len;
			ulong idx;

			for (zend_hash_internal_pointer_reset_ex(SOAP_GLOBAL(class_map), &pos);
			     zend_hash_get_current_data_ex(SOAP_GLOBAL(class_map), (void **) &tmp, &pos) == SUCCESS;
			     zend_hash_move_forward_ex(SOAP_GLOBAL(class_map), &pos)) {
				if (Z_TYPE_PP(tmp) == IS_STRING &&
				    ce->name_length == Z_STRLEN_PP(tmp) &&
				    zend_binary_strncasecmp(ce->name, ce->name_length, Z_STRVAL_PP(tmp), ce->name_length, ce->name_length) == 0 &&
				    zend_hash_get_current_key_ex(SOAP_GLOBAL(class_map), &type_name, &type_len, &idx, 0, &pos) == HASH_KEY_IS_STRING) {

					/* TODO: namespace isn't stored */
					encodePtr enc = NULL;
					if (SOAP_GLOBAL(sdl)) {
						enc = get_encoder(SOAP_GLOBAL(sdl), SOAP_GLOBAL(sdl)->target_ns, type_name);
						if (!enc) {
							enc = find_encoder_by_type_name(SOAP_GLOBAL(sdl), type_name);
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
			}
		}

		if (encode == NULL) {
			encode = get_conversion(UNKNOWN_TYPE);
		}
		if (SOAP_GLOBAL(typemap) && encode->details.type_str) {
			smart_str nscat = {0};
			encodePtr *new_enc;

			if (encode->details.ns) {
				smart_str_appends(&nscat, encode->details.ns);
				smart_str_appendc(&nscat, ':');
			}
			smart_str_appends(&nscat, encode->details.type_str);
			smart_str_0(&nscat);
			if (zend_hash_find(SOAP_GLOBAL(typemap), nscat.c, nscat.len + 1, (void**)&new_enc) == SUCCESS) {
				encode = *new_enc;
			}
			smart_str_free(&nscat);			
		}
		if (encode->to_xml) {
			node = encode->to_xml(&encode->details, data, style, parent TSRMLS_CC);
			if (add_type) {
				set_ns_and_type(node, &encode->details);
			}
		}
	}
	return node;
}

xmlNodePtr master_to_xml(encodePtr encode, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return master_to_xml_int(encode, data, style, parent, 1 TSRMLS_CC);
}

static zval *master_to_zval_int(encodePtr encode, xmlNodePtr data TSRMLS_DC)
{
	zval *ret = NULL;

	if (SOAP_GLOBAL(typemap)) {
		if (encode->details.type_str) {
			smart_str nscat = {0};
			encodePtr *new_enc;

			if (encode->details.ns) {
				smart_str_appends(&nscat, encode->details.ns);
				smart_str_appendc(&nscat, ':');
			}
			smart_str_appends(&nscat, encode->details.type_str);
			smart_str_0(&nscat);
			if (zend_hash_find(SOAP_GLOBAL(typemap), nscat.c, nscat.len + 1, (void**)&new_enc) == SUCCESS) {
				encode = *new_enc;
			}
			smart_str_free(&nscat);			
		} else {
			xmlAttrPtr type_attr = get_attribute_ex(data->properties,"type", XSI_NAMESPACE);

			if (type_attr != NULL) {
				encodePtr *new_enc;
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
				if (zend_hash_find(SOAP_GLOBAL(typemap), nscat.c, nscat.len + 1, (void**)&new_enc) == SUCCESS) {
					encode = *new_enc;
				}
				smart_str_free(&nscat);			
			}
		}
	}
	if (encode->to_zval) {
		ret = encode->to_zval(&encode->details, data TSRMLS_CC);
	}
	return ret;
}

zval *master_to_zval(encodePtr encode, xmlNodePtr data TSRMLS_DC)
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
	return master_to_zval_int(encode, data TSRMLS_CC);
}

xmlNodePtr to_xml_user(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret = NULL;
	zval *return_value;

	if (type && type->map && type->map->to_xml) {
		MAKE_STD_ZVAL(return_value);

		if (call_user_function(EG(function_table), NULL, type->map->to_xml, return_value, 1, &data TSRMLS_CC) == FAILURE) {
			soap_error0(E_ERROR, "Encoding: Error calling to_xml callback");
		}
		if (Z_TYPE_P(return_value) == IS_STRING) {		
			xmlDocPtr doc = soap_xmlParseMemory(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value));
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

zval *to_zval_user(encodeTypePtr type, xmlNodePtr node TSRMLS_DC)
{
	zval *return_value;

	if (type && type->map && type->map->to_zval) {
		xmlBufferPtr buf;
		zval *data;
		xmlNodePtr copy;

		copy = xmlCopyNode(node, 1);
		buf = xmlBufferCreate();
		xmlNodeDump(buf, NULL, copy, 0, 0);
		MAKE_STD_ZVAL(data);
		ZVAL_STRING(data, (char*)xmlBufferContent(buf), 1);
		xmlBufferFree(buf);
		xmlFreeNode(copy);

		ALLOC_INIT_ZVAL(return_value);
		
		if (call_user_function(EG(function_table), NULL, type->map->to_zval, return_value, 1, &data TSRMLS_CC) == FAILURE) {
			soap_error0(E_ERROR, "Encoding: Error calling from_xml callback");
		}
		zval_ptr_dtor(&data);
	} else {
		ALLOC_INIT_ZVAL(return_value);
	}
	return return_value;
}

/* TODO: get rid of "bogus".. ither by passing in the already created xmlnode or passing in the node name */
/* String encode/decode */
static zval *to_zval_string(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			if (SOAP_GLOBAL(encoding) != NULL) {
				xmlBufferPtr in  = xmlBufferCreateStatic(data->children->content, xmlStrlen(data->children->content));
				xmlBufferPtr out = xmlBufferCreate();
				int n = xmlCharEncOutFunc(SOAP_GLOBAL(encoding), out, in);

				if (n >= 0) {
					ZVAL_STRING(ret, (char*)xmlBufferContent(out), 1);
				} else {
					ZVAL_STRING(ret, (char*)data->children->content, 1);
				}
				xmlBufferFree(out);
				xmlBufferFree(in);
			} else {
				ZVAL_STRING(ret, (char*)data->children->content, 1);
			}
		} else if (data->children->type == XML_CDATA_SECTION_NODE && data->children->next == NULL) {
			ZVAL_STRING(ret, (char*)data->children->content, 1);
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static zval *to_zval_stringr(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_replace(data->children->content);
			if (SOAP_GLOBAL(encoding) != NULL) {
				xmlBufferPtr in  = xmlBufferCreateStatic(data->children->content, xmlStrlen(data->children->content));
				xmlBufferPtr out = xmlBufferCreate();
				int n = xmlCharEncOutFunc(SOAP_GLOBAL(encoding), out, in);

				if (n >= 0) {
					ZVAL_STRING(ret, (char*)xmlBufferContent(out), 1);
				} else {
					ZVAL_STRING(ret, (char*)data->children->content, 1);
				}
				xmlBufferFree(out);
				xmlBufferFree(in);
			} else {
				ZVAL_STRING(ret, (char*)data->children->content, 1);
			}
		} else if (data->children->type == XML_CDATA_SECTION_NODE && data->children->next == NULL) {
			ZVAL_STRING(ret, (char*)data->children->content, 1);
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static zval *to_zval_stringc(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			if (SOAP_GLOBAL(encoding) != NULL) {
				xmlBufferPtr in  = xmlBufferCreateStatic(data->children->content, xmlStrlen(data->children->content));
				xmlBufferPtr out = xmlBufferCreate();
				int n = xmlCharEncOutFunc(SOAP_GLOBAL(encoding), out, in);

				if (n >= 0) {
					ZVAL_STRING(ret, (char*)xmlBufferContent(out), 1);
				} else {
					ZVAL_STRING(ret, (char*)data->children->content, 1);
				}
				xmlBufferFree(out);
				xmlBufferFree(in);
			} else {
				ZVAL_STRING(ret, (char*)data->children->content, 1);
			}
		} else if (data->children->type == XML_CDATA_SECTION_NODE && data->children->next == NULL) {
			ZVAL_STRING(ret, (char*)data->children->content, 1);
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static zval *to_zval_base64(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	char *str;
	int str_len;

	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			str = (char*)php_base64_decode(data->children->content, strlen((char*)data->children->content), &str_len);
			if (!str) {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			ZVAL_STRINGL(ret, str, str_len, 0);
		} else if (data->children->type == XML_CDATA_SECTION_NODE && data->children->next == NULL) {
			str = (char*)php_base64_decode(data->children->content, strlen((char*)data->children->content), &str_len);
			if (!str) {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			ZVAL_STRINGL(ret, str, str_len, 0);
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static zval *to_zval_hexbin(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	unsigned char *str;
	int str_len, i, j;
	unsigned char c;

	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
		} else if (data->children->type != XML_CDATA_SECTION_NODE || data->children->next != NULL) {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			return ret;
		}
		str_len = strlen((char*)data->children->content) / 2;
		str = emalloc(str_len+1);
		for (i = j = 0; i < str_len; i++) {
			c = data->children->content[j++];
			if (c >= '0' && c <= '9') {
				str[i] = (c - '0') << 4;
			} else if (c >= 'a' && c <= 'f') {
				str[i] = (c - 'a' + 10) << 4;
			} else if (c >= 'A' && c <= 'F') {
				str[i] = (c - 'A' + 10) << 4;
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			c = data->children->content[j++];
			if (c >= '0' && c <= '9') {
				str[i] |= c - '0';
			} else if (c >= 'a' && c <= 'f') {
				str[i] |= c - 'a' + 10;
			} else if (c >= 'A' && c <= 'F') {
				str[i] |= c - 'A' + 10;
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
		}
		str[str_len] = '\0';
		ZVAL_STRINGL(ret, (char*)str, str_len, 0);
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}

static xmlNodePtr to_xml_string(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
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
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		str = estrndup(Z_STRVAL(tmp), Z_STRLEN(tmp));
		new_len = Z_STRLEN(tmp);
		zval_dtor(&tmp);
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

static xmlNodePtr to_xml_base64(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret, text;
	unsigned char *str;
	int str_len;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) == IS_STRING) {
		str = php_base64_encode((unsigned char*)Z_STRVAL_P(data), Z_STRLEN_P(data), &str_len);
		text = xmlNewTextLen(str, str_len);
		xmlAddChild(ret, text);
		efree(str);
	} else {
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		str = php_base64_encode((unsigned char*)Z_STRVAL(tmp), Z_STRLEN(tmp), &str_len);
		text = xmlNewTextLen(str, str_len);
		xmlAddChild(ret, text);
		efree(str);
		zval_dtor(&tmp);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static xmlNodePtr to_xml_hexbin(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	static char hexconvtab[] = "0123456789ABCDEF";
	xmlNodePtr ret, text;
	unsigned char *str;
	zval tmp;
	int i, j;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) != IS_STRING) {
		tmp = *data;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
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
		zval_dtor(&tmp);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static zval *to_zval_double(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			long lval;
			double dval;

			whiteSpace_collapse(data->children->content);
			switch (is_numeric_string((char*)data->children->content, strlen((char*)data->children->content), &lval, &dval, 0)) {
				case IS_LONG:
					Z_TYPE_P(ret) = IS_DOUBLE;
					Z_DVAL_P(ret) = lval;
					break;
				case IS_DOUBLE:
					Z_TYPE_P(ret) = IS_DOUBLE;
					Z_DVAL_P(ret) = dval;
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

static zval *to_zval_long(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			long lval;
			double dval;

			whiteSpace_collapse(data->children->content);
			errno = 0;

			switch ((Z_TYPE_P(ret) = is_numeric_string((char*)data->children->content, strlen((char*)data->children->content), &lval, &dval, 0))) {
				case IS_LONG:
					Z_LVAL_P(ret) = lval;
					break;
				case IS_DOUBLE:
					Z_DVAL_P(ret) = dval;
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

static xmlNodePtr to_xml_long(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
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
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		if (Z_TYPE(tmp) != IS_LONG) {
			convert_to_long(&tmp);
		}
		convert_to_string(&tmp);
		xmlNodeSetContentLen(ret, BAD_CAST(Z_STRVAL(tmp)), Z_STRLEN(tmp));
		zval_dtor(&tmp);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static xmlNodePtr to_xml_double(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret;
	zval tmp;
	char *str;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	tmp = *data;
	if (Z_TYPE(tmp) != IS_DOUBLE) {
		zval_copy_ctor(&tmp);
		convert_to_double(&tmp);
	}
	
	str = (char *) safe_emalloc(EG(precision), 1, MAX_LENGTH_OF_DOUBLE + 1);
	php_gcvt(Z_DVAL(tmp), EG(precision), '.', 'E', str);
	xmlNodeSetContentLen(ret, BAD_CAST(str), strlen(str));
	efree(str);

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}

static zval *to_zval_bool(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			if (stricmp((char*)data->children->content, "true") == 0 ||
				stricmp((char*)data->children->content, "t") == 0 ||
				strcmp((char*)data->children->content, "1") == 0) {
				ZVAL_BOOL(ret, 1);
			} else if (stricmp((char*)data->children->content, "false") == 0 ||
				stricmp((char*)data->children->content, "f") == 0 ||
				strcmp((char*)data->children->content, "0") == 0) {
				ZVAL_BOOL(ret, 0);
			} else {
				ZVAL_STRING(ret, (char*)data->children->content, 1);
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

static xmlNodePtr to_xml_bool(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
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
static zval *to_zval_null(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	ZVAL_NULL(ret);
	return ret;
}

static xmlNodePtr to_xml_null(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	if (style == SOAP_ENCODED) {
		set_xsi_nil(ret);
	}
	return ret;
}

static void set_zval_property(zval* object, char* name, zval* val TSRMLS_DC)
{
	zend_class_entry *old_scope;

	old_scope = EG(scope);
	EG(scope) = Z_OBJCE_P(object);
	Z_DELREF_P(val);
	add_property_zval(object, name, val);
	EG(scope) = old_scope;
}

static zval* get_zval_property(zval* object, char* name TSRMLS_DC)
{
	if (Z_TYPE_P(object) == IS_OBJECT) {
		zval member;
		zval *data;
		zend_class_entry *old_scope;

		INIT_PZVAL(&member);
		ZVAL_STRING(&member, name, 0);
		old_scope = EG(scope);
		EG(scope) = Z_OBJCE_P(object);
		data = Z_OBJ_HT_P(object)->read_property(object, &member, BP_VAR_IS, 0 TSRMLS_CC);
		if (data == EG(uninitialized_zval_ptr)) {
			/* Hack for bug #32455 */
			zend_property_info *property_info;

			property_info = zend_get_property_info(Z_OBJCE_P(object), &member, 1 TSRMLS_CC);
			EG(scope) = old_scope;
			if (property_info && zend_hash_quick_exists(Z_OBJPROP_P(object), property_info->name, property_info->name_length+1, property_info->h)) {
				return data;
			}
			return NULL;
		}
		EG(scope) = old_scope;
		return data;
	} else if (Z_TYPE_P(object) == IS_ARRAY) {
		zval **data_ptr;

		if (zend_hash_find(Z_ARRVAL_P(object), name, strlen(name)+1, (void**)&data_ptr) == SUCCESS) {
		  return *data_ptr;
		}
	}
  return NULL;
}

static void unset_zval_property(zval* object, char* name TSRMLS_DC)
{
	if (Z_TYPE_P(object) == IS_OBJECT) {
		zval member;
		zend_class_entry *old_scope;

		INIT_PZVAL(&member);
		ZVAL_STRING(&member, name, 0);
		old_scope = EG(scope);
		EG(scope) = Z_OBJCE_P(object);
		Z_OBJ_HT_P(object)->unset_property(object, &member, 0 TSRMLS_CC);
		EG(scope) = old_scope;
	} else if (Z_TYPE_P(object) == IS_ARRAY) {
		zend_hash_del(Z_ARRVAL_P(object), name, strlen(name)+1);
	}
}

static void model_to_zval_any(zval *ret, xmlNodePtr node TSRMLS_DC)
{
	zval* any = NULL;
	char* name = NULL;

	while (node != NULL) {
		if (get_zval_property(ret, (char*)node->name TSRMLS_CC) == NULL) {
			zval* val = master_to_zval(get_conversion(XSD_ANYXML), node TSRMLS_CC);
			
			if (any && Z_TYPE_P(any) != IS_ARRAY) {
				/* Convert into array */
				zval *arr;

				MAKE_STD_ZVAL(arr);
				array_init(arr);
				if (name) {
					add_assoc_zval(arr, name, any);
				} else {
					add_next_index_zval(arr, any);
				}
				any = arr;
			}

			if (Z_TYPE_P(val) == IS_STRING && *Z_STRVAL_P(val) == '<') {
				name = NULL;
				while (node->next != NULL) {
					zval* val2 = master_to_zval(get_conversion(XSD_ANYXML), node->next TSRMLS_CC);
					if (Z_TYPE_P(val2) != IS_STRING ||  *Z_STRVAL_P(val) != '<') {
						break;
					}
					add_string_to_string(val, val, val2);
					zval_ptr_dtor(&val2);
					node = node->next;
				}
			} else {
				name = (char*)node->name;
			}

			if (any == NULL) {
				if (name) {
					/* Convert into array */
					zval *arr;

					MAKE_STD_ZVAL(arr);
					array_init(arr);
					add_assoc_zval(arr, name, val);
					any = arr;
					name = NULL;
				} else {
					any = val;
				}
			} else {
				/* Add array element */
				if (name) {
					zval **el;
					if (zend_hash_find(Z_ARRVAL_P(any), name, strlen(name)+1, (void**)&el) == SUCCESS) {
						if (Z_TYPE_PP(el) != IS_ARRAY) {
							/* Convert into array */
							zval *arr;
	
							MAKE_STD_ZVAL(arr);
							array_init(arr);
							add_next_index_zval(arr, *el);
							*el = arr;
						}
						add_next_index_zval(*el, val);
					} else {
						add_assoc_zval(any, name, val);
					}
				} else {
					add_next_index_zval(any, val);
				}
				name = NULL;
			}
		}
		node = node->next;
	}
	if (any) {
		set_zval_property(ret, name ? name : "any", any TSRMLS_CC);
	}
}

static void model_to_zval_object(zval *ret, sdlContentModelPtr model, xmlNodePtr data, sdlPtr sdl TSRMLS_DC)
{
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT:
			if (model->u.element->name) {
				xmlNodePtr node = get_node(data->children, model->u.element->name);

				if (node) {
					zval *val;
					xmlNodePtr r_node;

					r_node = check_and_resolve_href(node);
					if (r_node && r_node->children && r_node->children->content) {
						if (model->u.element->fixed && strcmp(model->u.element->fixed, (char*)r_node->children->content) != 0) {
							soap_error3(E_ERROR, "Encoding: Element '%s' has fixed value '%s' (value '%s' is not allowed)", model->u.element->name, model->u.element->fixed, r_node->children->content);
						}
						val = master_to_zval(model->u.element->encode, r_node TSRMLS_CC);
					} else if (model->u.element->fixed) {
						xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlNodeSetContent(dummy, BAD_CAST(model->u.element->fixed));
						val = master_to_zval(model->u.element->encode, dummy TSRMLS_CC);
						xmlFreeNode(dummy);
					} else if (model->u.element->def && !model->u.element->nillable) {
						xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlNodeSetContent(dummy, BAD_CAST(model->u.element->def));
						val = master_to_zval(model->u.element->encode, dummy TSRMLS_CC);
						xmlFreeNode(dummy);
					} else {
						val = master_to_zval(model->u.element->encode, r_node TSRMLS_CC);
					}
					if ((node = get_node(node->next, model->u.element->name)) != NULL) {
						zval *array;

						MAKE_STD_ZVAL(array);
						array_init(array);
						add_next_index_zval(array, val);
						do {
							if (node && node->children && node->children->content) {
								if (model->u.element->fixed && strcmp(model->u.element->fixed, (char*)node->children->content) != 0) {
									soap_error3(E_ERROR, "Encoding: Element '%s' has fixed value '%s' (value '%s' is not allowed)", model->u.element->name, model->u.element->fixed, node->children->content);
								}
								val = master_to_zval(model->u.element->encode, node TSRMLS_CC);
							} else if (model->u.element->fixed) {
								xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
								xmlNodeSetContent(dummy, BAD_CAST(model->u.element->fixed));
								val = master_to_zval(model->u.element->encode, dummy TSRMLS_CC);
								xmlFreeNode(dummy);
							} else if (model->u.element->def && !model->u.element->nillable) {
								xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
								xmlNodeSetContent(dummy, BAD_CAST(model->u.element->def));
								val = master_to_zval(model->u.element->encode, dummy TSRMLS_CC);
								xmlFreeNode(dummy);
							} else {
								val = master_to_zval(model->u.element->encode, node TSRMLS_CC);
							}
							add_next_index_zval(array, val);
						} while ((node = get_node(node->next, model->u.element->name)) != NULL);
						val = array;
					} else if ((Z_TYPE_P(val) != IS_NULL || !model->u.element->nillable) &&
					           (SOAP_GLOBAL(features) & SOAP_SINGLE_ELEMENT_ARRAYS) &&
					           (model->max_occurs == -1 || model->max_occurs > 1)) {
						zval *array;

						MAKE_STD_ZVAL(array);
						array_init(array);
						add_next_index_zval(array, val);
						val = array;
					}
					set_zval_property(ret, model->u.element->name, val TSRMLS_CC);
				}
			}
			break;
		case XSD_CONTENT_ALL:
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr *tmp;
			HashPosition pos;
			sdlContentModelPtr any = NULL;

			zend_hash_internal_pointer_reset_ex(model->u.content, &pos);
			while (zend_hash_get_current_data_ex(model->u.content, (void**)&tmp, &pos) == SUCCESS) {
				if ((*tmp)->kind == XSD_CONTENT_ANY) {
					any = *tmp;
				} else {
					model_to_zval_object(ret, *tmp, data, sdl TSRMLS_CC);
				}
				zend_hash_move_forward_ex(model->u.content, &pos);
			}
			if (any) {
				model_to_zval_any(ret, data->children TSRMLS_CC);
			}
			break;
		}
		case XSD_CONTENT_GROUP:
			model_to_zval_object(ret, model->u.group->model, data, sdl TSRMLS_CC);
			break;
		default:
		  break;
	}
}

/* Struct encode/decode */
static zval *to_zval_object_ex(encodeTypePtr type, xmlNodePtr data, zend_class_entry *pce TSRMLS_DC)
{
	zval *ret;
	xmlNodePtr trav;
	sdlPtr sdl;
	sdlTypePtr sdlType = type->sdl_type;
	zend_class_entry *ce = ZEND_STANDARD_CLASS_DEF_PTR;
	zval *redo_any = NULL;

	if (pce) {
		ce = pce;
	} else if (SOAP_GLOBAL(class_map) && type->type_str) {
		zval             **classname;
		zend_class_entry  *tmp;

		if (zend_hash_find(SOAP_GLOBAL(class_map), type->type_str, strlen(type->type_str)+1, (void**)&classname) == SUCCESS &&
		    Z_TYPE_PP(classname) == IS_STRING &&
		    (tmp = zend_fetch_class(Z_STRVAL_PP(classname), Z_STRLEN_PP(classname), ZEND_FETCH_CLASS_AUTO TSRMLS_CC)) != NULL) {
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
				zval *base;

				ALLOC_INIT_ZVAL(ret);
				if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
					return ret;
				}

				object_init_ex(ret, ce);
				base = master_to_zval_int(enc, data TSRMLS_CC);
				set_zval_property(ret, "_", base TSRMLS_CC);
			} else {
				ALLOC_INIT_ZVAL(ret);
				FIND_XML_NULL(data, ret);
				if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
					return ret;
				}
				object_init_ex(ret, ce);
			}
		} else if (sdlType->kind == XSD_TYPEKIND_EXTENSION &&
		           sdlType->encode &&
		           type != &sdlType->encode->details) {
			if (sdlType->encode->details.sdl_type &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_UNION) {

				CHECK_XML_NULL(data);
				if ((ret = soap_find_xml_ref(data TSRMLS_CC)) != NULL) {
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
					ret = to_zval_object_ex(&sdlType->encode->details, data, ce TSRMLS_CC);
			    } else {
					ret = master_to_zval_int(sdlType->encode, data TSRMLS_CC);
				}
				if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
					return ret;
				}
				redo_any = get_zval_property(ret, "any" TSRMLS_CC);
				if (Z_TYPE_P(ret) == IS_OBJECT && ce != ZEND_STANDARD_CLASS_DEF_PTR) {
					zend_object *zobj = zend_objects_get_address(ret TSRMLS_CC);
					zobj->ce = ce;
				}
			} else {
				zval *base;

				ALLOC_INIT_ZVAL(ret);
				if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
					return ret;
				}

				object_init_ex(ret, ce);
				base = master_to_zval_int(sdlType->encode, data TSRMLS_CC);
				set_zval_property(ret, "_", base TSRMLS_CC);
			}
		} else {
			ALLOC_INIT_ZVAL(ret);
			FIND_XML_NULL(data, ret);
			if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
				return ret;
			}
			object_init_ex(ret, ce);
		}
		if (sdlType->model) {
			if (redo_any) {
				Z_ADDREF_P(redo_any);
				unset_zval_property(ret, "any" TSRMLS_CC);
			}
			model_to_zval_object(ret, sdlType->model, data, sdl TSRMLS_CC);
			if (redo_any) {
				zval *tmp = get_zval_property(ret, "any" TSRMLS_CC);

				if (tmp == NULL) {
					model_to_zval_any(ret, data->children TSRMLS_CC);
				} else if (Z_REFCOUNT_P(tmp) == 0) {
					zval_dtor(tmp);
					efree(tmp);
				}
				zval_ptr_dtor(&redo_any);
			}
		}
		if (sdlType->attributes) {
			sdlAttributePtr *attr;
			HashPosition pos;

			zend_hash_internal_pointer_reset_ex(sdlType->attributes, &pos);
			while (zend_hash_get_current_data_ex(sdlType->attributes, (void**)&attr, &pos) == SUCCESS) {
				if ((*attr)->name) {
					xmlAttrPtr val = get_attribute(data->properties, (*attr)->name);
					char *str_val = NULL;

					if (val && val->children && val->children->content) {
						str_val = (char*)val->children->content;
						if ((*attr)->fixed && strcmp((*attr)->fixed, str_val) != 0) {
							soap_error3(E_ERROR, "Encoding: Attribute '%s' has fixed value '%s' (value '%s' is not allowed)", (*attr)->name, (*attr)->fixed, str_val);
						}
					} else if ((*attr)->fixed) {
						str_val = (*attr)->fixed;
					} else if ((*attr)->def) {
						str_val = (*attr)->def;
					}
					if (str_val) {
						xmlNodePtr dummy, text;
						zval *data;

						dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						text = xmlNewText(BAD_CAST(str_val));
						xmlAddChild(dummy, text);
						data = master_to_zval((*attr)->encode, dummy TSRMLS_CC);
						xmlFreeNode(dummy);
						set_zval_property(ret, (*attr)->name, data TSRMLS_CC);
					}
				}
				zend_hash_move_forward_ex(sdlType->attributes, &pos);
			}
		}
	} else {

		ALLOC_INIT_ZVAL(ret);
		FIND_XML_NULL(data, ret);
		if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
			return ret;
		}

		object_init_ex(ret, ce);
		trav = data->children;

		while (trav != NULL) {
			if (trav->type == XML_ELEMENT_NODE) {
				zval  *tmpVal;
				zval *prop;

				tmpVal = master_to_zval(NULL, trav TSRMLS_CC);

				prop = get_zval_property(ret, (char*)trav->name TSRMLS_CC);
				if (!prop) {
					if (!trav->next || !get_node(trav->next, (char*)trav->name)) {
						set_zval_property(ret, (char*)trav->name, tmpVal TSRMLS_CC);
					} else {
						zval *arr;

						MAKE_STD_ZVAL(arr);
						array_init(arr);
						add_next_index_zval(arr, tmpVal);
						set_zval_property(ret, (char*)trav->name, arr TSRMLS_CC);
					}
				} else {
					/* Property already exist - make array */
					if (Z_TYPE_P(prop) != IS_ARRAY) {
						/* Convert into array */
						zval *arr;

						MAKE_STD_ZVAL(arr);
						array_init(arr);
						Z_ADDREF_P(prop);
						add_next_index_zval(arr, prop);
						set_zval_property(ret, (char*)trav->name, arr TSRMLS_CC);
						prop = arr;
					}
					/* Add array element */
					add_next_index_zval(prop, tmpVal);
				}
			}
			trav = trav->next;
		}
	}
	return ret;
}

static zval *to_zval_object(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	return to_zval_object_ex(type, data, NULL TSRMLS_CC);
}


static int model_to_xml_object(xmlNodePtr node, sdlContentModelPtr model, zval *object, int style, int strict TSRMLS_DC)
{
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT: {
			zval *data;
			xmlNodePtr property;
			encodePtr enc;

			data = get_zval_property(object, model->u.element->name TSRMLS_CC);
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
					zval **val;

					zend_hash_internal_pointer_reset(ht);
					while (zend_hash_get_current_data(ht,(void**)&val) == SUCCESS) {
						if (Z_TYPE_PP(val) == IS_NULL && model->u.element->nillable) {
							property = xmlNewNode(NULL, BAD_CAST("BOGUS"));
							xmlAddChild(node, property);
							set_xsi_nil(property);
						} else {
							property = master_to_xml(enc, *val, style, node TSRMLS_CC);
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
						zend_hash_move_forward(ht);
					}
				} else {
					if (Z_TYPE_P(data) == IS_NULL && model->u.element->nillable) {
						property = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlAddChild(node, property);
						set_xsi_nil(property);
					} else if (Z_TYPE_P(data) == IS_NULL && model->min_occurs == 0) {
						return 1;
					} else {
						property = master_to_xml(enc, data, style, node TSRMLS_CC);
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
			xmlNodePtr property;
			encodePtr enc;

			data = get_zval_property(object, "any" TSRMLS_CC);
			if (data) {
				enc = get_conversion(XSD_ANYXML);
				if ((model->max_occurs == -1 || model->max_occurs > 1) &&
				    Z_TYPE_P(data) == IS_ARRAY &&
				    !is_map(data)) {
					HashTable *ht = Z_ARRVAL_P(data);
					zval **val;

					zend_hash_internal_pointer_reset(ht);
					while (zend_hash_get_current_data(ht,(void**)&val) == SUCCESS) {
						property = master_to_xml(enc, *val, style, node TSRMLS_CC);
						zend_hash_move_forward(ht);
					}
				} else {
					property = master_to_xml(enc, data, style, node TSRMLS_CC);
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
			sdlContentModelPtr *tmp;
			HashPosition pos;

			zend_hash_internal_pointer_reset_ex(model->u.content, &pos);
			while (zend_hash_get_current_data_ex(model->u.content, (void**)&tmp, &pos) == SUCCESS) {
				if (!model_to_xml_object(node, *tmp, object, style, strict && ((*tmp)->min_occurs > 0) TSRMLS_CC)) {
					if (!strict || (*tmp)->min_occurs > 0) {
						return 0;
					}
				}
				strict = 1;
				zend_hash_move_forward_ex(model->u.content, &pos);
			}
			return 1;
		}
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr *tmp;
			HashPosition pos;
			int ret = 0;

			zend_hash_internal_pointer_reset_ex(model->u.content, &pos);
			while (zend_hash_get_current_data_ex(model->u.content, (void**)&tmp, &pos) == SUCCESS) {
				int tmp_ret = model_to_xml_object(node, *tmp, object, style, 0 TSRMLS_CC);
				if (tmp_ret == 1) {
					return 1;
				} else if (tmp_ret != 0) {
					ret = 1;
				}
				zend_hash_move_forward_ex(model->u.content, &pos);
			}
			return ret;
		}
		case XSD_CONTENT_GROUP: {
			return model_to_xml_object(node, model->u.group->model, object, style, strict && model->min_occurs > 0 TSRMLS_CC);
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
			sdlContentModelPtr *tmp;
			HashPosition pos;

			if (zend_hash_num_elements(model->u.content) != 1) {
			  return NULL;
			}
			zend_hash_internal_pointer_reset_ex(model->u.content, &pos);
			zend_hash_get_current_data_ex(model->u.content, (void**)&tmp, &pos);
			return model_array_element(*tmp);
		}
		case XSD_CONTENT_GROUP: {
			return model_array_element(model->u.group->model);
		}
		default:
		  break;
	}
	return NULL;
}

static xmlNodePtr to_xml_object(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr xmlParam;
	HashTable *prop = NULL;
	int i;
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
				zval *tmp = get_zval_property(data, "_" TSRMLS_CC);
				if (tmp) {
					xmlParam = master_to_xml(enc, tmp, style, parent TSRMLS_CC);
				} else if (prop == NULL) {
					xmlParam = master_to_xml(enc, data, style, parent TSRMLS_CC);
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

				if (prop) prop->nApplyCount++;
				xmlParam = master_to_xml(sdlType->encode, data, style, parent TSRMLS_CC);
				if (prop) prop->nApplyCount--;
			} else {
				zval *tmp = get_zval_property(data, "_" TSRMLS_CC);

				if (tmp) {
					xmlParam = master_to_xml(sdlType->encode, tmp, style, parent TSRMLS_CC);
				} else if (prop == NULL) {
					xmlParam = master_to_xml(sdlType->encode, data, style, parent TSRMLS_CC);
				} else {
					xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
					xmlAddChild(parent, xmlParam);
				}
			}
		} else {
			xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
			xmlAddChild(parent, xmlParam);
		}

		if (soap_check_zval_ref(data, xmlParam TSRMLS_CC)) {
			return xmlParam;
		}
		if (prop != NULL) {
			sdlTypePtr array_el;

			if (Z_TYPE_P(data) == IS_ARRAY &&
		      !is_map(data) &&
		      sdlType->attributes == NULL &&
		      sdlType->model != NULL &&
		      (array_el = model_array_element(sdlType->model)) != NULL) {
				zval **val;

				zend_hash_internal_pointer_reset(prop);
				while (zend_hash_get_current_data(prop,(void**)&val) == SUCCESS) {
					xmlNodePtr property;
					if (Z_TYPE_PP(val) == IS_NULL && array_el->nillable) {
						property = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlAddChild(xmlParam, property);
						set_xsi_nil(property);
					} else {
						property = master_to_xml(array_el->encode, *val, style, xmlParam TSRMLS_CC);
					}
					xmlNodeSetName(property, BAD_CAST(array_el->name));
					if (style == SOAP_LITERAL &&
					   array_el->namens &&
					   array_el->form == XSD_FORM_QUALIFIED) {
						xmlNsPtr nsp = encode_add_ns(property, array_el->namens);
						xmlSetNs(property, nsp);
					}
					zend_hash_move_forward(prop);
				}
			} else if (sdlType->model) {
				model_to_xml_object(xmlParam, sdlType->model, data, style, 1 TSRMLS_CC);
			}
			if (sdlType->attributes) {
				sdlAttributePtr *attr;
				zval *zattr;
				HashPosition pos;

				zend_hash_internal_pointer_reset_ex(sdlType->attributes, &pos);
				while (zend_hash_get_current_data_ex(sdlType->attributes, (void**)&attr, &pos) == SUCCESS) {
					if ((*attr)->name) {
						zattr = get_zval_property(data, (*attr)->name TSRMLS_CC);
						if (zattr) {
							xmlNodePtr dummy;

							dummy = master_to_xml((*attr)->encode, zattr, SOAP_LITERAL, xmlParam TSRMLS_CC);
							if (dummy->children && dummy->children->content) {
								if ((*attr)->fixed && strcmp((*attr)->fixed, (char*)dummy->children->content) != 0) {
									soap_error3(E_ERROR, "Encoding: Attribute '%s' has fixed value '%s' (value '%s' is not allowed)", (*attr)->name, (*attr)->fixed, dummy->children->content);
								}
								/* we need to handle xml: namespace specially, since it is
								   an implicit schema. Otherwise, use form.
								*/
								if ((*attr)->namens &&
								    (!strncmp((*attr)->namens, XML_NAMESPACE, sizeof(XML_NAMESPACE)) ||
								     (*attr)->form == XSD_FORM_QUALIFIED)) {
									xmlNsPtr nsp = encode_add_ns(xmlParam, (*attr)->namens);

									xmlSetNsProp(xmlParam, nsp, BAD_CAST((*attr)->name), dummy->children->content);
								} else {
									xmlSetProp(xmlParam, BAD_CAST((*attr)->name), dummy->children->content);
								}
							}
							xmlUnlinkNode(dummy);
							xmlFreeNode(dummy);
						}
					}
					zend_hash_move_forward_ex(sdlType->attributes, &pos);
				}
			}
		}
		if (style == SOAP_ENCODED) {
			set_ns_and_type(xmlParam, type);
		}
	} else {
		xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
		xmlAddChild(parent, xmlParam);

		if (soap_check_zval_ref(data, xmlParam TSRMLS_CC)) {
			return xmlParam;
		}
		if (prop != NULL) {
			i = zend_hash_num_elements(prop);
			zend_hash_internal_pointer_reset(prop);

			for (;i > 0;i--) {
				xmlNodePtr property;
				zval **zprop;
				char *str_key;
				ulong index;
				int key_type;
				unsigned int str_key_len;

				key_type = zend_hash_get_current_key_ex(prop, &str_key, &str_key_len, &index, FALSE, NULL);
				zend_hash_get_current_data(prop, (void **)&zprop);

				property = master_to_xml(get_conversion((*zprop)->type), (*zprop), style, xmlParam TSRMLS_CC);

				if (key_type == HASH_KEY_IS_STRING) {
					const char *prop_name;

					if (Z_TYPE_P(data) == IS_OBJECT) {
						const char *class_name;

						zend_unmangle_property_name(str_key, str_key_len-1, &class_name, &prop_name);
					} else {
						prop_name = str_key;
					}
					if (prop_name) {
						xmlNodeSetName(property, BAD_CAST(prop_name));
					}
				}
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
static xmlNodePtr guess_array_map(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
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

	return master_to_xml(enc, data, style, parent TSRMLS_CC);
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
                                   TSRMLS_DC)
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
 						xparam = master_to_xml(get_conversion((*zdata)->type), (*zdata), style, xmlParam TSRMLS_CC);
 					} else {
 						xparam = master_to_xml(enc, (*zdata), style, xmlParam TSRMLS_CC);
		 			}
		 		} else {
					xparam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
					xmlAddChild(xmlParam, xparam);
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
 				if (zdata) {
	 			  add_xml_array_elements(xmlParam, type, enc, ns, dimension-1, dims+1, *zdata, style TSRMLS_CC);
	 			} else {
	 			  add_xml_array_elements(xmlParam, type, enc, ns, dimension-1, dims+1, NULL, style TSRMLS_CC);
	 			}
 			}
 			zend_hash_move_forward(data->value.ht);
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
 			  add_xml_array_elements(xmlParam, type, enc, ns, dimension-1, dims+1, NULL, style TSRMLS_CC);
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

static xmlNodePtr to_xml_array(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
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
	zval *array_copy = NULL;

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

	if (Z_TYPE_P(data) == IS_OBJECT && instanceof_function(Z_OBJCE_P(data), zend_ce_traversable TSRMLS_CC)) {
		zend_object_iterator   *iter;
		zend_class_entry       *ce = Z_OBJCE_P(data);
		zval                  **val;

		ALLOC_ZVAL(array_copy);
		INIT_PZVAL(array_copy);
		array_init(array_copy);

		iter = ce->get_iterator(ce, data, 0 TSRMLS_CC);

		if (EG(exception)) {
			goto iterator_done;
		}

		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
			if (EG(exception)) {
				goto iterator_done;
			}
		}

		while (iter->funcs->valid(iter TSRMLS_CC) == SUCCESS) {
			if (EG(exception)) {
				goto iterator_done;
			}

			iter->funcs->get_current_data(iter, &val TSRMLS_CC);
			if (EG(exception)) {
				goto iterator_done;
			}
			if (iter->funcs->get_current_key) {
				zval key;
				iter->funcs->get_current_key(iter, &key TSRMLS_CC);
				if (EG(exception)) {
					goto iterator_done;
				}
				array_set_zval_key(Z_ARRVAL_P(array_copy), &key, *val);
				zval_ptr_dtor(val);
				zval_dtor(&key);
			} else {
				add_next_index_zval(array_copy, *val);
			}
			Z_ADDREF_PP(val);

			iter->funcs->move_forward(iter TSRMLS_CC);
			if (EG(exception)) {
				goto iterator_done;
			}
		}
iterator_done:
		iter->funcs->dtor(iter TSRMLS_CC);
		if (EG(exception)) {
			zval_ptr_dtor(&array_copy);
			array_copy = NULL;
		} else {
			data = array_copy;
		}
	}

	if (Z_TYPE_P(data) == IS_ARRAY) {
		sdlAttributePtr *arrayType;
		sdlExtraAttributePtr *ext;
		sdlTypePtr elementType;

		i = zend_hash_num_elements(Z_ARRVAL_P(data));

		if (sdl_type &&
		    sdl_type->attributes &&
		    zend_hash_find(sdl_type->attributes, SOAP_1_1_ENC_NAMESPACE":arrayType",
		      sizeof(SOAP_1_1_ENC_NAMESPACE":arrayType"),
		      (void **)&arrayType) == SUCCESS &&
		    (*arrayType)->extraAttributes &&
		    zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType"), (void **)&ext) == SUCCESS) {

			char *value, *end;
			zval** el;

			value = estrdup((*ext)->val);
			end = strrchr(value,'[');
			if (end) {
				*end = '\0';
				end++;
				dimension = calc_dimension(end);
			}
			if ((*ext)->ns != NULL) {
				enc = get_encoder(SOAP_GLOBAL(sdl), (*ext)->ns, value);
				get_type_str(xmlParam, (*ext)->ns, value, &array_type);
			} else {
				smart_str_appends(&array_type, value);
			}

			dims = safe_emalloc(sizeof(int), dimension, 0);
			dims[0] = i;
			el = &data;
			for (i = 1; i < dimension; i++) {
				if (el != NULL && Z_TYPE_PP(el) == IS_ARRAY &&
				    zend_hash_num_elements(Z_ARRVAL_PP(el)) > 0) {
				  zend_hash_internal_pointer_reset(Z_ARRVAL_PP(el));
					zend_hash_get_current_data(Z_ARRVAL_PP(el), (void**)&el);
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

		} else if (sdl_type &&
		           sdl_type->attributes &&
		           zend_hash_find(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":itemType",
		             sizeof(SOAP_1_2_ENC_NAMESPACE":itemType"),
		             (void **)&arrayType) == SUCCESS &&
		           (*arrayType)->extraAttributes &&
		           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":itemType", sizeof(WSDL_NAMESPACE":itemType"), (void **)&ext) == SUCCESS) {
			if ((*ext)->ns != NULL) {
				enc = get_encoder(SOAP_GLOBAL(sdl), (*ext)->ns, (*ext)->val);
				get_type_str(xmlParam, (*ext)->ns, (*ext)->val, &array_type);
			} else {
				smart_str_appends(&array_type, (*ext)->val);
			}
			if (zend_hash_find(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
			                   sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
			                   (void **)&arrayType) == SUCCESS &&
			    (*arrayType)->extraAttributes &&
			    zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize"), (void **)&ext) == SUCCESS) {
				dimension = calc_dimension_12((*ext)->val);
				dims = get_position_12(dimension, (*ext)->val);
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
		           (*arrayType)->extraAttributes &&
		           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraySize"), (void **)&ext) == SUCCESS) {
			dimension = calc_dimension_12((*ext)->val);
			dims = get_position_12(dimension, (*ext)->val);
			if (dims[0] == 0) {dims[0] = i;}

			smart_str_append_long(&array_size, dims[0]);
			for (i=1; i<dimension; i++) {
				smart_str_appendc(&array_size, ',');
				smart_str_append_long(&array_size, dims[i]);
			}

			if (sdl_type && sdl_type->elements &&
			    zend_hash_num_elements(sdl_type->elements) == 1 &&
			    (zend_hash_internal_pointer_reset(sdl_type->elements),
			     zend_hash_get_current_data(sdl_type->elements, (void**)&elementType) == SUCCESS) &&
					(elementType = *(sdlTypePtr*)elementType) != NULL &&
			     elementType->encode && elementType->encode->details.type_str) {
				element_type = elementType;
				enc = elementType->encode;
				get_type_str(xmlParam, elementType->encode->details.ns, elementType->encode->details.type_str, &array_type);
			} else {
				enc = get_array_type(xmlParam, data, &array_type TSRMLS_CC);
			}
		} else if (sdl_type && sdl_type->elements &&
		           zend_hash_num_elements(sdl_type->elements) == 1 &&
		           (zend_hash_internal_pointer_reset(sdl_type->elements),
		            zend_hash_get_current_data(sdl_type->elements, (void**)&elementType) == SUCCESS) &&
		           (elementType = *(sdlTypePtr*)elementType) != NULL &&
		           elementType->encode && elementType->encode->details.type_str) {

			element_type = elementType;
			enc = elementType->encode;
			get_type_str(xmlParam, elementType->encode->details.ns, elementType->encode->details.type_str, &array_type);

			smart_str_append_long(&array_size, i);

			dims = safe_emalloc(sizeof(int), dimension, 0);
			dims[0] = i;
		} else {

			enc = get_array_type(xmlParam, data, &array_type TSRMLS_CC);
			smart_str_append_long(&array_size, i);
			dims = safe_emalloc(sizeof(int), dimension, 0);
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
				set_ns_prop(xmlParam, SOAP_1_1_ENC_NAMESPACE, "arrayType", array_type.c);
			} else {
				int i = 0;
				while (i < array_size.len) {
					if (array_size.c[i] == ',') {array_size.c[i] = ' ';}
					++i;
				}
				smart_str_0(&array_type);
				smart_str_0(&array_size);
				set_ns_prop(xmlParam, SOAP_1_2_ENC_NAMESPACE, "itemType", array_type.c);
				set_ns_prop(xmlParam, SOAP_1_2_ENC_NAMESPACE, "arraySize", array_size.c);
			}
		}
		smart_str_free(&array_type);
		smart_str_free(&array_size);

		add_xml_array_elements(xmlParam, element_type, enc, enc?encode_add_ns(xmlParam,enc->details.ns):NULL, dimension, dims, data, style TSRMLS_CC);
		efree(dims);
	}
	if (style == SOAP_ENCODED) {
		if (SOAP_GLOBAL(features) & SOAP_USE_XSI_ARRAY_TYPE) {
			set_ns_and_type_ex(xmlParam, (soap_version == SOAP_1_1) ? SOAP_1_1_ENC_NAMESPACE : SOAP_1_2_ENC_NAMESPACE, "Array");
		} else {
			set_ns_and_type(xmlParam, type);
		}
	}

	if (array_copy) {
		zval_ptr_dtor(&array_copy);
	}

	return xmlParam;
}

static zval *to_zval_array(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	xmlNodePtr trav;
	encodePtr enc = NULL;
	int dimension = 1;
	int* dims = NULL;
	int* pos = NULL;
	xmlAttrPtr attr;
	sdlPtr sdl;
	sdlAttributePtr *arrayType;
	sdlExtraAttributePtr *ext;
	sdlTypePtr elementType;

	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	sdl = SOAP_GLOBAL(sdl);

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
	           zend_hash_find(type->sdl_type->attributes, SOAP_1_1_ENC_NAMESPACE":arrayType",
	                          sizeof(SOAP_1_1_ENC_NAMESPACE":arrayType"),
	                          (void **)&arrayType) == SUCCESS &&
	           (*arrayType)->extraAttributes &&
	           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType"), (void **)&ext) == SUCCESS) {
		char *type, *end;

		type = estrdup((*ext)->val);
		end = strrchr(type,'[');
		if (end) {
			*end = '\0';
		}
		if ((*ext)->ns != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), (*ext)->ns, type);
		}
		efree(type);

		dims = emalloc(sizeof(int));
		*dims = 0;

	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           zend_hash_find(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":itemType",
	                          sizeof(SOAP_1_2_ENC_NAMESPACE":itemType"),
	                          (void **)&arrayType) == SUCCESS &&
	           (*arrayType)->extraAttributes &&
	           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":itemType", sizeof(WSDL_NAMESPACE":itemType"), (void **)&ext) == SUCCESS) {

		if ((*ext)->ns != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), (*ext)->ns, (*ext)->val);
		}

		if (zend_hash_find(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
		                   sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
		                   (void **)&arrayType) == SUCCESS &&
		    (*arrayType)->extraAttributes &&
		    zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize"), (void **)&ext) == SUCCESS) {
			dimension = calc_dimension_12((*ext)->val);
			dims = get_position_12(dimension, (*ext)->val);
		} else {
			dims = emalloc(sizeof(int));
			*dims = 0;
		}
	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           zend_hash_find(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
	                          sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
	                          (void **)&arrayType) == SUCCESS &&
	           (*arrayType)->extraAttributes &&
	           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize"), (void **)&ext) == SUCCESS) {

		dimension = calc_dimension_12((*ext)->val);
		dims = get_position_12(dimension, (*ext)->val);
		if (type->sdl_type && type->sdl_type->elements &&
		    zend_hash_num_elements(type->sdl_type->elements) == 1 &&
		    (zend_hash_internal_pointer_reset(type->sdl_type->elements),
		     zend_hash_get_current_data(type->sdl_type->elements, (void**)&elementType) == SUCCESS) &&
		    (elementType = *(sdlTypePtr*)elementType) != NULL &&
		    elementType->encode) {
			enc = elementType->encode;
		}
	} else if (type->sdl_type && type->sdl_type->elements &&
	           zend_hash_num_elements(type->sdl_type->elements) == 1 &&
	           (zend_hash_internal_pointer_reset(type->sdl_type->elements),
	            zend_hash_get_current_data(type->sdl_type->elements, (void**)&elementType) == SUCCESS) &&
	           (elementType = *(sdlTypePtr*)elementType) != NULL &&
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
			zval *tmpVal, *ar;
			xmlAttrPtr position = get_attribute(trav->properties,"position");

			tmpVal = master_to_zval(enc, trav TSRMLS_CC);
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
static xmlNodePtr to_xml_map(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr xmlParam;
	int i;

	xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, xmlParam);
	FIND_ZVAL_NULL(data, xmlParam, style);

	if (Z_TYPE_P(data) == IS_ARRAY) {
		i = zend_hash_num_elements(Z_ARRVAL_P(data));
		zend_hash_internal_pointer_reset(data->value.ht);
		for (;i > 0;i--) {
			xmlNodePtr xparam, item;
			xmlNodePtr key;
			zval **temp_data;
			char *key_val;
			ulong int_val;

			zend_hash_get_current_data(data->value.ht, (void **)&temp_data);
			item = xmlNewNode(NULL, BAD_CAST("item"));
			xmlAddChild(xmlParam, item);
			key = xmlNewNode(NULL, BAD_CAST("key"));
			xmlAddChild(item,key);
			if (zend_hash_get_current_key(data->value.ht, &key_val, &int_val, FALSE) == HASH_KEY_IS_STRING) {
				if (style == SOAP_ENCODED) {
					set_xsi_type(key, "xsd:string");
				}
				xmlNodeSetContent(key, BAD_CAST(key_val));
			} else {
				smart_str tmp = {0};
				smart_str_append_long(&tmp, int_val);
				smart_str_0(&tmp);

				if (style == SOAP_ENCODED) {
					set_xsi_type(key, "xsd:int");
				}
				xmlNodeSetContentLen(key, BAD_CAST(tmp.c), tmp.len);

				smart_str_free(&tmp);
			}

			xparam = master_to_xml(get_conversion((*temp_data)->type), (*temp_data), style, item TSRMLS_CC);
			xmlNodeSetName(xparam, BAD_CAST("value"));

			zend_hash_move_forward(data->value.ht);
		}
	}
	if (style == SOAP_ENCODED) {
		set_ns_and_type(xmlParam, type);
	}

	return xmlParam;
}

static zval *to_zval_map(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret, *key, *value;
	xmlNodePtr trav, item, xmlKey, xmlValue;

	MAKE_STD_ZVAL(ret);
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

			key = master_to_zval(NULL, xmlKey TSRMLS_CC);
			value = master_to_zval(NULL, xmlValue TSRMLS_CC);

			if (Z_TYPE_P(key) == IS_STRING) {
				zend_symtable_update(Z_ARRVAL_P(ret), Z_STRVAL_P(key), Z_STRLEN_P(key) + 1, &value, sizeof(zval *), NULL);
			} else if (Z_TYPE_P(key) == IS_LONG) {
				zend_hash_index_update(Z_ARRVAL_P(ret), Z_LVAL_P(key), &value, sizeof(zval *), NULL);
			} else {
				soap_error0(E_ERROR,  "Encoding: Can't decode apache map, only Strings or Longs are allowd as keys");
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
static xmlNodePtr guess_xml_convert(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	encodePtr  enc;
	xmlNodePtr ret;

	if (data) {
		enc = get_conversion(data->type);
	} else {
		enc = get_conversion(IS_NULL);
	}
	ret = master_to_xml_int(enc, data, style, parent, 0 TSRMLS_CC);
/*
	if (style == SOAP_LITERAL && SOAP_GLOBAL(sdl)) {
		set_ns_and_type(ret, &enc->details);
	}
*/
	return ret;
}

static zval *guess_zval_convert(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	encodePtr enc = NULL;
	xmlAttrPtr tmpattr;
	xmlChar *type_name = NULL;
	zval *ret;

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
	ret = master_to_zval_int(enc, data TSRMLS_CC);
	if (SOAP_GLOBAL(sdl) && type_name && enc->details.sdl_type) {
		zval* soapvar;
		char *ns, *cptype;
		xmlNsPtr nsptr;

		MAKE_STD_ZVAL(soapvar);
		object_init_ex(soapvar, soap_var_class_entry);
		add_property_long(soapvar, "enc_type", enc->details.type);
		Z_DELREF_P(ret);
		add_property_zval(soapvar, "enc_value", ret);
		parse_namespace(type_name, &cptype, &ns);
		nsptr = xmlSearchNs(data->doc, data, BAD_CAST(ns));
		add_property_string(soapvar, "enc_stype", cptype, 1);
		if (nsptr) {
			add_property_string(soapvar, "enc_ns", (char*)nsptr->href, 1);
		}
		efree(cptype);
		if (ns) {efree(ns);}
		ret = soapvar;
	}
	return ret;
}

/* Time encode/decode */
static xmlNodePtr to_xml_datetime_ex(encodeTypePtr type, zval *data, char *format, int style, xmlNodePtr parent TSRMLS_DC)
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
			soap_error1(E_ERROR, "Encoding: Invalid timestamp %ld", Z_LVAL_P(data));
		}

		buf = (char *) emalloc(buf_len);
		while ((real_len = strftime(buf, buf_len, format, ta)) == buf_len || real_len == 0) {
			buf_len *= 2;
			buf = (char *) erealloc(buf, buf_len);
			if (!--max_reallocs) break;
		}

		/* Time zone support */
#ifdef HAVE_TM_GMTOFF
		snprintf(tzbuf, sizeof(tzbuf), "%c%02d:%02d", (ta->tm_gmtoff < 0) ? '-' : '+', abs(ta->tm_gmtoff / 3600), abs( (ta->tm_gmtoff % 3600) / 60 ));
#else
# if defined(__CYGWIN__) || defined(NETWARE)
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

static xmlNodePtr to_xml_duration(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	/* TODO: '-'?P([0-9]+Y)?([0-9]+M)?([0-9]+D)?T([0-9]+H)?([0-9]+M)?([0-9]+S)? */
	return to_xml_string(type, data, style, parent TSRMLS_CC);
}

static xmlNodePtr to_xml_datetime(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "%Y-%m-%dT%H:%M:%S", style, parent TSRMLS_CC);
}

static xmlNodePtr to_xml_time(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	/* TODO: microsecconds */
	return to_xml_datetime_ex(type, data, "%H:%M:%S", style, parent TSRMLS_CC);
}

static xmlNodePtr to_xml_date(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "%Y-%m-%d", style, parent TSRMLS_CC);
}

static xmlNodePtr to_xml_gyearmonth(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "%Y-%m", style, parent TSRMLS_CC);
}

static xmlNodePtr to_xml_gyear(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "%Y", style, parent TSRMLS_CC);
}

static xmlNodePtr to_xml_gmonthday(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "--%m-%d", style, parent TSRMLS_CC);
}

static xmlNodePtr to_xml_gday(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "---%d", style, parent TSRMLS_CC);
}

static xmlNodePtr to_xml_gmonth(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "--%m--", style, parent TSRMLS_CC);
}

static zval* to_zval_list(encodeTypePtr enc, xmlNodePtr data TSRMLS_DC) {
	/*FIXME*/
	return to_zval_stringc(enc, data TSRMLS_CC);
}

static xmlNodePtr to_xml_list(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent TSRMLS_DC) {
	xmlNodePtr ret;
	encodePtr list_enc = NULL;

	if (enc->sdl_type && enc->sdl_type->kind == XSD_TYPEKIND_LIST && enc->sdl_type->elements) {
		sdlTypePtr *type;

		zend_hash_internal_pointer_reset(enc->sdl_type->elements);
		if (zend_hash_get_current_data(enc->sdl_type->elements, (void**)&type) == SUCCESS) {
			list_enc = (*type)->encode;
		}
	}

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);
	if (Z_TYPE_P(data) == IS_ARRAY) {
		zval **tmp;
		smart_str list = {0};
		HashTable *ht = Z_ARRVAL_P(data);

		zend_hash_internal_pointer_reset(ht);
		while (zend_hash_get_current_data(ht, (void**)&tmp) == SUCCESS) {
			xmlNodePtr dummy = master_to_xml(list_enc, *tmp, SOAP_LITERAL, ret TSRMLS_CC);
			if (dummy && dummy->children && dummy->children->content) {
				if (list.len != 0) {
					smart_str_appendc(&list, ' ');
				}
				smart_str_appends(&list, (char*)dummy->children->content);
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			xmlUnlinkNode(dummy);
			xmlFreeNode(dummy);
			zend_hash_move_forward(ht);
		}
		smart_str_0(&list);
		xmlNodeSetContentLen(ret, BAD_CAST(list.c), list.len);
		smart_str_free(&list);
	} else {
		zval tmp = *data;
		char *str, *start, *next;
		smart_str list = {0};

		if (Z_TYPE_P(data) != IS_STRING) {
			zval_copy_ctor(&tmp);
			convert_to_string(&tmp);
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
			ZVAL_STRING(&dummy_zval, start, 0);
			dummy = master_to_xml(list_enc, &dummy_zval, SOAP_LITERAL, ret TSRMLS_CC);
			if (dummy && dummy->children && dummy->children->content) {
				if (list.len != 0) {
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
		xmlNodeSetContentLen(ret, BAD_CAST(list.c), list.len);
		smart_str_free(&list);
		efree(str);
		if (data == &tmp) {zval_dtor(&tmp);}
	}
	return ret;
}

static xmlNodePtr to_xml_list1(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent TSRMLS_DC) {
	/*FIXME: minLength=1 */
	return to_xml_list(enc,data,style, parent TSRMLS_CC);
}

static zval* to_zval_union(encodeTypePtr enc, xmlNodePtr data TSRMLS_DC) {
	/*FIXME*/
	return to_zval_list(enc, data TSRMLS_CC);
}

static xmlNodePtr to_xml_union(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent TSRMLS_DC) {
	/*FIXME*/
	return to_xml_list(enc,data,style, parent TSRMLS_CC);
}

static zval *to_zval_any(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	xmlBufferPtr buf;
	zval *ret;

	if (SOAP_GLOBAL(sdl) && SOAP_GLOBAL(sdl)->elements && data->name) {
		smart_str nscat = {0};		
		sdlTypePtr *sdl_type;

		if (data->ns && data->ns->href) {
			smart_str_appends(&nscat, (char*)data->ns->href);
			smart_str_appendc(&nscat, ':');			
		}
		smart_str_appends(&nscat, (char*)data->name);
		smart_str_0(&nscat);

		if (zend_hash_find(SOAP_GLOBAL(sdl)->elements, nscat.c, nscat.len+1, (void **)&sdl_type) == SUCCESS &&
		    (*sdl_type)->encode) {
			smart_str_free(&nscat);
			return master_to_zval_int((*sdl_type)->encode, data TSRMLS_CC);
		}		
		smart_str_free(&nscat);
	}

	buf = xmlBufferCreate();
	xmlNodeDump(buf, NULL, data, 0, 0);
	MAKE_STD_ZVAL(ret);
	ZVAL_STRING(ret, (char*)xmlBufferContent(buf), 1);
	xmlBufferFree(buf);
	return ret;
}

static xmlNodePtr to_xml_any(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret = NULL;

	if (Z_TYPE_P(data) == IS_ARRAY) {
		HashPosition pos;
		zval **el;
		encodePtr enc = get_conversion(XSD_ANYXML);
		char *name;
		uint name_len;
		ulong idx;

		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(data), &pos);
		     zend_hash_get_current_data_ex(Z_ARRVAL_P(data), (void **) &el, &pos) == SUCCESS;
		     zend_hash_move_forward_ex(Z_ARRVAL_P(data), &pos)) {
			ret = master_to_xml(enc, *el, style, parent TSRMLS_CC);
		    if (ret &&
		        ret->name != xmlStringTextNoenc &&
		        zend_hash_get_current_key_ex(Z_ARRVAL_P(data), &name, &name_len, &idx, 0, &pos) == HASH_KEY_IS_STRING) {
				xmlNodeSetName(ret, BAD_CAST(name));
		    }
		}
		return ret;
	}
	if (Z_TYPE_P(data) == IS_STRING) {
		ret = xmlNewTextLen(BAD_CAST(Z_STRVAL_P(data)), Z_STRLEN_P(data));
	} else {
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		ret = xmlNewTextLen(BAD_CAST(Z_STRVAL(tmp)), Z_STRLEN(tmp));
		zval_dtor(&tmp);
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

zval *sdl_guess_convert_zval(encodeTypePtr enc, xmlNodePtr data TSRMLS_DC)
{
	sdlTypePtr type;

	type = enc->sdl_type;
	if (type == NULL) {
		return guess_zval_convert(enc, data TSRMLS_CC);
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
				return master_to_zval_int(type->encode, data TSRMLS_CC);
			} else {
				return guess_zval_convert(enc, data TSRMLS_CC);
			}
			break;
		case XSD_TYPEKIND_LIST:
			return to_zval_list(enc, data TSRMLS_CC);
		case XSD_TYPEKIND_UNION:
			return to_zval_union(enc, data TSRMLS_CC);
		case XSD_TYPEKIND_COMPLEX:
		case XSD_TYPEKIND_RESTRICTION:
		case XSD_TYPEKIND_EXTENSION:
			if (type->encode &&
			    (type->encode->details.type == IS_ARRAY ||
			     type->encode->details.type == SOAP_ENC_ARRAY)) {
				return to_zval_array(enc, data TSRMLS_CC);
			}
			return to_zval_object(enc, data TSRMLS_CC);
		default:
	  	soap_error0(E_ERROR, "Encoding: Internal Error");
			return guess_zval_convert(enc, data TSRMLS_CC);
	}
}

xmlNodePtr sdl_guess_convert_xml(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	sdlTypePtr type;
	xmlNodePtr ret = NULL;

	type = enc->sdl_type;

	if (type == NULL) {
		ret = guess_xml_convert(enc, data, style, parent TSRMLS_CC);
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
				ret = master_to_xml(type->encode, data, style, parent TSRMLS_CC);
			} else {
				ret = guess_xml_convert(enc, data, style, parent TSRMLS_CC);
			}
			break;
		case XSD_TYPEKIND_LIST:
			ret = to_xml_list(enc, data, style, parent TSRMLS_CC);
			break;
		case XSD_TYPEKIND_UNION:
			ret = to_xml_union(enc, data, style, parent TSRMLS_CC);
			break;
		case XSD_TYPEKIND_COMPLEX:
		case XSD_TYPEKIND_RESTRICTION:
		case XSD_TYPEKIND_EXTENSION:
			if (type->encode &&
			    (type->encode->details.type == IS_ARRAY ||
			     type->encode->details.type == SOAP_ENC_ARRAY)) {
				return to_xml_array(enc, data, style, parent TSRMLS_CC);
			} else {
				return to_xml_object(enc, data, style, parent TSRMLS_CC);
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
	set_xsi_type(node, nstype.c);
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
        TSRMLS_FETCH();

		if (zend_hash_find(&SOAP_GLOBAL(defEncNs), (char*)ns, strlen(ns) + 1, (void **)&prefix) == SUCCESS) {
			xmlns = xmlNewNs(node->doc->children, BAD_CAST(ns), prefix);
		} else {
			smart_str prefix = {0};
			int num = ++SOAP_GLOBAL(cur_uniq_ns);

			while (1) {
				smart_str_appendl(&prefix, "ns", 2);
				smart_str_append_long(&prefix, num);
				smart_str_0(&prefix);
				if (xmlSearchNs(node->doc, node, BAD_CAST(prefix.c)) == NULL) {
					break;
				}
				smart_str_free(&prefix);
				prefix.c = NULL;
				prefix.len = 0;
				num = ++SOAP_GLOBAL(cur_uniq_ns);
			}

			xmlns = xmlNewNs(node->doc->children, BAD_CAST(ns), BAD_CAST(prefix.c));
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

void encode_reset_ns()
{
	TSRMLS_FETCH();
	SOAP_GLOBAL(cur_uniq_ns) = 0;
	SOAP_GLOBAL(cur_uniq_ref) = 0;
	if (SOAP_GLOBAL(ref_map)) {
		zend_hash_destroy(SOAP_GLOBAL(ref_map));
	} else {
		SOAP_GLOBAL(ref_map) = emalloc(sizeof(HashTable));
	}
	zend_hash_init(SOAP_GLOBAL(ref_map), 0, NULL, NULL, 0);
}

void encode_finish()
{
	TSRMLS_FETCH();
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
	encodePtr *enc = NULL;
	TSRMLS_FETCH();

	if (zend_hash_index_find(&SOAP_GLOBAL(defEncIndex), encode, (void **)&enc) == FAILURE) {
		soap_error0(E_ERROR,  "Encoding: Cannot find encoding");
		return NULL;
	} else {
		return *enc;
	}
}

static int is_map(zval *array)
{
	int i, count = zend_hash_num_elements(Z_ARRVAL_P(array));

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(array));
	for (i = 0; i < count; i++) {
		char *str_index;
		ulong num_index;

		if (zend_hash_get_current_key(Z_ARRVAL_P(array), &str_index, &num_index, 0) == HASH_KEY_IS_STRING ||
		    num_index != i) {
			return TRUE;
		}
		zend_hash_move_forward(Z_ARRVAL_P(array));
	}
	return FALSE;
}

static encodePtr get_array_type(xmlNodePtr node, zval *array, smart_str *type TSRMLS_DC)
{
	HashTable *ht;
	int i, count, cur_type, prev_type, different;
	zval **tmp;
	char *prev_stype = NULL, *cur_stype = NULL, *prev_ns = NULL, *cur_ns = NULL;

	if (!array || Z_TYPE_P(array) != IS_ARRAY) {
		smart_str_appendl(type, "xsd:anyType", sizeof("xsd:anyType")-1);
		return get_conversion(XSD_ANYTYPE);
	}

	different = FALSE;
	cur_type = prev_type = 0;
	ht = HASH_OF(array);
	count = zend_hash_num_elements(ht);

	zend_hash_internal_pointer_reset(ht);
	for (i = 0;i < count;i++) {
		zend_hash_get_current_data(ht, (void **)&tmp);

		if (Z_TYPE_PP(tmp) == IS_OBJECT &&
		    Z_OBJCE_PP(tmp) == soap_var_class_entry) {
			zval **ztype;

			if (zend_hash_find(Z_OBJPROP_PP(tmp), "enc_type", sizeof("enc_type"), (void **)&ztype) == FAILURE ||
			    Z_TYPE_PP(ztype) != IS_LONG) {
				soap_error0(E_ERROR,  "Encoding: SoapVar has no 'enc_type' property");
			}
			cur_type = Z_LVAL_PP(ztype);

			if (zend_hash_find(Z_OBJPROP_PP(tmp), "enc_stype", sizeof("enc_stype"), (void **)&ztype) == SUCCESS &&
			    Z_TYPE_PP(ztype) == IS_STRING) {
				cur_stype = Z_STRVAL_PP(ztype);
			} else {
				cur_stype = NULL;
			}

			if (zend_hash_find(Z_OBJPROP_PP(tmp), "enc_ns", sizeof("enc_ns"), (void **)&ztype) == SUCCESS &&
			    Z_TYPE_PP(ztype) == IS_STRING) {
				cur_ns = Z_STRVAL_PP(ztype);
			} else {
				cur_ns = NULL;
			}

		} else if (Z_TYPE_PP(tmp) == IS_ARRAY && is_map(*tmp)) {
			cur_type = APACHE_MAP;
			cur_stype = NULL;
			cur_ns = NULL;
		} else {
			cur_type = Z_TYPE_PP(tmp);
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
		zend_hash_move_forward(ht);
	}

	if (different || count == 0) {
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

			enc = get_encoder_ex(SOAP_GLOBAL(sdl), array_type.c, array_type.len);
			smart_str_free(&array_type);
			return enc;
		} else {
			enc = get_conversion(cur_type);
			get_type_str(node, enc->details.ns, enc->details.type_str, type);
			return enc;
		}
	}
}

static void get_type_str(xmlNodePtr node, const char* ns, const char* type, smart_str* ret)
{
	TSRMLS_FETCH();

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

	if (map->to_xml) {
		zval_ptr_dtor(&map->to_xml);
	}
	if (map->to_zval) {
		zval_ptr_dtor(&map->to_zval);
	}
	efree(map);
}

void delete_encoder(void *encode)
{
	encodePtr t = *((encodePtr*)encode);
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

void delete_encoder_persistent(void *encode)
{
	encodePtr t = *((encodePtr*)encode);
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
