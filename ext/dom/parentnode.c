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
   |          Niels Dossche <nielsdos@php.net>                            |
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
		if (Z_TYPE(nodes[i]) == IS_OBJECT) {
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
bool php_dom_pre_insert_is_parent_invalid(xmlNodePtr parent)
{
	return parent->type != XML_DOCUMENT_NODE
		&& parent->type != XML_HTML_DOCUMENT_NODE
		&& parent->type != XML_ELEMENT_NODE
		&& parent->type != XML_DOCUMENT_FRAG_NODE;
}

/* https://dom.spec.whatwg.org/#concept-node-ensure-pre-insertion-validity */
static bool dom_is_pre_insert_valid_without_step_1(php_libxml_ref_obj *document, xmlNodePtr parentNode, xmlNodePtr node, xmlNodePtr child, xmlDocPtr documentNode)
{
	ZEND_ASSERT(parentNode != NULL);

	/* 1. If parent is not a Document, DocumentFragment, or Element node, then throw a "HierarchyRequestError" DOMException.
	 *    => Impossible */
	ZEND_ASSERT(!php_dom_pre_insert_is_parent_invalid(parentNode));

	if (node->doc != documentNode) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, dom_get_strict_error(document));
		return false;
	}

	/* 3. If child is non-null and its parent is not parent, then throw a "NotFoundError" DOMException. */
	if (child != NULL && child->parent != parentNode) {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(document));
		return false;
	}

	bool parent_is_document = parentNode->type == XML_DOCUMENT_NODE || parentNode->type == XML_HTML_DOCUMENT_NODE;

	if (/* 2. If node is a host-including inclusive ancestor of parent, then throw a "HierarchyRequestError" DOMException. */
		dom_hierarchy(parentNode, node) != SUCCESS
		/* 4. If node is not a DocumentFragment, DocumentType, Element, or CharacterData node, then throw a "HierarchyRequestError" DOMException. */
		|| node->type == XML_ATTRIBUTE_NODE
		|| (document != NULL && php_dom_follow_spec_doc_ref(document) && (
			node->type == XML_ENTITY_REF_NODE
			|| node->type == XML_ENTITY_NODE
			|| node->type == XML_NOTATION_NODE
			|| node->type == XML_DOCUMENT_NODE
			|| node->type == XML_HTML_DOCUMENT_NODE
			|| node->type >= XML_ELEMENT_DECL))) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, dom_get_strict_error(document));
		return false;
	}

	if (document != NULL && php_dom_follow_spec_doc_ref(document)) {
		/* 5. If either node is a Text node and parent is a document... */
		if (parent_is_document && (node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE)) {
			php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot insert text as a child of a document", /* strict */ true);
			return false;
		}

		/* 5. ..., or node is a doctype and parent is not a document, then throw a "HierarchyRequestError" DOMException. */
		if (!parent_is_document && node->type == XML_DTD_NODE) {
			php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot insert a document type into anything other than a document", /* strict */ true);
			return false;
		}

		/* 6. If parent is a document, and any of the statements below, switched on the interface node implements,
		 *    are true, then throw a "HierarchyRequestError" DOMException. */
		if (parent_is_document) {
			/* DocumentFragment */
			if (node->type == XML_DOCUMENT_FRAG_NODE) {
				if (!php_dom_fragment_insertion_hierarchy_check(node)) {
					return false;
				}
			}
			/* Element */
			else if (node->type == XML_ELEMENT_NODE) {
				/* parent has an element child, child is a doctype, or child is non-null and a doctype is following child. */
				if (php_dom_has_child_of_type(parentNode, XML_ELEMENT_NODE)) {
					php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot have more than one element child in a document", /* strict */ true);
					return false;
				}
				if (child != NULL && (child->type == XML_DTD_NODE || php_dom_has_sibling_following_node(child, XML_DTD_NODE))) {
					php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Document types must be the first child in a document", /* strict */ true);
					return false;
				}
			}
			/* DocumentType */
			else if (node->type == XML_DTD_NODE) {
				/* parent has a doctype child, child is non-null and an element is preceding child, or child is null and parent has an element child. */
				if (php_dom_has_child_of_type(parentNode, XML_DTD_NODE)) {
					php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot have more than one document type", /* strict */ true);
					return false;
				}
				if ((child != NULL && php_dom_has_sibling_preceding_node(child, XML_ELEMENT_NODE))
					|| (child == NULL && php_dom_has_child_of_type(parentNode, XML_ELEMENT_NODE))) {
					php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Document types must be the first child in a document", /* strict */ true);
					return false;
				}
			}
		}
	}

	return true;
}

