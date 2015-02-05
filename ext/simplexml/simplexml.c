/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
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

static php_sxe_object* php_sxe_object_new(zend_class_entry *ce);
static xmlNodePtr php_sxe_reset_iterator(php_sxe_object *sxe, int use_data);
static xmlNodePtr php_sxe_iterator_fetch(php_sxe_object *sxe, xmlNodePtr node, int use_data);
static zval *sxe_get_value(zval *z, zval *rv);
static void php_sxe_iterator_dtor(zend_object_iterator *iter);
static int php_sxe_iterator_valid(zend_object_iterator *iter);
static zval *php_sxe_iterator_current_data(zend_object_iterator *iter);
static void php_sxe_iterator_current_key(zend_object_iterator *iter, zval *key);
static void php_sxe_iterator_move_forward(zend_object_iterator *iter);
static void php_sxe_iterator_rewind(zend_object_iterator *iter);

/* {{{ _node_as_zval()
 */
static void _node_as_zval(php_sxe_object *sxe, xmlNodePtr node, zval *value, SXE_ITER itertype, char *name, const xmlChar *nsprefix, int isprefix)
{
	php_sxe_object *subnode;

	subnode = php_sxe_object_new(sxe->zo.ce);
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

	php_libxml_increment_node_ptr((php_libxml_node_object *)subnode, node, NULL);

	ZVAL_OBJ(value, &subnode->zo);
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
		php_error_docref(NULL, E_WARNING, "Node no longer exists"); \
	} \
}

static xmlNodePtr php_sxe_get_first_node(php_sxe_object *sxe, xmlNodePtr node) /* {{{ */
{
	php_sxe_object *intern;
	xmlNodePtr retnode = NULL;

	if (sxe && sxe->iter.type != SXE_ITER_NONE) {
		php_sxe_reset_iterator(sxe, 1);
		if (!Z_ISUNDEF(sxe->iter.data)) {
			intern = Z_SXEOBJ_P(&sxe->iter.data);
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

static xmlNodePtr sxe_get_element_by_offset(php_sxe_object *sxe, zend_long offset, xmlNodePtr node, zend_long *cnt) /* {{{ */
{
	zend_long nodendx = 0;

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

static xmlNodePtr sxe_find_element_by_name(php_sxe_object *sxe, xmlNodePtr node, xmlChar *name) /* {{{ */
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

static xmlNodePtr sxe_get_element_by_name(php_sxe_object *sxe, xmlNodePtr node, char **name, SXE_ITER *type) /* {{{ */
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
		node = php_sxe_get_first_node(sxe, node);
		sxe->iter.type = orgtype;
	}

	if (sxe->iter.type == SXE_ITER_ELEMENT) {
		orgnode = sxe_find_element_by_name(sxe, node, sxe->iter.name);
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
static zval *sxe_prop_dim_read(zval *object, zval *member, zend_bool elements, zend_bool attribs, int type, zval *rv)
{
	php_sxe_object *sxe;
	char           *name;
	xmlNodePtr      node;
	xmlAttrPtr      attr = NULL;
	zval            tmp_zv;
	int             nodendx = 0;
	int             test = 0;

	sxe = Z_SXEOBJ_P(object);

	if (!member || Z_TYPE_P(member) == IS_LONG) {
		if (sxe->iter.type != SXE_ITER_ATTRLIST) {
			attribs = 0;
			elements = 1;
		} else if (!member) {
			/* This happens when the user did: $sxe[]->foo = $value */
			php_error_docref(NULL, E_ERROR, "Cannot create unnamed attribute");
			return NULL;
		}
		name = NULL;
	} else {
		if (Z_TYPE_P(member) != IS_STRING) {
			ZVAL_STR(&tmp_zv, zval_get_string(member));
			member = &tmp_zv;
		}
		name = Z_STRVAL_P(member);
	}

	GET_NODE(sxe, node);

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		attribs = 1;
		elements = 0;
		node = php_sxe_get_first_node(sxe, node);
		attr = (xmlAttrPtr)node;
		test = sxe->iter.name != NULL;
	} else if (sxe->iter.type != SXE_ITER_CHILD) {
		node = php_sxe_get_first_node(sxe, node);
		attr = node ? node->properties : NULL;
		test = 0;
		if (!member && node && node->parent &&
		    node->parent->type == XML_DOCUMENT_NODE) {
			/* This happens when the user did: $sxe[]->foo = $value */
			php_error_docref(NULL, E_ERROR, "Cannot create unnamed attribute");
			return NULL;
		}
	}

	ZVAL_UNDEF(rv);

	if (node) {
		if (attribs) {
			if (Z_TYPE_P(member) != IS_LONG || sxe->iter.type == SXE_ITER_ATTRLIST) {
				if (Z_TYPE_P(member) == IS_LONG) {
					while (attr && nodendx <= Z_LVAL_P(member)) {
						if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
							if (nodendx == Z_LVAL_P(member)) {
								_node_as_zval(sxe, (xmlNodePtr) attr, rv, SXE_ITER_NONE, NULL, sxe->iter.nsprefix, sxe->iter.isprefix);
								break;
							}
							nodendx++;
						}
						attr = attr->next;
					}
				} else {
					while (attr) {
						if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && !xmlStrcmp(attr->name, (xmlChar *)name) && match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
							_node_as_zval(sxe, (xmlNodePtr) attr, rv, SXE_ITER_NONE, NULL, sxe->iter.nsprefix, sxe->iter.isprefix);
							break;
						}
						attr = attr->next;
					}
				}
			}
		}

		if (elements) {
			if (!sxe->node) {
				php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, node, NULL);
			}
			if (!member || Z_TYPE_P(member) == IS_LONG) {
				zend_long cnt = 0;
				xmlNodePtr mynode = node;

				if (sxe->iter.type == SXE_ITER_CHILD) {
					node = php_sxe_get_first_node(sxe, node);
				}
				if (sxe->iter.type == SXE_ITER_NONE) {
					if (member && Z_LVAL_P(member) > 0) {
						php_error_docref(NULL, E_WARNING, "Cannot add element %s number %pd when only 0 such elements exist", mynode->name, Z_LVAL_P(member));
					}
				} else if (member) {
					node = sxe_get_element_by_offset(sxe, Z_LVAL_P(member), node, &cnt);
				} else {
					node = NULL;
				}
				if (node) {
					_node_as_zval(sxe, node, rv, SXE_ITER_NONE, NULL, sxe->iter.nsprefix, sxe->iter.isprefix);
				} else if (type == BP_VAR_W || type == BP_VAR_RW) {
					if (member && cnt < Z_LVAL_P(member)) {
						php_error_docref(NULL, E_WARNING, "Cannot add element %s number %pd when only %pd such elements exist", mynode->name, Z_LVAL_P(member), cnt);
					}
					node = xmlNewTextChild(mynode->parent, mynode->ns, mynode->name, NULL);
					_node_as_zval(sxe, node, rv, SXE_ITER_NONE, NULL, sxe->iter.nsprefix, sxe->iter.isprefix);
				}
			} else {
#if SXE_ELEMENT_BY_NAME
				int newtype;

				GET_NODE(sxe, node);
				node = sxe_get_element_by_name(sxe, node, &name, &newtype);
				if (node) {
					_node_as_zval(sxe, node, rv, newtype, name, sxe->iter.nsprefix, sxe->iter.isprefix);
				}
#else
				_node_as_zval(sxe, node, rv, SXE_ITER_ELEMENT, name, sxe->iter.nsprefix, sxe->iter.isprefix);
#endif
			}
		}
	}

	if (member == &tmp_zv) {
		zval_dtor(&tmp_zv);
	}

	if (Z_ISUNDEF_P(rv)) {
		ZVAL_COPY_VALUE(rv, &EG(uninitialized_zval));
	}

	return rv;
}
/* }}} */

