/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xmlreader.h"
#ifdef HAVE_DOM
#include "ext/dom/xml_common.h"
#endif
#include <libxml/xmlreader.h>
#include <libxml/uri.h>

zend_class_entry *xmlreader_class_entry;

static zend_object_handlers xmlreader_object_handlers;

static HashTable xmlreader_prop_handlers;

typedef int (*xmlreader_read_int_t)(xmlTextReaderPtr reader);
typedef unsigned char *(*xmlreader_read_char_t)(xmlTextReaderPtr reader);
typedef const unsigned char *(*xmlreader_read_const_char_t)(xmlTextReaderPtr reader);
typedef int (*xmlreader_write_t)(xmlreader_object *obj, zval *newval TSRMLS_DC);

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
static void xmlreader_register_prop_handler(HashTable *prop_handler, char *name, xmlreader_read_int_t read_int_func, xmlreader_read_const_char_t read_char_func, int rettype TSRMLS_DC)
{
	xmlreader_prop_handler hnd;
	
	hnd.read_char_func = read_char_func;
	hnd.read_int_func = read_int_func;
	hnd.type = rettype;
	zend_hash_add(prop_handler, name, strlen(name)+1, &hnd, sizeof(xmlreader_prop_handler), NULL);
}
/* }}} */

/* {{{ xmlreader_property_reader */
static int xmlreader_property_reader(xmlreader_object *obj, xmlreader_prop_handler *hnd, zval **retval TSRMLS_DC)
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
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal libxml error returned");
					return FAILURE;
				}
			}
		}
	}

	ALLOC_ZVAL(*retval);

	switch (hnd->type) {
		case IS_STRING:
			if (retchar) {
				ZVAL_STRING(*retval, (char *) retchar, 1);
			} else {
				ZVAL_EMPTY_STRING(*retval);
			}
			break;
		case IS_BOOL:
			ZVAL_BOOL(*retval, retint);
			break;
		case IS_LONG:
			ZVAL_LONG(*retval, retint);
			break;
		default:
			ZVAL_NULL(*retval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ xmlreader_get_property_ptr_ptr */
zval **xmlreader_get_property_ptr_ptr(zval *object, zval *member, const zend_literal *key TSRMLS_DC)
{
	xmlreader_object *obj;
	zval tmp_member;
	zval **retval = NULL;
	xmlreader_prop_handler *hnd;
	zend_object_handlers *std_hnd;
	int ret = FAILURE;

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	obj = (xmlreader_object *)zend_objects_get_address(object TSRMLS_CC);

	if (obj->prop_handler != NULL) {
		ret = zend_hash_find(obj->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
	}
	if (ret == FAILURE) {
		std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->get_property_ptr_ptr(object, member, key TSRMLS_CC);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return retval;
}
/* }}} */

/* {{{ xmlreader_read_property */
zval *xmlreader_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC)
{
	xmlreader_object *obj;
	zval tmp_member;
	zval *retval;
	xmlreader_prop_handler *hnd;
	zend_object_handlers *std_hnd;
	int ret;

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	ret = FAILURE;
	obj = (xmlreader_object *)zend_objects_get_address(object TSRMLS_CC);

	if (obj->prop_handler != NULL) {
		ret = zend_hash_find(obj->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
	}
	if (ret == SUCCESS) {
		ret = xmlreader_property_reader(obj, hnd, &retval TSRMLS_CC);
		if (ret == SUCCESS) {
			/* ensure we're creating a temporary variable */
			Z_SET_REFCOUNT_P(retval, 0);
		} else {
			retval = EG(uninitialized_zval_ptr);
		}
	} else {
		std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->read_property(object, member, type, key TSRMLS_CC);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return retval;
}
/* }}} */

/* {{{ xmlreader_write_property */
void xmlreader_write_property(zval *object, zval *member, zval *value, const zend_literal *key TSRMLS_DC)
{
	xmlreader_object *obj;
	zval tmp_member;
	xmlreader_prop_handler *hnd;
	zend_object_handlers *std_hnd;
	int ret;

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	ret = FAILURE;
	obj = (xmlreader_object *)zend_objects_get_address(object TSRMLS_CC);

	if (obj->prop_handler != NULL) {
		ret = zend_hash_find((HashTable *)obj->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
	}
	if (ret == SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot write to read-only property");
	} else {
		std_hnd = zend_get_std_object_handlers();
		std_hnd->write_property(object, member, value, key TSRMLS_CC);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
}
/* }}} */

/* {{{ _xmlreader_get_valid_file_path */
/* _xmlreader_get_valid_file_path and _xmlreader_get_relaxNG should be made a 
	common function in libxml extension as code is common to a few xml extensions */
char *_xmlreader_get_valid_file_path(char *source, char *resolved_path, int resolved_path_len  TSRMLS_DC) {
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
		if (!VCWD_REALPATH(source, resolved_path) && !expand_filepath(source, resolved_path TSRMLS_CC)) {
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
static xmlRelaxNGPtr _xmlreader_get_relaxNG(char *source, int source_len, int type, 
											xmlRelaxNGValidityErrorFunc error_func, 
											xmlRelaxNGValidityWarningFunc warn_func TSRMLS_DC)
{
	char *valid_file = NULL;
	xmlRelaxNGParserCtxtPtr parser = NULL;
	xmlRelaxNGPtr           sptr;
	char resolved_path[MAXPATHLEN + 1];

	switch (type) {
	case XMLREADER_LOAD_FILE:
		valid_file = _xmlreader_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
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
	"0.1", /* Replace with version number for your extension */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XMLREADER
ZEND_GET_MODULE(xmlreader)
#endif

/* {{{ xmlreader_objects_clone */
void xmlreader_objects_clone(void *object, void **object_clone TSRMLS_DC)
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
void xmlreader_objects_free_storage(void *object TSRMLS_DC)
{
	xmlreader_object *intern = (xmlreader_object *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	
	xmlreader_free_resources(intern);

	efree(object);
}
/* }}} */

/* {{{ xmlreader_objects_new */
zend_object_value xmlreader_objects_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	xmlreader_object *intern;

	intern = emalloc(sizeof(xmlreader_object));
	memset(&intern->std, 0, sizeof(zend_object));
	intern->ptr = NULL;
	intern->input = NULL;
	intern->schema = NULL;
	intern->prop_handler = &xmlreader_prop_handlers;

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) xmlreader_objects_free_storage, xmlreader_objects_clone TSRMLS_CC);
	intern->handle = retval.handle;
	retval.handlers = &xmlreader_object_handlers;
	return retval;
}
/* }}} */

/* {{{ php_xmlreader_string_arg */
static void php_xmlreader_string_arg(INTERNAL_FUNCTION_PARAMETERS, xmlreader_read_one_char_t internal_function) {
	zval *id;
	int name_len = 0;
	char *retchar = NULL;
	xmlreader_object *intern;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	if (!name_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument cannot be an empty string");
		RETURN_FALSE;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retchar = (char *)internal_function(intern->ptr, (const unsigned char *)name);
	}
	if (retchar) {
		RETVAL_STRING(retchar, 1);
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

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retval = internal_function(intern->ptr);
		if (retval == 1) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

#if LIBXML_VERSION >= 20620
/* {{{ php_xmlreader_no_arg_string */
static void php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAMETERS, xmlreader_read_char_t internal_function) {
	zval *id;
	char *retchar = NULL;
	xmlreader_object *intern;

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retchar = (char *)internal_function(intern->ptr);
	}
	if (retchar) {
		RETVAL_STRING(retchar, 1);
		xmlFree(retchar);
		return;
	} else {
		RETVAL_EMPTY_STRING();
	}
}
/* }}} */
#endif

/* {{{ php_xmlreader_set_relaxng_schema */
static void php_xmlreader_set_relaxng_schema(INTERNAL_FUNCTION_PARAMETERS, int type) {
#ifdef LIBXML_SCHEMAS_ENABLED
	zval *id;
	int source_len = 0, retval = -1;
	xmlreader_object *intern;
	xmlRelaxNGPtr schema = NULL;
	char *source;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p!", &source, &source_len) == FAILURE) {
		return;
	}

	if (source != NULL && !source_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Schema data source is required");
		RETURN_FALSE;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		if (source) {
			schema =  _xmlreader_get_relaxNG(source, source_len, type, NULL, NULL TSRMLS_CC);
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
	
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set schema. This must be set prior to reading or schema contains errors.");

	RETURN_FALSE;
#else
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "No Schema support built into libxml.");

	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto boolean XMLReader::close()
Closes xmlreader - current frees resources until xmlTextReaderClose is fixed in libxml */
PHP_METHOD(xmlreader, close)
{
	zval *id;
	xmlreader_object *intern;

	id = getThis();
	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
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
	long attr_pos;
	char *retchar = NULL;
	xmlreader_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &attr_pos) == FAILURE) {
		return;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retchar = (char *)xmlTextReaderGetAttributeNo(intern->ptr, attr_pos);
	}
	if (retchar) {
		RETVAL_STRING(retchar, 1);
		xmlFree(retchar);
		return;
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string XMLReader::getAttributeNs(string name, string namespaceURI)
Get value of a attribute via name and namespace from current element */
PHP_METHOD(xmlreader, getAttributeNs)
{
	zval *id;
	int name_len = 0, ns_uri_len = 0;
	xmlreader_object *intern;
	char *name, *ns_uri, *retchar = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &ns_uri, &ns_uri_len) == FAILURE) {
		return;
	}

	if (name_len == 0 || ns_uri_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attribute Name and Namespace URI cannot be empty");
		RETURN_FALSE;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retchar = (char *)xmlTextReaderGetAttributeNs(intern->ptr, (xmlChar *)name, (xmlChar *)ns_uri);
	}
	if (retchar) {
		RETVAL_STRING(retchar, 1);
		xmlFree(retchar);
		return;
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto boolean XMLReader::getParserProperty(int property)
Indicates whether given property (one of the parser option constants) is set or not on parser */
PHP_METHOD(xmlreader, getParserProperty)
{
	zval *id;
	long property;
	int retval = -1;
	xmlreader_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &property) == FAILURE) {
		return;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retval = xmlTextReaderGetParserProp(intern->ptr,property);
	}
	if (retval == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parser property");
		RETURN_FALSE;
	}

	RETURN_BOOL(retval);
}
/* }}} */

/* {{{ proto boolean XMLReader::isValid()
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

/* {{{ proto boolean XMLReader::moveToAttribute(string name)
Positions reader at specified attribute - Returns TRUE on success and FALSE on failure */
PHP_METHOD(xmlreader, moveToAttribute)
{
	zval *id;
	int name_len = 0, retval;
	xmlreader_object *intern;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	if (name_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attribute Name is required");
		RETURN_FALSE;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retval = xmlTextReaderMoveToAttribute(intern->ptr, (xmlChar *)name);
		if (retval == 1) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean XMLReader::moveToAttributeNo(int index)
Positions reader at attribute at spcecified index.
Returns TRUE on success and FALSE on failure */
PHP_METHOD(xmlreader, moveToAttributeNo)
{
	zval *id;
	long attr_pos;
	int retval;
	xmlreader_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &attr_pos) == FAILURE) {
		return;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retval = xmlTextReaderMoveToAttributeNo(intern->ptr, attr_pos);
		if (retval == 1) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean XMLReader::moveToAttributeNs(string name, string namespaceURI)
Positions reader at attribute spcified by name and namespaceURI.
Returns TRUE on success and FALSE on failure */
PHP_METHOD(xmlreader, moveToAttributeNs)
{
	zval *id;
	int name_len=0, ns_uri_len=0, retval;
	xmlreader_object *intern;
	char *name, *ns_uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &ns_uri, &ns_uri_len) == FAILURE) {
		return;
	}

	if (name_len == 0 || ns_uri_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attribute Name and Namespace URI cannot be empty");
		RETURN_FALSE;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retval = xmlTextReaderMoveToAttributeNs(intern->ptr, (xmlChar *)name, (xmlChar *)ns_uri);
		if (retval == 1) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean XMLReader::moveToElement()
Moves the position of the current instance to the node that contains the current Attribute node. */
PHP_METHOD(xmlreader, moveToElement)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderMoveToElement);
}
/* }}} */

/* {{{ proto boolean XMLReader::moveToFirstAttribute()
Moves the position of the current instance to the first attribute associated with the current node. */
PHP_METHOD(xmlreader, moveToFirstAttribute)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderMoveToFirstAttribute);
}
/* }}} */

/* {{{ proto boolean XMLReader::moveToNextAttribute()
Moves the position of the current instance to the next attribute associated with the current node. */
PHP_METHOD(xmlreader, moveToNextAttribute)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderMoveToNextAttribute);
}
/* }}} */

/* {{{ proto boolean XMLReader::read()
Moves the position of the current instance to the next node in the stream. */
PHP_METHOD(xmlreader, read)
{
	zval *id;
	int retval;
	xmlreader_object *intern;

	id = getThis();
	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL && intern->ptr != NULL) {
		retval = xmlTextReaderRead(intern->ptr);
		if (retval == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "An Error Occured while reading");
			RETURN_FALSE;
		} else {
			RETURN_BOOL(retval);
		}
	}
	
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Load Data before trying to read");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean XMLReader::next([string localname])
Moves the position of the current instance to the next node in the stream. */
PHP_METHOD(xmlreader, next)
{
	zval *id;
	int retval, name_len=0;
	xmlreader_object *intern;
	char *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	id = getThis();
	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL && intern->ptr != NULL) {
#if LIBXML_VERSION <= 20617
		/* Bug in libxml prevents a next in certain cases when positioned on end of element */
		if (xmlTextReaderNodeType(intern->ptr) == XML_READER_TYPE_END_ELEMENT) {
			retval = xmlTextReaderRead(intern->ptr);
		} else
#endif
		retval = xmlTextReaderNext(intern->ptr);
		while (name != NULL && retval == 1) {
			if (xmlStrEqual(xmlTextReaderConstLocalName(intern->ptr), (xmlChar *)name)) {
				RETURN_TRUE;
			}
			retval = xmlTextReaderNext(intern->ptr); 
		}
		if (retval == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "An Error Occured while reading");
			RETURN_FALSE;
		} else {
			RETURN_BOOL(retval);
		}
	}
	
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Load Data before trying to read");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean XMLReader::open(string URI [, string encoding [, int options]])
Sets the URI that the XMLReader will parse. */
PHP_METHOD(xmlreader, open)
{
	zval *id;
	int source_len = 0, encoding_len = 0;
	long options = 0;
	xmlreader_object *intern = NULL;
	char *source, *valid_file = NULL;
	char *encoding = NULL;
	char resolved_path[MAXPATHLEN + 1];
	xmlTextReaderPtr reader = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|s!l", &source, &source_len, &encoding, &encoding_len, &options) == FAILURE) {
		return;
	}

	id = getThis();
	if (id != NULL) {
		if (! instanceof_function(Z_OBJCE_P(id), xmlreader_class_entry TSRMLS_CC)) {
			id = NULL;
		} else {
			intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
			xmlreader_free_resources(intern);
		}
	}

	if (!source_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string supplied as input");
		RETURN_FALSE;
	}

	valid_file = _xmlreader_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);

	if (valid_file) {
		reader = xmlReaderForFile(valid_file, encoding, options);
	}

	if (reader == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open source data");
		RETURN_FALSE;
	}

	if (id == NULL) {
		object_init_ex(return_value, xmlreader_class_entry);
		intern = (xmlreader_object *)zend_objects_get_address(return_value TSRMLS_CC);
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

#if LIBXML_VERSION >= 20620
/* {{{ proto string XMLReader::readInnerXml()
Reads the contents of the current node, including child nodes and markup. */
PHP_METHOD(xmlreader, readInnerXml)
{
	php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderReadInnerXml);
}
/* }}} */

/* {{{ proto boolean XMLReader::readOuterXml()
Reads the contents of the current node, including child nodes and markup. */
PHP_METHOD(xmlreader, readOuterXml)
{
	php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderReadOuterXml);
}
/* }}} */

