/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/character_data.h"
#include "lexbor/dom/interfaces/document.h"


lxb_dom_character_data_t *
lxb_dom_character_data_interface_create(lxb_dom_document_t *document)
{
    lxb_dom_character_data_t *element;

    element = lexbor_mraw_calloc(document->mraw,
                                 sizeof(lxb_dom_character_data_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_dom_document_owner(document);
    node->type = LXB_DOM_NODE_TYPE_UNDEF;

    return element;
}

lxb_dom_character_data_t *
lxb_dom_character_data_interface_clone(lxb_dom_document_t *document,
                                       const lxb_dom_character_data_t *ch_data)
{
    lxb_dom_character_data_t *new;

    new = lxb_dom_character_data_interface_create(document);
    if (new == NULL) {
        return NULL;
    }

    if (lxb_dom_character_data_interface_copy(new, ch_data) != LXB_STATUS_OK) {
        return lxb_dom_character_data_interface_destroy(new);
    }

    return new;
}

lxb_dom_character_data_t *
lxb_dom_character_data_interface_destroy(lxb_dom_character_data_t *character_data)
{
    lxb_dom_node_t *node;
    lxb_dom_document_t *doc;
    lexbor_str_t data;

    data = character_data->data;
    node = lxb_dom_interface_node(character_data);
    doc = node->owner_document;

    (void) lxb_dom_node_interface_destroy(node);

    lexbor_str_destroy(&data, doc->text, false);

    return NULL;
}

lxb_status_t
lxb_dom_character_data_interface_copy(lxb_dom_character_data_t *dst,
                                      const lxb_dom_character_data_t *src)
{
    lxb_status_t status;

    status = lxb_dom_node_interface_copy(&dst->node, &src->node, false);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    dst->data.length = 0;

    if (lexbor_str_copy(&dst->data, &src->data,
                        lxb_dom_interface_node(dst)->owner_document->text) == NULL)
    {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_OK;
}

/* TODO: oh, need to... https://dom.spec.whatwg.org/#concept-cd-replace */
lxb_status_t
lxb_dom_character_data_replace(lxb_dom_character_data_t *ch_data,
                               const lxb_char_t *data, size_t len,
                               size_t offset, size_t count)
{
    if (ch_data->data.data == NULL) {
        lexbor_str_init(&ch_data->data, ch_data->node.owner_document->text, len);
        if (ch_data->data.data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }
    else if (lexbor_str_size(&ch_data->data) < len) {
        const lxb_char_t *data;

        data = lexbor_str_realloc(&ch_data->data,
                                  ch_data->node.owner_document->text, (len + 1));
        if (data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    memcpy(ch_data->data.data, data, sizeof(lxb_char_t) * len);

    ch_data->data.data[len] = 0x00;
    ch_data->data.length = len;

    return LXB_STATUS_OK;
}