/* {{{ sxe_property_read()
 */
static zval *sxe_property_read(zval *object, zval *member, int type, void **cache_slot, zval *rv)
{
	return sxe_prop_dim_read(object, member, 1, 0, type, rv);
}
/* }}} */

/* {{{ sxe_dimension_read()
 */
static zval *sxe_dimension_read(zval *object, zval *offset, int type, zval *rv)
{
	return sxe_prop_dim_read(object, offset, 0, 1, type, rv);
}
/* }}} */

/* {{{ change_node_zval()
 */
static void change_node_zval(xmlNodePtr node, zval *value)
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
		case IS_FALSE:
		case IS_TRUE:
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
			php_error_docref(NULL, E_WARNING, "It is not possible to assign complex types to nodes");
			break;
	}
}
/* }}} */

/* {{{ sxe_property_write()
 */
static int sxe_prop_dim_write(zval *object, zval *member, zval *value, zend_bool elements, zend_bool attribs, xmlNodePtr *pnewnode)
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
	zend_long            cnt = 0;
	int				retval = SUCCESS;
	zval            tmp_zv, zval_copy;
	zend_string    *trim_str;

	sxe = Z_SXEOBJ_P(object);

	if (!member || Z_TYPE_P(member) == IS_LONG) {
		if (sxe->iter.type != SXE_ITER_ATTRLIST) {
			attribs = 0;
			elements = 1;
		} else if (!member) {
			/* This happens when the user did: $sxe[] = $value
			 * and could also be E_PARSE, but we use this only during parsing
			 * and this is during runtime.
			 */
			php_error_docref(NULL, E_ERROR, "Cannot create unnamed attribute");
			return FAILURE;
		}
	} else {
		if (Z_TYPE_P(member) != IS_STRING) {
			trim_str = zval_get_string(member);
			ZVAL_STR(&tmp_zv, php_trim(trim_str, NULL, 0, 3));
			zend_string_release(trim_str);
			member = &tmp_zv;
		}

		if (!Z_STRLEN_P(member)) {
			php_error_docref(NULL, E_WARNING, "Cannot write or create unnamed %s", attribs ? "attribute" : "element");
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
		node = php_sxe_get_first_node(sxe, node);
		attr = (xmlAttrPtr)node;
		test = sxe->iter.name != NULL;
	} else if (sxe->iter.type != SXE_ITER_CHILD) {
		mynode = node;
		node = php_sxe_get_first_node(sxe, node);
		attr = node ? node->properties : NULL;
		test = 0;
		if (!member && node && node->parent &&
		    node->parent->type == XML_DOCUMENT_NODE) {
			/* This happens when the user did: $sxe[] = $value
			 * and could also be E_PARSE, but we use this only during parsing
			 * and this is during runtime.
			 */
			php_error_docref(NULL, E_ERROR, "Cannot create unnamed attribute");
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
			case IS_FALSE:
			case IS_TRUE:
			case IS_DOUBLE:
			case IS_NULL:
				if (Z_TYPE_P(value) != IS_STRING) {
					ZVAL_DUP(&zval_copy, value);
					value = &zval_copy;
					convert_to_string(value);
					new_value = 1;
				}
				break;
			case IS_STRING:
				break;
			case IS_OBJECT:
				if (Z_OBJCE_P(value) == sxe_class_entry) {
					//???
					value = sxe_get_value(value, &zval_copy);
					//INIT_PZVAL(value);
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
					php_error_docref(NULL, E_ERROR, "Cannot create duplicate attribute");
					return FAILURE;
				}

				if (sxe->iter.type == SXE_ITER_NONE) {
					newnode = node;
					++counter;
					if (member && Z_LVAL_P(member) > 0) {
						php_error_docref(NULL, E_WARNING, "Cannot add element %s number %pd when only 0 such elements exist", mynode->name, Z_LVAL_P(member));
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
					php_libxml_node_free_resource((xmlNodePtr) tempnode);
				}
				change_node_zval(newnode, value);
			}
		} else if (counter > 1) {
			php_error_docref(NULL, E_WARNING, "Cannot assign to an array of nodes (duplicate subnodes or attr detected)");
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
					php_error_docref(NULL, E_WARNING, "Cannot add element %s number %pd when only %pd such elements exist", mynode->name, Z_LVAL_P(member), cnt);
					retval = FAILURE;
				}
				newnode = xmlNewTextChild(mynode->parent, mynode->ns, mynode->name, value ? (xmlChar *)Z_STRVAL_P(value) : NULL);
			}
		} else if (attribs) {
			if (Z_TYPE_P(member) == IS_LONG) {
				php_error_docref(NULL, E_WARNING, "Cannot change attribute number %pd when only %d attributes exist", Z_LVAL_P(member), nodendx);
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
	if (new_value) {
		zval_ptr_dtor(value);
	}
	return retval;
}
/* }}} */

/* {{{ sxe_property_write()
 */
static void sxe_property_write(zval *object, zval *member, zval *value, void **cache_slot)
{
	sxe_prop_dim_write(object, member, value, 1, 0, NULL);
}
/* }}} */

/* {{{ sxe_dimension_write()
 */
static void sxe_dimension_write(zval *object, zval *offset, zval *value)
{
	sxe_prop_dim_write(object, offset, value, 0, 1, NULL);
}
/* }}} */

static zval *sxe_property_get_adr(zval *object, zval *member, int fetch_type, void **cache_slot) /* {{{ */
{
	php_sxe_object *sxe;
	xmlNodePtr      node;
	zval            ret;
	char           *name;
	SXE_ITER        type;

	sxe = Z_SXEOBJ_P(object);

	GET_NODE(sxe, node);
	convert_to_string(member);
	name = Z_STRVAL_P(member);
	node = sxe_get_element_by_name(sxe, node, &name, &type);
	if (node) {
		return NULL;
	}
	if (sxe_prop_dim_write(object, member, NULL, 1, 0, &node) != SUCCESS) {
		return NULL;
	}
	type = SXE_ITER_NONE;
	name = NULL;

	_node_as_zval(sxe, node, &ret, type, name, sxe->iter.nsprefix, sxe->iter.isprefix);

	sxe = Z_SXEOBJ_P(&ret);
	if (!Z_ISUNDEF(sxe->tmp)) {
		zval_ptr_dtor(&sxe->tmp);
	}

	ZVAL_COPY_VALUE(&sxe->tmp, &ret);
	//???? Z_SET_ISREF_P(return_value);

	return &sxe->tmp;
}
/* }}} */

/* {{{ sxe_prop_dim_exists()
 */
static int sxe_prop_dim_exists(zval *object, zval *member, int check_empty, zend_bool elements, zend_bool attribs)
{
	php_sxe_object *sxe;
	xmlNodePtr      node;
	xmlAttrPtr      attr = NULL;
	int				exists = 0;
	int             test = 0;
	zval            tmp_zv;

	if (Z_TYPE_P(member) != IS_STRING && Z_TYPE_P(member) != IS_LONG) {
		ZVAL_STR(&tmp_zv, zval_get_string(member));
		member = &tmp_zv;
	}

	sxe = Z_SXEOBJ_P(object);

	GET_NODE(sxe, node);

	if (Z_TYPE_P(member) == IS_LONG) {
		if (sxe->iter.type != SXE_ITER_ATTRLIST) {
			attribs = 0;
			elements = 1;
			if (sxe->iter.type == SXE_ITER_CHILD) {
				node = php_sxe_get_first_node(sxe, node);
			}
		}
	}

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		attribs = 1;
		elements = 0;
		node = php_sxe_get_first_node(sxe, node);
		attr = (xmlAttrPtr)node;
		test = sxe->iter.name != NULL;
	} else if (sxe->iter.type != SXE_ITER_CHILD) {
		node = php_sxe_get_first_node(sxe, node);
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
				(!attr->children || !attr->children->content || !attr->children->content[0] || !xmlStrcmp(attr->children->content, (const xmlChar *) "0")) ) {
				/* Attribute with no content in it's text node */
				exists = 0;
			}
		}

		if (elements) {
			if (Z_TYPE_P(member) == IS_LONG) {
				if (sxe->iter.type == SXE_ITER_CHILD) {
					node = php_sxe_get_first_node(sxe, node);
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
					 (!node->children->content || !node->children->content[0] || !xmlStrcmp(node->children->content, (const xmlChar *) "0")))) ) {
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
static int sxe_property_exists(zval *object, zval *member, int check_empty, void **cache_slot)
{
	return sxe_prop_dim_exists(object, member, check_empty, 1, 0);
}
/* }}} */

/* {{{ sxe_property_exists()
 */
static int sxe_dimension_exists(zval *object, zval *member, int check_empty)
{
	return sxe_prop_dim_exists(object, member, check_empty, 0, 1);
}
/* }}} */

/* {{{ sxe_prop_dim_delete()
 */
static void sxe_prop_dim_delete(zval *object, zval *member, zend_bool elements, zend_bool attribs)
{
	php_sxe_object *sxe;
	xmlNodePtr      node;
	xmlNodePtr      nnext;
	xmlAttrPtr      attr = NULL;
	xmlAttrPtr      anext;
	zval            tmp_zv;
	int             test = 0;

	if (Z_TYPE_P(member) != IS_STRING && Z_TYPE_P(member) != IS_LONG) {
		ZVAL_STR(&tmp_zv, zval_get_string(member));
		member = &tmp_zv;
	}

	sxe = Z_SXEOBJ_P(object);

	GET_NODE(sxe, node);

	if (Z_TYPE_P(member) == IS_LONG) {
		if (sxe->iter.type != SXE_ITER_ATTRLIST) {
			attribs = 0;
			elements = 1;
			if (sxe->iter.type == SXE_ITER_CHILD) {
				node = php_sxe_get_first_node(sxe, node);
			}
		}
	}

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		attribs = 1;
		elements = 0;
		node = php_sxe_get_first_node(sxe, node);
		attr = (xmlAttrPtr)node;
		test = sxe->iter.name != NULL;
	} else if (sxe->iter.type != SXE_ITER_CHILD) {
		node = php_sxe_get_first_node(sxe, node);
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
							php_libxml_node_free_resource((xmlNodePtr) attr);
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
						php_libxml_node_free_resource((xmlNodePtr) attr);
						break;
					}
					attr = anext;
				}
			}
		}

		if (elements) {
			if (Z_TYPE_P(member) == IS_LONG) {
				if (sxe->iter.type == SXE_ITER_CHILD) {
					node = php_sxe_get_first_node(sxe, node);
				}
				node = sxe_get_element_by_offset(sxe, Z_LVAL_P(member), node, NULL);
				if (node) {
					xmlUnlinkNode(node);
					php_libxml_node_free_resource(node);
				}
			} else {
				node = node->children;
				while (node) {
					nnext = node->next;

					SKIP_TEXT(node);

					if (!xmlStrcmp(node->name, (xmlChar *)Z_STRVAL_P(member))) {
						xmlUnlinkNode(node);
						php_libxml_node_free_resource(node);
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
static void sxe_property_delete(zval *object, zval *member, void **cache_slot)
{
	sxe_prop_dim_delete(object, member, 1, 0);
}
/* }}} */

/* {{{ sxe_dimension_unset()
 */
static void sxe_dimension_delete(zval *object, zval *offset)
{
	sxe_prop_dim_delete(object, offset, 0, 1);
}
/* }}} */

static inline zend_string *sxe_xmlNodeListGetString(xmlDocPtr doc, xmlNodePtr list, int inLine) /* {{{ */
{
	xmlChar *tmp = xmlNodeListGetString(doc, list, inLine);
	zend_string *res;

	if (tmp) {
		res = zend_string_init((char*)tmp, strlen((char *)tmp), 0);
		xmlFree(tmp);
	} else {
		res = STR_EMPTY_ALLOC();
	}

	return res;
}
/* }}} */

/* {{{ _get_base_node_value()
 */
static void _get_base_node_value(php_sxe_object *sxe_ref, xmlNodePtr node, zval *value, xmlChar *nsprefix, int isprefix)
{
	php_sxe_object *subnode;
	xmlChar        *contents;

	if (node->children && node->children->type == XML_TEXT_NODE && !xmlIsBlankNode(node->children)) {
		contents = xmlNodeListGetString(node->doc, node->children, 1);
		if (contents) {
			ZVAL_STRING(value, (char *)contents);
			xmlFree(contents);
		}
	} else {
		subnode = php_sxe_object_new(sxe_ref->zo.ce);
		subnode->document = sxe_ref->document;
		subnode->document->refcount++;
		if (nsprefix && *nsprefix) {
			subnode->iter.nsprefix = xmlStrdup((xmlChar *)nsprefix);
			subnode->iter.isprefix = isprefix;
		}
		php_libxml_increment_node_ptr((php_libxml_node_object *)subnode, node, NULL);

		ZVAL_OBJ(value, &subnode->zo);
		/*zval_add_ref(value);*/
	}
}
/* }}} */

static void sxe_properties_add(HashTable *rv, char *name, int namelen, zval *value) /* {{{ */
{
	zval  *data_ptr;
	zval  newptr;

	if ((data_ptr = zend_hash_str_find(rv, name, namelen)) != NULL) {
		if (Z_TYPE_P(data_ptr) == IS_ARRAY) {
			zend_hash_next_index_insert(Z_ARRVAL_P(data_ptr), value);
		} else {
			array_init(&newptr);

			if (Z_REFCOUNTED_P(data_ptr)) {
				Z_ADDREF_P(data_ptr);
			}
			zend_hash_next_index_insert(Z_ARRVAL(newptr), data_ptr);
			zend_hash_next_index_insert(Z_ARRVAL(newptr), value);

			zend_hash_str_update(rv, name, namelen, &newptr);
		}
	} else {
		zend_hash_str_update(rv, name, namelen, value);
	}
}
/* }}} */

static HashTable *sxe_get_prop_hash(zval *object, int is_debug) /* {{{ */
{
	zval            value;
	zval            zattr;
	HashTable       *rv;
	php_sxe_object  *sxe;
	char            *name;
	xmlNodePtr       node;
	xmlAttrPtr       attr;
	int              namelen;
	int              test;
	char 		 	 use_iter;
	zval             iter_data;

	use_iter = 0;

	sxe = Z_SXEOBJ_P(object);

	if (is_debug) {
		ALLOC_HASHTABLE(rv);
		zend_hash_init(rv, 0, NULL, ZVAL_PTR_DTOR, 0);
	} else if (sxe->properties) {
		zend_hash_clean(sxe->properties);
		rv = sxe->properties;
	} else {
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
			node = php_sxe_get_first_node(sxe, node);
		}
		if (!node || node->type != XML_ENTITY_DECL) {
			attr = node ? (xmlAttrPtr)node->properties : NULL;
			ZVAL_UNDEF(&zattr);
			test = sxe->iter.name && sxe->iter.type == SXE_ITER_ATTRLIST;
			while (attr) {
				if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) && match_ns(sxe, (xmlNodePtr)attr, sxe->iter.nsprefix, sxe->iter.isprefix)) {
					ZVAL_STR(&value, sxe_xmlNodeListGetString((xmlDocPtr) sxe->document->ptr, attr->children, 1));
					namelen = xmlStrlen(attr->name);
					if (Z_ISUNDEF(zattr)) {
						array_init(&zattr);
						sxe_properties_add(rv, "@attributes", sizeof("@attributes") - 1, &zattr);
					}
					add_assoc_zval_ex(&zattr, (char*)attr->name, namelen, &value);
				}
				attr = attr->next;
			}
		}
	}

	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node);

	if (node && sxe->iter.type != SXE_ITER_ATTRLIST) {
		if (node->type == XML_ATTRIBUTE_NODE) {
			ZVAL_STR(&value, sxe_xmlNodeListGetString(node->doc, node->children, 1));
			zend_hash_next_index_insert(rv, &value);
			node = NULL;
		} else if (sxe->iter.type != SXE_ITER_CHILD) {

			if ( !node->children || !node->parent || !node->next || node->children->next || node->children->children || node->parent->children == node->parent->last ) {
				node = node->children;
			} else {
				ZVAL_COPY_VALUE(&iter_data, &sxe->iter.data);
				ZVAL_UNDEF(&sxe->iter.data);

				node = php_sxe_reset_iterator(sxe, 0);

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
						ZVAL_STR(&value, sxe_xmlNodeListGetString(node->doc, node, 1));
						zend_hash_next_index_insert(rv, &value);
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
				namelen = xmlStrlen(node->name);
			}

			_get_base_node_value(sxe, node, &value, sxe->iter.nsprefix, sxe->iter.isprefix);

			if ( use_iter ) {
				zend_hash_next_index_insert(rv, &value);
			} else {
				sxe_properties_add(rv, name, namelen, &value);
			}
next_iter:
			if (use_iter) {
				node = php_sxe_iterator_fetch(sxe, node->next, 0);
			} else {
				node = node->next;
			}
		}
	}

	if (use_iter) {
		if (!Z_ISUNDEF(sxe->iter.data)) {
			zval_ptr_dtor(&sxe->iter.data);
		}
		ZVAL_COPY_VALUE(&sxe->iter.data, &iter_data);
	}

	return rv;
}
/* }}} */

