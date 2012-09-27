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
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "sxe.h"

#define SXE_ELEMENT_BY_NAME 0

zend_class_entry *sxe_class_entry = NULL;

PHP_SXE_API zend_class_entry *sxe_get_element_class_entry() /* {{{ */
{
	return sxe_class_entry;
}
/* }}} */

#define SXE_ME(func, arg_info, flags) PHP_ME(simplexml_element, func, arg_info, flags)
#define SXE_MALIAS(func, alias, arg_info, flags) PHP_MALIAS(simplexml_element, func, alias, arg_info, flags)

#define SXE_METHOD(func) PHP_METHOD(simplexml_element, func)

static php_sxe_object* php_sxe_object_new(zend_class_entry *ce TSRMLS_DC);
static zend_object_value php_sxe_register_object(php_sxe_object * TSRMLS_DC);
static xmlNodePtr php_sxe_reset_iterator(php_sxe_object *sxe, int use_data TSRMLS_DC);
static xmlNodePtr php_sxe_iterator_fetch(php_sxe_object *sxe, xmlNodePtr node, int use_data TSRMLS_DC);
static zval *sxe_get_value(zval *z TSRMLS_DC);
static void php_sxe_iterator_dtor(zend_object_iterator *iter TSRMLS_DC);
static int php_sxe_iterator_valid(zend_object_iterator *iter TSRMLS_DC);
static void php_sxe_iterator_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC);
static int php_sxe_iterator_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC);
static void php_sxe_iterator_move_forward(zend_object_iterator *iter TSRMLS_DC);
static void php_sxe_iterator_rewind(zend_object_iterator *iter TSRMLS_DC);

/* {{{ _node_as_zval()
 */
static void _node_as_zval(php_sxe_object *sxe, xmlNodePtr node, zval *value, SXE_ITER itertype, char *name, const xmlChar *nsprefix, int isprefix TSRMLS_DC)
{
	php_sxe_object *subnode;

	subnode = php_sxe_object_new(sxe->zo.ce TSRMLS_CC);
	subnode->document = sxe->document;
	subnode->document->refcount++;
	subnode->iter.type = itertype;
	if (name) {
		subnode->iter.name = xmlStrdup((xmlChar *)name);
	}
	if (nsprefix && *nsprefix) {
		subnode->iter.nsprefix = xmlStrdup(nsprefix);
		subnode->iter.isprefix = isprefix;
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Node no longer exists"); \
	} \
}

static xmlNodePtr php_sxe_get_first_node(php_sxe_object *sxe, xmlNodePtr node TSRMLS_DC) /* {{{ */
{
	php_sxe_object *intern;
	xmlNodePtr retnode = NULL;

	if (sxe && sxe->iter.type != SXE_ITER_NONE) {
		php_sxe_reset_iterator(sxe, 1 TSRMLS_CC);
		if (sxe->iter.data) {
			intern = (php_sxe_object *)zend_object_store_get_object(sxe->iter.data TSRMLS_CC);
			GET_NODE(intern, retnode)
		}
		return retnode;
	} else {
		return node;
	}
}
/* }}} */

static inline int match_ns(php_sxe_object *sxe, xmlNodePtr node, xmlChar *name, int prefix) /* {{{ */
{
	if (name == NULL && (node->ns == NULL || node->ns->prefix == NULL)) {
		return 1;
	}

	if (node->ns && !xmlStrcmp(prefix ? node->ns->prefix : node->ns->href, name)) {
		return 1;
	}

	return 0;
}
/* }}} */

static xmlNodePtr sxe_get_element_by_offset(php_sxe_object *sxe, long offset, xmlNodePtr node, long *cnt) /* {{{ */
{
	long nodendx = 0;

	if (sxe->iter.type == SXE_ITER_NONE) {
		if (offset == 0) {
			if (cnt) {
				*cnt = 0;
			}
			return node;
		} else {
			return NULL;
		}
	}
	while (node && nodendx <= offset) {
		SKIP_TEXT(node)
		if (node->type == XML_ELEMENT_NODE && match_ns(sxe, node, sxe->iter.nsprefix, sxe->iter.isprefix)) {
			if (sxe->iter.type == SXE_ITER_CHILD || (
				sxe->iter.type == SXE_ITER_ELEMENT && !xmlStrcmp(node->name, sxe->iter.name))) {
				if (nodendx == offset) {
					break;
				}
				nodendx++;
			}
		}
next_iter:
		node = node->next;
	}

	if (cnt) {
		*cnt = nodendx;
	}

	return node;
}
/* }}} */

static xmlNodePtr sxe_find_element_by_name(php_sxe_object *sxe, xmlNodePtr node, xmlChar *name TSRMLS_DC) /* {{{ */
{
	while (node) {
		SKIP_TEXT(node)
		if (node->type == XML_ELEMENT_NODE && match_ns(sxe, node, sxe->iter.nsprefix, sxe->iter.isprefix)) {
			if (!xmlStrcmp(node->name, name)) {
				return node;
			}
		}
next_iter:
		node = node->next;
	}
	return NULL;
} /* }}} */

static xmlNodePtr sxe_get_element_by_name(php_sxe_object *sxe, xmlNodePtr node, char **name, SXE_ITER *type TSRMLS_DC) /* {{{ */
{
	int         orgtype;
	xmlNodePtr  orgnode = node;
	xmlNodePtr  retnode = NULL;

	if (sxe->iter.type != SXE_ITER_ATTRLIST)
	{
		orgtype = sxe->iter.type;
		if (sxe->iter.type == SXE_ITER_NONE) {
			sxe->iter.type = SXE_ITER_CHILD;
		}
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
		sxe->iter.type = orgtype;
	}

	if (sxe->iter.type == SXE_ITER_ELEMENT) {
		orgnode = sxe_find_element_by_name(sxe, node, sxe->iter.name TSRMLS_CC);
		if (!orgnode) {
			return NULL;
		}
		node = orgnode->children;
	}

	while (node) {
		SKIP_TEXT(node)
		if (node->type == XML_ELEMENT_NODE && match_ns(sxe, node, sxe->iter.nsprefix, sxe->iter.isprefix)) {
			if (!xmlStrcmp(node->name, (xmlChar *)*name)) {
				if (1||retnode)
				{
					*type = SXE_ITER_ELEMENT;
					return orgnode;
				}
				retnode = node;
			}
		}
next_iter:
		node = node->next;
	}

	if (retnode)
	{
		*type = SXE_ITER_NONE;
		*name = NULL;
		return retnode;
	}

	return NULL;
}
/* }}} */

/* {{{ sxe_prop_dim_read()
 */
static zval * sxe_prop_dim_read(zval *object, zval *member, zend_bool elements, zend_bool attribs, int type TSRMLS_DC)
{
	zval           *return_value;
	php_sxe_object *sxe;
	char           *name;
	xmlNodePtr      node;
	xmlAttrPtr      attr = NULL;
	zval            tmp_zv;
	int             nodendx = 0;
	int             test = 0;

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	if (!member || Z_TYPE_P(member) == IS_LONG) {
		if (sxe->iter.type != SXE_ITER_ATTRLIST) {
			attribs = 0;
			elements = 1;
		} else if (!member) {
			/* This happens when the user did: $sxe[]->foo = $value */
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot create unnamed attribute");
			return NULL;
		}
		name = NULL;
	} else {
		if (Z_TYPE_P(member) != IS_STRING) {
			tmp_zv = *member;
			zval_copy_ctor(&tmp_zv);
			member = &tmp_zv;
			convert_to_string(member);
		}
		name = Z_STRVAL_P(member);
	}

	GET_NODE(sxe, node);

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		attribs = 1;
		elements = 0;
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
		attr = (xmlAttrPtr)node;
		test = sxe->iter.name != NULL;
	} else if (sxe->iter.type != SXE_ITER_CHILD) {
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
		attr = node ? node->properties : NULL;
		test = 0;
		if (!member && node && node->parent &&
		    node->parent->type == XML_DOCUMENT_NODE) {
			/* This happens when the user did: $sxe[]->foo = $value */
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot create unnamed attribute");
			return NULL;
		}
	}

	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	if (node) {
		if (attribs) {
			if (Z_TYPE_P(member) != IS_LONG || sxe->iter.type == SXE_ITER_ATTRLIST) {
				if (Z_TYPE_P(member) == IS_LONG) {
					while (attr && nodendx <= Z_LVAL_P(member)) {
						if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
							if (nodendx == Z_LVAL_P(member)) {
								_node_as_zval(sxe, (xmlNodePtr) attr, return_value, SXE_ITER_NONE, NULL, sxe->iter.nsprefix, sxe->iter.isprefix TSRMLS_CC);
								break;
							}
							nodendx++;
						}
						attr = attr->next;
					}
				} else {
					while (attr) {
						if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && !xmlStrcmp(attr->name, (xmlChar *)name) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
							_node_as_zval(sxe, (xmlNodePtr) attr, return_value, SXE_ITER_NONE, NULL, sxe->iter.nsprefix, sxe->iter.isprefix TSRMLS_CC);
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
			if (!member || Z_TYPE_P(member) == IS_LONG) {
				long cnt = 0;
				xmlNodePtr mynode = node;

				if (sxe->iter.type == SXE_ITER_CHILD) {
					node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
				}
				if (sxe->iter.type == SXE_ITER_NONE) {
					if (member && Z_LVAL_P(member) > 0) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot add element %s number %ld when only 0 such elements exist", mynode->name, Z_LVAL_P(member));
					}
				} else if (member) {
					node = sxe_get_element_by_offset(sxe, Z_LVAL_P(member), node, &cnt);
				} else {
					node = NULL;
				}
				if (node) {
					_node_as_zval(sxe, node, return_value, SXE_ITER_NONE, NULL, sxe->iter.nsprefix, sxe->iter.isprefix TSRMLS_CC);
				} else if (type == BP_VAR_W || type == BP_VAR_RW) {
					if (member && cnt < Z_LVAL_P(member)) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot add element %s number %ld when only %ld such elements exist", mynode->name, Z_LVAL_P(member), cnt);
					}
					node = xmlNewTextChild(mynode->parent, mynode->ns, mynode->name, NULL);
					_node_as_zval(sxe, node, return_value, SXE_ITER_NONE, NULL, sxe->iter.nsprefix, sxe->iter.isprefix TSRMLS_CC);
				}
			} else {
#if SXE_ELEMENT_BY_NAME
				int newtype;

				GET_NODE(sxe, node);
				node = sxe_get_element_by_name(sxe, node, &name, &newtype TSRMLS_CC);
				if (node) {
					_node_as_zval(sxe, node, return_value, newtype, name, sxe->iter.nsprefix, sxe->iter.isprefix TSRMLS_CC);
				}
#else
				_node_as_zval(sxe, node, return_value, SXE_ITER_ELEMENT, name, sxe->iter.nsprefix, sxe->iter.isprefix TSRMLS_CC);
#endif
			}
		}
	}

	Z_SET_REFCOUNT_P(return_value, 0);
	Z_UNSET_ISREF_P(return_value);

	if (member == &tmp_zv) {
		zval_dtor(&tmp_zv);
	}
	if (Z_TYPE_P(return_value) == IS_NULL) {
		FREE_ZVAL(return_value);
		return_value = &EG(uninitialized_zval);
	}

	return return_value;
}
/* }}} */

/* {{{ sxe_property_read()
 */
static zval * sxe_property_read(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC)
{
	return sxe_prop_dim_read(object, member, 1, 0, type TSRMLS_CC);
}
/* }}} */

/* {{{ sxe_dimension_read()
 */
static zval * sxe_dimension_read(zval *object, zval *offset, int type TSRMLS_DC)
{
	return sxe_prop_dim_read(object, offset, 0, 1, type TSRMLS_CC);
}
/* }}} */

/* {{{ change_node_zval()
 */
static void change_node_zval(xmlNodePtr node, zval *value TSRMLS_DC)
{
	zval value_copy;
	xmlChar *buffer;
	int buffer_len;

	if (!value)
	{
		xmlNodeSetContentLen(node, (xmlChar *)"", 0);
		return;
	}
	switch (Z_TYPE_P(value)) {
		case IS_LONG:
		case IS_BOOL:
		case IS_DOUBLE:
		case IS_NULL:
			if (Z_REFCOUNT_P(value) > 1) {
				value_copy = *value;
				zval_copy_ctor(&value_copy);
				value = &value_copy;
			}
			convert_to_string(value);
			/* break missing intentionally */
		case IS_STRING:
			buffer = xmlEncodeEntitiesReentrant(node->doc, (xmlChar *)Z_STRVAL_P(value));
			buffer_len = xmlStrlen(buffer);
			/* check for NULL buffer in case of memory error in xmlEncodeEntitiesReentrant */
			if (buffer) {
				xmlNodeSetContentLen(node, buffer, buffer_len);
				xmlFree(buffer);
			}
			if (value == &value_copy) {
				zval_dtor(value);
			}
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "It is not possible to assign complex types to nodes");
			break;
	}
}
/* }}} */

