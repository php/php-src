/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_FRAME_SET_ELEMENT_H
#define LEXBOR_HTML_FRAME_SET_ELEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/interface.h"
#include "lexbor/html/interfaces/element.h"


struct lxb_html_frame_set_element {
    lxb_html_element_t element;
};


LXB_API lxb_html_frame_set_element_t *
lxb_html_frame_set_element_interface_create(lxb_html_document_t *document);

LXB_API lxb_html_frame_set_element_t *
lxb_html_frame_set_element_interface_destroy(lxb_html_frame_set_element_t *frame_set_element);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_FRAME_SET_ELEMENT_H */
