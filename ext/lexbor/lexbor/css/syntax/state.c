/*
 * Copyright (C) 2018-2025 Alexander Borisov
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
#include "lexbor/core/str_res.h"


#define LXB_CSS_SYNTAX_ERROR_CODEPOINT 0x1FFFFF

#define lxb_css_syntax_buffer_append_m(_tkz, _begin, _length)                 \
    do {                                                                      \
        if ((_length) > 0) {                                                  \
            lxb_status_t status = lxb_css_syntax_string_append(_tkz,          \
                                               (const lxb_char_t *) (_begin), \
                                               _length);                      \
            if (status != LXB_STATUS_OK) {                                    \
                return NULL;                                                  \
            }                                                                 \
        }                                                                     \
    }                                                                         \
    while (0)

#define LXB_CSS_SYNTAX_CARRIAGE_RETURN_BLOCK(tkz, begin, data, end)           \
    {                                                                         \
        lxb_css_syntax_buffer_append_m(tkz, begin, ((data) - (begin)) + 1);   \
        (tkz)->pos[-1] = 0x0A;                                                \
                                                                              \
        /* U+000A LINE FEED (LF) */                                           \
        if ((data) + 1 < (end) && (data)[1] == 0x0A) {                        \
            (data) += 1;                                                      \
        }                                                                     \
                                                                              \
        (begin) = (data) + 1;                                                 \
    }

#define LXB_CSS_SYNTAX_FORM_FEED_BLOCK(tkz, begin, data, end)                 \
    {                                                                         \
        lxb_css_syntax_buffer_append_m(tkz, begin, ((data) - (begin)) + 1);   \
        (tkz)->pos[-1] = 0x0A;                                                \
                                                                              \
        (begin) = (data) + 1;                                                 \
    }

#define LXB_CSS_SYNTAX_NULL_BLOCK(tkz, begin, data, end)                      \
    {                                                                         \
        lxb_css_syntax_buffer_append_m(tkz, begin, ((data) - (begin)));       \
                                                                              \
        /* U+FFFD REPLACEMENT CHARACTER */                                    \
        lxb_css_syntax_buffer_append_m(tkz, "\uFFFD",                         \
                                       lxb_css_syntax_replacement_length);    \
        (begin) = (data) + 1;                                                 \
    }

#define LXB_CSS_SYNTAX_UTF_8_UP_80_BLOCK(tkz, begin, data, end)               \
        lxb_codepoint_t cp = lxb_css_syntax_state_decode_utf_8_up_80(&data,   \
                                                                     end);    \
                                                                              \
        if (cp == LXB_CSS_SYNTAX_ERROR_CODEPOINT) {                           \
            lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);         \
            /* U+FFFD REPLACEMENT CHARACTER */                                \
            lxb_css_syntax_buffer_append_m(tkz, "\uFFFD",                     \
                                           lxb_css_syntax_replacement_length);\
            data += 1;                                                        \
            begin = data;                                                     \
        }                                                                     \
        /* Surrogate U+D800 to U+DFFF. */                                     \
        else if (cp >= 0xD800 && cp <= 0xDFFF) {                              \
            lxb_css_syntax_buffer_append_m(tkz, begin,                        \
                                           (data - begin) - 3);               \
            /* U+FFFD REPLACEMENT CHARACTER */                                \
            lxb_css_syntax_buffer_append_m(tkz, "\uFFFD",                     \
                                           lxb_css_syntax_replacement_length);\
            begin = data;                                                     \
        }


static const size_t lxb_css_syntax_replacement_length = 3; /* sizeof("\uFFFD") - 1 */


