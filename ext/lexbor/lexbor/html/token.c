/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/token.h"
#include "lexbor/html/tokenizer.h"

#define LEXBOR_STR_RES_MAP_LOWERCASE
#define LEXBOR_STR_RES_ANSI_REPLACEMENT_CHARACTER
#define LEXBOR_STR_RES_MAP_HEX
#define LEXBOR_STR_RES_MAP_NUM
#include "lexbor/core/str_res.h"

#include "lexbor/dom/interfaces/document_type.h"


const lxb_tag_data_t *
lxb_tag_append_lower(lexbor_hash_t *hash,
                     const lxb_char_t *name, size_t length);


lxb_html_token_t *
lxb_html_token_create(lexbor_dobject_t *dobj)
{
    return lexbor_dobject_calloc(dobj);
}

lxb_html_token_t *
lxb_html_token_destroy(lxb_html_token_t *token, lexbor_dobject_t *dobj)
{
    return lexbor_dobject_free(dobj, token);
}

lxb_html_token_attr_t *
lxb_html_token_attr_append(lxb_html_token_t *token, lexbor_dobject_t *dobj)
{
    lxb_html_token_attr_t *attr = lxb_html_token_attr_create(dobj);
    if (attr == NULL) {
        return NULL;
    }

    if (token->attr_last == NULL) {
        token->attr_first = attr;
        token->attr_last = attr;

        return attr;
    }

    token->attr_last->next = attr;
    attr->prev = token->attr_last;

    token->attr_last = attr;

    return attr;
}

void
lxb_html_token_attr_remove(lxb_html_token_t *token, lxb_html_token_attr_t *attr)
{
    if (token->attr_first == attr) {
        token->attr_first = attr->next;
    }

    if (token->attr_last == attr) {
        token->attr_last = attr->prev;
    }

    if (attr->next != NULL) {
        attr->next->prev = attr->prev;
    }

    if (attr->prev != NULL) {
        attr->prev->next = attr->next;
    }

    attr->next = NULL;
    attr->prev = NULL;
}

void
lxb_html_token_attr_delete(lxb_html_token_t *token,
                           lxb_html_token_attr_t *attr, lexbor_dobject_t *dobj)
{
    lxb_html_token_attr_remove(token, attr);
    lxb_html_token_attr_destroy(attr, dobj);
}

