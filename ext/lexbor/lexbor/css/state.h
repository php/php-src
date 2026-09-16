/*
 * Copyright (C) 2021-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_STATE_H
#define LXB_CSS_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"
#include "lexbor/css/syntax/syntax.h"


LXB_API bool
lxb_css_state_success(lxb_css_parser_t *parser,
                      const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_state_failed(lxb_css_parser_t *parser,
                     const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_state_stop(lxb_css_parser_t *parser,
                   const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_state_blank(lxb_css_parser_t *parser,
                    const lxb_css_syntax_token_t *token, void *ctx);

LXB_API const lxb_css_syntax_cb_list_rules_t *
lxb_css_state_cb_list_rules(void);

LXB_API const lxb_css_syntax_cb_at_rule_t *
lxb_css_state_cb_at_rule(void);

LXB_API const lxb_css_syntax_cb_qualified_rule_t *
lxb_css_state_cb_qualified_rule(void);

LXB_API const lxb_css_syntax_cb_block_t *
lxb_css_state_cb_block(void);

LXB_API const lxb_css_syntax_cb_declarations_t *
lxb_css_state_cb_declarations(void);

const lxb_css_syntax_cb_block_t *
lxb_css_state_at_rule_block_begin(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, void **out_rule);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_STATE_H */
