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
  | Author: Rob Richards <rrichards@php.net>                             |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xmlreader.h"
#ifdef HAVE_DOM
#include "ext/dom/xml_common.h"
#include "ext/dom/dom_ce.h"
#endif
#include <libxml/xmlreader.h>
#include <libxml/uri.h>
#include "xmlreader_arginfo.h"

zend_class_entry *xmlreader_class_entry;

static zend_object_handlers xmlreader_object_handlers;

static HashTable xmlreader_prop_handlers;

static zend_internal_function xmlreader_open_fn;
static zend_internal_function xmlreader_xml_fn;

typedef int (*xmlreader_read_int_t)(xmlTextReaderPtr reader);
typedef unsigned char *(*xmlreader_read_char_t)(xmlTextReaderPtr reader);
typedef const unsigned char *(*xmlreader_read_const_char_t)(xmlTextReaderPtr reader);
typedef int (*xmlreader_write_t)(xmlreader_object *obj, zval *newval);

typedef unsigned char *(*xmlreader_read_one_char_t)(xmlTextReaderPtr reader, const unsigned char *);

typedef struct _xmlreader_prop_handler {
	xmlreader_read_int_t read_int_func;
	xmlreader_read_const_char_t read_char_func;
	xmlreader_write_t write_func;
	int type;
} xmlreader_prop_handler;

#define XMLREADER_LOAD_STRING 0
#define XMLREADER_LOAD_FILE 1

/* {{{ xmlreader_register_prop_handler */
static void xmlreader_register_prop_handler(HashTable *prop_handler, char *name, xmlreader_read_int_t read_int_func, xmlreader_read_const_char_t read_char_func, int rettype)
{
	xmlreader_prop_handler hnd;
	zend_string *str;

	hnd.read_char_func = read_char_func;
	hnd.read_int_func = read_int_func;
	hnd.type = rettype;
	str = zend_string_init_interned(name, strlen(name), 1);
	zend_hash_add_mem(prop_handler, str, &hnd, sizeof(xmlreader_prop_handler));
	zend_string_release_ex(str, 1);
}
/* }}} */

/* {{{ xmlreader_property_reader */
static int xmlreader_property_reader(xmlreader_object *obj, xmlreader_prop_handler *hnd, zval *rv)
{
	const xmlChar *retchar = NULL;
	int retint = 0;

	if (obj->ptr != NULL) {
		if (hnd->read_char_func) {
			retchar = hnd->read_char_func(obj->ptr);
		} else {
			if (hnd->read_int_func) {
				retint = hnd->read_int_func(obj->ptr);
				if (retint == -1) {
					php_error_docref(NULL, E_WARNING, "Internal libxml error returned");
					return FAILURE;
				}
			}
		}
	}

	switch (hnd->type) {
		case IS_STRING:
			if (retchar) {
				ZVAL_STRING(rv, (char *) retchar);
			} else {
				ZVAL_EMPTY_STRING(rv);
			}
			break;
		/* this IS_FALSE actually means it's a BOOL type */
		case IS_FALSE:
			ZVAL_BOOL(rv, retint);
			break;
		case IS_LONG:
			ZVAL_LONG(rv, retint);
			break;
		default:
			ZVAL_NULL(rv);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ xmlreader_get_property_ptr_ptr */
zval *xmlreader_get_property_ptr_ptr(zend_object *object, zend_string *name, int type, void **cache_slot)
{
	xmlreader_object *obj;
	zval *retval = NULL;
	xmlreader_prop_handler *hnd = NULL;

	obj = php_xmlreader_fetch_object(object);

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, name);
	}

	if (hnd == NULL) {
		retval = zend_std_get_property_ptr_ptr(object, name, type, cache_slot);
	}

	return retval;
}
/* }}} */

/* {{{ xmlreader_read_property */
zval *xmlreader_read_property(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv)
{
	xmlreader_object *obj;
	zval *retval = NULL;
	xmlreader_prop_handler *hnd = NULL;

	obj = php_xmlreader_fetch_object(object);

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, name);
	}

	if (hnd != NULL) {
		if (xmlreader_property_reader(obj, hnd, rv) == FAILURE) {
			retval = &EG(uninitialized_zval);
		} else {
			retval = rv;
		}
	} else {
		retval = zend_std_read_property(object, name, type, cache_slot, rv);
	}

	return retval;
}
/* }}} */

/* {{{ xmlreader_write_property */
zval *xmlreader_write_property(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	xmlreader_object *obj;
	xmlreader_prop_handler *hnd = NULL;

	obj = php_xmlreader_fetch_object(object);

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, name);
	}
	if (hnd != NULL) {
		php_error_docref(NULL, E_WARNING, "Cannot write to read-only property");
	} else {
		value = zend_std_write_property(object, name, value, cache_slot);
	}

	return value;
}
/* }}} */

