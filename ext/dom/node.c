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
* class domnode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1950641247
* Since: 
*/

zend_function_entry php_dom_node_class_functions[] = {
	PHP_FALIAS(insertBefore, dom_node_insert_before, NULL)
	PHP_FALIAS(replaceChild, dom_node_replace_child, NULL)
	PHP_FALIAS(removeChild, dom_node_remove_child, NULL)
	PHP_FALIAS(appendChild, dom_node_append_child, NULL)
	PHP_FALIAS(hasChildNodes, dom_node_has_child_nodes, NULL)
	PHP_FALIAS(cloneNode, dom_node_clone_node, NULL)
	PHP_FALIAS(normalize, dom_node_normalize, NULL)
	PHP_FALIAS(isSupported, dom_node_is_supported, NULL)
	PHP_FALIAS(hasAttributes, dom_node_has_attributes, NULL)
	PHP_FALIAS(compareDocumentPosition, dom_node_compare_document_position, NULL)
	PHP_FALIAS(isSameNode, dom_node_is_same_node, NULL)
	PHP_FALIAS(lookupPrefix, dom_node_lookup_prefix, NULL)
	PHP_FALIAS(isDefaultNamespace, dom_node_is_default_namespace, NULL)
	PHP_FALIAS(lookupNamespaceUri, dom_node_lookup_namespace_uri, NULL)
	PHP_FALIAS(isEqualNode, dom_node_is_equal_node, NULL)
	PHP_FALIAS(getFeature, dom_node_get_feature, NULL)
	PHP_FALIAS(setUserData, dom_node_set_user_data, NULL)
	PHP_FALIAS(getUserData, dom_node_get_user_data, NULL)
	{NULL, NULL, NULL}
};

static void dom_reconcile_ns(xmlDocPtr doc, xmlNodePtr nodep) {
	xmlNsPtr nsptr;

	if (nodep->type == XML_ELEMENT_NODE) {
		/* Following if block primarily used for inserting nodes created via createElementNS */
		if (nodep->nsDef != NULL && nodep->nsDef->href != NULL) {
			if((nsptr = xmlSearchNsByHref(doc, nodep->parent, nodep->nsDef->href)) && 
				(nodep->nsDef->prefix == NULL || xmlStrEqual(nsptr->prefix, nodep->nsDef->prefix))) {
				dom_set_old_ns(doc, nodep->nsDef);
				nodep->nsDef = NULL;
			}
		}
		xmlReconciliateNs(doc, nodep);
	}
}

/* {{{ proto nodeName	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-F68D095
Since: 
*/
int dom_node_node_name_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;
	xmlNsPtr ns;
	char *str = NULL;
	xmlChar *qname = NULL;

	nodep = dom_object_get_node(obj);

	switch (nodep->type) {
		case XML_ATTRIBUTE_NODE:
		case XML_ELEMENT_NODE:
			ns = nodep->ns;
			if (ns != NULL && ns->prefix) {
				qname = xmlStrdup(ns->prefix);
				qname = xmlStrcat(qname, ":");
				qname = xmlStrcat(qname, nodep->name);
				str = qname;
			} else {
				str = (char *) nodep->name;
			}
			break;
		case XML_NAMESPACE_DECL:
			ns = nodep->ns;
			if (ns != NULL && ns->prefix) {
				qname = xmlStrdup("xmlns");
				qname = xmlStrcat(qname, ":");
				qname = xmlStrcat(qname, nodep->name);
				str = qname;
			} else {
				str = (char *) nodep->name;
			}
			break;
		case XML_DOCUMENT_TYPE_NODE:
		case XML_DTD_NODE:
		case XML_PI_NODE:
		case XML_ENTITY_DECL:
		case XML_ENTITY_REF_NODE:
		case XML_NOTATION_NODE:
			str = (char *) nodep->name;
			break;
		case XML_CDATA_SECTION_NODE:
			str = "#cdata-section";
			break;
		case XML_COMMENT_NODE:
			str = "#comment";
			break;
		case XML_HTML_DOCUMENT_NODE:
		case XML_DOCUMENT_NODE:
			str = "#document";
			break;
		case XML_DOCUMENT_FRAG_NODE:
			str = "#document-fragment";
			break;
		case XML_TEXT_NODE:
			str = "#text";
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Node Type");
			return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	if(str != NULL) {
		ZVAL_STRING(*retval, str, 1);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}
	
	if (qname != NULL) {
		xmlFree(qname);
	}

	return SUCCESS;

}

