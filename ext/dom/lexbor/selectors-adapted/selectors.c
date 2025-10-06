/*
 * Copyright (C) 2021-2025 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 * Adapted for PHP + libxml2 by: Niels Dossche <nielsdos@php.net>
 * Based on Lexbor (upstream commit 971faf11a5f45433b9193a143e2897d8c0fd5611)
 */

#include <libxml/xmlstring.h>
#include <libxml/dict.h>
#include <Zend/zend.h>
#include <Zend/zend_operators.h>
#include <Zend/zend_API.h>
#include <php.h>

#include "selectors.h"
#include "../../namespace_compat.h"
#include "../../domexception.h"
#include "../../php_dom.h"

#include <math.h>

/* Note: casting and then comparing is a bit faster on my i7-4790 */
#define CMP_NODE_TYPE(node, ty) ((unsigned char) (node)->type == ty)

typedef struct dom_lxb_str_wrapper {
	lexbor_str_t str;
	bool should_free;
} dom_lxb_str_wrapper;

static void dom_lxb_str_wrapper_release(dom_lxb_str_wrapper *wrapper)
{
	if (wrapper->should_free) {
		xmlFree(wrapper->str.data);
	}
}

static zend_always_inline bool lxb_selectors_adapted_cmp_local_name_literal(const xmlNode *node, const char *name)
{
	return strcmp((const char *) node->name, name) == 0;
}

static zend_always_inline bool lxb_selectors_adapted_cmp_ns(const xmlNode *a, const xmlNode *b)
{
	/* Namespace URIs are not interned, hence a->href != b->href. */
	return a->ns == b->ns || (a->ns != NULL && b->ns != NULL && xmlStrEqual(a->ns->href, b->ns->href));
}

static zend_always_inline bool lxb_selectors_adapted_cmp_local_name_id(const xmlNode *node, const lxb_selectors_adapted_id *id)
{
	uintptr_t ptr = (uintptr_t) node->name;
	if (id->interned && (ptr & (ZEND_MM_ALIGNMENT - 1)) != 0) {
		/* It cannot be a heap-allocated string because the pointer is not properly aligned for a heap allocation.
		 * Therefore, it must be interned into the dictionary pool. */
		return node->name == id->name;
	}

	return strcmp((const char *) node->name, (const char *) id->name) == 0;
}

static zend_always_inline const xmlAttr *lxb_selectors_adapted_attr(const xmlNode *node, const lxb_char_t *name)
{
	const xmlAttr *attr = NULL;
	ZEND_ASSERT(node->doc != NULL);
	if (php_dom_ns_is_html_and_document_is_html(node)) {
		/* No need to handle DTD entities as we're in HTML. */
		size_t name_bound = strlen((const char *) name) + 1;
		for (const xmlAttr *cur = node->properties; cur != NULL; cur = cur->next) {
			if (lexbor_str_data_nlocmp_right(cur->name, name, name_bound)) {
				attr = cur;
				break;
			}
		}
	} else {
		attr = xmlHasProp(node, (const xmlChar *) name);
	}

	if (attr != NULL && attr->ns != NULL) {
		return NULL;
	}
	return attr;
}

static zend_always_inline bool lxb_selectors_adapted_has_attr(const xmlNode *node, const char *name)
{
	return lxb_selectors_adapted_attr(node, (const lxb_char_t *) name) != NULL;
}

static zend_always_inline dom_lxb_str_wrapper lxb_selectors_adapted_attr_value(const xmlAttr *attr)
{
	dom_lxb_str_wrapper ret;
	ret.str.data = (lxb_char_t *) php_libxml_attr_value(attr, &ret.should_free);
	ret.str.length = strlen((const char *) ret.str.data);
	return ret;
}

static bool lxb_selectors_attrib_name_cmp(const lxb_css_selector_t *selector, const char *name, size_t len)
{
	return selector->name.length == len && lexbor_str_data_nlocmp_right((const lxb_char_t *) name, selector->name.data, len);
}

/* From https://html.spec.whatwg.org/#case-sensitivity-of-selectors
 * "Attribute selectors on an HTML element in an HTML document must treat the values of attributes with the following names as ASCII case-insensitive:" */
