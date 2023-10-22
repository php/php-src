/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_SHS_H
#define LEXBOR_SHS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "lexbor/core/base.h"


typedef struct {
    char   *key;
    void   *value;

    size_t key_len;
    size_t next;
}
lexbor_shs_entry_t;

typedef struct {
    uint32_t key;
    uint32_t value;

    uint32_t next;
}
lexbor_shs_hash_t;


LXB_API const lexbor_shs_entry_t *
lexbor_shs_entry_get_static(const lexbor_shs_entry_t *tree,
                            const lxb_char_t *key, size_t size);

LXB_API const lexbor_shs_entry_t *
lexbor_shs_entry_get_lower_static(const lexbor_shs_entry_t *root,
                                  const lxb_char_t *key, size_t key_len);

LXB_API const lexbor_shs_entry_t *
lexbor_shs_entry_get_upper_static(const lexbor_shs_entry_t *root,
                                  const lxb_char_t *key, size_t key_len);

/*
 * Inline functions
 */
lxb_inline const lexbor_shs_hash_t *
lexbor_shs_hash_get_static(const lexbor_shs_hash_t *table,
                           const size_t table_size, const uint32_t key)
{
    const lexbor_shs_hash_t *entry;

    entry = &table[ (key % table_size) + 1 ];

    do {
        if (entry->key == key) {
            return entry;
        }

        entry = &table[entry->next];
    }
    while (entry != table);

    return NULL;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_SHS_H */





