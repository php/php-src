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
* class DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1950641247
* Since:
*/

/* {{{ nodeName	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-F68D095
Since:
*/
int dom_node_node_name_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep;
	xmlNsPtr ns;
	char *str = NULL;
	xmlChar *qname = NULL;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	switch (nodep->type) {
		case XML_ATTRIBUTE_NODE:
		case XML_ELEMENT_NODE:
			ns = nodep->ns;
			if (ns != NULL && ns->prefix) {
				qname = xmlStrdup(ns->prefix);
				qname = xmlStrcat(qname, (xmlChar *) ":");
				qname = xmlStrcat(qname, nodep->name);
				str = (char *) qname;
			} else {
				str = (char *) nodep->name;
			}
			break;
		case XML_NAMESPACE_DECL:
			ns = nodep->ns;
			if (ns != NULL && ns->prefix) {
				qname = xmlStrdup((xmlChar *) "xmlns");
				qname = xmlStrcat(qname, (xmlChar *) ":");
				qname = xmlStrcat(qname, nodep->name);
				str = (char *) qname;
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
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	if (str != NULL) {
		ZVAL_STRING(retval, str);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	if (qname != NULL) {
		xmlFree(qname);
	}

	return SUCCESS;

}

/* }}} */

/* {{{ nodeValue	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-F68D080
Since:
*/
int dom_node_node_value_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	char *str = NULL;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	/* Access to Element node is implemented as a convenience method */
	switch (nodep->type) {
		case XML_ATTRIBUTE_NODE:
		case XML_TEXT_NODE:
		case XML_ELEMENT_NODE:
		case XML_COMMENT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_PI_NODE:
			str = (char *) xmlNodeGetContent(nodep);
			break;
		case XML_NAMESPACE_DECL:
			str = (char *) xmlNodeGetContent(nodep->children);
			break;
		default:
			str = NULL;
			break;
	}

	if(str != NULL) {
		ZVAL_STRING(retval, str);
		xmlFree(str);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;

}

int dom_node_node_value_write(dom_object *obj, zval *newval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	zend_string *str;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	str = zval_try_get_string(newval);
	if (UNEXPECTED(!str)) {
		return FAILURE;
	}

	/* Access to Element node is implemented as a convenience method */
	switch (nodep->type) {
		case XML_ELEMENT_NODE:
		case XML_ATTRIBUTE_NODE:
			if (nodep->children) {
				node_list_unlink(nodep->children);
				php_libxml_node_free_list((xmlNodePtr) nodep->children);
				nodep->children = NULL;
			}
			ZEND_FALLTHROUGH;
		case XML_TEXT_NODE:
		case XML_COMMENT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_PI_NODE:
			xmlNodeSetContentLen(nodep, (xmlChar *) ZSTR_VAL(str), ZSTR_LEN(str) + 1);
			break;
		default:
			break;
	}

	zend_string_release_ex(str, 0);
	return SUCCESS;
}

/* }}} */

/* {{{ nodeType	int
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-111237558
Since:
*/
int dom_node_node_type_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	/* Specs dictate that they are both type XML_DOCUMENT_TYPE_NODE */
	if (nodep->type == XML_DTD_NODE) {
		ZVAL_LONG(retval, XML_DOCUMENT_TYPE_NODE);
	} else {
		ZVAL_LONG(retval, nodep->type);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ parentNode	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1060184317
Since:
*/
int dom_node_parent_node_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *nodeparent;

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

/* {{{ childNodes	DomNodeList
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1451460987
Since:
*/
int dom_node_child_nodes_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	dom_object *intern;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	php_dom_create_iterator(retval, DOM_NODELIST);
	intern = Z_DOMOBJ_P(retval);
	dom_namednode_iter(obj, XML_ELEMENT_NODE, intern, NULL, NULL, NULL);

	return SUCCESS;
}
/* }}} */

/* {{{ firstChild DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-169727388
Since:
*/
int dom_node_first_child_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *first = NULL;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (dom_node_children_valid(nodep) == SUCCESS) {
		first = nodep->children;
	}

	if (!first) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(first, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ lastChild	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-61AD09FB
Since:
*/
int dom_node_last_child_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *last = NULL;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (dom_node_children_valid(nodep) == SUCCESS) {
		last = nodep->last;
	}

	if (!last) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(last, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ previousSibling	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-640FB3C8
Since:
*/
int dom_node_previous_sibling_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *prevsib;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	prevsib = nodep->prev;
	if (!prevsib) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(prevsib, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ nextSibling	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6AC54C2F
Since:
*/
int dom_node_next_sibling_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *nextsib;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	nextsib = nodep->next;
	if (!nextsib) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(nextsib, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ previousElementSibling	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-640FB3C8
Since:
*/
int dom_node_previous_element_sibling_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *prevsib;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	prevsib = nodep->prev;

	while (prevsib && prevsib->type != XML_ELEMENT_NODE) {
		prevsib = prevsib->prev;
	}

	if (!prevsib) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(prevsib, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ nextElementSibling	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6AC54C2F
Since:
*/
int dom_node_next_element_sibling_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *nextsib;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	nextsib = nodep->next;

	while (nextsib != NULL && nextsib->type != XML_ELEMENT_NODE) {
		nextsib = nextsib->next;
	}

	if (!nextsib) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(nextsib, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ attributes	DomNamedNodeMap
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-84CF096
Since:
*/
int dom_node_attributes_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	dom_object *intern;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (nodep->type == XML_ELEMENT_NODE) {
		php_dom_create_iterator(retval, DOM_NAMEDNODEMAP);
		intern = Z_DOMOBJ_P(retval);
		dom_namednode_iter(obj, XML_ATTRIBUTE_NODE, intern, NULL, NULL, NULL);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ ownerDocument	DomDocument
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-node-ownerDoc
Since:
*/
int dom_node_owner_document_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	xmlDocPtr docp;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	docp = nodep->doc;
	if (!docp) {
		return FAILURE;
	}

	php_dom_create_object((xmlNodePtr) docp, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ namespaceUri	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeNSname
Since: DOM Level 2
*/
int dom_node_namespace_uri_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	char *str = NULL;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

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

	if (str != NULL) {
		ZVAL_STRING(retval, str);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ prefix	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeNSPrefix
Since: DOM Level 2
*/
int dom_node_prefix_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	xmlNsPtr ns;
	char *str = NULL;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

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

	if (str == NULL) {
		ZVAL_EMPTY_STRING(retval);
	} else {
		ZVAL_STRING(retval, str);
	}
	return SUCCESS;

}

int dom_node_prefix_write(dom_object *obj, zval *newval)
{
	zend_string *prefix_str;
	xmlNode *nodep, *nsnode = NULL;
	xmlNsPtr ns = NULL, curns;
	char *strURI;
	char *prefix;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	switch (nodep->type) {
		case XML_ELEMENT_NODE:
			nsnode = nodep;
			ZEND_FALLTHROUGH;
		case XML_ATTRIBUTE_NODE:
			if (nsnode == NULL) {
				nsnode = nodep->parent;
				if (nsnode == NULL) {
					nsnode = xmlDocGetRootElement(nodep->doc);
				}
			}
			prefix_str = zval_try_get_string(newval);
			if (UNEXPECTED(!prefix_str)) {
				return FAILURE;
			}

			prefix = ZSTR_VAL(prefix_str);
			if (nsnode && nodep->ns != NULL && !xmlStrEqual(nodep->ns->prefix, (xmlChar *)prefix)) {
				strURI = (char *) nodep->ns->href;
				if (strURI == NULL ||
					(zend_string_equals_literal(prefix_str, "xml") && strcmp(strURI, (char *) XML_XML_NAMESPACE)) ||
					(nodep->type == XML_ATTRIBUTE_NODE && zend_string_equals_literal(prefix_str, "xmlns") &&
					 strcmp(strURI, (char *) DOM_XMLNS_NAMESPACE)) ||
					(nodep->type == XML_ATTRIBUTE_NODE && !strcmp((char *) nodep->name, "xmlns"))) {
					ns = NULL;
				} else {
					curns = nsnode->nsDef;
					while (curns != NULL) {
						if (xmlStrEqual((xmlChar *)prefix, curns->prefix) && xmlStrEqual(nodep->ns->href, curns->href)) {
							ns = curns;
							break;
						}
						curns = curns->next;
					}
					if (ns == NULL) {
						ns = xmlNewNs(nsnode, nodep->ns->href, (xmlChar *)prefix);
					}
				}

				if (ns == NULL) {
					zend_string_release_ex(prefix_str, 0);
					php_dom_throw_error(NAMESPACE_ERR, dom_get_strict_error(obj->document));
					return FAILURE;
				}

				xmlSetNs(nodep, ns);
			}
			zend_string_release_ex(prefix_str, 0);
			break;
		default:
			break;
	}

	return SUCCESS;
}

/* }}} */

/* {{{ localName	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeNSLocalN
Since: DOM Level 2
*/
int dom_node_local_name_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (nodep->type == XML_ELEMENT_NODE || nodep->type == XML_ATTRIBUTE_NODE || nodep->type == XML_NAMESPACE_DECL) {
		ZVAL_STRING(retval, (char *) (nodep->name));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ baseURI	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-baseURI
Since: DOM Level 3
*/
int dom_node_base_uri_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	xmlChar *baseuri;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	baseuri = xmlNodeGetBase(nodep->doc, nodep);
	if (baseuri) {
		ZVAL_STRING(retval, (char *) (baseuri));
		xmlFree(baseuri);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ textContent	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-textContent
Since: DOM Level 3
*/
int dom_node_text_content_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	char *str = NULL;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	str = (char *) xmlNodeGetContent(nodep);

	if (str != NULL) {
		ZVAL_STRING(retval, str);
		xmlFree(str);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

int dom_node_text_content_write(dom_object *obj, zval *newval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	zend_string *str;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	str = zval_try_get_string(newval);
	if (UNEXPECTED(!str)) {
		return FAILURE;
	}

	const xmlChar *xmlChars = (const xmlChar *) ZSTR_VAL(str);
	int type = nodep->type;

	/* We can't directly call xmlNodeSetContent, because it might encode the string through
	 * xmlStringLenGetNodeList for types XML_DOCUMENT_FRAG_NODE, XML_ELEMENT_NODE, XML_ATTRIBUTE_NODE.
	 * See tree.c:xmlNodeSetContent in libxml.
	 * In these cases we need to use a text node to avoid the encoding.
	 * For the other cases, we *can* rely on xmlNodeSetContent because it is either a no-op, or handles
	 * the content without encoding. */
	if (type == XML_DOCUMENT_FRAG_NODE || type == XML_ELEMENT_NODE || type == XML_ATTRIBUTE_NODE) {
		if (nodep->children) {
			node_list_unlink(nodep->children);
			php_libxml_node_free_list((xmlNodePtr) nodep->children);
			nodep->children = NULL;
		}

		xmlNode *textNode = xmlNewText(xmlChars);
		xmlAddChild(nodep, textNode);
	} else {
		xmlNodeSetContent(nodep, xmlChars);
	}

	zend_string_release_ex(str, 0);

	return SUCCESS;
}

/* }}} */

static xmlNodePtr _php_dom_insert_fragment(xmlNodePtr nodep, xmlNodePtr prevsib, xmlNodePtr nextsib, xmlNodePtr fragment, dom_object *intern, dom_object *childobj) /* {{{ */
{
	xmlNodePtr newchild, node;

	newchild = fragment->children;

	if (newchild) {
		if (prevsib == NULL) {
			nodep->children = newchild;
		} else {
			prevsib->next = newchild;
		}
		newchild->prev = prevsib;
		if (nextsib == NULL) {
			nodep->last = fragment->last;
		} else {
			fragment->last->next = nextsib;
			nextsib->prev = fragment->last;
		}

		node = newchild;
		while (node != NULL) {
			node->parent = nodep;
			if (node->doc != nodep->doc) {
				xmlSetTreeDoc(node, nodep->doc);
				if (node->_private != NULL) {
					childobj = node->_private;
					childobj->document = intern->document;
					php_libxml_increment_doc_ref((php_libxml_node_object *)childobj, NULL);
				}
			}
			if (node == fragment->last) {
				break;
			}
			node = node->next;
		}

		fragment->children = NULL;
		fragment->last = NULL;
	}

	return newchild;
}
/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-952280727
Since:
*/
PHP_METHOD(DOMNode, insertBefore)
{
	zval *id, *node, *ref = NULL;
	xmlNodePtr child, new_child, parentp, refp;
	dom_object *intern, *childobj, *refpobj;
	int ret, stricterror;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|O!", &node, dom_node_class_entry, &ref, dom_node_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(parentp, id, xmlNodePtr, intern);

	if (dom_node_children_valid(parentp) == FAILURE) {
		RETURN_FALSE;
	}

	DOM_GET_OBJ(child, node, xmlNodePtr, childobj);

	new_child = NULL;

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(parentp) == SUCCESS ||
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		RETURN_FALSE;
	}

	if (dom_hierarchy(parentp, child) == FAILURE) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
		RETURN_FALSE;
	}

	if (child->doc != parentp->doc && child->doc != NULL) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror);
		RETURN_FALSE;
	}

	if (child->type == XML_DOCUMENT_FRAG_NODE && child->children == NULL) {
		/* TODO Drop Warning? */
		php_error_docref(NULL, E_WARNING, "Document Fragment is empty");
		RETURN_FALSE;
	}

	if (child->doc == NULL && parentp->doc != NULL) {
		childobj->document = intern->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)childobj, NULL);
	}

	if (ref != NULL) {
		DOM_GET_OBJ(refp, ref, xmlNodePtr, refpobj);
		if (refp->parent != parentp) {
			php_dom_throw_error(NOT_FOUND_ERR, stricterror);
			RETURN_FALSE;
		}

		if (child->parent != NULL) {
			xmlUnlinkNode(child);
		}

		if (child->type == XML_TEXT_NODE && (refp->type == XML_TEXT_NODE ||
			(refp->prev != NULL && refp->prev->type == XML_TEXT_NODE))) {
			if (child->doc == NULL) {
				xmlSetTreeDoc(child, parentp->doc);
			}
			new_child = child;
			new_child->parent = refp->parent;
			new_child->next = refp;
			new_child->prev = refp->prev;
			refp->prev = new_child;
			if (new_child->prev != NULL) {
				new_child->prev->next = new_child;
			}
			if (new_child->parent != NULL) {
				if (new_child->parent->children == refp) {
					new_child->parent->children = new_child;
				}
			}

		} else if (child->type == XML_ATTRIBUTE_NODE) {
			xmlAttrPtr lastattr;

			if (child->ns == NULL)
				lastattr = xmlHasProp(refp->parent, child->name);
			else
				lastattr = xmlHasNsProp(refp->parent, child->name, child->ns->href);
			if (lastattr != NULL && lastattr->type != XML_ATTRIBUTE_DECL) {
				if (lastattr != (xmlAttrPtr) child) {
					xmlUnlinkNode((xmlNodePtr) lastattr);
					php_libxml_node_free_resource((xmlNodePtr) lastattr);
				} else {
					DOM_RET_OBJ(child, &ret, intern);
					return;
				}
			}
			new_child = xmlAddPrevSibling(refp, child);
			if (UNEXPECTED(NULL == new_child)) {
				goto cannot_add;
			}
		} else if (child->type == XML_DOCUMENT_FRAG_NODE) {
			xmlNodePtr last = child->last;
			new_child = _php_dom_insert_fragment(parentp, refp->prev, refp, child, intern, childobj);
			dom_reconcile_ns_list(parentp->doc, new_child, last);
		} else {
			new_child = xmlAddPrevSibling(refp, child);
			if (UNEXPECTED(NULL == new_child)) {
				goto cannot_add;
			}
			dom_reconcile_ns(parentp->doc, new_child);
		}
	} else {
		if (child->parent != NULL){
			xmlUnlinkNode(child);
		}
		if (child->type == XML_TEXT_NODE && parentp->last != NULL && parentp->last->type == XML_TEXT_NODE) {
			child->parent = parentp;
			if (child->doc == NULL) {
				xmlSetTreeDoc(child, parentp->doc);
			}
			new_child = child;
			if (parentp->children == NULL) {
				parentp->children = child;
				parentp->last = child;
			} else {
				child = parentp->last;
				child->next = new_child;
				new_child->prev = child;
				parentp->last = new_child;
			}
		} else 	if (child->type == XML_ATTRIBUTE_NODE) {
			xmlAttrPtr lastattr;

			if (child->ns == NULL)
				lastattr = xmlHasProp(parentp, child->name);
			else
				lastattr = xmlHasNsProp(parentp, child->name, child->ns->href);
			if (lastattr != NULL && lastattr->type != XML_ATTRIBUTE_DECL) {
				if (lastattr != (xmlAttrPtr) child) {
					xmlUnlinkNode((xmlNodePtr) lastattr);
					php_libxml_node_free_resource((xmlNodePtr) lastattr);
				} else {
					DOM_RET_OBJ(child, &ret, intern);
					return;
				}
			}
			new_child = xmlAddChild(parentp, child);
			if (UNEXPECTED(NULL == new_child)) {
				goto cannot_add;
			}
		} else if (child->type == XML_DOCUMENT_FRAG_NODE) {
			xmlNodePtr last = child->last;
			new_child = _php_dom_insert_fragment(parentp, parentp->last, NULL, child, intern, childobj);
			dom_reconcile_ns_list(parentp->doc, new_child, last);
		} else {
			new_child = xmlAddChild(parentp, child);
			if (UNEXPECTED(NULL == new_child)) {
				goto cannot_add;
			}
			dom_reconcile_ns(parentp->doc, new_child);
		}
	}

	DOM_RET_OBJ(new_child, &ret, intern);
	return;
cannot_add:
	zend_throw_error(NULL, "Cannot add newnode as the previous sibling of refnode");
	RETURN_THROWS();
}
/* }}} end dom_node_insert_before */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-785887307
Since:
*/
PHP_METHOD(DOMNode, replaceChild)
{
	zval *id, *newnode, *oldnode;
	xmlNodePtr newchild, oldchild, nodep;
	dom_object *intern, *newchildobj, *oldchildobj;
	int stricterror;
	bool replacedoctype = false;

	int ret;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OO", &newnode, dom_node_class_entry, &oldnode, dom_node_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_children_valid(nodep) == FAILURE) {
		RETURN_FALSE;
	}

	DOM_GET_OBJ(newchild, newnode, xmlNodePtr, newchildobj);
	DOM_GET_OBJ(oldchild, oldnode, xmlNodePtr, oldchildobj);

	if (!nodep->children) {
		RETURN_FALSE;
	}

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(nodep) == SUCCESS ||
		(newchild->parent != NULL && dom_node_is_read_only(newchild->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		RETURN_FALSE;
	}

	if (newchild->doc != nodep->doc && newchild->doc != NULL) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror);
		RETURN_FALSE;
	}

	if (dom_hierarchy(nodep, newchild) == FAILURE) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
		RETURN_FALSE;
	}

	if (oldchild->parent != nodep) {
		php_dom_throw_error(NOT_FOUND_ERR, stricterror);
		RETURN_FALSE;
	}

	if (newchild->type == XML_DOCUMENT_FRAG_NODE) {
		xmlNodePtr prevsib, nextsib;
		prevsib = oldchild->prev;
		nextsib = oldchild->next;

		xmlUnlinkNode(oldchild);

		xmlNodePtr last = newchild->last;
		newchild = _php_dom_insert_fragment(nodep, prevsib, nextsib, newchild, intern, newchildobj);
		if (newchild) {
			dom_reconcile_ns_list(nodep->doc, newchild, last);
		}
	} else if (oldchild != newchild) {
		xmlDtdPtr intSubset = xmlGetIntSubset(nodep->doc);
		replacedoctype = (intSubset == (xmlDtd *) oldchild);

		if (newchild->doc == NULL && nodep->doc != NULL) {
			xmlSetTreeDoc(newchild, nodep->doc);
			newchildobj->document = intern->document;
			php_libxml_increment_doc_ref((php_libxml_node_object *)newchildobj, NULL);
		}
		xmlReplaceNode(oldchild, newchild);
		dom_reconcile_ns(nodep->doc, newchild);

		if (replacedoctype) {
			nodep->doc->intSubset = (xmlDtd *) newchild;
		}
	}
	DOM_RET_OBJ(oldchild, &ret, intern);
}
/* }}} end dom_node_replace_child */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1734834066
Since:
*/
PHP_METHOD(DOMNode, removeChild)
{
	zval *id, *node;
	xmlNodePtr child, nodep;
	dom_object *intern, *childobj;
	int ret, stricterror;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node, dom_node_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_children_valid(nodep) == FAILURE) {
		RETURN_FALSE;
	}

	DOM_GET_OBJ(child, node, xmlNodePtr, childobj);

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(nodep) == SUCCESS ||
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		RETURN_FALSE;
	}

	if (!nodep->children || child->parent != nodep) {
		php_dom_throw_error(NOT_FOUND_ERR, stricterror);
		RETURN_FALSE;
	}

	xmlUnlinkNode(child);
	DOM_RET_OBJ(child, &ret, intern);
}
/* }}} end dom_node_remove_child */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-184E7107
Since:
*/
PHP_METHOD(DOMNode, appendChild)
{
	zval *id, *node;
	xmlNodePtr child, nodep, new_child = NULL;
	dom_object *intern, *childobj;
	int ret, stricterror;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node, dom_node_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_children_valid(nodep) == FAILURE) {
		RETURN_FALSE;
	}

	DOM_GET_OBJ(child, node, xmlNodePtr, childobj);

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(nodep) == SUCCESS ||
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		RETURN_FALSE;
	}

	if (dom_hierarchy(nodep, child) == FAILURE) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
		RETURN_FALSE;
	}

	if (!(child->doc == NULL || child->doc == nodep->doc)) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror);
		RETURN_FALSE;
	}

	if (child->type == XML_DOCUMENT_FRAG_NODE && child->children == NULL) {
		/* TODO Drop Warning? */
		php_error_docref(NULL, E_WARNING, "Document Fragment is empty");
		RETURN_FALSE;
	}

	if (child->doc == NULL && nodep->doc != NULL) {
		childobj->document = intern->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)childobj, NULL);
	}

	if (child->parent != NULL){
		xmlUnlinkNode(child);
	}

	if (child->type == XML_TEXT_NODE && nodep->last != NULL && nodep->last->type == XML_TEXT_NODE) {
		child->parent = nodep;
		if (child->doc == NULL) {
			xmlSetTreeDoc(child, nodep->doc);
		}
		new_child = child;
		if (nodep->children == NULL) {
			nodep->children = child;
			nodep->last = child;
		} else {
			child = nodep->last;
			child->next = new_child;
			new_child->prev = child;
			nodep->last = new_child;
		}
	} else 	if (child->type == XML_ATTRIBUTE_NODE) {
		xmlAttrPtr lastattr;

		if (child->ns == NULL)
			lastattr = xmlHasProp(nodep, child->name);
		else
			lastattr = xmlHasNsProp(nodep, child->name, child->ns->href);
		if (lastattr != NULL && lastattr->type != XML_ATTRIBUTE_DECL) {
			if (lastattr != (xmlAttrPtr) child) {
				xmlUnlinkNode((xmlNodePtr) lastattr);
				php_libxml_node_free_resource((xmlNodePtr) lastattr);
			}
		}
		new_child = xmlAddChild(nodep, child);
		if (UNEXPECTED(new_child == NULL)) {
			goto cannot_add;
		}
	} else if (child->type == XML_DOCUMENT_FRAG_NODE) {
		xmlNodePtr last = child->last;
		new_child = _php_dom_insert_fragment(nodep, nodep->last, NULL, child, intern, childobj);
		dom_reconcile_ns_list(nodep->doc, new_child, last);
	} else {
		new_child = xmlAddChild(nodep, child);
		if (UNEXPECTED(new_child == NULL)) {
			goto cannot_add;
		}
		dom_reconcile_ns(nodep->doc, new_child);
	}

	DOM_RET_OBJ(new_child, &ret, intern);
	return;
