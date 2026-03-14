/*
 * Copyright (C) 2021-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/state.h"
#include "lexbor/css/css.h"
#include "lexbor/css/at_rule/state.h"


static bool
lxb_css_state_list_rules_next(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx);

static lxb_status_t
lxb_css_state_list_rules_end(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token,
                             void *ctx, bool failed);

static const lxb_css_syntax_cb_at_rule_t *
lxb_css_state_at_rule_begin(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token, void *ctx,
                            void **out_rule);

static bool
lxb_css_state_at_rule_prelude(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              void *ctx);

static lxb_status_t
lxb_css_state_at_rule_prelude_end(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, bool failed);
static bool
lxb_css_state_at_rule_prelude_failed(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx);
static lxb_status_t
lxb_css_state_at_rule_end(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          void *ctx, bool failed);

static const lxb_css_syntax_cb_qualified_rule_t *
lxb_css_state_qualified_rule_begin(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token,
                                   void *ctx, void **out_rule);

static bool
lxb_css_state_qualified_rule_prelude(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx);

static lxb_status_t
lxb_css_state_qualified_rule_prelude_end(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, bool failed);

static const lxb_css_syntax_cb_block_t *
lxb_css_state_qualified_rule_block_begin(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, void **out_rule);

static bool
lxb_css_state_qualified_rule_prelude_failed(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token,
                                            void *ctx);
static lxb_status_t
lxb_css_state_qualified_rule_end(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx, bool failed);
static bool
lxb_css_state_block_next(lxb_css_parser_t *parser,
                         const lxb_css_syntax_token_t *token, void *ctx);

static lxb_status_t
lxb_css_state_block_end(lxb_css_parser_t *parser,
                        const lxb_css_syntax_token_t *token,
                        void *ctx, bool failed);

static const lxb_css_syntax_cb_declarations_t *
lxb_css_state_declarations_begin(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx, void **out_rule);

static lxb_css_parser_state_f
lxb_css_state_declaration_name(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               void *ctx, void **out_rule);

static lxb_status_t
lxb_css_state_declaration_end(lxb_css_parser_t *parser,
                              void *declarations, void *ctx,
                              const lxb_css_syntax_token_t *token,
                              lxb_css_syntax_declaration_offset_t *offset,
                              bool important, bool failed);

static lxb_status_t
lxb_css_state_declarations_end(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               void *ctx, bool failed);

static bool
lxb_css_state_declarations_bad(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx);


static const lxb_css_syntax_cb_list_rules_t lxb_css_state_list_rules = {
    .at_rule = lxb_css_state_at_rule_begin,
    .qualified_rule = lxb_css_state_qualified_rule_begin,
    .next = lxb_css_state_list_rules_next,
    .cb.failed = lxb_css_state_failed,
    .cb.end = lxb_css_state_list_rules_end
};

static const lxb_css_syntax_cb_at_rule_t lxb_css_state_at_rule = {
    .prelude = lxb_css_state_at_rule_prelude,
    .prelude_end = lxb_css_state_at_rule_prelude_end,
    .block = lxb_css_state_at_rule_block_begin,
    .cb.failed = lxb_css_state_at_rule_prelude_failed,
    .cb.end = lxb_css_state_at_rule_end
};

static const lxb_css_syntax_cb_qualified_rule_t lxb_css_state_qualified_rule = {
    .prelude = lxb_css_state_qualified_rule_prelude,
    .prelude_end = lxb_css_state_qualified_rule_prelude_end,
    .block = lxb_css_state_qualified_rule_block_begin,
    .cb.failed = lxb_css_state_qualified_rule_prelude_failed,
    .cb.end = lxb_css_state_qualified_rule_end
};

static const lxb_css_syntax_cb_block_t lxb_css_state_block = {
    .at_rule = lxb_css_state_at_rule_begin,
    .declarations = lxb_css_state_declarations_begin,
    .qualified_rule = lxb_css_state_qualified_rule_begin,
    .next = lxb_css_state_block_next,
    .cb.failed = lxb_css_state_failed,
    .cb.end = lxb_css_state_block_end,
};

static const lxb_css_syntax_cb_declarations_t lxb_css_state_declaration = {
    .name = lxb_css_state_declaration_name,
    .end = lxb_css_state_declaration_end,
    .cb.failed = lxb_css_state_declarations_bad,
    .cb.end = lxb_css_state_declarations_end
};


bool
lxb_css_state_success(lxb_css_parser_t *parser,
                      const lxb_css_syntax_token_t *token, void *ctx)
{
    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            lxb_css_syntax_parser_consume(parser);
            return true;

        case LXB_CSS_SYNTAX_TOKEN__END:
            return true;

        default:
            break;
    }

    return lxb_css_parser_failed(parser);
}

bool
lxb_css_state_failed(lxb_css_parser_t *parser,
                     const lxb_css_syntax_token_t *token, void *ctx)
{
    if (token->type == LXB_CSS_SYNTAX_TOKEN__END) {
        return lxb_css_parser_success(parser);
    }

    /* The lxb_css_syntax_parser_consume(...) locked in this state. */

    lxb_css_syntax_token_consume(parser->tkz);

    return true;
}

