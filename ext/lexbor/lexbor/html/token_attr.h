/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TOKEN_ATTR_H
#define LEXBOR_HTML_TOKEN_ATTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/str.h"
#include "lexbor/core/dobject.h"

#include "lexbor/dom/interfaces/attr.h"

#include "lexbor/html/base.h"


typedef struct lxb_html_token_attr lxb_html_token_attr_t;
typedef int lxb_html_token_attr_type_t;

enum lxb_html_token_attr_type {
    LXB_HTML_TOKEN_ATTR_TYPE_UNDEF      = 0x0000,
    LXB_HTML_TOKEN_ATTR_TYPE_NAME_NULL  = 0x0001,
    LXB_HTML_TOKEN_ATTR_TYPE_VALUE_NULL = 0x0002
};

struct lxb_html_token_attr {
    const lxb_char_t           *name_begin;
    const lxb_char_t           *name_end;

    const lxb_char_t           *value_begin;
    const lxb_char_t           *value_end;

    const lxb_dom_attr_data_t  *name;
    lxb_char_t                 *value;
    size_t                     value_size;

    lxb_html_token_attr_t      *next;
    lxb_html_token_attr_t      *prev;

    lxb_html_token_attr_type_t type;
};


LXB_API lxb_html_token_attr_t *
lxb_html_token_attr_create(lexbor_dobject_t *dobj);

LXB_API void
lxb_html_token_attr_clean(lxb_html_token_attr_t *attr);

LXB_API lxb_html_token_attr_t *
lxb_html_token_attr_destroy(lxb_html_token_attr_t *attr,
                            lexbor_dobject_t *dobj);

LXB_API const lxb_char_t *
lxb_html_token_attr_name(lxb_html_token_attr_t *attr, size_t *length);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TOKEN_ATTR_H */
