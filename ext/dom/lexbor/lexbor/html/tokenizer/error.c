/*
 * Copyright (C) 2018-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/str.h"
#include "lexbor/html/tokenizer/error.h"


lxb_html_tokenizer_error_t *
lxb_html_tokenizer_error_add(lexbor_array_obj_t *parse_errors,
                             const lxb_char_t *pos,
                             lxb_html_tokenizer_error_id_t id)
{
    if (parse_errors == NULL) {
        return NULL;
    }

    lxb_html_tokenizer_error_t *entry = lexbor_array_obj_push(parse_errors);
    if (entry == NULL) {
        return NULL;
    }

    entry->id = id;
    entry->pos = pos;

    return entry;
}

const lxb_char_t *
lxb_html_tokenizer_error_to_string(lxb_html_tokenizer_error_id_t id,
                                   size_t *len)
{
    static const lexbor_str_t unknown = lexbor_str("unknown error");

    static const lexbor_str_t errors[LXB_HTML_TOKENIZER_ERROR_LAST_ENTRY] = {
        lexbor_str("abrupt closing of empty comment"),
        lexbor_str("abrupt doctype public identifier"),
        lexbor_str("abrupt doctype system identifier"),
        lexbor_str("absence of digits in numeric character reference"),
        lexbor_str("cdata in html content"),
        lexbor_str("character reference outside unicode range"),
        lexbor_str("control character in input stream"),
        lexbor_str("control character reference"),
        lexbor_str("end tag with attributes"),
        lexbor_str("duplicate attribute"),
        lexbor_str("end tag with trailing solidus"),
        lexbor_str("eof before tag name"),
        lexbor_str("eof in cdata"),
        lexbor_str("eof in comment"),
        lexbor_str("eof in doctype"),
        lexbor_str("eof in script html comment like text"),
        lexbor_str("eof in tag"),
        lexbor_str("incorrectly closed comment"),
        lexbor_str("incorrectly opened comment"),
        lexbor_str("invalid character sequence after doctype name"),
        lexbor_str("invalid first character of tag name"),
        lexbor_str("missing attribute value"),
        lexbor_str("missing doctype name"),
        lexbor_str("missing doctype public identifier"),
        lexbor_str("missing doctype system identifier"),
        lexbor_str("missing end tag name"),
        lexbor_str("missing quote before doctype public identifier"),
        lexbor_str("missing quote before doctype system identifier"),
        lexbor_str("missing semicolon after character reference"),
        lexbor_str("missing whitespace after doctype public keyword"),
        lexbor_str("missing whitespace after doctype system keyword"),
        lexbor_str("missing whitespace before doctype name"),
        lexbor_str("missing whitespace between attributes"),
        lexbor_str("missing whitespace between doctype public and system identifiers"),
        lexbor_str("nested comment"),
        lexbor_str("noncharacter character reference"),
        lexbor_str("noncharacter in input stream"),
        lexbor_str("non void html element start tag with trailing solidus"),
        lexbor_str("null character reference"),
        lexbor_str("surrogate character reference"),
        lexbor_str("surrogate in input stream"),
        lexbor_str("unexpected character after doctype system identifier"),
        lexbor_str("unexpected character in attribute name"),
        lexbor_str("unexpected character in unquoted attribute value"),
        lexbor_str("unexpected equals sign before attribute name"),
        lexbor_str("unexpected null character"),
        lexbor_str("unexpected question mark instead of tag name"),
        lexbor_str("unexpected solidus in tag"),
        lexbor_str("unknown named character reference")
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
