/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/interfaces/html_element.h"


static bool
lxb_html_tree_insertion_mode_before_html_open(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token);

static bool
lxb_html_tree_insertion_mode_before_html_closed(lxb_html_tree_t *tree,
                                                lxb_html_token_t *token);

lxb_inline bool
lxb_html_tree_insertion_mode_before_html_anything_else(lxb_html_tree_t *tree);

lxb_inline lxb_status_t
lxb_html_tree_insertion_mode_before_html_html(lxb_html_tree_t *tree,
                                              lxb_dom_node_t *node_html);


bool
lxb_html_tree_insertion_mode_before_html(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        return lxb_html_tree_insertion_mode_before_html_closed(tree, token);;
    }

    return lxb_html_tree_insertion_mode_before_html_open(tree, token);
}

static bool
lxb_html_tree_insertion_mode_before_html_open(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    switch (token->tag_id) {
        case LXB_TAG__EM_DOCTYPE:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_DOTOINBEHTMO);
            break;

        case LXB_TAG__EM_COMMENT: {
            lxb_dom_comment_t *comment;

            comment = lxb_html_tree_insert_comment(tree, token,
                                        lxb_dom_interface_node(tree->document));
            if (comment == NULL) {
                return lxb_html_tree_process_abort(tree);
            }

            break;
        }

        case LXB_TAG_HTML: {
            lxb_dom_node_t *node_html;
            lxb_html_element_t *element;

            element = lxb_html_tree_create_element_for_token(tree, token,
                                            LXB_NS_HTML,
                                            &tree->document->dom_document.node);
            if (element == NULL) {
                tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return lxb_html_tree_process_abort(tree);
            }

            node_html = lxb_dom_interface_node(element);

            tree->status = lxb_html_tree_insertion_mode_before_html_html(tree,
                                                                     node_html);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            tree->has_explicit_html_tag = true;

            tree->mode = lxb_html_tree_insertion_mode_before_head;

            break;
        }

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
            return lxb_html_tree_insertion_mode_before_html_anything_else(tree);
    }

    return true;
}

static bool
lxb_html_tree_insertion_mode_before_html_closed(lxb_html_tree_t *tree,
                                                lxb_html_token_t *token)
{
    switch (token->tag_id) {
        case LXB_TAG_HEAD:
        case LXB_TAG_BODY:
        case LXB_TAG_HTML:
        case LXB_TAG_BR:
            return lxb_html_tree_insertion_mode_before_html_anything_else(tree);

        default:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_UNCLTOINBEHTMO);
            break;
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_before_html_anything_else(lxb_html_tree_t *tree)
{
    lxb_dom_node_t *node_html;

    node_html = lxb_html_tree_create_node(tree, LXB_TAG_HTML, LXB_NS_HTML);
    if (node_html == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        return lxb_html_tree_process_abort(tree);
    }

    tree->status = lxb_html_tree_insertion_mode_before_html_html(tree,
                                                                 node_html);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_before_head;

    return false;
}

lxb_inline lxb_status_t
lxb_html_tree_insertion_mode_before_html_html(lxb_html_tree_t *tree,
                                              lxb_dom_node_t *node_html)
{
    lxb_status_t status;

    status = lxb_html_tree_open_elements_push(tree, node_html);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lxb_html_tree_insert_node(lxb_dom_interface_node(tree->document),
                              node_html,
                              LXB_HTML_TREE_INSERTION_POSITION_CHILD);

    lxb_dom_document_attach_element(&tree->document->dom_document,
                                    lxb_dom_interface_element(node_html));

    return LXB_STATUS_OK;
}
