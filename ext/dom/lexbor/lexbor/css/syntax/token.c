/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/shs.h"
#include "lexbor/core/conv.h"
#include "lexbor/core/serialize.h"
#include "lexbor/core/print.h"

#include "lexbor/css/parser.h"
#include "lexbor/css/syntax/token.h"
#include "lexbor/css/syntax/state.h"
#include "lexbor/css/syntax/state_res.h"

#define LXB_CSS_SYNTAX_TOKEN_RES_NAME_SHS_MAP
#include "lexbor/css/syntax/token_res.h"

#define LEXBOR_STR_RES_MAP_HEX
#define LEXBOR_STR_RES_ANSI_REPLACEMENT_CHARACTER
#include "lexbor/core/str_res.h"


lxb_css_syntax_token_t *
lxb_css_syntax_tokenizer_token(lxb_css_syntax_tokenizer_t *tkz);

lxb_status_t
lxb_css_syntax_tokenizer_cache_push(lxb_css_syntax_tokenizer_cache_t *cache,
                                    lxb_css_syntax_token_t *value);


typedef struct {
    lexbor_str_t  *str;
    lexbor_mraw_t *mraw;
}
lxb_css_syntax_token_ctx_t;


static lxb_status_t
lxb_css_syntax_token_str_cb(const lxb_char_t *data, size_t len, void *ctx);


lxb_css_syntax_token_t *
lxb_css_syntax_token(lxb_css_syntax_tokenizer_t *tkz)
{
    if (tkz->cache_pos < tkz->cache->length
        && (tkz->prepared == 0 || tkz->cache_pos < tkz->prepared))
    {
        return tkz->cache->list[tkz->cache_pos];
    }

    return lxb_css_syntax_tokenizer_token(tkz);
}

lxb_css_syntax_token_t *
lxb_css_syntax_token_next(lxb_css_syntax_tokenizer_t *tkz)
{
    return lxb_css_syntax_tokenizer_token(tkz);
}

void
lxb_css_syntax_token_consume(lxb_css_syntax_tokenizer_t *tkz)
{
    lxb_css_syntax_token_t *token;

    if (tkz->cache_pos < tkz->cache->length) {
        if (tkz->prepared != 0 && tkz->cache_pos >= tkz->prepared) {
            return;
        }

        token = tkz->cache->list[tkz->cache_pos];

        lxb_css_syntax_token_string_free(tkz, token);
        lexbor_dobject_free(tkz->tokens, token);

        tkz->cache_pos += 1;

        if (tkz->cache_pos >= tkz->cache->length) {
            tkz->cache->length = 0;
            tkz->cache_pos = 0;
        }
    }
}

void
lxb_css_syntax_token_consume_n(lxb_css_syntax_tokenizer_t *tkz, unsigned count)
{
    while (count != 0) {
        count--;
        lxb_css_syntax_token_consume(tkz);
    }
}

