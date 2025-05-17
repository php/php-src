/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TEMPLATE_INSERTION_H
#define LEXBOR_HTML_TEMPLATE_INSERTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/array.h"

#include "lexbor/html/tree.h"


typedef struct {
    lxb_html_tree_insertion_mode_f mode;
}
lxb_html_tree_template_insertion_t;


/*
 * Inline functions
 */
lxb_inline lxb_html_tree_insertion_mode_f
lxb_html_tree_template_insertion_current(lxb_html_tree_t *tree)
{
    if (lexbor_array_obj_length(tree->template_insertion_modes) == 0) {
        return NULL;
    }

    lxb_html_tree_template_insertion_t *tmp_ins;

    tmp_ins = (lxb_html_tree_template_insertion_t *)
              lexbor_array_obj_last(tree->template_insertion_modes);

    return tmp_ins->mode;
}

lxb_inline lxb_html_tree_insertion_mode_f
lxb_html_tree_template_insertion_get(lxb_html_tree_t *tree, size_t idx)
{
    lxb_html_tree_template_insertion_t *tmp_ins;

    tmp_ins = (lxb_html_tree_template_insertion_t *)
              lexbor_array_obj_get(tree->template_insertion_modes, idx);
    if (tmp_ins == NULL) {
        return NULL;
    }

    return tmp_ins->mode;
}

lxb_inline lxb_html_tree_insertion_mode_f
lxb_html_tree_template_insertion_first(lxb_html_tree_t *tree)
{
    return lxb_html_tree_template_insertion_get(tree, 0);
}

lxb_inline lxb_status_t
lxb_html_tree_template_insertion_push(lxb_html_tree_t *tree,
                                      lxb_html_tree_insertion_mode_f mode)
{
    lxb_html_tree_template_insertion_t *tmp_ins;

    tmp_ins = (lxb_html_tree_template_insertion_t *)
              lexbor_array_obj_push(tree->template_insertion_modes);
    if (tmp_ins == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    tmp_ins->mode = mode;

    return LXB_STATUS_OK;
}

lxb_inline lxb_html_tree_insertion_mode_f
lxb_html_tree_template_insertion_pop(lxb_html_tree_t *tree)
{
    lxb_html_tree_template_insertion_t *tmp_ins;

    tmp_ins = (lxb_html_tree_template_insertion_t *)
              lexbor_array_obj_pop(tree->template_insertion_modes);
    if (tmp_ins == NULL) {
        return NULL;
    }

    return tmp_ins->mode;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TEMPLATE_INSERTION_H */

