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
  |         Marcus Boerger <helly@php.net>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_SIMPLEXML

#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_simplexml.h"

zend_class_entry *sxe_class_entry;

#define SKIP_TEXT(__p) \
	if ((__p)->type == XML_TEXT_NODE) { \
		goto next_iter; \
	}

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
_node_as_zval(php_sxe_object *sxe, xmlNodePtr node, zval *value TSRMLS_DC)
{
	php_sxe_object *subnode;

	subnode = php_sxe_object_new(TSRMLS_C);
	subnode->document = sxe->document;
	subnode->document->refcount++;
	subnode->nsmapptr = sxe->nsmapptr;
	subnode->nsmapptr->refcount++;

	php_libxml_increment_node_ptr((php_libxml_node_object *)subnode, node, NULL TSRMLS_CC);

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

#define GET_NODE(__s, __n) { \
	if ((__s)->node && (__s)->node->node) { \
		__n = (__s)->node->node; \
	} else { \
		__n = NULL; \
		php_error(E_WARNING, "Node no longer exists"); \
	} \
}


/* {{{ match_ns()
 */
static inline int 
match_ns(php_sxe_object *sxe, xmlNodePtr node, xmlChar *name)
{
	xmlChar *prefix;
	
	if (sxe->nsmapptr) {
		prefix = xmlHashLookup(sxe->nsmapptr->nsmap, node->ns->href);
		if (prefix == NULL) {
			prefix = (xmlChar*)node->ns->prefix;
		}

		if (prefix == NULL) {
			return 0;
		}

		if (!xmlStrcmp(prefix, name)) {
			return 1;
		}
	}

	return 0;
}
/* }}} */
	
/* {{{ sxe_prop_dim_read()
 */
static zval *
sxe_prop_dim_read(zval *object, zval *member, zend_bool elements, zend_bool attribs, zend_bool silent TSRMLS_DC)
{
	zval           *return_value;
	zval           *value = NULL;
	php_sxe_object *sxe;
	char           *name;
	char           *contents;
	xmlNodePtr      node;
	xmlAttrPtr      attr;
	int             counter = 0;
	zval            tmp_zv;

	if (Z_TYPE_P(member) != IS_STRING) {
		tmp_zv = *member;
		zval_copy_ctor(&tmp_zv);
		member = &tmp_zv;
		convert_to_string(member);
	}

	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	name = Z_STRVAL_P(member);

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	GET_NODE(sxe, node);

	if (node) {
		if (attribs) {
			attr = node->properties;
			while (attr) {
				if (!xmlStrcmp(attr->name, name)) {
					APPEND_PREV_ELEMENT(counter, value);
					
					MAKE_STD_ZVAL(value);
					contents = xmlNodeListGetString((xmlDocPtr) sxe->document->ptr, attr->children, 1);
					ZVAL_STRING(value, contents, 1);
					if (contents) {
						xmlFree(contents);
					}
					APPEND_CUR_ELEMENT(counter, value);
				}
				attr = attr->next;
			}
		}

		if (elements) {
			if (!sxe->node) {
				php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, node, NULL TSRMLS_CC);
			}
			node = node->children;
	
			while (node) {
				SKIP_TEXT(node);
				
				do if (node->ns) {
					if (node->parent->ns) {
						if (!xmlStrcmp(node->ns->href, node->parent->ns->href)) {
							break;
						}
					}
					
					if (match_ns(sxe, node, name)) {
						MAKE_STD_ZVAL(value);
						_node_as_zval(sxe, node->parent, value TSRMLS_CC);
						APPEND_CUR_ELEMENT(counter, value);
						goto next_iter;
					}
				} while (0);
	
				if (!xmlStrcmp(node->name, name)) {
					APPEND_PREV_ELEMENT(counter, value);
					MAKE_STD_ZVAL(value);
					_node_as_zval(sxe, node, value TSRMLS_CC);
					APPEND_CUR_ELEMENT(counter, value);
				}
	
next_iter:
				node = node->next;
			}
		}

		/* Only one value found */
		if (counter == 1) {
			SEPARATE_ZVAL(&value);
			zval_dtor(return_value);
			FREE_ZVAL(return_value); 
			return_value = value;
		}
	}

	return_value->refcount = 0;
	return_value->is_ref = 0;

	if (member == &tmp_zv) {
		zval_dtor(&tmp_zv);
	}

	return return_value;
}
/* }}} */

