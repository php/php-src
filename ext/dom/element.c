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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"


/*
* class domelement extends domnode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-745549614
* Since: 
*/

zend_function_entry php_dom_element_class_functions[] = {
	PHP_FALIAS(getAttribute, dom_element_get_attribute, NULL)
	PHP_FALIAS(setAttribute, dom_element_set_attribute, NULL)
	PHP_FALIAS(removeAttribute, dom_element_remove_attribute, NULL)
	PHP_FALIAS(getAttributeNode, dom_element_get_attribute_node, NULL)
	PHP_FALIAS(setAttributeNode, dom_element_set_attribute_node, NULL)
	PHP_FALIAS(removeAttributeNode, dom_element_remove_attribute_node, NULL)
	PHP_FALIAS(getElementsByTagName, dom_element_get_elements_by_tag_name, NULL)
	PHP_FALIAS(getAttributeNS, dom_element_get_attribute_ns, NULL)
	PHP_FALIAS(setAttributeNS, dom_element_set_attribute_ns, NULL)
	PHP_FALIAS(removeAttributeNS, dom_element_remove_attribute_ns, NULL)
	PHP_FALIAS(getAttributeNodeNS, dom_element_get_attribute_node_ns, NULL)
	PHP_FALIAS(setAttributeNodeNS, dom_element_set_attribute_node_ns, NULL)
	PHP_FALIAS(getElementsByTagNameNS, dom_element_get_elements_by_tag_name_ns, NULL)
	PHP_FALIAS(hasAttribute, dom_element_has_attribute, NULL)
	PHP_FALIAS(hasAttributeNS, dom_element_has_attribute_ns, NULL)
	PHP_FALIAS(setIdAttribute, dom_element_set_id_attribute, NULL)
	PHP_FALIAS(setIdAttributeNS, dom_element_set_id_attribute_ns, NULL)
	PHP_FALIAS(setIdAttributeNode, dom_element_set_id_attribute_node, NULL)
	PHP_FALIAS(domelement, dom_element_element, NULL)
	{NULL, NULL, NULL}
};

/* {{{ proto domnode dom_element_element(string name, [string value]); */
PHP_FUNCTION(dom_element_element)
{

	zval *id;
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *name, *value = NULL;
	int name_len, value_len = 0;

	id = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	if (name_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Element name is required");
		RETURN_FALSE;
	}

	nodep = xmlNewNode(NULL, (xmlChar *) name);

	if (!nodep)
		RETURN_FALSE;

	if (value_len > 0) {
		xmlNodeSetContentLen(nodep, value, value_len);
	}

	intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL) {
		oldnode = (xmlNodePtr)intern->ptr;
		if (oldnode != NULL) {
			node_free_resource(oldnode  TSRMLS_CC);
		}
		php_dom_set_object(intern, nodep TSRMLS_CC);
	}
}
/* }}} end dom_element_element */

/* {{{ proto tagName	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-104682815
Since: 
*/
int dom_element_tag_name_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNodePtr nodep;
	xmlNsPtr ns;
	xmlChar *qname;

	nodep = dom_object_get_node(obj);
	ALLOC_ZVAL(*retval);
	ns = nodep->ns;
	if (ns != NULL && ns->prefix) {
		qname = xmlStrdup(ns->prefix);
		qname = xmlStrcat(qname, ":");
		qname = xmlStrcat(qname, nodep->name);
		ZVAL_STRING(*retval, qname, 1);
		xmlFree(qname);
	} else {
		ZVAL_STRING(*retval, nodep->name, 1);
	}

	return SUCCESS;
}

/* }}} */



/* {{{ proto schemaTypeInfo	typeinfo	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Element-schemaTypeInfo
Since: DOM Level 3
*/
int dom_element_schema_type_info_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	ZVAL_NULL(*retval);
	return SUCCESS;
}

/* }}} */



