/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_LIBXML && HAVE_DOM

#include "php_dom.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_attr_is_id, 0, 0, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_attr_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class DOMAttr extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-637646024
* Since:
*/

const zend_function_entry php_dom_attr_class_functions[] = {
	PHP_FALIAS(isId, dom_attr_is_id, arginfo_dom_attr_is_id)
	PHP_ME(domattr, __construct, arginfo_dom_attr_construct, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/* {{{ proto DOMAttr::__construct(string name, [string value]) */
PHP_METHOD(domattr, __construct)
{
	xmlAttrPtr nodep = NULL;
	xmlNodePtr oldnode = NULL;
	dom_object *intern;
	char *name, *value = NULL;
	size_t name_len, value_len, name_valid;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s|s", &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);

	name_valid = xmlValidateName((xmlChar *) name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, 1);
		RETURN_FALSE;
	}

	nodep = xmlNewProp(NULL, (xmlChar *) name, (xmlChar *) value);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		RETURN_FALSE;
	}

	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_free_resource(oldnode );
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)nodep, (void *)intern);
}

/* }}} end DOMAttr::__construct */

/* {{{ name	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1112119403
Since:
*/
int dom_attr_name_read(dom_object *obj, zval *retval)
{
	xmlAttrPtr attrp;

	attrp = (xmlAttrPtr) dom_object_get_node(obj);

	if (attrp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	ZVAL_STRING(retval, (char *) attrp->name);

	return SUCCESS;
}

/* }}} */

/* {{{ specified	boolean
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-862529273
Since:
*/
int dom_attr_specified_read(dom_object *obj, zval *retval)
{
	/* TODO */
	ZVAL_TRUE(retval);
	return SUCCESS;
}

/* }}} */

/* {{{ value	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-221662474
Since:
*/
int dom_attr_value_read(dom_object *obj, zval *retval)
{
	xmlAttrPtr attrp = (xmlAttrPtr) dom_object_get_node(obj);
	xmlChar *content;

	if (attrp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	if ((content = xmlNodeGetContent((xmlNodePtr) attrp)) != NULL) {
		ZVAL_STRING(retval, (char *) content);
		xmlFree(content);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;

}

int dom_attr_value_write(dom_object *obj, zval *newval)
{
	zend_string *str;
	xmlAttrPtr attrp = (xmlAttrPtr) dom_object_get_node(obj);

	if (attrp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	str = zval_try_get_string(newval);
	if (UNEXPECTED(!str)) {
		return FAILURE;
	}

	if (attrp->children) {
		node_list_unlink(attrp->children);
	}

	xmlNodeSetContentLen((xmlNodePtr) attrp, (xmlChar *) ZSTR_VAL(str), ZSTR_LEN(str) + 1);

	zend_string_release_ex(str, 0);
	return SUCCESS;
}

/* }}} */

/* {{{ ownerElement	DOMElement
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Attr-ownerElement
Since: DOM Level 2
*/
int dom_attr_owner_element_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep, nodeparent;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	nodeparent = nodep->parent;
	if (!nodeparent) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(nodeparent, retval, obj);
	return SUCCESS;

}

/* }}} */

/* {{{ schemaTypeInfo	DOMTypeInfo
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Attr-schemaTypeInfo
Since: DOM Level 3
*/
int dom_attr_schema_type_info_read(dom_object *obj, zval *retval)
{
	/* TODO */
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* {{{ proto bool dom_attr_is_id()
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Attr-isId
Since: DOM Level 3
*/
PHP_FUNCTION(dom_attr_is_id)
{
	zval *id;
	dom_object *intern;
	xmlAttrPtr attrp;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	DOM_GET_OBJ(attrp, id, xmlAttrPtr, intern);

	if (attrp->atype == XML_ATTRIBUTE_ID) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_attr_is_id */

#endif
