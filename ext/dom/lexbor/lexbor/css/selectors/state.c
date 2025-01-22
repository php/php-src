/*
 * Copyright (C) 2020-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/parser.h"
#include "lexbor/css/css.h"
#include "lexbor/css/selectors/selectors.h"
#include "lexbor/css/selectors/pseudo.h"
#include "lexbor/css/selectors/pseudo_const.h"


static const char lxb_css_selectors_module_name[] = "Selectors";


#define lxb_css_selectors_state_string_dup_m(selectors, name)                  \
    do {                                                                       \
        (status) = lxb_css_syntax_token_string_dup(                            \
                    lxb_css_syntax_token_string(token), (name),                \
                    (parser)->memory->mraw);                                   \
        if ((status) != LXB_STATUS_OK) {                                       \
            return (status);                                                   \
        }                                                                      \
    }                                                                          \
    while (false)

#define lxb_css_selectors_state_append(parser, selectors, selector)            \
    do {                                                                       \
        (selector) = lxb_css_selector_create((selectors)->list_last);          \
        if ((selector) == NULL) {                                              \
            return lxb_css_parser_memory_fail(parser);                         \
        }                                                                      \
                                                                               \
        lxb_css_selectors_append_next((selectors), (selector));                \
                                                                               \
        (selector)->combinator = (selectors)->combinator;                      \
        (selectors)->combinator = LXB_CSS_SELECTOR_COMBINATOR_CLOSE;           \
    }                                                                          \
    while (false)

#define lxb_css_selectors_state_list_append(parser, selectors, list)           \
    do {                                                                       \
        (list) = lxb_css_selector_list_create((parser)->memory);               \
        if ((list) == NULL) {                                                  \
            return lxb_css_parser_memory_fail(parser);                         \
        }                                                                      \
                                                                               \
        lxb_css_selectors_list_append_next((selectors), (list));               \
                                                                               \
        (list)->parent = selectors->parent;                                    \
    }                                                                          \
    while (false)


static bool
lxb_css_selectors_state_complex_list_end(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx);

static bool
lxb_css_selectors_state_relative_list_end(lxb_css_parser_t *parser,
                                          const lxb_css_syntax_token_t *token,
                                          void *ctx);

static bool
lxb_css_selectors_state_relative_list_wo_root(lxb_css_parser_t *parser,
                                              const lxb_css_syntax_token_t *token,
                                              void *ctx);

static bool
lxb_css_selectors_state_relative_wo_root(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx);

static bool
lxb_css_selectors_state_relative_handler(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, bool list, bool root);

static bool
lxb_css_selectors_state_complex_end(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    void *ctx);

static bool
lxb_css_selectors_state_complex_wo_root(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        void *ctx);

static bool
lxb_css_selectors_state_complex_handler(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        void *ctx, bool root);

static bool
lxb_css_selectors_state_compound_list_end(lxb_css_parser_t *parser,
                                          const lxb_css_syntax_token_t *token,
                                          void *ctx);

static bool
lxb_css_selectors_state_compound_wo_root(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx);

static bool
lxb_css_selectors_state_compound_handler(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx);

static bool
lxb_css_selectors_state_compound_sub(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx);

static bool
lxb_css_selectors_state_compound_pseudo(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        void *ctx);

static bool
lxb_css_selectors_state_simple_list_end(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        void *ctx);

static bool
lxb_css_selectors_state_simple_wo_root(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       void *ctx);

static bool
lxb_css_selectors_state_simple_handler(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       void *ctx);

static bool
lxb_css_selectors_state_simple_back(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    void *ctx);

static lxb_status_t
lxb_css_selectors_state_hash(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token);

static lxb_status_t
lxb_css_selectors_state_class(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token);

static lxb_status_t
lxb_css_selectors_state_element_ns(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token);

static lxb_status_t
lxb_css_selectors_state_element(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token);

static lxb_status_t
lxb_css_selectors_state_attribute(lxb_css_parser_t *parser);

static lxb_status_t
lxb_css_selectors_state_ns(lxb_css_parser_t *parser,
                           lxb_css_selector_t *selector);

static lxb_status_t
lxb_css_selectors_state_ns_ident(lxb_css_parser_t *parser,
                                 lxb_css_selector_t *selector);

static lxb_status_t
lxb_css_selectors_state_pseudo_class(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token);

static lxb_status_t
lxb_css_selectors_state_pseudo_class_function(lxb_css_parser_t *parser,
                                              const lxb_css_syntax_token_t *token,
                                              lxb_css_parser_state_f success);

static lxb_status_t
lxb_css_selectors_state_pseudo_element(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token);

static lxb_status_t
lxb_css_selectors_state_pseudo_element_function(lxb_css_parser_t *parser,
                                                const lxb_css_syntax_token_t *token,
                                                lxb_css_parser_state_f success);

static lxb_status_t
lxb_css_selectors_state_forgiving_cb(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx, lxb_css_parser_state_f state,
                                     bool failed);

static void
lxb_css_selectors_state_restore_parent(lxb_css_selectors_t *selectors,
                                       lxb_css_selector_list_t *last);

static bool
lxb_css_selectors_state_list_end(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 lxb_css_parser_state_f state);

static bool
lxb_css_selectors_state_end(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token, void *ctx);

static const lxb_css_syntax_token_t *
lxb_css_selectors_state_function_error(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token);


lxb_inline bool
lxb_css_selectors_done(lxb_css_parser_t *parser)
{
    lxb_css_parser_states_pop(parser);

    return lxb_css_parser_states_set_back(parser);
}

lxb_inline void
lxb_css_selectors_state_specificity_set_b(lxb_css_selectors_t *selectors)
{
    lxb_css_selector_list_t *last = selectors->list_last;

    if (selectors->parent == NULL) {
        lxb_css_selector_sp_set_b(last->specificity,
                                  lxb_css_selector_sp_b(last->specificity) + 1);
    }
    else if (last->specificity > LXB_CSS_SELECTOR_SP_B_MAX) {
        if (selectors->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
            lxb_css_selector_sp_set_b(last->specificity, 1);
        }
    }
    else {
        if (selectors->combinator != LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
            last->specificity = 0;
        }

        lxb_css_selector_sp_set_b(last->specificity, 1);
    }
}

lxb_inline void
lxb_css_selectors_state_specificity_set_c(lxb_css_selectors_t *selectors)
{
    lxb_css_selector_list_t *last = selectors->list_last;

    if (selectors->parent == NULL) {
        lxb_css_selector_sp_set_c(last->specificity,
                                  lxb_css_selector_sp_c(last->specificity) + 1);
    }
    else if (last->specificity > LXB_CSS_SELECTOR_SP_C_MAX) {
        if (selectors->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
            lxb_css_selector_sp_set_c(last->specificity, 1);
        }
    }
    else {
        if (selectors->combinator != LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
            last->specificity = 0;
        }

        lxb_css_selector_sp_set_c(last->specificity, 1);
    }
}

lxb_inline void
lxb_css_selectors_state_func_specificity(lxb_css_selectors_t *selectors)
{
    lxb_css_selector_list_t *prev, *last;

    last = selectors->list_last;
    prev = last->prev;

    if (prev->specificity > last->specificity) {
        last->specificity = prev->specificity;
    }

    prev->specificity = 0;
}

/*
 * <complex-selector-list>
 */
