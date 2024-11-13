/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#define LEXBOR_TOKENIZER_CHARS_MAP
#include "lexbor/core/str_res.h"

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/tree/active_formatting.h"
#include "lexbor/html/interfaces/head_element.h"
#include "lexbor/html/tokenizer/state.h"
#include "lexbor/html/tokenizer/state_rcdata.h"


/*
 * User case insertion mode.
 *
 * After "pre" and "listing" tag we need skip one newline in text tag.
 * Since we have a stream of tokens,
 * we can "look into the future" only in this way.
 */
bool
lxb_html_tree_insertion_mode_in_body_skip_new_line(lxb_html_tree_t *tree,
                                                   lxb_html_token_t *token)
{
    tree->mode = tree->original_mode;

    if (token->tag_id != LXB_TAG__TEXT) {
        return false;
    }

    tree->status = lxb_html_token_data_skip_one_newline_begin(token);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    if (token->text_start == token->text_end) {
        return true;
    }

    return false;
}

/*
 * User case insertion mode.
 *
 * After "textarea" tag we need skip one newline in text tag.
 */
bool
lxb_html_tree_insertion_mode_in_body_skip_new_line_textarea(lxb_html_tree_t *tree,
                                                            lxb_html_token_t *token)
{
    tree->mode = tree->original_mode;

    if (token->tag_id != LXB_TAG__TEXT) {
        return false;
    }

    tree->status = lxb_html_token_data_skip_one_newline_begin(token);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    if (token->text_start == token->text_end) {
        return true;
    }

    return false;
}

/* Open */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_text(lxb_html_tree_t *tree,
                                          lxb_html_token_t *token)
{
    lexbor_str_t str;

    if (token->null_count != 0) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_NUCH);

        tree->status = lxb_html_token_make_text_drop_null(token, &str,
                                                          tree->document->dom_document.text);
    }
    else {
        tree->status = lxb_html_token_make_text(token, &str,
                                                tree->document->dom_document.text);
    }

    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    /* Can be zero only if all NULL are gone */
    if (str.length == 0) {
        lexbor_str_destroy(&str, tree->document->dom_document.text, false);

        return true;
    }

    lxb_html_tree_insertion_mode_in_body_text_append(tree, &str);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_status_t
lxb_html_tree_insertion_mode_in_body_text_append(lxb_html_tree_t *tree,
                                                 lexbor_str_t *str)
{
    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return tree->status;
    }

    if (tree->frameset_ok) {
        const lxb_char_t *pos = str->data;
        const lxb_char_t *end = str->data + str->length;

        while (pos != end) {
            if (lexbor_tokenizer_chars_map[*pos]
                != LEXBOR_STR_RES_MAP_CHAR_WHITESPACE)
            {
                tree->frameset_ok = false;
                break;
            }

            pos++;
        }
    }

    tree->status = lxb_html_tree_insert_character_for_data(tree, str, NULL);
    if (tree->status != LXB_STATUS_OK) {
        return tree->status;
    }

    return LXB_STATUS_OK;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_comment(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token)
{
    lxb_dom_comment_t *comment;

    comment = lxb_html_tree_insert_comment(tree, token, NULL);
    if (comment == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_doctype(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_DOTOINBOMO);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_html(lxb_html_tree_t *tree,
                                          lxb_html_token_t *token)
{
    lxb_dom_node_t *temp_node;
    lxb_dom_element_t *html;

    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

    temp_node = lxb_html_tree_open_elements_find(tree, LXB_TAG_TEMPLATE,
                                                 LXB_NS_HTML, NULL);
    if (temp_node != NULL) {
        return true;
    }

    html = lxb_dom_interface_element(lxb_html_tree_open_elements_first(tree));

    tree->status = lxb_html_tree_append_attributes(tree, html, token,
                                                   html->node.ns);
    if (tree->status != LXB_HTML_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

/*
 * Start tag:
 *      "base", "basefont", "bgsound", "link", "meta", "noframes",
 *      "script", "style", "template", "title"
 * End Tag:
 *      "template"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_blmnst(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_head(tree, token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_body(lxb_html_tree_t *tree,
                                          lxb_html_token_t *token)
{
    lxb_dom_node_t *node, *temp;
    lxb_dom_element_t *body;

    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

    node = lxb_html_tree_open_elements_get(tree, 1);
    if (node == NULL || node->local_name != LXB_TAG_BODY) {
        return true;
    }

    temp = lxb_html_tree_open_elements_find_reverse(tree, LXB_TAG_TEMPLATE,
                                                    LXB_NS_HTML, NULL);
    if (temp != NULL) {
        return true;
    }

    tree->frameset_ok = false;

    body = lxb_dom_interface_element(node);

    tree->status = lxb_html_tree_append_attributes(tree, body, token, node->ns);
    if (tree->status != LXB_HTML_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_frameset(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

    node = lxb_html_tree_open_elements_get(tree, 1);
    if (node == NULL || node->local_name != LXB_TAG_BODY) {
        return true;
    }

    if (tree->frameset_ok == false) {
        return true;
    }

    lxb_html_tree_node_delete_deep(tree, node);

    /* node is HTML */
    node = lxb_html_tree_open_elements_get(tree, 0);
    lxb_html_tree_open_elements_pop_until_node(tree, node, false);

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_frameset;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_eof(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token)
{
    if (lexbor_array_obj_length(tree->template_insertion_modes) != 0) {
        return lxb_html_tree_insertion_mode_in_template(tree, token);
    }

    bool it_is = lxb_html_tree_check_scope_element(tree);
    if (it_is == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_BAENOPELISWR);
    }

    tree->status = lxb_html_tree_stop_parsing(tree);
    if (tree->status != LXB_HTML_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_body_closed(lxb_html_tree_t *tree,
                                                 lxb_html_token_t *token)
{
    lxb_dom_node_t *body_node;

    body_node = lxb_html_tree_element_in_scope(tree, LXB_TAG_BODY, LXB_NS_HTML,
                                               LXB_HTML_TAG_CATEGORY_SCOPE);
    if (body_node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_NOBOELINSC);

        return true;
    }

    bool it_is = lxb_html_tree_check_scope_element(tree);
    if (it_is == false) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_OPELISWR);
    }

    tree->mode = lxb_html_tree_insertion_mode_after_body;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_html_closed(lxb_html_tree_t *tree,
                                                 lxb_html_token_t *token)
{
    lxb_dom_node_t *body_node;

    body_node = lxb_html_tree_element_in_scope(tree, LXB_TAG_BODY, LXB_NS_HTML,
                                               LXB_HTML_TAG_CATEGORY_SCOPE);
    if (body_node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_NOBOELINSC);

        return true;
    }

    bool it_is = lxb_html_tree_check_scope_element(tree);
    if (it_is == false) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_OPELISWR);
    }

    tree->mode = lxb_html_tree_insertion_mode_after_body;

    return false;
}

