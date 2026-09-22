/*
 * Copyright (C) 2020-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/css.h"
#include "lexbor/css/state.h"
#include "lexbor/css/syntax/parser.h"
#include "lexbor/css/syntax/syntax.h"
#include "lexbor/css/at_rule/state.h"


static const lexbor_str_t lxb_css_err_rc = lexbor_str("Unexpected token in "
                                                      "qualified rule: RC_BRACKET");

static lxb_css_syntax_rule_t *
lxb_css_syntax_parser_stack_pop(lxb_css_parser_t *parser);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_list_rules(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_list_rules_at(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_list_rules_qualified(lxb_css_parser_t *parser,
                                           const lxb_css_syntax_token_t *token,
                                           lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_list_rules_back(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_begin(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_rule_t *rule,
                               lxb_css_parser_state_f back_state,
                               lxb_css_syntax_begin_at_rule_f cb,
                               bool nested);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_rule(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_block(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_prelude_end(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_prelude_end_consule_token(lxb_css_parser_t *parser,
                                                   const lxb_css_syntax_token_t *token,
                                                   lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_back_to_prelude(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_qualified_rule(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_qualified_rule_block(lxb_css_parser_t *parser,
                                           const lxb_css_syntax_token_t *token,
                                           lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_qualified_prelude_end(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token,
                                            lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_qualified_back_to_prelude(lxb_css_parser_t *parser,
                                                const lxb_css_syntax_token_t *token,
                                                lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block_begin(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  lxb_css_syntax_rule_t *rule,
                                  lxb_css_parser_state_f back_state,
                                  lxb_css_syntax_begin_block_f cb);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token,
                            lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block_at(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block_declarations(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block_qualified(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block_back(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_validate(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token,
                                            lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_begin(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_name(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_value(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_drop(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_next(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declaration_back_to_value(lxb_css_parser_t *parser,
                                                const lxb_css_syntax_token_t *token,
                                                lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declaration_end(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declaration_end_handler(lxb_css_parser_t *parser,
                                              const lxb_css_syntax_token_t *token,
                                              lxb_css_syntax_rule_t *rule,
                                              bool skip_token);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_end(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_end_h(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_syntax_rule_t *rule,
                                         bool skip_token);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_components(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_components_back_to_value(lxb_css_parser_t *parser,
                                               const lxb_css_syntax_token_t *token,
                                               lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_function(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_function_back(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_pipe(lxb_css_parser_t *parser,
                           const lxb_css_syntax_token_t *token,
                           lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_pipe_back(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token,
                                lxb_css_syntax_rule_t *rule);

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_end_back(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_rule_t *rule);


lxb_inline const lxb_css_syntax_token_t *
lxb_css_syntax_parser_failed(lxb_css_parser_t *parser, lxb_status_t status)
{
    parser->status = status;
    return NULL;
}

lxb_inline const lxb_css_syntax_token_t *
lxb_css_syntax_token_parser_do_phase_again(lxb_css_parser_t *parser)
{
    parser->fake_null = true;
    return NULL;
}

lxb_inline const lxb_css_syntax_token_t *
lxb_css_syntax_parser_call_back(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token)
{
    lxb_css_syntax_rule_t *rules;

    (void) lxb_css_syntax_parser_stack_pop(parser);

    rules = parser->rules;

    if (parser->rules <= parser->rules_begin) {
        rules->state = lxb_css_state_stop;
        return token;
    }

    return rules->back(parser, token, rules);
}


lxb_status_t
lxb_css_syntax_parser_run(lxb_css_parser_t *parser)
{
    const lxb_css_syntax_token_t *token;

    parser->loop = true;

    do {
        token = lxb_css_syntax_parser_token(parser);
        if (token == NULL) {
            if (parser->fake_null) {
                parser->fake_null = false;
                continue;
            }

            return parser->status;
        }

        while (parser->rules->state(parser, token,
                                    parser->rules->context) == false) {};
    }
    while (parser->loop);

    return parser->status;
}

const lxb_css_syntax_token_t *
lxb_css_syntax_parser_token(lxb_css_parser_t *parser)
{
    lxb_css_syntax_token_t *token;
    lxb_css_syntax_rule_t *rule = parser->rules;

    token = lxb_css_syntax_token(parser->tkz);
    if (token == NULL) {
        return lxb_css_syntax_parser_failed(parser, parser->tkz->status);
    }

    return rule->phase(parser, token, rule);
}

const lxb_css_syntax_token_t *
lxb_css_syntax_parser_token_wo_ws(lxb_css_parser_t *parser)
{
    const lxb_css_syntax_token_t *token;

    token = lxb_css_syntax_parser_token(parser);

    if (token != NULL && token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
        lxb_css_syntax_parser_consume(parser);
        return lxb_css_syntax_parser_token(parser);
    }

    return token;
}

void
lxb_css_syntax_parser_consume(lxb_css_parser_t *parser)
{
    if (!parser->rules->skip_consume) {
        lxb_css_syntax_token_consume(parser->tkz);
    }
}

lxb_css_syntax_rule_t *
lxb_css_syntax_parser_list_rules_push(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_cb_list_rules_t *cb_rules,
                                      lxb_css_parser_state_f back,
                                      void *ctx, lxb_css_syntax_token_type_t stop)
{
    lxb_css_syntax_rule_t *rule;

    if ((parser->rules + 1) >= parser->rules_end) {
        parser->status = lxb_css_syntax_stack_expand(parser, 1);
        if (parser->status != LXB_STATUS_OK) {
            return NULL;
        }
    }
 
    parser->rules->back_state = back;

    rule = ++parser->rules;

    *rule = (lxb_css_syntax_rule_t) {
        .phase = lxb_css_syntax_parser_list_rules,
        .state = lxb_css_state_blank,
        .back = lxb_css_syntax_parser_list_rules_back,
        .cbx.list_rules = cb_rules,
        .context = ctx,
        .block_end = stop
    };

    return rule;
}

lxb_css_syntax_rule_t *
lxb_css_syntax_parser_at_rule_push(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_cb_at_rule_t *at_rule,
                                   lxb_css_parser_state_f back, void *ctx,
                                   lxb_css_syntax_token_type_t stop,
                                   bool nested)
{
    lxb_css_syntax_rule_t *rule;

    if ((parser->rules + 1) >= parser->rules_end) {
        parser->status = lxb_css_syntax_stack_expand(parser, 1);
        if (parser->status != LXB_STATUS_OK) {
            return NULL;
        }
    }

    parser->rules->back_state = back;

    rule = ++parser->rules;

    *rule = (lxb_css_syntax_rule_t) {
        .phase = lxb_css_syntax_parser_at_rule,
        .state = at_rule->prelude,
        .back = lxb_css_syntax_parser_at_back_to_prelude,
        .cbx.at_rule = at_rule,
        .context = ctx,
        .block_end = stop,
        .nested = nested
    };

    return rule;
}

lxb_css_syntax_rule_t *
lxb_css_syntax_parser_qualified_push(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_cb_qualified_rule_t *qualified,
                                     lxb_css_parser_state_f back, void *ctx,
                                     lxb_css_syntax_token_type_t stop,
                                     bool nested)
{
    lxb_css_syntax_rule_t *rule;

    if ((parser->rules + 1) >= parser->rules_end) {
        parser->status = lxb_css_syntax_stack_expand(parser, 1);
        if (parser->status != LXB_STATUS_OK) {
            return NULL;
        }
    }

    parser->rules->back_state = back;

    rule = ++parser->rules;

    *rule = (lxb_css_syntax_rule_t) {
        .phase = lxb_css_syntax_parser_qualified_rule,
        .state = qualified->prelude,
        .back = lxb_css_syntax_parser_qualified_back_to_prelude,
        .cbx.qualified_rule = qualified,
        .context = ctx,
        .block_end = stop,
        .nested = nested
    };

    return rule;
}

lxb_css_syntax_rule_t *
lxb_css_syntax_parser_block_push(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_cb_block_t *block,
                                 lxb_css_parser_state_f back, void *ctx)
{
    lxb_css_syntax_rule_t *rule;

    if ((parser->rules + 1) >= parser->rules_end) {
        parser->status = lxb_css_syntax_stack_expand(parser, 1);
        if (parser->status != LXB_STATUS_OK) {
            return NULL;
        }
    }

    parser->rules->back_state = back;

    rule = ++parser->rules;

    *rule = (lxb_css_syntax_rule_t) {
        .phase = lxb_css_syntax_parser_block,
        .state = lxb_css_state_blank,
        .back = lxb_css_syntax_parser_block_back,
        .cbx.block = block,
        .context = ctx
    };

    return rule;
}

lxb_css_syntax_rule_t *
lxb_css_syntax_parser_declarations_push(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_cb_declarations_t *declr,
                                        lxb_css_parser_state_f back, void *ctx,
                                        lxb_css_syntax_token_type_t stop,
                                        bool name_validate, bool nested)
{
    lxb_css_syntax_rule_t *rule;
    lxb_css_syntax_state_f phase;

    if ((parser->rules + 1) >= parser->rules_end) {
        parser->status = lxb_css_syntax_stack_expand(parser, 1);
        if (parser->status != LXB_STATUS_OK) {
            return NULL;
        }
    }

    parser->rules->back_state = back;

    rule = ++parser->rules;

    phase = (!name_validate) ? lxb_css_syntax_parser_declarations_begin
                             : lxb_css_syntax_parser_declarations_validate;

    *rule = (lxb_css_syntax_rule_t) {
        .phase = phase,
        .state = lxb_css_state_blank,
        .back = lxb_css_syntax_parser_declaration_back_to_value,
        .cbx.declarations = declr,
        .context = ctx,
        .block_end = stop,
        .nested = nested
    };

    return rule;
}

lxb_css_syntax_rule_t *
lxb_css_syntax_parser_components_push(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_cb_components_t *comp,
                                      lxb_css_parser_state_f back, void *ctx,
                                      lxb_css_syntax_token_type_t stop)
{
    lxb_css_syntax_rule_t *rule;

    if ((parser->rules + 1) >= parser->rules_end) {
        parser->status = lxb_css_syntax_stack_expand(parser, 1);
        if (parser->status != LXB_STATUS_OK) {
            return NULL;
        }
    }

    parser->rules->back_state = back;

    rule = ++parser->rules;

    *rule = (lxb_css_syntax_rule_t) {
        .phase = lxb_css_syntax_parser_components,
        .state = comp->prelude,
        .back = lxb_css_syntax_parser_components_back_to_value,
        .cbx.components = comp,
        .context = ctx,
        .block_end = stop
    };

    return rule;
}

lxb_css_syntax_rule_t *
lxb_css_syntax_parser_function_push(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_cb_function_t *func,
                                    lxb_css_parser_state_f back, void *ctx)
{
    lxb_css_syntax_rule_t *rule;

    if ((parser->rules + 1) >= parser->rules_end) {
        parser->status = lxb_css_syntax_stack_expand(parser, 1);
        if (parser->status != LXB_STATUS_OK) {
            return NULL;
        }
    }

    parser->rules->back_state = back;

    rule = ++parser->rules;

    *rule = (lxb_css_syntax_rule_t) {
        .phase = lxb_css_syntax_parser_function,
        .state = func->value,
        .back = lxb_css_syntax_parser_function_back,
        .cbx.func = func,
        .context = ctx
    };

    return rule;
}

lxb_css_syntax_rule_t *
lxb_css_syntax_parser_pipe_push(lxb_css_parser_t *parser,
                                const lxb_css_syntax_cb_pipe_t *pipe,
                                lxb_css_parser_state_f back, void *ctx,
                                lxb_css_syntax_token_type_t stop)
{
    lxb_css_syntax_rule_t *rule;

    if ((parser->rules + 1) >= parser->rules_end) {
        parser->status = lxb_css_syntax_stack_expand(parser, 1);
        if (parser->status != LXB_STATUS_OK) {
            return NULL;
        }
    }

    parser->rules->back_state = back;

    rule = ++parser->rules;

    *rule = (lxb_css_syntax_rule_t) {
        .phase = lxb_css_syntax_parser_pipe,
        .state = pipe->prelude,
        .back = lxb_css_syntax_parser_pipe_back,
        .cbx.pipe = pipe,
        .context = ctx,
        .block_end = stop
    };

    return rule;
}

static lxb_css_syntax_rule_t *
lxb_css_syntax_parser_stack_pop(lxb_css_parser_t *parser)
{
    return parser->rules--;
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_list_rules(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 lxb_css_syntax_rule_t *rule)
{
    if (rule->offset > token->offset) {
        return token;
    }

    rule->skip_consume = true;

begin:

    rule->offset = token->offset + lxb_css_syntax_token_base(token)->length;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
        case LXB_CSS_SYNTAX_TOKEN_CDC:
        case LXB_CSS_SYNTAX_TOKEN_CDO:
            lxb_css_syntax_token_consume(parser->tkz);

            token = lxb_css_syntax_token(parser->tkz);
            if (token == NULL) {
                return lxb_css_syntax_parser_failed(parser,
                                                    parser->tkz->status);
            }

            goto begin;

        case LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD:
            rule->phase = lxb_css_syntax_parser_list_rules_at;
            break;

        case LXB_CSS_SYNTAX_TOKEN__EOF:
            goto done;

        default:
            if (rule->block_end == token->type && rule->deep == 0) {
                goto done;
            }

            rule->phase = lxb_css_syntax_parser_list_rules_qualified;
            break;
    }

    return token;

done:

    rule->phase = lxb_css_syntax_parser_end;

    return lxb_css_parser_token_end(parser, 0);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_list_rules_at(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    lxb_css_syntax_rule_t *rule)
{
    const lxb_css_syntax_cb_list_rules_t *list_rules = rule->cbx.list_rules;

    return lxb_css_syntax_parser_at_begin(parser, token, rule, list_rules->next,
                                          list_rules->at_rule, false);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_list_rules_qualified(lxb_css_parser_t *parser,
                                           const lxb_css_syntax_token_t *token,
                                           lxb_css_syntax_rule_t *rule)
{
    void *qualified_ctx;
    lxb_css_syntax_rule_t *qualified;
    lxb_css_syntax_begin_qualified_rule_f cb;
    const lxb_css_syntax_cb_list_rules_t *list_rules;
    const lxb_css_syntax_cb_qualified_rule_t *cb_qualified;

    qualified_ctx = NULL;
    list_rules = rule->cbx.list_rules;
    cb = list_rules->qualified_rule;

    rule->skip_consume = false;

    cb_qualified = cb(parser, token, rule->context, &qualified_ctx);
    if (cb_qualified == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_UNEXPECTED_DATA);
    }

    qualified = lxb_css_syntax_parser_qualified_push(parser, cb_qualified,
                                                     list_rules->next,
                                                     qualified_ctx,
                                                     rule->block_end,
                                                     false);
    if (qualified == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_MEMORY_ALLOCATION);
    }

    return lxb_css_syntax_token_parser_do_phase_again(parser);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_list_rules_back(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_syntax_rule_t *rule)
{
    rule->phase = lxb_css_syntax_parser_list_rules;
    rule->state = rule->back_state;
    rule->skip_consume = true;

    return token;
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_begin(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_rule_t *rule,
                               lxb_css_parser_state_f back_state,
                               lxb_css_syntax_begin_at_rule_f cb,
                               bool nested)
{
    void *at_rule_ctx;
    lxb_css_syntax_rule_t *at_rule;
    const lxb_css_syntax_cb_at_rule_t *cb_at_rule;

    at_rule_ctx = NULL;
    rule->skip_consume = false;

    cb_at_rule = cb(parser, token, rule->context, &at_rule_ctx);
    if (cb_at_rule == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_UNEXPECTED_DATA);
    }

    at_rule = lxb_css_syntax_parser_at_rule_push(parser, cb_at_rule,
                                                 back_state, at_rule_ctx,
                                                 rule->block_end, nested);
    if (at_rule == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_MEMORY_ALLOCATION);
    }

    lxb_css_syntax_token_consume(parser->tkz);

    token = lxb_css_syntax_token(parser->tkz);
    if (token == NULL) {
        return lxb_css_syntax_parser_failed(parser, parser->tkz->status);
    }

    if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
        lxb_css_syntax_token_consume(parser->tkz);
    }

    return lxb_css_syntax_token_parser_do_phase_again(parser);
}

/*
 * First call for this code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 * Rule -- lxb_css_syntax_rule_t with at_rule callbacks.
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_rule(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              lxb_css_syntax_rule_t *rule)
{
    lxb_status_t status;
    const lxb_css_log_message_t *msg;

    if (rule->offset > token->offset) {
        return token;
    }

    rule->offset = token->offset + lxb_css_syntax_token_base(token)->length;

    if (rule->block_end == token->type && rule->deep == 0) {
        rule->phase = lxb_css_syntax_parser_at_prelude_end;
        goto done;
    }

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RS_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
        case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS);
            break;

        case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
            if (rule->deep == 0) {
                rule->phase = lxb_css_syntax_parser_at_block;
                rule->back = lxb_css_syntax_parser_end_back;
                rule->skip_consume = true;

                return lxb_css_parser_token_end(parser, token->offset);
            }

            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RC_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
            if (rule->deep != 0) {
                if (parser->types_pos[-1] == token->type) {
                    parser->types_pos--;
                    rule->deep--;
                }
            }
            else {
                msg = lxb_css_log_format(parser->log,
                                         LXB_CSS_LOG_SYNTAX_ERROR,
                                         "%S", &lxb_css_err_rc);
                if (msg == NULL) {
                    return lxb_css_syntax_parser_failed(parser,
                                           LXB_STATUS_ERROR_MEMORY_ALLOCATION);
                }

                if (rule->nested) {
                    rule->phase = lxb_css_syntax_parser_at_prelude_end;
                    goto done;
                }
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
        case LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS:
            if (rule->deep != 0 && parser->types_pos[-1] == token->type) {
                parser->types_pos--;
                rule->deep--;
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN_SEMICOLON:
            if (rule->deep == 0) {
                rule->phase = lxb_css_syntax_parser_at_prelude_end_consule_token;
                goto done;
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN__EOF:
            rule->phase = lxb_css_syntax_parser_at_prelude_end;
            goto done;

        default:
            return token;
    }

    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    rule->deep++;

    return token;

done:

    rule->skip_consume = true;

    return lxb_css_parser_token_end(parser, token->offset);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_block(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_rule_t *rule)
{
    lxb_status_t status;
    const lxb_css_syntax_cb_at_rule_t *at_rule = rule->cbx.at_rule;

    status = at_rule->prelude_end(parser, token, rule->context, rule->failed);
    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    return lxb_css_syntax_parser_block_begin(parser, token, rule,
                                             at_rule->prelude, at_rule->block);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_prelude_end(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     lxb_css_syntax_rule_t *rule)
{
    lxb_status_t status;
    const lxb_css_syntax_cb_at_rule_t *at_rule = rule->cbx.at_rule;

    rule->phase = lxb_css_syntax_parser_end;
    rule->state = lxb_css_state_success;

    status = at_rule->prelude_end(parser, token, rule->context, rule->failed);
    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    return lxb_css_syntax_token_parser_do_phase_again(parser);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_prelude_end_consule_token(lxb_css_parser_t *parser,
                                                   const lxb_css_syntax_token_t *token,
                                                   lxb_css_syntax_rule_t *rule)
{
    lxb_status_t status;
    const lxb_css_syntax_cb_at_rule_t *at_rule = rule->cbx.at_rule;

    rule->phase = lxb_css_syntax_parser_end_consume_token;

    status = at_rule->prelude_end(parser, token, rule->context, rule->failed);
    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    return lxb_css_syntax_token_parser_do_phase_again(parser);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_at_back_to_prelude(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_syntax_rule_t *rule)
{
    rule->phase = lxb_css_syntax_parser_at_rule;
    rule->state = rule->back_state;
    rule->skip_consume = false;

    return token;
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_qualified_rule(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     lxb_css_syntax_rule_t *rule)
{
    lxb_status_t status;
    const lxb_css_log_message_t *msg;

    if (rule->offset > token->offset) {
        return token;
    }

    rule->offset = token->offset + lxb_css_syntax_token_base(token)->length;

    if (rule->block_end == token->type && rule->deep == 0) {
        goto done;
    }

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RS_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
        case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS);
            break;

        case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
            if (rule->deep == 0) {
                rule->phase = lxb_css_syntax_parser_qualified_rule_block;
                rule->back = lxb_css_syntax_parser_end_back;
                rule->skip_consume = true;

                return lxb_css_parser_token_end(parser, token->offset);
            }

            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RC_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
            if (rule->deep != 0) {
                if (parser->types_pos[-1] == token->type) {
                    parser->types_pos--;
                    rule->deep--;
                }
            }
            else {
                msg = lxb_css_log_format(parser->log,
                                         LXB_CSS_LOG_SYNTAX_ERROR,
                                         "%S", &lxb_css_err_rc);
                if (msg == NULL) {
                    return lxb_css_syntax_parser_failed(parser,
                                        LXB_STATUS_ERROR_MEMORY_ALLOCATION);
                }

                if (rule->nested) {
                    goto done;
                }
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
        case LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS:
            if (rule->deep != 0 && parser->types_pos[-1] == token->type) {
                parser->types_pos--;
                rule->deep--;
            }

            return token;
 
        case LXB_CSS_SYNTAX_TOKEN__EOF:
            goto done;

        default:
            return token;
    }

    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    rule->deep++;

    return token;

done:

    rule->phase = lxb_css_syntax_parser_qualified_prelude_end;
    rule->skip_consume = true;

    return lxb_css_parser_token_end(parser, token->offset);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_qualified_rule_block(lxb_css_parser_t *parser,
                                           const lxb_css_syntax_token_t *token,
                                           lxb_css_syntax_rule_t *rule)
{
    const lxb_css_syntax_cb_qualified_rule_t *qualified_rule;

    qualified_rule = rule->cbx.qualified_rule;

    parser->status = qualified_rule->prelude_end(parser, token,
                                                 rule->context, rule->failed);
    if (parser->status != LXB_STATUS_OK) {
        return NULL;
    }

    return lxb_css_syntax_parser_block_begin(parser, token, rule,
                                             qualified_rule->prelude,
                                             qualified_rule->block);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_qualified_prelude_end(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token,
                                            lxb_css_syntax_rule_t *rule)
{
    const lxb_css_syntax_cb_qualified_rule_t *qualified_rule;

    qualified_rule = rule->cbx.qualified_rule;

    parser->status = qualified_rule->prelude_end(parser, token,
                                                 rule->context, rule->failed);
    if (parser->status != LXB_STATUS_OK) {
        return NULL;
    }

    rule->phase = lxb_css_syntax_parser_end;
    rule->state = lxb_css_state_success;

    return lxb_css_syntax_token_parser_do_phase_again(parser);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_qualified_back_to_prelude(lxb_css_parser_t *parser,
                                                const lxb_css_syntax_token_t *token,
                                                lxb_css_syntax_rule_t *rule)
{
    rule->phase = lxb_css_syntax_parser_qualified_rule;
    rule->state = rule->back_state;
    rule->skip_consume = false;

    return token;
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block_begin(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  lxb_css_syntax_rule_t *rule,
                                  lxb_css_parser_state_f back_state,
                                  lxb_css_syntax_begin_block_f cb)
{
    void *block_ctx;
    lxb_css_syntax_rule_t *block;
    const lxb_css_syntax_cb_block_t *cb_block;

    block_ctx = NULL;
    rule->skip_consume = false;

    cb_block = cb(parser, token, rule->context, &block_ctx);
    if (cb_block == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_UNEXPECTED_DATA);
    }

    block = lxb_css_syntax_parser_block_push(parser, cb_block, back_state,
                                             block_ctx);
    if (block == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_MEMORY_ALLOCATION);
    }

    lxb_css_syntax_token_consume(parser->tkz);

    return lxb_css_syntax_token_parser_do_phase_again(parser);
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token,
                            lxb_css_syntax_rule_t *rule)
{
    if (rule->offset > token->offset) {
        return token;
    }

begin:

    rule->offset = token->offset + lxb_css_syntax_token_base(token)->length;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_SEMICOLON:
        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            lxb_css_syntax_token_consume(parser->tkz);

            token = lxb_css_syntax_token(parser->tkz);
            if (token == NULL) {
                return lxb_css_syntax_parser_failed(parser,
                                                    parser->tkz->status);
            }

            goto begin;

        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
            if (rule->deep == 0) {
                rule->phase = lxb_css_syntax_parser_end_consume_token;
                break;
            }

            if (parser->types_pos[-1] == token->type) {
                parser->types_pos--;
                rule->deep--;
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN__EOF:
            rule->phase = lxb_css_syntax_parser_end;
            break;

        case LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD:
            rule->phase = lxb_css_syntax_parser_block_at;
            break;

        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            if (lxb_css_syntax_tokenizer_lookup_colon(parser->tkz)) {
                rule->phase = lxb_css_syntax_parser_block_declarations;
                break;
            }

            /* Fall Through. */

        default:
            rule->phase = lxb_css_syntax_parser_block_qualified;
            break;
    }

    rule->skip_consume = true;

    return lxb_css_parser_token_end(parser, 0);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block_at(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_rule_t *rule)
{
    const lxb_css_syntax_cb_block_t *block = rule->cbx.block;

    return lxb_css_syntax_parser_at_begin(parser, token, rule,
                                          block->next, block->at_rule, true);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block_declarations(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_syntax_rule_t *rule)
{
    void *declr_ctx;
    lxb_css_syntax_rule_t *declr_rule;
    lxb_css_syntax_begin_declarations_f cb;
    const lxb_css_syntax_cb_block_t *block;
    const lxb_css_syntax_cb_declarations_t *cb_declr;

    declr_ctx = NULL;
    block = rule->cbx.block;
    cb = block->declarations;

    rule->skip_consume = false;

    cb_declr = cb(parser, token, rule->context, &declr_ctx);
    if (cb_declr == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_UNEXPECTED_DATA);
    }

    declr_rule = lxb_css_syntax_parser_declarations_push(parser, cb_declr,
                                                         block->next, declr_ctx,
                                                         LXB_CSS_SYNTAX_TOKEN_RC_BRACKET,
                                                         false, true);
    if (declr_rule == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_MEMORY_ALLOCATION);
    }

    return lxb_css_syntax_token_parser_do_phase_again(parser);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block_qualified(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_syntax_rule_t *rule)
{
    void *qualified_ctx;
    lxb_css_syntax_rule_t *qualified;
    const lxb_css_syntax_cb_block_t *block;
    lxb_css_syntax_begin_qualified_rule_f cb;
    const lxb_css_syntax_cb_qualified_rule_t *cb_qualified;

    qualified_ctx = NULL;
    block = rule->cbx.block;
    cb = block->qualified_rule;

    rule->skip_consume = false;

    cb_qualified = cb(parser, token, rule->context, &qualified_ctx);
    if (cb_qualified == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_UNEXPECTED_DATA);
    }

    qualified = lxb_css_syntax_parser_qualified_push(parser, cb_qualified,
                                                     block->next, qualified_ctx,
                                                     LXB_CSS_SYNTAX_TOKEN_SEMICOLON,
                                                     true);
    if (qualified == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_MEMORY_ALLOCATION);
    }

    return lxb_css_syntax_token_parser_do_phase_again(parser);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_block_back(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 lxb_css_syntax_rule_t *rule)
{
    rule->phase = lxb_css_syntax_parser_block;
    rule->state = rule->back_state;
    rule->skip_consume = true;

    return token;
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_validate(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token,
                                            lxb_css_syntax_rule_t *rule)
{
    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT
        || !lxb_css_syntax_tokenizer_lookup_colon(parser->tkz))
    {
        rule->phase = lxb_css_syntax_parser_declarations_drop;
        rule->state = rule->cbx.cb->failed;
        rule->begin = token->offset;
        rule->context_old = rule->context;
        rule->context = NULL;
        rule->failed = true;

        parser->offset.value_end = 0;

        return lxb_css_syntax_token_parser_do_phase_again(parser);
    }

    return lxb_css_syntax_parser_declarations_begin(parser, token, rule);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_begin(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_syntax_rule_t *rule)
{
    void *declr_ctx;
    lxb_css_syntax_declaration_name_f cb;

    declr_ctx = NULL;
    cb = rule->cbx.declarations->name;

    rule->state = cb(parser, token, rule->context, &declr_ctx);
    if (rule->state == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            LXB_STATUS_ERROR_UNEXPECTED_DATA);
    }

    rule->phase = lxb_css_syntax_parser_declarations_name;
    rule->context_old = rule->context;
    rule->context = declr_ctx;
    rule->skip_consume = false;

    lxb_css_syntax_token_consume(parser->tkz);

    return lxb_css_syntax_token_parser_do_phase_again(parser);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_name(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        lxb_css_syntax_rule_t *rule)
{
    /* 1. */

    if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
        lxb_css_syntax_token_consume(parser->tkz);

        token = lxb_css_syntax_token(parser->tkz);
        if (token == NULL) {
            return lxb_css_syntax_parser_failed(parser, parser->tkz->status);
        }
    }

    /* 2. */

    if (token->type != LXB_CSS_SYNTAX_TOKEN_COLON) {
        /* Parse error. */

        /*
         * It can't be.
         *
         * Before entering the lxb_css_syntax_parser_declarations_name()
         * function, data validation takes place. In fact, these checks are not
         * needed here.
         */

        /*
         * But it's good for validation, if we come here it means we're
         * doing badly.
         */

        return NULL;
    }

    lxb_css_syntax_token_consume(parser->tkz);

    token = lxb_css_syntax_token(parser->tkz);
    if (token == NULL) {
        return lxb_css_syntax_parser_failed(parser, parser->tkz->status);
    }

    /* 3. */

    if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
        lxb_css_syntax_token_consume(parser->tkz);

        token = lxb_css_syntax_token(parser->tkz);
        if (token == NULL) {
            return lxb_css_syntax_parser_failed(parser, parser->tkz->status);
        }
    }

    rule->begin = token->offset;
    parser->offset.value_end = 0;

    /* 4. */

    rule->phase = lxb_css_syntax_parser_declarations_value;

    return lxb_css_syntax_parser_declarations_value(parser, token, rule);
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_value(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_syntax_rule_t *rule)
{
    bool imp;
    size_t offset;
    lxb_status_t status;

    if (rule->offset > token->offset) {
        return token;
    }

again:

    rule->offset = token->offset + lxb_css_syntax_token_base(token)->length;

    if (rule->block_end == token->type && rule->deep == 0) {
        goto done;
    }

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            if (rule->deep != 0) {
                return token;
            }

            imp = lxb_css_syntax_tokenizer_lookup_declaration_ws_end(parser->tkz,
            rule->block_end,
            (rule->block_end == LXB_CSS_SYNTAX_TOKEN_RC_BRACKET) ? 0x7D : 0x00);

            if (!imp) {
                return token;
            }

            parser->offset.value_end = token->offset;

            lxb_css_syntax_token_consume(parser->tkz);

            token = lxb_css_syntax_token(parser->tkz);
            if (token == NULL) {
                return lxb_css_syntax_parser_failed(parser, parser->tkz->status);
            }

            /* Have !important? */

            if (token->type == LXB_CSS_SYNTAX_TOKEN_DELIM) {
                rule->important = true;
                parser->offset.important_begin = token->offset;

                lxb_css_syntax_token_consume(parser->tkz);

                /* Skip important */

                token = lxb_css_syntax_token(parser->tkz);
                if (token == NULL) {
                    return lxb_css_syntax_parser_failed(parser, parser->tkz->status);
                }

                parser->offset.important_end = token->offset
                    + lxb_css_syntax_token_base(token)->length;

                lxb_css_syntax_token_consume(parser->tkz);

                token = lxb_css_syntax_token(parser->tkz);
                if (token == NULL) {
                    return lxb_css_syntax_parser_failed(parser, parser->tkz->status);
                }

                if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
                    lxb_css_syntax_token_consume(parser->tkz);

                    token = lxb_css_syntax_token(parser->tkz);
                    if (token == NULL) {
                        return lxb_css_syntax_parser_failed(parser,
                                                            parser->tkz->status);
                    }
                }
            }
            else {
                parser->offset.important_begin = 0;
                parser->offset.important_end = 0;
            }

            goto again;

        case LXB_CSS_SYNTAX_TOKEN_SEMICOLON:
            if (rule->deep == 0) {
                rule->phase = lxb_css_syntax_parser_declarations_next;

                offset = token->offset;

                parser->offset.value_begin = rule->begin;
                parser->offset.end = offset;

                if (parser->offset.value_end == 0) {
                    parser->offset.value_end = offset;
                    parser->offset.important_begin = 0;
                    parser->offset.important_end = 0;
                }

                lxb_css_syntax_token_consume(parser->tkz);

                return lxb_css_parser_token_end(parser, offset);
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            if (lxb_css_syntax_token_delim(token)->character != '!') {
                return token;
            }

            imp = lxb_css_syntax_tokenizer_lookup_important(parser->tkz,
            rule->block_end,
            (rule->block_end == LXB_CSS_SYNTAX_TOKEN_RC_BRACKET) ? 0x7D : 0x00);

            if (!imp) {
                return token;
            }

            rule->important = true;

            parser->offset.value_end = token->offset;
            parser->offset.important_begin = token->offset;

            lxb_css_syntax_token_consume(parser->tkz);

            token = lxb_css_syntax_token(parser->tkz);
            if (token == NULL) {
                return lxb_css_syntax_parser_failed(parser, parser->tkz->status);
            }

            parser->offset.important_end = token->offset
                + lxb_css_syntax_token_base(token)->length;

            lxb_css_syntax_token_consume(parser->tkz);

            token = lxb_css_syntax_token(parser->tkz);
            if (token == NULL) {
                return lxb_css_syntax_parser_failed(parser, parser->tkz->status);
            }

            if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
                lxb_css_syntax_token_consume(parser->tkz);

                token = lxb_css_syntax_token(parser->tkz);
                if (token == NULL) {
                    return lxb_css_syntax_parser_failed(parser,
                                                        parser->tkz->status);
                }
            }

            goto again;

        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RS_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
        case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS);
            break;

        case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RC_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
        case LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS:
            if (rule->deep != 0 && parser->types_pos[-1] == token->type) {
                parser->types_pos--;
                rule->deep--;
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN__EOF:
            goto done;

        default:
            return token;
    }

    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    rule->deep++;

    return token;

