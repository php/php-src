/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
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
#include <libxml/uri.h>

zend_class_entry *xmlreader_class_entry;

static zend_object_handlers xmlreader_object_handlers;

static HashTable xmlreader_prop_handlers;

typedef int (*xmlreader_read_int_t)(xmlTextReaderPtr reader);
typedef unsigned char *(*xmlreader_read_char_t)(xmlTextReaderPtr reader);
typedef int (*xmlreader_write_t)(xmlreader_object *obj, zval *newval TSRMLS_DC);

typedef unsigned char *(*xmlreader_read_one_char_t)(xmlTextReaderPtr reader, const unsigned char *);

typedef struct _xmlreader_prop_handler {
	xmlreader_read_int_t read_int_func;
	xmlreader_read_char_t read_char_func;
	xmlreader_write_t write_func;
	int type;
} xmlreader_prop_handler;

#define XMLREADER_LOAD_STRING 0
#define XMLREADER_LOAD_FILE 1

static void xmlreader_register_prop_handler(HashTable *prop_handler, char *name, xmlreader_read_int_t read_int_func, xmlreader_read_char_t read_char_func, int rettype TSRMLS_DC)
{
	xmlreader_prop_handler hnd;
	
	hnd.read_char_func = read_char_func;
	hnd.read_int_func = read_int_func;
	hnd.type = rettype;
	zend_hash_add(prop_handler, name, strlen(name)+1, &hnd, sizeof(xmlreader_prop_handler), NULL);
}

static int xmlreader_property_reader(xmlreader_object *obj, xmlreader_prop_handler *hnd, zval **retval TSRMLS_DC)
{
	char *retchar = NULL;
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
				ZVAL_STRING(*retval, retchar, 1);
				xmlFree(retchar);
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

/* {{{ xmlreader_read_property */
zval *xmlreader_read_property(zval *object, zval *member, int type TSRMLS_DC)
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
			retval->refcount = 0;
		} else {
			retval = EG(uninitialized_zval_ptr);
		}
	} else {
		std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->read_property(object, member, type TSRMLS_CC);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return retval;
}
/* }}} */

/* {{{ xmlreader_write_property */
void xmlreader_write_property(zval *object, zval *member, zval *value TSRMLS_DC)
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
		std_hnd->write_property(object, member, value TSRMLS_CC);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
}
/* }}} */

/* _xmlreader_get_valid_file_path and _xmlreader_get_relaxNG should be made a 
	common function in libxml extension as code is common to a few xml extensions */
char *_xmlreader_get_valid_file_path(char *source, char *resolved_path, int resolved_path_len  TSRMLS_DC) {
	xmlURI *uri;
	xmlChar *escsource;
	char *file_dest;
	int isFileUri = 0;

	uri = xmlCreateURI();
	escsource = xmlURIEscapeStr(source, ":");
	xmlParseURIReference(uri, escsource);
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
		/* XXX possible buffer overflow if VCWD_REALPATH does not know size of resolved_path */
		if (! VCWD_REALPATH(source, resolved_path)) {
			expand_filepath(source, resolved_path TSRMLS_CC);
		}
		file_dest = resolved_path;
	}

	xmlFreeURI(uri);

	return file_dest;
}

#ifdef LIBXML_SCHEMAS_ENABLED
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
#endif

/* {{{ xmlreader_module_entry
 */
zend_module_entry xmlreader_module_entry = {
	STANDARD_MODULE_HEADER,
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

/* {{{ xmlreader_objects_free_storage */
void xmlreader_objects_free_storage(void *object TSRMLS_DC)
{
	xmlreader_object *intern = (xmlreader_object *)object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);
	
	xmlreader_free_resources(intern);

	efree(object);
}
/* }}} */

/* {{{ xmlreader_objects_new */
zend_object_value xmlreader_objects_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	xmlreader_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(xmlreader_object));
	intern->std.ce = class_type;
	intern->std.in_get = 0;
	intern->std.in_set = 0;
	intern->ptr = NULL;
	intern->input = NULL;
	intern->schema = NULL;
	intern->prop_handler = &xmlreader_prop_handlers;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) xmlreader_objects_free_storage, xmlreader_objects_clone TSRMLS_CC);
	intern->handle = retval.handle;
	retval.handlers = &xmlreader_object_handlers;
	return retval;
}
/* }}} */

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
		retchar = internal_function(intern->ptr, name);
	}
	if (retchar) {
		RETVAL_STRING(retchar, 1);
		xmlFree(retchar);
		return;
	} else {
		RETVAL_EMPTY_STRING();
	}
}

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

