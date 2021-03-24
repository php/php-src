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
   | Authors: Benjamin Eberlei <beberlei@php.net>                         |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"

/* {{{ firstElementChild DomParentNode
readonly=yes
URL: https://www.w3.org/TR/dom/#dom-parentnode-firstelementchild
*/
int dom_parent_node_first_element_child_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *first = NULL;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	if (dom_node_children_valid(nodep) == SUCCESS) {
		first = nodep->children;

		while (first && first->type != XML_ELEMENT_NODE) {
			first = first->next;
		}
	}

	if (!first) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(first, retval, obj);
	return SUCCESS;
}
/* }}} */

/* {{{ lastElementChild DomParentNode
readonly=yes
URL: https://www.w3.org/TR/dom/#dom-parentnode-lastelementchild
*/
int dom_parent_node_last_element_child_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *last = NULL;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	if (dom_node_children_valid(nodep) == SUCCESS) {
		last = nodep->last;

		while (last && last->type != XML_ELEMENT_NODE) {
			last = last->prev;
		}
	}

	if (!last) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(last, retval, obj);
	return SUCCESS;
}
/* }}} */

/* {{{ childElementCount DomParentNode
readonly=yes
https://www.w3.org/TR/dom/#dom-parentnode-childelementcount
*/
int dom_parent_node_child_element_count(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *first = NULL;
	zend_long count = 0;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	if (dom_node_children_valid(nodep) == SUCCESS) {
		first = nodep->children;

		while (first != NULL) {
			if (first->type == XML_ELEMENT_NODE) {
				count++;
			}

			first = first->next;
		}
	}

	ZVAL_LONG(retval, count);

	return SUCCESS;
}
/* }}} */

xmlNode* dom_zvals_to_fragment(php_libxml_ref_obj *document, xmlNode *contextNode, zval *nodes, int nodesc)
{
	int i;
	xmlDoc *documentNode;
	xmlNode *fragment;
	xmlNode *newNode;
	zend_class_entry *ce;
	dom_object *newNodeObj;
	int stricterror;

	if (document == NULL) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, 1);
		return NULL;
	}

	if (contextNode->type == XML_DOCUMENT_NODE || contextNode->type == XML_HTML_DOCUMENT_NODE) {
		documentNode = (xmlDoc *) contextNode;
	} else {
		documentNode = contextNode->doc;
	}

	fragment = xmlNewDocFragment(documentNode);

	if (!fragment) {
		return NULL;
	}

	stricterror = dom_get_strict_error(document);

	for (i = 0; i < nodesc; i++) {
		if (Z_TYPE(nodes[i]) == IS_OBJECT) {
			ce = Z_OBJCE(nodes[i]);

			if (instanceof_function(ce, dom_node_class_entry)) {
				newNodeObj = Z_DOMOBJ_P(&nodes[i]);
				newNode = dom_object_get_node(newNodeObj);

				if (newNode->doc != documentNode) {
					xmlFree(fragment);
					php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror);
					return NULL;
				}

				if (newNode->parent != NULL) {
					xmlUnlinkNode(newNode);
				}

				newNodeObj->document = document;
				xmlSetTreeDoc(newNode, documentNode);

				if (newNode->type == XML_ATTRIBUTE_NODE) {
					xmlFree(fragment);

					php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
					return NULL;
				}

				if (!xmlAddChild(fragment, newNode)) {
					xmlFree(fragment);

					php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
					return NULL;
				}

				continue;
			} else {
				xmlFree(fragment);

				zend_argument_type_error(i + 1, "must be of type DOMNode|string, %s given", zend_zval_type_name(&nodes[i]));
				return NULL;
			}
		} else if (Z_TYPE(nodes[i]) == IS_STRING) {
			newNode = xmlNewDocText(documentNode, (xmlChar *) Z_STRVAL(nodes[i]));

			xmlSetTreeDoc(newNode, documentNode);

			if (!xmlAddChild(fragment, newNode)) {
				xmlFree(fragment);

				return NULL;
			}
		} else {
			xmlFree(fragment);

			zend_argument_type_error(i + 1, "must be of type DOMNode|string, %s given", zend_zval_type_name(&nodes[i]));

			return NULL;
		}
	}

	return fragment;
}

