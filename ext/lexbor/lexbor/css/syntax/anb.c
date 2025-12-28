/*
 * Copyright (C) 2021-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/conv.h"
#include "lexbor/core/serialize.h"
#include "lexbor/css/css.h"
#include "lexbor/css/parser.h"
#include "lexbor/css/syntax/anb.h"


static bool
lxb_css_syntax_anb_state(lxb_css_parser_t *parser,
                         const lxb_css_syntax_token_t *token, void *ctx);

static lxb_status_t
lxb_css_syntax_anb_end(lxb_css_parser_t *parser,
                       const lxb_css_syntax_token_t *token,
                       void *ctx, bool failed);

static lxb_css_log_message_t *
lxb_css_syntax_anb_fail(lxb_css_parser_t *parser,
                        const lxb_css_syntax_token_t *token);

static lxb_status_t
lxb_css_syntax_anb_state_ident(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_anb_t *anb);

static lxb_status_t
lxb_css_syntax_anb_state_ident_data(lxb_css_parser_t *parser,
                                    lxb_css_syntax_anb_t *anb,
                                    const lxb_css_syntax_token_t *token,
                                    const lxb_char_t *data,
                                    const lxb_char_t *end);


static const lxb_css_syntax_cb_pipe_t lxb_css_syntax_anb_pipe = {
    .state = lxb_css_syntax_anb_state,
    .block = NULL,
    .failed = lxb_css_state_failed,
    .end = lxb_css_syntax_anb_end
};


lxb_css_syntax_anb_t
lxb_css_syntax_anb_parse(lxb_css_parser_t *parser,
                         const lxb_char_t *data, size_t length)
{
    lxb_status_t status;
    lxb_css_syntax_anb_t anb;
    lxb_css_syntax_rule_t *rule;

    memset(&anb, 0, sizeof(lxb_css_syntax_anb_t));

    if (parser->stage != LXB_CSS_PARSER_CLEAN) {
        if (parser->stage == LXB_CSS_PARSER_RUN) {
            parser->status = LXB_STATUS_ERROR_WRONG_ARGS;
            return anb;
        }

        lxb_css_parser_clean(parser);
    }

    lxb_css_parser_buffer_set(parser, data, length);

    rule = lxb_css_syntax_parser_pipe_push(parser, NULL,
                                           &lxb_css_syntax_anb_pipe, &anb,
                                           LXB_CSS_SYNTAX_TOKEN_UNDEF);
    if (rule == NULL) {
        return anb;
    }

    parser->tkz->with_comment = false;
    parser->stage = LXB_CSS_PARSER_RUN;

    status = lxb_css_syntax_parser_run(parser);
    if (status != LXB_STATUS_OK) {
        /* Destroy. */
    }

    parser->stage = LXB_CSS_PARSER_END;

    return anb;
}

