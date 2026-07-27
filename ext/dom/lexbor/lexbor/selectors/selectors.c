/*
 * Copyright (C) 2021-2025 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/selectors/selectors.h"

#include <math.h>


static lxb_status_t
lxb_selectors_tree(lxb_selectors_t *selectors, lxb_dom_node_t *root);

static lxb_status_t
lxb_selectors_run(lxb_selectors_t *selectors, lxb_dom_node_t *node);

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
                    lxb_dom_node_t *node);

static bool
lxb_selectors_match_element(const lxb_css_selector_t *selector,
                            lxb_dom_node_t *node, lxb_selectors_entry_t *entry);

static bool
lxb_selectors_match_id(const lxb_css_selector_t *selector, lxb_dom_node_t *node);

static bool
lxb_selectors_match_class(const lexbor_str_t *target, const lexbor_str_t *src,
                          bool quirks);

static bool
lxb_selectors_match_attribute(const lxb_css_selector_t *selector,
                              lxb_dom_node_t *node, lxb_selectors_entry_t *entry);

static bool
lxb_selectors_pseudo_class(const lxb_css_selector_t *selector,
                           const lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_function(lxb_selectors_t *selectors,
                                    const lxb_css_selector_t *selector,
                                    lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_element(const lxb_css_selector_t *selector,
                             const lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_disabled(const lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_first_child(const lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_first_of_type(const lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_last_child(const lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_last_of_type(const lxb_dom_node_t *node);

static bool
lxb_selectors_pseudo_class_read_write(const lxb_dom_node_t *node);

static bool
lxb_selectors_anb_calc(lxb_css_selector_anb_of_t *anb, size_t index);

static lxb_status_t
lxb_selectors_cb_ok(lxb_dom_node_t *node,
                    lxb_css_selector_specificity_t spec, void *ctx);

static lxb_status_t
lxb_selectors_cb_not(lxb_dom_node_t *node,
                     lxb_css_selector_specificity_t spec, void *ctx);

static lxb_status_t
lxb_selectors_cb_nth_ok(lxb_dom_node_t *node,
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

lxb_selectors_t *
lxb_selectors_destroy(lxb_selectors_t *selectors, bool self_destroy)
{
    if (selectors == NULL) {
        return NULL;
    }

    selectors->objs = lexbor_dobject_destroy(selectors->objs, true);
    selectors->nested = lexbor_dobject_destroy(selectors->nested, true);

    if (self_destroy) {
        return lexbor_free(selectors);
    }

    return selectors;
}

static lxb_selectors_entry_t *
lxb_selectors_state_entry_create(lxb_selectors_t *selectors,
                                 const lxb_css_selector_t *selector,
                                 lxb_selectors_entry_t *root,
                                 lxb_dom_node_t *node)
{
    lxb_selectors_entry_t *entry;
    lxb_css_selector_combinator_t combinator;

    combinator = selector->combinator;

    do {
        selector = selector->prev;

        entry = lexbor_dobject_calloc(selectors->objs);
        if (entry == NULL) {
            selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            return NULL;
        }

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
                                         lxb_dom_node_t *node)
{
    lxb_selectors_entry_t *entry;

    selector = selector->next;

    entry = lexbor_dobject_calloc(selectors->objs);
    if (entry == NULL) {
        selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }

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
        if (entry == NULL) {
            return NULL;
        }

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

lxb_inline lxb_dom_node_t *
lxb_selectors_descendant(lxb_selectors_t *selectors,
                         lxb_selectors_entry_t *entry,
                         lxb_dom_node_t *node)
{
    node = node->parent;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT
            && lxb_selectors_match(selectors, entry, node))
        {
            return node;
        }

        node = node->parent;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_descendant_forward(lxb_selectors_t *selectors,
                                 lxb_selectors_entry_t *entry,
                                 lxb_dom_node_t *node)
{
    lxb_dom_node_t *root;
    lxb_selectors_nested_t *current = selectors->current;

    if (entry->prev != NULL) {
        root = entry->prev->node;
    }
    else {
        root = current->root;
    }

    do {
        if (node->first_child != NULL) {
            node = node->first_child;
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

        if (node->type != LXB_DOM_NODE_TYPE_ELEMENT) {
            goto next;
        }

        if (lxb_selectors_match(selectors, entry, node)) {
            return node;
        }
    }
    while (node != NULL);

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_close(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                    lxb_dom_node_t *node)
{
    if (lxb_selectors_match(selectors, entry, node)) {
        return node;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_close_forward(lxb_selectors_t *selectors,
                            lxb_selectors_entry_t *entry, lxb_dom_node_t *node)
{
    if (lxb_selectors_match(selectors, entry, node)) {
        return node;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_child(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                    lxb_dom_node_t *root)
{
    root = root->parent;

    if (root != NULL && root->type == LXB_DOM_NODE_TYPE_ELEMENT
        && lxb_selectors_match(selectors, entry, root))
    {
        return root;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_child_forward(lxb_selectors_t *selectors,
                            lxb_selectors_entry_t *entry, lxb_dom_node_t *root)
{
    if (entry->prev != NULL) {
        if (entry->prev->node == root) {
            root = root->first_child;
        }
        else {
            root = root->next;
        }
    }
    else if (selectors->current->root == root) {
        root = root->first_child;
    }
    else {
        root = root->next;
    }

    while (root != NULL) {
        if (root->type == LXB_DOM_NODE_TYPE_ELEMENT
            && lxb_selectors_match(selectors, entry, root))
        {
            return root;
        }

        root = root->next;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_sibling(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                      lxb_dom_node_t *node)
{
    node = node->prev;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            if (lxb_selectors_match(selectors, entry, node)) {
                return node;
            }

            return NULL;
        }

        node = node->prev;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_sibling_forward(lxb_selectors_t *selectors,
                              lxb_selectors_entry_t *entry, lxb_dom_node_t *node)
{
    node = node->next;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            if (lxb_selectors_match(selectors, entry, node)) {
                return node;
            }

            return NULL;
        }

        node = node->next;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_following(lxb_selectors_t *selectors, lxb_selectors_entry_t *entry,
                        lxb_dom_node_t *node)
{
    node = node->prev;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT &&
            lxb_selectors_match(selectors, entry, node))
        {
            return node;
        }

        node = node->prev;
    }

    return NULL;
}

lxb_inline lxb_dom_node_t *
lxb_selectors_following_forward(lxb_selectors_t *selectors,
                                lxb_selectors_entry_t *entry,
                                lxb_dom_node_t *node)
{
    node = node->next;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT &&
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

static lxb_selectors_entry_t *
lxb_selectors_state_failed(lxb_selectors_t *selectors,
                           lxb_selectors_entry_t *entry)
{
    return NULL;
}

lxb_status_t
lxb_selectors_find(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                   const lxb_css_selector_list_t *list,
                   lxb_selectors_cb_f cb, void *ctx)
{
    lxb_selectors_entry_t *entry;
    lxb_selectors_nested_t nested;

    entry = lxb_selectors_entry_make_first(selectors, list->last);
    if (entry == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

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
lxb_selectors_match_node(lxb_selectors_t *selectors, lxb_dom_node_t *node,
                         const lxb_css_selector_list_t *list,
                         lxb_selectors_cb_f cb, void *ctx)
{
    lxb_status_t status;
    lxb_selectors_entry_t *entry;
    lxb_selectors_nested_t nested;

    if (node->type != LXB_DOM_NODE_TYPE_ELEMENT) {
        return LXB_STATUS_OK;
    }

    entry = lxb_selectors_entry_make_first(selectors, list->last);
    if (entry == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

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
lxb_selectors_find_reverse(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                           const lxb_css_selector_list_t *list,
                           lxb_selectors_cb_f cb, void *ctx)
{
    return lxb_selectors_find(selectors, root, list, cb, ctx);
}

static lxb_status_t
lxb_selectors_tree(lxb_selectors_t *selectors, lxb_dom_node_t *root)
{
    lxb_status_t status;
    lxb_dom_node_t *node;

    if (selectors->options & LXB_SELECTORS_OPT_MATCH_ROOT) {
        node = root;

        if (node->type == LXB_DOM_NODE_TYPE_DOCUMENT) {
            node = root->first_child;
        }
    }
    else {
        node = root->first_child;
    }

    if (node == NULL) {
        goto out;
    }

    do {
        if (node->type != LXB_DOM_NODE_TYPE_ELEMENT) {
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

        if (node->first_child != NULL) {
            node = node->first_child;
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
lxb_selectors_run(lxb_selectors_t *selectors, lxb_dom_node_t *node)
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
    lxb_dom_node_t *node;

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
    lxb_dom_node_t *node;

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
    lxb_dom_node_t *node;
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
    lxb_dom_node_t *node;
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
    if (next == NULL) {
        selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }

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
    if (next == NULL) {
        selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }

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

lxb_inline lxb_dom_node_t *
lxb_selectors_state_nth_child_node(const lxb_css_selector_pseudo_t *pseudo,
                                   lxb_dom_node_t *node)
{
    if (pseudo->type == LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD) {
        node = node->prev;

        while (node != NULL) {
            if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
                break;
            }

            node = node->prev;
        }
    }
    else {
        node = node->next;

        while (node != NULL) {
            if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
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
    lxb_dom_node_t *node;
    lxb_selectors_nested_t *current;
    const lxb_css_selector_pseudo_t *pseudo;

    current = selectors->current;

    if (current->index == 0) {
        selectors->current = selectors->current->parent;
        lxb_selectors_switch_to_not_found(selectors, selectors->current);

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
    lxb_dom_node_t *node;
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
                    lxb_dom_node_t *node)
{
    lxb_dom_element_t *element;

    switch (entry->selector->type) {
        case LXB_CSS_SELECTOR_TYPE_ANY:
            return true;

        case LXB_CSS_SELECTOR_TYPE_ELEMENT:
            return lxb_selectors_match_element(entry->selector, node, entry);

        case LXB_CSS_SELECTOR_TYPE_ID:
            return lxb_selectors_match_id(entry->selector, node);

        case LXB_CSS_SELECTOR_TYPE_CLASS:
            element = lxb_dom_interface_element(node);

            if (element->attr_class == NULL 
                || element->attr_class->value == NULL)
            {
                return false;
            }

            return lxb_selectors_match_class(element->attr_class->value,
                                             &entry->selector->name, true);

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

        default:
            break;
    }

    return false;
}

static bool
lxb_selectors_match_element(const lxb_css_selector_t *selector,
                            lxb_dom_node_t *node, lxb_selectors_entry_t *entry)
{
    lxb_tag_id_t tag_id;

    if (entry->id == 0) {
        tag_id = lxb_tag_id_by_name(node->owner_document->tags,
                                    selector->name.data, selector->name.length);
        if (tag_id == LXB_TAG__UNDEF) {
            return false;
        }

        entry->id = tag_id;
    }

    return node->local_name == entry->id;
}

static bool
lxb_selectors_match_id(const lxb_css_selector_t *selector, lxb_dom_node_t *node)
{
    const lexbor_str_t *trg, *src;
    lxb_dom_element_t *element;

    element = lxb_dom_interface_element(node);

    if (element->attr_id == NULL || element->attr_id->value == NULL) {
        return false;
    }

    trg = element->attr_id->value;
    src = &selector->name;

    return trg->length == src->length
           && lexbor_str_data_ncasecmp(trg->data, src->data, src->length);
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
                              lxb_dom_node_t *node, lxb_selectors_entry_t *entry)
{
    bool res, ins;
    lxb_dom_attr_t *dom_attr;
    lxb_dom_element_t *element;
    const lexbor_str_t *trg, *src;
    const lxb_dom_attr_data_t *attr_data;
    const lxb_css_selector_attribute_t *attr;

    static const lexbor_str_t lxb_blank_str = {
        .data = (lxb_char_t *) "",
        .length = 0
    };

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
            break;
    }

    return false;
}

static bool
lxb_selectors_pseudo_class(const lxb_css_selector_t *selector,
                           const lxb_dom_node_t *node)
{
    lexbor_str_t *str;
    lxb_dom_attr_t *attr;
    const lxb_dom_node_t *root;
    const lxb_css_selector_pseudo_t *pseudo = &selector->u.pseudo;

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

static lxb_selectors_nested_t *
lxb_selectors_nested_make(lxb_selectors_t *selectors, lxb_dom_node_t *node,
                          lxb_css_selector_t *selector, bool forward)
{
    lxb_selectors_entry_t *next;
    lxb_selectors_entry_t *entry;

    entry = selectors->current->entry;
    entry->node = node;

    if (entry->nested == NULL) {
        if (!forward) {
            next = lxb_selectors_entry_make_first(selectors, selector);
            if (next == NULL) {
                selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                return NULL;
            }
        }
        else {
            next = lexbor_dobject_calloc(selectors->objs);
            if (next == NULL) {
                selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                return NULL;
            }

            next->combinator = selector->combinator;
            next->selector = selector;
        }

        entry->nested = lexbor_dobject_calloc(selectors->nested);
        if (entry->nested == NULL) {
            selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            return NULL;
        }

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
                                    lxb_dom_node_t *node)
{
    bool is;
    size_t index;
    lxb_dom_node_t *base;
    lxb_selectors_nested_t *current;
    const lexbor_str_t *str;
    const lxb_dom_text_t *text;
    const lxb_css_selector_list_t *list;
    const lxb_css_selector_anb_of_t *anb;
    const lxb_css_selector_pseudo_t *pseudo;
    const lxb_css_selector_contains_t *contains;

    pseudo = &selector->u.pseudo;

    switch (pseudo->type) {
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_HAS:
            list = (lxb_css_selector_list_t *) pseudo->data;

            current = lxb_selectors_nested_make(selectors, node,
                                                list->first, true);
            if (current == NULL) {
                goto failed;
            }

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
            if (current == NULL) {
                goto failed;
            }

            current->cb = lxb_selectors_cb_ok;
            current->return_state = lxb_selectors_state_after_find;
            selectors->state = lxb_selectors_state_find;

            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NOT:
            list = (lxb_css_selector_list_t *) pseudo->data;

            current = lxb_selectors_nested_make(selectors, node, list->last,
                                                false);
            if (current == NULL) {
                goto failed;
            }

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
                if (current == NULL) {
                    goto failed;
                }

                current->return_state = lxb_selectors_state_after_nth_child;
                current->cb = lxb_selectors_cb_nth_ok;
                current->index = 0;
                selectors->state = lxb_selectors_state_find;

                return true;
            }

            index = 0;

            if (pseudo->type == LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD) {
                while (node != NULL) {
                    if (node->local_name != LXB_TAG__TEXT
                        && node->local_name != LXB_TAG__EM_COMMENT)
                    {
                        index++;
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
                    if(node->local_name == base->local_name
                       && node->ns == base->ns)
                    {
                        index++;
                    }

                    node = node->prev;
                }
            }
            else {
                while (node != NULL) {
                    if(node->local_name == base->local_name
                       && node->ns == base->ns)
                    {
                        index++;
                    }

                    node = node->next;
                }
            }

            return lxb_selectors_anb_calc(pseudo->data, index);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_LEXBOR_CONTAINS:
            contains = pseudo->data;

            node = node->first_child;
            while (node != NULL) {
                if (node->type == LXB_DOM_NODE_TYPE_TEXT) {
                    text = lxb_dom_interface_text(node);
                    str = &text->char_data.data;

                    if (contains->insensitive) {
                        is = lexbor_str_data_ncasecmp_contain(str->data, str->length,
                                                              contains->str.data,
                                                              contains->str.length);
                    }
                    else {
                        is = lexbor_str_data_ncmp_contain(str->data, str->length,
                                                          contains->str.data,
                                                          contains->str.length);
                    }

                    if (is) {
                        return true;
                    }
                }

                node = node->next;
            }

            return false;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_DIR:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_LANG:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_COL:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_COL:
        default:
            return false;
    }

    return true;

failed:

    selectors->state = lxb_selectors_state_failed;
    selectors->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

    return true;
}

static bool
lxb_selectors_pseudo_element(const lxb_css_selector_t *selector,
                             const lxb_dom_node_t *node)
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

static bool
lxb_selectors_pseudo_class_disabled(const lxb_dom_node_t *node)
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
lxb_selectors_pseudo_class_first_child(const lxb_dom_node_t *node)
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
lxb_selectors_pseudo_class_first_of_type(const lxb_dom_node_t *node)
{
    const lxb_dom_node_t *root = node;
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
lxb_selectors_pseudo_class_last_child(const lxb_dom_node_t *node)
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
lxb_selectors_pseudo_class_last_of_type(const lxb_dom_node_t *node)
{
    const lxb_dom_node_t *root = node;
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
lxb_selectors_pseudo_class_read_write(const lxb_dom_node_t *node)
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
lxb_selectors_cb_ok(lxb_dom_node_t *node,
                    lxb_css_selector_specificity_t spec, void *ctx)
{
    lxb_selectors_t *selectors = ctx;

    lxb_selectors_switch_to_found_check(selectors, selectors->current->parent);

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_selectors_cb_not(lxb_dom_node_t *node,
                     lxb_css_selector_specificity_t spec, void *ctx)
{
    lxb_selectors_t *selectors = ctx;

    lxb_selectors_switch_to_not_found(selectors, selectors->current->parent);

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_selectors_cb_nth_ok(lxb_dom_node_t *node,
                        lxb_css_selector_specificity_t spec, void *ctx)
{
    lxb_selectors_t *selectors = ctx;

    selectors->current->index += 1;
    selectors->state = lxb_selectors_state_nth_child_found;

    return LXB_STATUS_OK;
}

void
lxb_selectors_opt_set_noi(lxb_selectors_t *selectors, lxb_selectors_opt_t opt)
{
    lxb_selectors_opt_set(selectors, opt);
}

const lxb_css_selector_list_t *
lxb_selectors_selector_noi(const lxb_selectors_t *selectors)
{
    return lxb_selectors_selector(selectors);
}