/*
 * "address", "article", "aside", "blockquote", "center", "details", "dialog",
 * "dir", "div", "dl", "fieldset", "figcaption", "figure", "footer", "header",
 * "hgroup", "main", "menu", "nav", "ol", "p", "section", "summary", "ul"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_abcdfhmnopsu(lxb_html_tree_t *tree,
                                                  lxb_html_token_t *token)
{
    lxb_dom_node_t *body_node;
    lxb_html_element_t *element;

    body_node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                               LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
    if (body_node != NULL) {
        lxb_html_tree_close_p_element(tree, token);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

/*
 * "h1", "h2", "h3", "h4", "h5", "h6"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_h123456(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
    if (node != NULL) {
        lxb_html_tree_close_p_element(tree, token);
    }

    node = lxb_html_tree_current_node(tree);

    switch (node->local_name) {
        case LXB_TAG_H1:
        case LXB_TAG_H2:
        case LXB_TAG_H3:
        case LXB_TAG_H4:
        case LXB_TAG_H5:
        case LXB_TAG_H6:
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_UNELINOPELST);

            lxb_html_tree_open_elements_pop(tree);
            break;

        default:
            break;
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

/*
 * "pre", "listing"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_pre_listing(lxb_html_tree_t *tree,
                                                 lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
    if (node != NULL) {
        lxb_html_tree_close_p_element(tree, token);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->original_mode = tree->mode;
    tree->mode = lxb_html_tree_insertion_mode_in_body_skip_new_line;
    tree->frameset_ok = false;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_form(lxb_html_tree_t *tree,
                                          lxb_html_token_t *token)
{
    lxb_dom_node_t *node, *temp;
    lxb_html_element_t *element;

    temp = lxb_html_tree_open_elements_find_reverse(tree, LXB_TAG_TEMPLATE,
                                                    LXB_NS_HTML, NULL);

    if (tree->form != NULL && temp == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

        return true;
    }

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
    if (node != NULL) {
        lxb_html_tree_close_p_element(tree, token);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    if (temp == NULL) {
        tree->form = lxb_html_interface_form(element);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_li(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token)
{
    bool is_special;
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    void **list = tree->open_elements->list;
    size_t idx = tree->open_elements->length;

    tree->frameset_ok = false;

    while (idx != 0) {
        idx--;

        node = list[idx];

        if (lxb_html_tree_node_is(node, LXB_TAG_LI)) {
            lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG_LI,
                                                    LXB_NS_HTML);

            node = lxb_html_tree_current_node(tree);

            if (lxb_html_tree_node_is(node, LXB_TAG_LI) == false) {
                lxb_html_tree_parse_error(tree, token,
                                          LXB_HTML_RULES_ERROR_UNELINOPELST);
            }

            lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_LI,
                                                         LXB_NS_HTML, true);
            break;
        }

        is_special = lxb_html_tag_is_category(node->local_name, node->ns,
                                              LXB_HTML_TAG_CATEGORY_SPECIAL);
        if (is_special
            && lxb_html_tree_node_is(node, LXB_TAG_ADDRESS) == false
            && lxb_html_tree_node_is(node, LXB_TAG_DIV) == false
            && lxb_html_tree_node_is(node, LXB_TAG_P) == false)
        {
            break;
        }
    }

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
    if (node != NULL) {
        lxb_html_tree_close_p_element(tree, token);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

/*
 * "dd", "dt"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_dd_dt(lxb_html_tree_t *tree,
                                           lxb_html_token_t *token)
{
    bool is_special;
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    void **list = tree->open_elements->list;
    size_t idx = tree->open_elements->length;

    tree->frameset_ok = false;

    while (idx != 0) {
        idx--;

        node = list[idx];

        if (lxb_html_tree_node_is(node, LXB_TAG_DD)) {
            lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG_DD,
                                                    LXB_NS_HTML);

            node = lxb_html_tree_current_node(tree);

            if (lxb_html_tree_node_is(node, LXB_TAG_DD) == false) {
                lxb_html_tree_parse_error(tree, token,
                                          LXB_HTML_RULES_ERROR_UNELINOPELST);
            }

            lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_DD,
                                                         LXB_NS_HTML, true);
            break;
        }

        if (lxb_html_tree_node_is(node, LXB_TAG_DT)) {
            lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG_DT,
                                                    LXB_NS_HTML);

            node = lxb_html_tree_current_node(tree);

            if (lxb_html_tree_node_is(node, LXB_TAG_DT) == false) {
                lxb_html_tree_parse_error(tree, token,
                                          LXB_HTML_RULES_ERROR_UNELINOPELST);
            }

            lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_DT,
                                                         LXB_NS_HTML, true);
            break;
        }

        is_special = lxb_html_tag_is_category(node->local_name, node->ns,
                                              LXB_HTML_TAG_CATEGORY_SPECIAL);
        if (is_special
            && lxb_html_tree_node_is(node, LXB_TAG_ADDRESS) == false
            && lxb_html_tree_node_is(node, LXB_TAG_DIV) == false
            && lxb_html_tree_node_is(node, LXB_TAG_P) == false)
        {
            break;
        }
    }

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
    if (node != NULL) {
        lxb_html_tree_close_p_element(tree, token);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_plaintext(lxb_html_tree_t *tree,
                                               lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
    if (node != NULL) {
        lxb_html_tree_close_p_element(tree, token);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    lxb_html_tokenizer_state_set(tree->tkz_ref,
                                 lxb_html_tokenizer_state_plaintext_before);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_button(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_BUTTON, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE);
    if (node != NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

        lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                                LXB_NS__UNDEF);

        lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_BUTTON,
                                                     LXB_NS_HTML, true);
    }

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->frameset_ok = false;

    return true;
}

/*
 * "address", "article", "aside", "blockquote", "button",  "center", "details",
 * "dialog", "dir", "div", "dl", "fieldset", "figcaption", "figure", "footer",
 * "header", "hgroup", "listing", "main", "menu", "nav", "ol", "pre", "section",
 * "summary", "ul"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_abcdfhlmnopsu_closed(lxb_html_tree_t *tree,
                                                          lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, token->tag_id,
                                          LXB_NS_HTML, LXB_HTML_TAG_CATEGORY_SCOPE);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

        return true;
    }

    lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                            LXB_NS__UNDEF);

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, token->tag_id) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_tag_id(tree, token->tag_id,
                                                 LXB_NS_HTML, true);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_form_closed(lxb_html_tree_t *tree,
                                                 lxb_html_token_t *token)
{
    lxb_dom_node_t *node, *current;

    node = lxb_html_tree_open_elements_find_reverse(tree, LXB_TAG_TEMPLATE,
                                                    LXB_NS_HTML, NULL);
    if (node == NULL) {
        node = lxb_dom_interface_node(tree->form);

        tree->form = NULL;

        if (node == NULL) {
            lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

            return true;
        }

        node = lxb_html_tree_element_in_scope_by_node(tree, node,
                                                      LXB_HTML_TAG_CATEGORY_SCOPE);
        if (node == NULL) {
            lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

            return true;
        }

        lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                                LXB_NS__UNDEF);

        current = lxb_html_tree_current_node(tree);

        if (current != node) {
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_UNELINOPELST);
        }

        lxb_html_tree_open_elements_remove_by_node(tree, node);

        return true;
    }

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_FORM, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

        return true;
    }

    lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                            LXB_NS__UNDEF);

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, LXB_TAG_FORM) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_FORM,
                                                 LXB_NS_HTML, true);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_p_closed(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
    if (node == NULL) {
        lxb_html_token_t fake_token = {0};
        lxb_html_element_t *element;

        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNCLTO);

        fake_token.tag_id = LXB_TAG_P;

        element = lxb_html_tree_insert_html_element(tree, &fake_token);
        if (element == NULL) {
            tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

            return lxb_html_tree_process_abort(tree);
        }
    }

    lxb_html_tree_close_p_element(tree, token);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_li_closed(lxb_html_tree_t *tree,
                                               lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_LI, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_LIST_ITEM);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNCLTO);
        return true;
    }

    lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG_LI, LXB_NS_HTML);

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, LXB_TAG_LI) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_LI, LXB_NS_HTML,
                                                 true);

    return true;
}

/*
 * "dd", "dt"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_dd_dt_closed(lxb_html_tree_t *tree,
                                                  lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, token->tag_id, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNCLTO);
        return true;
    }

    lxb_html_tree_generate_implied_end_tags(tree, token->tag_id, LXB_NS_HTML);

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, token->tag_id) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_tag_id(tree, token->tag_id,
                                                 LXB_NS_HTML, true);

    return true;
}

/*
 * "h1", "h2", "h3", "h4", "h5", "h6"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_h123456_closed(lxb_html_tree_t *tree,
                                                    lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope_h123456(tree);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNCLTO);
        return true;
    }

    lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                            LXB_NS__UNDEF);

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, token->tag_id) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_h123456(tree);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_a(lxb_html_tree_t *tree,
                                       lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    node = lxb_html_tree_active_formatting_between_last_marker(tree,
                                                               token->tag_id,
                                                               NULL);
    if (node != NULL) {
        /* bool is; */

        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINACFOST);

        lxb_html_tree_adoption_agency_algorithm(tree, token,
                                                &tree->status);
        if (tree->status != LXB_STATUS_OK) {
            return lxb_html_tree_process_abort(tree);
        }

