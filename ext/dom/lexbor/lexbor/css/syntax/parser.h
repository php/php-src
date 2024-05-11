/*
 * Copyright (C) 2020-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_PARSER_H
#define LEXBOR_CSS_SYNTAX_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "lexbor/css/base.h"
#include "lexbor/css/at_rule.h"


LXB_API lxb_status_t
lxb_css_syntax_parser_run(lxb_css_parser_t *parser);

LXB_API const lxb_css_syntax_token_t *
lxb_css_syntax_parser_token(lxb_css_parser_t *parser);

LXB_API const lxb_css_syntax_token_t *
lxb_css_syntax_parser_token_wo_ws(lxb_css_parser_t *parser);

LXB_API void
lxb_css_syntax_parser_consume(lxb_css_parser_t *parser);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_parser_list_rules_push(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_parser_state_f state_back,
                                      const lxb_css_syntax_cb_list_rules_t *list_rules,
                                      void *ctx, bool top_level,
                                      lxb_css_syntax_token_type_t stop);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_parser_at_rule_push(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token,
                                   lxb_css_parser_state_f state_back,
                                   const lxb_css_syntax_cb_at_rule_t *at_rule,
                                   void *ctx, lxb_css_syntax_token_type_t stop);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_parser_qualified_push(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     lxb_css_parser_state_f state_back,
                                     const lxb_css_syntax_cb_qualified_rule_t *qualified,
                                     void *ctx, lxb_css_syntax_token_type_t stop);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_parser_declarations_push(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        lxb_css_parser_state_f state_back,
                                        const lxb_css_syntax_cb_declarations_t *declarations,
                                        void *ctx, lxb_css_syntax_token_type_t stop);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_parser_components_push(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_parser_state_f state_back,
                                      const lxb_css_syntax_cb_components_t *comp,
                                      void *ctx, lxb_css_syntax_token_type_t stop);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_parser_function_push(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    lxb_css_parser_state_f state_back,
                                    const lxb_css_syntax_cb_function_t *func,
                                    void *ctx);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_parser_block_push(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 lxb_css_parser_state_f state_back,
                                 const lxb_css_syntax_cb_block_t *block,
                                 void *ctx);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_parser_pipe_push(lxb_css_parser_t *parser,
                                lxb_css_parser_state_f state_back,
                                const lxb_css_syntax_cb_pipe_t *pipe,
                                void *ctx, lxb_css_syntax_token_type_t stop);

LXB_API lxb_status_t
lxb_css_syntax_parser_tkz_cb(lxb_css_syntax_tokenizer_t *tkz,
                             const lxb_char_t **data, const lxb_char_t **end,
                             void *ctx);

LXB_API const lxb_css_syntax_token_t *
lxb_css_syntax_parser_start_block(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  lxb_css_syntax_rule_t *rule);

LXB_API const lxb_css_syntax_token_t *
lxb_css_syntax_parser_end(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          lxb_css_syntax_rule_t *rule);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_PARSER_H */
