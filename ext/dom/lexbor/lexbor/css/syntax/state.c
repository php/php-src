/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include <string.h>
#include <float.h>

#include "lexbor/core/utils.h"
#include "lexbor/core/strtod.h"

#include "lexbor/css/syntax/state.h"
#include "lexbor/css/syntax/syntax.h"
#include "lexbor/css/syntax/tokenizer/error.h"

#define LXB_CSS_SYNTAX_RES_NAME_MAP
#include "lexbor/css/syntax/res.h"

#define LEXBOR_STR_RES_MAP_HEX
#define LEXBOR_STR_RES_ANSI_REPLACEMENT_CHARACTER
#include "lexbor/core/str_res.h"


#define LXB_CSS_SYNTAX_NEXT_CHUNK(_tkz, _status, _data, _end)                  \
    do {                                                                       \
        _status = lxb_css_syntax_tokenizer_next_chunk(_tkz, &_data, &_end);    \
        if (_status != LXB_STATUS_OK) {                                        \
            return NULL;                                                       \
        }                                                                      \
    }                                                                          \
    while (0)


#define LXB_CSS_SYNTAX_STR_APPEND_LEN(_tkz, _status, _begin, _length)          \
    do {                                                                       \
        _status = lxb_css_syntax_string_append(_tkz, _begin, _length);         \
        if (_status != LXB_STATUS_OK) {                                        \
            return NULL;                                                       \
        }                                                                      \
    }                                                                          \
    while (0)

#define LXB_CSS_SYNTAX_STR_APPEND(_tkz, _status, _begin, _end)                 \
    LXB_CSS_SYNTAX_STR_APPEND_LEN(_tkz, _status, _begin, (_end - _begin))

#define LXB_CSS_SYNTAX_DELIM_APPEND(_tkz, _begin, _length, _ch)                \
    do {                                                                       \
        if (lxb_css_syntax_list_append_delim(_tkz, _begin, _length, _ch)       \
            == NULL)                                                           \
        {                                                                      \
            return NULL;                                                       \
        }                                                                      \
    }                                                                          \
    while (false)


