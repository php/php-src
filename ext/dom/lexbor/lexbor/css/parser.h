/*
 * Copyright (C) 2021-2026 Alexander Borisov
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
    /* Modules */
    lxb_css_syntax_tokenizer_t          *tkz;
    lxb_css_selectors_t                 *selectors;

    /* Memory for all structures. */
    lxb_css_memory_t                    *memory;

    /* Syntax parse rules. */
    lxb_css_syntax_rule_t               *rules_begin;
    lxb_css_syntax_rule_t               *rules_end;
    lxb_css_syntax_rule_t               *rules;

    /* States */
    lxb_css_parser_state_t              *states_begin;
    lxb_css_parser_state_t              *states_end;
    lxb_css_parser_state_t              *states;

    /* Types */
    lxb_css_syntax_token_type_t         *types_begin;
    lxb_css_syntax_token_type_t         *types_end;
    lxb_css_syntax_token_type_t         *types_pos;

    lxb_css_syntax_token_t              token_end;

    lexbor_str_t                        str;
    size_t                              str_size;

    lxb_css_log_t                       *log;

    lxb_css_parser_stage_t              stage;

    lxb_css_syntax_declaration_offset_t offset;

    bool                                loop;
    bool                                fake_null;
    bool                                my_tkz;

    lxb_status_t                        status;
};

struct lxb_css_parser_state {
    lxb_css_parser_state_f state;
    void                   *context;
    bool                   root;
};

struct lxb_css_parser_error {
    lexbor_str_t message;
};


/*
 * Create a new CSS parser object.
 *
 * @return lxb_css_parser_t* if successful, otherwise NULL.
 */
LXB_API lxb_css_parser_t *
lxb_css_parser_create(void);

/*
 * Initialize the CSS parser with a tokenizer.
 *
 * @param[in] parser Required. The CSS parser object.
 *                   If NULL, the function returns
 *                   LXB_STATUS_ERROR_OBJECT_IS_NULL.
 * @param[in] tkz    Optional. The tokenizer object.
 *                   If NULL, a new tokenizer will be created and used.
 *                   (In that case the parser manages the tokenizer lifetime.)
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_css_parser_init(lxb_css_parser_t *parser, lxb_css_syntax_tokenizer_t *tkz);

/*
 * Initialize the Selectors module in the parser.
 *
 * Note:
 *   Be sure to destroy the Selectors object you created yourself using
 *   the lxb_css_parser_selectors_destroy() function.
 *
 * @param[in] parser Required. The CSS parser object.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_css_parser_selectors_init(lxb_css_parser_t *parser);

/*
 * Destroy the selectors module in the parser.
 *
 * @param[in] parser Required. The CSS parser object.
 */
LXB_API void
lxb_css_parser_selectors_destroy(lxb_css_parser_t *parser);

/*
 * Clean the parser object.
 *
 * This resets internal parser state, but keeps allocated memory so the parser
 * can be reused.
 *
 * @param[in] parser Required. The CSS parser object.
 */
LXB_API void
lxb_css_parser_clean(lxb_css_parser_t *parser);

/*
 * Erase the parser object.
 *
 * This resets internal parser state and releases internal allocations owned by
 * the parser so it can be re-initialized.
 *
 * @param[in] parser Required. The CSS parser object.
 */
LXB_API void
lxb_css_parser_erase(lxb_css_parser_t *parser);

/*
 * Destroy the parser object.
 *
 * @param[in] parser        Optional. The CSS parser object.
 *                          If NULL, the function returns NULL.
 * @param[in] self_destroy  Optional flag.
 *                          If true, also frees the parser object itself.
 *                          If false, only inner resources are freed and the
 *                          parser object remains valid.
 *
 * @return parser if self_destroy is false, otherwise NULL.
 */
LXB_API lxb_css_parser_t *
lxb_css_parser_destroy(lxb_css_parser_t *parser, bool self_destroy);

/*
 * Stop the parser main loop.
 *
 * This is a helper for state callbacks that want to immediately stop parsing
 * without marking the current rule as failed.
 *
 * @param[in] parser Required. The CSS parser object.
 *
 * @return true (convenience return value, useful for state callbacks).
 */
LXB_API bool
lxb_css_parser_stop(lxb_css_parser_t *parser);

/*
 * Fail the parser with a status and stop the main loop.
 *
 * This is used for hard errors (OOM, overflow, etc.). It sets parser->status
 * and stops the parser loop.
 *
 * @param[in] parser  Required. The CSS parser object.
 * @param[in] status  Required. Status code describing the failure.
 *
 * @return true (convenience return value, useful for state callbacks).
 */
LXB_API bool
lxb_css_parser_fail(lxb_css_parser_t *parser, lxb_status_t status);

