/*
 * Copyright (C) 2021-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_STATE_H
#define LXB_CSS_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"


LXB_API bool
lxb_css_state_success(lxb_css_parser_t *parser,
                      const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_state_failed(lxb_css_parser_t *parser,
                     const lxb_css_syntax_token_t *token, void *ctx);

LXB_API bool
lxb_css_state_stop(lxb_css_parser_t *parser,
                   const lxb_css_syntax_token_t *token, void *ctx);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_STATE_H */