/* {{{ sxe_property_read()
 */
static zval *
sxe_property_read(zval *object, zval *member, zend_bool silent TSRMLS_DC)
{
	return sxe_prop_dim_read(object, member, 1, 0, silent TSRMLS_CC);
}
/* }}} */

/* {{{ sxe_dimension_read()
 */
static zval *sxe_dimension_read(zval *object, zval *offset TSRMLS_DC)
{
	return sxe_prop_dim_read(object, offset, 0, 1, 0 TSRMLS_CC);
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
static void sxe_prop_dim_write(zval *object, zval *member, zval *value, zend_bool elements, zend_bool attribs TSRMLS_DC)
{
	php_sxe_object *sxe;
	char           *name;
	xmlNodePtr      node;
	xmlNodePtr      newnode = NULL;
	xmlNodePtr		tempnode;
	xmlAttrPtr      attr = NULL;
	int             counter = 0;
	int             is_attr = 0;
	zval            tmp_zv;

	if (Z_TYPE_P(member) != IS_STRING) {
		tmp_zv = *member;
		zval_copy_ctor(&tmp_zv);
		member = &tmp_zv;
		convert_to_string(member);
	}

	name = Z_STRVAL_P(member);
	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	GET_NODE(sxe, node);

	if (node) {
		if (attribs) {
			attr = node->properties;
			while (attr) {
				if (!xmlStrcmp(attr->name, name)) {
					is_attr = 1;
					++counter;
					break;
				}
	
				attr = attr->next;
			}
		}

		if (elements) {
			node = node->children;
			while (node) {
				SKIP_TEXT(node);
				if (!xmlStrcmp(node->name, name)) {
					newnode = node;
					++counter;
				}
	
next_iter:
				node = node->next;
			}
		}

		if (counter == 1) {
			if (is_attr) {
				newnode = (xmlNodePtr) attr;
			}
			while ((tempnode = (xmlNodePtr) newnode->children)) {
				xmlUnlinkNode(tempnode);
				php_libxml_node_free_resource((xmlNodePtr) tempnode TSRMLS_CC);
			}
			change_node_zval(newnode, value);
		} else if (counter > 1) {
			php_error(E_WARNING, "Cannot assign to an array of nodes (duplicate subnodes or attr detected)\n");
		} else {
			php_error(E_WARNING, "Cannot create new atrribute\n");
		}
	}

	if (member == &tmp_zv) {
		zval_dtor(&tmp_zv);
	}
}
/* }}} */

/* {{{ sxe_property_write()
 */
static void sxe_property_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
	sxe_prop_dim_write(object, member, value, 1, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ sxe_dimension_write()
 */
static void sxe_dimension_write(zval *object, zval *offset, zval *value TSRMLS_DC)
{
	sxe_prop_dim_write(object, offset, value, 0, 1 TSRMLS_CC);
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

	if (node) {
		node = node->children;
		while (node) {
			SKIP_TEXT(node);

			if (!xmlStrcmp(node->name, name)) {
				return 1;
			}

next_iter:
			node = node->next;
		}
	}

	return 0;
}
/* }}} */

/* {{{ sxe_prop_dim_delete()
 */
static void sxe_prop_dim_delete(zval *object, zval *member, zend_bool elements, zend_bool attribs TSRMLS_DC)
{
	php_sxe_object *sxe;
	xmlNodePtr      node;
	xmlNodePtr      nnext;
	xmlAttrPtr      attr;
	xmlAttrPtr      anext;
	zval            tmp_zv;

	if (Z_TYPE_P(member) != IS_STRING) {
		tmp_zv = *member;
		zval_copy_ctor(&tmp_zv);
		member = &tmp_zv;
		convert_to_string(member);
	}

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	GET_NODE(sxe, node);

	if (node) {
		if (attribs) {
			attr = node->properties;
			while (attr) {
				anext = attr->next;
				if (!xmlStrcmp(attr->name, Z_STRVAL_P(member))) {
					xmlUnlinkNode((xmlNodePtr) attr);
					xmlFreeProp(attr);
				}
				attr = anext;
			}
		}

		if (elements) {
			node = node->children;
			while (node) {
				nnext = node->next;
	
				SKIP_TEXT(node);
				
				if (!xmlStrcmp(node->name, Z_STRVAL_P(member))) {
					xmlUnlinkNode(node);
					xmlFreeNode(node);
				}

next_iter:
				node = nnext;
			}
		}
	}

	if (member == &tmp_zv) {
		zval_dtor(&tmp_zv);
	}
}
/* }}} */

/* {{{ sxe_property_delete()
 */
static void
sxe_property_delete(zval *object, zval *member TSRMLS_DC)
{
	sxe_prop_dim_delete(object, member, 1, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ sxe_dimension_unset()
 */
static void sxe_dimension_delete(zval *object, zval *offset TSRMLS_DC)
{
	sxe_prop_dim_delete(object, offset, 1, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ _get_base_node_value()
 */
static void
_get_base_node_value(php_sxe_object *sxe_ref, xmlNodePtr node, zval **value TSRMLS_DC)
{
	php_sxe_object *subnode;
	xmlChar        *contents;

	MAKE_STD_ZVAL(*value);
	
	if (node->children && node->children->type == XML_TEXT_NODE && !xmlIsBlankNode(node->children)) {
		contents = xmlNodeListGetString(node->doc, node->children, 1);
		if (contents) {
			ZVAL_STRING(*value, contents, 1);
			xmlFree(contents);
		}
	} else {
		subnode = php_sxe_object_new(TSRMLS_C);
		subnode->document = sxe_ref->document;
		subnode->document->refcount++;
		subnode->nsmapptr = sxe_ref->nsmapptr;
		subnode->nsmapptr->refcount++;
		php_libxml_increment_node_ptr((php_libxml_node_object *)subnode, node, NULL TSRMLS_CC);

		(*value)->type = IS_OBJECT;
		(*value)->value.obj = php_sxe_register_object(subnode TSRMLS_CC);
		/*zval_add_ref(value);*/
	}
}
/* }}} */

/* {{{ sxe_properties_get()
 */
static HashTable *
sxe_properties_get(zval *object TSRMLS_DC)
{
	zval           **data_ptr;
	zval            *value;
	zval            *newptr;
	HashTable       *rv;
	php_sxe_object  *sxe;
	char            *name;
	xmlNodePtr       node;
	ulong            h;
	int              namelen;

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	if (sxe->properties) {
		zend_hash_clean(sxe->properties);
		rv = sxe->properties;
	} else {
		ALLOC_HASHTABLE(rv);
		zend_hash_init(rv, 0, NULL, ZVAL_PTR_DTOR, 0);
		sxe->properties = rv;
	}

	GET_NODE(sxe, node);

	if (node) {
		node = node->children;

		while (node) {
			SKIP_TEXT(node);

			_get_base_node_value(sxe, node, &value TSRMLS_CC);
			
			name = (char *) node->name;
			if (!name) {
				name = "CDATA";
				namelen = sizeof("CDATA");
			} else {
				namelen = xmlStrlen(node->name) + 1;
			}

			h = zend_hash_func(name, namelen);
			if (zend_hash_quick_find(rv, name, namelen, h, (void **) &data_ptr) == SUCCESS) {
				if (Z_TYPE_PP(data_ptr) == IS_ARRAY) {
					zend_hash_next_index_insert(Z_ARRVAL_PP(data_ptr), &value, sizeof(zval *), NULL);
				} else {
					MAKE_STD_ZVAL(newptr);
					array_init(newptr);

					zval_add_ref(data_ptr);
					zend_hash_next_index_insert(Z_ARRVAL_P(newptr), data_ptr, sizeof(zval *), NULL);
					zend_hash_next_index_insert(Z_ARRVAL_P(newptr), &value, sizeof(zval *), NULL);

					zend_hash_quick_update(rv, name, namelen, h, &newptr, sizeof(zval *), NULL);
				}
			} else {
				zend_hash_quick_update(rv, name, namelen, h, &value, sizeof(zval *), NULL);
			}

next_iter:
			node = node->next;
		}
	}

	return rv;
}
/* }}} */

/* {{{ sxe_objects_compare()
 */
static int
sxe_objects_compare(zval *object1, zval *object2 TSRMLS_DC)
{
	php_sxe_object *sxe1;
	php_sxe_object *sxe2;

	sxe1 = php_sxe_fetch_object(object1 TSRMLS_CC);
	sxe2 = php_sxe_fetch_object(object2 TSRMLS_CC);

	if (sxe1->node == NULL) {
		if (sxe2->node) {
			return 1;
		} else if (sxe1->document->ptr == sxe2->document->ptr) {
			return 0;
		}
	} else {
		return !(sxe1->node == sxe2->node);
	}
	return 1;
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
	f->type = ZEND_OVERLOADED_FUNCTION_TEMPORARY;
	f->arg_info = NULL;
	f->num_args = 0;
	f->scope = sxe_class_entry;
	f->fn_flags = 0;
	f->function_name = estrndup(name, len);

	return (union _zend_function *) f;
}
/* }}} */

/* {{{ simplexml_ce_xpath_search()
 */
static void 
simplexml_ce_xpath_search(INTERNAL_FUNCTION_PARAMETERS)
{
	php_sxe_object    *sxe;
	zval              *value;
	char              *query;
	int                query_len;
	int                i;
	xmlXPathObjectPtr  retval;
	xmlNodeSetPtr      result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &query, &query_len) == FAILURE) {
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	if (!sxe->xpath) {
		sxe->xpath = xmlXPathNewContext((xmlDocPtr) sxe->document->ptr);
	}
	if (!sxe->node) {
		php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement((xmlDocPtr) sxe->document->ptr), NULL TSRMLS_CC);
	}

	sxe->xpath->node = sxe->node->node;

	retval = xmlXPathEval(query, sxe->xpath);
	if (!retval) {
		RETURN_FALSE;
	}

	
	result = retval->nodesetval;
	if (!result) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i = 0; i < result->nodeNr; ++i) {
		MAKE_STD_ZVAL(value);
		/**
		 * Detect the case where the last selector is text(), simplexml
		 * always accesses the text() child by default, therefore we assign
		 * to the parent node.
		 */
		if (result->nodeTab[i]->type == XML_TEXT_NODE) {
			_node_as_zval(sxe, result->nodeTab[i]->parent, value TSRMLS_CC);
		} else {
			_node_as_zval(sxe, result->nodeTab[i], value TSRMLS_CC);
		}
		add_next_index_zval(return_value, value);
	}
	
}
/* }}} */

#define SCHEMA_FILE 0
#define SCHEMA_BLOB 1
#define SCHEMA_OBJECT 2

#ifdef LIBXML_SCHEMAS_ENABLED

/* {{{ simplexml_ce_schema_validate_file()
 */
static void
simplexml_ce_schema_validate(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	php_sxe_object         *sxe;
	zval                   **source;
	xmlSchemaParserCtxtPtr  parser;
	xmlSchemaPtr            sptr;
	xmlSchemaValidCtxtPtr   vptr;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &source) == FAILURE) {
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);

	switch (type) {
		case SCHEMA_FILE:
			convert_to_string_ex(source);
			parser = xmlSchemaNewParserCtxt(Z_STRVAL_PP(source));
			if (parser == NULL) {
				php_error_docref1(NULL TSRMLS_CC, Z_STRVAL_PP(source), E_WARNING, "Unable to load XML Schema file");
				RETURN_FALSE;
			}
			sptr = xmlSchemaParse(parser);
			break;
		case SCHEMA_BLOB:
			convert_to_string_ex(source);
			parser = xmlSchemaNewMemParserCtxt(Z_STRVAL_PP(source), Z_STRLEN_PP(source));
			sptr = xmlSchemaParse(parser);
			break;
		default:
			parser = NULL;
			sptr = NULL;
	}

	if (sptr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Malformed XML Schema");
		xmlSchemaFreeParserCtxt(parser);
		RETURN_FALSE;
	}

	vptr = xmlSchemaNewValidCtxt(sptr);

	if (vptr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create XML Schema validation context");
		xmlSchemaFree(sptr);
		xmlSchemaFreeParserCtxt(parser);
		RETURN_FALSE;
	}

	switch (xmlSchemaValidateDoc(vptr, (xmlDocPtr) sxe->document->ptr)) {
		case 0: /* validated */
			RETVAL_TRUE;
			break;
		case -1: /* internal error */
			RETVAL_FALSE;
			break;
		default: /* error */
			RETVAL_FALSE;
			break;
	}

	xmlSchemaFree(sptr);
	xmlSchemaFreeValidCtxt(vptr);
	xmlSchemaFreeParserCtxt(parser);
}
/* }}} */

#endif

/* {{{ simplexml_ce_register_ns()
 */
static void
simplexml_ce_register_ns(INTERNAL_FUNCTION_PARAMETERS)
{
	php_sxe_object *sxe;
	char *nsname;
	char *nsvalue;
	int   nsname_len;
	int   nsvalue_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &nsname, &nsname_len, &nsvalue, &nsvalue_len) == FAILURE) {
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);

	xmlHashAddEntry(sxe->nsmapptr->nsmap, nsvalue, nsname);
}
/* }}} */

