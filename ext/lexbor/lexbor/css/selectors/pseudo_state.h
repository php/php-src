/*
 * Copyright (C) 2020-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SELECTORS_PSEUDO_STATE_H
#define LEXBOR_CSS_SELECTORS_PSEUDO_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"
#include "lexbor/css/parser.h"
#include "lexbor/css/syntax/parser.h"
#include "lexbor/css/selectors/base.h"


LXB_API bool
lxb_css_selectors_state_pseudo_class_function__undef(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_current(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_dir(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_has(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_is(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_lang(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_not(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_nth_child(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_nth_col(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_nth_last_child(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_nth_last_col(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_nth_last_of_type(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_nth_of_type(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_class_function_where(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_pseudo_element_function__undef(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SELECTORS_PSEUDO_STATE_H */
