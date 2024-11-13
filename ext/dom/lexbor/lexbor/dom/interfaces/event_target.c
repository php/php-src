/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/event_target.h"
#include "lexbor/dom/interfaces/document.h"


lxb_dom_event_target_t *
lxb_dom_event_target_create(lxb_dom_document_t *document)
{
    lxb_dom_event_target_t *element;

    element = lexbor_mraw_calloc(document->mraw,
                                 sizeof(lxb_dom_event_target_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_interface_node(element)->type = LXB_DOM_NODE_TYPE_UNDEF;

    return element;
}

lxb_dom_event_target_t *
lxb_dom_event_target_destroy(lxb_dom_event_target_t *event_target,
                             lxb_dom_document_t *document)
{
    return lexbor_mraw_free(document->mraw, event_target);
}
