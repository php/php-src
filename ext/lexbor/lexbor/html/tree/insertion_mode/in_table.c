/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/tree/active_formatting.h"


lxb_inline void
lxb_html_tree_clear_stack_back_to_table_context(lxb_html_tree_t *tree)
{
    lxb_dom_node_t *current = lxb_html_tree_current_node(tree);

    while ((current->local_name != LXB_TAG_TABLE
            && current->local_name != LXB_TAG_TEMPLATE
            && current->local_name != LXB_TAG_HTML)
           || current->ns != LXB_NS_HTML)
    {
        lxb_html_tree_open_elements_pop(tree);
        current = lxb_html_tree_current_node(tree);
    }
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_text_open(lxb_html_tree_t *tree,
                                                lxb_html_token_t *token)
{
    lxb_dom_node_t *node = lxb_html_tree_current_node(tree);

    if (node->ns == LXB_NS_HTML &&
        (node->local_name == LXB_TAG_TABLE
         || node->local_name == LXB_TAG_TBODY
         || node->local_name == LXB_TAG_TFOOT
         || node->local_name == LXB_TAG_THEAD
         || node->local_name == LXB_TAG_TR))
    {
        tree->pending_table.text_list->length = 0;
        tree->pending_table.have_non_ws = false;

        tree->original_mode = tree->mode;
        tree->mode = lxb_html_tree_insertion_mode_in_table_text;

        return false;
    }

    return lxb_html_tree_insertion_mode_in_table_anything_else(tree, token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_comment(lxb_html_tree_t *tree,
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
lxb_html_tree_insertion_mode_in_table_doctype(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_DOTOINTAMO);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_caption(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    lxb_html_tree_clear_stack_back_to_table_context(tree);

    tree->status = lxb_html_tree_active_formatting_push_marker(tree);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_caption;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_colgroup(lxb_html_tree_t *tree,
                                               lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    lxb_html_tree_clear_stack_back_to_table_context(tree);

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_column_group;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_col(lxb_html_tree_t *tree,
                                          lxb_html_token_t *token)
{
    lxb_html_element_t *element;
    lxb_html_token_t fake_token = {0};

    lxb_html_tree_clear_stack_back_to_table_context(tree);

    fake_token.tag_id = LXB_TAG_COLGROUP;
    fake_token.attr_first = NULL;
    fake_token.attr_last = NULL;

    element = lxb_html_tree_insert_html_element(tree, &fake_token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_column_group;

    return false;
}

/*
 * "tbody", "tfoot", "thead"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_table_tbtfth(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    lxb_html_tree_clear_stack_back_to_table_context(tree);

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_table_body;

    return true;
}

/*
 * "td", "th", "tr"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_table_tdthtr(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token)
{
    lxb_html_element_t *element;
    lxb_html_token_t fake_token = {0};

    lxb_html_tree_clear_stack_back_to_table_context(tree);

    fake_token.tag_id = LXB_TAG_TBODY;
    fake_token.attr_first = NULL;
    fake_token.attr_last = NULL;

    element = lxb_html_tree_insert_html_element(tree, &fake_token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_table_body;

    return false;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_table(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_TABLE, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_TABLE);
    if (node == NULL) {
        return true;
    }

    lxb_html_tree_open_elements_pop_until_node(tree, node, true);
    lxb_html_tree_reset_insertion_mode_appropriately(tree);

    return false;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_table_closed(lxb_html_tree_t *tree,
                                                   lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, LXB_TAG_TABLE, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_TABLE);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

        return true;
    }

    lxb_html_tree_open_elements_pop_until_node(tree, node, true);
    lxb_html_tree_reset_insertion_mode_appropriately(tree);

    return true;
}

/*
 * "body", "caption", "col", "colgroup", "html", "tbody", "td", "tfoot", "th",
 * "thead", "tr"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_table_bcht_closed(lxb_html_tree_t *tree,
                                                  lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

    return true;
}

/*
 * A start tag whose tag name is one of: "style", "script", "template"
 * An end tag whose tag name is "template"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_in_table_st_open_closed(lxb_html_tree_t *tree,
                                                     lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_head(tree, token);
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_input(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token)
{
    lxb_html_element_t *element;
    lxb_html_token_attr_t *attr = token->attr_first;

    while (attr != NULL) {

        /* Name == "type" and value == "hidden" */
        if (attr->name != NULL && attr->name->attr_id == LXB_DOM_ATTR_TYPE) {
            if (attr->value_size == 6
                && lexbor_str_data_ncasecmp(attr->value,
                                            (const lxb_char_t *) "hidden", 6))
            {
                goto have_hidden;
            }
        }

        attr = attr->next;
    }

    return lxb_html_tree_insertion_mode_in_table_anything_else(tree, token);

have_hidden:

    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    lxb_html_tree_open_elements_pop_until_node(tree,
                                               lxb_dom_interface_node(element),
                                               true);

    lxb_html_tree_acknowledge_token_self_closing(tree, token);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_form(lxb_html_tree_t *tree,
                                           lxb_html_token_t *token)
{
    lxb_dom_node_t *node;
    lxb_html_element_t *element;

    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

    if (tree->form != NULL) {
        return true;
    }

    node = lxb_html_tree_open_elements_find_reverse(tree, LXB_TAG_TEMPLATE,
                                                    LXB_NS_HTML, NULL);
    if (node != NULL) {
        return true;
    }

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->form = lxb_html_interface_form(element);

    lxb_html_tree_open_elements_pop_until_node(tree,
                                               lxb_dom_interface_node(element),
                                               true);
    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_end_of_file(lxb_html_tree_t *tree,
                                                  lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_body(tree, token);
}

bool
lxb_html_tree_insertion_mode_in_table_anything_else(lxb_html_tree_t *tree,
                                                    lxb_html_token_t *token)
{
    tree->foster_parenting = true;

    lxb_html_tree_insertion_mode_in_body(tree, token);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    tree->foster_parenting = false;

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_in_table_anything_else_closed(lxb_html_tree_t *tree,
                                                           lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_table_anything_else(tree, token);
}

bool
lxb_html_tree_insertion_mode_in_table(lxb_html_tree_t *tree,
                                      lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        switch (token->tag_id) {
            case LXB_TAG_TABLE:
                return lxb_html_tree_insertion_mode_in_table_table_closed(tree,
                                                                          token);
            case LXB_TAG_BODY:
            case LXB_TAG_CAPTION:
            case LXB_TAG_COL:
            case LXB_TAG_COLGROUP:
            case LXB_TAG_HTML:
            case LXB_TAG_TBODY:
            case LXB_TAG_TD:
            case LXB_TAG_TFOOT:
            case LXB_TAG_TH:
            case LXB_TAG_THEAD:
            case LXB_TAG_TR:
                return lxb_html_tree_insertion_mode_in_table_bcht_closed(tree,
                                                                         token);
            case LXB_TAG_TEMPLATE:
                return lxb_html_tree_insertion_mode_in_table_st_open_closed(tree,
                                                                            token);
            default:
                return lxb_html_tree_insertion_mode_in_table_anything_else_closed(tree,
                                                                                  token);
        }
    }

    switch (token->tag_id) {
        case LXB_TAG__TEXT:
            return lxb_html_tree_insertion_mode_in_table_text_open(tree, token);

        case LXB_TAG__EM_COMMENT:
            return lxb_html_tree_insertion_mode_in_table_comment(tree, token);

        case LXB_TAG__EM_DOCTYPE:
            return lxb_html_tree_insertion_mode_in_table_doctype(tree, token);

        case LXB_TAG_CAPTION:
            return lxb_html_tree_insertion_mode_in_table_caption(tree, token);

        case LXB_TAG_COLGROUP:
            return lxb_html_tree_insertion_mode_in_table_colgroup(tree, token);

        case LXB_TAG_COL:
            return lxb_html_tree_insertion_mode_in_table_col(tree, token);

        case LXB_TAG_TBODY:
        case LXB_TAG_TFOOT:
        case LXB_TAG_THEAD:
            return lxb_html_tree_insertion_mode_in_table_tbtfth(tree, token);

        case LXB_TAG_TD:
        case LXB_TAG_TH:
        case LXB_TAG_TR:
            return lxb_html_tree_insertion_mode_in_table_tdthtr(tree, token);

        case LXB_TAG_TABLE:
            return lxb_html_tree_insertion_mode_in_table_table(tree, token);

        case LXB_TAG_STYLE:
        case LXB_TAG_SCRIPT:
        case LXB_TAG_TEMPLATE:
            return lxb_html_tree_insertion_mode_in_table_st_open_closed(tree,
                                                                        token);
        case LXB_TAG_INPUT:
            return lxb_html_tree_insertion_mode_in_table_input(tree, token);

        case LXB_TAG_FORM:
            return lxb_html_tree_insertion_mode_in_table_form(tree, token);

        case LXB_TAG__END_OF_FILE:
            return lxb_html_tree_insertion_mode_in_table_end_of_file(tree,
                                                                     token);
        default:
            return lxb_html_tree_insertion_mode_in_table_anything_else(tree,
                                                                       token);
    }
}
