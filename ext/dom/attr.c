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
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_dom.h"


/*
* class domattr extends domnode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-637646024
* Since: 
*/

zend_function_entry php_dom_attr_class_functions[] = {
	PHP_FALIAS(isId, dom_attr_is_id, NULL)
	PHP_FALIAS(domattr, dom_attr_attr, NULL)
	{NULL, NULL, NULL}
};

/* {{{ proto domnode dom_attr_attr(string name, [string value]); */
PHP_FUNCTION(dom_attr_attr)
{

	zval *id;
	xmlAttrPtr nodep = NULL;
	xmlNodePtr oldnode = NULL;
	dom_object *intern;
	char *name, *value = NULL;
	int name_len, value_len;

	id = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	if (name_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attribute name is required");
		RETURN_FALSE;
	}

	nodep = xmlNewProp(NULL, (xmlChar *) name, value);

	if (!nodep)
		RETURN_FALSE;

	intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL) {
		oldnode = (xmlNodePtr)intern->ptr;
		if (oldnode != NULL) {
			node_free_resource(oldnode  TSRMLS_CC);
		}
		php_dom_set_object(id, (xmlNodePtr) nodep TSRMLS_CC);
	}
}

/* }}} end dom_attr_attr */


/* {{{ proto name	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1112119403
Since: 
*/
int dom_attr_name_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlAttrPtr attrp;

	attrp = obj->ptr;
	ALLOC_ZVAL(*retval);
	ZVAL_STRING(*retval, (char *) (attrp->name), 1);

	return SUCCESS;
}

/* }}} */



/* {{{ proto specified	boolean	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-862529273
Since: 
*/
int dom_attr_specified_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	/* TODO */
	ALLOC_ZVAL(*retval);
	ZVAL_TRUE(*retval);
	return SUCCESS;
}

/* }}} */



/* {{{ proto value	string	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-221662474
Since: 
*/
int dom_attr_value_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlAttrPtr attrp;
	xmlChar *content;

	attrp = obj->ptr;

	ALLOC_ZVAL(*retval);

	
	if ((content = xmlNodeGetContent((xmlNodePtr) attrp)) != NULL) {
		ZVAL_STRING(*retval, content, 1);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}

	xmlFree(content);

	return SUCCESS;

}

int dom_attr_value_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	xmlAttrPtr attrp;

	attrp = obj->ptr;

	if (attrp->children) {
		node_list_unlink(attrp->children TSRMLS_CC);
	}
	xmlNodeSetContentLen((xmlNodePtr) attrp, Z_STRVAL_P(newval), Z_STRLEN_P(newval) + 1);

	return SUCCESS;
}

/* }}} */



/* {{{ proto ownerElement	element	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Attr-ownerElement
Since: DOM Level 2
*/
int dom_attr_owner_element_read(dom_object *obj, zval **retval TSRMLS_DC)
{

	zval *wrapper;
	xmlNodePtr nodep, nodeparent;
	int ret;

	nodep = obj->ptr;

	nodeparent = nodep->parent;
	if (!nodeparent) {
		return FAILURE;
	}

	wrapper = dom_object_get_data(nodeparent);
	if (wrapper == NULL) {
		ALLOC_ZVAL(*retval);
	}
	if (NULL == (*retval = php_dom_create_object(nodeparent, &ret, wrapper, *retval TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;

}

/* }}} */



/* {{{ proto schemaTypeInfo	typeinfo	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Attr-schemaTypeInfo
Since: DOM Level 3
*/
int dom_attr_schema_type_info_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not yet implemented");
	ALLOC_ZVAL(*retval);
	ZVAL_NULL(*retval);
	return SUCCESS;
}

/* }}} */




/* {{{ proto boolean dom_attr_is_id();
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Attr-isId
Since: DOM Level 3
*/
PHP_FUNCTION(dom_attr_is_id)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_attr_is_id */
