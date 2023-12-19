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
zend_result dom_parent_node_first_element_child_read(dom_object *obj, zval *retval)
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
zend_result dom_parent_node_last_element_child_read(dom_object *obj, zval *retval)
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
zend_result dom_parent_node_child_element_count(dom_object *obj, zval *retval)
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
		if (Z_TYPE(nodes[i]) == IS_OBJECT && instanceof_function(Z_OBJCE(nodes[i]), dom_node_class_entry)) {
			if (dom_object_get_node(Z_DOMOBJ_P(nodes + i)) == node_to_find) {
				return true;
			}
		}
	}

	return false;
}

static xmlDocPtr dom_doc_from_context_node(xmlNodePtr contextNode)
{
	if (contextNode->type == XML_DOCUMENT_NODE || contextNode->type == XML_HTML_DOCUMENT_NODE) {
		return (xmlDocPtr) contextNode;
	} else {
		return contextNode->doc;
	}
}

/* Citing from the docs (https://gnome.pages.gitlab.gnome.org/libxml2/devhelp/libxml2-tree.html#xmlAddChild): 
 * "Add a new node to @parent, at the end of the child (or property) list merging adjacent TEXT nodes (in which case @cur is freed)".
 * So we must use a custom way of adding that does not merge. */
static void dom_add_child_without_merging(xmlNodePtr parent, xmlNodePtr child)
{
	if (parent->children == NULL) {
		parent->children = child;
	} else {
		xmlNodePtr last = parent->last;
		last->next = child;
		child->prev = last;
	}
	parent->last = child;
	child->parent = parent;
}