done:

    parser->offset.value_begin = rule->begin;
    parser->offset.end = token->offset;

    if (parser->offset.value_end == 0) {
        parser->offset.value_end = token->offset;
        parser->offset.important_begin = 0;
        parser->offset.important_end = 0;
    }

    rule->phase = lxb_css_syntax_parser_declaration_end;
    rule->skip_consume = true;

    return lxb_css_parser_token_end(parser, token->offset);
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_drop(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        lxb_css_syntax_rule_t *rule)
{
    size_t offset;
    lxb_status_t status;

    if (rule->offset > token->offset) {
        return token;
    }

    rule->offset = token->offset + lxb_css_syntax_token_base(token)->length;

    if (rule->block_end == token->type && rule->deep == 0) {
        goto done;
    }

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_SEMICOLON:
            if (rule->deep == 0) {
                rule->phase = lxb_css_syntax_parser_declarations_next;

                offset = token->offset;

                parser->offset.value_begin = rule->begin;
                parser->offset.end = offset;

                if (parser->offset.value_end == 0) {
                    parser->offset.value_end = offset;
                    parser->offset.important_begin = 0;
                    parser->offset.important_end = 0;
                }

                lxb_css_syntax_token_consume(parser->tkz);

                return lxb_css_parser_token_end(parser, offset);
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RS_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
        case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS);
            break;

        case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RC_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
            if (rule->deep == 0) {
                if (rule->nested) {
                    goto done;
                }
            }
            else if (parser->types_pos[-1] == token->type) {
                parser->types_pos--;
                rule->deep--;
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
        case LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS:
            if (rule->deep != 0 && parser->types_pos[-1] == token->type) {
                parser->types_pos--;
                rule->deep--;
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN__EOF:
            goto done;

        default:
            return token;
    }

    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    rule->deep++;

    return token;

done:

    parser->offset.value_begin = rule->begin;
    parser->offset.end = token->offset;

    if (parser->offset.value_end == 0) {
        parser->offset.value_end = token->offset;
        parser->offset.important_begin = 0;
        parser->offset.important_end = 0;
    }

    rule->phase = lxb_css_syntax_parser_declaration_end;
    rule->skip_consume = true;

    return lxb_css_parser_token_end(parser, token->offset);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_next(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        lxb_css_syntax_rule_t *rule)
{
    void *out_rule;
    lxb_status_t status;
    lxb_css_syntax_declaration_name_f name;

    if (rule->state != lxb_css_state_success) {
        rule->skip_consume = true;

        return lxb_css_parser_token_end(parser, token->offset);
    }

    status = rule->cbx.declarations->end(parser, rule->context_old,
                                         rule->context, token, &parser->offset,
                                         rule->important, rule->failed);
    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

begin:

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_SEMICOLON:
        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            lxb_css_syntax_token_consume(parser->tkz);

            token = lxb_css_syntax_token(parser->tkz);
            if (token == NULL) {
                return lxb_css_syntax_parser_failed(parser,
                                                    parser->tkz->status);
            }

            goto begin;

        case LXB_CSS_SYNTAX_TOKEN__EOF:
            rule->phase = lxb_css_syntax_parser_declarations_end;
            break;

        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            if (lxb_css_syntax_tokenizer_lookup_colon(parser->tkz)) {
                out_rule = NULL;
                name = rule->cbx.declarations->name;
                rule->skip_consume = true;

                rule->state = name(parser, token, rule->context, &out_rule);
                if (rule->state == NULL) {
                    return lxb_css_syntax_parser_failed(parser,
                                                        LXB_STATUS_ERROR_UNEXPECTED_DATA);
                }

                rule->context = out_rule;

                lxb_css_syntax_token_consume(parser->tkz);

                rule->phase = lxb_css_syntax_parser_declarations_name;
                rule->context = out_rule;

                break;
            }

            /* Fall through. */

        default:
            if (rule->nested) {
                rule->phase = lxb_css_syntax_parser_declarations_end;
            }
            else {
                rule->phase = lxb_css_syntax_parser_declarations_drop;
                rule->state = rule->cbx.cb->failed;
                rule->begin = token->offset;
                rule->context = NULL;
                rule->failed = true;

                parser->offset.value_end = 0;

                return lxb_css_syntax_token_parser_do_phase_again(parser);
            }

            break;
    }

    rule->skip_consume = false;
    rule->important = false;
    rule->failed = false;

    return lxb_css_syntax_token_parser_do_phase_again(parser);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declaration_back_to_value(lxb_css_parser_t *parser,
                                                const lxb_css_syntax_token_t *token,
                                                lxb_css_syntax_rule_t *rule)
{
    rule->phase = lxb_css_syntax_parser_declarations_value;
    rule->state = rule->back_state;
    rule->skip_consume = false;

    return token;
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declaration_end(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_syntax_rule_t *rule)
{
    return lxb_css_syntax_parser_declaration_end_handler(parser, token,
                                                         rule, false);
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declaration_end_handler(lxb_css_parser_t *parser,
                                              const lxb_css_syntax_token_t *token,
                                              lxb_css_syntax_rule_t *rule,
                                              bool skip_token)
{
    lxb_status_t status;

    if (rule->state != lxb_css_state_success) {
        rule->skip_consume = true;

        return lxb_css_parser_token_end(parser, token->offset);
    }

    status = rule->cbx.declarations->end(parser, rule->context_old,
                                         rule->context, token, &parser->offset,
                                         rule->important, rule->failed);
    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    return lxb_css_syntax_parser_declarations_end_h(parser, token,
                                                    rule, skip_token);
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_end(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       lxb_css_syntax_rule_t *rule)
{
    return lxb_css_syntax_parser_declarations_end_h(parser, token,
                                                    rule, false);
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_declarations_end_h(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_syntax_rule_t *rule,
                                         bool skip_token)
{
    lxb_status_t status;

    /* This code will be called exclusively from the lxb_css_parser_run(...). */

    rule->context = rule->context_old;

    status = rule->cbx.cb->end(parser, token, rule->context, false);
    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    if (skip_token) {
        lxb_css_syntax_token_consume(parser->tkz);

        token = lxb_css_syntax_token(parser->tkz);
        if (token == NULL) {
            return lxb_css_syntax_parser_failed(parser,
                                                parser->tkz->status);
        }
    }

    return lxb_css_syntax_parser_call_back(parser, token);
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_components(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 lxb_css_syntax_rule_t *rule)
{
    lxb_status_t status;

    if (rule->offset > token->offset) {
        return token;
    }

    rule->offset = token->offset + lxb_css_syntax_token_base(token)->length;

    if (rule->block_end == token->type && rule->deep == 0) {
        goto done;
    }

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RS_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
        case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS);
            break;

        case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RC_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
        case LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS:
            if (rule->deep != 0 && parser->types_pos[-1] == token->type) {
                parser->types_pos--;
                rule->deep--;
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN__EOF:
            goto done;

        default:
            return token;
    }

    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    rule->deep++;

    return token;

done:

    rule->phase = lxb_css_syntax_parser_end;
    rule->skip_consume = true;

    return lxb_css_parser_token_end(parser, token->offset);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_components_back_to_value(lxb_css_parser_t *parser,
                                               const lxb_css_syntax_token_t *token,
                                               lxb_css_syntax_rule_t *rule)
{
    rule->phase = lxb_css_syntax_parser_components;
    rule->state = rule->back_state;
    rule->skip_consume = false;

    return token;
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_function(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_rule_t *rule)
{
    lxb_status_t status;

    if (rule->offset > token->offset) {
        return token;
    }

    rule->offset = token->offset + lxb_css_syntax_token_base(token)->length;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RS_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
        case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS);
            break;

        case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
            status = lxb_css_parser_types_push(parser,
                                               LXB_CSS_SYNTAX_TOKEN_RC_BRACKET);
            break;

        case LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS:
            if (rule->deep == 0)  {
                rule->phase = lxb_css_syntax_parser_end_consume_token;
                goto done;
            }

            if (parser->types_pos[-1] == token->type) {
                parser->types_pos--;
                rule->deep--;
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
            if (rule->deep != 0 && parser->types_pos[-1] == token->type) {
                parser->types_pos--;
                rule->deep--;
            }

            return token;

        case LXB_CSS_SYNTAX_TOKEN__EOF:
            rule->phase = lxb_css_syntax_parser_end;
            goto done;

        default:
            return token;
    }

    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    rule->deep++;

    return token;

done:

    rule->skip_consume = true;

    return lxb_css_parser_token_end(parser, token->offset);
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_function_back(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    lxb_css_syntax_rule_t *rule)
{
    rule->phase = lxb_css_syntax_parser_function;
    rule->state = rule->back_state;
    rule->skip_consume = false;

    return token;
}

static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_pipe(lxb_css_parser_t *parser,
                           const lxb_css_syntax_token_t *token,
                           lxb_css_syntax_rule_t *rule)
{
    if ((rule->block_end == token->type && rule->deep == 0)
        || token->type == LXB_CSS_SYNTAX_TOKEN__EOF)
    {
        rule->phase = lxb_css_syntax_parser_end;
        rule->skip_consume = true;

        return lxb_css_parser_token_end(parser, token->offset);
    }

    return token;
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_pipe_back(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token,
                                lxb_css_syntax_rule_t *rule)
{
    rule->phase = lxb_css_syntax_parser_pipe;
    rule->state = rule->back_state;
    rule->skip_consume = false;

    return token;
}

/*
 * This code will be called before rule->state is called.
 * Exclusively from the lxb_css_parser_run(...).
 */
static const lxb_css_syntax_token_t *
lxb_css_syntax_parser_end_back(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_syntax_rule_t *rule)
{
    rule->phase = lxb_css_syntax_parser_end;
    rule->state = lxb_css_state_blank;

    return token;
}

const lxb_css_syntax_token_t *
lxb_css_syntax_parser_end(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          lxb_css_syntax_rule_t *rule)
{
    lxb_status_t status;
    lxb_css_syntax_cb_base_t *base;

    rule->skip_consume = true;

    if (rule->state != lxb_css_state_success) {
        return lxb_css_parser_token_end(parser, token->offset);
    }

    /* This code will be called exclusively from the lxb_css_parser_run(...). */

    base = rule->cbx.user;

    status = base->end(parser, token, rule->context, rule->failed);
    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    rule->skip_consume = false;

    return lxb_css_syntax_parser_call_back(parser, token);
}

const lxb_css_syntax_token_t *
lxb_css_syntax_parser_end_consume_token(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        lxb_css_syntax_rule_t *rule)
{
    lxb_status_t status;
    lxb_css_syntax_cb_base_t *base;

    rule->skip_consume = true;

    if (rule->state != lxb_css_state_success) {
        return lxb_css_parser_token_end(parser, token->offset);
    }

    /* This code will be called exclusively from the lxb_css_parser_run(...). */

    base = rule->cbx.user;

    status = base->end(parser, token, rule->context, rule->failed);
    if (status != LXB_STATUS_OK) {
        return lxb_css_syntax_parser_failed(parser, status);
    }

    rule->skip_consume = false;

    lxb_css_syntax_token_consume(parser->tkz);

    token = lxb_css_syntax_token(parser->tkz);
    if (token == NULL) {
        return lxb_css_syntax_parser_failed(parser,
                                            parser->tkz->status);
    }

    return lxb_css_syntax_parser_call_back(parser, token);
}