/*
        if (is) {
            return lxb_html_tree_insertion_mode_in_body_anything_else_closed(tree,
                                                                             token);
        }
*/

        lxb_html_tree_active_formatting_remove_by_node(tree, node);
        lxb_html_tree_open_elements_remove_by_node(tree, node);
    }

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    node = lxb_dom_interface_node(element);

    lxb_html_tree_active_formatting_push_with_check_dupl(tree, node);

    return true;
}

/*
 * "b", "big", "code", "em", "font", "i", "s", "small", "strike", "strong",
 * "tt", "u"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_bcefistu(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    node = lxb_dom_interface_node(element);

    lxb_html_tree_active_formatting_push_with_check_dupl(tree, node);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_nobr(lxb_html_tree_t *tree,
                                          lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_NOBR, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE);
    if (node != NULL) {
        /* bool is; */

        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINSC);

        lxb_html_tree_adoption_agency_algorithm(tree, token, &tree->status);
        if (tree->status != LXB_STATUS_OK) {
            return lxb_html_tree_process_abort(tree);
        }
/*
        if (is) {
            return lxb_html_tree_insertion_mode_in_body_anything_else_closed(tree,
                                                                             token);
        }
*/
        tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
        if (tree->status != LXB_STATUS_OK) {
            return lxb_html_tree_process_abort(tree);
        }
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    node = lxb_dom_interface_node(element);

    lxb_html_tree_active_formatting_push_with_check_dupl(tree, node);

    return true;
}