bool
lxb_css_selectors_state_complex_list(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx)
{
    lxb_css_parser_state_t *states;

    states = lxb_css_parser_states_next(parser,
                                        lxb_css_selectors_state_complex_wo_root,
                                        lxb_css_selectors_state_complex_list_end,
                                        ctx, true);
    if (states == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return false;
}

static bool
lxb_css_selectors_state_complex_list_end(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx)
{
    return lxb_css_selectors_state_list_end(parser, token,
                                            lxb_css_selectors_state_complex_wo_root);
}

/*
 * <relative-selector-list>
 */
bool
lxb_css_selectors_state_relative_list(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx)
{
    lxb_css_parser_state_t *states;

    states = lxb_css_parser_states_next(parser,
                                        lxb_css_selectors_state_relative_list_wo_root,
                                        lxb_css_selectors_state_relative_list_end,
                                        ctx, true);
    if (states == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return false;
}

static bool
lxb_css_selectors_state_relative_list_end(lxb_css_parser_t *parser,
                                          const lxb_css_syntax_token_t *token,
                                          void *ctx)
{
    return lxb_css_selectors_state_list_end(parser, token,
                                            lxb_css_selectors_state_relative_list_wo_root);
}

/*
 * <relative-selector>
 */
bool
lxb_css_selectors_state_relative(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx)
{
    lxb_css_parser_state_t *states;

    states = lxb_css_parser_states_next(parser,
                                        lxb_css_selectors_state_relative_wo_root,
                                        lxb_css_selectors_state_end,
                                        ctx, true);
    if (states == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return false;
}

static bool
lxb_css_selectors_state_relative_list_wo_root(lxb_css_parser_t *parser,
                                              const lxb_css_syntax_token_t *token,
                                              void *ctx)
{
    return lxb_css_selectors_state_relative_handler(parser, token, ctx, true,
                                                    false);
}

static bool
lxb_css_selectors_state_relative_wo_root(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx)
{
    return lxb_css_selectors_state_relative_handler(parser, token, ctx, false,
                                                    false);
}

static bool
lxb_css_selectors_state_relative_handler(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, bool list, bool root)
{
    lxb_css_parser_state_f back;
    lxb_css_parser_state_t *states;
    lxb_css_selectors_t *selectors = parser->selectors;

    /* <combinator> */

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            lxb_css_syntax_parser_consume(parser);
            selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT;
            return true;

        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            switch (lxb_css_syntax_token_delim_char(token)) {
                case '>':
                    selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_CHILD;
                    break;

                case '+':
                    selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_SIBLING;
                    break;

                case '~':
                    selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING;
                    break;

                case '|':
                    lxb_css_parser_token_next_m(parser, token);

                    if (token->type == LXB_CSS_SYNTAX_TOKEN_DELIM
                        && lxb_css_syntax_token_delim_char(token) == '|')
                    {
                        lxb_css_syntax_parser_consume(parser);
                        selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_CELL;
                        break;
                    }

                    goto done;

                default:
                    goto done;
            }

            break;

        default:
            goto done;
    }

    lxb_css_syntax_parser_consume(parser);

done:

    back = (list) ? lxb_css_selectors_state_complex_end
                  : lxb_css_selectors_state_end;

    states = lxb_css_parser_states_next(parser,
                                        lxb_css_selectors_state_compound_wo_root,
                                        back, ctx, root);
    if (states == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return true;
}

/*
 * <complex-selector>
 */
bool
lxb_css_selectors_state_complex(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_parser_state_t *states;

    states = lxb_css_parser_states_next(parser,
                                        lxb_css_selectors_state_complex_wo_root,
                                        lxb_css_selectors_state_end,
                                        ctx, true);
    if (states == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return false;
}

static bool
lxb_css_selectors_state_complex_wo_root(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        void *ctx)
{
    return lxb_css_selectors_state_complex_handler(parser, token, ctx, false);
}

static bool
lxb_css_selectors_state_complex_handler(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        void *ctx, bool root)
{
    lxb_css_parser_state_t *states;

    states = lxb_css_parser_states_next(parser,
                                        lxb_css_selectors_state_compound_wo_root,
                                        lxb_css_selectors_state_complex_end,
                                        ctx, root);
    if (states == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return false;
}

static bool
lxb_css_selectors_state_complex_end(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    void *ctx)
{
    lxb_css_selectors_t *selectors = parser->selectors;

    /* <combinator> */

again:

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            lxb_css_syntax_parser_consume(parser);

            selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT;

            lxb_css_parser_token_m(parser, token);
            goto again;

        case LXB_CSS_SYNTAX_TOKEN__END:
            return lxb_css_selectors_done(parser);

        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            switch (lxb_css_syntax_token_delim_char(token)) {
                case '>':
                    selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_CHILD;
                    break;

                case '+':
                    selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_SIBLING;
                    break;

                case '~':
                    selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING;
                    break;

                case '|':
                    lxb_css_parser_token_next_m(parser, token);

                    if (token->type == LXB_CSS_SYNTAX_TOKEN_DELIM
                        && lxb_css_syntax_token_delim_char(token) == '|')
                    {
                        lxb_css_syntax_parser_consume(parser);
                        selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_CELL;
                        break;
                    }

                    goto done;

                default:
                    if (selectors->combinator != LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT) {
                        goto unexpected;
                    }

                    goto done;
            }

            break;

        case LXB_CSS_SYNTAX_TOKEN_COMMA:
            return lxb_css_selectors_done(parser);

        default:
            if (selectors->combinator != LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT) {
                goto unexpected;
            }

            goto done;
    }

    lxb_css_syntax_parser_consume(parser);

done:

    lxb_css_parser_state_set(parser, lxb_css_selectors_state_compound_handler);

    return true;

unexpected:

    (void) lxb_css_selectors_done(parser);

    return lxb_css_parser_unexpected(parser);
}

/*
 * <compound-selector-list>
 */
bool
lxb_css_selectors_state_compound_list(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx)
{
    lxb_css_parser_state_t *states;

    states = lxb_css_parser_states_next(parser,
                                        lxb_css_selectors_state_compound_wo_root,
                                        lxb_css_selectors_state_compound_list_end,
                                        ctx, true);
    if (states == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return false;
}

static bool
lxb_css_selectors_state_compound_list_end(lxb_css_parser_t *parser,
                                          const lxb_css_syntax_token_t *token,
                                          void *ctx)
{
    return lxb_css_selectors_state_list_end(parser, token,
                                            lxb_css_selectors_state_compound_wo_root);
}

/*
 *
 * <compound-selector>
 */
bool
lxb_css_selectors_state_compound(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx)
{
    lxb_css_parser_state_t *states;

    states = lxb_css_parser_states_next(parser,
                                        lxb_css_selectors_state_compound_wo_root,
                                        lxb_css_selectors_state_end,
                                        ctx, true);
    if (states == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return false;
}

static bool
lxb_css_selectors_state_compound_wo_root(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx)
{
    lxb_css_selector_list_t *list;

    lxb_css_selectors_state_list_append(parser, parser->selectors, list);

    lxb_css_parser_state_set(parser, lxb_css_selectors_state_compound_handler);

    return false;
}

static bool
lxb_css_selectors_state_compound_handler(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx)
{
    lxb_status_t status;
    lxb_css_selectors_t *selectors;

again:

    lxb_css_parser_state_set(parser, lxb_css_selectors_state_compound_sub);

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_HASH:
            status = lxb_css_selectors_state_hash(parser, token);
            break;

        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            switch (lxb_css_syntax_token_delim_char(token)) {
                case '.':
                    lxb_css_syntax_parser_consume(parser);
                    status = lxb_css_selectors_state_class(parser, token);
                    break;

                case '|':
                case '*':
                    status = lxb_css_selectors_state_element_ns(parser, token);
                    break;

                default:
                    goto unexpected;
            }

            break;

        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            status = lxb_css_selectors_state_element(parser, token);
            break;

        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            lxb_css_syntax_parser_consume(parser);
            status = lxb_css_selectors_state_attribute(parser);
            break;

        case LXB_CSS_SYNTAX_TOKEN_COLON:
            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_m(parser, token);

            if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
                status = lxb_css_selectors_state_pseudo_class(parser, token);
                break;
            }
            else if (token->type == LXB_CSS_SYNTAX_TOKEN_COLON) {
                lxb_css_syntax_parser_consume(parser);
                lxb_css_parser_token_m(parser, token);

                if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
                    lxb_css_parser_state_set(parser,
                                             lxb_css_selectors_state_compound_pseudo);
                    status = lxb_css_selectors_state_pseudo_element(parser, token);
                    break;
                }
                else if (token->type != LXB_CSS_SYNTAX_TOKEN_FUNCTION) {
                    return lxb_css_parser_unexpected(parser);
                }

                status = lxb_css_selectors_state_pseudo_element_function(parser, token,
                                               lxb_css_selectors_state_compound_pseudo);
                break;
            }
            else if (token->type != LXB_CSS_SYNTAX_TOKEN_FUNCTION) {
                goto unexpected;
            }

            status = lxb_css_selectors_state_pseudo_class_function(parser, token,
                                            lxb_css_selectors_state_compound_sub);
            break;

        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_m(parser, token);
            goto again;

        case LXB_CSS_SYNTAX_TOKEN__END:
            selectors = parser->selectors;

            if (selectors->combinator > LXB_CSS_SELECTOR_COMBINATOR_CLOSE
                || selectors->list_last->first == NULL)
            {
                goto unexpected;
            }

            return lxb_css_selectors_done(parser);

        default:
            goto unexpected;
    }

    if (status == LXB_STATUS_OK) {
        return true;
    }

    if (status == LXB_STATUS_ERROR_MEMORY_ALLOCATION) {
        return lxb_css_parser_memory_fail(parser);
    }

unexpected:

    (void) lxb_css_parser_states_to_root(parser);
    (void) lxb_css_parser_states_set_back(parser);

    return lxb_css_parser_unexpected(parser);
}

static bool
lxb_css_selectors_state_compound_sub(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx)
{
    lxb_status_t status;

    /* <subclass-selector> */

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_HASH:
            status = lxb_css_selectors_state_hash(parser, token);
            break;

        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            switch (lxb_css_syntax_token_delim_char(token)) {
                case '.':
                    lxb_css_syntax_parser_consume(parser);
                    status = lxb_css_selectors_state_class(parser, token);
                    break;

                default:
                    return lxb_css_parser_states_set_back(parser);
            }

            break;

        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            lxb_css_syntax_parser_consume(parser);
            status = lxb_css_selectors_state_attribute(parser);
            break;

        case LXB_CSS_SYNTAX_TOKEN_COLON:
            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_m(parser, token);

            if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
                status = lxb_css_selectors_state_pseudo_class(parser, token);
                break;
            }
            else if (token->type == LXB_CSS_SYNTAX_TOKEN_COLON) {
                lxb_css_syntax_parser_consume(parser);
                lxb_css_parser_token_m(parser, token);

                if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
                    lxb_css_parser_state_set(parser,
                                      lxb_css_selectors_state_compound_pseudo);
                    status = lxb_css_selectors_state_pseudo_element(parser,
                                                                    token);
                    break;
                }
                else if (token->type != LXB_CSS_SYNTAX_TOKEN_FUNCTION) {
                    return lxb_css_parser_unexpected(parser);
                }

                status = lxb_css_selectors_state_pseudo_element_function(parser,
                                token, lxb_css_selectors_state_compound_pseudo);
                break;
            }
            else if (token->type != LXB_CSS_SYNTAX_TOKEN_FUNCTION) {
                return lxb_css_parser_unexpected(parser);
            }

            status = lxb_css_selectors_state_pseudo_class_function(parser, token,
                                            lxb_css_selectors_state_compound_sub);
            break;

        default:
            return lxb_css_parser_states_set_back(parser);
    }

    if (status == LXB_STATUS_OK) {
        return true;
    }

    if (status == LXB_STATUS_ERROR_MEMORY_ALLOCATION) {
        return lxb_css_parser_memory_fail(parser);
    }

    return lxb_css_parser_unexpected(parser);
}

