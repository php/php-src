/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

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