bool
lxb_css_state_stop(lxb_css_parser_t *parser,
                   const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_parser_stop(parser);
}

bool
lxb_css_state_blank(lxb_css_parser_t *parser,
                    const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_parser_success(parser);
}

const lxb_css_syntax_cb_list_rules_t *
lxb_css_state_cb_list_rules(void)
{
    return &lxb_css_state_list_rules;
}

const lxb_css_syntax_cb_at_rule_t *
lxb_css_state_cb_at_rule(void)
{
    return &lxb_css_state_at_rule;
}

const lxb_css_syntax_cb_qualified_rule_t *
lxb_css_state_cb_qualified_rule(void)
{
    return &lxb_css_state_qualified_rule;
}

const lxb_css_syntax_cb_block_t *
lxb_css_state_cb_block(void)
{
    return &lxb_css_state_block;
}

const lxb_css_syntax_cb_declarations_t *
lxb_css_state_cb_declarations(void)
{
    return &lxb_css_state_declaration;
}

static bool
lxb_css_state_list_rules_next(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx)
{
    void *returned = lxb_css_syntax_returned(parser);
    lxb_css_rule_list_t *list = ctx;

    if (returned != NULL) {
        lxb_css_rule_list_append(list, returned);
    }

    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_state_list_rules_end(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token,
                             void *ctx, bool failed)
{
    lxb_css_rule_list_t *list = ctx;

    lxb_css_syntax_set_return(parser, list);

    return LXB_STATUS_OK;
}

static const lxb_css_syntax_cb_at_rule_t *
lxb_css_state_at_rule_begin(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token, void *ctx,
                            void **out_rule)
{
    lxb_css_rule_at_t *at;
    const lxb_css_entry_at_rule_data_t *entry;

    at = lxb_css_at_rule_create(parser, lxb_css_syntax_token_ident(token)->data,
                                lxb_css_syntax_token_ident(token)->length,
                                &entry);
    if (at == NULL) {
        return lxb_css_parser_memory_fail_null(parser);
    }

    *out_rule = at;

    return entry->cbs;
}

static bool
lxb_css_state_at_rule_prelude(lxb_css_parser_t *parser,
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

static lxb_status_t
lxb_css_state_at_rule_prelude_end(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, bool failed)
{
    lxb_status_t status;
    lxb_css_rule_at_t *at = ctx;
    lxb_css_at_rule__custom_t *custom;

    at->prelude_end = token->offset;

    custom = at->u.custom;

    status = lxb_css_make_data(parser, &custom->prelude, at->prelude_begin,
                               at->prelude_end);
    if (status != LXB_STATUS_OK) {
        return lxb_css_parser_memory_fail_status(parser);
    }

    return LXB_STATUS_OK;
}

const lxb_css_syntax_cb_block_t *
lxb_css_state_at_rule_block_begin(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, void **out_rule)
{
    lxb_css_rule_list_t *list;

    list = lxb_css_rule_list_create(parser->memory);
    if (list == NULL) {
        return lxb_css_parser_memory_fail_null(parser);
    }

    *out_rule = list;

    return &lxb_css_state_block;
}

static bool
lxb_css_state_at_rule_prelude_failed(lxb_css_parser_t *parser,
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
lxb_css_state_at_rule_end(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          void *ctx, bool failed)
{
    lxb_css_rule_at_t *at = ctx;
    lxb_css_rule_list_t *block = lxb_css_syntax_returned(parser);

    at->u.custom->block = block;

    lxb_css_syntax_set_return(parser, at);

    return LXB_STATUS_OK;
}

static const lxb_css_syntax_cb_qualified_rule_t *
lxb_css_state_qualified_rule_begin(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token,
                                   void *ctx, void **out_rule)
{
    lxb_css_rule_style_t *style;

    style = lxb_css_rule_style_create(parser->memory);
    if (style == NULL) {
        return lxb_css_parser_memory_fail_null(parser);
    }

    *out_rule = style;

    return &lxb_css_state_qualified_rule;
}

static bool
lxb_css_state_qualified_rule_prelude(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx)
{
    lxb_css_rule_style_t *style = ctx;

    lxb_css_selectors_clean(parser->selectors);
    lxb_css_parser_state_set(parser, lxb_css_selectors_state_complex_list);

    style->prelude_begin = token->offset;

    return false;
}

static lxb_status_t
lxb_css_state_qualified_rule_prelude_end(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, bool failed)
{
    size_t begin;
    lxb_status_t status;
    lxb_css_rule_style_t *style = ctx;
    lxb_css_rule_bad_style_t *bad;

    if (!failed) {
        style->selector = parser->selectors->list;
        style->prelude_end = token->offset;

        return LXB_STATUS_OK;
    }

    begin = style->prelude_begin;

    (void) lxb_css_rule_style_destroy(style, true);

    bad = lxb_css_rule_bad_style_create(parser->memory);
    if (bad == NULL) {
        return lxb_css_parser_memory_fail_status(parser);
    }

    bad->prelude_begin = begin;
    bad->prelude_end = token->offset;

    status = lxb_css_make_data(parser, &bad->selectors, bad->prelude_begin,
                               bad->prelude_end);
    if (status != LXB_STATUS_OK) {
        return lxb_css_parser_memory_fail_status(parser);
    }

    lxb_css_parser_set_context(parser, bad);

    return LXB_STATUS_OK;
}

static const lxb_css_syntax_cb_block_t *
lxb_css_state_qualified_rule_block_begin(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, void **out_rule)
{
    lxb_css_rule_list_t *list;

    list = lxb_css_rule_list_create(parser->memory);
    if (list == NULL) {
        return lxb_css_parser_memory_fail_null(parser);
    }

    *out_rule = list;

    return &lxb_css_state_block;
}

static bool
lxb_css_state_qualified_rule_prelude_failed(lxb_css_parser_t *parser,
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
lxb_css_state_qualified_rule_end(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx, bool failed)
{
    lxb_css_rule_t *declrs;
    lxb_css_rule_list_t *list;
    lxb_css_rule_style_t *style;
    lxb_css_rule_bad_style_t *bad;

    list = lxb_css_syntax_returned(parser);

    if (!failed) {
        style = ctx;

        if (list != NULL && list->first != NULL
            && list->first->type == LXB_CSS_RULE_DECLARATION_LIST)
        {
            declrs = list->first;
            style->declarations = lxb_css_rule_declaration_list(declrs);

            declrs->parent = lxb_css_rule(style);

            if (list->first == list->last) {
                list->last = NULL;
            }

            if (declrs->next != NULL) {
                declrs->next->prev = NULL;
            }

            list->first = declrs->next;
            declrs->next = NULL;
        }

        style->child = list;

        lxb_css_syntax_set_return(parser, style);
    }
    else {
        bad = ctx;

        if (list != NULL && list->first != NULL
            && list->first->type == LXB_CSS_RULE_DECLARATION_LIST)
        {
            declrs = list->first;
            bad->declarations = lxb_css_rule_declaration_list(declrs);

            declrs->parent = lxb_css_rule(bad);

            if (list->first == list->last) {
                list->last = NULL;
            }

            if (declrs->next != NULL) {
                declrs->next->prev = NULL;
            }

            list->first = declrs->next;
            declrs->next = NULL;
        }

        bad->child = list;

        lxb_css_syntax_set_return(parser, bad);
    }

    return LXB_STATUS_OK;
}

static bool
lxb_css_state_block_next(lxb_css_parser_t *parser,
                         const lxb_css_syntax_token_t *token, void *ctx)
{
    /* rule can be either a declaration list or an at rule.  */

    lxb_css_rule_t *rule = lxb_css_syntax_returned(parser);
    lxb_css_rule_list_t *list = ctx;

    lxb_css_rule_list_append(list, rule);

    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_state_block_end(lxb_css_parser_t *parser,
                        const lxb_css_syntax_token_t *token,
                        void *ctx, bool failed)
{
    lxb_css_rule_list_t *list = ctx;

    lxb_css_syntax_set_return(parser, list);

    return LXB_STATUS_OK;
}

static const lxb_css_syntax_cb_declarations_t *
lxb_css_state_declarations_begin(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx, void **out_rule)
{
    lxb_css_rule_declaration_list_t *list;

    list = lxb_css_rule_declaration_list_create(parser->memory);
    if (list == NULL) {
        return lxb_css_parser_memory_fail_null(parser);
    }

    *out_rule = list;

    return &lxb_css_state_declaration;
}

static lxb_css_parser_state_f
lxb_css_state_declaration_name(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               void *ctx, void **out_rule)
{
    const lxb_css_entry_data_t *entry;
    lxb_css_rule_declaration_t *declar;

    declar = lxb_css_declaration_create(parser,
                                        lxb_css_syntax_token_ident(token)->data,
                                        lxb_css_syntax_token_ident(token)->length,
                                        &entry);
    if (declar == NULL) {
        (void) lxb_css_parser_memory_fail_null(parser);
        return NULL;
    }

    /* We present an original position. */

    declar->offset.name_begin = token->offset;
    declar->offset.name_end = token->offset
                                + lxb_css_syntax_token_base(token)->length;
    *out_rule = declar;

    return entry->state;
}

static lxb_status_t
lxb_css_state_declaration_end(lxb_css_parser_t *parser,
                              void *declarations, void *ctx,
                              const lxb_css_syntax_token_t *token,
                              lxb_css_syntax_declaration_offset_t *offset,
                              bool important, bool failed)
{
    lxb_status_t status;
    lxb_css_property__undef_t *undef;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_rule_declaration_list_t *list = declarations;

    declar->offset.value_begin = offset->value_begin;
    declar->offset.value_end = offset->value_end;
    declar->offset.important_begin = offset->important_begin;
    declar->offset.important_end = offset->important_end;
    declar->important = important;

    if (failed) {
        lxb_css_rule_declaration_destroy(declar, false);

        undef = lxb_css_property__undef_create(parser->memory);
        if (undef == NULL) {
            return lxb_css_parser_memory_fail_status(parser);
        }

        undef->type = declar->type;

        status = lxb_css_make_data(parser, &undef->value,
                                   declar->offset.value_begin,
                                   declar->offset.value_end);
        if (status != LXB_STATUS_OK) {
            return lxb_css_parser_memory_fail_status(parser);
        }

        declar->u.undef = undef;
        declar->type = LXB_CSS_PROPERTY__UNDEF;
    }

    lxb_css_rule_declaration_list_append(list, lxb_css_rule(declar));

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_state_declarations_end(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               void *ctx, bool failed)
{
    lxb_css_rule_declaration_list_t *list = ctx;

    lxb_css_syntax_set_return(parser, list);

    return LXB_STATUS_OK;
}

static bool
lxb_css_state_declarations_bad(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_rule_declaration_t *declar;

    if (ctx == NULL) {
        declar = lxb_css_rule_declaration_create(parser->memory);
        if (declar == NULL) {
            return lxb_css_parser_memory_fail(parser);
        }

        lxb_css_parser_current_rule(parser)->context = declar;
        declar->type = LXB_CSS_PROPERTY__UNDEF;
    }

    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}
