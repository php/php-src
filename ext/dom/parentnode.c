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

static bool dom_is_node_in_list(const zval *nodes, uint32_t nodesc, const xmlNodePtr node_to_find)
{
	for (uint32_t i = 0; i < nodesc; i++) {
		if (Z_TYPE(nodes[i]) == IS_OBJECT) {
			const zend_class_entry *ce = Z_OBJCE(nodes[i]);

			if (instanceof_function(ce, dom_node_class_entry)) {
				if (dom_object_get_node(Z_DOMOBJ_P(nodes + i)) == node_to_find) {
					return true;
				}
			}
		}
	}

	return false;
}

xmlNode* dom_zvals_to_fragment(php_libxml_ref_obj *document, xmlNode *contextNode, zval *nodes, uint32_t nodesc)
{
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

	for (uint32_t i = 0; i < nodesc; i++) {
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

				/* Citing from the docs (https://gnome.pages.gitlab.gnome.org/libxml2/devhelp/libxml2-tree.html#xmlAddChild): 
				 * "Add a new node to @parent, at the end of the child (or property) list merging adjacent TEXT nodes (in which case @cur is freed)".
				 * So we must take a copy if this situation arises to prevent a use-after-free. */
				bool will_free = newNode->type == XML_TEXT_NODE && fragment->last && fragment->last->type == XML_TEXT_NODE;
				if (will_free) {
					newNode = xmlCopyNode(newNode, 1);
				}

				if (!xmlAddChild(fragment, newNode)) {
					if (will_free) {
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

static zend_result dom_hierarchy_node_list(xmlNodePtr parentNode, zval *nodes, uint32_t nodesc)
{
	for (uint32_t i = 0; i < nodesc; i++) {
		if (Z_TYPE(nodes[i]) == IS_OBJECT) {
			const zend_class_entry *ce = Z_OBJCE(nodes[i]);

			if (instanceof_function(ce, dom_node_class_entry)) {
				if (dom_hierarchy(parentNode, dom_object_get_node(Z_DOMOBJ_P(nodes + i))) != SUCCESS) {
					return FAILURE;
				}
			}
		}
	}

	return SUCCESS;
}

void dom_parent_node_append(dom_object *context, zval *nodes, uint32_t nodesc)
{
	xmlNode *parentNode = dom_object_get_node(context);
	xmlNodePtr newchild, prevsib;

	if (UNEXPECTED(dom_hierarchy_node_list(parentNode, nodes, nodesc) != SUCCESS)) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, dom_get_strict_error(context->document));
		return;
	}

	php_libxml_invalidate_node_list_cache_from_doc(parentNode->doc);

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

		xmlNodePtr last = fragment->last;
		parentNode->last = last;

		newchild->prev = prevsib;

		dom_fragment_assign_parent_node(parentNode, fragment);

		dom_reconcile_ns_list(parentNode->doc, newchild, last);
	}

	xmlFree(fragment);
}

void dom_parent_node_prepend(dom_object *context, zval *nodes, uint32_t nodesc)
{
	xmlNode *parentNode = dom_object_get_node(context);

	if (parentNode->children == NULL) {
		dom_parent_node_append(context, nodes, nodesc);
		return;
	}

	if (UNEXPECTED(dom_hierarchy_node_list(parentNode, nodes, nodesc) != SUCCESS)) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, dom_get_strict_error(context->document));
		return;
	}

	php_libxml_invalidate_node_list_cache_from_doc(parentNode->doc);

	xmlNodePtr newchild, nextsib;
	xmlNode *fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);

	if (fragment == NULL) {
		return;
	}

	newchild = fragment->children;
	nextsib = parentNode->children;

	if (newchild) {
		xmlNodePtr last = fragment->last;
		parentNode->children = newchild;
		fragment->last->next = nextsib;
		nextsib->prev = last;

		dom_fragment_assign_parent_node(parentNode, fragment);

		dom_reconcile_ns_list(parentNode->doc, newchild, last);
	}

	xmlFree(fragment);
}

static void dom_pre_insert(xmlNodePtr insertion_point, xmlNodePtr parentNode, xmlNodePtr newchild, xmlNodePtr fragment)
{
	if (!insertion_point) {
		/* Place it as last node */
		if (parentNode->children) {
			/* There are children */
			fragment->last->prev = parentNode->last;
			newchild->prev = parentNode->last->prev;
			parentNode->last->next = newchild;
		} else {
			/* No children, because they moved out when they became a fragment */
			parentNode->children = newchild;
			parentNode->last = newchild;
		}
	} else {
		/* Insert fragment before insertion_point */
		fragment->last->next = insertion_point;
		if (insertion_point->prev) {
			insertion_point->prev->next = newchild;
			newchild->prev = insertion_point->prev;
		}
		insertion_point->prev = newchild;
		if (parentNode->children == insertion_point) {
			parentNode->children = newchild;
		}
	}
}

