/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/syntax/tokenizer.h"
#include "lexbor/css/syntax/tokenizer/error.h"
#include "lexbor/css/syntax/state.h"
#include "lexbor/css/syntax/state_res.h"

#include "lexbor/core/array.h"

#define LEXBOR_STR_RES_MAP_LOWERCASE
#include "lexbor/core/str_res.h"


static const lxb_char_t lxb_css_syntax_tokenizer_important[] = "important";

static lxb_css_syntax_tokenizer_cache_t *
lxb_css_syntax_tokenizer_cache_create(void);

static lxb_status_t
lxb_css_syntax_tokenizer_cache_init(lxb_css_syntax_tokenizer_cache_t *cache,
                                    size_t size);

static void
lxb_css_syntax_tokenizer_cache_clean(lxb_css_syntax_tokenizer_cache_t *cache);

static lxb_css_syntax_tokenizer_cache_t *
lxb_css_syntax_tokenizer_cache_destroy(lxb_css_syntax_tokenizer_cache_t *cache);

LXB_API lxb_status_t
lxb_css_syntax_tokenizer_cache_push(lxb_css_syntax_tokenizer_cache_t *cache,
                                    lxb_css_syntax_token_t *value);

static lxb_status_t
lxb_css_syntax_tokenizer_blank(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t **data, const lxb_char_t **end,
                               void *ctx);

static bool
lxb_css_syntax_tokenizer_lookup_important_ch(lxb_css_syntax_tokenizer_t *tkz,
                                             const lxb_char_t *p,
                                             const lxb_char_t *end,
                                             const lxb_char_t stop_ch,
                                             lxb_css_syntax_token_type_t stop,
                                             bool skip_first);

static bool
lxb_css_syntax_tokenizer_lookup_important_end(lxb_css_syntax_tokenizer_t *tkz,
                                              const lxb_char_t *p,
                                              const lxb_char_t *end,
                                              const lxb_char_t stop_ch,
                                              lxb_css_syntax_token_type_t stop,
                                              bool skip_first);

static bool
lxb_css_syntax_tokenizer_lookup_important_tokens(lxb_css_syntax_tokenizer_t *tkz,
                                                 lxb_css_syntax_token_type_t stop,
                                                 bool skip_first);


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

    /* Cache for Tokens. */

    tkz->cache = lxb_css_syntax_tokenizer_cache_create();
    status = lxb_css_syntax_tokenizer_cache_init(tkz->cache, 128);
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

    tkz->offset = 0;
    tkz->cache_pos = 0;
    tkz->prepared = 0;

    tkz->eof = false;
    tkz->with_comment = false;
    tkz->status = LXB_STATUS_OK;
    tkz->opt = LXB_CSS_SYNTAX_TOKENIZER_OPT_UNDEF;
    tkz->chunk_cb = lxb_css_syntax_tokenizer_blank;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_syntax_tokenizer_clean(lxb_css_syntax_tokenizer_t *tkz)
{
    lexbor_mraw_clean(tkz->mraw);
    lexbor_array_obj_clean(tkz->parse_errors);
    lxb_css_syntax_tokenizer_cache_clean(tkz->cache);
    lexbor_dobject_clean(tkz->tokens);

    tkz->offset = 0;
    tkz->cache_pos = 0;
    tkz->prepared = 0;

    tkz->eof = false;
    tkz->status = LXB_STATUS_OK;
    tkz->in_begin = NULL;
    tkz->in_end = NULL;
    tkz->pos = tkz->start;

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
        tkz->cache = lxb_css_syntax_tokenizer_cache_destroy(tkz->cache);
    }

    tkz->mraw = lexbor_mraw_destroy(tkz->mraw, true);
    tkz->parse_errors = lexbor_array_obj_destroy(tkz->parse_errors, true);

    if (tkz->start != NULL) {
        tkz->start = lexbor_free(tkz->start);
    }

    return lexbor_free(tkz);
}

static lxb_css_syntax_tokenizer_cache_t *
lxb_css_syntax_tokenizer_cache_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_css_syntax_tokenizer_cache_t));
}

