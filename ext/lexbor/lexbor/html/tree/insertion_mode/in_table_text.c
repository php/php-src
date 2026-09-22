/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/token.h"


static void
lxb_html_tree_insertion_mode_in_table_text_erase(lxb_html_tree_t *tree);


bool
lxb_html_tree_insertion_mode_in_table_text(lxb_html_tree_t *tree,
                                           lxb_html_token_t *token)
{
    lxb_status_t status;
    lexbor_str_t *text;
    lexbor_array_obj_t *pt_list = tree->pending_table.text_list;

    if (token->tag_id == LXB_TAG__TEXT) {
        if (token->null_count != 0) {
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_NUCH);
        }

        text = lexbor_array_obj_push(pt_list);
        if (text == NULL) {
            tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

            lxb_html_tree_insertion_mode_in_table_text_erase(tree);

            return lxb_html_tree_process_abort(tree);
        }

        if (token->null_count != 0) {
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_NUCH);

            tree->status = lxb_html_token_make_text_drop_null(token, text,
                                                              tree->document->dom_document.text);
        }
        else {
            tree->status = lxb_html_token_make_text(token, text,
                                                    tree->document->dom_document.text);
        }

        if (tree->status != LXB_STATUS_OK) {
            lxb_html_tree_insertion_mode_in_table_text_erase(tree);

            return lxb_html_tree_process_abort(tree);
        }

        if (text->length == 0) {
            lexbor_array_obj_pop(pt_list);
            lexbor_str_destroy(text, tree->document->dom_document.text, false);

            return true;
        }

        /*
         * The lxb_html_token_data_skip_ws_begin function
         * can change token->text_start value.
         */
        size_t i_pos = lexbor_str_whitespace_from_begin(text);

        if (i_pos != text->length) {
            if (!tree->pending_table.have_non_ws) {
                tree->pending_table.have_non_ws = true;
            }
        }

        return true;
    }

    if (tree->pending_table.have_non_ws) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_CHINTATE);

        tree->foster_parenting = true;

        for (size_t i = 0; i < lexbor_array_obj_length(pt_list); i++) {
            text = lexbor_array_obj_get(pt_list, i);

            status = lxb_html_tree_insertion_mode_in_body_text_append(tree,
                                                                      text);
            if (status != LXB_STATUS_OK) {
                lxb_html_tree_insertion_mode_in_table_text_erase(tree);

                return lxb_html_tree_process_abort(tree);
            }
        }

        tree->foster_parenting = false;
    }
    else {
        for (size_t i = 0; i < lexbor_array_obj_length(pt_list); i++) {
            text = lexbor_array_obj_get(pt_list, i);

            tree->status = lxb_html_tree_insert_character_for_data(tree, text,
                                                                   NULL);
            if (tree->status != LXB_STATUS_OK) {
                lxb_html_tree_insertion_mode_in_table_text_erase(tree);

                return lxb_html_tree_process_abort(tree);
            }
        }
    }

    tree->mode = tree->original_mode;

    return false;
}

static void
lxb_html_tree_insertion_mode_in_table_text_erase(lxb_html_tree_t *tree)
{
    lexbor_str_t *text;
    lexbor_array_obj_t *pt_list = tree->pending_table.text_list;

    for (size_t i = 0; i < lexbor_array_obj_length(pt_list); i++) {
        text = lexbor_array_obj_get(pt_list, i);

        lexbor_str_destroy(text, tree->document->dom_document.text, false);
    }
}
