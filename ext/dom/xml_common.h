/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Christian Stocker <chregu@php.net>                          |
  |          Rob Richards <rrichards@php.net>                            |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_XML_COMMON_H
#define PHP_XML_COMMON_H 

typedef struct _node_list_pointer {
	xmlNodePtr nodep;
	void *next;
} node_list_pointer;

typedef struct _dom_ref_obj {
	void *ptr;
	int   refcount;
	node_list_pointer *node_list;
} dom_ref_obj;

typedef struct _dom_object {
	zend_object  std;
	void *ptr;
	dom_ref_obj *document;
	HashTable *prop_handler;
	zend_object_handle handle;
} dom_object;

#ifdef PHP_WIN32
#ifdef PHPAPI
#undef PHPAPI
#endif
#ifdef DOM_EXPORTS
#define PHPAPI __declspec(dllexport)
#else
#define PHPAPI __declspec(dllimport)
#endif /* DOM_EXPORTS */
#endif /* PHP_WIN32 */

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_DOM_EXPORT(__type) PHPAPI __type

PHP_DOM_EXPORT(zval *) php_dom_create_object(xmlNodePtr obj, int *found, zval *in, zval* return_value, dom_object *domobj TSRMLS_DC);
PHP_DOM_EXPORT(void) dom_objects_clone(void *object, void **object_clone TSRMLS_DC);
void dom_objects_dtor(void *object, zend_object_handle handle TSRMLS_DC);
PHP_DOM_EXPORT(zval *) dom_read_property(zval *object, zval *member TSRMLS_DC);
PHP_DOM_EXPORT(void) dom_write_property(zval *object, zval *member, zval *value TSRMLS_DC);
zend_object_value dom_objects_new(zend_class_entry *class_type TSRMLS_DC);
void dom_unregister_node(xmlNodePtr nodep TSRMLS_DC);
zend_object_handlers dom_object_handlers;

#define DOM_XMLNS_NAMESPACE \
    (const xmlChar *) "http://www.w3.org/2000/xmlns/"

#define DOM_NOT_IMPLEMENTED() \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not yet implemented"); \
	return;
    
#define REGISTER_DOM_CLASS(ce, name, parent_ce, funcs, entry) \
INIT_CLASS_ENTRY(ce, name, funcs); \
ce.create_object = dom_objects_new; \
entry = zend_register_internal_class_ex(&ce, parent_ce, NULL TSRMLS_CC);

#define DOM_GET_OBJ(__ptr, __id, __prtype, __intern) { \
	__intern = (dom_object *)zend_object_store_get_object(__id TSRMLS_CC); \
	if (!(__ptr = (__prtype)__intern->ptr)) { \
  		php_error(E_WARNING, "Couldn't fetch %s", __intern->std.ce->name);\
  		RETURN_NULL();\
  	} \
}

#define DOM_DOMOBJ_NEW(zval, obj, ret, domobject) \
	if (NULL == (zval = php_dom_create_object(obj, ret, zval, return_value, domobject TSRMLS_CC))) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required DOM object"); \
		RETURN_FALSE; \
	}

#define DOM_RET_OBJ(zval, obj, ret, domobject) \
	DOM_DOMOBJ_NEW(zval, obj, ret, domobject);

#define DOM_GET_THIS(zval) \
	if (NULL == (zval = getThis())) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing"); \
		RETURN_FALSE; \
	}

#define DOM_GET_THIS_OBJ(__ptr, __id, __prtype, __intern) \
	DOM_GET_THIS(__id); \
	DOM_GET_OBJ(__ptr, __id, __prtype, __intern);

#endif
