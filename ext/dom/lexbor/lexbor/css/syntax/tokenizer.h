/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_TOKENIZER_H
#define LEXBOR_CSS_SYNTAX_TOKENIZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/array_obj.h"

#include "lexbor/css/syntax/base.h"
#include "lexbor/css/syntax/token.h"


/* State */
typedef const lxb_char_t *
(*lxb_css_syntax_tokenizer_state_f)(lxb_css_syntax_tokenizer_t *tkz,
                                 lxb_css_syntax_token_t *token,
                                 const lxb_char_t *data, const lxb_char_t *end);

typedef lxb_status_t
(*lxb_css_syntax_tokenizer_chunk_f)(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t **data,
                                    const lxb_char_t **end, void *ctx);


enum lxb_css_syntax_tokenizer_opt {
    LXB_CSS_SYNTAX_TOKENIZER_OPT_UNDEF = 0x00,
};

typedef struct {
    lxb_css_syntax_token_t **list;
    size_t                 size;
    size_t                 length;
}
lxb_css_syntax_tokenizer_cache_t;

struct lxb_css_syntax_tokenizer {
    lxb_css_syntax_tokenizer_cache_t   *cache;
    lexbor_dobject_t                   *tokens;

    lexbor_array_obj_t                 *parse_errors;

    const lxb_char_t                   *in_begin;
    const lxb_char_t                   *in_end;
    const lxb_char_t                   *begin;

    uintptr_t                          offset;
    size_t                             cache_pos;
    size_t                             prepared;

    lexbor_mraw_t                      *mraw;

    lxb_css_syntax_tokenizer_chunk_f   chunk_cb;
    void                               *chunk_ctx;

    /* Temp */
    lxb_char_t                         *start;
    lxb_char_t                         *pos;
    const lxb_char_t                   *end;
    lxb_char_t                         buffer[128];
    lxb_css_syntax_token_data_t        token_data;

    /* Process */
    unsigned int                       opt;             /* bitmap */
    lxb_status_t                       status;
    bool                               eof;
    bool                               with_comment;
};


LXB_API lxb_css_syntax_tokenizer_t *
lxb_css_syntax_tokenizer_create(void);

LXB_API lxb_status_t
lxb_css_syntax_tokenizer_init(lxb_css_syntax_tokenizer_t *tkz);

LXB_API lxb_status_t
lxb_css_syntax_tokenizer_clean(lxb_css_syntax_tokenizer_t *tkz);

LXB_API lxb_css_syntax_tokenizer_t *
lxb_css_syntax_tokenizer_destroy(lxb_css_syntax_tokenizer_t *tkz);

LXB_API lxb_status_t
lxb_css_syntax_tokenizer_next_chunk(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t **data, const lxb_char_t **end);

LXB_API bool
lxb_css_syntax_tokenizer_lookup_colon(lxb_css_syntax_tokenizer_t *tkz);

LXB_API bool
lxb_css_syntax_tokenizer_lookup_important(lxb_css_syntax_tokenizer_t *tkz,
                                          lxb_css_syntax_token_type_t stop,
                                          const lxb_char_t stop_ch);

LXB_API bool
lxb_css_syntax_tokenizer_lookup_declaration_ws_end(lxb_css_syntax_tokenizer_t *tkz,
                                                   lxb_css_syntax_token_type_t stop,
                                                   const lxb_char_t stop_ch);

/*
 * Inline functions
 */
lxb_inline lxb_status_t
lxb_css_syntax_tokenizer_status(lxb_css_syntax_tokenizer_t *tkz)
{
    return tkz->status;
}

lxb_inline void
lxb_css_syntax_tokenizer_chunk_cb_set(lxb_css_syntax_tokenizer_t *tkz,
                                      lxb_css_syntax_tokenizer_chunk_f cb,
                                      void *ctx)
{
    tkz->chunk_cb = cb;
    tkz->chunk_ctx = ctx;
}

lxb_inline void
lxb_css_syntax_tokenizer_buffer_set(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t *data, size_t size)
{
    tkz->in_begin = data;
    tkz->in_end = data + size;
}


/*
 * No inline functions for ABI.
 */
LXB_API lxb_status_t
lxb_css_syntax_tokenizer_status_noi(lxb_css_syntax_tokenizer_t *tkz);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_TOKENIZER_H */