static bool
lxb_css_selectors_state_compound_pseudo(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        void *ctx)
{
    lxb_status_t status;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_COLON) {
        return lxb_css_parser_states_set_back(parser);
    }

    lxb_css_syntax_parser_consume(parser);
    lxb_css_parser_token_m(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        status = lxb_css_selectors_state_pseudo_class(parser, token);
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_COLON) {
        lxb_css_syntax_parser_consume(parser);
        lxb_css_parser_token_m(parser, token);

        if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
            status = lxb_css_selectors_state_pseudo_element(parser, token);
        }
        else if (token->type == LXB_CSS_SYNTAX_TOKEN_FUNCTION) {
            status = lxb_css_selectors_state_pseudo_element_function(parser, token,
                                           lxb_css_selectors_state_compound_pseudo);
        }
        else {
            return lxb_css_parser_unexpected(parser);
        }
    }
    else if (token->type != LXB_CSS_SYNTAX_TOKEN_FUNCTION) {
        return lxb_css_parser_unexpected(parser);
    }
    else {
        status = lxb_css_selectors_state_pseudo_class_function(parser, token,
                                       lxb_css_selectors_state_compound_pseudo);
    }

    if (status == LXB_STATUS_OK) {
        return true;
    }

    if (status == LXB_STATUS_ERROR_MEMORY_ALLOCATION) {
        return lxb_css_parser_memory_fail(parser);
    }

    return lxb_css_parser_unexpected(parser);
}