cannot_add:
	// TODO Convert to Error?
	php_error_docref(NULL, E_WARNING, "Couldn't append node");
	RETURN_FALSE;
}
/* }}} end dom_node_append_child */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-810594187
Since:
*/
PHP_METHOD(DOMNode, hasChildNodes)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

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

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-3A0ED0A4
Since:
*/
PHP_METHOD(DOMNode, cloneNode)
{
	zval *id;
	xmlNode *n, *node;
	int ret;
	dom_object *intern;
	bool recursive = 0;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &recursive) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(n, id, xmlNodePtr, intern);

	node = xmlDocCopyNode(n, n->doc, recursive);

	if (!node) {
		RETURN_FALSE;
	}

	/* When deep is false Element nodes still require the attributes
	Following taken from libxml as xmlDocCopyNode doesn't do this */
	if (n->type == XML_ELEMENT_NODE && recursive == 0) {
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

	/* If document cloned we want a new document proxy */
	if (node->doc != n->doc) {
		intern = NULL;
	}

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_node_clone_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-normalize
Since:
*/
PHP_METHOD(DOMNode, normalize)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	dom_normalize(nodep);

}
/* }}} end dom_node_normalize */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Level-2-Core-Node-supports
Since: DOM Level 2
*/
PHP_METHOD(DOMNode, isSupported)
{
	zend_string *feature, *version;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &feature, &version) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(dom_has_feature(feature, version));
}
/* }}} end dom_node_is_supported */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeHasAttrs
Since: DOM Level 2
*/
PHP_METHOD(DOMNode, hasAttributes)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (nodep->type != XML_ELEMENT_NODE)
		RETURN_FALSE;

	if (nodep->properties) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_node_has_attributes */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-isSameNode