static const lxb_char_t *
lxb_css_syntax_state_consume_numeric(lxb_css_syntax_tokenizer_t *tkz,
                                     lxb_css_syntax_token_t *token,
                                     const lxb_char_t *data,
                                     const lxb_char_t *end);

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
lxb_css_syntax_state_consume_unicode_range(lxb_css_syntax_tokenizer_t *tkz,
                                           lxb_css_syntax_token_t *token,
                                           const lxb_char_t *data,
                                           const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_url(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                         const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_bad_url(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                             const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_escaped(lxb_css_syntax_tokenizer_t *tkz,
                             const lxb_char_t *data, const lxb_char_t *end);

static bool
lxb_css_syntax_state_start_number(const lxb_char_t *data, const lxb_char_t *end);

static bool
lxb_css_syntax_state_start_ident_sequence(const lxb_char_t *data,
                                          const lxb_char_t *end);

static lxb_codepoint_t
lxb_css_syntax_state_decode_utf_8_up_80(const lxb_char_t **data,
                                        const lxb_char_t *end);


lxb_inline bool
lxb_css_syntax_state_valid_escape(const lxb_char_t *data,
                                  const lxb_char_t *end)
{
    /* U+005C REVERSE SOLIDUS (\) */
    if (*data == 0x5C) {
        data += 1;
        if (data >= end || (*data != 0x0A && *data != 0x0C && *data != 0x0D)) {
            return true;
        }
    }

    return false;
}

lxb_inline bool
lxb_css_syntax_state_non_ascii(lxb_codepoint_t cp)
{
    /*
     * Actually here the comparison should be to U+D7FF.
     * But we capture surrogate pairs, they will be further converted to 0xFFFD.
     */

    /* This code is generated by the file utils/lexbor/css/syntax/non_ascii.pl */

    /* Begin */
    if (cp <= 0x2FEF) {
        if (cp <= 0x1FFF) {
            if (cp >= 0x00F8 && cp < 0x037D) {
                return true;
            }
            else if (cp >= 0x037F) {
                return true;
            }
        }
        else if (cp >= 0x2070) {
            if (cp <= 0x218F) {
                return true;
            }
            else if (cp >= 0x2C00) {
                return true;
            }
        }
    }
    else if (cp >= 0x3001) {
        if (cp <= 0xFDCF) {
            if (cp <= 0xDFFF) {
                return true;
            }
            else if (cp >= 0xF900) {
                return true;
            }
        }
        else if (cp >= 0xFDF0) {
            if (cp <= 0xFFFD) {
                return true;
            }
            else if (cp >= 0x10000) {
                if (cp <= 0x10FFFF) {
                    return true;
                }
            }
        }
    }
    /* End */

    return cp == 0x200C || cp == 0x200D || cp == 0x203F || cp == 0x2040;
}

lxb_inline bool
lxb_css_syntax_state_start_ident_utf_8_80(const lxb_char_t **data,
                                          const lxb_char_t *end)
{
    lxb_codepoint_t cp;

    cp = lxb_css_syntax_state_decode_utf_8_up_80(data, end);
    if (cp == LXB_CSS_SYNTAX_ERROR_CODEPOINT) {
        return true; /* To be replaced by \uFFFD. */
    }

    return lxb_css_syntax_state_non_ascii(cp);
}

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

/*
 * Delim
 */
lxb_inline const lxb_char_t *
lxb_css_syntax_state_delim_set(lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, lxb_char_t ch)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

    lxb_css_syntax_token_delim(token)->character = (lxb_codepoint_t) ch;
    lxb_css_syntax_token_base(token)->length = 1;

    return data + 1;
}

lxb_inline const lxb_char_t *
lxb_css_syntax_state_delim_set_cp(lxb_css_syntax_token_t *token,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_codepoint_t cp;

    if (*data < 0x80) {
        cp = *data++;
    }
    else {
        cp = lxb_css_syntax_state_decode_utf_8_up_80(&data, end);
    }

    token->type = LXB_CSS_SYNTAX_TOKEN_DELIM;

    lxb_css_syntax_token_delim(token)->character = cp;

    return data;
}

const lxb_char_t *
lxb_css_syntax_state_delim(lxb_css_syntax_tokenizer_t *tkz,
                           lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_state_delim_set(token, data, *data);

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
    bool failed;
    const lxb_char_t *begin;

    /* Skip forward slash (/) */
    data += 1;

    /* U+002A ASTERISK (*) */
    if (data >= end || *data != 0x2A) {
        return lxb_css_syntax_state_delim_set(token, data - 1, '/');
    }

    /* Skip U+002A ASTERISK (*) */
    begin = ++data;
    failed = true;

    while (data < end) {
        switch (*data) {
            /* U+002A ASTERISK (*) */
            case 0x2A:
                data += 1;

                if (data >= end) {
                    lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);
                    goto done;
                }

                /* U+002F Forward slash (/) */
                if (*data == 0x2F) {
                    lxb_css_syntax_buffer_append_m(tkz, begin,
                                                   (data - begin) - 1);
                    data += 1;
                    failed = false;
                    goto done;
                }

                continue;

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                LXB_CSS_SYNTAX_CARRIAGE_RETURN_BLOCK(tkz, begin, data, end)
                break;

            /* U+000C FORM FEED (FF) */
            case 0x0C:
                LXB_CSS_SYNTAX_FORM_FEED_BLOCK(tkz, begin, data, end)
                break;

            /* U+0000 NULL */
            case 0x00:
                LXB_CSS_SYNTAX_NULL_BLOCK(tkz, begin, data, end)
                break;

            default:
                if (*data >= 0x80) {
                    LXB_CSS_SYNTAX_UTF_8_UP_80_BLOCK(tkz, begin, data, end)
                    continue;
                }

                break;
        }

        data += 1;
    }

    lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

done:

    if (failed) {
        lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                           LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINCO);
    }

    token->type = LXB_CSS_SYNTAX_TOKEN_COMMENT;

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
    const lxb_char_t *begin;

    begin = data;

    do {
        switch (*data) {
            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                LXB_CSS_SYNTAX_CARRIAGE_RETURN_BLOCK(tkz, begin, data, end)
                break;

            /* U+000C FORM FEED (FF) */
            case 0x0C:
                LXB_CSS_SYNTAX_FORM_FEED_BLOCK(tkz, begin, data, end)
                break;

            case 0x09:
            case 0x20:
            case 0x0A:
                break;

            default:
                goto done;
        }

        data += 1;
    }
    while (data < end);

done:

    token->type = LXB_CSS_SYNTAX_TOKEN_WHITESPACE;

    lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

    return lxb_css_syntax_state_string_set(tkz, token, data);
}

/*
 * String token for U+0022 Quotation Mark (") and U+0027 Apostrophe (')
 */