static HashTable *sxe_get_gc(zval *object, zval **table, int *n) /* {{{ */ {
	php_sxe_object *sxe;
	sxe = Z_SXEOBJ_P(object);

	*table = NULL;
	*n = 0;
	return sxe->properties;
}
/* }}} */

static HashTable *sxe_get_properties(zval *object) /* {{{ */
{
	return sxe_get_prop_hash(object, 0);
}
/* }}} */

static HashTable * sxe_get_debug_info(zval *object, int *is_temp) /* {{{ */
{
	*is_temp = 1;
	return sxe_get_prop_hash(object, 1);
}
/* }}} */

static int sxe_objects_compare(zval *object1, zval *object2) /* {{{ */
{
	php_sxe_object *sxe1;
	php_sxe_object *sxe2;

	sxe1 = Z_SXEOBJ_P(object1);
	sxe2 = Z_SXEOBJ_P(object2);

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
	zval               value;
	char              *query;
	size_t                query_len;
	int                i;
	int                nsnbr = 0;
	xmlNsPtr          *ns = NULL;
	xmlXPathObjectPtr  retval;
	xmlNodeSetPtr      result;
	xmlNodePtr		   nodeptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &query, &query_len) == FAILURE) {
		return;
	}

	sxe = Z_SXEOBJ_P(getThis());

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		return; /* attributes don't have attributes */
	}

	if (!sxe->xpath) {
		sxe->xpath = xmlXPathNewContext((xmlDocPtr) sxe->document->ptr);
	}
	if (!sxe->node) {
		php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement((xmlDocPtr) sxe->document->ptr), NULL);
		if (!sxe->node) {
			RETURN_FALSE;
		}
	}

	nodeptr = php_sxe_get_first_node(sxe, sxe->node->node);

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
				/**
				 * Detect the case where the last selector is text(), simplexml
				 * always accesses the text() child by default, therefore we assign
				 * to the parent node.
				 */
				if (nodeptr->type == XML_TEXT_NODE) {
					_node_as_zval(sxe, nodeptr->parent, &value, SXE_ITER_NONE, NULL, NULL, 0);
				} else if (nodeptr->type == XML_ATTRIBUTE_NODE) {
					_node_as_zval(sxe, nodeptr->parent, &value, SXE_ITER_ATTRLIST, (char*)nodeptr->name, nodeptr->ns ? (xmlChar *)nodeptr->ns->href : NULL, 0);
				} else {
					_node_as_zval(sxe, nodeptr, &value, SXE_ITER_NONE, NULL, NULL, 0);
				}

				add_next_index_zval(return_value, &value);
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
	size_t prefix_len, ns_uri_len;
	char *prefix, *ns_uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &prefix, &prefix_len, &ns_uri, &ns_uri_len) == FAILURE) {
		return;
	}

	sxe = Z_SXEOBJ_P(getThis());
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
	size_t                 filename_len;

	if (ZEND_NUM_ARGS() > 1) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &filename, &filename_len) == FAILURE) {
			RETURN_FALSE;
		}

		sxe = Z_SXEOBJ_P(getThis());
		GET_NODE(sxe, node);
		node = php_sxe_get_first_node(sxe, node);

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

	sxe = Z_SXEOBJ_P(getThis());
	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node);

	if (node) {
		if (node->parent && (XML_DOCUMENT_NODE == node->parent->type)) {
			xmlDocDumpMemoryEnc((xmlDocPtr) sxe->document->ptr, &strval, &strval_len, (const char *) ((xmlDocPtr) sxe->document->ptr)->encoding);
			RETVAL_STRINGL((char *)strval, strval_len);
			xmlFree(strval);
		} else {
			/* Should we be passing encoding information instead of NULL? */
			outbuf = xmlAllocOutputBuffer(NULL);

			if (outbuf == NULL) {
				RETURN_FALSE;
			}

			xmlNodeDumpOutput(outbuf, (xmlDocPtr) sxe->document->ptr, node, 0, 0, (const char *) ((xmlDocPtr) sxe->document->ptr)->encoding);
			xmlOutputBufferFlush(outbuf);
#ifdef LIBXML2_NEW_BUFFER
			RETVAL_STRINGL((char *)xmlOutputBufferGetContent(outbuf), xmlOutputBufferGetSize(outbuf));
#else
			RETVAL_STRINGL((char *)outbuf->buffer->content, outbuf->buffer->use);
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
	if (zend_hash_str_exists(Z_ARRVAL_P(return_value), prefix, strlen(prefix)) == 0) {
		add_assoc_string(return_value, prefix, (char*)ns->href);
	}
}
/* }}} */

static void sxe_add_namespaces(php_sxe_object *sxe, xmlNodePtr node, zend_bool recursive, zval *return_value) /* {{{ */
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
				sxe_add_namespaces(sxe, node, recursive, return_value);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &recursive) == FAILURE) {
		return;
	}

	array_init(return_value);

	sxe = Z_SXEOBJ_P(getThis());
	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node);

	if (node) {
		if (node->type == XML_ELEMENT_NODE) {
			sxe_add_namespaces(sxe, node, recursive, return_value);
		} else if (node->type == XML_ATTRIBUTE_NODE && node->ns) {
			sxe_add_namespace_name(return_value, node->ns);
		}
	}
}
/* }}} */