Since: DOM Level 3
*/
PHP_METHOD(DOMNode, isSameNode)
{
	zval *id, *node;
	xmlNodePtr nodeotherp, nodep;
	dom_object *intern, *nodeotherobj;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node, dom_node_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	DOM_GET_OBJ(nodeotherp, node, xmlNodePtr, nodeotherobj);

	if (nodep == nodeotherp) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_node_is_same_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-lookupNamespacePrefix
Since: DOM Level 3
*/
PHP_METHOD(DOMNode, lookupPrefix)
{
	zval *id;
	xmlNodePtr nodep, lookupp = NULL;
	dom_object *intern;
	xmlNsPtr nsptr;
	size_t uri_len = 0;
	char *uri;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &uri, &uri_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

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

		if (lookupp != NULL) {
			nsptr = xmlSearchNsByHref(lookupp->doc, lookupp, (xmlChar *) uri);
			if (nsptr && nsptr->prefix != NULL) {
				RETURN_STRING((char *) nsptr->prefix);
			}
		}
	}

	RETURN_NULL();
}
/* }}} end dom_node_lookup_prefix */

/* {{{ URL: http://www.w3.org/TR/DOM-Level-3-Core/core.html#Node3-isDefaultNamespace
Since: DOM Level 3
*/
PHP_METHOD(DOMNode, isDefaultNamespace)
{
	zval *id;
	xmlNodePtr nodep;
	dom_object *intern;
	xmlNsPtr nsptr;
	size_t uri_len = 0;
	char *uri;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &uri, &uri_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);
	if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
		nodep = xmlDocGetRootElement((xmlDocPtr) nodep);
	}

	if (nodep && uri_len > 0) {
		nsptr = xmlSearchNs(nodep->doc, nodep, NULL);
		if (nsptr && xmlStrEqual(nsptr->href, (xmlChar *) uri)) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} end dom_node_is_default_namespace */