static lxb_status_t
lxb_css_syntax_tokenizer_cache_init(lxb_css_syntax_tokenizer_cache_t *cache,
                                    size_t size)
{
    cache->length = 0;
    cache->size = size;

    cache->list = lexbor_malloc(sizeof(lxb_css_syntax_token_t *) * size);
    if (cache->list == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_OK;
}

static void
lxb_css_syntax_tokenizer_cache_clean(lxb_css_syntax_tokenizer_cache_t *cache)
{
    if (cache != NULL) {
        cache->length = 0;
    }
}

static lxb_css_syntax_tokenizer_cache_t *
lxb_css_syntax_tokenizer_cache_destroy(lxb_css_syntax_tokenizer_cache_t *cache)
{
    if (cache == NULL) {
        return NULL;
    }

    if (cache->list) {
        lexbor_free(cache->list);
    }

    return lexbor_free(cache);
}

static lxb_css_syntax_token_t **
lxb_css_syntax_tokenizer_cache_expand(lxb_css_syntax_tokenizer_cache_t *cache,
                                      size_t up_to)
{
    size_t new_size;
    lxb_css_syntax_token_t **list;

    if (cache == NULL) {
        return NULL;
    }

    new_size = cache->length + up_to;
    list = lexbor_realloc(cache->list,
                          sizeof(lxb_css_syntax_token_t *) * new_size);
    if (cache == NULL) {
        return NULL;
    }

    cache->list = list;
    cache->size = new_size;

    return list;
}

lxb_status_t
lxb_css_syntax_tokenizer_cache_push(lxb_css_syntax_tokenizer_cache_t *cache,
                                    lxb_css_syntax_token_t *value)
{
    if (cache->length >= cache->size) {
        if ((lxb_css_syntax_tokenizer_cache_expand(cache, 128) == NULL)) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    cache->list[ cache->length ] = value;
    cache->length++;

    return LXB_STATUS_OK;
}


static lxb_status_t
lxb_css_syntax_tokenizer_blank(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t **data, const lxb_char_t **end,
                               void *ctx)
{
    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_syntax_tokenizer_chunk(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, size_t size)
{
    return tkz->status;
}

lxb_css_syntax_token_t *
lxb_css_syntax_tokenizer_token(lxb_css_syntax_tokenizer_t *tkz)
{
    lxb_status_t status;
    lxb_css_syntax_token_t *token;
    const lxb_char_t *begin, *end;

    begin = tkz->in_begin;
    end = tkz->in_end;

    if (tkz->prepared != 0) {
        if (tkz->cache_pos < tkz->prepared) {
            token = tkz->cache->list[tkz->prepared - 1];

            status = lxb_css_syntax_token_string_make(tkz, token);
            if (status != LXB_STATUS_OK) {
                return NULL;
            }
        }

        token = tkz->cache->list[tkz->prepared];
        token->offset = tkz->offset;

        tkz->prepared += 1;

        if (tkz->prepared >= tkz->cache->length) {
            tkz->prepared = 0;
        }

        if (lxb_css_syntax_token_base(token)->length != 0) {
            tkz->offset += lxb_css_syntax_token_base(token)->length;
            token->cloned = false;
            return token;
        }

        if (begin >= end) {
            status = lxb_css_syntax_tokenizer_next_chunk(tkz, &begin, &end);
            if (status != LXB_STATUS_OK) {
                return NULL;
            }

            if (begin >= end) {
                lxb_css_syntax_token_base(token)->length = 1;
                goto done;
            }
        }

        if (lxb_css_syntax_token_delim(token)->character == '-') {
            begin = lxb_css_syntax_state_minus_process(tkz, token, begin, end);
        }
        else {
            begin = lxb_css_syntax_state_plus_process(tkz, token, begin, end);
        }

        goto done;
    }

    if (tkz->cache_pos < tkz->cache->length) {
        token = tkz->cache->list[tkz->cache->length - 1];

        status = lxb_css_syntax_token_string_make(tkz, token);
        if (status != LXB_STATUS_OK) {
            return NULL;
        }
    }

    token = lxb_css_syntax_token_cached_create(tkz);
    if (token == NULL) {
        return NULL;
    }

    token->offset = tkz->offset;

again:

    if (begin >= end) {
        status = lxb_css_syntax_tokenizer_next_chunk(tkz, &begin, &end);
        if (status != LXB_STATUS_OK) {
            return NULL;
        }

        if (begin >= end) {
            token->type = LXB_CSS_SYNTAX_TOKEN__EOF;

            lxb_css_syntax_token_base(token)->begin = begin;
            lxb_css_syntax_token_base(token)->length = 0;

            token->cloned = false;

            return token;
        }
    }

    begin = lxb_css_syntax_state_res_map[*begin](tkz, token, begin, end);

done:

    token->cloned = false;

    if (begin == NULL) {
        return NULL;
    }

    tkz->in_begin = begin;
    tkz->offset += lxb_css_syntax_token_base(token)->length;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_COMMENT && !tkz->with_comment) {
        end = tkz->in_end;
        goto again;
    }

    return token;
}

lxb_status_t
lxb_css_syntax_tokenizer_next_chunk(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t **data, const lxb_char_t **end)
{
    const lxb_char_t *begin;

    if (tkz->eof == false) {
        begin = *data;

        tkz->status = tkz->chunk_cb(tkz, data, end, tkz->chunk_ctx);
        if (tkz->status != LXB_STATUS_OK) {
            return tkz->status;
        }

        if (*data >= *end) {
            *data = begin;
            *end = begin;

            tkz->in_begin = begin;
            tkz->in_end = begin;

            tkz->eof = true;
        }
        else {
            tkz->in_begin = *data;
            tkz->in_end = *end;
        }
    }

    return LXB_STATUS_OK;
}

bool
lxb_css_syntax_tokenizer_lookup_colon(lxb_css_syntax_tokenizer_t *tkz)
{
    const lxb_char_t *p, *end;
    lxb_css_syntax_token_t *token;

    if (tkz->cache_pos + 1 < tkz->cache->length) {
        token = tkz->cache->list[tkz->cache_pos + 1];

        if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
            if (tkz->cache_pos + 2 < tkz->cache->length) {
                token = tkz->cache->list[tkz->cache_pos + 2];

                return token->type == LXB_CSS_SYNTAX_TOKEN_COLON;
            }
        }
        else if (token->type == LXB_CSS_SYNTAX_TOKEN_COLON) {
            return true;
        }

        return false;
    }

    p = tkz->in_begin;
    end = tkz->in_end;

    do {
        if (p >= end) {
            token = lxb_css_syntax_token_next(tkz);
            if (token == NULL) {
                return false;
            }

            if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
                token = lxb_css_syntax_token_next(tkz);
                if (token == NULL) {
                    return false;
                }
            }

            return token->type == LXB_CSS_SYNTAX_TOKEN_COLON;
        }

        switch (*p) {
            case 0x3A:
                return true;

            case 0x0D:
            case 0x0C:
            case 0x09:
            case 0x20:
            case 0x0A:
                p++;
                break;

            default:
                return false;
        }
    }
    while (true);
}

