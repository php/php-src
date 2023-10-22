/*

 * Copyright (C) 2021-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_PARSER_H
#define LEXBOR_CSS_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/log.h"
#include "lexbor/css/stylesheet.h"
#include "lexbor/css/syntax/parser.h"
#include "lexbor/css/selectors/selectors.h"


#define lxb_css_parser_token_m(parser, token)                                  \
    do {                                                                       \
        token = lxb_css_syntax_parser_token(parser);                           \
        if (token == NULL) {                                                   \
            return lxb_css_parser_fail((parser), (parser)->tkz->status);       \
        }                                                                      \
    }                                                                          \
    while (false)

#define lxb_css_parser_token_next_m(parser, token)                             \
    do {                                                                       \
        token = lxb_css_syntax_token_next((parser)->tkz);                      \
        if (token == NULL) {                                                   \
            return lxb_css_parser_fail((parser), (parser)->tkz->status);       \
        }                                                                      \
    }                                                                          \
    while (false)

#define lxb_css_parser_token_wo_ws_m(parser, token)                            \
    do {                                                                       \
        token = lxb_css_syntax_parser_token(parser);                           \
        if (token == NULL) {                                                   \
            return lxb_css_parser_fail((parser), (parser)->tkz->status);       \
        }                                                                      \
                                                                               \
        if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {                  \
            lxb_css_syntax_parser_consume(parser);                             \
            token = lxb_css_syntax_parser_token(parser);                       \
            if (token == NULL) {                                               \
                return lxb_css_parser_fail((parser), (parser)->tkz->status);   \
            }                                                                  \
        }                                                                      \
    }                                                                          \
    while (false)

#define lxb_css_parser_token_status_m(parser, token)                           \
    do {                                                                       \
        if ((token = lxb_css_syntax_parser_token(parser)) == NULL) {           \
            return parser->tkz->status;                                        \
        }                                                                      \
    }                                                                          \
    while (false)

#define lxb_css_parser_token_status_next_m(parser, token)                      \
    do {                                                                       \
        token = lxb_css_syntax_token_next((parser)->tkz);                      \
        if (token == NULL) {                                                   \
            return parser->tkz->status;                                        \
        }                                                                      \
    }                                                                          \
    while (false)


#define lxb_css_parser_token_status_wo_ws_m(parser, token)                     \
    do {                                                                       \
        if ((token = lxb_css_syntax_parser_token(parser)) == NULL) {           \
            return parser->tkz->status;                                        \
        }                                                                      \
                                                                               \
        if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {                  \
            lxb_css_syntax_parser_consume(parser);                             \
            if ((token = lxb_css_syntax_parser_token(parser)) == NULL) {       \
                return parser->tkz->status;                                    \
            }                                                                  \
        }                                                                      \
    }                                                                          \
    while (false)

#define lxb_css_parser_string_dup_m(parser, token, _str, mraw)                 \
    do {                                                                       \
        if (lxb_css_syntax_token_string_dup(lxb_css_syntax_token_string(token),\
                                            (_str), (mraw)) != LXB_STATUS_OK)  \
        {                                                                      \
            return lxb_css_parser_fail((parser),                               \
                                       LXB_STATUS_ERROR_MEMORY_ALLOCATION);    \
        }                                                                      \
    }                                                                          \
    while (false)


enum {
    LXB_CSS_SYNTAX_PARSER_ERROR_UNDEF = 0x0000,
    /* eof-in-at-rule */
    LXB_CSS_SYNTAX_PARSER_ERROR_EOINATRU,
    /* eof-in-qualified-rule */
    LXB_CSS_SYNTAX_PARSER_ERROR_EOINQURU,
    /* eof-in-simple-block */
    LXB_CSS_SYNTAX_PARSER_ERROR_EOINSIBL,
    /* eof-in-function */
    LXB_CSS_SYNTAX_PARSER_ERROR_EOINFU,
    /* eof-before-parse-rule */
    LXB_CSS_SYNTAX_PARSER_ERROR_EOBEPARU,
    /* unexpected-token-after-parse-rule */
    LXB_CSS_SYNTAX_PARSER_ERROR_UNTOAFPARU,
    /* eof-before-parse-component-value */
    LXB_CSS_SYNTAX_PARSER_ERROR_EOBEPACOVA,
    /* unexpected-token-after-parse-component-value */
    LXB_CSS_SYNTAX_PARSER_ERROR_UNTOAFPACOVA,
    /* unexpected-token-in-declaration */
    LXB_CSS_SYNTAX_PARSER_ERROR_UNTOINDE,
};

typedef enum {
    LXB_CSS_PARSER_CLEAN = 0,
    LXB_CSS_PARSER_RUN,
    LXB_CSS_PARSER_STOP,
    LXB_CSS_PARSER_END
}
lxb_css_parser_stage_t;

struct lxb_css_parser {
    lxb_css_parser_state_f           block;
    void                             *context;

    /* Modules */
    lxb_css_syntax_tokenizer_t       *tkz;
    lxb_css_selectors_t              *selectors;
    lxb_css_selectors_t              *old_selectors;