const lxb_char_t *
lxb_css_syntax_state_string(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                            const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_char_t mark;
    const lxb_char_t *begin;
    lxb_codepoint_t cp;

    mark = *data++;
    begin = data;

    token->type = LXB_CSS_SYNTAX_TOKEN_STRING;

    while (data < end) {
        switch (*data) {
            /* U+0000 NULL */
            case 0x00:
                LXB_CSS_SYNTAX_NULL_BLOCK(tkz, begin, data, end)
                break;

            /*
             * U+000A LINE FEED
             * U+000D CARRIAGE RETURN
             * U+000C FORM FEED
             */
            case 0x0A:
            case 0x0D:
            case 0x0C:
                lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_NEINST);

                token->type = LXB_CSS_SYNTAX_TOKEN_BAD_STRING;

                goto done;

            /* U+005C REVERSE SOLIDUS (\) */
            case 0x5C:
                lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

                data += 1;
                begin = data;

                if (data >= end) {
                    data = lxb_css_syntax_state_escaped(tkz, data, end);
                    begin = data;
                    continue;
                }

                switch (*data) {
                    /* U+000D CARRIAGE RETURN (CR) */
                    case 0x0D:
                        /* U+000A LINE FEED (LF) */
                        if (data + 1 < end && data[1] == 0x0A) {
                            data += 1;
                        }

                        begin = data + 1;
                        break;

                    /* U+000C FORM FEED (FF) */
                    case 0x0A:
                    case 0x0C:
                        begin = data + 1;
                        break;

                    /* U+0000 NULL */
                    case 0x00:
                        LXB_CSS_SYNTAX_NULL_BLOCK(tkz, begin, data, end)
                        break;

                    default:
                        if (*data < 0x80) {
                            data = lxb_css_syntax_state_escaped(tkz, data, end);
                            begin = data;
                        }
                        else {
                            cp = lxb_css_syntax_state_decode_utf_8_up_80(&data, end);

                            if (cp == LXB_CSS_SYNTAX_ERROR_CODEPOINT) {
                                /* U+FFFD REPLACEMENT CHARACTER */
                                lxb_css_syntax_buffer_append_m(tkz, "\uFFFD",
                                                               lxb_css_syntax_replacement_length);
                                data += 1;
                                begin = data;
                            }
                            /* Surrogate U+D800 to U+DFFF. */
                            else if (cp >= 0xD800 && cp <= 0xDFFF) {
                                /* U+FFFD REPLACEMENT CHARACTER */
                                lxb_css_syntax_buffer_append_m(tkz, "\uFFFD",
                                                               lxb_css_syntax_replacement_length);
                                begin = data;
                            }
                        }

                        continue;
                }

                break;

            default:
                /* '"' or '\'' */
                if (*data == mark) {
                    lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

                    return lxb_css_syntax_state_string_set(tkz, token, data + 1);
                }

                if (*data >= 0x80) {
                    LXB_CSS_SYNTAX_UTF_8_UP_80_BLOCK(tkz, begin, data, end)
                    continue;
                }

                break;
        }

        data += 1;
    }

    lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                       LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINST);
done:

    lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

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
    lxb_char_t ch;
    const lxb_char_t *begin;

    data += 1;
    if (data >= end) {
        return lxb_css_syntax_state_delim_set(token, data - 1, '#');
    }

    if (lxb_css_syntax_res_name_map[*data] == 0x00) {
        if (*data == 0x00) {
            goto hash;
        }

        /* U+005C REVERSE SOLIDUS (\) */
        if (*data != 0x5C) {
            if (*data >= 0x80) {
                begin = data;

                if (lxb_css_syntax_state_start_ident_utf_8_80(&data, end)) {
                    data = begin;
                    goto hash;
                }
            }

            return lxb_css_syntax_state_delim_set(token, data - 1, '#');
        }

        data += 1;

        if (data < end) {
            ch = *data;

            if (ch == 0x0A || ch == 0x0C || ch == 0x0D) {
                return lxb_css_syntax_state_delim_set(token, data - 2, '#');
            }
        }

        data -= 1;
    }

hash:

    token->type = LXB_CSS_SYNTAX_TOKEN_HASH;

    return lxb_css_syntax_state_consume_ident(tkz, token, data, end);
}

/*
 * U+0028 LEFT PARENTHESIS (()
 */
const lxb_char_t *
lxb_css_syntax_state_lparenthesis(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS;
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
    /* Skip U+002B PLUS SIGN (+). */
    data += 1;

    if (data >= end) {
        return lxb_css_syntax_state_delim_set(token, data - 1, '+');
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        lxb_css_syntax_token_number(token)->have_sign = true;
        return lxb_css_syntax_state_consume_numeric(tkz, token, data, end);
    }

    /* U+002E FULL STOP (.) */
    if (*data == 0x2E) {
        data += 1;

        if (data >= end) {
            return lxb_css_syntax_state_delim_set(token, data - 2, '+');
        }

        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (*data >= 0x30 && *data <= 0x39) {
            lxb_css_syntax_token_number(token)->have_sign = true;
            return lxb_css_syntax_state_consume_numeric(tkz, token,
                                                        data - 1, end);
        }

        data -= 1;
    }

    return lxb_css_syntax_state_delim_set(token, data - 1, '+');
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
    return data + 1;
}

/*
 * U+002D HYPHEN-MINUS (-)
 */
