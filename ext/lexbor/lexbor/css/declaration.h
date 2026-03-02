/*
 * Copyright (C) 2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_DECLARATION_H
#define LXB_CSS_DECLARATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/css.h"
#include "lexbor/core/mraw.h"


LXB_API lxb_status_t
lxb_css_declaration_list_prepare(lxb_css_parser_t *parser,
                                 lxb_css_memory_t *mem);

LXB_API lxb_css_rule_declaration_list_t *
lxb_css_declaration_list_process(lxb_css_parser_t *parser,
                                 const lxb_char_t *data, size_t length);

LXB_API void
lxb_css_declaration_list_finish(lxb_css_parser_t *parser);

LXB_API lxb_css_rule_declaration_list_t *
lxb_css_declaration_list_parse(lxb_css_parser_t *parser, lxb_css_memory_t *mem,
                               const lxb_char_t *data, size_t length);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_DECLARATION_H */