/* }}} */



/* {{{ proto nodeValue	string	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-F68D080
Since: 
*/
int dom_node_node_value_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;
	char *str = NULL;

	nodep = dom_object_get_node(obj);
	/* Access to Element node is implemented as a convience method */
	switch (nodep->type) {
		case XML_ATTRIBUTE_NODE:
		case XML_TEXT_NODE:
		case XML_ELEMENT_NODE:
		case XML_COMMENT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_PI_NODE:
			str = xmlNodeGetContent(nodep);
			break;
		case XML_NAMESPACE_DECL:
			str = xmlNodeGetContent(nodep->children);
			break;
		default:
			str = NULL;
			break;
	}

	ALLOC_ZVAL(*retval);

	if(str != NULL) {
		ZVAL_STRING(*retval, str, 1);
		xmlFree(str);
	} else {
		ZVAL_NULL(*retval);
	}


	return SUCCESS;

}

int dom_node_node_value_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	xmlNode *nodep;

	nodep = dom_object_get_node(obj);

	/* Access to Element node is implemented as a convience method */
	switch (nodep->type) {
		case XML_ELEMENT_NODE:
		case XML_ATTRIBUTE_NODE:
			if (nodep->children) {
				node_list_unlink(nodep->children TSRMLS_CC);
			}
		case XML_TEXT_NODE:
		case XML_COMMENT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_PI_NODE:
			convert_to_string(newval);
			xmlNodeSetContentLen(nodep, Z_STRVAL_P(newval), Z_STRLEN_P(newval) + 1);
			break;
		default:
			break;
	}

	return SUCCESS;
}

/* }}} */



/* {{{ proto nodeType	unsigned short	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-111237558
Since:
*/
int dom_node_node_type_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;

	nodep = dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);

	/* Specs dictate that they are both type XML_DOCUMENT_TYPE_NODE */
	if (nodep->type == XML_DTD_NODE) {
		ZVAL_LONG(*retval, XML_DOCUMENT_TYPE_NODE);
	} else {
		ZVAL_LONG(*retval, nodep->type);
	}

	return SUCCESS;
}

/* }}} */



/* {{{ proto parentNode	node	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1060184317
Since: 
*/
int dom_node_parent_node_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep, *nodeparent;
	int ret;

	nodep = dom_object_get_node(obj);

	nodeparent = nodep->parent;
	if (!nodeparent) {
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	if (NULL == (*retval = php_dom_create_object(nodeparent, &ret, NULL, *retval, obj TSRMLS_CC))) {
		php_error(E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
}

/* }}} */



/* {{{ proto childNodes	nodelist	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1451460987
Since: 
*/
int dom_node_child_nodes_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;
	dom_object *intern;

	nodep = dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);
	
	if (dom_node_children_valid(nodep) == FAILURE) {
		ZVAL_NULL(*retval);
	} else {
		php_dom_create_interator(*retval, DOM_NODELIST TSRMLS_CC);
		intern = (dom_object *)zend_objects_get_address(*retval TSRMLS_CC);
		dom_namednode_iter(obj, XML_ELEMENT_NODE, intern, NULL, NULL, NULL);
	}

	return SUCCESS;
}

/* }}} */



