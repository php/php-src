/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_STATE_H
#define LEXBOR_CSS_SYNTAX_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/syntax/base.h"
#include "lexbor/css/syntax/tokenizer.h"


const lxb_char_t *
lxb_css_syntax_state_delim(lxb_css_syntax_tokenizer_t *tkz,
                           lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_comment(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                             const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_whitespace(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_string(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                            const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_hash(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                          const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_lparenthesis(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                  const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_rparenthesis(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                  const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_plus(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                          const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_plus_process(lxb_css_syntax_tokenizer_t *tkz,
                                  lxb_css_syntax_token_t *token,
                                  const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_comma(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_minus(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_minus_process(lxb_css_syntax_tokenizer_t *tkz,
                                   lxb_css_syntax_token_t *token,
                                   const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_full_stop(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_colon(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                           const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_semicolon(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_less_sign(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                               const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_at(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                        const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_ls_bracket(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_rsolidus(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                              const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_rs_bracket(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_lc_bracket(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_rc_bracket(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_consume_before_numeric(lxb_css_syntax_tokenizer_t *tkz, lxb_css_syntax_token_t *token,
                                            const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_ident_like_begin(lxb_css_syntax_tokenizer_t *tkz,
                                      lxb_css_syntax_token_t *token,
                                      const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_ident_like_not_url_begin(lxb_css_syntax_tokenizer_t *tkz,
                                              lxb_css_syntax_token_t *token,
                                              const lxb_char_t *data, const lxb_char_t *end);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_STATE_H */
