/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_simplexml.h"


zend_class_entry *sxe_class_entry;

static php_sxe_object *php_sxe_object_new(TSRMLS_D);
static zend_object_value php_sxe_register_object(php_sxe_object * TSRMLS_DC);

/* {{{ php_sxe_fetch_object()
 */
static inline php_sxe_object *
php_sxe_fetch_object(zval *object TSRMLS_DC)
{
	return (php_sxe_object *) zend_object_store_get_object(object TSRMLS_CC);
}
/* }}} */

/* {{{ _node_as_zval()
 */
static void
_node_as_zval(php_sxe_object *sxe, xmlNodePtr node, zval *value)
{
	php_sxe_object *subnode;

	subnode = php_sxe_object_new(TSRMLS_C);
	subnode->document = sxe->document;
	subnode->node = node;

	value->type = IS_OBJECT;
	value->value.obj = php_sxe_register_object(subnode TSRMLS_CC);
}
/* }}} */

#define APPEND_PREV_ELEMENT(__c, __v) \
	if ((__c) == 1) { \
		array_init(return_value); \
		add_next_index_zval(return_value, __v); \
	}

#define APPEND_CUR_ELEMENT(__c, __v) \
	if (++(__c) > 1) { \
		add_next_index_zval(return_value, __v); \
	}

#define GET_NODE(__s, __n) (__n) = (__s)->node ? (__s)->node : xmlDocGetRootElement((__s)->document)

/* {{{ sxe_property_read()
 */
static zval *
sxe_property_read(zval *object, zval *member TSRMLS_DC)
{
	zval           *return_value;
	zval           *value;
	php_sxe_object *sxe;
	char           *name;
	char           *contents;
	xmlNodePtr      node;
	xmlAttrPtr      attr;
	int             counter = 0;

	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	name = Z_STRVAL_P(member);

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	GET_NODE(sxe, node);

	attr = node->properties;
	while (attr) {
		if (!xmlStrcmp(attr->name, name)) {
			APPEND_PREV_ELEMENT(counter, value);
			
			MAKE_STD_ZVAL(value);
			contents = xmlNodeListGetString(sxe->document, attr->xmlChildrenNode, 1);
			ZVAL_STRING(value, contents, 0);

			APPEND_CUR_ELEMENT(counter, value);
		}
		attr = attr->next;
	}

	if (!sxe->node) {
		sxe->node = node;
	}
	node = node->xmlChildrenNode;

	while (node) {
		if (!xmlStrcmp(node->name, name)) {
			APPEND_PREV_ELEMENT(counter, value);

			MAKE_STD_ZVAL(value);
			_node_as_zval(sxe, node, value);

			APPEND_CUR_ELEMENT(counter, value);
		}

		node = node->next;
	}

	/* Only one value found */
	if (counter == 1) {
		return_value = value;
	}

	return return_value;
}
/* }}} */

/* {{{ change_node_zval()
 */
static void
change_node_zval(xmlNodePtr node, zval *value)
{
	switch (Z_TYPE_P(value)) {
		case IS_LONG:
		case IS_BOOL:
		case IS_DOUBLE:
		case IS_NULL:
			convert_to_string(value);
		case IS_STRING:
			xmlNodeSetContentLen(node, Z_STRVAL_P(value), Z_STRLEN_P(value));
			break;
		default:
			php_error(E_WARNING, "It is not yet possible to assign complex types to attributes");
			break;
	}
}
/* }}} */


/* {{{ sxe_property_write()
 */
static void
sxe_property_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
	php_sxe_object *sxe;
	char           *name;
	xmlNodePtr      node;
	xmlNodePtr      newnode;
	xmlAttrPtr      attr;
	int             counter = 0;
	int             is_attr = 0;

	name = Z_STRVAL_P(member);
	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	GET_NODE(sxe, node);

	attr = node->properties;
	while (attr) {
		if (!xmlStrcmp(attr->name, name)) {
			is_attr = 1;
			++counter;
			break;
		}

		attr = attr->next;
	}

	node = node->xmlChildrenNode;
	while (node) {
		if (!xmlStrcmp(node->name, name)) {
			newnode = node;
			++counter;
		}

		node = node->next;
	}

	if (counter == 1) {
		if (is_attr) {
			change_node_zval(attr->xmlChildrenNode, value);
		} else {
			change_node_zval(newnode->xmlChildrenNode, value);
		}
	} else if (counter > 1) {
		php_error(E_WARNING, "Cannot assign to an array of nodes (duplicate subnodes or attr detected)\n");
	}
		
}
/* }}} */

/* {{{ sxe_property_get_ptr()
 */
static zval **
sxe_property_get_ptr(zval *object, zval *member TSRMLS_DC)
{
	zval **property_ptr;
	zval *property;

	property_ptr = emalloc(sizeof(zval **));

	property = sxe_property_read(object, member TSRMLS_CC);
	zval_add_ref(&property);

	*property_ptr = property;
	
	return property_ptr;
}
/* }}} */