static const lxb_char_t *
lxb_css_syntax_state_consume_numeric(lxb_css_syntax_tokenizer_t *tkz,
                                     lxb_css_syntax_token_t *token,
                                     const lxb_char_t *data,
                                     const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_decimal(lxb_css_syntax_tokenizer_t *tkz,
                             lxb_css_syntax_token_t *token,
                             lxb_char_t *buf_start, lxb_char_t *buf_end,
                             const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_consume_numeric_name_start(lxb_css_syntax_tokenizer_t *tkz,
                                                lxb_css_syntax_token_t *token,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_consume_ident(lxb_css_syntax_tokenizer_t *tkz,
                                   lxb_css_syntax_token_t *token,
                                   const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_ident_like(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_ident_like_not_url(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                        const lxb_char_t *data, const lxb_char_t *end);


static const lxb_char_t *
lxb_css_syntax_state_url(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                         const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_bad_url(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                             const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_escaped(lxb_css_syntax_tokenizer_t *tkz,
                             const lxb_char_t *data,
                             const lxb_char_t **end, size_t *length);

static const lxb_char_t *
lxb_css_syntax_state_escaped_string(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t *data,
                                    const lxb_char_t **end, size_t *length);


lxb_inline lxb_status_t
lxb_css_syntax_string_realloc(lxb_css_syntax_tokenizer_t *tkz, size_t upto)
{
    size_t len = tkz->pos - tkz->start;
    size_t size = (tkz->end - tkz->start) + upto;

    lxb_char_t *tmp = lexbor_realloc(tkz->start, size);
    if (tmp == NULL) {
        tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        return tkz->status;
    }

    tkz->start = tmp;
    tkz->pos = tmp + len;
    tkz->end = tmp + size;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_css_syntax_string_append(lxb_css_syntax_tokenizer_t *tkz,
                             const lxb_char_t *data, size_t length)
{
    if ((size_t) (tkz->end - tkz->pos) <= length) {
        if (lxb_css_syntax_string_realloc(tkz, length + 1024) != LXB_STATUS_OK) {
            return tkz->status;
        }
    }

    memcpy(tkz->pos, data, length);

    tkz->pos += length;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_css_syntax_state_string_term(lxb_css_syntax_tokenizer_t *tkz)
{
    if (tkz->pos >= tkz->end) {
        if (lxb_css_syntax_string_realloc(tkz, 1024) != LXB_STATUS_OK) {
            return tkz->status;
        }
    }

    *tkz->pos = 0x00;

    return LXB_STATUS_OK;
}

lxb_inline const lxb_char_t *
lxb_css_syntax_state_string_set(lxb_css_syntax_tokenizer_t *tkz,
                                lxb_css_syntax_token_t *token,
                                const lxb_char_t *data)
{
    if(lxb_css_syntax_state_string_term(tkz) != LXB_STATUS_OK) {
        return NULL;
    }

    lxb_css_syntax_token_string(token)->data = tkz->start;
    lxb_css_syntax_token_string(token)->length = tkz->pos - tkz->start;

    tkz->pos = tkz->start;

    return data;
}

lxb_inline const lxb_char_t *
lxb_css_syntax_state_dimension_set(lxb_css_syntax_tokenizer_t *tkz,
                                   lxb_css_syntax_token_t *token,
                                   const lxb_char_t *data)
{
    if(lxb_css_syntax_state_string_term(tkz) != LXB_STATUS_OK) {
        return NULL;
    }

    lxb_css_syntax_token_dimension_string(token)->data = tkz->start;
    lxb_css_syntax_token_dimension_string(token)->length = tkz->pos - tkz->start;

    tkz->pos = tkz->start;

    return data;
}

lxb_inline lxb_css_syntax_token_t *
lxb_css_syntax_state_token_create(lxb_css_syntax_tokenizer_t *tkz)
{
    if (tkz->prepared == 0) {
        tkz->prepared = tkz->cache->length;
    }

    return lxb_css_syntax_token_cached_create(tkz);
}

/*
 * Delim
 */
lxb_inline void
lxb_css_syntax_state_delim_set(lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, lxb_char_t ch,
                               size_t length)
{
    lxb_css_syntax_token_delim(token)->character = ch;
    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = length;

    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;
}

lxb_inline lxb_css_syntax_token_t *
lxb_css_syntax_list_append_delim(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data,
                                 size_t length, lxb_char_t ch)
{
    lxb_css_syntax_token_t *delim;

    delim = lxb_css_syntax_state_token_create(tkz);
    if (delim == NULL) {
        return NULL;
    }

    lxb_css_syntax_state_delim_set(delim, data, ch, length);

    return delim;
}

const lxb_char_t *
lxb_css_syntax_state_delim(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_state_delim_set(token, data, *data, 1);

    return data + 1;
}

/*
 * Comment
 */
const lxb_char_t *
lxb_css_syntax_state_comment(lxb_css_syntax_tokenizer_t *tkz,
                             lxb_css_syntax_token_t *token,
                             const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_status_t status;
    const lxb_char_t *begin;

    lxb_css_syntax_token_base(token)->begin = data;

    /* Skip forward slash (/) */
    data++;

    if (data >= end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            goto delim;
        }
    }

    /* U+002A ASTERISK (*) */
    if (*data != 0x2A) {
        goto delim;
    }

    begin = ++data;
    length = 2;

    do {
        if (data >= end) {
            if (begin < data) {
                length += data - begin;
                LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
            }

            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                goto error;
            }

            begin = data;
        }

        switch (*data) {
            case 0x00:
                if (begin < data) {
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status,
                                              lexbor_str_res_ansi_replacement_character,
                                              sizeof(lexbor_str_res_ansi_replacement_character) - 1);
                data += 1;
                length += data - begin;
                begin = data;

                continue;

            case 0x0D:
                data++;
                length += data - begin;

                LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);

                tkz->pos[-1] = '\n';

                if (data >= end) {
                    LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
                    if (data >= end) {
                        goto error;
                    }
                }

                if (*data != 0x0A) {
                    data--;
                }
                else {
                    length += 1;
                }

                begin = ++data;

                continue;

            case 0x0C:
                if (begin < data) {
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status,
                                              (lxb_char_t *) "\n", 1);
                data += 1;
                length += data - begin;
                begin = data;

                continue;

            /* U+002A ASTERISK (*) */
            case 0x2A:
                data++;

                if (data >= end) {
                    length += data - begin;

                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);

                    LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
                    if (data >= end) {
                        goto error;
                    }

                    if (*data == 0x2F) {
                        tkz->pos--;
                        *tkz->pos = 0x00;

                        data++;
                        length++;

                        goto done;
                    }

                    begin = data;
                }

                /* U+002F Forward slash (/) */
                if (*data == 0x2F) {
                    length += data - begin;

                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, (data - 1));

                    data++;
                    length++;

                    goto done;
                }

                continue;
        }

        data++;
    }
    while (true);

done:

    token->type = LXB_CSS_SYNTAX_TOKEN_COMMENT;

    lxb_css_syntax_token_base(token)->length = length;

    return lxb_css_syntax_state_string_set(tkz, token, data);

delim:

    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

    lxb_css_syntax_token_base(token)->length = 1;
    lxb_css_syntax_token_delim(token)->character = '/';

    return data;

error:

    token->type = LXB_CSS_SYNTAX_TOKEN_COMMENT;

    lxb_css_syntax_token_base(token)->length = length;

    lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, NULL,
                                       LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINCO);

    return lxb_css_syntax_state_string_set(tkz, token, data);
}

/*
 * Whitespace
 */
const lxb_char_t *
lxb_css_syntax_state_whitespace(lxb_css_syntax_tokenizer_t *tkz,
                                lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_status_t status;
    const lxb_char_t *begin;

    token->type = LXB_CSS_SYNTAX_TOKEN_WHITESPACE;

    lxb_css_syntax_token_base(token)->begin = data;

    begin = data;
    length = 0;

    do {
        switch (*data) {
            case 0x0D:
                data++;
                length += data - begin;

                LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);

                tkz->pos[-1] = '\n';

                if (data >= end) {
                    LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
                    if (data >= end) {
                        goto done;
                    }
                }

                if (*data != 0x0A) {
                    data--;
                }
                else {
                    length += 1;
                }

                begin = data + 1;
                break;

            case 0x0C:
                length += (data + 1) - begin;

                if (begin < data) {
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status,
                                              (const lxb_char_t *) "\n", 1);
                begin = data + 1;
                break;

            case 0x09:
            case 0x20:
            case 0x0A:
                break;

            default:
                if (begin < data) {
                    length += data - begin;

                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                lxb_css_syntax_token_base(token)->length = length;

                return lxb_css_syntax_state_string_set(tkz, token, data);
        }

        data++;

        if (data >= end) {
            if (begin < data) {
                length += data - begin;

                LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
            }

            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                break;
            }

            begin = data;
        }
    }
    while (true);

done:

    lxb_css_syntax_token_base(token)->length = length;

    return lxb_css_syntax_state_string_set(tkz, token, data);
}

/*
 * String token for U+0022 Quotation Mark (") and U+0027 Apostrophe (')
 */
const lxb_char_t *
lxb_css_syntax_state_string(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                            const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_char_t mark;
    lxb_status_t status;
    const lxb_char_t *begin;

    lxb_css_syntax_token_base(token)->begin = data;

    mark = *data++;
    begin = data;
    length = 1;

    for (;; data++) {
        if (data >= end) {
            if (begin < data) {
                length += data - begin;

                LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
            }

            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                goto error;
            }

            begin = data;
        }

        switch (*data) {
            case 0x00:
                length += (data + 1) - begin;

                if (begin < data) {
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status,
                                              lexbor_str_res_ansi_replacement_character,
                                              sizeof(lexbor_str_res_ansi_replacement_character) - 1);
                begin = data + 1;
                break;

            /*
             * U+000A LINE FEED
             * U+000D CARRIAGE RETURN
             * U+000C FORM FEED
             */
            case 0x0A:
            case 0x0D:
            case 0x0C:
                length += data - begin;

                if (begin < data) {
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_NEINST);

                token->type = LXB_CSS_SYNTAX_TOKEN_BAD_STRING;

                lxb_css_syntax_token_base(token)->length = length;

                return lxb_css_syntax_state_string_set(tkz, token, data);

            /* U+005C REVERSE SOLIDUS (\) */
            case 0x5C:
                length += (data + 1) - begin;

                if (begin < data) {
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                data++;

                if (data >= end) {
                    LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
                    if (data >= end) {
                        LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status,
                                                  (const lxb_char_t *) "\\", 1);
                        goto error;
                    }
                }

                data = lxb_css_syntax_state_escaped_string(tkz, data, &end,
                                                           &length);
                if (data == NULL) {
                    return NULL;
                }

                begin = data;

                data--;
                break;

            default:
                /* '"' or '\'' */
                if (*data == mark) {
                    length += (data + 1) - begin;

                    if (begin < data) {
                        LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                    }

                    token->type = LXB_CSS_SYNTAX_TOKEN_STRING;

                    lxb_css_syntax_token_base(token)->length = length;

                    return lxb_css_syntax_state_string_set(tkz, token,
                                                           data + 1);
                }

                break;
        }
    }

    return data;

error:

    lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, NULL,
                                       LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINST);

    token->type = LXB_CSS_SYNTAX_TOKEN_STRING;

    lxb_css_syntax_token_base(token)->length = length;

    return lxb_css_syntax_state_string_set(tkz, token, data);
}

/*
 * U+0023 NUMBER SIGN (#)
 */
const lxb_char_t *
lxb_css_syntax_state_hash(lxb_css_syntax_tokenizer_t *tkz,
                          lxb_css_syntax_token_t *token, const lxb_char_t *data,
                          const lxb_char_t *end)
{
    size_t length;
    lxb_char_t ch;
    lxb_status_t status;
    const lxb_char_t *begin;
    lxb_css_syntax_token_t *delim;

    lxb_css_syntax_token_base(token)->begin = data++;

    if (data >= end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            goto delim;
        }
    }

    length = 1;

    if (lxb_css_syntax_res_name_map[*data] == 0x00) {
        if (*data == 0x00) {
            goto hash;
        }

        /* U+005C REVERSE SOLIDUS (\) */
        if (*data != 0x5C) {
            goto delim;
        }

        begin = data++;

        if (data >= end) {
            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                goto push_delim;
            }
        }

        ch = *data;

        if (ch == 0x0A || ch == 0x0C || ch == 0x0D) {
            goto push_delim;
        }

        length += 1;

        data = lxb_css_syntax_state_escaped(tkz, data, &end, &length);
        if (data == NULL) {
            return NULL;
        }
    }

hash:

    token->type = LXB_CSS_SYNTAX_TOKEN_HASH;

    lxb_css_syntax_token_base(token)->length = length;

    return lxb_css_syntax_state_consume_ident(tkz, token, data, end);

