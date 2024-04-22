/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/interfaces/element.h"

#define LEXBOR_TOKENIZER_CHARS_MAP
#define LEXBOR_STR_RES_ANSI_REPLACEMENT_CHARACTER
#include "lexbor/core/str_res.h"


lxb_status_t
lxb_dom_element_qualified_name_set(lxb_dom_element_t *element,
                                   const lxb_char_t *prefix, size_t prefix_len,
                                   const lxb_char_t *lname, size_t lname_len);


lxb_inline bool
lxb_html_tree_insertion_mode_foreign_content_anything_else_closed(lxb_html_tree_t *tree,
                                                                  lxb_html_token_t *token)
{
    if (tree->open_elements->length == 0) {
        return tree->mode(tree, token);
    }

    lxb_dom_node_t **list = (lxb_dom_node_t **) tree->open_elements->list;

    size_t idx = tree->open_elements->length - 1;

    if (idx > 0 && list[idx]->local_name != token->tag_id) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINOPELST);
    }

    while (idx != 0) {
        if (list[idx]->local_name == token->tag_id) {
            lxb_html_tree_open_elements_pop_until_node(tree, list[idx], true);

            return true;
        }

        idx--;

        if (list[idx]->ns == LXB_NS_HTML) {
            break;
        }
    }

    return tree->mode(tree, token);
}

/*
 * TODO: Need to process script
 */