/* {{{ sxe_property_exists()
 */
static int
sxe_property_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	php_sxe_object *sxe;
	char           *name;
	xmlNodePtr      node;
	xmlAttrPtr      attr;
	
	sxe = php_sxe_fetch_object(object TSRMLS_CC);
	name = Z_STRVAL_P(member);

	GET_NODE(sxe, node);

	attr = node->properties;
	while (attr) {
		if (!xmlStrcmp(attr->name, name)) {
			return 1;
		}

		attr = attr->next;
	}

	node = node->xmlChildrenNode;
	while (node) {
		if (!xmlStrcmp(node->name, name)) {
			return 1;
		}

		node = node->next;
	}

	return 0;
}
/* }}} */

/* {{{ sxe_property_delete()
 */
static void
sxe_property_delete(zval *object, zval *member TSRMLS_DC)
{
}
/* }}} */

/* {{{ sxe_properties_get()
 */
static HashTable *
sxe_properties_get(zval *object TSRMLS_DC)
{
	HashTable      *rv;
	zval           *value;
	php_sxe_object *sxe;
	char           *name;
	char           *contents;
	xmlNodePtr      node;
	xmlAttrPtr      attr;
	int             counter = 0;

	ALLOC_HASHTABLE_REL(rv);
	zend_hash_init(rv, 0, NULL, ZVAL_PTR_DTOR, 0);
	
	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	GET_NODE(sxe, node);

	/* 
	 * XXX: TODO
	 * Recurse from the current node through the XML document
	 * and build an array return value.  Expensive? sure.  But
	 * if you ask for it, you get it :)
	 */
	
	return rv;
}
/* }}} */

/* {{{ sxe_objects_compare()
 */
static int
sxe_objects_compare(zval *object1, zval *object2 TSRMLS_DC)
{
}
/* }}} */

/* {{{ sxe_constructor_get()
 */
static union _zend_function *
sxe_constructor_get(zval *object TSRMLS_DC)
{
	return NULL;
}
/* }}} */

/* {{{ sxe_method_get()
 */
static union _zend_function *
sxe_method_get(zval *object, char *name, int len TSRMLS_DC)
{
	zend_internal_function *f;

	f = emalloc(sizeof(zend_internal_function));
	f->type = ZEND_OVERLOADED_FUNCTION;
	f->arg_types = NULL;
	f->scope = sxe_class_entry;
	f->fn_flags = 0;
	f->function_name = estrndup(name, len);

	return (union _zend_function *) f;
}
/* }}} */

/* {{{ sxe_call_method()
 */
static int
sxe_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	RETURN_NULL();
	return 1;
}
/* }}} */

/* {{{ sxe_class_entry_get()
 */
static zend_class_entry *
sxe_class_entry_get(zval *object TSRMLS_DC)
{
	return sxe_class_entry;
}
/* }}} */

/* {{{ sxe_class_name_get()
 */
static int
sxe_class_name_get(zval *object, char** class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
	*class_name = estrdup("simplexml_element");
	*class_name_len = sizeof("simplexml_element");
	return 0;
}
/* }}} */

/* {{{ cast_empty_object()
 */
static inline void
cast_empty_object(zval *object, int type TSRMLS_DC)
{
	object->type = IS_NULL;
	switch (type) {
		case IS_BOOL:
			convert_to_boolean(object);
			break;
		case IS_LONG:
			convert_to_long(object);
			break;
		case IS_DOUBLE:
			convert_to_double(object);
			break;
		case IS_STRING:
			convert_to_string(object);
			break;
	}
}
/* }}} */

/* {{{ cast_object_with_contents()
 */
static inline void
cast_object_with_contents(zval *object, int type, char *contents TSRMLS_DC)
{
	int len = strlen(contents);
	
	object->value.str.val = estrndup(contents, len);
	object->value.str.len = len;
	object->type = IS_STRING;

	switch (type) {
		case IS_STRING:
			return;
		case IS_BOOL:
			convert_to_boolean(object);
			break;
		case IS_LONG:
			convert_to_long(object);
			break;
		case IS_DOUBLE:
			convert_to_double(object);
			break;
	}
}
/* }}} */

/* {{{ sxe_object_cast()
 */
static void
sxe_object_cast(zval *readobj, zval *writeobj, int type, int should_free TSRMLS_DC)
{
	php_sxe_object *sxe;
	char           *contents;

    sxe = php_sxe_fetch_object(readobj TSRMLS_CC);
	
	if (should_free) {
		zval_dtor(writeobj);
	}

	contents = xmlNodeListGetString(sxe->document, sxe->node->xmlChildrenNode, 1);
	if (!xmlIsBlankNode(sxe->node->xmlChildrenNode) && contents) {
		cast_empty_object(writeobj, type TSRMLS_CC);
	}

	cast_object_with_contents(writeobj, type, contents);
}
/* }}} */