    /* Memory for all structures. */
    lxb_css_memory_t                 *memory;
    lxb_css_memory_t                 *old_memory;

    /* Syntax parse rules. */
    lxb_css_syntax_rule_t            *rules_begin;
    lxb_css_syntax_rule_t            *rules_end;
    lxb_css_syntax_rule_t            *rules;

    /* States */
    lxb_css_parser_state_t           *states_begin;
    lxb_css_parser_state_t           *states_end;
    lxb_css_parser_state_t           *states;

    /* Types */
    lxb_css_syntax_token_type_t      *types_begin;
    lxb_css_syntax_token_type_t      *types_end;
    lxb_css_syntax_token_type_t      *types_pos;

    lxb_css_syntax_tokenizer_chunk_f chunk_cb;
    void                             *chunk_ctx;

    const lxb_char_t                 *pos;
    uintptr_t                        offset;

    lexbor_str_t                     str;
    size_t                           str_size;

    lxb_css_log_t                    *log;

    lxb_css_parser_stage_t           stage;

    bool                             loop;
    bool                             fake_null;
    bool                             my_tkz;
    bool                             receive_endings;

    lxb_status_t                     status;
};

struct lxb_css_parser_state {
    lxb_css_parser_state_f state;
    void                   *context;
    bool                   root;
};

struct lxb_css_parser_error {
    lexbor_str_t message;
};


LXB_API lxb_css_parser_t *
lxb_css_parser_create(void);

LXB_API lxb_status_t
lxb_css_parser_init(lxb_css_parser_t *parser, lxb_css_syntax_tokenizer_t *tkz);

LXB_API void
lxb_css_parser_clean(lxb_css_parser_t *parser);

LXB_API void
lxb_css_parser_erase(lxb_css_parser_t *parser);

LXB_API lxb_css_parser_t *
lxb_css_parser_destroy(lxb_css_parser_t *parser, bool self_destroy);

LXB_API lxb_css_parser_state_t *
lxb_css_parser_states_push(lxb_css_parser_t *parser,
                           lxb_css_parser_state_f state,
                           void *context, bool stop);

LXB_API lxb_css_parser_state_t *
lxb_css_parser_states_next(lxb_css_parser_t *parser,
                           lxb_css_parser_state_f next,
                           lxb_css_parser_state_f back, void *ctx, bool root);

LXB_API lxb_status_t
lxb_css_parser_types_push(lxb_css_parser_t *parser,
                          lxb_css_syntax_token_type_t type);

LXB_API bool
lxb_css_parser_stop(lxb_css_parser_t *parser);

LXB_API bool
lxb_css_parser_fail(lxb_css_parser_t *parser, lxb_status_t status);

LXB_API bool
lxb_css_parser_unexpected(lxb_css_parser_t *parser);

LXB_API bool
lxb_css_parser_success(lxb_css_parser_t *parser);

LXB_API bool
lxb_css_parser_failed(lxb_css_parser_t *parser);

LXB_API lxb_status_t
lxb_css_parser_unexpected_status(lxb_css_parser_t *parser);

LXB_API bool
lxb_css_parser_unexpected_data(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token);

LXB_API lxb_status_t
lxb_css_parser_unexpected_data_status(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token);

LXB_API bool
lxb_css_parser_memory_fail(lxb_css_parser_t *parser);

lxb_status_t
lxb_css_parser_memory_fail_status(lxb_css_parser_t *parser);

/*
 * Inline functions
 */
lxb_inline lxb_status_t
lxb_css_parser_status(lxb_css_parser_t *parser)
{
    return parser->status;
}

lxb_inline lxb_css_memory_t *
lxb_css_parser_memory(lxb_css_parser_t *parser)
{
    return parser->memory;
}

lxb_inline void
lxb_css_parser_memory_set(lxb_css_parser_t *parser, lxb_css_memory_t *memory)
{
    parser->memory = memory;
}

lxb_inline lxb_css_selectors_t *
lxb_css_parser_selectors(lxb_css_parser_t *parser)
{
    return parser->selectors;
}

lxb_inline void
lxb_css_parser_selectors_set(lxb_css_parser_t *parser,
                             lxb_css_selectors_t *selectors)
{
    parser->selectors = selectors;
}

lxb_inline bool
lxb_css_parser_is_running(lxb_css_parser_t *parser)
{
    return parser->stage == LXB_CSS_PARSER_RUN;
}

lxb_inline bool
lxb_css_parser_status_is_unexpected_data(lxb_css_parser_t *parser)
{
    return parser->status == LXB_STATUS_ERROR_UNEXPECTED_DATA;
}

lxb_inline void
lxb_css_parser_failed_set(lxb_css_parser_t *parser, bool is)
{
    parser->rules->failed = is;
}

lxb_inline void
lxb_css_parser_failed_set_by_id(lxb_css_parser_t *parser, int idx, bool is)
{
    lxb_css_syntax_rule_t *rules = parser->rules + idx;

    if (rules > parser->rules_begin && rules < parser->rules_end) {
        rules->failed = is;
    }
}

