/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 * Adapted for PHP + libxml2 by: Niels Dossche <nielsdos@php.net>
 * Based on https://github.com/lexbor/lexbor/blob/795126188f8df25ed1f5fc8ed368bdd5110a8045/source/lexbor/selectors/selectors.c Lexbor 2.4.0
 */

#include <libxml/xmlstring.h>
#include <libxml/dict.h>
#include <Zend/zend.h>
#include <Zend/zend_operators.h>
#include <Zend/zend_API.h>
#include <php.h>

#include "ext/dom/namespace_compat.h"
#include "lexbor/selectors-adapted/selectors.h"

#include <math.h>

/* TODO: performance improvement idea: use more interned string comparisons when possible */

/* Note: casting and then comparing is a bit faster on my i7-4790 */
#define CMP_NODE_TYPE(node, ty) ((unsigned char) (node)->type == ty)

/**
 * The following methods are adapters to ease the porting of the
 * lexbor selectors library to libxml2, and changing the underlying implementations without having to replace libxml calls.
 */

/* Note: assumes a != b */
static zend_always_inline bool lxb_selectors_adapted_cmp_nullable_string_a_neq_b(const char *a, const char *b)
{
	ZEND_ASSERT(a != b);
	if (a == NULL || b == NULL) {
		return false; /* We know that they can't be both NULL because a != b. */
	}
	return strcmp(a, b) == 0;
}

static zend_always_inline bool lxb_selectors_adapted_cmp_ns(const xmlNode *a, const xmlNode *b)
{
	/* Namespace URIs are not interned, hence a->href != b->href. */
	return a->ns == b->ns || (a->ns != NULL && b->ns != NULL && lxb_selectors_adapted_cmp_nullable_string_a_neq_b((const char *) a->ns->href, (const char *) b->ns->href));
}

static zend_always_inline bool lxb_selectors_adapted_is_matchable_child(const xmlNode *node)
{
	return CMP_NODE_TYPE(node, XML_ELEMENT_NODE);
}

static zend_always_inline bool lxb_selectors_adapted_cmp_local_name_literal(const xmlNode *node, const char *name)
{
	return strcmp((const char *) node->name, name) == 0;
}

static zend_always_inline bool lxb_selectors_adapted_cmp_local_name_id(const xmlNode *node, const lxb_selectors_adapted_id *id)
{
	return node->name == id->name || strcmp((const char *) node->name, (const char *) id->name) == 0;
}

static zend_always_inline bool lxb_selectors_adapted_cmp_local_name_node(const xmlNode *node, const xmlNode *other)
{
	return node->name == other->name || strcmp((const char *) node->name, (const char *) other->name) == 0;
}

static zend_always_inline const xmlAttr *lxb_selectors_adapted_attr(const xmlNode *node, const lxb_char_t *name)
{
	const xmlAttr *attr = xmlHasProp(node, (const xmlChar *) name);
	if (attr != NULL && attr->ns != NULL) {
		return NULL;
	}
	return attr;
}

static zend_always_inline bool lxb_selectors_adapted_has_attr(const xmlNode *node, const char *name)
{
	return lxb_selectors_adapted_attr(node, (const lxb_char_t *) name) != NULL;
}

static zend_always_inline const lxb_char_t *lxb_selectors_adapted_attr_value_raw(const xmlAttr *attr)
{
	ZEND_ASSERT(attr != NULL);

	if (attr->children == NULL) {
		return NULL;
	}

	return (const lxb_char_t *) attr->children->content;
}

static zend_always_inline lexbor_str_t lxb_selectors_adapted_attr_value_nullable(const xmlAttr *attr)
{
	lexbor_str_t ret;
	ret.data = (lxb_char_t *) lxb_selectors_adapted_attr_value_raw(attr);
	ret.length = (ret.data == NULL) ? 0 : strlen((const char *) ret.data);
	return ret;
}

static zend_always_inline lexbor_str_t lxb_selectors_adapted_attr_value_empty(const xmlAttr *attr)
{
	lexbor_str_t ret;
	ret.data = (lxb_char_t *) lxb_selectors_adapted_attr_value_raw(attr);
	if (ret.data == NULL) {
		ret.data = (lxb_char_t *) "";
		ret.length = 0;
	} else {
		ret.length = strlen((const char *) ret.data);
	}
	return ret;
}

/**
 * The following code is adapted from Lexbor.
 */