/* {{{ proto domstring dom_element_get_attribute(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-666EE0F9
Since:
*/
PHP_FUNCTION(dom_element_get_attribute)
{
	zval *id;
	xmlNode *nodep;
	char *name, *value;
	dom_object *intern;
	int name_len;


	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	value = xmlGetProp(nodep, name);
	if (value == NULL) {
		RETURN_EMPTY_STRING();
	} else {
		RETVAL_STRING(value, 1);
		xmlFree(value);
	}
}
/* }}} end dom_element_get_attribute */


/* {{{ proto dom_void dom_element_set_attribute(string name, string value);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-F68F082
Since: 
*/
PHP_FUNCTION(dom_element_set_attribute)
{
	zval *id, *rv = NULL;
	xmlNode *nodep;
	xmlAttr *attr;
	int ret, name_len, value_len;
	dom_object *intern;
	char *name, *value;


	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",  &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	attr = xmlHasProp(nodep,name);
	if (attr != NULL) {
		node_list_unlink(attr->children TSRMLS_CC);
	}
	attr = xmlSetProp(nodep, name, value);
	if (!attr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such attribute '%s'", name);
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, (xmlNodePtr) attr, &ret, intern);

}
/* }}} end dom_element_set_attribute */


/* {{{ proto dom_void dom_element_remove_attribute(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6D6AC0F9
Since:
*/
PHP_FUNCTION(dom_element_remove_attribute)
{
	zval *id;
	xmlNode *nodep;
	xmlAttr *attrp;
	dom_object *intern;
	int name_len;
	char *name;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	attrp = xmlHasProp(nodep,name);
	if (attrp == NULL) {
		RETURN_FALSE;
	}

	/*	TODO: DTD defined attributes are handled special */
	if (dom_object_get_data((xmlNodePtr) attrp) == NULL) {
		node_list_unlink(attrp->children TSRMLS_CC);
		xmlUnlinkNode((xmlNodePtr) attrp);
		xmlFreeProp(attrp);
	} else {
		xmlUnlinkNode((xmlNodePtr) attrp);
	}

	RETURN_TRUE;
}
/* }}} end dom_element_remove_attribute */


/* {{{ proto domattr dom_element_get_attribute_node(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-217A91B8
Since: 
*/
PHP_FUNCTION(dom_element_get_attribute_node)
{
	zval *id, *rv = NULL;
	xmlNode *nodep;
	xmlAttr  *attrp;
	int name_len, ret;
	dom_object *intern;
	char *name;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",  &name, &name_len) == FAILURE) {
		return;
	}

	attrp = xmlHasProp(nodep,name);
	if (attrp == NULL) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, (xmlNodePtr) attrp, &ret, intern);
}
/* }}} end dom_element_get_attribute_node */


/* {{{ proto domattr dom_element_set_attribute_node(attr newAttr);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-887236154
Since: 
*/
PHP_FUNCTION(dom_element_set_attribute_node)
{
	zval *id, *node, *rv = NULL;
	xmlNode *nodep;
	xmlAttr *attrp, *existattrp = NULL;
	dom_object *intern, *attrobj, *oldobj;
	int ret;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &node) == FAILURE) {
		return;
	}

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	DOM_GET_OBJ(attrp, node, xmlAttrPtr, attrobj);

	if (attrp->type != XML_ATTRIBUTE_NODE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attribute node is required");
		RETURN_FALSE;
	}

	existattrp = xmlHasProp(nodep, attrp->name);
	if (existattrp != NULL) {
		if ((oldobj = dom_object_get_data((xmlNodePtr) existattrp)) != NULL && 
			((node_ptr *)oldobj->ptr)->node == (xmlNodePtr) attrp)
		{
			RETURN_NULL();
		}
		xmlUnlinkNode((xmlNodePtr) existattrp);
	}

	if (attrp->doc == NULL && nodep->doc != NULL) {
		attrobj->document = intern->document;
		increment_document_reference(attrobj, NULL TSRMLS_CC);
	}

	xmlAddChild(nodep, (xmlNodePtr) attrp);

	/* Returns old property if removed otherwise NULL */
	if (existattrp != NULL) {
		DOM_RET_OBJ(rv, (xmlNodePtr) existattrp, &ret, intern);
	} else {
		RETVAL_NULL();
	}

}
/* }}} end dom_element_set_attribute_node */