/* {{{ proto boolean XMLReader::readString()
Reads the contents of an element or a text node as a string. */
PHP_METHOD(xmlreader, readString)
{
	php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderReadString);
}
/* }}} */

/* {{{ proto boolean XMLReader::setSchema(string filename)
Use W3C XSD schema to validate the document as it is processed. Activation is only possible before the first Read(). */
PHP_METHOD(xmlreader, setSchema)
{
#ifdef LIBXML_SCHEMAS_ENABLED
	zval *id;
	int source_len = 0, retval = -1;
	xmlreader_object *intern;
	char *source;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p!", &source, &source_len) == FAILURE) {
		return;
	}

	if (source != NULL && !source_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Schema data source is required");
		RETURN_FALSE;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retval = xmlTextReaderSchemaValidate(intern->ptr, source);

		if (retval == 0) {
			RETURN_TRUE;
		}
	}
	
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set schema. This must be set prior to reading or schema contains errors.");

	RETURN_FALSE;
#else
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "No Schema support built into libxml.");

	RETURN_FALSE;
#endif
}
/* }}} */
#endif

/* {{{ proto boolean XMLReader::setParserProperty(int property, boolean value)
Sets parser property (one of the parser option constants).
Properties must be set after open() or XML() and before the first read() is called */
PHP_METHOD(xmlreader, setParserProperty)
{
	zval *id;
	long property;
	int retval = -1;
	zend_bool value;
	xmlreader_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lb", &property, &value) == FAILURE) {
		return;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retval = xmlTextReaderSetParserProp(intern->ptr,property, value);
	}
	if (retval == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parser property");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean XMLReader::setRelaxNGSchema(string filename)
Sets the string that the XMLReader will parse. */
PHP_METHOD(xmlreader, setRelaxNGSchema)
{
	php_xmlreader_set_relaxng_schema(INTERNAL_FUNCTION_PARAM_PASSTHRU, XMLREADER_LOAD_FILE);
}
/* }}} */

/* {{{ proto boolean XMLReader::setRelaxNGSchemaSource(string source)
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

/* {{{ proto boolean XMLReader::XML(string source [, string encoding [, int options]])
Sets the string that the XMLReader will parse. */
PHP_METHOD(xmlreader, XML)
{
	zval *id;
	int source_len = 0, encoding_len = 0;
	long options = 0;
	xmlreader_object *intern = NULL;
	char *source, *uri = NULL, *encoding = NULL;
	int resolved_path_len, ret = 0;
	char *directory=NULL, resolved_path[MAXPATHLEN];
	xmlParserInputBufferPtr inputbfr;
	xmlTextReaderPtr reader;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!l", &source, &source_len, &encoding, &encoding_len, &options) == FAILURE) {
		return;
	}

	id = getThis();
	if (id != NULL && ! instanceof_function(Z_OBJCE_P(id), xmlreader_class_entry TSRMLS_CC)) {
		id = NULL;
	}
	if (id != NULL) {
		intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
		xmlreader_free_resources(intern);
	}

	if (!source_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string supplied as input");
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
#if LIBXML_VERSION >= 20628
			ret = xmlTextReaderSetup(reader, NULL, uri, encoding, options);
#endif
			if (ret == 0) {
				if (id == NULL) {
					object_init_ex(return_value, xmlreader_class_entry);
					intern = (xmlreader_object *)zend_objects_get_address(return_value TSRMLS_CC);
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
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load source data");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean XMLReader::expand()
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

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|O!", &id, xmlreader_class_entry, &basenode, dom_node_class_entry) == FAILURE) {
		return;
	}
	
	if (basenode != NULL) {
		NODE_GET_OBJ(node, basenode, xmlNodePtr, domobj);
		docp = node->doc;
	}

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);

	if (intern && intern->ptr) {
		node = xmlTextReaderExpand(intern->ptr);
		
		if (node == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "An Error Occured while expanding ");
			RETURN_FALSE;
		} else {
			nodec = xmlDocCopyNode(node, docp, 1);
			if (nodec == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot expand this node type");
				RETURN_FALSE;
			} else {
				DOM_RET_OBJ(nodec, &ret, (dom_object *)domobj);
			}
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Load Data before trying to expand");
		RETURN_FALSE;
	}
#else
	php_error(E_WARNING, "DOM support is not enabled");
	return;
#endif
}
/* }}} */
/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_close, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_getAttribute, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_getAttributeNo, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_getAttributeNs, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, namespaceURI)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_getParserProperty, 0)
	ZEND_ARG_INFO(0, property)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_isValid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_lookupNamespace, 0)
