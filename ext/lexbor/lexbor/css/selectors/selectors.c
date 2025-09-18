/*
 * Copyright (C) 2020-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/print.h"
#include "lexbor/css/css.h"


static lxb_css_selector_list_t *
lxb_css_selectors_parse_list(lxb_css_parser_t *parser,
                             const lxb_css_syntax_cb_components_t *components,
                             const lxb_char_t *data, size_t length);

static lxb_status_t
lxb_css_selectors_components_end(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx, bool failed);


static const lxb_css_syntax_cb_components_t lxb_css_selectors_complex_list_cb = {
    .state = lxb_css_selectors_state_complex_list,
    .block = NULL,
    .failed = lxb_css_state_failed,
    .end = lxb_css_selectors_components_end
};

static const lxb_css_syntax_cb_components_t lxb_css_selectors_compound_list_cb = {
    .state = lxb_css_selectors_state_compound_list,
    .block = NULL,
    .failed = lxb_css_state_failed,
    .end = lxb_css_selectors_components_end
};

static const lxb_css_syntax_cb_components_t lxb_css_selectors_simple_list_cb = {
    .state = lxb_css_selectors_state_simple_list,
    .block = NULL,
    .failed = lxb_css_state_failed,
    .end = lxb_css_selectors_components_end
};

static const lxb_css_syntax_cb_components_t lxb_css_selectors_relative_list_cb = {
    .state = lxb_css_selectors_state_relative_list,
    .block = NULL,
    .failed = lxb_css_state_failed,
    .end = lxb_css_selectors_components_end
};

static const lxb_css_syntax_cb_components_t lxb_css_selectors_complex_cb = {
    .state = lxb_css_selectors_state_complex,
    .block = NULL,
    .failed = lxb_css_state_failed,
    .end = lxb_css_selectors_components_end
};

static const lxb_css_syntax_cb_components_t lxb_css_selectors_compound_cb = {
    .state = lxb_css_selectors_state_compound,
    .block = NULL,
    .failed = lxb_css_state_failed,
    .end = lxb_css_selectors_components_end
};

static const lxb_css_syntax_cb_components_t lxb_css_selectors_simple_cb = {
    .state = lxb_css_selectors_state_simple,
    .block = NULL,
    .failed = lxb_css_state_failed,
    .end = lxb_css_selectors_components_end
};

static const lxb_css_syntax_cb_components_t lxb_css_selectors_relative_cb = {
    .state = lxb_css_selectors_state_relative,
    .block = NULL,
    .failed = lxb_css_state_failed,
    .end = lxb_css_selectors_components_end
};


lxb_css_selectors_t *
lxb_css_selectors_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_css_selectors_t));
}

lxb_status_t
lxb_css_selectors_init(lxb_css_selectors_t *selectors)
{
    if (selectors == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    selectors->list = NULL;
    selectors->list_last = NULL;
    selectors->parent = NULL;
    selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT;
    selectors->comb_default = LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT;
    selectors->error = 0;
    selectors->err_in_function = false;
    selectors->failed = false;

    return LXB_STATUS_OK;
}

void
lxb_css_selectors_clean(lxb_css_selectors_t *selectors)
{
    if (selectors != NULL) {
        selectors->list = NULL;
        selectors->list_last = NULL;
        selectors->parent = NULL;
        selectors->combinator = LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT;
        selectors->comb_default = LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT;
        selectors->error = 0;
        selectors->err_in_function = false;
        selectors->failed = false;
    }
}

lxb_css_selectors_t *
lxb_css_selectors_destroy(lxb_css_selectors_t *selectors, bool self_destroy)
{
    if (selectors == NULL) {
        return NULL;
    }

    if (self_destroy) {
        return lexbor_free(selectors);
    }

    return selectors;
}

lxb_css_selector_list_t *
lxb_css_selectors_parse(lxb_css_parser_t *parser,
                        const lxb_char_t *data, size_t length)
{
    return lxb_css_selectors_parse_complex_list(parser, data, length);
}

lxb_css_selector_list_t *
lxb_css_selectors_parse_complex_list(lxb_css_parser_t *parser,
                                     const lxb_char_t *data, size_t length)
{
    return lxb_css_selectors_parse_list(parser, &lxb_css_selectors_complex_list_cb,
                                        data, length);
}

lxb_css_selector_list_t *
lxb_css_selectors_parse_compound_list(lxb_css_parser_t *parser,
                                      const lxb_char_t *data, size_t length)
{
    return lxb_css_selectors_parse_list(parser, &lxb_css_selectors_compound_list_cb,
                                        data, length);
}

lxb_css_selector_list_t *
lxb_css_selectors_parse_simple_list(lxb_css_parser_t *parser,
                                    const lxb_char_t *data, size_t length)
{
    return lxb_css_selectors_parse_list(parser, &lxb_css_selectors_simple_list_cb,
                                        data, length);
}

lxb_css_selector_list_t *
lxb_css_selectors_parse_relative_list(lxb_css_parser_t *parser,
                                      const lxb_char_t *data, size_t length)
{
    return lxb_css_selectors_parse_list(parser, &lxb_css_selectors_relative_list_cb,
                                        data, length);
}

static lxb_status_t
lxb_css_selectors_parse_prepare(lxb_css_parser_t *parser,
                                lxb_css_memory_t *memory,
                                lxb_css_selectors_t *selectors)
{
    if (parser->stage != LXB_CSS_PARSER_CLEAN) {
        if (parser->stage == LXB_CSS_PARSER_RUN) {
            return LXB_STATUS_ERROR_WRONG_ARGS;
        }

        lxb_css_parser_clean(parser);
    }

    parser->tkz->with_comment = false;
    parser->stage = LXB_CSS_PARSER_RUN;

    parser->old_memory = parser->memory;
    parser->old_selectors = parser->selectors;

    parser->memory = memory;
    parser->selectors = selectors;

    return LXB_STATUS_OK;
}

static lxb_css_selector_list_t *
lxb_css_selectors_parse_process(lxb_css_parser_t *parser,
                                const lxb_css_syntax_cb_components_t *components,
                                const lxb_char_t *data, size_t length)
{
    lxb_css_syntax_rule_t *rule;

    lxb_css_parser_buffer_set(parser, data, length);

    rule = lxb_css_syntax_parser_components_push(parser, NULL, NULL,
                                                 components, NULL,
                                                 LXB_CSS_SYNTAX_TOKEN_UNDEF);
    if (rule == NULL) {
        return NULL;
    }

    parser->status = lxb_css_syntax_parser_run(parser);
    if (parser->status != LXB_STATUS_OK) {
        return NULL;
    }

    return parser->selectors->list;
}

static void
lxb_css_selectors_parse_finish(lxb_css_parser_t *parser)
{
    parser->stage = LXB_CSS_PARSER_END;

    parser->memory = parser->old_memory;
    parser->selectors = parser->old_selectors;
}

static lxb_css_selector_list_t *
lxb_css_selectors_parse_list(lxb_css_parser_t *parser,
                             const lxb_css_syntax_cb_components_t *components,
                             const lxb_char_t *data, size_t length)
{
    lxb_css_memory_t *memory;
    lxb_css_selectors_t *selectors;
    lxb_css_selector_list_t *list;

    memory = parser->memory;
    selectors = parser->selectors;

    if (selectors == NULL) {
        selectors = lxb_css_selectors_create();
        parser->status = lxb_css_selectors_init(selectors);

        if (parser->status != LXB_STATUS_OK) {
            (void) lxb_css_selectors_destroy(selectors, true);
            return NULL;
        }
    }
    else {
        lxb_css_selectors_clean(selectors);
    }

    if (memory == NULL) {
        memory = lxb_css_memory_create();
        parser->status = lxb_css_memory_init(memory, 256);

        if (parser->status != LXB_STATUS_OK) {
            if (selectors != parser->selectors) {
                (void) lxb_css_selectors_destroy(selectors, true);
            }

            (void) lxb_css_memory_destroy(memory, true);
            return NULL;
        }
    }

    parser->status = lxb_css_selectors_parse_prepare(parser, memory, selectors);
    if (parser->status != LXB_STATUS_OK) {
        list = NULL;
        goto end;
    }

    list = lxb_css_selectors_parse_process(parser, components, data, length);

    lxb_css_selectors_parse_finish(parser);

end:

    if (list == NULL && memory != parser->memory) {
        (void) lxb_css_memory_destroy(memory, true);
    }

    if (selectors != parser->selectors) {
        (void) lxb_css_selectors_destroy(selectors, true);
    }

    return list;
}

static lxb_status_t
lxb_css_selectors_components_end(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 void *ctx, bool failed)
{
    lxb_css_selector_list_t *list;
    lxb_css_selectors_t *selectors = parser->selectors;

    if (failed) {
        list = selectors->list_last;

        if (list != NULL) {
            lxb_css_selector_list_selectors_remove(selectors, list);
            lxb_css_selector_list_destroy(list);
        }
    }

    return LXB_STATUS_OK;
}

lxb_css_selector_list_t *
lxb_css_selectors_parse_complex(lxb_css_parser_t *parser,
                                const lxb_char_t *data, size_t length)
{
    return lxb_css_selectors_parse_list(parser, &lxb_css_selectors_complex_cb,
                                        data, length);
}

lxb_css_selector_list_t *
lxb_css_selectors_parse_compound(lxb_css_parser_t *parser,
                                 const lxb_char_t *data, size_t length)
{
    return lxb_css_selectors_parse_list(parser, &lxb_css_selectors_compound_cb,
                                        data, length);
}

lxb_css_selector_list_t *
lxb_css_selectors_parse_simple(lxb_css_parser_t *parser,
                               const lxb_char_t *data, size_t length)
{
    return lxb_css_selectors_parse_list(parser, &lxb_css_selectors_simple_cb,
                                        data, length);
}

lxb_css_selector_list_t *
lxb_css_selectors_parse_relative(lxb_css_parser_t *parser,
                                 const lxb_char_t *data, size_t length)
{
    return lxb_css_selectors_parse_list(parser, &lxb_css_selectors_relative_cb,
                                        data, length);
}
