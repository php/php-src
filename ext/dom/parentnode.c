/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
		php_dom_throw_error(INVALID_STATE_ERR, 1);
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
		php_dom_throw_error(INVALID_STATE_ERR, 1);
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
		php_dom_throw_error(INVALID_STATE_ERR, 1);
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
					php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror);
					goto err;
				}

				if (newNode->parent != NULL) {
					xmlUnlinkNode(newNode);
				}

				newNodeObj->document = document;
				xmlSetTreeDoc(newNode, documentNode);

				if (newNode->type == XML_ATTRIBUTE_NODE) {
					goto hierarchy_request_err;
				}

				/*
				 * xmlNewDocText function will always returns same address to the second parameter if the parameters are greater than or equal to three.
				 * If it's text, that's fine, but if it's an object, it can cause invalid pointer because many new nodes point to the same memory address.
				 * So we must copy the new node to avoid this situation.
				 */
				if (nodesc > 1) {
					newNode = xmlCopyNode(newNode, 1);
				}

				if (!xmlAddChild(fragment, newNode)) {
					if (nodesc > 1) {
						xmlFreeNode(newNode);
					}
					goto hierarchy_request_err;
				}

				continue;
			} else {
				zend_argument_type_error(i + 1, "must be of type DOMNode|string, %s given", zend_zval_value_name(&nodes[i]));
				goto err;
			}
		} else if (Z_TYPE(nodes[i]) == IS_STRING) {
			newNode = xmlNewDocText(documentNode, (xmlChar *) Z_STRVAL(nodes[i]));

			xmlSetTreeDoc(newNode, documentNode);

			if (!xmlAddChild(fragment, newNode)) {
				xmlFreeNode(newNode);
				goto hierarchy_request_err;
			}
		} else {
			zend_argument_type_error(i + 1, "must be of type DOMNode|string, %s given", zend_zval_value_name(&nodes[i]));
			goto err;
		}
	}

	return fragment;

hierarchy_request_err:
	php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
err:
	xmlFreeNode(fragment);
	return NULL;
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
	xmlNode *fragment, *nextsib;
	xmlDoc *doc;
	bool afterlastchild;

	int stricterror = dom_get_strict_error(context->document);

	if (!prevsib->parent) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		return;
	}

	doc = prevsib->doc;
	parentNode = prevsib->parent;
	nextsib = prevsib->next;
	afterlastchild = (nextsib == NULL);
	fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);

	if (fragment == NULL) {
		return;
	}

	newchild = fragment->children;

	if (newchild) {
		/* first node and last node are both both parameters to DOMElement::after() method so nextsib and prevsib are null. */
		if (!parentNode->children) {
			prevsib = nextsib = NULL;
		} else if (afterlastchild) {
			/*
			 * The new node will be inserted after last node, prevsib is last node.
			 * The first node is the parameter to DOMElement::after() if parentNode->children == prevsib is true
			 * and prevsib does not change, otherwise prevsib is parentNode->last (first node).
			 */
			prevsib = parentNode->children == prevsib ? prevsib : parentNode->last;
		} else {
			/*
			 * The new node will be inserted after first node, prevsib is first node.
			 * The first node is not the parameter to DOMElement::after() if parentNode->children == prevsib is true
			 * and prevsib does not change otherwise prevsib is null to mean that parentNode->children is the new node.
			 */
			prevsib = parentNode->children == prevsib ? prevsib : NULL;
		}

		if (prevsib) {
			fragment->last->next = prevsib->next;
			if (prevsib->next) {
				prevsib->next->prev = fragment->last;
			}
			prevsib->next = newchild;
		} else {
			parentNode->children = newchild;
			if (nextsib) {
				fragment->last->next = nextsib;
				nextsib->prev = fragment->last;
			}
		}

		newchild->prev = prevsib;
		dom_fragment_assign_parent_node(parentNode, fragment);
		dom_reconcile_ns(doc, newchild);
	}

	xmlFree(fragment);
}

void dom_parent_node_before(dom_object *context, zval *nodes, int nodesc)
{
	xmlNode *nextsib = dom_object_get_node(context);
	xmlNodePtr newchild, prevsib, parentNode;
	xmlNode *fragment, *afternextsib;
	xmlDoc *doc;
	bool beforefirstchild;

	doc = nextsib->doc;
	prevsib = nextsib->prev;
	afternextsib = nextsib->next;
	parentNode = nextsib->parent;
	beforefirstchild = !prevsib;
	fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);

	if (fragment == NULL) {
		return;
	}

	newchild = fragment->children;

	if (newchild) {
		/* first node and last node are both both parameters to DOMElement::before() method so nextsib is null. */
		if (!parentNode->children) {
			nextsib = NULL;
		} else if (beforefirstchild) {
			/*
			 * The new node will be inserted before first node, nextsib is first node and afternextsib is last node.
			 * The first node is not the parameter to DOMElement::before() if parentNode->children == nextsib is true
			 * and nextsib does not change, otherwise nextsib is the last node.
			 */
			nextsib = parentNode->children == nextsib ? nextsib : afternextsib;
		} else {
			/*
			 * The new node will be inserted before last node, prevsib is first node and nestsib is last node.
			 * The first node is not the parameter to DOMElement::before() if parentNode->children == prevsib is true
			 * but last node may be, so use prevsib->next to determine the value of nextsib, otherwise nextsib does not change.
			 */
			nextsib = parentNode->children == prevsib ? prevsib->next : nextsib;
		}

		if (parentNode->children == nextsib) {
			parentNode->children = newchild;
		} else {
			prevsib->next = newchild;
		}

		fragment->last->next = nextsib;
		if (nextsib) {
			nextsib->prev = fragment->last;
		}

		newchild->prev = prevsib;

		dom_fragment_assign_parent_node(parentNode, fragment);
		dom_reconcile_ns(doc, newchild);
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
