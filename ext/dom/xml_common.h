#ifndef PHP_XML_COMMON_H
#define PHP_XML_COMMON_H 

typedef struct _node_list_pointer {
	xmlNodePtr nodep;
	void *next;
} node_list_pointer;

typedef struct _dom_object {
	zend_object  std;
	void *ptr;
	HashTable *prop_handler;
	node_list_pointer *node_list;
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

PHP_DOM_EXPORT(zval *) php_dom_create_object(xmlNodePtr obj, int *found, zval* in, zval* return_value TSRMLS_DC);
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
/* entry = zend_register_internal_ns_class(&ce, parent_ce, ns, NULL TSRMLS_CC); */

#define DOM_GET_OBJ(__ptr, __id, __prtype) { \
	dom_object *intern = (dom_object *)zend_object_store_get_object(__id TSRMLS_CC); \
	if (!(__ptr = (__prtype)intern->ptr)) { \
  		php_error(E_WARNING, "Couldn't fetch %s", intern->std.ce->name);\
  		RETURN_NULL();\
  	} \
}

#define DOM_DOMOBJ_NEW(zval, obj, ret) \
	if (NULL == (zval = php_dom_create_object(obj, ret, zval, return_value TSRMLS_CC))) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required DOM object"); \
		RETURN_FALSE; \
	}

#define DOM_RET_OBJ(zval, obj, ret) \
	DOM_DOMOBJ_NEW(zval, obj, ret);

#define DOM_GET_THIS(zval) \
	if (NULL == (zval = getThis())) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing"); \
		RETURN_FALSE; \
	}

#define DOM_GET_THIS_OBJ(__ptr, __id, __prtype) \
	DOM_GET_THIS(__id); \
	DOM_GET_OBJ(__ptr, __id, __prtype);

#endif
