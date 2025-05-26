/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_ACTIVE_FORMATTING_H
#define LEXBOR_HTML_ACTIVE_FORMATTING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/array.h"

#include "lexbor/html/tree.h"


lxb_html_element_t *
lxb_html_tree_active_formatting_marker(void);

void
lxb_html_tree_active_formatting_up_to_last_marker(lxb_html_tree_t *tree);

void
lxb_html_tree_active_formatting_remove_by_node(lxb_html_tree_t *tree,
                                               lxb_dom_node_t *node);

bool
lxb_html_tree_active_formatting_find_by_node(lxb_html_tree_t *tree,
                                             lxb_dom_node_t *node,
                                             size_t *return_pos);

bool
lxb_html_tree_active_formatting_find_by_node_reverse(lxb_html_tree_t *tree,
                                                     lxb_dom_node_t *node,
                                                     size_t *return_pos);

lxb_status_t
lxb_html_tree_active_formatting_reconstruct_elements(lxb_html_tree_t *tree);

lxb_dom_node_t *
lxb_html_tree_active_formatting_between_last_marker(lxb_html_tree_t *tree,
                                                    lxb_tag_id_t tag_idx,
                                                    size_t *return_idx);

void
lxb_html_tree_active_formatting_push_with_check_dupl(lxb_html_tree_t *tree,
                                                     lxb_dom_node_t *node);


/*
 * Inline functions
 */
lxb_inline lxb_dom_node_t *
lxb_html_tree_active_formatting_current_node(lxb_html_tree_t *tree)
{
    if (tree->active_formatting->length == 0) {
        return NULL;
    }

    return (lxb_dom_node_t *) tree->active_formatting->list
        [ (tree->active_formatting->length - 1) ];
}

lxb_inline lxb_dom_node_t *
lxb_html_tree_active_formatting_first(lxb_html_tree_t *tree)
{
    return (lxb_dom_node_t *) lexbor_array_get(tree->active_formatting, 0);
}

lxb_inline lxb_dom_node_t *
lxb_html_tree_active_formatting_get(lxb_html_tree_t *tree, size_t idx)
{
    return (lxb_dom_node_t *) lexbor_array_get(tree->active_formatting, idx);
}

lxb_inline lxb_status_t
lxb_html_tree_active_formatting_push(lxb_html_tree_t *tree,
                                     lxb_dom_node_t *node)
{
    return lexbor_array_push(tree->active_formatting, node);
}

lxb_inline lxb_dom_node_t *
lxb_html_tree_active_formatting_pop(lxb_html_tree_t *tree)
{
    return (lxb_dom_node_t *) lexbor_array_pop(tree->active_formatting);
}

lxb_inline lxb_status_t
lxb_html_tree_active_formatting_push_marker(lxb_html_tree_t *tree)
{
    return lexbor_array_push(tree->active_formatting,
                             lxb_html_tree_active_formatting_marker());
}

lxb_inline lxb_status_t
lxb_html_tree_active_formatting_insert(lxb_html_tree_t *tree,
                                       lxb_dom_node_t *node, size_t idx)
{
    return lexbor_array_insert(tree->active_formatting, idx, node);
}

lxb_inline void
lxb_html_tree_active_formatting_remove(lxb_html_tree_t *tree, size_t idx)
{
    lexbor_array_delete(tree->active_formatting, idx, 1);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_ACTIVE_FORMATTING_H */

