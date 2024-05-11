/*
 * Copyright (C) 2020-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_H
#define LXB_CSS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"
#include "lexbor/css/log.h"
#include "lexbor/css/parser.h"
#include "lexbor/css/stylesheet.h"
#include "lexbor/css/property.h"
#include "lexbor/css/value.h"
#include "lexbor/css/at_rule.h"
#include "lexbor/css/rule.h"
#include "lexbor/css/unit.h"
#include "lexbor/css/state.h"
#include "lexbor/css/declaration.h"
#include "lexbor/css/syntax/tokenizer/error.h"
#include "lexbor/css/syntax/tokenizer.h"
#include "lexbor/css/syntax/token.h"
#include "lexbor/css/syntax/parser.h"
#include "lexbor/css/syntax/anb.h"
#include "lexbor/css/selectors/selectors.h"
#include "lexbor/css/selectors/selector.h"
#include "lexbor/css/selectors/state.h"
#include "lexbor/css/selectors/pseudo.h"


LXB_API lxb_status_t
lxb_css_make_data(lxb_css_parser_t *parser, lexbor_str_t *str,
                  uintptr_t begin, uintptr_t end);

LXB_API lxb_char_t *
lxb_css_serialize_char_handler(const void *style, lxb_css_style_serialize_f cb,
                               size_t *out_length);
LXB_API lxb_status_t
lxb_css_serialize_str_handler(const void *style, lexbor_str_t *str,
                              lexbor_mraw_t *mraw, lxb_css_style_serialize_f cb);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_H */
