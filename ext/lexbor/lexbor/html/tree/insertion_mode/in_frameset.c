/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"


bool
lxb_html_tree_insertion_mode_in_frameset(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        if (token->tag_id == LXB_TAG_FRAMESET)
        {
            lxb_dom_node_t *node;
            node = lxb_html_tree_current_node(tree);

            if (node == lxb_html_tree_open_elements_first(tree)) {
                lxb_html_tree_parse_error(tree, token,
                                          LXB_HTML_RULES_ERROR_UNELINOPELST);
                return true;
            }

            lxb_html_tree_open_elements_pop(tree);

            node = lxb_html_tree_current_node(tree);

            if (tree->fragment == NULL
                && lxb_html_tree_node_is(node, LXB_TAG_FRAMESET) == false)
            {
                tree->mode = lxb_html_tree_insertion_mode_after_frameset;
            }

            return true;
        }

        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

        return true;
    }

    switch (token->tag_id) {
        case LXB_TAG__EM_COMMENT: {
            lxb_dom_comment_t *comment;

            comment = lxb_html_tree_insert_comment(tree, token, NULL);
            if (comment == NULL) {
                return lxb_html_tree_process_abort(tree);
            }

            break;
        }

        case LXB_TAG__EM_DOCTYPE:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_DOTOINFRMO);
            break;

        case LXB_TAG_HTML:
            return lxb_html_tree_insertion_mode_in_body(tree, token);

        case LXB_TAG_FRAMESET: {
            lxb_html_element_t *element;

            element = lxb_html_tree_insert_html_element(tree, token);
            if (element == NULL) {
                tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return lxb_html_tree_process_abort(tree);
            }

            break;
        }

        case LXB_TAG_FRAME: {
            lxb_html_element_t *element;

            element = lxb_html_tree_insert_html_element(tree, token);
            if (element == NULL) {
                tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return lxb_html_tree_process_abort(tree);
            }

            lxb_html_tree_open_elements_pop(tree);
            lxb_html_tree_acknowledge_token_self_closing(tree, token);

            break;
        }

        case LXB_TAG_NOFRAMES:
            return lxb_html_tree_insertion_mode_in_head(tree, token);

        case LXB_TAG__END_OF_FILE: {
            lxb_dom_node_t *node = lxb_html_tree_current_node(tree);

            if (node != lxb_html_tree_open_elements_first(tree)) {
                lxb_html_tree_parse_error(tree, token,
                                          LXB_HTML_RULES_ERROR_UNELINOPELST);
            }

            tree->status = lxb_html_tree_stop_parsing(tree);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            break;
        }

        case LXB_TAG__TEXT: {
            size_t cur_len;
            lexbor_str_t str;

            tree->status = lxb_html_token_make_text(token, &str,
                                                    tree->document->dom_document.text);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            cur_len = str.length;

            lexbor_str_stay_only_whitespace(&str);

            if (str.length != 0) {
                tree->status = lxb_html_tree_insert_character_for_data(tree,
                                                                       &str,
                                                                       NULL);
                if (tree->status != LXB_STATUS_OK) {
                    return lxb_html_tree_process_abort(tree);
                }
            }

            if (str.length == cur_len) {
                return true;
            }
        }
        /* fall through */

        default:
            lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

            break;
    }

    return true;
}