const lxb_char_t *
lxb_css_syntax_state_minus(lxb_css_syntax_tokenizer_t *tkz,
                           lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token_number_t *number;

    /* Check for <number-token> */

    if (lxb_css_syntax_state_start_number(data, end)) {
        data = lxb_css_syntax_state_consume_numeric(tkz, token, data + 1, end);

        number = lxb_css_syntax_token_number(token);
        number->num = -number->num;

        lxb_css_syntax_token_number(token)->have_sign = true;

        return data;
    }

    /* Check for <CDC-token> */

    if (data + 3 <= end) {
        /* U+002D HYPHEN-MINUS U+003E GREATER-THAN SIGN (->) */
        if (data[1] == 0x2D && data[2] == 0x3E) {
            token->type = LXB_CSS_SYNTAX_TOKEN_CDC;
            return data + 3;
        }
    }

    if (lxb_css_syntax_state_start_ident_sequence(data, end)) {
        return lxb_css_syntax_state_ident_like_not_url(tkz, token,
                                                       data, end);
    }

    return lxb_css_syntax_state_delim_set(token, data, '-');
}

/*
 * U+002E FULL STOP (.)
 */
const lxb_char_t *
lxb_css_syntax_state_full_stop(lxb_css_syntax_tokenizer_t *tkz,
                               lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    if (lxb_css_syntax_state_start_number(data, end)) {
        lxb_css_syntax_token_number(token)->have_sign = false;
        return lxb_css_syntax_state_consume_numeric(tkz, token, data, end);
    }

    return lxb_css_syntax_state_delim_set(token, data, '.');
}

/*
 * U+003A COLON (:)
 */
const lxb_char_t *
lxb_css_syntax_state_colon(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_COLON;
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
    return data + 1;
}

/*
 * U+003C LESS-THAN SIGN (<)
 */
const lxb_char_t *
lxb_css_syntax_state_less_sign(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    if (data + 4 <= end) {
        /* U+0021 EXCLAMATION MARK
         * U+002D HYPHEN-MINUS
         * U+002D HYPHEN-MINUS
         * (!--)
         */
        if (data[1] == 0x21 && data[2] == 0x2D && data[3] == 0x2D) {
            token->type = LXB_CSS_SYNTAX_TOKEN_CDO;
            return data + 4;
        }
    }

    return lxb_css_syntax_state_delim_set(token, data, '<');
}

/*
 * U+0040 COMMERCIAL AT (@)
 */
const lxb_char_t *
lxb_css_syntax_state_at(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                        const lxb_char_t *data, const lxb_char_t *end)
{
    /* Skip U+0040 COMMERCIAL AT (@) */
    data += 1;

    if (lxb_css_syntax_state_start_ident_sequence(data, end)) {
        token->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;

        return lxb_css_syntax_state_consume_ident(tkz, token, data, end);
    }

    return lxb_css_syntax_state_delim_set(token, data - 1, '@');
}

/*
 * U+005B LEFT SQUARE BRACKET ([)
 */
const lxb_char_t *
lxb_css_syntax_state_ls_bracket(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_LS_BRACKET;
    return data + 1;
}

/*
 * U+005C REVERSE SOLIDUS (\)
 */
const lxb_char_t *
lxb_css_syntax_state_rsolidus(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                              const lxb_char_t *data, const lxb_char_t *end)
{
    if (lxb_css_syntax_state_valid_escape(data, end)) {
        return lxb_css_syntax_state_ident_like(tkz, token, data, end);
    }

    return lxb_css_syntax_state_delim_set(token, data, '\\');
}

/*
 * U+005D RIGHT SQUARE BRACKET (])
 */
const lxb_char_t *
lxb_css_syntax_state_rs_bracket(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    token->type = LXB_CSS_SYNTAX_TOKEN_RS_BRACKET;
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
    return data + 1;
}

/*
 * Numeric
 */
lxb_inline void
lxb_css_syntax_consume_numeric_set(lxb_css_syntax_tokenizer_t *tkz,
                                   lxb_css_syntax_token_t *token,
                                   const lxb_char_t *start, const lxb_char_t *end,
                                   bool is_float, bool e_is_negative,
                                   int exponent, int e_digit)
{
    if (e_is_negative) {
        exponent = e_digit - exponent;
        exponent = -exponent;
    }
    else {
        exponent = e_digit + exponent;
    }

    double num = lexbor_strtod_internal(start, (end - start), exponent);

    token->type = LXB_CSS_SYNTAX_TOKEN_NUMBER;

    lxb_css_syntax_token_number(token)->is_float = is_float;
    lxb_css_syntax_token_number(token)->num = num;
}

const lxb_char_t *
lxb_css_syntax_state_consume_before_numeric(lxb_css_syntax_tokenizer_t *tkz,
                                            lxb_css_syntax_token_t *token,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end)
{
    lxb_css_syntax_token_number(token)->have_sign = false;

    return lxb_css_syntax_state_consume_numeric(tkz, token, data, end);
}

