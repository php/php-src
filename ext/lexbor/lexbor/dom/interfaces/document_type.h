/*
 * Copyright (C) 2018-2025 Alexander Borisov
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
 * Create DocumentType by specification.
 *
 * https://dom.spec.whatwg.org/#dom-domimplementation-createdocumenttype
 *
 * @param[in] lxb_dom_document_t *. Not NULL.
 * @param[in] const lxb_char_t *. Name. May be NULL, but then the return value
 *            will be NULL and an exception code will be recorded.
 * @param[in] size_t. Length of name. May be 0, but then the return value
 *            will be NULL and an exception code will be recorded.
 * @param[in] const lxb_char_t *. PublicID. Can be NULL.
 * @param[in] size_t. Length of PublicID. Can be 0.
 * @param[in] const lxb_char_t *. SystemID. Can be NULL.
 * @param[in] size_t. Length of SystemID. Can be 0.
 * @param[out] lxb_dom_exception_code_t. Can be NULL. If the variable is passed,
 *             the code will definitely be assigned. LXB_DOM_EXCEPTION_OK
 *             if successful.
 *
 * @return lxb_dom_document_type_t * if successful, otherwise NULL.
 */
LXB_API lxb_dom_document_type_t *
lxb_dom_document_type_create(lxb_dom_document_t *document,
                             const lxb_char_t *name, size_t name_len,
                             const lxb_char_t *pub, size_t pub_len,
                             const lxb_char_t *sys, size_t sys_len,
                             lxb_dom_exception_code_t *code);
LXB_API bool
lxb_dom_document_type_valid_name(const lxb_char_t *ame, size_t name_len);

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
