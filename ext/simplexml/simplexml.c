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
  | Authors: Sterling Hughes <sterling@php.net>                          |
  |          Marcus Boerger <helly@php.net>                              |
  |          Rob Richards <rrichards@php.net>                            |
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
#include "ext/standard/php_string.h"
#include "php_simplexml.h"
#include "php_simplexml_exports.h"
#include "zend_default_classes.h"
#include "zend_interfaces.h"
#if HAVE_SPL && !defined(COMPILE_DL_SPL)
#include "ext/spl/spl_sxe.h"
#endif

zend_class_entry *sxe_class_entry = NULL;

ZEND_API zend_class_entry *sxe_get_element_class_entry()
{
	return sxe_class_entry;
}

#define SXE_ME(func, arg_info, flags) PHP_ME(simplexml_element, func, arg_info, flags)

#define SXE_METHOD(func) PHP_METHOD(simplexml_element, func)

static php_sxe_object* php_sxe_object_new(zend_class_entry *ce TSRMLS_DC);
static zend_object_value php_sxe_register_object(php_sxe_object * TSRMLS_DC);

/* {{{ _node_as_zval()
 */
static void _node_as_zval(php_sxe_object *sxe, xmlNodePtr node, zval *value, int itertype, char *name, char *prefix TSRMLS_DC)
{
	php_sxe_object *subnode;

	subnode = php_sxe_object_new(sxe->zo.ce TSRMLS_CC);
	subnode->document = sxe->document;
	subnode->document->refcount++;
	subnode->iter.type = itertype;
	if (name) {
		subnode->iter.name = xmlStrdup(name);
	}
	if (prefix) {
		subnode->iter.nsprefix = xmlStrdup(prefix);
	}

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

static xmlNodePtr php_sxe_get_first_node(php_sxe_object *sxe, xmlNodePtr node TSRMLS_DC) {
	php_sxe_object *intern;
	xmlNodePtr retnode = NULL;

	if (sxe && sxe->iter.type != SXE_ITER_NONE) {
		php_sxe_reset_iterator(sxe TSRMLS_CC);
		if (sxe->iter.data) {
			intern = (php_sxe_object *)zend_object_store_get_object(sxe->iter.data TSRMLS_CC);
			GET_NODE(intern, retnode)
		}
		return retnode;
	} else {
		return node;
	}
}

/* {{{ match_ns()
 */
static inline int 
match_ns(php_sxe_object *sxe, xmlNodePtr node, xmlChar *name)
{
	if (name == NULL && (node->ns == NULL || node->ns->prefix == NULL)) {
		return 1;
	}
	
	if (!xmlStrcmp(node->ns->href, name)) {
		return 1;
	}	

	return 0;
}
/* }}} */
	
/* {{{ sxe_prop_dim_read()
 */
static zval * sxe_prop_dim_read(zval *object, zval *member, zend_bool elements, zend_bool attribs, zend_bool silent TSRMLS_DC)
{
	zval           *return_value;
	php_sxe_object *sxe;
	char           *name;
	xmlNodePtr      node;
	xmlAttrPtr      attr;
	zval            tmp_zv;
	int				nodendx = 0;
	char			*prefix;

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	if (Z_TYPE_P(member) == IS_LONG) {
		if (sxe->iter.type != SXE_ITER_ATTRLIST) {
			attribs = 0;
			elements = 1;
		}
	} else {
		if (Z_TYPE_P(member) != IS_STRING) {
			tmp_zv = *member;
			zval_copy_ctor(&tmp_zv);
			member = &tmp_zv;
			convert_to_string(member);
		}
	}

	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	name = Z_STRVAL_P(member);

	GET_NODE(sxe, node);

	if (sxe->iter.type != SXE_ITER_CHILD && sxe->iter.type != SXE_ITER_ATTRLIST) {
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
	}

	if (node) {
		if (attribs) {
			if (Z_TYPE_P(member) == IS_LONG && sxe->iter.type != SXE_ITER_ATTRLIST) {
				attr = NULL;
			} else {
				attr = node->properties;
				if (Z_TYPE_P(member) == IS_LONG) {
					while (attr && nodendx <= Z_LVAL_P(member)) {
						if (match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix)) {
							if (nodendx == Z_LVAL_P(member)) {
								_node_as_zval(sxe, (xmlNodePtr) attr, return_value, SXE_ITER_NONE, NULL, NULL TSRMLS_CC);
								break;
							}
							nodendx++;
						}
						attr = attr->next;
					}
				} else {
					while (attr) {
						if (!xmlStrcmp(attr->name, name) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix)) {
							_node_as_zval(sxe, (xmlNodePtr) attr, return_value, SXE_ITER_NONE, NULL, NULL TSRMLS_CC);
							break;
						}
						attr = attr->next;
					}
				}
			}
		}

		if (elements) {
			if (!sxe->node) {
				php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, node, NULL TSRMLS_CC);
			}
			if (Z_TYPE_P(member) == IS_LONG) {
				if (sxe->iter.type == SXE_ITER_NONE || sxe->iter.type == SXE_ITER_ATTR) {
					node = NULL;
				}

				prefix = sxe->iter.nsprefix;

				while (node && nodendx <= Z_LVAL_P(member)) {
					SKIP_TEXT(node)
					if (node->type == XML_ELEMENT_NODE) {
						if (match_ns(sxe, node, prefix)) {
							if (sxe->iter.type == SXE_ITER_ELEMENT && !xmlStrcmp(node->name, sxe->iter.name)) {
								if (nodendx == Z_LVAL_P(member)) {
									break;
								}
								nodendx++;
							}
						} else {
							break;
						}
					}
next_iter:
					node = node->next;
				}
				if (node) {
					_node_as_zval(sxe, node, return_value, SXE_ITER_NONE, NULL, NULL TSRMLS_CC);
				}
			} else {
				_node_as_zval(sxe, node, return_value, SXE_ITER_ELEMENT, name, sxe->iter.nsprefix TSRMLS_CC);
			}
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
static zval * sxe_property_read(zval *object, zval *member, zend_bool silent TSRMLS_DC)
{
	return sxe_prop_dim_read(object, member, 1, 0, silent TSRMLS_CC);
}
/* }}} */

/* {{{ sxe_dimension_read()
 */
static zval * sxe_dimension_read(zval *object, zval *offset TSRMLS_DC)
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
			php_error(E_WARNING, "It is not possible to assign complex types to nodes");
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
	int				itercount = 0;
	zval            tmp_zv, trim_zv;

	if (!member) {
		/* This happens when the user did: $sxe[] = $value
		 * and could also be E_PARSE, but we use this only during parsing
		 * and this is during runtime.
		 */
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot create unnamed attribute");
		return;
	}

	if (Z_TYPE_P(member) == IS_LONG) {
		if (Z_LVAL_P(member) == 0) {
			elements = 1;
			attribs = 0;
		} else
			return;
	} else
		if (Z_TYPE_P(member) != IS_STRING) {
			trim_zv = *member;
			zval_copy_ctor(&trim_zv);
			convert_to_string(&trim_zv);
			php_trim(Z_STRVAL(trim_zv), Z_STRLEN(trim_zv), NULL, 0, &tmp_zv, 3 TSRMLS_CC);
			zval_dtor(&trim_zv);
			member = &tmp_zv;
		}

	if (!Z_STRLEN_P(member)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot write or create unnamed %s", attribs ? "attribute" : "element");
		if (member == &tmp_zv) {
			zval_dtor(&tmp_zv);
		}
		return;
	}

	name = Z_STRVAL_P(member);
	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	if (node) {
		if (attribs) {
			attr = node->properties;
			while (attr) {
				if (!xmlStrcmp(attr->name, name) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix)) {
					is_attr = 1;
					++counter;
					break;
				}
				attr = attr->next;
			}

		}

		if (elements) {
			if (Z_TYPE_P(member) == IS_LONG) {
				newnode = node;
				++counter;
			} else {
				node = node->children;
				while (node) {
					SKIP_TEXT(node);

					if (!xmlStrcmp(node->name, name)) {
						newnode = node;
						++counter;
					}
		
next_iter:
					itercount++;
					node = node->next;
				}
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
			switch (Z_TYPE_P(value)) {
				case IS_LONG:
				case IS_BOOL:
				case IS_DOUBLE:
				case IS_NULL:
					convert_to_string(value);
				case IS_STRING:
					newnode = (xmlNodePtr)xmlNewProp(node, name, Z_STRVAL_P(value));
					break;
				default:
					php_error(E_WARNING, "It is not yet possible to assign complex types to attributes");
			}
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
	sxe_prop_dim_write(object, member, value, 1, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ sxe_dimension_write()
 */
static void sxe_dimension_write(zval *object, zval *offset, zval *value TSRMLS_DC)
{
	sxe_prop_dim_write(object, offset, value, 0, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ sxe_prop_dim_exists()
 */
static int sxe_prop_dim_exists(zval *object, zval *member, int check_empty, zend_bool elements, zend_bool attribs TSRMLS_DC)
{
	php_sxe_object *sxe;
	char           *name;
	xmlNodePtr      node;
	xmlAttrPtr      attr = NULL;
	int				exists = 0;
	int				nodendx = 0;
	
	sxe = php_sxe_fetch_object(object TSRMLS_CC);
	name = Z_STRVAL_P(member);

	GET_NODE(sxe, node);

	if (sxe->iter.type != SXE_ITER_CHILD && sxe->iter.type != SXE_ITER_ATTRLIST) {
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
	}

	if (node) {
		if (attribs) {
			attr = node->properties;
			while (attr) {
				if (!xmlStrcmp(attr->name, name) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix)) {
					exists = 1;
					break;
				}
	
				attr = attr->next;
			}
		}

		if (elements) {
			if (Z_TYPE_P(member) == IS_LONG) {
				if (sxe->iter.type == SXE_ITER_NONE || sxe->iter.type == SXE_ITER_ATTR) {
					node = NULL;
				}

				while (node && nodendx <= Z_LVAL_P(member)) {
					SKIP_TEXT(node)
					if (node->type == XML_ELEMENT_NODE) {
						if (match_ns(sxe, node, sxe->iter.nsprefix)) {
							if (sxe->iter.type == SXE_ITER_ELEMENT && !xmlStrcmp(node->name, sxe->iter.name)) {
								if (nodendx == Z_LVAL_P(member)) {
									break;
								}
								nodendx++;
							}
						} else {
							break;
						}
					}
next_iter:
					node = node->next;
				}
			}

			if (node) {
				return 1;
			}
		}
	}

	return exists;
}
/* }}} */

/* {{{ sxe_property_exists()
 */
static int sxe_property_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	return sxe_prop_dim_exists(object, member, check_empty, 1, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ sxe_property_exists()
 */
static int sxe_dimension_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	return sxe_prop_dim_exists(object, member, check_empty, 0, 1 TSRMLS_CC);
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
	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	if (node) {
		if (attribs) {
			attr = node->properties;
			while (attr) {
				anext = attr->next;
				if (!xmlStrcmp(attr->name, Z_STRVAL_P(member)) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix)) {
					xmlUnlinkNode((xmlNodePtr) attr);
					php_libxml_node_free_resource((xmlNodePtr) attr TSRMLS_CC);
					break;
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
					php_libxml_node_free_resource(node TSRMLS_CC);
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
static void sxe_property_delete(zval *object, zval *member TSRMLS_DC)
{
	sxe_prop_dim_delete(object, member, 1, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ sxe_dimension_unset()
 */
static void sxe_dimension_delete(zval *object, zval *offset TSRMLS_DC)
{
	sxe_prop_dim_delete(object, offset, 0, 1 TSRMLS_CC);
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
		subnode = php_sxe_object_new(sxe_ref->zo.ce TSRMLS_CC);
		subnode->document = sxe_ref->document;
		subnode->document->refcount++;
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
	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	if (node) {
		node = node->children;

		while (node) {
			if (node->children != NULL || node->prev != NULL || node->next != NULL) {
				SKIP_TEXT(node);
			} else {
				if (node->type == XML_TEXT_NODE) {
					MAKE_STD_ZVAL(value);
					ZVAL_STRING(value, xmlNodeListGetString(node->doc, node, 1), 1);
					zend_hash_next_index_insert(rv, &value, sizeof(zval *), NULL);
					goto next_iter;
				}
			}
			
			name = (char *) node->name;
			if (!name) {
				goto next_iter;
			} else {
				namelen = xmlStrlen(node->name) + 1;
			}

			_get_base_node_value(sxe, node, &value TSRMLS_CC);
			
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

/* {{{ xpath()
 */ 
SXE_METHOD(xpath)
{
	php_sxe_object    *sxe;
	zval              *value;
	char              *query;
	int                query_len;
	int                i;
	int                nsnbr = 0;
	xmlNsPtr          *ns = NULL;
	xmlXPathObjectPtr  retval;
	xmlNodeSetPtr      result;
	xmlNodePtr		   nodeptr;

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

 	ns = xmlGetNsList((xmlDocPtr) sxe->document->ptr, (xmlNodePtr) sxe->node->node);
	if (ns != NULL) {
		while (ns[nsnbr] != NULL) {
			nsnbr++;
		}
	}

	sxe->xpath->namespaces = ns;
	sxe->xpath->nsNr = nsnbr;

	retval = xmlXPathEval(query, sxe->xpath);
	if (ns != NULL) {
		xmlFree(ns);
		sxe->xpath->namespaces = NULL;
		sxe->xpath->nsNr = 0;
	}

	if (!retval) {
		RETURN_FALSE;
	}

	result = retval->nodesetval;
	if (!result) {
		xmlXPathFreeObject(retval);
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i = 0; i < result->nodeNr; ++i) {
		nodeptr = result->nodeTab[i];
		if (nodeptr->type == XML_TEXT_NODE || nodeptr->type == XML_ELEMENT_NODE || nodeptr->type == XML_ATTRIBUTE_NODE) {
			MAKE_STD_ZVAL(value);
			/**
			 * Detect the case where the last selector is text(), simplexml
			 * always accesses the text() child by default, therefore we assign
			 * to the parent node.
			 */
			if (nodeptr->type == XML_TEXT_NODE) {
				_node_as_zval(sxe, nodeptr->parent, value, SXE_ITER_NONE, NULL, NULL TSRMLS_CC);
			} else  {
				_node_as_zval(sxe, nodeptr, value, SXE_ITER_NONE, NULL, NULL TSRMLS_CC);
			}

			add_next_index_zval(return_value, value);
		}
	}
	
	xmlXPathFreeObject(retval);
}
/* }}} */

/* {{{ proto asXML([string filename])
 */
SXE_METHOD(asXML)
{
	php_sxe_object     *sxe;
	xmlNodePtr          node;
	xmlOutputBufferPtr  outbuf;
	xmlChar            *strval;
	int                 strval_len;
	char               *filename;
	int                 filename_len;

	if (ZEND_NUM_ARGS() > 1) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
			RETURN_FALSE;
		}
		
		sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
		GET_NODE(sxe, node);
		
		if (node) {
			if (XML_DOCUMENT_NODE == node->parent->type) {
				xmlSaveFile(filename, (xmlDocPtr) sxe->document->ptr);
			} else {
				outbuf = xmlOutputBufferCreateFilename(filename, NULL, 0);

				if (outbuf == NULL) {
					RETURN_FALSE;
				}		

				xmlNodeDumpOutput(outbuf, (xmlDocPtr) sxe->document->ptr, node, 0, 1, NULL);
				xmlOutputBufferClose(outbuf);
				RETURN_TRUE;
			}
		} else {
			RETURN_FALSE;
		}
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	GET_NODE(sxe, node);

	if (node) {
		if (XML_DOCUMENT_NODE == node->parent->type) {
			xmlDocDumpMemory((xmlDocPtr) sxe->document->ptr, &strval, &strval_len);
		} else {
			/* Should we be passing encoding information instead of NULL? */
			outbuf = xmlAllocOutputBuffer(NULL);

			if (outbuf == NULL) {
				RETURN_FALSE;
			}		

			xmlNodeDumpOutput(outbuf, (xmlDocPtr) sxe->document->ptr, node, 0, 1, ((xmlDocPtr) sxe->document->ptr)->encoding);
			xmlOutputBufferFlush(outbuf);
			strval = xmlStrndup(outbuf->buffer->content, outbuf->buffer->use);	
			xmlOutputBufferClose(outbuf);
		}

		RETVAL_STRINGL(strval, strlen(strval), 1);
		xmlFree(strval);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ simplexml_children()
 */
SXE_METHOD(children)
{
	php_sxe_object *sxe;
	char           *nsprefix = NULL;
	int             nsprefix_len;
	xmlNodePtr      node;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!", &nsprefix, &nsprefix_len) == FAILURE) {
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	_node_as_zval(sxe, node, return_value, SXE_ITER_CHILD, NULL, nsprefix TSRMLS_CC);

}
/* }}} */

/* {{{ simplexml_attributes()
 */
SXE_METHOD(attributes)
{
	php_sxe_object *sxe;
	char           *nsprefix = NULL;
	int             nsprefix_len;
	xmlNodePtr      node;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!", &nsprefix, &nsprefix_len) == FAILURE) {
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	_node_as_zval(sxe, node, return_value, SXE_ITER_ATTRLIST, NULL, nsprefix TSRMLS_CC);
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
	xmlNodePtr	    node;
	zval free_obj;
	int rv;

	sxe = php_sxe_fetch_object(readobj TSRMLS_CC);

	if (should_free) {
		free_obj = *writeobj;
	}

	if (sxe->iter.type != SXE_ITER_NONE) {
		node = php_sxe_get_first_node(sxe, NULL TSRMLS_CC);
		if (node) {
			contents = xmlNodeListGetString((xmlDocPtr) sxe->document->ptr, node->children, 1);
		}
	} else {
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
	}

	rv = cast_object(writeobj, type, contents TSRMLS_CC);

	if (contents) {
		xmlFree(contents);
	}
	if (should_free) {
		zval_dtor(&free_obj);
	}
	return rv;
}
/* }}} */

static zend_object_handlers sxe_object_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	sxe_property_read,
	sxe_property_write,
	sxe_dimension_read,
	sxe_dimension_write,
	NULL,
	NULL,
	NULL,
	sxe_property_exists,
	sxe_property_delete,
	sxe_dimension_exists,
	sxe_dimension_delete,
	sxe_properties_get,
	NULL, /* zend_get_std_object_handlers()->get_method,*/
	NULL, /* zend_get_std_object_handlers()->call_method,*/
	NULL, /* zend_get_std_object_handlers()->get_constructor, */
	NULL, /* zend_get_std_object_handlers()->get_class_entry,*/
	NULL, /* zend_get_std_object_handlers()->get_class_name,*/
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

	clone = php_sxe_object_new(sxe->zo.ce TSRMLS_CC);
	clone->document = sxe->document;
	if (clone->document) {
		clone->document->refcount++;
		docp = clone->document->ptr;
	}
	if (sxe->node) {
		nodep = xmlDocCopyNode(sxe->node->node, docp, 1);
	}

	php_libxml_increment_node_ptr((php_libxml_node_object *)clone, nodep, NULL TSRMLS_CC);

	*clone_ptr = (void *) clone;
}
/* }}} */

/* {{{ sxe_object_dtor()
 */
static void sxe_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	php_sxe_object *sxe;

	sxe = (php_sxe_object *) object;

	zend_hash_destroy(sxe->zo.properties);
	FREE_HASHTABLE(sxe->zo.properties);

	if (sxe->iter.data) {
		zval_ptr_dtor(&sxe->iter.data);
	}

	if (sxe->iter.name) {
		xmlFree(sxe->iter.name);
	}
	if (sxe->iter.nsprefix) {
		xmlFree(sxe->iter.nsprefix);
	}

	php_libxml_node_decrement_resource((php_libxml_node_object *)sxe TSRMLS_CC);

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
static php_sxe_object* php_sxe_object_new(zend_class_entry *ce TSRMLS_DC)
{
	php_sxe_object *intern;

	intern = ecalloc(1, sizeof(php_sxe_object));
	intern->zo.ce = ce;

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
ZEND_API zend_object_value
sxe_object_new(zend_class_entry *ce TSRMLS_DC)
{
	php_sxe_object    *intern;

	intern = php_sxe_object_new(ce TSRMLS_CC);
	return php_sxe_register_object(intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto simplemxml_element simplexml_load_file(string filename [, string class_name])
   Load a filename and return a simplexml_element object to allow for processing */
PHP_FUNCTION(simplexml_load_file)
{
	php_sxe_object *sxe;
	char           *filename;
	int             filename_len;
	xmlDocPtr       docp;
	char           *classname = "";
	int             classname_len = 0;
	zend_class_entry *ce= sxe_class_entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &filename, &filename_len, &classname, &classname_len) == FAILURE) {
		return;
	}

	docp = xmlParseFile(filename);
	if (! docp) {
		RETURN_FALSE;
	}

	if (classname_len) {
		zend_class_entry **pce;
		if (zend_lookup_class(classname, classname_len, &pce TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Class %s does not exist", classname);
		}
		ce = *pce;
	}

	sxe = php_sxe_object_new(ce TSRMLS_CC);
	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp TSRMLS_CC);
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL TSRMLS_CC);

	return_value->type = IS_OBJECT;
	return_value->value.obj = php_sxe_register_object(sxe TSRMLS_CC);
}
/* }}} */

/* {{{ proto simplemxml_element simplexml_load_string(string data [, string class_name])
   Load a string and return a simplexml_element object to allow for processing */
PHP_FUNCTION(simplexml_load_string)
{
	php_sxe_object *sxe;
	char           *data;
	int             data_len;
	xmlDocPtr       docp;
	char           *classname = "";
	int             classname_len = 0;
	zend_class_entry *ce= sxe_class_entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &data, &data_len, &classname, &classname_len) == FAILURE) {
		return;
	}

	docp = xmlParseMemory(data, data_len);
	if (! docp) {
		RETURN_FALSE;
	}

	if (classname_len) {
		zend_class_entry **pce;
		if (zend_lookup_class(classname, classname_len, &pce TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Class %s does not exist", classname);
		}
		ce = *pce;
	}

	sxe = php_sxe_object_new(ce TSRMLS_CC);
	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp TSRMLS_CC);
	sxe->iter.type = SXE_ITER_NONE;
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL TSRMLS_CC);

	return_value->type = IS_OBJECT;
	return_value->value.obj = php_sxe_register_object(sxe TSRMLS_CC);
}
/* }}} */

SXE_METHOD(__construct)
{
	php_sxe_object *sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	char           *data;
	int             data_len;
	xmlDocPtr       docp;

	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
		php_std_error_handling();
		return;
	}

	php_std_error_handling();
	docp = xmlParseMemory(data, data_len);
	if (!docp) {
		(php_libxml_node_object *)sxe->document = NULL;
		zend_throw_exception(zend_exception_get_default(), "String could not be parsed as XML", 0 TSRMLS_CC);
		return;
	}

	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp TSRMLS_CC);
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL TSRMLS_CC);
}

typedef struct {
	zend_object_iterator  intern;
	php_sxe_object        *sxe;
} php_sxe_iterator;

static void php_sxe_iterator_dtor(zend_object_iterator *iter TSRMLS_DC);
static int php_sxe_iterator_has_more(zend_object_iterator *iter TSRMLS_DC);
static void php_sxe_iterator_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC);
static int php_sxe_iterator_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC);
static void php_sxe_iterator_move_forward(zend_object_iterator *iter TSRMLS_DC);
static void php_sxe_iterator_rewind(zend_object_iterator *iter TSRMLS_DC);

zend_object_iterator_funcs php_sxe_iterator_funcs = {
	php_sxe_iterator_dtor,
	php_sxe_iterator_has_more,
	php_sxe_iterator_current_data,
	php_sxe_iterator_current_key,
	php_sxe_iterator_move_forward,
	php_sxe_iterator_rewind,
};

ZEND_API void php_sxe_reset_iterator(php_sxe_object *sxe TSRMLS_DC)
{
	xmlNodePtr node;
	char *prefix;

	if (sxe->iter.data) {
		zval_ptr_dtor(&sxe->iter.data);
		sxe->iter.data = NULL;
	}

	GET_NODE(sxe, node)

	if (node) {
		switch (sxe->iter.type) {
			case SXE_ITER_ELEMENT:
			case SXE_ITER_CHILD:
			case SXE_ITER_NONE:
				node = node->children;
				break;
			case SXE_ITER_ATTR:
			case SXE_ITER_ATTRLIST:
				node = (xmlNodePtr) node->properties;
		}
	}

	prefix = sxe->iter.nsprefix;

	while (node) {
		SKIP_TEXT(node);
		if (sxe->iter.type != SXE_ITER_ATTR && sxe->iter.type != SXE_ITER_ATTRLIST && node->type == XML_ELEMENT_NODE) {
			if (sxe->iter.type == SXE_ITER_ELEMENT) { 
				if (!xmlStrcmp(node->name, sxe->iter.name) && match_ns(sxe, node, prefix)) {
					break;
				}
			} else {
				if (match_ns(sxe, node, prefix)) {
					break;
				}
			}
		} else {
			if (node->type == XML_ATTRIBUTE_NODE) {
				if (sxe->iter.type == SXE_ITER_ATTR) {
					if (!xmlStrcmp(node->name, sxe->iter.name) && match_ns(sxe, node, sxe->iter.nsprefix)) {
						break;
					}
				} else {
					if (match_ns(sxe, node, sxe->iter.nsprefix)) {
						break;
					}
				}
			}
		}
next_iter:
		node = node->next;
	}

	if (node) {
		ALLOC_INIT_ZVAL(sxe->iter.data);
		_node_as_zval(sxe, node, sxe->iter.data, SXE_ITER_NONE, NULL, NULL TSRMLS_CC);
	}
}

zend_object_iterator *php_sxe_get_iterator(zend_class_entry *ce, zval *object TSRMLS_DC)
{
	php_sxe_iterator *iterator = emalloc(sizeof(php_sxe_iterator));

	object->refcount++;
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = &php_sxe_iterator_funcs;
	iterator->sxe = php_sxe_fetch_object(object TSRMLS_CC);
	
	return (zend_object_iterator*)iterator;
}

static void php_sxe_iterator_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	zval_ptr_dtor((zval**)&iterator->intern.data);
	
	efree(iterator);
}
	
static int php_sxe_iterator_has_more(zend_object_iterator *iter TSRMLS_DC)
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	return iterator->sxe->iter.data ? SUCCESS : FAILURE;
}

static void php_sxe_iterator_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	*data = &iterator->sxe->iter.data;
}

static int php_sxe_iterator_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	zval *curobj;
	xmlNodePtr curnode = NULL;
	php_sxe_object *intern;
	int namelen;

	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	curobj = iterator->sxe->iter.data;

	intern = (php_sxe_object *)zend_object_store_get_object(curobj TSRMLS_CC);
	if (intern != NULL && intern->node != NULL) {
		curnode = (xmlNodePtr)((php_libxml_node_ptr *)intern->node)->node;
	}

	namelen = xmlStrlen(curnode->name);
	*str_key = estrndup(curnode->name, namelen);
	*str_key_len = namelen + 1;
	return HASH_KEY_IS_STRING;

}

ZEND_API void php_sxe_move_forward_iterator(php_sxe_object *sxe TSRMLS_DC)
{
	xmlNodePtr      node;
	php_sxe_object  *intern;
	char *prefix;

	if (sxe->iter.data) {
		intern = (php_sxe_object *)zend_object_store_get_object(sxe->iter.data TSRMLS_CC);
		GET_NODE(intern, node)
		zval_ptr_dtor(&sxe->iter.data);
		sxe->iter.data = NULL;
	} else {
		node = sxe->iter.node->node;
	}

	if (node) {
		node = node->next;
	}

	prefix = sxe->iter.nsprefix;

	while (node) {
		SKIP_TEXT(node);

		if (sxe->iter.type != SXE_ITER_ATTR && sxe->iter.type != SXE_ITER_ATTRLIST && node->type == XML_ELEMENT_NODE) {
			if (sxe->iter.type == SXE_ITER_ELEMENT) { 
				if (!xmlStrcmp(node->name, sxe->iter.name) && match_ns(sxe, node, prefix)) {
					break;
				}
			} else {
				if (match_ns(sxe, node, prefix)) {
					break;
				}
			}
		} else {
			if (node->type == XML_ATTRIBUTE_NODE) {
				if (sxe->iter.type == SXE_ITER_ATTR) {
					if (!xmlStrcmp(node->name, sxe->iter.name) && match_ns(sxe, node, sxe->iter.nsprefix)) {
						break;
					}
				} else {
					if (match_ns(sxe, node, sxe->iter.nsprefix)) {
						break;
					}
				}
			}
		}
next_iter:
		node = node->next;
	}

	if (node) {
		ALLOC_INIT_ZVAL(sxe->iter.data);
		_node_as_zval(sxe, node, sxe->iter.data, SXE_ITER_NONE, NULL, NULL TSRMLS_CC);
	}
}

static void php_sxe_iterator_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	php_sxe_move_forward_iterator(iterator->sxe TSRMLS_CC);
}

static void php_sxe_iterator_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	php_sxe_object	*sxe;

	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	sxe = iterator->sxe;

	php_sxe_reset_iterator(sxe TSRMLS_CC);
}