static void dom_free_node_after_zval_single_node_creation(xmlNodePtr node)
{
	/* For the object cases, the user did provide them, so they don't have to be freed as there are still references.
	 * For the newly created text nodes, we do have to free them. */
	xmlNodePtr next;
	for (xmlNodePtr child = node->children; child != NULL; child = next) {
		next = child->next;
		xmlUnlinkNode(child);
		if (child->_private == NULL) {
			xmlFreeNode(child);
		}
	}
}

/* https://dom.spec.whatwg.org/#converting-nodes-into-a-node */
xmlNode* dom_zvals_to_single_node(php_libxml_ref_obj *document, xmlNode *contextNode, zval *nodes, uint32_t nodesc)
{
	xmlDoc *documentNode;
	xmlNode *newNode;
	dom_object *newNodeObj;

	documentNode = dom_doc_from_context_node(contextNode);

	/* 1. Let node be null. */
	xmlNodePtr node = NULL;

	/* 2. => handled in the loop. */

	/* 3. If nodes contains one node, then set node to nodes[0]. */
	if (nodesc == 1) {
		/* ... and return */
		if (Z_TYPE_P(nodes) == IS_OBJECT) {
			node = dom_object_get_node(Z_DOMOBJ_P(nodes));
			/* Fragments can cause problems because of unpacking (see below). Skip this special case. */
			if (node->type != XML_DOCUMENT_FRAG_NODE) {
				return node;
			}
		} else {
			ZEND_ASSERT(Z_TYPE_P(nodes) == IS_STRING);
			return xmlNewDocText(documentNode, BAD_CAST Z_STRVAL_P(nodes));
		}
	}

	node = xmlNewDocFragment(documentNode);
	if (UNEXPECTED(!node)) {
		return NULL;
	}

	/* 4. Otherwise, set node to a new DocumentFragment node whose node document is document,
	 *    and then append each node in nodes, if any, to it. */
	for (uint32_t i = 0; i < nodesc; i++) {
		if (Z_TYPE(nodes[i]) == IS_OBJECT) {
			newNodeObj = Z_DOMOBJ_P(&nodes[i]);
			newNode = dom_object_get_node(newNodeObj);

			if (!dom_is_pre_insert_valid_without_step_1(document, node, newNode, NULL, documentNode)) {
				goto err;
			}

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
					dom_add_child_without_merging(node, newNode);
					newNode = next;
				}
			} else {
				dom_add_child_without_merging(node, newNode);
			}
		} else {
			/* 2. Replace each string in nodes with a new Text node whose data is the string and node document is document. */
			ZEND_ASSERT(Z_TYPE(nodes[i]) == IS_STRING);

			/* Text nodes can't violate the hierarchy at this point. */
			newNode = xmlNewDocText(documentNode, BAD_CAST Z_STRVAL(nodes[i]));
			dom_add_child_without_merging(node, newNode);
		}
	}

	/* 5. Return node. */
	return node;

err:
	/* For the object cases, the user did provide them, so they don't have to be freed as there are still references.
	 * For the newly created text nodes, we do have to free them. */
	dom_free_node_after_zval_single_node_creation(node);
	xmlFree(node);
	return NULL;
}

