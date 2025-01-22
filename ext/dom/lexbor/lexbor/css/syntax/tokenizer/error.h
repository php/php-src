/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_TOKENIZER_ERROR_H
#define LEXBOR_CSS_SYNTAX_TOKENIZER_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"
#include "lexbor/core/array_obj.h"


typedef enum {
    /* unexpected-eof */
    LXB_CSS_SYNTAX_TOKENIZER_ERROR_UNEOF = 0x0000,
    /* eof-in-comment */
    LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINCO,
    /* eof-in-string */
    LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINST,
    /* eof-in-url */
    LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINUR,
    /* qo-in-url */
    LXB_CSS_SYNTAX_TOKENIZER_ERROR_QOINUR,
    /* wrong-escape-in-url */
    LXB_CSS_SYNTAX_TOKENIZER_ERROR_WRESINUR,
    /* newline-in-string */
    LXB_CSS_SYNTAX_TOKENIZER_ERROR_NEINST,
    /* bad-char */
    LXB_CSS_SYNTAX_TOKENIZER_ERROR_BACH,
    /* bad-code-point */
    LXB_CSS_SYNTAX_TOKENIZER_ERROR_BACOPO,
}
lxb_css_syntax_tokenizer_error_id_t;

typedef struct {
    const lxb_char_t                    *pos;
    lxb_css_syntax_tokenizer_error_id_t id;
}
lxb_css_syntax_tokenizer_error_t;


LXB_API lxb_css_syntax_tokenizer_error_t *
lxb_css_syntax_tokenizer_error_add(lexbor_array_obj_t *parse_errors,
                                   const lxb_char_t *pos,
                                   lxb_css_syntax_tokenizer_error_id_t id);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_TOKENIZER_ERROR_H */