static const lxb_char_t *
lxb_css_syntax_state_consume_numeric(lxb_css_syntax_tokenizer_t *tkz,
                                     lxb_css_syntax_token_t *token,
                                     const lxb_char_t *data,
                                     const lxb_char_t *end)
{
    bool e_is_negative, is_float;
    int exponent, e_digit;
    lxb_char_t ch, *buf_p;
    const lxb_char_t *begin, *buf_end;
    lxb_css_syntax_token_t *t_str;
    lxb_css_syntax_token_string_t *str;
    lxb_char_t buf[128];

    buf_p = buf;
    buf_end = buf + sizeof(buf);

    str = lxb_css_syntax_token_dimension_string(token);
    t_str = (lxb_css_syntax_token_t *) (void *) str;

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    while (*data >= 0x30 && *data <= 0x39) {
        if (buf_p != buf_end) {
            *buf_p++ = *data;
        }

        data += 1;

        if (data >= end) {
            lxb_css_syntax_consume_numeric_set(tkz, token, buf, buf_p,
                                               false, false, 0, 0);
            return data;
        }
    }

    exponent = 0;
    is_float = false;

    /* U+002E FULL STOP (.) */
    if (*data == 0x2E) {
        data += 1;

        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (data >= end || *data < 0x30 || *data > 0x39) {
            lxb_css_syntax_consume_numeric_set(tkz, token, buf, buf_p,
                                               false, false, 0, 0);
            return data - 1;
        }

        begin = buf_p;

        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        do {
            if (buf_p != buf_end) {
                *buf_p++ = *data;
            }

            data += 1;
        }
        while (data < end && *data >= 0x30 && *data <= 0x39);

        exponent = -(int) (buf_p - begin);
        is_float = true;

        if (data >= end) {
            lxb_css_syntax_consume_numeric_set(tkz, token, buf, buf_p,
                                               true, false, exponent, 0);
            return data;
        }
    }

    ch = *data;

    /* U+0045 Latin Capital Letter (E) or U+0065 Latin Small Letter (e) */
    if (ch != 0x45 && ch != 0x65) {
        lxb_css_syntax_consume_numeric_set(tkz, token, buf, buf_p,
                                           is_float, false, exponent, 0);

        return lxb_css_syntax_state_consume_numeric_name_start(tkz, token,
                                                               data, end);
    }

    lxb_css_syntax_token_base(t_str)->begin = data;

    data += 1;

    if (data >= end) {
        data -= 1;

        lxb_css_syntax_token_base(t_str)->length = 1;
        lxb_css_syntax_buffer_append_m(tkz, data, 1);

        lxb_css_syntax_consume_numeric_set(tkz, token, buf, buf_p,
                                           is_float, false, exponent, 0);

        token->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

        return lxb_css_syntax_state_dimension_set(tkz, token, data + 1);
    }

    e_is_negative = false;

    switch (*data) {
        /* U+002D HYPHEN-MINUS (-) */
        case 0x2D:
            e_is_negative = true;
            /* fall through */

        /* U+002B PLUS SIGN (+) */
        case 0x2B:
            data += 1;
            break;

        default:
            break;
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (data >= end || *data < 0x30 || *data > 0x39) {
        data -= 1;
        if (*data != ch) {
            data -= 1;
        }

        lxb_css_syntax_consume_numeric_set(tkz, token, buf, buf_p,
                                           is_float, false, exponent, 0);

        token->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

        begin = lxb_css_syntax_state_consume_ident(tkz, t_str, data, end);

        lxb_css_syntax_token_base(t_str)->begin = data;
        lxb_css_syntax_token_base(t_str)->length = begin - data;

        return begin;
    }

    e_digit = 0;

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    do {
        e_digit = (*data - 0x30) + e_digit * 0x0A;

        data += 1;

        if (data >= end) {
            lxb_css_syntax_consume_numeric_set(tkz, token, buf, buf_p,
                                               true, e_is_negative,
                                               exponent, e_digit);
            return data;
        }
    }
    while(*data >= 0x30 && *data <= 0x39);

    lxb_css_syntax_consume_numeric_set(tkz, token, buf, buf_p,
                                       true, e_is_negative,
                                       exponent, e_digit);

    return lxb_css_syntax_state_consume_numeric_name_start(tkz, token,
                                                           data, end);
}

static const lxb_char_t *
lxb_css_syntax_state_consume_numeric_name_start(lxb_css_syntax_tokenizer_t *tkz,
                                                lxb_css_syntax_token_t *token,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end)
{
    const lxb_char_t *p;
    lxb_css_syntax_token_t *t_str;
    lxb_css_syntax_token_string_t *str;

    if (lxb_css_syntax_state_start_ident_sequence(data, end)) {
        str = lxb_css_syntax_token_dimension_string(token);
        t_str = (lxb_css_syntax_token_t *) (void *) str;

        token->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

        p = lxb_css_syntax_state_consume_ident(tkz, t_str, data, end);

        lxb_css_syntax_token_base(t_str)->begin = data;
        lxb_css_syntax_token_base(t_str)->length = p - data;

        return p;
    }

    /* U+0025 PERCENTAGE SIGN (%) */
    if (*data == 0x25) {
        token->type = LXB_CSS_SYNTAX_TOKEN_PERCENTAGE;
        return data + 1;
    }

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_consume_ident(lxb_css_syntax_tokenizer_t *tkz,
                                   lxb_css_syntax_token_t *token,
                                   const lxb_char_t *data, const lxb_char_t *end)
{
    const lxb_char_t *begin;

    begin = data;

    while (data < end) {
        if (*data < 0x80) {
            if (lxb_css_syntax_res_name_map[*data] == 0x00) {
                /* U+005C REVERSE SOLIDUS (\) */
                if (*data == 0x5C) {
                    lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

                    data += 1;

                    if (data < end) {
                        if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                            data -= 1;
                            begin = data;
                            break;
                        }
                    }

                    data = lxb_css_syntax_state_escaped(tkz, data, end);
                    if (data == NULL) {
                        return NULL;
                    }

                    begin = data;
                    continue;
                }
                else if (*data == 0x00)
                    LXB_CSS_SYNTAX_NULL_BLOCK(tkz, begin, data, end)
                else {
                    break;
                }
            }

            data += 1;
        }
        else {
            LXB_CSS_SYNTAX_UTF_8_UP_80_BLOCK(tkz, begin, data, end)
            else if (!lxb_css_syntax_state_non_ascii(cp)) {
                break;
            }
        }
    }

    lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

    return lxb_css_syntax_state_string_set(tkz, token, data);
}

static const lxb_char_t *
lxb_css_syntax_state_consume_unicode_range(lxb_css_syntax_tokenizer_t *tkz,
                                           lxb_css_syntax_token_t *token,
                                           const lxb_char_t *data,
                                           const lxb_char_t *end)
{
    bool mq;
    unsigned count, question;
    lxb_codepoint_t cp, range_start, range_end;

    token->type = LXB_CSS_SYNTAX_TOKEN_UNICODE_RANGE;

    /* Skip [Uu]\+ */
    data += 2;
    mq = true;

again:

    cp = 0x0000;
    count = 6;

    while (data < end && count > 0) {
        if (lexbor_str_res_map_hex[*data] == LEXBOR_STR_RES_SLIP) {
            /* U+003F QUESTION MARK (?) */
            if (*data == 0x3F && mq) {
                question = 0;

                do {
                    question += 1;
                    count -= 1;
                    data += 1;
                }
                while (data < end && *data == 0x3F && count > 0);

                range_start = cp << (4 * question);
                range_end = range_start | ((1 << (4 * question)) - 1);

                lxb_css_syntax_token_unicode_range(token)->start = range_start;
                lxb_css_syntax_token_unicode_range(token)->end = range_end;

                return data;
            }

            break;
        }

        cp <<= 4;
        cp |= lexbor_str_res_map_hex[ *data ];

        count -= 1;
        data += 1;
    }

    if (mq) {
        mq = false;

        lxb_css_syntax_token_unicode_range(token)->start = cp;

        /* U+002D HYPHEN-MINUS (-) */
        if (data + 2 > end || data[0] != 0x2D
            || lexbor_str_res_map_hex[data[1]] == LEXBOR_STR_RES_SLIP)
        {
            lxb_css_syntax_token_unicode_range(token)->end = cp;
            return data;
        }

        /* Skip U+002D HYPHEN-MINUS (-) */
        data += 1;

        goto again;
    }

    lxb_css_syntax_token_unicode_range(token)->end = cp;

    return data;
}


const lxb_char_t *
lxb_css_syntax_state_ident_like(lxb_css_syntax_tokenizer_t *tkz,
                                lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_char_t ch;
    const lxb_char_t *begin;
    lxb_css_syntax_token_string_t *str;
    static const lxb_char_t url[] = "url";

    /* Would start a unicode-range. */
    /*
     * U+002B PLUS SIGN (+)
     * U+003F QUESTION MARK (?)
     */
    if (tkz->with_unicode_range && data + 3 <= end && data[1] == 0x2B
        && (lexbor_str_res_map_hex[ data[2] ] != 0xFF || data[2] == 0x3F))
    {
        return lxb_css_syntax_state_consume_unicode_range(tkz, token,
                                                          data, end);
    }

    data = lxb_css_syntax_state_consume_ident(tkz, token, data, end);
    if (data == NULL) {
        goto done;
    }

    if (data < end && *data == '(') {
        data += 1;
        str = lxb_css_syntax_token_string(token);

        if (str->length != 3 || !lexbor_str_data_casecmp(str->data, url)) {
            token->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;

            return data;
        }

        begin = data;

        while (data < end) {
            ch = *data;

            if (lexbor_utils_whitespace(ch, !=, &&)) {
                /* U+0022 QUOTATION MARK (") or U+0027 APOSTROPHE (') */
                if (ch == 0x22 || ch == 0x27) {
                    break;
                }

                tkz->pos = tkz->start;

                return lxb_css_syntax_state_url(tkz, token, data, end);
            }

            data++;
        }

        token->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;

        return begin;
    }

done:

    token->type = LXB_CSS_SYNTAX_TOKEN_IDENT;

    return data;
}

const lxb_char_t *
lxb_css_syntax_state_ident_like_not_url_start(lxb_css_syntax_tokenizer_t *tkz,
                                              lxb_css_syntax_token_t *token,
                                              const lxb_char_t *data, const lxb_char_t *end)
{
    const lxb_char_t *begin = data;

    if (lxb_css_syntax_state_start_ident_utf_8_80(&data, end)) {
        return lxb_css_syntax_state_ident_like_not_url(tkz, token, begin, end);
    }

    return lxb_css_syntax_state_delim_set_cp(token, begin, end);
}

const lxb_char_t *
lxb_css_syntax_state_ident_like_not_url(lxb_css_syntax_tokenizer_t *tkz,
                                        lxb_css_syntax_token_t *token,
                                        const lxb_char_t *data, const lxb_char_t *end)
{
    data = lxb_css_syntax_state_consume_ident(tkz, token, data, end);
    if (data == NULL) {
        return NULL;
    }

    if (data < end && *data == '(') {
        token->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;

        return data + 1;
    }

    token->type = LXB_CSS_SYNTAX_TOKEN_IDENT;

    return data;
}

const lxb_char_t *
lxb_css_syntax_state_ident_like_not_url_surrogate(lxb_css_syntax_tokenizer_t *tkz,
                                                  lxb_css_syntax_token_t *token,
                                                  const lxb_char_t *data, const lxb_char_t *end)
{
    const lxb_char_t *begin = data;

    if (data + 3 <= end) {
        lxb_codepoint_t srp = data[1] << 8 | data[2];

        /* Leading surrogate U+D800 to U+DFFF. */
        if (srp >= (0xA0 << 8 | 0x80) && srp <= (0xBF << 8 | 0xBF)) {
            return lxb_css_syntax_state_ident_like_not_url(tkz, token,
                                                           data, end);
        }
    }

    if (lxb_css_syntax_state_start_ident_utf_8_80(&data, end)) {
        return lxb_css_syntax_state_ident_like_not_url(tkz, token, begin, end);
    }

    return lxb_css_syntax_state_delim_set_cp(token, begin, end);
}

/*
 * URL
 */
static const lxb_char_t *
lxb_css_syntax_state_url(lxb_css_syntax_tokenizer_t *tkz,
                         lxb_css_syntax_token_t *token,
                         const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_char_t ch;
    const lxb_char_t *begin;

    *tkz->pos = 0x00;

    begin = data;

    while (data < end) {
        switch (*data) {
            /* U+0000 NULL */
            case 0x00:
                LXB_CSS_SYNTAX_NULL_BLOCK(tkz, begin, data, end)
                break;

            /* U+0029 RIGHT PARENTHESIS ()) */
            case 0x29:
                lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);
                data += 1;
                goto done;

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
                goto bad;

            /* U+005C REVERSE SOLIDUS (\) */
            case 0x5C:
                lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

                data += 1;

                if (data >= end) {
                    lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                      LXB_CSS_SYNTAX_TOKENIZER_ERROR_WRESINUR);
                }
                else {
                    ch = *data;

                    if (ch == 0x0A || ch == 0x0C || ch == 0x0D) {
                        lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                          LXB_CSS_SYNTAX_TOKENIZER_ERROR_WRESINUR);

                        return lxb_css_syntax_state_bad_url(tkz, token, data, end);
                    }
                }

                data = lxb_css_syntax_state_escaped(tkz, data, end);
                if (data == NULL) {
                    return NULL;
                }

                begin = data;
                continue;

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
                lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

                data += 1;

                while (data < end) {
                    ch = *data;

                    if (lexbor_utils_whitespace(ch, !=, &&)) {
                        /* U+0029 RIGHT PARENTHESIS ()) */
                        if (*data == 0x29) {
                            data += 1;
                            goto done;
                        }

                        return lxb_css_syntax_state_bad_url(tkz, token,
                                                            data, end);
                    }

                    data += 1;
                }

                lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                        LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINUR);

                return lxb_css_syntax_state_string_set(tkz, token, data);

            default:
                if (*data >= 0x80) {
                    LXB_CSS_SYNTAX_UTF_8_UP_80_BLOCK(tkz, begin, data, end)
                    continue;
                }

                /*
                 * Inclusive:
                 * U+0000 NULL and U+0008 BACKSPACE or
                 * U+000E SHIFT OUT and U+001F INFORMATION SEPARATOR ONE
                 */
                if ((*data <= 0x08) || (*data >= 0x0E && *data <= 0x1F)) {
                    goto bad;
                }

                break;
        }

        data += 1;
    }

    lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

    lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                       LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINUR);