static zend_result dom_sanity_check_node_list_types(zval *nodes, int nodesc)
{
	for (uint32_t i = 0; i < nodesc; i++) {
		zend_uchar type = Z_TYPE(nodes[i]);
		if (type == IS_OBJECT) {
			const zend_class_entry *ce = Z_OBJCE(nodes[i]);

			if (!instanceof_function(ce, dom_node_class_entry)) {
				zend_argument_type_error(i + 1, "must be of type DOMNode|string, %s given", zend_zval_type_name(&nodes[i]));
				return FAILURE;
			}
		} else if (type != IS_STRING) {
			zend_argument_type_error(i + 1, "must be of type DOMNode|string, %s given", zend_zval_type_name(&nodes[i]));
			return FAILURE;
		}
	}

	return SUCCESS;
}

static void php_dom_pre_insert_helper(xmlNodePtr insertion_point, xmlNodePtr parentNode, xmlNodePtr newchild, xmlNodePtr last)
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
		parentNode->last = last;
	} else {
		/* Insert fragment before insertion_point */
		last->next = insertion_point;
		if (insertion_point->prev) {
			insertion_point->prev->next = newchild;
			newchild->prev = insertion_point->prev;
		}
		insertion_point->prev = last;
		if (parentNode->children == insertion_point) {
			parentNode->children = newchild;
		}
	}
}

static void dom_insert_node_list_cleanup(xmlNodePtr node)
{
	if (node->_private != NULL) {
		/* Not a temporary node. */
		return;
	}
	if (node->type == XML_DOCUMENT_FRAG_NODE) {
		dom_free_node_after_zval_single_node_creation(node);
		xmlFree(node); /* Don't free the children, now-empty fragment! */
	} else if (node->type == XML_TEXT_NODE) {
		ZEND_ASSERT(node->parent == NULL);
		xmlFreeNode(node);
	} else {
		/* Must have been a directly-passed node. */
		ZEND_ASSERT(node->_private != NULL);
	}
}

/* https://dom.spec.whatwg.org/#concept-node-pre-insert */
static void dom_insert_node_list_unchecked(php_libxml_ref_obj *document, xmlNodePtr node, xmlNodePtr parent, xmlNodePtr insertion_point)
{
	/* Step 1 should be checked by the caller. */

	if (node->type == XML_DOCUMENT_FRAG_NODE) {
		/* Steps 2-3 are not applicable here, the condition is impossible. */

		xmlNodePtr newchild = node->children;

		/* 4. Insert node into parent before referenceChild. */
		if (newchild) {
			xmlNodePtr last = node->last;
			php_dom_pre_insert_helper(insertion_point, parent, newchild, last);
			dom_fragment_assign_parent_node(parent, node);
			dom_reconcile_ns_list(parent->doc, newchild, last);
			if (parent->doc && newchild->type == XML_DTD_NODE) {
				parent->doc->intSubset = (xmlDtdPtr) newchild;
			}
		}

		if (node->_private == NULL) {
			xmlFree(node);
		} else {
			node->children = NULL;
			node->last = NULL;
		}
	} else {
		/* 2. Let referenceChild be child.
		 * 3. If referenceChild is node, then set referenceChild to node’s next sibling. */
		if (insertion_point == node) {
			insertion_point = node->next;
		}

		/* 4. Insert node into parent before referenceChild. */
		xmlUnlinkNode(node);
		php_dom_pre_insert_helper(insertion_point, parent, node, node);
		node->parent = parent;
		if (parent->doc && node->type == XML_DTD_NODE) {
			parent->doc->intSubset = (xmlDtdPtr) node;
		} else {
			dom_reconcile_ns(parent->doc, node);
		}
	}
}

/* https://dom.spec.whatwg.org/#concept-node-pre-insert */
bool php_dom_pre_insert(php_libxml_ref_obj *document, xmlNodePtr node, xmlNodePtr parent, xmlNodePtr insertion_point)
{
	if (UNEXPECTED(node == NULL)) {
		return false;
	}

	/* Step 1 checked here, other steps delegated to other function. */
	if (dom_is_pre_insert_valid_without_step_1(document, parent, node, insertion_point, parent->doc)) {
		dom_insert_node_list_unchecked(document, node, parent, insertion_point);
		return true;
	} else {
		dom_insert_node_list_cleanup(node);
		return false;
	}
}

