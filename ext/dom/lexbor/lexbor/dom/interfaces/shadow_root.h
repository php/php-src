/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_SHADOW_ROOT_H
#define LEXBOR_DOM_SHADOW_ROOT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/element.h"
#include "lexbor/dom/interfaces/document_fragment.h"


typedef enum {
    LXB_DOM_SHADOW_ROOT_MODE_OPEN   = 0x00,
    LXB_DOM_SHADOW_ROOT_MODE_CLOSED = 0x01
}
lxb_dom_shadow_root_mode_t;

struct lxb_dom_shadow_root {
    lxb_dom_document_fragment_t document_fragment;

    lxb_dom_shadow_root_mode_t  mode;
    lxb_dom_element_t           *host;
};


LXB_API lxb_dom_shadow_root_t *
lxb_dom_shadow_root_interface_create(lxb_dom_document_t *document);

LXB_API lxb_dom_shadow_root_t *
lxb_dom_shadow_root_interface_destroy(lxb_dom_shadow_root_t *shadow_root);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_SHADOW_ROOT_H */
