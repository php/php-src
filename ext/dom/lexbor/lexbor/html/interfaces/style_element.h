/*
 * Copyright (C) 2018-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_STYLE_ELEMENT_H
#define LEXBOR_HTML_STYLE_ELEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/interface.h"
#include "lexbor/html/interfaces/element.h"


struct lxb_html_style_element {
    lxb_html_element_t   element;
    lxb_css_stylesheet_t *stylesheet;
};


LXB_API lxb_html_style_element_t *
lxb_html_style_element_interface_create(lxb_html_document_t *document);

LXB_API lxb_html_style_element_t *
lxb_html_style_element_interface_destroy(lxb_html_style_element_t *style_element);

LXB_API lxb_status_t
lxb_html_style_element_parse(lxb_html_style_element_t *element);

LXB_API lxb_status_t
lxb_html_element_style_remove(lxb_html_style_element_t *style);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_STYLE_ELEMENT_H */
