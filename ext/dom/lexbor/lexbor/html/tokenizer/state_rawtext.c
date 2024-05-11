/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tokenizer/state_rawtext.h"
#include "lexbor/html/tokenizer/state.h"

#define LEXBOR_STR_RES_ANSI_REPLACEMENT_CHARACTER
#define LEXBOR_STR_RES_ALPHA_CHARACTER
#include "lexbor/core/str_res.h"


const lxb_tag_data_t *
lxb_tag_append_lower(lexbor_hash_t *hash,
                     const lxb_char_t *name, size_t length);


static const lxb_char_t *
lxb_html_tokenizer_state_rawtext(lxb_html_tokenizer_t *tkz,
                                const lxb_char_t *data,
                                const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_rawtext_less_than_sign(lxb_html_tokenizer_t *tkz,
                                               const lxb_char_t *data,
                                               const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_rawtext_end_tag_open(lxb_html_tokenizer_t *tkz,
                                             const lxb_char_t *data,
                                             const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_rawtext_end_tag_name(lxb_html_tokenizer_t *tkz,
                                             const lxb_char_t *data,
                                             const lxb_char_t *end);


/*
 * Helper function. No in the specification. For 12.2.5.3 RAWTEXT state
 */
const lxb_char_t *
lxb_html_tokenizer_state_rawtext_before(lxb_html_tokenizer_t *tkz,
                                        const lxb_char_t *data,
                                        const lxb_char_t *end)
{
    if (tkz->is_eof == false) {
        lxb_html_tokenizer_state_token_set_begin(tkz, data);
    }

    tkz->state = lxb_html_tokenizer_state_rawtext;

    return data;
}

/*
 * 12.2.5.3 RAWTEXT state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_rawtext(lxb_html_tokenizer_t *tkz,
                                 const lxb_char_t *data,
                                 const lxb_char_t *end)
{
    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /* U+003C LESS-THAN SIGN (<) */
            case 0x3C:
                lxb_html_tokenizer_state_append_data_m(tkz, data + 1);
                lxb_html_tokenizer_state_token_set_end(tkz, data);

                tkz->state = lxb_html_tokenizer_state_rawtext_less_than_sign;

                return (data + 1);

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_rawtext;

                    return data;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                tkz->pos[-1] = 0x0A;

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);

                if (*data != 0x0A) {
                    lxb_html_tokenizer_state_begin_set(tkz, data);
                    data--;
                }

                break;

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                lxb_html_tokenizer_state_append_data_m(tkz, data);

                if (tkz->is_eof) {
                    if (tkz->token->begin != NULL) {
                        lxb_html_tokenizer_state_token_set_end_oef(tkz);
                    }

                    tkz->token->tag_id = LXB_TAG__TEXT;

                    lxb_html_tokenizer_state_set_text(tkz);
                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);
                break;

            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.12 RAWTEXT less-than sign state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_rawtext_less_than_sign(lxb_html_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end)
{
    /* U+002F SOLIDUS (/) */
    if (*data == 0x2F) {
        tkz->state = lxb_html_tokenizer_state_rawtext_end_tag_open;

        return (data + 1);
    }

    tkz->state = lxb_html_tokenizer_state_rawtext;

    return data;
}

/*
 * 12.2.5.13 RAWTEXT end tag open state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_rawtext_end_tag_open(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end)
{
    if (lexbor_str_res_alpha_character[*data] != LEXBOR_STR_RES_SLIP) {
        tkz->temp = data;
        tkz->entity_start = (tkz->pos - 1) - tkz->start;

        tkz->state = lxb_html_tokenizer_state_rawtext_end_tag_name;
    }
    else {
        tkz->state = lxb_html_tokenizer_state_rawtext;
    }

    lxb_html_tokenizer_state_append_m(tkz, "/", 1);

    return data;
}

/*
 * 12.2.5.14 RAWTEXT end tag name state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_rawtext_end_tag_name(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end)
{
    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /*
             * U+0009 CHARACTER TABULATION (tab)
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             * U+0020 SPACE
             */
            case 0x09:
            case 0x0A:
            case 0x0C:
            case 0x0D:
            case 0x20:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_tag_m(tkz, &tkz->start[tkz->entity_start] + 2,
                                                   tkz->pos);

                if (tkz->tmp_tag_id != tkz->token->tag_id) {
                    goto anything_else;
                }

                tkz->state = lxb_html_tokenizer_state_before_attribute_name;
                goto done;

            /* U+002F SOLIDUS (/) */
            case 0x2F:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_tag_m(tkz, &tkz->start[tkz->entity_start] + 2,
                                                   tkz->pos);

                if (tkz->tmp_tag_id != tkz->token->tag_id) {
                    goto anything_else;
                }

                tkz->state = lxb_html_tokenizer_state_self_closing_start_tag;
                goto done;

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_tag_m(tkz, &tkz->start[tkz->entity_start] + 2,
                                                   tkz->pos);

                if (tkz->tmp_tag_id != tkz->token->tag_id) {
                    goto anything_else;
                }

                tkz->state = lxb_html_tokenizer_state_data_before;

                /* Emit text token */
                tkz->token->tag_id = LXB_TAG__TEXT;
                tkz->pos = &tkz->start[tkz->entity_start];

                lxb_html_tokenizer_state_set_text(tkz);
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                /* Init close token */
                tkz->token->tag_id = tkz->tmp_tag_id;
                tkz->token->begin = tkz->temp;
                tkz->token->end = data;
                tkz->token->type |= LXB_HTML_TOKEN_TYPE_CLOSE;

                /* Emit close token */
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            default:
                if (lexbor_str_res_alpha_character[*data]
                    == LEXBOR_STR_RES_SLIP)
                {
                    lxb_html_tokenizer_state_append_data_m(tkz, data);

                    goto anything_else;
                }

                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;

anything_else:

    tkz->state = lxb_html_tokenizer_state_rawtext;

    return data;

done:

    /* Emit text token */
    tkz->token->tag_id = LXB_TAG__TEXT;
    tkz->pos = &tkz->start[tkz->entity_start];

    lxb_html_tokenizer_state_set_text(tkz);
    lxb_html_tokenizer_state_token_done_m(tkz, end);

    /* Init close token */
    tkz->token->tag_id = tkz->tmp_tag_id;
    tkz->token->begin = tkz->temp;
    tkz->token->end = data;
    tkz->token->type |= LXB_HTML_TOKEN_TYPE_CLOSE;

    return (data + 1);
}
