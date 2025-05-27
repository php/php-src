/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/node.h"

#include "lexbor/html/tree/active_formatting.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/interfaces/element.h"


static lxb_html_element_t lxb_html_tree_active_formatting_marker_static;

static lxb_dom_node_t *lxb_html_tree_active_formatting_marker_node_static =
    (lxb_dom_node_t *) &lxb_html_tree_active_formatting_marker_static;


lxb_html_element_t *
lxb_html_tree_active_formatting_marker(void)
{
    return &lxb_html_tree_active_formatting_marker_static;
}

void
lxb_html_tree_active_formatting_up_to_last_marker(lxb_html_tree_t *tree)
{
    void **list = tree->active_formatting->list;

    while (tree->active_formatting->length != 0) {
        tree->active_formatting->length--;

        if (list[tree->active_formatting->length]
            == &lxb_html_tree_active_formatting_marker_static)
        {
            break;
        }
    }
}

void
lxb_html_tree_active_formatting_remove_by_node(lxb_html_tree_t *tree,
                                               lxb_dom_node_t *node)
{
    size_t delta;
    void **list = tree->active_formatting->list;
    size_t idx = tree->active_formatting->length;

    while (idx != 0) {
        idx--;

        if (list[idx] == node) {
            delta = tree->active_formatting->length - idx - 1;

            memmove(list + idx, list + idx + 1, sizeof(void *) * delta);

            tree->active_formatting->length--;

            break;
        }
    }
}

bool
lxb_html_tree_active_formatting_find_by_node(lxb_html_tree_t *tree,
                                             lxb_dom_node_t *node,
                                             size_t *return_pos)
{
    void **list = tree->active_formatting->list;

    for (size_t i = 0; i < tree->active_formatting->length; i++) {
        if (list[i] == node) {
            if (return_pos) {
                *return_pos = i;
            }

            return true;
        }
    }

    if (return_pos) {
        *return_pos = 0;
    }

    return false;
}

bool
lxb_html_tree_active_formatting_find_by_node_reverse(lxb_html_tree_t *tree,
                                                     lxb_dom_node_t *node,
                                                     size_t *return_pos)
{
    void **list = tree->active_formatting->list;
    size_t len = tree->active_formatting->length;

    while (len != 0) {
        len--;

        if (list[len] == node) {
            if (return_pos) {
                *return_pos = len;
            }

            return true;
        }
    }

    if (return_pos) {
        *return_pos = 0;
    }

    return false;
}

lxb_status_t
lxb_html_tree_active_formatting_reconstruct_elements(lxb_html_tree_t *tree)
{
    /* Step 1 */
    if (tree->active_formatting->length == 0) {
        return LXB_STATUS_OK;
    }

    lexbor_array_t *af = tree->active_formatting;
    void **list = af->list;

    /* Step 2-3 */
    size_t af_idx = af->length - 1;

    if(list[af_idx] == &lxb_html_tree_active_formatting_marker_static
       || lxb_html_tree_open_elements_find_by_node_reverse(tree, list[af_idx],
                                                           NULL))
    {
        return LXB_STATUS_OK;
    }

    /*
     * Step 4-6
     * Rewind
     */
    while (af_idx != 0) {
        af_idx--;

        if(list[af_idx] == &lxb_html_tree_active_formatting_marker_static ||
           lxb_html_tree_open_elements_find_by_node_reverse(tree, list[af_idx],
                                                            NULL))
        {
            /* Step 7 */
            af_idx++;

            break;
        }
    }

    /*
     * Step 8-10
     * Create
     */
    lxb_dom_node_t *node;
    lxb_html_element_t *element;
    lxb_html_token_t fake_token = {0};

    while (af_idx < af->length) {
        node = list[af_idx];

        fake_token.tag_id = node->local_name;
        fake_token.base_element = node;

        element = lxb_html_tree_insert_html_element(tree, &fake_token);
        if (element == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        /* Step 9 */
        list[af_idx] = lxb_dom_interface_node(element);

        /* Step 10 */
        af_idx++;
    }

    return LXB_STATUS_OK;
}

lxb_dom_node_t *
lxb_html_tree_active_formatting_between_last_marker(lxb_html_tree_t *tree,
                                                    lxb_tag_id_t tag_idx,
                                                    size_t *return_idx)
{
    lxb_dom_node_t **list = (lxb_dom_node_t **) tree->active_formatting->list;
    size_t idx = tree->active_formatting->length;

    while (idx) {
        idx--;

        if (list[idx] == lxb_html_tree_active_formatting_marker_node_static) {
            return NULL;
        }

        if (list[idx]->local_name == tag_idx && list[idx]->ns == LXB_NS_HTML) {
            if (return_idx) {
                *return_idx = idx;
            }

            return list[idx];
        }
    }

    return NULL;
}

void
lxb_html_tree_active_formatting_push_with_check_dupl(lxb_html_tree_t *tree,
                                                     lxb_dom_node_t *node)
{
    lxb_dom_node_t **list = (lxb_dom_node_t **) tree->active_formatting->list;
    size_t idx = tree->active_formatting->length;
    size_t earliest_idx = (idx ? (idx - 1) : 0);
    size_t count = 0;

    while (idx) {
        idx--;

        if (list[idx] == lxb_html_tree_active_formatting_marker_node_static) {
            break;
        }

        if(list[idx]->local_name == node->local_name && list[idx]->ns == node->ns
            && lxb_dom_element_compare(lxb_dom_interface_element(list[idx]),
                                       lxb_dom_interface_element(node)))
        {
            count++;
            earliest_idx = idx;
        }
    }

    if(count >= 3) {
        lxb_html_tree_active_formatting_remove(tree, earliest_idx);
    }

    lxb_html_tree_active_formatting_push(tree, node);
}