/* {{{ simplexml_ce_to_xml_string()
 */
static void 
simplexml_ce_to_xml_string(INTERNAL_FUNCTION_PARAMETERS)
{
	php_sxe_object *sxe;
	xmlChar *strval;


	if (ZEND_NUM_ARGS() != 0) {
		RETURN_FALSE;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	xmlDocDumpMemory((xmlDocPtr) sxe->document->ptr, &strval, &Z_STRLEN_P(return_value));
	Z_STRVAL_P(return_value) = estrndup(strval, Z_STRLEN_P(return_value));
	xmlFree(strval);

	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */

/* {{{ simplexml_ce_to_xml_file()
 */
static void
simplexml_ce_to_xml_file(INTERNAL_FUNCTION_PARAMETERS)
{
	php_sxe_object *sxe;
	char           *filename;
	int             filename_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);

	xmlSaveFile(filename, (xmlDocPtr) sxe->document->ptr);

	RETURN_TRUE;
}
/* }}} */

/* {{{ sxe_call_method()
 */
static int
sxe_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	if (!strcmp(method, "xsearch")) {
		simplexml_ce_xpath_search(INTERNAL_FUNCTION_PARAM_PASSTHRU);
#ifdef LIBXML_SCHEMAS_ENABLED
	} else if (!strcmp(method, "validate_schema_file")) {
		simplexml_ce_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, SCHEMA_FILE);	
	} else if (!strcmp(method, "validate_schema_buffer")) {
		simplexml_ce_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, SCHEMA_BLOB);