push_delim:

    delim = lxb_css_syntax_list_append_delim(tkz, begin, 1, '\\');
    if (delim == NULL) {
        return NULL;
    }

delim:

    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

    lxb_css_syntax_token_base(token)->length = 1;
    lxb_css_syntax_token_delim(token)->character = '#';

    return data;
}

/*
 * U+0028 LEFT PARENTHESIS (()
 */
const lxb_char_t *
lxb_css_syntax_state_lparenthesis(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS;

    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 1;

    return data + 1;
}

/*
 * U+0029 RIGHT PARENTHESIS ())
 */
const lxb_char_t *
lxb_css_syntax_state_rparenthesis(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS;

    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 1;

    return data + 1;
}

/*
 * U+002B PLUS SIGN (+)
 */
const lxb_char_t *
lxb_css_syntax_state_plus(lxb_css_syntax_tokenizer_t *tkz,
                          lxb_css_syntax_token_t *token,
                          const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_status_t status;

    lxb_css_syntax_token_base(token)->begin = data++;

    if (data >= end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

            lxb_css_syntax_token_base(token)->length = 1;
            lxb_css_syntax_token_delim(token)->character = '+';

            return data;
        }
    }

    return lxb_css_syntax_state_plus_process(tkz, token, data, end);
}

const lxb_char_t *
lxb_css_syntax_state_plus_process(lxb_css_syntax_tokenizer_t *tkz,
                                  lxb_css_syntax_token_t *token,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_status_t status;
    const lxb_char_t *begin;
    lxb_css_syntax_token_t *delim;

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        lxb_css_syntax_token_number(token)->have_sign = true;
        lxb_css_syntax_token_base(token)->length = 1;

        return lxb_css_syntax_state_consume_numeric(tkz, token, data, end);
    }

    /* U+002E FULL STOP (.) */
    if (*data == 0x2E) {
        begin = data++;

        if (data == end) {
            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);

            if (data >= end || *data < 0x30 || *data > 0x39) {
                goto push_delim;
            }

            lxb_css_syntax_token_number(token)->have_sign = true;
            lxb_css_syntax_token_base(token)->length = 2;

            return lxb_css_syntax_state_decimal(tkz, token, tkz->buffer,
                                                tkz->buffer + sizeof(tkz->buffer),
                                                data, end);
        }

        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (*data >= 0x30 && *data <= 0x39) {
            lxb_css_syntax_token_number(token)->have_sign = true;
            lxb_css_syntax_token_base(token)->length = 2;

            return lxb_css_syntax_state_decimal(tkz, token, tkz->buffer,
                                                tkz->buffer + sizeof(tkz->buffer),
                                                data, end);
        }

    push_delim:

        delim = lxb_css_syntax_list_append_delim(tkz, begin, 1, '.');
        if (delim == NULL) {
            return NULL;
        }
    }

    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

    lxb_css_syntax_token_base(token)->length = 1;
    lxb_css_syntax_token_delim(token)->character = '+';

    return data;
}

/*
 * U+002C COMMA (,)
 */
const lxb_char_t *
lxb_css_syntax_state_comma(lxb_css_syntax_tokenizer_t *tkz,
                           lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_COMMA;

    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 1;

    return data + 1;
}

/*
 * U+002D HYPHEN-MINUS (-)
 */
const lxb_char_t *
lxb_css_syntax_state_minus(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_status_t status;

    lxb_css_syntax_token_base(token)->begin = data++;

    if (data >= end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

            lxb_css_syntax_token_base(token)->length = 1;
            lxb_css_syntax_token_delim(token)->character = '-';

            return data;
        }
    }

    return lxb_css_syntax_state_minus_process(tkz, token, data, end);
}

const lxb_char_t *
lxb_css_syntax_state_minus_process(lxb_css_syntax_tokenizer_t *tkz,
                                   lxb_css_syntax_token_t *token,
                                   const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_char_t ch;
    lxb_status_t status;
    const lxb_char_t *begin, *second;
    lxb_css_syntax_token_t *delim;
    lxb_css_syntax_token_number_t *number;

    unsigned minuses_len = 1;
    static const lxb_char_t minuses[3] = "---";

    /* Check for <number-token> */

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        lxb_css_syntax_token_base(token)->length = 1;

        data = lxb_css_syntax_state_consume_numeric(tkz, token, data, end);

        number = lxb_css_syntax_token_number(token);
        number->num = -number->num;

        lxb_css_syntax_token_number(token)->have_sign = true;

        return data;
    }

    /* U+002E FULL STOP (.) */
    if (*data == 0x2E) {
        begin = data++;

        if (data == end) {
            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                goto push_delim;
            }
        }

        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (*data >= 0x30 && *data <= 0x39) {
            lxb_css_syntax_token_base(token)->length = 2;

            data = lxb_css_syntax_state_decimal(tkz, token, tkz->buffer,
                                                tkz->buffer + sizeof(tkz->buffer),
                                                data, end);

            number = lxb_css_syntax_token_number(token);
            number->num = -number->num;

            lxb_css_syntax_token_number(token)->have_sign = true;

            return data;
        }

    push_delim:

        delim = lxb_css_syntax_list_append_delim(tkz, begin, 1, '.');
        if (delim == NULL) {
            return NULL;
        }

        goto delim;
    }

    second = data;

    /* U+002D HYPHEN-MINUS (-) */
    if (*data == 0x2D) {
        data++;

        /* Check for <CDC-token> */

        if (data == end) {
            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                delim = lxb_css_syntax_list_append_delim(tkz, second, 1, '-');
                if (delim == NULL) {
                    return NULL;
                }

                goto delim;
            }
        }

        if (*data == 0x2D) {
            lxb_css_syntax_token_base(token)->length = 3;
            LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, minuses, 3);

            return lxb_css_syntax_state_ident_like_not_url(tkz, token,
                                                           ++data, end);
        }
        else if (*data == 0x3E) {
            token->type = LXB_CSS_SYNTAX_TOKEN_CDC;

            lxb_css_syntax_token_base(token)->length = 3;

            return data + 1;
        }

        minuses_len++;
    }

    /* Check for <ident-token> */

    if (lxb_css_syntax_res_name_map[*data] == LXB_CSS_SYNTAX_RES_NAME_START
        || *data == 0x00)
    {
        lxb_css_syntax_token_base(token)->length = minuses_len;
        LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, minuses, minuses_len);

        return lxb_css_syntax_state_ident_like_not_url(tkz, token, data, end);
    }

    length = 0;

    /* U+005C REVERSE SOLIDUS (\) */
    if (*data == 0x5C) {
        begin = data++;

        if (data == end) {
            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                goto delim_rev_solidus;
            }

            ch = *data;

            if (ch != 0x0A && ch != 0x0C && ch != 0x0D) {
                length += 1;
                goto ident;
            }

            goto delim_rev_solidus;
        }

        ch = *data;

        if (ch != 0x0A && ch != 0x0C && ch != 0x0D) {
            length += 1;
            goto ident;
        }

    delim_rev_solidus:

        if (minuses_len == 2) {
            LXB_CSS_SYNTAX_DELIM_APPEND(tkz, second, 1, '-');
        }

        LXB_CSS_SYNTAX_DELIM_APPEND(tkz, begin, 1, '\\');

        goto delim;
    }

    if (minuses_len == 2) {
        LXB_CSS_SYNTAX_DELIM_APPEND(tkz, second, 0, '-');
    }

delim:

    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

    lxb_css_syntax_token_base(token)->length = 1;
    lxb_css_syntax_token_delim(token)->character = '-';

    return data;

