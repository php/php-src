/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/tree/active_formatting.h"
#include "lexbor/html/tree/template_insertion.h"
#include "lexbor/html/interfaces/script_element.h"
#include "lexbor/html/interfaces/template_element.h"
#include "lexbor/html/tokenizer/state_script.h"


static bool
lxb_html_tree_insertion_mode_in_head_open(lxb_html_tree_t *tree,
                                          lxb_html_token_t *token);

static bool
lxb_html_tree_insertion_mode_in_head_closed(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token);

lxb_inline bool
lxb_html_tree_insertion_mode_in_head_script(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token);

lxb_inline bool
lxb_html_tree_insertion_mode_in_head_template(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token);

lxb_inline bool
lxb_html_tree_insertion_mode_in_head_template_closed(lxb_html_tree_t *tree,
                                                     lxb_html_token_t *token);

lxb_inline bool
lxb_html_tree_insertion_mode_in_head_anything_else(lxb_html_tree_t *tree);


bool
lxb_html_tree_insertion_mode_in_head(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        return lxb_html_tree_insertion_mode_in_head_closed(tree, token);;
    }

    return lxb_html_tree_insertion_mode_in_head_open(tree, token);
}

static bool
lxb_html_tree_insertion_mode_in_head_open(lxb_html_tree_t *tree,
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
                                      LXB_HTML_RULES_ERROR_DOTOINHEMO);
            break;

        case LXB_TAG_HTML:
            return lxb_html_tree_insertion_mode_in_body(tree, token);

        case LXB_TAG_BASE:
        case LXB_TAG_BASEFONT:
        case LXB_TAG_BGSOUND:
        case LXB_TAG_LINK: {
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

        case LXB_TAG_META: {
            lxb_html_element_t *element;

            element = lxb_html_tree_insert_html_element(tree, token);
            if (element == NULL) {
                tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return lxb_html_tree_process_abort(tree);
            }

            lxb_html_tree_open_elements_pop(tree);
            lxb_html_tree_acknowledge_token_self_closing(tree, token);

            /*
             * TODO: Check encoding: charset attribute or http-equiv attribute.
             */

            break;
        }

        case LXB_TAG_TITLE: {
            lxb_html_element_t *element;

            element = lxb_html_tree_generic_rcdata_parsing(tree, token);
            if (element == NULL) {
                tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return lxb_html_tree_process_abort(tree);
            }

            break;
        }

        case LXB_TAG_NOSCRIPT: {
            lxb_html_element_t *element;

            if (tree->document->dom_document.scripting) {
                element = lxb_html_tree_generic_rawtext_parsing(tree, token);
            }
            else {
                element = lxb_html_tree_insert_html_element(tree, token);
                tree->mode = lxb_html_tree_insertion_mode_in_head_noscript;
            }

            if (element == NULL) {
                tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return lxb_html_tree_process_abort(tree);
            }

            break;
        }

        case LXB_TAG_NOFRAMES:
        case LXB_TAG_STYLE: {
            lxb_html_element_t *element;

            element = lxb_html_tree_generic_rawtext_parsing(tree, token);
            if (element == NULL) {
                tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return lxb_html_tree_process_abort(tree);
            }

            break;
        }

        case LXB_TAG_SCRIPT:
            return lxb_html_tree_insertion_mode_in_head_script(tree, token);

        case LXB_TAG_TEMPLATE:
            return lxb_html_tree_insertion_mode_in_head_template(tree, token);

        case LXB_TAG_HEAD:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_HETOINHEMO);
            break;

        /*
         * We can create function for this, but...
         *
         * The "in head noscript" insertion mode use this
         * is you change this code, please, change it in in head noscript" mode
         */
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
            return lxb_html_tree_insertion_mode_in_head_anything_else(tree);
    }

    return true;
}

