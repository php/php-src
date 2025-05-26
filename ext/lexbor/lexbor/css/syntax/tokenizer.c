/*
 * Copyright (C) 2018-2025 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/syntax/tokenizer.h"
#include "lexbor/css/syntax/tokenizer/error.h"
#include "lexbor/css/syntax/state.h"
#include "lexbor/css/syntax/state_res.h"
#include "lexbor/css/syntax/token.h"

#include "lexbor/core/array.h"

#define LEXBOR_STR_RES_MAP_LOWERCASE
#include "lexbor/core/str_res.h"


static const lxb_char_t lxb_css_syntax_tokenizer_important[] = "important";


static bool
lxb_css_syntax_tokenizer_lookup_important_ch(lxb_css_syntax_tokenizer_t *tkz,
                                             const lxb_char_t *p,
                                             const lxb_char_t *end,
                                             const lxb_char_t stop_ch);

static bool
lxb_css_syntax_tokenizer_lookup_important_end(lxb_css_syntax_tokenizer_t *tkz,
                                              const lxb_char_t *p,
                                              const lxb_char_t *end,
                                              const lxb_char_t stop_ch);

lxb_css_syntax_tokenizer_t *
lxb_css_syntax_tokenizer_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_css_syntax_tokenizer_t));
}

lxb_status_t
lxb_css_syntax_tokenizer_init(lxb_css_syntax_tokenizer_t *tkz)
{
    lxb_status_t status;
    static const unsigned tmp_size = 1024;

    if (tkz == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    /* Tokens. */

    tkz->tokens = lexbor_dobject_create();
    status = lexbor_dobject_init(tkz->tokens, 128,
                                 sizeof(lxb_css_syntax_token_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Memory for text. */

    tkz->mraw = lexbor_mraw_create();
    status = lexbor_mraw_init(tkz->mraw, 4096);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Temp */
    tkz->start = lexbor_malloc(tmp_size);
    if (tkz->start == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    tkz->pos = tkz->start;
    tkz->end = tkz->start + tmp_size;

    /* Parse errors */
    tkz->parse_errors = lexbor_array_obj_create();
    status = lexbor_array_obj_init(tkz->parse_errors, 16,
                                   sizeof(lxb_css_syntax_tokenizer_error_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    tkz->first = NULL;
    tkz->last = NULL;

    tkz->offset = 0;

    tkz->with_comment = false;
    tkz->with_unicode_range = false;
    tkz->status = LXB_STATUS_OK;
    tkz->opt = LXB_CSS_SYNTAX_TOKENIZER_OPT_UNDEF;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_syntax_tokenizer_clean(lxb_css_syntax_tokenizer_t *tkz)
{
    lexbor_mraw_clean(tkz->mraw);
    lexbor_array_obj_clean(tkz->parse_errors);
    lexbor_dobject_clean(tkz->tokens);

    tkz->in_begin = NULL;
    tkz->in_p = NULL;
    tkz->in_end = NULL;
    tkz->first = NULL;
    tkz->last = NULL;
    tkz->pos = tkz->start;
    tkz->offset = 0;
    tkz->status = LXB_STATUS_OK;

    return LXB_STATUS_OK;
}

lxb_css_syntax_tokenizer_t *
lxb_css_syntax_tokenizer_destroy(lxb_css_syntax_tokenizer_t *tkz)
{
    if (tkz == NULL) {
        return NULL;
    }

    if (tkz->tokens != NULL) {
        tkz->tokens = lexbor_dobject_destroy(tkz->tokens, true);
    }

    tkz->mraw = lexbor_mraw_destroy(tkz->mraw, true);
    tkz->parse_errors = lexbor_array_obj_destroy(tkz->parse_errors, true);

    if (tkz->start != NULL) {
        tkz->start = lexbor_free(tkz->start);
    }

    return lexbor_free(tkz);
}

lxb_css_syntax_token_t *
lxb_css_syntax_tokenizer_token(lxb_css_syntax_tokenizer_t *tkz)
{
    lxb_status_t status;
    lxb_css_syntax_token_t *token;
    const lxb_char_t *begin, *end;

    begin = tkz->in_p;
    end = tkz->in_end;

    token = lexbor_dobject_calloc(tkz->tokens);
    if (token == NULL) {
        tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }

    if (tkz->first == NULL) {
        tkz->first = token;
    }

    if (tkz->last != NULL) {
        status = lxb_css_syntax_token_string_make(tkz, tkz->last);
        if (status != LXB_STATUS_OK) {
            return NULL;
        }

        tkz->last->next = token;
    }

    tkz->last = token;

again:

    lxb_css_syntax_token_base(token)->begin = begin;

    if (begin < end) {
        begin = lxb_css_syntax_state_res_map[*begin](tkz, token, begin, end);
        if (begin == NULL) {
            return NULL;
        }
    }
    else {
        token->type = LXB_CSS_SYNTAX_TOKEN__EOF;
    }

    token->offset = tkz->offset;
    lxb_css_syntax_token_base(token)->length = begin - tkz->in_p;
    tkz->offset += lxb_css_syntax_token_base(token)->length;

    tkz->in_p = begin;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_COMMENT && !tkz->with_comment) {
        goto again;
    }

    return token;
}

bool
lxb_css_syntax_tokenizer_lookup_colon(lxb_css_syntax_tokenizer_t *tkz)
{
    const lxb_char_t *p, *end;
    lxb_css_syntax_token_t *token;

    if (tkz->first != NULL && tkz->first->next != NULL) {
        token = tkz->first->next;

        if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
            if (token->next != NULL) {
                return token->next->type == LXB_CSS_SYNTAX_TOKEN_COLON;
            }
        }
        else if (token->type == LXB_CSS_SYNTAX_TOKEN_COLON) {
            return true;
        }

        return false;
    }

    p = tkz->in_p;
    end = tkz->in_end;

    while (p < end) {
        switch (*p) {
            case 0x3A:
                return true;

            case 0x0D:
            case 0x0C:
            case 0x09:
            case 0x20:
            case 0x0A:
                p += 1;
                break;

            default:
                return false;
        }
    }

    return false;
}

bool
lxb_css_syntax_tokenizer_lookup_important(lxb_css_syntax_tokenizer_t *tkz,
                                          lxb_css_syntax_token_type_t stop,
                                          const lxb_char_t stop_ch)
{
    const lxb_char_t *p, *end;
    lxb_css_syntax_token_t *token;

    static const size_t length = sizeof(lxb_css_syntax_tokenizer_important) - 1;

    p = tkz->in_p;
    end = tkz->in_end;

    if (tkz->first != NULL && tkz->first->next != NULL) {
        token = tkz->first->next;

        if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
            return false;
        }

        if (!(lxb_css_syntax_token_ident(token)->length == length
              && lexbor_str_data_ncasecmp(lxb_css_syntax_token_ident(token)->data,
                                          lxb_css_syntax_tokenizer_important,
                                          length)))
        {
            return false;
        }

        if (token->next != NULL) {
            token = token->next;

            if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
                if (token->next == NULL) {
                    return lxb_css_syntax_tokenizer_lookup_important_end(tkz,
                                                              p, end, stop_ch);
                }

                token = token->next;
            }

            return (token->type == LXB_CSS_SYNTAX_TOKEN_SEMICOLON
                    || token->type == stop
                    || token->type == LXB_CSS_SYNTAX_TOKEN__EOF);
        }

        return lxb_css_syntax_tokenizer_lookup_important_end(tkz, p, end,
                                                             stop_ch);
    }

    return lxb_css_syntax_tokenizer_lookup_important_ch(tkz, p, end, stop_ch);
}

static bool
lxb_css_syntax_tokenizer_lookup_important_ch(lxb_css_syntax_tokenizer_t *tkz,
                                             const lxb_char_t *p,
                                             const lxb_char_t *end,
                                             const lxb_char_t stop_ch)
{
    static const size_t length = sizeof(lxb_css_syntax_tokenizer_important) - 1;

    if (!(end - p >= length
           && lexbor_str_data_ncasecmp(p, lxb_css_syntax_tokenizer_important,
                                       length)))
    {
        return false;
    }

    return lxb_css_syntax_tokenizer_lookup_important_end(tkz, p + length,
                                                         end, stop_ch);
}

static bool
lxb_css_syntax_tokenizer_lookup_important_end(lxb_css_syntax_tokenizer_t *tkz,
                                              const lxb_char_t *p,
                                              const lxb_char_t *end,
                                              const lxb_char_t stop_ch)
{
    while (p < end) {
        switch (*p) {
            case 0x3B:
                return true;

            case 0x0D:
            case 0x0C:
            case 0x09:
            case 0x20:
            case 0x0A:
                p += 1;
                break;

            default:
                return (stop_ch != 0x00 && stop_ch == *p);
        }
    }

    /* EOF */
    return true;
}

bool
lxb_css_syntax_tokenizer_lookup_declaration_ws_end(lxb_css_syntax_tokenizer_t *tkz,
                                                   lxb_css_syntax_token_type_t stop,
                                                   const lxb_char_t stop_ch)
{
    lxb_css_syntax_token_t *token;
    const lxb_char_t *p, *end;

    if (tkz->first != NULL && tkz->first->next) {
        token = tkz->first->next;

        switch (token->type) {
            case LXB_CSS_SYNTAX_TOKEN_DELIM:
                if (lxb_css_syntax_token_delim(token)->character != '!') {
                    return lxb_css_syntax_tokenizer_lookup_important(tkz, stop,
                                                                     stop_ch);
                }

                return false;

            case LXB_CSS_SYNTAX_TOKEN_SEMICOLON:
                return true;

            default:
                return token->type == stop_ch ||
                       token->type == LXB_CSS_SYNTAX_TOKEN__EOF;
        }
    }

    p = tkz->in_p;
    end = tkz->in_end;

    while (p < end) {
        switch (*p) {
            case 0x3B:
                return true;

            case 0x21:
                p += 1;
                return lxb_css_syntax_tokenizer_lookup_important_ch(tkz, p, end,
                                                                    stop_ch);

            default:
                return (stop_ch != 0x00 && stop_ch == *p);
        }
    }

    return false;
}

/*
 * No inline functions for ABI.
 */
lxb_status_t
lxb_css_syntax_tokenizer_status_noi(lxb_css_syntax_tokenizer_t *tkz)
{
    return lxb_css_syntax_tokenizer_status(tkz);
}