static lxb_selectors_entry_t *
lxb_selectors_find_by_selector(lxb_selectors_t *selectors, const xmlNode *root,
							   lxb_selectors_entry_t *entry,
							   lxb_css_selector_t *selector,
							   lxb_selectors_cb_f cb, void *ctx);

static lxb_selectors_entry_child_t *
lxb_selectors_next(lxb_selectors_t *selectors, const xmlNode *root,
				   lxb_selectors_entry_child_t *child,
				   lxb_css_selector_list_t *list,
				   lxb_selectors_cb_f cb, void *ctx);

static lxb_selectors_entry_child_t *
lxb_selectors_current(lxb_selectors_t *selectors, const xmlNode *root,
					  lxb_selectors_entry_child_t *child,
					  lxb_css_selector_list_t *list,
					  lxb_selectors_cb_f cb, void *ctx);

static lxb_selectors_entry_t *
lxb_selectors_next_by_selector(lxb_selectors_t *selectors, const xmlNode *root,
							   lxb_selectors_entry_t *entry,
							   lxb_css_selector_t *selector,
							   lxb_selectors_cb_f cb, void *ctx);

static lxb_status_t
lxb_selectors_find_by(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					  const xmlNode *root, const xmlNode *node,
					  lxb_css_selector_t *selector, lxb_selectors_cb_f cb, void *ctx);

static bool
lxb_selectors_match(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					lxb_css_selector_t *selector, const xmlNode *node);

static bool
lxb_selectors_match_class(const lexbor_str_t *target, const lexbor_str_t *src,
						  bool quirks);

static bool
lxb_selectors_pseudo_class(lxb_css_selector_t *selector, const xmlNode *node);

static bool
lxb_selectors_pseudo_class_function(lxb_selectors_t *selectors,
									lxb_css_selector_t *selector,
									const xmlNode *node,
									lxb_selectors_entry_t *entry);

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

static lxb_status_t
lxb_selectors_first_match(const xmlNode *node,
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

	selectors->chld = lexbor_dobject_create();
	status = lexbor_dobject_init(selectors->chld,
								 32, sizeof(lxb_selectors_entry_child_t));
	if (status != LXB_STATUS_OK) {
		return status;
	}

	return LXB_STATUS_OK;
}

void
lxb_selectors_clean(lxb_selectors_t *selectors)
{
	lexbor_dobject_clean(selectors->objs);
	lexbor_dobject_clean(selectors->chld);
}

void
lxb_selectors_destroy(lxb_selectors_t *selectors)
{
	selectors->objs = lexbor_dobject_destroy(selectors->objs, true);
	selectors->chld = lexbor_dobject_destroy(selectors->chld, true);
}


lxb_inline const xmlNode *
lxb_selectors_descendant(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
						 lxb_css_selector_t *selector, const xmlNode *root,
						 const xmlNode *node)
{
	do {
		if (lxb_selectors_adapted_is_matchable_child(node) &&
			lxb_selectors_match(selectors, entry, selector, node))
		{
			return node;
		}

		if (node->children != NULL) {
			node = node->children;
		}
		else {
			while (node != root && node->next == NULL) {
				node = node->parent;
			}

			if (node == root) {
				return NULL;
			}

			node = node->next;
		}
	}
	while (true);
}

lxb_inline const xmlNode *
lxb_selectors_descendant_next(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
							  lxb_css_selector_t *selector, const xmlNode *root,
							  const xmlNode *node)
{
	do {
		if (node->children != NULL) {
			node = node->children;
		}
		else {
			while (node != root && node->next == NULL) {
				node = node->parent;
			}

			if (node == root) {
				return NULL;
			}

			node = node->next;
		}

		if (lxb_selectors_adapted_is_matchable_child(node) &&
			lxb_selectors_match(selectors, entry, selector, node))
		{
			return node;
		}

		/* While this helps, this doesn't seem worth it too much to take the risk of a penalty. */
#if 0
		if (node->next != NULL) {
			__builtin_prefetch(&node->next->parent);
		}
#endif
	}
	while (true);
}

