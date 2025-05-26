/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_DOCUMENT_FRAGMENT_H
#define LEXBOR_DOM_DOCUMENT_FRAGMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/node.h"


struct lxb_dom_document_fragment {
    lxb_dom_node_t    node;

    lxb_dom_element_t *host;
};


LXB_API lxb_dom_document_fragment_t *
lxb_dom_document_fragment_interface_create(lxb_dom_document_t *document);

LXB_API lxb_dom_document_fragment_t *
lxb_dom_document_fragment_interface_destroy(lxb_dom_document_fragment_t *document_fragment);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_DOCUMENT_FRAGMENT_H */
