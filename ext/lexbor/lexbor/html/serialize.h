/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_SERIALIZE_H
#define LEXBOR_HTML_SERIALIZE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/str.h"
#include "lexbor/dom/interfaces/element.h"

#include "lexbor/html/base.h"


typedef int lxb_html_serialize_opt_t;

enum lxb_html_serialize_opt {
    LXB_HTML_SERIALIZE_OPT_UNDEF               = 0x00,
    LXB_HTML_SERIALIZE_OPT_SKIP_WS_NODES       = 0x01,
    LXB_HTML_SERIALIZE_OPT_SKIP_COMMENT        = 0x02,
    LXB_HTML_SERIALIZE_OPT_RAW                 = 0x04,
    LXB_HTML_SERIALIZE_OPT_WITHOUT_CLOSING     = 0x08,
    LXB_HTML_SERIALIZE_OPT_TAG_WITH_NS         = 0x10,
    LXB_HTML_SERIALIZE_OPT_WITHOUT_TEXT_INDENT = 0x20,
    LXB_HTML_SERIALIZE_OPT_FULL_DOCTYPE        = 0x40
};


typedef lxb_status_t
(*lxb_html_serialize_cb_f)(const lxb_char_t *data, size_t len, void *ctx);


LXB_API lxb_status_t
lxb_html_serialize_cb(lxb_dom_node_t *node,
                      lxb_html_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_html_serialize_str(lxb_dom_node_t *node, lexbor_str_t *str);

LXB_API lxb_status_t
lxb_html_serialize_tree_cb(lxb_dom_node_t *node,
                           lxb_html_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_html_serialize_tree_str(lxb_dom_node_t *node, lexbor_str_t *str);

LXB_API lxb_status_t
lxb_html_serialize_deep_cb(lxb_dom_node_t *node,
                           lxb_html_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_html_serialize_deep_str(lxb_dom_node_t *node, lexbor_str_t *str);

LXB_API lxb_status_t
lxb_html_serialize_pretty_cb(lxb_dom_node_t *node,
                             lxb_html_serialize_opt_t opt, size_t indent,
                             lxb_html_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_html_serialize_pretty_str(lxb_dom_node_t *node,
                              lxb_html_serialize_opt_t opt, size_t indent,
                              lexbor_str_t *str);

LXB_API lxb_status_t
lxb_html_serialize_pretty_tree_cb(lxb_dom_node_t *node,
                                  lxb_html_serialize_opt_t opt, size_t indent,
                                  lxb_html_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_html_serialize_pretty_tree_str(lxb_dom_node_t *node,
                                   lxb_html_serialize_opt_t opt, size_t indent,
                                   lexbor_str_t *str);

LXB_API lxb_status_t
lxb_html_serialize_pretty_deep_cb(lxb_dom_node_t *node,
                                  lxb_html_serialize_opt_t opt, size_t indent,
                                  lxb_html_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_html_serialize_pretty_deep_str(lxb_dom_node_t *node,
                                   lxb_html_serialize_opt_t opt, size_t indent,
                                   lexbor_str_t *str);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_SERIALIZE_H */
