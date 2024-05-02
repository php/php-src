/*
 * Copyright (C) 2018-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/interfaces/style_element.h"


bool
lxb_html_tree_insertion_mode_text(lxb_html_tree_t *tree,
                                  lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_style_element_t *style;

    switch (token->tag_id) {
        case LXB_TAG__TEXT: {
            tree->status = lxb_html_tree_insert_character(tree, token, NULL);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            break;
        }

        case LXB_TAG__END_OF_FILE: {
            lxb_dom_node_t *node;

            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_UNENOFFI);

            node = lxb_html_tree_current_node(tree);

            if (lxb_html_tree_node_is(node, LXB_TAG_SCRIPT)) {
                /* TODO: mark the script element as "already started" */
            }

            lxb_html_tree_open_elements_pop(tree);

            tree->mode = tree->original_mode;

            return false;
        }

        /* TODO: need to implement */
        case LXB_TAG_SCRIPT:
            lxb_html_tree_open_elements_pop(tree);

            tree->mode = tree->original_mode;

            break;

        case LXB_TAG_STYLE:
            node = lxb_html_tree_open_elements_pop(tree);

            tree->mode = tree->original_mode;

            if (!tree->document->css_init) {
                break;
            }

            style = lxb_html_interface_style(node);

            tree->status = lxb_html_style_element_parse(style);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            tree->status = lxb_html_document_stylesheet_add(tree->document,
                                                            style->stylesheet);
            if (tree->status != LXB_STATUS_OK) {
                return lxb_html_tree_process_abort(tree);
            }

            break;

        default:
            lxb_html_tree_open_elements_pop(tree);

            tree->mode = tree->original_mode;

            break;
    }

    return true;
}