/*
 * <simple-selector-list>
 */
bool
lxb_css_selectors_state_simple_list(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    void *ctx)
{
    lxb_css_parser_state_t *states;

    states = lxb_css_parser_states_next(parser, lxb_css_selectors_state_simple_wo_root,
                                        lxb_css_selectors_state_simple_list_end,
                                        ctx, true);
    if (states == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return false;
}

static bool
lxb_css_selectors_state_simple_list_end(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        void *ctx)
{
    return lxb_css_selectors_state_list_end(parser, token,
                                            lxb_css_selectors_state_simple_wo_root);
}

/*
 * <simple-selector>
 */
bool
lxb_css_selectors_state_simple(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_parser_state_t *states;

    states = lxb_css_parser_states_next(parser,
                                        lxb_css_selectors_state_simple_wo_root,
                                        lxb_css_selectors_state_end,
                                        ctx, true);
    if (states == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    return false;
}

static bool
lxb_css_selectors_state_simple_wo_root(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       void *ctx)
{
    lxb_css_selector_list_t *list;

    lxb_css_selectors_state_list_append(parser, parser->selectors, list);

    lxb_css_parser_state_set(parser, lxb_css_selectors_state_simple_handler);

    return false;
}

static bool
lxb_css_selectors_state_simple_handler(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       void *ctx)
{
    lxb_status_t status;

again:

    lxb_css_parser_state_set(parser, lxb_css_selectors_state_simple_back);

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_HASH:
            status = lxb_css_selectors_state_hash(parser, token);
            break;

        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            switch (lxb_css_syntax_token_delim_char(token)) {
                case '.':
                    lxb_css_syntax_parser_consume(parser);
                    status = lxb_css_selectors_state_class(parser, token);
                    break;

                case '|':
                case '*':
                    status = lxb_css_selectors_state_element_ns(parser, token);
                    break;

                default:
                    goto unexpected;
            }

            break;

        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            status = lxb_css_selectors_state_element(parser, token);
            break;

        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            lxb_css_syntax_parser_consume(parser);
            status = lxb_css_selectors_state_attribute(parser);
            break;

        case LXB_CSS_SYNTAX_TOKEN_COLON:
            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_m(parser, token);

            if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
                status = lxb_css_selectors_state_pseudo_class(parser, token);
                break;
            }
            else if (token->type != LXB_CSS_SYNTAX_TOKEN_FUNCTION) {
                goto unexpected;
            }

            status = lxb_css_selectors_state_pseudo_class_function(parser, token,
                                           lxb_css_selectors_state_simple_back);
            break;

        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_m(parser, token);
            goto again;

        case LXB_CSS_SYNTAX_TOKEN__END:
            return lxb_css_parser_states_set_back(parser);

        default:
            goto unexpected;
    }

    if (status == LXB_STATUS_OK) {
        return true;
    }

    if (status == LXB_STATUS_ERROR_MEMORY_ALLOCATION) {
        return lxb_css_parser_memory_fail(parser);
    }

unexpected:

    (void) lxb_css_parser_states_set_back(parser);

    return lxb_css_parser_unexpected(parser);
}

static bool
lxb_css_selectors_state_simple_back(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    void *ctx)
{
    return lxb_css_parser_states_set_back(parser);
}

static lxb_status_t
lxb_css_selectors_state_hash(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token)
{
    lxb_status_t status;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors;
    lxb_css_selector_list_t *last;

    selectors = parser->selectors;
    last = selectors->list_last;

    if (selectors->parent == NULL) {
        lxb_css_selector_sp_set_a(last->specificity,
                                  lxb_css_selector_sp_a(last->specificity) + 1);
    }
    else if (lxb_css_selector_sp_a(last->specificity) == 0) {
        if (selectors->combinator != LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
            last->specificity = 0;
        }

        lxb_css_selector_sp_set_a(last->specificity, 1);
    }

    lxb_css_selectors_state_append(parser, selectors, selector);

    selector->type = LXB_CSS_SELECTOR_TYPE_ID;

    status = lxb_css_syntax_token_string_dup(lxb_css_syntax_token_string(token),
                                             &selector->name, parser->memory->mraw);
    lxb_css_syntax_parser_consume(parser);

    return status;
}

