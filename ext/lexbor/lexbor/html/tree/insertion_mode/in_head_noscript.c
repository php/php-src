/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"


static bool
lxb_html_tree_insertion_mode_in_head_noscript_open(lxb_html_tree_t *tree,
                                                   lxb_html_token_t *token);

static bool
lxb_html_tree_insertion_mode_in_head_noscript_closed(lxb_html_tree_t *tree,
                                                     lxb_html_token_t *token);


bool
lxb_html_tree_insertion_mode_in_head_noscript(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
       return lxb_html_tree_insertion_mode_in_head_noscript_closed(tree, token);
    }

    return lxb_html_tree_insertion_mode_in_head_noscript_open(tree, token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_head_noscript_anything_else(lxb_html_tree_t *tree,
                                                            lxb_html_token_t *token);


static bool
lxb_html_tree_insertion_mode_in_head_noscript_open(lxb_html_tree_t *tree,
                                                   lxb_html_token_t *token)
{
    switch (token->tag_id) {
        case LXB_TAG__EM_DOCTYPE:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_DOTOINHENOMO);
            break;

        case LXB_TAG_HTML:
            return lxb_html_tree_insertion_mode_in_body(tree, token);

        case LXB_TAG__EM_COMMENT:
        case LXB_TAG_BASEFONT:
        case LXB_TAG_BGSOUND:
        case LXB_TAG_LINK:
        case LXB_TAG_META:
        case LXB_TAG_NOFRAMES:
        case LXB_TAG_STYLE:
            return lxb_html_tree_insertion_mode_in_head(tree, token);

        case LXB_TAG_HEAD:
        case LXB_TAG_NOSCRIPT:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_UNTO);
            break;

        /* CopyPast from "in head" insertion mode */
        case LXB_TAG__TEXT: {
            lxb_html_token_t ws_token = {0};

            tree->status = lxb_html_token_data_split_ws_begin(token, &ws_token);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            if (ws_token.text_start != ws_token.text_end) {
                tree->status = lxb_html_tree_insert_character(tree, &ws_token,
                                                              NULL);
                if (tree->status != LXB_STATUS_OK) {
                    return lxb_html_tree_process_abort(tree);
                }
            }

            if (token->text_start == token->text_end) {
                return true;
            }
        }
        /* fall through */

        default:
            return lxb_html_tree_insertion_mode_in_head_noscript_anything_else(tree,
                                                                               token);
    }

    return true;
}

static bool
lxb_html_tree_insertion_mode_in_head_noscript_closed(lxb_html_tree_t *tree,
                                                     lxb_html_token_t *token)
{
    if(token->tag_id == LXB_TAG_BR) {
        return lxb_html_tree_insertion_mode_in_head_noscript_anything_else(tree,
                                                                            token);
    }

    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_head_noscript_anything_else(lxb_html_tree_t *tree,
                                                            lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token,
                              LXB_HTML_RULES_ERROR_UNTO);

    lxb_html_tree_open_elements_pop(tree);

    tree->mode = lxb_html_tree_insertion_mode_in_head;

    return false;
}