/* {{{ sxe_property_write()
 */
static int sxe_prop_dim_write(zval *object, zval *member, zval *value, zend_bool elements, zend_bool attribs, xmlNodePtr *pnewnode TSRMLS_DC)
{
	php_sxe_object *sxe;
	xmlNodePtr      node;
	xmlNodePtr      newnode = NULL;
	xmlNodePtr      mynode;
	xmlNodePtr		tempnode;
	xmlAttrPtr      attr = NULL;
	int             counter = 0;
	int             is_attr = 0;
	int				nodendx = 0;
	int             test = 0;
	int				new_value = 0;
	long            cnt = 0;
	int				retval = SUCCESS;
	zval            tmp_zv, trim_zv, value_copy;

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	if (!member || Z_TYPE_P(member) == IS_LONG) {
		if (sxe->iter.type != SXE_ITER_ATTRLIST) {
			attribs = 0;
			elements = 1;
		} else if (!member) {
			/* This happens when the user did: $sxe[] = $value
			 * and could also be E_PARSE, but we use this only during parsing
			 * and this is during runtime.
			 */
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot create unnamed attribute");
			return FAILURE;
		}
	} else {
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
			return FAILURE;
		}
	}

	GET_NODE(sxe, node);

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		attribs = 1;
		elements = 0;
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
		attr = (xmlAttrPtr)node;
		test = sxe->iter.name != NULL;
	} else if (sxe->iter.type != SXE_ITER_CHILD) {
		mynode = node;
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
		attr = node ? node->properties : NULL;
		test = 0;
		if (!member && node && node->parent &&
		    node->parent->type == XML_DOCUMENT_NODE) {
			/* This happens when the user did: $sxe[] = $value
			 * and could also be E_PARSE, but we use this only during parsing
			 * and this is during runtime.
			 */
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot create unnamed attribute");
			return FAILURE;
		}
		if (attribs && !node && sxe->iter.type == SXE_ITER_ELEMENT) {
			node = xmlNewChild(mynode, mynode->ns, sxe->iter.name, NULL);
			attr = node->properties;
		}
	}

	mynode = node;

	if (value) {
		switch (Z_TYPE_P(value)) {
			case IS_LONG:
			case IS_BOOL:
			case IS_DOUBLE:
			case IS_NULL:
				if (Z_REFCOUNT_P(value) > 1) {
					value_copy = *value;
					zval_copy_ctor(&value_copy);
					value = &value_copy;
				}
				convert_to_string(value);
				break;
			case IS_STRING:
				break;
			case IS_OBJECT:
				if (Z_OBJCE_P(value) == sxe_class_entry) {
					value = sxe_get_value(value TSRMLS_CC);
					INIT_PZVAL(value);
					new_value = 1;
					break;
				}
				/* break is missing intentionally */
			default:
				if (member == &tmp_zv) {
					zval_dtor(&tmp_zv);
				}
				zend_error(E_WARNING, "It is not yet possible to assign complex types to %s", attribs ? "attributes" : "properties");
				return FAILURE;
		}
	}

	if (node) {
		if (attribs) {
			if (Z_TYPE_P(member) == IS_LONG) {
				while (attr && nodendx <= Z_LVAL_P(member)) {
					if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
						if (nodendx == Z_LVAL_P(member)) {
							is_attr = 1;
							++counter;
							break;
						}
						nodendx++;
					}
					attr = attr->next;
				}
			} else {
				while (attr) {
					if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && !xmlStrcmp(attr->name, (xmlChar *)Z_STRVAL_P(member)) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
						is_attr = 1;
						++counter;
						break;
					}
					attr = attr->next;
				}
			}

		}

		if (elements) {
			if (!member || Z_TYPE_P(member) == IS_LONG) {
				if (node->type == XML_ATTRIBUTE_NODE) {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot create duplicate attribute");
					return FAILURE;
				}

				if (sxe->iter.type == SXE_ITER_NONE) {
					newnode = node;
					++counter;
					if (member && Z_LVAL_P(member) > 0) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot add element %s number %ld when only 0 such elements exist", mynode->name, Z_LVAL_P(member));
						retval = FAILURE;
					}
				} else if (member) {
					newnode = sxe_get_element_by_offset(sxe, Z_LVAL_P(member), node, &cnt);
					if (newnode) {
						++counter;
					}
				}
			} else {
				node = node->children;
				while (node) {
					SKIP_TEXT(node);

					if (!xmlStrcmp(node->name, (xmlChar *)Z_STRVAL_P(member))) {
						newnode = node;
						++counter;
					}

next_iter:
					node = node->next;
				}
			}
		}

		if (counter == 1) {
			if (is_attr) {
				newnode = (xmlNodePtr) attr;
			}
			if (value) {
				while ((tempnode = (xmlNodePtr) newnode->children)) {
					xmlUnlinkNode(tempnode);
					php_libxml_node_free_resource((xmlNodePtr) tempnode TSRMLS_CC);
				}
				change_node_zval(newnode, value TSRMLS_CC);
			}
		} else if (counter > 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot assign to an array of nodes (duplicate subnodes or attr detected)");
			retval = FAILURE;
		} else if (elements) {
			if (!node) {
				if (!member || Z_TYPE_P(member) == IS_LONG) {
					newnode = xmlNewTextChild(mynode->parent, mynode->ns, mynode->name, value ? (xmlChar *)Z_STRVAL_P(value) : NULL);
				} else {
					newnode = xmlNewTextChild(mynode, mynode->ns, (xmlChar *)Z_STRVAL_P(member), value ? (xmlChar *)Z_STRVAL_P(value) : NULL);
				}
			} else if (!member || Z_TYPE_P(member) == IS_LONG) {
				if (member && cnt < Z_LVAL_P(member)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot add element %s number %ld when only %ld such elements exist", mynode->name, Z_LVAL_P(member), cnt);
					retval = FAILURE;
				}
				newnode = xmlNewTextChild(mynode->parent, mynode->ns, mynode->name, value ? (xmlChar *)Z_STRVAL_P(value) : NULL);
			}
		} else if (attribs) {
			if (Z_TYPE_P(member) == IS_LONG) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot change attribute number %ld when only %d attributes exist", Z_LVAL_P(member), nodendx);
				retval = FAILURE;
			} else {
				newnode = (xmlNodePtr)xmlNewProp(node, (xmlChar *)Z_STRVAL_P(member), value ? (xmlChar *)Z_STRVAL_P(value) : NULL);
			}
		}
	}

	if (member == &tmp_zv) {
		zval_dtor(&tmp_zv);
	}
	if (pnewnode) {
		*pnewnode = newnode;
	}
	if (value && value == &value_copy) {
		zval_dtor(value);
	}
	if (new_value) {
		zval_ptr_dtor(&value);
	}
	return retval;
}
/* }}} */