bool
lxb_css_syntax_tokenizer_lookup_important(lxb_css_syntax_tokenizer_t *tkz,
                                          lxb_css_syntax_token_type_t stop,
                                          const lxb_char_t stop_ch)
{
    const lxb_char_t *p, *end;
    lxb_css_syntax_token_t *token;

    static const size_t length = sizeof(lxb_css_syntax_tokenizer_important) - 1;

    p = tkz->in_begin;
    end = tkz->in_end;

    if (tkz->cache_pos + 1 < tkz->cache->length) {
        token = tkz->cache->list[tkz->cache_pos + 1];

        if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
            return false;
        }

        if (!(lxb_css_syntax_token_ident(token)->length == length
              && lexbor_str_data_ncasecmp(lxb_css_syntax_token_ident(token)->data,
                                          lxb_css_syntax_tokenizer_important,
                                          length)))
        {
            return false;
        }

        if (tkz->cache_pos + 2 < tkz->cache->length) {
            token = tkz->cache->list[tkz->cache_pos + 2];

            if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
                if (tkz->cache_pos + 3 >= tkz->cache->length) {
                    return lxb_css_syntax_tokenizer_lookup_important_end(tkz,
                                                 p, end, stop_ch, stop, false);
                }

                token = tkz->cache->list[tkz->cache_pos + 3];
            }

            return (token->type == LXB_CSS_SYNTAX_TOKEN_SEMICOLON
                    || token->type == stop
                    || token->type == LXB_CSS_SYNTAX_TOKEN__EOF);
        }

        return lxb_css_syntax_tokenizer_lookup_important_end(tkz, p, end,
                                                         stop_ch, stop, false);
    }

    return lxb_css_syntax_tokenizer_lookup_important_ch(tkz, p, end, stop_ch,
                                                        stop, false);
}