lxb_inline bool
lxb_html_tree_insertion_mode_foreign_content_script_closed(lxb_html_tree_t *tree,
                                                           lxb_html_token_t *token)
{
    lxb_dom_node_t *node = lxb_html_tree_current_node(tree);

    if (node->local_name != LXB_TAG_SCRIPT || node->ns != LXB_NS_SVG) {
        return lxb_html_tree_insertion_mode_foreign_content_anything_else_closed(tree,
                                                                                 token);
    }

    lxb_html_tree_open_elements_pop(tree);

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_foreign_content_anything_else(lxb_html_tree_t *tree,
                                                           lxb_html_token_t *token)
{
    lxb_html_element_t *element;
    const lxb_html_tag_fixname_t *fixname_svg;
    lxb_dom_node_t *node = lxb_html_tree_adjusted_current_node(tree);

    if (node->ns == LXB_NS_MATH) {
        tree->before_append_attr = lxb_html_tree_adjust_attributes_mathml;
    }
    else if (node->ns == LXB_NS_SVG) {
        tree->before_append_attr = lxb_html_tree_adjust_attributes_svg;
    }

    element = lxb_html_tree_insert_foreign_element(tree, token, node->ns);
    if (element == NULL) {
        tree->before_append_attr = NULL;
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    if (node->ns == LXB_NS_SVG) {
        fixname_svg = lxb_html_tag_fixname_svg(element->element.node.local_name);
        if (fixname_svg != NULL && fixname_svg->name != NULL) {
            lxb_dom_element_qualified_name_set(&element->element, NULL, 0,
                                               fixname_svg->name,
                                               (size_t) fixname_svg->len);
        }
    }

    tree->before_append_attr = NULL;

    if ((token->type & LXB_HTML_TOKEN_TYPE_CLOSE_SELF) == 0) {
        return true;
    }

    node = lxb_html_tree_current_node(tree);

    if (token->tag_id == LXB_TAG_SCRIPT && node->ns == LXB_NS_SVG) {
        lxb_html_tree_acknowledge_token_self_closing(tree, token);
        return lxb_html_tree_insertion_mode_foreign_content_script_closed(tree, token);
    }
    else {
        lxb_html_tree_open_elements_pop(tree);
        lxb_html_tree_acknowledge_token_self_closing(tree, token);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_foreign_content_text(lxb_html_tree_t *tree,
                                                  lxb_html_token_t *token)
{
    lexbor_str_t str;

    if (token->null_count != 0) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_NUCH);

        tree->status = lxb_html_token_make_text_replace_null(token, &str,
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

    if (tree->frameset_ok) {
        const lxb_char_t *pos = str.data;
        const lxb_char_t *end = str.data + str.length;

        static const lxb_char_t *rep = lexbor_str_res_ansi_replacement_character;
        static const unsigned rep_len = sizeof(lexbor_str_res_ansi_replacement_character) - 1;

        while (pos != end) {
            /* Need skip U+FFFD REPLACEMENT CHARACTER */
            if (*pos == *rep) {
                if ((end - pos) < rep_len) {
                    tree->frameset_ok = false;

                    break;
                }

                if (memcmp(pos, rep, sizeof(lxb_char_t) * rep_len) != 0) {
                    tree->frameset_ok = false;

                    break;
                }

                pos = pos + rep_len;

                continue;
            }

            if (lexbor_tokenizer_chars_map[*pos]
                != LEXBOR_STR_RES_MAP_CHAR_WHITESPACE)
            {
                tree->frameset_ok = false;

                break;
            }

            pos++;
        }
    }

    tree->status = lxb_html_tree_insert_character_for_data(tree, &str, NULL);
    if (tree->status != LXB_STATUS_OK) {
        return lxb_html_tree_process_abort(tree);
    }

    return true;
}

lxb_inline bool
lxb_html_tree_insertion_mode_foreign_content_comment(lxb_html_tree_t *tree,
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
lxb_html_tree_insertion_mode_foreign_content_doctype(lxb_html_tree_t *tree,
                                                     lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_DOTOFOCOMO);

    return true;
}

/*
 * "b", "big", "blockquote", "body", "br", "center", "code", "dd", "div", "dl",
 * "dt", "em", "embed", "h1", "h2", "h3", "h4", "h5", "h6", "head", "hr", "i",
 * "img", "li", "listing", "menu", "meta", "nobr", "ol", "p", "pre", "ruby",
 * "s", "small", "span", "strong", "strike", "sub", "sup", "table", "tt", "u",
 * "ul", "var"
 * "font", if the token has any attributes named "color", "face", or "size"
 */
lxb_inline bool
lxb_html_tree_insertion_mode_foreign_content_all(lxb_html_tree_t *tree,
                                                 lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    if (token->tag_id == LXB_TAG_FONT) {
        lxb_html_token_attr_t *attr = token->attr_first;

        while (attr != NULL) {
            if (attr->name != NULL
                && (attr->name->attr_id == LXB_DOM_ATTR_COLOR
                || attr->name->attr_id == LXB_DOM_ATTR_FACE
                || attr->name->attr_id == LXB_DOM_ATTR_SIZE))
            {
                goto go_next;
            }

            attr = attr->next;
        }

        return lxb_html_tree_insertion_mode_foreign_content_anything_else(tree,
                                                                          token);
    }

go_next:

    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

    if (tree->fragment != NULL) {
        return lxb_html_tree_insertion_mode_foreign_content_anything_else(tree,
                                                                          token);
    }

    do {
        lxb_html_tree_open_elements_pop(tree);

        node = lxb_html_tree_current_node(tree);
    }
    while (node &&
           !(lxb_html_tree_mathml_text_integration_point(node)
            || lxb_html_tree_html_integration_point(node)
            || node->ns == LXB_NS_HTML));

    return false;
}

bool
lxb_html_tree_insertion_mode_foreign_content(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token)
{
    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        switch (token->tag_id) {
            case LXB_TAG_SCRIPT:
                return lxb_html_tree_insertion_mode_foreign_content_script_closed(tree,
                                                                                  token);
            default:
                return lxb_html_tree_insertion_mode_foreign_content_anything_else_closed(tree,
                                                                                         token);
        }
    }

    switch (token->tag_id) {
        case LXB_TAG__TEXT:
            return lxb_html_tree_insertion_mode_foreign_content_text(tree,
                                                                     token);
        case LXB_TAG__EM_COMMENT:
            return lxb_html_tree_insertion_mode_foreign_content_comment(tree,
                                                                        token);
        case LXB_TAG__EM_DOCTYPE:
            return lxb_html_tree_insertion_mode_foreign_content_doctype(tree,
                                                                        token);

        case LXB_TAG_B:
        case LXB_TAG_BIG:
        case LXB_TAG_BLOCKQUOTE:
        case LXB_TAG_BODY:
        case LXB_TAG_BR:
        case LXB_TAG_CENTER:
        case LXB_TAG_CODE:
        case LXB_TAG_DD:
        case LXB_TAG_DIV:
        case LXB_TAG_DL:
        case LXB_TAG_DT:
        case LXB_TAG_EM:
        case LXB_TAG_EMBED:
        case LXB_TAG_H1:
        case LXB_TAG_H2:
        case LXB_TAG_H3:
        case LXB_TAG_H4:
        case LXB_TAG_H5:
        case LXB_TAG_H6:
        case LXB_TAG_HEAD:
        case LXB_TAG_HR:
        case LXB_TAG_I:
        case LXB_TAG_IMG:
        case LXB_TAG_LI:
        case LXB_TAG_LISTING:
        case LXB_TAG_MENU:
        case LXB_TAG_META:
        case LXB_TAG_NOBR:
        case LXB_TAG_OL:
        case LXB_TAG_P:
        case LXB_TAG_PRE:
        case LXB_TAG_RUBY:
        case LXB_TAG_S:
        case LXB_TAG_SMALL:
        case LXB_TAG_SPAN:
        case LXB_TAG_STRONG:
        case LXB_TAG_STRIKE:
        case LXB_TAG_SUB:
        case LXB_TAG_TABLE:
        case LXB_TAG_TT:
        case LXB_TAG_U:
        case LXB_TAG_UL:
        case LXB_TAG_VAR:
        case LXB_TAG_FONT:
            return lxb_html_tree_insertion_mode_foreign_content_all(tree,
                                                                    token);
        default:
            return lxb_html_tree_insertion_mode_foreign_content_anything_else(tree,
                                                                              token);
    }
}