static bool
lxb_html_tree_insertion_mode_in_head_closed(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    switch (token->tag_id) {
        case LXB_TAG_HEAD:
            lxb_html_tree_open_elements_pop(tree);

            tree->mode = lxb_html_tree_insertion_mode_after_head;

            break;

        case LXB_TAG_BODY:
        case LXB_TAG_HTML:
        case LXB_TAG_BR:
            return lxb_html_tree_insertion_mode_in_head_anything_else(tree);

        case LXB_TAG_TEMPLATE:
            return lxb_html_tree_insertion_mode_in_head_template_closed(tree,
                                                                        token);

        default:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_UNCLTOINHEMO);
            break;

    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_head_script(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    lxb_dom_node_t *ap_node;
    lxb_html_element_t *element;
    lxb_html_tree_insertion_position_t ipos;

    ap_node = lxb_html_tree_appropriate_place_inserting_node(tree, NULL, &ipos);
    if (ap_node == NULL) {
        tree->status = LXB_STATUS_ERROR;

        return lxb_html_tree_process_abort(tree);
    }

    if (ipos == LXB_HTML_TREE_INSERTION_POSITION_CHILD) {
        element = lxb_html_tree_create_element_for_token(tree, token,
                                                         LXB_NS_HTML, ap_node);
    }
    else {
        element = lxb_html_tree_create_element_for_token(tree, token,
                                                         LXB_NS_HTML,
                                                         ap_node->parent);
    }

    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    /* TODO: Need code for set flags for Script Element */

    tree->status = lxb_html_tree_open_elements_push(tree,
                                                    lxb_dom_interface_node(element));
    if (tree->status != LXB_HTML_STATUS_OK) {
        lxb_html_script_element_interface_destroy(lxb_html_interface_script(element));

        return lxb_html_tree_process_abort(tree);
    }

    lxb_html_tree_insert_node(ap_node, lxb_dom_interface_node(element), ipos);

    /*
     * Need for tokenizer state Script
     * See description for
     * 'lxb_html_tokenizer_state_script_data_before' function
     */
    lxb_html_tokenizer_tmp_tag_id_set(tree->tkz_ref, token->tag_id);
    lxb_html_tokenizer_state_set(tree->tkz_ref,
                                 lxb_html_tokenizer_state_script_data_before);

    tree->original_mode = tree->mode;
    tree->mode = lxb_html_tree_insertion_mode_text;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_head_template(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->status = lxb_html_tree_active_formatting_push_marker(tree);
    if (tree->status != LXB_STATUS_OK) {
        lxb_html_template_element_interface_destroy(lxb_html_interface_template(element));

        return lxb_html_tree_process_abort(tree);
    }

    tree->frameset_ok = false;
    tree->mode = lxb_html_tree_insertion_mode_in_template;

    tree->status = lxb_html_tree_template_insertion_push(tree,
                                      lxb_html_tree_insertion_mode_in_template);
    if (tree->status != LXB_STATUS_OK) {
        lxb_html_template_element_interface_destroy(lxb_html_interface_template(element));

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_head_template_closed(lxb_html_tree_t *tree,
                                                     lxb_html_token_t *token)
{
    lxb_dom_node_t *temp_node;

    temp_node = lxb_html_tree_open_elements_find_reverse(tree, LXB_TAG_TEMPLATE,
                                                         LXB_NS_HTML, NULL);
    if (temp_node == NULL) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_TECLTOWIOPINHEMO);
        return true;
    }

    lxb_html_tree_generate_all_implied_end_tags_thoroughly(tree, LXB_TAG__UNDEF,
                                                           LXB_NS__UNDEF);

    temp_node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(temp_node, LXB_TAG_TEMPLATE) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_TEELISNOCUINHEMO);
    }

    lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_TEMPLATE,
                                                 LXB_NS_HTML, true);

    lxb_html_tree_active_formatting_up_to_last_marker(tree);
    lxb_html_tree_template_insertion_pop(tree);
    lxb_html_tree_reset_insertion_mode_appropriately(tree);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_head_anything_else(lxb_html_tree_t *tree)
{
    lxb_html_tree_open_elements_pop(tree);

    tree->mode = lxb_html_tree_insertion_mode_after_head;

    return false;
}
