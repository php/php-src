/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_TEXT_H
#define LEXBOR_DOM_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/character_data.h"


struct lxb_dom_text {
    lxb_dom_character_data_t char_data;
};


LXB_API lxb_dom_text_t *
lxb_dom_text_interface_create(lxb_dom_document_t *document);

LXB_API lxb_dom_text_t *
lxb_dom_text_interface_clone(lxb_dom_document_t *document,
                             const lxb_dom_text_t *text);

LXB_API lxb_dom_text_t *
lxb_dom_text_interface_destroy(lxb_dom_text_t *text);


LXB_API lxb_status_t
lxb_dom_text_interface_copy(lxb_dom_text_t *dst, const lxb_dom_text_t *src);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_TEXT_H */