/* {{{ sxe_property_write()
 */
static void sxe_property_write(zval *object, zval *member, zval *value, const zend_literal *key TSRMLS_DC)
{
	sxe_prop_dim_write(object, member, value, 1, 0, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ sxe_dimension_write()
 */
static void sxe_dimension_write(zval *object, zval *offset, zval *value TSRMLS_DC)
{
	sxe_prop_dim_write(object, offset, value, 0, 1, NULL TSRMLS_CC);
}
/* }}} */

static zval** sxe_property_get_adr(zval *object, zval *member, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	php_sxe_object *sxe;
	xmlNodePtr      node;
	zval           *return_value;
	char           *name;
	SXE_ITER        type;

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	GET_NODE(sxe, node);
	convert_to_string(member);
	name = Z_STRVAL_P(member);
	node = sxe_get_element_by_name(sxe, node, &name, &type TSRMLS_CC);
	if (node) {
		return NULL;
	}
	if (sxe_prop_dim_write(object, member, NULL, 1, 0, &node TSRMLS_CC) != SUCCESS) {
		return NULL;
	}
	type = SXE_ITER_NONE;
	name = NULL;

	MAKE_STD_ZVAL(return_value);
	_node_as_zval(sxe, node, return_value, type, name, sxe->iter.nsprefix, sxe->iter.isprefix TSRMLS_CC);

	sxe = php_sxe_fetch_object(return_value TSRMLS_CC);
	if (sxe->tmp) {
		zval_ptr_dtor(&sxe->tmp);
	}
	sxe->tmp = return_value;
	Z_SET_ISREF_P(return_value);

	return &sxe->tmp;
}
/* }}} */

/* {{{ sxe_prop_dim_exists()
 */
static int sxe_prop_dim_exists(zval *object, zval *member, int check_empty, zend_bool elements, zend_bool attribs TSRMLS_DC)
{
	php_sxe_object *sxe;
	xmlNodePtr      node;
	xmlAttrPtr      attr = NULL;
	int				exists = 0;
	int             test = 0;
	zval            tmp_zv;

	if (Z_TYPE_P(member) != IS_STRING && Z_TYPE_P(member) != IS_LONG) {
		tmp_zv = *member;
		zval_copy_ctor(&tmp_zv);
		member = &tmp_zv;
		convert_to_string(member);
	}

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	GET_NODE(sxe, node);

	if (Z_TYPE_P(member) == IS_LONG) {
		if (sxe->iter.type != SXE_ITER_ATTRLIST) {
			attribs = 0;
			elements = 1;
			if (sxe->iter.type == SXE_ITER_CHILD) {
				node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
			}
		}
	}

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		attribs = 1;
		elements = 0;
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
		attr = (xmlAttrPtr)node;
		test = sxe->iter.name != NULL;
	} else if (sxe->iter.type != SXE_ITER_CHILD) {
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
		attr = node ? node->properties : NULL;
		test = 0;
	}

	if (node) {
		if (attribs) {
			if (Z_TYPE_P(member) == IS_LONG) {
				int	nodendx = 0;

				while (attr && nodendx <= Z_LVAL_P(member)) {
					if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
						if (nodendx == Z_LVAL_P(member)) {
							exists = 1;
							break;
						}
						nodendx++;
					}
					attr = attr->next;
				}
			} else {
				while (attr) {
					if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && !xmlStrcmp(attr->name, (xmlChar *)Z_STRVAL_P(member)) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
						exists = 1;
						break;
					}

					attr = attr->next;
				}
			}
			if (exists && check_empty == 1 &&
				(!attr->children || !attr->children->content || !attr->children->content[0] || !xmlStrcmp(attr->children->content, "0")) ) {
				/* Attribute with no content in it's text node */
				exists = 0;
			}
		}

		if (elements) {
			if (Z_TYPE_P(member) == IS_LONG) {
				if (sxe->iter.type == SXE_ITER_CHILD) {
					node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
				}
				node = sxe_get_element_by_offset(sxe, Z_LVAL_P(member), node, NULL);
			}
			else {
				node = node->children;
				while (node) {
					xmlNodePtr nnext;
					nnext = node->next;
					if ((node->type == XML_ELEMENT_NODE) && !xmlStrcmp(node->name, (xmlChar *)Z_STRVAL_P(member))) {
						break;
					}
					node = nnext;
				}
			}
			if (node) {
				exists = 1;
                                if (check_empty == 1 &&
					(!node->children || (node->children->type == XML_TEXT_NODE && !node->children->next &&
						(!node->children->content || !node->children->content[0] || !xmlStrcmp(node->children->content, "0")))) ) {
					exists = 0;
				}
			}
		}
	}

	if (member == &tmp_zv) {
		zval_dtor(&tmp_zv);
	}

	return exists;
}
/* }}} */

/* {{{ sxe_property_exists()
 */
