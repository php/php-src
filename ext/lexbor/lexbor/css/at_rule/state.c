/*
 * Copyright (C) 2021-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/css.h"
#include "lexbor/css/at_rule.h"
#include "lexbor/css/parser.h"
#include "lexbor/css/rule.h"
#include "lexbor/css/blank.h"
#include "lexbor/css/at_rule/state.h"


/* Undef */

bool
lxb_css_at_rule__undef_prelude(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_rule_at_t *at = ctx;

    at->prelude_begin = token->offset;

    /* Skip all prelude. */

    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}

lxb_status_t
lxb_css_at_rule__undef_prelude_end(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token,
                                   void *ctx, bool failed)
{
    lxb_status_t status;
    lxb_css_rule_at_t *at = ctx;

    at->prelude_end = token->offset;

    status = lxb_css_make_data(parser, &at->u.undef->prelude,
                               at->prelude_begin, at->prelude_end);
    if (status != LXB_STATUS_OK) {
        return lxb_css_parser_memory_fail_status(parser);
    }

    return LXB_STATUS_OK;
}

const lxb_css_syntax_cb_block_t *
lxb_css_at_rule__undef_block(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token,
                             void *ctx, void **out_rule)
{
    return lxb_css_state_at_rule_block_begin(parser, token, ctx, out_rule);
}

bool
lxb_css_at_rule__undef_prelude_failed(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx)
{
    /* For _UNDEF and _CUSTOM, access here is not possible. */

    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}

lxb_status_t
lxb_css_at_rule__undef_end(lxb_css_parser_t *parser,
                           const lxb_css_syntax_token_t *token,
                           void *ctx, bool failed)
{
    lxb_css_rule_at_t *at = ctx;
    lxb_css_rule_list_t *block = lxb_css_syntax_returned(parser);

    /*
     * Here we do not check for failed because it cannot happen; we do not call
     * failed anywhere in _UNDEF and _CUSTOM, we simply skip the tokens.
     */

    at->u.undef->block = block;

    lxb_css_syntax_set_return(parser, at);

    return LXB_STATUS_OK;
}

/* Custom */

bool
lxb_css_at_rule__custom_prelude(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_at_rule__undef_prelude(parser, token, ctx);
}

lxb_status_t
lxb_css_at_rule__custom_prelude_end(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    void *ctx, bool failed)
{
    lxb_status_t status;
    lxb_css_rule_at_t *at = ctx;

    at->prelude_end = token->offset;

    status = lxb_css_make_data(parser, &at->u.custom->prelude,
                               at->prelude_begin, at->prelude_end);
    if (status != LXB_STATUS_OK) {
        return lxb_css_parser_memory_fail_status(parser);
    }

    return LXB_STATUS_OK;
}

const lxb_css_syntax_cb_block_t *
lxb_css_at_rule__custom_block(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token,
                             void *ctx, void **out_rule)
{
    return lxb_css_state_at_rule_block_begin(parser, token, ctx, out_rule);
}

bool
lxb_css_at_rule__custom_prelude_failed(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       void *ctx)
{
    return lxb_css_at_rule__undef_prelude_failed(parser, token, ctx);
}

lxb_status_t
lxb_css_at_rule__custom_end(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token,
                            void *ctx, bool failed)
{
    lxb_css_rule_at_t *at = ctx;
    lxb_css_rule_list_t *block = lxb_css_syntax_returned(parser);

    /*
     * Here we do not check for failed because it cannot happen; we do not call
     * failed anywhere in _UNDEF and _CUSTOM, we simply skip the tokens.
     */

    at->u.custom->block = block;

    lxb_css_syntax_set_return(parser, at);

    return LXB_STATUS_OK;
}

/* Namespace */

bool
lxb_css_at_rule_namespace_prelude(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx)
{
    lxb_css_rule_at_t *at = ctx;

    at->prelude_begin = token->offset;

    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}