#endif
	} else if (!strcmp(method, "register_ns")) {
		simplexml_ce_register_ns(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	} else if (!strcmp(method, "to_xml")) {
		simplexml_ce_to_xml_string(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	} else if (!strcmp(method, "to_xml_file")) {
		simplexml_ce_to_xml_file(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	} else {
		return 0;
	}

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
sxe_class_name_get(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
	*class_name = estrdup("simplexml_element");
	*class_name_len = sizeof("simplexml_element");

	return 0;
}
/* }}} */

/* {{{ cast_object()
 */
static int
cast_object(zval *object, int type, char *contents TSRMLS_DC)
{
	if (contents) {
		ZVAL_STRINGL(object, contents, strlen(contents), 1);
	} else {
		ZVAL_NULL(object);
	}
	object->refcount = 1;
	object->is_ref = 0;

	switch (type) {
		case IS_STRING:
			convert_to_string(object);
			break;
		case IS_BOOL:
			convert_to_boolean(object);
			break;
		case IS_LONG:
			convert_to_long(object);
			break;
		case IS_DOUBLE:
			convert_to_double(object);
			break;
		default:
			return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ sxe_object_cast()
 */
static int
sxe_object_cast(zval *readobj, zval *writeobj, int type, int should_free TSRMLS_DC)
{
	php_sxe_object *sxe;
	char           *contents = NULL;
	zval free_obj;
	int rv;

	sxe = php_sxe_fetch_object(readobj TSRMLS_CC);
	if (should_free) {
		free_obj = *writeobj;
	}

	if (!sxe->node) {
		if (sxe->document) {
			php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement((xmlDocPtr) sxe->document->ptr), NULL TSRMLS_CC);
		}
	}

	if (sxe->node && sxe->node->node) {
		if (sxe->node->node->children) {
			contents = xmlNodeListGetString((xmlDocPtr) sxe->document->ptr, sxe->node->node->children, 1);
		}
	} 

	rv = cast_object(writeobj, type, contents TSRMLS_CC);

	if (contents) {
		xmlFree(contents);
	}
	if (should_free) {
		/*zval_dtor(&free_obj);*/
	}
	return rv;
}
/* }}} */

/* {{{ sxe_object_set()
 */
static void
sxe_object_set(zval **property, zval *value TSRMLS_DC)
{
	/* XXX: TODO
	 * This call is not yet implemented in the engine
	 * so leave it blank for now.
	 */
}
/* }}} */

/* {{{ sxe_object_get()
 */
static zval *
sxe_object_get(zval *property TSRMLS_DC)
{
	/* XXX: TODO
	 * This call is not yet implemented in the engine
	 * so leave it blank for now.
	 */
	return NULL;
}
/* }}} */

static zend_object_handlers sxe_object_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	sxe_property_read,
	sxe_property_write,
	sxe_dimension_read,
	sxe_dimension_write,
	NULL,
	sxe_object_get,
	sxe_object_set,
	sxe_property_exists,
	sxe_property_delete,
	sxe_dimension_delete,
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
sxe_object_clone(void *object, void **clone_ptr TSRMLS_DC)
{
	php_sxe_object *sxe = (php_sxe_object *) object;
	php_sxe_object *clone;
	xmlNodePtr nodep = NULL;
	xmlDocPtr docp = NULL;

	clone = php_sxe_object_new(TSRMLS_C);
	clone->document = sxe->document;
	if (clone->document) {
		clone->document->refcount++;
		docp = clone->document->ptr;
	}
	if (sxe->node) {
		nodep = xmlDocCopyNode(sxe->node->node, docp, 1);
	}
	clone->nsmapptr = emalloc(sizeof(simplexml_nsmap));
	clone->nsmapptr->nsmap = xmlHashCreate(10);
	clone->nsmapptr->refcount = 1;

	php_libxml_increment_node_ptr((php_libxml_node_object *)clone, nodep, NULL TSRMLS_CC);

	*clone_ptr = (void *) clone;
}
/* }}} */

/* {{{ _free_ns_entry()
 */
static void
_free_ns_entry(void *p, xmlChar *data)
{
	xmlFree(p);
}
/* }}} */

/* {{{ sxe_object_dtor()
 */
static void
sxe_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	php_sxe_object *sxe;

	sxe = (php_sxe_object *) object;

	zend_hash_destroy(sxe->zo.properties);
	FREE_HASHTABLE(sxe->zo.properties);

	php_libxml_node_decrement_resource((php_libxml_node_object *)sxe TSRMLS_CC);

	if (--sxe->nsmapptr->refcount == 0) {
		xmlHashFree(sxe->nsmapptr->nsmap, _free_ns_entry);
		efree(sxe->nsmapptr);
	}
	
	if (sxe->xpath) {
		xmlXPathFreeContext(sxe->xpath);
	}
	
	if (sxe->properties) {
		zend_hash_destroy(sxe->properties);
		FREE_HASHTABLE(sxe->properties);
	}

	efree(object);
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
	intern->node = NULL;
	intern->document = NULL;
	intern->nsmapptr = NULL;
	intern->xpath = NULL;
	intern->properties = NULL;

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

	rv.handle = zend_objects_store_put(intern, sxe_object_dtor, sxe_object_clone TSRMLS_CC);
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
	xmlDocPtr       docp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}

	docp = xmlParseFile(filename);
	if (! docp) {
		RETURN_FALSE;
	}

	sxe = php_sxe_object_new(TSRMLS_C);
	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp TSRMLS_CC);
	sxe->nsmapptr = emalloc(sizeof(simplexml_nsmap));
	sxe->nsmapptr->nsmap = xmlHashCreate(10);
	sxe->nsmapptr->refcount = 1;
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL TSRMLS_CC);

	return_value->type = IS_OBJECT;
	return_value->value.obj = php_sxe_register_object(sxe TSRMLS_CC);
}
/* }}} */