/* This function not yet needed until some additional functions are implemented in libxml
static void php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAMETERS, xmlreader_read_char_t internal_function) {
	zval *id;
	char *retchar = NULL;
	xmlreader_object *intern;

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retchar = xmlTextReaderReadString(intern->ptr);
	}
	if (retchar) {
		RETVAL_STRING(retchar, 1);
		xmlFree(retchar);
		return;
	} else {
		RETVAL_EMPTY_STRING();
	}
}
*/

static void php_xmlreader_set_relaxng_schema(INTERNAL_FUNCTION_PARAMETERS, int type) {
#ifdef LIBXML_SCHEMAS_ENABLED
	zval *id;
	int source_len = 0, retval = -1;
	xmlreader_object *intern;
	xmlRelaxNGPtr schema = NULL;
	char *source;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!", &source, &source_len) == FAILURE) {
		return;
	}

	if (source != NULL && !source_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Schema data source is requried");
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

/* {{{ proto boolean close()
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

/* {{{ proto string getAttribute(string name)
Get value of an attribute from current element */
PHP_METHOD(xmlreader, getAttribute)
{
	php_xmlreader_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderGetAttribute);
}
/* }}} */

/* {{{ proto string getAttributeNo(int index)
Get value of an attribute at index from current element */
PHP_METHOD(xmlreader, getAttributeNo)
{
	zval *id;
	int attr_pos;
	char *retchar = NULL;
	xmlreader_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &attr_pos) == FAILURE) {
		return;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern && intern->ptr) {
		retchar = xmlTextReaderGetAttributeNo(intern->ptr,attr_pos);
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

/* {{{ proto string getAttributeNs(string name, string namespaceURI)
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
		retchar = xmlTextReaderGetAttributeNs(intern->ptr, name, ns_uri);
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

/* {{{ proto boolean getParserProperty(int property)
Indicates wether given property (one of the parser option constants) is set or not on parser */
PHP_METHOD(xmlreader, getParserProperty)
{
	zval *id;
	int property, retval = -1;
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

/* {{{ proto boolean isValid()
Returns boolean indicating if parsed document is valid or not.
Must set XMLREADER_LOADDTD or XMLREADER_VALIDATE parser option prior to the first call to read 
or this method will always return FALSE */
PHP_METHOD(xmlreader, isValid)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderIsValid);
}
/* }}} */

/* {{{ proto string lookupNamespace(string prefix)
Return namespaceURI for associated prefix on current node */
PHP_METHOD(xmlreader, lookupNamespace)
{
	php_xmlreader_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderLookupNamespace);
}
/* }}} */

/* {{{ proto boolean moveToAttribute(string name)
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
		retval = xmlTextReaderMoveToAttribute(intern->ptr, name);
		if (retval == 1) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean moveToAttributeNo(int index)
Positions reader at attribute at spcecified index.
Returns TRUE on success and FALSE on failure */
PHP_METHOD(xmlreader, moveToAttributeNo)
{
	zval *id;
	int attr_pos, retval;
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

/* {{{ proto boolean moveToAttributeNs(string name, string namespaceURI)
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
		retval = xmlTextReaderMoveToAttributeNs(intern->ptr, name, ns_uri);
		if (retval == 1) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean moveToElement()
Moves the position of the current instance to the node that contains the current Attribute node. */
PHP_METHOD(xmlreader, moveToElement)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderMoveToElement);
}
/* }}} */

/* {{{ proto boolean moveToFirstAttribute()
Moves the position of the current instance to the first attribute associated with the current node. */
PHP_METHOD(xmlreader, moveToFirstAttribute)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderMoveToFirstAttribute);
}
/* }}} */

/* {{{ proto boolean moveToNextAttribute()
Moves the position of the current instance to the next attribute associated with the current node. */
PHP_METHOD(xmlreader, moveToNextAttribute)
{
	php_xmlreader_no_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderMoveToNextAttribute);
}
/* }}} */

