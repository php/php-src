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
  | Authors: Christian Stocker <chregu@php.net>                          |
  |          Rob Richards <rrichards@php.net>                            |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_XML_COMMON_H
#define PHP_XML_COMMON_H

#include "ext/libxml/php_libxml.h"

typedef libxml_doc_props *dom_doc_propsptr;

typedef struct _dom_object {
	void *ptr;
	php_libxml_ref_obj *document;
	HashTable *prop_handler;
	zend_object std;
} dom_object;

static inline dom_object *php_dom_obj_from_obj(zend_object *obj) {
	return (dom_object*)((char*)(obj) - XtOffsetOf(dom_object, std));
}

#define Z_DOMOBJ_P(zv)  php_dom_obj_from_obj(Z_OBJ_P((zv)))

#ifdef PHP_WIN32
#	ifdef DOM_EXPORTS
#		define PHP_DOM_EXPORT __declspec(dllexport)
#	elif !defined(DOM_LOCAL_DEFINES) /* Allow to counteract LNK4049 warning. */
#		define PHP_DOM_EXPORT __declspec(dllimport)
#   else
#		define PHP_DOM_EXPORT
#	endif /* DOM_EXPORTS */
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_DOM_EXPORT __attribute__ ((visibility("default")))
#elif defined(PHPAPI)
#   define PHP_DOM_EXPORT PHPAPI
#else
#   define PHP_DOM_EXPORT
#endif

PHP_DOM_EXPORT extern zend_class_entry *dom_node_class_entry;
PHP_DOM_EXPORT dom_object *php_dom_object_get_data(xmlNodePtr obj);
PHP_DOM_EXPORT bool php_dom_create_object(xmlNodePtr obj, zval* return_value, dom_object *domobj);
PHP_DOM_EXPORT xmlNodePtr dom_object_get_node(dom_object *obj);

#define NODE_GET_OBJ(__ptr, __id, __prtype, __intern) { \
	__intern = Z_LIBXML_NODE_P(__id); \
	if (UNEXPECTED(__intern->node == NULL)) { \
		php_error_docref(NULL, E_WARNING, "Couldn't fetch %s", \
			ZSTR_VAL(Z_OBJCE_P(__zv)->name));\
		RETURN_NULL();\
	} \
	__ptr = (__prtype)__intern->node->node; \
}

#define DOC_GET_OBJ(__ptr, __id, __prtype, __intern) { \
	__intern = Z_LIBXML_NODE_P(__id); \
	if (EXPECTED(__intern->document != NULL)) { \
		__ptr = (__prtype)__intern->document->ptr); \
	} \
}

#define DOM_RET_OBJ(obj, domobject) \
	php_dom_create_object(obj, return_value, domobject)

#define DOM_GET_THIS_OBJ(__ptr, __id, __prtype, __intern) \
	__id = ZEND_THIS; \
	DOM_GET_OBJ(__ptr, __id, __prtype, __intern);

struct php_dom_private_data;
typedef struct php_dom_private_data php_dom_private_data;

static zend_always_inline php_dom_private_data *php_dom_get_private_data(dom_object *intern)
{
	ZEND_ASSERT(intern->document != NULL);
	return (php_dom_private_data *) intern->document->private_data;
}

static zend_always_inline xmlNodePtr php_dom_next_in_tree_order(const xmlNode *nodep, const xmlNode *basep)
{
	if (nodep->type == XML_ELEMENT_NODE && nodep->children) {
		return nodep->children;
	}

	if (nodep->next) {
		return nodep->next;
	} else {
		/* Go upwards, until we find a parent node with a next sibling, or until we hit the base. */
		do {
			nodep = nodep->parent;
			if (nodep == basep) {
				return NULL;
			}
			/* This shouldn't happen, unless there's an invalidation bug somewhere. */
			if (UNEXPECTED(nodep == NULL)) {
				zend_throw_error(NULL, "Current node in traversal is not in the document. Please report this as a bug in php-src.");
				return NULL;
			}
		} while (nodep->next == NULL);
		return nodep->next;
	}
}

static zend_always_inline bool php_dom_follow_spec_doc_ref(const php_libxml_ref_obj *document)
{
	return document != NULL && document->class_type == PHP_LIBXML_CLASS_MODERN;
}

static zend_always_inline bool php_dom_follow_spec_intern(const dom_object *intern)
{
	ZEND_ASSERT(intern != NULL);
	return php_dom_follow_spec_doc_ref(intern->document);
}

#endif
