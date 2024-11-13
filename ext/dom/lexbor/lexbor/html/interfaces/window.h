/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_WINDOW_H
#define LEXBOR_HTML_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/interface.h"
#include "lexbor/dom/interfaces/event_target.h"


struct lxb_html_window {
    lxb_dom_event_target_t event_target;
};


LXB_API lxb_html_window_t *
lxb_html_window_create(lxb_html_document_t *document);

LXB_API lxb_html_window_t *
lxb_html_window_destroy(lxb_html_window_t *window);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_WINDOW_H */
