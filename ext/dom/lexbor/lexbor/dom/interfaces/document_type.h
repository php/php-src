/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_DOCUMENT_TYPE_H
#define LEXBOR_DOM_DOCUMENT_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/str.h"

#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/node.h"
#include "lexbor/dom/interfaces/attr.h"
#include "lexbor/dom/interfaces/document_type.h"


struct lxb_dom_document_type {
    lxb_dom_node_t    node;

    lxb_dom_attr_id_t name;
    lexbor_str_t      public_id;
    lexbor_str_t      system_id;
};


LXB_API lxb_dom_document_type_t *
lxb_dom_document_type_interface_create(lxb_dom_document_t *document);

LXB_API lxb_dom_document_type_t *
lxb_dom_document_type_interface_clone(lxb_dom_document_t *document,
                                      const lxb_dom_document_type_t *dtype);

LXB_API lxb_dom_document_type_t *
lxb_dom_document_type_interface_destroy(lxb_dom_document_type_t *document_type);


/*
 * Inline functions
 */
lxb_inline const lxb_char_t *
lxb_dom_document_type_name(lxb_dom_document_type_t *doc_type, size_t *len)
{
    const lxb_dom_attr_data_t *data;

    static const lxb_char_t lxb_empty[] = "";

    data = lxb_dom_attr_data_by_id(doc_type->node.owner_document->attrs,
                                   doc_type->name);
    if (data == NULL || doc_type->name == LXB_DOM_ATTR__UNDEF) {
        if (len != NULL) {
            *len = 0;
        }

        return lxb_empty;
    }

    if (len != NULL) {
        *len = data->entry.length;
    }

    return lexbor_hash_entry_str(&data->entry);
}

lxb_inline const lxb_char_t *
lxb_dom_document_type_public_id(lxb_dom_document_type_t *doc_type, size_t *len)
{
    if (len != NULL) {
        *len = doc_type->public_id.length;
    }

    return doc_type->public_id.data;
}

lxb_inline const lxb_char_t *
lxb_dom_document_type_system_id(lxb_dom_document_type_t *doc_type, size_t *len)
{
    if (len != NULL) {
        *len = doc_type->system_id.length;
    }

    return doc_type->system_id.data;
}

/*
 * No inline functions for ABI.
 */
LXB_API const lxb_char_t *
lxb_dom_document_type_name_noi(lxb_dom_document_type_t *doc_type, size_t *len);

LXB_API const lxb_char_t *
lxb_dom_document_type_public_id_noi(lxb_dom_document_type_t *doc_type,
                                    size_t *len);

LXB_API const lxb_char_t *
lxb_dom_document_type_system_id_noi(lxb_dom_document_type_t *doc_type,
                                    size_t *len);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_DOCUMENT_TYPE_H */
