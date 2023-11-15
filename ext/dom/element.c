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
* class DOMElement extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-745549614
* Since:
*/

/* {{{ */
PHP_METHOD(DOMElement, __construct)
{
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *name, *value = NULL, *uri = NULL;
	char *localname = NULL, *prefix = NULL;
	int errorcode = 0;
	size_t name_len, value_len = 0, uri_len = 0;
	int name_valid;
	xmlNsPtr nsptr = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!s", &name, &name_len, &value, &value_len, &uri, &uri_len) == FAILURE) {
		RETURN_THROWS();
	}

	name_valid = xmlValidateName((xmlChar *) name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, 1);
		RETURN_THROWS();
	}

	/* Namespace logic is separate and only when uri passed in to insure no BC breakage */
	if (uri_len > 0) {
		errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);
		if (errorcode == 0) {
			nodep = xmlNewNode (NULL, (xmlChar *)localname);
			if (nodep != NULL && uri != NULL) {
				nsptr = dom_get_ns(nodep, uri, &errorcode, prefix);
				xmlSetNs(nodep, nsptr);
			}
		}
		xmlFree(localname);
		if (prefix != NULL) {
			xmlFree(prefix);
		}
		if (errorcode != 0) {
			if (nodep != NULL) {
				xmlFreeNode(nodep);
			}
			php_dom_throw_error(errorcode, 1);
			RETURN_THROWS();
		}
	} else {
	    /* If you don't pass a namespace uri, then you can't set a prefix */
	    localname = (char *) xmlSplitQName2((xmlChar *) name, (xmlChar **) &prefix);
	    if (prefix != NULL) {
			xmlFree(localname);
			xmlFree(prefix);
	        php_dom_throw_error(NAMESPACE_ERR, 1);
	        RETURN_THROWS();
	    }
		nodep = xmlNewNode(NULL, (xmlChar *) name);
	}

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		RETURN_THROWS();
	}

	if (value_len > 0) {
		xmlNodeSetContentLen(nodep, (xmlChar *) value, value_len);
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);
	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_decrement_resource((php_libxml_node_object *)intern);
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern);
}
/* }}} end DOMElement::__construct */

