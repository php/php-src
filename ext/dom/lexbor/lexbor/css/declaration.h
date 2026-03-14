/*
 * Copyright (C) 2022-2026 Alexander Borisov
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


LXB_API lxb_css_rule_declaration_t *
lxb_css_declaration_create(lxb_css_parser_t *parser,
                           const lxb_char_t *name, size_t length,
                           const lxb_css_entry_data_t **out_entry);


LXB_API lxb_css_rule_declaration_list_t *
lxb_css_declaration_list_parse(lxb_css_parser_t *parser,
                               const lxb_char_t *data, size_t length);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_DECLARATION_H */
