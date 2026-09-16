/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/tree/active_formatting.h"


lxb_inline bool
lxb_html_tree_insertion_mode_in_caption_caption_closed(lxb_html_tree_t *tree,
                                                       lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_CAPTION, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_TABLE);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_MIELINSC);

        return true;
    }

    lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                            LXB_NS__UNDEF);

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, LXB_TAG_CAPTION) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_CAPTION,
                                                 LXB_NS_HTML, true);

    lxb_html_tree_active_formatting_up_to_last_marker(tree);

    tree->mode = lxb_html_tree_insertion_mode_in_table;

    return true;
}

/*
 * A start tag whose tag name is one of: "caption", "col", "colgroup", "tbody",
 * "td", "tfoot", "th", "thead", "tr"
 * An end tag whose tag name is "table"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_caption_ct_open_closed(lxb_html_tree_t *tree,
                                                       lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_CAPTION, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_TABLE);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_MIELINSC);

        return true;
    }

    lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                            LXB_NS__UNDEF);

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, LXB_TAG_CAPTION) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_CAPTION,
                                                 LXB_NS_HTML, true);

    lxb_html_tree_active_formatting_up_to_last_marker(tree);

    tree->mode = lxb_html_tree_insertion_mode_in_table;

    return false;
}

/*
 * "body", "col", "colgroup", "html", "tbody", "td", "tfoot", "th", "thead",
 * "tr"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_caption_bcht_closed(lxb_html_tree_t *tree,
                                                    lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_caption_anything_else(lxb_html_tree_t *tree,
                                                      lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_body(tree, token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_caption_anything_else_closed(lxb_html_tree_t *tree,
                                                             lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_caption_anything_else(tree, token);
}

bool
lxb_html_tree_insertion_mode_in_caption(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        switch (token->tag_id) {
            case LXB_TAG_CAPTION:
                return lxb_html_tree_insertion_mode_in_caption_caption_closed(tree,
                                                                              token);
            case LXB_TAG_TABLE:
                return lxb_html_tree_insertion_mode_in_caption_ct_open_closed(tree,
                                                                              token);
            case LXB_TAG_BODY:
            case LXB_TAG_COL:
            case LXB_TAG_COLGROUP:
            case LXB_TAG_HTML:
            case LXB_TAG_TBODY:
            case LXB_TAG_TD:
            case LXB_TAG_TFOOT:
            case LXB_TAG_TH:
            case LXB_TAG_THEAD:
            case LXB_TAG_TR:
                return lxb_html_tree_insertion_mode_in_caption_bcht_closed(tree,
                                                                           token);
            default:
                return lxb_html_tree_insertion_mode_in_caption_anything_else_closed(tree,
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
            return lxb_html_tree_insertion_mode_in_caption_ct_open_closed(tree,
                                                                          token);
        default:
            return lxb_html_tree_insertion_mode_in_caption_anything_else(tree,
                                                                         token);
    }
}