static void sxe_add_registered_namespaces(php_sxe_object *sxe, xmlNodePtr node, zend_bool recursive, zval *return_value) /* {{{ */
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
				sxe_add_registered_namespaces(sxe, node, recursive, return_value);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|bb", &recursive, &from_root) == FAILURE) {
		return;
	}

	sxe = Z_SXEOBJ_P(getThis());
	if(from_root){
		node = xmlDocGetRootElement((xmlDocPtr)sxe->document->ptr);
	}else{
		GET_NODE(sxe, node);
	}

	if (node == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	sxe_add_registered_namespaces(sxe, node, recursive, return_value);
}
/* }}} */

/* {{{ proto object SimpleXMLElement::children([string ns [, bool is_prefix]])
   Finds children of given node */
SXE_METHOD(children)
{
	php_sxe_object *sxe;
	char           *nsprefix = NULL;
	size_t             nsprefix_len = 0;
	xmlNodePtr      node;
	zend_bool       isprefix = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!b", &nsprefix, &nsprefix_len, &isprefix) == FAILURE) {
		return;
	}

	sxe = Z_SXEOBJ_P(getThis());

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		return; /* attributes don't have attributes */
	}

	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node);

	_node_as_zval(sxe, node, return_value, SXE_ITER_CHILD, NULL, (xmlChar *)nsprefix, isprefix);

}
/* }}} */

