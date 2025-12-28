/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/shs.h"

#define LEXBOR_STR_RES_MAP_LOWERCASE
#include "lexbor/core/str_res.h"

#include "lexbor/ns/ns.h"
#include "lexbor/ns/res.h"


LXB_API const lxb_ns_data_t *
lxb_ns_append(lexbor_hash_t *hash, const lxb_char_t *link, size_t length)
{
    lxb_ns_data_t *data;
    const lexbor_shs_entry_t *entry;

    if (link == NULL || length == 0) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_lower_static(lxb_ns_res_shs_link_data,
                                              link, length);
    if (entry != NULL) {
        return entry->value;
    }

    data = lexbor_hash_insert(hash, lexbor_hash_insert_lower, link, length);
    if ((lxb_ns_id_t) data <= LXB_NS__LAST_ENTRY) {
        return NULL;
    }

    data->ns_id = (lxb_ns_id_t) data;

    return data;
}

const lxb_char_t *
lxb_ns_by_id(lexbor_hash_t *hash, lxb_ns_id_t ns_id, size_t *length)
{
    const lxb_ns_data_t *data;

    data = lxb_ns_data_by_id(hash, ns_id);
    if (data == NULL) {
        if (length != NULL) {
            *length = 0;
        }

        return NULL;
    }

    if (length != NULL) {
        *length = data->entry.length;
    }

    return lexbor_hash_entry_str(&data->entry);
}

const lxb_ns_data_t *
lxb_ns_data_by_id(lexbor_hash_t *hash, lxb_ns_id_t ns_id)
{
    if (ns_id >= LXB_NS__LAST_ENTRY) {
        if (ns_id == LXB_NS__LAST_ENTRY) {
            return NULL;
        }

        return (const lxb_ns_data_t *) ns_id;
    }

    return &lxb_ns_res_data[ns_id];
}

const lxb_ns_data_t *
lxb_ns_data_by_link(lexbor_hash_t *hash, const lxb_char_t *link, size_t length)
{
    const lexbor_shs_entry_t *entry;

    if (link == NULL || length == 0) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_lower_static(lxb_ns_res_shs_link_data,
                                              link, length);
    if (entry != NULL) {
        return entry->value;
    }

    return lexbor_hash_search(hash, lexbor_hash_search_lower, link, length);
}

/* Prefix */
const lxb_ns_prefix_data_t *
lxb_ns_prefix_append(lexbor_hash_t *hash,
                     const lxb_char_t *prefix, size_t length)
{
    lxb_ns_prefix_data_t *data;
    const lexbor_shs_entry_t *entry;

    if (prefix == NULL || length == 0) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_lower_static(lxb_ns_res_shs_data,
                                              prefix, length);
    if (entry != NULL) {
        return entry->value;
    }

    data = lexbor_hash_insert(hash, lexbor_hash_insert_lower, prefix, length);
    if ((lxb_ns_prefix_id_t) data <= LXB_NS__LAST_ENTRY) {
        return NULL;
    }

    data->prefix_id = (lxb_ns_prefix_id_t) data;

    return data;
}

const lxb_ns_prefix_data_t *
lxb_ns_prefix_data_by_id(lexbor_hash_t *hash, lxb_ns_prefix_id_t prefix_id)
{
    if (prefix_id >= LXB_NS__LAST_ENTRY) {
        if (prefix_id == LXB_NS__LAST_ENTRY) {
            return NULL;
        }

        return (const lxb_ns_prefix_data_t *) prefix_id;
    }

    return &lxb_ns_prefix_res_data[prefix_id];
}

const lxb_ns_prefix_data_t *
lxb_ns_prefix_data_by_name(lexbor_hash_t *hash,
                           const lxb_char_t *prefix, size_t length)
{
    const lexbor_shs_entry_t *entry;

    if (prefix == NULL || length == 0) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_lower_static(lxb_ns_res_shs_data,
                                              prefix, length);
    if (entry != NULL) {
        return entry->value;
    }

    return lexbor_hash_search(hash, lexbor_hash_search_lower, prefix, length);
}
