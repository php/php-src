/*
 * Copyright (C) 2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/blank.h"
#include "lexbor/css/state.h"
#include "lexbor/css/css.h"
#include "lexbor/css/at_rule/state.h"


static bool
lxb_css_blank_list_rules_next(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx);

static lxb_status_t
lxb_css_blank_list_rules_end(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token,
                             void *ctx, bool failed);

static const lxb_css_syntax_cb_at_rule_t *
lxb_css_blank_at_rule_begin(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token, void *ctx,
                            void **out_rule);

static bool
lxb_css_blank_at_rule_prelude(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              void *ctx);

static lxb_status_t
lxb_css_blank_at_rule_prelude_end(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, bool failed);

static const lxb_css_syntax_cb_block_t *
lxb_css_blank_at_rule_block_begin(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, void **out_rule);
static bool
lxb_css_blank_at_rule_prelude_failed(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx);
static lxb_status_t
lxb_css_blank_at_rule_end(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          void *ctx, bool failed);

static const lxb_css_syntax_cb_qualified_rule_t *
lxb_css_blank_qualified_rule_begin(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token,
                                   void *ctx, void **out_rule);

static bool
lxb_css_blank_qualified_rule_prelude(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx);

static lxb_status_t
lxb_css_blank_qualified_rule_prelude_end(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, bool failed);

static const lxb_css_syntax_cb_block_t *
lxb_css_blank_qualified_rule_block_begin(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, void **out_rule);

static bool
lxb_css_blank_qualified_rule_prelude_failed(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token,
                                            void *ctx);
static lxb_status_t
lxb_css_blank_qualified_rule_end(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx, bool failed);
static bool
lxb_css_blank_block_next(lxb_css_parser_t *parser,
                         const lxb_css_syntax_token_t *token, void *ctx);

static lxb_status_t
lxb_css_blank_block_end(lxb_css_parser_t *parser,
                        const lxb_css_syntax_token_t *token,
                        void *ctx, bool failed);

static const lxb_css_syntax_cb_declarations_t *
lxb_css_blank_declarations_begin(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx, void **out_rule);

static lxb_css_parser_state_f
lxb_css_blank_declaration_name(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               void *ctx, void **out_rule);
static bool
lxb_css_blank_declaration_value(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx);

static lxb_status_t
lxb_css_blank_declaration_end(lxb_css_parser_t *parser,
                              void *declarations, void *ctx,
                              const lxb_css_syntax_token_t *token,
                              lxb_css_syntax_declaration_offset_t *offset,
                              bool important, bool failed);

static lxb_status_t
lxb_css_blank_declarations_end(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               void *ctx, bool failed);

static bool
lxb_css_blank_declarations_bad(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx);


static const lxb_css_syntax_cb_list_rules_t lxb_css_blank_list_rules = {
    .at_rule = lxb_css_blank_at_rule_begin,
    .qualified_rule = lxb_css_blank_qualified_rule_begin,
    .next = lxb_css_blank_list_rules_next,
    .cb.failed = lxb_css_state_failed,
    .cb.end = lxb_css_blank_list_rules_end
};

static const lxb_css_syntax_cb_at_rule_t lxb_css_blank_at_rule = {
    .prelude = lxb_css_blank_at_rule_prelude,
    .prelude_end = lxb_css_blank_at_rule_prelude_end,
    .block = lxb_css_blank_at_rule_block_begin,
    .cb.failed = lxb_css_blank_at_rule_prelude_failed,
    .cb.end = lxb_css_blank_at_rule_end
};

static const lxb_css_syntax_cb_qualified_rule_t lxb_css_blank_qualified_rule = {
    .prelude = lxb_css_blank_qualified_rule_prelude,
    .prelude_end = lxb_css_blank_qualified_rule_prelude_end,
    .block = lxb_css_blank_qualified_rule_block_begin,
    .cb.failed = lxb_css_blank_qualified_rule_prelude_failed,
    .cb.end = lxb_css_blank_qualified_rule_end
};

static const lxb_css_syntax_cb_block_t lxb_css_blank_block = {
    .at_rule = lxb_css_blank_at_rule_begin,
    .declarations = lxb_css_blank_declarations_begin,
    .qualified_rule = lxb_css_blank_qualified_rule_begin,
    .next = lxb_css_blank_block_next,
    .cb.failed = lxb_css_state_failed,
    .cb.end = lxb_css_blank_block_end,
};

static const lxb_css_syntax_cb_declarations_t lxb_css_blank_declaration = {
    .name = lxb_css_blank_declaration_name,
    .end = lxb_css_blank_declaration_end,
    .cb.failed = lxb_css_blank_declarations_bad,
    .cb.end = lxb_css_blank_declarations_end
};


const lxb_css_syntax_cb_list_rules_t *
lxb_css_blank_cb_list_rules(void)
{
    return &lxb_css_blank_list_rules;
}

const lxb_css_syntax_cb_at_rule_t *
lxb_css_blank_cb_at_rule(void)
{
    return &lxb_css_blank_at_rule;
}

const lxb_css_syntax_cb_qualified_rule_t *
lxb_css_blank_cb_qualified_rule(void)
{
    return &lxb_css_blank_qualified_rule;
}

const lxb_css_syntax_cb_block_t *
lxb_css_blank_cb_block(void)
{
    return &lxb_css_blank_block;
}

const lxb_css_syntax_cb_declarations_t *
lxb_css_blank_cb_declarations(void)
{
    return &lxb_css_blank_declaration;
}

static bool
lxb_css_blank_list_rules_next(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_blank_list_rules_end(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token,
                             void *ctx, bool failed)
{
    return LXB_STATUS_OK;
}

static const lxb_css_syntax_cb_at_rule_t *
lxb_css_blank_at_rule_begin(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token, void *ctx,
                            void **out_rule)
{
    return &lxb_css_blank_at_rule;
}

static bool
lxb_css_blank_at_rule_prelude(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx)
{
    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_blank_at_rule_prelude_end(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, bool failed)
{
    return LXB_STATUS_OK;
}

static const lxb_css_syntax_cb_block_t *
lxb_css_blank_at_rule_block_begin(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, void **out_rule)
{
    return &lxb_css_blank_block;
}

static bool
lxb_css_blank_at_rule_prelude_failed(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx)
{
    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_blank_at_rule_end(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          void *ctx, bool failed)
{
    return LXB_STATUS_OK;
}

static const lxb_css_syntax_cb_qualified_rule_t *
lxb_css_blank_qualified_rule_begin(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token,
                                   void *ctx, void **out_rule)
{
    return &lxb_css_blank_qualified_rule;
}

static bool
lxb_css_blank_qualified_rule_prelude(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx)
{
    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_blank_qualified_rule_prelude_end(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, bool failed)
{
    return LXB_STATUS_OK;
}

static const lxb_css_syntax_cb_block_t *
lxb_css_blank_qualified_rule_block_begin(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, void **out_rule)
{
    return &lxb_css_blank_block;
}

static bool
lxb_css_blank_qualified_rule_prelude_failed(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token,
                                            void *ctx)
{
    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_blank_qualified_rule_end(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx, bool failed)
{
    return LXB_STATUS_OK;
}

static bool
lxb_css_blank_block_next(lxb_css_parser_t *parser,
                         const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_blank_block_end(lxb_css_parser_t *parser,
                        const lxb_css_syntax_token_t *token,
                        void *ctx, bool failed)
{
    return LXB_STATUS_OK;
}

static const lxb_css_syntax_cb_declarations_t *
lxb_css_blank_declarations_begin(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx, void **out_rule)
{
    return &lxb_css_blank_declaration;
}

static lxb_css_parser_state_f
lxb_css_blank_declaration_name(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               void *ctx, void **out_rule)
{
    return lxb_css_blank_declaration_value;
}

static bool
lxb_css_blank_declaration_value(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_blank_declaration_end(lxb_css_parser_t *parser,
                              void *declarations, void *ctx,
                              const lxb_css_syntax_token_t *token,
                              lxb_css_syntax_declaration_offset_t *offset,
                              bool important, bool failed)
{
    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_blank_declarations_end(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               void *ctx, bool failed)
{
    return LXB_STATUS_OK;
}

static bool
lxb_css_blank_declarations_bad(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx)
{
    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}