/* {{{ */
static zend_function *xmlreader_get_method(zend_object **obj, zend_string *name, const zval *key)
{
	if (ZSTR_LEN(name) == sizeof("open") - 1
			&& (ZSTR_VAL(name)[0] == 'o' || ZSTR_VAL(name)[0] == 'O')
			&& (ZSTR_VAL(name)[1] == 'p' || ZSTR_VAL(name)[1] == 'P')
			&& (ZSTR_VAL(name)[2] == 'e' || ZSTR_VAL(name)[2] == 'E')
			&& (ZSTR_VAL(name)[3] == 'n' || ZSTR_VAL(name)[3] == 'N')) {
		return (zend_function*)&xmlreader_open_fn;
	} else if (ZSTR_LEN(name) == sizeof("xml") - 1
			&& (ZSTR_VAL(name)[0] == 'x' || ZSTR_VAL(name)[0] == 'X')
			&& (ZSTR_VAL(name)[1] == 'm' || ZSTR_VAL(name)[1] == 'M')
			&& (ZSTR_VAL(name)[2] == 'l' || ZSTR_VAL(name)[2] == 'L')) {
		return (zend_function*)&xmlreader_xml_fn;
	}
	return zend_std_get_method(obj, name, key);;
}
/* }}} */

/* {{{ _xmlreader_get_valid_file_path */
/* _xmlreader_get_valid_file_path and _xmlreader_get_relaxNG should be made a
	common function in libxml extension as code is common to a few xml extensions */
char *_xmlreader_get_valid_file_path(char *source, char *resolved_path, int resolved_path_len ) {
	xmlURI *uri;
	xmlChar *escsource;
	char *file_dest;
	int isFileUri = 0;

	uri = xmlCreateURI();
	escsource = xmlURIEscapeStr((xmlChar *)source, (xmlChar *)":");
	xmlParseURIReference(uri, (const char *)escsource);
	xmlFree(escsource);

	if (uri->scheme != NULL) {
		/* absolute file uris - libxml only supports localhost or empty host */
		if (strncasecmp(source, "file:///",8) == 0) {
			isFileUri = 1;
#ifdef PHP_WIN32
			source += 8;
#else
			source += 7;
#endif
		} else if (strncasecmp(source, "file://localhost/",17) == 0) {
			isFileUri = 1;
#ifdef PHP_WIN32
			source += 17;
#else
			source += 16;
#endif
		}
	}

	file_dest = source;

	if ((uri->scheme == NULL || isFileUri)) {
		if (!VCWD_REALPATH(source, resolved_path) && !expand_filepath(source, resolved_path)) {
			xmlFreeURI(uri);
			return NULL;
		}
		file_dest = resolved_path;
	}

	xmlFreeURI(uri);

	return file_dest;
}
/* }}} */

#ifdef LIBXML_SCHEMAS_ENABLED
/* {{{ _xmlreader_get_relaxNG */
static xmlRelaxNGPtr _xmlreader_get_relaxNG(char *source, size_t source_len, size_t type,
											xmlRelaxNGValidityErrorFunc error_func,
											xmlRelaxNGValidityWarningFunc warn_func)
{
	char *valid_file = NULL;
	xmlRelaxNGParserCtxtPtr parser = NULL;
	xmlRelaxNGPtr           sptr;
	char resolved_path[MAXPATHLEN + 1];

	switch (type) {
	case XMLREADER_LOAD_FILE:
		valid_file = _xmlreader_get_valid_file_path(source, resolved_path, MAXPATHLEN );
		if (!valid_file) {
			return NULL;
		}
		parser = xmlRelaxNGNewParserCtxt(valid_file);
		break;
	case XMLREADER_LOAD_STRING:
		parser = xmlRelaxNGNewMemParserCtxt(source, source_len);
		/* If loading from memory, we need to set the base directory for the document
		   but it is not apparent how to do that for schema's */
		break;
	default:
		return NULL;
	}

	if (parser == NULL) {
		return NULL;
	}

	if (error_func || warn_func) {
		xmlRelaxNGSetParserErrors(parser,
			(xmlRelaxNGValidityErrorFunc) error_func,
			(xmlRelaxNGValidityWarningFunc) warn_func,
			parser);
	}
	sptr = xmlRelaxNGParse(parser);
	xmlRelaxNGFreeParserCtxt(parser);

	return sptr;
}
/* }}} */
#endif

static const zend_module_dep xmlreader_deps[] = {
	ZEND_MOD_REQUIRED("libxml")
	ZEND_MOD_END
};

/* {{{ xmlreader_module_entry
 */
zend_module_entry xmlreader_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	xmlreader_deps,
	"xmlreader",
	NULL,
	PHP_MINIT(xmlreader),
	PHP_MSHUTDOWN(xmlreader),
	NULL,
	NULL,
	PHP_MINFO(xmlreader),
	PHP_XMLREADER_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XMLREADER
ZEND_GET_MODULE(xmlreader)
#endif

/* {{{ xmlreader_objects_clone */
void xmlreader_objects_clone(void *object, void **object_clone)
{
	/* TODO */
}
/* }}} */