/* {{{ proto simplemxml_element simplexml_load_string(string data)
   Load a string and return a simplexml_element object to allow for processing */
PHP_FUNCTION(simplexml_load_string)
{
	php_sxe_object *sxe;
	char           *data;
	int             data_len;
	xmlDocPtr       docp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
		return;
	}

	docp = xmlParseMemory(data, data_len);
	if (! docp) {
		RETURN_FALSE;
	}

	sxe = php_sxe_object_new(TSRMLS_C);
	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp TSRMLS_CC);
	sxe->nsmapptr = emalloc(sizeof(simplexml_nsmap));
	sxe->nsmapptr->nsmap = xmlHashCreate(10);
	sxe->nsmapptr->refcount = 1;
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL TSRMLS_CC);

	return_value->type = IS_OBJECT;
	return_value->value.obj = php_sxe_register_object(sxe TSRMLS_CC);
}
/* }}} */

typedef struct {
	zend_object_iterator  intern;
	php_sxe_object        *sxe;
	xmlNodePtr            node;
	char                  *name;
	int                   namelen;
	zval                  *data;
} php_sxe_iterator;

static void php_sxe_iterator_dtor(zend_object_iterator *iter TSRMLS_DC);
static int php_sxe_iterator_has_more(zend_object_iterator *iter TSRMLS_DC);
static void php_sxe_iterator_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC);
static int php_sxe_iterator_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC);
static void php_sxe_iterator_move_forward(zend_object_iterator *iter TSRMLS_DC);