static int sxe_property_exists(zval *object, zval *member, int check_empty, const zend_literal *key TSRMLS_DC)
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
	xmlAttrPtr      attr = NULL;
	xmlAttrPtr      anext;
	zval            tmp_zv;
	int             test = 0;

	if (Z_TYPE_P(member) != IS_STRING && Z_TYPE_P(member) != IS_LONG) {
		tmp_zv = *member;
		zval_copy_ctor(&tmp_zv);
		member = &tmp_zv;
		convert_to_string(member);
	}

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	GET_NODE(sxe, node);

	if (Z_TYPE_P(member) == IS_LONG) {
		if (sxe->iter.type != SXE_ITER_ATTRLIST) {
			attribs = 0;
			elements = 1;
			if (sxe->iter.type == SXE_ITER_CHILD) {
				node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
			}
		}
	}

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		attribs = 1;
		elements = 0;
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
		attr = (xmlAttrPtr)node;
		test = sxe->iter.name != NULL;
	} else if (sxe->iter.type != SXE_ITER_CHILD) {
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
		attr = node ? node->properties : NULL;
		test = 0;
	}

	if (node) {
		if (attribs) {
			if (Z_TYPE_P(member) == IS_LONG) {
				int	nodendx = 0;

				while (attr && nodendx <= Z_LVAL_P(member)) {
					if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
						if (nodendx == Z_LVAL_P(member)) {
							xmlUnlinkNode((xmlNodePtr) attr);
							php_libxml_node_free_resource((xmlNodePtr) attr TSRMLS_CC);
							break;
						}
						nodendx++;
					}
					attr = attr->next;
				}
			} else {
				while (attr) {
					anext = attr->next;
					if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && !xmlStrcmp(attr->name, (xmlChar *)Z_STRVAL_P(member)) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
						xmlUnlinkNode((xmlNodePtr) attr);
						php_libxml_node_free_resource((xmlNodePtr) attr TSRMLS_CC);
						break;
					}
					attr = anext;
				}
			}
		}

		if (elements) {
			if (Z_TYPE_P(member) == IS_LONG) {
				if (sxe->iter.type == SXE_ITER_CHILD) {
					node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
				}
				node = sxe_get_element_by_offset(sxe, Z_LVAL_P(member), node, NULL);
				if (node) {
					xmlUnlinkNode(node);
					php_libxml_node_free_resource(node TSRMLS_CC);
				}
			} else {
				node = node->children;
				while (node) {
					nnext = node->next;

					SKIP_TEXT(node);

					if (!xmlStrcmp(node->name, (xmlChar *)Z_STRVAL_P(member))) {
						xmlUnlinkNode(node);
						php_libxml_node_free_resource(node TSRMLS_CC);
					}

next_iter:
					node = nnext;
				}
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
static void sxe_property_delete(zval *object, zval *member, const zend_literal *key TSRMLS_DC)
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

static inline char * sxe_xmlNodeListGetString(xmlDocPtr doc, xmlNodePtr list, int inLine) /* {{{ */
{
	xmlChar *tmp = xmlNodeListGetString(doc, list, inLine);
	char    *res;
	
	if (tmp) {
		res = estrdup((char*)tmp);
		xmlFree(tmp);
	} else {
		res = STR_EMPTY_ALLOC();
	}

	return res;
}
/* }}} */

/* {{{ _get_base_node_value()
 */
static void _get_base_node_value(php_sxe_object *sxe_ref, xmlNodePtr node, zval **value, xmlChar *nsprefix, int isprefix TSRMLS_DC)
{
	php_sxe_object *subnode;
	xmlChar        *contents;

	MAKE_STD_ZVAL(*value);

	if (node->children && node->children->type == XML_TEXT_NODE && !xmlIsBlankNode(node->children)) {
		contents = xmlNodeListGetString(node->doc, node->children, 1);
		if (contents) {
			ZVAL_STRING(*value, (char *)contents, 1);
			xmlFree(contents);
		}
	} else {
		subnode = php_sxe_object_new(sxe_ref->zo.ce TSRMLS_CC);
		subnode->document = sxe_ref->document;
		subnode->document->refcount++;
		if (nsprefix && *nsprefix) {
			subnode->iter.nsprefix = xmlStrdup((xmlChar *)nsprefix);
			subnode->iter.isprefix = isprefix;
		}
		php_libxml_increment_node_ptr((php_libxml_node_object *)subnode, node, NULL TSRMLS_CC);

		(*value)->type = IS_OBJECT;
		(*value)->value.obj = php_sxe_register_object(subnode TSRMLS_CC);
		/*zval_add_ref(value);*/
	}
}
/* }}} */

static void sxe_properties_add(HashTable *rv, char *name, int namelen, zval *value TSRMLS_DC) /* {{{ */
{
	zval  **data_ptr;
	zval  *newptr;
	ulong h = zend_hash_func(name, namelen);

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
}
/* }}} */

static HashTable * sxe_get_prop_hash(zval *object, int is_debug TSRMLS_DC) /* {{{ */
{
	zval            *value;
	zval            *zattr;
	HashTable       *rv;
	php_sxe_object  *sxe;
	char            *name;
	xmlNodePtr       node;
	xmlAttrPtr       attr;
	int              namelen;
	int              test;
	char 		 use_iter;
	zval            *iter_data;

	use_iter = 0;

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	if (is_debug) {
		ALLOC_HASHTABLE(rv);
		zend_hash_init(rv, 0, NULL, ZVAL_PTR_DTOR, 0);
	}
	else if (sxe->properties) {
		if (GC_G(gc_active)) {
			return sxe->properties;
		}
		zend_hash_clean(sxe->properties);
		rv = sxe->properties;
	} else {
		if (GC_G(gc_active)) {
			return NULL;
		}
		ALLOC_HASHTABLE(rv);
		zend_hash_init(rv, 0, NULL, ZVAL_PTR_DTOR, 0);
		sxe->properties = rv;
	}

	GET_NODE(sxe, node);
	if (!node) {
		return rv;
	}
	if (is_debug || sxe->iter.type != SXE_ITER_CHILD) {
		if (sxe->iter.type == SXE_ITER_ELEMENT) {
			node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
		}
		if (!node || node->type != XML_ENTITY_DECL) {
			attr = node ? (xmlAttrPtr)node->properties : NULL;
			zattr = NULL;
			test = sxe->iter.name && sxe->iter.type == SXE_ITER_ATTRLIST;
			while (attr) {
				if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && match_ns(sxe, (xmlNodePtr)attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
					MAKE_STD_ZVAL(value);
					ZVAL_STRING(value, sxe_xmlNodeListGetString((xmlDocPtr) sxe->document->ptr, attr->children, 1), 0);
					namelen = xmlStrlen(attr->name) + 1;
					if (!zattr) {
						MAKE_STD_ZVAL(zattr);
						array_init(zattr);
						sxe_properties_add(rv, "@attributes", sizeof("@attributes"), zattr TSRMLS_CC);
					}
					add_assoc_zval_ex(zattr, (char*)attr->name, namelen, value);
				}
				attr = attr->next;
			}
		}
	}

	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	if (node && sxe->iter.type != SXE_ITER_ATTRLIST) {
		if (node->type == XML_ATTRIBUTE_NODE) {
			MAKE_STD_ZVAL(value);
			ZVAL_STRING(value, sxe_xmlNodeListGetString(node->doc, node->children, 1), 0);
			zend_hash_next_index_insert(rv, &value, sizeof(zval *), NULL);
			node = NULL;
		} else if (sxe->iter.type != SXE_ITER_CHILD) {

			if ( !node->children || !node->parent || node->children->next || node->children->children || node->parent->children == node->parent->last ) {
				node = node->children;
			} else {
				iter_data = sxe->iter.data;
				sxe->iter.data = NULL;

				node = php_sxe_reset_iterator(sxe, 0 TSRMLS_CC);

				use_iter = 1;
			}
		}

		while (node) {
			if (node->children != NULL || node->prev != NULL || node->next != NULL) {
				SKIP_TEXT(node);
			} else {
				if (node->type == XML_TEXT_NODE) {
					const xmlChar *cur = node->content;
					
					if (*cur != 0) {
						MAKE_STD_ZVAL(value);
						ZVAL_STRING(value, sxe_xmlNodeListGetString(node->doc, node, 1), 0);
						zend_hash_next_index_insert(rv, &value, sizeof(zval *), NULL);
					}
					goto next_iter;
				}
			}

			if (node->type == XML_ELEMENT_NODE && (! match_ns(sxe, node, sxe->iter.nsprefix, sxe->iter.isprefix))) {
				goto next_iter;
			}

			name = (char *) node->name;
			if (!name) {
				goto next_iter;
			} else {
				namelen = xmlStrlen(node->name) + 1;
			}

			_get_base_node_value(sxe, node, &value, sxe->iter.nsprefix, sxe->iter.isprefix TSRMLS_CC);

			if ( use_iter ) {
				zend_hash_next_index_insert(rv, &value, sizeof(zval *), NULL);
			} else {
				sxe_properties_add(rv, name, namelen, value TSRMLS_CC);
			}
next_iter:
			if ( use_iter ) {
				node = php_sxe_iterator_fetch(sxe, node->next, 0 TSRMLS_CC);
			} else {
				node = node->next;
			}
		}
	}

	if ( use_iter ) {
		if (sxe->iter.data) {
			zval_ptr_dtor(&sxe->iter.data);
		}
		sxe->iter.data = iter_data;
	}

	return rv;
}
/* }}} */

static HashTable * sxe_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	return sxe_get_prop_hash(object, 0 TSRMLS_CC);
}
/* }}} */

static HashTable * sxe_get_debug_info(zval *object, int *is_temp TSRMLS_DC) /* {{{ */
{
	*is_temp = 1;
	return sxe_get_prop_hash(object, 1 TSRMLS_CC);
}
/* }}} */

static int sxe_objects_compare(zval *object1, zval *object2 TSRMLS_DC) /* {{{ */
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

/* {{{ proto array SimpleXMLElement::xpath(string path)
   Runs XPath query on the XML data */
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

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		return; /* attributes don't have attributes */
	}

	if (!sxe->xpath) {
		sxe->xpath = xmlXPathNewContext((xmlDocPtr) sxe->document->ptr);
	}
	if (!sxe->node) {
		php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement((xmlDocPtr) sxe->document->ptr), NULL TSRMLS_CC);
	}

	nodeptr = php_sxe_get_first_node(sxe, sxe->node->node TSRMLS_CC);

	sxe->xpath->node = nodeptr;

 	ns = xmlGetNsList((xmlDocPtr) sxe->document->ptr, nodeptr);
	if (ns != NULL) {
		while (ns[nsnbr] != NULL) {
			nsnbr++;
		}
	}

	sxe->xpath->namespaces = ns;
	sxe->xpath->nsNr = nsnbr;

	retval = xmlXPathEval((xmlChar *)query, sxe->xpath);
	if (ns != NULL) {
		xmlFree(ns);
		sxe->xpath->namespaces = NULL;
		sxe->xpath->nsNr = 0;
	}

	if (!retval) {
		RETURN_FALSE;
	}

	result = retval->nodesetval;

	array_init(return_value);
		
	if (result != NULL) {
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
					_node_as_zval(sxe, nodeptr->parent, value, SXE_ITER_NONE, NULL, NULL, 0 TSRMLS_CC);
				} else if (nodeptr->type == XML_ATTRIBUTE_NODE) {
					_node_as_zval(sxe, nodeptr->parent, value, SXE_ITER_ATTRLIST, (char*)nodeptr->name, nodeptr->ns ? (xmlChar *)nodeptr->ns->href : NULL, 0 TSRMLS_CC);
				} else {
					_node_as_zval(sxe, nodeptr, value, SXE_ITER_NONE, NULL, NULL, 0 TSRMLS_CC);
				}

				add_next_index_zval(return_value, value);
			}
		}
	}

	xmlXPathFreeObject(retval);
}
/* }}} */