static bool
lxb_css_syntax_anb_state(lxb_css_parser_t *parser,
                         const lxb_css_syntax_token_t *token, void *ctx)
{
    parser->status = lxb_css_syntax_anb_handler(parser, token, ctx);

    token = lxb_css_syntax_parser_token(parser);
    if (token == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    if (parser->status != LXB_STATUS_OK
        || (token->type != LXB_CSS_SYNTAX_TOKEN__END))
    {
        (void) lxb_css_syntax_anb_fail(parser, token);
    }

    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_syntax_anb_end(lxb_css_parser_t *parser,
                       const lxb_css_syntax_token_t *token,
                       void *ctx, bool failed)
{
    return LXB_STATUS_OK;
}

static lxb_css_log_message_t *
lxb_css_syntax_anb_fail(lxb_css_parser_t *parser,
                        const lxb_css_syntax_token_t *token)
{
    parser->status = LXB_STATUS_ERROR_UNEXPECTED_DATA;

    static const char anb[] = "An+B";

    return lxb_css_syntax_token_error(parser, token, anb);
}

lxb_status_t
lxb_css_syntax_anb_handler(lxb_css_parser_t *parser,
                           const lxb_css_syntax_token_t *token,
                           lxb_css_syntax_anb_t *anb)
{
    const lxb_char_t *data, *end;
    lxb_css_syntax_token_ident_t *ident;

again:

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            if (lxb_css_syntax_token_dimension(token)->num.is_float) {
                return LXB_STATUS_ERROR_UNEXPECTED_DATA;
            }

            anb->a = lexbor_conv_double_to_long(lxb_css_syntax_token_dimension(token)->num.num);

            ident = lxb_css_syntax_token_dimension_string(token);

            goto ident;

        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            return lxb_css_syntax_anb_state_ident(parser, token, anb);

        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            if (lxb_css_syntax_token_number(token)->is_float) {
                return LXB_STATUS_ERROR_UNEXPECTED_DATA;
            }

            anb->a = 0;
            anb->b = lexbor_conv_double_to_long(lxb_css_syntax_token_number(token)->num);
            break;

        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            if (lxb_css_syntax_token_delim(token)->character != '+') {
                return LXB_STATUS_ERROR_UNEXPECTED_DATA;
            }

            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_status_m(parser, token);

            if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
                return LXB_STATUS_ERROR_UNEXPECTED_DATA;
            }

            anb->a = 1;

            ident = lxb_css_syntax_token_ident(token);

            goto ident;

        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_status_m(parser, token);
            goto again;

        default:
            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
    }

    lxb_css_syntax_parser_consume(parser);

    return LXB_STATUS_OK;

ident:

    data = ident->data;
    end = ident->data + ident->length;

    if (*data != 'n' && *data != 'N') {
        return LXB_STATUS_ERROR_UNEXPECTED_DATA;
    }

    data++;

    return lxb_css_syntax_anb_state_ident_data(parser, anb, token, data, end);
}

static lxb_status_t
lxb_css_syntax_anb_state_ident(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_anb_t *anb)
{
    size_t length;
    lxb_char_t c;
    const lxb_char_t *data, *end;
    lxb_css_syntax_token_ident_t *ident;

    static const lxb_char_t odd[] = "odd";
    static const lxb_char_t even[] = "even";

    ident = lxb_css_syntax_token_ident(token);

    length = ident->length;
    data = ident->data;
    end = data + length;

    c = *data++;

    /* 'n' or '-n' */

    if (c == 'n' || c == 'N') {
        anb->a = 1;
    }
    else if (c == '-') {
        if (data >= end) {
            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }

        c = *data++;

        if (c != 'n' && c != 'N') {
            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }

        anb->a = -1;
    }
    else if (length == sizeof(odd) - 1
             && lexbor_str_data_ncasecmp(ident->data, odd, sizeof(odd) - 1))
    {
        anb->a = 2;
        anb->b = 1;

        lxb_css_syntax_parser_consume(parser);
        return LXB_STATUS_OK;
    }
    else if (length == sizeof(even) - 1
             && lexbor_str_data_ncasecmp(ident->data, even, sizeof(even) - 1))
    {
        anb->a = 2;
        anb->b = 0;

        lxb_css_syntax_parser_consume(parser);
        return LXB_STATUS_OK;
    }
    else {
        return LXB_STATUS_ERROR_UNEXPECTED_DATA;
    }

    return lxb_css_syntax_anb_state_ident_data(parser, anb, token, data, end);
}