static zend_object_handlers sxe_object_handlers[] = {
	ZEND_OBJECTS_STORE_HANDLERS,
	sxe_property_read,
	sxe_property_write,
	sxe_property_get_ptr,
	NULL,
	NULL,
	NULL,
	sxe_property_exists,
	sxe_property_delete,
	sxe_properties_get,
	sxe_method_get,
	sxe_call_method,
	sxe_constructor_get,
	sxe_class_entry_get,
	sxe_class_name_get,
	sxe_objects_compare,
	sxe_object_cast
};

/* {{{ sxe_object_clone()
 */
static void
sxe_object_clone(void *object, void **clone TSRMLS_DC)
{
	
}
/* }}} */

/* {{{ sxe_object_dtor()
 */
static void
sxe_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	zend_objects_destroy_object(object, handle TSRMLS_CC);
}
/* }}} */

/* {{{ php_sxe_object_new() 
 */
static php_sxe_object *
php_sxe_object_new(TSRMLS_D)
{
	php_sxe_object *intern;

	intern = ecalloc(1, sizeof(php_sxe_object));
	intern->zo.ce = sxe_class_entry;
	intern->zo.in_get = 0;
	intern->zo.in_set = 0;

	ALLOC_HASHTABLE(intern->zo.properties);
	zend_hash_init(intern->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	return intern;
}
/* }}} */

/* {{{ php_sxe_register_object
 */
static zend_object_value
php_sxe_register_object(php_sxe_object *intern TSRMLS_DC)
{
	zend_object_value rv;

	rv.handle = zend_objects_store_put(intern, sxe_object_dtor, sxe_object_clone);
	rv.handlers = (zend_object_handlers *) &sxe_object_handlers;

	return rv;
}
/* }}} */

/* {{{ sxe_object_new()
 */
static zend_object_value
sxe_object_new(zend_class_entry *ce TSRMLS_DC)
{
	php_sxe_object    *intern;
	zend_object_value  rv;

	intern = php_sxe_object_new(TSRMLS_C);
	return php_sxe_register_object(intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto simplemxml_element simplexml_load_file(string filename)
   Load a filename and return a simplexml_element object to allow for processing */
PHP_FUNCTION(simplexml_load_file)
{
	php_sxe_object *sxe;
	char           *filename;
	int             filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}

	sxe = php_sxe_object_new(TSRMLS_C);
	sxe->document = xmlParseFile(filename);
	if (sxe->document == NULL) {
		RETURN_FALSE;
	}

	return_value->type = IS_OBJECT;
	return_value->value.obj = php_sxe_register_object(sxe TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool simplexml_save_file(string filename, simplexml_element node)
   Save a document from a SimpleXML node */
PHP_FUNCTION(simplexml_save_document_file)
{
	php_sxe_object *sxe;
	zval           *element;
	char           *filename;
	int             filename_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &filename, &filename_len, &element) == FAILURE) {
		return;
	}

	sxe = php_sxe_fetch_object(element TSRMLS_CC);

	xmlSaveFile(filename, sxe->document);

	RETURN_TRUE;
}
/* }}} */

function_entry simplexml_functions[] = {
	PHP_FE(simplexml_load_file, NULL)
	PHP_FE(simplexml_save_document_file, NULL)
	{NULL, NULL, NULL}
};


zend_module_entry simplexml_module_entry = {
	STANDARD_MODULE_HEADER,
	"simplexml",
	simplexml_functions,
	PHP_MINIT(simplexml),
	PHP_MSHUTDOWN(simplexml),
	PHP_RINIT(simplexml),	
	PHP_RSHUTDOWN(simplexml),
	PHP_MINFO(simplexml),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SIMPLEXML
ZEND_GET_MODULE(simplexml)
#endif

/* {{{ PHP_MINIT_FUNCTION(simplexml)
 */
PHP_MINIT_FUNCTION(simplexml)
{
	zend_class_entry sxe;
	zend_internal_function sxe_constructor;

	INIT_CLASS_ENTRY(sxe, "simplexml_element", NULL);
	sxe.create_object = sxe_object_new;
	sxe_class_entry = zend_register_internal_class(&sxe TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(simplexml)
 */
PHP_MSHUTDOWN_FUNCTION(simplexml)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION(simplexml)
 */
PHP_RINIT_FUNCTION(simplexml)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION(simplexml)
 */
PHP_RSHUTDOWN_FUNCTION(simplexml)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION(simplexml)
 */
PHP_MINFO_FUNCTION(simplexml)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Simplexml support", "enabled");
	php_info_print_table_row(2, "Revision", "$Revision$");
	php_info_print_table_end();

}
/* }}} */

/**
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: t
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
