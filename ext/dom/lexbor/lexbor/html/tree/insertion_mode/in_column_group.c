/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"


lxb_inline bool
lxb_html_tree_insertion_mode_in_column_group_anything_else(lxb_html_tree_t *tree,
                                                           lxb_html_token_t *token)
{
    lxb_dom_node_t *node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, LXB_TAG_COLGROUP) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_MIELINOPELST);

        return true;
    }

    lxb_html_tree_open_elements_pop(tree);

    tree->mode = lxb_html_tree_insertion_mode_in_table;

    return false;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_column_group_text(lxb_html_tree_t *tree,
                                                  lxb_html_token_t *token)
{
    lxb_html_token_t ws_token = {0};

    tree->status = lxb_html_token_data_split_ws_begin(token, &ws_token);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    if (ws_token.text_start != ws_token.text_end) {
        tree->status = lxb_html_tree_insert_character(tree, &ws_token, NULL);
        if (tree->status != LXB_STATUS_OK) {
            return lxb_html_tree_process_abort(tree);
        }
    }

    if (token->text_start == token->text_end) {
        return true;
    }

    return lxb_html_tree_insertion_mode_in_column_group_anything_else(tree,
                                                                      token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_column_group_comment(lxb_html_tree_t *tree,
                                                     lxb_html_token_t *token)
{
    lxb_dom_comment_t *comment;

    comment = lxb_html_tree_insert_comment(tree, token, NULL);
    if (comment == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_column_group_html(lxb_html_tree_t *tree,
                                                  lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_body(tree, token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_column_group_col(lxb_html_tree_t *tree,
                                                 lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    lxb_html_tree_open_elements_pop(tree);
    lxb_html_tree_acknowledge_token_self_closing(tree, token);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_column_group_colgroup_closed(lxb_html_tree_t *tree,
                                                             lxb_html_token_t *token)
{
    lxb_dom_node_t *node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, LXB_TAG_COLGROUP) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_MIELINOPELST);

        return true;
    }

    lxb_html_tree_open_elements_pop(tree);

    tree->mode = lxb_html_tree_insertion_mode_in_table;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_column_group_col_closed(lxb_html_tree_t *tree,
                                                        lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_column_group_template_open_closed(lxb_html_tree_t *tree,
                                                                  lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_head(tree, token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_column_group_end_of_file(lxb_html_tree_t *tree,
                                                         lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_body(tree, token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_column_group_anything_else_closed(lxb_html_tree_t *tree,
                                                                  lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_column_group_anything_else(tree, token);
}

bool
lxb_html_tree_insertion_mode_in_column_group(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        switch (token->tag_id) {
            case LXB_TAG_COLGROUP:
                return lxb_html_tree_insertion_mode_in_column_group_colgroup_closed(tree,
                                                                                    token);
            case LXB_TAG_COL:
                return lxb_html_tree_insertion_mode_in_column_group_col_closed(tree,
                                                                               token);
            case LXB_TAG_TEMPLATE:
                return lxb_html_tree_insertion_mode_in_column_group_template_open_closed(tree,
                                                                                         token);
            default:
                return lxb_html_tree_insertion_mode_in_column_group_anything_else_closed(tree,
                                                                                         token);
        }
    }

    switch (token->tag_id) {
        case LXB_TAG__TEXT:
            return lxb_html_tree_insertion_mode_in_column_group_text(tree,
                                                                     token);
        case LXB_TAG__EM_COMMENT:
            return lxb_html_tree_insertion_mode_in_column_group_comment(tree,
                                                                        token);
        case LXB_TAG_HTML:
            return lxb_html_tree_insertion_mode_in_column_group_html(tree,
                                                                     token);
        case LXB_TAG_COL:
            return lxb_html_tree_insertion_mode_in_column_group_col(tree,
                                                                    token);
        case LXB_TAG_TEMPLATE:
            return lxb_html_tree_insertion_mode_in_column_group_template_open_closed(tree,
                                                                                     token);

        case LXB_TAG__END_OF_FILE:
            return lxb_html_tree_insertion_mode_in_column_group_end_of_file(tree,
                                                                            token);
        default:
            return lxb_html_tree_insertion_mode_in_column_group_anything_else(tree,
                                                                              token);
    }
}