/* {{{ tagName	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-104682815
Since:
*/
int dom_element_tag_name_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep;
	xmlNsPtr ns;
	xmlChar *qname;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	ns = nodep->ns;
	if (ns != NULL && ns->prefix) {
		qname = xmlStrdup(ns->prefix);
		qname = xmlStrcat(qname, (xmlChar *)":");
		qname = xmlStrcat(qname, nodep->name);
		ZVAL_STRING(retval, (char *)qname);
		xmlFree(qname);
	} else {
		ZVAL_STRING(retval, (char *) nodep->name);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ schemaTypeInfo	typeinfo
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Element-schemaTypeInfo
Since: DOM Level 3
*/
int dom_element_schema_type_info_read(dom_object *obj, zval *retval)
{
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* Note: the object returned is not necessarily a node, but can be an attribute or a namespace declaration. */
static xmlNodePtr dom_get_dom1_attribute(xmlNodePtr elem, xmlChar *name) /* {{{ */
{
	int len;
	const xmlChar *nqname;

	nqname = xmlSplitQName3(name, &len);
	if (nqname != NULL) {
		xmlNsPtr ns;
		xmlChar *prefix = xmlStrndup(name, len);
		if (prefix && xmlStrEqual(prefix, (xmlChar *)"xmlns")) {
			ns = elem->nsDef;
			while (ns) {
				if (xmlStrEqual(ns->prefix, nqname)) {
					break;
				}
				ns = ns->next;
			}
			xmlFree(prefix);
			return (xmlNodePtr)ns;
		}
		ns = xmlSearchNs(elem->doc, elem, prefix);
		if (prefix != NULL) {
			xmlFree(prefix);
		}
		if (ns != NULL) {
			return (xmlNodePtr)xmlHasNsProp(elem, nqname, ns->href);
		}
	} else {
		if (xmlStrEqual(name, (xmlChar *)"xmlns")) {
			xmlNsPtr nsPtr = elem->nsDef;
			while (nsPtr) {
				if (nsPtr->prefix == NULL) {
					return (xmlNodePtr)nsPtr;
				}
				nsPtr = nsPtr->next;
			}
			return NULL;
		}
	}
	return (xmlNodePtr)xmlHasNsProp(elem, name, NULL);
}
/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-666EE0F9
Since:
*/
PHP_METHOD(DOMElement, getAttribute)
{
	zval *id;
	xmlNode *nodep;
	char *name;
	xmlChar *value = NULL;
	dom_object *intern;
	xmlNodePtr attr;
	size_t name_len;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	attr = dom_get_dom1_attribute(nodep, (xmlChar *)name);
	if (attr) {
		switch (attr->type) {
			case XML_ATTRIBUTE_NODE:
				value = xmlNodeListGetString(attr->doc, attr->children, 1);
				break;
			case XML_NAMESPACE_DECL:
				value = xmlStrdup(((xmlNsPtr)attr)->href);
				break;
			default:
				value = xmlStrdup(((xmlAttributePtr)attr)->defaultValue);
		}
	}

	if (value == NULL) {
		RETURN_EMPTY_STRING();
	} else {
		RETVAL_STRING((char *)value);
		xmlFree(value);
	}
}
/* }}} end dom_element_get_attribute */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-F68F082
Since:
*/
PHP_METHOD(DOMElement, setAttribute)
{
	zval *id;
	xmlNode *nodep;
	xmlNodePtr attr = NULL;
	int ret, name_valid;
	size_t name_len, value_len;
	dom_object *intern;
	char *name, *value;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &name, &name_len, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (name_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	name_valid = xmlValidateName((xmlChar *) name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, 1);
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	attr = dom_get_dom1_attribute(nodep, (xmlChar *)name);
	if (attr != NULL) {
		switch (attr->type) {
			case XML_ATTRIBUTE_NODE:
				node_list_unlink(attr->children);
				break;
			case XML_NAMESPACE_DECL:
				RETURN_FALSE;
			default:
				break;
		}

	}

	if (xmlStrEqual((xmlChar *)name, (xmlChar *)"xmlns")) {
		if (xmlNewNs(nodep, (xmlChar *)value, NULL)) {
			RETURN_TRUE;
		}
	} else {
		attr = (xmlNodePtr)xmlSetProp(nodep, (xmlChar *) name, (xmlChar *)value);
	}
	if (!attr) {
		zend_argument_value_error(1, "must be a valid XML attribute");
		RETURN_THROWS();
	}

	DOM_RET_OBJ(attr, &ret, intern);

}
/* }}} end dom_element_set_attribute */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6D6AC0F9
Since:
*/
PHP_METHOD(DOMElement, removeAttribute)
{
	zval *id;
	xmlNodePtr nodep, attrp;
	dom_object *intern;
	size_t name_len;
	char *name;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	attrp = dom_get_dom1_attribute(nodep, (xmlChar *)name);
	if (attrp == NULL) {
		RETURN_FALSE;
	}

	switch (attrp->type) {
		case XML_ATTRIBUTE_NODE:
			if (php_dom_object_get_data(attrp) == NULL) {
			node_list_unlink(attrp->children);
				xmlUnlinkNode(attrp);
				xmlFreeProp((xmlAttrPtr)attrp);
		} else {
				xmlUnlinkNode(attrp);
		}
			break;
		case XML_NAMESPACE_DECL:
			RETURN_FALSE;
		default:
			break;
	}

	RETURN_TRUE;
}
/* }}} end dom_element_remove_attribute */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-217A91B8
Since:
*/
PHP_METHOD(DOMElement, getAttributeNode)
{
	zval *id;
	xmlNodePtr nodep, attrp;
	size_t name_len;
	int ret;
	dom_object *intern;
	char *name;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	attrp = dom_get_dom1_attribute(nodep, (xmlChar *)name);
	if (attrp == NULL) {
		RETURN_FALSE;
	}

	if (attrp->type == XML_NAMESPACE_DECL) {
		xmlNsPtr original = (xmlNsPtr) attrp;
		/* Keep parent alive, because we're a fake child. */
		GC_ADDREF(&intern->std);
		(void) php_dom_create_fake_namespace_decl(nodep, original, return_value, intern);
	} else {
		DOM_RET_OBJ((xmlNodePtr) attrp, &ret, intern);
	}
}
/* }}} end dom_element_get_attribute_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-887236154
Since:
*/
PHP_METHOD(DOMElement, setAttributeNode)
{
	zval *id, *node;
	xmlNode *nodep;
	xmlAttr *attrp, *existattrp = NULL;
	dom_object *intern, *attrobj, *oldobj;
	int ret;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node, dom_attr_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	DOM_GET_OBJ(attrp, node, xmlAttrPtr, attrobj);

	if (attrp->type != XML_ATTRIBUTE_NODE) {
		zend_argument_value_error(1, "must have the node attribute");
		RETURN_THROWS();
	}

	if (!(attrp->doc == NULL || attrp->doc == nodep->doc)) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	existattrp = xmlHasProp(nodep, attrp->name);
	if (existattrp != NULL && existattrp->type != XML_ATTRIBUTE_DECL) {
		if ((oldobj = php_dom_object_get_data((xmlNodePtr) existattrp)) != NULL &&
			((php_libxml_node_ptr *)oldobj->ptr)->node == (xmlNodePtr) attrp)
		{
			RETURN_NULL();
		}
		xmlUnlinkNode((xmlNodePtr) existattrp);
	}

	if (attrp->parent != NULL) {
		xmlUnlinkNode((xmlNodePtr) attrp);
	}

	if (attrp->doc == NULL && nodep->doc != NULL) {
		attrobj->document = intern->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)attrobj, NULL);
	}

	xmlAddChild(nodep, (xmlNodePtr) attrp);

	/* Returns old property if removed otherwise NULL */
	if (existattrp != NULL) {
		DOM_RET_OBJ((xmlNodePtr) existattrp, &ret, intern);
	} else {
		RETVAL_NULL();
	}

}
/* }}} end dom_element_set_attribute_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D589198
Since:
*/
PHP_METHOD(DOMElement, removeAttributeNode)
{
	zval *id, *node;
	xmlNode *nodep;
	xmlAttr *attrp;
	dom_object *intern, *attrobj;
	int ret;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node, dom_attr_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	DOM_GET_OBJ(attrp, node, xmlAttrPtr, attrobj);

	if (attrp->type != XML_ATTRIBUTE_NODE || attrp->parent != nodep) {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	xmlUnlinkNode((xmlNodePtr) attrp);

	DOM_RET_OBJ((xmlNodePtr) attrp, &ret, intern);

}
/* }}} end dom_element_remove_attribute_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1938918D
Since:
*/
PHP_METHOD(DOMElement, getElementsByTagName)
{
	zval *id;
	xmlNodePtr elemp;
	size_t name_len;
	dom_object *intern, *namednode;
	char *name;
	xmlChar *local;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	php_dom_create_iterator(return_value, DOM_NODELIST);
	namednode = Z_DOMOBJ_P(return_value);
	local = xmlCharStrndup(name, name_len);
	dom_namednode_iter(intern, 0, namednode, NULL, local, NULL);
}
/* }}} end dom_element_get_elements_by_tag_name */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElGetAttrNS
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, getAttributeNS)
{
	zval *id;
	xmlNodePtr elemp;
	xmlNsPtr nsptr;
	dom_object *intern;
	size_t uri_len = 0, name_len = 0;
	char *uri, *name;
	xmlChar *strattr;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	strattr = xmlGetNsProp(elemp, (xmlChar *) name, (xmlChar *) uri);

	if (strattr != NULL) {
		RETVAL_STRING((char *)strattr);
		xmlFree(strattr);
	} else {
		if (xmlStrEqual((xmlChar *) uri, (xmlChar *)DOM_XMLNS_NAMESPACE)) {
			nsptr = dom_get_nsdecl(elemp, (xmlChar *)name);
			if (nsptr != NULL) {
				RETVAL_STRING((char *) nsptr->href);
			} else {
				RETVAL_EMPTY_STRING();
			}
		} else {
			RETVAL_EMPTY_STRING();
		}
	}

}
/* }}} end dom_element_get_attribute_ns */

static xmlNsPtr _dom_new_reconNs(xmlDocPtr doc, xmlNodePtr tree, xmlNsPtr ns) /* {{{ */
{
	xmlNsPtr def;
	xmlChar prefix[50];
	int counter = 1;

	if ((tree == NULL) || (ns == NULL) || (ns->type != XML_NAMESPACE_DECL)) {
		return NULL;
	}

	/* Code taken from libxml2 (2.6.20) xmlNewReconciliedNs
	 *
	 * Find a close prefix which is not already in use.
	 * Let's strip namespace prefixes longer than 20 chars !
	 */
	if (ns->prefix == NULL)
		snprintf((char *) prefix, sizeof(prefix), "default");
	else
		snprintf((char *) prefix, sizeof(prefix), "%.20s", (char *)ns->prefix);

	def = xmlSearchNs(doc, tree, prefix);
	while (def != NULL) {
		if (counter > 1000) return(NULL);
		if (ns->prefix == NULL)
			snprintf((char *) prefix, sizeof(prefix), "default%d", counter++);
		else
			snprintf((char *) prefix, sizeof(prefix), "%.20s%d",
			(char *)ns->prefix, counter++);
		def = xmlSearchNs(doc, tree, prefix);
	}

	/*
	 * OK, now we are ready to create a new one.
	 */
	def = xmlNewNs(tree, ns->href, prefix);
	return(def);
}
/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetAttrNS
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, setAttributeNS)
{
	zval *id;
	xmlNodePtr elemp, nodep = NULL;
	xmlNsPtr nsptr;
	xmlAttr *attr;
	size_t uri_len = 0, name_len = 0, value_len = 0;
	char *uri, *name, *value;
	char *localname = NULL, *prefix = NULL;
	dom_object *intern;
	int errorcode = 0, stricterror, is_xmlns = 0, name_valid;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!ss", &uri, &uri_len, &name, &name_len, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (name_len == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(elemp) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		RETURN_NULL();
	}

	errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);

	if (errorcode == 0) {
		if (uri_len > 0) {
			nodep = (xmlNodePtr) xmlHasNsProp(elemp, (xmlChar *) localname, (xmlChar *) uri);
			if (nodep != NULL && nodep->type != XML_ATTRIBUTE_DECL) {
				node_list_unlink(nodep->children);
			}

			if ((xmlStrEqual((xmlChar *) prefix, (xmlChar *)"xmlns") ||
				(prefix == NULL && xmlStrEqual((xmlChar *) localname, (xmlChar *)"xmlns"))) &&
				xmlStrEqual((xmlChar *) uri, (xmlChar *)DOM_XMLNS_NAMESPACE)) {
				is_xmlns = 1;
				if (prefix == NULL) {
					nsptr = dom_get_nsdecl(elemp, NULL);
				} else {
					nsptr = dom_get_nsdecl(elemp, (xmlChar *)localname);
				}
			} else {
				nsptr = xmlSearchNsByHref(elemp->doc, elemp, (xmlChar *)uri);
				if (nsptr && nsptr->prefix == NULL) {
					xmlNsPtr tmpnsptr;

					tmpnsptr = nsptr->next;
					while (tmpnsptr) {
						if ((tmpnsptr->prefix != NULL) && (tmpnsptr->href != NULL) &&
							(xmlStrEqual(tmpnsptr->href, (xmlChar *) uri))) {
							nsptr = tmpnsptr;
							break;
						}
						tmpnsptr = tmpnsptr->next;
					}
					if (tmpnsptr == NULL) {
						nsptr = _dom_new_reconNs(elemp->doc, elemp, nsptr);
					}
				}
			}

			if (nsptr == NULL) {
				if (prefix == NULL) {
					if (is_xmlns == 1) {
						xmlNewNs(elemp, (xmlChar *)value, NULL);
						xmlReconciliateNs(elemp->doc, elemp);
					} else {
						errorcode = NAMESPACE_ERR;
					}
				} else {
					if (is_xmlns == 1) {
						xmlNewNs(elemp, (xmlChar *)value, (xmlChar *)localname);
					} else {
						nsptr = dom_get_ns(elemp, uri, &errorcode, prefix);
					}
					xmlReconciliateNs(elemp->doc, elemp);
				}
			} else {
				if (is_xmlns == 1) {
					if (nsptr->href) {
						xmlFree((xmlChar *) nsptr->href);
					}
					nsptr->href = xmlStrdup((xmlChar *)value);
				}
			}

			if (errorcode == 0 && is_xmlns == 0) {
				xmlSetNsProp(elemp, nsptr, (xmlChar *)localname, (xmlChar *)value);
			}
		} else {
			name_valid = xmlValidateName((xmlChar *) localname, 0);
			if (name_valid != 0) {
				errorcode = INVALID_CHARACTER_ERR;
				stricterror = 1;
			} else {
				attr = xmlHasProp(elemp, (xmlChar *)localname);
				if (attr != NULL && attr->type != XML_ATTRIBUTE_DECL) {
					node_list_unlink(attr->children);
				}
				xmlSetProp(elemp, (xmlChar *)localname, (xmlChar *)value);
			}
		}
	}

	xmlFree(localname);
	if (prefix != NULL) {
		xmlFree(prefix);
	}

	if (errorcode != 0) {
		php_dom_throw_error(errorcode, stricterror);
	}

	RETURN_NULL();
}
/* }}} end dom_element_set_attribute_ns */