/* {{{ proto firstChild node	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-169727388
Since: 
*/
int dom_node_first_child_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep, *first = NULL;
	int ret;

	nodep = dom_object_get_node(obj);

	if (dom_node_children_valid(nodep) == SUCCESS) {
		first = nodep->children;
	}

	if (!first) {
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	if (NULL == (*retval = php_dom_create_object(first, &ret, NULL, *retval, obj TSRMLS_CC))) {
		php_error(E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
}

/* }}} */



/* {{{ proto lastChild	node	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-61AD09FB
Since: 
*/
int dom_node_last_child_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep, *last = NULL;
	int ret;

	nodep = dom_object_get_node(obj);

	if (dom_node_children_valid(nodep) == SUCCESS) {
		last = nodep->last;
	}

	if (!last) {
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	if (NULL == (*retval = php_dom_create_object(last, &ret, NULL, *retval, obj TSRMLS_CC))) {
		php_error(E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
}

/* }}} */



/* {{{ proto previousSibling	node	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-640FB3C8
Since: 
*/
int dom_node_previous_sibling_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep, *prevsib;
	int ret;

	nodep = dom_object_get_node(obj);

	prevsib = nodep->prev;
	if (!prevsib) {
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	if (NULL == (*retval = php_dom_create_object(prevsib, &ret, NULL, *retval, obj TSRMLS_CC))) {
		php_error(E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
}

/* }}} */



/* {{{ proto nextSibling	node
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6AC54C2F
Since:
*/
int dom_node_next_sibling_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep, *nextsib;
	int ret;

	nodep = dom_object_get_node(obj);

	nextsib = nodep->next;
	if (!nextsib) {
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	if (NULL == (*retval = php_dom_create_object(nextsib, &ret, NULL, *retval, obj TSRMLS_CC))) {
		php_error(E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
}

/* }}} */



/* {{{ proto attributes	namednodemap	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-84CF096
Since: 
*/
int dom_node_attributes_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;
	dom_object *intern;

	nodep = dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);

	if (nodep->type == XML_ELEMENT_NODE) {
		php_dom_create_interator(*retval, DOM_NAMEDNODEMAP TSRMLS_CC);
		intern = (dom_object *)zend_objects_get_address(*retval TSRMLS_CC);
		dom_namednode_iter(obj, XML_ATTRIBUTE_NODE, intern, NULL, NULL, NULL);
	} else {
		ZVAL_NULL(*retval);
	}

	return SUCCESS;
}

/* }}} */



/* {{{ proto ownerDocument	dom_document	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-node-ownerDoc
Since: 
*/
int dom_node_owner_document_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;
	xmlDocPtr docp;
	int ret;

	nodep = dom_object_get_node(obj);

	if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
		ALLOC_ZVAL(*retval);
		ZVAL_NULL(*retval);
		return SUCCESS;
	}

	docp = nodep->doc;
	if (!docp) {
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	if (NULL == (*retval = php_dom_create_object((xmlNodePtr) docp, &ret, NULL, *retval, obj TSRMLS_CC))) {
		php_error(E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
}

/* }}} */



/* {{{ proto namespaceUri	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeNSname
Since: DOM Level 2
*/
int dom_node_namespace_uri_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;
	char *str = NULL;

	nodep = dom_object_get_node(obj);

	switch (nodep->type) {
		case XML_ELEMENT_NODE:
		case XML_ATTRIBUTE_NODE:
		case XML_NAMESPACE_DECL:
			if (nodep->ns != NULL) {
				str = (char *) nodep->ns->href;
			}
			break;
		default:
			str = NULL;
			break;
	}

	ALLOC_ZVAL(*retval);

	if(str != NULL) {
		ZVAL_STRING(*retval, str, 1);
	} else {
		ZVAL_NULL(*retval);
	}

	return SUCCESS;
}

/* }}} */



/* {{{ proto prefix	string	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeNSPrefix
Since: DOM Level 2
*/
int dom_node_prefix_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;
	xmlNsPtr ns;
	char *str = NULL;

	nodep = dom_object_get_node(obj);

	switch (nodep->type) {
		case XML_ELEMENT_NODE:
		case XML_ATTRIBUTE_NODE:
		case XML_NAMESPACE_DECL:
			ns = nodep->ns;
			if (ns != NULL && ns->prefix) {
				str = (char *) ns->prefix;
			}
			break;
		default:
			str = NULL;
			break;
	}

	ALLOC_ZVAL(*retval);

	if (str == NULL) {
		ZVAL_EMPTY_STRING(*retval);
	} else {
		ZVAL_STRING(*retval, str, 1);
	}
	return SUCCESS;

}

int dom_node_prefix_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	xmlNode *nodep;
	xmlDocPtr doc;
	xmlNsPtr ns, curns = NULL;
	char *strURI;
	char *prefix;

	nodep = dom_object_get_node(obj);

	switch (nodep->type) {
		case XML_ELEMENT_NODE:
		case XML_ATTRIBUTE_NODE:
			prefix = Z_STRVAL_P(newval);
			if (nodep->ns != NULL && !xmlStrEqual(nodep->ns->prefix, (xmlChar *)prefix)) {
				strURI = (char *) nodep->ns->href;
				if (strURI == NULL || 
					(!strcmp (prefix, "xml") && strcmp(strURI, XML_XML_NAMESPACE)) ||
					(nodep->type == XML_ATTRIBUTE_NODE && !strcmp (prefix, "xmlns") &&
					 strcmp (strURI, DOM_XMLNS_NAMESPACE)) ||
					(nodep->type == XML_ATTRIBUTE_NODE && !strcmp (nodep->name, "xmlns"))) {

					php_dom_throw_error(NAMESPACE_ERR, dom_get_strict_error(obj->document) TSRMLS_CC);
					return FAILURE;	
				}
				ns = xmlNewNs(NULL, nodep->ns->href, (xmlChar *)prefix);
				if (nodep->doc != NULL) {
					doc = nodep->doc;
					if (doc->oldNs == NULL) {
						doc->oldNs = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
						memset(doc->oldNs, 0, sizeof(xmlNs));
						doc->oldNs->type = XML_LOCAL_NAMESPACE;
						doc->oldNs->href = xmlStrdup(XML_XML_NAMESPACE); 
						doc->oldNs->prefix = xmlStrdup((const xmlChar *)"xml"); 
					}

					curns = doc->oldNs;
					while (curns->next != NULL) {
						curns = curns->next;
					}
					curns->next = ns;
				}

				nodep->ns = curns;
			}
			break;
		default:
			break;
	}

	return SUCCESS;
}

/* }}} */



/* {{{ proto localName	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeNSLocalN
Since: DOM Level 2
*/
int dom_node_local_name_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;

	nodep = dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);

	if (nodep->type == XML_ELEMENT_NODE || nodep->type == XML_ATTRIBUTE_NODE || nodep->type == XML_NAMESPACE_DECL) {
		ZVAL_STRING(*retval, (char *) (nodep->name), 1);
	} else {
		ZVAL_NULL(*retval);
	}

	return SUCCESS;
}

/* }}} */



/* {{{ proto baseURI	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-baseURI
Since: DOM Level 3
*/
int dom_node_base_uri_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;
	xmlChar *baseuri;

	nodep = dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);

	baseuri = xmlNodeGetBase(nodep->doc, nodep);
	if (baseuri) {
		ZVAL_STRING(*retval, (char *) (baseuri), 1);
		xmlFree(baseuri);
	} else {
		ZVAL_NULL(*retval);
	}

	return SUCCESS;
}