/*
 * "a", "b", "big", "code", "em", "font", "i", "nobr", "s", "small", "strike",
 * "strong", "tt", "u"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_abcefinstu_closed(lxb_html_tree_t *tree,
                                                       lxb_html_token_t *token)
{
    /* bool is; */

    lxb_html_tree_adoption_agency_algorithm(tree, token, &tree->status);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

/*
    if (is) {
        return lxb_html_tree_insertion_mode_in_body_anything_else_closed(tree,
                                                                         token);
    }
*/

    return true;
}

/*
 * "applet", "marquee", "object"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_amo(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->status = lxb_html_tree_active_formatting_push_marker(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->frameset_ok = false;

    return true;
}

/*
 * "applet", "marquee", "object"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_amo_closed(lxb_html_tree_t *tree,
                                                lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, token->tag_id, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNCLTO);
        return true;
    }

    lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                            LXB_NS__UNDEF);

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, token->tag_id) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_tag_id(tree, token->tag_id,
                                                 LXB_NS_HTML, true);

    lxb_html_tree_active_formatting_up_to_last_marker(tree);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_table(lxb_html_tree_t *tree,
                                           lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    if (lxb_dom_interface_document(tree->document)->compat_mode
        != LXB_DOM_DOCUMENT_CMODE_QUIRKS)
    {
        node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                              LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
        if (node != NULL) {
            lxb_html_tree_close_p_element(tree, token);
        }
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->frameset_ok = false;
    tree->mode = lxb_html_tree_insertion_mode_in_table;

    return true;
}

/*
 * "area", "br", "embed", "img", "keygen", "wbr"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_abeikw(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    lxb_html_tree_open_elements_pop(tree);
    lxb_html_tree_acknowledge_token_self_closing(tree, token);

    tree->frameset_ok = false;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_br_closed(lxb_html_tree_t *tree,
                                               lxb_html_token_t *token)
{
    token->type ^= (token->type & LXB_HTML_TOKEN_TYPE_CLOSE);
    token->attr_first = NULL;
    token->attr_last = NULL;

    return lxb_html_tree_insertion_mode_in_body_abeikw(tree, token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_input(lxb_html_tree_t *tree,
                                           lxb_html_token_t *token)
{
    lxb_dom_attr_t *attr;
    lxb_html_element_t *element;

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    lxb_html_tree_open_elements_pop(tree);
    lxb_html_tree_acknowledge_token_self_closing(tree, token);

    attr = lxb_dom_element_attr_is_exist(lxb_dom_interface_element(element),
                                         (lxb_char_t *) "type", 4);
    if (attr != NULL) {
        if (attr->value == NULL || attr->value->length != 6
            || lexbor_str_data_cmp(attr->value->data, (lxb_char_t *) "hidden") == false)
        {
            tree->frameset_ok = false;
        }
    }
    else {
        tree->frameset_ok = false;
    }

    return true;
}

/*
 * "param", "source", "track"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_pst(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    lxb_html_tree_open_elements_pop(tree);
    lxb_html_tree_acknowledge_token_self_closing(tree, token);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_hr(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
    if (node != NULL) {
        lxb_html_tree_close_p_element(tree, token);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    lxb_html_tree_open_elements_pop(tree);
    lxb_html_tree_acknowledge_token_self_closing(tree, token);

    tree->frameset_ok = false;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_image(lxb_html_tree_t *tree,
                                           lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

    token->tag_id = LXB_TAG_IMG;

    return false;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_textarea(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    lxb_html_tokenizer_tmp_tag_id_set(tree->tkz_ref, LXB_TAG_TEXTAREA);
    lxb_html_tokenizer_state_set(tree->tkz_ref,
                                 lxb_html_tokenizer_state_rcdata_before);

    tree->frameset_ok = false;

    tree->original_mode = tree->mode;
    tree->mode = lxb_html_tree_insertion_mode_in_body_skip_new_line_textarea;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_xmp(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_P, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON);
    if (node != NULL) {
        lxb_html_tree_close_p_element(tree, token);
    }

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->frameset_ok = false;

    element = lxb_html_tree_generic_rawtext_parsing(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_iframe(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    tree->frameset_ok = false;

    element = lxb_html_tree_generic_rawtext_parsing(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_noembed(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    element = lxb_html_tree_generic_rawtext_parsing(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_select(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->frameset_ok = false;

    if (tree->mode == lxb_html_tree_insertion_mode_in_table
        || tree->mode == lxb_html_tree_insertion_mode_in_caption
        || tree->mode == lxb_html_tree_insertion_mode_in_table_body
        || tree->mode == lxb_html_tree_insertion_mode_in_row
        || tree->mode == lxb_html_tree_insertion_mode_in_cell)
    {
        tree->mode = lxb_html_tree_insertion_mode_in_select_in_table;
    }
    else {
        tree->mode = lxb_html_tree_insertion_mode_in_select;
    }

    return true;
}

/*
 * "optgroup", "option"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_optopt(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    node = lxb_html_tree_current_node(tree);
    if (lxb_html_tree_node_is(node, LXB_TAG_OPTION)) {
        lxb_html_tree_open_elements_pop(tree);
    }

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

/*
 * "rb", "rtc"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_rbrtc(lxb_html_tree_t *tree,
                                           lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_RUBY, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE);
    if (node != NULL) {
        lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG__UNDEF,
                                                LXB_NS__UNDEF);
    }

    node = lxb_html_tree_current_node(tree);
    if (lxb_html_tree_node_is(node, LXB_TAG_RUBY) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_MIELINOPELST);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

/*
 * "rp", "rt"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_rprt(lxb_html_tree_t *tree,
                                          lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_RUBY, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE);
    if (node != NULL) {
        lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG_RTC,
                                                LXB_NS_HTML);
    }

    node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, LXB_TAG_RTC) == false
        || lxb_html_tree_node_is(node, LXB_TAG_RUBY) == false)
    {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_MIELINOPELST);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_math(lxb_html_tree_t *tree,
                                          lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->before_append_attr = lxb_html_tree_adjust_attributes_mathml;

    element = lxb_html_tree_insert_foreign_element(tree, token, LXB_NS_MATH);
    if (element == NULL) {
        tree->before_append_attr = NULL;
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->before_append_attr = NULL;

    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE_SELF) {
        lxb_html_tree_open_elements_pop(tree);
        lxb_html_tree_acknowledge_token_self_closing(tree, token);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_svg(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->before_append_attr = lxb_html_tree_adjust_attributes_svg;

    element = lxb_html_tree_insert_foreign_element(tree, token, LXB_NS_SVG);
    if (element == NULL) {
        tree->before_append_attr = NULL;
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->before_append_attr = NULL;

    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE_SELF) {
        lxb_html_tree_open_elements_pop(tree);
        lxb_html_tree_acknowledge_token_self_closing(tree, token);
    }

    return true;
}

/*
 * "caption", "col", "colgroup", "frame", "head", "tbody", "td", "tfoot", "th",
 * "thead", "tr"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_body_cfht(lxb_html_tree_t *tree,
                                          lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token,
                              LXB_HTML_RULES_ERROR_UNTO);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_anything_else(lxb_html_tree_t *tree,
                                                   lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    tree->status = lxb_html_tree_active_formatting_reconstruct_elements(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_noscript(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    if (tree->document->dom_document.scripting == false) {
        return lxb_html_tree_insertion_mode_in_body_anything_else(tree, token);
    }

    lxb_html_element_t *element;

    element = lxb_html_tree_generic_rawtext_parsing(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_body_anything_else_closed(lxb_html_tree_t *tree,
                                                          lxb_html_token_t *token)
{
    bool is;
    lxb_dom_node_t **list = (lxb_dom_node_t **) tree->open_elements->list;
    size_t len = tree->open_elements->length;

    while (len != 0) {
        len--;

        if (lxb_html_tree_node_is(list[len], token->tag_id)) {
            lxb_html_tree_generate_implied_end_tags(tree, token->tag_id,
                                                    LXB_NS_HTML);

            if (list[len] != lxb_html_tree_current_node(tree)) {
                lxb_html_tree_parse_error(tree, token,
                                          LXB_HTML_RULES_ERROR_UNELINOPELST);
            }

            lxb_html_tree_open_elements_pop_until_node(tree, list[len], true);

            return true;
        }

        is = lxb_html_tag_is_category(list[len]->local_name, list[len]->ns,
                                      LXB_HTML_TAG_CATEGORY_SPECIAL);
        if (is) {
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_UNCLTO);
            return true;
        }
    }

    return true;
}


bool
lxb_html_tree_insertion_mode_in_body(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        switch (token->tag_id) {
            case LXB_TAG_TEMPLATE:
                return lxb_html_tree_insertion_mode_in_body_blmnst(tree, token);

            case LXB_TAG_BODY:
                return lxb_html_tree_insertion_mode_in_body_body_closed(tree,
                                                                        token);
            case LXB_TAG_HTML:
                return lxb_html_tree_insertion_mode_in_body_html_closed(tree,
                                                                        token);
            case LXB_TAG_ADDRESS:
            case LXB_TAG_ARTICLE:
            case LXB_TAG_ASIDE:
            case LXB_TAG_BLOCKQUOTE:
            case LXB_TAG_BUTTON:
            case LXB_TAG_CENTER:
            case LXB_TAG_DETAILS:
            case LXB_TAG_DIALOG:
            case LXB_TAG_DIR:
            case LXB_TAG_DIV:
            case LXB_TAG_DL:
            case LXB_TAG_FIELDSET:
            case LXB_TAG_FIGCAPTION:
            case LXB_TAG_FIGURE:
            case LXB_TAG_FOOTER:
            case LXB_TAG_HEADER:
            case LXB_TAG_HGROUP:
            case LXB_TAG_LISTING:
            case LXB_TAG_MAIN:
            case LXB_TAG_MENU:
            case LXB_TAG_NAV:
            case LXB_TAG_OL:
            case LXB_TAG_PRE:
            case LXB_TAG_SECTION:
            case LXB_TAG_SUMMARY:
            case LXB_TAG_UL:
                return lxb_html_tree_insertion_mode_in_body_abcdfhlmnopsu_closed(tree,
                                                                                 token);
            case LXB_TAG_FORM:
                return lxb_html_tree_insertion_mode_in_body_form_closed(tree,
                                                                        token);
            case LXB_TAG_P:
                return lxb_html_tree_insertion_mode_in_body_p_closed(tree,
                                                                     token);
            case LXB_TAG_LI:
                return lxb_html_tree_insertion_mode_in_body_li_closed(tree,
                                                                      token);
            case LXB_TAG_DD:
            case LXB_TAG_DT:
                return lxb_html_tree_insertion_mode_in_body_dd_dt_closed(tree,
                                                                         token);
            case LXB_TAG_H1:
            case LXB_TAG_H2:
            case LXB_TAG_H3:
            case LXB_TAG_H4:
            case LXB_TAG_H5:
            case LXB_TAG_H6:
                return lxb_html_tree_insertion_mode_in_body_h123456_closed(tree,
                                                                           token);
            case LXB_TAG_A:
            case LXB_TAG_B:
            case LXB_TAG_BIG:
            case LXB_TAG_CODE:
            case LXB_TAG_EM:
            case LXB_TAG_FONT:
            case LXB_TAG_I:
            case LXB_TAG_NOBR:
            case LXB_TAG_S:
            case LXB_TAG_SMALL:
            case LXB_TAG_STRIKE:
            case LXB_TAG_STRONG:
            case LXB_TAG_TT:
            case LXB_TAG_U:
                return lxb_html_tree_insertion_mode_in_body_abcefinstu_closed(tree,
                                                                              token);
            case LXB_TAG_APPLET:
            case LXB_TAG_MARQUEE:
            case LXB_TAG_OBJECT:
                return lxb_html_tree_insertion_mode_in_body_amo_closed(tree,
                                                                       token);
            case LXB_TAG_BR:
                return lxb_html_tree_insertion_mode_in_body_br_closed(tree,
                                                                      token);

            default:
                return lxb_html_tree_insertion_mode_in_body_anything_else_closed(tree,
                                                                                 token);
        }
    }

    switch (token->tag_id) {
        case LXB_TAG__TEXT:
            return lxb_html_tree_insertion_mode_in_body_text(tree, token);

        case LXB_TAG__EM_COMMENT:
            return lxb_html_tree_insertion_mode_in_body_comment(tree, token);

        case LXB_TAG__EM_DOCTYPE:
            return lxb_html_tree_insertion_mode_in_body_doctype(tree, token);

        case LXB_TAG_HTML:
            return lxb_html_tree_insertion_mode_in_body_html(tree, token);

        case LXB_TAG_BASE:
        case LXB_TAG_BASEFONT:
        case LXB_TAG_BGSOUND:
        case LXB_TAG_LINK:
        case LXB_TAG_META:
        case LXB_TAG_NOFRAMES:
        case LXB_TAG_SCRIPT:
        case LXB_TAG_STYLE:
        case LXB_TAG_TEMPLATE:
        case LXB_TAG_TITLE:
            return lxb_html_tree_insertion_mode_in_body_blmnst(tree, token);

        case LXB_TAG_BODY:
            return lxb_html_tree_insertion_mode_in_body_body(tree, token);

        case LXB_TAG_FRAMESET:
            return lxb_html_tree_insertion_mode_in_body_frameset(tree, token);

        case LXB_TAG__END_OF_FILE:
            return lxb_html_tree_insertion_mode_in_body_eof(tree, token);

        case LXB_TAG_ADDRESS:
        case LXB_TAG_ARTICLE:
        case LXB_TAG_ASIDE:
        case LXB_TAG_BLOCKQUOTE:
        case LXB_TAG_CENTER:
        case LXB_TAG_DETAILS:
        case LXB_TAG_DIALOG:
        case LXB_TAG_DIR:
        case LXB_TAG_DIV:
        case LXB_TAG_DL:
        case LXB_TAG_FIELDSET:
        case LXB_TAG_FIGCAPTION:
        case LXB_TAG_FIGURE:
        case LXB_TAG_FOOTER:
        case LXB_TAG_HEADER:
        case LXB_TAG_HGROUP:
        case LXB_TAG_MAIN:
        case LXB_TAG_MENU:
        case LXB_TAG_NAV:
        case LXB_TAG_OL:
        case LXB_TAG_P:
        case LXB_TAG_SECTION:
        case LXB_TAG_SUMMARY:
        case LXB_TAG_UL:
            return lxb_html_tree_insertion_mode_in_body_abcdfhmnopsu(tree,
                                                                     token);

        case LXB_TAG_H1:
        case LXB_TAG_H2:
        case LXB_TAG_H3:
        case LXB_TAG_H4:
        case LXB_TAG_H5:
        case LXB_TAG_H6:
            return lxb_html_tree_insertion_mode_in_body_h123456(tree, token);

        case LXB_TAG_PRE:
        case LXB_TAG_LISTING:
            return lxb_html_tree_insertion_mode_in_body_pre_listing(tree,
                                                                    token);

        case LXB_TAG_FORM:
            return lxb_html_tree_insertion_mode_in_body_form(tree, token);

        case LXB_TAG_LI:
            return lxb_html_tree_insertion_mode_in_body_li(tree, token);

        case LXB_TAG_DD:
        case LXB_TAG_DT:
            return lxb_html_tree_insertion_mode_in_body_dd_dt(tree, token);

        case LXB_TAG_PLAINTEXT:
            return lxb_html_tree_insertion_mode_in_body_plaintext(tree, token);

        case LXB_TAG_BUTTON:
            return lxb_html_tree_insertion_mode_in_body_button(tree, token);

        case LXB_TAG_A:
            return lxb_html_tree_insertion_mode_in_body_a(tree, token);

        case LXB_TAG_B:
        case LXB_TAG_BIG:
        case LXB_TAG_CODE:
        case LXB_TAG_EM:
        case LXB_TAG_FONT:
        case LXB_TAG_I:
        case LXB_TAG_S:
        case LXB_TAG_SMALL:
        case LXB_TAG_STRIKE:
        case LXB_TAG_STRONG:
        case LXB_TAG_TT:
        case LXB_TAG_U:
            return lxb_html_tree_insertion_mode_in_body_bcefistu(tree, token);

        case LXB_TAG_NOBR:
            return lxb_html_tree_insertion_mode_in_body_nobr(tree, token);

        case LXB_TAG_APPLET:
        case LXB_TAG_MARQUEE:
        case LXB_TAG_OBJECT:
            return lxb_html_tree_insertion_mode_in_body_amo(tree, token);

        case LXB_TAG_TABLE:
            return lxb_html_tree_insertion_mode_in_body_table(tree, token);

        case LXB_TAG_AREA:
        case LXB_TAG_BR:
        case LXB_TAG_EMBED:
        case LXB_TAG_IMG:
        case LXB_TAG_KEYGEN:
        case LXB_TAG_WBR:
            return lxb_html_tree_insertion_mode_in_body_abeikw(tree, token);

        case LXB_TAG_INPUT:
            return lxb_html_tree_insertion_mode_in_body_input(tree, token);

        case LXB_TAG_PARAM:
        case LXB_TAG_SOURCE:
        case LXB_TAG_TRACK:
            return lxb_html_tree_insertion_mode_in_body_pst(tree, token);

        case LXB_TAG_HR:
            return lxb_html_tree_insertion_mode_in_body_hr(tree, token);

        case LXB_TAG_IMAGE:
            return lxb_html_tree_insertion_mode_in_body_image(tree, token);

        case LXB_TAG_TEXTAREA:
            return lxb_html_tree_insertion_mode_in_body_textarea(tree, token);

        case LXB_TAG_XMP:
            return lxb_html_tree_insertion_mode_in_body_xmp(tree, token);

        case LXB_TAG_IFRAME:
            return lxb_html_tree_insertion_mode_in_body_iframe(tree, token);

        case LXB_TAG_NOEMBED:
            return lxb_html_tree_insertion_mode_in_body_noembed(tree, token);

        case LXB_TAG_NOSCRIPT:
            return lxb_html_tree_insertion_mode_in_body_noscript(tree, token);

        case LXB_TAG_SELECT:
            return lxb_html_tree_insertion_mode_in_body_select(tree, token);

        case LXB_TAG_OPTGROUP:
        case LXB_TAG_OPTION:
            return lxb_html_tree_insertion_mode_in_body_optopt(tree, token);

        case LXB_TAG_RB:
        case LXB_TAG_RTC:
            return lxb_html_tree_insertion_mode_in_body_rbrtc(tree, token);

        case LXB_TAG_RP:
        case LXB_TAG_RT:
            return lxb_html_tree_insertion_mode_in_body_rprt(tree, token);

        case LXB_TAG_MATH:
            return lxb_html_tree_insertion_mode_in_body_math(tree, token);

        case LXB_TAG_SVG:
            return lxb_html_tree_insertion_mode_in_body_svg(tree, token);

        case LXB_TAG_CAPTION:
        case LXB_TAG_COL:
        case LXB_TAG_COLGROUP:
        case LXB_TAG_FRAME:
        case LXB_TAG_HEAD:
        case LXB_TAG_TBODY:
        case LXB_TAG_TD:
        case LXB_TAG_TFOOT:
        case LXB_TAG_TH:
        case LXB_TAG_THEAD:
        case LXB_TAG_TR:
            return lxb_html_tree_insertion_mode_in_body_cfht(tree, token);

        default:
            return lxb_html_tree_insertion_mode_in_body_anything_else(tree,
                                                                      token);
    }
}
