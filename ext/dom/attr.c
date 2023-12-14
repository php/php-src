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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if defined(HAVE_LIBXML) && defined(HAVE_DOM)

#include "php_dom.h"

/*
* class DOMAttr extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-637646024
* Since:
*/

/* {{{ */
PHP_METHOD(DOMAttr, __construct)
{
	xmlAttrPtr nodep = NULL;
	xmlNodePtr oldnode = NULL;
	dom_object *intern;
	char *name, *value = NULL;
	size_t name_len, value_len, name_valid;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &name, &name_len, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);

	name_valid = xmlValidateName((xmlChar *) name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, 1);
		RETURN_THROWS();
	}

	// TODO: what to do with entity subtitution here?
	nodep = xmlNewProp(NULL, (xmlChar *) name, (xmlChar *) value);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		RETURN_THROWS();
	}

	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_decrement_resource((php_libxml_node_object *)intern);
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)nodep, (void *)intern);
}

/* }}} end DOMAttr::__construct */

/* {{{ name	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1112119403
Since:
*/
zend_result dom_attr_name_read(dom_object *obj, zval *retval)
{
	xmlAttrPtr attrp;

	attrp = (xmlAttrPtr) dom_object_get_node(obj);

	if (attrp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
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
zend_result dom_attr_specified_read(dom_object *obj, zval *retval)
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
zend_result dom_attr_value_read(dom_object *obj, zval *retval)
{
	xmlAttrPtr attrp = (xmlAttrPtr) dom_object_get_node(obj);
	xmlChar *content;

	if (attrp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	/* Can't avoid a content copy because it's an attribute node */
	if ((content = xmlNodeGetContent((xmlNodePtr) attrp)) != NULL) {
		ZVAL_STRING(retval, (char *) content);
		xmlFree(content);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;

}

zend_result dom_attr_value_write(dom_object *obj, zval *newval)
{
	xmlAttrPtr attrp = (xmlAttrPtr) dom_object_get_node(obj);

	if (attrp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	/* Typed property, this is already a string */
	ZEND_ASSERT(Z_TYPE_P(newval) == IS_STRING);
	zend_string *str = Z_STR_P(newval);

	dom_remove_all_children((xmlNodePtr) attrp);

	if (php_dom_follow_spec_intern(obj)) {
		xmlNodePtr node = xmlNewTextLen((xmlChar *) ZSTR_VAL(str), ZSTR_LEN(str));
		xmlAddChild((xmlNodePtr) attrp, node);
	} else {
		xmlNodeSetContentLen((xmlNodePtr) attrp, (xmlChar *) ZSTR_VAL(str), ZSTR_LEN(str));
	}

	return SUCCESS;
}

/* }}} */

/* {{{ ownerElement	DOMElement
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Attr-ownerElement
Since: DOM Level 2
*/
zend_result dom_attr_owner_element_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep, nodeparent;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
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
zend_result dom_attr_schema_type_info_read(dom_object *obj, zval *retval)
{
	/* TODO */
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Attr-isId
Since: DOM Level 3
*/
PHP_METHOD(DOMAttr, isId)
{
	zval *id;
	dom_object *intern;
	xmlAttrPtr attrp;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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