xmlNode* dom_zvals_to_fragment(php_libxml_ref_obj *document, xmlNode *contextNode, zval *nodes, int nodesc)
{
	xmlDoc *documentNode;
	xmlNode *fragment;
	xmlNode *newNode;
	dom_object *newNodeObj;

	documentNode = dom_doc_from_context_node(contextNode);

	fragment = xmlNewDocFragment(documentNode);

	if (!fragment) {
		return NULL;
	}

	for (uint32_t i = 0; i < nodesc; i++) {
		if (Z_TYPE(nodes[i]) == IS_OBJECT) {
			newNodeObj = Z_DOMOBJ_P(&nodes[i]);
			newNode = dom_object_get_node(newNodeObj);

			if (newNode->parent != NULL) {
				xmlUnlinkNode(newNode);
			}

			newNodeObj->document = document;
			xmlSetTreeDoc(newNode, documentNode);

			if (newNode->type == XML_DOCUMENT_FRAG_NODE) {
				/* Unpack document fragment nodes, the behaviour differs for different libxml2 versions. */
				newNode = newNode->children;
				while (newNode) {
					xmlNodePtr next = newNode->next;
					xmlUnlinkNode(newNode);
					dom_add_child_without_merging(fragment, newNode);
					newNode = next;
				}
			} else {
				dom_add_child_without_merging(fragment, newNode);
				if (UNEXPECTED(newNode->type == XML_DTD_NODE)) {
					documentNode->intSubset = (xmlDtdPtr) newNode;
				}
			}
		} else {
			ZEND_ASSERT(Z_TYPE(nodes[i]) == IS_STRING);

			newNode = xmlNewDocText(documentNode, (xmlChar *) Z_STRVAL(nodes[i]));
			dom_add_child_without_merging(fragment, newNode);
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

bool php_dom_fragment_insertion_hierarchy_check(xmlNodePtr node)
{
	/* If node has more than one element child or has a Text node child. */
	xmlNodePtr iter = node->children;
	bool seen_element = false;
	while (iter != NULL) {
		if (iter->type == XML_ELEMENT_NODE) {
			if (seen_element) {
				php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot have more than one element child in a document", /* strict */ true);
				return false;
			}
			seen_element = true;
		} else if (iter->type == XML_TEXT_NODE || iter->type == XML_CDATA_SECTION_NODE) {
			php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot insert text as a child of a document", /* strict */ true);
			return false;
		}
		iter = iter->next;
	}
	return true;
}

/* https://dom.spec.whatwg.org/#concept-node-ensure-pre-insertion-validity */
static zend_result dom_sanity_check_node_list_for_insertion(php_libxml_ref_obj *document, xmlNodePtr parentNode, xmlNodePtr child, zval *nodes, int nodesc)
{
	if (UNEXPECTED(parentNode == NULL)) {
		/* No error required, this must be a no-op per spec */
		return FAILURE;
	}

	/* 1. If parent is not a Document, DocumentFragment, or Element node, then throw a "HierarchyRequestError" DOMException.
	 *    => Impossible */
	ZEND_ASSERT(parentNode->type == XML_ELEMENT_NODE
				|| parentNode->type == XML_DOCUMENT_NODE
				|| parentNode->type == XML_HTML_DOCUMENT_NODE
				|| parentNode->type == XML_DOCUMENT_FRAG_NODE
				|| parentNode->type == XML_ELEMENT_NODE);

	bool parent_is_document = parentNode->type == XML_DOCUMENT_NODE || parentNode->type == XML_HTML_DOCUMENT_NODE;

	xmlDocPtr documentNode = dom_doc_from_context_node(parentNode);

	for (uint32_t i = 0; i < nodesc; i++) {
		zend_uchar type = Z_TYPE(nodes[i]);
		if (type == IS_OBJECT) {
			const zend_class_entry *ce = Z_OBJCE(nodes[i]);

			if (instanceof_function(ce, dom_node_class_entry)) {
				xmlNodePtr node = dom_object_get_node(Z_DOMOBJ_P(nodes + i));

				if (node->doc != documentNode) {
					php_dom_throw_error(WRONG_DOCUMENT_ERR, dom_get_strict_error(document));
					return FAILURE;
				}

				/* 3. If child is non-null and its parent is not parent, then throw a "NotFoundError" DOMException.
				 *    => Impossible */

				if (/* 2. If node is a host-including inclusive ancestor of parent, then throw a "HierarchyRequestError" DOMException. */
					dom_hierarchy(parentNode, node) != SUCCESS
					/* 4. If node is not a DocumentFragment, DocumentType, Element, or CharacterData node, then throw a "HierarchyRequestError" DOMException. */
					|| node->type == XML_ATTRIBUTE_NODE
					|| (php_dom_follow_spec_doc_ref(document) && (
						node->type == XML_ENTITY_REF_NODE
						|| node->type == XML_ENTITY_NODE
						|| node->type == XML_NOTATION_NODE
						|| node->type == XML_DOCUMENT_NODE
						|| node->type == XML_HTML_DOCUMENT_NODE
						|| node->type >= XML_ELEMENT_DECL))) {
					php_dom_throw_error(HIERARCHY_REQUEST_ERR, dom_get_strict_error(document));
					return FAILURE;
				}

				if (php_dom_follow_spec_doc_ref(document)) {
					/* 5. If either node is a Text node and parent is a document... */
					if (parent_is_document && (node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE)) {
						php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot insert text as a child of a document", /* strict */ true);
						return FAILURE;
					}

					/* 5. ..., or node is a doctype and parent is not a document, then throw a "HierarchyRequestError" DOMException. */
					if (!parent_is_document && node->type == XML_DTD_NODE) {
						php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot insert a document type into anything other than a document", /* strict */ true);
						return FAILURE;
					}

					/* 6. If parent is a document, and any of the statements below, switched on the interface node implements,
					 *    are true, then throw a "HierarchyRequestError" DOMException. */
					if (parent_is_document) {
						/* DocumentFragment */
						if (node->type == XML_DOCUMENT_FRAG_NODE) {
							if (!php_dom_fragment_insertion_hierarchy_check(node)) {
								return FAILURE;
							}
						}
						/* Element */
						else if (node->type == XML_ELEMENT_NODE) {
							/* parent has an element child, child is a doctype, or child is non-null and a doctype is following child. */
							if (php_dom_has_child_of_type(parentNode, XML_ELEMENT_NODE)) {
								php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot have more than one element child in a document", /* strict */ true);
								return FAILURE;
							}
							if (child != NULL && (child->type == XML_DTD_NODE || php_dom_has_sibling_following_node(child, XML_DTD_NODE))) {
								php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Document types must be the first child in a document", /* strict */ true);
								return FAILURE;
							}
						}
						/* DocumentType */
						else if (node->type == XML_DTD_NODE) {
							/* parent has a doctype child, child is non-null and an element is preceding child, or child is null and parent has an element child. */
							if (php_dom_has_child_of_type(parentNode, XML_DTD_NODE)) {
								php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot have more than one document type", /* strict */ true);
								return FAILURE;
							}
							if ((child != NULL && php_dom_has_sibling_preceding_node(child, XML_ELEMENT_NODE))
								|| (child == NULL && php_dom_has_child_of_type(parentNode, XML_ELEMENT_NODE))) {
								php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Document types must be the first child in a document", /* strict */ true);
								return FAILURE;
							}
						}
					}
				}
			} else {
				zend_argument_type_error(i + 1, "must be of type DOMNode|string, %s given", zend_zval_type_name(&nodes[i]));
				return FAILURE;
			}
		} else if (type == IS_STRING) {
			/* Repetition of step 5. If either node is a Text node and parent is a document... */
			if (parent_is_document && php_dom_follow_spec_doc_ref(document)) {
				php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot insert text as a child of a document", /* strict */ true);
				return FAILURE;
			}
		} else {
			zend_argument_type_error(i + 1, "must be of type DOMNode|string, %s given", zend_zval_type_name(&nodes[i]));
			return FAILURE;
		}
	}

	return SUCCESS;
}

static void dom_pre_insert(xmlNodePtr insertion_point, xmlNodePtr parentNode, xmlNodePtr newchild, xmlNodePtr fragment)
{
	if (!insertion_point) {
		/* Place it as last node */
		if (parentNode->children) {
			/* There are children */
			newchild->prev = parentNode->last;
			parentNode->last->next = newchild;
		} else {
			/* No children, because they moved out when they became a fragment */
			parentNode->children = newchild;
		}
		parentNode->last = fragment->last;
	} else {
		/* Insert fragment before insertion_point */
		fragment->last->next = insertion_point;
		if (insertion_point->prev) {
			insertion_point->prev->next = newchild;
			newchild->prev = insertion_point->prev;
		}
		insertion_point->prev = fragment->last;
		if (parentNode->children == insertion_point) {
			parentNode->children = newchild;
		}
	}
}

void dom_parent_node_append(dom_object *context, zval *nodes, uint32_t nodesc)
{
	xmlNode *parentNode = dom_object_get_node(context);
	xmlNodePtr newchild, prevsib;

	if (UNEXPECTED(dom_sanity_check_node_list_for_insertion(context->document, parentNode, NULL, nodes, nodesc) != SUCCESS)) {
		return;
	}

	php_libxml_invalidate_node_list_cache(context->document);

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

	if (UNEXPECTED(dom_sanity_check_node_list_for_insertion(context->document, parentNode, parentNode->children, nodes, nodesc) != SUCCESS)) {
		return;
	}

	php_libxml_invalidate_node_list_cache(context->document);

	xmlNode *fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);

	if (fragment == NULL) {
		return;
	}

	xmlNode *newchild = fragment->children;

	if (newchild) {
		xmlNodePtr last = fragment->last;

		dom_pre_insert(parentNode->children, parentNode, newchild, fragment);

		dom_fragment_assign_parent_node(parentNode, fragment);

		dom_reconcile_ns_list(parentNode->doc, newchild, last);
	}

	xmlFree(fragment);
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

	/* Spec step 3: find first following child not in nodes; otherwise null */
	xmlNodePtr viable_next_sibling = prevsib->next;
	while (viable_next_sibling) {
		if (!dom_is_node_in_list(nodes, nodesc, viable_next_sibling)) {
			break;
		}
		viable_next_sibling = viable_next_sibling->next;
	}

	/* Sanity check for fragment, includes spec step 2 */
	if (UNEXPECTED(dom_sanity_check_node_list_for_insertion(context->document, parentNode, viable_next_sibling, nodes, nodesc) != SUCCESS)) {
		return;
	}

	doc = prevsib->doc;

	php_libxml_invalidate_node_list_cache(context->document);

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

	/* Spec step 3: find first following child not in nodes; otherwise null */
	xmlNodePtr viable_previous_sibling = nextsib->prev;
	while (viable_previous_sibling) {
		if (!dom_is_node_in_list(nodes, nodesc, viable_previous_sibling)) {
			break;
		}
		viable_previous_sibling = viable_previous_sibling->prev;
	}

	/* Sanity check for fragment, includes spec step 2 */
	if (UNEXPECTED(dom_sanity_check_node_list_for_insertion(context->document, parentNode, viable_previous_sibling, nodes, nodesc) != SUCCESS)) {
		return;
	}

	doc = nextsib->doc;

	php_libxml_invalidate_node_list_cache(context->document);

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
	int stricterror;

	stricterror = dom_get_strict_error(context->document);

	if (UNEXPECTED(dom_child_removal_preconditions(child, stricterror) != SUCCESS)) {
		return;
	}

	php_libxml_invalidate_node_list_cache(context->document);

	xmlUnlinkNode(child);
}

void dom_child_replace_with(dom_object *context, zval *nodes, uint32_t nodesc)
{
	/* Spec link: https://dom.spec.whatwg.org/#dom-childnode-replacewith */

	xmlNodePtr child = dom_object_get_node(context);

	/* Spec step 1 */
	xmlNodePtr parentNode = child->parent;

	/* Spec step 3: find first following child not in nodes; otherwise null */
	xmlNodePtr viable_next_sibling = child->next;
	while (viable_next_sibling) {
		if (!dom_is_node_in_list(nodes, nodesc, viable_next_sibling)) {
			break;
		}
		viable_next_sibling = viable_next_sibling->next;
	}

	/* Sanity check for fragment, includes spec step 2 */
	if (UNEXPECTED(dom_sanity_check_node_list_for_insertion(context->document, parentNode, viable_next_sibling, nodes, nodesc) != SUCCESS)) {
		return;
	}

	int stricterror = dom_get_strict_error(context->document);
	if (UNEXPECTED(dom_child_removal_preconditions(child, stricterror) != SUCCESS)) {
		return;
	}

	xmlDocPtr doc = parentNode->doc;
	php_libxml_invalidate_node_list_cache(context->document);

	/* Spec step 4: convert nodes into fragment */
	xmlNodePtr fragment = dom_zvals_to_fragment(context->document, parentNode, nodes, nodesc);
	if (UNEXPECTED(fragment == NULL)) {
		return;
	}

	/* Spec step 5: perform the replacement */

	xmlNodePtr newchild = fragment->children;

	/* Unlink it unless it became a part of the fragment.
	 * Freeing will be taken care of by the lifetime of the returned dom object. */
	if (child->parent != fragment) {
		xmlUnlinkNode(child);
	}

	if (newchild) {
		xmlNodePtr last = fragment->last;

		dom_pre_insert(viable_next_sibling, parentNode, newchild, fragment);

		dom_fragment_assign_parent_node(parentNode, fragment);
		dom_reconcile_ns_list(doc, newchild, last);
	}

	xmlFree(fragment);
}

void dom_parent_node_replace_children(dom_object *context, zval *nodes, uint32_t nodesc)
{
	/* Spec link: https://dom.spec.whatwg.org/#dom-parentnode-replacechildren */

	xmlNodePtr thisp = dom_object_get_node(context);
	if (UNEXPECTED(dom_sanity_check_node_list_for_insertion(context->document, thisp, NULL, nodes, nodesc) != SUCCESS)) {
		return;
	}

	xmlNodePtr fragment = dom_zvals_to_fragment(context->document, thisp, nodes, nodesc);
	if (UNEXPECTED(fragment == NULL)) {
		return;
	}

	php_libxml_invalidate_node_list_cache(context->document);

	dom_remove_all_children(thisp);

	xmlNodePtr newchild = fragment->children;
	if (newchild) {
		xmlNodePtr last = fragment->last;

		dom_pre_insert(NULL, thisp, newchild, fragment);

		dom_fragment_assign_parent_node(thisp, fragment);
		dom_reconcile_ns_list(thisp->doc, newchild, last);
	}

	xmlFree(fragment);
}

#endif