lxb_inline bool
lxb_css_parser_is_failed(lxb_css_parser_t *parser)
{
    return parser->rules->failed;
}

lxb_inline void
lxb_css_parser_set_ok(lxb_css_parser_t *parser)
{
    parser->rules->failed = false;
    parser->status = LXB_STATUS_OK;
}

lxb_inline const lxb_char_t *
lxb_css_parser_buffer(lxb_css_parser_t *parser, size_t *length)
{
    if (length != NULL) {
        *length = parser->tkz->in_end - parser->tkz->in_begin;
    }

    return parser->tkz->in_begin;
}

lxb_inline void
lxb_css_parser_buffer_set(lxb_css_parser_t *parser,
                          const lxb_char_t *data, size_t length)
{
    lxb_css_syntax_tokenizer_buffer_set(parser->tkz, data, length);
}

lxb_inline lxb_css_parser_state_f
lxb_css_parser_state(lxb_css_parser_t *parser)
{
    return parser->rules->state;
}

lxb_inline void
lxb_css_parser_state_set(lxb_css_parser_t *parser, lxb_css_parser_state_f state)
{
    parser->rules->state = state;
}

lxb_inline void
lxb_css_parser_state_block_set(lxb_css_parser_t *parser,
                               lxb_css_parser_state_f state)
{
    parser->block = state;
}

lxb_inline void
lxb_css_parser_state_value_set(lxb_css_parser_t *parser,
                               lxb_css_parser_state_f state)
{
    lxb_css_parser_state_block_set(parser, state);
}

lxb_inline void *
lxb_css_parser_context(lxb_css_parser_t *parser)
{
    return parser->context;
}

lxb_inline void
lxb_css_parser_context_set(lxb_css_parser_t *parser, void *context)
{
    parser->context = context;
}

lxb_inline lxb_css_syntax_rule_t *
lxb_css_parser_current_rule(lxb_css_parser_t *parser)
{
    return parser->rules;
}

lxb_inline size_t
lxb_css_parser_rule_deep(lxb_css_parser_t *parser)
{
    return parser->rules->deep;
}

lxb_inline lxb_css_parser_state_t *
lxb_css_parser_states_pop(lxb_css_parser_t *parser)
{
    return parser->states--;
}

lxb_inline lxb_css_parser_state_t *
lxb_css_parser_states_to_root(lxb_css_parser_t *parser)
{
    lxb_css_parser_state_t *entry = parser->states;

    while (!entry->root) {
        entry--;
    }

    parser->states = entry;

    return entry;
}

lxb_inline bool
lxb_css_parser_states_set_back(lxb_css_parser_t *parser)
{
    const lxb_css_parser_state_t *entry = parser->states;
    lxb_css_syntax_rule_t *rules = parser->rules;

    rules->state = entry->state;
    rules->context = entry->context;

    return true;
}

lxb_inline void
lxb_css_parser_states_change_back(lxb_css_parser_t *parser,
                                  lxb_css_parser_state_f state)
{
    parser->rules->state_back = state;
}

lxb_inline void
lxb_css_parser_states_clean(lxb_css_parser_t *parser)
{
    parser->states = parser->states_begin;
}

lxb_inline lxb_css_parser_state_t *
lxb_css_parser_states_current(lxb_css_parser_t *parser)
{
    return parser->states;
}

lxb_inline void
lxb_css_parser_states_set(lxb_css_parser_state_t *states,
                          lxb_css_parser_state_f state, void *context)
{
    states->state = state;
    states->context = context;
}

lxb_inline void
lxb_css_parser_states_up(lxb_css_parser_t *parser)
{
    parser->states++;
}

lxb_inline void
lxb_css_parser_states_down(lxb_css_parser_t *parser)
{
    parser->states--;
}

lxb_inline lxb_css_log_t *
lxb_css_parser_log(lxb_css_parser_t *parser)
{
    return parser->log;
}

lxb_inline void
lxb_css_parser_offset_set(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token)
{
    if (parser->pos == NULL) {
        if (token == NULL) {
            parser->pos = parser->tkz->in_begin;
            parser->offset = 0;
        }
        else {
            parser->pos = lxb_css_syntax_token_base(token)->begin
                          + lxb_css_syntax_token_base(token)->length;
            parser->offset = token->offset + lxb_css_syntax_token_base(token)->length;
        }
    }
}

lxb_inline const lxb_css_syntax_list_rules_offset_t *
lxb_css_parser_list_rules_offset(lxb_css_parser_t *parser)
{
    return &parser->rules->u.list_rules;
}

lxb_inline const lxb_css_syntax_at_rule_offset_t *
lxb_css_parser_at_rule_offset(lxb_css_parser_t *parser)
{
    return &parser->rules->u.at_rule;
}

lxb_inline const lxb_css_syntax_qualified_offset_t *
lxb_css_parser_qualified_rule_offset(lxb_css_parser_t *parser)
{
    return &parser->rules->u.qualified;
}

lxb_inline const lxb_css_syntax_declarations_offset_t *
lxb_css_parser_declarations_offset(lxb_css_parser_t *parser)
{
    return &parser->rules->u.declarations;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_PARSER_H */