zend_object_iterator_funcs php_sxe_iterator_funcs = {
	php_sxe_iterator_dtor,
	php_sxe_iterator_has_more,
	php_sxe_iterator_current_data,
	php_sxe_iterator_current_key,
	php_sxe_iterator_move_forward,
	NULL
};

static void php_sxe_iterator_current(php_sxe_iterator *iterator TSRMLS_DC)
{
	while (iterator->node) {
		SKIP_TEXT(iterator->node);
	
		_get_base_node_value(iterator->sxe, iterator->node, &iterator->data TSRMLS_CC);
		
		if (!iterator->node->name) {
			iterator->name = "CDATA";
			iterator->namelen = sizeof("CDATA");
		} else {
			iterator->namelen = xmlStrlen(iterator->node->name)+1;
			iterator->name = (char *) iterator->node->name;
		}
		break;
next_iter:
		iterator->node = iterator->node->next;
	}
}

zend_object_iterator *php_sxe_get_iterator(zend_class_entry *ce, zval *object TSRMLS_DC)
{
	php_sxe_iterator *iterator = emalloc(sizeof(php_sxe_iterator));

	object->refcount++;
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = &php_sxe_iterator_funcs;
	iterator->sxe = php_sxe_fetch_object(object TSRMLS_CC);
	GET_NODE(iterator->sxe, iterator->node);
	iterator->node = iterator->node->children;
	iterator->data = NULL;
	php_sxe_iterator_current(iterator TSRMLS_CC);
	
	return (zend_object_iterator*)iterator;
}