static void dom_remove_eliminated_ns_single_element(xmlNodePtr node, xmlNsPtr eliminatedNs)
{
	ZEND_ASSERT(node->type == XML_ELEMENT_NODE);
	if (node->ns == eliminatedNs) {
		node->ns = NULL;
	}

	for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
		if (attr->ns == eliminatedNs) {
			attr->ns = NULL;
		}
	}
}

static void dom_remove_eliminated_ns(xmlNodePtr node, xmlNsPtr eliminatedNs)
{
	dom_remove_eliminated_ns_single_element(node, eliminatedNs);

	xmlNodePtr base = node;
	node = node->children;
	while (node != NULL) {
		ZEND_ASSERT(node != base);

		if (node->type == XML_ELEMENT_NODE) {
			dom_remove_eliminated_ns_single_element(node, eliminatedNs);

			if (node->children) {
				node = node->children;
				continue;
			}
		}

		if (node->next) {
			node = node->next;
		} else {
			/* Go upwards, until we find a parent node with a next sibling, or until we hit the base. */
			do {
				node = node->parent;
				if (node == base) {
					return;
				}
			} while (node->next == NULL);
			node = node->next;
		}
	}
}

static void dom_eliminate_ns(xmlNodePtr nodep, xmlNsPtr nsptr)
{
	if (nsptr->href != NULL) {
		xmlFree((char *) nsptr->href);
		nsptr->href = NULL;
	}
	if (nsptr->prefix != NULL) {
		xmlFree((char *) nsptr->prefix);
		nsptr->prefix = NULL;
	}

	/* Remove it from the list and move it to the old ns list */
	xmlNsPtr current_ns = nodep->nsDef;
	if (current_ns == nsptr) {
		nodep->nsDef = nsptr->next;
	} else {
		do {
			if (current_ns->next == nsptr) {
				current_ns->next = nsptr->next;
				break;
			}
			current_ns = current_ns->next;
		} while (current_ns != NULL);
	}
	nsptr->next = NULL;
	dom_set_old_ns(nodep->doc, nsptr);

	dom_remove_eliminated_ns(nodep, nsptr);
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElRemAtNS
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, removeAttributeNS)
{
	zval *id;
	xmlNode *nodep;
	xmlAttr *attrp;
	xmlNsPtr nsptr;
	dom_object *intern;
	size_t name_len, uri_len;
	char *name, *uri;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_NULL();
	}

	attrp = xmlHasNsProp(nodep, (xmlChar *)name, (xmlChar *)uri);

	nsptr = dom_get_nsdecl(nodep, (xmlChar *)name);
	if (nsptr != NULL) {
		if (xmlStrEqual((xmlChar *)uri, nsptr->href)) {
			dom_eliminate_ns(nodep, nsptr);
		} else {
			RETURN_NULL();
		}
	}

	if (attrp && attrp->type != XML_ATTRIBUTE_DECL) {
		if (php_dom_object_get_data((xmlNodePtr) attrp) == NULL) {
			node_list_unlink(attrp->children);
			xmlUnlinkNode((xmlNodePtr) attrp);
			xmlFreeProp(attrp);
		} else {
			xmlUnlinkNode((xmlNodePtr) attrp);
		}
	}

	RETURN_NULL();
}
/* }}} end dom_element_remove_attribute_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElGetAtNodeNS
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, getAttributeNodeNS)
{
	zval *id;
	xmlNodePtr elemp;
	xmlAttrPtr attrp;
	dom_object *intern;
	size_t uri_len, name_len;
	int ret;
	char *uri, *name;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	attrp = xmlHasNsProp(elemp, (xmlChar *)name, (xmlChar *)uri);

	if (attrp == NULL) {
		if (xmlStrEqual((xmlChar *) uri, (xmlChar *)DOM_XMLNS_NAMESPACE)) {
			xmlNsPtr nsptr;
			nsptr = dom_get_nsdecl(elemp, (xmlChar *)name);
			if (nsptr != NULL) {
				/* Keep parent alive, because we're a fake child. */
				GC_ADDREF(&intern->std);
				(void) php_dom_create_fake_namespace_decl(elemp, nsptr, return_value, intern);
			} else {
				RETURN_NULL();
			}
		} else {
		   RETURN_NULL();
		}
	} else {
		DOM_RET_OBJ((xmlNodePtr) attrp, &ret, intern);
	}

}
/* }}} end dom_element_get_attribute_node_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetAtNodeNS
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, setAttributeNodeNS)
{
	zval *id, *node;
	xmlNode *nodep;
	xmlNs *nsp;
	xmlAttr *attrp, *existattrp = NULL;
	dom_object *intern, *attrobj, *oldobj;
	int ret;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node, dom_attr_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	DOM_GET_OBJ(attrp, node, xmlAttrPtr, attrobj);

	/* ZPP Guarantees that a DOMAttr class is given, as it is converted to a xmlAttr
	 * to pass to libxml (see http://www.xmlsoft.org/html/libxml-tree.html#xmlAttr)
	 * if it is not of type XML_ATTRIBUTE_NODE it indicates a bug somewhere */
	ZEND_ASSERT(attrp->type == XML_ATTRIBUTE_NODE);

	if (!(attrp->doc == NULL || attrp->doc == nodep->doc)) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	nsp = attrp->ns;
	if (nsp != NULL) {
		existattrp = xmlHasNsProp(nodep, attrp->name, nsp->href);
	} else {
		existattrp = xmlHasProp(nodep, attrp->name);
	}

	if (existattrp != NULL && existattrp->type != XML_ATTRIBUTE_DECL) {
		if ((oldobj = php_dom_object_get_data((xmlNodePtr) existattrp)) != NULL &&
			((php_libxml_node_ptr *)oldobj->ptr)->node == (xmlNodePtr) attrp)
		{
			RETURN_NULL();
		}
		xmlUnlinkNode((xmlNodePtr) existattrp);
	}

	if (attrp->parent != NULL) {
		xmlUnlinkNode((xmlNodePtr) attrp);
	}

	if (attrp->doc == NULL && nodep->doc != NULL) {
		attrobj->document = intern->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)attrobj, NULL);
	}

	xmlAddChild(nodep, (xmlNodePtr) attrp);

	/* Returns old property if removed otherwise NULL */
	if (existattrp != NULL) {
		DOM_RET_OBJ((xmlNodePtr) existattrp, &ret, intern);
	} else {
		RETVAL_NULL();
	}

}
/* }}} end dom_element_set_attribute_node_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-A6C90942
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, getElementsByTagNameNS)
{
	zval *id;
	xmlNodePtr elemp;
	size_t uri_len, name_len;
	dom_object *intern, *namednode;
	char *uri, *name;
	xmlChar *local, *nsuri;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	php_dom_create_iterator(return_value, DOM_NODELIST);
	namednode = Z_DOMOBJ_P(return_value);
	local = xmlCharStrndup(name, name_len);
	nsuri = xmlCharStrndup(uri ? uri : "", uri_len);
	dom_namednode_iter(intern, 0, namednode, NULL, local, nsuri);

}
/* }}} end dom_element_get_elements_by_tag_name_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElHasAttr
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, hasAttribute)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	char *name;
	size_t name_len;
	xmlNodePtr attr;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	attr = dom_get_dom1_attribute(nodep, (xmlChar *)name);
	if (attr == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} end dom_element_has_attribute */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElHasAttrNS
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, hasAttributeNS)
{
	zval *id;
	xmlNodePtr elemp;
	xmlNs *nsp;
	dom_object *intern;
	size_t uri_len, name_len;
	char *uri, *name;
	xmlChar *value;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	value = xmlGetNsProp(elemp, (xmlChar *)name, (xmlChar *)uri);

	if (value != NULL) {
		xmlFree(value);
		RETURN_TRUE;
	} else {
		if (xmlStrEqual((xmlChar *)uri, (xmlChar *)DOM_XMLNS_NAMESPACE)) {
			nsp = dom_get_nsdecl(elemp, (xmlChar *)name);
			if (nsp != NULL) {
				RETURN_TRUE;
			}
		}
	}

	RETURN_FALSE;
}
/* }}} end dom_element_has_attribute_ns */