done:

    token->type = LXB_CSS_SYNTAX_TOKEN_URL;

    return lxb_css_syntax_state_string_set(tkz, token, data);

bad:

    lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

    lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                       LXB_CSS_SYNTAX_TOKENIZER_ERROR_QOINUR);

    return lxb_css_syntax_state_bad_url(tkz, token, data, end);
}

/*
 * Bad URL
 */
static const lxb_char_t *
lxb_css_syntax_state_bad_url(lxb_css_syntax_tokenizer_t *tkz,
                             lxb_css_syntax_token_t *token,
                             const lxb_char_t *data, const lxb_char_t *end)
{
    const lxb_char_t *begin;

    token->type = LXB_CSS_SYNTAX_TOKEN_BAD_URL;

    begin = data;

    while (data < end) {
        /* U+0029 RIGHT PARENTHESIS ()) */
        if (*data == 0x29) {
            lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);
            return lxb_css_syntax_state_string_set(tkz, token, data + 1);
        }
        /* U+005C REVERSE SOLIDUS (\) */
        else if (*data == 0x5C) {
            data += 1;
            if (data >= end) {
                break;
            }
        }

        if (*data >= 0x80) {
            LXB_CSS_SYNTAX_UTF_8_UP_80_BLOCK(tkz, begin, data, end)
            continue;
        }

        data += 1;
    }

    lxb_css_syntax_buffer_append_m(tkz, begin, data - begin);

    return lxb_css_syntax_state_string_set(tkz, token, data);
}