/* }}} */



/* {{{ proto textContent	string	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-textContent
Since: DOM Level 3
*/
int dom_node_text_content_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNode *nodep;
	char *str = NULL;

	nodep = dom_object_get_node(obj);

	str = xmlNodeGetContent(nodep);

	ALLOC_ZVAL(*retval);

	if(str != NULL) {
		ZVAL_STRING(*retval, str, 1);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}
	xmlFree(str);

	return SUCCESS;
}

int dom_node_text_content_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	return SUCCESS;
}

/* }}} */




/* {{{ proto domnode dom_node_insert_before(node newChild, node refChild);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-952280727
Since:
*/
PHP_FUNCTION(dom_node_insert_before)
{
	zval *id, *node, *ref, *rv = NULL;
	xmlNodePtr child, new_child, parentp, refp;
	dom_object *intern, *childobj, *refpobj;
	int ret, stricterror;

	DOM_GET_THIS_OBJ(parentp, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oo!", &node, &ref) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(child, node, xmlNodePtr, childobj);

	new_child = NULL;

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(parentp) == SUCCESS ||
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	if (dom_hierarchy(parentp, child) == FAILURE) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	if (child->doc != parentp->doc && child->doc != NULL) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	if (child->doc == NULL && parentp->doc != NULL) {
		childobj->document = intern->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)childobj, NULL TSRMLS_CC);
	}

	if (ref != NULL) {
		DOM_GET_OBJ(refp, ref, xmlNodePtr, refpobj);
		if (refp->parent != parentp) {
			php_dom_throw_error(NOT_FOUND_ERR, stricterror TSRMLS_CC);
			RETURN_FALSE;
		}

		if (child->parent != NULL) {
			xmlUnlinkNode(child);
		}

		if (child->type == XML_TEXT_NODE) {
			if (refp->type == XML_TEXT_NODE) {
				xmlChar *tmp;

				tmp = xmlStrdup(child->content);
				tmp = xmlStrcat(tmp, refp->content);
				xmlNodeSetContent(refp, tmp);
				xmlFree(tmp);
				php_libxml_node_free_resource(child TSRMLS_CC);
				DOM_RET_OBJ(rv, refp, &ret, intern);
				return;
			}
			if ((refp->prev != NULL) && (refp->prev->type == XML_TEXT_NODE)
				&& (refp->name == refp->prev->name)) {
				xmlNodeAddContent(refp->prev, child->content);
				php_libxml_node_free_resource(child TSRMLS_CC);
				DOM_RET_OBJ(rv, refp->prev, &ret, intern);
				return;
			}
		} else if (child->type == XML_ATTRIBUTE_NODE) {
			xmlAttrPtr lastattr;

			if (child->ns == NULL)
				lastattr = xmlHasProp(refp->parent, child->name);
			else
				lastattr = xmlHasNsProp(refp->parent, child->name, child->ns->href);
			if (lastattr != NULL) {
				if (lastattr != (xmlAttrPtr) child) {
					xmlUnlinkNode((xmlNodePtr) lastattr);
					php_libxml_node_free_resource((xmlNodePtr) lastattr TSRMLS_CC);
				} else {
					DOM_RET_OBJ(rv, child, &ret, intern);
					return;
				}
			}

		}
		new_child = xmlAddPrevSibling(refp, child);
	} else {
		if (child->parent == parentp){
			xmlUnlinkNode(child);
		}
		if (child->type == XML_TEXT_NODE) {
			if ((parentp->type == XML_TEXT_NODE) &&
			(parentp->content != NULL) &&
			(parentp != child)) {
				xmlNodeAddContent(parentp, child->content);
				php_libxml_node_free_resource(child TSRMLS_CC);
				DOM_RET_OBJ(rv, parentp, &ret, intern);
				return;
			}
			if ((parentp->last != NULL) && (parentp->last->type == XML_TEXT_NODE) &&
			(parentp->last->name == child->name) &&
			(parentp->last != child)) {
				xmlNodeAddContent(parentp->last, child->content);
				php_libxml_node_free_resource(child TSRMLS_CC);
				DOM_RET_OBJ(rv, parentp->last, &ret, intern);
				return;
			}
		} else 	if (child->type == XML_ATTRIBUTE_NODE) {
			xmlAttrPtr lastattr;

			if (child->ns == NULL)
				lastattr = xmlHasProp(parentp, child->name);
			else
				lastattr = xmlHasNsProp(parentp, child->name, child->ns->href);
			if (lastattr != NULL) {
				if (lastattr != (xmlAttrPtr) child) {
					xmlUnlinkNode((xmlNodePtr) lastattr);
					php_libxml_node_free_resource((xmlNodePtr) lastattr TSRMLS_CC);
				} else {
					DOM_RET_OBJ(rv, child, &ret, intern);
					return;
				}
			}
		} else 	if (child->type == XML_DOCUMENT_FRAG_NODE) {
			new_child = xmlAddChildList(parentp, child->children);
			if (new_child != NULL) {
				child->children = NULL;
			}
			DOM_RET_OBJ(rv, new_child, &ret, intern);
			return;
		}
		new_child = xmlAddChild(parentp, child);
	}

	if (NULL == new_child) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't add newnode as the previous sibling of refnode");
		RETURN_FALSE;
	}

	dom_reconcile_ns(parentp->doc, new_child);

	DOM_RET_OBJ(rv, new_child, &ret, intern);

}
/* }}} end dom_node_insert_before */