/*
 * Mark the current token as unexpected.
 *
 * This is a specialized helper for the common "unexpected data" parse error.
 * It sets parser->status to LXB_STATUS_ERROR_UNEXPECTED_DATA and marks the
 * current rule as failed.
 *
 * @param[in] parser Required. The CSS parser object.
 *
 * @return true (convenience return value, useful for state callbacks).
 */
LXB_API bool
lxb_css_parser_unexpected(lxb_css_parser_t *parser);

/*
 * Mark parsing of the current grammar/rule as successful.
 *
 * Why this function exists:
 * - Most callbacks in the CSS parser work as a state machine: each callback
 *   receives the current token and returns bool to decide what to do next.
 * - "Success" is not just a bool. The parser must switch to a special
 *   lxb_css_state_success state to properly handle trailing whitespace and
 *   the end-of-input token.
 * - If you don't consume all tokens up to LXB_CSS_SYNTAX_TOKEN__END in the
 *   current rule and call this function, the parser will switch to "failed"
 *   mode. This is the same as calling the lxb_css_parser_failed() function.
 *   The exception is the LXB_CSS_SYNTAX_TOKEN_WHITESPACE token. If you are
 *   sure that only this token remains, the parser will simply exclude it.
 *
 * What it does:
 * - Sets the current rule state to lxb_css_state_success.
 * - Does not stop the parser loop by itself.
 *
 * Typical usage:
 * - In css/property/state.c, after you have parsed and consumed the last token
 *   of a property value, you do:
 *   return lxb_css_parser_success(parser);
 * - That transfers control to lxb_css_state_success, which consumes any
 *   whitespace and completes on LXB_CSS_SYNTAX_TOKEN__END.
 *
 * @param[in] parser Required. The CSS parser object.
 *
 * @return true (convenience return value, useful for state callbacks).
 */
LXB_API bool
lxb_css_parser_success(lxb_css_parser_t *parser);

/*
 * Mark parsing of the current grammar/rule as failed.
 *
 * Why this function exists:
 * - In the state machine, failure is also not "just return false". The parser
 *   needs to switch to a rule-specific recovery state so it can safely skip
 *   input until it is allowed to stop (usually until end-of-input or a known
 *   synchronisation point).
 *
 * What it does:
 * - Sets the current rule state to the rule-specific "failed" handler
 *   (rule->cbx.cb->failed).
 * - Sets rule->failed = true.
 * - Does not directly change parser->status. Use lxb_css_parser_fail() or
 *   lxb_css_parser_unexpected() when you need a concrete status code.
 *
 * Typical usage:
 * - In css/property/state.c, if a token doesn't match the expected grammar,
 *   you do:
 *   return lxb_css_parser_failed(parser);
 * - That switches the parser into the failed handler. For properties this lets
 *   the parser consume/skip tokens until it reaches LXB_CSS_SYNTAX_TOKEN__END.
 *   (See lxb_css_state_failed(): it consumes tokens until end, then calls
 *   lxb_css_parser_success() to finish.)
 *
 * Important!
 *   The user sets the callback to "failed" and must decide what to do in this
 *   situation. If the user does not consume all tokens before
 *   LXB_CSS_SYNTAX_TOKEN__END, there will be an infinite loop.
 *
 * @param[in] parser Required. The CSS parser object.
 *
 * @return true (convenience return value, useful for state callbacks).
 */

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

LXB_API void *
lxb_css_parser_memory_fail_null(lxb_css_parser_t *parser);

LXB_API bool
lxb_css_parser_memory_fail(lxb_css_parser_t *parser);

LXB_API lxb_status_t
lxb_css_parser_memory_fail_status(lxb_css_parser_t *parser);

/*
 * Push a new state to the parser.
 *
 * @param[in] parser   Required. The CSS parser object.
 * @param[in] state    Required. State callback to execute.
 * @param[in] context  Optional user pointer associated with this state.
 *                     Stored as-is; the parser doesn't own it.
 * @param[in] stop     If true, the pushed state is marked as a "stop point".
 *                     This is used by the parser to stop/return control when
 *                     this state is reached.
 *
 * @return lxb_css_parser_state_t* if successful, otherwise NULL.
 */
LXB_API lxb_css_parser_state_t *
lxb_css_parser_states_push(lxb_css_parser_t *parser,
                           lxb_css_parser_state_f state,
                           void *context, bool stop);

/*
 * Go to the next state in the parser.
 *
 * @param[in] parser  Required. The CSS parser object.
 * @param[in] next    Required. Next state callback.
 * @param[in] back    Required. State callback to return to.
 * @param[in] ctx     Optional user pointer for the next state.
 *                    Stored as-is; the parser doesn't own it.
 * @param[in] root    If true, marks this state as a "root" state. Root states
 *                    are used by helper functions (e.g. to rewind to root).
 *
 * @return lxb_css_parser_state_t* if successful, otherwise NULL.
 */