/* https://dom.spec.whatwg.org/#concept-node-append */
void php_dom_node_append(php_libxml_ref_obj *document, xmlNodePtr node, xmlNodePtr parent)
{
	php_dom_pre_insert(document, node, parent, NULL);
}

/* https://dom.spec.whatwg.org/#dom-parentnode-append */
void dom_parent_node_append(dom_object *context, zval *nodes, uint32_t nodesc)
{
	if (UNEXPECTED(dom_sanity_check_node_list_types(nodes, nodesc) != SUCCESS)) {
		return;
	}

	xmlNode *parentNode = dom_object_get_node(context);

	php_libxml_invalidate_node_list_cache(context->document);

	/* 1. Let node be the result of converting nodes into a node given nodes and this’s node document. */
	xmlNodePtr node = dom_zvals_to_single_node(context->document, parentNode, nodes, nodesc);
	if (UNEXPECTED(node == NULL)) {
		return;
	}

	/* 2. Append node to this. */
	php_dom_node_append(context->document, node, parentNode);
}

/* https://dom.spec.whatwg.org/#dom-parentnode-prepend */
void dom_parent_node_prepend(dom_object *context, zval *nodes, uint32_t nodesc)
{
	xmlNode *parentNode = dom_object_get_node(context);

	if (parentNode->children == NULL) {
		dom_parent_node_append(context, nodes, nodesc);
		return;
	}

	if (UNEXPECTED(dom_sanity_check_node_list_types(nodes, nodesc) != SUCCESS)) {
		return;
	}

	php_libxml_invalidate_node_list_cache(context->document);

	/* 1. Let node be the result of converting nodes into a node given nodes and this’s node document. */
	xmlNodePtr node = dom_zvals_to_single_node(context->document, parentNode, nodes, nodesc);
	if (UNEXPECTED(node == NULL)) {
		return;
	}

	/* 2. Pre-insert node into this before this’s first child. */
	php_dom_pre_insert(context->document, node, parentNode, parentNode->children);
}

/* https://dom.spec.whatwg.org/#dom-childnode-after */
void dom_parent_node_after(dom_object *context, zval *nodes, uint32_t nodesc)
{
	if (UNEXPECTED(dom_sanity_check_node_list_types(nodes, nodesc) != SUCCESS)) {
		return;
	}

	xmlNode *thisp = dom_object_get_node(context);

	/* 1. Let parent be this’s parent. */
	xmlNodePtr parentNode = thisp->parent;

	/* 2. If parent is null, then return. */
	if (UNEXPECTED(parentNode == NULL)) {
		return;
	}

	/* 3. Let viableNextSibling be this’s first following sibling not in nodes; otherwise null. */
	xmlNodePtr viable_next_sibling = thisp->next;
	while (viable_next_sibling && dom_is_node_in_list(nodes, nodesc, viable_next_sibling)) {
		viable_next_sibling = viable_next_sibling->next;
	}

	php_libxml_invalidate_node_list_cache(context->document);

	/* 4. Let node be the result of converting nodes into a node, given nodes and this’s node document. */
	xmlNodePtr fragment = dom_zvals_to_single_node(context->document, parentNode, nodes, nodesc);

	/* 5. Pre-insert node into parent before viableNextSibling. */
	php_dom_pre_insert(context->document, fragment, parentNode, viable_next_sibling);
}