/* {{{ proto domnode dom_node_replace_child(node newChild, node oldChild);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-785887307
Since: 
*/
PHP_FUNCTION(dom_node_replace_child)
{
	zval *id, *newnode, *oldnode;
	xmlNodePtr children, newchild, oldchild, nodep;
	dom_object *intern, *newchildobj, *oldchildobj;
	int foundoldchild = 0, stricterror;

	int ret;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_children_valid(nodep) == FAILURE) {
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oo", &newnode, &oldnode) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(newchild, newnode, xmlNodePtr, newchildobj);
	DOM_GET_OBJ(oldchild, oldnode, xmlNodePtr, oldchildobj);

	children = nodep->children;
	if (!children) {
		RETURN_FALSE;
	}

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(nodep) == SUCCESS || 
		(newchild->parent != NULL && dom_node_is_read_only(newchild->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	if (newchild->doc != nodep->doc && newchild->doc != NULL) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	if (dom_hierarchy(nodep, newchild) == FAILURE) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	/* check for the old child and wether the new child is already a child */
	while (children) {
		if (children == oldchild) {
			foundoldchild = 1;
			break;
		}
		children = children->next;
	}

	if (foundoldchild) {
		zval *rv = NULL;
		if (oldchild != newchild) {
			xmlNodePtr node;
			if (newchild->doc == NULL && nodep->doc != NULL) {
				xmlSetTreeDoc(newchild, nodep->doc);
				newchildobj->document = intern->document;
				php_libxml_increment_doc_ref((php_libxml_node_object *)newchildobj, NULL TSRMLS_CC);
			}
			node = xmlReplaceNode(oldchild, newchild);
			dom_reconcile_ns(nodep->doc, newchild);
		}
		DOM_RET_OBJ(rv, oldchild, &ret, intern);
		return;
	} else {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

}
/* }}} end dom_node_replace_child */


/* {{{ proto domnode dom_node_remove_child(node oldChild);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1734834066
Since: 
*/
PHP_FUNCTION(dom_node_remove_child)
{
	zval *id, *node;
	xmlNodePtr children, child, nodep;
	dom_object *intern, *childobj;
	int ret, stricterror;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &node) == FAILURE) {
		return;
	}

	if (dom_node_children_valid(nodep) == FAILURE) {
		RETURN_FALSE;
	}

	DOM_GET_OBJ(child, node, xmlNodePtr, childobj);

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(nodep) == SUCCESS || 
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	children = nodep->children;
	if (!children) {
		php_dom_throw_error(NOT_FOUND_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	while (children) {
		if (children == child) {
			zval *rv = NULL;
			xmlUnlinkNode(child);
			DOM_RET_OBJ(rv, child, &ret, intern);
			return;
		}
		children = children->next;
	}

	php_dom_throw_error(NOT_FOUND_ERR, stricterror TSRMLS_CC);
	RETURN_FALSE
}
/* }}} end dom_node_remove_child */


/* {{{ proto domnode dom_node_append_child(node newChild);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-184E7107
Since: 
*/
PHP_FUNCTION(dom_node_append_child)
{
	zval *id, *node, *rv = NULL;
	xmlNodePtr child, nodep, new_child = NULL;
	dom_object *intern, *childobj;
	int ret, stricterror;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &node) == FAILURE) {
		return;
	}

	if (dom_node_children_valid(nodep) == FAILURE) {
		RETURN_FALSE;
	}

	DOM_GET_OBJ(child, node, xmlNodePtr, childobj);

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(nodep) == SUCCESS ||
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	if (dom_hierarchy(nodep, child) == FAILURE) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	if (!(child->doc == NULL || child->doc == nodep->doc)) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror TSRMLS_CC);
		RETURN_FALSE;
	}

	if (child->doc == NULL && nodep->doc != NULL) {
		childobj->document = intern->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)childobj, NULL TSRMLS_CC);
	}

	if (child->parent != NULL){
		xmlUnlinkNode(child);
	}

	if (child->type == XML_TEXT_NODE) {
		if ((nodep->type == XML_TEXT_NODE) &&
		(nodep->content != NULL)) {
			xmlNodeAddContent(nodep, child->content);
			php_libxml_node_free_resource(child TSRMLS_CC);
			DOM_RET_OBJ(rv, nodep, &ret, intern);
			return;
		}
		if ((nodep->last != NULL) && (nodep->last->type == XML_TEXT_NODE) &&
		(nodep->last->name == child->name)) {
			xmlNodeAddContent(nodep->last, child->content);
			php_libxml_node_free_resource(child TSRMLS_CC);
			DOM_RET_OBJ(rv, nodep->last, &ret, intern);
			return;
		}
	} else 	if (child->type == XML_ATTRIBUTE_NODE) {
		xmlAttrPtr lastattr;

		if (child->ns == NULL)
			lastattr = xmlHasProp(nodep, child->name);
		else
			lastattr = xmlHasNsProp(nodep, child->name, child->ns->href);
		if (lastattr != NULL) {
			if (lastattr != (xmlAttrPtr) child) {
				xmlUnlinkNode((xmlNodePtr) lastattr);
				php_libxml_node_free_resource((xmlNodePtr) lastattr TSRMLS_CC);
			}
		}
	} else 	if (child->type == XML_DOCUMENT_FRAG_NODE) {
		new_child = xmlAddChildList(nodep, child->children);
		if (new_child != NULL) {
			child->children = NULL;
		}
		DOM_RET_OBJ(rv, new_child, &ret, intern);
		return;
	}

	new_child = xmlAddChild(nodep, child);

	if (new_child == NULL) {
		php_error(E_WARNING, "Couldn't append node");
		RETURN_FALSE;
	}

	dom_reconcile_ns(nodep->doc, new_child);

	DOM_RET_OBJ(rv, new_child, &ret, intern);
}
/* }}} end dom_node_append_child */


