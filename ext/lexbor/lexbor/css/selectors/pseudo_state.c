/*
 * Copyright (C) 2020-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/css.h"
#include "lexbor/css/selectors/pseudo_state.h"
#include "lexbor/css/selectors/selectors.h"


static bool
lxb_css_selectors_state_pseudo_anb(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

static bool
lxb_css_selectors_state_pseudo_of_begin(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx);

static lxb_status_t
lxb_css_selectors_state_pseudo_of_end(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx, bool failed);
static bool
lxb_css_selectors_state_pseudo_of_back(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx);


static const lxb_css_syntax_cb_components_t lxb_css_selectors_comp = {
    .state = lxb_css_selectors_state_complex_list,
    .block = NULL,
    .failed = lxb_css_state_failed,
    .end = lxb_css_selectors_state_pseudo_of_end
};


lxb_inline bool
lxb_css_selectors_state_pseudo_anb_begin(lxb_css_parser_t *parser,
    const lxb_css_syntax_token_t *token, void *ctx)
{
    (void) lxb_css_selectors_state_pseudo_anb(parser, token, ctx);
    if (parser->status != LXB_STATUS_OK) {
        parser->selectors->list = NULL;
        parser->selectors->list_last = NULL;

        return lxb_css_parser_failed(parser);
    }

    parser->selectors->list = NULL;

    return lxb_css_parser_success(parser);
}


bool
lxb_css_selectors_state_pseudo_class_function__undef(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_parser_fail(parser, LXB_STATUS_ERROR_UNEXPECTED_DATA);
}

bool
lxb_css_selectors_state_pseudo_class_function_current(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_parser_state_set(parser, lxb_css_selectors_state_complex_list);

    parser->selectors->list = NULL;
    parser->selectors->list_last = NULL;

    return true;
}

bool
lxb_css_selectors_state_pseudo_class_function_dir(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_parser_fail(parser, LXB_STATUS_ERROR_UNEXPECTED_DATA);
}

bool
lxb_css_selectors_state_pseudo_class_function_has(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_parser_state_set(parser, lxb_css_selectors_state_relative_list);

    parser->selectors->list = NULL;
    parser->selectors->list_last = NULL;

    return true;
}

bool
lxb_css_selectors_state_pseudo_class_function_is(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_parser_state_set(parser, lxb_css_selectors_state_complex_list);

    parser->selectors->list = NULL;
    parser->selectors->list_last = NULL;

    return true;
}

bool
lxb_css_selectors_state_pseudo_class_function_lang(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_parser_fail(parser, LXB_STATUS_ERROR_UNEXPECTED_DATA);
}

bool
lxb_css_selectors_state_pseudo_class_function_not(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_parser_state_set(parser, lxb_css_selectors_state_complex_list);

    parser->selectors->list = NULL;
    parser->selectors->list_last = NULL;

    return true;
}

bool
lxb_css_selectors_state_pseudo_class_function_nth_child(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_selectors_state_pseudo_of_begin(parser, token, ctx);
}

bool
lxb_css_selectors_state_pseudo_class_function_nth_col(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_selectors_state_pseudo_anb_begin(parser, token, ctx);
}

bool
lxb_css_selectors_state_pseudo_class_function_nth_last_child(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_selectors_state_pseudo_of_begin(parser, token, ctx);
}

bool
lxb_css_selectors_state_pseudo_class_function_nth_last_col(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_selectors_state_pseudo_anb_begin(parser, token, ctx);
}

bool
lxb_css_selectors_state_pseudo_class_function_nth_last_of_type(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_selectors_state_pseudo_anb_begin(parser, token, ctx);
}

bool
lxb_css_selectors_state_pseudo_class_function_nth_of_type(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_selectors_state_pseudo_anb_begin(parser, token, ctx);
}

bool
lxb_css_selectors_state_pseudo_class_function_where(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_parser_state_set(parser, lxb_css_selectors_state_complex_list);

    parser->selectors->list = NULL;
    parser->selectors->list_last = NULL;

    return true;
}

bool
lxb_css_selectors_state_pseudo_element_function__undef(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    return true;
}

static bool
lxb_css_selectors_state_pseudo_anb(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token,
                                   void *ctx)
{
    lxb_css_selectors_t *selectors;
    lxb_css_selector_list_t *list;
    lxb_css_selector_anb_of_t *anbof;

    selectors = parser->selectors;

    anbof = lexbor_mraw_alloc(parser->memory->mraw,
                              sizeof(lxb_css_selector_anb_of_t));
    if (anbof == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    parser->status = lxb_css_syntax_anb_handler(parser, token, &anbof->anb);
    if (parser->status != LXB_STATUS_OK) {
        lexbor_mraw_free(parser->memory->mraw, anbof);
        return true;
    }

    list = selectors->list_last;
    list->last->u.pseudo.data = anbof;

    anbof->of = NULL;

    return true;
}

static bool
lxb_css_selectors_state_pseudo_of_begin(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        void *ctx)
{
    lxb_css_syntax_rule_t *rule;
    lxb_css_selectors_t *selectors;
    lxb_css_selector_list_t *list;
    lxb_css_syntax_token_ident_t *ident;

    static const lxb_char_t of[] = "of";

    selectors = parser->selectors;

    (void) lxb_css_selectors_state_pseudo_anb(parser, token, ctx);
    if (parser->status != LXB_STATUS_OK) {
        selectors->list = NULL;
        selectors->list_last = NULL;

        token = lxb_css_syntax_parser_token(parser);
        if (token == NULL) {
            return lxb_css_parser_memory_fail(parser);
        }

        if (token->type != LXB_CSS_SYNTAX_TOKEN__END) {
            if (lxb_css_syntax_token_error(parser, token, "Selectors") == NULL) {
                return lxb_css_parser_memory_fail(parser);
            }
        }

        return lxb_css_parser_failed(parser);
    }

    list = selectors->list_last;

    selectors->list = NULL;

    lxb_css_parser_token_wo_ws_m(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        ident = lxb_css_syntax_token_ident(token);

        if (ident->length == sizeof(of) - 1
            && lexbor_str_data_ncasecmp(ident->data, of, ident->length))
        {
            lxb_css_syntax_token_consume(parser->tkz);

            selectors->list = NULL;
            selectors->list_last = NULL;

            token = lxb_css_syntax_parser_token(parser);
            if (token == NULL) {
                return lxb_css_parser_memory_fail(parser);
            }

            rule = lxb_css_syntax_parser_components_push(parser, token,
                                                         lxb_css_selectors_state_pseudo_of_back,
                                                         &lxb_css_selectors_comp, list,
                                                         LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS);
            if (rule == NULL) {
                lexbor_mraw_free(parser->memory->mraw,
                                 list->last->u.pseudo.data);
                return lxb_css_parser_memory_fail(parser);
            }

            lxb_css_parser_state_set(parser,
                                     lxb_css_selectors_state_complex_list);
            return true;
        }
    }

    return lxb_css_parser_success(parser);
}

static lxb_status_t
lxb_css_selectors_state_pseudo_of_end(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      void *ctx, bool failed)
{
    lxb_css_selector_anb_of_t *anbof;
    lxb_css_selector_list_t *list = ctx;

    anbof = list->last->u.pseudo.data;
    anbof->of = parser->selectors->list;

    parser->selectors->list = NULL;

    return LXB_STATUS_OK;
}

static bool
lxb_css_selectors_state_pseudo_of_back(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx)
{
    parser->selectors->list = NULL;

    return lxb_css_parser_success(parser);
}