/* {{{ proto domattr dom_element_remove_attribute_node(attr oldAttr);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D589198
Since: 
*/
PHP_FUNCTION(dom_element_remove_attribute_node)
{
	zval *id, *node, *rv = NULL;
	xmlNode *nodep;
	xmlAttr *attrp;
	dom_object *intern, *attrobj;
	int ret;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o",  &node) == FAILURE) {
		return;
	}

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	DOM_GET_OBJ(attrp, node, xmlAttrPtr, attrobj);

	if (attrp->type != XML_ATTRIBUTE_NODE || attrp->parent != nodep) {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	xmlUnlinkNode((xmlNodePtr) attrp);

	DOM_RET_OBJ(rv, (xmlNodePtr) attrp, &ret, intern);

}
/* }}} end dom_element_remove_attribute_node */


/* {{{ proto domnodelist dom_element_get_elements_by_tag_name(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1938918D
Since: 
*/
PHP_FUNCTION(dom_element_get_elements_by_tag_name)
{
	zval *id;
	xmlXPathContextPtr ctxp;
	xmlNodePtr nodep;
	xmlDocPtr docp;
	xmlXPathObjectPtr xpathobjp;
	int name_len, ret;
	dom_object *intern;
	char *str,*name;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	docp = nodep->doc;

	ctxp = xmlXPathNewContext(docp);

	ctxp->node = nodep;
	str = (char*) safe_emalloc((name_len+13), sizeof(char), 0) ;
	sprintf(str ,"descendant::%s",name);

	xpathobjp = xmlXPathEval(str, ctxp);
	efree(str);
	ctxp->node = NULL;

	if (!xpathobjp) {
		RETURN_FALSE;
	}

	if (xpathobjp->type ==  XPATH_NODESET) {
		int i;
		xmlNodeSetPtr nodesetp;

		if (NULL == (nodesetp = xpathobjp->nodesetval)) {
			xmlXPathFreeObject (xpathobjp);
			xmlXPathFreeContext(ctxp);
			RETURN_FALSE;
		}

		array_init(return_value);

		for (i = 0; i < nodesetp->nodeNr; i++) {
			xmlNodePtr node = nodesetp->nodeTab[i];
			zval *child;
			MAKE_STD_ZVAL(child);

			child = php_dom_create_object(node, &ret, NULL, child, intern TSRMLS_CC);
			add_next_index_zval(return_value, child);
		}
	}

	xmlXPathFreeObject(xpathobjp);
	xmlXPathFreeContext(ctxp);
}
/* }}} end dom_element_get_elements_by_tag_name */


/* {{{ proto domstring dom_element_get_attribute_ns(string namespaceURI, string localName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElGetAttrNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_element_get_attribute_ns)
{
	zval *id;
	xmlNodePtr elemp;
	xmlNsPtr nsptr;
	dom_object *intern;
	int uri_len = 0, name_len = 0;
	char *uri, *name, *strattr;

	DOM_GET_THIS_OBJ(elemp, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}
	strattr = xmlGetNsProp(elemp, name, uri);

	if (strattr != NULL) {
		RETVAL_STRING(strattr, 1);
		xmlFree(strattr);
	} else {
		if (xmlStrEqual(uri, DOM_XMLNS_NAMESPACE)) {
			nsptr = dom_get_nsdecl(elemp, name);
			if (nsptr != NULL) {
				RETVAL_STRING((char *) nsptr->href, 1);
			} else {
				RETVAL_EMPTY_STRING();
			}
		} else {
			RETVAL_EMPTY_STRING();
		}
	}

}
/* }}} end dom_element_get_attribute_ns */