/* {{{ proto boolean dom_node_has_child_nodes();
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-810594187
Since: 
*/
PHP_FUNCTION(dom_node_has_child_nodes)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	DOM_NO_ARGS();
	
	if (dom_node_children_valid(nodep) == FAILURE) {
		RETURN_FALSE;
	}

	if (nodep->children) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_node_has_child_nodes */


/* {{{ proto domnode dom_node_clone_node(boolean deep);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-3A0ED0A4
Since: 
*/
PHP_FUNCTION(dom_node_clone_node)
{
	zval *rv = NULL;
	zval *id;
	xmlNode *n, *node;
	int ret;
	dom_object *intern;
	long recursive = 0;

	DOM_GET_THIS_OBJ(n, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &recursive) == FAILURE) {
		return;
	}

	node = xmlDocCopyNode(n, n->doc, recursive);

	/* When deep is false Element nodes still require the attributes 
	Following taken from libxml as xmlDocCopyNode doesnt do this */
	if (node && n->type == XML_ELEMENT_NODE && recursive == 0) {
		if (n->nsDef != NULL) {
			node->nsDef = xmlCopyNamespaceList(n->nsDef);
		}
		if (n->ns != NULL) {
			xmlNsPtr ns;
			ns = xmlSearchNs(n->doc, node, n->ns->prefix);
			if (ns == NULL) {
				ns = xmlSearchNs(n->doc, n, n->ns->prefix);
				if (ns != NULL) {
					xmlNodePtr root = node;

					while (root->parent != NULL) {
						root = root->parent;
					}
					node->ns = xmlNewNs(root, ns->href, ns->prefix);
				}
			} else {
				node->ns = ns;
			}
		}
		if (n->properties != NULL) {
			node->properties = xmlCopyPropList(node, n->properties);
		}
	}

	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, node, &ret, intern);
}
/* }}} end dom_node_clone_node */