static bool
lxb_css_syntax_tokenizer_lookup_important_ch(lxb_css_syntax_tokenizer_t *tkz,
                                             const lxb_char_t *p,
                                             const lxb_char_t *end,
                                             const lxb_char_t stop_ch,
                                             lxb_css_syntax_token_type_t stop,
                                             bool skip_first)
{
    const lxb_char_t *imp;

    imp = lxb_css_syntax_tokenizer_important;

    do {
        if (p >= end) {
            return lxb_css_syntax_tokenizer_lookup_important_tokens(tkz, stop,
                                                                    skip_first);
        }

        if (lexbor_str_res_map_lowercase[*p++] != *imp++) {
            return false;
        }
    }
    while (*imp != 0x00);

    return lxb_css_syntax_tokenizer_lookup_important_end(tkz, p, end, stop_ch,
                                                         stop, skip_first);
}

static bool
lxb_css_syntax_tokenizer_lookup_important_end(lxb_css_syntax_tokenizer_t *tkz,
                                              const lxb_char_t *p,
                                              const lxb_char_t *end,
                                              const lxb_char_t stop_ch,
                                              lxb_css_syntax_token_type_t stop,
                                              bool skip_first)
{
    do {
        if (p >= end) {
            return lxb_css_syntax_tokenizer_lookup_important_tokens(tkz, stop,
                                                                    skip_first);
        }

        switch (*p) {
            case 0x3B:
                return true;

            case 0x0D:
            case 0x0C:
            case 0x09:
            case 0x20:
            case 0x0A:
                p++;
                break;

            default:
                return (stop_ch != 0x00 && stop_ch == *p);
        }
    }
    while (true);
}

static bool
lxb_css_syntax_tokenizer_lookup_important_tokens(lxb_css_syntax_tokenizer_t *tkz,
                                                 lxb_css_syntax_token_type_t stop,
                                                 bool skip_first)
{
    const lxb_css_syntax_token_t *next;

    static const size_t length = sizeof(lxb_css_syntax_tokenizer_important) - 1;

    if (skip_first) {
        next = lxb_css_syntax_token_next(tkz);
        if (next == NULL) {
            return false;
        }
    }

    next = lxb_css_syntax_token_next(tkz);
    if (next == NULL) {
        return false;
    }

    if (next->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return false;
    }

    if (!(lxb_css_syntax_token_ident(next)->length == length
          && lexbor_str_data_ncasecmp(lxb_css_syntax_token_ident(next)->data,
                                      lxb_css_syntax_tokenizer_important,
                                      length)))
    {
        return false;
    }

    next = lxb_css_syntax_token_next(tkz);
    if (next == NULL) {
        return false;
    }

    if (next->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
        next = lxb_css_syntax_token_next(tkz);
        if (next == NULL) {
            return false;
        }
    }

    return (next->type == LXB_CSS_SYNTAX_TOKEN_SEMICOLON
            || next->type == stop || next->type == LXB_CSS_SYNTAX_TOKEN__EOF);
}

bool
lxb_css_syntax_tokenizer_lookup_declaration_ws_end(lxb_css_syntax_tokenizer_t *tkz,
                                                   lxb_css_syntax_token_type_t stop,
                                                   const lxb_char_t stop_ch)
{
    lxb_css_syntax_token_t *token;
    const lxb_char_t *p, *end;

    if (tkz->cache_pos + 1 < tkz->cache->length) {
        token = tkz->cache->list[tkz->cache_pos + 1];

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

    p = tkz->in_begin;
    end = tkz->in_end;

    do {
        if (p >= end) {
            return lxb_css_syntax_tokenizer_lookup_important_tokens(tkz, stop,
                                                                    true);
        }

        switch (*p) {
            case 0x3B:
                return true;

            case 0x21:
                p++;
                return lxb_css_syntax_tokenizer_lookup_important_ch(tkz, p, end,
                                                           stop_ch, stop, true);

            default:
                return (stop_ch != 0x00 && stop_ch == *p);
        }
    }
    while (true);
}

/*
 * No inline functions for ABI.
 */
lxb_status_t
lxb_css_syntax_tokenizer_status_noi(lxb_css_syntax_tokenizer_t *tkz)
{
    return lxb_css_syntax_tokenizer_status(tkz);
}