/* {{{ proto simplemxml_element simplexml_import_dom(domNode node [, string class_name])
   Get a simplexml_element object from dom to allow for processing */
PHP_FUNCTION(simplexml_import_dom)
{
#ifdef HAVE_DOM
	php_sxe_object *sxe;
	zval *node;
	php_libxml_node_object *object;
	xmlNodePtr		nodep = NULL;
	char           *classname = "";
	int             classname_len = 0;
	zend_class_entry *ce= sxe_class_entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|s", &node, &classname, &classname_len) == FAILURE) {
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
		if (classname_len) {
			zend_class_entry **pce;
			if (zend_lookup_class(classname, classname_len, &pce TSRMLS_CC) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Class %s does not exist", classname);
			}
			ce = *pce;
		}
	
		sxe = php_sxe_object_new(ce TSRMLS_CC);
		sxe->document = object->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, nodep->doc TSRMLS_CC);
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
	NULL,
	NULL,
	NULL,
	PHP_MINFO(simplexml),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SIMPLEXML
ZEND_GET_MODULE(simplexml)
#endif

/* the method table */
/* each method can have its own parameters and visibility */
static zend_function_entry sxe_functions[] = {
	SXE_ME(__construct,            NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL) /* must be called */
	SXE_ME(asXML,                  NULL, ZEND_ACC_PUBLIC)
	SXE_ME(xpath,                  NULL, ZEND_ACC_PUBLIC)
	SXE_ME(attributes,             NULL, ZEND_ACC_PUBLIC)
	SXE_ME(children,			   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION(simplexml)
 */
PHP_MINIT_FUNCTION(simplexml)
{
	zend_class_entry sxe;

	INIT_CLASS_ENTRY(sxe, "simplexml_element", sxe_functions);
	sxe.create_object = sxe_object_new;
	sxe_class_entry = zend_register_internal_class(&sxe TSRMLS_CC);
	sxe_class_entry->get_iterator = php_sxe_get_iterator;
	zend_class_implements(sxe_class_entry TSRMLS_CC, 1, zend_ce_traversable);
	sxe_object_handlers.get_method = zend_get_std_object_handlers()->get_method;
	sxe_object_handlers.get_constructor = zend_get_std_object_handlers()->get_constructor;
	sxe_object_handlers.get_class_entry = zend_get_std_object_handlers()->get_class_entry;
	sxe_object_handlers.get_class_name = zend_get_std_object_handlers()->get_class_name;

#if HAVE_SPL && !defined(COMPILE_DL_SPL)
	if (zend_get_module_started("spl") == SUCCESS) {
		PHP_MINIT(spl_sxe)(INIT_FUNC_ARGS_PASSTHRU);
	}
#endif /* HAVE_SPL */

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