static bool lxb_selectors_is_lowercased_html_attrib_name(const lxb_css_selector_t *selector)
{
	return lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("accept"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("accept-charset"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("align"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("alink"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("axis"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("bgcolor"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("charset"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("checked"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("clear"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("codetype"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("color"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("compact"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("declare"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("defer"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("dir"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("direction"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("disabled"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("enctype"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("face"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("frame"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("hreflang"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("http-equiv"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("lang"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("language"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("link"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("media"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("method"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("multiple"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("nohref"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("noresize"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("noshade"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("nowrap"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("readonly"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("rel"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("rev"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("rules"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("scope"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("scrolling"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("selected"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("shape"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("target"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("text"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("type"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("valign"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("valuetype"))
		|| lxb_selectors_attrib_name_cmp(selector, ZEND_STRL("vlink"));
}

static void lxb_selectors_adapted_set_entry_id_ex(lxb_selectors_entry_t *entry, const lxb_css_selector_t *selector, const xmlNode *node)
{
	entry->id.attr_case_insensitive = lxb_selectors_is_lowercased_html_attrib_name(selector);

	if (node->doc != NULL && node->doc->dict != NULL) {
		const xmlChar *interned = xmlDictExists(node->doc->dict, selector->name.data, selector->name.length);
		if (interned != NULL) {
			entry->id.name = interned;
			entry->id.interned = true;
			return;
		}
	}

	entry->id.name = selector->name.data;
	entry->id.interned = false;
}

static zend_always_inline void lxb_selectors_adapted_set_entry_id(lxb_selectors_entry_t *entry, const lxb_css_selector_t *selector, const xmlNode *node)
{
	if (entry->id.name == NULL) {
		lxb_selectors_adapted_set_entry_id_ex(entry, selector, node);
	}
}

static lxb_status_t
lxb_selectors_tree(lxb_selectors_t *selectors, const xmlNode *root);

static lxb_status_t
lxb_selectors_run(lxb_selectors_t *selectors, const xmlNode *node);

static lxb_selectors_entry_t *
lxb_selectors_state_find(lxb_selectors_t *selectors,
						 lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_found_check(lxb_selectors_t *selectors,
								lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_found_check_forward(lxb_selectors_t *selectors,
										lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_found(lxb_selectors_t *selectors,
						  lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_found_forward(lxb_selectors_t *selectors,
								  lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_not_found(lxb_selectors_t *selectors,
							  lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_not_found_forward(lxb_selectors_t *selectors,
									  lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_next_list(lxb_selectors_t *selectors,
						lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_next_list_forward(lxb_selectors_t *selectors,
								lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_make_following(lxb_selectors_t *selectors,
							 lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_make_following_forward(lxb_selectors_t *selectors,
									 lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_after_find(lxb_selectors_t *selectors,
							   lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_after_not(lxb_selectors_t *selectors,
							  lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_after_nth_child(lxb_selectors_t *selectors,
									lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_nth_child_found(lxb_selectors_t *selectors,
									lxb_selectors_entry_t *entry);

static bool
lxb_selectors_match(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					const xmlNode *node);

static bool
lxb_selectors_match_element(const lxb_css_selector_t *selector,
							const xmlNode *node, lxb_selectors_entry_t *entry);

static bool
lxb_selectors_match_id(const lxb_css_selector_t *selector, const xmlNode *node, bool quirks);

static bool
lxb_selectors_match_class(const lexbor_str_t *target, const lexbor_str_t *src,
						  bool quirks);

static bool
lxb_selectors_match_attribute(const lxb_css_selector_t *selector,
							  const xmlNode *node, lxb_selectors_entry_t *entry);

static bool
lxb_selectors_pseudo_class(const lxb_css_selector_t *selector,
						   const xmlNode *node);

static bool
lxb_selectors_pseudo_class_function(lxb_selectors_t *selectors,
									const lxb_css_selector_t *selector,
									const xmlNode *node);

static bool
lxb_selectors_pseudo_element(const lxb_css_selector_t *selector,
							 const xmlNode *node);

static bool
lxb_selectors_pseudo_class_disabled(const xmlNode *node);

static bool
lxb_selectors_pseudo_class_first_child(const xmlNode *node);

static bool
lxb_selectors_pseudo_class_first_of_type(const xmlNode *node);

static bool
lxb_selectors_pseudo_class_last_child(const xmlNode *node);

static bool
lxb_selectors_pseudo_class_last_of_type(const xmlNode *node);

static bool
lxb_selectors_pseudo_class_read_write(const xmlNode *node);

static bool
lxb_selectors_anb_calc(const lxb_css_selector_anb_of_t *anb, size_t index);

static lxb_status_t
lxb_selectors_cb_ok(const xmlNode *node,
					lxb_css_selector_specificity_t spec, void *ctx);

static lxb_status_t
lxb_selectors_cb_not(const xmlNode *node,
					 lxb_css_selector_specificity_t spec, void *ctx);

static lxb_status_t
lxb_selectors_cb_nth_ok(const xmlNode *node,
						lxb_css_selector_specificity_t spec, void *ctx);


lxb_status_t
lxb_selectors_init(lxb_selectors_t *selectors)
{
	lxb_status_t status;

	selectors->objs = lexbor_dobject_create();
	status = lexbor_dobject_init(selectors->objs,
								 128, sizeof(lxb_selectors_entry_t));
	if (status != LXB_STATUS_OK) {
		return status;
	}

	selectors->nested = lexbor_dobject_create();
	status = lexbor_dobject_init(selectors->nested,
								 64, sizeof(lxb_selectors_nested_t));
	if (status != LXB_STATUS_OK) {
		return status;
	}

	selectors->options = LXB_SELECTORS_OPT_DEFAULT;

	return LXB_STATUS_OK;
}

void
lxb_selectors_clean(lxb_selectors_t *selectors)
{
	lexbor_dobject_clean(selectors->objs);
	lexbor_dobject_clean(selectors->nested);
}

void
lxb_selectors_destroy(lxb_selectors_t *selectors)
{
	selectors->objs = lexbor_dobject_destroy(selectors->objs, true);
	selectors->nested = lexbor_dobject_destroy(selectors->nested, true);
}

static lxb_selectors_entry_t *
lxb_selectors_state_entry_create(lxb_selectors_t *selectors,
								 const lxb_css_selector_t *selector,
								 lxb_selectors_entry_t *root,
								 const xmlNode *node)
{
	lxb_selectors_entry_t *entry;
	lxb_css_selector_combinator_t combinator;

	combinator = selector->combinator;

	do {
		selector = selector->prev;

		entry = lexbor_dobject_calloc(selectors->objs);

		entry->combinator = selector->combinator;
		entry->selector = selector;
		entry->node = node;

		if (root->prev != NULL) {
			root->prev->next = entry;
			entry->prev = root->prev;
		}

		entry->next = root;
		root->prev = entry;
	}
	while (selector->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE
		   && selector->prev != NULL);

	entry->combinator = combinator;

	return entry;
}

static lxb_selectors_entry_t *
lxb_selectors_state_entry_create_forward(lxb_selectors_t *selectors,
										 const lxb_css_selector_t *selector,
										 lxb_selectors_entry_t *root,
										 const xmlNode *node)
{
	lxb_selectors_entry_t *entry;

	selector = selector->next;

	entry = lexbor_dobject_calloc(selectors->objs);

	entry->combinator = selector->combinator;
	entry->selector = selector;
	entry->node = node;

	entry->prev = root;
	root->next = entry;

	return entry;
}

static lxb_selectors_entry_t *
lxb_selectors_entry_make_first(lxb_selectors_t *selectors,
							   lxb_css_selector_t *selector)
{
	lxb_selectors_entry_t *entry, *prev;

	prev = NULL;

	do {
		entry = lexbor_dobject_calloc(selectors->objs);

		entry->selector = selector;
		entry->combinator = LXB_CSS_SELECTOR_COMBINATOR_CLOSE;

		if (prev != NULL) {
			prev->next = entry;
			entry->prev = prev;
		}

		if (selector->combinator != LXB_CSS_SELECTOR_COMBINATOR_CLOSE
			|| selector->prev == NULL)
		{
			break;
		}

		prev = entry;
		selector = selector->prev;
	}
	while (true);

	return entry;
}

lxb_inline const xmlNode *
lxb_selectors_descendant(lxb_selectors_t *selectors,
						 lxb_selectors_entry_t *entry,
						 const xmlNode *node)
{
	node = node->parent;

	while (node != NULL) {
		if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)
			&& lxb_selectors_match(selectors, entry, node))
		{
			return node;
		}

		node = node->parent;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_descendant_forward(lxb_selectors_t *selectors,
								 lxb_selectors_entry_t *entry,
								 const xmlNode *node)
{
	const xmlNode *root;
	lxb_selectors_nested_t *current = selectors->current;

	if (entry->prev != NULL) {
		root = entry->prev->node;
	}
	else {
		root = current->root;
	}

	do {
		if (node->children != NULL) {
			node = node->children;
		}
		else {

		next:

			while (node != root && node->next == NULL) {
				node = node->parent;
			}

			if (node == root) {
				break;
			}

			node = node->next;
		}

		if (!CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
			goto next;
		}

		if (lxb_selectors_match(selectors, entry, node)) {
			return node;
		}
	}
	while (node != NULL);

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_close(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					const xmlNode *node)
{
	if (lxb_selectors_match(selectors, entry, node)) {
		return node;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_close_forward(lxb_selectors_t *selectors,
							lxb_selectors_entry_t *entry, const xmlNode *node)
{
	if (lxb_selectors_match(selectors, entry, node)) {
		return node;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_child(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					const xmlNode *root)
{
	root = root->parent;

	if (root != NULL && CMP_NODE_TYPE(root, XML_ELEMENT_NODE)
		&& lxb_selectors_match(selectors, entry, root))
	{
		return root;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_child_forward(lxb_selectors_t *selectors,
							lxb_selectors_entry_t *entry, const xmlNode *root)
{
	if (entry->prev != NULL) {
		if (entry->prev->node == root) {
			root = root->children;
		}
		else {
			root = root->next;
		}
	}
	else if (selectors->current->root == root) {
		root = root->children;
	}
	else {
		root = root->next;
	}

	while (root != NULL) {
		if (CMP_NODE_TYPE(root, XML_ELEMENT_NODE)
			&& lxb_selectors_match(selectors, entry, root))
		{
			return root;
		}

		root = root->next;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_sibling(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					  const xmlNode *node)
{
	node = node->prev;

	while (node != NULL) {
		if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
			if (lxb_selectors_match(selectors, entry, node)) {
				return node;
			}

			return NULL;
		}

		node = node->prev;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_sibling_forward(lxb_selectors_t *selectors,
							  lxb_selectors_entry_t *entry, const xmlNode *node)
{
	node = node->next;

	while (node != NULL) {
		if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
			if (lxb_selectors_match(selectors, entry, node)) {
				return node;
			}

			return NULL;
		}

		node = node->next;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_following(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
						const xmlNode *node)
{
	node = node->prev;

	while (node != NULL) {
		if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE) &&
			lxb_selectors_match(selectors, entry, node))
		{
			return node;
		}

		node = node->prev;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_following_forward(lxb_selectors_t *selectors,
								lxb_selectors_entry_t *entry,
								const xmlNode *node)
{
	node = node->next;

	while (node != NULL) {
		if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE) &&
			lxb_selectors_match(selectors, entry, node))
		{
			return node;
		}

		node = node->next;
	}

	return NULL;
}

lxb_inline void
lxb_selectors_switch_to_found_check(lxb_selectors_t *selectors,
									lxb_selectors_nested_t *current)
{
	if (current->forward) {
		selectors->state = lxb_selectors_state_found_check_forward;
	}
	else {
		selectors->state = lxb_selectors_state_found_check;
	}
}

lxb_inline void
lxb_selectors_switch_to_not_found(lxb_selectors_t *selectors,
								  lxb_selectors_nested_t *current)
{
	if (current->forward) {
		selectors->state = lxb_selectors_state_not_found_forward;
	}
	else {
		selectors->state = lxb_selectors_state_not_found;
	}
}

lxb_status_t
lxb_selectors_find(lxb_selectors_t *selectors, const xmlNode *root,
				   const lxb_css_selector_list_t *list,
				   lxb_selectors_cb_f cb, void *ctx)
{
	lxb_selectors_entry_t *entry;
	lxb_selectors_nested_t nested;

	entry = lxb_selectors_entry_make_first(selectors, list->last);

	nested.parent = NULL;
	nested.entry = entry;
	nested.first = entry;
	nested.top = entry;
	nested.cb = cb;
	nested.ctx = ctx;
	nested.forward = false;

	selectors->current = &nested;
	selectors->status = LXB_STATUS_OK;

	return lxb_selectors_tree(selectors, root);
}

lxb_status_t
lxb_selectors_match_node(lxb_selectors_t *selectors, const xmlNode *node,
						 const lxb_css_selector_list_t *list,
						 lxb_selectors_cb_f cb, void *ctx)
{
	lxb_status_t status;
	lxb_selectors_entry_t *entry;
	lxb_selectors_nested_t nested;

	if (!CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
		return LXB_STATUS_OK;
	}

	entry = lxb_selectors_entry_make_first(selectors, list->last);

	nested.parent = NULL;
	nested.entry = entry;
	nested.first = entry;
	nested.top = entry;
	nested.cb = cb;
	nested.ctx = ctx;
	nested.forward = false;

	selectors->current = &nested;
	selectors->status = LXB_STATUS_OK;

	status = lxb_selectors_run(selectors, node);

	lxb_selectors_clean(selectors);

	return status;
}

lxb_status_t
lxb_selectors_find_reverse(lxb_selectors_t *selectors, const xmlNode *root,
						   const lxb_css_selector_list_t *list,
						   lxb_selectors_cb_f cb, void *ctx)
{
	return lxb_selectors_find(selectors, root, list, cb, ctx);
}

static lxb_status_t
lxb_selectors_tree(lxb_selectors_t *selectors, const xmlNode *root)
{
	lxb_status_t status;
	const xmlNode *node;

#if 0
	if (selectors->options & LXB_SELECTORS_OPT_MATCH_ROOT) {
		node = root;

		if (CMP_NODE_TYPE(node, XML_DOCUMENT_NODE) || CMP_NODE_TYPE(node, XML_HTML_DOCUMENT_NODE)
			 || CMP_NODE_TYPE(node, XML_DOCUMENT_FRAG_NODE)) {
			node = root->children;
		}
	}
	else
#endif
	{
		node = root->children;
	}

	if (node == NULL) {
		goto out;
	}

	do {
		if (!CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
			goto next;
		}

		status = lxb_selectors_run(selectors, node);
		if (status != LXB_STATUS_OK) {
			if (status == LXB_STATUS_STOP) {
				break;
			}

			lxb_selectors_clean(selectors);

			return status;
		}

		if (node->children != NULL) {
			node = node->children;
		}
		else {

		next:

			while (node != root && node->next == NULL) {
				node = node->parent;
			}

			if (node == root) {
				break;
			}

			node = node->next;
		}
	}
	while (true);

out:
	lxb_selectors_clean(selectors);

	return LXB_STATUS_OK;
}

static lxb_status_t
lxb_selectors_run(lxb_selectors_t *selectors, const xmlNode *node)
{
	lxb_selectors_entry_t *entry;
	lxb_selectors_nested_t *current = selectors->current;

	entry = current->entry;

	entry->node = node;
	current->root = node;
	selectors->state = lxb_selectors_state_find;

	do {
		entry = selectors->state(selectors, entry);
	}
	while (entry != NULL);

	current->first = current->top;
	current->entry = current->top;

	return selectors->status;
}

static lxb_selectors_entry_t *
lxb_selectors_state_find(lxb_selectors_t *selectors,
						 lxb_selectors_entry_t *entry)
{
	const xmlNode *node;

	selectors->state = lxb_selectors_state_found_check;

	switch (entry->combinator) {
		case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
			node = lxb_selectors_descendant(selectors, entry, entry->node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
			node = lxb_selectors_close(selectors, entry, entry->node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
			node = lxb_selectors_child(selectors, entry, entry->node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
			node = lxb_selectors_sibling(selectors, entry, entry->node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
			node = lxb_selectors_following(selectors, entry, entry->node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CELL:
		default:
			selectors->status = LXB_STATUS_ERROR;
			return NULL;
	}

	if (node == NULL) {
		selectors->state = lxb_selectors_state_not_found;
	}
	else {
		selectors->current->entry->node = node;
	}

	return selectors->current->entry;
}

static lxb_selectors_entry_t *
lxb_selectors_state_find_forward(lxb_selectors_t *selectors,
								 lxb_selectors_entry_t *entry)
{
	const xmlNode *node;

	selectors->state = lxb_selectors_state_found_check_forward;

	switch (entry->combinator) {
		case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
			node = lxb_selectors_descendant_forward(selectors, entry,
													entry->node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
			node = lxb_selectors_close_forward(selectors, entry,
											   entry->node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
			node = lxb_selectors_child_forward(selectors, entry, entry->node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
			node = lxb_selectors_sibling_forward(selectors, entry, entry->node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
			node = lxb_selectors_following_forward(selectors, entry,
												   entry->node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CELL:
		default:
			selectors->status = LXB_STATUS_ERROR;
			return NULL;
	}

	if (node == NULL) {
	try_next:

		do {
			if (entry->prev == NULL) {
				return lxb_selectors_next_list_forward(selectors, entry);
			}

			entry = entry->prev;
		}
		while (entry->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE);

		if (entry->combinator == LXB_CSS_SELECTOR_COMBINATOR_SIBLING) {
			goto try_next;
		}

		selectors->current->entry = entry;
		selectors->state = lxb_selectors_state_find_forward;
	}
	else {
		selectors->current->entry->node = node;
	}

	return selectors->current->entry;
}

lxb_inline lxb_selectors_entry_t *
lxb_selectors_done(lxb_selectors_t *selectors)
{
	lxb_selectors_nested_t *current = selectors->current;

	if (current->parent == NULL) {
		return NULL;
	}

	selectors->current = current->parent;

	return selectors->current->entry;
}

lxb_inline lxb_selectors_entry_t *
lxb_selectors_exit(lxb_selectors_t *selectors)
{
	lxb_selectors_nested_t *current = selectors->current;

	if (current->parent == NULL) {
		return NULL;
	}

	selectors->state = current->return_state;

	return current->entry;
}

static lxb_selectors_entry_t *
lxb_selectors_state_found_check(lxb_selectors_t *selectors,
								lxb_selectors_entry_t *entry)
{
	lxb_selectors_nested_t *current;
	const xmlNode *node;
	lxb_selectors_entry_t *prev;
	const lxb_css_selector_t *selector;

	current = selectors->current;
	entry = current->entry;
	node = entry->node;

	if (entry->prev == NULL) {
		selector = entry->selector;

		while (selector->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE
			   && selector->prev != NULL)
		{
			selector = selector->prev;
		}

		if (selector->prev == NULL) {
			return lxb_selectors_state_found(selectors, entry);
		}

		prev = lxb_selectors_state_entry_create(selectors, selector,
												entry, node);
		current->entry = prev;
		selectors->state = lxb_selectors_state_find;

		return prev;
	}

	selectors->state = lxb_selectors_state_find;

	current->entry = entry->prev;
	entry->prev->node = node;

	return entry->prev;
}

static lxb_selectors_entry_t *
lxb_selectors_state_found_check_forward(lxb_selectors_t *selectors,
										lxb_selectors_entry_t *entry)
{
	lxb_selectors_nested_t *current;
	const xmlNode *node;
	lxb_selectors_entry_t *next;
	const lxb_css_selector_t *selector;

	current = selectors->current;
	entry = current->entry;
	node = entry->node;

	if (entry->next == NULL) {
		selector = entry->selector;

		if (selector->next == NULL) {
			return lxb_selectors_state_found_forward(selectors, entry);
		}

		next = lxb_selectors_state_entry_create_forward(selectors, selector,
														entry, node);
		current->entry = next;
		selectors->state = lxb_selectors_state_find_forward;

		return next;
	}

	selectors->state = lxb_selectors_state_find_forward;

	current->entry = entry->next;
	entry->next->node = node;

	return entry->next;
}

static lxb_selectors_entry_t *
lxb_selectors_state_found(lxb_selectors_t *selectors,
						  lxb_selectors_entry_t *entry)
{
	lxb_selectors_nested_t *current;
	const lxb_css_selector_t *selector;

	current = selectors->current;
	selector = current->entry->selector;

	selectors->state = lxb_selectors_state_find;

	selectors->status = current->cb(current->root,
									selector->list->specificity,
									current->ctx);

	if ((selectors->options & LXB_SELECTORS_OPT_MATCH_FIRST) == 0
		&& current->parent == NULL)
	{
		if (selectors->status == LXB_STATUS_OK) {
			entry = selectors->current->first;
			return lxb_selectors_next_list(selectors, entry);
		}
	}

	return lxb_selectors_done(selectors);
}

static lxb_selectors_entry_t *
lxb_selectors_state_found_forward(lxb_selectors_t *selectors,
								  lxb_selectors_entry_t *entry)
{
	lxb_selectors_nested_t *current;
	const lxb_css_selector_t *selector;

	current = selectors->current;
	selector = current->entry->selector;

	selectors->state = lxb_selectors_state_find_forward;

	selectors->status = current->cb(current->root,
									selector->list->specificity,
									current->ctx);

	if ((selectors->options & LXB_SELECTORS_OPT_MATCH_FIRST) == 0
		&& current->parent == NULL)
	{
		if (selectors->status == LXB_STATUS_OK) {
			entry = selectors->current->first;
			return lxb_selectors_next_list_forward(selectors, entry);
		}
	}

	return lxb_selectors_done(selectors);
}


static lxb_selectors_entry_t *
lxb_selectors_state_not_found(lxb_selectors_t *selectors,
							  lxb_selectors_entry_t *entry)
{
	lxb_selectors_nested_t *current;

	current = selectors->current;
	entry = current->entry;

try_next:

	if (entry->next == NULL) {
		return lxb_selectors_next_list(selectors, entry);
	}

	entry = entry->next;

	while (entry->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
		if (entry->next == NULL) {
			goto try_next;
		}

		entry = entry->next;
	}

	switch (entry->combinator) {
		case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
		case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
		case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
			goto try_next;

		default:
			break;
	}

	current->entry = entry;
	selectors->state = lxb_selectors_state_find;

	return entry;
}

static lxb_selectors_entry_t *
lxb_selectors_state_not_found_forward(lxb_selectors_t *selectors,
									  lxb_selectors_entry_t *entry)
{
try_next:

	if (entry->prev == NULL) {
		return lxb_selectors_next_list_forward(selectors, entry);
	}

	while (entry->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
		if (entry->prev == NULL) {
			goto try_next;
		}

		entry = entry->prev;
	}

	if (entry->combinator == LXB_CSS_SELECTOR_COMBINATOR_SIBLING) {
		if (entry->prev != NULL) {
			entry = entry->prev;
		}

		goto try_next;
	}

	selectors->current->entry = entry;
	selectors->state = lxb_selectors_state_find_forward;

	return entry;
}

static lxb_selectors_entry_t *
lxb_selectors_next_list(lxb_selectors_t *selectors,
						lxb_selectors_entry_t *entry)
{
	if (entry->selector->list->next == NULL) {
		return lxb_selectors_exit(selectors);
	}

	selectors->state = lxb_selectors_state_find;

	/*
	 * Try the following selectors from the selector list.
	 */

	return lxb_selectors_make_following(selectors, entry);
}

static lxb_selectors_entry_t *
lxb_selectors_next_list_forward(lxb_selectors_t *selectors,
								lxb_selectors_entry_t *entry)
{
	if (entry->selector->list->next == NULL) {
		return lxb_selectors_exit(selectors);
	}

	selectors->state = lxb_selectors_state_find_forward;

	/*
	 * Try the following selectors from the selector list.
	 */

	return lxb_selectors_make_following_forward(selectors, entry);
}

static lxb_selectors_entry_t *
lxb_selectors_make_following(lxb_selectors_t *selectors,
							 lxb_selectors_entry_t *entry)
{
	lxb_selectors_entry_t *next;
	lxb_selectors_nested_t *current;
	const lxb_css_selector_t *selector;

	selector = entry->selector;
	current = selectors->current;

	if (entry->following != NULL) {
		entry->following->node = current->root;
		current->first = entry->following;
		current->entry = entry->following;

		return entry->following;
	}

	next = lxb_selectors_entry_make_first(selectors,
										  selector->list->next->last);

	next->node = current->root;

	entry->following = next;
	current->first = next;
	current->entry = next;

	return next;
}

static lxb_selectors_entry_t *
lxb_selectors_make_following_forward(lxb_selectors_t *selectors,
									 lxb_selectors_entry_t *entry)
{
	lxb_selectors_entry_t *next;
	lxb_selectors_nested_t *current;
	const lxb_css_selector_t *selector;

	selector = entry->selector;
	current = selectors->current;

	if (entry->following != NULL) {
		entry->following->node = current->root;
		current->first = entry->following;
		current->entry = entry->following;

		return entry->following;
	}

	next = lexbor_dobject_calloc(selectors->objs);

	next->selector = selector->list->next->first;
	next->node = current->root;
	next->combinator = next->selector->combinator;

	entry->following = next;
	current->first = next;
	current->entry = next;

	return next;
}

static lxb_selectors_entry_t *
lxb_selectors_state_after_find(lxb_selectors_t *selectors,
							   lxb_selectors_entry_t *entry)
{
	selectors->current = selectors->current->parent;

	lxb_selectors_switch_to_not_found(selectors, selectors->current);

	return selectors->current->entry;
}

static lxb_selectors_entry_t *
lxb_selectors_state_after_not(lxb_selectors_t *selectors,
							  lxb_selectors_entry_t *entry)
{
	selectors->current = selectors->current->parent;

	lxb_selectors_switch_to_found_check(selectors, selectors->current);

	return selectors->current->entry;
}

lxb_inline const xmlNode *
lxb_selectors_state_nth_child_node(const lxb_css_selector_pseudo_t *pseudo,
								   const xmlNode *node)
{
	if (pseudo->type == LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD) {
		node = node->prev;

		while (node != NULL) {
			if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
				break;
			}

			node = node->prev;
		}
	}
	else {
		node = node->next;

		while (node != NULL) {
			if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
				break;
			}

			node = node->next;
		}
	}

	return node;
}

lxb_inline lxb_selectors_entry_t *
lxb_selectors_state_nth_child_done(lxb_selectors_t *selectors,
								   const lxb_css_selector_pseudo_t *pseudo,
								   size_t index)
{
	if (lxb_selectors_anb_calc(pseudo->data, index)) {
		lxb_selectors_switch_to_found_check(selectors, selectors->current);
	}
	else {
		lxb_selectors_switch_to_not_found(selectors, selectors->current);
	}

	return selectors->current->entry;
}

static lxb_selectors_entry_t *
lxb_selectors_state_after_nth_child(lxb_selectors_t *selectors,
									lxb_selectors_entry_t *entry)
{
	const xmlNode *node;
	lxb_selectors_nested_t *current;
	const lxb_css_selector_pseudo_t *pseudo;

	current = selectors->current;

	if (current->index == 0) {
		selectors->state = lxb_selectors_state_not_found;
		selectors->current = selectors->current->parent;

		return selectors->current->entry;
	}

	pseudo = &current->parent->entry->selector->u.pseudo;
	node = lxb_selectors_state_nth_child_node(pseudo, current->root);

	if (node == NULL) {
		selectors->current = selectors->current->parent;

		return lxb_selectors_state_nth_child_done(selectors, pseudo,
												  current->index);
	}

	current->root = node;
	current->entry->node = node;

	selectors->state = lxb_selectors_state_find;

	return entry;
}

static lxb_selectors_entry_t *
lxb_selectors_state_nth_child_found(lxb_selectors_t *selectors,
									lxb_selectors_entry_t *entry)
{
	const xmlNode *node;
	lxb_selectors_nested_t *current;
	const lxb_css_selector_pseudo_t *pseudo;

	current = entry->nested;
	pseudo = &entry->selector->u.pseudo;
	node = lxb_selectors_state_nth_child_node(pseudo, current->root);

	if (node == NULL) {
		return lxb_selectors_state_nth_child_done(selectors, pseudo,
												  current->index);
	}

	current->root = node;
	current->entry->node = node;

	selectors->current = current;
	selectors->state = lxb_selectors_state_find;

	return current->entry;
}

static bool
lxb_selectors_match(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					const xmlNode *node)
{
	switch (entry->selector->type) {
		case LXB_CSS_SELECTOR_TYPE_ANY:
			return true;

		case LXB_CSS_SELECTOR_TYPE_ELEMENT:
			return lxb_selectors_match_element(entry->selector, node, entry);

		case LXB_CSS_SELECTOR_TYPE_ID:
			return lxb_selectors_match_id(entry->selector, node, selectors->options & LXB_SELECTORS_OPT_QUIRKS_MODE);

		case LXB_CSS_SELECTOR_TYPE_CLASS: {
			const xmlAttr *dom_attr = lxb_selectors_adapted_attr(node, (const lxb_char_t *) "class");
			if (dom_attr == NULL) {
				return false;
			}

			dom_lxb_str_wrapper trg = lxb_selectors_adapted_attr_value(dom_attr);

			if (trg.str.length == 0) {
				dom_lxb_str_wrapper_release(&trg);
				return false;
			}
			bool ret = lxb_selectors_match_class(&trg.str,
												 &entry->selector->name, selectors->options & LXB_SELECTORS_OPT_QUIRKS_MODE);
			dom_lxb_str_wrapper_release(&trg);
			return ret;
		}

		case LXB_CSS_SELECTOR_TYPE_ATTRIBUTE:
			return lxb_selectors_match_attribute(entry->selector, node, entry);

		case LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS:
			return lxb_selectors_pseudo_class(entry->selector, node);

		case LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION:
			return lxb_selectors_pseudo_class_function(selectors,
													   entry->selector, node);
		case LXB_CSS_SELECTOR_TYPE_PSEUDO_ELEMENT:
			return lxb_selectors_pseudo_element(entry->selector, node);

		case LXB_CSS_SELECTOR_TYPE_PSEUDO_ELEMENT_FUNCTION:
			return false;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return false;
}

static bool
lxb_selectors_match_element(const lxb_css_selector_t *selector,
							const xmlNode *node, lxb_selectors_entry_t *entry)
{
	lxb_selectors_adapted_set_entry_id(entry, selector, node);
	return lxb_selectors_adapted_cmp_local_name_id(node, &entry->id);
}

static bool
lxb_selectors_match_id(const lxb_css_selector_t *selector, const xmlNode *node, bool quirks)
{
	const xmlAttr *dom_attr = lxb_selectors_adapted_attr(node, (const lxb_char_t *) "id");
	if (dom_attr == NULL) {
		return false;
	}

	const lexbor_str_t *src = &selector->name;
	dom_lxb_str_wrapper trg = lxb_selectors_adapted_attr_value(dom_attr);
	bool ret = false;
	if (trg.str.length == src->length) {
		if (quirks) {
			ret = lexbor_str_data_ncasecmp(trg.str.data, src->data, src->length);
		} else {
			ret = lexbor_str_data_ncmp(trg.str.data, src->data, src->length);
		}
	}
	dom_lxb_str_wrapper_release(&trg);

	return ret;
}

static bool
lxb_selectors_match_class(const lexbor_str_t *target, const lexbor_str_t *src,
						  bool quirks)
{
	lxb_char_t chr;

	if (target->length < src->length) {
		return false;
	}

	bool is_it = false;

	const lxb_char_t *data = target->data;
	const lxb_char_t *pos = data;
	const lxb_char_t *end = data + target->length;

	for (; data < end; data++) {
		chr = *data;

		if (lexbor_utils_whitespace(chr, ==, ||)) {

			if ((size_t) (data - pos) == src->length) {
				if (quirks) {
					is_it = lexbor_str_data_ncasecmp(pos, src->data, src->length);
				}
				else {
					is_it = lexbor_str_data_ncmp(pos, src->data, src->length);
				}

				if (is_it) {
					return true;
				}
			}

			if ((size_t) (end - data) < src->length) {
				return false;
			}

			pos = data + 1;
		}
	}

	if ((size_t) (end - pos) == src->length && src->length != 0) {
		if (quirks) {
			is_it = lexbor_str_data_ncasecmp(pos, src->data, src->length);
		}
		else {
			is_it = lexbor_str_data_ncmp(pos, src->data, src->length);
		}
	}

	return is_it;
}

static bool
lxb_selectors_match_attribute_value(const lxb_css_selector_attribute_t *attr, bool force_modifier_i, const lexbor_str_t *trg, const lexbor_str_t *src)
{
	bool res;
	bool ins = attr->modifier == LXB_CSS_SELECTOR_MODIFIER_I || force_modifier_i;

	switch (attr->match) {
		case LXB_CSS_SELECTOR_MATCH_EQUAL:      /*  = */
			if (trg->length == src->length) {
				if (ins) {
					return lexbor_str_data_ncasecmp(trg->data, src->data,
													src->length);
				}

				return lexbor_str_data_ncmp(trg->data, src->data,
											src->length);
			}

			return false;

		case LXB_CSS_SELECTOR_MATCH_INCLUDE:    /* ~= */
			return lxb_selectors_match_class(trg, src, ins);

		case LXB_CSS_SELECTOR_MATCH_DASH:       /* |= */
			if (trg->length == src->length) {
				if (ins) {
					return lexbor_str_data_ncasecmp(trg->data, src->data,
													src->length);
				}

				return lexbor_str_data_ncmp(trg->data, src->data,
											src->length);
			}

			if (trg->length > src->length) {
				if (ins) {
					res = lexbor_str_data_ncasecmp(trg->data,
												   src->data, src->length);
				}
				else {
					res = lexbor_str_data_ncmp(trg->data,
											   src->data, src->length);
				}

				if (res && trg->data[src->length] == '-') {
					return true;
				}
			}

			return false;

		case LXB_CSS_SELECTOR_MATCH_PREFIX:     /* ^= */
			if (src->length != 0 && trg->length >= src->length) {
				if (ins) {
					return lexbor_str_data_ncasecmp(trg->data, src->data,
													src->length);
				}

				return lexbor_str_data_ncmp(trg->data, src->data,
											src->length);
			}

			return false;

		case LXB_CSS_SELECTOR_MATCH_SUFFIX:     /* $= */
			if (src->length != 0 && trg->length >= src->length) {
				size_t dif = trg->length - src->length;

				if (ins) {
					return lexbor_str_data_ncasecmp(trg->data + dif,
													src->data, src->length);
				}

				return lexbor_str_data_ncmp(trg->data + dif, src->data,
											src->length);
			}

			return false;

		case LXB_CSS_SELECTOR_MATCH_SUBSTRING:  /* *= */
			if (src->length == 0) {
				return false;
			}

			if (ins) {
				return lexbor_str_data_ncasecmp_contain(trg->data, trg->length,
														src->data, src->length);
			}

			return lexbor_str_data_ncmp_contain(trg->data, trg->length,
												src->data, src->length);
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return false;
}

static bool
lxb_selectors_match_attribute(const lxb_css_selector_t *selector,
							  const xmlNode *node, lxb_selectors_entry_t *entry)
{
	const lxb_css_selector_attribute_t *attr = &selector->u.attribute;

	lxb_selectors_adapted_set_entry_id(entry, selector, node);

	const xmlAttr *dom_attr = lxb_selectors_adapted_attr(node, entry->id.name);
	if (dom_attr == NULL) {
		return false;
	}

	const lexbor_str_t *src = &attr->value;
	if (src->data == NULL) {
		return true;
	}

	dom_lxb_str_wrapper trg = lxb_selectors_adapted_attr_value(dom_attr);
	ZEND_ASSERT(node->doc != NULL);
	bool res = lxb_selectors_match_attribute_value(
		attr,
		entry->id.attr_case_insensitive && php_dom_ns_is_html_and_document_is_html(node),
		&trg.str,
		src
	);
	dom_lxb_str_wrapper_release(&trg);
	return res;
}

static bool
lxb_selectors_pseudo_class(const lxb_css_selector_t *selector,
						   const xmlNode *node)
{
	const lxb_css_selector_pseudo_t *pseudo = &selector->u.pseudo;

	static const lxb_char_t checkbox[] = "checkbox";
	static const size_t checkbox_length = sizeof(checkbox) / sizeof(lxb_char_t) - 1;

	static const lxb_char_t radio[] = "radio";
	static const size_t radio_length = sizeof(radio) / sizeof(lxb_char_t) - 1;

	switch (pseudo->type) {
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_ACTIVE:
			return false;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_ANY_LINK:
			/* https://drafts.csswg.org/selectors/#the-any-link-pseudo */
			if (php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)
				&& (lxb_selectors_adapted_cmp_local_name_literal(node, "a")
					|| lxb_selectors_adapted_cmp_local_name_literal(node, "area")))
			{
				return lxb_selectors_adapted_has_attr(node, "href");
			}

			return false;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_BLANK:
			if (!EG(exception)) {
				php_dom_throw_error_with_message(NOT_SUPPORTED_ERR, ":blank selector is not implemented because CSSWG has not yet decided its semantics (https://github.com/w3c/csswg-drafts/issues/1967)", true);
			}
			return false;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_CHECKED:
			/* https://drafts.csswg.org/selectors/#checked */
			if (!php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)) {
				return false;
			}
			if (lxb_selectors_adapted_cmp_local_name_literal(node, "input")) {
				const xmlAttr *dom_attr = lxb_selectors_adapted_attr(node, (const lxb_char_t *) "type");
				if (dom_attr == NULL) {
					return false;
				}

				dom_lxb_str_wrapper str = lxb_selectors_adapted_attr_value(dom_attr);
				bool res = false;

				if (str.str.length == 8) {
					if (lexbor_str_data_ncasecmp(checkbox, str.str.data, checkbox_length)) {
						res = lxb_selectors_adapted_has_attr(node, "checked");
					}
				}
				else if (str.str.length == 5) {
					if (lexbor_str_data_ncasecmp(radio, str.str.data, radio_length)) {
						res = lxb_selectors_adapted_has_attr(node, "checked");
					}
				}

				dom_lxb_str_wrapper_release(&str);

				return res;
			}
			else if(lxb_selectors_adapted_cmp_local_name_literal(node, "option")) {
				return lxb_selectors_adapted_has_attr(node, "selected");
			}

			return false;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_CURRENT:
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_DEFAULT:
			return false;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_DISABLED:
			return lxb_selectors_pseudo_class_disabled(node);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_EMPTY:
			node = node->children;

			while (node != NULL) {
				/* Following https://developer.mozilla.org/en-US/docs/Web/CSS/:empty, i.e. what currently happens in browsers,
				 * not the CSS Selectors Level 4 Draft that no one implements yet. */
				if (!CMP_NODE_TYPE(node, XML_COMMENT_NODE) && !CMP_NODE_TYPE(node, XML_PI_NODE)) {
					return false;
				}

				node = node->next;
			}

			return true;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_ENABLED:
			return !lxb_selectors_pseudo_class_disabled(node);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FIRST_CHILD:
			return lxb_selectors_pseudo_class_first_child(node);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FIRST_OF_TYPE:
			return lxb_selectors_pseudo_class_first_of_type(node);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS_VISIBLE:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS_WITHIN:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FULLSCREEN:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUTURE:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_HOVER:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_IN_RANGE:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_INDETERMINATE:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_INVALID:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_LAST_CHILD:
			return lxb_selectors_pseudo_class_last_child(node);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_LAST_OF_TYPE:
			return lxb_selectors_pseudo_class_last_of_type(node);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_LINK:
			/* https://html.spec.whatwg.org/multipage/semantics-other.html#selector-link */
			if (php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)
				&& (lxb_selectors_adapted_cmp_local_name_literal(node, "a")
					|| lxb_selectors_adapted_cmp_local_name_literal(node, "area")))
			{
				return lxb_selectors_adapted_has_attr(node, "href");
			}

			return false;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_LOCAL_LINK:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_ONLY_CHILD:
			return lxb_selectors_pseudo_class_first_child(node)
				&& lxb_selectors_pseudo_class_last_child(node);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_ONLY_OF_TYPE:
			return lxb_selectors_pseudo_class_first_of_type(node)
				&& lxb_selectors_pseudo_class_last_of_type(node);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_OPTIONAL:
			if (php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)
				&& (lxb_selectors_adapted_cmp_local_name_literal(node, "input")
					|| lxb_selectors_adapted_cmp_local_name_literal(node, "select")
					|| lxb_selectors_adapted_cmp_local_name_literal(node, "textarea")))
			{
				return !lxb_selectors_adapted_has_attr(node, "required");
			}

			return false;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_OUT_OF_RANGE:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_PAST:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_PLACEHOLDER_SHOWN:
			if (php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)
				&& (lxb_selectors_adapted_cmp_local_name_literal(node, "input")
					|| lxb_selectors_adapted_cmp_local_name_literal(node, "textarea")))
			{
				return lxb_selectors_adapted_has_attr(node, "placeholder");
			}

			return false;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_READ_ONLY:
			return !lxb_selectors_pseudo_class_read_write(node);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_READ_WRITE:
			return lxb_selectors_pseudo_class_read_write(node);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_REQUIRED:
			if (php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)
				&& (lxb_selectors_adapted_cmp_local_name_literal(node, "input")
					|| lxb_selectors_adapted_cmp_local_name_literal(node, "select")
					|| lxb_selectors_adapted_cmp_local_name_literal(node, "textarea")))
			{
				return lxb_selectors_adapted_has_attr(node, "required");
			}

			return false;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_ROOT:
			return node->parent != NULL
				&& (node->parent->type == XML_DOCUMENT_FRAG_NODE || node->parent->type == XML_DOCUMENT_NODE
					|| node->parent->type == XML_HTML_DOCUMENT_NODE);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_SCOPE:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_TARGET:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_TARGET_WITHIN:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_USER_INVALID:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_VALID:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_VISITED:
			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_WARNING:
			break;
	}

	return false;
}

static lxb_selectors_nested_t *
lxb_selectors_nested_make(lxb_selectors_t *selectors, const xmlNode *node,
						  lxb_css_selector_t *selector, bool forward)
{
	lxb_selectors_entry_t *next;
	lxb_selectors_entry_t *entry;

	entry = selectors->current->entry;
	entry->node = node;

	if (entry->nested == NULL) {
		if (!forward) {
			next = lxb_selectors_entry_make_first(selectors, selector);
		}
		else {
			next = lexbor_dobject_calloc(selectors->objs);

			next->combinator = selector->combinator;
			next->selector = selector;
		}

		entry->nested = lexbor_dobject_calloc(selectors->nested);

		entry->nested->top = next;
		entry->nested->parent = selectors->current;
		entry->nested->forward = forward;
	}

	selectors->current = entry->nested;
	entry->nested->entry = entry->nested->top;
	entry->nested->first = entry->nested->top;

	selectors->current->root = node;
	selectors->current->ctx = selectors;

	return selectors->current;
}

static bool
lxb_selectors_pseudo_class_function(lxb_selectors_t *selectors,
									const lxb_css_selector_t *selector,
									const xmlNode *node)
{
	size_t index;
	const xmlNode *base;
	lxb_selectors_nested_t *current;
	const lxb_css_selector_list_t *list;
	const lxb_css_selector_anb_of_t *anb;
	const lxb_css_selector_pseudo_t *pseudo;

	pseudo = &selector->u.pseudo;

	switch (pseudo->type) {
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_HAS:
			list = (lxb_css_selector_list_t *) pseudo->data;

			current = lxb_selectors_nested_make(selectors, node,
												list->first, true);

			current->cb = lxb_selectors_cb_ok;
			current->return_state = lxb_selectors_state_after_find;
			selectors->state = lxb_selectors_state_find_forward;

			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_CURRENT:
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_IS:
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_WHERE:
			list = (lxb_css_selector_list_t *) pseudo->data;

			current = lxb_selectors_nested_make(selectors, node, list->last,
												false);

			current->cb = lxb_selectors_cb_ok;
			current->return_state = lxb_selectors_state_after_find;
			selectors->state = lxb_selectors_state_find;

			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NOT:
			list = (lxb_css_selector_list_t *) pseudo->data;

			current = lxb_selectors_nested_make(selectors, node, list->last,
												false);

			current->cb = lxb_selectors_cb_not;
			current->return_state = lxb_selectors_state_after_not;
			selectors->state = lxb_selectors_state_find;

			break;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD:
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_CHILD:
			anb = pseudo->data;

			if (anb->of != NULL) {
				current = lxb_selectors_nested_make(selectors, node,
													anb->of->last, false);

				current->return_state = lxb_selectors_state_after_nth_child;
				current->cb = lxb_selectors_cb_nth_ok;
				current->index = 0;
				selectors->state = lxb_selectors_state_find;

				return true;
			}

			index = 0;

			if (pseudo->type == LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD) {
				while (node != NULL) {
					if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE))
					{
						index++;
					}

					node = node->prev;
				}
			}
			else {
				while (node != NULL) {
					if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE))
					{
						index++;
					}

					node = node->next;
				}
			}

			return lxb_selectors_anb_calc(pseudo->data, index);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_OF_TYPE:
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_OF_TYPE:
			index = 0;
			base = node;

			if (pseudo->type == LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_OF_TYPE) {
				while (node != NULL) {
					if(CMP_NODE_TYPE(node, XML_ELEMENT_NODE)
						&& xmlStrEqual(node->name, base->name)
						&& lxb_selectors_adapted_cmp_ns(node, base))
					{
						index++;
					}

					node = node->prev;
				}
			}
			else {
				while (node != NULL) {
					if(CMP_NODE_TYPE(node, XML_ELEMENT_NODE)
						&& xmlStrEqual(node->name, base->name)
						&& lxb_selectors_adapted_cmp_ns(node, base))
					{
						index++;
					}

					node = node->next;
				}
			}

			return lxb_selectors_anb_calc(pseudo->data, index);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_DIR:
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_LANG:
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_COL:
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_COL:
		default:
			return false;
	}

	return true;
}

static bool
lxb_selectors_pseudo_element(const lxb_css_selector_t *selector,
							 const xmlNode *node)
{
	const lxb_css_selector_pseudo_t *pseudo = &selector->u.pseudo;

	switch (pseudo->type) {
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_AFTER:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_BACKDROP:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_BEFORE:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_FIRST_LETTER:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_FIRST_LINE:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_GRAMMAR_ERROR:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_INACTIVE_SELECTION:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_MARKER:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_PLACEHOLDER:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_SELECTION:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_SPELLING_ERROR:
		case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_TARGET_TEXT:
			break;
	}

	return false;
}

/* https://html.spec.whatwg.org/multipage/semantics-other.html#concept-element-disabled */
static bool
lxb_selectors_pseudo_class_disabled(const xmlNode *node)
{
	if (!php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)) {
		return false;
	}

	if (lxb_selectors_adapted_has_attr(node, "disabled")
		&& (lxb_selectors_adapted_cmp_local_name_literal(node, "button")
			|| lxb_selectors_adapted_cmp_local_name_literal(node, "input")
			|| lxb_selectors_adapted_cmp_local_name_literal(node, "select")
			|| lxb_selectors_adapted_cmp_local_name_literal(node, "textarea")
			|| lxb_selectors_adapted_cmp_local_name_literal(node, "optgroup")
			|| lxb_selectors_adapted_cmp_local_name_literal(node, "fieldset")))
	{
		return true;
	}

	if (lxb_selectors_adapted_cmp_local_name_literal(node, "fieldset")) {
		const xmlNode *fieldset = node;
		node = node->parent;

		while (node != NULL && CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
			/* node is a disabled fieldset that is an ancestor of fieldset */
			if (php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)
				&& lxb_selectors_adapted_cmp_local_name_literal(node, "fieldset")
				&& lxb_selectors_adapted_has_attr(node, "disabled"))
			{
				/* Search first legend child and figure out if fieldset is a descendent from that. */
				const xmlNode *search_current = node->children;
				do {
					if (search_current->type == XML_ELEMENT_NODE
						&& php_dom_ns_is_fast(search_current, php_dom_ns_is_html_magic_token)
						&& lxb_selectors_adapted_cmp_local_name_literal(search_current, "legend")) {
						/* search_current is a legend element. */
						const xmlNode *inner_search_current = fieldset;

						/* Disabled does not apply if fieldset is a descendant from search_current */
						do {
							if (inner_search_current == search_current) {
								return false;
							}

							inner_search_current = inner_search_current->parent;
						} while (inner_search_current != NULL);

						return true;
					}

					search_current = search_current->next;
				} while (search_current != NULL);
			}

			node = node->parent;
		}
	}

	return false;
}

static bool
lxb_selectors_pseudo_class_first_child(const xmlNode *node)
{
	node = node->prev;

	while (node != NULL) {
		if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE))
		{
			return false;
		}

		node = node->prev;
	}

	return true;
}

static bool
lxb_selectors_pseudo_class_first_of_type(const xmlNode *node)
{
	const xmlNode *root = node;
	node = node->prev;

	while (node) {
		if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)
			&& xmlStrEqual(node->name, root->name)
			&& lxb_selectors_adapted_cmp_ns(node, root))
		{
			return false;
		}

		node = node->prev;
	}

	return true;
}

static bool
lxb_selectors_pseudo_class_last_child(const xmlNode *node)
{
	node = node->next;

	while (node != NULL) {
		if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE))
		{
			return false;
		}

		node = node->next;
	}

	return true;
}

static bool
lxb_selectors_pseudo_class_last_of_type(const xmlNode *node)
{
	const xmlNode *root = node;
	node = node->next;

	while (node) {
		if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)
			&& xmlStrEqual(node->name, root->name)
			&& lxb_selectors_adapted_cmp_ns(node, root))
		{
			return false;
		}

		node = node->next;
	}

	return true;
}

/* https://drafts.csswg.org/selectors/#rw-pseudos */
static bool
lxb_selectors_pseudo_class_read_write(const xmlNode *node)
{
	if (php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)) {
		if (lxb_selectors_adapted_cmp_local_name_literal(node, "input")
			|| lxb_selectors_adapted_cmp_local_name_literal(node, "textarea")) {
			return !lxb_selectors_adapted_has_attr(node, "readonly") && !lxb_selectors_adapted_has_attr(node, "disabled");
		} else {
			const xmlAttr *attr = lxb_selectors_adapted_attr(node, (const lxb_char_t *) "contenteditable");
			return attr && !dom_compare_value(attr, BAD_CAST "false");
		}
	}

	return false;
}

static bool
lxb_selectors_anb_calc(const lxb_css_selector_anb_of_t *anb, size_t index)
{
	double num;

	if (anb->anb.a == 0) {
		if (anb->anb.b >= 0 && (size_t) anb->anb.b == index) {
			return true;
		}
	}
	else {
		num = ((double) index - (double) anb->anb.b) / (double) anb->anb.a;

		if (num >= 0.0f && (num - trunc(num)) == 0.0f) {
			return true;
		}
	}

	return false;
}

static lxb_status_t
lxb_selectors_cb_ok(const xmlNode *node,
					lxb_css_selector_specificity_t spec, void *ctx)
{
	lxb_selectors_t *selectors = ctx;

	lxb_selectors_switch_to_found_check(selectors, selectors->current->parent);

	return LXB_STATUS_OK;
}

static lxb_status_t
lxb_selectors_cb_not(const xmlNode *node,
					 lxb_css_selector_specificity_t spec, void *ctx)
{
	lxb_selectors_t *selectors = ctx;

	lxb_selectors_switch_to_not_found(selectors, selectors->current->parent);

	return LXB_STATUS_OK;
}

static lxb_status_t
lxb_selectors_cb_nth_ok(const xmlNode *node,
						lxb_css_selector_specificity_t spec, void *ctx)
{
	lxb_selectors_t *selectors = ctx;

	selectors->current->index += 1;
	selectors->state = lxb_selectors_state_nth_child_found;

	return LXB_STATUS_OK;
}