static const lxb_char_t *
lxb_css_syntax_state_escaped(lxb_css_syntax_tokenizer_t *tkz,
                             const lxb_char_t *data, const lxb_char_t *end)
{
    unsigned count;
    lxb_codepoint_t cp;

    cp = 0;

    for (count = 0; count < 6; count++, data++) {
        if (data >= end) {
            if (count == 0) {
                lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                                   LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINES);
            }

            goto done;
        }

        if (lexbor_str_res_map_hex[*data] == 0xFF) {
            if (count == 0) {
                cp = (lxb_codepoint_t) *data++;
                goto done;
            }

            break;
        }

        cp <<= 4;
        cp |= lexbor_str_res_map_hex[*data];
    }

    if (data < end) {
        switch (*data) {
            case 0x0D:
                data += 1;

                if (data >= end) {
                    break;
                }

                if (*data == 0x0A) {
                    data += 1;
                }

                break;

            case 0x09:
            case 0x20:
            case 0x0A:
            case 0x0C:
                data += 1;
                break;

            default:
                break;
        }
    }

done:

    if ((tkz->end - tkz->pos) < 6) {
        if (lxb_css_syntax_string_realloc(tkz, 1024) != LXB_STATUS_OK) {
            return NULL;
        }
    }

    lxb_css_syntax_codepoint_to_ascii(tkz, cp);

    return data;
}

