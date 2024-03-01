/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/selectors/selectors.h"

#include <math.h>


static lxb_selectors_entry_t *
lxb_selectors_find_by_selector(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                               lxb_selectors_entry_t *entry,
                               lxb_css_selector_t *selector,
                               lxb_selectors_cb_f cb, void *ctx);

static lxb_selectors_entry_child_t *
lxb_selectors_next(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                   lxb_selectors_entry_child_t *child,
                   lxb_css_selector_list_t *list,
                   lxb_selectors_cb_f cb, void *ctx);

static lxb_selectors_entry_child_t *
lxb_selectors_current(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                      lxb_selectors_entry_child_t *child,
                      lxb_css_selector_list_t *list,
                      lxb_selectors_cb_f cb, void *ctx);

static lxb_selectors_entry_t *
lxb_selectors_next_by_selector(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                               lxb_selectors_entry_t *entry,
                               lxb_css_selector_t *selector,
                               lxb_selectors_cb_f cb, void *ctx);

static lxb_status_t
lxb_selectors_find_by(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                      lxb_dom_node_t *root, lxb_dom_node_t *node,
                      lxb_css_selector_t *selector, lxb_selectors_cb_f cb, void *ctx);

static lxb_status_t
lxb_selectors_find_by_reverse(lxb_selectors_t *selectors, lxb_dom_node_t *node,
                              lxb_css_selector_t *selector,
                              lxb_selectors_cb_f cb, void *ctx);

static bool
lxb_selectors_match(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                    lxb_css_selector_t *selector, lxb_dom_node_t *node);

static bool
lxb_selectors_match_class(const lexbor_str_t *target, const lexbor_str_t *src,
                          bool quirks);

