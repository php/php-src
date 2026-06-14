/*
 * Copyright (C) 2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_BLANK_H
#define LXB_CSS_BLANK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"
#include "lexbor/css/syntax/syntax.h"


LXB_API const lxb_css_syntax_cb_list_rules_t *
lxb_css_blank_cb_list_rules(void);

LXB_API const lxb_css_syntax_cb_at_rule_t *
lxb_css_blank_cb_at_rule(void);

LXB_API const lxb_css_syntax_cb_qualified_rule_t *
lxb_css_blank_cb_qualified_rule(void);

LXB_API const lxb_css_syntax_cb_block_t *
lxb_css_blank_cb_block(void);

LXB_API const lxb_css_syntax_cb_declarations_t *
lxb_css_blank_cb_declarations(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_BLANK_H */