ident:

    LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, minuses, minuses_len);

    data = lxb_css_syntax_state_escaped(tkz, data, &end, &length);
    if (data == NULL) {
        return NULL;
    }

    lxb_css_syntax_token_base(token)->length = minuses_len + length;

    return lxb_css_syntax_state_ident_like_not_url(tkz, token, data, end);
}

/*
 * U+002E FULL STOP (.)
 */
const lxb_char_t *
lxb_css_syntax_state_full_stop(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_status_t status;

    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_number(token)->have_sign = false;

    data++;

    if (data >= end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            goto delim;
        }
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        lxb_css_syntax_token_base(token)->length = 1;

        return lxb_css_syntax_state_decimal(tkz, token, tkz->buffer,
                                            tkz->buffer + sizeof(tkz->buffer),
                                            data, end);
    }

delim:

    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

    lxb_css_syntax_token_base(token)->length = 1;
    lxb_css_syntax_token_delim(token)->character = '.';

    return data;
}

/*
 * U+003A COLON (:)
 */
const lxb_char_t *
lxb_css_syntax_state_colon(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_COLON;

    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 1;

    return data + 1;
}

/*
 * U+003B SEMICOLON (;)
 */
const lxb_char_t *
lxb_css_syntax_state_semicolon(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_SEMICOLON;

    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 1;

    return data + 1;
}

/*
 * U+003C LESS-THAN SIGN (<)
 */
const lxb_char_t *
lxb_css_syntax_state_less_sign(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_char_t ch;
    lxb_status_t status;
    const lxb_char_t *mark, *minus, *esc, *idnt;
    lxb_css_syntax_token_t *ident;

    lxb_css_syntax_token_base(token)->begin = data++;

    if ((end - data) > 2) {
        if (data[0] == '!' && data[1] == '-' && data[2] == '-') {
            data += 3;

            token->type = LXB_CSS_SYNTAX_TOKEN_CDO;
            lxb_css_syntax_token_base(token)->length = 4;

            return data;
        }

        token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

        lxb_css_syntax_token_base(token)->length = 1;
        lxb_css_syntax_token_delim(token)->character = '<';

        return data;
    }

    if (data >= end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            goto delim;
        }
    }

    /* U+0021 EXCLAMATION MARK */
    if (*data != 0x21) {
        goto delim;
    }

    mark = data++;

    if (data == end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            goto delim_mark;
        }
    }

    /* U+002D HYPHEN-MINUS */
    if (*data != 0x2D) {
        goto delim_mark;
    }

    minus = data++;

    if (data == end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            LXB_CSS_SYNTAX_DELIM_APPEND(tkz, mark, 1, '!');
            LXB_CSS_SYNTAX_DELIM_APPEND(tkz, minus, 1, '-');

            goto delim;
        }
    }

    /* U+002D HYPHEN-MINUS */
    if (*data == 0x2D) {
        token->type = LXB_CSS_SYNTAX_TOKEN_CDO;

        lxb_css_syntax_token_base(token)->length = 4;

        return data + 1;
    }

    length = 1;
    idnt = data;

    if (lxb_css_syntax_res_name_map[*data] == LXB_CSS_SYNTAX_RES_NAME_START) {
        goto ident_with_minus;
    }

    /* U+005C REVERSE SOLIDUS (\) */
    if (*data == 0x5C) {
        esc = data++;
        length += 1;

        if (data == end) {
            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                goto delim_esc;
            }

            ch = *data;

            if (ch != 0x0A && ch != 0x0C && ch != 0x0D) {
                LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status,
                                              (const lxb_char_t *) "-", 1);

                data = lxb_css_syntax_state_escaped(tkz, data, &end, &length);
                if (data == NULL) {
                    return NULL;
                }

                goto ident;
            }

        delim_esc:

            LXB_CSS_SYNTAX_DELIM_APPEND(tkz, mark, 1, '!');
            LXB_CSS_SYNTAX_DELIM_APPEND(tkz, minus, 1, '-');
            LXB_CSS_SYNTAX_DELIM_APPEND(tkz, esc, 1, '\\');

            goto delim;
        }

        ch = *data--;

        if (ch == 0x0A || ch == 0x0C || ch == 0x0D) {
            LXB_CSS_SYNTAX_DELIM_APPEND(tkz, mark, 1, '!');
            LXB_CSS_SYNTAX_DELIM_APPEND(tkz, minus, 1, '-');

            goto delim;
        }

        data = lxb_css_syntax_state_escaped(tkz, data + 1, &end, &length);
        if (data == NULL) {
            return NULL;
        }
    }
    else if (*data != 0x00) {
        LXB_CSS_SYNTAX_DELIM_APPEND(tkz, mark, 1, '!');
        LXB_CSS_SYNTAX_DELIM_APPEND(tkz, minus, 0, '-');

        goto delim;
    }

ident_with_minus:

    LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, (const lxb_char_t *) "-", 1);

ident:

    LXB_CSS_SYNTAX_DELIM_APPEND(tkz, mark, 1, '!');

    ident = lxb_css_syntax_state_token_create(tkz);
    if (ident == NULL) {
        return NULL;
    }

    lxb_css_syntax_token_base(ident)->begin = idnt;
    lxb_css_syntax_token_base(ident)->length = length;

    data = lxb_css_syntax_state_ident_like_not_url(tkz, ident, data, end);
    if (data == NULL) {
        return NULL;
    }

    goto delim;

delim_mark:

    LXB_CSS_SYNTAX_DELIM_APPEND(tkz, mark, 1, '!');

delim:

    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

    lxb_css_syntax_token_base(token)->length = 1;
    lxb_css_syntax_token_delim(token)->character = '<';

    return data;
}

/*
 * U+0040 COMMERCIAL AT (@)
 */
const lxb_char_t *
lxb_css_syntax_state_at(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                        const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_char_t ch;
    lxb_status_t status;
    const lxb_char_t *minus, *esc;

    unsigned minuses_len = 0;
    static const lxb_char_t minuses[2] = "--";

    token->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;

    lxb_css_syntax_token_base(token)->begin = data++;
    lxb_css_syntax_token_base(token)->length = 1;

    if (data >= end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            goto delim;
        }
    }

    if (lxb_css_syntax_res_name_map[*data] == LXB_CSS_SYNTAX_RES_NAME_START) {
        return lxb_css_syntax_state_consume_ident(tkz, token, data, end);
    }

    minus = data;

    /* U+002D HYPHEN-MINUS */
    if (*data == 0x2D) {
        data++;

        if (data == end) {
            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                LXB_CSS_SYNTAX_DELIM_APPEND(tkz, minus, 1, '-');
                goto delim;
            }
        }

        if (lxb_css_syntax_res_name_map[*data] == LXB_CSS_SYNTAX_RES_NAME_START
            || *data == 0x00)
        {
            lxb_css_syntax_token_base(token)->length += 1;
            LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, minuses, 1);
            return lxb_css_syntax_state_consume_ident(tkz, token, data, end);
        }
        else if (*data == 0x2D) {
            lxb_css_syntax_token_base(token)->length += 2;
            LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, minuses, 2);
            return lxb_css_syntax_state_consume_ident(tkz, token,
                                                      data + 1, end);
        }

        minuses_len++;
    }

    /* U+005C REVERSE SOLIDUS (\) */
    if (*data == 0x5C) {
        esc = data++;

        if (data == end) {
            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                goto delim_esc;
            }
        }

        ch = *data;

        if (ch != 0x0A && ch != 0x0C && ch != 0x0D) {
            LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, minuses, minuses_len);

            length = 0;

            data = lxb_css_syntax_state_escaped(tkz, data, &end, &length);
            if (data == NULL) {
                return NULL;
            }

            lxb_css_syntax_token_base(token)->length += 1 + minuses_len + length;

            return lxb_css_syntax_state_consume_ident(tkz, token, data, end);
        }

        goto delim_esc;
    }
    else if (*data != 0x00) {
        if (minuses_len != 0) {
            LXB_CSS_SYNTAX_DELIM_APPEND(tkz, minus, 0, '-');
        }

        goto delim;
    }

    LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, minuses, minuses_len);

    lxb_css_syntax_token_base(token)->length += minuses_len;

    return lxb_css_syntax_state_consume_ident(tkz, token, data, end);