/* {{{ proto boolean read()
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

/* {{{ proto boolean open(string URI)
Sets the URI that the the XMLReader will parse. */
PHP_METHOD(xmlreader, open)
{
	zval *id;
	int source_len = 0;
	xmlreader_object *intern;
	char *source, *valid_file = NULL;
	char resolved_path[MAXPATHLEN + 1];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &source_len) == FAILURE) {
		return;
	}

	if (!source_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string supplied as input");
		RETURN_FALSE;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);

	xmlreader_free_resources(intern);

	valid_file = _xmlreader_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);

	if (valid_file) {
		intern->ptr = xmlNewTextReaderFilename(valid_file);
	}

	if (intern->ptr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open source data");
		RETURN_FALSE;
	}

	RETURN_TRUE;

}
/* }}} */

/* Not Yet Implemented in libxml - functions exist just not coded
PHP_METHOD(xmlreader, readInnerXml)
{
	php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderReadInnerXml);
}

PHP_METHOD(xmlreader, readOuterXml)
{
	php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderReadInnerXml);
}

PHP_METHOD(xmlreader, readString)
{
	php_xmlreader_no_arg_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextReaderReadString);
}

PHP_METHOD(xmlreader, resetState)
{

}
*/

/* {{{ proto boolean setParserProperty(int property, boolean value)
Sets parser property (one of the parser option constants).
Properties must be set after open() or XML() and before the first read() is called */
PHP_METHOD(xmlreader, setParserProperty)
{
	zval *id;
	int property, value, retval = -1;
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

/* {{{ proto boolean setRelaxNGSchemaSource(string filename)
Sets the string that the the XMLReader will parse. */
PHP_METHOD(xmlreader, setRelaxNGSchema)
{
	php_xmlreader_set_relaxng_schema(INTERNAL_FUNCTION_PARAM_PASSTHRU, XMLREADER_LOAD_FILE);
}
/* }}} */

/* {{{ proto boolean setRelaxNGSchemaSource(string source)
Sets the string that the the XMLReader will parse. */
PHP_METHOD(xmlreader, setRelaxNGSchemaSource)
{
	php_xmlreader_set_relaxng_schema(INTERNAL_FUNCTION_PARAM_PASSTHRU, XMLREADER_LOAD_STRING);
}
/* }}} */

/* {{{ proto boolean XML(string source)
Sets the string that the the XMLReader will parse. */
PHP_METHOD(xmlreader, XML)
{
	zval *id;
	int source_len = 0;
	xmlreader_object *intern;
	char *source, *uri = NULL;
	int resolved_path_len;
	char *directory=NULL, resolved_path[MAXPATHLEN];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &source_len) == FAILURE) {
		return;
	}

	if (!source_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string supplied as input");
		RETURN_FALSE;
	}

	id = getThis();

	intern = (xmlreader_object *)zend_object_store_get_object(id TSRMLS_CC);
	
	xmlreader_free_resources(intern);

	intern->input = xmlParserInputBufferCreateMem(source, source_len, XML_CHAR_ENCODING_NONE);

    if (intern->input != NULL) {
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
		intern->ptr = xmlNewTextReader(intern->input, uri);
		if (uri) {
			xmlFree(uri);
		}
		if (intern->ptr != NULL) {
			RETURN_TRUE;
		}
	}

	xmlreader_free_resources(intern);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load source data");
	RETURN_FALSE;
}
/* }}} */