/* {{{ proto object SimpleXMLElement::getName()
   Finds children of given node */
SXE_METHOD(getName)
{
	php_sxe_object *sxe;
	xmlNodePtr      node;
	int             namelen;

	sxe = Z_SXEOBJ_P(getThis());

	GET_NODE(sxe, node);
	node = php_sxe_get_first_node(sxe, node);
	if (node) {
		namelen = xmlStrlen(node->name);
		RETURN_STRINGL((char*)node->name, namelen);
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
	size_t             nsprefix_len = 0;
	xmlNodePtr      node;
	zend_bool       isprefix = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!b", &nsprefix, &nsprefix_len, &isprefix) == FAILURE) {
		return;
	}

	sxe = Z_SXEOBJ_P(getThis());
	GET_NODE(sxe, node);

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		return; /* attributes don't have attributes */
	}

	node = php_sxe_get_first_node(sxe, node);

	_node_as_zval(sxe, node, return_value, SXE_ITER_ATTRLIST, NULL, (xmlChar *)nsprefix, isprefix);
}
/* }}} */

/* {{{ proto void SimpleXMLElement::addChild(string qName [, string value [, string ns]])
   Add Element with optional namespace information */
SXE_METHOD(addChild)
{
	php_sxe_object *sxe;
	char           *qname, *value = NULL, *nsuri = NULL;
	size_t             qname_len, value_len = 0, nsuri_len = 0;
	xmlNodePtr      node, newnode;
	xmlNsPtr        nsptr = NULL;
	xmlChar        *localname, *prefix = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!s!",
		&qname, &qname_len, &value, &value_len, &nsuri, &nsuri_len) == FAILURE) {
		return;
	}

	if (qname_len == 0) {
		php_error_docref(NULL, E_WARNING, "Element name is required");
		return;
	}

	sxe = Z_SXEOBJ_P(getThis());
	GET_NODE(sxe, node);

	if (sxe->iter.type == SXE_ITER_ATTRLIST) {
		php_error_docref(NULL, E_WARNING, "Cannot add element to attributes");
		return;
	}

	node = php_sxe_get_first_node(sxe, node);

	if (node == NULL) {
		php_error_docref(NULL, E_WARNING, "Cannot add child. Parent is not a permanent member of the XML tree");
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

	_node_as_zval(sxe, newnode, return_value, SXE_ITER_NONE, (char *)localname, prefix, 0);

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
	size_t             qname_len, value_len = 0, nsuri_len = 0;
	xmlNodePtr      node;
	xmlAttrPtr      attrp = NULL;
	xmlNsPtr        nsptr = NULL;
	xmlChar        *localname, *prefix = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|s!",
		&qname, &qname_len, &value, &value_len, &nsuri, &nsuri_len) == FAILURE) {
		return;
	}

	if (qname_len == 0) {
		php_error_docref(NULL, E_WARNING, "Attribute name is required");
		return;
	}

	sxe = Z_SXEOBJ_P(getThis());
	GET_NODE(sxe, node);

	node = php_sxe_get_first_node(sxe, node);

	if (node && node->type != XML_ELEMENT_NODE) {
		node = node->parent;
	}

	if (node == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to locate parent Element");
		return;
	}

	localname = xmlSplitQName2((xmlChar *)qname, &prefix);
	if (localname == NULL) {
		if (nsuri_len > 0) {
			if (prefix != NULL) {
				xmlFree(prefix);
			}
			php_error_docref(NULL, E_WARNING, "Attribute requires prefix for namespace");
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
		php_error_docref(NULL, E_WARNING, "Attribute already exists");
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
static int cast_object(zval *object, int type, char *contents)
{
	if (contents) {
		ZVAL_STRINGL(object, contents, strlen(contents));
	} else {
		ZVAL_NULL(object);
	}
	//???? Z_SET_REFCOUNT_P(object, 1);
	//Z_UNSET_ISREF_P(object);

	switch (type) {
		case IS_STRING:
			convert_to_string(object);
			break;
		case _IS_BOOL:
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
static int sxe_object_cast_ex(zval *readobj, zval *writeobj, int type)
{
	php_sxe_object *sxe;
	xmlChar        *contents = NULL;
	xmlNodePtr	    node;
	int rv;
	HashTable      *prop_hash;

	sxe = Z_SXEOBJ_P(readobj);

	if (type == _IS_BOOL) {
		node = php_sxe_get_first_node(sxe, NULL);
		prop_hash = sxe_get_prop_hash(readobj, 1);
		ZVAL_BOOL(writeobj, node != NULL || zend_hash_num_elements(prop_hash) > 0);
		zend_hash_destroy(prop_hash);
		efree(prop_hash);
		return SUCCESS;
	}

	if (sxe->iter.type != SXE_ITER_NONE) {
		node = php_sxe_get_first_node(sxe, NULL);
		if (node) {
			contents = xmlNodeListGetString((xmlDocPtr) sxe->document->ptr, node->children, 1);
		}
	} else {
		if (!sxe->node) {
			if (sxe->document) {
				php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement((xmlDocPtr) sxe->document->ptr), NULL);
			}
		}

		if (sxe->node && sxe->node->node) {
			if (sxe->node->node->children) {
				contents = xmlNodeListGetString((xmlDocPtr) sxe->document->ptr, sxe->node->node->children, 1);
			}
		}
	}

	if (readobj == writeobj) {
		zval_ptr_dtor(readobj);
	}

	rv = cast_object(writeobj, type, (char *)contents);

	if (contents) {
		xmlFree(contents);
	}

	return rv;
}
/* }}} */

/* Variant of sxe_object_cast_ex that handles overwritten __toString() method */
static int sxe_object_cast(zval *readobj, zval *writeobj, int type)
{
	if (type == IS_STRING
		&& zend_std_cast_object_tostring(readobj, writeobj, IS_STRING) == SUCCESS
	) {
		return SUCCESS;
	}

	return sxe_object_cast_ex(readobj, writeobj, type);
}

/* {{{ proto object SimpleXMLElement::__toString() U
   Returns the string content */
SXE_METHOD(__toString)
{
	zval           result;

	if (sxe_object_cast_ex(getThis(), &result, IS_STRING) == SUCCESS) {
		RETURN_ZVAL(&result, 0, 0);
	} else {
		zval_ptr_dtor(&result);
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

static int php_sxe_count_elements_helper(php_sxe_object *sxe, zend_long *count) /* {{{ */
{
	xmlNodePtr      node;
	zval            data;

	*count = 0;

	ZVAL_COPY_VALUE(&data, &sxe->iter.data);
	ZVAL_UNDEF(&sxe->iter.data);

	node = php_sxe_reset_iterator(sxe, 0);

	while (node)
	{
		(*count)++;
		node = php_sxe_iterator_fetch(sxe, node->next, 0);
	}

	if (!Z_ISUNDEF(sxe->iter.data)) {
		zval_ptr_dtor(&sxe->iter.data);
	}
	ZVAL_COPY_VALUE(&sxe->iter.data, &data);

	return SUCCESS;
}
/* }}} */

static int sxe_count_elements(zval *object, zend_long *count) /* {{{ */
{
	php_sxe_object  *intern;
	intern = Z_SXEOBJ_P(object);
	if (intern->fptr_count) {
		zval rv;
		zend_call_method_with_0_params(object, intern->zo.ce, &intern->fptr_count, "count", &rv);
		if (!Z_ISUNDEF(rv)) {
			if (!Z_ISUNDEF(intern->tmp)) {
				zval_ptr_dtor(&intern->tmp);
			}
			ZVAL_ZVAL(&intern->tmp, &rv, 0, 0);
			convert_to_long(&intern->tmp);
			*count = (zend_long)Z_LVAL(intern->tmp);
			return SUCCESS;
		}
		return FAILURE;
	}
	return php_sxe_count_elements_helper(intern, count);
}
/* }}} */

/* {{{ proto int SimpleXMLElement::count()
 Get number of child elements */
SXE_METHOD(count)
{
	zend_long count = 0;
	php_sxe_object *sxe = Z_SXEOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	php_sxe_count_elements_helper(sxe, &count);

	RETURN_LONG(count);
}
/* }}} */

static zval *sxe_get_value(zval *z, zval *rv) /* {{{ */
{
	if (sxe_object_cast_ex(z, rv, IS_STRING) == FAILURE) {
		zend_error(E_ERROR, "Unable to cast node to string");
		/* FIXME: Should not be fatal */
	}

	return rv;
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
	NULL, /* zend_get_std_object_handlers()->get_class_name,*/
	sxe_objects_compare,
	sxe_object_cast,
	sxe_count_elements,
	sxe_get_debug_info,
	NULL,
	sxe_get_gc
};
/* }}} */

/* {{{ sxe_object_clone()
 */
static zend_object *
sxe_object_clone(zval *object)
{
	php_sxe_object *sxe = Z_SXEOBJ_P(object);
	php_sxe_object *clone;
	xmlNodePtr nodep = NULL;
	xmlDocPtr docp = NULL;

	clone = php_sxe_object_new(sxe->zo.ce);
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

	php_libxml_increment_node_ptr((php_libxml_node_object *)clone, nodep, NULL);

	return &clone->zo;
}
/* }}} */

/* {{{ sxe_object_dtor()
 */
static void sxe_object_dtor(zend_object *object)
{
	/* dtor required to cleanup iterator related data properly */
	php_sxe_object *sxe;

	sxe = php_sxe_fetch_object(object);

	if (!Z_ISUNDEF(sxe->iter.data)) {
		zval_ptr_dtor(&sxe->iter.data);
		ZVAL_UNDEF(&sxe->iter.data);
	}

	if (sxe->iter.name) {
		xmlFree(sxe->iter.name);
		sxe->iter.name = NULL;
	}
	if (sxe->iter.nsprefix) {
		xmlFree(sxe->iter.nsprefix);
		sxe->iter.nsprefix = NULL;
	}
	if (!Z_ISUNDEF(sxe->tmp)) {
		zval_ptr_dtor(&sxe->tmp);
		ZVAL_UNDEF(&sxe->tmp);
	}
}
/* }}} */

/* {{{ sxe_object_free_storage()
 */
static void sxe_object_free_storage(zend_object *object)
{
	php_sxe_object *sxe;

	sxe = php_sxe_fetch_object(object);

	zend_object_std_dtor(&sxe->zo);

	php_libxml_node_decrement_resource((php_libxml_node_object *)sxe);

	if (sxe->xpath) {
		xmlXPathFreeContext(sxe->xpath);
	}

	if (sxe->properties) {
		zend_hash_destroy(sxe->properties);
		FREE_HASHTABLE(sxe->properties);
	}
}
/* }}} */

/* {{{ php_sxe_object_new()
 */
static php_sxe_object* php_sxe_object_new(zend_class_entry *ce)
{
	php_sxe_object *intern;
	zend_class_entry *parent = ce;
	int inherited = 0;

	intern = ecalloc(1, sizeof(php_sxe_object) + zend_object_properties_size(parent));

	intern->iter.type = SXE_ITER_NONE;
	intern->iter.nsprefix = NULL;
	intern->iter.name = NULL;
	intern->fptr_count = NULL;

	zend_object_std_init(&intern->zo, ce);
	object_properties_init(&intern->zo, ce);
	intern->zo.handlers = &sxe_object_handlers;

	while (parent) {
		if (parent == sxe_class_entry) {
			break;
		}

		parent = parent->parent;
		inherited = 1;
	}

	if (inherited) {
		intern->fptr_count = zend_hash_str_find_ptr(&ce->function_table, "count", sizeof("count") - 1);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}

	return intern;
}
/* }}} */

/* {{{ sxe_object_new()
 */
PHP_SXE_API zend_object *
sxe_object_new(zend_class_entry *ce)
{
	php_sxe_object    *intern;

	intern = php_sxe_object_new(ce);
	return &intern->zo;
}
/* }}} */

/* {{{ proto simplemxml_element simplexml_load_file(string filename [, string class_name [, int options [, string ns [, bool is_prefix]]]])
   Load a filename and return a simplexml_element object to allow for processing */
PHP_FUNCTION(simplexml_load_file)
{
	php_sxe_object *sxe;
	char           *filename;
	size_t             filename_len;
	xmlDocPtr       docp;
	char           *ns = NULL;
	size_t             ns_len = 0;
	zend_long            options = 0;
	zend_class_entry *ce= sxe_class_entry;
	zend_bool       isprefix = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|C!lsb", &filename, &filename_len, &ce, &options, &ns, &ns_len, &isprefix) == FAILURE) {
		return;
	}

	docp = xmlReadFile(filename, NULL, options);

	if (!docp) {
		RETURN_FALSE;
	}

	if (!ce) {
		ce = sxe_class_entry;
	}
	sxe = php_sxe_object_new(ce);
	sxe->iter.nsprefix = ns_len ? xmlStrdup((xmlChar *)ns) : NULL;
	sxe->iter.isprefix = isprefix;
	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp);
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL);

	ZVAL_OBJ(return_value, &sxe->zo);
}
/* }}} */

/* {{{ proto simplemxml_element simplexml_load_string(string data [, string class_name [, int options [, string ns [, bool is_prefix]]]])
   Load a string and return a simplexml_element object to allow for processing */
PHP_FUNCTION(simplexml_load_string)
{
	php_sxe_object *sxe;
	char           *data;
	size_t             data_len;
	xmlDocPtr       docp;
	char           *ns = NULL;
	size_t             ns_len = 0;
	zend_long            options = 0;
	zend_class_entry *ce= sxe_class_entry;
	zend_bool       isprefix = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|C!lsb", &data, &data_len, &ce, &options, &ns, &ns_len, &isprefix) == FAILURE) {
		return;
	}

	docp = xmlReadMemory(data, data_len, NULL, NULL, options);

	if (!docp) {
		RETURN_FALSE;
	}

	if (!ce) {
		ce = sxe_class_entry;
	}
	sxe = php_sxe_object_new(ce);
	sxe->iter.nsprefix = ns_len ? xmlStrdup((xmlChar *)ns) : NULL;
	sxe->iter.isprefix = isprefix;
	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp);
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL);

	ZVAL_OBJ(return_value, &sxe->zo);
}
/* }}} */