static void dom_fragment_assign_parent_node(xmlNodePtr parentNode, xmlNodePtr fragment)
{
	xmlNodePtr node = fragment->children;

	while (node != NULL) {
		node->parent = parentNode;

		if (node == fragment->last) {
			break;
		}
		node = node->next;
	}

	fragment->children = NULL;
	fragment->last = NULL;
}

void dom_parent_node_append(dom_object *context, zval *nodes, int nodesc)
{
	xmlNode *parentNode = dom_object_get_node(context);
	xmlNodePtr newchild, prevsib;
	xmlNode *fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);

	if (fragment == NULL) {
		return;
	}

	newchild = fragment->children;
	prevsib = parentNode->last;

	if (newchild) {
		if (prevsib != NULL) {
			prevsib->next = newchild;
		} else {
			parentNode->children = newchild;
		}

		parentNode->last = fragment->last;

		newchild->prev = prevsib;

		dom_fragment_assign_parent_node(parentNode, fragment);

		dom_reconcile_ns(parentNode->doc, newchild);
	}

	xmlFree(fragment);
}

void dom_parent_node_prepend(dom_object *context, zval *nodes, int nodesc)
{
	xmlNode *parentNode = dom_object_get_node(context);

	if (parentNode->children == NULL) {
		dom_parent_node_append(context, nodes, nodesc);
		return;
	}

	xmlNodePtr newchild, nextsib;
	xmlNode *fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);

	if (fragment == NULL) {
		return;
	}

	newchild = fragment->children;
	nextsib = parentNode->children;

	if (newchild) {
		parentNode->children = newchild;
		fragment->last->next = nextsib;
		nextsib->prev = fragment->last;

		dom_fragment_assign_parent_node(parentNode, fragment);

		dom_reconcile_ns(parentNode->doc, newchild);
	}

	xmlFree(fragment);
}

void dom_parent_node_after(dom_object *context, zval *nodes, int nodesc)
{
	xmlNode *prevsib = dom_object_get_node(context);
	xmlNodePtr newchild, parentNode;
	xmlNode *fragment;

	int stricterror = dom_get_strict_error(context->document);

	if (!prevsib->parent) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		return;
	}

	parentNode = prevsib->parent;
	fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);

	if (fragment == NULL) {
		return;
	}

	newchild = fragment->children;

	if (newchild) {
		fragment->last->next = prevsib->next;
		prevsib->next = newchild;

		newchild->prev = prevsib;

		dom_fragment_assign_parent_node(parentNode, fragment);
		dom_reconcile_ns(prevsib->doc, newchild);
	}

	xmlFree(fragment);
}

void dom_parent_node_before(dom_object *context, zval *nodes, int nodesc)
{
	xmlNode *nextsib = dom_object_get_node(context);
	xmlNodePtr newchild, prevsib, parentNode;
	xmlNode *fragment;

	prevsib = nextsib->prev;
	parentNode = nextsib->parent;
	fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);

	if (fragment == NULL) {
		return;
	}

	newchild = fragment->children;

	if (newchild) {
		if (parentNode->children == nextsib) {
			parentNode->children = newchild;
		} else {
			prevsib->next = newchild;
		}
		fragment->last->next = nextsib;
		nextsib->prev = fragment->last;

		newchild->prev = prevsib;

		dom_fragment_assign_parent_node(parentNode, fragment);

		dom_reconcile_ns(nextsib->doc, newchild);
	}

	xmlFree(fragment);
}

void dom_child_node_remove(dom_object *context)
{
	xmlNode *child = dom_object_get_node(context);
	xmlNodePtr children;
	int stricterror;

	stricterror = dom_get_strict_error(context->document);

	if (dom_node_is_read_only(child) == SUCCESS ||
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		return;
	}

	if (!child->parent) {
		php_dom_throw_error(NOT_FOUND_ERR, stricterror);
		return;
	}

	if (dom_node_children_valid(child->parent) == FAILURE) {
		return;
	}

	children = child->parent->children;
	if (!children) {
		php_dom_throw_error(NOT_FOUND_ERR, stricterror);
		return;
	}

	while (children) {
		if (children == child) {
			xmlUnlinkNode(child);
			return;
		}
		children = children->next;
	}

	php_dom_throw_error(NOT_FOUND_ERR, stricterror);
}

#endif