/* {{{ xmlreader_free_resources */
static void xmlreader_free_resources(xmlreader_object *intern) {
	if (intern) {
		if (intern->input) {
			xmlFreeParserInputBuffer(intern->input);
			intern->input = NULL;
		}

		if (intern->ptr) {
			xmlFreeTextReader(intern->ptr);
			intern->ptr = NULL;
		}
#ifdef LIBXML_SCHEMAS_ENABLED
		if (intern->schema) {
			xmlRelaxNGFree((xmlRelaxNGPtr) intern->schema);
			intern->schema = NULL;
		}
#endif
	}
}
/* }}} */

/* {{{ xmlreader_objects_free_storage */
void xmlreader_objects_free_storage(zend_object *object)
{
	xmlreader_object *intern = php_xmlreader_fetch_object(object);

	zend_object_std_dtor(&intern->std);

	xmlreader_free_resources(intern);
}
/* }}} */

/* {{{ xmlreader_objects_new */
zend_object *xmlreader_objects_new(zend_class_entry *class_type)
{
	xmlreader_object *intern;

	intern = zend_object_alloc(sizeof(xmlreader_object), class_type);
	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->prop_handler = &xmlreader_prop_handlers;
	intern->std.handlers = &xmlreader_object_handlers;

	return &intern->std;
}
/* }}} */

/* {{{ php_xmlreader_string_arg */
static void php_xmlreader_string_arg(INTERNAL_FUNCTION_PARAMETERS, xmlreader_read_one_char_t internal_function) {
	zval *id;
	size_t name_len = 0;
	char *retchar = NULL;
	xmlreader_object *intern;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		return;
	}

	if (!name_len) {
		php_error_docref(NULL, E_WARNING, "Argument cannot be an empty string");
		RETURN_FALSE;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retchar = (char *)internal_function(intern->ptr, (const unsigned char *)name);
	}
	if (retchar) {
		RETVAL_STRING(retchar);
		xmlFree(retchar);
		return;
	} else {
		RETVAL_NULL();
	}
}
/* }}} */

/* {{{ php_xmlreader_no_arg */
static void php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAMETERS, xmlreader_read_int_t internal_function) {
	zval *id;
	int retval;
	xmlreader_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retval = internal_function(intern->ptr);
		if (retval == 1) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

static void php_xmlreader_free_prop_handler(zval *el) /* {{{ */ {
	pefree(Z_PTR_P(el), 1);
} /* }}} */

/* {{{ php_xmlreader_no_arg_string */
static void php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAMETERS, xmlreader_read_char_t internal_function) {
	zval *id;
	char *retchar = NULL;
	xmlreader_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retchar = (char *)internal_function(intern->ptr);
	}
	if (retchar) {
		RETVAL_STRING(retchar);
		xmlFree(retchar);
		return;
	} else {
		RETVAL_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ php_xmlreader_set_relaxng_schema */
static void php_xmlreader_set_relaxng_schema(INTERNAL_FUNCTION_PARAMETERS, int type) {
#ifdef LIBXML_SCHEMAS_ENABLED
	zval *id;
	size_t source_len = 0;
	int retval = -1;
	xmlreader_object *intern;
	xmlRelaxNGPtr schema = NULL;
	char *source;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p!", &source, &source_len) == FAILURE) {
		return;
	}

	if (source != NULL && !source_len) {
		php_error_docref(NULL, E_WARNING, "Schema data source is required");
		RETURN_FALSE;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		if (source) {
			schema =  _xmlreader_get_relaxNG(source, source_len, type, NULL, NULL);
			if (schema) {
				retval = xmlTextReaderRelaxNGSetSchema(intern->ptr, schema);
			}
		} else {
			/* unset the associated relaxNG context and schema if one exists */
			retval = xmlTextReaderRelaxNGSetSchema(intern->ptr, NULL);
		}

		if (retval == 0) {
			if (intern->schema) {
				xmlRelaxNGFree((xmlRelaxNGPtr) intern->schema);
			}

			intern->schema = schema;

			RETURN_TRUE;
		}
	}

	php_error_docref(NULL, E_WARNING, "Unable to set schema. This must be set prior to reading or schema contains errors.");

	RETURN_FALSE;
#else
	php_error_docref(NULL, E_WARNING, "No Schema support built into libxml.");

	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool XMLReader::close()
Closes xmlreader - current frees resources until xmlTextReaderClose is fixed in libxml */
PHP_METHOD(xmlreader, close)
{
	zval *id;
	xmlreader_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	id = ZEND_THIS;
	intern = Z_XMLREADER_P(id);
	/* libxml is segfaulting in versions up to 2.6.8 using xmlTextReaderClose so for
	now we will free the whole reader when close is called as it would get rebuilt on
	a new load anyways */
	xmlreader_free_resources(intern);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string XMLReader::getAttribute(string name)
Get value of an attribute from current element */
PHP_METHOD(xmlreader, getAttribute)
{
	php_xmlreader_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderGetAttribute);
}
/* }}} */

/* {{{ proto string XMLReader::getAttributeNo(int index)
Get value of an attribute at index from current element */
PHP_METHOD(xmlreader, getAttributeNo)
{
	zval *id;
	zend_long attr_pos;
	char *retchar = NULL;
	xmlreader_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &attr_pos) == FAILURE) {
		return;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retchar = (char *)xmlTextReaderGetAttributeNo(intern->ptr, attr_pos);
	}
	if (retchar) {
		RETVAL_STRING(retchar);
		xmlFree(retchar);
	}
}
/* }}} */

