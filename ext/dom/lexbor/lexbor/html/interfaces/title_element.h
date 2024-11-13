/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TITLE_ELEMENT_H
#define LEXBOR_HTML_TITLE_ELEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/interface.h"
#include "lexbor/html/interfaces/element.h"


struct lxb_html_title_element {
    lxb_html_element_t element;

    lexbor_str_t       *strict_text;
};


LXB_API lxb_html_title_element_t *
lxb_html_title_element_interface_create(lxb_html_document_t *document);

LXB_API lxb_html_title_element_t *
lxb_html_title_element_interface_destroy(lxb_html_title_element_t *title_element);

LXB_API const lxb_char_t *
lxb_html_title_element_text(lxb_html_title_element_t *title, size_t *len);

LXB_API const lxb_char_t *
lxb_html_title_element_strict_text(lxb_html_title_element_t *title, size_t *len);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TITLE_ELEMENT_H */