/* {{{ proto bool SimpleXMLElement::registerXPathNamespace(string prefix, string ns)
   Creates a prefix/ns context for the next XPath query */
SXE_METHOD(registerXPathNamespace)
{
	php_sxe_object    *sxe;
	int prefix_len, ns_uri_len;
	char *prefix, *ns_uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &prefix, &prefix_len, &ns_uri, &ns_uri_len) == FAILURE) {
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	if (!sxe->xpath) {
		sxe->xpath = xmlXPathNewContext((xmlDocPtr) sxe->document->ptr);
	}

	if (xmlXPathRegisterNs(sxe->xpath, (xmlChar *)prefix, (xmlChar *)ns_uri) != 0) {
		RETURN_FALSE
	}
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto string SimpleXMLElement::asXML([string filename])
   Return a well-formed XML string based on SimpleXML element */
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
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &filename, &filename_len) == FAILURE) {
			RETURN_FALSE;
		}

		sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
		GET_NODE(sxe, node);
		node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

		if (node) {
			if (node->parent && (XML_DOCUMENT_NODE == node->parent->type)) {
				int bytes;
				bytes = xmlSaveFile(filename, (xmlDocPtr) sxe->document->ptr);
				if (bytes == -1) {
					RETURN_FALSE;
				} else {
					RETURN_TRUE;
				}
			} else {
				outbuf = xmlOutputBufferCreateFilename(filename, NULL, 0);

				if (outbuf == NULL) {
					RETURN_FALSE;
				}

				xmlNodeDumpOutput(outbuf, (xmlDocPtr) sxe->document->ptr, node, 0, 0, NULL);
				xmlOutputBufferClose(outbuf);
				RETURN_TRUE;
			}
		} else {
			RETURN_FALSE;
		}
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	if (node) {
		if (node->parent && (XML_DOCUMENT_NODE == node->parent->type)) {
			xmlDocDumpMemoryEnc((xmlDocPtr) sxe->document->ptr, &strval, &strval_len, ((xmlDocPtr) sxe->document->ptr)->encoding);
			RETVAL_STRINGL((char *)strval, strval_len, 1);
			xmlFree(strval);
		} else {
			/* Should we be passing encoding information instead of NULL? */
			outbuf = xmlAllocOutputBuffer(NULL);

			if (outbuf == NULL) {
				RETURN_FALSE;
			}

			xmlNodeDumpOutput(outbuf, (xmlDocPtr) sxe->document->ptr, node, 0, 0, ((xmlDocPtr) sxe->document->ptr)->encoding);
			xmlOutputBufferFlush(outbuf);
#ifdef LIBXML2_NEW_BUFFER
			RETVAL_STRINGL((char *)xmlOutputBufferGetContent(outbuf), xmlOutputBufferGetSize(outbuf), 1);
#else
			RETVAL_STRINGL((char *)outbuf->buffer->content, outbuf->buffer->use, 1);
#endif
			xmlOutputBufferClose(outbuf);
		}
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

#define SXE_NS_PREFIX(ns) (ns->prefix ? (char*)ns->prefix : "")

static inline void sxe_add_namespace_name(zval *return_value, xmlNsPtr ns) /* {{{ */
{
	char *prefix = SXE_NS_PREFIX(ns);
	if (zend_hash_exists(Z_ARRVAL_P(return_value), prefix, strlen(prefix) + 1) == 0) {
		add_assoc_string(return_value, prefix, (char*)ns->href, 1);
	}
}
/* }}} */

static void sxe_add_namespaces(php_sxe_object *sxe, xmlNodePtr node, zend_bool recursive, zval *return_value TSRMLS_DC) /* {{{ */
{
	xmlAttrPtr  attr;

	if (node->ns) {
		sxe_add_namespace_name(return_value, node->ns);
	}

	attr = node->properties;
	while (attr) {
		if (attr->ns) {
			sxe_add_namespace_name(return_value, attr->ns);
		}
		attr = attr->next;
	}

	if (recursive) {
		node = node->children;
		while (node) {
			if (node->type == XML_ELEMENT_NODE) {
				sxe_add_namespaces(sxe, node, recursive, return_value TSRMLS_CC);
			}
			node = node->next;
		}
	}
} /* }}} */

/* {{{ proto string SimpleXMLElement::getNamespaces([bool recursve])
   Return all namespaces in use */
SXE_METHOD(getNamespaces)
{
	zend_bool           recursive = 0;
	php_sxe_object     *sxe;
	xmlNodePtr          node;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &recursive) == FAILURE) {
		return;
	}

	array_init(return_value);

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	if (node) {
		if (node->type == XML_ELEMENT_NODE) {
			sxe_add_namespaces(sxe, node, recursive, return_value TSRMLS_CC);
		} else if (node->type == XML_ATTRIBUTE_NODE && node->ns) {
			sxe_add_namespace_name(return_value, node->ns);
		}
	}
}
/* }}} */

static void sxe_add_registered_namespaces(php_sxe_object *sxe, xmlNodePtr node, zend_bool recursive, zval *return_value TSRMLS_DC) /* {{{ */
{
	xmlNsPtr ns;

	if (node->type == XML_ELEMENT_NODE) {
		ns = node->nsDef;
		while (ns != NULL) {
			sxe_add_namespace_name(return_value, ns);
			ns = ns->next;
		}
		if (recursive) {
			node = node->children;
			while (node) {
				sxe_add_registered_namespaces(sxe, node, recursive, return_value TSRMLS_CC);
				node = node->next;
			}
		}
	}
}
/* }}} */

/* {{{ proto string SimpleXMLElement::getDocNamespaces([bool recursive [, bool from_root])
   Return all namespaces registered with document */
SXE_METHOD(getDocNamespaces)
{
	zend_bool           recursive = 0, from_root = 1;
	php_sxe_object     *sxe;
	xmlNodePtr          node;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|bb", &recursive, &from_root) == FAILURE) {
		return;
	}

	array_init(return_value);

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	if(from_root){
		node = xmlDocGetRootElement((xmlDocPtr)sxe->document->ptr);
	}else{
		GET_NODE(sxe, node);
	}

	sxe_add_registered_namespaces(sxe, node, recursive, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto object SimpleXMLElement::children([string ns [, bool is_prefix]])
   Finds children of given node */
SXE_METHOD(children)
{
	php_sxe_object *sxe;
	char           *nsprefix = NULL;
	int             nsprefix_len = 0;
	xmlNodePtr      node;
	zend_bool       isprefix = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!b", &nsprefix, &nsprefix_len, &isprefix) == FAILURE) {
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		return; /* attributes don't have attributes */
	}

	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	_node_as_zval(sxe, node, return_value, SXE_ITER_CHILD, NULL, (xmlChar *)nsprefix, isprefix TSRMLS_CC);

}
/* }}} */

/* {{{ proto object SimpleXMLElement::getName()
   Finds children of given node */
SXE_METHOD(getName)
{
	php_sxe_object *sxe;
	xmlNodePtr      node;
	int             namelen;

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);

	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);
	if (node) {
		namelen = xmlStrlen(node->name);
		RETURN_STRINGL((char*)node->name, namelen, 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto array SimpleXMLElement::attributes([string ns [, bool is_prefix]])
   Identifies an element's attributes */
SXE_METHOD(attributes)
{
	php_sxe_object *sxe;
	char           *nsprefix = NULL;
	int             nsprefix_len = 0;
	xmlNodePtr      node;
	zend_bool       isprefix = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!b", &nsprefix, &nsprefix_len, &isprefix) == FAILURE) {
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	GET_NODE(sxe, node);

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		return; /* attributes don't have attributes */
	}

	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	_node_as_zval(sxe, node, return_value, SXE_ITER_ATTRLIST, NULL, (xmlChar *)nsprefix, isprefix TSRMLS_CC);
}
/* }}} */

/* {{{ proto void SimpleXMLElement::addChild(string qName [, string value [, string ns]])
   Add Element with optional namespace information */
SXE_METHOD(addChild)
{
	php_sxe_object *sxe;
	char           *qname, *value = NULL, *nsuri = NULL;
	int             qname_len, value_len = 0, nsuri_len = 0;
	xmlNodePtr      node, newnode;
	xmlNsPtr        nsptr = NULL;
	xmlChar        *localname, *prefix = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!s!",
		&qname, &qname_len, &value, &value_len, &nsuri, &nsuri_len) == FAILURE) {
		return;
	}

	if (qname_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Element name is required");
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	GET_NODE(sxe, node);

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot add element to attributes");
		return;
	}

	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	if (node == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot add child. Parent is not a permanent member of the XML tree");
		return;
	}

	localname = xmlSplitQName2((xmlChar *)qname, &prefix);
	if (localname == NULL) {
		localname = xmlStrdup((xmlChar *)qname);
	}

	newnode = xmlNewChild(node, NULL, localname, (xmlChar *)value);

	if (nsuri != NULL) {
		if (nsuri_len == 0) {
			newnode->ns = NULL;
			nsptr = xmlNewNs(newnode, (xmlChar *)nsuri, prefix);
		} else {
			nsptr = xmlSearchNsByHref(node->doc, node, (xmlChar *)nsuri);
			if (nsptr == NULL) {
				nsptr = xmlNewNs(newnode, (xmlChar *)nsuri, prefix);
			}
			newnode->ns = nsptr;
		}
	}

	_node_as_zval(sxe, newnode, return_value, SXE_ITER_NONE, (char *)localname, prefix, 0 TSRMLS_CC);

	xmlFree(localname);
	if (prefix != NULL) {
		xmlFree(prefix);
	}
}
/* }}} */

