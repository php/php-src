/*
 * Copyright (C) 2018-2025 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/document_type.h"
#include "lexbor/dom/interfaces/document.h"


LXB_API lxb_dom_attr_data_t *
lxb_dom_attr_qualified_name_append(lexbor_hash_t *hash, const lxb_char_t *name,
                                   size_t length);

LXB_API lxb_dom_attr_data_t *
lxb_dom_attr_local_name_append(lexbor_hash_t *hash,
                               const lxb_char_t *name, size_t length);


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

    element->name = LXB_DOM_ATTR_HTML;

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

lxb_dom_document_type_t *
lxb_dom_document_type_create(lxb_dom_document_t *document,
                             const lxb_char_t *name, size_t name_len,
                             const lxb_char_t *pub, size_t pub_len,
                             const lxb_char_t *sys, size_t sys_len,
                             lxb_dom_exception_code_t *code)
{
    lxb_dom_attr_data_t *data;
    lxb_dom_document_type_t *doctype;

    if (!lxb_dom_document_type_valid_name(name, name_len)) {
        if (code != NULL) {
            *code = LXB_DOM_EXCEPTION_INVALID_CHARACTER_ERR;
        }

        return NULL;
    }

    doctype = lxb_dom_document_type_interface_create(document);
    if (doctype == NULL) {
        goto failed;
    }

    data = lxb_dom_attr_local_name_append(document->attrs, name, name_len);
    if (data == NULL) {
        goto failed;
    }

    doctype->name = data->attr_id;

    if (pub != NULL && pub_len != 0) {
        doctype->public_id.data = lxb_dom_document_create_text(document,
                                                               pub_len + 1);
        if (doctype->public_id.data == NULL) {
            goto failed;
        }

        (void) lexbor_str_copy_to_with_null(&doctype->public_id, pub, pub_len);
    }

    if (sys != NULL && sys_len != 0) {
        doctype->system_id.data = lxb_dom_document_create_text(document,
                                                               sys_len + 1);
        if (doctype->system_id.data == NULL) {
            goto failed;
        }

        (void) lexbor_str_copy_to_with_null(&doctype->system_id, sys, sys_len);
    }

    if (code != NULL) {
        *code = LXB_DOM_EXCEPTION_OK;
    }

    return doctype;

failed:

    if (doctype != NULL && doctype->public_id.data != NULL) {
        lxb_dom_document_destroy_text(document, doctype->public_id.data);
    }

    if (code != NULL) {
        *code = LXB_DOM_EXCEPTION_ERR;
    }

    return NULL;
}

bool
lxb_dom_document_type_valid_name(const lxb_char_t *name, size_t length)
{
    lxb_char_t c;
    const lxb_char_t *end;

    if (name == NULL || length == 0) {
        return false;
    }

    end = name + length;

    while (name < end) {
        c = *name++;
        /*
         * U+0009 TAB, U+000A LF, U+000C FF, U+000D CR, or U+0020 SPACE,
         * or U+0000 NULL, or U+003E (>)
         */
        if (c == 0x09 || c == 0x0A || c == 0x0C || c == 0x0D || c == 0x20
            || c == 0x00 || c == 0x3E)
        {
            return false;
        }
    }

    return true;
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