/* {{{ proto dom_void dom_element_set_attribute_ns(string namespaceURI, string qualifiedName, string value);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetAttrNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_element_set_attribute_ns)
{
	zval *id, *rv = NULL;
	xmlNodePtr elemp, nodep = NULL;
	xmlNsPtr nsptr;
	xmlAttr *attr;
	int ret, uri_len = 0, name_len = 0, value_len = 0;
	char *uri, *name, *value;
	char *localname = NULL, *prefix = NULL;
	dom_object *intern;
	int errorcode = 0, stricterror;

	DOM_GET_THIS_OBJ(elemp, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &uri, &uri_len, &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(elemp) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);

	if (errorcode == 0) {
		nodep = (xmlNodePtr) xmlHasNsProp(elemp, localname, uri);
		if (nodep != NULL) {
			node_list_unlink(nodep->children TSRMLS_CC);
		}
		nsptr = xmlSearchNsByHref(elemp->doc, elemp, uri);
		while (nsptr && nsptr->prefix == NULL) {
			nsptr = nsptr->next;
		}
		if (nsptr == NULL) {
			if (prefix == NULL) {
				errorcode = NAMESPACE_ERR;
			} else {
				nsptr = dom_get_ns(elemp, uri, &errorcode, prefix);
			}
		}

		if (errorcode == 0) {
			nodep = (xmlNodePtr) xmlSetNsProp(elemp, nsptr, localname, NULL);
		}

		attr = xmlSetProp(nodep, localname, value);
		if (!attr) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such attribute '%s'", localname);
			RETURN_FALSE;
		}
	}

	xmlFree(localname);
	if (prefix != NULL) {
		xmlFree(prefix);
	}

	if (errorcode != 0) {
		php_dom_throw_error(errorcode, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	if (nodep == NULL) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, nodep, &ret, intern);
}
/* }}} end dom_element_set_attribute_ns */


/* {{{ proto dom_void dom_element_remove_attribute_ns(string namespaceURI, string localName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElRemAtNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_element_remove_attribute_ns)
{
	zval *id;
	xmlNode *nodep;
	xmlAttr *attrp;
	dom_object *intern;
	int name_len, uri_len;
	char *name, *uri;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	if (xmlStrEqual(uri, DOM_XMLNS_NAMESPACE)) {
		DOM_NOT_IMPLEMENTED();
	} else {
		attrp = xmlHasNsProp(nodep, name, uri);
		if (attrp == NULL) {
			RETURN_FALSE;
		}

		if (dom_object_get_data((xmlNodePtr) attrp) == NULL) {
			node_list_unlink(attrp->children TSRMLS_CC);
			xmlUnlinkNode((xmlNodePtr) attrp);
			xmlFreeProp(attrp);
		} else {
			xmlUnlinkNode((xmlNodePtr) attrp);
		}
	}

	RETURN_TRUE;
}
/* }}} end dom_element_remove_attribute_ns */


/* {{{ proto domattr dom_element_get_attribute_node_ns(string namespaceURI, string localName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElGetAtNodeNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_element_get_attribute_node_ns)
{
	zval *id;
	xmlNodePtr elemp;
	xmlNs *nsp;
	dom_object *intern;
	int uri_len, name_len;
	char *uri, *name, *value;

	DOM_GET_THIS_OBJ(elemp, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	value = xmlGetNsProp(elemp, name, uri);

	if (value != NULL) {
		RETVAL_STRING(value, 1);
		xmlFree(value);
	} else {
		if (xmlStrEqual(name, DOM_XMLNS_NAMESPACE)) {
			nsp = dom_get_nsdecl(elemp, name);
			if (nsp != NULL) {
				RETVAL_STRING((char *) nsp->href, 1);
			} else {
				RETVAL_EMPTY_STRING();
			}
		} else {
			RETVAL_EMPTY_STRING();
		}
	}

}
/* }}} end dom_element_get_attribute_node_ns */


