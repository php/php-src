/*
 * Copyright (C) 2020-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_STYLESHEET_H
#define LXB_CSS_STYLESHEET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/rule.h"
#include "lexbor/core/mraw.h"
#include "lexbor/css/at_rule.h"
#include "lexbor/css/syntax/tokenizer.h"
#include "lexbor/css/selectors/base.h"


struct lxb_css_stylesheet {
    lxb_css_rule_t           *root;
    lxb_css_memory_t         *memory;

    void                     *element; /* lxb_html_style_element_t * */
};

LXB_API lxb_css_stylesheet_t *
lxb_css_stylesheet_create(lxb_css_memory_t *memory);

LXB_API lxb_css_stylesheet_t *
lxb_css_stylesheet_destroy(lxb_css_stylesheet_t *sst, bool destroy_memory);

LXB_API lxb_css_stylesheet_t *
lxb_css_stylesheet_parse(lxb_css_parser_t *parser,
                         const lxb_char_t *data, size_t length);

LXB_API lxb_status_t
lxb_css_stylesheet_prepare(lxb_css_parser_t *parser, lxb_css_memory_t *memory,
                           lxb_css_selectors_t *selectors);

LXB_API lxb_css_stylesheet_t *
lxb_css_stylesheet_process(lxb_css_parser_t *parser,
                           const lxb_char_t *data, size_t length);

LXB_API void
lxb_css_stylesheet_finish(lxb_css_parser_t *parser);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_STYLESHEET_H */