/* {{{ proto SimpleXMLElement::__construct(string data [, int options [, bool data_is_url [, string ns [, bool is_prefix]]]])
   SimpleXMLElement constructor */
SXE_METHOD(__construct)
{
	php_sxe_object *sxe = Z_SXEOBJ_P(getThis());
	char           *data, *ns = NULL;
	size_t             data_len, ns_len = 0;
	xmlDocPtr       docp;
	zend_long            options = 0;
	zend_bool       is_url = 0, isprefix = 0;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, NULL, &error_handling);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|lbsb", &data, &data_len, &options, &is_url, &ns, &ns_len, &isprefix) == FAILURE) {
		zend_restore_error_handling(&error_handling);
		return;
	}

	zend_restore_error_handling(&error_handling);

	docp = is_url ? xmlReadFile(data, NULL, options) : xmlReadMemory(data, data_len, NULL, NULL, options);

	if (!docp) {
		((php_libxml_node_object *)sxe)->document = NULL;
		zend_throw_exception(zend_exception_get_default(), "String could not be parsed as XML", 0);
		return;
	}

	sxe->iter.nsprefix = ns_len ? xmlStrdup((xmlChar *)ns) : NULL;
	sxe->iter.isprefix = isprefix;
	php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, docp);
	php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, xmlDocGetRootElement(docp), NULL);
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

