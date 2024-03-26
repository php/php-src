/*
 * Copyright (C) 2021-2024 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 * Adapted for PHP + libxml2 by: Niels Dossche <nielsdos@php.net>
 * Based on Lexbor 2.4.0 (upstream commit d190520f9be4a9076411c70319df7de8318d8241)
 */

#include <libxml/xmlstring.h>
#include <libxml/dict.h>
#include <Zend/zend.h>
#include <Zend/zend_operators.h>
#include <Zend/zend_API.h>
#include <php.h>

#include "lexbor/selectors-adapted/selectors.h"
#include "ext/dom/namespace_compat.h"
#include "ext/dom/domexception.h"

#include <math.h>

/* Note: casting and then comparing is a bit faster on my i7-4790 */
#define CMP_NODE_TYPE(node, ty) ((unsigned char) (node)->type == ty)

static zend_always_inline bool lxb_selectors_adapted_is_matchable_child(const xmlNode *node)
{
	return CMP_NODE_TYPE(node, XML_ELEMENT_NODE);
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
	if ((ptr & (ZEND_MM_ALIGNMENT - 1)) != 0) {
		/* It cannot be a heap-allocated string because the pointer is not properly aligned for a heap allocation.
		 * Therefore, it must be interned into the dictionary pool. */
		return node->name == id->name;
	}

	return strcmp((const char *) node->name, (const char *) id->name) == 0;
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

static void lxb_selectors_adapted_set_entry_id_ex(lxb_selectors_entry_t *entry, const lxb_css_selector_t *selector, const xmlNode *node)
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

static zend_always_inline void lxb_selectors_adapted_set_entry_id(lxb_selectors_entry_t *entry, const lxb_css_selector_t *selector, const xmlNode *node)
{
	if (entry->id.name == NULL) {
		lxb_selectors_adapted_set_entry_id_ex(entry, selector, node);
	}
}

static lxb_status_t
lxb_selectors_state_tree(lxb_selectors_t *selectors, const xmlNode *root,
                         const lxb_css_selector_list_t *list);

static lxb_status_t
lxb_selectors_state_run(lxb_selectors_t *selectors, const xmlNode *node,
                        const lxb_css_selector_list_t *list);

static lxb_selectors_entry_t *
lxb_selectors_state_find(lxb_selectors_t *selectors,
                         lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_find_check(lxb_selectors_t *selectors, const xmlNode *node,
                               const lxb_css_selector_t *selector,
                               lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_pseudo_class_function(lxb_selectors_t *selectors,
                                          lxb_selectors_entry_t *entry);

static const xmlNode *
lxb_selectors_next_node(lxb_selectors_nested_t *main);

static const xmlNode *
lxb_selectors_state_has_relative(const xmlNode *node,
                                 const lxb_css_selector_t *selector);

static lxb_selectors_entry_t *
lxb_selectors_state_after_find_has(lxb_selectors_t *selectors,
                                   lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_after_find(lxb_selectors_t *selectors,
                               lxb_selectors_entry_t *entry);

static lxb_selectors_entry_t *
lxb_selectors_state_after_nth_child(lxb_selectors_t *selectors,
                                    lxb_selectors_entry_t *entry);

static bool
lxb_selectors_match(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                    const lxb_css_selector_t *selector, const xmlNode *node);

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
lxb_selectors_pseudo_class_function(const lxb_css_selector_t *selector,
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
lxb_selectors_anb_calc(lxb_css_selector_anb_of_t *anb, size_t index);

static lxb_status_t
lxb_selectors_cb_ok(const xmlNode *node,
                    lxb_css_selector_specificity_t spec, void *ctx);

static lxb_status_t
lxb_selectors_cb_not(const xmlNode *node,
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

lxb_inline const xmlNode *
lxb_selectors_descendant(lxb_selectors_t *selectors,
                         lxb_selectors_entry_t *entry,
                         const lxb_css_selector_t *selector,
                         const xmlNode *node)
{
    node = node->parent;

    while (node != NULL) {
        if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)
            && lxb_selectors_match(selectors, entry, selector, node))
        {
            return node;
        }

        node = node->parent;
    }

    return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_close(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                    const lxb_css_selector_t *selector, const xmlNode *node)
{
    if (lxb_selectors_match(selectors, entry, selector, node)) {
        return node;
    }

    return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_child(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                    const lxb_css_selector_t *selector, const xmlNode *root)
{
    root = root->parent;

    if (root != NULL && CMP_NODE_TYPE(root, XML_ELEMENT_NODE)
        && lxb_selectors_match(selectors, entry, selector, root))
    {
        return root;
    }

    return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_sibling(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                      const lxb_css_selector_t *selector, const xmlNode *node)
{
    node = node->prev;

    while (node != NULL) {
        if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
            if (lxb_selectors_match(selectors, entry, selector, node)) {
                return node;
            }

            return NULL;
        }

        node = node->prev;
    }

    return NULL;
}

lxb_inline const xmlNode *
lxb_selectors_following(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                        const lxb_css_selector_t *selector, const xmlNode *node)
{
    node = node->prev;

    while (node != NULL) {
        if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE) &&
            lxb_selectors_match(selectors, entry, selector, node))
        {
            return node;
        }

        node = node->prev;
    }

    return NULL;
}

lxb_status_t
lxb_selectors_find(lxb_selectors_t *selectors, const xmlNode *root,
                   const lxb_css_selector_list_t *list,
                   lxb_selectors_cb_f cb, void *ctx)
{
    lxb_selectors_entry_t *entry;
    lxb_selectors_nested_t nested;

    entry = lexbor_dobject_calloc(selectors->objs);

    entry->combinator = LXB_CSS_SELECTOR_COMBINATOR_CLOSE;
    entry->selector = list->last;

    nested.parent = NULL;
    nested.entry = entry;
    nested.cb = cb;
    nested.ctx = ctx;

    selectors->current = &nested;
    selectors->status = LXB_STATUS_OK;

    return lxb_selectors_state_tree(selectors, root, list);
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

    entry = lexbor_dobject_calloc(selectors->objs);

    entry->combinator = LXB_CSS_SELECTOR_COMBINATOR_CLOSE;
    entry->selector = list->last;

    nested.parent = NULL;
    nested.entry = entry;
    nested.cb = cb;
    nested.ctx = ctx;

    selectors->current = &nested;
    selectors->status = LXB_STATUS_OK;

    status = lxb_selectors_state_run(selectors, node, list);

    lxb_selectors_clean(selectors);

    return status;
}

static lxb_status_t
lxb_selectors_state_tree(lxb_selectors_t *selectors, const xmlNode *root,
                         const lxb_css_selector_list_t *list)
{
    lxb_status_t status;
    const xmlNode *node;

    if (selectors->options & LXB_SELECTORS_OPT_MATCH_ROOT) {
        node = root;

        if (CMP_NODE_TYPE(node, XML_DOCUMENT_NODE) || CMP_NODE_TYPE(node, XML_HTML_DOCUMENT_NODE)) {
            node = root->children;
        }
    }
    else {
        node = root->children;
    }

    if (node == NULL) {
        goto out;
    }

    do {
        if (!CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
            goto next;
        }

        status = lxb_selectors_state_run(selectors, node, list);
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
lxb_selectors_state_run(lxb_selectors_t *selectors, const xmlNode *node,
                        const lxb_css_selector_list_t *list)
{
    lxb_selectors_entry_t *entry;

    entry = selectors->current->entry;

    entry->node = node;
    selectors->state = lxb_selectors_state_find;
    selectors->first = entry;

again:

    do {
        entry = selectors->state(selectors, entry);
    }
    while (entry != NULL);

    if (selectors->current->parent != NULL
        && selectors->status == LXB_STATUS_OK)
    {
        entry = selectors->current->entry;
        selectors->state = selectors->current->return_state;

        goto again;
    }

    return selectors->status;
}

static lxb_selectors_entry_t *
lxb_selectors_state_find(lxb_selectors_t *selectors,
                         lxb_selectors_entry_t *entry)
{
    const xmlNode *node;
    lxb_selectors_entry_t *next;
    const lxb_css_selector_t *selector;
    const lxb_css_selector_anb_of_t *anb;
    const lxb_css_selector_pseudo_t *pseudo;

    selector = entry->selector;

    if (selector->type == LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION) {
        pseudo = &selector->u.pseudo;

        /* Optimizing. */

        switch (pseudo->type) {
            case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD:
            case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_CHILD:
                anb = pseudo->data;

                if (anb->of != NULL) {
                    break;
                }

                goto godoit;

            case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_OF_TYPE:
            case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_OF_TYPE:
                goto godoit;

            default:
                break;
        }

        if (entry->nested == NULL) {
            next = lexbor_dobject_calloc(selectors->objs);

            next->combinator = LXB_CSS_SELECTOR_COMBINATOR_CLOSE;

            entry->nested = lexbor_dobject_calloc(selectors->nested);

            entry->nested->entry = next;
            entry->nested->parent = selectors->current;
        }

        selectors->state = lxb_selectors_state_pseudo_class_function;
        selectors->current->last = entry;
        selectors->current = entry->nested;

        next = entry->nested->entry;
        next->node = entry->node;

        return next;
    }

godoit:

    switch (entry->combinator) {
        case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
            node = lxb_selectors_descendant(selectors, entry,
                                            selector, entry->node);
            break;

        case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
            node = lxb_selectors_close(selectors, entry,
                                       selector, entry->node);
            break;

        case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
            node = lxb_selectors_child(selectors, entry,
                                       selector, entry->node);
            break;

        case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
            node = lxb_selectors_sibling(selectors, entry,
                                         selector, entry->node);
            break;

        case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
            node = lxb_selectors_following(selectors, entry,
                                           selector, entry->node);
            break;

        case LXB_CSS_SELECTOR_COMBINATOR_CELL:
        default:
            selectors->status = LXB_STATUS_ERROR;
            return NULL;
    }

    return lxb_selectors_state_find_check(selectors, node, selector, entry);
}

static lxb_selectors_entry_t *
lxb_selectors_state_find_check(lxb_selectors_t *selectors, const xmlNode *node,
                               const lxb_css_selector_t *selector,
                               lxb_selectors_entry_t *entry)
{
    lxb_selectors_entry_t *next;
    lxb_selectors_nested_t *current;

    if (node == NULL) {

    try_next:

        if (entry->next == NULL) {

        try_next_list:

            if (selector->list->next == NULL) {
                return NULL;
            }

            /*
             * Try the following selectors from the selector list.
             */

            if (entry->following != NULL) {
                entry->following->node = entry->node;

                if (selectors->current->parent == NULL) {
                    selectors->first = entry->following;
                }

                return entry->following;
            }

            next = lexbor_dobject_calloc(selectors->objs);

            next->combinator = LXB_CSS_SELECTOR_COMBINATOR_CLOSE;
            next->selector = selector->list->next->last;
            next->node = entry->node;

            entry->following = next;

            if (selectors->current->parent == NULL) {
                selectors->first = next;
            }

            return next;
        }

        do {
            entry = entry->next;

            while (entry->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
                if (entry->next == NULL) {
                    selector = entry->selector;
                    goto try_next;
                }

                entry = entry->next;
            }

            switch (entry->combinator) {
                case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
                    node = entry->node->parent;

                    if (node == NULL
                        || !CMP_NODE_TYPE(node, XML_ELEMENT_NODE))
                    {
                        node = NULL;
                    }

                    break;

                case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
                    node = entry->node->prev;
                    break;

                case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
                case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
                case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
                    node = NULL;
                    break;

                case LXB_CSS_SELECTOR_COMBINATOR_CELL:
                default:
                    selectors->status = LXB_STATUS_ERROR;
                    return NULL;
            }
        }
        while (node == NULL);

        entry->node = node;

        return entry;
    }

    if (selector->prev == NULL) {
        current = selectors->current;

        selectors->status = current->cb(current->entry->node,
                                        selector->list->specificity,
                                        current->ctx);

        if ((selectors->options & LXB_SELECTORS_OPT_MATCH_FIRST) == 0
            && current->parent == NULL)
        {
            if (selectors->status == LXB_STATUS_OK) {
                entry = selectors->first;
                goto try_next_list;
            }
        }

        return NULL;
    }

    if (entry->prev == NULL) {
        next = lexbor_dobject_calloc(selectors->objs);

        next->combinator = selector->combinator;
        next->selector = selector->prev;
        next->node = node;

        next->next = entry;
        entry->prev = next;

        return next;
    }

    entry->prev->node = node;

    return entry->prev;
}

static lxb_selectors_entry_t *
lxb_selectors_state_pseudo_class_function(lxb_selectors_t *selectors,
                                          lxb_selectors_entry_t *entry)
{
    const xmlNode *node, *base;
    lxb_selectors_nested_t *current;
    const lxb_css_selector_list_t *list;
    lxb_css_selector_anb_of_t *anb;
    const lxb_css_selector_pseudo_t *pseudo;

    current = selectors->current;

    base = lxb_selectors_next_node(current);
    if (base == NULL) {
        goto not_found;
    }

    pseudo = &current->parent->last->selector->u.pseudo;

    switch (pseudo->type) {
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_HAS:
            list = (lxb_css_selector_list_t *) pseudo->data;
            node = lxb_selectors_state_has_relative(base, list->first);

            if (node == NULL) {
                selectors->current = selectors->current->parent;
                entry = selectors->current->last;

                selectors->state = lxb_selectors_state_find;

                return lxb_selectors_state_find_check(selectors, NULL,
                                                      entry->selector, entry);
            }

            current->root = base;

            current->entry->selector = list->last;
            current->entry->node = node;
            current->return_state = lxb_selectors_state_after_find_has;
            current->cb = lxb_selectors_cb_ok;
            current->ctx = &current->found;
            current->found = false;

            selectors->state = lxb_selectors_state_find;

            return entry;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_CURRENT:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_IS:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_WHERE:
            current->entry->selector = ((lxb_css_selector_list_t *) pseudo->data)->last;
            current->entry->node = base;
            current->return_state = lxb_selectors_state_after_find;
            current->cb = lxb_selectors_cb_ok;
            current->ctx = &current->found;
            current->found = false;

            selectors->state = lxb_selectors_state_find;

            return entry;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NOT:
            current->entry->selector = ((lxb_css_selector_list_t *) pseudo->data)->last;
            current->entry->node = base;
            current->return_state = lxb_selectors_state_after_find;
            current->cb = lxb_selectors_cb_not;
            current->ctx = &current->found;
            current->found = true;

            selectors->state = lxb_selectors_state_find;

            return entry;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_CHILD:
            anb = pseudo->data;

            current->entry->selector = anb->of->last;
            current->entry->node = base;
            current->return_state = lxb_selectors_state_after_nth_child;
            current->cb = lxb_selectors_cb_ok;
            current->ctx = &current->found;
            current->root = base;
            current->index = 0;
            current->found = false;

            selectors->state = lxb_selectors_state_find;

            return entry;

        /*
         * This one can only happen if the user has somehow messed up the
         * selector.
         */

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_OF_TYPE:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_OF_TYPE:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_DIR:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_LANG:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_COL:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_COL:
        default:
            break;
    }

not_found:

    selectors->current = selectors->current->parent;
    entry = selectors->current->last;

    selectors->state = lxb_selectors_state_find;

    return lxb_selectors_state_find_check(selectors, NULL,
                                          entry->selector, entry);
}

static const xmlNode *
lxb_selectors_next_node(lxb_selectors_nested_t *main)
{
    const xmlNode *node = main->entry->node;

    switch (main->parent->last->combinator) {
        case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
        case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
            if (node->parent == NULL
                || !CMP_NODE_TYPE(node->parent, XML_ELEMENT_NODE))
            {
                return NULL;
            }

            return node->parent;

        case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
            return node;

        case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
        case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
            node = node->prev;
            break;

        default:
            return NULL;
    }

    while (node != NULL) {
        if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
            break;
        }

        node = node->prev;
    }

    return node;
}

static const xmlNode *
lxb_selectors_state_has_relative(const xmlNode *node,
                                 const lxb_css_selector_t *selector)
{
    const xmlNode *root = node;

    switch (selector->combinator) {
        case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
        case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
            node = node->children;
            break;

        case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
        case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
            node = node->next;
            break;

        default:
            return NULL;
    }

    while (node != NULL) {
        if (CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
            break;
        }

        while (node !=root && node->next == NULL) {
            node = node->parent;
        }

        if (node == root) {
            return NULL;
        }

        node = node->next;
    }

    return node;
}

static lxb_selectors_entry_t *
lxb_selectors_state_after_find_has(lxb_selectors_t *selectors,
                                   lxb_selectors_entry_t *entry)
{
    const xmlNode *node;
    lxb_selectors_entry_t *parent;
    lxb_selectors_nested_t *current;

    if (selectors->current->found) {
        node = selectors->current->root;

        selectors->current = selectors->current->parent;
        parent = selectors->current->last;

        selectors->state = lxb_selectors_state_find;

        return lxb_selectors_state_find_check(selectors, node,
                                              parent->selector, parent);
    }

    current = selectors->current;
    node = entry->node;

    switch (entry->selector->list->first->combinator) {
        case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
            if (node->children != NULL) {
                node = node->children;
            }
            else {

            next:

                while (node != current->root && node->next == NULL) {
                    node = node->parent;
                }

                if (node == current->root) {
                    goto failed;
                }

                node = node->next;
            }

            if (!CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
                goto next;
            }

            break;

        case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
        case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
            node = node->next;

            while (node != NULL && !CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
                node = node->next;
            }

            if (node == NULL) {
                goto failed;
            }

            break;

        case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
            goto failed;

        case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
        case LXB_CSS_SELECTOR_COMBINATOR_CELL:
        default:
            selectors->status = LXB_STATUS_ERROR;
            return NULL;
    }

    entry->node = node;
    selectors->state = lxb_selectors_state_find;

    return entry;

failed:

    selectors->current = selectors->current->parent;
    parent = selectors->current->last;

    selectors->state = lxb_selectors_state_find;

    return lxb_selectors_state_find_check(selectors, NULL,
                                          parent->selector, parent);
}


static lxb_selectors_entry_t *
lxb_selectors_state_after_find(lxb_selectors_t *selectors,
                               lxb_selectors_entry_t *entry)
{
    const xmlNode *node;
    lxb_selectors_entry_t *parent;
    lxb_selectors_nested_t *current;

    current = selectors->current;

    if (current->found) {
        node = entry->node;

        selectors->current = current->parent;
        parent = selectors->current->last;

        selectors->state = lxb_selectors_state_find;

        return lxb_selectors_state_find_check(selectors, node,
                                              parent->selector, parent);
    }

    node = entry->node;

    switch (current->parent->last->combinator) {
        case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
            if (node->parent != NULL
                && CMP_NODE_TYPE(node->parent, XML_ELEMENT_NODE))
            {
                node = node->parent;
            }
            else {
                node = NULL;
            }

            break;

        case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
            node = node->prev;

            while (node != NULL && !CMP_NODE_TYPE(node, XML_ELEMENT_NODE)) {
                node = node->prev;
            }

            break;

        case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
        case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
        case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
            node = NULL;
            break;

        case LXB_CSS_SELECTOR_COMBINATOR_CELL:
        default:
            selectors->status = LXB_STATUS_ERROR;
            return NULL;
    }

    if (node == NULL) {
        selectors->current = current->parent;
        parent = selectors->current->last;

        selectors->state = lxb_selectors_state_find;

        return lxb_selectors_state_find_check(selectors, node,
                                              parent->selector, parent);
    }

    entry->node = node;
    selectors->state = lxb_selectors_state_find;

    return entry;
}

static lxb_selectors_entry_t *
lxb_selectors_state_after_nth_child(lxb_selectors_t *selectors,
                                    lxb_selectors_entry_t *entry)
{
    bool found;
    const xmlNode *node;
    lxb_selectors_entry_t *parent;
    lxb_selectors_nested_t *current;
    const lxb_css_selector_t *selector;
    const lxb_css_selector_pseudo_t *pseudo;

    current = selectors->current;
    selector = current->parent->last->selector;
    pseudo = &selector->u.pseudo;

    node = entry->node;

    if (current->found) {
        current->index += 1;
    }
    else if (current->root == node) {
        node = NULL;
        goto done;
    }

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

    if (node == NULL) {
        goto done;
    }

    entry->node = node;
    current->found = false;
    selectors->state = lxb_selectors_state_find;

    return entry;

done:

    if (current->index > 0) {
        found = lxb_selectors_anb_calc(pseudo->data, current->index);

        node = (found) ? current->root : NULL;
    }

    selectors->state = lxb_selectors_state_find;
    selectors->current = selectors->current->parent;

    parent = selectors->current->last;

    return lxb_selectors_state_find_check(selectors, node,
                                          parent->selector, parent);
}

static bool
lxb_selectors_match(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                    const lxb_css_selector_t *selector, const xmlNode *node)
{
    switch (selector->type) {
        case LXB_CSS_SELECTOR_TYPE_ANY:
            return true;

        case LXB_CSS_SELECTOR_TYPE_ELEMENT:
            return lxb_selectors_match_element(selector, node, entry);

        case LXB_CSS_SELECTOR_TYPE_ID:
            return lxb_selectors_match_id(selector, node, selectors->options & LXB_SELECTORS_OPT_QUIRKS_MODE);

        case LXB_CSS_SELECTOR_TYPE_CLASS: {
            const xmlAttr *dom_attr = lxb_selectors_adapted_attr(node, (const lxb_char_t *) "class");
			if (dom_attr == NULL) {
				return false;
			}

			lexbor_str_t trg = lxb_selectors_adapted_attr_value_empty(dom_attr);

			if (trg.length == 0) {
				return false;
			}

			return lxb_selectors_match_class(&trg,
											 &selector->name, selectors->options & LXB_SELECTORS_OPT_QUIRKS_MODE);
		}

        case LXB_CSS_SELECTOR_TYPE_ATTRIBUTE:
            return lxb_selectors_match_attribute(selector, node, entry);

        case LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS:
            return lxb_selectors_pseudo_class(selector, node);

        case LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION:
            return lxb_selectors_pseudo_class_function(selector, node);

        case LXB_CSS_SELECTOR_TYPE_PSEUDO_ELEMENT:
            return lxb_selectors_pseudo_element(selector, node);

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
	lexbor_str_t trg = lxb_selectors_adapted_attr_value_empty(dom_attr);
	if (trg.length == src->length) {
        if (quirks) {
            return lexbor_str_data_ncasecmp(trg.data, src->data, src->length);
        } else {
		    return lexbor_str_data_ncmp(trg.data, src->data, src->length);
        }
	}

	return false;
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
lxb_selectors_match_attribute(const lxb_css_selector_t *selector,
                              const xmlNode *node, lxb_selectors_entry_t *entry)
{
    bool res, ins;
    const xmlAttr *dom_attr;
    lexbor_str_t trg;
	const lexbor_str_t *src;
    const lxb_css_selector_attribute_t *attr;

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
        EMPTY_SWITCH_DEFAULT_CASE();
    }

    return false;
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

static bool
lxb_selectors_pseudo_class_function(const lxb_css_selector_t *selector,
                                    const xmlNode *node)
{
    size_t index;
    const xmlNode *base;
    const lxb_css_selector_pseudo_t *pseudo;

    pseudo = &selector->u.pseudo;

    switch (pseudo->type) {
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_CHILD:
            index = 0;

            if (pseudo->type == LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD) {
                while (node != NULL) {
                    if (lxb_selectors_adapted_is_matchable_child(node))
                    {
                        index++;
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
                    if(xmlStrEqual(node->name, base->name)
                       && lxb_selectors_adapted_cmp_ns(node, base))
                    {
                        index++;
                    }

                    node = node->prev;
                }
            }
            else {
                while (node != NULL) {
                    if(xmlStrEqual(node->name, base->name)
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
            break;
    }

    return false;
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
        if (xmlStrEqual(node->name, root->name)
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
        if (xmlStrEqual(node->name, root->name)
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

static bool
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

static lxb_status_t
lxb_selectors_cb_ok(const xmlNode *node,
                    lxb_css_selector_specificity_t spec, void *ctx)
{
    *((bool *) ctx) = true;
    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_selectors_cb_not(const xmlNode *node,
                     lxb_css_selector_specificity_t spec, void *ctx)
{
    *((bool *) ctx) = false;
    return LXB_STATUS_OK;
}