/* {{{ proto string XMLReader::getAttributeNs(string name, string namespaceURI)
Get value of a attribute via name and namespace from current element */
PHP_METHOD(xmlreader, getAttributeNs)
{
	zval *id;
	size_t name_len = 0, ns_uri_len = 0;
	xmlreader_object *intern;
	char *name, *ns_uri, *retchar = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &name, &name_len, &ns_uri, &ns_uri_len) == FAILURE) {
		return;
	}

	if (name_len == 0 || ns_uri_len == 0) {
		php_error_docref(NULL, E_WARNING, "Attribute Name and Namespace URI cannot be empty");
		RETURN_FALSE;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retchar = (char *)xmlTextReaderGetAttributeNs(intern->ptr, (xmlChar *)name, (xmlChar *)ns_uri);
	}
	if (retchar) {
		RETVAL_STRING(retchar);
		xmlFree(retchar);
	}
}
/* }}} */

/* {{{ proto bool XMLReader::getParserProperty(int property)
Indicates whether given property (one of the parser option constants) is set or not on parser */
PHP_METHOD(xmlreader, getParserProperty)
{
	zval *id;
	zend_long property;
	int retval = -1;
	xmlreader_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &property) == FAILURE) {
		return;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retval = xmlTextReaderGetParserProp(intern->ptr,property);
	}
	if (retval == -1) {
		php_error_docref(NULL, E_WARNING, "Invalid parser property");
		RETURN_FALSE;
	}

	RETURN_BOOL(retval);
}
/* }}} */

/* {{{ proto bool XMLReader::isValid()
Returns boolean indicating if parsed document is valid or not.
Must set XMLREADER_LOADDTD or XMLREADER_VALIDATE parser option prior to the first call to read
or this method will always return FALSE */
PHP_METHOD(xmlreader, isValid)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderIsValid);
}
/* }}} */

/* {{{ proto string XMLReader::lookupNamespace(string prefix)
Return namespaceURI for associated prefix on current node */
PHP_METHOD(xmlreader, lookupNamespace)
{
	php_xmlreader_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderLookupNamespace);
}
/* }}} */

/* {{{ proto bool XMLReader::moveToAttribute(string name)
Positions reader at specified attribute - Returns TRUE on success and FALSE on failure */
PHP_METHOD(xmlreader, moveToAttribute)
{
	zval *id;
	size_t name_len = 0;
	int retval;
	xmlreader_object *intern;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		return;
	}

	if (name_len == 0) {
		php_error_docref(NULL, E_WARNING, "Attribute Name is required");
		RETURN_FALSE;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retval = xmlTextReaderMoveToAttribute(intern->ptr, (xmlChar *)name);
		if (retval == 1) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool XMLReader::moveToAttributeNo(int index)
Positions reader at attribute at specified index.
Returns TRUE on success and FALSE on failure */
PHP_METHOD(xmlreader, moveToAttributeNo)
{
	zval *id;
	zend_long attr_pos;
	int retval;
	xmlreader_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &attr_pos) == FAILURE) {
		return;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retval = xmlTextReaderMoveToAttributeNo(intern->ptr, attr_pos);
		if (retval == 1) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool XMLReader::moveToAttributeNs(string name, string namespaceURI)
Positions reader at attribute spcified by name and namespaceURI.
Returns TRUE on success and FALSE on failure */
PHP_METHOD(xmlreader, moveToAttributeNs)
{
	zval *id;
	size_t name_len=0, ns_uri_len=0;
	int retval;
	xmlreader_object *intern;
	char *name, *ns_uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &name, &name_len, &ns_uri, &ns_uri_len) == FAILURE) {
		return;
	}

	if (name_len == 0 || ns_uri_len == 0) {
		php_error_docref(NULL, E_WARNING, "Attribute Name and Namespace URI cannot be empty");
		RETURN_FALSE;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retval = xmlTextReaderMoveToAttributeNs(intern->ptr, (xmlChar *)name, (xmlChar *)ns_uri);
		if (retval == 1) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool XMLReader::moveToElement()
Moves the position of the current instance to the node that contains the current Attribute node. */
PHP_METHOD(xmlreader, moveToElement)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderMoveToElement);
}
/* }}} */

/* {{{ proto bool XMLReader::moveToFirstAttribute()
Moves the position of the current instance to the first attribute associated with the current node. */
PHP_METHOD(xmlreader, moveToFirstAttribute)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderMoveToFirstAttribute);
}
/* }}} */

/* {{{ proto bool XMLReader::moveToNextAttribute()
Moves the position of the current instance to the next attribute associated with the current node. */
PHP_METHOD(xmlreader, moveToNextAttribute)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderMoveToNextAttribute);
}
/* }}} */