/* {{{ proto dom_void dom_node_normalize();
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-normalize
Since: 
*/
PHP_FUNCTION(dom_node_normalize)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	DOM_NO_ARGS();

	dom_normalize(nodep TSRMLS_CC);

}
/* }}} end dom_node_normalize */


/* {{{ proto boolean dom_node_is_supported(string feature, string version);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Level-2-Core-Node-supports
Since: DOM Level 2
*/
PHP_FUNCTION(dom_node_is_supported)
{
	int feature_len, version_len;
	char *feature, *version;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &feature, &feature_len, &version, &version_len) == FAILURE) {
		return;
	}

	if (dom_has_feature(feature, version)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_node_is_supported */


/* {{{ proto boolean dom_node_has_attributes();
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeHasAttrs
Since: DOM Level 2
*/
PHP_FUNCTION(dom_node_has_attributes)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	DOM_NO_ARGS();

	if (nodep->type != XML_ELEMENT_NODE)
		RETURN_FALSE;

	if (nodep->properties) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_node_has_attributes */


/* {{{ proto unsigned short dom_node_compare_document_position(node other);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-compareDocumentPosition
Since: DOM Level 3
*/
PHP_FUNCTION(dom_node_compare_document_position)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_node_compare_document_position */


/* {{{ proto boolean dom_node_is_same_node(node other);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-isSameNode
Since: DOM Level 3
*/
PHP_FUNCTION(dom_node_is_same_node)
{
	zval *id, *node;
	xmlNodePtr nodeotherp, nodep;
	dom_object *intern, *nodeotherobj;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &node) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(nodeotherp, node, xmlNodePtr, nodeotherobj);

	if (nodep == nodeotherp) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_node_is_same_node */


/* {{{ proto domstring dom_node_lookup_prefix(string namespaceURI);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-lookupNamespacePrefix
Since: DOM Level 3
*/
PHP_FUNCTION(dom_node_lookup_prefix)
{
	zval *id;
	xmlNodePtr nodep, lookupp = NULL;
	dom_object *intern;
	xmlNsPtr nsptr;
	int uri_len = 0;
	char *uri;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &uri, &uri_len) == FAILURE) {
		return;
	}
	
	if (uri_len > 0) {
		switch (nodep->type) { 
			case XML_ELEMENT_NODE:
				lookupp = nodep;
				break; 
			case XML_DOCUMENT_NODE:
			case XML_HTML_DOCUMENT_NODE:
				lookupp = xmlDocGetRootElement((xmlDocPtr) nodep);
				break;
			case XML_ENTITY_NODE : 
			case XML_NOTATION_NODE: 
			case XML_DOCUMENT_FRAG_NODE: 
			case XML_DOCUMENT_TYPE_NODE:
			case XML_DTD_NODE:
				RETURN_NULL();
				break;
			default:
				lookupp =  nodep->parent;
		} 

		if (lookupp != NULL && (nsptr = xmlSearchNsByHref(lookupp->doc, lookupp, uri))) {
			if (nsptr->prefix != NULL) {
				RETURN_STRING((char *) nsptr->prefix, 1);
			}
		}
	}
		
	RETURN_NULL();
}
/* }}} end dom_node_lookup_prefix */


