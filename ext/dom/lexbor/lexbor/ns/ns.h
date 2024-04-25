/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_NS_H
#define LEXBOR_NS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/hash.h"
#include "lexbor/core/shs.h"

#include "lexbor/ns/const.h"


typedef struct {
    lexbor_hash_entry_t  entry;

    lxb_ns_id_t          ns_id;
    size_t               ref_count;
    bool                 read_only;
}
lxb_ns_data_t;

typedef struct {
    lexbor_hash_entry_t  entry;

    lxb_ns_prefix_id_t   prefix_id;
    size_t               ref_count;
    bool                 read_only;
}
lxb_ns_prefix_data_t;


/* Link */
LXB_API const lxb_char_t *
lxb_ns_by_id(lexbor_hash_t *hash, lxb_ns_id_t ns_id, size_t *length);

LXB_API const lxb_ns_data_t *
lxb_ns_data_by_id(lexbor_hash_t *hash, lxb_ns_id_t ns_id);

LXB_API const lxb_ns_data_t *
lxb_ns_data_by_link(lexbor_hash_t *hash, const lxb_char_t *name, size_t length);

/* Prefix */
LXB_API const lxb_ns_prefix_data_t *
lxb_ns_prefix_append(lexbor_hash_t *hash,
                     const lxb_char_t *prefix, size_t length);

LXB_API const lxb_ns_prefix_data_t *
lxb_ns_prefix_data_by_id(lexbor_hash_t *hash, lxb_ns_prefix_id_t prefix_id);

LXB_API const lxb_ns_prefix_data_t *
lxb_ns_prefix_data_by_name(lexbor_hash_t *hash,
                           const lxb_char_t *name, size_t length);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_NS_H */
