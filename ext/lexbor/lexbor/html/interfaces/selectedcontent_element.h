/*
 * Copyright (C) 2025 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_SELECTEDCONTENT_ELEMENT_H
#define LEXBOR_HTML_SELECTEDCONTENT_ELEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/interface.h"
#include "lexbor/html/interfaces/element.h"


struct lxb_html_selectedcontent_element {
    lxb_html_element_t element;
};


LXB_API lxb_html_selectedcontent_element_t *
lxb_html_selectedcontent_element_interface_create(lxb_html_document_t *document);

LXB_API lxb_html_selectedcontent_element_t *
lxb_html_selectedcontent_element_interface_destroy(lxb_html_selectedcontent_element_t *selectedcontent_element);

LXB_API lxb_dom_exception_code_t
lxb_html_selectedcontent_clone_option(lxb_html_selectedcontent_element_t *sc,
                                      lxb_html_option_element_t *option);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_SELECTEDCONTENT_ELEMENT_H */