/* https://dom.spec.whatwg.org/#dom-childnode-before */
void dom_parent_node_before(dom_object *context, zval *nodes, uint32_t nodesc)
{
	if (UNEXPECTED(dom_sanity_check_node_list_types(nodes, nodesc) != SUCCESS)) {
		return;
	}

	xmlNode *thisp = dom_object_get_node(context);

	/* 1. Let parent be this’s parent. */
	xmlNodePtr parentNode = thisp->parent;

	/* 2. If parent is null, then return. */
	if (UNEXPECTED(parentNode == NULL)) {
		return;
	}

	/* 3. Let viablePreviousSibling be this’s first preceding sibling not in nodes; otherwise null. */
	xmlNodePtr viable_previous_sibling = thisp->prev;
	while (viable_previous_sibling && dom_is_node_in_list(nodes, nodesc, viable_previous_sibling)) {
		viable_previous_sibling = viable_previous_sibling->prev;
	}

	php_libxml_invalidate_node_list_cache(context->document);

	/* 4. Let node be the result of converting nodes into a node, given nodes and this’s node document. */
	xmlNodePtr fragment = dom_zvals_to_single_node(context->document, parentNode, nodes, nodesc);

	/* 5. If viable_previous_sibling is null, set it to the parent's first child, otherwise viable_previous_sibling's next sibling. */
	if (!viable_previous_sibling) {
		viable_previous_sibling = parentNode->children;
	} else {
		viable_previous_sibling = viable_previous_sibling->next;
	}

	/* 6. Pre-insert node into parent before viablePreviousSibling. */
	php_dom_pre_insert(context->document, fragment, parentNode, viable_previous_sibling);
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

/* https://dom.spec.whatwg.org/#dom-childnode-replacewith */
void dom_child_replace_with(dom_object *context, zval *nodes, uint32_t nodesc)
{
	if (UNEXPECTED(dom_sanity_check_node_list_types(nodes, nodesc) != SUCCESS)) {
		return;
	}

	xmlNodePtr child = dom_object_get_node(context);

	/* 1. Let parent be this’s parent. */
	xmlNodePtr parentNode = child->parent;

	/* 2. If parent is null, then return. */
	if (UNEXPECTED(parentNode == NULL)) {
		return;
	}

	/* 3. Let viableNextSibling be this’s first following sibling not in nodes; otherwise null. */
	xmlNodePtr viable_next_sibling = child->next;
	while (viable_next_sibling && dom_is_node_in_list(nodes, nodesc, viable_next_sibling)) {
		viable_next_sibling = viable_next_sibling->next;
	}

	int stricterror = dom_get_strict_error(context->document);
	if (UNEXPECTED(dom_child_removal_preconditions(child, stricterror) != SUCCESS)) {
		return;
	}

	php_libxml_invalidate_node_list_cache(context->document);

	/* 4. Let node be the result of converting nodes into a node, given nodes and this’s node document. */
	xmlNodePtr node = dom_zvals_to_single_node(context->document, parentNode, nodes, nodesc);
	if (UNEXPECTED(node == NULL)) {
		return;
	}

	/* Spec step 5-6: perform the replacement */
	if (dom_is_pre_insert_valid_without_step_1(context->document, parentNode, node, viable_next_sibling, parentNode->doc)) {
		/* Unlink it unless it became a part of the fragment.
		 * Freeing will be taken care of by the lifetime of the returned dom object. */
		if (child->parent != node) {
			xmlUnlinkNode(child);
		}

		dom_insert_node_list_unchecked(context->document, node, parentNode, viable_next_sibling);
	} else {
		dom_insert_node_list_cleanup(node);
	}
}

/* https://dom.spec.whatwg.org/#dom-parentnode-replacechildren */
void dom_parent_node_replace_children(dom_object *context, zval *nodes, uint32_t nodesc)
{
	if (UNEXPECTED(dom_sanity_check_node_list_types(nodes, nodesc) != SUCCESS)) {
		return;
	}

	xmlNodePtr thisp = dom_object_get_node(context);

	php_libxml_invalidate_node_list_cache(context->document);

	/* 1. Let node be the result of converting nodes into a node given nodes and this’s node document. */
	xmlNodePtr node = dom_zvals_to_single_node(context->document, thisp, nodes, nodesc);
	if (UNEXPECTED(node == NULL)) {
		return;
	}

	/* Spec steps 2-3: replace all */
	if (dom_is_pre_insert_valid_without_step_1(context->document, thisp, node, NULL, thisp->doc)) {
		dom_remove_all_children(thisp);
		php_dom_pre_insert(context->document, node, thisp, NULL);
	} else {
		dom_insert_node_list_cleanup(node);
	}
}

#endif