static xmlNodePtr php_sxe_iterator_fetch(php_sxe_object *sxe, xmlNodePtr node, int use_data) /* {{{ */
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
		_node_as_zval(sxe, node, &sxe->iter.data, SXE_ITER_NONE, NULL, prefix, isprefix);
	}

	return node;
}
/* }}} */

static xmlNodePtr php_sxe_reset_iterator(php_sxe_object *sxe, int use_data) /* {{{ */
{
	xmlNodePtr node;

	if (!Z_ISUNDEF(sxe->iter.data)) {
		zval_ptr_dtor(&sxe->iter.data);
		ZVAL_UNDEF(&sxe->iter.data);
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
		return php_sxe_iterator_fetch(sxe, node, use_data);
	}
	return NULL;
}
/* }}} */

zend_object_iterator *php_sxe_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	php_sxe_iterator *iterator;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}
	iterator = emalloc(sizeof(php_sxe_iterator));
	zend_iterator_init(&iterator->intern);

	ZVAL_COPY(&iterator->intern.data, object);
	iterator->intern.funcs = &php_sxe_iterator_funcs;
	iterator->sxe = Z_SXEOBJ_P(object);

	return (zend_object_iterator*)iterator;
}
/* }}} */

static void php_sxe_iterator_dtor(zend_object_iterator *iter) /* {{{ */
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	/* cleanup handled in sxe_object_dtor as we dont always have an iterator wrapper */
	if (!Z_ISUNDEF(iterator->intern.data)) {
		zval_ptr_dtor(&iterator->intern.data);
	}
}
/* }}} */