/* {{{ proto void SimpleXMLElement::addAttribute(string qName, string value [,string ns])
   Add Attribute with optional namespace information */
SXE_METHOD(addAttribute)
{
	php_sxe_object *sxe;
	char           *qname, *value = NULL, *nsuri = NULL;
	int             qname_len, value_len = 0, nsuri_len = 0;
	xmlNodePtr      node;
	xmlAttrPtr      attrp = NULL;
	xmlNsPtr        nsptr = NULL;
	xmlChar        *localname, *prefix = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s!",
		&qname, &qname_len, &value, &value_len, &nsuri, &nsuri_len) == FAILURE) {
		return;
	}

	if (qname_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attribute name is required");
		return;
	}

	sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	GET_NODE(sxe, node);

	node = php_sxe_get_first_node(sxe, node TSRMLS_CC);

	if (node && node->type != XML_ELEMENT_NODE) {
		node = node->parent;
	}

	if (node == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to locate parent Element");
		return;
	}

	localname = xmlSplitQName2((xmlChar *)qname, &prefix);
	if (localname == NULL) {
		if (nsuri_len > 0) {
			if (prefix != NULL) {
				xmlFree(prefix);
			}
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attribute requires prefix for namespace");
			return;
		}
		localname = xmlStrdup((xmlChar *)qname);
	}

	attrp = xmlHasNsProp(node, localname, (xmlChar *)nsuri);
	if (attrp != NULL && attrp->type != XML_ATTRIBUTE_DECL) {
		xmlFree(localname);
		if (prefix != NULL) {
			xmlFree(prefix);
		}
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attribute already exists");
		return;
	}

	if (nsuri != NULL) {
		nsptr = xmlSearchNsByHref(node->doc, node, (xmlChar *)nsuri);
		if (nsptr == NULL) {
			nsptr = xmlNewNs(node, (xmlChar *)nsuri, prefix);
		}
	}

	attrp = xmlNewNsProp(node, nsptr, localname, (xmlChar *)value);

	xmlFree(localname);
	if (prefix != NULL) {
		xmlFree(prefix);
	}
}
/* }}} */

/* {{{ cast_object()
 */
static int cast_object(zval *object, int type, char *contents TSRMLS_DC)
{
	if (contents) {
		ZVAL_STRINGL(object, contents, strlen(contents), 1);
	} else {
		ZVAL_NULL(object);
	}
	Z_SET_REFCOUNT_P(object, 1);
	Z_UNSET_ISREF_P(object);

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
static int sxe_object_cast(zval *readobj, zval *writeobj, int type TSRMLS_DC)
{
	php_sxe_object *sxe;
	xmlChar           *contents = NULL;
	xmlNodePtr	    node;
	int rv;
	HashTable      *prop_hash;

	sxe = php_sxe_fetch_object(readobj TSRMLS_CC);

	if (type == IS_BOOL) {
		node = php_sxe_get_first_node(sxe, NULL TSRMLS_CC);
		prop_hash = sxe_get_prop_hash(readobj, 1 TSRMLS_CC);
		INIT_PZVAL(writeobj);
		ZVAL_BOOL(writeobj, node != NULL || zend_hash_num_elements(prop_hash) > 0);
		zend_hash_destroy(prop_hash);
		efree(prop_hash);
		return SUCCESS;
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

	if (readobj == writeobj) {
		INIT_PZVAL(writeobj);
		zval_dtor(readobj);
	}

	rv = cast_object(writeobj, type, (char *)contents TSRMLS_CC);

	if (contents) {
		xmlFree(contents);
	}
	return rv;
}
/* }}} */

/* {{{ proto object SimpleXMLElement::__toString() U
   Returns the string content */
SXE_METHOD(__toString)
{
	zval           *result;

	ALLOC_INIT_ZVAL(result);

	if (sxe_object_cast(getThis(), result, IS_STRING TSRMLS_CC) == SUCCESS) {
		RETURN_ZVAL(result, 1, 1);
	} else {
		zval_ptr_dtor(&result);
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

static int php_sxe_count_elements_helper(php_sxe_object *sxe, long *count TSRMLS_DC) /* {{{ */
{
	xmlNodePtr       node;
	zval            *data;

	*count = 0;

	data = sxe->iter.data;
	sxe->iter.data = NULL;

	node = php_sxe_reset_iterator(sxe, 0 TSRMLS_CC);

	while (node)
	{
		(*count)++;
		node = php_sxe_iterator_fetch(sxe, node->next, 0 TSRMLS_CC);
	}

	if (sxe->iter.data) {
		zval_ptr_dtor(&sxe->iter.data);
	}
	sxe->iter.data = data;

	return SUCCESS;
}
/* }}} */

static int sxe_count_elements(zval *object, long *count TSRMLS_DC) /* {{{ */
{
	php_sxe_object  *intern;
	intern = php_sxe_fetch_object(object TSRMLS_CC);
	if (intern->fptr_count) {
		zval *rv;
		zend_call_method_with_0_params(&object, intern->zo.ce, &intern->fptr_count, "count", &rv);
		if (rv) {
			if (intern->tmp) {
				zval_ptr_dtor(&intern->tmp);
			}
			MAKE_STD_ZVAL(intern->tmp);
			ZVAL_ZVAL(intern->tmp, rv, 1, 1);
			convert_to_long(intern->tmp);
			*count = (long) Z_LVAL_P(intern->tmp);
			return SUCCESS;
		}
		return FAILURE;
	}
	return php_sxe_count_elements_helper(intern, count TSRMLS_CC);
}
/* }}} */

/* {{{ proto int SimpleXMLElement::count()
 Get number of child elements */
SXE_METHOD(count)
{
	long count = 0;
	php_sxe_object *sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	php_sxe_count_elements_helper(sxe, &count TSRMLS_CC);
	
	RETURN_LONG(count);
}
/* }}} */

static zval *sxe_get_value(zval *z TSRMLS_DC) /* {{{ */
{
	zval *retval;

	MAKE_STD_ZVAL(retval);

	if (sxe_object_cast(z, retval, IS_STRING TSRMLS_CC)==FAILURE) {
		zend_error(E_ERROR, "Unable to cast node to string");
		/* FIXME: Should not be fatal */
	}

	Z_SET_REFCOUNT_P(retval, 0);
	return retval;
}
/* }}} */

static zend_object_handlers sxe_object_handlers = { /* {{{ */
	ZEND_OBJECTS_STORE_HANDLERS,
	sxe_property_read,
	sxe_property_write,
	sxe_dimension_read,
	sxe_dimension_write,
	sxe_property_get_adr,
	sxe_get_value,			/* get */
	NULL,
	sxe_property_exists,
	sxe_property_delete,
	sxe_dimension_exists,
	sxe_dimension_delete,
	sxe_get_properties,
	NULL, /* zend_get_std_object_handlers()->get_method,*/
	NULL, /* zend_get_std_object_handlers()->call_method,*/
	NULL, /* zend_get_std_object_handlers()->get_constructor, */
	NULL, /* zend_get_std_object_handlers()->get_class_entry,*/
	NULL, /* zend_get_std_object_handlers()->get_class_name,*/
	sxe_objects_compare,
	sxe_object_cast,
	sxe_count_elements,
	sxe_get_debug_info
};
/* }}} */

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

	clone->iter.isprefix = sxe->iter.isprefix;
	if (sxe->iter.name != NULL) {
		clone->iter.name = xmlStrdup((xmlChar *)sxe->iter.name);
	}
	if (sxe->iter.nsprefix != NULL) {
		clone->iter.nsprefix = xmlStrdup((xmlChar *)sxe->iter.nsprefix);
	}
	clone->iter.type = sxe->iter.type;

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
	/* dtor required to cleanup iterator related data properly */

	php_sxe_object *sxe;

	sxe = (php_sxe_object *) object;

	if (sxe->iter.data) {
		zval_ptr_dtor(&sxe->iter.data);
		sxe->iter.data = NULL;
	}

	if (sxe->iter.name) {
		xmlFree(sxe->iter.name);
		sxe->iter.name = NULL;
	}
	if (sxe->iter.nsprefix) {
		xmlFree(sxe->iter.nsprefix);
		sxe->iter.nsprefix = NULL;
	}
	if (sxe->tmp) {
		zval_ptr_dtor(&sxe->tmp);
		sxe->tmp = NULL;
	}
}
/* }}} */

/* {{{ sxe_object_free_storage()
 */
static void sxe_object_free_storage(void *object TSRMLS_DC)
{
	php_sxe_object *sxe;

	sxe = (php_sxe_object *) object;

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 1 && PHP_RELEASE_VERSION > 2) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 1) || (PHP_MAJOR_VERSION > 5)
	zend_object_std_dtor(&sxe->zo TSRMLS_CC);
#else
	if (sxe->zo.guards) {
		zend_hash_destroy(sxe->zo.guards);
		FREE_HASHTABLE(sxe->zo.guards);
	}

	if (sxe->zo.properties) {
		zend_hash_destroy(sxe->zo.properties);
		FREE_HASHTABLE(sxe->zo.properties);
	}
#endif

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
	zend_class_entry     *parent = ce;
	int inherited = 0;

	intern = ecalloc(1, sizeof(php_sxe_object));

	intern->iter.type = SXE_ITER_NONE;
	intern->iter.nsprefix = NULL;
	intern->iter.name = NULL;
	intern->fptr_count = NULL;

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 1 && PHP_RELEASE_VERSION > 2) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 1) || (PHP_MAJOR_VERSION > 5)
	zend_object_std_init(&intern->zo, ce TSRMLS_CC);