delim_esc:

    if (minuses_len != 0) {
        LXB_CSS_SYNTAX_DELIM_APPEND(tkz, minus, 1, '-');
    }

    LXB_CSS_SYNTAX_DELIM_APPEND(tkz, esc, 1, '\\');

delim:

    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

    lxb_css_syntax_token_base(token)->length = 1;
    lxb_css_syntax_token_delim(token)->character = '@';

    return data;
}

/*
 * U+005B LEFT SQUARE BRACKET ([)
 */
const lxb_char_t *
lxb_css_syntax_state_ls_bracket(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_LS_BRACKET;

    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 1;

    return data + 1;
}

/*
 * U+005C REVERSE SOLIDUS (\)
 */
const lxb_char_t *
lxb_css_syntax_state_rsolidus(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                              const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_char_t ch;
    lxb_status_t status;

    lxb_css_syntax_token_base(token)->begin = data++;

    if (data >= end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            goto delim;
        }
    }

    ch = *data;

    if (ch == 0x0A || ch == 0x0C || ch == 0x0D) {
        goto delim;
    }

    length = 1;

    data = lxb_css_syntax_state_escaped(tkz, data, &end, &length);
    if (data == NULL) {
        return NULL;
    }

    lxb_css_syntax_token_base(token)->length = length;

    return lxb_css_syntax_state_ident_like(tkz, token, data, end);

delim:

    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

    lxb_css_syntax_token_base(token)->length = 1;
    lxb_css_syntax_token_delim(token)->character = '\\';

    return data;
}

/*
 * U+005D RIGHT SQUARE BRACKET (])
 */
const lxb_char_t *
lxb_css_syntax_state_rs_bracket(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_RS_BRACKET;

    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 1;

    return data + 1;
}

/*
 * U+007B LEFT CURLY BRACKET ({)
 */
const lxb_char_t *
lxb_css_syntax_state_lc_bracket(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_LC_BRACKET;

    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 1;

    return data + 1;
}

/*
 * U+007D RIGHT CURLY BRACKET (})
 */
const lxb_char_t *
lxb_css_syntax_state_rc_bracket(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_RC_BRACKET;

    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 1;

    return data + 1;
}

/*
 * Numeric
 */
lxb_inline void
lxb_css_syntax_consume_numeric_set_int(lxb_css_syntax_tokenizer_t *tkz,
                                       lxb_css_syntax_token_t *token,
                                       const lxb_char_t *start, const lxb_char_t *end)
{
    double num = lexbor_strtod_internal(start, (end - start), 0);

    token->type = LXB_CSS_SYNTAX_TOKEN_NUMBER;

    lxb_css_syntax_token_number(token)->is_float = false;
    lxb_css_syntax_token_number(token)->num = num;
}

lxb_inline void
lxb_css_syntax_consume_numeric_set_float(lxb_css_syntax_tokenizer_t *tkz,
                                         lxb_css_syntax_token_t *token,
                                         const lxb_char_t *start, const lxb_char_t *end,
                                         bool e_is_negative, int exponent, int e_digit)
{
    if (e_is_negative) {
        exponent -= e_digit;
    }
    else {
        exponent += e_digit;
    }

    double num = lexbor_strtod_internal(start, (end - start), exponent);

    token->type = LXB_CSS_SYNTAX_TOKEN_NUMBER;

    lxb_css_syntax_token_number(token)->num = num;
    lxb_css_syntax_token_number(token)->is_float = true;
}

const lxb_char_t *
lxb_css_syntax_state_consume_before_numeric(lxb_css_syntax_tokenizer_t *tkz,
                                            lxb_css_syntax_token_t *token,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end)
{
    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 0;
    lxb_css_syntax_token_number(token)->have_sign = false;

    return lxb_css_syntax_state_consume_numeric(tkz, token, data, end);
}

static const lxb_char_t *
lxb_css_syntax_state_consume_numeric(lxb_css_syntax_tokenizer_t *tkz,
                                     lxb_css_syntax_token_t *token,
                                     const lxb_char_t *data,
                                     const lxb_char_t *end)
{
    size_t length;
    lxb_status_t status;
    const lxb_char_t *begin;

    lxb_char_t *buf_start = tkz->buffer;
    lxb_char_t *buf_end = buf_start + sizeof(tkz->buffer);

    begin = data;
    length = 0;

    do {
        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (*data < 0x30 || *data > 0x39) {
            length += data - begin;
            break;
        }

        if (buf_start != buf_end) {
            *buf_start++ = *data;
        }

        if (++data == end) {
            length += data - begin;

            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                lxb_css_syntax_token_base(token)->length += length;

                lxb_css_syntax_consume_numeric_set_int(tkz, token, tkz->buffer,
                                                       buf_start);
                return data;
            }

            begin = data;
        }
    }
    while (true);

    lxb_css_syntax_token_base(token)->length += length;

    /* U+002E FULL STOP (.) */
    if (*data != 0x2E) {
        lxb_css_syntax_consume_numeric_set_int(tkz, token, tkz->buffer,
                                               buf_start);

        return lxb_css_syntax_state_consume_numeric_name_start(tkz, token,
                                                               data, end);
    }

    begin = data++;

    if (data == end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            goto delim;
        }
    }

    if (*data >= 0x30 && *data <= 0x39) {
        lxb_css_syntax_token_base(token)->length += 1;

        return lxb_css_syntax_state_decimal(tkz, token, buf_start, buf_end,
                                            data, end);
    }

