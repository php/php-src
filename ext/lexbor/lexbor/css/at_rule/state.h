/*
 * Copyright (C) 2021-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_AT_RULE_STATE_H
#define LXB_CSS_AT_RULE_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"


LXB_API bool
lxb_css_at_rule__undef_prelude(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule__undef_prelude_end(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token,
                                   void *ctx, bool failed);

LXB_API const lxb_css_syntax_cb_block_t *
lxb_css_at_rule__undef_block(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token,
                             void *ctx, void **out_rule);
LXB_API bool
lxb_css_at_rule__undef_prelude_failed(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule__undef_end(lxb_css_parser_t *parser,
                           const lxb_css_syntax_token_t *token,
                           void *ctx, bool failed);

LXB_API bool
lxb_css_at_rule__custom_prelude(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule__custom_prelude_end(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    void *ctx, bool failed);

LXB_API const lxb_css_syntax_cb_block_t *
lxb_css_at_rule__custom_block(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              void *ctx, void **out_rule);
LXB_API bool
lxb_css_at_rule__custom_prelude_failed(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule__custom_end(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token,
                            void *ctx, bool failed);

LXB_API bool
lxb_css_at_rule_namespace_prelude(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule_namespace_prelude_end(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx, bool failed);

LXB_API const lxb_css_syntax_cb_block_t *
lxb_css_at_rule_namespace_block(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token,
                                void *ctx, void **out_rule);
LXB_API bool
lxb_css_at_rule_namespace_prelude_failed(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule_namespace_end(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              void *ctx, bool failed);

LXB_API bool
lxb_css_at_rule_media_prelude(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule_media_prelude_end(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, bool failed);

LXB_API const lxb_css_syntax_cb_block_t *
lxb_css_at_rule_media_block(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token,
                            void *ctx, void **out_rule);
LXB_API bool
lxb_css_at_rule_media_prelude_failed(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule_media_end(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          void *ctx, bool failed);

LXB_API bool
lxb_css_at_rule_font_face_prelude(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule_font_face_prelude_end(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx, bool failed);

LXB_API const lxb_css_syntax_cb_block_t *
lxb_css_at_rule_font_face_block(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token,
                                void *ctx, void **out_rule);
LXB_API bool
lxb_css_at_rule_font_face_prelude_failed(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule_font_face_end(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              void *ctx, bool failed);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_AT_RULE_STATE_H */
