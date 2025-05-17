/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/shadow_root.h"
#include "lexbor/dom/interfaces/document.h"


lxb_dom_shadow_root_t *
lxb_dom_shadow_root_interface_create(lxb_dom_document_t *document)
{
    lxb_dom_shadow_root_t *element;

    element = lexbor_mraw_calloc(document->mraw,
                                 sizeof(lxb_dom_shadow_root_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_dom_document_owner(document);
    node->type = LXB_DOM_NODE_TYPE_UNDEF;

    return element;
}

lxb_dom_shadow_root_t *
lxb_dom_shadow_root_interface_destroy(lxb_dom_shadow_root_t *shadow_root)
{
    (void) lxb_dom_document_fragment_interface_destroy(
                        lxb_dom_interface_document_fragment(shadow_root));
    return NULL;
}