static void php_sxe_iterator_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	zval_ptr_dtor((zval**)&iterator->intern.data);
	
	if (iterator->data) {
		zval_ptr_dtor(&iterator->data);
	}
	efree(iterator);
}
	
static int php_sxe_iterator_has_more(zend_object_iterator *iter TSRMLS_DC)
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	return iterator->node ? SUCCESS : FAILURE;
}

static void php_sxe_iterator_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	
	*data = &iterator->data;
}

static int php_sxe_iterator_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	
	*str_key = estrndup(iterator->name, iterator->namelen-1);
	*str_key_len = iterator->namelen;
	return HASH_KEY_IS_STRING;
}

static void php_sxe_iterator_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	
	if (iterator->data) {
		zval_ptr_dtor(&iterator->data);
		iterator->data = NULL;
	}
	iterator->node = iterator->node->next;
	php_sxe_iterator_current(iterator TSRMLS_CC);
}

/* {{{ proto simplemxml_element simplexml_import_dom(domNode node)
   Get a simplexml_element object from dom to allow for processing */
PHP_FUNCTION(simplexml_import_dom)
{
#ifdef HAVE_DOM
	php_sxe_object *sxe;
	zval *node;
	php_libxml_node_object *object;
	xmlNodePtr		nodep = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &node) == FAILURE) {
		return;
	}

	object = (php_libxml_node_object *)zend_object_store_get_object(node TSRMLS_CC);

	if (object->node && object->node->node) {
		nodep = object->node->node;
		if (nodep->doc == NULL) {
			php_error(E_WARNING, "Imported Node must have associated Document");
			RETURN_NULL();
		}
		if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
			nodep = xmlDocGetRootElement((xmlDocPtr) nodep);
		}
	}

	if (nodep && nodep->type == XML_ELEMENT_NODE) {
		sxe = php_sxe_object_new(TSRMLS_C);
		sxe->document = object->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, nodep->doc TSRMLS_CC);
		sxe->nsmapptr = emalloc(sizeof(simplexml_nsmap));
		sxe->nsmapptr->nsmap = xmlHashCreate(10);
		sxe->nsmapptr->refcount = 1;
		php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, nodep, NULL TSRMLS_CC);

		return_value->type = IS_OBJECT;
		return_value->value.obj = php_sxe_register_object(sxe TSRMLS_CC);
	} else {
		php_error(E_WARNING, "Invalid Nodetype to import");
		RETVAL_NULL();
	}
#else
	php_error(E_WARNING, "DOM support is not enabled");
	return;
#endif
}
/* }}} */

function_entry simplexml_functions[] = {
	PHP_FE(simplexml_load_file, NULL)
	PHP_FE(simplexml_load_string, NULL)
	PHP_FE(simplexml_import_dom, NULL)
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

	INIT_CLASS_ENTRY(sxe, "simplexml_element", NULL);
	sxe.create_object = sxe_object_new;
	sxe_class_entry = zend_register_internal_class(&sxe TSRMLS_CC);
	sxe_class_entry->get_iterator = php_sxe_get_iterator;

	php_libxml_initialize();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(simplexml)
 */
PHP_MSHUTDOWN_FUNCTION(simplexml)
{
	php_libxml_shutdown();

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
	php_info_print_table_row(2, "Schema support", 
#ifdef LIBXML_SCHEMAS_ENABLED
		"enabled");
#else
		"not available");
#endif
	php_info_print_table_end();
}
/* }}} */

#endif

/**
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: t
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
