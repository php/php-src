/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_ANB_H
#define LEXBOR_CSS_SYNTAX_ANB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/syntax/token.h"


typedef struct {
    long a;
    long b;
}
lxb_css_syntax_anb_t;


LXB_API lxb_css_syntax_anb_t
lxb_css_syntax_anb_parse(lxb_css_parser_t *parser,
                         const lxb_char_t *data, size_t length);

LXB_API lxb_status_t
lxb_css_syntax_anb_handler(lxb_css_parser_t *parser,
                           const lxb_css_syntax_token_t *token,
                           lxb_css_syntax_anb_t *anb);

LXB_API lxb_status_t
lxb_css_syntax_anb_serialize(lxb_css_syntax_anb_t *anb,
                             lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_char_t *
lxb_css_syntax_anb_serialize_char(lxb_css_syntax_anb_t *anb,
                                  size_t *out_length);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_ANB_H */