static int php_sxe_iterator_valid(zend_object_iterator *iter) /* {{{ */
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	return Z_ISUNDEF(iterator->sxe->iter.data) ? FAILURE : SUCCESS;
}
/* }}} */

static zval *php_sxe_iterator_current_data(zend_object_iterator *iter) /* {{{ */
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;

	return &iterator->sxe->iter.data;
}
/* }}} */

static void php_sxe_iterator_current_key(zend_object_iterator *iter, zval *key) /* {{{ */
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	zval *curobj = &iterator->sxe->iter.data;
	php_sxe_object *intern = Z_SXEOBJ_P(curobj);

	xmlNodePtr curnode = NULL;
	if (intern != NULL && intern->node != NULL) {
		curnode = (xmlNodePtr)((php_libxml_node_ptr *)intern->node)->node;
	}

	if (curnode) {
		ZVAL_STRINGL(key, (char *) curnode->name, xmlStrlen(curnode->name));
	} else {
		ZVAL_NULL(key);
	}
}
/* }}} */

PHP_SXE_API void php_sxe_move_forward_iterator(php_sxe_object *sxe) /* {{{ */
{
	xmlNodePtr      node = NULL;
	php_sxe_object  *intern;

	if (!Z_ISUNDEF(sxe->iter.data)) {
		intern = Z_SXEOBJ_P(&sxe->iter.data);
		GET_NODE(intern, node)
		zval_ptr_dtor(&sxe->iter.data);
		ZVAL_UNDEF(&sxe->iter.data);
	}

	if (node) {
		php_sxe_iterator_fetch(sxe, node->next, 1);
	}
}
/* }}} */

static void php_sxe_iterator_move_forward(zend_object_iterator *iter) /* {{{ */
{
	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	php_sxe_move_forward_iterator(iterator->sxe);
}
/* }}} */

static void php_sxe_iterator_rewind(zend_object_iterator *iter) /* {{{ */
{
	php_sxe_object	*sxe;

	php_sxe_iterator *iterator = (php_sxe_iterator *)iter;
	sxe = iterator->sxe;

	php_sxe_reset_iterator(sxe, 1);
}
/* }}} */

void *simplexml_export_node(zval *object) /* {{{ */
{
	php_sxe_object *sxe;
	xmlNodePtr node;

	sxe = Z_SXEOBJ_P(object);
	GET_NODE(sxe, node);
	return php_sxe_get_first_node(sxe, node);
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
	zend_class_entry *ce = sxe_class_entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|C!", &node, &ce) == FAILURE) {
		return;
	}

	object = Z_LIBXML_NODE_P(node);

	nodep = php_libxml_import_node(node);

	if (nodep) {
		if (nodep->doc == NULL) {
			php_error_docref(NULL, E_WARNING, "Imported Node must have associated Document");
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
		sxe = php_sxe_object_new(ce);
		sxe->document = object->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)sxe, nodep->doc);
		php_libxml_increment_node_ptr((php_libxml_node_object *)sxe, nodep, NULL);

		ZVAL_OBJ(return_value, &sxe->zo);
	} else {
		php_error_docref(NULL, E_WARNING, "Invalid Nodetype to import");
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
	sxe_class_entry = zend_register_internal_class(&sxe);
	sxe_class_entry->get_iterator = php_sxe_get_iterator;
	sxe_class_entry->iterator_funcs.funcs = &php_sxe_iterator_funcs;
	zend_class_implements(sxe_class_entry, 1, zend_ce_traversable);
	sxe_object_handlers.offset = XtOffsetOf(php_sxe_object, zo);
	sxe_object_handlers.dtor_obj = sxe_object_dtor;
	sxe_object_handlers.free_obj = sxe_object_free_storage;
	sxe_object_handlers.clone_obj = sxe_object_clone;
	sxe_object_handlers.get_method = zend_get_std_object_handlers()->get_method;
	sxe_object_handlers.get_constructor = zend_get_std_object_handlers()->get_constructor;
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