void dom_parent_node_after(dom_object *context, zval *nodes, uint32_t nodesc)
{
	/* Spec link: https://dom.spec.whatwg.org/#dom-childnode-after */

	xmlNode *prevsib = dom_object_get_node(context);
	xmlNodePtr newchild, parentNode;
	xmlNode *fragment;
	xmlDoc *doc;

	/* Spec step 1 */
	parentNode = prevsib->parent;
	/* Spec step 2 */
	if (!parentNode) {
		int stricterror = dom_get_strict_error(context->document);
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
		return;
	}

	/* Spec step 3: find first following child not in nodes; otherwise null */
	xmlNodePtr viable_next_sibling = prevsib->next;
	while (viable_next_sibling) {
		if (!dom_is_node_in_list(nodes, nodesc, viable_next_sibling)) {
			break;
		}
		viable_next_sibling = viable_next_sibling->next;
	}

	doc = prevsib->doc;

	php_libxml_invalidate_node_list_cache_from_doc(doc);

	/* Spec step 4: convert nodes into fragment */
	fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);

	if (fragment == NULL) {
		return;
	}

	newchild = fragment->children;

	if (newchild) {
		xmlNodePtr last = fragment->last;

		/* Step 5: place fragment into the parent before viable_next_sibling */
		dom_pre_insert(viable_next_sibling, parentNode, newchild, fragment);

		dom_fragment_assign_parent_node(parentNode, fragment);
		dom_reconcile_ns_list(doc, newchild, last);
	}

	xmlFree(fragment);
}

void dom_parent_node_before(dom_object *context, zval *nodes, uint32_t nodesc)
{
	/* Spec link: https://dom.spec.whatwg.org/#dom-childnode-before */

	xmlNode *nextsib = dom_object_get_node(context);
	xmlNodePtr newchild, parentNode;
	xmlNode *fragment;
	xmlDoc *doc;

	/* Spec step 1 */
	parentNode = nextsib->parent;
	/* Spec step 2 */
	if (!parentNode) {
		int stricterror = dom_get_strict_error(context->document);
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
		return;
	}

	/* Spec step 3: find first following child not in nodes; otherwise null */
	xmlNodePtr viable_previous_sibling = nextsib->prev;
	while (viable_previous_sibling) {
		if (!dom_is_node_in_list(nodes, nodesc, viable_previous_sibling)) {
			break;
		}
		viable_previous_sibling = viable_previous_sibling->prev;
	}

	doc = nextsib->doc;

	php_libxml_invalidate_node_list_cache_from_doc(doc);

	/* Spec step 4: convert nodes into fragment */
	fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);

	if (fragment == NULL) {
		return;
	}

	newchild = fragment->children;

	if (newchild) {
		xmlNodePtr last = fragment->last;

		/* Step 5: if viable_previous_sibling is null, set it to the parent's first child, otherwise viable_previous_sibling's next sibling */
		if (!viable_previous_sibling) {
			viable_previous_sibling = parentNode->children;
		} else {
			viable_previous_sibling = viable_previous_sibling->next;
		}
		/* Step 6: place fragment into the parent after viable_previous_sibling */
		dom_pre_insert(viable_previous_sibling, parentNode, newchild, fragment);

		dom_fragment_assign_parent_node(parentNode, fragment);
		dom_reconcile_ns_list(doc, newchild, last);
	}

	xmlFree(fragment);
}

static zend_result dom_child_removal_preconditions(const xmlNodePtr child, int stricterror)
{
	if (dom_node_is_read_only(child) == SUCCESS ||
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		return FAILURE;
	}

	if (!child->parent) {
		php_dom_throw_error(NOT_FOUND_ERR, stricterror);
		return FAILURE;
	}

	if (dom_node_children_valid(child->parent) == FAILURE) {
		return FAILURE;
	}

	xmlNodePtr children = child->parent->children;
	if (!children) {
		php_dom_throw_error(NOT_FOUND_ERR, stricterror);
		return FAILURE;
	}

	return SUCCESS;
}

void dom_child_node_remove(dom_object *context)
{
	xmlNode *child = dom_object_get_node(context);
	xmlNodePtr children;
	int stricterror;

	stricterror = dom_get_strict_error(context->document);

	if (UNEXPECTED(dom_child_removal_preconditions(child, stricterror) != SUCCESS)) {
		return;
	}

	children = child->parent->children;

	php_libxml_invalidate_node_list_cache_from_doc(context->document->ptr);

	while (children) {
		if (children == child) {
			xmlUnlinkNode(child);
			return;
		}
		children = children->next;
	}

	php_dom_throw_error(NOT_FOUND_ERR, stricterror);
}

void dom_child_replace_with(dom_object *context, zval *nodes, uint32_t nodesc)
{
	xmlNodePtr child = dom_object_get_node(context);
	xmlNodePtr parentNode = child->parent;

	int stricterror = dom_get_strict_error(context->document);
	if (UNEXPECTED(dom_child_removal_preconditions(child, stricterror) != SUCCESS)) {
		return;
	}

	xmlNodePtr insertion_point = child->next;

	xmlNodePtr fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);
	if (UNEXPECTED(fragment == NULL)) {
		return;
	}

	xmlNodePtr newchild = fragment->children;
	xmlDocPtr doc = parentNode->doc;

	if (newchild) {
		xmlNodePtr last = fragment->last;

		/* Unlink and free it unless it became a part of the fragment. */
		if (child->parent != fragment) {
			xmlUnlinkNode(child);
		}

		dom_pre_insert(insertion_point, parentNode, newchild, fragment);

		dom_fragment_assign_parent_node(parentNode, fragment);
		dom_reconcile_ns_list(doc, newchild, last);
	}

	xmlFree(fragment);
}

#endif