static void php_set_attribute_id(xmlAttrPtr attrp, bool is_id) /* {{{ */
{
	if (is_id == 1 && attrp->atype != XML_ATTRIBUTE_ID) {
		xmlChar *id_val;

		id_val = xmlNodeListGetString(attrp->doc, attrp->children, 1);
		if (id_val != NULL) {
			xmlAddID(NULL, attrp->doc, id_val, attrp);
			xmlFree(id_val);
		}
	} else if (is_id == 0 && attrp->atype == XML_ATTRIBUTE_ID) {
		xmlRemoveID(attrp->doc, attrp);
		attrp->atype = 0;
	}
}
/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetIdAttr
Since: DOM Level 3
*/
PHP_METHOD(DOMElement, setIdAttribute)
{
	zval *id;
	xmlNode *nodep;
	xmlAttrPtr attrp;
	dom_object *intern;
	char *name;
	size_t name_len;
	bool is_id;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sb", &name, &name_len, &is_id) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_NULL();
	}

	attrp = xmlHasNsProp(nodep, (xmlChar *)name, NULL);
	if (attrp == NULL || attrp->type == XML_ATTRIBUTE_DECL) {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(intern->document));
	} else {
		php_set_attribute_id(attrp, is_id);
	}

	RETURN_NULL();
}
/* }}} end dom_element_set_id_attribute */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetIdAttrNS
Since: DOM Level 3
*/
PHP_METHOD(DOMElement, setIdAttributeNS)
{
	zval *id;
	xmlNodePtr elemp;
	xmlAttrPtr attrp;
	dom_object *intern;
	size_t uri_len, name_len;
	char *uri, *name;
	bool is_id;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssb", &uri, &uri_len, &name, &name_len, &is_id) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(elemp) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_NULL();
	}

	attrp = xmlHasNsProp(elemp, (xmlChar *)name, (xmlChar *)uri);
	if (attrp == NULL || attrp->type == XML_ATTRIBUTE_DECL) {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(intern->document));
	} else {
		php_set_attribute_id(attrp, is_id);
	}

	RETURN_NULL();
}
/* }}} end dom_element_set_id_attribute_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetIdAttrNode
Since: DOM Level 3
*/
PHP_METHOD(DOMElement, setIdAttributeNode)
{
	zval *id, *node;
	xmlNode *nodep;
	xmlAttrPtr attrp;
	dom_object *intern, *attrobj;
	bool is_id;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ob", &node, dom_attr_class_entry, &is_id) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_NULL();
	}

	DOM_GET_OBJ(attrp, node, xmlAttrPtr, attrobj);

	if (attrp->parent != nodep) {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(intern->document));
	} else {
		php_set_attribute_id(attrp, is_id);
	}

	RETURN_NULL();
}
/* }}} end dom_element_set_id_attribute_node */