static lxb_status_t
lxb_css_syntax_anb_state_ident_data(lxb_css_parser_t *parser,
                                    lxb_css_syntax_anb_t *anb,
                                    const lxb_css_syntax_token_t *token,
                                    const lxb_char_t *data,
                                    const lxb_char_t *end)
{
    unsigned sign;
    lxb_char_t c;
    const lxb_char_t *p;

    sign = 0;

    if (data >= end) {
        lxb_css_syntax_parser_consume(parser);
        lxb_css_parser_token_status_wo_ws_m(parser, token);

        switch (token->type) {
            case LXB_CSS_SYNTAX_TOKEN_NUMBER:
                if (!lxb_css_syntax_token_number(token)->have_sign) {
                    anb->b = 0;
                    return LXB_STATUS_OK;
                }

                break;

            case LXB_CSS_SYNTAX_TOKEN_DELIM:
                c = lxb_css_syntax_token_delim(token)->character;

                switch (c) {
                    case '-':
                        sign = 1;
                        break;

                    case '+':
                        sign = 2;
                        break;

                    default:
                        anb->b = 0;
                        return LXB_STATUS_OK;
                }

                lxb_css_syntax_parser_consume(parser);
                lxb_css_parser_token_status_wo_ws_m(parser, token);

                break;

            case LXB_CSS_SYNTAX_TOKEN__EOF:
                anb->b = 0;
                return LXB_STATUS_OK;

            default:
                anb->b = 0;
                return LXB_STATUS_OK;
        }

        goto number;
    }

    c = *data++;

    if (c != '-') {
        return LXB_STATUS_ERROR_UNEXPECTED_DATA;
    }

    if (data < end) {
        p = data;
        anb->b = -lexbor_conv_data_to_long(&data, end - data);

        if (anb->b > 0 || data == p || data < end) {
            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }

        goto done;
    }

    sign = 1;

    lxb_css_syntax_parser_consume(parser);
    lxb_css_parser_token_status_wo_ws_m(parser, token);

number:

    if (token->type != LXB_CSS_SYNTAX_TOKEN_NUMBER) {
        return LXB_STATUS_ERROR_UNEXPECTED_DATA;
    }

    if (lxb_css_syntax_token_number(token)->is_float
        || (sign > 0 && lxb_css_syntax_token_number(token)->have_sign))
    {
        return LXB_STATUS_ERROR_UNEXPECTED_DATA;
    }

    anb->b = lexbor_conv_double_to_long(lxb_css_syntax_token_number(token)->num);

    if (sign == 1) {
        anb->b = -anb->b;
    }

done:

    lxb_css_syntax_parser_consume(parser);

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_syntax_anb_serialize(lxb_css_syntax_anb_t *anb,
                             lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_char_t buf[128];
    lxb_char_t *p, *end;

    if (anb == NULL) {
        return LXB_STATUS_OK;
    }

    static const lxb_char_t odd[] = "odd";
    static const lxb_char_t even[] = "even";

    if (anb->a == 2) {
        if (anb->b == 1) {
            return cb(odd, sizeof(odd) - 1, ctx);
        }

        if (anb->b == 0) {
            return cb(even, sizeof(even) - 1, ctx);
        }
    }

    p = buf;
    end = p + sizeof(buf);

    if (anb->a == 1) {
        *p = '+';
        p++;
    }
    else if (anb->a == -1) {
        *p = '-';
        p++;
    }
    else {
        p += lexbor_conv_float_to_data((double) anb->a, p, end - p);
        if (p >= end) {
            return LXB_STATUS_ERROR_SMALL_BUFFER;
        }
    }

    *p = 'n';
    p++;

    if (p >= end) {
        return cb(buf, p - buf, ctx);
    }

    if (anb->b == 0) {
        return cb(buf, p - buf, ctx);
    }

    if (anb->b > 0) {
        *p = '+';
        p++;

        if (p >= end) {
            return LXB_STATUS_ERROR_SMALL_BUFFER;
        }
    }

    p += lexbor_conv_float_to_data((double) anb->b, p, end - p);

    return cb(buf, p - buf, ctx);
}

lxb_char_t *
lxb_css_syntax_anb_serialize_char(lxb_css_syntax_anb_t *anb, size_t *out_length)
{
    size_t length = 0;
    lxb_status_t status;
    lexbor_str_t str;

    status = lxb_css_syntax_anb_serialize(anb, lexbor_serialize_length_cb,
                                          &length);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    /* + 1 == '\0' */
    str.data = lexbor_malloc(length + 1);
    if (str.data == NULL) {
        goto failed;
    }

    str.length = 0;

    status = lxb_css_syntax_anb_serialize(anb, lexbor_serialize_copy_cb, &str);
    if (status != LXB_STATUS_OK) {
        lexbor_free(str.data);
        goto failed;
    }

    str.data[str.length] = '\0';

    if (out_length != NULL) {
        *out_length = str.length;
    }

    return str.data;

failed:

    if (out_length != NULL) {
        *out_length = 0;
    }

    return NULL;
}
