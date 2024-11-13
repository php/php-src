/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"


static bool
lxb_html_tree_insertion_mode_after_head_open(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token);
static bool
lxb_html_tree_insertion_mode_after_head_closed(lxb_html_tree_t *tree,
                                               lxb_html_token_t *token);

lxb_inline bool
lxb_html_tree_insertion_mode_after_head_anything_else(lxb_html_tree_t *tree);

lxb_inline lxb_html_element_t *
lxb_html_tree_insertion_mode_after_head_create_body(lxb_html_tree_t *tree,
                                                    lxb_html_token_t *token);


bool
lxb_html_tree_insertion_mode_after_head(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        return lxb_html_tree_insertion_mode_after_head_closed(tree, token);
    }

    return lxb_html_tree_insertion_mode_after_head_open(tree, token);
}

static bool
lxb_html_tree_insertion_mode_after_head_open(lxb_html_tree_t *tree,
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
                                      LXB_HTML_RULES_ERROR_DOTOAFHEMO);
            break;

        case LXB_TAG_HTML:
            return lxb_html_tree_insertion_mode_in_body(tree, token);

        case LXB_TAG_BODY: {
            lxb_html_element_t *element;

            element = lxb_html_tree_insertion_mode_after_head_create_body(tree,
                                                                          token);
            if (element == NULL) {
                tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return lxb_html_tree_process_abort(tree);
            }

            tree->has_explicit_body_tag = true;
            tree->frameset_ok = false;
            tree->mode = lxb_html_tree_insertion_mode_in_body;

            break;
        }

        case LXB_TAG_FRAMESET: {
            lxb_html_element_t *element;

            element = lxb_html_tree_insert_html_element(tree, token);
            if (element == NULL) {
                tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return lxb_html_tree_process_abort(tree);
            }

            tree->mode = lxb_html_tree_insertion_mode_in_frameset;

            break;
        }

        case LXB_TAG_BASE:
        case LXB_TAG_BASEFONT:
        case LXB_TAG_BGSOUND:
        case LXB_TAG_LINK:
        case LXB_TAG_META:
        case LXB_TAG_NOFRAMES:
        case LXB_TAG_SCRIPT:
        case LXB_TAG_STYLE:
        case LXB_TAG_TEMPLATE:
        case LXB_TAG_TITLE: {
            lxb_dom_node_t *head_node;

            head_node = lxb_dom_interface_node(tree->document->head);
            if (head_node == NULL) {
                tree->status = LXB_STATUS_ERROR;

                return lxb_html_tree_process_abort(tree);
            }

            lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

            tree->status = lxb_html_tree_open_elements_push(tree, head_node);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            lxb_html_tree_insertion_mode_in_head(tree, token);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            lxb_html_tree_open_elements_remove_by_node(tree, head_node);

            break;
        }

        case LXB_TAG_HEAD:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_HETOAFHEMO);
            break;

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
            return lxb_html_tree_insertion_mode_after_head_anything_else(tree);
    }

    return true;
}

static bool
lxb_html_tree_insertion_mode_after_head_closed(lxb_html_tree_t *tree,
                                               lxb_html_token_t *token)
{
    switch (token->tag_id) {
        case LXB_TAG_TEMPLATE:
            return lxb_html_tree_insertion_mode_in_head(tree, token);

        case LXB_TAG_BODY:
        case LXB_TAG_HTML:
        case LXB_TAG_BR:
            return lxb_html_tree_insertion_mode_after_head_anything_else(tree);

        default:
            lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

            break;
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_after_head_anything_else(lxb_html_tree_t *tree)
{
    lxb_html_element_t *element;
    lxb_html_token_t fake_token = {0};

    fake_token.tag_id = LXB_TAG_BODY;

    element = lxb_html_tree_insertion_mode_after_head_create_body(tree,
                                                                  &fake_token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_body;

    return false;
}

lxb_inline lxb_html_element_t *
lxb_html_tree_insertion_mode_after_head_create_body(lxb_html_tree_t *tree,
                                                    lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        return NULL;
    }

    tree->document->body = lxb_html_interface_body(element);

    return element;
}
