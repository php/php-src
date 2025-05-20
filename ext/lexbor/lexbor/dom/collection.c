/*
 * Copyright (C) 2018-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/collection.h"
#include "lexbor/dom/interfaces/document.h"


lxb_dom_collection_t *
lxb_dom_collection_create(lxb_dom_document_t *document)
{
    lxb_dom_collection_t *col;

    col = lexbor_calloc(1, sizeof(lxb_dom_collection_t));
    if (col == NULL) {
        return NULL;
    }

    col->document = document;

    return col;
}

lxb_status_t
lxb_dom_collection_init(lxb_dom_collection_t *col, size_t start_list_size)
{
    if (col == NULL) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    if (col->document == NULL) {
        return LXB_STATUS_ERROR_INCOMPLETE_OBJECT;
    }

    return lexbor_array_init(&col->array, start_list_size);
}

lxb_dom_collection_t *
lxb_dom_collection_destroy(lxb_dom_collection_t *col, bool self_destroy)
{
    if (col == NULL) {
        return NULL;
    }

    if (col->array.list != NULL) {
        lexbor_array_destroy(&col->array, false);

        col->array.list = NULL;
    }

    if (self_destroy) {
        return lexbor_free(col);
    }

    return col;
}

/*
 * No inline functions for ABI.
 */
lxb_dom_collection_t *
lxb_dom_collection_make_noi(lxb_dom_document_t *document, size_t start_list_size)
{
    return lxb_dom_collection_make(document, start_list_size);
}

void
lxb_dom_collection_clean_noi(lxb_dom_collection_t *col)
{
    lxb_dom_collection_clean(col);
}

lxb_status_t
lxb_dom_collection_append_noi(lxb_dom_collection_t *col, void *value)
{
    return lxb_dom_collection_append(col, value);
}

lxb_dom_element_t *
lxb_dom_collection_element_noi(lxb_dom_collection_t *col, size_t idx)
{
    return lxb_dom_collection_element(col, idx);
}

lxb_dom_node_t *
lxb_dom_collection_node_noi(lxb_dom_collection_t *col, size_t idx)
{
    return lxb_dom_collection_node(col, idx);
}

size_t
lxb_dom_collection_length_noi(lxb_dom_collection_t *col)
{
    return lxb_dom_collection_length(col);
}
