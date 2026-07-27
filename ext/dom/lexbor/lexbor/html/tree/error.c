/*
 * Copyright (C) 2018-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/str.h"
#include "lexbor/html/tree/error.h"


lxb_html_tree_error_t *
lxb_html_tree_error_add(lexbor_array_obj_t *parse_errors,
                        lxb_html_token_t *token, lxb_html_tree_error_id_t id)
{
    if (parse_errors == NULL) {
        return NULL;
    }

    lxb_html_tree_error_t *entry = lexbor_array_obj_push(parse_errors);
    if (entry == NULL) {
        return NULL;
    }

    entry->id = id;
    entry->line = token->line;
    entry->column = token->column;
    entry->length = token->end - token->begin;

    return entry;
}

const lxb_char_t *
lxb_html_tree_error_to_string(lxb_html_tree_error_id_t id, size_t *len)
{
    static const lexbor_str_t unknown = lexbor_str("unknown error");

    static const lexbor_str_t errors[LXB_HTML_RULES_ERROR_LAST_ENTRY] = {
        lexbor_str("unexpected token"),
        lexbor_str("unexpected closed token"),
        lexbor_str("null character"),
        lexbor_str("unexpected character token"),
        lexbor_str("unexpected token in initial mode"),
        lexbor_str("bad doctype token in initial mode"),
        lexbor_str("doctype token in before html mode"),
        lexbor_str("unexpected closed token in before html mode"),
        lexbor_str("doctype token in before head mode"),
        lexbor_str("unexpected closed token in before head mode"),
        lexbor_str("doctype token in head mode"),
        lexbor_str("non void html element start tag with trailing solidus"),
        lexbor_str("head token in head mode"),
        lexbor_str("unexpected closed token in head mode"),
        lexbor_str("template closed token without opening in head mode"),
        lexbor_str("template element is not current in head mode"),
        lexbor_str("doctype token in head noscript mode"),
        lexbor_str("doctype token after head mode"),
        lexbor_str("head token after head mode"),
        lexbor_str("doctype token in body mode"),
        lexbor_str("bad ending open elements is wrong"),
        lexbor_str("open elements is wrong"),
        lexbor_str("unexpected element in open elements stack"),
        lexbor_str("missing element in open elements stack"),
        lexbor_str("no body element in scope"),
        lexbor_str("missing element in scope"),
        lexbor_str("unexpected element in scope"),
        lexbor_str("unexpected element in active formatting stack"),
        lexbor_str("unexpected end of file"),
        lexbor_str("characters in table text"),
        lexbor_str("doctype token in table mode"),
        lexbor_str("doctype token in select mode"),
        lexbor_str("doctype token after body mode"),
        lexbor_str("doctype token in frameset mode"),
        lexbor_str("doctype token after frameset mode"),
        lexbor_str("doctype token foreign content mode"),
        lexbor_str("select in scope"),
        lexbor_str("fragment parsing select in context parse input"),
        lexbor_str("fragment parsing select in context parse select"),
        lexbor_str("hr parsing select option optgroup in scope"),
        lexbor_str("option parsing option in scope"),
        lexbor_str("optgroup parsing option optgroup in scope")
    };

    if (id >= (sizeof(errors) / sizeof(lexbor_str_t))) {
        if (len != NULL) {
            *len = unknown.length;
        }

        return unknown.data;
    }

    if (len != NULL) {
        *len = errors[id].length;
    }

    return errors[id].data;
}