#else
	ALLOC_HASHTABLE(intern->zo.properties);
	zend_hash_init(intern->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	intern->zo.ce = ce;
	intern->zo.guards = NULL;
#endif

	while (parent) {
		if (parent == sxe_class_entry) {
			break;
		}

		parent = parent->parent;
		inherited = 1;
	}

	if (inherited) {
		zend_hash_find(&ce->function_table, "count", sizeof("count"),(void **) &intern->fptr_count);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}

	return intern;
}
/* }}} */

/* {{{ php_sxe_register_object
 */
static zend_object_value
php_sxe_register_object(php_sxe_object *intern TSRMLS_DC)
{
	zend_object_value rv;

	rv.handle = zend_objects_store_put(intern, sxe_object_dtor, (zend_objects_free_object_storage_t)sxe_object_free_storage, sxe_object_clone TSRMLS_CC);
	rv.handlers = (zend_object_handlers *) &sxe_object_handlers;

	return rv;
}
/* }}} */

/* {{{ sxe_object_new()
 */
PHP_SXE_API zend_object_value
sxe_object_new(zend_class_entry *ce TSRMLS_DC)
{
	php_sxe_object    *intern;

	intern = php_sxe_object_new(ce TSRMLS_CC);
	return php_sxe_register_object(intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto simplemxml_element simplexml_load_file(string filename [, string class_name [, int options [, string ns [, bool is_prefix]]]])
   Load a filename and return a simplexml_element object to allow for processing */
PHP_FUNCTION(simplexml_load_file)
{
	php_sxe_object *sxe;
	char           *filename;
	int             filename_len;
	xmlDocPtr       docp;
	char           *ns = NULL;
	int             ns_len = 0;
	long            options = 0;
	zend_class_entry *ce= sxe_class_entry;
	zend_bool       isprefix = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|C!lsb", &filename, &filename_len, &ce, &options, &ns, &ns_len, &isprefix) == FAILURE) {
		return;
	}

	docp = xmlReadFile(filename, NULL, options);

	if (! docp) {
		RETURN_FALSE;
	}

	if (!ce) {
		ce = sxe_class_entry;
	}
	sxe = php_sxe_object_new(ce TSRMLS_CC);
	sxe->iter.nsprefix = ns_len ? xmlStrdup((xmlChar *)ns) : NULL;
	sxe->iter.isprefix = isprefix;
	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp TSRMLS_CC);
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL TSRMLS_CC);

	return_value->type = IS_OBJECT;
	return_value->value.obj = php_sxe_register_object(sxe TSRMLS_CC);
}
/* }}} */

/* {{{ proto simplemxml_element simplexml_load_string(string data [, string class_name [, int options [, string ns [, bool is_prefix]]]])
   Load a string and return a simplexml_element object to allow for processing */
PHP_FUNCTION(simplexml_load_string)
{
	php_sxe_object *sxe;
	char           *data;
	int             data_len;
	xmlDocPtr       docp;
	char           *ns = NULL;
	int             ns_len = 0;
	long            options = 0;
	zend_class_entry *ce= sxe_class_entry;
	zend_bool       isprefix = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|C!lsb", &data, &data_len, &ce, &options, &ns, &ns_len, &isprefix) == FAILURE) {
		return;
	}

	docp = xmlReadMemory(data, data_len, NULL, NULL, options);

	if (! docp) {
		RETURN_FALSE;
	}

	if (!ce) {
		ce = sxe_class_entry;
	}
	sxe = php_sxe_object_new(ce TSRMLS_CC);
	sxe->iter.nsprefix = ns_len ? xmlStrdup((xmlChar *)ns) : NULL;
	sxe->iter.isprefix = isprefix;
	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp TSRMLS_CC);
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL TSRMLS_CC);

	return_value->type = IS_OBJECT;
	return_value->value.obj = php_sxe_register_object(sxe TSRMLS_CC);
}
/* }}} */

/* {{{ proto SimpleXMLElement::__construct(string data [, int options [, bool data_is_url [, string ns [, bool is_prefix]]]])
   SimpleXMLElement constructor */
SXE_METHOD(__construct)
{
	php_sxe_object *sxe = php_sxe_fetch_object(getThis() TSRMLS_CC);
	char           *data, *ns = NULL;
	int             data_len, ns_len = 0;
	xmlDocPtr       docp;
	long            options = 0;
	zend_bool       is_url = 0, isprefix = 0;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lbsb", &data, &data_len, &options, &is_url, &ns, &ns_len, &isprefix) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);

	docp = is_url ? xmlReadFile(data, NULL, options) : xmlReadMemory(data, data_len, NULL, NULL, options);

	if (!docp) {
		((php_libxml_node_object *)sxe)->document = NULL;
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "String could not be parsed as XML", 0 TSRMLS_CC);
		return;
	}

	sxe->iter.nsprefix = ns_len ? xmlStrdup((xmlChar *)ns) : NULL;
	sxe->iter.isprefix = isprefix;
	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp TSRMLS_CC);
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL TSRMLS_CC);
}
/* }}} */

zend_object_iterator_funcs php_sxe_iterator_funcs = { /* {{{ */
	php_sxe_iterator_dtor,
	php_sxe_iterator_valid,
	php_sxe_iterator_current_data,
	php_sxe_iterator_current_key,
	php_sxe_iterator_move_forward,
	php_sxe_iterator_rewind,
};
/* }}} */

static xmlNodePtr php_sxe_iterator_fetch(php_sxe_object *sxe, xmlNodePtr node, int use_data TSRMLS_DC) /* {{{ */
{
	xmlChar *prefix  = sxe->iter.nsprefix;
	int isprefix  = sxe->iter.isprefix;
	int test_elem = sxe->iter.type == SXE_ITER_ELEMENT  && sxe->iter.name;
	int test_attr = sxe->iter.type == SXE_ITER_ATTRLIST && sxe->iter.name;

	while (node) {
		SKIP_TEXT(node);
		if (sxe->iter.type != SXE_ITER_ATTRLIST && node->type == XML_ELEMENT_NODE) {
			if ((!test_elem || !xmlStrcmp(node->name, sxe->iter.name)) && match_ns(sxe, node, prefix, isprefix)) {
				break;
			}
		} else if (node->type == XML_ATTRIBUTE_NODE) {
			if ((!test_attr || !xmlStrcmp(node->name, sxe->iter.name)) && match_ns(sxe, node, prefix, isprefix)) {
				break;
			}
		}
next_iter:
		node = node->next;
	}

	if (node && use_data) {
		ALLOC_INIT_ZVAL(sxe->iter.data);
		_node_as_zval(sxe, node, sxe->iter.data, SXE_ITER_NONE, NULL, prefix, isprefix TSRMLS_CC);
	}

	return node;
}
/* }}} */

static xmlNodePtr php_sxe_reset_iterator(php_sxe_object *sxe, int use_data TSRMLS_DC) /* {{{ */
{
	xmlNodePtr node;

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
			case SXE_ITER_ATTRLIST:
				node = (xmlNodePtr) node->properties;
		}
		return php_sxe_iterator_fetch(sxe, node, use_data TSRMLS_CC);
	}
	return NULL;
}
/* }}} */

zend_object_iterator *php_sxe_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	php_sxe_iterator *iterator;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}
	iterator = emalloc(sizeof(php_sxe_iterator));

	Z_ADDREF_P(object);
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = &php_sxe_iterator_funcs;
	iterator->sxe = php_sxe_fetch_object(object TSRMLS_CC);

	return (zend_object_iterator*)iterator;
}
/* }}} */

