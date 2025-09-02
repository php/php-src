/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"


bool
lxb_html_tree_insertion_mode_after_body(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token)
{
    switch (token->tag_id) {
        case LXB_TAG__EM_COMMENT: {
            lxb_dom_comment_t *comment;
            lxb_dom_node_t *html_node;

            html_node = lxb_html_tree_open_elements_first(tree);

            comment = lxb_html_tree_insert_comment(tree, token, html_node);
            if (comment == NULL) {
                return lxb_html_tree_process_abort(tree);
            }

            break;
        }

        case LXB_TAG__EM_DOCTYPE:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_DOTOAFBOMO);
            break;

        case LXB_TAG_HTML:
            if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE)
            {
                if (tree->fragment != NULL) {
                    lxb_html_tree_parse_error(tree, token,
                                              LXB_HTML_RULES_ERROR_UNCLTO);
                    return true;
                }

                tree->mode = lxb_html_tree_insertion_mode_after_after_body;

                return true;
            }

            return lxb_html_tree_insertion_mode_in_body(tree, token);

        case LXB_TAG__END_OF_FILE:
            tree->status = lxb_html_tree_stop_parsing(tree);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            break;

        case LXB_TAG__TEXT: {
            lxb_html_token_t ws_token = *token;

            tree->status = lxb_html_token_data_skip_ws_begin(&ws_token);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            if (ws_token.text_start == ws_token.text_end) {
                return lxb_html_tree_insertion_mode_in_body(tree, token);
            }
        }
        /* fall through */

        default:
            lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

            tree->mode = lxb_html_tree_insertion_mode_in_body;

            return false;
    }

    return true;
}
