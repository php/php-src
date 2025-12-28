/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/syntax/tokenizer/error.h"


lxb_css_syntax_tokenizer_error_t *
lxb_css_syntax_tokenizer_error_add(lexbor_array_obj_t *parse_errors,
                                   const lxb_char_t *pos,
                                   lxb_css_syntax_tokenizer_error_id_t id)
{
    if (parse_errors == NULL) {
        return NULL;
    }

    lxb_css_syntax_tokenizer_error_t *entry;

    entry = lexbor_array_obj_push(parse_errors);
    if (entry == NULL) {
        return NULL;
    }

    entry->id = id;
    entry->pos = pos;

    return entry;
}