static void php_sxe_iterator_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	/* cleanup handled in sxe_object_dtor as we dont always have an iterator wrapper */
	if (iterator->intern.data) {
		zval_ptr_dtor((zval**)&iterator->intern.data);
	}

	efree(iterator);
}
/* }}} */

static int php_sxe_iterator_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	return iterator->sxe->iter.data ? SUCCESS : FAILURE;
}
/* }}} */

static void php_sxe_iterator_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	*data = &iterator->sxe->iter.data;
}
/* }}} */

static int php_sxe_iterator_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC) /* {{{ */
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
	if (!curnode) {
		return HASH_KEY_NON_EXISTANT;
	}

	namelen = xmlStrlen(curnode->name);
	*str_key = estrndup((char *)curnode->name, namelen);
	*str_key_len = namelen + 1;
	return HASH_KEY_IS_STRING;

}
/* }}} */

PHP_SXE_API void php_sxe_move_forward_iterator(php_sxe_object *sxe TSRMLS_DC) /* {{{ */
{
	xmlNodePtr      node = NULL;
	php_sxe_object  *intern;

	if (sxe->iter.data) {
		intern = (php_sxe_object *)zend_object_store_get_object(sxe->iter.data TSRMLS_CC);
		GET_NODE(intern, node)
		zval_ptr_dtor(&sxe->iter.data);
		sxe->iter.data = NULL;
	}

	if (node) {
		php_sxe_iterator_fetch(sxe, node->next, 1 TSRMLS_CC);
	}
}
/* }}} */

static void php_sxe_iterator_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	php_sxe_move_forward_iterator(iterator->sxe TSRMLS_CC);
}
/* }}} */

static void php_sxe_iterator_rewind(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	php_sxe_object	*sxe;

	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	sxe = iterator->sxe;

	php_sxe_reset_iterator(sxe, 1 TSRMLS_CC);
}
/* }}} */

void *simplexml_export_node(zval *object TSRMLS_DC) /* {{{ */
{
	php_sxe_object *sxe;
	xmlNodePtr node;

	sxe = php_sxe_fetch_object(object TSRMLS_CC);
	GET_NODE(sxe, node);
	return php_sxe_get_first_node(sxe, node TSRMLS_CC);
}
/* }}} */

/* {{{ proto simplemxml_element simplexml_import_dom(domNode node [, string class_name])
   Get a simplexml_element object from dom to allow for processing */
PHP_FUNCTION(simplexml_import_dom)
{
	php_sxe_object *sxe;
	zval *node;
	php_libxml_node_object *object;
	xmlNodePtr		nodep = NULL;
	zend_class_entry *ce= sxe_class_entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|C!", &node, &ce) == FAILURE) {
		return;
	}

	object = (php_libxml_node_object *)zend_object_store_get_object(node TSRMLS_CC);

	nodep = php_libxml_import_node(node TSRMLS_CC);

	if (nodep) {
		if (nodep->doc == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Imported Node must have associated Document");
			RETURN_NULL();
		}
		if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
			nodep = xmlDocGetRootElement((xmlDocPtr) nodep);
		}
	}

	if (nodep && nodep->type == XML_ELEMENT_NODE) {
		if (!ce) {
			ce = sxe_class_entry;
		}
		sxe = php_sxe_object_new(ce TSRMLS_CC);
		sxe->document = object->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, nodep->doc TSRMLS_CC);
		php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, nodep, NULL TSRMLS_CC);

		return_value->type = IS_OBJECT;
		return_value->value.obj = php_sxe_register_object(sxe TSRMLS_CC);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Nodetype to import");
		RETVAL_NULL();
	}
}
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexml_load_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, ns)
	ZEND_ARG_INFO(0, is_prefix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexml_load_string, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, ns)
	ZEND_ARG_INFO(0, is_prefix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexml_import_dom, 0, 0, 1)
	ZEND_ARG_INFO(0, node)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexmlelement_xpath, 0, 0, 1)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexmlelement_registerxpathnamespace, 0, 0, 2)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, ns)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexmlelement_asxml, 0, 0, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexmlelement_getnamespaces, 0, 0, 0)
	ZEND_ARG_INFO(0, recursve)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexmlelement_getdocnamespaces, 0, 0, 0)
	ZEND_ARG_INFO(0, recursve)
	ZEND_ARG_INFO(0, from_root)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexmlelement_children, 0, 0, 0)
	ZEND_ARG_INFO(0, ns)
	ZEND_ARG_INFO(0, is_prefix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexmlelement__construct, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, data_is_url)
	ZEND_ARG_INFO(0, ns)
	ZEND_ARG_INFO(0, is_prefix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_simplexmlelement__void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_simplexmlelement_addchild, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, ns)
ZEND_END_ARG_INFO()
/* }}} */

const zend_function_entry simplexml_functions[] = { /* {{{ */
	PHP_FE(simplexml_load_file, 	arginfo_simplexml_load_file)
	PHP_FE(simplexml_load_string,	arginfo_simplexml_load_string)
	PHP_FE(simplexml_import_dom,	arginfo_simplexml_import_dom)
	PHP_FE_END
};
/* }}} */

static const zend_module_dep simplexml_deps[] = { /* {{{ */
	ZEND_MOD_REQUIRED("libxml")
	ZEND_MOD_REQUIRED("spl")
	ZEND_MOD_END
};
/* }}} */

zend_module_entry simplexml_module_entry = { /* {{{ */
	STANDARD_MODULE_HEADER_EX, NULL,
	simplexml_deps,
	"SimpleXML",
	simplexml_functions,
	PHP_MINIT(simplexml),
	PHP_MSHUTDOWN(simplexml),
	NULL,
	NULL,
	PHP_MINFO(simplexml),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SIMPLEXML
ZEND_GET_MODULE(simplexml)
#endif

/* the method table */
/* each method can have its own parameters and visibility */
static const zend_function_entry sxe_functions[] = { /* {{{ */
	SXE_ME(__construct,            arginfo_simplexmlelement__construct, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL) /* must be called */
	SXE_ME(asXML,                  arginfo_simplexmlelement_asxml, ZEND_ACC_PUBLIC)
	SXE_MALIAS(saveXML, asXML,	   arginfo_simplexmlelement_asxml, ZEND_ACC_PUBLIC)
	SXE_ME(xpath,                  arginfo_simplexmlelement_xpath, ZEND_ACC_PUBLIC)
	SXE_ME(registerXPathNamespace, arginfo_simplexmlelement_registerxpathnamespace, ZEND_ACC_PUBLIC)
	SXE_ME(attributes,             arginfo_simplexmlelement_children, ZEND_ACC_PUBLIC)
	SXE_ME(children,               arginfo_simplexmlelement_children, ZEND_ACC_PUBLIC)
	SXE_ME(getNamespaces,          arginfo_simplexmlelement_getnamespaces, ZEND_ACC_PUBLIC)
	SXE_ME(getDocNamespaces,       arginfo_simplexmlelement_getdocnamespaces, ZEND_ACC_PUBLIC)
	SXE_ME(getName,                arginfo_simplexmlelement__void, ZEND_ACC_PUBLIC)
	SXE_ME(addChild,               arginfo_simplexmlelement_addchild, ZEND_ACC_PUBLIC)
	SXE_ME(addAttribute,           arginfo_simplexmlelement_addchild, ZEND_ACC_PUBLIC)
	SXE_ME(__toString,             arginfo_simplexmlelement__void, ZEND_ACC_PUBLIC)
	SXE_ME(count,                  arginfo_simplexmlelement__void, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(simplexml)
 */
PHP_MINIT_FUNCTION(simplexml)
{
	zend_class_entry sxe;

	INIT_CLASS_ENTRY(sxe, "SimpleXMLElement", sxe_functions);
	sxe.create_object = sxe_object_new;
	sxe_class_entry = zend_register_internal_class(&sxe TSRMLS_CC);
	sxe_class_entry->get_iterator = php_sxe_get_iterator;
	sxe_class_entry->iterator_funcs.funcs = &php_sxe_iterator_funcs;
	zend_class_implements(sxe_class_entry TSRMLS_CC, 1, zend_ce_traversable);
	sxe_object_handlers.get_method = zend_get_std_object_handlers()->get_method;
	sxe_object_handlers.get_constructor = zend_get_std_object_handlers()->get_constructor;
	sxe_object_handlers.get_class_entry = zend_get_std_object_handlers()->get_class_entry;
	sxe_object_handlers.get_class_name = zend_get_std_object_handlers()->get_class_name;
	sxe_class_entry->serialize = zend_class_serialize_deny;
	sxe_class_entry->unserialize = zend_class_unserialize_deny;

	php_libxml_register_export(sxe_class_entry, simplexml_export_node);

	PHP_MINIT(sxe)(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(simplexml)
 */
PHP_MSHUTDOWN_FUNCTION(simplexml)
{
	sxe_class_entry = NULL;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION(simplexml)
 */
PHP_MINFO_FUNCTION(simplexml)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Simplexml support", "enabled");
	php_info_print_table_row(2, "Revision", "$Id$");
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
