/*
 * Copyright (C) 2018-2025 Alexander Borisov
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

enum lxb_css_syntax_tokenizer_opt {
    LXB_CSS_SYNTAX_TOKENIZER_OPT_UNDEF = 0x00,
};

struct lxb_css_syntax_tokenizer {
    lexbor_dobject_t                   *tokens;
    lexbor_array_obj_t                 *parse_errors;
    lexbor_mraw_t                      *mraw;

    lxb_css_syntax_token_t             *first;
    lxb_css_syntax_token_t             *last;

    const lxb_char_t                   *in_begin;
    const lxb_char_t                   *in_end;
    const lxb_char_t                   *in_p;

    /* Temp */
    lxb_char_t                         *start;
    lxb_char_t                         *pos;
    const lxb_char_t                   *end;

    size_t                             offset;

    /* Process */
    unsigned int                       opt;             /* bitmap */
    lxb_status_t                       status;
    bool                               with_comment;
    bool                               with_unicode_range;
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
lxb_css_syntax_tokenizer_buffer_set(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t *data, size_t size)
{
    tkz->in_begin = data;
    tkz->in_p = data;
    tkz->in_end = data + size;
}

lxb_inline void
lxb_css_syntax_tokenizer_with_unicode_range(lxb_css_syntax_tokenizer_t *tkz,
                                            bool with_range)
{
    tkz->with_unicode_range = with_range;
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
