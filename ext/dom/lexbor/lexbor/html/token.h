/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TOKEN_H
#define LEXBOR_HTML_TOKEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/dobject.h"
#include "lexbor/core/str.h"

#include "lexbor/html/base.h"
#include "lexbor/html/token_attr.h"
#include "lexbor/tag/tag.h"


typedef int lxb_html_token_type_t;


enum lxb_html_token_type {
    LXB_HTML_TOKEN_TYPE_OPEN         = 0x0000,
    LXB_HTML_TOKEN_TYPE_CLOSE        = 0x0001,
    LXB_HTML_TOKEN_TYPE_CLOSE_SELF   = 0x0002,
    LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS = 0x0004,
    LXB_HTML_TOKEN_TYPE_DONE         = 0x0008
};

typedef struct {
    const lxb_char_t      *begin;
    const lxb_char_t      *end;
    size_t                 line;
    size_t                 column;

    const lxb_char_t      *text_start;
    const lxb_char_t      *text_end;

    lxb_html_token_attr_t *attr_first;
    lxb_html_token_attr_t *attr_last;

    void                  *base_element;

    size_t                null_count;
    lxb_tag_id_t          tag_id;
    lxb_html_token_type_t type;
}
lxb_html_token_t;


LXB_API lxb_html_token_t *
lxb_html_token_create(lexbor_dobject_t *dobj);

LXB_API lxb_html_token_t *
lxb_html_token_destroy(lxb_html_token_t *token, lexbor_dobject_t *dobj);

LXB_API lxb_html_token_attr_t *
lxb_html_token_attr_append(lxb_html_token_t *token, lexbor_dobject_t *dobj);

LXB_API void
lxb_html_token_attr_remove(lxb_html_token_t *token,
                           lxb_html_token_attr_t *attr);

LXB_API void
lxb_html_token_attr_delete(lxb_html_token_t *token,
                           lxb_html_token_attr_t *attr, lexbor_dobject_t *dobj);

LXB_API lxb_status_t
lxb_html_token_make_text(lxb_html_token_t *token, lexbor_str_t *str,
                         lexbor_mraw_t *mraw);

LXB_API lxb_status_t
lxb_html_token_make_text_drop_null(lxb_html_token_t *token, lexbor_str_t *str,
                                   lexbor_mraw_t *mraw);

LXB_API lxb_status_t
lxb_html_token_make_text_replace_null(lxb_html_token_t *token,
                                      lexbor_str_t *str, lexbor_mraw_t *mraw);

LXB_API lxb_status_t
lxb_html_token_data_skip_ws_begin(lxb_html_token_t *token);

LXB_API lxb_status_t
lxb_html_token_data_skip_one_newline_begin(lxb_html_token_t *token);

LXB_API lxb_status_t
lxb_html_token_data_split_ws_begin(lxb_html_token_t *token,
                                   lxb_html_token_t *ws_token);

LXB_API lxb_status_t
lxb_html_token_doctype_parse(lxb_html_token_t *token,
                             lxb_dom_document_type_t *doc_type);

LXB_API lxb_html_token_attr_t *
lxb_html_token_find_attr(lxb_html_tokenizer_t *tkz, lxb_html_token_t *token,
                         const lxb_char_t *name, size_t name_len);


/*
 * Inline functions
 */
lxb_inline void
lxb_html_token_clean(lxb_html_token_t *token)
{
    memset(token, 0, sizeof(lxb_html_token_t));
}

lxb_inline lxb_html_token_t *
lxb_html_token_create_eof(lexbor_dobject_t *dobj)
{
    return (lxb_html_token_t *) lexbor_dobject_calloc(dobj);
}

/*
 * No inline functions for ABI.
 */
LXB_API void
lxb_html_token_clean_noi(lxb_html_token_t *token);

LXB_API lxb_html_token_t *
lxb_html_token_create_eof_noi(lexbor_dobject_t *dobj);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TOKEN_H */