/* {{{ proto bool XMLReader::read()
Moves the position of the current instance to the next node in the stream. */
PHP_METHOD(xmlreader, read)
{
	zval *id;
	int retval;
	xmlreader_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	id = ZEND_THIS;
	intern = Z_XMLREADER_P(id);
	if (intern != NULL && intern->ptr != NULL) {
		retval = xmlTextReaderRead(intern->ptr);
		if (retval == -1) {
			RETURN_FALSE;
		} else {
			RETURN_BOOL(retval);
		}
	}

	php_error_docref(NULL, E_WARNING, "Load Data before trying to read");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool XMLReader::next([string localname])
Moves the position of the current instance to the next node in the stream. */
PHP_METHOD(xmlreader, next)
{
	zval *id;
	int retval;
	size_t name_len=0;
	xmlreader_object *intern;
	char *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &name, &name_len) == FAILURE) {
		return;
	}

	id = ZEND_THIS;
	intern = Z_XMLREADER_P(id);
	if (intern != NULL && intern->ptr != NULL) {
		retval = xmlTextReaderNext(intern->ptr);
		while (name != NULL && retval == 1) {
			if (xmlStrEqual(xmlTextReaderConstLocalName(intern->ptr), (xmlChar *)name)) {
				RETURN_TRUE;
			}
			retval = xmlTextReaderNext(intern->ptr);
		}
		if (retval == -1) {
			RETURN_FALSE;
		} else {
			RETURN_BOOL(retval);
		}
	}

	php_error_docref(NULL, E_WARNING, "Load Data before trying to read");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool XMLReader::open(string URI [, string encoding [, int options]])
Sets the URI that the XMLReader will parse. */
PHP_METHOD(xmlreader, open)
{
	zval *id;
	size_t source_len = 0, encoding_len = 0;
	zend_long options = 0;
	xmlreader_object *intern = NULL;
	char *source, *valid_file = NULL;
	char *encoding = NULL;
	char resolved_path[MAXPATHLEN + 1];
	xmlTextReaderPtr reader = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|s!l", &source, &source_len, &encoding, &encoding_len, &options) == FAILURE) {
		return;
	}

	id = getThis();
	if (id != NULL) {
		if (! instanceof_function(Z_OBJCE_P(id), xmlreader_class_entry)) {
			id = NULL;
		} else {
			intern = Z_XMLREADER_P(id);
			xmlreader_free_resources(intern);
		}
	}

	if (!source_len) {
		php_error_docref(NULL, E_WARNING, "Empty string supplied as input");
		RETURN_FALSE;
	}

	valid_file = _xmlreader_get_valid_file_path(source, resolved_path, MAXPATHLEN );

	if (valid_file) {
		reader = xmlReaderForFile(valid_file, encoding, options);
	}

	if (reader == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to open source data");
		RETURN_FALSE;
	}

	if (id == NULL) {
		object_init_ex(return_value, xmlreader_class_entry);
		intern = Z_XMLREADER_P(return_value);
		intern->ptr = reader;
		return;
	}

	intern->ptr = reader;

	RETURN_TRUE;

}
/* }}} */

/* Not Yet Implemented in libxml - functions exist just not coded
PHP_METHOD(xmlreader, resetState)
{

}
*/

/* {{{ proto string XMLReader::readInnerXml()
Reads the contents of the current node, including child nodes and markup. */
PHP_METHOD(xmlreader, readInnerXml)
{
	php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderReadInnerXml);
}
/* }}} */

/* {{{ proto bool XMLReader::readOuterXml()
Reads the contents of the current node, including child nodes and markup. */
PHP_METHOD(xmlreader, readOuterXml)
{
	php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderReadOuterXml);
}
/* }}} */

/* {{{ proto bool XMLReader::readString()
Reads the contents of an element or a text node as a string. */
PHP_METHOD(xmlreader, readString)
{
	php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderReadString);
}
/* }}} */

/* {{{ proto bool XMLReader::setSchema(string filename)
Use W3C XSD schema to validate the document as it is processed. Activation is only possible before the first Read(). */
PHP_METHOD(xmlreader, setSchema)
{
#ifdef LIBXML_SCHEMAS_ENABLED
	zval *id;
	size_t source_len = 0;
	int retval = -1;
	xmlreader_object *intern;
	char *source;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p!", &source, &source_len) == FAILURE) {
		return;
	}

	if (source != NULL && !source_len) {
		php_error_docref(NULL, E_WARNING, "Schema data source is required");
		RETURN_FALSE;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retval = xmlTextReaderSchemaValidate(intern->ptr, source);

		if (retval == 0) {
			RETURN_TRUE;
		}
	}

	php_error_docref(NULL, E_WARNING, "Unable to set schema. This must be set prior to reading or schema contains errors.");

	RETURN_FALSE;
#else
	php_error_docref(NULL, E_WARNING, "No Schema support built into libxml.");

	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool XMLReader::setParserProperty(int property, bool value)
