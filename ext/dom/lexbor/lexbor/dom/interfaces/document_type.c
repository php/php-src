/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/document_type.h"
#include "lexbor/dom/interfaces/document.h"


LXB_API lxb_dom_attr_data_t *
lxb_dom_attr_qualified_name_append(lexbor_hash_t *hash, const lxb_char_t *name,
                                   size_t length);


lxb_dom_document_type_t *
lxb_dom_document_type_interface_create(lxb_dom_document_t *document)
{
    lxb_dom_document_type_t *element;

    element = lexbor_mraw_calloc(document->mraw,
                                 sizeof(lxb_dom_document_type_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_dom_document_owner(document);
    node->type = LXB_DOM_NODE_TYPE_DOCUMENT_TYPE;

    return element;
}

lxb_dom_document_type_t *
lxb_dom_document_type_interface_clone(lxb_dom_document_t *document,
                                      const lxb_dom_document_type_t *dtype)
{
    lxb_status_t status;
    lxb_dom_document_type_t *new;
    const lxb_dom_attr_data_t *data;

    new = lxb_dom_document_type_interface_create(document);
    if (new == NULL) {
        return NULL;
    }

    status = lxb_dom_node_interface_copy(&new->node, &dtype->node, false);
    if (status != LXB_STATUS_OK) {
        return lxb_dom_document_type_interface_destroy(new);
    }

    if (document == dtype->node.owner_document) {
        new->name = dtype->name;
    }
    else {
        data = lxb_dom_attr_data_by_id(dtype->node.owner_document->attrs,
                                       dtype->name);
        if (data == NULL) {
            return lxb_dom_document_type_interface_destroy(new);
        }

        data = lxb_dom_attr_qualified_name_append(document->attrs,
                                                  lexbor_hash_entry_str(&data->entry),
                                                  data->entry.length);
        if (data == NULL) {
            return lxb_dom_document_type_interface_destroy(new);
        }

        new->name = (lxb_dom_attr_id_t) data;
    }

    if (lexbor_str_copy(&new->public_id,
                        &dtype->public_id, document->text) == NULL)
    {
        return lxb_dom_document_type_interface_destroy(new);
    }

    if (lexbor_str_copy(&new->system_id,
                        &dtype->system_id, document->text) == NULL)
    {
        return lxb_dom_document_type_interface_destroy(new);
    }

    return new;
}


lxb_dom_document_type_t *
lxb_dom_document_type_interface_destroy(lxb_dom_document_type_t *document_type)
{
    lexbor_mraw_t *text;
    lexbor_str_t public_id;
    lexbor_str_t system_id;

    text = lxb_dom_interface_node(document_type)->owner_document->text;
    public_id = document_type->public_id;
    system_id = document_type->system_id;

    (void) lxb_dom_node_interface_destroy(lxb_dom_interface_node(document_type));

    (void) lexbor_str_destroy(&public_id, text, false);
    (void) lexbor_str_destroy(&system_id, text, false);

    return NULL;
}

/*
 * No inline functions for ABI.
 */
const lxb_char_t *
lxb_dom_document_type_name_noi(lxb_dom_document_type_t *doc_type, size_t *len)
{
    return lxb_dom_document_type_name(doc_type, len);
}

const lxb_char_t *
lxb_dom_document_type_public_id_noi(lxb_dom_document_type_t *doc_type,
                                    size_t *len)
{
    return lxb_dom_document_type_public_id(doc_type, len);
}

const lxb_char_t *
lxb_dom_document_type_system_id_noi(lxb_dom_document_type_t *doc_type,
                                    size_t *len)
{
    return lxb_dom_document_type_system_id(doc_type, len);
}
