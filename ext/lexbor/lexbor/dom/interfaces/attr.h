/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_ATTR_H
#define LEXBOR_DOM_ATTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/hash.h"
#include "lexbor/core/str.h"

#include "lexbor/ns/ns.h"

#include "lexbor/dom/interface.h"
#include "lexbor/dom/interfaces/node.h"
#include "lexbor/dom/interfaces/attr_const.h"
#include "lexbor/dom/interfaces/document.h"


typedef struct {
    lexbor_hash_entry_t  entry;
    lxb_dom_attr_id_t    attr_id;
    size_t               ref_count;
    bool                 read_only;
}
lxb_dom_attr_data_t;

/* More memory to God of memory! */
struct lxb_dom_attr {
    lxb_dom_node_t     node;

    /* For example: <LalAla:DiV Fix:Me="value"> */

    lxb_dom_attr_id_t  upper_name;     /* uppercase, with prefix: FIX:ME */
    lxb_dom_attr_id_t  qualified_name; /* original, with prefix: Fix:Me */

    lexbor_str_t       *value;

    lxb_dom_element_t  *owner;

    lxb_dom_attr_t     *next;
    lxb_dom_attr_t     *prev;
};


LXB_API lxb_dom_attr_t *
lxb_dom_attr_interface_create(lxb_dom_document_t *document);

LXB_API lxb_dom_attr_t *
lxb_dom_attr_interface_clone(lxb_dom_document_t *document,
                             const lxb_dom_attr_t *attr);

LXB_API lxb_dom_attr_t *
lxb_dom_attr_interface_destroy(lxb_dom_attr_t *attr);

LXB_API lxb_status_t
lxb_dom_attr_set_name(lxb_dom_attr_t *attr, const lxb_char_t *local_name,
                      size_t local_name_len, bool to_lowercase);

LXB_API lxb_status_t
lxb_dom_attr_set_value(lxb_dom_attr_t *attr,
                       const lxb_char_t *value, size_t value_len);

LXB_API lxb_status_t
lxb_dom_attr_set_value_wo_copy(lxb_dom_attr_t *attr,
                               lxb_char_t *value, size_t value_len);

LXB_API lxb_status_t
lxb_dom_attr_set_existing_value(lxb_dom_attr_t *attr,
                                const lxb_char_t *value, size_t value_len);

LXB_API lxb_status_t
lxb_dom_attr_clone_name_value(lxb_dom_attr_t *attr_from,
                              lxb_dom_attr_t *attr_to);

LXB_API bool
lxb_dom_attr_compare(lxb_dom_attr_t *first, lxb_dom_attr_t *second);

LXB_API void
lxb_dom_attr_remove(lxb_dom_attr_t *attr);

LXB_API const lxb_dom_attr_data_t *
lxb_dom_attr_data_undef(void);

LXB_API const lxb_dom_attr_data_t *
lxb_dom_attr_data_by_id(lexbor_hash_t *hash, lxb_dom_attr_id_t attr_id);

LXB_API const lxb_dom_attr_data_t *
lxb_dom_attr_data_by_local_name(lexbor_hash_t *hash,
                                const lxb_char_t *name, size_t length);

LXB_API const lxb_dom_attr_data_t *
lxb_dom_attr_data_by_qualified_name(lexbor_hash_t *hash,
                                    const lxb_char_t *name, size_t length);

LXB_API const lxb_char_t *
lxb_dom_attr_qualified_name(lxb_dom_attr_t *attr, size_t *len);


/*
 * Inline functions
 */
lxb_inline const lxb_char_t *
lxb_dom_attr_local_name(lxb_dom_attr_t *attr, size_t *len)
{
    const lxb_dom_attr_data_t *data;

    data = lxb_dom_attr_data_by_id(attr->node.owner_document->attrs,
                                   attr->node.local_name);

    if (len != NULL) {
        *len = data->entry.length;
    }

    return lexbor_hash_entry_str(&data->entry);
}

lxb_inline const lxb_char_t *
lxb_dom_attr_value(lxb_dom_attr_t *attr, size_t *len)
{
    if (attr->value == NULL) {
        if (len != NULL) {
            *len = 0;
        }

        return NULL;
    }

    if (len != NULL) {
        *len = attr->value->length;
    }

    return attr->value->data;
}

/*
 * No inline functions for ABI.
 */
LXB_API const lxb_char_t *
lxb_dom_attr_local_name_noi(lxb_dom_attr_t *attr, size_t *len);

LXB_API const lxb_char_t *
lxb_dom_attr_value_noi(lxb_dom_attr_t *attr, size_t *len);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_ATTR_H */