lxb_status_t
lxb_css_syntax_token_string_dup(lxb_css_syntax_token_string_t *token,
                                lexbor_str_t *str, lexbor_mraw_t *mraw)
{
    size_t length;

    length = token->length + 1;

    if (length > str->length) {
        if (str->data == NULL) {
            str->data = lexbor_mraw_alloc(mraw, length);
            if (str->data == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }

            str->length = 0;
        }
        else {
            if (lexbor_str_realloc(str, mraw, length) == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }
        }
    }

    /* + 1 = '\0' */
    memcpy(str->data, token->data, length);

    str->length = token->length;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_syntax_token_string_make(lxb_css_syntax_tokenizer_t *tkz,
                                 lxb_css_syntax_token_t *token)
{
    lxb_char_t *data;
    lxb_css_syntax_token_string_t *token_string;

    if (token->type >= LXB_CSS_SYNTAX_TOKEN_IDENT
        && token->type <= LXB_CSS_SYNTAX_TOKEN_WHITESPACE)
    {
        token_string = lxb_css_syntax_token_string(token);
        goto copy;
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_DIMENSION) {
        token_string = lxb_css_syntax_token_dimension_string(token);
        goto copy;
    }

    return LXB_STATUS_OK;

copy:

    data = lexbor_mraw_alloc(tkz->mraw, token_string->length + 1);
    if (data == NULL) {
        tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        return tkz->status;
    }

    /* + 1 = '\0' */
    memcpy(data, token_string->data, token_string->length + 1);

    token_string->data = data;
    token->cloned = true;

    return LXB_STATUS_OK;
}

lxb_css_syntax_token_t *
lxb_css_syntax_token_cached_create(lxb_css_syntax_tokenizer_t *tkz)
{
    lxb_status_t status;
    lxb_css_syntax_token_t *token;

    token = lexbor_dobject_alloc(tkz->tokens);
    if (token == NULL) {
        tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }

    status = lxb_css_syntax_tokenizer_cache_push(tkz->cache, token);
    if (status != LXB_STATUS_OK) {
        tkz->status = status;
        return NULL;
    }

    token->cloned = false;

    return token;
}


void
lxb_css_syntax_token_string_free(lxb_css_syntax_tokenizer_t *tkz,
                                 lxb_css_syntax_token_t *token)
{
    lxb_css_syntax_token_string_t *token_string;

    if (token->cloned) {
        if (token->type == LXB_CSS_SYNTAX_TOKEN_DIMENSION) {
            token_string = lxb_css_syntax_token_dimension_string(token);
        }
        else {
            token_string = lxb_css_syntax_token_string(token);
        }

        lexbor_mraw_free(tkz->mraw, (lxb_char_t *) token_string->data);
    }
}

const lxb_char_t *
lxb_css_syntax_token_type_name_by_id(lxb_css_syntax_token_type_t type)
{
    switch (type) {
        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            return (lxb_char_t *) "ident";
        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
            return (lxb_char_t *) "function";
        case LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD:
            return (lxb_char_t *) "at-keyword";
        case LXB_CSS_SYNTAX_TOKEN_HASH:
            return (lxb_char_t *) "hash";
        case LXB_CSS_SYNTAX_TOKEN_STRING:
            return (lxb_char_t *) "string";
        case LXB_CSS_SYNTAX_TOKEN_BAD_STRING:
            return (lxb_char_t *) "bad-string";
        case LXB_CSS_SYNTAX_TOKEN_URL:
            return (lxb_char_t *) "url";
        case LXB_CSS_SYNTAX_TOKEN_BAD_URL:
            return (lxb_char_t *) "bad-url";
        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            return (lxb_char_t *) "delim";
        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            return (lxb_char_t *) "number";
        case LXB_CSS_SYNTAX_TOKEN_PERCENTAGE:
            return (lxb_char_t *) "percentage";
        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            return (lxb_char_t *) "dimension";
        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            return (lxb_char_t *) "whitespace";
        case LXB_CSS_SYNTAX_TOKEN_CDO:
            return (lxb_char_t *) "CDO";
        case LXB_CSS_SYNTAX_TOKEN_CDC:
            return (lxb_char_t *) "CDC";
        case LXB_CSS_SYNTAX_TOKEN_COLON:
            return (lxb_char_t *) "colon";
        case LXB_CSS_SYNTAX_TOKEN_SEMICOLON:
            return (lxb_char_t *) "semicolon";
        case LXB_CSS_SYNTAX_TOKEN_COMMA:
            return (lxb_char_t *) "comma";
        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            return (lxb_char_t *) "left-square-bracket";
        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
            return (lxb_char_t *) "right-square-bracket";
        case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
            return (lxb_char_t *) "left-parenthesis";
        case LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS:
            return (lxb_char_t *) "right-parenthesis";
        case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
            return (lxb_char_t *) "left-curly-bracket";
        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
            return (lxb_char_t *) "right-curly-bracket";
        case LXB_CSS_SYNTAX_TOKEN_COMMENT:
            return (lxb_char_t *) "comment";
        case LXB_CSS_SYNTAX_TOKEN__EOF:
            return (lxb_char_t *) "end-of-file";
        case LXB_CSS_SYNTAX_TOKEN__END:
            return (lxb_char_t *) "end";
        default:
            return (lxb_char_t *) "undefined";
    }
}

lxb_css_syntax_token_type_t
lxb_css_syntax_token_type_id_by_name(const lxb_char_t *type_name, size_t len)
{
    const lexbor_shs_entry_t *entry;

    entry = lexbor_shs_entry_get_lower_static(lxb_css_syntax_token_res_name_shs_map,
                                              type_name, len);
    if (entry == NULL) {
        return LXB_CSS_SYNTAX_TOKEN_UNDEF;
    }

    return (lxb_css_syntax_token_type_t) (uintptr_t) entry->value;
}


lxb_status_t
lxb_css_syntax_token_serialize(const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_token_cb_f cb, void *ctx)
{
    size_t len;
    lxb_status_t status;
    lxb_char_t buf[128];
    const lxb_css_syntax_token_string_t *str;
    const lxb_css_syntax_token_dimension_t *dim;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            buf[0] = token->types.delim.character;
            buf[1] = 0x00;

            return cb(buf, 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            len = lexbor_conv_float_to_data(token->types.number.num,
                                            buf, (sizeof(buf) - 1));

            buf[len] = 0x00;

            return cb(buf, len, ctx);

        case LXB_CSS_SYNTAX_TOKEN_PERCENTAGE:
            len = lexbor_conv_float_to_data(token->types.number.num,
                                            buf, (sizeof(buf) - 1));

            buf[len] = 0x00;

            status = cb(buf, len, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return cb((lxb_char_t *) "%", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_CDO:
            return cb((lxb_char_t *) "<!--", 4, ctx);

        case LXB_CSS_SYNTAX_TOKEN_CDC:
            return cb((lxb_char_t *) "-->", 3, ctx);

        case LXB_CSS_SYNTAX_TOKEN_COLON:
            return cb((lxb_char_t *) ":", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_SEMICOLON:
            return cb((lxb_char_t *) ";", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_COMMA:
            return cb((lxb_char_t *) ",", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            return cb((lxb_char_t *) "[", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
            return cb((lxb_char_t *) "]", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
            return cb((lxb_char_t *) "(", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS:
            return cb((lxb_char_t *) ")", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
            return cb((lxb_char_t *) "{", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
            return cb((lxb_char_t *) "}", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_HASH:
            status = cb((lxb_char_t *) "#", 1, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            str = &token->types.string;

            return cb(str->data, str->length, ctx);

        case LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD:
            status = cb((lxb_char_t *) "@", 1, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            str = &token->types.string;

            return cb(str->data, str->length, ctx);

        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            str = &token->types.string;

            return cb(str->data, str->length, ctx);

        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
            str = &token->types.string;

            status = cb(str->data, str->length, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return cb((lxb_char_t *) "(", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_STRING:
        case LXB_CSS_SYNTAX_TOKEN_BAD_STRING: {
            status = cb((lxb_char_t *) "\"", 1, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            const lxb_char_t *begin = token->types.string.data;
            const lxb_char_t *end = begin + token->types.string.length;

            const lxb_char_t *ptr = begin;

            for (; begin < end; begin++) {
                /* 0x5C; '\'; Inverse/backward slash */
                if (*begin == 0x5C) {
                    begin += 1;

                    status = cb(ptr, (begin - ptr), ctx);
                    if (status != LXB_STATUS_OK) {
                        return status;
                    }

                    if (begin == end) {
                        status = cb((const lxb_char_t *) "\\", 1, ctx);
                        if (status != LXB_STATUS_OK) {
                            return status;
                        }

                        ptr = begin;

                        break;
                    }

                    begin -= 1;
                    ptr = begin;
                }
                /* 0x22; '"'; Only quotes above */
                else if (*begin == 0x22) {
                    if (ptr != begin) {
                        status = cb(ptr, (begin - ptr), ctx);
                        if (status != LXB_STATUS_OK) {
                            return status;
                        }
                    }

                    status = cb((const lxb_char_t *) "\\", 1, ctx);
                    if (status != LXB_STATUS_OK) {
                        return status;
                    }

                    ptr = begin;
                }
            }

            if (ptr != begin) {
                status = cb(ptr, (begin - ptr), ctx);
                if (status != LXB_STATUS_OK) {
                    return status;
                }
            }

            return cb((const lxb_char_t *) "\"", 1, ctx);
        }

        case LXB_CSS_SYNTAX_TOKEN_URL:
        case LXB_CSS_SYNTAX_TOKEN_BAD_URL:
            status = cb((lxb_char_t *) "url(", 4, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            str = &token->types.string;

            status = cb(str->data, str->length, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return cb((lxb_char_t *) ")", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_COMMENT:
            status = cb((lxb_char_t *) "/*", 2, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            str = &token->types.string;

            status = cb(str->data, str->length, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return cb((lxb_char_t *) "*/", 2, ctx);

        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            len = lexbor_conv_float_to_data(token->types.number.num,
                                            buf, (sizeof(buf) - 1));

            buf[len] = 0x00;

            status = cb(buf, len, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            dim = &token->types.dimension;

            return cb(dim->str.data, dim->str.length, ctx);

        case LXB_CSS_SYNTAX_TOKEN__EOF:
            return cb((lxb_char_t *) "END-OF-FILE", 11, ctx);

        case LXB_CSS_SYNTAX_TOKEN__END:
            return cb((lxb_char_t *) "END", 3, ctx);

        default:
            return LXB_STATUS_ERROR;
    }
}

lxb_status_t
lxb_css_syntax_token_serialize_str(const lxb_css_syntax_token_t *token,
                                   lexbor_str_t *str, lexbor_mraw_t *mraw)
{
    lxb_css_syntax_token_ctx_t ctx;

    ctx.str = str;
    ctx.mraw = mraw;

    if (str->data == NULL) {
        lexbor_str_init(str, mraw, 1);
        if (str->data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    return lxb_css_syntax_token_serialize(token, lxb_css_syntax_token_str_cb,
                                          &ctx);
}

static lxb_status_t
lxb_css_syntax_token_str_cb(const lxb_char_t *data, size_t len, void *cb_ctx)
{
    lxb_char_t *ptr;
    lxb_css_syntax_token_ctx_t *ctx = (lxb_css_syntax_token_ctx_t *) cb_ctx;

    ptr = lexbor_str_append(ctx->str, ctx->mraw, data, len);
    if (ptr == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_OK;
}


lxb_char_t *
lxb_css_syntax_token_serialize_char(const lxb_css_syntax_token_t *token,
                                    size_t *out_length)
{
    size_t length = 0;
    lxb_status_t status;
    lexbor_str_t str;

    status = lxb_css_syntax_token_serialize(token, lexbor_serialize_length_cb,
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

    status = lxb_css_syntax_token_serialize(token, lexbor_serialize_copy_cb,
                                            &str);
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

lxb_css_log_message_t *
lxb_css_syntax_token_error(lxb_css_parser_t *parser,
                           const lxb_css_syntax_token_t *token,
                           const char *module_name)
{
    lxb_char_t *name;
    lxb_css_log_message_t *msg;

    static const char unexpected[] = "%s. Unexpected token: %s";

    name = lxb_css_syntax_token_serialize_char(token, NULL);
    if (name == NULL) {
        return NULL;
    }

    msg = lxb_css_log_format(parser->log, LXB_CSS_LOG_SYNTAX_ERROR, unexpected,
                             module_name, name);

    lexbor_free(name);

    return msg;
}

/*
 * No inline functions for ABI.
 */
lxb_css_syntax_token_t *
lxb_css_syntax_token_create_noi(lexbor_dobject_t *dobj)
{
    return lxb_css_syntax_token_create(dobj);
}

void
lxb_css_syntax_token_clean_noi(lxb_css_syntax_token_t *token)
{
    lxb_css_syntax_token_clean(token);
}

lxb_css_syntax_token_t *
lxb_css_syntax_token_destroy_noi(lxb_css_syntax_token_t *token,
                                 lexbor_dobject_t *dobj)
{
    return lxb_css_syntax_token_destroy(token, dobj);
}

const lxb_char_t *
lxb_css_syntax_token_type_name_noi(lxb_css_syntax_token_t *token)
{
    return lxb_css_syntax_token_type_name(token);
}

lxb_css_syntax_token_type_t
lxb_css_syntax_token_type_noi(lxb_css_syntax_token_t *token)
{
    return lxb_css_syntax_token_type(token);
}