static zend_function_entry xmlreader_functions[] = {
	PHP_ME(xmlreader, close, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getAttribute, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getAttributeNo, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getAttributeNs, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, getParserProperty, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, isValid, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, lookupNamespace, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToAttributeNo, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToAttribute, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToAttributeNs, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToElement, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToFirstAttribute, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, moveToNextAttribute, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, open, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, read, NULL, ZEND_ACC_PUBLIC)
/* Not Yet Implemented though defined in libxml as of 2.6.9dev
	PHP_ME(xmlreader, readInnerXml, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, readOuterXml, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, readString, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, resetState, NULL, ZEND_ACC_PUBLIC)
*/
	PHP_ME(xmlreader, setParserProperty, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, setRelaxNGSchema, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, setRelaxNGSchemaSource, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(xmlreader, XML, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xmlreader)
{
	
	zend_class_entry ce;
	
	memcpy(&xmlreader_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	xmlreader_object_handlers.read_property = xmlreader_read_property;
	xmlreader_object_handlers.write_property = xmlreader_write_property;
	xmlreader_object_handlers.get_property_ptr_ptr = NULL;

	INIT_CLASS_ENTRY(ce, "XMLReader", xmlreader_functions);
	ce.create_object = xmlreader_objects_new;
	xmlreader_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	zend_hash_init(&xmlreader_prop_handlers, 0, NULL, NULL, 1);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "attributeCount", xmlTextReaderAttributeCount, NULL, IS_LONG TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "baseURI", NULL, xmlTextReaderBaseUri, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "depth", xmlTextReaderDepth, NULL, IS_LONG TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "hasAttributes", xmlTextReaderHasAttributes, NULL, IS_BOOL TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "hasValue", xmlTextReaderHasValue, NULL, IS_BOOL TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "isDefault", xmlTextReaderIsDefault, NULL, IS_BOOL TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "isEmptyElement", xmlTextReaderIsEmptyElement, NULL, IS_BOOL TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "localName", NULL, xmlTextReaderLocalName, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "name", NULL, xmlTextReaderName, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "namespaceURI", NULL, xmlTextReaderNamespaceUri, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "nodeType", xmlTextReaderNodeType, NULL, IS_LONG TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "prefix", NULL, xmlTextReaderPrefix, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "value", NULL, xmlTextReaderValue, IS_STRING TSRMLS_CC);
	xmlreader_register_prop_handler(&xmlreader_prop_handlers, "xmlLang", NULL, xmlTextReaderXmlLang, IS_STRING TSRMLS_CC);

	/* Constants for NodeType - cannot define common types to share with dom as there are differences in these types */
	REGISTER_LONG_CONSTANT("XMLREADER_NONE",			XML_READER_TYPE_NONE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_ELEMENT",			XML_READER_TYPE_ELEMENT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_ATTRIBUTE",		XML_READER_TYPE_ATTRIBUTE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_TEXT",			XML_READER_TYPE_TEXT,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_CDATA",			XML_READER_TYPE_CDATA,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_ENTITY_REF",		XML_READER_TYPE_ENTITY_REFERENCE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_ENTITY",			XML_READER_TYPE_ENTITY,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_PI",				XML_READER_TYPE_PROCESSING_INSTRUCTION,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_COMMENT",			XML_READER_TYPE_COMMENT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_DOC",				XML_READER_TYPE_DOCUMENT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_DOC_TYPE",		XML_READER_TYPE_DOCUMENT_TYPE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_DOC_FRAGMENT",	XML_READER_TYPE_DOCUMENT_FRAGMENT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_NOTATION",		XML_READER_TYPE_NOTATION,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_WHITESPACE",		XML_READER_TYPE_WHITESPACE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_SIGNIFICANT_WHITESPACE",	XML_READER_TYPE_SIGNIFICANT_WHITESPACE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_END_ELEMENT",		XML_READER_TYPE_END_ELEMENT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_END_ENTITY",		XML_READER_TYPE_END_ENTITY,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_XML_DECLARATION",	XML_READER_TYPE_XML_DECLARATION,	CONST_CS | CONST_PERSISTENT);

	/* Constants for Parser options */
	REGISTER_LONG_CONSTANT("XMLREADER_LOADDTD",			XML_PARSER_LOADDTD,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_DEFAULTATTRS",	XML_PARSER_DEFAULTATTRS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_VALIDATE",		XML_PARSER_VALIDATE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_SUBST_ENTITIES",	XML_PARSER_SUBST_ENTITIES,		CONST_CS | CONST_PERSISTENT);

	/* Constants for Errors when loading - not yet used until we implement custom error handling
	REGISTER_LONG_CONSTANT("XMLREADER_VALIDITY_WARNING",	XML_PARSER_SEVERITY_VALIDITY_WARNING,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_VALIDITY_ERROR",		XML_PARSER_SEVERITY_VALIDITY_ERROR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_WARNING",				XML_PARSER_SEVERITY_WARNING,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XMLREADER_ERROR",				XML_PARSER_SEVERITY_ERROR,				CONST_CS | CONST_PERSISTENT);
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

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
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
