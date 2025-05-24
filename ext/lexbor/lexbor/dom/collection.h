/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_COLLECTION_H
#define LEXBOR_DOM_COLLECTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"
#include "lexbor/core/array.h"

#include "lexbor/dom/interface.h"


typedef struct {
    lexbor_array_t     array;
    lxb_dom_document_t *document;
}
lxb_dom_collection_t;


LXB_API lxb_dom_collection_t *
lxb_dom_collection_create(lxb_dom_document_t *document);

LXB_API lxb_status_t
lxb_dom_collection_init(lxb_dom_collection_t *col, size_t start_list_size);

LXB_API lxb_dom_collection_t *
lxb_dom_collection_destroy(lxb_dom_collection_t *col, bool self_destroy);


/*
 * Inline functions
 */
lxb_inline lxb_dom_collection_t *
lxb_dom_collection_make(lxb_dom_document_t *document, size_t start_list_size)
{
    lxb_status_t status;
    lxb_dom_collection_t *col;

    col = lxb_dom_collection_create(document);
    status = lxb_dom_collection_init(col, start_list_size);

    if(status != LXB_STATUS_OK) {
        return lxb_dom_collection_destroy(col, true);
    }

    return col;
}

lxb_inline void
lxb_dom_collection_clean(lxb_dom_collection_t *col)
{
    lexbor_array_clean(&col->array);
}

lxb_inline lxb_status_t
lxb_dom_collection_append(lxb_dom_collection_t *col, void *value)
{
    return lexbor_array_push(&col->array, value);
}

lxb_inline lxb_dom_element_t *
lxb_dom_collection_element(lxb_dom_collection_t *col, size_t idx)
{
    return (lxb_dom_element_t *) lexbor_array_get(&col->array, idx);
}

lxb_inline lxb_dom_node_t *
lxb_dom_collection_node(lxb_dom_collection_t *col, size_t idx)
{
    return (lxb_dom_node_t *) lexbor_array_get(&col->array, idx);
}

lxb_inline size_t
lxb_dom_collection_length(lxb_dom_collection_t *col)
{
    return lexbor_array_length(&col->array);
}

/*
 * No inline functions for ABI.
 */
LXB_API lxb_dom_collection_t *
lxb_dom_collection_make_noi(lxb_dom_document_t *document, size_t start_list_size);

LXB_API void
lxb_dom_collection_clean_noi(lxb_dom_collection_t *col);

LXB_API lxb_status_t
lxb_dom_collection_append_noi(lxb_dom_collection_t *col, void *value);

LXB_API lxb_dom_element_t *
lxb_dom_collection_element_noi(lxb_dom_collection_t *col, size_t idx);

LXB_API lxb_dom_node_t *
lxb_dom_collection_node_noi(lxb_dom_collection_t *col, size_t idx);

LXB_API size_t
lxb_dom_collection_length_noi(lxb_dom_collection_t *col);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_COLLECTION_H */
