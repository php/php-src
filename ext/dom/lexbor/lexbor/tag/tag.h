/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_TAG_H
#define LEXBOR_TAG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/hash.h"
#include "lexbor/core/shs.h"
#include "lexbor/core/dobject.h"
#include "lexbor/core/str.h"

#include "lexbor/tag/const.h"


typedef struct {
    lexbor_hash_entry_t entry;
    lxb_tag_id_t        tag_id;
    size_t              ref_count;
    bool                read_only;
}
lxb_tag_data_t;


LXB_API const lxb_tag_data_t *
lxb_tag_data_by_id(lexbor_hash_t *hash, lxb_tag_id_t tag_id);

LXB_API const lxb_tag_data_t *
lxb_tag_data_by_name(lexbor_hash_t *hash, const lxb_char_t *name, size_t len);

LXB_API const lxb_tag_data_t *
lxb_tag_data_by_name_upper(lexbor_hash_t *hash,
                           const lxb_char_t *name, size_t len);

/*
 * Inline functions
 */
lxb_inline const lxb_char_t *
lxb_tag_name_by_id(lexbor_hash_t *hash, lxb_tag_id_t tag_id, size_t *len)
{
    const lxb_tag_data_t *data = lxb_tag_data_by_id(hash, tag_id);
    if (data == NULL) {
        if (len != NULL) {
            *len = 0;
        }

        return NULL;
    }

    if (len != NULL) {
        *len = data->entry.length;
    }

    return lexbor_hash_entry_str(&data->entry);
}

lxb_inline const lxb_char_t *
lxb_tag_name_upper_by_id(lexbor_hash_t *hash, lxb_tag_id_t tag_id, size_t *len)
{
    const lxb_tag_data_t *data = lxb_tag_data_by_id(hash, tag_id);
    if (data == NULL) {
        if (len != NULL) {
            *len = 0;
        }

        return NULL;
    }

    if (len != NULL) {
        *len = data->entry.length;
    }

    return lexbor_hash_entry_str(&data->entry);
}

lxb_inline lxb_tag_id_t
lxb_tag_id_by_name(lexbor_hash_t *hash, const lxb_char_t *name, size_t len)
{
    const lxb_tag_data_t *data = lxb_tag_data_by_name(hash, name, len);
    if (data == NULL) {
        return LXB_TAG__UNDEF;
    }

    return data->tag_id;
}

lxb_inline lexbor_mraw_t *
lxb_tag_mraw(lexbor_hash_t *hash)
{
    return lexbor_hash_mraw(hash);
}


/*
 * No inline functions for ABI.
 */
LXB_API const lxb_char_t *
lxb_tag_name_by_id_noi(lexbor_hash_t *hash, lxb_tag_id_t tag_id,
                       size_t *len);

LXB_API const lxb_char_t *
lxb_tag_name_upper_by_id_noi(lexbor_hash_t *hash,
                             lxb_tag_id_t tag_id, size_t *len);

LXB_API lxb_tag_id_t
lxb_tag_id_by_name_noi(lexbor_hash_t *hash,
                       const lxb_char_t *name, size_t len);

LXB_API lexbor_mraw_t *
lxb_tag_mraw_noi(lexbor_hash_t *hash);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_TAG_H */