lxb_status_t
lxb_css_at_rule_namespace_prelude_end(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx, bool failed)
{
    lxb_css_rule_at_t *at = ctx;

    at->prelude_end = token->offset;

    return LXB_STATUS_OK;
}

const lxb_css_syntax_cb_block_t *
lxb_css_at_rule_namespace_block(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token,
                                void *ctx, void **out_rule)
{
    lxb_css_parser_failed_set(parser, true);

    return lxb_css_state_at_rule_block_begin(parser, token, ctx, out_rule);
}

bool
lxb_css_at_rule_namespace_prelude_failed(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx)
{
    return lxb_css_at_rule__undef_prelude_failed(parser, token, ctx);
}

lxb_status_t
lxb_css_at_rule_namespace_end(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              void *ctx, bool failed)
{
    lxb_status_t status;
    lxb_css_rule_at_t *at = ctx;

    if (failed) {
        status = lxb_css_at_rule_convert_to_undef(parser, at);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        at->u.undef->block = lxb_css_syntax_returned(parser);
    }

    lxb_css_syntax_set_return(parser, at);

    return LXB_STATUS_OK;
}

/* Media */

bool
lxb_css_at_rule_media_prelude(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_rule_at_t *at = ctx;

    at->prelude_begin = token->offset;

    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}

lxb_status_t
lxb_css_at_rule_media_prelude_end(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, bool failed)
{
    lxb_css_rule_at_t *at = ctx;

    at->prelude_end = token->offset;

    return LXB_STATUS_OK;
}

const lxb_css_syntax_cb_block_t *
lxb_css_at_rule_media_block(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token,
                            void *ctx, void **out_rule)
{
    return lxb_css_state_at_rule_block_begin(parser, token, ctx, out_rule);
}

bool
lxb_css_at_rule_media_prelude_failed(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx)
{
    return lxb_css_at_rule__undef_prelude_failed(parser, token, ctx);
}

lxb_status_t
lxb_css_at_rule_media_end(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          void *ctx, bool failed)
{
    lxb_status_t status;
    lxb_css_rule_at_t *at = ctx;
    lxb_css_rule_list_t *list = lxb_css_syntax_returned(parser);

    if (list != NULL && !failed) {
        at->u.media->block = list;
    }
    else {
        status = lxb_css_at_rule_convert_to_undef(parser, at);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        at->u.undef->block = list;
    }

    lxb_css_syntax_set_return(parser, at);

    return LXB_STATUS_OK;
}

/* Font-face */

bool
lxb_css_at_rule_font_face_prelude(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx)
{
    lxb_css_rule_at_t *at = ctx;

    at->prelude_begin = token->offset;

    if (token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        return lxb_css_parser_failed(parser);
    }

    return lxb_css_parser_success(parser);
}

lxb_status_t
lxb_css_at_rule_font_face_prelude_end(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx, bool failed)
{
    lxb_css_rule_at_t *at = ctx;

    at->prelude_end = token->offset;

    return LXB_STATUS_OK;
}

const lxb_css_syntax_cb_block_t *
lxb_css_at_rule_font_face_block(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token,
                                void *ctx, void **out_rule)
{
    return lxb_css_state_at_rule_block_begin(parser, token, ctx, out_rule);
}

bool
lxb_css_at_rule_font_face_prelude_failed(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx)
{
    return lxb_css_at_rule__undef_prelude_failed(parser, token, ctx);
}

lxb_status_t
lxb_css_at_rule_font_face_end(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              void *ctx, bool failed)
{
    lxb_status_t status;
    lxb_css_rule_at_t *at = ctx;
    lxb_css_rule_list_t *block = lxb_css_syntax_returned(parser);

    if (!failed) {
        at->u.font_face->block = block;
    }
    else {
        status = lxb_css_at_rule_convert_to_undef(parser, at);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        at->u.undef->block = block;
    }

    lxb_css_syntax_set_return(parser, at);

    return LXB_STATUS_OK;
}
