/*
 * Copyright (C) 2021-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/css.h"
#include "lexbor/css/stylesheet.h"
#include "lexbor/css/parser.h"
#include "lexbor/css/at_rule.h"
#include "lexbor/css/property.h"
#include "lexbor/css/rule.h"
#include "lexbor/css/state.h"
#include "lexbor/css/selectors/selectors.h"
#include "lexbor/css/selectors/state.h"


lxb_css_stylesheet_t *
lxb_css_stylesheet_create(lxb_css_memory_t *memory)
{
    lxb_status_t status;
    lxb_css_stylesheet_t *stylesheet;

    if (memory == NULL) {
        memory = lxb_css_memory_create();
        status = lxb_css_memory_init(memory, 1024);

        if (status != LXB_STATUS_OK) {
            (void) lxb_css_memory_destroy(memory, true);
            return NULL;
        }
    }
    else {
        (void) lxb_css_memory_ref_inc(memory);
    }

    stylesheet = lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_stylesheet_t));
    if (stylesheet == NULL) {
        return NULL;
    }

    stylesheet->memory = memory;

    return stylesheet;
}

lxb_css_stylesheet_t *
lxb_css_stylesheet_destroy(lxb_css_stylesheet_t *sst, bool destroy_memory)
{
    if (sst == NULL) {
        return NULL;
    }

    if (destroy_memory) {
        (void) lxb_css_memory_ref_dec_destroy(sst->memory);
        return NULL;
    }

    if (sst->root != NULL) {
        (void) lxb_css_rule_destroy(sst->root, true);
    }

    (void) lexbor_mraw_free(sst->memory->mraw, sst);

    return NULL;
}

lxb_status_t
lxb_css_stylesheet_parse(lxb_css_stylesheet_t *sst, lxb_css_parser_t *parser,
                         const lxb_char_t *data, size_t length)
{
    lxb_status_t status;
    lxb_css_rule_list_t *list;
    lxb_css_selectors_t selectors;

    if (sst == NULL || parser == NULL) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    if (parser->selectors == NULL) {
        status = lxb_css_selectors_init(&selectors);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        parser->selectors = &selectors;
    }
    else {
        lxb_css_selectors_clean(parser->selectors);
    }

    parser->memory = sst->memory;

    list = lxb_css_syntax_parse_list_rules(parser,
                                           lxb_css_state_cb_list_rules(),
                                           data, length);

    if (parser->selectors == &selectors) {
        parser->selectors = lxb_css_selectors_destroy(&selectors, false);
    }

    if (list == NULL) {
        sst->root = NULL;
        return parser->status;
    }

    sst->root = &list->rule;

    return LXB_STATUS_OK;
}