delim:

    lxb_css_syntax_consume_numeric_set_int(tkz, token, tkz->buffer, buf_start);

    LXB_CSS_SYNTAX_DELIM_APPEND(tkz, begin, 1, '.');

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_decimal(lxb_css_syntax_tokenizer_t *tkz,
                             lxb_css_syntax_token_t *token,
                             lxb_char_t *buf_start, lxb_char_t *buf_end,
                             const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    bool e_is_negative;
    int exponent, e_digit;
    lxb_char_t ch, by;
    lxb_status_t status;
    const lxb_char_t *last, *begin;
    lxb_css_syntax_token_t *t_str;
    lxb_css_syntax_token_string_t *str;

    exponent = 0;
    begin = data;
    length = lxb_css_syntax_token_base(token)->length;

    str = lxb_css_syntax_token_dimension_string(token);
    t_str = (lxb_css_syntax_token_t *) (void *) str;

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    do {
        if (buf_start != buf_end) {
            *buf_start++ = *data;
            exponent -= 1;
        }

        data++;

        if (data >= end) {
            length += data - begin;

            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                lxb_css_syntax_token_base(token)->length = length;

                lxb_css_syntax_consume_numeric_set_float(tkz, token, tkz->buffer,
                                                         buf_start, 0, exponent, 0);
                return data;
            }

            begin = data;
        }
    }
    while (*data >= 0x30 && *data <= 0x39);

    length += data - begin;

    lxb_css_syntax_token_base(token)->length = length;
    lxb_css_syntax_token_base(str)->begin = data;

    ch = *data;

    /* U+0045 Latin Capital Letter (E) or U+0065 Latin Small Letter (e) */
    if (ch != 0x45 && ch != 0x65) {
        lxb_css_syntax_consume_numeric_set_float(tkz, token, tkz->buffer,
                                                 buf_start, 0, exponent, 0);

        return lxb_css_syntax_state_consume_numeric_name_start(tkz, token,
                                                               data, end);
    }

    e_digit = 0;
    e_is_negative = false;

    lxb_css_syntax_token_base(t_str)->length = 1;

    if (++data == end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            lxb_css_syntax_consume_numeric_set_float(tkz, token, tkz->buffer,
                                                     buf_start, 0, exponent, 0);

            LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, &ch, 1);

            token->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

            data = lxb_css_syntax_state_dimension_set(tkz, token, data);

            lxb_css_syntax_token_base(token)->length +=
                                      lxb_css_syntax_token_base(t_str)->length;
            return data;
        }
    }

    switch (*data) {
        /* U+002D HYPHEN-MINUS (-) */
        case 0x2D:
            e_is_negative = true;
            /* fall through */

        /* U+002B PLUS SIGN (+) */
        case 0x2B:
            last = data++;
            by = *last;

            if (data == end) {
                LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
                if (data >= end) {
                    goto dimension;
                }
            }

            /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
            if (*data < 0x30 || *data > 0x39) {
                goto dimension;
            }

            length += 1;
            break;

        default:
            /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
            if (*data < 0x30 || *data > 0x39) {
                lxb_css_syntax_consume_numeric_set_float(tkz, token,
                                                         tkz->buffer, buf_start,
                                                         0, exponent, 0);

                token->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

                LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, &ch, 1);

                data = lxb_css_syntax_state_consume_ident(tkz, t_str,
                                                           data, end);
                if (begin == NULL) {
                    return NULL;
                }

                lxb_css_syntax_token_base(token)->length = length
                                + lxb_css_syntax_token_base(t_str)->length;
                return data;
            }

            break;
    }

    length += 1;
    begin = data;

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    do {
        e_digit = (*data - 0x30) + e_digit * 0x0A;

        if (++data == end) {
            length += data - begin;

            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                lxb_css_syntax_token_base(token)->length = length;

                lxb_css_syntax_consume_numeric_set_float(tkz, token, tkz->buffer, buf_start,
                                                         e_is_negative, exponent, e_digit);
                return data;
            }

            begin = data;
        }
    }
    while(*data >= 0x30 && *data <= 0x39);

    length += data - begin;

    lxb_css_syntax_token_base(token)->length = length;

    lxb_css_syntax_consume_numeric_set_float(tkz, token, tkz->buffer, buf_start,
                                             e_is_negative, exponent, e_digit);

    return lxb_css_syntax_state_consume_numeric_name_start(tkz, token,
                                                           data, end);

dimension:

    lxb_css_syntax_consume_numeric_set_float(tkz, token,
                                             tkz->buffer, buf_start,
                                             0, exponent, 0);

    token->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

    LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, &ch, 1);

    if (by == '-') {
        LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status, &by, 1);

        lxb_css_syntax_token_base(t_str)->length += 1;

        data = lxb_css_syntax_state_consume_ident(tkz, t_str, data, end);

        lxb_css_syntax_token_base(token)->length = length
                                    + lxb_css_syntax_token_base(t_str)->length;
        return data;
    }

    LXB_CSS_SYNTAX_DELIM_APPEND(tkz, last, (data >= end), '+');

    lxb_css_syntax_token_base(token)->length = length
                                    + lxb_css_syntax_token_base(t_str)->length;

    return lxb_css_syntax_state_dimension_set(tkz, token, data);
}

static const lxb_char_t *
lxb_css_syntax_state_consume_numeric_name_start(lxb_css_syntax_tokenizer_t *tkz,
                                                lxb_css_syntax_token_t *token,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end)
{
    bool have_minus;
    size_t length;
    lxb_char_t ch;
    lxb_status_t status;
    const lxb_char_t *esc, *minus;
    lxb_css_syntax_token_t *t_str;
    lxb_css_syntax_token_string_t *str;

    str = lxb_css_syntax_token_dimension_string(token);
    t_str = (lxb_css_syntax_token_t *) (void *) str;

    lxb_css_syntax_token_base(t_str)->begin = data;

    ch = *data;

    if (lxb_css_syntax_res_name_map[ch] == LXB_CSS_SYNTAX_RES_NAME_START
        || ch == 0x00)
    {
        lxb_css_syntax_token_base(t_str)->length = 0;
        goto dimension;
    }

    /* U+0025 PERCENTAGE SIGN (%) */
    if (ch == 0x25) {
        token->type = LXB_CSS_SYNTAX_TOKEN_PERCENTAGE;

        lxb_css_syntax_token_base(token)->length += 1;

        return data + 1;
    }

    have_minus = false;
    minus = data;

    /* U+002D HYPHEN-MINUS */
    if (ch == 0x2D) {
        data++;

        if (data >= end) {
            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                LXB_CSS_SYNTAX_DELIM_APPEND(tkz, minus, 1, '-');
                return data;
            }
        }

        ch = *data;

        if (lxb_css_syntax_res_name_map[ch] == LXB_CSS_SYNTAX_RES_NAME_START
            || ch == 0x2D || ch == 0x00)
        {
            lxb_css_syntax_token_base(t_str)->length = 1;

            LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status,
                                          (const lxb_char_t *) "-", 1);
            goto dimension;
        }

        have_minus = true;
    }

    esc = data;

    /* U+005C REVERSE SOLIDUS (\) */
    if (ch == 0x5C) {
        data++;

        if (data >= end) {
            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                goto delim_rev_solidus;
            }
        }

        ch = *data;

        if (ch != 0x0A && ch != 0x0C && ch != 0x0D) {
            length = 1;

            if (have_minus) {
                length += 1;

                LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status,
                                              (const lxb_char_t *) "-", 1);
            }

            data = lxb_css_syntax_state_escaped(tkz, data, &end, &length);
            if (data == NULL) {
                return NULL;
            }

            lxb_css_syntax_token_base(t_str)->length = length;

            goto dimension;
        }

    delim_rev_solidus:

        if (have_minus) {
            LXB_CSS_SYNTAX_DELIM_APPEND(tkz, minus, 1, '-');
        }

        LXB_CSS_SYNTAX_DELIM_APPEND(tkz, esc, 1, '\\');

        return data;
    }

    if (have_minus) {
        LXB_CSS_SYNTAX_DELIM_APPEND(tkz, minus, 0, '-');
    }

    return data;