/* {{{ URL: http://www.w3.org/TR/DOM-Level-3-Core/core.html#Node3-lookupNamespaceURI
Since: DOM Level 3
*/
PHP_METHOD(DOMNode, lookupNamespaceURI)
{
	zval *id;
	xmlNodePtr nodep;
	dom_object *intern;
	xmlNsPtr nsptr;
	size_t prefix_len;
	char *prefix;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!", &prefix, &prefix_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);
	if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
		nodep = xmlDocGetRootElement((xmlDocPtr) nodep);
		if (nodep == NULL) {
			RETURN_NULL();
		}
	}

	nsptr = xmlSearchNs(nodep->doc, nodep, (xmlChar *) prefix);
	if (nsptr && nsptr->href != NULL) {
		RETURN_STRING((char *) nsptr->href);
	}

	RETURN_NULL();
}
/* }}} end dom_node_lookup_namespace_uri */

static void dom_canonicalization(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	zval *id;
	zval *xpath_array=NULL, *ns_prefixes=NULL;
	xmlNodePtr nodep;
	xmlDocPtr docp;
	xmlNodeSetPtr nodeset = NULL;
	dom_object *intern;
	bool exclusive=0, with_comments=0;
	xmlChar **inclusive_ns_prefixes = NULL;
	char *file = NULL;
	int ret = -1;
	size_t file_len = 0;
	xmlOutputBufferPtr buf;
	xmlXPathContextPtr ctxp=NULL;
	xmlXPathObjectPtr xpathobjp=NULL;

	id = ZEND_THIS;
	if (mode == 0) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(),
			"|bba!a!", &exclusive, &with_comments,
			&xpath_array, &ns_prefixes) == FAILURE) {
			RETURN_THROWS();
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(),
			"s|bba!a!", &file, &file_len, &exclusive,
			&with_comments, &xpath_array, &ns_prefixes) == FAILURE) {
			RETURN_THROWS();
		}
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	docp = nodep->doc;

	if (! docp) {
		zend_throw_error(NULL, "Node must be associated with a document");
		RETURN_THROWS();
	}

	if (xpath_array == NULL) {
		if (nodep->type != XML_DOCUMENT_NODE) {
			ctxp = xmlXPathNewContext(docp);
			ctxp->node = nodep;
			xpathobjp = xmlXPathEvalExpression((xmlChar *) "(.//. | .//@* | .//namespace::*)", ctxp);
			ctxp->node = NULL;
			if (xpathobjp && xpathobjp->type == XPATH_NODESET) {
				nodeset = xpathobjp->nodesetval;
			} else {
				if (xpathobjp) {
					xmlXPathFreeObject(xpathobjp);
				}
				xmlXPathFreeContext(ctxp);
				zend_throw_error(NULL, "XPath query did not return a nodeset");
				RETURN_THROWS();
			}
		}
	} else {
		/*xpath query from xpath_array */
		HashTable *ht = Z_ARRVAL_P(xpath_array);
		zval *tmp;
		char *xquery;

		tmp = zend_hash_str_find(ht, "query", sizeof("query")-1);
		if (!tmp) {
			/* if mode == 0 then $xpath arg is 3, if mode == 1 then $xpath is 4 */
			zend_argument_value_error(3 + mode, "must have a \"query\" key");
			RETURN_THROWS();
		}
		if (Z_TYPE_P(tmp) != IS_STRING) {
			/* if mode == 0 then $xpath arg is 3, if mode == 1 then $xpath is 4 */
			zend_argument_type_error(3 + mode, "\"query\" option must be a string, %s given", zend_zval_type_name(tmp));
			RETURN_THROWS();
		}
		xquery = Z_STRVAL_P(tmp);

		ctxp = xmlXPathNewContext(docp);
		ctxp->node = nodep;

		tmp = zend_hash_str_find(ht, "namespaces", sizeof("namespaces")-1);
		if (tmp && Z_TYPE_P(tmp) == IS_ARRAY && !HT_IS_PACKED(Z_ARRVAL_P(tmp))) {
			zval *tmpns;
			zend_string *prefix;

			ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(tmp), prefix, tmpns) {
				if (Z_TYPE_P(tmpns) == IS_STRING) {
					if (prefix) {
						xmlXPathRegisterNs(ctxp, (xmlChar *) ZSTR_VAL(prefix), (xmlChar *) Z_STRVAL_P(tmpns));
					}
				}
			} ZEND_HASH_FOREACH_END();
		}

		xpathobjp = xmlXPathEvalExpression((xmlChar *) xquery, ctxp);
		ctxp->node = NULL;
		if (xpathobjp && xpathobjp->type == XPATH_NODESET) {
			nodeset = xpathobjp->nodesetval;
		} else {
			if (xpathobjp) {
				xmlXPathFreeObject(xpathobjp);
			}
			xmlXPathFreeContext(ctxp);
			zend_throw_error(NULL, "XPath query did not return a nodeset");
			RETURN_THROWS();
		}
	}

	if (ns_prefixes != NULL) {
		if (exclusive) {
			zval *tmpns;
			int nscount = 0;

			inclusive_ns_prefixes = safe_emalloc(zend_hash_num_elements(Z_ARRVAL_P(ns_prefixes)) + 1,
				sizeof(xmlChar *), 0);
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(ns_prefixes), tmpns) {
				if (Z_TYPE_P(tmpns) == IS_STRING) {
					inclusive_ns_prefixes[nscount++] = (xmlChar *) Z_STRVAL_P(tmpns);
				}
			} ZEND_HASH_FOREACH_END();
			inclusive_ns_prefixes[nscount] = NULL;
		} else {
			php_error_docref(NULL, E_NOTICE,
				"Inclusive namespace prefixes only allowed in exclusive mode.");
		}
	}

	if (mode == 1) {
		buf = xmlOutputBufferCreateFilename(file, NULL, 0);
	} else {
		buf = xmlAllocOutputBuffer(NULL);
	}

	if (buf != NULL) {
		ret = xmlC14NDocSaveTo(docp, nodeset, exclusive, inclusive_ns_prefixes,
			with_comments, buf);
	}

	if (inclusive_ns_prefixes != NULL) {
		efree(inclusive_ns_prefixes);
	}
	if (xpathobjp != NULL) {
		xmlXPathFreeObject(xpathobjp);
	}
	if (ctxp != NULL) {
		xmlXPathFreeContext(ctxp);
	}

	if (buf == NULL || ret < 0) {
		RETVAL_FALSE;
	} else {
		if (mode == 0) {
#ifdef LIBXML2_NEW_BUFFER
			ret = xmlOutputBufferGetSize(buf);
#else
			ret = buf->buffer->use;
#endif
			if (ret > 0) {
#ifdef LIBXML2_NEW_BUFFER
				RETVAL_STRINGL((char *) xmlOutputBufferGetContent(buf), ret);
#else
				RETVAL_STRINGL((char *) buf->buffer->content, ret);
#endif
			} else {
				RETVAL_EMPTY_STRING();
			}
		}
	}

	if (buf) {
		int bytes;

		bytes = xmlOutputBufferClose(buf);
		if (mode == 1 && (ret >= 0)) {
			RETURN_LONG(bytes);
		}
	}
}
/* }}} */

