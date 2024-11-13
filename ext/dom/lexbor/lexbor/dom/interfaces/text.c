/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/text.h"
#include "lexbor/dom/interfaces/document.h"


lxb_dom_text_t *
lxb_dom_text_interface_create(lxb_dom_document_t *document)
{
    lxb_dom_text_t *element;

    element = lexbor_mraw_calloc(document->mraw,
                                 sizeof(lxb_dom_text_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_dom_document_owner(document);
    node->type = LXB_DOM_NODE_TYPE_TEXT;

    return element;
}

lxb_dom_text_t *
lxb_dom_text_interface_clone(lxb_dom_document_t *document,
                             const lxb_dom_text_t *text)
{
    lxb_status_t status;
    lxb_dom_text_t *new;

    new = lxb_dom_text_interface_create(document);
    if (new == NULL) {
        return NULL;
    }

    status = lxb_dom_text_interface_copy(new, text);
    if (status != LXB_STATUS_OK) {
        return lxb_dom_text_interface_destroy(new);
    }

    return new;
}

lxb_dom_text_t *
lxb_dom_text_interface_destroy(lxb_dom_text_t *text)
{
    (void) lxb_dom_character_data_interface_destroy(
                                        lxb_dom_interface_character_data(text));
    return NULL;
}

lxb_status_t
lxb_dom_text_interface_copy(lxb_dom_text_t *dst, const lxb_dom_text_t *src)
{
    return lxb_dom_character_data_interface_copy(&dst->char_data,
                                                 &src->char_data);
}