/* {{{ proto boolean dom_node_is_default_namespace(string namespaceURI);
URL: http://www.w3.org/TR/DOM-Level-3-Core/core.html#Node3-isDefaultNamespace
Since: DOM Level 3
*/
PHP_FUNCTION(dom_node_is_default_namespace)
{
	zval *id;
	xmlNodePtr nodep;
	dom_object *intern;
	xmlNsPtr nsptr;
	int uri_len = 0;
	char *uri;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &uri, &uri_len) == FAILURE) {
		return;
	}
	
	if (uri_len > 0) {
		nsptr = xmlSearchNs(nodep->doc, nodep, NULL);
		if (nsptr && xmlStrEqual(nsptr->href, uri)) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} end dom_node_is_default_namespace */


/* {{{ proto domstring dom_node_lookup_namespace_uri(string prefix);
URL: http://www.w3.org/TR/DOM-Level-3-Core/core.html#Node3-lookupNamespaceURI
Since: DOM Level 3
*/
PHP_FUNCTION(dom_node_lookup_namespace_uri)
{
	zval *id;
	xmlNodePtr nodep;
	dom_object *intern;
	xmlNsPtr nsptr;
	int prefix_len = 0;
	char *prefix;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &prefix, &prefix_len) == FAILURE) {
		return;
	}
	
	if (prefix_len > 0) {
		nsptr = xmlSearchNs(nodep->doc, nodep, prefix);
		if (nsptr && nsptr->href != NULL) {
			RETURN_STRING((char *) nsptr->href, 1);
		}
	}

	RETURN_NULL();
}
/* }}} end dom_node_lookup_namespace_uri */


/* {{{ proto boolean dom_node_is_equal_node(node arg);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-isEqualNode
Since: DOM Level 3
*/
PHP_FUNCTION(dom_node_is_equal_node)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_node_is_equal_node */


/* {{{ proto domnode dom_node_get_feature(string feature, string version);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-getFeature
Since: DOM Level 3
*/
PHP_FUNCTION(dom_node_get_feature)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_node_get_feature */


/* {{{ proto domuserdata dom_node_set_user_data(string key, domuserdata data, userdatahandler handler);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-setUserData
Since: DOM Level 3
*/
PHP_FUNCTION(dom_node_set_user_data)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_node_set_user_data */


/* {{{ proto domuserdata dom_node_get_user_data(string key);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-getUserData
Since: DOM Level 3
*/
PHP_FUNCTION(dom_node_get_user_data)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_node_get_user_data */
#endif