Sets parser property (one of the parser option constants).
Properties must be set after open() or XML() and before the first read() is called */
PHP_METHOD(xmlreader, setParserProperty)
{
	zval *id;
	zend_long property;
	int retval = -1;
	zend_bool value;
	xmlreader_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lb", &property, &value) == FAILURE) {
		return;
	}

	id = ZEND_THIS;

	intern = Z_XMLREADER_P(id);
	if (intern && intern->ptr) {
		retval = xmlTextReaderSetParserProp(intern->ptr,property, value);
	}
	if (retval == -1) {
		php_error_docref(NULL, E_WARNING, "Invalid parser property");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool XMLReader::setRelaxNGSchema(string filename)
Sets the string that the XMLReader will parse. */
PHP_METHOD(xmlreader, setRelaxNGSchema)
{
	php_xmlreader_set_relaxng_schema(INTERNAL_FUNCTION_PARAM_PASSTHRU, XMLREADER_LOAD_FILE);
}
/* }}} */

/* {{{ proto bool XMLReader::setRelaxNGSchemaSource(string source)
Sets the string that the XMLReader will parse. */
PHP_METHOD(xmlreader, setRelaxNGSchemaSource)
{
	php_xmlreader_set_relaxng_schema(INTERNAL_FUNCTION_PARAM_PASSTHRU, XMLREADER_LOAD_STRING);
}
/* }}} */

/* TODO
XMLPUBFUN int XMLCALL
		    xmlTextReaderSetSchema	(xmlTextReaderPtr reader,
		    				 xmlSchemaPtr schema);
*/

/* {{{ proto bool XMLReader::XML(string source [, string encoding [, int options]])
Sets the string that the XMLReader will parse. */
PHP_METHOD(xmlreader, XML)
{
	zval *id;
	size_t source_len = 0, encoding_len = 0;
	zend_long options = 0;
	xmlreader_object *intern = NULL;
	char *source, *uri = NULL, *encoding = NULL;
	int resolved_path_len, ret = 0;
	char *directory=NULL, resolved_path[MAXPATHLEN];
	xmlParserInputBufferPtr inputbfr;
	xmlTextReaderPtr reader;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!l", &source, &source_len, &encoding, &encoding_len, &options) == FAILURE) {
		return;
	}

	id = getThis();
	if (id != NULL && ! instanceof_function(Z_OBJCE_P(id), xmlreader_class_entry)) {
		id = NULL;
	}
	if (id != NULL) {
		intern = Z_XMLREADER_P(id);
		xmlreader_free_resources(intern);
	}

	if (!source_len) {
		php_error_docref(NULL, E_WARNING, "Empty string supplied as input");
		RETURN_FALSE;
	}

	inputbfr = xmlParserInputBufferCreateMem(source, source_len, XML_CHAR_ENCODING_NONE);

    if (inputbfr != NULL) {
/* Get the URI of the current script so that we can set the base directory in libxml */
#if HAVE_GETCWD
		directory = VCWD_GETCWD(resolved_path, MAXPATHLEN);
#elif HAVE_GETWD
		directory = VCWD_GETWD(resolved_path);
#endif
		if (directory) {
			resolved_path_len = strlen(resolved_path);
			if (resolved_path[resolved_path_len - 1] != DEFAULT_SLASH) {
				resolved_path[resolved_path_len] = DEFAULT_SLASH;
				resolved_path[++resolved_path_len] = '\0';
			}
			uri = (char *) xmlCanonicPath((const xmlChar *) resolved_path);
		}
		reader = xmlNewTextReader(inputbfr, uri);

		if (reader != NULL) {
			ret = xmlTextReaderSetup(reader, NULL, uri, encoding, options);
			if (ret == 0) {
				if (id == NULL) {
					object_init_ex(return_value, xmlreader_class_entry);
					intern = Z_XMLREADER_P(return_value);
				} else {
					RETVAL_TRUE;
				}
				intern->input = inputbfr;
				intern->ptr = reader;

				if (uri) {
					xmlFree(uri);
				}

				return;
			}
		}
	}

	if (uri) {
		xmlFree(uri);
	}

	if (inputbfr) {
		xmlFreeParserInputBuffer(inputbfr);
	}
	php_error_docref(NULL, E_WARNING, "Unable to load source data");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool XMLReader::expand()
Moves the position of the current instance to the next node in the stream. */
PHP_METHOD(xmlreader, expand)
{
#ifdef HAVE_DOM
	zval *id, *basenode = NULL;
	int ret;
	xmlreader_object *intern;
	xmlNode *node, *nodec;
	xmlDocPtr docp = NULL;
	php_libxml_node_object *domobj = NULL;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|O!", &basenode, dom_node_class_entry) == FAILURE) {
		return;
	}

	if (basenode != NULL) {
		NODE_GET_OBJ(node, basenode, xmlNodePtr, domobj);
		docp = node->doc;
	}

	intern = Z_XMLREADER_P(id);

	if (intern && intern->ptr) {
		node = xmlTextReaderExpand(intern->ptr);

		if (node == NULL) {
			php_error_docref(NULL, E_WARNING, "An Error Occurred while expanding ");
			RETURN_FALSE;
		} else {
			nodec = xmlDocCopyNode(node, docp, 1);
			if (nodec == NULL) {
				php_error_docref(NULL, E_NOTICE, "Cannot expand this node type");
				RETURN_FALSE;
			} else {
				DOM_RET_OBJ(nodec, &ret, (dom_object *)domobj);
			}
		}
	} else {
		php_error_docref(NULL, E_WARNING, "Load Data before trying to expand");
		RETURN_FALSE;
	}
#else
	php_error(E_WARNING, "DOM support is not enabled");
	return;
#endif
}
/* }}} */