static bool
lxb_css_syntax_state_start_number(const lxb_char_t *data, const lxb_char_t *end)
{
    switch (*data) {
        /*
         * U+002B PLUS SIGN (+)
         * U+002D HYPHEN-MINUS (-)
         */
        case 0x2B:
        case 0x2D:
            data += 1;
            if (data >= end) {
                return false;
            }

            /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
            if (*data >= 0x30 && *data <= 0x39) {
                return true;
            }

            break;

        default:
            break;
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        return true;
    }

    /* U+002E FULL STOP (.) */
    if (*data == 0x2E) {
        data += 1;

        if (data >= end || *data < 0x30 || *data > 0x39) {
            return false;
        }

        return true;
    }

    return false;
}

static bool
lxb_css_syntax_state_start_ident_sequence(const lxb_char_t *data,
                                          const lxb_char_t *end)
{
    if (data >= end) {
        return false;
    }

    if (*data < 0x80) {
        /* U+002D HYPHEN-MINUS */
        if (*data == 0x2D) {
            data += 1;

            if (data >= end) {
                return false;
            }

            if (*data < 0x80) {
                /* U+002D HYPHEN-MINUS */
                if (*data == 0x2D
                    || lxb_css_syntax_res_name_map[*data] == LXB_CSS_SYNTAX_RES_NAME_START
                    || lxb_css_syntax_state_valid_escape(data, end)
                    || *data == 0x00)
                {
                    return true;
                }
            }
        }
        /* U+005C REVERSE SOLIDUS (\) */
        else if (*data == 0x5C) {
            return lxb_css_syntax_state_valid_escape(data, end);
        }

        return lxb_css_syntax_res_name_map[*data] == LXB_CSS_SYNTAX_RES_NAME_START
               || *data == 0x00;
    }

    return lxb_css_syntax_state_start_ident_utf_8_80(&data, end);
}

static lxb_codepoint_t
lxb_css_syntax_state_decode_utf_8_up_80(const lxb_char_t **data,
                                        const lxb_char_t *end)
{
    lxb_char_t ch;
    const lxb_char_t *p;
    lxb_codepoint_t cp;

    p = *data;
    ch = *p;

    if (ch <= 0xDF) {
        if (ch < 0xC2 || p + 1 >= end) {
            return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
        }

        cp = ch & 0x1F;

        if (p[1] < 0x80 || p[1] > 0xBF) {
            return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
        }

        cp = (cp << 6) | (p[1] & 0x3F);

        *data += 2;
    }
    else if (ch < 0xF0) {
        cp = ch & 0x0F;

        if (p + 2 >= end) {
            return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
        }

        if (ch == 0xE0) {
            if (p[1] < 0xA0 || p[1] > 0xBF) {
                return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
            }
        }

        /*
         * We returning surrogate pair!
         */
/*
        else if (ch == 0xED) {
            if (p[1] < 0x80 || p[1] > 0x9F) {
                return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
            }
        }
 */
        else if (p[1] < 0x80 || p[1] > 0xBF) {
            return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
        }

        if (p[2] < 0x80 || p[2] > 0xBF) {
            return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
        }

        cp = (cp << 6) | (p[1] & 0x3F);
        cp = (cp << 6) | (p[2] & 0x3F);

        *data += 3;
    }
    else if (ch < 0xF5) {
        cp = ch & 0x07;

        if (p + 3 >= end) {
            return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
        }

        if (ch == 0xF0) {
            if (p[1] < 0x90 || p[1] > 0xBF) {
                return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
            }
        }
        else if (ch == 0xF4) {
            if (p[1] < 0x80 || p[1] > 0x8F) {
                return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
            }
        }
        else if (p[1] < 0x80 || p[1] > 0xBF) {
            return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
        }

        if ((p[2] < 0x80 || p[2] > 0xBF) || (p[3] < 0x80 || p[3] > 0xBF)) {
            return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
        }

        cp = (cp << 6) | (p[1] & 0x3F);
        cp = (cp << 6) | (p[2] & 0x3F);
        cp = (cp << 6) | (p[3] & 0x3F);

        *data += 4;
    }
    else {
        return LXB_CSS_SYNTAX_ERROR_CODEPOINT;
    }

    return cp;
}