lxb_inline const xmlNode *
lxb_selectors_close(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					lxb_css_selector_t *selector, const xmlNode *node)
{
	if (lxb_selectors_adapted_is_matchable_child(node) && lxb_selectors_match(selectors, entry, selector, node)) {
		return node;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_child(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					lxb_css_selector_t *selector, const xmlNode *root,
					const xmlNode *node)
{
	node = root->children;

	while (node != NULL) {
		if (lxb_selectors_adapted_is_matchable_child(node) &&
			lxb_selectors_match(selectors, entry, selector, node))
		{
			return node;
		}

		node = node->next;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_child_next(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
						 lxb_css_selector_t *selector, const xmlNode *root,
						 const xmlNode *node)
{
	node = node->next;

	while (node != NULL) {
		if (lxb_selectors_adapted_is_matchable_child(node) &&
			lxb_selectors_match(selectors, entry, selector, node))
		{
			return node;
		}

		node = node->next;
	}

	return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_sibling(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					  lxb_css_selector_t *selector, const xmlNode *node)
{
	node = node->next;

	while (node != NULL) {
		if (lxb_selectors_adapted_is_matchable_child(node)) {
			if (lxb_selectors_match(selectors, entry, selector, node)) {
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
						lxb_css_selector_t *selector, const xmlNode *node)
{
	node = node->next;

	while (node != NULL) {
		if (lxb_selectors_adapted_is_matchable_child(node) &&
			lxb_selectors_match(selectors, entry, selector, node))
		{
			return node;
		}

		node = node->next;
	}

	return NULL;
}

lxb_inline bool
lxb_selectors_anb_calc(lxb_css_selector_anb_of_t *anb, size_t index)
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

lxb_status_t
lxb_selectors_find(lxb_selectors_t *selectors, const xmlNode *root,
				   lxb_css_selector_list_t *list, lxb_selectors_cb_f cb, void *ctx)
{
	lxb_selectors_entry_t *child;

	while (list != NULL) {
		child = lxb_selectors_next_by_selector(selectors, root, NULL,
											   list->first, cb, ctx);
		if (child == NULL) {
			return LXB_STATUS_ERROR;
		}

		list = list->next;
	}

	lxb_selectors_clean(selectors);

	return LXB_STATUS_OK;
}

static lxb_selectors_entry_t *
lxb_selectors_find_by_selector(lxb_selectors_t *selectors, const xmlNode *root,
							   lxb_selectors_entry_t *entry,
							   lxb_css_selector_t *selector,
							   lxb_selectors_cb_f cb, void *ctx)
{
	lxb_status_t status;
	const xmlNode *node = root, *base = root;

	if (entry == NULL) {
		entry = lexbor_dobject_calloc(selectors->objs);
		entry->selector = selector;
	}

	switch (selector->combinator) {
		case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
			node = lxb_selectors_descendant(selectors, entry, selector,
											base, node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
			node = lxb_selectors_close(selectors, entry, selector, node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
			node = lxb_selectors_child(selectors, entry, selector, base, node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
			node = lxb_selectors_sibling(selectors, entry, selector, base);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
			node = lxb_selectors_following(selectors, entry, selector, node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CELL:
		default:
			return NULL;
	}

	status = lxb_selectors_find_by(selectors, entry, root, node, selector,
								   cb, ctx);
	if (status != LXB_STATUS_OK) {
		return NULL;
	}

	return entry;
}

static lxb_selectors_entry_child_t *
lxb_selectors_next(lxb_selectors_t *selectors, const xmlNode *root,
				   lxb_selectors_entry_child_t *child,
				   lxb_css_selector_list_t *list,
				   lxb_selectors_cb_f cb, void *ctx)
{
	lxb_selectors_entry_child_t *chld_root = child;

	if (list == NULL) {
		return NULL;
	}

	if (child == NULL) {
		child = lexbor_dobject_calloc(selectors->chld);
		chld_root = child;
	}

	do {
		child->entry = lxb_selectors_next_by_selector(selectors, root,
													  child->entry, list->first,
													  cb, ctx);
		if (child->entry == NULL) {
			return NULL;
		}

		if (list->next == NULL) {
			return chld_root;
		}

		if (child->next == NULL) {
			child->next = lexbor_dobject_calloc(selectors->chld);
		}

		child = child->next;
		list = list->next;
	}
	while (true);

	return chld_root;
}

static lxb_selectors_entry_child_t *
lxb_selectors_current(lxb_selectors_t *selectors, const xmlNode *root,
					  lxb_selectors_entry_child_t *child,
					  lxb_css_selector_list_t *list,
					  lxb_selectors_cb_f cb, void *ctx)
{
	lxb_selectors_entry_child_t *chld_root = child;

	if (list == NULL) {
		return NULL;
	}

	if (child == NULL) {
		child = lexbor_dobject_calloc(selectors->chld);
		chld_root = child;
	}

	do {
		child->entry = lxb_selectors_find_by_selector(selectors, root,
													  child->entry, list->first,
													  cb, ctx);
		if (child->entry == NULL) {
			return NULL;
		}

		if (list->next == NULL) {
			return chld_root;
		}

		if (child->next == NULL) {
			child->next = lexbor_dobject_calloc(selectors->chld);
		}

		child = child->next;
		list = list->next;
	}
	while (true);

	return chld_root;
}

static lxb_selectors_entry_t *
lxb_selectors_next_by_selector(lxb_selectors_t *selectors, const xmlNode *root,
							   lxb_selectors_entry_t *entry,
							   lxb_css_selector_t *selector,
							   lxb_selectors_cb_f cb, void *ctx)
{
	lxb_status_t status;
	const xmlNode *node = root, *base = root;

	if (entry == NULL) {
		entry = lexbor_dobject_calloc(selectors->objs);
		entry->selector = selector;
	}

	switch (selector->combinator) {
		case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
			node = lxb_selectors_descendant_next(selectors, entry, selector,
												 base, node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
			node = lxb_selectors_close(selectors, entry, selector, node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
			node = lxb_selectors_child(selectors, entry, selector, base, node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
			node = lxb_selectors_sibling(selectors, entry, selector, base);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
			node = lxb_selectors_following(selectors, entry, selector, node);
			break;

		case LXB_CSS_SELECTOR_COMBINATOR_CELL:
		default:
			selectors->status = LXB_STATUS_ERROR;
			return NULL;
	}

	status = lxb_selectors_find_by(selectors, entry, root, node, selector,
								 cb, ctx);
	if (status != LXB_STATUS_OK) {
		return NULL;
	}

	return entry;
}

static lxb_status_t
lxb_selectors_find_by(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					  const xmlNode *root, const xmlNode *node,
					  lxb_css_selector_t *selector, lxb_selectors_cb_f cb, void *ctx)
{
	lxb_status_t status;
	lxb_selectors_entry_t *next;
	const xmlNode *base = root;

	do {
		if (node == NULL) {
			if (entry->prev == NULL) {
				return LXB_STATUS_OK;
			}

			do {
				entry = entry->prev;
				selector = entry->selector;
			}
			while (entry->prev != NULL
				   && selector->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE);

			if (selector->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
				return LXB_STATUS_OK;
			}

			node = entry->node;
			base = (entry->prev != NULL) ? entry->prev->node : root;

			goto next;
		}

		if (selector->next == NULL) {
			status = cb(node, selector->list->specificity, ctx);
			if (status != LXB_STATUS_OK) {
				if (status == LXB_STATUS_STOP) {
					return LXB_STATUS_OK;
				}

				return status;
			}

			if (selector->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
				while (entry->prev != NULL
					   && entry->selector->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE)
				{
					entry = entry->prev;
				}

				selector = entry->selector;
				node = entry->node;
			}

			base = (entry->prev != NULL) ? entry->prev->node : root;

			goto next;
		}

		base = node;
		entry->node = node;

		if (entry->next == NULL) {
			next = lexbor_dobject_calloc(selectors->objs);
			next->selector = selector->next;
			next->prev = entry;
			entry->next = next;
			entry = next;
		}
		else {
			entry = entry->next;
		}

		selector = entry->selector;

		switch (selector->combinator) {
			case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
				node = lxb_selectors_descendant_next(selectors, entry, selector,
													 base, node);
				break;

			case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
				node = lxb_selectors_close(selectors, entry, selector, node);
				break;

			case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
				node = lxb_selectors_child(selectors, entry, selector,
										   base, node);
				break;

			case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
				node = lxb_selectors_sibling(selectors, entry, selector, node);
				break;

			case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
				node = lxb_selectors_following(selectors, entry,
											   selector, node);
				break;

			case LXB_CSS_SELECTOR_COMBINATOR_CELL:
			default:
				return LXB_STATUS_ERROR;
		}

		continue;

	next:

		switch (selector->combinator) {
			case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
				node = lxb_selectors_descendant_next(selectors, entry, selector,
													 base, node);
				break;

			case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
				node = lxb_selectors_close(selectors, entry, selector, node);
				break;

			case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
				node = lxb_selectors_child_next(selectors, entry, selector,
												base, node);
				break;

			case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
				node = NULL;
				break;

			case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
				node = lxb_selectors_following(selectors, entry,
											   selector, node);
				break;

			case LXB_CSS_SELECTOR_COMBINATOR_CELL:
			default:
				return LXB_STATUS_ERROR;
		}
	}
	while (true);

	return LXB_STATUS_OK;
}

static void lxb_selectors_adapted_set_entry_id_ex(lxb_selectors_entry_t *entry, lxb_css_selector_t *selector, const xmlNode *node)
{
	if (node->doc != NULL && node->doc->dict != NULL) {
		const xmlChar *interned = xmlDictExists(node->doc->dict, selector->name.data, selector->name.length);
		if (interned != NULL) {
			entry->id.name = interned;
		} else {
			entry->id.name = selector->name.data;
		}
	} else {
		entry->id.name = selector->name.data;
	}
}

static zend_always_inline void lxb_selectors_adapted_set_entry_id(lxb_selectors_entry_t *entry, lxb_css_selector_t *selector, const xmlNode *node)
{
	if (entry->id.name == NULL) {
		lxb_selectors_adapted_set_entry_id_ex(entry, selector, node);
	}
}

static bool
lxb_selectors_match(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
					lxb_css_selector_t *selector, const xmlNode *node)
{
	bool res, ins;
	const lexbor_str_t *src;
	lxb_css_selector_attribute_t *attr;
	const xmlAttr *dom_attr;
	lexbor_str_t trg;

	switch (selector->type) {
		case LXB_CSS_SELECTOR_TYPE_ANY:
			return true;

		case LXB_CSS_SELECTOR_TYPE_ELEMENT:
			lxb_selectors_adapted_set_entry_id(entry, selector, node);

			if (lxb_selectors_adapted_cmp_local_name_id(node, &entry->id)) {
				return true;
			}

			break;

		case LXB_CSS_SELECTOR_TYPE_ID:
			dom_attr = lxb_selectors_adapted_attr(node, (const lxb_char_t *) "id");
			if (dom_attr == NULL) {
				return false;
			}

			src = &selector->name;
			trg = lxb_selectors_adapted_attr_value_empty(dom_attr);
			if (trg.length == src->length
				&& lexbor_str_data_ncasecmp(trg.data, src->data, src->length))
			{
				return true;
			}

			return false;

		case LXB_CSS_SELECTOR_TYPE_CLASS:
			dom_attr = lxb_selectors_adapted_attr(node, (const lxb_char_t *) "class");
			if (dom_attr == NULL) {
				return false;
			}

			trg = lxb_selectors_adapted_attr_value_empty(dom_attr);

			if (trg.length == 0) {
				return false;
			}

			return lxb_selectors_match_class(&trg,
											 &selector->name, true);

		case LXB_CSS_SELECTOR_TYPE_ATTRIBUTE:
			attr = &selector->u.attribute;

			lxb_selectors_adapted_set_entry_id(entry, selector, node);

			dom_attr = lxb_selectors_adapted_attr(node, entry->id.name);
			if (dom_attr == NULL) {
				return false;
			}

			src = &attr->value;

			if (src->data == NULL) {
				return true;
			}

			trg = lxb_selectors_adapted_attr_value_empty(dom_attr);

			ins = attr->modifier == LXB_CSS_SELECTOR_MODIFIER_I;

			switch (attr->match) {
				case LXB_CSS_SELECTOR_MATCH_EQUAL:      /*  = */
					if (trg.length == src->length) {
						if (ins) {
							return lexbor_str_data_ncasecmp(trg.data, src->data,
															src->length);
						}

						return lexbor_str_data_ncmp(trg.data, src->data,
													src->length);
					}

					return false;

				case LXB_CSS_SELECTOR_MATCH_INCLUDE:    /* ~= */
					return lxb_selectors_match_class(&trg, src, ins);

				case LXB_CSS_SELECTOR_MATCH_DASH:       /* |= */
					if (trg.length == src->length) {
						if (ins) {
							return lexbor_str_data_ncasecmp(trg.data, src->data,
															src->length);
						}

						return lexbor_str_data_ncmp(trg.data, src->data,
													src->length);
					}

					if (trg.length > src->length) {
						if (ins) {
							res = lexbor_str_data_ncasecmp(trg.data,
														   src->data, src->length);
						}
						else {
							res = lexbor_str_data_ncmp(trg.data,
													   src->data, src->length);
						}

						if (res && trg.data[src->length] == '-') {
							return true;
						}
					}

					return false;

				case LXB_CSS_SELECTOR_MATCH_PREFIX:     /* ^= */
					if (src->length != 0 && trg.length >= src->length) {
						if (ins) {
							return lexbor_str_data_ncasecmp(trg.data, src->data,
															src->length);
						}

						return lexbor_str_data_ncmp(trg.data, src->data,
													src->length);
					}

					return false;

				case LXB_CSS_SELECTOR_MATCH_SUFFIX:     /* $= */
					if (src->length != 0 && trg.length >= src->length) {
						size_t dif = trg.length - src->length;

						if (ins) {
							return lexbor_str_data_ncasecmp(trg.data + dif,
															src->data, src->length);
						}

						return lexbor_str_data_ncmp(trg.data + dif, src->data,
													src->length);
					}

					return false;

				case LXB_CSS_SELECTOR_MATCH_SUBSTRING:  /* *= */
					if (src->length == 0) {
						return false;
					}

					if (ins) {
						return lexbor_str_data_ncasecmp_contain(trg.data, trg.length,
																src->data, src->length);
					}

					return lexbor_str_data_ncmp_contain(trg.data, trg.length,
														src->data, src->length);

				EMPTY_SWITCH_DEFAULT_CASE()
			}

			break;

		case LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS:
			return lxb_selectors_pseudo_class(selector, node);

		case LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION:
			return lxb_selectors_pseudo_class_function(selectors, selector,
													   node, entry);

		default:
			break;
	}

	return false;
}

static bool
lxb_selectors_match_class(const lexbor_str_t *target, const  lexbor_str_t *src,
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
lxb_selectors_pseudo_class(lxb_css_selector_t *selector, const xmlNode *node)
{
	const xmlAttr *dom_attr;
	lxb_css_selector_pseudo_t *pseudo = &selector->u.pseudo;

	static const lxb_char_t checkbox[] = "checkbox";
	static const size_t checkbox_length = sizeof(checkbox) / sizeof(lxb_char_t) - 1;

	static const lxb_char_t radio[] = "radio";
	static const size_t radio_length = sizeof(radio) / sizeof(lxb_char_t) - 1;

	switch (pseudo->type) {
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
				zend_argument_value_error(1, ":blank selector is not implemented because CSSWG has not yet decided its semantics (https://github.com/w3c/csswg-drafts/issues/1967)");
			}
			return false;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_CHECKED:
			/* https://drafts.csswg.org/selectors/#checked */
			if (!php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)) {
				return false;
			}
			if (lxb_selectors_adapted_cmp_local_name_literal(node, "input")) {
				dom_attr = lxb_selectors_adapted_attr(node, (const lxb_char_t *) "type");
				if (dom_attr == NULL) {
					return false;
				}

				lexbor_str_t str = lxb_selectors_adapted_attr_value_nullable(dom_attr);

				if (str.data == NULL) {
					return false;
				}

				if(str.length == 8) {
					if (lexbor_str_data_ncasecmp(checkbox, str.data, checkbox_length)) {
						return lxb_selectors_adapted_has_attr(node, "checked");
					}
				}
				else if(str.length == 5) {
					if (lexbor_str_data_ncasecmp(radio, str.data, radio_length)) {
						return lxb_selectors_adapted_has_attr(node, "checked");
					}
				}
			}
			else if(lxb_selectors_adapted_cmp_local_name_literal(node, "option")) {
				return lxb_selectors_adapted_has_attr(node, "selected");
			}

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
	}

	return false;
}

static bool
lxb_selectors_pseudo_class_function(lxb_selectors_t *selectors,
									lxb_css_selector_t *selector,
									const xmlNode *node,
									lxb_selectors_entry_t *entry)
{
	size_t index;
	bool found = false;
	const xmlNode *base;
	lxb_css_selector_anb_of_t *anb;
	lxb_css_selector_pseudo_t *pseudo = &selector->u.pseudo;

	switch (pseudo->type) {
		/* These four selectors share the same code path here, but will return the right thing based on their selector entry data. */
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_CURRENT:
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_HAS:
		/* Only difference between the following two selectors is specificity, which for matching elements doesn't matter. */
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_WHERE:
		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_IS:
			entry->child = lxb_selectors_next(selectors, node, entry->child,
											  pseudo->data,
											  lxb_selectors_first_match, &found);
			if (entry->child == NULL) {
				return false;
			}

			return found;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NOT:
			entry->child = lxb_selectors_next(selectors, node, entry->child,
											  pseudo->data,
											  lxb_selectors_first_match, &found);
			if (entry->child == NULL) {
				return false;
			}

			return !found;

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD:
			index = 0;
			found = false;
			anb = selector->u.pseudo.data;

			if (anb->of != NULL) {
				while (node != NULL) {
					if (lxb_selectors_adapted_is_matchable_child(node))
					{
						entry->child = lxb_selectors_current(selectors, node, entry->child,
															 anb->of, lxb_selectors_first_match,
															 &found);
						if (entry->child == NULL) {
							return false;
						}

						if (found) {
							index++;
						} else if (index == 0) {
							/* If the node we're currently at doesn't match the "of" requirement,
							 * then there's no point computing the index. */
							return false;
						}

						found = false;
					}

					node = node->prev;
				}
			}
			else {
				while (node != NULL) {
					if (lxb_selectors_adapted_is_matchable_child(node))
					{
						index++;
					}

					node = node->prev;
				}
			}

			return lxb_selectors_anb_calc(anb, index);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_CHILD:
			index = 0;
			found = false;
			anb = selector->u.pseudo.data;

			if (anb->of != NULL) {
				while (node != NULL) {
					if (lxb_selectors_adapted_is_matchable_child(node))
					{
						entry->child = lxb_selectors_current(selectors, node, entry->child,
															 anb->of, lxb_selectors_first_match,
															 &found);
						if (entry->child == NULL) {
							return false;
						}

						if (found) {
							index++;
						} else if (index == 0) {
							/* If the node we're currently at doesn't match the "of" requirement,
							 * then there's no point computing the index. */
							return false;
						}

						found = false;
					}

					node = node->next;
				}
			}
			else {
				while (node != NULL) {
					if (lxb_selectors_adapted_is_matchable_child(node))
					{
						index++;
					}

					node = node->next;
				}
			}

			return lxb_selectors_anb_calc(anb, index);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_OF_TYPE:
			index = 0;
			found = false;
			anb = selector->u.pseudo.data;
			base = node;

			while (node != NULL) {
				if(lxb_selectors_adapted_cmp_local_name_node(node, base)
				   && lxb_selectors_adapted_cmp_ns(node, base))
				{
					index++;
				}

				node = node->next;
			}

			return lxb_selectors_anb_calc(anb, index);

		case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_OF_TYPE:
			index = 0;
			found = false;
			anb = selector->u.pseudo.data;
			base = node;

			while (node != NULL) {
				if(lxb_selectors_adapted_cmp_local_name_node(node, base)
				   && lxb_selectors_adapted_cmp_ns(node, base))
				{
					index++;
				}

				node = node->prev;
			}

			return lxb_selectors_anb_calc(anb, index);
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

		while (node != NULL && lxb_selectors_adapted_is_matchable_child(node)) {
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
		if (lxb_selectors_adapted_is_matchable_child(node))
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
		if (lxb_selectors_adapted_cmp_local_name_node(node, root)
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
		if (lxb_selectors_adapted_is_matchable_child(node))
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
		if (lxb_selectors_adapted_cmp_local_name_node(node, root)
		   && lxb_selectors_adapted_cmp_ns(node, root))
		{
			return false;
		}

		node = node->next;
	}

	return true;
}

static bool
lxb_selectors_pseudo_class_read_write(const xmlNode *node)
{
	if (php_dom_ns_is_fast(node, php_dom_ns_is_html_magic_token)
		&& (lxb_selectors_adapted_cmp_local_name_literal(node, "input")
			|| lxb_selectors_adapted_cmp_local_name_literal(node, "textarea")))
	{
		if (lxb_selectors_adapted_has_attr(node, "readonly")) {
			return false;
		}

		return !lxb_selectors_adapted_has_attr(node, "disabled");
	}

	return false;
}

static lxb_status_t
lxb_selectors_first_match(const xmlNode *node,
						  lxb_css_selector_specificity_t spec, void *ctx)
{
	*((bool *) ctx) = true;
	return LXB_STATUS_STOP;
}
