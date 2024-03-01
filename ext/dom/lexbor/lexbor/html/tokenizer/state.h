/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TOKENIZER_STATE_H
#define LEXBOR_HTML_TOKENIZER_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/tokenizer.h"

#define lxb_html_tokenizer_state_begin_set(tkz, v_data)                        \
    (tkz->begin = v_data)

#define lxb_html_tokenizer_state_append_data_m(tkz, v_data)                    \
    do {                                                                       \
        if (lxb_html_tokenizer_temp_append_data(tkz, v_data)) {                \
            return end;                                                        \
        }                                                                      \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_append_m(tkz, v_data, size)                   \
    do {                                                                       \
        if (lxb_html_tokenizer_temp_append(tkz, (const lxb_char_t *) (v_data), \
                                           (size)))                            \
        {                                                                      \
            return end;                                                        \
        }                                                                      \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_append_replace_m(tkz)                         \
    do {                                                                       \
        if (lxb_html_tokenizer_temp_append(tkz,                                \
                        lexbor_str_res_ansi_replacement_character,             \
                        sizeof(lexbor_str_res_ansi_replacement_character) - 1))\
        {                                                                      \
            return end;                                                        \
        }                                                                      \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_set_tag_m(tkz, _start, _end)                  \
    do {                                                                       \
        const lxb_tag_data_t *tag;                                             \
        tag = lxb_tag_append_lower(tkz->tags, (_start), (_end) - (_start));    \
        if (tag == NULL) {                                                     \
            tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;                  \
            return end;                                                        \
        }                                                                      \
        tkz->token->tag_id = tag->tag_id;                                      \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_set_name_m(tkz)                               \
    do {                                                                       \
        lxb_dom_attr_data_t *data;                                             \
        data = lxb_dom_attr_local_name_append(tkz->attrs, tkz->start,          \
                                              tkz->pos - tkz->start);          \
        if (data == NULL) {                                                    \
            tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;                  \
            return end;                                                        \
        }                                                                      \
        tkz->token->attr_last->name = data;                                    \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_set_value_m(tkz)                              \
    do {                                                                       \
        lxb_html_token_attr_t *attr = tkz->token->attr_last;                   \
                                                                               \
        attr->value_size = (size_t) (tkz->pos - tkz->start);                   \
                                                                               \
        attr->value = lexbor_mraw_alloc(tkz->attrs_mraw, attr->value_size + 1);\
        if (attr->value == NULL) {                                             \
            tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;                  \
            return end;                                                        \
        }                                                                      \
        memcpy(attr->value, tkz->start, attr->value_size);                     \
        attr->value[attr->value_size] = 0x00;                                  \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_token_set_begin(tkz, v_begin)                 \
    do {                                                                       \
        tkz->pos = tkz->start;                                                 \
        tkz->token->begin = v_begin;                                           \
        tkz->token->line = tkz->current_line;                                  \
        tkz->token->column = tkz->current_column;                              \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_token_set_end(tkz, v_end)                     \
    (tkz->token->end = v_end)

#define lxb_html_tokenizer_state_token_set_end_down(tkz, v_end, offset)        \
    do {                                                                       \
        tkz->token->end = lexbor_in_node_pos_down(tkz->incoming_node, NULL,    \
                                                  v_end, offset);              \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_token_set_end_oef(tkz)                        \
    (tkz->token->end = tkz->last)

#define lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, v_return)         \
    do {                                                                       \
        attr = lxb_html_token_attr_append(tkz->token, tkz->dobj_token_attr);   \
        if (attr == NULL) {                                                    \
            tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;                  \
            return v_return;                                                   \
        }                                                                      \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_token_attr_set_name_begin(tkz, v_begin)       \
    do {                                                                       \
        tkz->pos = tkz->start;                                                 \
        tkz->token->attr_last->name_begin = v_begin;                           \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_token_attr_set_name_end(tkz, v_end)           \
    (tkz->token->attr_last->name_end = v_end)

#define lxb_html_tokenizer_state_token_attr_set_name_end_oef(tkz)              \
    (tkz->token->attr_last->name_end = tkz->last)

#define lxb_html_tokenizer_state_token_attr_set_value_begin(tkz, v_begin)      \
    do {                                                                       \
        tkz->pos = tkz->start;                                                 \
        tkz->token->attr_last->value_begin = v_begin;                          \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_token_attr_set_value_end(tkz, v_end)          \
    (tkz->token->attr_last->value_end = v_end)

#define lxb_html_tokenizer_state_token_attr_set_value_end_oef(tkz)             \
    (tkz->token->attr_last->value_end = tkz->last)

#define _lxb_html_tokenizer_state_token_done_m(tkz, v_end)                     \
    tkz->token = tkz->callback_token_done(tkz, tkz->token,                     \
                                          tkz->callback_token_ctx);            \
    if (tkz->token == NULL) {                                                  \
        if (tkz->status == LXB_STATUS_OK) {                                    \
            tkz->status = LXB_STATUS_ERROR;                                    \
        }                                                                      \
        return v_end;                                                          \
    }

#define lxb_html_tokenizer_state_token_done_m(tkz, v_end)                      \
    do {                                                                       \
        if (tkz->token->begin != tkz->token->end) {                            \
            _lxb_html_tokenizer_state_token_done_m(tkz, v_end)                 \
        }                                                                      \
        lxb_html_token_clean(tkz->token);                                      \
        tkz->pos = tkz->start;                                                 \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_token_done_wo_check_m(tkz, v_end)             \
    do {                                                                       \
        _lxb_html_tokenizer_state_token_done_m(tkz, v_end)                     \
        lxb_html_token_clean(tkz->token);                                      \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_set_text(tkz)                                 \
    do {                                                                       \
        tkz->token->text_start = tkz->start;                                   \
        tkz->token->text_end = tkz->pos;                                       \
    }                                                                          \
    while (0)

#define lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, v_end)       \
    do {                                                                       \
        if (tkz->token->begin != tkz->token->end) {                            \
            tkz->token->tag_id = LXB_TAG__TEXT;                                \
                                                                               \
            lxb_html_tokenizer_state_set_text(tkz);                            \
            _lxb_html_tokenizer_state_token_done_m(tkz, v_end)                 \
            lxb_html_token_clean(tkz->token);                                  \
        }                                                                      \
    }                                                                          \
    while (0)


LXB_API const lxb_char_t *
lxb_html_tokenizer_state_data_before(lxb_html_tokenizer_t *tkz,
                                     const lxb_char_t *data,
                                     const lxb_char_t *end);

LXB_API const lxb_char_t *
lxb_html_tokenizer_state_plaintext_before(lxb_html_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end);

LXB_API const lxb_char_t *
lxb_html_tokenizer_state_before_attribute_name(lxb_html_tokenizer_t *tkz,
                                               const lxb_char_t *data,
                                               const lxb_char_t *end);

LXB_API const lxb_char_t *
lxb_html_tokenizer_state_self_closing_start_tag(lxb_html_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end);

LXB_API const lxb_char_t *
lxb_html_tokenizer_state_cr(lxb_html_tokenizer_t *tkz, const lxb_char_t *data,
                            const lxb_char_t *end);

LXB_API const lxb_char_t *
lxb_html_tokenizer_state_char_ref(lxb_html_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TOKENIZER_STATE_H */
