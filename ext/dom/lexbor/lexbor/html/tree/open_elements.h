/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_OPEN_ELEMENTS_H
#define LEXBOR_HTML_OPEN_ELEMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/array.h"

#include "lexbor/html/tree.h"


LXB_API void
lxb_html_tree_open_elements_remove_by_node(lxb_html_tree_t *tree,
                                           lxb_dom_node_t *node);

void
lxb_html_tree_open_elements_pop_until_tag_id(lxb_html_tree_t *tree,
                                             lxb_tag_id_t tag_id,
                                             lxb_ns_id_t ns,
                                             bool exclude);

void
lxb_html_tree_open_elements_pop_until_h123456(lxb_html_tree_t *tree);

void
lxb_html_tree_open_elements_pop_until_td_th(lxb_html_tree_t *tree);

void
lxb_html_tree_open_elements_pop_until_node(lxb_html_tree_t *tree,
                                           lxb_dom_node_t *node,
                                           bool exclude);

void
lxb_html_tree_open_elements_pop_until(lxb_html_tree_t *tree, size_t idx,
                                      bool exclude);

bool
lxb_html_tree_open_elements_find_by_node(lxb_html_tree_t *tree,
                                         lxb_dom_node_t *node,
                                         size_t *return_pos);

bool
lxb_html_tree_open_elements_find_by_node_reverse(lxb_html_tree_t *tree,
                                                 lxb_dom_node_t *node,
                                                 size_t *return_pos);

lxb_dom_node_t *
lxb_html_tree_open_elements_find(lxb_html_tree_t *tree,
                                 lxb_tag_id_t tag_id, lxb_ns_id_t ns,
                                 size_t *return_index);

lxb_dom_node_t *
lxb_html_tree_open_elements_find_reverse(lxb_html_tree_t *tree,
                                         lxb_tag_id_t tag_id, lxb_ns_id_t ns,
                                         size_t *return_index);


/*
 * Inline functions
 */
lxb_inline lxb_dom_node_t *
lxb_html_tree_open_elements_first(lxb_html_tree_t *tree)
{
    return (lxb_dom_node_t *) lexbor_array_get(tree->open_elements, 0);
}

lxb_inline lxb_dom_node_t *
lxb_html_tree_open_elements_get(lxb_html_tree_t *tree, size_t idx)
{
    return (lxb_dom_node_t *) lexbor_array_get(tree->open_elements, idx);
}

lxb_inline lxb_status_t
lxb_html_tree_open_elements_push(lxb_html_tree_t *tree, lxb_dom_node_t *node)
{
    return lexbor_array_push(tree->open_elements, node);
}

lxb_inline lxb_dom_node_t *
lxb_html_tree_open_elements_pop(lxb_html_tree_t *tree)
{
    return (lxb_dom_node_t *) lexbor_array_pop(tree->open_elements);
}

lxb_inline lxb_status_t
lxb_html_tree_open_elements_insert_after(lxb_html_tree_t *tree, lxb_dom_node_t *node,
                                    size_t idx)
{
    return lexbor_array_insert(tree->open_elements, (idx + 1), node);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_OPEN_ELEMENTS_H */

