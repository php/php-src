/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/tree/active_formatting.h"
#include "lexbor/html/tree/template_insertion.h"


/*
 * "caption", "colgroup", "tbody", "tfoot", "thead"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_template_ct(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    lxb_html_tree_template_insertion_pop(tree);

    tree->status = lxb_html_tree_template_insertion_push(tree,
                                                         lxb_html_tree_insertion_mode_in_table);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_table;

    return false;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_template_col(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token)
{
    lxb_html_tree_template_insertion_pop(tree);

    tree->status = lxb_html_tree_template_insertion_push(tree,
                                                         lxb_html_tree_insertion_mode_in_column_group);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_column_group;

    return false;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_template_tr(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    lxb_html_tree_template_insertion_pop(tree);

    tree->status = lxb_html_tree_template_insertion_push(tree,
                                                         lxb_html_tree_insertion_mode_in_table_body);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_table_body;

    return false;
}

/*
 * "td", "th"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_template_tdth(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_html_tree_template_insertion_pop(tree);

    tree->status = lxb_html_tree_template_insertion_push(tree,
                                                         lxb_html_tree_insertion_mode_in_row);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_row;

    return false;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_template_end_of_file(lxb_html_tree_t *tree,
                                                     lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_open_elements_find(tree, LXB_TAG_TEMPLATE, LXB_NS_HTML,
                                            NULL);
    if (node == NULL) {
        tree->status =  lxb_html_tree_stop_parsing(tree);
        if (tree->status != LXB_STATUS_OK) {
            return lxb_html_tree_process_abort(tree);
        }

        return true;
    }

    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNENOFFI);

    lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_TEMPLATE,
                                                 LXB_NS_HTML, true);

    lxb_html_tree_active_formatting_up_to_last_marker(tree);
    lxb_html_tree_template_insertion_pop(tree);
    lxb_html_tree_reset_insertion_mode_appropriately(tree);

    return false;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_template_anything_else(lxb_html_tree_t *tree,
                                                       lxb_html_token_t *token)
{
    lxb_html_tree_template_insertion_pop(tree);

    tree->status = lxb_html_tree_template_insertion_push(tree,
                                                         lxb_html_tree_insertion_mode_in_body);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_body;

    return false;
}

bool
lxb_html_tree_insertion_mode_in_template(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        if (token->tag_id == LXB_TAG_TEMPLATE) {
            return lxb_html_tree_insertion_mode_in_head(tree, token);
        }

        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

        return true;
    }

    switch (token->tag_id) {
        case LXB_TAG__TEXT:
        case LXB_TAG__EM_COMMENT:
        case LXB_TAG__EM_DOCTYPE:
            return lxb_html_tree_insertion_mode_in_body(tree, token);

        case LXB_TAG_BASE:
        case LXB_TAG_BASEFONT:
        case LXB_TAG_BGSOUND:
        case LXB_TAG_LINK:
        case LXB_TAG_META:
        case LXB_TAG_NOFRAMES:
        case LXB_TAG_SCRIPT:
        case LXB_TAG_STYLE:
        case LXB_TAG_TEMPLATE:
        case LXB_TAG_TITLE:
            return lxb_html_tree_insertion_mode_in_head(tree, token);

        case LXB_TAG_CAPTION:
        case LXB_TAG_COLGROUP:
        case LXB_TAG_TBODY:
        case LXB_TAG_TFOOT:
        case LXB_TAG_THEAD:
            return lxb_html_tree_insertion_mode_in_template_ct(tree, token);

        case LXB_TAG_COL:
            return lxb_html_tree_insertion_mode_in_template_col(tree, token);

        case LXB_TAG_TR:
            return lxb_html_tree_insertion_mode_in_template_tr(tree, token);

        case LXB_TAG_TD:
        case LXB_TAG_TH:
            return lxb_html_tree_insertion_mode_in_template_tdth(tree, token);

        case LXB_TAG__END_OF_FILE:
            return lxb_html_tree_insertion_mode_in_template_end_of_file(tree,
                                                                        token);
        default:
            return lxb_html_tree_insertion_mode_in_template_anything_else(tree,
                                                                          token);
    }
}
