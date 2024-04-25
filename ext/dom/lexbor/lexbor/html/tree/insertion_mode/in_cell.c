/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/tree/active_formatting.h"


static void
lxb_html_tree_close_cell(lxb_html_tree_t *tree, lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                            LXB_NS__UNDEF);

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, LXB_TAG_TD) == false
        && lxb_html_tree_node_is(node, LXB_TAG_TH) == false)
    {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_MIELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_td_th(tree);
    lxb_html_tree_active_formatting_up_to_last_marker(tree);

    tree->mode = lxb_html_tree_insertion_mode_in_row;
}

/*
 * "td", "th"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_cell_tdth_closed(lxb_html_tree_t *tree,
                                                 lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, token->tag_id, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_TABLE);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

        return true;
    }

    lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                            LXB_NS__UNDEF);

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, token->tag_id) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_MIELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_tag_id(tree, token->tag_id,
                                                 LXB_NS_HTML, true);

    lxb_html_tree_active_formatting_up_to_last_marker(tree);

    tree->mode = lxb_html_tree_insertion_mode_in_row;

    return true;
}

/*
 * "caption", "col", "colgroup", "tbody", "td", "tfoot", "th", "thead", "tr"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_cell_ct(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope_td_th(tree);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_MIELINSC);

        return true;
    }

    lxb_html_tree_close_cell(tree, token);

    return false;
}

/*
 * "body", "caption", "col", "colgroup", "html"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_cell_bch_closed(lxb_html_tree_t *tree,
                                                lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

    return true;
}

/*
 * "table", "tbody", "tfoot", "thead", "tr"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_cell_t_closed(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, token->tag_id, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_TABLE);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

        return true;
    }

    lxb_html_tree_close_cell(tree, token);

    return false;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_cell_anything_else(lxb_html_tree_t *tree,
                                                   lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_body(tree, token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_cell_anything_else_closed(lxb_html_tree_t *tree,
                                                          lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_cell_anything_else(tree, token);
}

bool
lxb_html_tree_insertion_mode_in_cell(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        switch (token->tag_id) {
            case LXB_TAG_TD:
            case LXB_TAG_TH:
                return lxb_html_tree_insertion_mode_in_cell_tdth_closed(tree,
                                                                        token);
            case LXB_TAG_BODY:
            case LXB_TAG_CAPTION:
            case LXB_TAG_COL:
            case LXB_TAG_COLGROUP:
            case LXB_TAG_HTML:
                return lxb_html_tree_insertion_mode_in_cell_bch_closed(tree,
                                                                       token);
            case LXB_TAG_TABLE:
            case LXB_TAG_TBODY:
            case LXB_TAG_TFOOT:
            case LXB_TAG_THEAD:
            case LXB_TAG_TR:
                return lxb_html_tree_insertion_mode_in_cell_t_closed(tree,
                                                                     token);
            default:
                return lxb_html_tree_insertion_mode_in_cell_anything_else_closed(tree,
                                                                                 token);
        }
    }

    switch (token->tag_id) {
        case LXB_TAG_CAPTION:
        case LXB_TAG_COL:
        case LXB_TAG_COLGROUP:
        case LXB_TAG_TBODY:
        case LXB_TAG_TD:
        case LXB_TAG_TFOOT:
        case LXB_TAG_TH:
        case LXB_TAG_THEAD:
        case LXB_TAG_TR:
            return lxb_html_tree_insertion_mode_in_cell_ct(tree, token);

        default:
            return lxb_html_tree_insertion_mode_in_cell_anything_else(tree,
                                                                      token);
    }
}
