/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/tag/tag.h"

#include "lexbor/tag/res.h"


LXB_API const lxb_tag_data_t *
lxb_tag_append(lexbor_hash_t *hash, lxb_tag_id_t tag_id,
               const lxb_char_t *name, size_t length)
{
    lxb_tag_data_t *data;
    const lexbor_shs_entry_t *entry;

    entry = lexbor_shs_entry_get_static(lxb_tag_res_shs_data_default,
                                        name, length);
    if (entry != NULL) {
        return entry->value;
    }

    data = lexbor_hash_insert(hash, lexbor_hash_insert_raw, name, length);
    if (data == NULL) {
        return NULL;
    }

    if (tag_id == LXB_TAG__UNDEF) {
        data->tag_id = (lxb_tag_id_t) data;
    }
    else {
        data->tag_id = tag_id;
    }

    return data;
}

LXB_API const lxb_tag_data_t *
lxb_tag_append_lower(lexbor_hash_t *hash, const lxb_char_t *name, size_t length)
{
    lxb_tag_data_t *data;
    const lexbor_shs_entry_t *entry;

    entry = lexbor_shs_entry_get_lower_static(lxb_tag_res_shs_data_default,
                                              name, length);
    if (entry != NULL) {
        return entry->value;
    }

    data = lexbor_hash_insert(hash, lexbor_hash_insert_lower, name, length);
    if (data == NULL) {
        return NULL;
    }

    data->tag_id = (lxb_tag_id_t) data;

    return data;
}

const lxb_tag_data_t *
lxb_tag_data_by_id(lexbor_hash_t *hash, lxb_tag_id_t tag_id)
{
    if (tag_id >= LXB_TAG__LAST_ENTRY) {
        if (tag_id == LXB_TAG__LAST_ENTRY) {
            return NULL;
        }

        return (const lxb_tag_data_t *) tag_id;
    }

    return &lxb_tag_res_data_default[tag_id];
}

const lxb_tag_data_t *
lxb_tag_data_by_name(lexbor_hash_t *hash, const lxb_char_t *name, size_t len)
{
    const lexbor_shs_entry_t *entry;

    if (name == NULL || len == 0) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_lower_static(lxb_tag_res_shs_data_default,
                                              name, len);
    if (entry != NULL) {
        return (const lxb_tag_data_t *) entry->value;
    }

    return (const lxb_tag_data_t *) lexbor_hash_search(hash,
                                           lexbor_hash_search_lower, name, len);
}

const lxb_tag_data_t *
lxb_tag_data_by_name_upper(lexbor_hash_t *hash,
                           const lxb_char_t *name, size_t len)
{
    uintptr_t dif;
    const lexbor_shs_entry_t *entry;

    if (name == NULL || len == 0) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_upper_static(lxb_tag_res_shs_data_default,
                                              name, len);
    if (entry != NULL) {
        dif = (const lxb_tag_data_t *) entry->value - lxb_tag_res_data_default;

        return (const lxb_tag_data_t *) (lxb_tag_res_data_upper_default + dif);
    }

    return (const lxb_tag_data_t *) lexbor_hash_search(hash,
                                           lexbor_hash_search_upper, name, len);
}

/*
 * No inline functions for ABI.
 */
const lxb_char_t *
lxb_tag_name_by_id_noi(lexbor_hash_t *hash, lxb_tag_id_t tag_id, size_t *len)
{
    return lxb_tag_name_by_id(hash, tag_id, len);
}

const lxb_char_t *
lxb_tag_name_upper_by_id_noi(lexbor_hash_t *hash,
                             lxb_tag_id_t tag_id, size_t *len)
{
    return lxb_tag_name_upper_by_id(hash, tag_id, len);
}

lxb_tag_id_t
lxb_tag_id_by_name_noi(lexbor_hash_t *hash, const lxb_char_t *name, size_t len)
{
    return lxb_tag_id_by_name(hash, name, len);
}

lexbor_mraw_t *
lxb_tag_mraw_noi(lexbor_hash_t *hash)
{
    return lxb_tag_mraw(hash);
}