/* {{{ URL:
Since:
*/
PHP_METHOD(DOMElement, remove)
{
	zval *id;
	xmlNodePtr child;
	dom_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	id = ZEND_THIS;
	DOM_GET_OBJ(child, id, xmlNodePtr, intern);

	dom_child_node_remove(intern);
}
/* }}} end DOMElement::remove */

PHP_METHOD(DOMElement, after)
{
	int argc = 0;
	zval *args, *id;
	dom_object *intern;
	xmlNode *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	id = ZEND_THIS;
	DOM_GET_OBJ(context, id, xmlNodePtr, intern);

	dom_parent_node_after(intern, args, argc);
}

PHP_METHOD(DOMElement, before)
{
	int argc = 0;
	zval *args, *id;
	dom_object *intern;
	xmlNode *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	id = ZEND_THIS;
	DOM_GET_OBJ(context, id, xmlNodePtr, intern);

	dom_parent_node_before(intern, args, argc);
}

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-append
Since: DOM Living Standard (DOM4)
*/
PHP_METHOD(DOMElement, append)
{
	int argc = 0;
	zval *args, *id;
	dom_object *intern;
	xmlNode *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	id = ZEND_THIS;
	DOM_GET_OBJ(context, id, xmlNodePtr, intern);

	dom_parent_node_append(intern, args, argc);
}
/* }}} end DOMElement::append */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-prepend
Since: DOM Living Standard (DOM4)
*/
PHP_METHOD(DOMElement, prepend)
{
	int argc = 0;
	zval *args, *id;
	dom_object *intern;
	xmlNode *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	id = ZEND_THIS;
	DOM_GET_OBJ(context, id, xmlNodePtr, intern);

	dom_parent_node_prepend(intern, args, argc);
}
/* }}} end DOMElement::prepend */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-replacechildren
Since: DOM Living Standard (DOM4)
*/
PHP_METHOD(DOMElement, replaceWith)
{
	int argc = 0;
	zval *args, *id;
	dom_object *intern;
	xmlNode *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	id = ZEND_THIS;
	DOM_GET_OBJ(context, id, xmlNodePtr, intern);

	dom_child_replace_with(intern, args, argc);
}
/* }}} end DOMElement::prepend */

#endif
