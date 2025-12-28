/*
 * Copyright (C) 2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#define LEXBOR_HASH_EXTERN
#include "lexbor/core/hash.h"
#undef LEXBOR_HASH_EXTERN

#include "lexbor/core/str.h"

#define LEXBOR_STR_RES_MAP_LOWERCASE
#define LEXBOR_STR_RES_MAP_UPPERCASE
#include "lexbor/core/str_res.h"


/* Insert variable. */
const lexbor_hash_insert_t lexbor_hash_insert_var = {
    .hash = lexbor_hash_make_id,
    .copy = lexbor_hash_copy,
    .cmp = lexbor_str_data_ncmp
};

const lexbor_hash_insert_t lexbor_hash_insert_lower_var = {
    .hash = lexbor_hash_make_id_lower,
    .copy = lexbor_hash_copy_lower,
    .cmp = lexbor_str_data_nlocmp_right
};

const lexbor_hash_insert_t lexbor_hash_insert_upper_var = {
    .hash = lexbor_hash_make_id_upper,
    .copy = lexbor_hash_copy_upper,
    .cmp = lexbor_str_data_nupcmp_right
};

LXB_API const lexbor_hash_insert_t
*lexbor_hash_insert_raw = &lexbor_hash_insert_var;

LXB_API const lexbor_hash_insert_t
*lexbor_hash_insert_lower = &lexbor_hash_insert_lower_var;

LXB_API const lexbor_hash_insert_t
*lexbor_hash_insert_upper = &lexbor_hash_insert_upper_var;

/* Search variable. */
const lexbor_hash_search_t lexbor_hash_search_var = {
    .hash = lexbor_hash_make_id,
    .cmp = lexbor_str_data_ncmp
};

const lexbor_hash_search_t lexbor_hash_search_lower_var = {
    .hash = lexbor_hash_make_id_lower,
    .cmp = lexbor_str_data_nlocmp_right
};

const lexbor_hash_search_t lexbor_hash_search_upper_var = {
    .hash = lexbor_hash_make_id_upper,
    .cmp = lexbor_str_data_nupcmp_right
};

LXB_API const lexbor_hash_search_t
*lexbor_hash_search_raw = &lexbor_hash_search_var;

LXB_API const lexbor_hash_search_t
*lexbor_hash_search_lower = &lexbor_hash_search_lower_var;

LXB_API const lexbor_hash_search_t
*lexbor_hash_search_upper = &lexbor_hash_search_upper_var;


lxb_inline lexbor_hash_entry_t **
lexbor_hash_table_create(lexbor_hash_t *hash)
{
    return lexbor_calloc(hash->table_size, sizeof(lexbor_hash_entry_t *));
}

lxb_inline void
lexbor_hash_table_clean(lexbor_hash_t *hash)
{
    memset(hash->table, 0, sizeof(lexbor_hash_t *) * hash->table_size);
}

lxb_inline lexbor_hash_entry_t **
lexbor_hash_table_destroy(lexbor_hash_t *hash)
{
    if (hash->table != NULL) {
        return lexbor_free(hash->table);
    }

    return NULL;
}

lxb_inline lexbor_hash_entry_t *
_lexbor_hash_entry_create(lexbor_hash_t *hash, const lexbor_hash_copy_f copy_func,
                          const lxb_char_t *key, size_t length)
{
    lexbor_hash_entry_t *entry = lexbor_dobject_calloc(hash->entries);
    if (entry == NULL) {
        return NULL;
    }

    entry->length = length;

    if (copy_func(hash, entry, key, length) != LXB_STATUS_OK) {
        lexbor_dobject_free(hash->entries, entry);
        return NULL;
    }

    return entry;
}

lexbor_hash_t *
lexbor_hash_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_hash_t));
}