static const zend_function_entry xmlreader_functions[] /* {{{ */ =  {
	PHP_ME(xmlreader, close, arginfo_class_XMLReader_close, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getAttribute, arginfo_class_XMLReader_getAttribute, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getAttributeNo, arginfo_class_XMLReader_getAttributeNo, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getAttributeNs, arginfo_class_XMLReader_getAttributeNs, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getParserProperty, arginfo_class_XMLReader_getParserProperty, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, isValid, arginfo_class_XMLReader_isValid, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, lookupNamespace, arginfo_class_XMLReader_lookupNamespace, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToAttributeNo, arginfo_class_XMLReader_moveToAttributeNo, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToAttribute, arginfo_class_XMLReader_moveToAttribute, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToAttributeNs, arginfo_class_XMLReader_moveToAttributeNs, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToElement, arginfo_class_XMLReader_moveToElement, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToFirstAttribute, arginfo_class_XMLReader_moveToFirstAttribute, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToNextAttribute, arginfo_class_XMLReader_moveToNextAttribute, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, open, arginfo_class_XMLReader_open, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(xmlreader, read, arginfo_class_XMLReader_read, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, next, arginfo_class_XMLReader_next, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, readInnerXml, arginfo_class_XMLReader_readInnerXml, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, readOuterXml, arginfo_class_XMLReader_readOuterXml, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, readString, arginfo_class_XMLReader_readString, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, setSchema, arginfo_class_XMLReader_setSchema, ZEND_ACC_PUBLIC)
/* Not Yet Implemented though defined in libxml as of 2.6.9dev
	PHP_ME(xmlreader, resetState, NULL, ZEND_ACC_PUBLIC)
*/
	PHP_ME(xmlreader, setParserProperty, arginfo_class_XMLReader_setParserProperty, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, setRelaxNGSchema, arginfo_class_XMLReader_setRelaxNGSchema, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, setRelaxNGSchemaSource, arginfo_class_XMLReader_setRelaxNGSchemaSource, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, XML, arginfo_class_XMLReader_XML, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(xmlreader, expand, arginfo_class_XMLReader_expand, ZEND_ACC_PUBLIC)
	PHP_FE_END
}; /* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xmlreader)
{

	zend_class_entry ce;

	memcpy(&xmlreader_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	xmlreader_object_handlers.offset = XtOffsetOf(xmlreader_object, std);
	xmlreader_object_handlers.dtor_obj = zend_objects_destroy_object;
	xmlreader_object_handlers.free_obj = xmlreader_objects_free_storage;
	xmlreader_object_handlers.read_property = xmlreader_read_property;
	xmlreader_object_handlers.write_property = xmlreader_write_property;
	xmlreader_object_handlers.get_property_ptr_ptr = xmlreader_get_property_ptr_ptr;
	xmlreader_object_handlers.get_method = xmlreader_get_method;
	xmlreader_object_handlers.clone_obj = NULL;

	INIT_CLASS_ENTRY(ce, "XMLReader", xmlreader_functions);
	ce.create_object = xmlreader_objects_new;
	xmlreader_class_entry = zend_register_internal_class(&ce);

	memcpy(&xmlreader_open_fn, zend_hash_str_find_ptr(&xmlreader_class_entry->function_table, "open", sizeof("open")-1), sizeof(zend_internal_function));
	xmlreader_open_fn.fn_flags &= ~ZEND_ACC_STATIC;
	memcpy(&xmlreader_xml_fn, zend_hash_str_find_ptr(&xmlreader_class_entry->function_table, "xml", sizeof("xml")-1), sizeof(zend_internal_function));
	xmlreader_xml_fn.fn_flags &= ~ZEND_ACC_STATIC;

	zend_hash_init(&xmlreader_prop_handlers, 0, NULL, php_xmlreader_free_prop_handler, 1);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "attributeCount", xmlTextReaderAttributeCount, NULL, IS_LONG);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "baseURI", NULL, xmlTextReaderConstBaseUri, IS_STRING);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "depth", xmlTextReaderDepth, NULL, IS_LONG);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "hasAttributes", xmlTextReaderHasAttributes, NULL, IS_FALSE);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "hasValue", xmlTextReaderHasValue, NULL, IS_FALSE);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "isDefault", xmlTextReaderIsDefault, NULL, IS_FALSE);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "isEmptyElement", xmlTextReaderIsEmptyElement, NULL, IS_FALSE);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "localName", NULL, xmlTextReaderConstLocalName, IS_STRING);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "name", NULL, xmlTextReaderConstName, IS_STRING);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "namespaceURI", NULL, xmlTextReaderConstNamespaceUri, IS_STRING);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "nodeType", xmlTextReaderNodeType, NULL, IS_LONG);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "prefix", NULL, xmlTextReaderConstPrefix, IS_STRING);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "value", NULL, xmlTextReaderConstValue, IS_STRING);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "xmlLang", NULL, xmlTextReaderConstXmlLang, IS_STRING);

	/* Constants for NodeType - cannot define common types to share with dom as there are differences in these types */

	REGISTER_XMLREADER_CLASS_CONST_LONG("NONE",	XML_READER_TYPE_NONE);
	REGISTER_XMLREADER_CLASS_CONST_LONG("ELEMENT",	XML_READER_TYPE_ELEMENT);
	REGISTER_XMLREADER_CLASS_CONST_LONG("ATTRIBUTE",	XML_READER_TYPE_ATTRIBUTE);
	REGISTER_XMLREADER_CLASS_CONST_LONG("TEXT",	XML_READER_TYPE_TEXT);
	REGISTER_XMLREADER_CLASS_CONST_LONG("CDATA",	XML_READER_TYPE_CDATA);
	REGISTER_XMLREADER_CLASS_CONST_LONG("ENTITY_REF",	XML_READER_TYPE_ENTITY_REFERENCE);
	REGISTER_XMLREADER_CLASS_CONST_LONG("ENTITY",	XML_READER_TYPE_ENTITY);
	REGISTER_XMLREADER_CLASS_CONST_LONG("PI",	XML_READER_TYPE_PROCESSING_INSTRUCTION);
	REGISTER_XMLREADER_CLASS_CONST_LONG("COMMENT",	XML_READER_TYPE_COMMENT);
	REGISTER_XMLREADER_CLASS_CONST_LONG("DOC",	XML_READER_TYPE_DOCUMENT);
	REGISTER_XMLREADER_CLASS_CONST_LONG("DOC_TYPE",	XML_READER_TYPE_DOCUMENT_TYPE);
	REGISTER_XMLREADER_CLASS_CONST_LONG("DOC_FRAGMENT",	XML_READER_TYPE_DOCUMENT_FRAGMENT);
	REGISTER_XMLREADER_CLASS_CONST_LONG("NOTATION",	XML_READER_TYPE_NOTATION);
	REGISTER_XMLREADER_CLASS_CONST_LONG("WHITESPACE",	XML_READER_TYPE_WHITESPACE);
	REGISTER_XMLREADER_CLASS_CONST_LONG("SIGNIFICANT_WHITESPACE",	XML_READER_TYPE_SIGNIFICANT_WHITESPACE);
	REGISTER_XMLREADER_CLASS_CONST_LONG("END_ELEMENT",	XML_READER_TYPE_END_ELEMENT);
	REGISTER_XMLREADER_CLASS_CONST_LONG("END_ENTITY",	XML_READER_TYPE_END_ENTITY);
	REGISTER_XMLREADER_CLASS_CONST_LONG("XML_DECLARATION",	XML_READER_TYPE_XML_DECLARATION);

	/* Constants for Parser options */
	REGISTER_XMLREADER_CLASS_CONST_LONG("LOADDTD",	XML_PARSER_LOADDTD);
	REGISTER_XMLREADER_CLASS_CONST_LONG("DEFAULTATTRS",	XML_PARSER_DEFAULTATTRS);
	REGISTER_XMLREADER_CLASS_CONST_LONG("VALIDATE",	XML_PARSER_VALIDATE);
	REGISTER_XMLREADER_CLASS_CONST_LONG("SUBST_ENTITIES",	XML_PARSER_SUBST_ENTITIES);

	/* Constants for Errors when loading - not yet used until we implement custom error handling
	REGISTER_XMLREADER_CLASS_CONST_LONG("VALIDITY_WARNING",		XML_PARSER_SEVERITY_VALIDITY_WARNING,	CONST_CS | CONST_PERSISTENT);
	REGISTER_XMLREADER_CLASS_CONST_LONG("VALIDITY_ERROR",		XML_PARSER_SEVERITY_VALIDITY_ERROR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_XMLREADER_CLASS_CONST_LONG("WARNING",				XML_PARSER_SEVERITY_WARNING,			CONST_CS | CONST_PERSISTENT);
	REGISTER_XMLREADER_CLASS_CONST_LONG("ERROR",				XML_PARSER_SEVERITY_ERROR,				CONST_CS | CONST_PERSISTENT);
	*/

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(xmlreader)
{
	zend_hash_destroy(&xmlreader_prop_handlers);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xmlreader)
{
	php_info_print_table_start();
	{
		php_info_print_table_row(2, "XMLReader", "enabled");
	}
	php_info_print_table_end();
}
/* }}} */
