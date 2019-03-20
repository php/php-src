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
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"

const zend_function_entry php_dom_parent_node_class_functions[] = { /* {{{ */
	PHP_FE_END
};

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

void dom_parent_node_append(dom_object *context, zval *nodes, int nodesc)
{
	int i;
	xmlNode *documentNode;
	xmlNode *fragment;
	xmlNode *newNode;
	xmlNode *contextNode = dom_object_get_node(context);

	if (contextNode->type == XML_DOCUMENT_NODE || contextNode->type == XML_HTML_DOCUMENT_NODE) {
		documentNode = contextNode;
	} else {
		documentNode = contextNode->doc;
	}

	fragment = xmlNewDocFragment(documentNode);

	if (!fragment) {
		// err?
		return;
	}

	for (i = 0; i < nodesc; i++) {
		switch (Z_TYPE(nodes[i])) {
			case IS_STRING:
				newNode = xmlNewDocText(documentNode, (xmlChar *) Z_STRVAL(nodes[i]));

				xmlSetTreeDoc(newNode, documentNode);

				newNode = xmlAddChild(fragment, newNode);

				if (!newNode) {
					php_error_docref(NULL, E_WARNING, "Couldn't append node");
					return;
				}

				break;

			case IS_OBJECT:
				break;
		}
	}

	xmlNodePtr newchild, node, prevsib;

	newchild = fragment->children;
	prevsib = contextNode->last;

	if (newchild) {
		prevsib->next = newchild;
		newchild->prev = prevsib;
		contextNode->last = fragment->last;

		node = newchild;
		while (node != NULL) {
			node->parent = contextNode;
			if (node->doc != documentNode) {
				xmlSetTreeDoc(node, documentNode);
			}
			if (node == fragment->last) {
				break;
			}
			node = node->next;
		}

		fragment->children = NULL;
		fragment->last = NULL;
	}
}

#endif