LXB_API lxb_css_parser_state_t *
lxb_css_parser_states_next(lxb_css_parser_t *parser,
                           lxb_css_parser_state_f next,
                           lxb_css_parser_state_f back, void *ctx, bool root);

/*
 * Push a token type to the parser.
 *
 * @param[in] parser Required. The CSS parser object.
 * @param[in] type   Required. Token type to push.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_css_parser_types_push(lxb_css_parser_t *parser,
                          lxb_css_syntax_token_type_t type);


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

lxb_inline const lxb_css_syntax_token_t *
lxb_css_parser_token_end(lxb_css_parser_t *parser, size_t offset)
{
    parser->token_end.offset = offset;
    return &parser->token_end;
}

lxb_inline void
lxb_css_parser_set_context(lxb_css_parser_t *parser, void *ctx)
{
    parser->rules->context = ctx;
}

lxb_inline void
lxb_css_syntax_set_return(lxb_css_parser_t *parser, void *value)
{
    parser->rules[-1].returned = value;
}

lxb_inline void *
lxb_css_syntax_returned(lxb_css_parser_t *parser)
{
    return parser->rules->returned;
}

/*
 * No-inline functions for ABI.
 */
LXB_API lxb_status_t
lxb_css_parser_status_noi(lxb_css_parser_t *parser);

LXB_API lxb_css_memory_t *
lxb_css_parser_memory_noi(lxb_css_parser_t *parser);

LXB_API void
lxb_css_parser_memory_set_noi(lxb_css_parser_t *parser, lxb_css_memory_t *memory);

LXB_API lxb_css_selectors_t *
lxb_css_parser_selectors_noi(lxb_css_parser_t *parser);

LXB_API void
lxb_css_parser_selectors_set_noi(lxb_css_parser_t *parser,
                                 lxb_css_selectors_t *selectors);

LXB_API bool
lxb_css_parser_is_running_noi(lxb_css_parser_t *parser);

LXB_API bool
lxb_css_parser_status_is_unexpected_data_noi(lxb_css_parser_t *parser);

LXB_API void
lxb_css_parser_failed_set_noi(lxb_css_parser_t *parser, bool is);

LXB_API void
lxb_css_parser_failed_set_by_id_noi(lxb_css_parser_t *parser, int idx, bool is);

LXB_API bool
lxb_css_parser_is_failed_noi(lxb_css_parser_t *parser);

LXB_API void
lxb_css_parser_set_ok_noi(lxb_css_parser_t *parser);

LXB_API const lxb_char_t *
lxb_css_parser_buffer_noi(lxb_css_parser_t *parser, size_t *length);

LXB_API void
lxb_css_parser_buffer_set_noi(lxb_css_parser_t *parser,
                              const lxb_char_t *data, size_t length);

LXB_API lxb_css_parser_state_f
lxb_css_parser_state_noi(lxb_css_parser_t *parser);

LXB_API void
lxb_css_parser_state_set_noi(lxb_css_parser_t *parser,
                             lxb_css_parser_state_f state);

LXB_API lxb_css_syntax_rule_t *
lxb_css_parser_current_rule_noi(lxb_css_parser_t *parser);

LXB_API size_t
lxb_css_parser_rule_deep_noi(lxb_css_parser_t *parser);

LXB_API lxb_css_parser_state_t *
lxb_css_parser_states_pop_noi(lxb_css_parser_t *parser);

LXB_API lxb_css_parser_state_t *
lxb_css_parser_states_to_root_noi(lxb_css_parser_t *parser);

LXB_API bool
lxb_css_parser_states_set_back_noi(lxb_css_parser_t *parser);

LXB_API void
lxb_css_parser_states_clean_noi(lxb_css_parser_t *parser);

LXB_API lxb_css_parser_state_t *
lxb_css_parser_states_current_noi(lxb_css_parser_t *parser);

LXB_API void
lxb_css_parser_states_set_noi(lxb_css_parser_state_t *states,
                              lxb_css_parser_state_f state, void *context);

LXB_API void
lxb_css_parser_states_up_noi(lxb_css_parser_t *parser);

LXB_API void
lxb_css_parser_states_down_noi(lxb_css_parser_t *parser);

LXB_API lxb_css_log_t *
lxb_css_parser_log_noi(lxb_css_parser_t *parser);

LXB_API const lxb_css_syntax_token_t *
lxb_css_parser_token_end_noi(lxb_css_parser_t *parser, size_t offset);

LXB_API void
lxb_css_parser_set_context_noi(lxb_css_parser_t *parser, void *ctx);

LXB_API void
lxb_css_syntax_set_return_noi(lxb_css_parser_t *parser, void *value);

LXB_API void *
lxb_css_syntax_returned_noi(lxb_css_parser_t *parser);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_PARSER_H */