lxb_status_t
lxb_html_token_make_text(lxb_html_token_t *token, lexbor_str_t *str,
                         lexbor_mraw_t *mraw)
{
    size_t len = token->text_end - token->text_start;

    (void) lexbor_str_init(str, mraw, len);
    if (str->data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    memcpy(str->data, token->text_start, len);

    str->data[len] = 0x00;
    str->length = len;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_token_make_text_drop_null(lxb_html_token_t *token, lexbor_str_t *str,
                                   lexbor_mraw_t *mraw)
{
    lxb_char_t *p, c;
    const lxb_char_t *data = token->text_start;
    const lxb_char_t *end = token->text_end;

    size_t len = (end - data) - token->null_count;

    (void) lexbor_str_init(str, mraw, len);
    if (str->data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    p = str->data;

    while (data < end) {
        c = *data++;

        if (c != 0x00) {
            *p++ = c;
        }
    }

    str->data[len] = 0x00;
    str->length = len;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_token_make_text_replace_null(lxb_html_token_t *token,
                                      lexbor_str_t *str, lexbor_mraw_t *mraw)
{
    lxb_char_t *p, c;
    const lxb_char_t *data = token->text_start;
    const lxb_char_t *end = token->text_end;

    static const unsigned rep_len = sizeof(lexbor_str_res_ansi_replacement_character) - 1;

    size_t len = (end - data) + (token->null_count * rep_len) - token->null_count;

    (void) lexbor_str_init(str, mraw, len);
    if (str->data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    p = str->data;

    while (data < end) {
        c = *data++;

        if (c == 0x00) {
            memcpy(p, lexbor_str_res_ansi_replacement_character, rep_len);
            p += rep_len;

            continue;
        }

        *p++ = c;
    }

    str->data[len] = 0x00;
    str->length = len;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_token_data_skip_ws_begin(lxb_html_token_t *token)
{
    const lxb_char_t *data = token->text_start;
    const lxb_char_t *end = token->text_end;

    while (data < end) {
        switch (*data) {
            /*
             * U+0009 CHARACTER TABULATION (tab)
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+0020 SPACE
             */
            case 0x09:
            case 0x0A:
            case 0x0D:
            case 0x20:
                break;

            default:
                token->begin += data - token->text_start;
                token->text_start = data;

                return LXB_STATUS_OK;
        }

        data++;
    }

    token->begin += data - token->text_start;
    token->text_start = data;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_token_data_skip_one_newline_begin(lxb_html_token_t *token)
{
    const lxb_char_t *data = token->text_start;
    const lxb_char_t *end = token->text_end;

    if (data < end) {
        /* U+000A LINE FEED (LF) */
        if (*data == 0x0A) {
            token->begin++;
            token->text_start++;
        }
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_token_data_split_ws_begin(lxb_html_token_t *token,
                                   lxb_html_token_t *ws_token)
{
    *ws_token = *token;

    lxb_status_t status = lxb_html_token_data_skip_ws_begin(token);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (token->text_start == token->text_end) {
        return LXB_STATUS_OK;
    }

    if (token->text_start == ws_token->text_start) {
        memset(ws_token, 0, sizeof(lxb_html_token_t));

        return LXB_STATUS_OK;
    }

    ws_token->end = token->begin;
    ws_token->text_end = token->text_start;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_token_doctype_parse(lxb_html_token_t *token,
                             lxb_dom_document_type_t *doc_type)
{
    lxb_html_token_attr_t *attr;
    lexbor_mraw_t *mraw = doc_type->node.owner_document->mraw;

    /* Set all to empty string if attr not exist */
    if (token->attr_first == NULL) {
        goto set_name_pub_sys_empty;
    }

    /* Name */
    attr = token->attr_first;

    doc_type->name = attr->name->attr_id;

    /* PUBLIC or SYSTEM */
    attr = attr->next;
    if (attr == NULL) {
        goto set_pub_sys_empty;
    }

    if (attr->name->attr_id == LXB_DOM_ATTR_PUBLIC) {
        (void) lexbor_str_init(&doc_type->public_id, mraw, attr->value_size);
        if (doc_type->public_id.data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        if (attr->value_begin == NULL) {
            return LXB_STATUS_OK;
        }

        (void) lexbor_str_append(&doc_type->public_id, mraw, attr->value,
                                 attr->value_size);
    }
    else if (attr->name->attr_id == LXB_DOM_ATTR_SYSTEM) {
        (void) lexbor_str_init(&doc_type->system_id, mraw, attr->value_size);
        if (doc_type->system_id.data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        if (attr->value_begin == NULL) {
            return LXB_STATUS_OK;
        }

        (void) lexbor_str_append(&doc_type->system_id, mraw, attr->value,
                                 attr->value_size);

        return LXB_STATUS_OK;
    }
    else {
        goto set_pub_sys_empty;
    }

    /* SUSTEM */
    attr = attr->next;
    if (attr == NULL) {
        goto set_sys_empty;
    }

    (void) lexbor_str_init(&doc_type->system_id, mraw, attr->value_size);
    if (doc_type->system_id.data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    (void) lexbor_str_append(&doc_type->system_id, mraw, attr->value,
                             attr->value_size);

    return LXB_STATUS_OK;

set_name_pub_sys_empty:

    doc_type->name = LXB_DOM_ATTR__UNDEF;

set_pub_sys_empty:

    (void) lexbor_str_init(&doc_type->public_id, mraw, 0);
    if (doc_type->public_id.data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

set_sys_empty:

    (void) lexbor_str_init(&doc_type->system_id, mraw, 0);
    if (doc_type->system_id.data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_HTML_STATUS_OK;
}

lxb_html_token_attr_t *
lxb_html_token_find_attr(lxb_html_tokenizer_t *tkz, lxb_html_token_t *token,
                         const lxb_char_t *name, size_t name_len)
{
    const lxb_dom_attr_data_t *data;
    lxb_html_token_attr_t *attr = token->attr_first;

    data = lxb_dom_attr_data_by_local_name(tkz->attrs, name, name_len);
    if (data == NULL) {
        return NULL;
    }

    while (attr != NULL) {
        if (attr->name->attr_id == data->attr_id) {
            return attr;
        }

        attr = attr->next;
    }

    return NULL;
}

/*
 * No inline functions for ABI.
 */
void
lxb_html_token_clean_noi(lxb_html_token_t *token)
{
    lxb_html_token_clean(token);
}

lxb_html_token_t *
lxb_html_token_create_eof_noi(lexbor_dobject_t *dobj)
{
    return lxb_html_token_create_eof(dobj);
}