lxb_status_t
lexbor_hash_init(lexbor_hash_t *hash, size_t table_size, size_t struct_size)
{
    lxb_status_t status;
    size_t chunk_size;

    if (hash == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (table_size < LEXBOR_HASH_TABLE_MIN_SIZE) {
        table_size = LEXBOR_HASH_TABLE_MIN_SIZE;
    }

    chunk_size = table_size / 2;

    hash->table_size = table_size;

    hash->entries = lexbor_dobject_create();
    status = lexbor_dobject_init(hash->entries, chunk_size, struct_size);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    hash->mraw = lexbor_mraw_create();
    status = lexbor_mraw_init(hash->mraw, chunk_size * 12);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    hash->table = lexbor_hash_table_create(hash);
    if (hash->table == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    hash->struct_size = struct_size;

    return LXB_STATUS_OK;
}

void
lexbor_hash_clean(lexbor_hash_t *hash)
{
    lexbor_dobject_clean(hash->entries);
    lexbor_mraw_clean(hash->mraw);
    lexbor_hash_table_clean(hash);
}

lexbor_hash_t *
lexbor_hash_destroy(lexbor_hash_t *hash, bool destroy_obj)
{
    if (hash == NULL) {
        return NULL;
    }

    hash->entries = lexbor_dobject_destroy(hash->entries, true);
    hash->mraw = lexbor_mraw_destroy(hash->mraw, true);
    hash->table = lexbor_hash_table_destroy(hash);

    if (destroy_obj) {
        return lexbor_free(hash);
    }

    return hash;
}

void *
lexbor_hash_insert(lexbor_hash_t *hash, const lexbor_hash_insert_t *insert,
                   const lxb_char_t *key, size_t length)
{
    lxb_char_t *str;
    uint32_t hash_id, table_idx;
    lexbor_hash_entry_t *entry;

    hash_id = insert->hash(key, length);
    table_idx = hash_id % hash->table_size;

    entry = hash->table[table_idx];

    if (entry == NULL) {
        entry = _lexbor_hash_entry_create(hash, insert->copy, key, length);
        hash->table[table_idx] = entry;

        return entry;
    }

    do {
        str = lexbor_hash_entry_str(entry);

        if (entry->length == length && insert->cmp(str, key, length)) {
            return entry;
        }

        if (entry->next == NULL) {
            break;
        }

        entry = entry->next;
    }
    while (1);

    entry->next = _lexbor_hash_entry_create(hash, insert->copy, key, length);

    return entry->next;
}

void *
lexbor_hash_insert_by_entry(lexbor_hash_t *hash, lexbor_hash_entry_t *entry,
                            const lexbor_hash_search_t *search,
                            const lxb_char_t *key, size_t length)
{
    lxb_char_t *str;
    uint32_t hash_id, table_idx;
    lexbor_hash_entry_t *item;

    hash_id = search->hash(key, length);
    table_idx = hash_id % hash->table_size;

    item = hash->table[table_idx];

    if (item == NULL) {
        hash->table[table_idx] = entry;

        return entry;
    }

    do {
        str = lexbor_hash_entry_str(item);

        if (item->length == length && search->cmp(str, key, length)) {
            return item;
        }

        if (item->next == NULL) {
            break;
        }

        item = item->next;
    }
    while (1);

    item->next = entry;

    return entry;
}

void
lexbor_hash_remove(lexbor_hash_t *hash, const lexbor_hash_search_t *search,
                   const lxb_char_t *key, size_t length)
{
    lexbor_hash_remove_by_hash_id(hash, search->hash(key, length),
                                  key, length, search->cmp);
}

void *
lexbor_hash_search(lexbor_hash_t *hash, const lexbor_hash_search_t *search,
                   const lxb_char_t *key, size_t length)
{
    return lexbor_hash_search_by_hash_id(hash, search->hash(key, length),
                                         key, length, search->cmp);
}

void
lexbor_hash_remove_by_hash_id(lexbor_hash_t *hash, uint32_t hash_id,
                              const lxb_char_t *key, size_t length,
                              const lexbor_hash_cmp_f cmp_func)
{
    uint32_t table_idx;
    lxb_char_t *str;
    lexbor_hash_entry_t *entry, *prev;

    table_idx = hash_id % hash->table_size;
    entry = hash->table[table_idx];
    prev = NULL;

    while (entry != NULL) {
        str = lexbor_hash_entry_str(entry);

        if (entry->length == length && cmp_func(str, key, length)) {
            if (prev == NULL) {
                hash->table[table_idx] = entry->next;
            }
            else {
                prev->next = entry->next;
            }

            if (length > LEXBOR_HASH_SHORT_SIZE) {
                lexbor_mraw_free(hash->mraw, entry->u.long_str);
            }

            lexbor_dobject_free(hash->entries, entry);

            return;
        }

        prev = entry;
        entry = entry->next;
    }
}

void *
lexbor_hash_search_by_hash_id(lexbor_hash_t *hash, uint32_t hash_id,
                              const lxb_char_t *key, size_t length,
                              const lexbor_hash_cmp_f cmp_func)
{
    lxb_char_t *str;
    lexbor_hash_entry_t *entry;

    entry = hash->table[ hash_id % hash->table_size ];

    while (entry != NULL) {
        str = lexbor_hash_entry_str(entry);

        if (entry->length == length && cmp_func(str, key, length)) {
            return entry;
        }

        entry = entry->next;
    }

    return NULL;
}

uint32_t
lexbor_hash_make_id(const lxb_char_t *key, size_t length)
{
    size_t i;
    uint32_t hash_id;

    for (i = hash_id = 0; i < length; i++) {
        hash_id += key[i];
        hash_id += (hash_id << 10);
        hash_id ^= (hash_id >> 6);
    }

    hash_id += (hash_id << 3);
    hash_id ^= (hash_id >> 11);
    hash_id += (hash_id << 15);

    return hash_id;
}

uint32_t
lexbor_hash_make_id_lower(const lxb_char_t *key, size_t length)
{
    size_t i;
    uint32_t hash_id;

    for (i = hash_id = 0; i < length; i++) {
        hash_id += lexbor_str_res_map_lowercase[ key[i] ];
        hash_id += (hash_id << 10);
        hash_id ^= (hash_id >> 6);
    }

    hash_id += (hash_id << 3);
    hash_id ^= (hash_id >> 11);
    hash_id += (hash_id << 15);

    return hash_id;
}

uint32_t
lexbor_hash_make_id_upper(const lxb_char_t *key, size_t length)
{
    size_t i;
    uint32_t hash_id;

    for (i = hash_id = 0; i < length; i++) {
        hash_id += lexbor_str_res_map_uppercase[ key[i] ];
        hash_id += (hash_id << 10);
        hash_id ^= (hash_id >> 6);
    }

    hash_id += (hash_id << 3);
    hash_id ^= (hash_id >> 11);
    hash_id += (hash_id << 15);

    return hash_id;
}

lxb_status_t
lexbor_hash_copy(lexbor_hash_t *hash, lexbor_hash_entry_t *entry,
                 const lxb_char_t *key, size_t length)
{
    lxb_char_t *to;

    if (length <= LEXBOR_HASH_SHORT_SIZE) {
        to = entry->u.short_str;
    }
    else {
        entry->u.long_str = lexbor_mraw_alloc(hash->mraw, length + 1);
        if (entry->u.long_str == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        to = entry->u.long_str;
    }

    memcpy(to, key, length);

    to[length] = '\0';

    return LXB_STATUS_OK;
}

lxb_status_t
lexbor_hash_copy_lower(lexbor_hash_t *hash, lexbor_hash_entry_t *entry,
                       const lxb_char_t *key, size_t length)
{
    lxb_char_t *to;

    if (length <= LEXBOR_HASH_SHORT_SIZE) {
        to = entry->u.short_str;
    }
    else {
        entry->u.long_str = lexbor_mraw_alloc(hash->mraw, length + 1);
        if (entry->u.long_str == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        to = entry->u.long_str;
    }

    for (size_t i = 0; i < length; i++) {
        to[i] = lexbor_str_res_map_lowercase[ key[i] ];
    }

    to[length] = '\0';

    return LXB_STATUS_OK;
}

lxb_status_t
lexbor_hash_copy_upper(lexbor_hash_t *hash, lexbor_hash_entry_t *entry,
                       const lxb_char_t *key, size_t length)
{
    lxb_char_t *to;

    if (length <= LEXBOR_HASH_SHORT_SIZE) {
        to = entry->u.short_str;
    }
    else {
        entry->u.long_str = lexbor_mraw_alloc(hash->mraw, length + 1);
        if (entry->u.long_str == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        to = entry->u.long_str;
    }

    for (size_t i = 0; i < length; i++) {
        to[i] = lexbor_str_res_map_uppercase[ key[i] ];
    }

    to[length] = '\0';

    return LXB_STATUS_OK;
}
