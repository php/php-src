/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/token_attr.h"


lxb_html_token_attr_t *
lxb_html_token_attr_create(lexbor_dobject_t *dobj)
{
    return lexbor_dobject_calloc(dobj);
}

void
lxb_html_token_attr_clean(lxb_html_token_attr_t *attr)
{
    memset(attr, 0, sizeof(lxb_html_token_attr_t));
}

lxb_html_token_attr_t *
lxb_html_token_attr_destroy(lxb_html_token_attr_t *attr, lexbor_dobject_t *dobj)
{
    return lexbor_dobject_free(dobj, attr);
}

const lxb_char_t *
lxb_html_token_attr_name(lxb_html_token_attr_t *attr, size_t *length)
{
    if (attr->name == NULL) {
        if (length != NULL) {
            *length = 0;
        }

        return NULL;
    }

    if (length != NULL) {
        *length = attr->name->entry.length;
    }

    return lexbor_hash_entry_str(&attr->name->entry);
}