dimension:

    token->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

    data = lxb_css_syntax_state_consume_ident(tkz, t_str, data, end);

    lxb_css_syntax_token_base(token)->length +=
                                    lxb_css_syntax_token_base(t_str)->length;

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_consume_ident(lxb_css_syntax_tokenizer_t *tkz,
                                   lxb_css_syntax_token_t *token,
                                   const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_status_t status;
    const lxb_char_t *begin;

    begin = data;
    length = 0;

    for (;; data++) {
        if (data >= end) {
            if (begin < data) {
                length += data - begin;

                LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
            }

            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                lxb_css_syntax_token_base(token)->length += length;

                return lxb_css_syntax_state_string_set(tkz, token, data);
            }

            begin = data;
        }

        if (lxb_css_syntax_res_name_map[*data] == 0x00) {

            /* U+005C REVERSE SOLIDUS (\) */
            if (*data == 0x5C) {
                if (begin < data) {
                    length += data - begin;

                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                begin = data;

                if (++data == end) {
                    LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
                    if (data >= end) {
                        goto push_delim_last;
                    }
                }

                if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                    goto push_delim_last;
                }

                length += 1;

                data = lxb_css_syntax_state_escaped(tkz, data, &end, &length);
                if (data == NULL) {
                    return NULL;
                }

                begin = data--;
            }
            else if (*data == 0x00) {
                length += (data + 1) - begin;

                if (begin < data) {
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status,
                                              lexbor_str_res_ansi_replacement_character,
                                              sizeof(lexbor_str_res_ansi_replacement_character) - 1);
                begin = data + 1;
            }
            else {
                if (begin < data) {
                    length += data - begin;

                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                lxb_css_syntax_token_base(token)->length += length;

                return lxb_css_syntax_state_string_set(tkz, token, data);
            }
        }
    }

    return data;

push_delim_last:

    lxb_css_syntax_token_base(token)->length += length;

    LXB_CSS_SYNTAX_DELIM_APPEND(tkz, begin, 1, '\\');

    return lxb_css_syntax_state_string_set(tkz, token, data);
}

const lxb_char_t *
lxb_css_syntax_state_ident_like_begin(lxb_css_syntax_tokenizer_t *tkz,
                                      lxb_css_syntax_token_t *token,
                                      const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 0;

    return lxb_css_syntax_state_ident_like(tkz, token, data, end);
}

static const lxb_char_t *
lxb_css_syntax_state_ident_like(lxb_css_syntax_tokenizer_t *tkz,
                                lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_char_t ch;
    lxb_status_t status;
    const lxb_char_t *begin, *ws_begin;
    lxb_css_syntax_token_t *ws;
    lxb_css_syntax_token_string_t *str, *ws_str;
    static const lxb_char_t url[] = "url";

    data = lxb_css_syntax_state_consume_ident(tkz, token, data, end);

    end = tkz->in_end;

    if (data >= end) {
        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
        if (data >= end) {
            token->type = LXB_CSS_SYNTAX_TOKEN_IDENT;
            return data;
        }
    }

    if (data < end && *data == '(') {
        data++;

        lxb_css_syntax_token_base(token)->length += 1;

        str = lxb_css_syntax_token_string(token);

        if (str->length == 3 && lexbor_str_data_casecmp(str->data, url)) {
            begin = data;
            length = 0;

            tkz->pos += str->length + 1;
            ws_begin = tkz->pos;

            do {
                if (data >= end) {
                    if (begin < data) {
                        length += data - begin;
                        LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                    }

                    LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
                    if (data >= end) {
                        begin = data;
                        goto with_ws;
                    }

                    begin = data;
                }

                ch = *data;

                if (lexbor_utils_whitespace(ch, !=, &&)) {
                    /* U+0022 QUOTATION MARK (") or U+0027 APOSTROPHE (') */
                    if (ch == 0x22 || ch == 0x27) {
                        goto with_ws;
                    }

                    tkz->pos = tkz->start;
                    length += data - begin;

                    lxb_css_syntax_token_base(token)->length += length;

                    return lxb_css_syntax_state_url(tkz, token, data, end);
                }

                data++;
            }
            while (true);
        }

        token->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;

        return data;
    }

    token->type = LXB_CSS_SYNTAX_TOKEN_IDENT;

    return data;

with_ws:

    token->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;

    if (ws_begin != tkz->pos || begin < data) {
        if (begin < data) {
            length += data - begin;
            LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
        }

        if (tkz->pos >= tkz->end) {
            if (lxb_css_syntax_string_realloc(tkz, 1024) != LXB_STATUS_OK) {
                return NULL;
            }
        }

        str->data = tkz->start;
        *tkz->pos = 0x00;

        ws = lxb_css_syntax_state_token_create(tkz);
        if (ws == NULL) {
            return NULL;
        }

        ws->type = LXB_CSS_SYNTAX_TOKEN_WHITESPACE;

        lxb_css_syntax_token_base(ws)->begin = begin;
        lxb_css_syntax_token_base(ws)->length = length;

        ws_str = lxb_css_syntax_token_string(ws);

        ws_str->data = tkz->start + str->length + 1;
        ws_str->length = tkz->pos - ws_str->data;
    }

    tkz->pos = tkz->start;

    return data;
}

const lxb_char_t *
lxb_css_syntax_state_ident_like_not_url_begin(lxb_css_syntax_tokenizer_t *tkz,
                                              lxb_css_syntax_token_t *token,
                                              const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token_base(token)->begin = data;
    lxb_css_syntax_token_base(token)->length = 0;

    return lxb_css_syntax_state_ident_like_not_url(tkz, token, data, end);
}

static const lxb_char_t *
lxb_css_syntax_state_ident_like_not_url(lxb_css_syntax_tokenizer_t *tkz,
                                        lxb_css_syntax_token_t *token,
                                        const lxb_char_t *data, const lxb_char_t *end)
{
    data = lxb_css_syntax_state_consume_ident(tkz, token, data, end);
    if (data == NULL) {
        return NULL;
    }

    end = tkz->in_end;

    if (data < end && *data == '(') {
        token->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;

        lxb_css_syntax_token_base(token)->length += 1;

        return data + 1;
    }

    token->type = LXB_CSS_SYNTAX_TOKEN_IDENT;

    return data;
}

/*
 * URL
 */
