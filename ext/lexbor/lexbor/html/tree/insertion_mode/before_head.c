/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"


static bool
lxb_html_tree_insertion_mode_before_head_open(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token);

static bool
lxb_html_tree_insertion_mode_before_head_closed(lxb_html_tree_t *tree,
                                                lxb_html_token_t *token);

lxb_inline bool
lxb_html_tree_insertion_mode_before_head_anything_else(lxb_html_tree_t *tree);

lxb_inline lxb_status_t
lxb_html_tree_insertion_mode_before_head_head(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token);


bool
lxb_html_tree_insertion_mode_before_head(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        return lxb_html_tree_insertion_mode_before_head_closed(tree, token);
    }

    return lxb_html_tree_insertion_mode_before_head_open(tree, token);
}

static bool
lxb_html_tree_insertion_mode_before_head_open(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    switch (token->tag_id) {
        case LXB_TAG__EM_COMMENT: {
            lxb_dom_comment_t *comment;

            comment = lxb_html_tree_insert_comment(tree, token, NULL);
            if (comment == NULL) {
                tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return lxb_html_tree_process_abort(tree);
            }

            break;
        }

        case LXB_TAG__EM_DOCTYPE:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_DOTOINBEHEMO);
            break;

        case LXB_TAG_HTML:
            return lxb_html_tree_insertion_mode_in_body(tree, token);

        case LXB_TAG_HEAD:
            tree->status = lxb_html_tree_insertion_mode_before_head_head(tree,
                                                                         token);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            tree->has_explicit_head_tag = true;

            tree->mode = lxb_html_tree_insertion_mode_in_head;

            break;

        case LXB_TAG__TEXT:
            tree->status = lxb_html_token_data_skip_ws_begin(token);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            if (token->text_start == token->text_end) {
                return true;
            }
            /* fall through */

        default:
            return lxb_html_tree_insertion_mode_before_head_anything_else(tree);
    }

    return true;
}

static bool
lxb_html_tree_insertion_mode_before_head_closed(lxb_html_tree_t *tree,
                                                lxb_html_token_t *token)
{
    switch (token->tag_id) {
        case LXB_TAG_HEAD:
        case LXB_TAG_BODY:
        case LXB_TAG_HTML:
        case LXB_TAG_BR:
            return lxb_html_tree_insertion_mode_before_head_anything_else(tree);

        default:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_UNCLTOINBEHEMO);
            break;
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_before_head_anything_else(lxb_html_tree_t *tree)
{
    lxb_html_token_t fake_token = {0};

    fake_token.tag_id = LXB_TAG_HEAD;

    tree->status = lxb_html_tree_insertion_mode_before_head_head(tree,
                                                                 &fake_token);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_head;

    return false;
}

lxb_inline lxb_status_t
lxb_html_tree_insertion_mode_before_head_head(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    tree->document->head = lxb_html_interface_head(element);

    return LXB_STATUS_OK;
}
