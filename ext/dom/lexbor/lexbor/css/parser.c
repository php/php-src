/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/parser.h"
#include "lexbor/css/state.h"
#include "lexbor/css/syntax/syntax.h"


lxb_css_parser_t *
lxb_css_parser_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_css_parser_t));
}

lxb_status_t
lxb_css_parser_init(lxb_css_parser_t *parser, lxb_css_syntax_tokenizer_t *tkz)
{
    lxb_status_t status;
    static const size_t lxb_rules_length = 128;
    static const size_t lxb_states_length = 1024;

    if (parser == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    /* Stack */
    parser->states_begin = lexbor_malloc(sizeof(lxb_css_parser_state_t)
                                         * lxb_states_length);
    if (parser->states_begin == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    parser->states = parser->states_begin;
    parser->states_end = parser->states_begin + lxb_states_length;

    memset(parser->states, 0x00, sizeof(lxb_css_parser_state_t));
    parser->states->root = true;

    /* Syntax */
    parser->my_tkz = false;

    if (tkz == NULL) {
        tkz = lxb_css_syntax_tokenizer_create();
        status = lxb_css_syntax_tokenizer_init(tkz);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        parser->my_tkz = true;
    }

    /* Rules */
    parser->rules_begin = lexbor_malloc(sizeof(lxb_css_syntax_rule_t)
                                        * lxb_rules_length);
    if (parser->rules_begin == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    parser->rules_end = parser->rules_begin + lxb_rules_length;
    parser->rules = parser->rules_begin;

    /* Temp */
    parser->pos = NULL;
    parser->str.length = 0;
    parser->str_size = 4096;

    parser->str.data = lexbor_malloc(sizeof(lxb_char_t) * parser->str_size);
    if (parser->str.data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    parser->log = lxb_css_log_create();
    status = lxb_css_log_init(parser->log, NULL);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    parser->tkz = tkz;
    parser->types_begin = NULL;
    parser->types_pos = NULL;
    parser->types_end = NULL;
    parser->stage = LXB_CSS_PARSER_CLEAN;
    parser->receive_endings = false;
    parser->status = LXB_STATUS_OK;
    parser->fake_null = false;

    return LXB_STATUS_OK;
}

void
lxb_css_parser_clean(lxb_css_parser_t *parser)
{
    lxb_css_syntax_tokenizer_clean(parser->tkz);
    lxb_css_log_clean(parser->log);

    parser->rules = parser->rules_begin;
    parser->states = parser->states_begin;
    parser->types_pos = parser->types_begin;
    parser->stage = LXB_CSS_PARSER_CLEAN;
    parser->status = LXB_STATUS_OK;
    parser->pos = NULL;
    parser->str.length = 0;
    parser->fake_null = false;
}

void
lxb_css_parser_erase(lxb_css_parser_t *parser)
{
    lxb_css_parser_clean(parser);

    if (parser->memory != NULL) {
        lxb_css_memory_clean(parser->memory);
    }
}

lxb_css_parser_t *
lxb_css_parser_destroy(lxb_css_parser_t *parser, bool self_destroy)
{
    if (parser == NULL) {
        return NULL;
    }

    if (parser->my_tkz) {
        parser->tkz = lxb_css_syntax_tokenizer_destroy(parser->tkz);
    }

    parser->log = lxb_css_log_destroy(parser->log, true);

    if (parser->rules_begin != NULL) {
        parser->rules_begin = lexbor_free(parser->rules_begin);
    }

    if (parser->states_begin != NULL) {
        parser->states_begin = lexbor_free(parser->states_begin);
    }

    if (parser->types_begin != NULL) {
        parser->types_begin = lexbor_free(parser->types_begin);
    }

    if (parser->str.data != NULL) {
        parser->str.data = lexbor_free(parser->str.data);
    }

    if (self_destroy) {
        return lexbor_free(parser);
    }

    return parser;
}

lxb_css_parser_state_t *
lxb_css_parser_states_push(lxb_css_parser_t *parser,
                           lxb_css_parser_state_f state, void *ctx, bool root)
{
    size_t length, cur_length;
    lxb_css_parser_state_t *states = ++parser->states;

    if (states >= parser->states_end) {
        cur_length = states - parser->states_begin;

        if (SIZE_MAX - cur_length < 1024) {
            goto memory_error;
        }

        length = cur_length + 1024;

        states = lexbor_realloc(parser->states_begin,
                                length * sizeof(lxb_css_parser_state_t));
        if (states == NULL) {
            goto memory_error;
        }

        parser->states_begin = states;
        parser->states_end = states + length;
        parser->states = states + cur_length;

        states = parser->states;
    }

    states->state = state;
    states->context = ctx;
    states->root = root;

    return states;

memory_error:

    parser->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

    return NULL;
}

lxb_css_parser_state_t *
lxb_css_parser_states_next(lxb_css_parser_t *parser,
                           lxb_css_parser_state_f next,
                           lxb_css_parser_state_f back, void *ctx, bool root)
{
    lxb_css_parser_state_t *state;

    state = lxb_css_parser_states_push(parser, back, ctx, root);
    if (state == NULL) {
        return NULL;
    }

    parser->rules->state = next;

    return state;
}

lxb_status_t
lxb_css_parser_types_push(lxb_css_parser_t *parser,
                          lxb_css_syntax_token_type_t type)
{
    size_t length, new_length;
    lxb_css_syntax_token_type_t *tmp;

    if (parser->types_pos >= parser->types_end) {
        length = parser->types_end - parser->types_begin;

        if ((SIZE_MAX - length) < 1024) {
            return LXB_STATUS_ERROR_OVERFLOW;
        }

        new_length = length + 1024;

        tmp = lexbor_realloc(parser->types_begin,
                             new_length * sizeof(lxb_css_syntax_token_type_t));
        if (tmp == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        parser->types_begin = tmp;
        parser->types_end = tmp + new_length;
        parser->types_pos = parser->types_begin + length;
    }

    *parser->types_pos++ = type;

    return LXB_STATUS_OK;
}

bool
lxb_css_parser_stop(lxb_css_parser_t *parser)
{
    parser->loop = false;
    return true;
}

bool
lxb_css_parser_fail(lxb_css_parser_t *parser, lxb_status_t status)
{
    parser->status = status;
    parser->loop = false;
    return true;
}

bool
lxb_css_parser_unexpected(lxb_css_parser_t *parser)
{
    (void) lxb_css_parser_unexpected_status(parser);
    return true;
}

bool
lxb_css_parser_success(lxb_css_parser_t *parser)
{
    parser->rules->state = lxb_css_state_success;
    return true;
}

bool
lxb_css_parser_failed(lxb_css_parser_t *parser)
{
    lxb_css_syntax_rule_t *rule = parser->rules;

    rule->state = rule->cbx.cb->failed;
    rule->failed = true;

    return true;
}

lxb_status_t
lxb_css_parser_unexpected_status(lxb_css_parser_t *parser)
{
    parser->status = LXB_STATUS_ERROR_UNEXPECTED_DATA;

    parser->rules->failed = true;

    return LXB_STATUS_ERROR_UNEXPECTED_DATA;
}

bool
lxb_css_parser_unexpected_data(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token)
{
    static const char selectors[] = "Selectors";
    parser->status = LXB_STATUS_ERROR_UNEXPECTED_DATA;

    if (lxb_css_syntax_token_error(parser, token, selectors) == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return true;
}

lxb_status_t
lxb_css_parser_unexpected_data_status(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token)
{
    static const char selectors[] = "Selectors";
    parser->status = LXB_STATUS_ERROR_UNEXPECTED_DATA;

    if (lxb_css_syntax_token_error(parser, token, selectors) == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_ERROR_UNEXPECTED_DATA;
}

bool
lxb_css_parser_memory_fail(lxb_css_parser_t *parser)
{
    parser->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    parser->loop = false;
    return true;
}

lxb_status_t
lxb_css_parser_memory_fail_status(lxb_css_parser_t *parser)
{
    parser->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    parser->loop = false;

    return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
}
