/*
 * Copyright (C) 2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SELECTORS_STATE_H
#define LEXBOR_CSS_SELECTORS_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/parser.h"
#include "lexbor/css/selectors/base.h"


LXB_API bool
lxb_css_selectors_state_complex_list(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx);

LXB_API bool
lxb_css_selectors_state_compound_list(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx);

LXB_API bool
lxb_css_selectors_state_simple_list(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    void *ctx);

LXB_API bool
lxb_css_selectors_state_relative_list(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx);

LXB_API bool
lxb_css_selectors_state_relative(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx);

LXB_API bool
lxb_css_selectors_state_complex(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_compound(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_selectors_state_simple(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx);


LXB_API lxb_status_t
lxb_css_selectors_state_function_end(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx, bool failed);

LXB_API lxb_status_t
lxb_css_selectors_state_function_forgiving(lxb_css_parser_t *parser,
                                           const lxb_css_syntax_token_t *token,
                                           void *ctx, bool failed);

LXB_API lxb_status_t
lxb_css_selectors_state_function_forgiving_relative(lxb_css_parser_t *parser,
                                                    const lxb_css_syntax_token_t *token,
                                                    void *ctx, bool failed);

lxb_inline bool
lxb_css_selectors_state_fail(lxb_css_parser_t *parser)
{
    parser->status = LXB_STATUS_ERROR_UNEXPECTED_DATA;

    (void) lxb_css_parser_states_pop(parser);

    return false;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SELECTORS_STATE_H */