static lxb_status_t
lxb_css_selectors_state_class(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token)
{
    lxb_status_t status;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors;

    lxb_css_parser_token_status_m(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        selectors = parser->selectors;

        lxb_css_selectors_state_specificity_set_b(selectors);
        lxb_css_selectors_state_append(parser, selectors, selector);

        selector->type = LXB_CSS_SELECTOR_TYPE_CLASS;

        status = lxb_css_syntax_token_string_dup(lxb_css_syntax_token_string(token),
                                                 &selector->name, parser->memory->mraw);
        lxb_css_syntax_parser_consume(parser);

        return status;
    }

    return lxb_css_parser_unexpected_status(parser);
}

static lxb_status_t
lxb_css_selectors_state_element_ns(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token)
{
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors;

    selectors = parser->selectors;

    lxb_css_selectors_state_append(parser, selectors, selector);

    selector->type = LXB_CSS_SELECTOR_TYPE_ANY;

    selector->name.data = lexbor_mraw_alloc(parser->memory->mraw, 2);
    if (selector->name.data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    selector->name.data[0] = '*';
    selector->name.data[1] = '\0';
    selector->name.length = 1;

    if (lxb_css_syntax_token_delim_char(token) == '*') {
        lxb_css_syntax_parser_consume(parser);
        return lxb_css_selectors_state_ns(parser, selector);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_selectors_state_ns_ident(parser, selector);
}

static lxb_status_t
lxb_css_selectors_state_element(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token)
{
    lxb_status_t status;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors;

    selectors = parser->selectors;

    lxb_css_selectors_state_specificity_set_c(selectors);

    lxb_css_selectors_state_append(parser, selectors, selector);

    selector->type = LXB_CSS_SELECTOR_TYPE_ELEMENT;

    lxb_css_selectors_state_string_dup_m(selectors, &selector->name);

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_selectors_state_ns(parser, selector);
}


static lxb_status_t
lxb_css_selectors_state_attribute(lxb_css_parser_t *parser)
{
    lxb_char_t modifier;
    lxb_status_t status;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors;
    const lxb_css_syntax_token_t *token;
    lxb_css_selector_attribute_t *attribute;

    selectors = parser->selectors;

    lxb_css_selectors_state_append(parser, selectors, selector);
    lxb_css_parser_token_status_wo_ws_m(parser, token);

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            if (lxb_css_syntax_token_delim_char(token) != '|') {
                goto failed;
            }

            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_status_m(parser, token);

            if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
                goto failed;
            }

            selector->type = LXB_CSS_SELECTOR_TYPE_ATTRIBUTE;

            selector->ns.data = lexbor_mraw_alloc(parser->memory->mraw, 2);
            if (selector->ns.data == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }

            selector->ns.data[0] = '*';
            selector->ns.data[1] = '\0';
            selector->ns.length = 1;

            lxb_css_selectors_state_string_dup_m(parser->selectors,
                                                 &selector->name);

            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_status_wo_ws_m(parser, token);
            break;

        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            selector->type = LXB_CSS_SELECTOR_TYPE_ATTRIBUTE;

            lxb_css_selectors_state_string_dup_m(selectors, &selector->name);

            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_status_m(parser, token);

            if (token->type != LXB_CSS_SYNTAX_TOKEN_DELIM
                || lxb_css_syntax_token_delim_char(token) != '|')
            {
                if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
                    lxb_css_syntax_parser_consume(parser);
                    lxb_css_parser_token_status_m(parser, token);
                }

                break;
            }

            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_status_m(parser, token);

            if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
                attribute = &selector->u.attribute;
                attribute->match = LXB_CSS_SELECTOR_MATCH_DASH;

                goto assignment;
            }

            selector->ns = selector->name;
            lexbor_str_clean_all(&selector->name);

            lxb_css_selectors_state_string_dup_m(selectors, &selector->name);

            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_status_wo_ws_m(parser, token);
            break;

        default:
            goto failed;
    }

    attribute = &selector->u.attribute;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
            goto done;

        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            switch (lxb_css_syntax_token_delim_char(token)) {
                case '~':
                    attribute->match = LXB_CSS_SELECTOR_MATCH_INCLUDE;
                    break;

                case '|':
                    attribute->match = LXB_CSS_SELECTOR_MATCH_DASH;
                    break;

                case '^':
                    attribute->match = LXB_CSS_SELECTOR_MATCH_PREFIX;
                    break;

                case '$':
                    attribute->match = LXB_CSS_SELECTOR_MATCH_SUFFIX;
                    break;

                case '*':
                    attribute->match = LXB_CSS_SELECTOR_MATCH_SUBSTRING;
                    break;

                case '=':
                    attribute->match = LXB_CSS_SELECTOR_MATCH_EQUAL;

                    lxb_css_syntax_parser_consume(parser);
                    lxb_css_parser_token_status_wo_ws_m(parser, token);
                    goto string_or_ident;

                default:
                    goto failed;
            }

            lxb_css_syntax_parser_consume(parser);
            lxb_css_parser_token_status_m(parser, token);

            break;

        default:
            goto failed;
    }

assignment:

    if (token->type != LXB_CSS_SYNTAX_TOKEN_DELIM
        || lxb_css_syntax_token_delim_char(token) != '=')
    {
        goto failed;
    }

    lxb_css_syntax_parser_consume(parser);
    lxb_css_parser_token_status_wo_ws_m(parser, token);

string_or_ident:

    if (token->type != LXB_CSS_SYNTAX_TOKEN_STRING
        && token->type != LXB_CSS_SYNTAX_TOKEN_IDENT)
    {
        goto failed;
    }

    lxb_css_selectors_state_string_dup_m(selectors, &attribute->value);

    lxb_css_syntax_parser_consume(parser);
    lxb_css_parser_token_status_wo_ws_m(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_RS_BRACKET) {
        goto done;
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        goto failed;
    }

    modifier = *lxb_css_syntax_token_string(token)->data;

    switch (modifier) {
        case 'i':
            attribute->modifier = LXB_CSS_SELECTOR_MODIFIER_I;
            break;

        case 's':
            attribute->modifier = LXB_CSS_SELECTOR_MODIFIER_S;
            break;

        default:
            goto failed;
    }

    lxb_css_syntax_parser_consume(parser);
    lxb_css_parser_token_status_wo_ws_m(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_RS_BRACKET) {
        goto failed;
    }