ZEND_ARG_INFO(0, prefix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_moveToAttribute, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_moveToAttributeNo, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_moveToAttributeNs, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, namespaceURI)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_moveToElement, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_moveToFirstAttribute, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_moveToNextAttribute, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_read, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlreader_next, 0, 0, 0)
	ZEND_ARG_INFO(0, localname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlreader_open, 0, 0, 1)
	ZEND_ARG_INFO(0, URI)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_readInnerXml, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_readOuterXml, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_readString, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_setSchema, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_setParserProperty, 0)
	ZEND_ARG_INFO(0, property)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_setRelaxNGSchema, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_setRelaxNGSchemaSource, 0)
	ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlreader_XML, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xmlreader_expand, 0)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry xmlreader_functions[] = {
	PHP_ME(xmlreader, close, arginfo_xmlreader_close, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getAttribute, arginfo_xmlreader_getAttribute, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getAttributeNo, arginfo_xmlreader_getAttributeNo, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getAttributeNs, arginfo_xmlreader_getAttributeNs, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getParserProperty, arginfo_xmlreader_getParserProperty, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, isValid, arginfo_xmlreader_isValid, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, lookupNamespace, arginfo_xmlreader_lookupNamespace, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToAttributeNo, arginfo_xmlreader_moveToAttributeNo, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToAttribute, arginfo_xmlreader_moveToAttribute, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToAttributeNs, arginfo_xmlreader_moveToAttributeNs, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToElement, arginfo_xmlreader_moveToElement, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToFirstAttribute, arginfo_xmlreader_moveToFirstAttribute, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToNextAttribute, arginfo_xmlreader_moveToNextAttribute, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, open, arginfo_xmlreader_open, ZEND_ACC_PUBLIC|ZEND_ACC_ALLOW_STATIC)
	PHP_ME(xmlreader, read, arginfo_xmlreader_read, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, next, arginfo_xmlreader_next, ZEND_ACC_PUBLIC)
#if LIBXML_VERSION >= 20620
	PHP_ME(xmlreader, readInnerXml, arginfo_xmlreader_readInnerXml, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, readOuterXml, arginfo_xmlreader_readOuterXml, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, readString, arginfo_xmlreader_readString, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, setSchema, arginfo_xmlreader_setSchema, ZEND_ACC_PUBLIC)
#endif
/* Not Yet Implemented though defined in libxml as of 2.6.9dev
	PHP_ME(xmlreader, resetState, NULL, ZEND_ACC_PUBLIC)
*/
	PHP_ME(xmlreader, setParserProperty, arginfo_xmlreader_setParserProperty, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, setRelaxNGSchema, arginfo_xmlreader_setRelaxNGSchema, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, setRelaxNGSchemaSource, arginfo_xmlreader_setRelaxNGSchemaSource, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, XML, arginfo_xmlreader_XML, ZEND_ACC_PUBLIC|ZEND_ACC_ALLOW_STATIC)
	PHP_ME(xmlreader, expand, arginfo_xmlreader_expand, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xmlreader)
{
	
	zend_class_entry ce;
	
	memcpy(&xmlreader_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	xmlreader_object_handlers.read_property = xmlreader_read_property;
	xmlreader_object_handlers.write_property = xmlreader_write_property;
	xmlreader_object_handlers.get_property_ptr_ptr = xmlreader_get_property_ptr_ptr;

	INIT_CLASS_ENTRY(ce, "XMLReader", xmlreader_functions);
	ce.create_object = xmlreader_objects_new;
	xmlreader_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	zend_hash_init(&xmlreader_prop_handlers, 0, NULL, NULL, 1);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "attributeCount", xmlTextReaderAttributeCount, NULL, IS_LONG TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "baseURI", NULL, xmlTextReaderConstBaseUri, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "depth", xmlTextReaderDepth, NULL, IS_LONG TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "hasAttributes", xmlTextReaderHasAttributes, NULL, IS_BOOL TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "hasValue", xmlTextReaderHasValue, NULL, IS_BOOL TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "isDefault", xmlTextReaderIsDefault, NULL, IS_BOOL TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "isEmptyElement", xmlTextReaderIsEmptyElement, NULL, IS_BOOL TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "localName", NULL, xmlTextReaderConstLocalName, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "name", NULL, xmlTextReaderConstName, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "namespaceURI", NULL, xmlTextReaderConstNamespaceUri, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "nodeType", xmlTextReaderNodeType, NULL, IS_LONG TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "prefix", NULL, xmlTextReaderConstPrefix, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "value", NULL, xmlTextReaderConstValue, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "xmlLang", NULL, xmlTextReaderConstXmlLang, IS_STRING TSRMLS_CC);

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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