/* {{{ proto domattr dom_element_set_attribute_node_ns(attr newAttr);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetAtNodeNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_element_set_attribute_node_ns)
{
	zval *id, *node, *rv = NULL;
	xmlNode *nodep;
	xmlNs *nsp;
	xmlAttr *attrp, *existattrp = NULL;
	dom_object *intern, *attrobj, *oldobj;
	int ret;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &node) == FAILURE) {
		return;
	}

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	DOM_GET_OBJ(attrp, node, xmlAttrPtr, attrobj);

	if (attrp->type != XML_ATTRIBUTE_NODE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attribute node is required");
		RETURN_FALSE;
	}

    nsp = attrp->ns;
    if (nsp != NULL) {
        existattrp = xmlHasNsProp(nodep, nsp->href, attrp->name);
    } else {
        existattrp = xmlHasProp(nodep, attrp->name);
    }

	if (existattrp != NULL) {
		if ((oldobj = dom_object_get_data((xmlNodePtr) existattrp)) != NULL && 
			((node_ptr *)oldobj->ptr)->node == (xmlNodePtr) attrp)
		{
			RETURN_NULL();
		}
		xmlUnlinkNode((xmlNodePtr) existattrp);
	}

	if (attrp->doc == NULL && nodep->doc != NULL) {
		attrobj->document = intern->document;
		increment_document_reference(attrobj, NULL TSRMLS_CC);
	}

	xmlAddChild(nodep, (xmlNodePtr) attrp);

	/* Returns old property if removed otherwise NULL */
	if (existattrp != NULL) {
		DOM_RET_OBJ(rv, (xmlNodePtr) existattrp, &ret, intern);
	} else {
		RETVAL_NULL();
	}

}
/* }}} end dom_element_set_attribute_node_ns */



/* {{{ proto domnodelist dom_element_get_elements_by_tag_name_ns(string namespaceURI, string localName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-A6C90942
Since: DOM Level 2
*/
PHP_FUNCTION(dom_element_get_elements_by_tag_name_ns)
{
	zval *id;
	xmlNodePtr elemp;
	int uri_len, name_len;
	dom_object *intern;
	char *uri, *name;

	DOM_GET_THIS_OBJ(elemp, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	array_init(return_value);

	dom_get_elements_by_tag_name_ns_raw(elemp->children, uri, name, &return_value, intern TSRMLS_CC);
}
/* }}} end dom_element_get_elements_by_tag_name_ns */


/* {{{ proto boolean dom_element_has_attribute(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElHasAttr
Since: DOM Level 2
*/
PHP_FUNCTION(dom_element_has_attribute)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	char *name, *value;
	int name_len;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",  &name, &name_len) == FAILURE) {
		return;
	}

	value = xmlGetProp(nodep, name);
	if (value == NULL) {
		RETURN_FALSE;
	} else {
		xmlFree(value);
		RETURN_TRUE;
	}
}
/* }}} end dom_element_has_attribute */


/* {{{ proto boolean dom_element_has_attribute_ns(string namespaceURI, string localName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElHasAttrNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_element_has_attribute_ns)
{
	zval *id;
	xmlNodePtr elemp;
	xmlNs *nsp;
	dom_object *intern;
	int uri_len, name_len;
	char *uri, *name, *value;

	DOM_GET_THIS_OBJ(elemp, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	value = xmlGetNsProp(elemp, name, uri);

	if (value != NULL) {
		xmlFree(value);
		RETURN_TRUE;
	} else {
		if (xmlStrEqual(uri, DOM_XMLNS_NAMESPACE)) {
			nsp = dom_get_nsdecl(elemp, name);
			if (nsp != NULL) {
				RETURN_TRUE;
			}
		}
	}

	RETURN_FALSE;
}
/* }}} end dom_element_has_attribute_ns */


/* {{{ proto dom_void dom_element_set_id_attribute(string name, boolean isId);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetIdAttr
Since: DOM Level 3
*/
PHP_FUNCTION(dom_element_set_id_attribute)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_element_set_id_attribute */


/* {{{ proto dom_void dom_element_set_id_attribute_ns(string namespaceURI, string localName, boolean isId);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetIdAttrNS
Since: DOM Level 3
*/
PHP_FUNCTION(dom_element_set_id_attribute_ns)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_element_set_id_attribute_ns */


/* {{{ proto dom_void dom_element_set_id_attribute_node(attr idAttr, boolean isId);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetIdAttrNode
Since: DOM Level 3
*/
PHP_FUNCTION(dom_element_set_id_attribute_node)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_element_set_id_attribute_node */

#endif