static const lxb_char_t *
lxb_css_syntax_state_url(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                         const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_char_t ch;
    lxb_status_t status;
    const lxb_char_t *begin;

    status = LXB_STATUS_OK;

    *tkz->pos = 0x00;

    begin = data;
    length = 0;

    do {
        if (data >= end) {
            if (begin < data) {
                length += data - begin;
                LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
            }

            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                                   LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINUR);

                token->type = LXB_CSS_SYNTAX_TOKEN_URL;

                lxb_css_syntax_token_base(token)->length += length;

                return lxb_css_syntax_state_string_set(tkz, token, data);
            }

            begin = data;
        }

        switch (*data) {
            /* U+0000 NULL (\0) */
            case 0x00:
                if (begin < data) {
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                LXB_CSS_SYNTAX_STR_APPEND_LEN(tkz, status,
                                              lexbor_str_res_ansi_replacement_character,
                                              sizeof(lexbor_str_res_ansi_replacement_character) - 1);

                data += 1;
                length += data - begin;
                begin = data;

                continue;

            /* U+0029 RIGHT PARENTHESIS ()) */
            case 0x29:
                if (begin < data) {
                    length += data - begin;
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                token->type = LXB_CSS_SYNTAX_TOKEN_URL;

                lxb_css_syntax_token_base(token)->length += length + 1;

                return lxb_css_syntax_state_string_set(tkz, token, data + 1);

            /*
             * U+0022 QUOTATION MARK (")
             * U+0027 APOSTROPHE (')
             * U+0028 LEFT PARENTHESIS (()
             * U+000B LINE TABULATION
             * U+007F DELETE
             */
            case 0x22:
            case 0x27:
            case 0x28:
            case 0x0B:
            case 0x7F:
                if (begin < data) {
                    length += data - begin;
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_QOINUR);

                lxb_css_syntax_token_base(token)->length += length + 1;

                return lxb_css_syntax_state_bad_url(tkz, token, data + 1, end);

            /* U+005C REVERSE SOLIDUS (\) */
            case 0x5C:
                if (begin < data) {
                    length += data - begin;
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                begin = ++data;

                if (data == end) {
                    LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
                    if (data >= end) {
                        lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                                           LXB_CSS_SYNTAX_TOKENIZER_ERROR_WRESINUR);

                        token->type = LXB_CSS_SYNTAX_TOKEN_BAD_URL;

                        lxb_css_syntax_token_base(token)->length += length + 1;

                        return lxb_css_syntax_state_string_set(tkz, token, data);
                    }
                }

                ch = *data;

                if (ch == 0x0A || ch == 0x0C || ch == 0x0D) {
                    lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                       LXB_CSS_SYNTAX_TOKENIZER_ERROR_WRESINUR);

                    lxb_css_syntax_token_base(token)->length += length + 1;

                    return lxb_css_syntax_state_bad_url(tkz, token, data, end);
                }

                data = lxb_css_syntax_state_escaped(tkz, data, &end, &length);
                if (data == NULL) {
                    return NULL;
                }

                begin = data--;
                length += 1;

                break;

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
                if (begin < data) {
                    length += data - begin;
                    LXB_CSS_SYNTAX_STR_APPEND(tkz, status, begin, data);
                }

                begin = ++data;
                length += 1;

                do {
                    if (data == end) {
                        length += data - begin;

                        LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
                        if (data >= end) {
                            lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                                               LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINUR);

                            token->type = LXB_CSS_SYNTAX_TOKEN_BAD_URL;

                            lxb_css_syntax_token_base(token)->length += length;

                            return lxb_css_syntax_state_string_set(tkz, token, data);
                        }

                        begin = data;
                    }

                    ch = *data;

                    if (lexbor_utils_whitespace(ch, !=, &&)) {
                        length += data - begin;

                        /* U+0029 RIGHT PARENTHESIS ()) */
                        if (*data == 0x29) {
                            token->type = LXB_CSS_SYNTAX_TOKEN_URL;

                            lxb_css_syntax_token_base(token)->length += length + 1;

                            return lxb_css_syntax_state_string_set(tkz, token,
                                                                   data + 1);
                        }

                        lxb_css_syntax_token_base(token)->length += length;

                        return lxb_css_syntax_state_bad_url(tkz, token,
                                                            data, end);
                    }

                    data++;
                }
                while (true);

            default:
                /*
                 * Inclusive:
                 * U+0000 NULL and U+0008 BACKSPACE or
                 * U+000E SHIFT OUT and U+001F INFORMATION SEPARATOR ONE
                 */
                if ((*data <= 0x08)
                    || (*data >= 0x0E && *data <= 0x1F))
                {
                    lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_QOINUR);

                    lxb_css_syntax_token_base(token)->length += length;

                    return lxb_css_syntax_state_bad_url(tkz, token,
                                                        data + 1, end);
                }

                break;
        }

        data++;
    }
    while (true);

    return data;
}

/*
 * Bad URL
 */
static const lxb_char_t *
lxb_css_syntax_state_bad_url(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length;
    lxb_status_t status;
    const lxb_char_t *begin;

    token->type = LXB_CSS_SYNTAX_TOKEN_BAD_URL;

    if(lxb_css_syntax_state_string_set(tkz, token, data) == NULL) {
        return NULL;
    }

    begin = data;
    length = 0;

    do {
        if (data >= end) {
            length += data - begin;

            LXB_CSS_SYNTAX_NEXT_CHUNK(tkz, status, data, end);
            if (data >= end) {
                lxb_css_syntax_token_base(token)->length += length;
                return data;
            }

            begin = data;
        }

        /* U+0029 RIGHT PARENTHESIS ()) */
        if (*data == 0x29) {
            data++;
            length += data - begin;

            lxb_css_syntax_token_base(token)->length += length;

            return data;
        }
        /* U+005C REVERSE SOLIDUS (\) */
        else if (*data == 0x5C) {
            data++;

            if (data >= end) {
                continue;
            }
        }

        data++;
    }
    while (true);

    return data;
}

lxb_inline lxb_status_t
lxb_css_syntax_string_append_rep(lxb_css_syntax_tokenizer_t *tkz)
{
    return lxb_css_syntax_string_append(tkz, lexbor_str_res_ansi_replacement_character,
                                        sizeof(lexbor_str_res_ansi_replacement_character) - 1);
}

static const lxb_char_t *
lxb_css_syntax_state_escaped(lxb_css_syntax_tokenizer_t *tkz,
                             const lxb_char_t *data,
                             const lxb_char_t **end, size_t *length)
{
    uint32_t cp;
    unsigned count;
    lxb_status_t status;

    cp = 0;

    for (count = 0; count < 6; count++, data++) {
        if (data >= *end) {
            status = lxb_css_syntax_tokenizer_next_chunk(tkz, &data, end);
            if (status != LXB_STATUS_OK) {
                return NULL;
            }

            if (data >= *end) {
                if (count == 0) {
                    return *end;
                }

                break;
            }
        }

        if (lexbor_str_res_map_hex[*data] == 0xFF) {
            if (count == 0) {
                *length += 1;

                if (*data == 0x00) {
                    status = lxb_css_syntax_string_append_rep(tkz);
                    if (status != LXB_STATUS_OK) {
                        return NULL;
                    }

                    return data + 1;
                }

                status = lxb_css_syntax_string_append(tkz, data, 1);
                if (status != LXB_STATUS_OK) {
                    return NULL;
                }

                return data + 1;
            }

            switch (*data) {
                case 0x0D:
                    data++;
                    *length += 1;

                    status = lxb_css_syntax_tokenizer_next_chunk(tkz, &data,
                                                                 end);
                    if (status != LXB_STATUS_OK) {
                        return NULL;
                    }

                    if (data >= *end) {
                        break;
                    }

                    if (*data == 0x0A) {
                        data++;
                        *length += 1;
                    }

                    break;

                case 0x09:
                case 0x20:
                case 0x0A:
                case 0x0C:
                    data++;
                    *length += 1;
                    break;
            }

            break;
        }

        cp <<= 4;
        cp |= lexbor_str_res_map_hex[*data];
    }

    if ((tkz->end - tkz->pos) < 5) {
        if (lxb_css_syntax_string_realloc(tkz, 1024) != LXB_STATUS_OK) {
            return NULL;
        }
    }

    lxb_css_syntax_codepoint_to_ascii(tkz, cp);

    *length += count;

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_escaped_string(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t *data,
                                    const lxb_char_t **end, size_t *length)
{
    lxb_status_t status;

    /* U+000D CARRIAGE RETURN */
    if (*data == 0x0D) {
        data++;
        *length += 1;

        if (data >= *end) {
            status = lxb_css_syntax_tokenizer_next_chunk(tkz, &data, end);
            if (status != LXB_STATUS_OK) {
                return NULL;
            }

            if (data >= *end) {
                return data;
            }
        }

        /* U+000A LINE FEED */
        if (*data == 0x0A) {
            data++;
            *length += 1;
        }

        return data;
    }

    if (*data == 0x00) {
        status = lxb_css_syntax_string_append_rep(tkz);
        if (status != LXB_STATUS_OK) {
            return NULL;
        }

        *length += 1;

        return data + 1;
    }

    if (*data == 0x0A || *data == 0x0C) {
        *length += 1;

        return data + 1;
    }

    return lxb_css_syntax_state_escaped(tkz, data, end, length);
}