static bool
lxb_selectors_pseudo_class(lxb_css_selector_t *selector, lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_function(lxb_selectors_t *selectors,
                                    lxb_css_selector_t *selector,
                                    lxb_dom_node_t *node,
                                    lxb_selectors_entry_t *entry);

static bool
lxb_selectors_pseudo_element(lxb_selectors_t *selectors,
                             lxb_css_selector_t *selector,
                             lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_element_function(lxb_selectors_t *selectors,
                                      lxb_css_selector_t *selector,
                                      lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_disabled(lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_first_child(lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_first_of_type(lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_last_child(lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_last_of_type(lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_read_write(lxb_dom_node_t *node);

static lxb_status_t
lxb_selectors_first_match(lxb_dom_node_t *node,
                          lxb_css_selector_specificity_t spec, void *ctx);


lxb_selectors_t *
lxb_selectors_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_selectors_t));
}

lxb_status_t
lxb_selectors_init(lxb_selectors_t *selectors)
{
    lxb_status_t status;

    if (selectors == NULL) {
        return LXB_STATUS_ERROR_INCOMPLETE_OBJECT;
    }

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

lxb_selectors_t *
lxb_selectors_destroy(lxb_selectors_t *selectors, bool self_destroy)
{
    if (selectors == NULL) {
        return NULL;
    }

    selectors->objs = lexbor_dobject_destroy(selectors->objs, true);
    selectors->chld = lexbor_dobject_destroy(selectors->chld, true);

    if (self_destroy) {
        return lexbor_free(selectors);
    }

    return selectors;
}


lxb_inline lxb_dom_node_t *
lxb_selectors_descendant(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                         lxb_css_selector_t *selector, lxb_dom_node_t *root,
                         lxb_dom_node_t *node)
{
    do {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT &&
            lxb_selectors_match(selectors, entry, selector, node))
        {
            return node;
        }

        if (node->first_child != NULL) {
            node = node->first_child;
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

lxb_inline lxb_dom_node_t *
lxb_selectors_descendant_next(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                              lxb_css_selector_t *selector, lxb_dom_node_t *root,
                              lxb_dom_node_t *node)
{
    do {
        if (node->first_child != NULL) {
            node = node->first_child;
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

        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT &&
            lxb_selectors_match(selectors, entry, selector, node))
        {
            return node;
        }
    }
    while (true);
}

lxb_inline lxb_dom_node_t *
lxb_selectors_descendant_reverse(lxb_selectors_t *selectors,
                                 lxb_selectors_entry_t *entry,
                                 lxb_css_selector_t *selector,
                                 lxb_dom_node_t *node)
{
    node = node->parent;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT &&
            lxb_selectors_match(selectors, entry, selector, node))
        {
            return node;
        }

        node = node->parent;
    }

    return false;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_close(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                    lxb_css_selector_t *selector, lxb_dom_node_t *node)
{
    if (lxb_selectors_match(selectors, entry, selector, node)) {
        return node;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_child(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                    lxb_css_selector_t *selector, lxb_dom_node_t *root,
                    lxb_dom_node_t *node)
{
    node = root->first_child;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT &&
            lxb_selectors_match(selectors, entry, selector, node))
        {
            return node;
        }

        node = node->next;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_child_next(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                         lxb_css_selector_t *selector, lxb_dom_node_t *root,
                         lxb_dom_node_t *node)
{
    node = node->next;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT &&
            lxb_selectors_match(selectors, entry, selector, node))
        {
            return node;
        }

        node = node->next;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_sibling(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                      lxb_css_selector_t *selector, lxb_dom_node_t *node)
{
    node = node->next;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            if (lxb_selectors_match(selectors, entry, selector, node)) {
                return node;
            }

            return NULL;
        }

        node = node->next;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_sibling_reverse(lxb_selectors_t *selectors,
                              lxb_selectors_entry_t *entry,
                              lxb_css_selector_t *selector,
                              lxb_dom_node_t *node)
{
    node = node->prev;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            if (lxb_selectors_match(selectors, entry, selector, node)) {
                return node;
            }

            return NULL;
        }

        node = node->prev;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_following(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                        lxb_css_selector_t *selector, lxb_dom_node_t *node)
{
    node = node->next;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT &&
            lxb_selectors_match(selectors, entry, selector, node))
        {
            return node;
        }

        node = node->next;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_following_reverse(lxb_selectors_t *selectors,
                                lxb_selectors_entry_t *entry,
                                lxb_css_selector_t *selector,
                                lxb_dom_node_t *node)
{
    node = node->prev;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT &&
            lxb_selectors_match(selectors, entry, selector, node))
        {
            return node;
        }

        node = node->prev;
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
lxb_selectors_find(lxb_selectors_t *selectors, lxb_dom_node_t *root,
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

lxb_status_t
lxb_selectors_find_reverse(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                           lxb_css_selector_list_t *list,
                           lxb_selectors_cb_f cb, void *ctx)
{
    lxb_status_t status;

    while (list != NULL) {
        status = lxb_selectors_find_by_reverse(selectors, root,
                                               list->last, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        list = list->next;
    }

    lxb_selectors_clean(selectors);

    return LXB_STATUS_OK;
}

static lxb_selectors_entry_t *
lxb_selectors_find_by_selector(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                               lxb_selectors_entry_t *entry,
                               lxb_css_selector_t *selector,
                               lxb_selectors_cb_f cb, void *ctx)
{
    lxb_status_t status;
    lxb_dom_node_t *node = root, *base = root;

    if (entry == NULL) {
        entry = lexbor_dobject_calloc(selectors->objs);
        if (entry == NULL) {
            return NULL;
        }

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
lxb_selectors_next(lxb_selectors_t *selectors, lxb_dom_node_t *root,
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
        if (child == NULL) {
            goto failed;
        }

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
            if (child->next == NULL) {
                goto failed;
            }
        }

        child = child->next;
        list = list->next;
    }
    while (true);

    return chld_root;

failed:

    selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

    return NULL;
}

static lxb_selectors_entry_child_t *
lxb_selectors_current(lxb_selectors_t *selectors, lxb_dom_node_t *root,
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
        if (child == NULL) {
            goto failed;
        }

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
            if (child->next == NULL) {
                goto failed;
            }
        }

        child = child->next;
        list = list->next;
    }
    while (true);

    return chld_root;

failed:

    selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

    return NULL;
}

static lxb_selectors_entry_t *
lxb_selectors_next_by_selector(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                               lxb_selectors_entry_t *entry,
                               lxb_css_selector_t *selector,
                               lxb_selectors_cb_f cb, void *ctx)
{
    lxb_status_t status;
    lxb_dom_node_t *node = root, *base = root;

    if (entry == NULL) {
        entry = lexbor_dobject_calloc(selectors->objs);
        if (entry == NULL) {
            selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            return NULL;
        }

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
                      lxb_dom_node_t *root, lxb_dom_node_t *node,
                      lxb_css_selector_t *selector, lxb_selectors_cb_f cb, void *ctx)
{
    lxb_status_t status;
    lxb_selectors_entry_t *next;
    lxb_dom_node_t *base = root;

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
            if (next == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }

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

static lxb_status_t
lxb_selectors_find_by_reverse(lxb_selectors_t *selectors, lxb_dom_node_t *node,
                              lxb_css_selector_t *selector,
                              lxb_selectors_cb_f cb, void *ctx)
{
    lxb_status_t status;
    lxb_dom_node_t *base = node;
    lxb_selectors_entry_t *entry;
    lxb_css_selector_combinator_t combinator;

    entry = lexbor_dobject_calloc(selectors->objs);
    if (entry == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    entry->selector = selector;
    combinator = LXB_CSS_SELECTOR_COMBINATOR_CLOSE;

    do {
        entry->node = node;
        entry->id = 0;

        selector = entry->selector;

        switch (combinator) {
            case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
                node = lxb_selectors_descendant_reverse(selectors, entry,
                                                        selector, node);
                break;

            case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
                node = lxb_selectors_close(selectors, entry, selector, node);
                break;

            case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
                node = node->parent;

                if (node == NULL || node->type != LXB_DOM_NODE_TYPE_ELEMENT
                    || !lxb_selectors_match(selectors, entry, selector, node))
                {
                    return LXB_STATUS_OK;
                }

                break;

            case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
                node = lxb_selectors_sibling_reverse(selectors, entry,
                                                     selector, node);
                break;

            case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
                node = lxb_selectors_following_reverse(selectors, entry,
                                                       selector, node);
                break;

            case LXB_CSS_SELECTOR_COMBINATOR_CELL:
            default:
                return LXB_STATUS_ERROR;
        }

        if (node == NULL) {
            return LXB_STATUS_OK;
        }

        if (selector->prev == NULL) {
            status = cb(base, selector->list->specificity, ctx);
            if (status != LXB_STATUS_OK) {
                if (status == LXB_STATUS_STOP) {
                    return LXB_STATUS_OK;
                }

                return status;
            }

            return LXB_STATUS_OK;
        }

        combinator = selector->combinator;
        entry->selector = selector->prev;
    }
    while (true);

    return LXB_STATUS_OK;
}

static bool
lxb_selectors_match(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                    lxb_css_selector_t *selector, lxb_dom_node_t *node)
{
    bool res, ins;
    const lexbor_str_t *trg, *src;
    lxb_tag_id_t tag_id;
    lxb_dom_attr_t *dom_attr;
    lxb_dom_element_t *element;
    const lxb_dom_attr_data_t *attr_data;
    lxb_css_selector_attribute_t *attr;

    static const lexbor_str_t lxb_blank_str = {
        .data = (lxb_char_t *) "",
        .length = 0
    };

    switch (selector->type) {
        case LXB_CSS_SELECTOR_TYPE_ANY:
            return true;

        case LXB_CSS_SELECTOR_TYPE_ELEMENT:
            if (entry->id == 0) {
                tag_id = lxb_tag_id_by_name(node->owner_document->tags,
                                            selector->name.data, selector->name.length);
                if (tag_id == LXB_TAG__UNDEF) {
                    return false;
                }

                entry->id = tag_id;
            }

            if (node->local_name == entry->id) {
                return true;
            }

            break;

        case LXB_CSS_SELECTOR_TYPE_ID:
            element = lxb_dom_interface_element(node);

            if (element->attr_id == NULL || element->attr_id->value == NULL) {
                return false;
            }

            trg = element->attr_id->value;
            src = &selector->name;

            if (trg->length == src->length
                && lexbor_str_data_ncasecmp(trg->data, src->data, src->length))
            {
                return true;
            }

            return false;

        case LXB_CSS_SELECTOR_TYPE_CLASS:
            element = lxb_dom_interface_element(node);

            if (element->attr_class == NULL || element->attr_class->value == NULL) {
                return false;
            }

            return lxb_selectors_match_class(element->attr_class->value,
                                             &selector->name, true);

        case LXB_CSS_SELECTOR_TYPE_ATTRIBUTE:
            element = lxb_dom_interface_element(node);
            attr = &selector->u.attribute;

            if (entry->id == 0) {
                attr_data = lxb_dom_attr_data_by_local_name(node->owner_document->attrs,
                                                            selector->name.data, selector->name.length);
                if (attr_data == NULL) {
                    return false;
                }

                entry->id = attr_data->attr_id;
            }

            dom_attr = lxb_dom_element_attr_by_id(element, entry->id);
            if (dom_attr == NULL) {
                return false;
            }

            trg = dom_attr->value;
            src = &attr->value;

            if (src->data == NULL) {
                return true;
            }

            if (trg == NULL) {
                trg = &lxb_blank_str;
            }

            ins = attr->modifier == LXB_CSS_SELECTOR_MODIFIER_I;

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

                default:
                    return false;
            }

        case LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS:
            return lxb_selectors_pseudo_class(selector, node);

        case LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION:
            return lxb_selectors_pseudo_class_function(selectors, selector,
                                                       node, entry);

        case LXB_CSS_SELECTOR_TYPE_PSEUDO_ELEMENT:
            return lxb_selectors_pseudo_element(selectors, selector, node);

        case LXB_CSS_SELECTOR_TYPE_PSEUDO_ELEMENT_FUNCTION:
            return lxb_selectors_pseudo_element_function(selectors, selector, node);

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
lxb_selectors_pseudo_class(lxb_css_selector_t *selector, lxb_dom_node_t *node)
{
    lexbor_str_t *str;
    lxb_dom_attr_t *attr;
    lxb_dom_node_t *root;
    lxb_css_selector_pseudo_t *pseudo = &selector->u.pseudo;

    static const lxb_char_t checkbox[] = "checkbox";
    static const size_t checkbox_length = sizeof(checkbox) / sizeof(lxb_char_t) - 1;

    static const lxb_char_t radio[] = "radio";
    static const size_t radio_length = sizeof(radio) / sizeof(lxb_char_t) - 1;

    switch (pseudo->type) {
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_ACTIVE:
            attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                              LXB_DOM_ATTR_ACTIVE);
            return attr != NULL;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_ANY_LINK:
            if(node->local_name == LXB_TAG_A ||
               node->local_name == LXB_TAG_AREA ||
               node->local_name == LXB_TAG_MAP)
            {
                attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                                  LXB_DOM_ATTR_HREF);
                return attr != NULL;
            }

            return false;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_BLANK:
            return lxb_dom_node_is_empty(node);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_CHECKED:
            if (node->local_name == LXB_TAG_INPUT) {
                attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                                  LXB_DOM_ATTR_TYPE);
                if (attr == NULL) {
                    return false;
                }

                if (attr->value == NULL) {
                    return false;
                }

                str = attr->value;

                if(str->length == 8) {
                    if (lexbor_str_data_ncasecmp(checkbox, str->data, checkbox_length)) {
                        goto check;
                    }
                }
                else if(str->length == 5) {
                    if (lexbor_str_data_ncasecmp(radio, str->data, radio_length)) {
                        goto check;
                    }
                }
            }
            else if(node->local_name == LXB_TAG_OPTION) {
                attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                                  LXB_DOM_ATTR_SELECTED);
                if (attr != NULL) {
                    return true;
                }
            }
            else if(node->local_name >= LXB_TAG__LAST_ENTRY) {
                goto check;
            }

            return false;

        check:

            attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                              LXB_DOM_ATTR_CHECKED);
            if (attr != NULL) {
                return true;
            }

            return false;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_CURRENT:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_DEFAULT:
            return false;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_DISABLED:
            return lxb_selectors_pseudo_class_disabled(node);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_EMPTY:
            root = node;
            node = node->first_child;

            while (node != NULL) {
                if (node->local_name != LXB_TAG__EM_COMMENT) {
                    return false;
                }

                if (node->first_child != NULL) {
                    node = node->first_child;
                }
                else {
                    while (node != root && node->next == NULL) {
                        node = node->parent;
                    }

                    if (node == root) {
                        break;
                    }

                    node = node->next;
                }
            }

            return true;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_ENABLED:
            return !lxb_selectors_pseudo_class_disabled(node);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FIRST_CHILD:
            return lxb_selectors_pseudo_class_first_child(node);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FIRST_OF_TYPE:
            return lxb_selectors_pseudo_class_first_of_type(node);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS:
            attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                              LXB_DOM_ATTR_FOCUS);
            return attr != NULL;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS_VISIBLE:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS_WITHIN:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FULLSCREEN:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUTURE:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_HOVER:
            attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                              LXB_DOM_ATTR_HOVER);
            return attr != NULL;

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
            if (node->local_name == LXB_TAG_A
               || node->local_name == LXB_TAG_AREA
               || node->local_name == LXB_TAG_LINK)
            {
                attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                                  LXB_DOM_ATTR_HREF);
                return attr != NULL;
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
            if (node->local_name == LXB_TAG_INPUT
               || node->local_name == LXB_TAG_SELECT
               || node->local_name == LXB_TAG_TEXTAREA)
            {
                attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                                  LXB_DOM_ATTR_REQUIRED);
                return attr == NULL;
            }

            return false;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_OUT_OF_RANGE:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_PAST:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_PLACEHOLDER_SHOWN:
            if (node->local_name == LXB_TAG_INPUT
                || node->local_name == LXB_TAG_TEXTAREA)
            {
                attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                                  LXB_DOM_ATTR_PLACEHOLDER);
                return attr != NULL;
            }

            return false;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_READ_ONLY:
            return !lxb_selectors_pseudo_class_read_write(node);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_READ_WRITE:
            return lxb_selectors_pseudo_class_read_write(node);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_REQUIRED:
            if (node->local_name == LXB_TAG_INPUT
                || node->local_name == LXB_TAG_SELECT
                || node->local_name == LXB_TAG_TEXTAREA)
            {
                attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                                  LXB_DOM_ATTR_REQUIRED);
                return attr != NULL;
            }

            return false;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_ROOT:
            return lxb_dom_document_root(node->owner_document) == node;

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
lxb_selectors_pseudo_class_function(lxb_selectors_t *selectors,
                                    lxb_css_selector_t *selector,
                                    lxb_dom_node_t *node,
                                    lxb_selectors_entry_t *entry)
{
    size_t index;
    bool found = false;
    lxb_dom_node_t *base;
    lxb_css_selector_anb_of_t *anb;
    lxb_css_selector_pseudo_t *pseudo = &selector->u.pseudo;

    switch (pseudo->type) {
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_CURRENT:
            entry->child = lxb_selectors_next(selectors, node, entry->child,
                                              pseudo->data,
                                              lxb_selectors_first_match, &found);
            if (entry->child == NULL) {
                return false;
            }

            return found;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_DIR:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_HAS:
            entry->child = lxb_selectors_next(selectors, node, entry->child,
                                              pseudo->data,
                                              lxb_selectors_first_match, &found);
            if (entry->child == NULL) {
                return false;
            }

            return found;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_IS:
            entry->child = lxb_selectors_next(selectors, node, entry->child,
                                              pseudo->data,
                                              lxb_selectors_first_match, &found);
            if (entry->child == NULL) {
                return false;
            }

            return found;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_LANG:
            break;

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
                    if (node->local_name != LXB_TAG__TEXT
                       && node->local_name != LXB_TAG__EM_COMMENT)
                    {
                        entry->child = lxb_selectors_current(selectors, node, entry->child,
                                                             anb->of, lxb_selectors_first_match,
                                                             &found);
                        if (entry->child == NULL) {
                            return false;
                        }

                        if (found) {
                            index++;
                        }

                        found = false;
                    }

                    node = node->prev;
                }
            }
            else {
                while (node != NULL) {
                    if (node->local_name != LXB_TAG__TEXT
                       && node->local_name != LXB_TAG__EM_COMMENT)
                    {
                        index++;
                    }

                    node = node->prev;
                }
            }

            return lxb_selectors_anb_calc(anb, index);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_COL:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_CHILD:
            index = 0;
            found = false;
            anb = selector->u.pseudo.data;

            if (anb->of != NULL) {
                while (node != NULL) {
                    if (node->local_name != LXB_TAG__TEXT
                       && node->local_name != LXB_TAG__EM_COMMENT)
                    {
                        entry->child = lxb_selectors_current(selectors, node, entry->child,
                                                             anb->of, lxb_selectors_first_match,
                                                             &found);
                        if (entry->child == NULL) {
                            return false;
                        }

                        if (found) {
                            index++;
                        }

                        found = false;
                    }

                    node = node->next;
                }
            }
            else {
                while (node != NULL) {
                    if (node->local_name != LXB_TAG__TEXT
                       && node->local_name != LXB_TAG__EM_COMMENT)
                    {
                        index++;
                    }

                    node = node->next;
                }
            }

            return lxb_selectors_anb_calc(anb, index);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_COL:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_OF_TYPE:
            index = 0;
            found = false;
            anb = selector->u.pseudo.data;
            base = node;

            while (node != NULL) {
                if(node->local_name == base->local_name
                   && node->ns == base->ns)
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
                if(node->local_name == base->local_name
                   && node->ns == base->ns)
                {
                    index++;
                }

                node = node->prev;
            }

            return lxb_selectors_anb_calc(anb, index);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_WHERE:
            entry->child = lxb_selectors_next(selectors, node, entry->child,
                                              pseudo->data,
                                              lxb_selectors_first_match, &found);
            if (entry->child == NULL) {
                return false;
            }

            return found;
    }

    return false;
}

static bool
lxb_selectors_pseudo_element(lxb_selectors_t *selectors,
                             lxb_css_selector_t *selector, lxb_dom_node_t *node)
{
    lxb_css_selector_pseudo_t *pseudo = &selector->u.pseudo;

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

static bool
lxb_selectors_pseudo_element_function(lxb_selectors_t *selectors,
                                      lxb_css_selector_t *selector, lxb_dom_node_t *node)
{
    return false;
}

static bool
lxb_selectors_pseudo_class_disabled(lxb_dom_node_t *node)
{
    lxb_dom_attr_t *attr;
    uintptr_t tag_id = node->local_name;

    attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                      LXB_DOM_ATTR_DISABLED);
    if (attr == NULL) {
        return false;
    }

    if (tag_id == LXB_TAG_BUTTON || tag_id == LXB_TAG_INPUT ||
        tag_id == LXB_TAG_SELECT || tag_id == LXB_TAG_TEXTAREA ||
        tag_id >= LXB_TAG__LAST_ENTRY)
    {
        return true;
    }

    node = node->parent;

    while (node != NULL) {
        if (node->local_name == LXB_TAG_FIELDSET
            && node->first_child->local_name != LXB_TAG_LEGEND)
        {
            return true;
        }

        node = node->parent;
    }

    return false;
}

static bool
lxb_selectors_pseudo_class_first_child(lxb_dom_node_t *node)
{
    node = node->prev;

    while (node != NULL) {
        if (node->local_name != LXB_TAG__TEXT
           && node->local_name != LXB_TAG__EM_COMMENT)
        {
            return false;
        }

        node = node->prev;
    }

    return true;
}

static bool
lxb_selectors_pseudo_class_first_of_type(lxb_dom_node_t *node)
{
    lxb_dom_node_t *root = node;
    node = node->prev;

    while (node) {
        if (node->local_name == root->local_name
           && node->ns == root->ns)
        {
            return false;
        }

        node = node->prev;
    }

    return true;
}

static bool
lxb_selectors_pseudo_class_last_child(lxb_dom_node_t *node)
{
    node = node->next;

    while (node != NULL) {
        if (node->local_name != LXB_TAG__TEXT
           && node->local_name != LXB_TAG__EM_COMMENT)
        {
            return false;
        }

        node = node->next;
    }

    return true;
}

static bool
lxb_selectors_pseudo_class_last_of_type(lxb_dom_node_t *node)
{
    lxb_dom_node_t *root = node;
    node = node->next;

    while (node) {
        if (node->local_name == root->local_name
           && node->ns == root->ns)
        {
            return false;
        }

        node = node->next;
    }

    return true;
}

static bool
lxb_selectors_pseudo_class_read_write(lxb_dom_node_t *node)
{
    lxb_dom_attr_t *attr;

    if (node->local_name == LXB_TAG_INPUT
       || node->local_name == LXB_TAG_TEXTAREA)
    {
        attr = lxb_dom_element_attr_by_id(lxb_dom_interface_element(node),
                                          LXB_DOM_ATTR_READONLY);
        if (attr != NULL) {
            return false;
        }

        return !lxb_selectors_pseudo_class_disabled(node);
    }

    return false;
}

static lxb_status_t
lxb_selectors_first_match(lxb_dom_node_t *node,
                          lxb_css_selector_specificity_t spec, void *ctx)
{
    *((bool *) ctx) = true;
    return LXB_STATUS_STOP;
}
