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
#include <config.h>
#endif

#include "php.h"

#if defined(HAVE_LIBXML) && defined(HAVE_DOM)

#include "php_dom.h"
#include "dom_properties.h"
#include "internal_helpers.h"

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

	name_valid = xmlValidateName(BAD_CAST name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, true);
		RETURN_THROWS();
	}

	nodep = xmlNewProp(NULL, BAD_CAST name, BAD_CAST value);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
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
Modern spec URL: https://dom.spec.whatwg.org/#dom-attr-name
Since:
*/
zend_result dom_attr_name_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlAttrPtr, attrp, obj);

	if (php_dom_follow_spec_intern(obj)) {
		zend_string *str = dom_node_get_node_name_attribute_or_element((xmlNodePtr) attrp, false);
		ZVAL_NEW_STR(retval, str);
	} else {
		ZVAL_STRING(retval, (char *) attrp->name);
	}

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
	/* From spec: "useless; always returns true" */
	ZVAL_TRUE(retval);
	return SUCCESS;
}

/* }}} */

void dom_attr_value_will_change(dom_object *obj, xmlAttrPtr attrp)
{
	if (attrp->atype == XML_ATTRIBUTE_ID) {
		xmlRemoveID(attrp->doc, attrp);
		attrp->atype = XML_ATTRIBUTE_ID;
	}

	dom_mark_ids_modified(obj->document);
}

/* {{{ value	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-221662474
Since:
*/
zend_result dom_attr_value_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlNodePtr, attrp, obj);
	php_dom_get_content_into_zval(attrp, retval, false);
	return SUCCESS;
}

zend_result dom_attr_value_write(dom_object *obj, zval *newval)
{
	DOM_PROP_NODE(xmlAttrPtr, attrp, obj);

	dom_attr_value_will_change(obj, attrp);

	/* Typed property, this is already a string */
	ZEND_ASSERT(Z_TYPE_P(newval) == IS_STRING);
	zend_string *str = Z_STR_P(newval);

	dom_remove_all_children((xmlNodePtr) attrp);

	if (php_dom_follow_spec_intern(obj)) {
		xmlNodePtr node = xmlNewDocTextLen(attrp->doc, BAD_CAST ZSTR_VAL(str), ZSTR_LEN(str));
		xmlAddChild((xmlNodePtr) attrp, node);
	} else {
		xmlNodeSetContentLen((xmlNodePtr) attrp, BAD_CAST ZSTR_VAL(str), ZSTR_LEN(str));
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
	DOM_PROP_NODE(xmlNodePtr, nodep, obj);

	xmlNodePtr nodeparent = nodep->parent;

	php_dom_create_nullable_object(nodeparent, retval, obj);
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
	/* This was never implemented, and is dropped from the modern-day DOM spec.
	 * It only exists in old DOM to preserve BC. */
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Attr-isId
Since: DOM Level 3
*/
PHP_METHOD(DOMAttr, isId)
{
	dom_object *intern;
	xmlAttrPtr attrp;

	ZEND_PARSE_PARAMETERS_NONE();
	DOM_GET_OBJ(attrp, ZEND_THIS, xmlAttrPtr, intern);
	RETURN_BOOL(attrp->atype == XML_ATTRIBUTE_ID);
}
/* }}} end dom_attr_is_id */

bool dom_compare_value(const xmlAttr *attr, const xmlChar *value)
{
	bool free;
	xmlChar *attr_value = php_libxml_attr_value(attr, &free);
	bool result = xmlStrEqual(attr_value, value);
	if (free) {
		xmlFree(attr_value);
	}
	return result;
}

#endif