/* {{{ Canonicalize nodes to a string */
PHP_METHOD(DOMNode, C14N)
{
	dom_canonicalization(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Canonicalize nodes to a file */
PHP_METHOD(DOMNode, C14NFile)
{
	dom_canonicalization(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Gets an xpath for a node */
PHP_METHOD(DOMNode, getNodePath)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	char *value;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	value = (char *) xmlGetNodePath(nodep);
	if (value == NULL) {
		/* TODO Research if can return empty string */
		RETURN_NULL();
	} else {
		RETVAL_STRING(value);
		xmlFree(value);
	}
}
/* }}} */

/* {{{ Gets line number for a node */
PHP_METHOD(DOMNode, getLineNo)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	RETURN_LONG(xmlGetLineNo(nodep));
}
/* }}} */

/**
 * We want to block the serialization and unserialization of DOM classes.
 * However, using @not-serializable makes the child classes also not serializable, even if the user implements the methods.
 * So instead, we implement the methods wherein we throw exceptions.
 * The reason we choose these methods is because:
 *   - If the user implements __serialize / __unserialize, the respective throwing methods are not called.
 *   - If the user implements __sleep / __wakeup, then it's also not a problem because they will not enter the throwing methods.
 */

PHP_METHOD(DOMNode, __sleep)
{
	if (zend_parse_parameters_none() != SUCCESS) {
		RETURN_THROWS();
	}

	zend_throw_exception_ex(NULL, 0, "Serialization of '%s' is not allowed, unless serialization methods are implemented in a subclass", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
	RETURN_THROWS();
}

PHP_METHOD(DOMNode, __wakeup)
{
	if (zend_parse_parameters_none() != SUCCESS) {
		RETURN_THROWS();
	}

	zend_throw_exception_ex(NULL, 0, "Unserialization of '%s' is not allowed, unless unserialization methods are implemented in a subclass", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
	RETURN_THROWS();
}

#endif