done:

    lxb_css_selectors_state_specificity_set_b(selectors);
    lxb_css_syntax_parser_consume(parser);

    return LXB_STATUS_OK;

failed:

    return lxb_css_parser_unexpected_status(parser);
}

static lxb_status_t
lxb_css_selectors_state_ns(lxb_css_parser_t *parser,
                           lxb_css_selector_t *selector)
{
    const lxb_css_syntax_token_t *token;

    lxb_css_parser_token_status_m(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_DELIM
        && lxb_css_syntax_token_delim_char(token) == '|')
    {
        lxb_css_syntax_parser_consume(parser);
        return lxb_css_selectors_state_ns_ident(parser, selector);
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_selectors_state_ns_ident(lxb_css_parser_t *parser,
                                 lxb_css_selector_t *selector)
{
    lxb_status_t status;
    const lxb_css_syntax_token_t *token;
    lxb_css_selectors_t *selectors;

    lxb_css_parser_token_status_m(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        selectors = parser->selectors;

        lxb_css_selectors_state_specificity_set_c(selectors);

        selector->type = LXB_CSS_SELECTOR_TYPE_ELEMENT;

        selector->ns = selector->name;
        lexbor_str_clean_all(&selector->name);

        status = lxb_css_syntax_token_string_dup(lxb_css_syntax_token_string(token),
                                           &selector->name, parser->memory->mraw);

        lxb_css_syntax_parser_consume(parser);

        return status;
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_DELIM
             && lxb_css_syntax_token_delim_char(token) == '*')
    {
        lxb_css_syntax_parser_consume(parser);

        selector->type = LXB_CSS_SELECTOR_TYPE_ANY;

        selector->ns = selector->name;

        selector->name.data = lexbor_mraw_alloc(parser->memory->mraw, 2);
        if (selector->name.data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        selector->name.data[0] = '*';
        selector->name.data[1] = '\0';
        selector->name.length = 1;

        return LXB_STATUS_OK;
    }

    return lxb_css_parser_unexpected_status(parser);
}

static lxb_status_t
lxb_css_selectors_state_pseudo_class(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token)
{
    lxb_status_t status;
    lxb_css_log_message_t *msg;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors;
    const lxb_css_selectors_pseudo_data_t *pseudo;

    selectors = parser->selectors;

    lxb_css_selectors_state_append(parser, selectors, selector);
    selector->type = LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS;

    lxb_css_selectors_state_string_dup_m(selectors, &selector->name);

    pseudo = lxb_css_selector_pseudo_class_by_name(selector->name.data,
                                                   selector->name.length);
    if (pseudo == NULL) {
        return lxb_css_parser_unexpected_status(parser);
    }

    switch (pseudo->id) {
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_CURRENT:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_DEFAULT:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS_VISIBLE:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FOCUS_WITHIN:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FULLSCREEN:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUTURE:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_IN_RANGE:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_INDETERMINATE:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_INVALID:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_LOCAL_LINK:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_OUT_OF_RANGE:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_PAST:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_SCOPE:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_TARGET:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_TARGET_WITHIN:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_USER_INVALID:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_VALID:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_VISITED:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_WARNING:
            msg = lxb_css_log_not_supported(parser->log,
                                            lxb_css_selectors_module_name,
                                            (const char *) selector->name.data);
            if (msg == NULL) {
                return lxb_css_parser_memory_fail(parser);
            }

            return lxb_css_parser_unexpected_status(parser);

        default:
            break;
    }

    selector->u.pseudo.type = pseudo->id;
    selector->u.pseudo.data = NULL;

    lxb_css_syntax_parser_consume(parser);

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_selectors_state_pseudo_class_function(lxb_css_parser_t *parser,
                                              const lxb_css_syntax_token_t *token,
                                              lxb_css_parser_state_f success)
{
    lxb_status_t status;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors;
    lxb_css_log_message_t *msg;
    lxb_css_syntax_rule_t *rule;
    const lxb_css_selectors_pseudo_data_func_t *func;

    selectors = parser->selectors;

    lxb_css_selectors_state_append(parser, selectors, selector);
    selector->type = LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION;

    lxb_css_selectors_state_string_dup_m(selectors, &selector->name);

    func = lxb_css_selector_pseudo_class_function_by_name(selector->name.data,
                                                          selector->name.length);
    if (func == NULL) {
        return lxb_css_parser_unexpected_status(parser);
    }

    switch (func->id) {
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_DIR:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_LANG:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_COL:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_COL:
            msg = lxb_css_log_not_supported(parser->log,
                                            lxb_css_selectors_module_name,
                                            (const char *) selector->name.data);
            if (msg == NULL) {
                goto failed;
            }

            return lxb_css_parser_unexpected_status(parser);

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_CHILD:
            lxb_css_selectors_state_specificity_set_b(selectors);
            break;

        default:
            break;
    }

    selector->u.pseudo.type = func->id;
    selector->u.pseudo.data = NULL;

    selectors->combinator = func->combinator;
    selectors->comb_default = func->combinator;
    selectors->parent = selector;

    rule = lxb_css_syntax_parser_function_push(parser, token, success,
                                               &func->cb, selectors->list_last);
    if (rule == NULL) {
        goto failed;
    }

    lxb_css_syntax_parser_consume(parser);

    return LXB_STATUS_OK;

failed:

    (void) lxb_css_parser_memory_fail(parser);

    return parser->status;
}

static lxb_status_t
lxb_css_selectors_state_pseudo_element(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token)
{
    lxb_status_t status;
    lxb_css_log_message_t *msg;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors;
    const lxb_css_selectors_pseudo_data_t *pseudo;

    selectors = parser->selectors;

    lxb_css_selectors_state_append(parser, selectors, selector);
    selector->type = LXB_CSS_SELECTOR_TYPE_PSEUDO_ELEMENT;

    lxb_css_selectors_state_string_dup_m(selectors, &selector->name);

    pseudo = lxb_css_selector_pseudo_element_by_name(selector->name.data,
                                                     selector->name.length);
    if (pseudo == NULL) {
        return lxb_css_parser_unexpected_status(parser);
    }

    switch (pseudo->id) {
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_AFTER:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_BACKDROP:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_BEFORE:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_FIRST_LETTER:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_FIRST_LINE:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_GRAMMAR_ERROR:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_INACTIVE_SELECTION:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_MARKER:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_PLACEHOLDER:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_SELECTION:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_SPELLING_ERROR:
        case LXB_CSS_SELECTOR_PSEUDO_ELEMENT_TARGET_TEXT:
            msg = lxb_css_log_not_supported(parser->log,
                                            lxb_css_selectors_module_name,
                                            (const char *) selector->name.data);
            if (msg == NULL) {
                (void) lxb_css_parser_memory_fail(parser);
                return parser->status;
            }

            return lxb_css_parser_unexpected_status(parser);

        default:
            break;
    }

    selector->u.pseudo.type = pseudo->id;
    selector->u.pseudo.data = NULL;

    lxb_css_syntax_parser_consume(parser);

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_selectors_state_pseudo_element_function(lxb_css_parser_t *parser,
                                                const lxb_css_syntax_token_t *token,
                                                lxb_css_parser_state_f success)
{
    lxb_status_t status;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors;
    lxb_css_syntax_rule_t *rule;
    const lxb_css_selectors_pseudo_data_func_t *func;

    selectors = parser->selectors;

    lxb_css_selectors_state_append(parser, selectors, selector);
    selector->type = LXB_CSS_SELECTOR_TYPE_PSEUDO_ELEMENT_FUNCTION;

    lxb_css_selectors_state_string_dup_m(selectors, &selector->name);

    func = lxb_css_selector_pseudo_element_function_by_name(selector->name.data,
                                                            selector->name.length);
    if (func == NULL) {
        return lxb_css_parser_unexpected_status(parser);
    }

    selector->u.pseudo.type = func->id;
    selector->u.pseudo.data = NULL;

    selectors->combinator = func->combinator;
    selectors->comb_default = func->combinator;
    selectors->parent = selector;

    rule = lxb_css_syntax_parser_function_push(parser, token, success,
                                               &func->cb, selectors->list_last);
    if (rule == NULL) {
        (void) lxb_css_parser_memory_fail(parser);
        return parser->status;
    }

    lxb_css_syntax_parser_consume(parser);

    return LXB_STATUS_OK;
}

lxb_inline void
lxb_css_selectors_state_restore_combinator(lxb_css_selectors_t *selectors)
{
    lxb_css_selector_t *parent;
    lxb_css_selector_combinator_t comb_default;
    const lxb_css_selectors_pseudo_data_func_t *data_func;

    comb_default = LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT;

    if (selectors->parent != NULL) {
        parent = selectors->parent;

        if (parent->type == LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION) {
            data_func = lxb_css_selector_pseudo_class_function_by_id(parent->u.pseudo.type);
        }
        else {
            data_func = lxb_css_selector_pseudo_element_function_by_id(parent->u.pseudo.type);
        }

        comb_default = data_func->combinator;
    }

    selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_CLOSE;
    selectors->comb_default = comb_default;
}

lxb_status_t
lxb_css_selectors_state_function_end(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx, bool failed)
{
    bool cy;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors = parser->selectors;

    if (token->type == LXB_CSS_SYNTAX_TOKEN__EOF) {
        (void) lxb_css_log_format(parser->log, LXB_CSS_LOG_ERROR,
                                  "%s. End Of File in pseudo function",
                                  lxb_css_selectors_module_name);
    }

    if (selectors->list_last == NULL) {
        lxb_css_selectors_state_restore_parent(selectors, ctx);
        goto empty;
    }

    lxb_css_selectors_state_restore_parent(selectors, ctx);

    return LXB_STATUS_OK;

empty:

    selector = selectors->list_last->last;

    cy = selector->type == LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION;
    cy = lxb_css_selector_pseudo_function_can_empty(selector->u.pseudo.type,
                                                    cy);
    if (cy) {
        lxb_css_parser_set_ok(parser);
        return LXB_STATUS_OK;
    }

    (void) lxb_css_log_format(parser->log, LXB_CSS_LOG_ERROR,
                              "%s. Pseudo function can't be empty: %S()",
                              lxb_css_selectors_module_name, &selector->name);

    lxb_css_selector_remove(selector);
    lxb_css_selector_destroy(selector);

    lxb_css_parser_failed_set_by_id(parser, -1, true);
    selectors->err_in_function = true;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_selectors_state_function_forgiving(lxb_css_parser_t *parser,
                                           const lxb_css_syntax_token_t *token,
                                           void *ctx, bool failed)
{
    return lxb_css_selectors_state_forgiving_cb(parser, token, ctx,
                                                lxb_css_selectors_state_complex_list,
                                                failed);
}

lxb_status_t
lxb_css_selectors_state_function_forgiving_relative(lxb_css_parser_t *parser,
                                                    const lxb_css_syntax_token_t *token,
                                                    void *ctx, bool failed)
{
    return lxb_css_selectors_state_forgiving_cb(parser, token, ctx,
                                                lxb_css_selectors_state_relative_list,
                                                failed);
}

static lxb_status_t
lxb_css_selectors_state_forgiving_cb(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx, lxb_css_parser_state_f state,
                                     bool failed)
{
    bool cy;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors = parser->selectors;

    lxb_css_parser_set_ok(parser);

    if (token->type == LXB_CSS_SYNTAX_TOKEN__EOF) {
        (void) lxb_css_log_format(parser->log, LXB_CSS_LOG_ERROR,
                                  "%s. End Of File in pseudo function",
                                  lxb_css_selectors_module_name);
    }

    if (selectors->list_last == NULL) {
        lxb_css_selectors_state_restore_parent(selectors, ctx);
        goto empty;
    }

    if (selectors->parent->u.pseudo.type
        == LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_WHERE)
    {
        selectors->list_last->specificity = 0;
    }

    lxb_css_selectors_state_restore_parent(selectors, ctx);

    return LXB_STATUS_OK;

empty:

    selector = selectors->list_last->last;

    cy = selector->type == LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION;
    cy = lxb_css_selector_pseudo_function_can_empty(selector->u.pseudo.type,
                                                    cy);
    if (cy) {
        return LXB_STATUS_OK;
    }

    (void) lxb_css_log_format(parser->log, LXB_CSS_LOG_ERROR,
                              "%s. Pseudo function can't be empty: %S()",
                              lxb_css_selectors_module_name, &selector->name);

    lxb_css_selector_remove(selector);
    lxb_css_selector_destroy(selector);

    lxb_css_parser_failed_set_by_id(parser, -1, true);
    selectors->err_in_function = true;

    return LXB_STATUS_OK;
}

static void
lxb_css_selectors_state_restore_parent(lxb_css_selectors_t *selectors,
                                       lxb_css_selector_list_t *last)
{
    uint32_t src, dst;

    if (selectors->list_last != NULL && selectors->list_last != last) {
        dst = last->specificity;
        src = selectors->list_last->specificity;

        selectors->list_last = 0;

        if (last->parent == NULL) {
            lxb_css_selector_sp_add_a(dst, lxb_css_selector_sp_a(src));
            lxb_css_selector_sp_add_b(dst, lxb_css_selector_sp_b(src));
            lxb_css_selector_sp_add_c(dst, lxb_css_selector_sp_c(src));
        }
        else if (selectors->combinator == LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
            dst |= src;
        }
        else if (src > dst) {
            dst = src;
        }

        last->specificity = dst;
    }

    if (selectors->list != NULL) {
        last->last->u.pseudo.data = selectors->list;
    }

    selectors->list_last = last;

    /* Get first Selector in chain. */
    while (last->prev != NULL) {
        last = last->prev;
    }

    selectors->list = last;
    selectors->parent = last->parent;

    lxb_css_selectors_state_restore_combinator(selectors);
}

static bool
lxb_css_selectors_state_list_end(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 lxb_css_parser_state_f state)
{
    lxb_css_parser_state_t *states;
    lxb_css_selectors_t *selectors = parser->selectors;

    if (lxb_css_parser_is_failed(parser)) {
        token = lxb_css_selectors_state_function_error(parser, token);
        if (token == NULL) {
            return lxb_css_parser_fail(parser,
                                       LXB_STATUS_ERROR_MEMORY_ALLOCATION);
        }
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
        lxb_css_syntax_parser_consume(parser);
        lxb_css_parser_token_status_m(parser, token);
    }

    if (selectors->parent != NULL && selectors->list_last &&
        selectors->list_last->prev != NULL)
    {
        lxb_css_selectors_state_func_specificity(selectors);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_COMMA) {
        states = lxb_css_parser_states_current(parser);

        if (states->root) {
            if (token->type != LXB_CSS_SYNTAX_TOKEN__END) {
                token = lxb_css_selectors_state_function_error(parser, token);
                if (token == NULL) {
                    return lxb_css_parser_fail(parser,
                                               LXB_STATUS_ERROR_MEMORY_ALLOCATION);
                }
            }

            (void) lxb_css_parser_states_pop(parser);
            return lxb_css_parser_success(parser);
        }

        return lxb_css_selectors_done(parser);
    }

    selectors->combinator = selectors->comb_default;

    lxb_css_syntax_token_consume(parser->tkz);
    lxb_css_parser_state_set(parser, state);
    lxb_css_parser_set_ok(parser);

    return true;
}

static bool
lxb_css_selectors_state_end(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_parser_state_t *states;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_WHITESPACE) {
        lxb_css_syntax_parser_consume(parser);
        lxb_css_parser_token_status_m(parser, token);
    }

    if (lxb_css_parser_is_failed(parser)) {
        token = lxb_css_selectors_state_function_error(parser, token);
        if (token == NULL) {
            return lxb_css_parser_fail(parser,
                                       LXB_STATUS_ERROR_MEMORY_ALLOCATION);
        }
    }

    states = lxb_css_parser_states_current(parser);

    if (states->root) {
        if (token->type != LXB_CSS_SYNTAX_TOKEN__END) {
            token = lxb_css_selectors_state_function_error(parser, token);
            if (token == NULL) {
                return lxb_css_parser_fail(parser,
                                           LXB_STATUS_ERROR_MEMORY_ALLOCATION);
            }
        }

        (void) lxb_css_parser_states_pop(parser);
        return lxb_css_parser_success(parser);
    }

    return lxb_css_selectors_done(parser);
}


static const lxb_css_syntax_token_t *
lxb_css_selectors_state_function_error(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token)
{
    bool cy, comma;
    lxb_css_selector_list_t *list;
    lxb_css_selector_t *selector;
    lxb_css_selectors_t *selectors = parser->selectors;
    const lxb_css_syntax_token_t *origin;
    const lxb_css_selectors_pseudo_data_func_t *func;

    cy = false;
    comma = true;
    list = selectors->list_last;
    selector = selectors->parent;

    if (selector != NULL) {
        cy = selector->type == LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION;

        func = lxb_css_selector_pseudo_function_by_id(selector->u.pseudo.type,
                                                      cy);
        if (func == NULL) {
            return NULL;
        }

        cy = func->forgiving;
        comma = func->comma;
    }

    if (!selectors->err_in_function) {
        origin = lxb_css_syntax_token(parser->tkz);
        if (origin == NULL) {
            return NULL;
        }

        if (token->type != LXB_CSS_SYNTAX_TOKEN__END) {
            origin = token;
        }
        else if (origin->type != LXB_CSS_SYNTAX_TOKEN__EOF) {
            origin = NULL;
        }

        if (origin != NULL) {
            if (lxb_css_syntax_token_error(parser, origin,
                                           "Selectors") == NULL)
            {
                return NULL;
            }
        }
    }

    selectors->err_in_function = false;

    if (cy) {
        lxb_css_selector_list_selectors_remove(selectors, list);
        lxb_css_selector_list_destroy(list);

        while (token != NULL
               && token->type != LXB_CSS_SYNTAX_TOKEN__END)
        {
            if (comma == true
                && token->type == LXB_CSS_SYNTAX_TOKEN_COMMA
                && lxb_css_parser_rule_deep(parser) == 0)
            {
                break;
            }

            lxb_css_syntax_parser_consume(parser);
            token = lxb_css_syntax_parser_token(parser);
        }

        return token;
    }

    lxb_css_selector_list_destroy_chain(selectors->list);

    selectors->list = NULL;
    selectors->list_last = NULL;

    while (token != NULL
           && token->type != LXB_CSS_SYNTAX_TOKEN__END)
    {
        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return token;
}
